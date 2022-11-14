# CAN2040 Loopback

Repository to demonstrate a bug in the can2040 library causing non-ACKed messages to lock up the transmit queue.


## Project Structure
* `can_loopback_logged`: pico-sdk project containing code demonstrating the bug with Trice logging enabled in the can2040 code
* `can_loopback_unmodified`: pico-sdk project containing code demonstrating the bug with an unmodified `can2040.c` file
* `lineTransformerANSI.go`: Source code to enable colors for Trice (see below)

## Trice

[Trice](https://github.com/rokath/trice) is used in this project to enable logging in timing-critical sections of the can2040 code. This requires an external tool on the host computer to decode these log messages. A pre-compiled version of this tool can be found in the releases of the Trice repository.

To use trice with this project, upload the compiled project to an RP2040, and in the `can_loopback_logged` directory, run the following command: (Replacing `ttyACMx` with the serial port of the target microcontroller)

    trice log -port /dev/ttyACMx

Note that this repository contains an ANSI color transformer file for Trice, which will add color to the log messages. Copying this file into the Trice repository under `internal/emitter/lineTransformerANSI.go` and recompiling the Go application will result in the trice log application coloring the logging output:

![Logging with colors](/logging_color.png)