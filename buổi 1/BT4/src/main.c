#include <stdint.h>

/* =====================================================
 * STM32F103 REGISTER
 * ===================================================== */

/* RCC APB2 Peripheral Clock Enable Register */
#define RCC_APB2ENR   (*(volatile uint32_t *)0x40021018)

/* GPIOA Configuration Register Low */
#define GPIOA_CRL     (*(volatile uint32_t *)0x40010800)

/* GPIOA Input Data Register */
#define GPIOA_IDR     (*(volatile uint32_t *)0x40010808)

/* GPIOA Output Data Register */
#define GPIOA_ODR     (*(volatile uint32_t *)0x4001080C)


/* =====================================================
 * DELAY
 * ===================================================== */

static void delay_ms(uint32_t ms)
{
    volatile uint32_t i;
    volatile uint32_t j;

    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 8000; j++)
        {
            __asm volatile ("nop");
        }
    }
}


/* =====================================================
 * GPIO INIT
 *
 * PA0 = INPUT PULL-UP
 * PA1 = OUTPUT PUSH-PULL
 * ===================================================== */

static void GPIOA_Init(void)
{
    /* Enable clock GPIOA */
    RCC_APB2ENR |= (1 << 2);


    /*
     * PA0:
     *
     * CNF = 10
     * MODE = 00
     *
     * Input Pull-up / Pull-down
     *
     * PA0 nibble = 1000
     */


    /*
     * PA1:
     *
     * CNF = 00
     * MODE = 10
     *
     * Output Push-Pull
     * Speed = 2 MHz
     *
     * PA1 nibble = 0010
     */


    /*
     * PA1 PA0
     *
     * 0010 1000
     *
     * = 0x28
     */
    GPIOA_CRL &= ~0xFF;
    GPIOA_CRL |= 0x28;


    /*
     * PA0 = 1
     *
     * Chọn Pull-up cho PA0
     */
    GPIOA_ODR |= (1 << 0);


    /*
     * PA1 = 0
     *
     * LED ban đầu tắt
     */
    GPIOA_ODR &= ~(1 << 1);
}


/* =====================================================
 * MAIN
 *
 * PA0 = BUTTON
 * PA1 = LED
 *
 * Nhấn + nhả nút:
 * LED đảo trạng thái
 * ===================================================== */

int main(void)
{
    /*
     * led_state:
     *
     * 0 = LED OFF
     * 1 = LED ON
     */
    uint8_t led_state = 0;


    /* Khởi tạo GPIO */
    GPIOA_Init();


    while (1)
    {
        /*
         * Kiểm tra nút nhấn
         *
         * PA0 = 0 -> đang nhấn
         */
        if ((GPIOA_IDR & (1 << 0)) == 0)
        {
            /*
             * Chống dội phím
             */
            delay_ms(20);


            /*
             * Kiểm tra lại nút
             */
            if ((GPIOA_IDR & (1 << 0)) == 0)
            {
                /*
                 * Chờ người dùng NHẢ nút
                 *
                 * Khi nhả:
                 * PA0 = 1
                 */
                while ((GPIOA_IDR & (1 << 0)) == 0)
                {
                }


                /*
                 * Chống dội khi nhả
                 */
                delay_ms(20);


                /*
                 * Đảo trạng thái LED
                 */
                led_state = !led_state;


                if (led_state)
                {
                    /* LED ON */
                    GPIOA_ODR |= (1 << 1);
                }
                else
                {
                    /* LED OFF */
                    GPIOA_ODR &= ~(1 << 1);
                }
            }
        }
    }
}
