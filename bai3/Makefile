CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

TARGET = app
MCU = -mcpu=cortex-m3 -mthumb
DEFINES = -DSTM32F103xB
LINKER_SCRIPT = ./startup/linker.ld

INCLUDES = -I./inc
C_SRCS = $(wildcard src/*.c)

# Biên dịch cả file startup.c từ thư mục startup
STARTUP_SRC = startup/startup.c

OBJS = $(C_SRCS:.c=.o) startup/startup.o

CFLAGS = $(MCU) -O2 -Wall $(DEFINES) $(INCLUDES) -nostdlib
LDFLAGS = $(MCU) -T $(LINKER_SCRIPT) -nostdlib -Wl,-Map=$(TARGET).map

all: $(TARGET).bin print_size

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

# Quy tắc dịch các file .c chung (cho cả src/ và startup/)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

print_size: $(TARGET).elf
	$(SIZE) $<

flash: $(TARGET).bin
	st-flash write $(TARGET).bin 0x8000000

clean:
	rm -f src/*.o startup/*.o $(TARGET).elf $(TARGET).bin $(TARGET).map
