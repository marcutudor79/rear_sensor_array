cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(accelerometer_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(accelerometer
        accelerometer.cpp
        MCP2515.cpp
        CANPacket.cpp
        Utilities.cpp
        )

# pull in common dependencies and additional i2c hardware support
target_link_libraries(accelerometer pico_stdlib hardware_i2c hardware_spi)

# create map/bin/hex file etc.
pico_add_extra_outputs(accelerometer)

#enable pico to send data via uart
pico_enable_stdio_usb(accelerometer 1) 
pico_enable_stdio_uart(accelerometer 0)