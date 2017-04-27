/**
 * STM32L052X8 linker script
 *
 * Copyright 2017 Kevin Cuzner
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(Reset_Handler)

/**
 * A word about the PMA
 *
 * TODO: PMA might be 512 16-bit words on the STM32L0x2xx
 *
 * The PMA is 512 bytes long, arranged as 256 16-bit words. However, because it
 * is attached to the APB, it must be accessed in 32-bit chunks. So, each
 * 32-bit address is mapped into the PMA per the example below:
 *
 *  ADDR+OFFSET: |  0x0  |  0x1  |  0x2  |  0x3  |
 *  -------------|-------|-------|-------|-------|
 *    0x40006000 | 0x000 | 0x001 | ----- | ----- |
 *    0x40006004 | 0x002 | 0x003 | ----- | ----- |
 *    0x40006008 | 0x004 | 0x005 | ----- | ----- |
 *    0x4000600C | 0x006 | 0x007 | ----- | ----- |
 *    0x40006010 | 0x008 | 0x009 | ----- | ----- |
 *    ....
 *    0x400063F8 | 0x1FC | 0x1FD | ----- | ----- |
 *    0x400063FC | 0x1FE | 0x1FF | ----- | ----- |
 *
 * Basically, the access into each 16-bit word (0x0, 0x2, 0x4, etc) must be
 * 32-bit aligned (0x0, 0x4, 0x8, etc). Therefore, because each 16-bit word
 * essentially "takes up" 32 bits of space, 512 bytes of PMA actually uses
 * 1024 bytes of virtual address space and comes with the caveat that it must
 * be accessed only by word, with one word every 4 virtual bytes.
 *
 * However, the purpose of this linker script is to enable gcc to manage usage
 * of symbols located inside the PMA and produce linker errors when statically
 * allocated symbols overrun the area. In order to accomplish this, this linker
 * script declares the PMA to be 512 bytes long. In the USB driver, macros are
 * provided which will translate a linker-generated address into a "local
 * address" which can be used with the USB registers and an "application
 * address" which can be used by normal code to make word-accesses into the
 * memory. Sadly, it does not seem possible to wrangle gcc into allowing
 * transparent byte-level accesses into memory. So, standard functions like
 * memcpy cannot be used. Functions like memset can be used, provided that the
 * application address is fed to the function and the size of the area to be
 * set is multiplied by two.
 *
 * Direct usage of symbols allocated in the PMA may cause unexpected behavior.
 */

MEMORY
{
    FLASH (RX)  : ORIGIN = 0x08000000, LENGTH = 8K
    EEPROM (W)  : ORIGIN = 0x08080000, LENGTH = 256
    RAM (W!RX)  : ORIGIN = 0x20000000, LENGTH = 8K
    PMA (W)     : ORIGIN = 0x40006000, LENGTH = 512 /* 256 x 16bit */
}

/**
 * Define the top of the stack as the end of RAM
 */
_heap_size_min = 256;
_stack_size_min = 512; /* On a system with 64K, 8K was used */
_estack = (0x20000000+8K);

SECTIONS
{
    /* General code */
    .text :
    {
        . = ALIGN(4);
        /* At beginning of flash is:
         *
         * Required:
         * 0x0000 Initial stack pointer
         * 0x0004 Reset Handler
         *
         * Optional:
         * 0x0008 and beyond: NVIC ISR Table
         */
        KEEP(*(.isr_vector))
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.glue_7)
        *(.glue_7t)

        /* C startup support */
        /* TODO: Convert to -nostartfiles for maximum DIY */
        *(.eh_frame)
        KEEP(*(.init))
        KEEP(*(.fini))
    } > FLASH

    /* Additional C++ Support sections */
    .preinit_array :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } > FLASH
    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
    } > FLASH
    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } > FLASH

    .eeprom :
    {
        *(.eeprom)
        *(.eeprom*)
    } > EEPROM

    /* Load address of .data in flash for startup code */
    _sidata = LOADADDR(.data);

    /* Initialized data */
    .data :
    {
        . = ALIGN(4);
        _sdata = .; /* Start of data in RAM */
        *(.data);
        *(.data*);

        . = ALIGN(4);
        _edata = .; /* End of data in RAM */
    } > RAM AT > FLASH

    /* Zeroed data */
    . = ALIGN(4);
    .bss :
    {
        _sbss = .; /* Start of bss in RAM */
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        . = ALIGN(4);
        _ebss = .; /* End of bss in RAM */
        __bss_end__ = _ebss;
    } > RAM

    /* Fake stack section to ensure that sufficient space is left */
    ._user_heap_stack :
    {
        . = ALIGN(4);
        . = . + _stack_size_min;
        . = . + _heap_size_min;
        . = ALIGN(4);
    } > RAM

    /* USB/CAN Packet Memory Area (PMA) */
    .pma :
    {
        _pma_start = .; /* Start of PMA in real memory space */
        . = ALIGN(2);
        *(.pma)
        *(.pma*)
        . = ALIGN(2);
        _pma_end = .; /* End of PMA in PMA space */
    } > PMA

    /* Remove information from the standard libraries */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

    PROVIDE_HIDDEN (__exidx_start = .);
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    }
    PROVIDE_HIDDEN (__exidx_end = .);
}

