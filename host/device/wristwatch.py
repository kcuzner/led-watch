"""
Basic methods and classes for interfacing with the wristwatch
"""

import hid
import struct, time

VID = 0x16c0
PID = 0x05dc

class Command(object):
    """
    Basic command structure: 4 bytes of command followed by up to 60 bytes of data
    """
    def __init__(self, code, data_bytes):
        self.code = code
        self.data_bytes = data_bytes

    def pack(self):
        return struct.pack('<I60s', self.code, self.data_bytes)

class SetTimeCommand(Command):
    COMMAND = 1
    def __init__(self, timestamp):
        parts = bytes([int(s) for s in time.strftime('%y,%m,%d,%H,%M,%S', timestamp).split(',')])
        super().__init__(SetTimeCommand.COMMAND, parts)

class Device(hid.device):
    MANUFACTURER='kevincuzner.com'
    PRODUCT='LED Wristwatch'
    def __init__(self, path):
        self.path = path

    def __enter__(self):
        self.open_path(self.path)
        return self

    def __exit__(self, *args):
        self.close()

    def set_time(self):
        """
        Sets the watch time to the current time
        """
        cmd = SetTimeCommand(time.localtime())
        self.write_command(cmd)

    def write_command(self, command):
        data = command.pack()
        res = self.write(data)
        if res < 0:
            raise ValueError(self.error())


def find_device(cls=Device):
    info = hid.enumerate(VID, PID)
    for i in info:
        if i['manufacturer_string'] == cls.MANUFACTURER and\
                i['product_string'] == cls.PRODUCT:
            return Device(i['path'])
    return None


