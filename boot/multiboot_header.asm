section .multiboot_header
header_start:
    dd 0xe85250d6                ; magic number (multiboot 2)
    dd 0                         ; architecture 0 (protected mode i386)
    dd header_end - header_start ; header length
    ; checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; insert optional multiboot tags here

framebuffer_tag_start:
    dw 5    ; MULTIBOOT_HEADER_TAG_FRAMEBUFFER
    dw 1    ; MULTIBOOT_HEADER_TAG_OPTIONAL
    dd (framebuffer_tag_end - framebuffer_tag_start) ; size
    dd 800  ; width
    dd 600  ; height
    dd 32   ; bpp
framebuffer_tag_end:
    dw 0    ; MULTIBOOT_HEADER_TAG_END

    ; required end tag
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end:

