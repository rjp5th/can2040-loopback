cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(can_loopback_logged C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(can_loopback_logged
	main.c
	can2040.c
	trice/trice.c
)

target_compile_definitions(can_loopback_logged PUBLIC PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS=2000)
target_link_libraries(can_loopback_logged pico_stdlib)

pico_enable_stdio_usb(can_loopback_logged 1)
pico_enable_stdio_uart(can_loopback_logged 0)

pico_add_extra_outputs(can_loopback_logged)
