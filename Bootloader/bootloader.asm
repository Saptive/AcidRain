[BITS 16]
[ORG 0x7C00]      						; BIOS loads bootloader here

start:
    xor ax, ax
    mov ds, ax     						; Set data segment to 0
    mov es, ax     						; Set extra segment to 0
	mov ss, ax
	mov sp, 0x9000

    mov si, msg
    call printString

	mov ah, 0         					; Function 0: Wait for a key press
	int 16h           					; call interrupt

    call loadKernel  					; Load kernel into memory

    cli
    lgdt [gdt_descriptor]  			  	; Load Global Descriptor Table (GDT)
    mov eax, cr0
    or eax, 1
    mov cr0, eax     				  	; Enable protected mode
    jmp CODE_SEG:init_protected_mode  	; Far jump to protected mode

printString:
    lodsb
    or al, al
    jz done
    mov ah, 0x0E
    int 0x10
    jmp printString
done:
    ret

loadKernel:
    mov ah, 0x02    					; BIOS interrupt 13h: Read disk
    mov al, 20      					; Read 10 sectors (adjust for kernel size)
    mov ch, 0       					; Cylinder 0
    mov cl, 2       					; Sector 1 on the disk (here it starts from 1 not 0!!!!)
    mov dh, 0       					; Head 0
    mov dl, 0x80    					; Primary HDD
    mov bx, 0x1000  					; Load kernel to 0x1000 (4 KB)
    int 0x13
    jc diskError    					; If carry flag is set, disk error
    ret

diskError:
    mov si, errormsg
    call printString
    hlt

msg 	 db "Loading Kernel...", 13, 10, "Press any key...", 0
errormsg db "Disk Read Error!", 0

gdt:
    dq 0x0000000000000000   			; Null descriptor

	; 32-bit code segment
	dw 0ffffh
	dw 0
	db 0
	db 10011010b
	db 11001111b
	db 0

	; 32-bit data segment
	dw 0ffffh
	dw 0
	db 0
	db 10010010b
	db 11001111b
	db 0

    ;dq 0x00CF9A000000FFFF   			; Code segment
    ;dq 0x00CF92000000FFFF   			; Data segment

gdt_descriptor:
    dw gdt_descriptor - gdt - 1
    dd gdt

CODE_SEG equ 0x08
DATA_SEG equ 0x10

[BITS 32]
init_protected_mode:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9000  					; Set stack pointer

	;mov ebx, 0xb8000
	;mov esi, hello
l00p:
	;lodsb
	;or al, al
	;jz vege
	;mov byte [ebx], al
	;add ebx, 2
	;jmp l00p

vege:
	;mov ebx, 0xb8000
	;mov esi, 0x1000
	;call dsp_byte

	;add ebx, 4
	;add esi, 1
	;call dsp_byte

	;add ebx, 4
	;add esi, 1
	;call dsp_byte

	;add ebx, 4
	;add esi, 1
	;call dsp_byte

	;add ebx, 4
	;add esi, 1
	;call dsp_byte

	;add ebx, 4
	;add esi, 1
	;call dsp_byte

	jmp 0x1000       					; Jump to kernel entry at 0x1000

		
infinite:
	jmp infinite

dsp_byte:
	pusha
	mov edx, hex
	lodsb
	xor ecx, ecx
	mov cl, al
	and cl, 00001111b
	mov edi, edx
	add edi, ecx
	mov ch, [edi]
	mov byte [ebx + 2], ch

	xor ecx, ecx
	mov cl, al
	shr cl, 4
	mov edi, edx
	add edi, ecx
	mov ch, [edi]
	mov byte [ebx], ch
	popa
	ret
	
hello db "a---------------------mari neni tokosmakos retese", 0
hex   db "0123456789ABCDEF!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
dbgmessage db "dbgMessage reached"
	
times 510-($-$$) db 0
dw 0xAA55