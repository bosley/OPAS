# Grotto

## Building

1. Ensure that the the Arduino SDK is downloaded and installed to ``` /usr/shar/arduino* ``` and that the ``` Arduino-CMake-Toolchain ``` submodule is initiated.

2. Create a build directory somewhere. Then run ``` cmake path/to/groto ```

3. Once this is done, and the Arduino compiler is detected ccmake can be used to select a board if not given in step two with ```-D /path/to/BoardOptions.cmake ```. This is the way it is because the Arduino-CMake-Toolchain repo being used dictates as such. Once the project is made a ```BoardOptions.cmake``` is generated. Yay.


## Uploading

    cmake --build . --target upload-%project_name% -- SERIAL_PORT=/dev/ttyUSB0