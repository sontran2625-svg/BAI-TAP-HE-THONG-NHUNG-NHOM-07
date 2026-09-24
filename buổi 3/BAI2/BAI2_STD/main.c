#include "stm32f10x.h"
#include "max7219.h"

// Mảng dữ liệu: Hình trái tim tĩnh
const uint8_t heart_rotated[8] = {
    0x30, // 00110000
    0x78, // 01111000
    0x7C, // 01111100
    0x3E, // 00111110
    0x3E, // 00111110
    0x7C, // 01111100
    0x78, // 01111000
    0x30  // 00110000
};

int main(void) {
    SystemInit();
    
    // Delay khởi động chờ ổn định nguồn
    for(volatile int i = 0; i < 500000; i++);

    MAX7219_Init();

    // Đẩy 8 byte của hình trái tim đã xoay ra ma trận LED
    for (int i = 0; i < 8; i++) {
        MAX7219_Write(i + 1, heart_rotated[i]);
    }

    while (1) {
        // Chip đã chốt dữ liệu, CPU nghỉ ngơi
    }
}
