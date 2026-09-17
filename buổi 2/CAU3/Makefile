# --- Makefile cho STM32F10x STD Library ---

TARGET = main
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

# Cấu hình chip STM32F103 (Medium Density)
CPU = -mcpu=cortex-m3 -mthumb
DEFS = -DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD

# Đường dẫn đến các thư mục chứa file .h
INCLUDES = \
-I. \
-ILibraries/CMSIS/CM3/CoreSupport \
-ILibraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
-ILibraries/STM32F10x_StdPeriph_Driver/inc

# Lấy main.c, các file lõi và quét toàn bộ thư viện STD
C_SOURCES = \
main.c \
stm32f10x_it.c \
system_stm32f10x.c \
Libraries/CMSIS/CM3/CoreSupport/core_cm3.c \
$(wildcard Libraries/STM32F10x_StdPeriph_Driver/src/*.c)

# File khởi động Assembly
ASM_SOURCES = Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_md.s

# Linker script
LDSCRIPT = stm32_flash.ld

# Cờ biên dịch
CFLAGS = $(CPU) $(DEFS) $(INCLUDES) -Os -Wall -fdata-sections -ffunction-sections
LDFLAGS = $(CPU) -T$(LDSCRIPT) --specs=nano.specs -Wl,--gc-sections -lc -lm -lnosys

OBJS = $(C_SOURCES:.c=.o) $(ASM_SOURCES:.s=.o)

all: $(TARGET).elf $(TARGET).bin
	$(SIZE) $(TARGET).elf

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.s
	$(CC) -c $(CFLAGS) $< -o $@

$(TARGET).elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).bin

# ==========================================
# LỆNH NẠP CODE (TỰ ĐỘNG NHẬN DIỆN MẠCH NẠP)
# ==========================================
flash: $(TARGET).bin
	@if lsusb | grep -qi "st-link"; then \
		echo "\n=> Phat hien ST-Link! Dang nap code qua ST-Link..."; \
		st-flash --reset write $(TARGET).bin 0x8000000; \
	elif ls /dev/ttyUSB* >/dev/null 2>&1; then \
		echo "\n=> Phat hien cap USB-TTL! Dang nap code qua UART..."; \
		stm32flash -w $(TARGET).bin -v -g 0x0 /dev/ttyUSB0; \
	else \
		echo "\n=> LOI: Khong tim thay mach nap ST-Link hay cap USB-TTL nao duoc cam vao may!"; \
	fi
