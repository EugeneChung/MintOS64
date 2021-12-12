[ORG 0x00]
[BITS 16]

SECTION .text

START:
    mov ax, 0x1000
    mov ds, ax          ; set the entry point address of protected mode (0x1000 * 0x10 = 0x10000)
    mov es, ax

    cli                 ; block interrupt and set GDTR
    lgdt [GDTR]
    mov eax, 0x4000003B ; Diable Paging/Cache, Internal FPU, Diable Align Check
    mov cr0, eax

    jmp dword 0x08: (PROTECTED_MODE - $$ + 0x10000)

;
; From now on, it's protected mode
;
[BITS 32]
PROTECTED_MODE:
    mov ax, 0x10

    ; set segement registers
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; set stack
    mov ss, ax
    mov esp, 0xFFFE
    mov ebp, 0xFFFE

    push (SWITCH_SUCCESS_MESSAGE - $$ + 0x10000)
    push 2
    push 0
    call FUNC_PRINT_MESSAGE
    add esp, 12

    jmp dword 0x08: 0x10200 ; jump to the kernel with setting CS register to 0x08(kernel code segment)

; function FUNC_PRINT_MESSAGE(x, y, message) for 32bits
FUNC_PRINT_MESSAGE:
    push ebp
    mov ebp, esp

    ; push values of registers into stack for backup
    push esi
    push edi
    push eax
    push ecx
    push edx

    ; calculate video memory address denoted by (x, y)
    mov eax, dword [ebp + 12] ; y
    mov esi, 160
    mul esi
    mov edi, eax

    mov eax, dword [ebp + 8] ; x
    mov esi, 2
    mul esi
    add edi, eax

    mov esi, dword [ebp + 16] ; set message parameter address to esi register

.MESSAGE_LOOP:
    mov cl, byte [esi]
    cmp cl, 0
    je .MESSAGEEND

    mov byte [edi + 0xB8000], cl

    add esi, 1               ; move to the next byte
    add edi, 2               ; move to the next video memory address

    jmp .MESSAGE_LOOP

.MESSAGEEND:
    pop edx
    pop ecx
    pop eax
    pop edi
    pop esi
    pop ebp
    ret

;
; Data Area
;
align 8, db 0

dw 0x0000               ; for GDTR padding
GDTR:
    dw GDT_END - GDT - 1
    dd (GDT - $$ + 0x10000)

GDT:
    NULL_DESCRIPTOR:
        dw 0x0000
        dw 0x0000
        db 0x00
        db 0x00
        db 0x00
        db 0x00

    CODE_DESCRIPTOR:
        dw 0xFFFF
        dw 0x0000
        db 0x00
        db 0x9A       ; P=1, DPL=0, Code Segement, Execute/Read
        db 0xCF       ; G=1, D=1, L=0
        db 0x00

    DATA_DESCRIPTOR:
        dw 0xFFFF
        dw 0x0000
        db 0x00
        db 0x92       ; P=1, DPL=0, Data Segement, Read/Write
        db 0xCF       ; G=1, D=1, L=0
        db 0x00
GDT_END:

SWITCH_SUCCESS_MESSAGE: db 'Switched To Protected Mode', 0

times 512 - ($ - $$) db 0x00
