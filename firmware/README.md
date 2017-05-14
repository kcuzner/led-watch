# LED Wristwatch Firmware

## Core features

 - Show time on LED ring display.
 - Keep track of time using the RTC (unless crystal is damaged).
 - Set time using the buttons around the periphery of the watch.
 - Use the power saving capabilities of the STM32L0 to remain powered for long
   periods of time between recharges.

## USB Features

 - Connect as an HID peripheral
 - Get/Set watch time through HID
 - Possible feature: Reflash firmware through USB

## Build instructions

Prerequisites:

 - arm-none-eabi-gcc
 - arm-none-eabi-binutils
 - python3
 - [python-hidapi](https://pypi.python.org/pypi/hidapi)
 - A wristwatch programmed with the bootloader

To build:

 1. Run `make` in this directory.

To flash the device:

 1. Connect the device to the host computer over USB, ensuring the user has
    permissions to access it. The device must have been previously flashed with
    the bootloader found in `../bootloader`
 2. Run `make install` in this directory. If the previously installed firmware
    does not enumerate as a valid LED Wristwatch device, reset the wristwatch by
    grounding the reset pin exposed through the programming header.

To debug the device:

 1. Connect the STLink to the device. Ensure your user has permissions to access
    it.
 2. Change to the `../bootloader` directory.
 2. Run `make gdb` in the bootloader directory

