"""
Live update a grid of subplots and a single image plot, side-by-side
"""

import argparse
import sys
import time
import signal

from sys import platform
import serial.tools.list_ports
import numpy as np
import pyqtgraph as pg
import serial
from PyQt6 import QtCore, QtWidgets
from util import readline_and_decode


class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, serial_port, num_zones, depth_img_only=False, verbose=False):
        super().__init__()

        if platform == "darwin":  # macOS
            ports = list(serial.tools.list_ports.comports())
            if ports:
                # Try to auto-select a port with likely USB/Serial in description
                for port in ports:
                    # print(port.device)
                    if "USB" in port.description or "Serial" in port.description:
                        self.serial_port = port.device
                        break
            else:
                self.serial_port = ports[0].device
        elif platform == "win32":  # Windows
            ports = list(serial.tools.list_ports.comports())
            if ports:
                # Try to auto-select a port that typically starts with "COM"
                for port in ports:
                    if "COM" in port.device:
                        self.serial_port = port.device
                        break
            else:
                self.serial_port = ports[0].device

        else:
            self.serial_port = "/dev/ttyACM0"
        print("Using serial port:", self.serial_port)

        self.num_zones = num_zones
        self.depth_img_only = depth_img_only
        self.verbose = verbose

        # the sensor only supports 16 or 64 zones
        if not num_zones in [16, 64]:
            print("num_zones must be 16 or 64")
            return

        self.central_widget = QtWidgets.QWidget()
        self.setCentralWidget(self.central_widget)
        self.grid_layout = QtWidgets.QGridLayout(self.central_widget)

        self.plot_widgets = []
        self.lines = []

        grid_size = int(np.ceil(np.sqrt(self.num_zones)))
        pen = pg.mkPen(color=(255, 255, 255))

        for i in range(self.num_zones):
            plot_widget = pg.PlotWidget()
            plot_widget.setBackground("k")
            self.plot_widgets.append(plot_widget)
            self.lines.append(plot_widget.plot([], [], pen=pen))
            row = i // grid_size
            col = i % grid_size
            self.grid_layout.addWidget(plot_widget, row, col)

        # Add the image plot to the right of the 4x4 grid
        self.image_plot_widget = pg.PlotWidget()
        self.image_plot_widget.setBackground("k")
        self.image_item = pg.ImageItem()
        self.image_plot_widget.addItem(self.image_item)
        self.grid_layout.addWidget(
            self.image_plot_widget,  # widget to add
            1,  # row (0-indexed)
            grid_size,  # column (0-indexed)
            2,  # row span
            2,  # column span
        )

        # Set column stretch factors
        for col in range(grid_size):
            self.grid_layout.setColumnStretch(col, 1)
        self.grid_layout.setColumnStretch(grid_size, 3)  # Make the image column wider

        self.ser = serial.Serial(
            self.serial_port,
            921600,
            bytesize=8,
            parity=serial.PARITY_ODD,
            stopbits=1,
            dsrdtr=True,
            rtscts=True,
            xonxoff=False,
        )

        if not self.ser.is_open:
            print("Serial port failed to open")
            return

        self.timer = QtCore.QTimer()
        self.timer.setInterval(1)  # Update interval in milliseconds
        self.timer.timeout.connect(self.update_plot)
        self.timer.start()

        self.start_time = time.time()
        self.frame_idx = 0

        self.started = False
        self.second_cycle = False
        self.concat_frame_data = {}

    def update_plot(self):
        self.concat_frame_data.clear()
        reading_counter = None
        i = 0
        while i < 3:  # 3 frames per shift
            frame_data = {}

            # We expect zone idx 0 to come first - if not, bad frame and throw it out until we see
            # zone idx 0
            expected_zone_idx = 0
            while expected_zone_idx < self.num_zones:
                try:
                    zone_idx, zone_data, _, distances, reading_counter, start_bin = (
                        readline_and_decode(self.ser, self.num_zones)
                    )
                except Exception as e:
                    print(f"(1) Notice, reading serial data: {e}")
                    continue
                if reading_counter == 0 and start_bin == 0 and self.started:
                    self.second_cycle = True  # after the second cycle, data is consistent
                if reading_counter == 0 and start_bin == 0:
                    self.started = True

                frame_data[zone_idx] = zone_data
                if zone_idx == 63 and expected_zone_idx != 63:
                    expected_zone_idx = 0
                else:
                    expected_zone_idx += 1

            # After collecting a complete frame, add it to concat_frame_data
            if self.second_cycle and reading_counter == 2:
                i += 1
                for zone_idx, zone_data in frame_data.items():
                    if zone_idx not in self.concat_frame_data:
                        self.concat_frame_data[zone_idx] = []
                    self.concat_frame_data[zone_idx].extend(zone_data)

        print(f"FPS: {self.frame_idx / (time.time() - self.start_time)}")
        self.frame_idx += 1

        if not self.depth_img_only:
            # update line plot data
            for zone in range(self.num_zones):
                self.lines[zone].setData(self.concat_frame_data[zone])

        # update image plot data
        # depth img just gives the bin index for each pixel, so we can set the min vis value
        # to 0 and the max to the number of bins
        self.image_item.setImage(
            np.array(distances).reshape(int(np.sqrt(self.num_zones)), int(np.sqrt(self.num_zones)))
        )


def signal_handler(sig, frame):
    print("\nCaught Ctrl+C, exiting gracefully...")
    QtWidgets.QApplication.quit()
    sys.exit(0)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--verbose", "-v", action="store_true", help="Print verbose output (for debugging)"
    )
    parser.add_argument(
        "--port",
        "-p",
        type=str,
        default="/dev/ttyACM0",
        help="Serial port of MCU with VL sensor",
    )
    parser.add_argument(
        "--depth_img_only",
        action="store_true",
        help="Only show the depth image plot, not the individual zone plots (avoids dropped frames)",
    )
    parser.add_argument(
        "--num_zones",
        "-z",
        type=int,
        default=64,
        help="Number of zones to expect from the sensor (sensor only supports 16 or 64)",
    )
    args = parser.parse_args()

    # Set up the Ctrl+C handler
    signal.signal(signal.SIGINT, signal_handler)

    app = QtWidgets.QApplication([])
    window = MainWindow(
        args.port, args.num_zones, depth_img_only=args.depth_img_only, verbose=args.verbose
    )
    window.show()

    # This allows SIGINT to be processed
    timer = QtCore.QTimer()
    timer.timeout.connect(lambda: None)
    timer.start(100)  # Small timeout to allow signals to be processed

    sys.exit(app.exec())
