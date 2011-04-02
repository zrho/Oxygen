OUTPUT_FORMAT(elf64-x86-64)
SECTIONS
{
    .text 0x100000 :
    {
        code = .; _code = .; __code = .;
        *(.text)
        . = ALIGN(4096);
    }

    .data :
    {
        data = .; _data = .; __data = .;
        *(.data)
        *(.rodata)
        . = ALIGN(4096);
    }

    .bss :
    {
        bss = .; _bss = .; __bss = .;
        *(.bss)
        . = ALIGN(4096);
    }
    
    .eh_frame : 
    {
        *(.eh_frame)
    }
    
    .eh_frame_hdr :
    {
        *(eh_frame_hdr)
    }

    end = .; _end = .; __end = .;
}
