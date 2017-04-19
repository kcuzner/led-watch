# LED Wristwatch Host Software

This software is written in Python and intended for use on Linux. It should be
easy enough to port to other OSes.

## Prerequisites

 * Python 3
 * hidapi from PyPi
 * Corresponding hidapi installation

## Running instructions

The host software has two modes: Clock update and Bootloader mode. For
instructions, use:

```
$ ./wristwatch -h
```

## Troubleshooting

Not able to find device, even though it is plugged in and working properly:

 - Did you remember to run the program with sudo?
 - Did you remember to make a udev rule for hidraw to avoid sudo (add user to
   plugdev group):

```
KERNEL=="hidraw*", SUBSYSTEM=="hidraw", MODE="0664", GROUP="plugdev"
```

