#include "stm32f10x.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// ====================================================================
// 1. CÁC BIẾN TOÀN CỤC CHO UART
// ====================================================================
volatile char rx_buffer[30];
volatile uint8_t rx_index = 0;
volatile uint8_t command_ready = 0; 

// ====================================================================
// 2. CẤU HÌNH NGOẠI VI BẰNG THƯ VIỆN STD
// ====================================================================

void Timer2_CH2_PA1_PWM_Init(void) {
    // 1. Bật xung nhịp cho GPIOA và TIM2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // 2. Cấu hình chân PA1 (TIM2_CH2)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // Alternate Function Push-Pull
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3. Cấu hình Time Base cho TIM2 (Tạo chu kỳ 1000)
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;       // Chia tần xuống 1MHz (giả sử SystemCoreClock = 72MHz)
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1;        // Đếm đến 1000
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // 4. Cấu hình PWM Kênh 2
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;                  // Độ rộng xung ban đầu = 0
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_Cmd(TIM2, ENABLE);
}

void PWM_SetPercent(uint8_t percent) {
    if (percent > 100) percent = 100;
    // Chỉnh thanh ghi CCR2 qua hàm STD (nhân 10 vì Period là 1000)
    TIM_SetCompare2(TIM2, percent * 10); 
}

void UART2_Init_Interrupt(void) {
    // 1. Bật xung nhịp cho GPIOA và USART2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // 2. Cấu hình chân PA2 (TX) và PA3 (RX)
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // PA2 - TX (Alternate Function Push-Pull)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // PA3 - RX (Input Floating)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3. Cấu hình UART (Thư viện tự lo việc tính toán Baudrate)
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    // 4. Cấu hình Ngắt UART
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); // Bật ngắt khi nhận được dữ liệu (RXNE)
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 5. Bật UART
    USART_Cmd(USART2, ENABLE);
}

void UART2_SendString(const char* str) {
    while (*str) {
        // Chờ cờ TXE (Transmit Data Register Empty) set lên 1
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
        USART_SendData(USART2, *str++);
    }
}

// ====================================================================
// 3. TRÌNH PHỤC VỤ NGẮT UART2
// ====================================================================
void USART2_IRQHandler(void) {
    // Kiểm tra xem ngắt có thực sự do nhận dữ liệu (RXNE) gây ra không
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        char c = (char)USART_ReceiveData(USART2);
        
        // Echo dội âm lên PC
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
        USART_SendData(USART2, c);
        
        if (c == '!') { // Ký tự kết thúc lệnh
            rx_buffer[rx_index] = '\0'; 
            command_ready = 1;          
        } 
        else if (c >= 32) {
            if (rx_index < 29) {
                rx_buffer[rx_index++] = c;
            } else {
                rx_index = 0; 
            }
        }
    }
}

// ====================================================================
// 4. CHƯƠNG TRÌNH CHÍNH
// ====================================================================
int main(void) {
    // Luôn gọi SystemInit để đảm bảo chip chạy đúng tốc độ thạch anh
    SystemInit(); 

    // Dọn rác khởi động RAM
    rx_index = 0;
    command_ready = 0;
    memset((void*)rx_buffer, 0, sizeof(rx_buffer)); 

    Timer2_CH2_PA1_PWM_Init();
    UART2_Init_Interrupt();

    uint8_t is_on = 0;           
    uint8_t current_pwm = 0;     
    char tx_buf[64]; // Buffer để in chuỗi phản hồi

    UART2_SendString("\r\n--- HE THONG SAN SANG ---\r\n");

    while (1) {
        if (command_ready) {
            // Xử lý các lệnh bằng thư viện string.h chuẩn
            if (strcmp((char*)rx_buffer, "ON") == 0) {
                is_on = 1;
                if (current_pwm == 0) {
                    current_pwm = 80; 
                }
                PWM_SetPercent(current_pwm);
                UART2_SendString("\r\n-> Den Da BAT\r\n");
            } 
            else if (strcmp((char*)rx_buffer, "OFF") == 0) {
                is_on = 0;
                PWM_SetPercent(0); 
                UART2_SendString("\r\n-> Den Da TAT\r\n");
            } 
            else if (strcmp((char*)rx_buffer, "Status") == 0) {
                // Dùng sprintf thay vì tự nối chuỗi
                sprintf(tx_buf, "\r\nTrang thai: %s, Do sang: %d%%\r\n", is_on ? "ON" : "OFF", current_pwm);
                UART2_SendString(tx_buf);
            } 
            else if (strncmp((char*)rx_buffer, "PWM:", 4) == 0) {
                // Dùng atoi (ASCII to Integer) để lấy số
                int duty = atoi((char*)&rx_buffer[4]); 
                
                if (duty > 100) duty = 100;
                if (duty < 0) duty = 0;
                
                current_pwm = (uint8_t)duty; 
                
                if (is_on) {
                    PWM_SetPercent(current_pwm);
                }
                UART2_SendString("\r\n-> Da cap nhat PWM\r\n");
            }
            else {
                UART2_SendString("\r\n[Loi] Sai cu phap!\r\n");
            }

            // Xóa sạch bộ đệm và reset index
            memset((void*)rx_buffer, 0, sizeof(rx_buffer));
            rx_index = 0;
            command_ready = 0;
        }
    }
}
