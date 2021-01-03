[ORG 0x00]
[BITS 16]

SECTION .text

mov ax, 0xB800
mov ds, ax

mov byte [0x00], 'M'
mov byte [0x01], 0x4A

jmp $

times 510 - ( $ - $$ ) db 0x00

db 0x55
db 0xAA