%define FILE_BASE       0x9000
%define INFO_OFFSET_VBE 0x0000
%define INFO_OFFSET_MEM 0x0100
%define KERNEL_BASE     0x1000
%define KERNEL_OFFSET   0x0000

%define KERNEL_ADDRESS    0x100000  ; 转移后的kernel地址
%define TMP_KERNEL_START   0x10000
%define TMP_KERNEL_END     0x70000
%define KERNEL_SIZE     TMP_KERNEL_END-TMP_KERNEL_START

%define START_OF_DATA_SECT    3180  ; 数据区起始扇区号
%define START_OF_FAT1_SECT      32  ; FAT1起始扇区号
%define START_OF_ROOT_CLUS       2  ; 根目录起始簇号
%define BIAS_OF_DATA_SECT     3178  ; 调整后的数据区起始扇区号(用来和簇号相加计算实际逻辑扇区号)

%define NAME_LEN                11  ; 文件名长度

%define SECT_BASE           0x8000  ; 检索文件的时候把根目录项的一个扇区暂存在这里
%define SECT_OFFSET              0  ; offset of up
%define FAT_BASE            0x7000
%define FAT_OFFSET          0xfe00

%define BPB_SecPerTrk           63  ; 每磁道扇区数         
%define BS_DrvNum             0x80  ; 驱动器号

%define NUM_OF_H                16  ; 磁头数 
%define NUM_OF_S                63  ; 扇区数
%define H_MUL_S               1008  ; h·s

%define FAT_PER_SECT           128  ; 每扇区的FAT表项数
%define SECT_PER_CLUS            1  ; 每簇扇区数

[org 0x91000]
[section code_16]
[bits 16]
start_loader:
    mov     ax, 0x9000
    mov     ds, ax
    mov     es, ax
    mov     ss, ax
    mov     sp, 0x7e00
    ; org这里把msg等标签的offset多算了0x1000，改org的话代码跳转又有问题，只好改段寄存器了
    ; 不知道为什么会这样，当org-0x90200的时候没这个问题

    mov     si, msg
    call    fn_print_in_new_line

    ; 读取目录项
    mov     di, START_OF_ROOT_CLUS
    mov     ax, SECT_BASE
    mov     es, ax
    mov     bx, SECT_OFFSET
    call    fn_read_a_file

    ; 读取kernel.bin并读入KERNEL_BASE:KERNEL_OFFSET
    mov     di, kernel_name
    mov     si, ax
    call    fn_find_a_file_short_name
    cmp     ax, 0
    jnz     read_kernel_successful
    mov     si, load_kernel_failed
    call    fn_print_in_new_line
    jmp     $
read_kernel_successful:
    mov     di, ax
    mov     ax, KERNEL_BASE
    mov     es, ax
    mov     bx, KERNEL_OFFSET
    call    fn_read_a_file
    mov     si, load_kernel_successful
    call    fn_print_in_new_line
    
    ; 保留了0x90000->0x9ffff的空间用来存放硬件信息
    ; 获取VBE信息: 0x90000->0x900ff
    mov     ax, FILE_BASE
    mov     es, ax
    mov     di, INFO_OFFSET_VBE
    mov     ax, 0x4f00
    int     10h

    ; 读取内存信息: 0x90100->0x901ff
    ; 下面是地址范围描述符结构符(ARDS)的说明，其被bios写入es:di处
    ; -------------------------------------------------------
    ; offset    attrname        description
    ; -------------------------------------------------------
    ; 0         BaseAddrLow     基地址低32位
    ; 4         BaseAddrHigh    基地址高32位
    ; 8         LengthLow       内存地址低32位，单位=字节
    ; 12        LengthHigh      内存地址高32位
    ; 16        Type            内存类型，1为可用，2为保留
    ; -------------------------------------------------------
    ;
    ; 此段指令结束后返回的重要值如下：
    ; di 被置为下段信息的偏移量，测试为0x8c
    ;
    mov     ax, FILE_BASE
    mov     es, ax
    mov     di, INFO_OFFSET_MEM
    mov     ebx, 0          ; 内存信息要多次返回，第一次中断置0，此后bios会更新之
read_next_ARDS:
    mov     eax, 0xe820     ; 全是子功能号
    mov     ecx, 20         ; 地址范围描述符结构的大小，目前来说是20字节
    mov     edx, 0x534d4150 ; 这是SMAP的ASCII码，作为签名标记
    int     15h
    jc      fail_to_read_meminfo
    add     di, cx
    cmp     ebx, 0
    jne     read_next_ARDS  ; ebx=0且CF复位说明所有的ARDS读取完成
    jmp     read_meminfo_successful
fail_to_read_meminfo:
    mov     si, ARDS_fault
    call    fn_print_in_new_line
    jmp     $
read_meminfo_successful:
    mov     si, ARDS_success
    call    fn_print_in_new_line

    ; 设置SVGA模式
    mov     ax, 0x4f02
    mov     bx, 0x4180
    int     10h

    ; 打开a20地址线
    push    ax
    in      al, 92h
    or      al, 2
    out     92h, al
    pop     ax

    ; 加载gdt idt，PE置位，进入保护模式，跳转至32位代码
    cli
    lgdt    [gdt_48]
    lidt    [idt_48]
    mov     eax, cr0

    mov     ax, 0x10000

    or      eax, 1
    mov     cr0, eax
    jmp     dword 8:bits_32
    sti

[section code_32]
[bits 32]
bits_32:
    mov     ax, 16
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     ss, ax
    mov     esp, 0x7e00

    ; 把临时kernel文件搬运至0x100000
    mov     esi, TMP_KERNEL_START
    mov     edi, KERNEL_ADDRESS
    mov     ecx, TMP_KERNEL_END
    cld
    rep     movsb

    mov     eax, 0x80000000
    cpuid
    cmp     eax, 0x80000001
    setnb   al
    jb      IA32e_support
    mov     eax, 0x80000001
    cpuid
    bt      edx, 29
    setc    al
IA32e_support:
    movzx   eax, al

    test    eax, eax
    jz      IA32e_not_support

    ; 创建64位临时页表
    mov     dword [0x70000], 0x71007    ; 顶级页表，指向0x71000，可读写用户模式
    mov     dword [0x70800], 0x71007    ; 用于将0xffff800000000000映射到0地址处
                                        ; 地址中PML4共9位，上面地址即100000000(0x100)
                                        ; 每项8个字节，所以偏移应该是0x800

    mov     dword [0x71000], 0x72007    ; 二级页表，指向0x72000

    mov     dword [0x72000], 0x000083   ; 0-0xfffff, 可读写，超级模式，2mb分页
    mov     dword [0x72008], 0x200083   ; 0x200000-0x2fffff
    mov     dword [0x72010], 0x400083   ; 0x400000-0x4fffff
    mov     dword [0x72018], 0x600083   ; 0x600000-0x6fffff
    mov     dword [0x72020], 0x800083   ; 0x800000-0x8fffff
    mov     dword [0x72028], 0xa00083   ; 0xa00000-0xafffff

    lgdt    [gdt_64_ptr]

    ; 开启物理地址扩展功能
    mov     eax, cr4
    bts     eax, 5          ; Bit Test and Set，将第5位置位，原始值存入CF
    mov     cr4, eax

    ; 页目录基址寄存器(cr3)
    mov     eax, 0x70000
    mov     cr3, eax

    ; 开启IA-32e模式
    ;----------------------------------------------------------------------------------
    ; 和PE位一样，进入IA-32e需要将IA32_EFER的LME(低八位)置位，IA32_EFER是MSR寄存器组的
    ; 一个寄存器，MSR即Model Specific Registers，特殊模块寄存器，其中的寄存器不可直接
    ; 访问，而是应该用rdmsr/wrmsr访问。
    ;
    ; 访问前，应向ecx传入寄存器地址，rdmsr向eax返回目标寄存器的低32位，向edx返回高32位。
    ; 在64位下也是这样，只是rax和rdx的高32位补0
    ;
    ; rdmsr和wrmsr须在0特权级或实模式下执行，执行前应先用cpuid验证处理器是否支持msr
    ;----------------------------------------------------------------------------------
    mov     ecx, 0c0000080h
    rdmsr
    bts     eax, 8
    wrmsr

    ; 置位PE、PG
    mov     eax, cr0
    bts     eax, 0
    bts     eax, 31
    mov     cr0, eax

    ; 分页开启完成，跳转至64位代码
    jmp     8:KERNEL_ADDRESS

IA32e_not_support:
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


fn_print_in_new_line:
    ; si=字符串地址
    push    dx
    mov     dx, word [cur_pos]
    add     dx, 0x100           ; 换行
    and     dx, 0xff00          ; 列数清零
    call    fn_print_string
    mov     word [cur_pos], dx
    pop     dx
    ret


fn_read_a_sector:
    ;-------------------------------------
    ; di=起始扇区号(逻辑)
    ; si=读取的扇区数(必须小于255!!!)
    ; es:bx=缓冲区地址
    ; 无返回值
    ;-------------------------------------
    push    dx
    push    bx

    xor     dx, dx
    mov     ax, di
    mov     bx, H_MUL_S
    div     bx
    mov     ch, al      ; 柱面号

    xor     dx, dx
    mov     ax, di
    mov     bx, NUM_OF_S
    div     bx
    inc     dl
    mov     cl, dl      ; 扇区

    xor     dx, dx
    mov     bx, NUM_OF_H
    div     bx
    mov     dh, dl      ; 磁道
    
    pop     bx
    mov     dl, 0x80
    mov     ax, si
    mov     ah, 2
    int     13h
    pop     dx

    ret


    fn_read_a_file:
    ;-------------------------------------------------------------------
    ; 注意，如果一个文件某部分的簇号过高(超过62357)，其可能不会被读取完全
    ; di=文件首簇号
    ; es:bx=缓冲区地址
    ; 返回：
    ; ax=共读取的扇区数目
    ; es:bx=缓冲区下一个空的512字节的开头
    ;-------------------------------------------------------------------
    push    bp
    mov     bp ,sp

    sub     sp, 10
    mov     ax, 0
    mov     word [bp-8], ax         ; 记录读取的扇区数
    mov     word [bp-10], bx        ; bx
    ;mov     word [bp-10], ax        ; 记录上次读取的FAT表所在扇区号, 有富裕空间再写

.read_next_sect:
    mov     word [bp-2], di         ; 文件首簇号
    mov     ax, di
    mov     di, FAT_PER_SECT
    xor     dx, dx
    div     di
    mov     word [bp-4], dx         ; offset
    add     ax, START_OF_FAT1_SECT
    ;mov     word [bp-10], ax
    mov     di, ax
    mov     si, 1                   ; 从FAT1中读1个扇区

    push    es
    push    bx

    mov     ax, FAT_BASE
    mov     es, ax
    mov     bx, FAT_OFFSET
    call    fn_read_a_sector
    mov     di, word [bp-4]
    shl     di, 2
    mov     ax, word [es:bx+di]     ; 下一部分簇号
    mov     dx, word [es:bx+di+2]
    mov     word [bp-6], ax

    pop     bx
    pop     es

    mov     di, word [bp-2]
    add     di, BIAS_OF_DATA_SECT
    mov     si, SECT_PER_CLUS
    mov     bx, word [bp-10]
    call    fn_read_a_sector
    inc     word [bp-8]
    add     word [bp-10],512

    cmp     dx, 0
    ja      .done
    mov     di, word [bp-6]
    call    .read_next_sect
    
.done:
    mov     ax, [bp-8]
    leave
    ret



fn_find_a_file_short_name:
    ;------------------------------------------
    ; di=待查找文件名地址
    ; si=目录项所占扇区数
    ; es:bx=目录项所在的内存地址(提前读入)
    ; 返回：
    ; ax=文件首簇号
    ; ax=0表示没找到
    ;-----------------------------------------

    push    bp
    mov     bp, sp
    sub     sp, 4

    push    dx
    shl     si, 9
    mov     word [bp-2], si  ; 最大偏移量
    mov     word [bp-4], di  ; 文件名地址

.next_file:
    mov     di, word [bp-4]
    mov     ax, 0xf
    cmp     ax, [es:bx + 0xb]
    je      .long

    xor     si, si
    mov     cx, 11
.cmp_next_letter:
    mov     dl, byte [es:bx + si]
    and     dl, 0b11011111
    mov     al, byte [di]
    and     al, 0b11011111
    cmp     dl, al
    jne     .not_equal

    inc     si
    inc     di
    loop    .cmp_next_letter

    mov     ax, word [es:bx + 0x1a]
    jmp     .find

.long:
.not_equal:
    cmp     bx, [bp-2]
    ja      .none
    add     bx, 32
    jmp     .next_file

.none:
    mov     ax, 0
.find:
    pop     dx
    leave
    ret

[section data]
[bits 16]
msg:
    db "build file system successfully, enter into loader, fuck yeah!!!", 0

ARDS_success:
    db "memory info read successfully.", 0

ARDS_fault:
    db "fail to read memory info!", 0

kernel_name:
    db "KERNEL  BIN"

load_kernel_failed:
    db "kernel.bin not found.",0

load_kernel_successful:
    db "kernel.bin load to 0x10000 successfully.",0

cur_pos:
    dw 0

[section tables]
; thank linux 0.11

gdt:
	dw	0,0,0,0		; dummy

    ; 代码段, offset=8
	dw	0x07FF		; 8Mb - limit=2047 (2048*4096=8Mb)
	dw	0x0000		; base address=0
	dw	0x9A00		; code read/exec
	dw	0x00C0		; granularity=4096, 386

    ; 数据段, offset=16
	dw	0x07FF		; 8Mb - limit=2047 (2048*4096=8Mb)
	dw	0x0000		; base address=0
	dw	0x9200		; data read/write
	dw	0x00C0		; granularity=4096, 386

idt_48:
	dw	0			; idt limit=0
	dd	0,0			; idt base=0L

gdt_48:
	dw	0x800		; gdt limit=2048, 256 GDT entries
	dd	gdt         ; nasm如此可直接获得全局偏移

[section tables_64]
gdt_64:
    dq 0
    
    ; 代码段, offset=8
    dw 0x0000
    dw 0x0000
    dw 0x9800
    dw 0x0020

    ; 数据段, offset=16
    dw 0x0000
    dw 0x0000
    dw 0x9200
    dw 0x0000

gdt_64_ptr:
    dw $-gdt_64-1
    dd gdt_64
