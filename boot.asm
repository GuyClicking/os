;boot.asm
[ORG 0x7c00]
[BITS 16]
    jmp main
;    %include "print.asm"
main:
    ; Initialise ds with 0 so it isn't random stuff
    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov sp, 0x7C00
    cld

    mov byte [drive], dl

    mov ah, 0x00
    int 0x13

    xor ax, ax
    mov es, ax
    mov ax, 0x9c00
    mov bx, ax

    mov ah, 0x02
    mov al, 0x02
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, [drive]
    int 0x13

	; enable A20 line
	mov ax,0x2401
	int 0x15
	jb hang
	cmp ah,0
	jnz hang

	cli

	;mov ah, 0x00
	;mov al, 0x13
	;int 0x10
    mov ax, 0x4f02
    mov bx, 0x4103
    int 0x10

	; protected mode i think
	lgdt [gdtinfo]

    jmp start

hang:
	jmp hang

drive db 0x00

gdtinfo:
	dw gdtend - gdt - 1
	dw gdt

gdt dd 0,0
gdtend:

    ; -($-$$) means just subtract the amount of bytes in the section prior from 512
    times 510-($-$$) db 0x00
    db 0x55
    db 0xAA

start:
; lol the c code is written directly after this so it runs that wow

; old code ignore this
;    call _start
;    jmp hang
;    mov ax, 0x4f02
;    mov bx, 0x100
;    int 0x10
;
;    mov byte [c], 200
;
;draw:
;    mov word [x], 0
;    mov word [y], 0
;
;fori:
;    cmp word [x], 400
;    je exiti
;forj:
;    cmp word [y], 640
;    je exitj
;    mov ah, 0x0C
;    mov al, byte [c]
;    mov bh, 0x00
;    mov cx, word [x]
;    mov dx, word [y]
;    int 0x10
;
;    inc word [y]
;    jmp forj
;
;exitj:
;    mov word [y], 0
;    inc word [x]
;    jmp fori
;
;exiti:
;    mov word [x], 0
;    inc byte [c]
;
;    jmp draw
;
;hang:
;    jmp hang
;
;x dw 0
;y dw 0
;c db 0
