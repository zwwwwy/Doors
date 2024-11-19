org 0x10000
bits 16

%define FILE_BASE       0x9000
%define INFO_OFFSET     0x0000
%define KERNEL_OFFSET   0x0200

; FAT12引导扇区
    jmp             start_loader
    nop                                 ; 第一个数字是偏移量, base=0x7c00
    BS_OEMName      db "Doors   "       ;  3 八个字节
    BPB_BytesPerSec dw 512              ; 11 每扇区字节数
    BPB_SecPerClus  db 1                ; 13 每簇扇区数
    BPB_RsvdSecCnt  dw 1                ; 14 保留扇区数. 只有本扇区, 其他的文件都在FAT系统内
    BPB_NumFATs     db 2                ; 16 FAT表的份数
    BPB_RootEntCnt  dw 224              ; 17 根目录可容纳的目录项数
    BPB_TotSec16    dw 2880             ; 19 总扇区数, 含保留扇区等
    BPB_Media       db 0xF0             ; 21 介质描述符->可移动介质
    BPB_FATSz16     dw 9                ; 22 单个FAT表的扇区数
    BPB_SecPerTrk   dw 18               ; 24 每磁道扇区数
    BPB_NumHeads    dw 2                ; 26 磁头数
    BPB_HiddSec     dd 0                ; 28 隐藏磁头数
    BPB_TotSec32    dd 0                ; 32 若TotSec16和TotSec32二选一
    BS_DrvNum       db 0                ; 36 int 13h的驱动器号
    BS_Reserved1    db 0                ; 37 预留
    BS_BootSig      db 0x29             ; 38 拓展引导标记
    BS_VolID        dd 0                ; 39 卷序列号
    BS_VolLab       db "boot loader"    ; 43 卷标, 即磁盘名
    BS_FileSysType  db "FAT12   "       ; 54 文件系统类型
; 别动位宽, 第一行代码偏移量应该是62(0x3e), csdn那个表是错的

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
    mov     ax, 0x2401
    int     15h
    push    ax
    in      al, 92h
    or      al, 2
    out     92h, al
    pop     ax


    ;cli
    ;db      0x66
    ;lgdt    [gdt_48]
    ;mov     eax, cr0
    ;or      eax, 1
    ;mov     cr0, eax
    ;sti

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
	dw	0,0			; idt base=0L

gdt_48:
	dw	0x800		; gdt limit=2048, 256 GDT entries
	dw	512+gdt,0x9	; gdt base = 0X9xxxx
