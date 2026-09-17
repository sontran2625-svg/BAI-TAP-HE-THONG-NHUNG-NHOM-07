#include "stm32f10x.h"
#include <stdio.h>

// ==========================================================
// 1. CẤU HÌNH HÀM DELAY BẰNG SYSTICK (KHÔNG DÙNG NGẮT)
// ==========================================================
void Delay_ms(uint32_t ms) {
    // Nạp giá trị đếm cho 1ms (dựa theo tốc độ thạch anh hệ thống)
    SysTick->LOAD = (SystemCoreClock / 1000) - 1;
    SysTick->VAL = 0; // Xóa bộ đếm hiện tại về 0
    
    // Bật SysTick, dùng nguồn xung nhịp hệ thống, KHÔNG BẬT NGẮT
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

    for (uint32_t i = 0; i < ms; i++) {
        // Chờ cờ COUNTFLAG bật lên 1 (báo hiệu đã đếm đủ 1ms)
        while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
    }
    
    // Tắt bộ đếm SysTick sau khi dùng xong
    SysTick->CTRL = 0; 
}

// ==========================================================
// 2. CẤU HÌNH UART2 (ĐỂ GỬI DỮ LIỆU LÊN MÁY TÍNH)
// ==========================================================
void UART2_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // PA2 - TX
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx; // Bài này chỉ cần phát (Tx)
    USART_Init(USART2, &USART_InitStructure);

    USART_Cmd(USART2, ENABLE);
}

void UART2_SendString(const char* str) {
    while (*str) {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
        USART_SendData(USART2, *str++);
    }
}

// ==========================================================
// 3. CẤU HÌNH ADC1 ĐỂ ĐỌC CẢM BIẾN/BIẾN TRỞ TẠI CHÂN PA0
// ==========================================================
void ADC1_Init(void) {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);

    // Cấu hình chân PA0 là Analog Input
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; // Chế độ Analog
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Cấu hình bộ ADC1
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // Đọc đơn lẻ
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Cmd(ADC1, ENABLE);

    // Hiệu chỉnh ADC (Bắt buộc để số đo chính xác)
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

uint16_t ADC1_Read(void) {
    // Đọc kênh 0 (tương ứng chân PA0)
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); // Chờ đọc xong
    return ADC_GetConversionValue(ADC1);
}

// ==========================================================
// 4. CHƯƠNG TRÌNH CHÍNH
// ==========================================================
int main(void) {
    // Khởi tạo xung nhịp hệ thống
    SystemInit(); 

    UART2_Init();
    ADC1_Init();

    char buffer[100];
    uint16_t adc_value;
    float voltage;

    UART2_SendString("\r\n--- BAT DAU DOC ADC BAITAP03 ---\r\n");

    while (1) {
        // Đọc giá trị thô từ ADC (Từ 0 đến 4095)
        adc_value = ADC1_Read();

        // Tính toán điện áp (Chip chạy 3.3V, ADC 12-bit nên chia 4095)
        voltage = ((float)adc_value * 3.3f) / 4095.0f;

        /* MẸO IN SỐ THẬP PHÂN: 
           Tách phần nguyên và phần thập phân ra để in. */
        int v_nguyen = (int)voltage;
        int v_thapphan = (int)((voltage - v_nguyen) * 1000); // Lấy 3 số lẻ

        // Đóng gói thành chuỗi
        sprintf(buffer, "Gia tri ADC: %4d | Dien ap: %d.%03d V\r\n", adc_value, v_nguyen, v_thapphan);
        
        // Gửi lên máy tính
        UART2_SendString(buffer);

        // Chờ đúng 1 giây (1000 ms)
        Delay_ms(1000);
    }
}
