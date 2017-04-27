# LED Wristwatch Bootloader

The bootloader provides a mechanism for reprogramming the system without using
SWD beyond the first initial flashing of this image.

## Program Format

This bootloader lives in the first 8KB of program flash and reserves the first
256 bytes of EEPROM. Programs to be loaded through this bootloader must have
their sections located in the following format:

```
Flash:
0x08002000: User program (.text, .rodata, etc)

EEPROM:
0x08080010-0x08080FFF: EEPROM (the bootloader reserves the first 256 bytes)

Information block:
All sections may be used by the program
```

All writes and reads to the bootloader section (0x08000000-0x08001FFF,
0x08080000-0x080800FF) will be ignored and generate an appropriate status
report.

Firmware which writes to the first 256 bytes of EEPROM may cause the bootloader
to behave unexpectedly.

## Programming Verification

The bootloader includes automatic program verification as the device is
programmed in half-pages. However, it is still recommended to read back the
entire user section of program memory to verify that all blocks have been
written.

As an additional countermeasure against incorrect programming, once the device
enters bootloader mode, it will always enter bootloader mode on reset until
the "Exit Bootloader Mode" command is received. This prevents errors stemming
from device resets or loss of power during the programming process by allowing
the host software to start over programming without needing to resort to an
external reset.

## Protocol

### Entering Bootloader Mode

Bootloader is entered by simply performing any reset except a power-on reset.
If the power-on reset bit is set, the bootloader clears it and then jumps to the
user program, if it has been programmed.

### USB

The bootloader will appear as a USB device using the shared libusb VID/PID and
the name "LED Wristwatch Bootloader". It enumerates as an HID device and will
send and receive 64-byte reports. Each report encapsulates a command. As the
device is stateful, multiple host programs accessing the device at once may
cause corruption. The IN and OUT reports both utilize the default HID report ID,
so no HID report ID needs to be sent with the report.

The device will generate 64-byte IN reports on occasion to communicate the
device status. These reports have the following format:

```
Byte 0-3: First four bytes of the last command received or 0x00000000
Byte 4: Status flags
Byte 5-7: N/A
Byte 8-11: CRC32 of the page report to follow, if applicable
Byte 12-63: N/A
```

The status flag byte may be interpreted as follows:

7 | 6 | 5 | 4 | 3 | 2 | 1 | 0
--- | --- | --- | --- | --- | --- | --- | ---
Command Received | Command OK | Command Status | Command Error | 0 | 0 | 0 | Page Report to follow

### Resetting the programming state machine

When the host program starts, a state machine reset command should be sent to
ensure the device is in a known state before performing any programming
commands. The reset command consists of sending the following report three
times:

The reset command has the following format:

```
Byte 0-63: 0xFF
```

### Programming the Flash

Programming the flash requires 2 reports to be sent: A write/erase command a
page OUT report.

The write command has the following format:

```
Byte 0: 0x80
Byte 1-3: 0x00
Byte 4: Destination address bits 7-0 (bits 6-0 are ignored)
Byte 5: Destination address bits 15-8
Byte 6: Destination address bits 23-16
Byte 7: Destination address bits 31-24
Byte 8-11: CRC32 of the 16 32-bit words from the page command to follow, LSB first
Byte 5-63: N/A
```

Upon receipt of this command, the device will enter programming mode and
initiate an erase of the referenced block. A status report will be generated
once the erase has been completed.

The page command has the following format:

```
Byte 0-63: 16 32-bit words to program
```

Upon receipt of this command, the device will proceed to program the 16 words
starting at the destination address. In the event that less than a page needs
to be written, the remaining bytes should be filled with 0xFF. When programming
has been completed, a status report will be generated and the device returns
to the reset state, being ready to accept the next command.

### Reading the Flash

Reading the flash requires 1 report to be sent and another to be received:
A read command and the page IN report.

The read command has the following format:

```
Byte 0: 0x40
Byte 1-3: 0x00
Byte 4: Source address bits 7-0 (bits 6-0 are ignored)
Byte 5: Source address bits 15-8
Byte 6: Source address bits 23-16
Byte 7: Source address bits 31-24
Byte 8-63: N/A
```

Upon receipt of this command, the device will generate an IN report containing
the contents of the page. A status report will immediately follow and the
device returns to the reset state, being ready to accept the next command.

### Exiting Bootloader Mode

To exit bootloader mode, a report with the following format should be sent:

```
Byte 0: 0xC3
Byte 1-3: 0x00
Byte 4-7: Vector table offset in user flash
Byte 8-63: N/A
```

Upon receipt of this command, the device will send a status report and perform
a USB reset after the host has read the report. The newly written program will
then be executed.

**This command must not be sent until the entire program has been written to
the flash.** In the event that the device stops programming before receiving
this command, it will automatically enter bootloader mode at the next reset
until it receives this command.

