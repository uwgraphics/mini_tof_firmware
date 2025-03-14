# VL53L8CH Firmware

Microcontroller firmware for the ST VL53L8CH miniature time-of-flight sensor. This firmware is based on the open source ["ULD" firmware](https://www.st.com/en/embedded-software/stsw-img043.html) provided by ST, with modifications to improve speed and add features. This firmware is required to utilize the [mini_tof ROS 2 package](https://github.com/uwgraphics/mini_tof).

## Features

## Versions

## Usage

### Flashing the Microcontroller

### Reading from the Microcontroller
If you are using ROS 2, we recommend that you utilize the [mini_tof ROS 2 package](https://github.com/uwgraphics/mini_tof), which handles converting measurements from the sensor to a ROS 2 topic.

If you would like lower-level access to data from the sensor, example python programs which read and decode the USB serial port data from the sensor are available in the "examples" subdirectory.