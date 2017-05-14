"""
Basic methods and classes for interfacing with the bootloader based on the
wristwatch
"""

import device.wristwatch as wristwatch
import struct, zlib

class InvalidFileException(Exception):
    pass

class InvalidPageBlockException(Exception):
    pass

class BootloaderError(Exception):
    def __init__(self, flags):
        message = ''
        if flags & 0x1:
            message += 'Internal Error;'
        if flags & 0x2:
            message += 'Invalid Command;'
        if flags & 0x4:
            message += 'Invalid Address;'
        if flags & 0x8:
            message += 'Invalid CRC32;'
        if flags & 0x10:
            message += 'Write Failed;'
        if flags & 0x20:
            message += 'Short OUT report length;'
        super(Exception, self).__init__(message)

class ProgramBlock(object):
    def __init__(self, address, data):
        self.address = address
        self.data = bytes(data)

    def subblock(self, offset, length):
        """
        Creates an subblock from the data in this block using a passed offset
        """
        address = self.address + offset
        data = self.data[offset:offset+length]
        return ProgramBlock(address, data)

    def crc32(self):
        return zlib.crc32(self.data)

    def pack(self):
        """
        Compatiblity function so this can be passed directly as a command
        """
        if len(self.data) != 64:
            raise InvalidPageBlockException('Block is not half-page sized and cannot be used directly as command')
        return self.data

    def __str__(self):
        return '[Page at 0x{:X}, {:d} bytes]'.format(self.address, len(self.data))

class IntelHexLoader(object):
    def __init__(self, filename):
        self.filename = filename
        self.file = None
        self.line_number = 0
        self.base_address = 0

    def __enter__(self):
        self.file = open(self.filename)
        self.line_number = 0
        return self
    def __exit__(self, *args):
        self.file.close()
        self.file = None

    def __iter__(self):
        return self
    def __next__(self):
        while True:
            self.line_number += 1
            line = next(self.file).strip()
            if line[0] != ':':
                raise InvalidFileException('Invalid character found at line '+str(self.line_number))
            if len(line) < 3:
                raise InvalidFileException('Short line found at line '+str(self.line_number))
            length = int(line[1:3], 16)
            if len(line) != 11 + 2*length:
                raise InvalidFileException('Invalid line length at line '+str(self.line_number))
            address = int(line[3:7], 16)
            record_type = int(line[7:9], 16)
            checksum = int(line[-2:], 16)
            data = bytes([int(line[9+2*i:9+2*(i+1)], 16) for i in range(0, length)])
            if record_type == 0:
                return ProgramBlock(self.base_address + address, data)
            elif record_type == 1:
                raise StopIteration
            elif record_type == 2:
                raise InvalidFileException('Record type 02 not supported in Intel Hex')
            elif record_type == 3:
                continue # ignore start segment addresses
            elif record_type == 4:
                self.base_address = struct.unpack('>H', data)[0] << 16
            elif record_type == 5:
                continue # ignore the start address for now, but it is convenient

class ProgramPager(object):
    """
    Iterates through program blocks, creating aligned pages of PAGE_SIZE. These
    are returned as tuples of (address, data)
    """
    def __init__(self, blocks, page_power = 7):
        self.page_size = pow(2, page_power)
        self.address_mask = self.page_size - 1
        self.segments = []
        current_segment = None
        for b in sorted(blocks, key=lambda b: b.address):
            if not current_segment:
                current_segment = (b.address, bytes(b.data))
            else:
                if current_segment[0] + len(current_segment[1]) == len(b.data):
                    current_segment = (current_segment[0],\
                            current_segment[1] + bytes(b.data))
                else:
                    self.segments.append(current_segment)
                    current_segment = (b.address, bytes(b.data))
        if current_segment is not None:
            self.segments.append(current_segment)

    def __iter__(self):
        return self

    def __next__(self):
        if len(self.segments) == 0:
            raise StopIteration
        segment = self.segments[0]
        preceding_bytes = segment[0] & self.address_mask
        address = segment[0] & ~self.address_mask
        data = b'\x00' * preceding_bytes
        while len(data) < self.page_size and len(self.segments) > 0:
            segment = self.segments[0]
            if segment[0] >= address + self.page_size:
                break # segment not within page
            next_data = segment[1][:(self.page_size - len(data))]
            if len(next_data) < len(segment[1]):
                self.segments[0] = (segment[0] + len(next_data),
                        segment[1][len(next_data):])
            else:
                self.segments.pop(0) # page is consumed
            data += next_data
        data += b'\x00' * (self.page_size - len(data))
        return ProgramBlock(address, data)



class ResetCommand(wristwatch.Command):
    COMMAND = 0x00000000
    def __init__(self):
        super().__init__(ResetCommand.COMMAND, b'\x00' * 60)

class ProgramStartCommand(wristwatch.Command):
    COMMAND = 0x00000080
    def __init__(self, lower, upper):
        data = struct.pack('<III', lower.address, lower.crc32(), upper.crc32())
        super().__init__(ProgramStartCommand.COMMAND, data)

class BootloaderExitCommand(wristwatch.Command):
    COMMAND = 0x000000C3
    def __init__(self, vtor):
        data = struct.pack('<I', vtor)
        super().__init__(BootloaderExitCommand.COMMAND, data)

class BootloaderAbortCommand(wristwatch.Command):
    COMMAND = 0x0000003E
    def __init__(self):
        data = b''
        super().__init__(BootloaderAbortCommand.COMMAND, data)

class BootloaderStatus(object):
    def __init__(self, data):
        unpacked = struct.unpack('<IIII48s', bytes(data))
        self.last_command = unpacked[0]
        self.flags = unpacked[1]
        self.crc32_lower = unpacked[2]
        self.crc32_upper = unpacked[3]

class Bootloader(wristwatch.Device):
    MANUFACTURER='kevincuzner.com'
    PRODUCT='LED Wristwatch Bootloader'
    def bootloader_command(self, cmd, wait_for_status=True):
        self.write_command(cmd)
        while wait_for_status:
            result = self.read(64)
            if len(result):
                return BootloaderStatus(result)
    def try_read_status(self, timeout_ms=1):
        result = self.read(64, timeout_ms=timeout_ms)
        return BootloaderStatus(result) if len(result) else None

    def reset(self):
        cmd = ResetCommand()
        self.bootloader_command(cmd, wait_for_status=False)
        self.bootloader_command(cmd, wait_for_status=False)
        self.bootloader_command(cmd, wait_for_status=False)
        self.try_read_status(timeout_ms=1000)
        self.try_read_status(timeout_ms=1000)
        self.try_read_status(timeout_ms=1000)

    def program(self, page):
        if (page.address & 0x7F) != 0 or len(page.data) != 128:
            raise InvalidPageBlockException('The passed block does not fit the definition of a 128-byte aligned page')
        lower_block = page.subblock(0, 64)
        upper_block = page.subblock(64, 64)
        status = self.bootloader_command(ProgramStartCommand(lower_block, upper_block))
        if status.flags:
            print(str(page))
            raise BootloaderError(status.flags)
        status = self.bootloader_command(lower_block)
        if status.flags:
            print(str(page), [hex(i) for i in lower_block.pack()])
            print(hex(status.crc32_lower), hex(status.crc32_upper))
            print(hex(lower_block.crc32()), hex(upper_block.crc32()))
            raise BootloaderError(status.flags)
        status = self.bootloader_command(upper_block)
        if status.flags:
            print(str(page))
            print(hex(status.crc32_lower), hex(status.crc32_upper))
            print(hex(lower_block.crc32()), hex(upper_block.crc32()))
            raise BootloaderError(status.flags)

    def exit(self, vtor):
        cmd = BootloaderExitCommand(vtor)
        try:
            self.bootloader_command(cmd)
        except OSError as ex:
            return True
        else:
            return False

    def abort(self):
        cmd = BootloaderAbortCommand()
        result = None
        try:
            self.bootloader_command(cmd)
        except OSError as ex:
            return True
        else:
            return False


def find_device(cls=Bootloader):
    return wristwatch.find_device(cls)

