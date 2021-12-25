[ORG 0x00]
[BITS 16]

SECTION .text

jmp 0x07C0:START   ; set the boot loader starting address to CS register (0x07C0 * 0x10 = 0x7C00)

TOTALSECTORCOUNT: dw 1146    ; the size of total kernel image (boot loader itself is not included)
KERNEL32SECTORCOUNT: dw 0x02 ; the size of the kernel32 image

START:
    mov ax, cs
    mov ds, ax     ; set the boot loader starting address to DS register
    mov ax, 0xB800
    mov es, ax     ; set the video memory starting address to ES register (0x8B00 * 0x10 = 0xB8000)

    ; set stack as 0x0000:0000~0x0000:FFFF
    mov ax, 0x0000
    mov ss, ax
    mov sp, 0xFFFE
    mov bp, 0xFFFE

    mov byte [BOOT_DRIVE], dl ; save the number of the boot drive

; clear screen
    mov si, 0
.SCREEN_CLEAR_LOOP:
    mov byte [es: si], 0
    mov byte [es: si + 1], 0x0A ; set screen color to green on black background
    add si, 2
    cmp si, 85 * 25 * 2
    jl .SCREEN_CLEAR_LOOP

; print some messages
    push WELCOME_MESSAGE
    push 0
    push 0
    call FUNC_PRINT_MESSAGE
    add  sp, 6

    push IMAGE_LOADING_MESSAGE
    push 1
    push 0
    call FUNC_PRINT_MESSAGE
    add  sp, 6

; Load Kernel Image From Disk Using BIOS
; BIOS interrupt call 0x13 : https://www.wikiwand.com/en/INT_13H
RESET_DISK:
    mov ax, 0                       ; [BIOS] Reset Disk Drives
    mov dl, byte [BOOT_DRIVE]
    int 0x13
    jc HANDLE_DISK_RESET_ERROR

    mov ah, 0x08                    ; [BIOS] Read Disk Parameters
    mov dl, byte [BOOT_DRIVE]
    int 0x13
    jc HANDLE_DISK_READ_PARAM_ERROR
    mov byte [LAST_HEAD], dh        ; for floppy disk, 1
    mov al, cl
    and al, 0x3f                    ; take sector information (& 0b00111111)
    mov byte [LAST_SECTOR], al      ; for floppy disk, 18
    mov byte [LAST_TRACK], ch

    ; Let's load the OS image from disk
    mov si, 0x1000                  ; loading target address is 0x10000
    mov es, si
    mov bx, 0x0000

    mov di, word [TOTALSECTORCOUNT] ; to read the defined sectors of disk

READ_DATA:
    cmp di, 0
    je READ_END
    sub di, 0x1

    mov ah, 0x02                    ; [BIOS] Read Sector
    mov al, 0x1                     ; sector count to read = 1
    mov ch, byte [TRACK_NUMBER]
    mov cl, byte [SECTOR_NUMBER]
    mov dh, byte [HEAD_NUMBER]
    mov dl, byte [BOOT_DRIVE]
    int 0x13
    jc HANDLE_DISK_READ_ERROR

    add si, 0x0020                  ; move address as many as the sector size(512 or 0x200 bytes)
    mov es, si

    mov al, byte [SECTOR_NUMBER]
    add al, 0x01
    mov byte [SECTOR_NUMBER], al
    cmp al, byte [LAST_SECTOR]      ; check if it read the last sector of the track
    jle READ_DATA

    add byte [HEAD_NUMBER], 0x01    ; set the current head as 1 and reset the current sector
    mov byte [SECTOR_NUMBER], 0x01
    mov al, byte [LAST_HEAD]
    cmp byte [HEAD_NUMBER], al      ; check if it's the last head
    jle READ_DATA

    mov byte [HEAD_NUMBER], 0x00    ; reset the current head and move to the next track
    add byte [TRACK_NUMBER], 0x01
    jmp READ_DATA

READ_END:
    push LOADING_COMPLETE_MESSAGE
    push 1
    push 0
    call FUNC_PRINT_MESSAGE
    add sp, 6

    jmp 0x1000:0x0000               ; execute the OS image

HANDLE_DISK_RESET_ERROR:
    push DISK_RESET_ERROR_MESSAGE
    push 2
    push 20
    call FUNC_PRINT_MESSAGE
    jmp $

HANDLE_DISK_READ_PARAM_ERROR:
    push DISK_READ_PARAM_ERROR_MESSAGE
    push 2
    push 20
    call FUNC_PRINT_MESSAGE
    jmp $

HANDLE_DISK_READ_ERROR:
    push DISK_READ_ERROR_MESSAGE
    push 2
    push 20
    call FUNC_PRINT_MESSAGE
    jmp $

; function FUNC_PRINT_MESSAGE(x, y, message)
FUNC_PRINT_MESSAGE:
    push bp
    mov bp, sp

    ; push values of registers into stack for backup
    push es
    push si
    push di
    push ax
    push cx
    push dx

    mov ax, 0xB800
    mov es, ax

    ; calculate video memory address denoted by (x, y)
    mov ax, word [bp + 6] ; y
    mov si, 160
    mul si
    mov di, ax

    mov ax, word [bp + 4] ; x
    mov si, 2
    mul si
    add di, ax

    mov si, word [bp + 8] ; set message parameter address to si register

.MESSAGE_LOOP:
    mov cl, byte [si]
    cmp cl, 0
    je .MESSAGEEND

    mov byte [es: di], cl

    add si, 1               ; move to the next byte
    add di, 2               ; move to the next video memory address

    jmp .MESSAGE_LOOP

.MESSAGEEND:
    pop dx
    pop cx
    pop ax
    pop di
    pop si
    pop es
    pop bp
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Messages
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
WELCOME_MESSAGE:               db  'Mint64 OS Boot Loader', 0
IMAGE_LOADING_MESSAGE:         db  'Loading OS Image ...', 0
LOADING_COMPLETE_MESSAGE:      db  'OS Image Loaded     ', 0
DISK_RESET_ERROR_MESSAGE:      db  'Disk Reset Error', 0
DISK_READ_PARAM_ERROR_MESSAGE: db  'Disk Read Parameters Error', 0
DISK_READ_ERROR_MESSAGE:       db  'Disk Read Sector Error', 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Variables
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
SECTOR_NUMBER:           db  0x02
HEAD_NUMBER:             db  0x00
TRACK_NUMBER:            db  0x00
BOOT_DRIVE:              db  0x00
LAST_SECTOR:             db  0x00
LAST_HEAD:               db  0x00
LAST_TRACK:              db  0x00


times 510 - ($ - $$) db 0x00

db 0x55
db 0xAA