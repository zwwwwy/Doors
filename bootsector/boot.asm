[org 0x7c00]
[bits 16]

%define ORG_ADDR            0x7c00
%define LOADER_BASE         0x9100
%define LOADER_OFFSET       0x0000

%define START_OF_DATA_SECT    3180  ; 数据区起始扇区号
%define START_OF_FAT1_SECT      32  ; FAT1起始扇区号
%define START_OF_ROOT_CLUS       2  ; 根目录起始簇号
%define BIAS_OF_DATA_SECT     3178  ; 调整后的数据区起始扇区号(用来和簇号相加计算实际逻辑扇区号)

%define NAME_LEN                11  ; 文件名长度

%define SECT_BASE           0x8000  ; 检索文件的时候把根目录项的一个扇区暂存在这里
%define SECT_OFFSET              0  ; offset of up

%define BPB_SecPerTrk           63  ; 每磁道扇区数         
%define BS_DrvNum             0x80  ; 驱动器号

%define NUM_OF_H                16  ; 磁头数 
%define NUM_OF_S                63  ; 扇区数
%define H_MUL_S               1008  ; h·s

%define FAT_PER_SECT           128  ; 每扇区的FAT表项数
%define SECT_PER_CLUS            1  ; 每簇扇区数


    jmp        start_boot
    nop
%include "fat32.inc"
; 这个硬盘格式化的方法比较离谱，不过也没找到什么省事的办法
; 先用mkfs.fat -F 32 ./build/imgs/boot.img
; 从格式化好的磁盘文件中的读取引导扇区的信息部分，更改少量内容后写入fat32.inc
; 然后把引导扇区写入磁盘的第1和第6扇区
;
; 下面是fat32的某些信息
; 扇区数：0x31f40
; 每簇扇区数：1
; 保留扇区数：32
; FAT表数：2
; 逻辑每磁道扇区数: 32
; 逻辑磁头数: 8
; 每FAT表扇区数: 1574
; 备份引导扇区位置: 6
; 物理柱面/磁头/每磁头扇区: 203/16/63
;
;若目录项的第12个字节(offset=0x0b)为0x0f，则该目录项为长文件名目录项

start_boot:         ; offset=90
    ; 设置段寄存器
    ; qemu中cs=0,ip=0x7c00,教材和这个反着的,垃圾教材,怪不得这么奇怪...
    mov     ax, cs  ; ax = 0
    mov     ds, ax
    mov     es, ax
    mov     ss, ax
    mov     sp, ORG_ADDR

    ; 清屏
    mov     ax, 0x0600  ; AH=6->向上，AL=0->清屏
    mov     bx, 0x0700  ; BH: 0000 0111 BL: 0000 0000
    mov     cx, 0       ; 左上角坐标
    mov     dx, 0x184f  ; 右下角坐标
    int     10h

    ; 光标初始化
    mov     ax, 0x0200  ; AH=2
    mov     bx, 0       ; BH=0->页码
    mov     dx, 0       ; DH=y, DL=x
    int     10h

    mov     si, msg
    mov     dx, 0x0000
    call    fn_print_string
    
    ; 读取根目录项到0x80000
    mov     di, START_OF_ROOT_CLUS
    mov     ax, SECT_BASE
    mov     es, ax
    mov     bx, SECT_OFFSET
    call    fn_read_a_file

    ; 从0x80000处查找loader.bin的簇号
    mov     si, ax
    mov     di, file_name
    call    fn_find_a_file_short_name

    cmp     ax, 0
    jne     load_loader
    mov     si, error
    mov     dx, 0x0100
    call    fn_print_string
    jmp $

    ; 把loader.bin写入0x10000
load_loader:
    mov     di, ax
    mov     ax, LOADER_BASE
    mov     es, ax
    mov     bx, LOADER_OFFSET
    call    fn_read_a_file

    jmp     LOADER_BASE:LOADER_OFFSET


loader_not_found:
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

;fn_print_a_num:
;    ; 打印si中的单个数字, 起始坐标放在dx
;    push    bp
;    mov     bp, sp
;                    ; 又被占了,怪不得很多人只用di和si传参,写了这么多了懒得改了
;    push    dx      ; 果然不能图省事随便乱写...
;    xor     dx, dx
;    mov     ax, si  ; 被除数
;    mov     di, 10
;    div     di
;    mov     di, dx
;    pop     dx
;
;    ; 创建栈帧
;    sub     sp, 2
;    mov     word [bp-2], di     ; 余数
;
;    cmp     ax, 0
;    je      .done
;
;    mov     si, ax
;    call    fn_print_a_num
;
;.done:
;    mov     di, word [bp-2]
;    add     di, 0x30
;    ; 改变光标位置
;    mov     ah, 0x2
;    xor     bx, bx
;    int     10h
;
;    call    fn_print_char
;    inc     dx
;
;    ;mov     sp, bp
;    ;pop     bp
;    leave
;    ret

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

    mov     ax, SECT_BASE
    mov     es, ax
    mov     bx, SECT_OFFSET
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

msg:
    db "booting...",0

error:
    db "loader not found", 0


file_name:
    db "LOADER  BIN"

    times   510 - ($-$$) db 0
    dw      0xaa55
