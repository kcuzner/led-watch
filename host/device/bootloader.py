"""
Basic methods and classes for interfacing with the bootloader based on the
wristwatch
"""

import device.wristwatch as wristwatch

class ResetCommand(wristwatch.Command):
    COMMAND = 0xFFFFFFFF
    def __init__(self):
        super().__init__(ResetCommand.COMMAND, b'\xFF' * 60)

class Bootloader(wristwatch.Device):
    MANUFACTURER='kevincuzner.com'
    PRODUCT='LED Wristwatch Bootloader'
    def bootloader_command(self, cmd):
        print(len(cmd.pack()))
        self.write_command(cmd)
        while True:
            result = self.read(64)
            if len(result):
                return result


def find_device(cls=Bootloader):
    return wristwatch.find_device(cls)

