#include <stdint.h>


/* =====================================================
 * EXTERNAL SYMBOLS FROM LINKER
 * ===================================================== */

extern uint32_t _estack;

extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;

extern uint32_t _sbss;
extern uint32_t _ebss;


/* =====================================================
 * MAIN
 * ===================================================== */

extern int main(void);


/* =====================================================
 * RESET HANDLER
 * ===================================================== */

void Reset_Handler(void)
{
    uint32_t *src;
    uint32_t *dst;


    /* ---------------------------------------------
     * Copy .data from FLASH to RAM
     * --------------------------------------------- */

    src = &_sidata;
    dst = &_sdata;

    while (dst < &_edata)
    {
        *dst++ = *src++;
    }


    /* ---------------------------------------------
     * Clear .bss
     * --------------------------------------------- */

    dst = &_sbss;

    while (dst < &_ebss)
    {
        *dst++ = 0;
    }


    /* ---------------------------------------------
     * Call main()
     * --------------------------------------------- */

    main();


    /* ---------------------------------------------
     * Should never reach here
     * --------------------------------------------- */

    while (1)
    {
    }
}


/* =====================================================
 * DEFAULT INTERRUPT HANDLER
 * ===================================================== */

void Default_Handler(void)
{
    while (1)
    {
    }
}


/* =====================================================
 * VECTOR TABLE
 * ===================================================== */

__attribute__((section(".isr_vector")))
const uint32_t vector_table[] =
{
    /* Initial Stack Pointer */
    (uint32_t)&_estack,

    /* Reset */
    (uint32_t)Reset_Handler,

    /* NMI */
    (uint32_t)Default_Handler,

    /* HardFault */
    (uint32_t)Default_Handler,

    /* MemManage */
    (uint32_t)Default_Handler,

    /* BusFault */
    (uint32_t)Default_Handler,

    /* UsageFault */
    (uint32_t)Default_Handler,

    /* Reserved */
    0,

    /* Reserved */
    0,

    /* Reserved */
    0,

    /* Reserved */
    0,

    /* SVCall */
    (uint32_t)Default_Handler,

    /* Debug Monitor */
    (uint32_t)Default_Handler,

    /* Reserved */
    0,

    /* PendSV */
    (uint32_t)Default_Handler,

    /* SysTick */
    (uint32_t)Default_Handler
};
