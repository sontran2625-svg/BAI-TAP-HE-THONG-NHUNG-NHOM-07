#include "stm32f1xx.h"

int main(void) {
    // 1. Cấp xung nhịp cho GPIOA và đặc biệt phải cấp cho AFIO để cấu hình lại chân PA15
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

    // 2. Cấu hình PA0 - PA7 là Input Pull-up (CRL = 0x88888888)
    GPIOA->CRL = 0x88888888; 
    // Bật điện trở kéo lên cho PA0 - PA7 (đảm bảo khi nhả nút luôn ở mức 1)
    GPIOA->ODR |= 0x00FF; 

    // 3. Vô hiệu hóa JTAG để giải phóng PA15 làm GPIO, giữ nguyên SWD (PA13, PA14) cho ST-Link
    AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG_Msk;
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    // 4. Cấu hình PA8 - PA12 và PA15 là Output Push-Pull 2MHz. 
    // CRH: PA15=2, PA14=4, PA13=4, PA12=2, PA11=2, PA10=2, PA9=2, PA8=2 -> 0x24422222
    GPIOA->CRH = 0x24422222;

    while (1) {
        uint32_t input_val = GPIOA->IDR;

        // Đảo trạng thái nút bấm (0 thành 1)
        uint32_t b0_4 = (~input_val) & 0x001F;       // Lấy 5 bit từ PA0 -> PA4
        uint32_t b7   = (~input_val >> 7) & 0x0001;  // Lấy bit PA7

        // Ánh xạ tương ứng: 
        // - PA0 -> PA4 điều khiển PA8 -> PA12
        // - PA7 điều khiển PA15
        uint32_t led_mask = (b0_4 << 8) | (b7 << 15);

        // Cập nhật ODR nhưng vẫn giữ nguyên:
        // - Các bit 0-7 (giữ điện trở Pull-up cho Input)
        // - Các chân SWD (PA13, PA14)
        uint32_t current_odr = GPIOA->ODR;
        current_odr &= ~((0x1F << 8) | (1 << 15)); // Xóa vùng LED cũ (PA8-PA12 và PA15)
        GPIOA->ODR = current_odr | led_mask;        // Ghi giá trị mới ra LED
    }
}
