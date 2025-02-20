ENTRY(_start) /* Define entry point */

SECTIONS 
{
    . = 0x1000; /* Load at 4KB (0x1000)*/

    .text : ALIGN(0x1000)
    {
        *(.text)  /* Code section */
    }

    .data : ALIGN(0x1000)
    {
        *(.data)  /* Data section */
    }

    .bss : ALIGN(0x1000)
    {
        *(COMMON) /* Uninitialized data */
        *(.bss)
    }
}