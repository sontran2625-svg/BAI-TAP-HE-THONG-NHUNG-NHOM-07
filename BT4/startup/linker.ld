ENTRY(Reset_Handler)


MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    RAM   (xrw) : ORIGIN = 0x20000000, LENGTH = 20K
}


/* Stack ở cuối RAM */

_estack = ORIGIN(RAM) + LENGTH(RAM);


SECTIONS
{
    /* =================================================
     * CODE + VECTOR TABLE
     * ================================================= */

    .text :
    {
        . = ALIGN(4);

        KEEP(*(.isr_vector))

        *(.text)
        *(.text.*)

        *(.rodata)
        *(.rodata.*)

        . = ALIGN(4);

        _etext = .;

    } > FLASH


    /* =================================================
     * DATA
     * ================================================= */

    .data :
    {
        . = ALIGN(4);

        _sdata = .;

        *(.data)
        *(.data.*)

        . = ALIGN(4);

        _edata = .;

    } > RAM AT > FLASH


    /* Địa chỉ .data nằm trong FLASH */

    _sidata = LOADADDR(.data);


    /* =================================================
     * BSS
     * ================================================= */

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


    /* =================================================
     * DISCARD
     * ================================================= */

    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
    }
}
