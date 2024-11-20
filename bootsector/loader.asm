[org 0x10000]

%define FILE_BASE       0x9000
%define INFO_OFFSET     0x0000
%define KERNEL_OFFSET   0x0200

[section code_16]
[bits 16]
start_loader:
    mov     ax, cs
    mov     ds, ax
    mov     es, ax
    mov     ax, 0
    mov     ss, ax
    mov     sp, 0x7c00

    mov     si, msg1
    mov     dx, 0x0100
    call    fn_print_string


    ; 打开a20地址线
    push    ax
    in      al, 92h
    or      al, 2
    out     92h, al
    pop     ax


    cli
    lgdt    [gdt_48]
    mov     eax, cr0
    or      eax, 1
    mov     cr0, eax

    jmp     dword 8:bits_32

[section code_32]
[bits 32]
bits_32:
    mov     eax, 0xABCDffff
    jmp     $

[section func_16]
[bits 16]
fn_print_char:
    ; 打印单个字符，认为其位于di中, 光标位置需提前设好
    push    ax
    push    bx  ; fn_print_string的两个调用都用到了ax,bx
    mov     ax, di
    mov     ah, 0x09
    mov     bx, 0x000f
    mov     cx, 1
    int     10h
    pop     bx
    pop     ax
    ret


fn_print_string:
    ; 字符串起始地址放在si，逢0停止输出, 起始光标坐标放在dx
    push    bx
    push    cx
.print:
    ; 改变光标位置
    mov     ah, 0x2
    xor     bx, bx
    int     10h

    push    ax              ; 直接从内存读到si会读俩字节，实在找不到别的寄存器用了...
    xor     ax, ax
    mov     al, byte [si]
    mov     di, ax
    pop     ax
    cmp     di, 0
    je      .finish_print
    call    fn_print_char
    inc     si
    inc     dl              ; 右移光标

    jmp     .print
.finish_print:
    pop     cx
    pop     bx
    ret


msg1:
    db "build file system successfully, enter into loader, fuck yeah!!!!!", 0

[section tables]
gdt:
	dw	0,0,0,0		; dummy

	dw	0x07FF		; 8Mb - limit=2047 (2048*4096=8Mb)
	dw	0x0000		; base address=0
	dw	0x9A00		; code read/exec
	dw	0x00C0		; granularity=4096, 386

	dw	0x07FF		; 8Mb - limit=2047 (2048*4096=8Mb)
	dw	0x0000		; base address=0
	dw	0x9200		; data read/write
	dw	0x00C0		; granularity=4096, 386

idt_48:
	dw	0			; idt limit=0
	dd	0,0			; idt base=0L

gdt_48:
	dw	0x800		; gdt limit=2048, 256 GDT entries
	dd	gdt         ; nasm如此可直接获得全局便宜

    times 512 db 6
