"""
Live print data from the VL53L8CH sensor over serial.
"""

import sys
import time

from sys import platform
import serial.tools.list_ports
import numpy as np
import serial
from util import readline_and_decode

num_zones = 64

if platform == "darwin":  # macOS
    ports = list(serial.tools.list_ports.comports())
    if ports:
        # Try to auto-select a port with likely USB/Serial in description
        for port in ports:
            # print(port.device)
            if "USB" in port.description or "Serial" in port.description:
                serial_port = port.device
                break
    else:
        serial_port = ports[0].device
elif platform == "win32":  # Windows
    ports = list(serial.tools.list_ports.comports())
    if ports:
        # Try to auto-select a port that typically starts with "COM"
        for port in ports:
            if "COM" in port.device:
                serial_port = port.device
                break
    else:
        serial_port = ports[0].device
else: # linux
    serial_port = "/dev/ttyACM0"
print("Using serial port:", serial_port)

ser = serial.Serial(
    serial_port,
    921600,
    bytesize=8,
    parity=serial.PARITY_ODD,
    stopbits=1,
    dsrdtr=True,
    rtscts=True,
    xonxoff=False,
)

if not ser.is_open:
    print("Serial port failed to open")
    sys.exit(1)

num_skipped_frames = 0
started = False
second_cycle = False
start_time = time.time()
frame_idx = 0

while True:
    frame_data = {}

    # We expect zone idx 0 to come first - if not, bad frame and throw it out until we see
    # zone idx 0
    expected_zone_idx = 0
    while expected_zone_idx < num_zones:
        try:
            zone_idx, zone_data, length, distances, reading_counter, start_bin = (
                readline_and_decode(ser, num_zones)
            )
        except Exception as e:
            print(f"(1) Notice, reading serial data: {e}")
            continue
        # if still out of sync add this: expected_zone_idx == 63
        if reading_counter == 0 and start_bin == 0 and started:
            second_cycle = True  # after the second cycle, data is consistent
        if reading_counter == 0 and start_bin == 0:
            started = True

        frame_data[zone_idx] = zone_data

        if zone_idx == 63 and expected_zone_idx != 63:
            expected_zone_idx = 0
        else:
            expected_zone_idx += 1

    if second_cycle and reading_counter == 2:
        try:
            max_values = [int(np.max(frame_data[idx])) for idx in range(num_zones)]
            print(
                f"Max values: {np.array(max_values).max()}, start_bin: {start_bin}, FPS: {frame_idx / (time.time() - start_time)}"
            )
            print(np.mean(np.array(distances)))
            frame_idx += 1
        except Exception as e:
            print(f"(2) Notice, processing frame: {e}")
