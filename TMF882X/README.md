# TMF882X Firmware (TMF8820, TMF8821, TMF8828)

Microcontroller firmware for the AMS TMF8820 miniature time-of-flight sensor. This firmware is based on the open source [Arduino Driver](https://ams-osram.com/products/sensor-solutions/direct-time-of-flight-sensors-dtof/ams-tmf8820-3x3-multi-zone-time-of-flight-sensor) provided by AMS, with modifications to improve speed and add features. This firmware is required to utilize the [mini_tof ROS 2 package](https://github.com/uwgraphics/mini_tof).

# Hardware Setup

## Components
We recommend the [SparkFun TMF8820 Breakout Board](https://www.sparkfun.com/sparkfun-qwiic-mini-dtof-imager-tmf8820.html) paired with the [SparkFun Qwiic Pro Micro](https://www.sparkfun.com/sparkfun-qwiic-pro-micro-usb-c-atmega32u4.html) for the smallest package and easiest setup.

## Connections
If using the recommended hardware, the sensor and microcontroller can be connected with a single [Qwiic connect cable](https://www.sparkfun.com/qwiic), and the microcontroller connected to a computer via USB-C. 

If using other hardware, follow [SparkFun's setup guide](https://learn.sparkfun.com/tutorials/qwiic-dtof-imager-tmf882x-hookup-guide) to connect the microcontroller to the sensor.

# Software Setup
1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Follow [SparkFun's guide](https://learn.sparkfun.com/tutorials/qwiic-pro-micro-usb-c-atmega32u4-hookup-guide/all) to add support for the Qwiic Pro Micro to the Arduino IDE.
3. Open the Arduino IDE, navigate to File -> Open and select the `firmware/firmware.ino` file in this repository.
4. Be sure to select the SparkFun Qwiic Pro Micro board from the Tools -> Board menu.
5. Select the correct port from the Tools -> Port menu.
6. Click the upload button (right arrow) in the Arduino IDE to compile and upload the firmware to the microcontroller.
7. Once the upload is complete, open the Serial Monitor (Tools -> Serial Monitor) and set the baud rate to 1000000. You should see the firmware outputting data from the sensor.

## Utilizing the data
If you are using ROS 2, we recommend that you utilize the [mini_tof ROS 2 package](https://github.com/uwgraphics/mini_tof), which handles converting measurements from the sensor to a ROS 2 topic.

If you would like lower-level access to data from the sensor, example python programs which read and decode the USB serial port data from the sensor are available in the "examples" subdirectory.
