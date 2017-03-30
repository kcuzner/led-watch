# Makefile for the STM32F103C8 blink program
#
# Kevin Cuzner
#

PROJECT = wristwatch

# Project Structure
SRCDIR = src
BINDIR = bin
OBJDIR = obj
INCDIR = include

# Project target
CPU = cortex-m0plus

# Sources
SRC = $(wildcard $(SRCDIR)/*.c)
ASM = $(wildcard $(SRCDIR)/*.s)

# Include directories
INCLUDE  = -I$(INCDIR) -Icmsis

# Linker
LSCRIPT = STM32L052X8_RAM.ld

# C Flags
GCFLAGS  = -std=c99 -Wall -Os -fno-common -mthumb -mcpu=$(CPU) -DSTM32L052xx --specs=nosys.specs --specs=nano.specs -g -Wa,-ahlms=$(addprefix $(OBJDIR)/,$(notdir $(<:.c=.lst)))
GCFLAGS += $(INCLUDE)
LDFLAGS += -T$(LSCRIPT) -mthumb -mcpu=$(CPU) --specs=nosys.specs --specs=nano.specs -Wl,-Map,$(BINDIR)/$(PROJECT).map -Wl,--gc-sections
ASFLAGS += -mcpu=$(CPU)

# Flashing
OCDFLAGS = -f openocd/openocd.cfg

# Tools
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
AR = arm-none-eabi-ar
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size --format=SysV
OBJDUMP = arm-none-eabi-objdump
GDB = arm-none-eabi-gdb
OCD = openocd

RM = rm -rf

## Build process

OBJ := $(addprefix $(OBJDIR)/,$(notdir $(SRC:.c=.o)))
OBJ += $(addprefix $(OBJDIR)/,$(notdir $(ASM:.s=.o)))


all:: $(BINDIR)/$(PROJECT).bin $(BINDIR)/$(PROJECT).hex

Build: $(BINDIR)/$(PROJECT).bin

macros:
	$(CC) $(GCFLAGS) -dM -E - < /dev/null

cleanBuild: clean

clean:
	$(RM) $(BINDIR)
	$(RM) $(OBJDIR)

size:
	$(SIZE) $(BINDIR)/$(PROJECT).elf

# Compilation

$(BINDIR)/$(PROJECT).hex: $(BINDIR)/$(PROJECT).elf
	$(OBJCOPY) -O ihex $(BINDIR)/$(PROJECT).elf $(BINDIR)/$(PROJECT).hex

$(BINDIR)/$(PROJECT).bin: $(BINDIR)/$(PROJECT).elf
	$(OBJCOPY) -O binary $(BINDIR)/$(PROJECT).elf $(BINDIR)/$(PROJECT).bin

$(BINDIR)/$(PROJECT).elf: $(OBJ) $(LSCRIPT)
	@mkdir -p $(dir $@)
	$(CC) $(OBJ) $(LDFLAGS) -o $(BINDIR)/$(PROJECT).elf
	$(OBJDUMP) -D $(BINDIR)/$(PROJECT).elf > $(BINDIR)/$(PROJECT).lst
	$(SIZE) $(BINDIR)/$(PROJECT).elf

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(GCFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<


$(OBJDIR)/%.o: $(COMDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(GCFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(COMDIR)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<

$(OBJ): Makefile

