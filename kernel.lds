SECTIONS
{
    . = 0xffff000000000000;
    .text.boot : { *(.text.boot) }

    . = ALIGN(0x1000);
    user_start = .;
    .text.user : { */user* (.text) }
    .rodata.user : { */user* (.rodata) }
    .data.user : { */user* (.data) }
    .bss.user : { */user* (.bss) }
    user_end = .;

    .text :  { *(.text) }
    .rodata : { *(.rodata) }
    .data : { *(.data) }

    . = ALIGN(0x8);
    bss_start = .;
    .bss : { *(.bss*) } 
    . = ALIGN(0x8);
    bss_end = .;

    . = ALIGN(0x1000);
    fb_start = .;
    .data.fb : { . += (1920 * 1080 * 4); }

    . = ALIGN(0x1000);
    pg_dir = .;
    .data.pgd : { . += (3 * (1 << 12)); }
}