CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

TARGET = app

MCU = -mcpu=cortex-m3 -mthumb

DEFINES = -DSTM32F103xB

LINKER_SCRIPT = ./startup/linker.ld

INCLUDES = -I./inc


# ==========================================
# SOURCE FILES
# ==========================================

C_SRCS = $(wildcard src/*.c)

C_OBJS = $(C_SRCS:.c=.o)

OBJS = $(C_OBJS) startup/startup.o


# ==========================================
# C FLAGS
# ==========================================

CFLAGS = $(MCU) \
         -O2 \
         -Wall \
         -Wextra \
         $(DEFINES) \
         $(INCLUDES) \
         -ffreestanding \
         -nostdlib


# ==========================================
# LINKER FLAGS
# ==========================================

LDFLAGS = $(MCU) \
          -T $(LINKER_SCRIPT) \
          -nostdlib \
          -Wl,-Map=$(TARGET).map


# ==========================================
# DEFAULT BUILD
# ==========================================

all: $(TARGET).bin
	$(SIZE) $(TARGET).elf


# ==========================================
# LINK
# ==========================================

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@


# ==========================================
# COMPILE MAIN.C
# ==========================================

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@


# ==========================================
# COMPILE STARTUP.C
# ==========================================

startup/startup.o: startup/startup.c
	$(CC) $(CFLAGS) -c $< -o $@


# ==========================================
# CREATE BIN
# ==========================================

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@


# ==========================================
# FLASH STM32
# ==========================================

flash: $(TARGET).bin
	st-flash --reset write $(TARGET).bin 0x08000000


# ==========================================
# CLEAN
# ==========================================

clean:
	rm -f src/*.o
	rm -f startup/*.o
	rm -f $(TARGET).elf
	rm -f $(TARGET).bin
	rm -f $(TARGET).map


.PHONY: all flash clean
