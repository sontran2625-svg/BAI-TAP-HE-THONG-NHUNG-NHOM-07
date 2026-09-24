#include "oled.h"

#define OLED_ADDR 0x78 // Địa chỉ I2C mặc định của SSD1306

static void I2C1_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    // Bật xung nhịp cho GPIOB và I2C1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    // Cấu hình chân PB6 (SCL) và PB7 (SDA) ở chế độ Alternate Function Open Drain
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Cấu hình thông số I2C1
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 400000; // Tốc độ 400kHz
    
    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_Cmd(I2C1, ENABLE);
}

static void OLED_WriteCmd(uint8_t cmd) {
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, OLED_ADDR, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(I2C1, 0x00); // 0x00 báo hiệu đây là Lệnh
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(I2C1, cmd);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(I2C1, ENABLE);
}

static void OLED_WriteData(uint8_t data) {
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, OLED_ADDR, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(I2C1, 0x40); // 0x40 báo hiệu đây là Dữ liệu
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(I2C1, data);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(I2C1, ENABLE);
}

void OLED_Init(void) {
    I2C1_Init();
    // Delay ngắn chờ màn hình khởi động
    for(volatile int i=0; i<100000; i++); 

    OLED_WriteCmd(0xAE); // Tắt màn hình
    OLED_WriteCmd(0x20); // Cấu hình chế độ địa chỉ bộ nhớ
    OLED_WriteCmd(0x00); // Horizontal Addressing Mode
    OLED_WriteCmd(0x8D); // Kích hoạt Charge Pump
    OLED_WriteCmd(0x14);
    OLED_WriteCmd(0xAF); // Bật màn hình
}

void OLED_DrawImage(const uint8_t *image) {
    // Thiết lập vùng giới hạn ghi đè từ cột 0->127, trang 0->7
    OLED_WriteCmd(0x21); OLED_WriteCmd(0); OLED_WriteCmd(127);
    OLED_WriteCmd(0x22); OLED_WriteCmd(0); OLED_WriteCmd(7);
    
    // Bắn toàn bộ 1024 byte (128x64 pixel) vào RAM màn hình
    for(int i = 0; i < 1024; i++) {
        OLED_WriteData(image[i]);
    }
}
