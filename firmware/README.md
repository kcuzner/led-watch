# LED Wristwatch Firmware

## Core features

 - Show time on LED ring display.
 - Keep track of time using the RTC (unless crystal is damaged).
 - Set time using the buttons around the periphery of the watch.
 - Use the power saving capabilities of the STM32L0 to remain powered for long
   periods of time between recharges.

## Build instructions

Prerequisites:

 - arm-none-eabi-gcc
 - arm-none-eabi-binutils
 - openocd

To build:

 1. Run `make` in this directory.

To flash the device:

 1. Connect the STLink to the device. Ensure your user has permissions to access
    it.
 2. Run `make install` in this directory.

To debug the device:

 1. Connect the STLink to the device. Ensure your user has permissions to access
    it.
 2. Run `make gdb` in this directory.

To gracefully halt the openocd process started during install or debug:

 1. Run `make stop` in this directory.

