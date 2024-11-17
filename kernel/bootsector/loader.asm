org 0x10000
bits 16

start:
    mov     ax, cs
    mov     ds, ax
    mov     es, ax
    mov     ax, 0
    mov     ss, ax
    mov     sp, 0x7c00

    mov     si, msg1
    mov     dx, 0x0100
    call    fn_print_string
    jmp     $


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
