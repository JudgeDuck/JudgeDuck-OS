ENTRY(start)

SECTIONS {
    . = 1M;

    .boot : {
        /* ensure that the multiboot header is at the beginning */
        KEEP (*(.multiboot_header))
    }

    .text : {
        *(.text .text.*)
    }
    
    .rodata : {
        *(.rodata .rodata.*)
    }
    
    .preinit_array : {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP (*(SORT(.preinit_array.*)))
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    }
    
    .init_array : {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
    }
    
    .fini_array : {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
    }
    
    .tdata : {
        *(.tdata)
    }
    .tbss : {
        *(.tbss)
        . = . + 0x1000;
        PROVIDE(__duck64__tls_base = .);
        . = . + 0x1000;  /* Some space maybe for canary ? */
    }
    
    .data : {
        *(.data .data.*)
    }
    
    .bss : {
        *(.bss .bss.*)
    }
    
    /DISCARD/ : {
        *(.eh_frame .note.GNU-stack .comment)
    }
}
