# mini_tof_firmware
Microcontroller firmware for reading from miniature time-of-flight sensors. This firmware is necessary to utilize the [mini_tof ROS 2 package](https://github.com/uwgraphics/mini_tof).

Each sensor has a different hardware setup and requires different firmware. See the README files in the corresponding subdirectories ([TMF882X](https://github.com/uwgraphics/mini_tof_firmware/tree/main/TMF882X), [VL53L8CH](https://github.com/uwgraphics/mini_tof_firmware/tree/main/VL53L8CH)) for instructions for either sensor.

# Supported Sensors
Currently, two sensors are supported: The AMS TMF882X (the TMF8820, TMF8821, and TMF8828) and the ST VL53L8CH. The TMF882X is a more compact package and easier to use, but has a limited frame rate (~4Hz) when reporting full histograms, and is less configurable. The VL53L8CH is larger and more difficult to use, but supports a higher bandwidth SPI interface, enabling higher frame rates (up to ~30Hz) and more configurable settings such as (temporal and spatial) resolution.