#include "stm32f103xb.h"

void delay(volatile uint32_t time) {
    while (time--);
}

int main(void) {
    // 1. Bật clock cho GPIOC (APB2 peripheral clock enable register, bit 4)
    RCC->APB2ENR |= (1 << 4);

    // 2. Cấu hình chân PC13 làm Output Push-Pull 2MHz (CRH bits [23:20])
    GPIOC->CRH &= ~(0xF << 20); 
    GPIOC->CRH |=  (0x2 << 20); 

    while (1) {
        // Đảo trạng thái chân PC13
        GPIOC->ODR ^= (1 << 13);
        
        // Delay tương đối để mắt thường kịp nhìn thấy
        delay(1000000);
    }
}
