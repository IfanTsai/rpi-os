SECTIONS
{
    .text.boot : { *(.text.boot) }
    .text :  { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }

    . = ALIGN(0x8);
    bss_start = .;
    .bss : { *(.bss*) } 
    . = ALIGN(0x8);
    bss_end = .;
}