#include "stm32f10x.h"
#include "oled.h"
#include <string.h>

const uint8_t heart_16x16[32] = {
    	0x30, 0x78, 0xFC, 0xFE, 0xFE, 0xFC, 0x78, 0x30, 0x30, 0x78, 0xFC, 0xFE, 0xFE, 0xFC, 0x78, 0x30, 
    0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01, 0x00, 0x00
};

void DrawPixel(uint8_t* buffer, int x, int y) {
    if (x >= 0 && x < 128 && y >= 0 && y < 64) {
        buffer[x + (y / 8) * 128] |= (1 << (y % 8));
    }
}

void DrawBitmap(uint8_t* buffer, int x, int y, const uint8_t* bitmap, int w, int h) {
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            // Đọc từng bit trong mảng, nếu là 1 thì vẽ pixel đó lên
            if (bitmap[i + (j / 8) * w] & (1 << (j % 8))) {
                DrawPixel(buffer, x + i, y + j);
            }
        }
    }
}

int main(void) {
    SystemInit(); 
    
    OLED_Init();

    uint8_t image_data[1024];
    memset(image_data, 0x00, sizeof(image_data)); 

    DrawBitmap(image_data, 20, 24, heart_16x16, 16, 16);   
    DrawBitmap(image_data, 56, 24, heart_16x16, 16, 16);   
    DrawBitmap(image_data, 92, 24, heart_16x16, 16, 16);  

    OLED_DrawImage(image_data);

    while (1) {
    }
}
