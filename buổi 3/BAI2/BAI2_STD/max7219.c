#include "max7219.h"

#define REG_DECODE_MODE   0x09
#define REG_INTENSITY     0x0A
#define REG_SCAN_LIMIT    0x0B
#define REG_SHUTDOWN      0x0C
#define REG_DISPLAY_TEST  0x0F

void MAX7219_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

    // Chân SCK (PA5) và MOSI (PA7)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Chân CS (PA4)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_4); 

    // Khởi tạo SPI1 (16-bit)
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; 
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE);

    // CẤU HÌNH CHO MA TRẬN LED
    MAX7219_Write(REG_DISPLAY_TEST, 0x00); 
    MAX7219_Write(REG_DECODE_MODE, 0x00);  // 0x00: TẮT GIẢI MÃ ĐỂ VẼ TỰ DO LÊN MA TRẬN
    MAX7219_Write(REG_SCAN_LIMIT, 0x07);   // Quét đủ 8 hàng
    MAX7219_Write(REG_INTENSITY, 0x05);    // Độ sáng vừa
    MAX7219_Write(REG_SHUTDOWN, 0x01);     // Bật chip

    // Xóa đen toàn bộ ma trận ban đầu
    for(int i = 1; i <= 8; i++) {
        MAX7219_Write(i, 0x00); 
    }
}

void MAX7219_Write(uint8_t address, uint8_t data) {
    uint16_t frame = (address << 8) | data; 
    GPIO_ResetBits(GPIOA, GPIO_Pin_4); 
    
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, frame);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET); 
    
    GPIO_SetBits(GPIOA, GPIO_Pin_4);
}
