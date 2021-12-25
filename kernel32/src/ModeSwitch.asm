; file      ModeSwitch.asm
; date      2009/01/01
; author    kkamagui
;           Copyright(c)2008 All rights reserved by kkamagui
; brief     모드 전환에 관련된 소스 파일

[BITS 32]

; Export labels which make them be called from the C kernel
global _KREAD_CPUID, _KSWITCH_TO_IA32E

SECTION .text

; Return CPUID information
;   PARAM: DWORD eax, DWORD *peax, DWORD *pebx, DWORD *pecx, DWORD *pedx
_KREAD_CPUID:
    push ebp
    mov ebp, esp
    push eax
    push ebx
    push ecx
    push edx
    push esi
    
    mov eax, dword [ebp + 8]    ; save the value of parameter eax to EAX register
    cpuid

    ; Save the CPUID result to each result parameter
    mov esi, dword [ebp + 12]
    mov dword [esi], eax

    mov esi, dword [ebp + 16]
    mov dword [esi], ebx

    mov esi, dword [ebp + 20]
    mov dword [esi], ecx

    mov esi, dword [ebp + 24]
    mov dword [esi], edx

    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    pop ebp
    ret

; Transition to the IA-32e mode
_KSWITCH_TO_IA32E:
    mov eax, cr4
    or eax, 0x20
    mov cr4, eax
    
    mov eax, 0x100000
    mov cr3, eax
    
    mov ecx, 0xC0000080
    rdmsr
        
    or eax, 0x0100
    wrmsr

    mov eax, cr0
    or eax, 0xE0000000
    xor eax, 0x60000000
    mov cr0, eax
    
    jmp 0x08:0x200000   ; jump to the 64-bit kernel at 2MB address with setting CS register to 0x08(kernel code segment)
