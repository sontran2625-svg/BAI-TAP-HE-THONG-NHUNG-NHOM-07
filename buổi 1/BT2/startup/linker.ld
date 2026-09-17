ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    RAM   (xrw) : ORIGIN = 0x20000000, LENGTH = 20K
}


/* Stack top */
_estack = ORIGIN(RAM) + LENGTH(RAM);


SECTIONS
{
    /* =========================================
     * FLASH
     * ========================================= */

    .text :
    {
        . = ALIGN(4);

        /* Vector table */
        KEEP(*(.isr_vector))

        /* Code */
        *(.text)
        *(.text.*)

        /* Read-only data */
        *(.rodata)
        *(.rodata.*)

        . = ALIGN(4);
    } > FLASH


    /* =========================================
     * DATA
     * ========================================= */

    .data :
    {
        . = ALIGN(4);

        _sdata = .;

        *(.data)
        *(.data.*)

        . = ALIGN(4);

        _edata = .;

    } > RAM AT > FLASH


    /* =========================================
     * BSS
     * ========================================= */

    .bss :
    {
        . = ALIGN(4);

        _sbss = .;

        *(.bss)
        *(.bss.*)
        *(COMMON)

        . = ALIGN(4);

        _ebss = .;

    } > RAM


    /* =========================================
     * DISCARD
     * ========================================= */

    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
    }
}
