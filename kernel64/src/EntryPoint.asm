; file      EntryPoint.asm
; date      2008/11/27
; author    kkamagui
;           Copyright(c)2008 All rights reserved by kkamagui
; brief     IA-32e 모드 커널 엔트리 포인트

[BITS 64]

SECTION .text

extern main

START:
    mov ax, 0x10 ; set the address of data segment descriptor
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov es, ax

    mov ss, ax
    mov rsp, 0x6FFFF8
    mov rbp, 0x6FFFF8

    call main

    jmp $
