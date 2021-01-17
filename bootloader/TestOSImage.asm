[ORG 0x00]
[BITS 16]

SECTION .text

jmp 0x1000:START

SECTOR_COUNT dw 0x00
TOTAL_SECTOR_COUNT equ 1024 ; the total number of sectors for the test OS image

START:
    mov ax, cs
    mov ds, ax
    mov ax, 0xB800
    mov es, ax              ; set the video memory starting address to ES register (0x8B00 * 0x10 = 0xB8000)

    ; generate the OS code for each sector
    %assign i 0
    %rep TOTAL_SECTOR_COUNT
        %assign i i + 1

        mov ax, 2
        mul word [SECTOR_COUNT]
        mov si, ax
        mov byte [es: si + (160 * 2)], '0' + (i % 10)
        add word [SECTOR_COUNT], 1

        %if i == TOTAL_SECTOR_COUNT
            jmp $
        %else
            jmp (0x1000 + i * 0x20):0x0000
        %endif

        times (512 - ($ - $$) % 512) db 0x00
    %endrep