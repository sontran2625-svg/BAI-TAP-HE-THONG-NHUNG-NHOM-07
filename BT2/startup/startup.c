#include <stdint.h>

/* =====================================================
 * Linker symbol
 * ===================================================== */

extern uint32_t _estack;


/* =====================================================
 * Main function
 * ===================================================== */

extern int main(void);


/* =====================================================
 * Reset Handler
 * ===================================================== */

void Reset_Handler(void)
{
    main();

    while (1)
    {
    }
}


/* =====================================================
 * Default Handler
 * ===================================================== */

void Default_Handler(void)
{
    while (1)
    {
    }
}


/* =====================================================
 * Interrupt Vector Table
 * ===================================================== */

__attribute__((section(".isr_vector")))
const uint32_t vector_table[] =
{
    /* Initial Stack Pointer */
    (uint32_t)&_estack,

    /* Reset Handler */
    (uint32_t)Reset_Handler,

    /* NMI */
    (uint32_t)Default_Handler,

    /* Hard Fault */
    (uint32_t)Default_Handler,

    /* Memory Management */
    (uint32_t)Default_Handler,

    /* Bus Fault */
    (uint32_t)Default_Handler,

    /* Usage Fault */
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
