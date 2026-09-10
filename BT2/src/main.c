#include <stdint.h>

/* =====================================================
 * STM32F103
 * GPIOA REGISTER
 * ===================================================== */

/* RCC APB2 Peripheral Clock Enable Register */
#define RCC_APB2ENR   (*(volatile uint32_t *)0x40021018)

/* GPIOA Configuration Register Low */
#define GPIOA_CRL     (*(volatile uint32_t *)0x40010800)

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
 * GPIOA INITIALIZATION
 *
 * PA0 -> PA7
 * Output Push-Pull
 * Speed 2 MHz
 * ===================================================== */

static void GPIOA_Init(void)
{
    /* Bat clock GPIOA */
    RCC_APB2ENR |= (1 << 2);

    /*
     * PA0 -> PA7:
     * Output Push-Pull, 2 MHz
     */
    GPIOA_CRL = 0x22222222;

    /* Tat tat ca LED */
    GPIOA_ODR &= ~0xFF;
}


/* =====================================================
 * MAIN - BAI TAP 02
 * ===================================================== */

int main(void)
{
    uint32_t led;

    GPIOA_Init();

    while (1)
    {
        /* =============================================
         * PA0 -> PA7
         * ============================================= */

        for (led = 0; led < 8; led++)
        {
            /* Tat tat ca LED */
            GPIOA_ODR &= ~0xFF;

            /* Bat LED hien tai */
            GPIOA_ODR |= (1 << led);

            /* Delay */
            delay_ms(100);
        }


        /* =============================================
         * PA7 -> PA0
         *
         * Bat dau tu PA6 de khong lap PA7
         * ============================================= */

        for (led = 6; led < 7; led--)
        {
            /* Tat tat ca LED */
            GPIOA_ODR &= ~0xFF;

            /* Bat LED hien tai */
            GPIOA_ODR |= (1 << led);

            /* Delay */
            delay_ms(100);

            /*
             * Khi led = 0 thi dung vong lap
             *
             * unsigned nen khong dung led >= 0
             */
            if (led == 0)
                break;
        }
    }
}
