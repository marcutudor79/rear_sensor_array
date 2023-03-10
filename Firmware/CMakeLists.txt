cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(firmware_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(COMPILE_FLAGS "-o2 -Wpedantic")

pico_sdk_init()

add_executable(firmware
        firmware.c
        ${PROJECT_SOURCE_DIR}/can2040/src/can2040.c
        )

# pull in common dependencies and additional i2c hardware support
target_link_libraries(firmware pico_stdlib hardware_i2c hardware_spi hardware_adc)

target_include_directories(firmware PRIVATE ~/pico/pico-sdk/src/rp2_common/cmsis/stub/CMSIS/Device/RaspberryPi/RP2040/Include)
target_include_directories(firmware PRIVATE ~/pico/pico-sdk/src/rp2_common/cmsis/stub/CMSIS/Core/Include)

# create map/bin/hex file etc.
pico_add_extra_outputs(firmware)

# enable pico to send data via uart
pico_enable_stdio_usb(firmware 1) 
pico_enable_stdio_uart(firmware 0)