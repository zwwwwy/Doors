org 0x7c00
bits 16

%define ORG_ADDR            0x7c00
%define LOADER_BASE         0x1000
%define LOADER_BIAS         0x0000

%define NUM_OF_ROOT_DIR_SECTS      14  ; 根目录占的扇区数: 根目录项数*32/扇区大小，向上取整
%define START_OF_ROOT_DIR_SECT     19  ; 根目录起始扇区号: 1+9+9=19
%define START_OF_FAT1_SECT          1  ; FAT1起始扇区号
%define START_OF_DATA_SECT         33  ; 

%define NAME_LEN                   11  ; 文件名长度

%define SECT_BASE           0x8000  ; 检索文件的时候把根目录项的一个扇区暂存在这里
%define SECT_OFFSET         0       ; offset of up

; FAT12引导扇区
    jmp             start_boot
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


start_boot:         ; offset=0x3e
    ; 设置段寄存器
    ; qemu中cs=0,ip=0x7c00,教材和这个反着的,垃圾教材,怪不得这么奇怪...
    mov     ax, cs  ; ax = 0
    mov     ds, ax
    mov     es, ax
    mov     ss, ax
    mov     sp, ORG_ADDR  ; 在boot阶段sp没啥用吧

    ; 清屏
    mov     ax, 0x0600  ; AH=6->向上，AL=0->清屏
    mov     bx, 0x0700  ; BH: 0000 0111 BL: 0000 0000
    mov     cx, 0       ; 左上角坐标
    mov     dx, 0x184f  ; 右下角坐标
    int     10h

    ; 光标初始化
    ;mov     ax, 0x0200  ; AH=2
    ;mov     bx, 0       ; BH=0->页码
    ;mov     dx, 0       ; DH=y, DL=x
    ;int     10h
    
    ; 打印

    ;mov     si, msg1
    ;mov     dx, 0x0000
    ;call fn_print_string


    ; 软驱复位
    ;mov     ax, 0
    ;mov     dx, 0
    ;int     13h
    ;mov     ax, NUM_OF_ROOT_DIR_SECTS

    ; 查找loader.bin

    mov     si, file_name
    call    fn_find_file_in_root

    cmp     ax, 0
    je      loader_not_found

    mov     si, success
    mov     dx, 0x0000
    call    fn_print_string

    ; 解析loader的根目录项(首字节位于[es:bx]),并加载到0x1000
    mov     di, word [es:bx+0x1a] ; 文件首簇号
    mov     ax, SECT_BASE
    mov     es, ax
    mov     bx, SECT_OFFSET
    call    fn_load_file

    jmp     0x1000:0x0000

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
    ; di=起始扇区号(逻辑)
    ; si=读取的扇区数(必须小于255!!!)
    ; es:bx=缓冲区地址
    push    bp
    mov     bp, sp

    sub     sp, 6
    ;mov     word [bp-2], cx
    mov     word [bp-4], dx
    xor     dx, dx
    mov     ax, di
    mov     cx, word [BPB_SecPerTrk]   ; 每磁道扇区数

    div     cx
    mov     word [bp-6], ax     ; 商
    inc     dl                  ; 转换后的起始扇区号

    mov     cl, dl              ; 扇区号
    shr     ax, 1               ; 每磁道俩磁头
    mov     ch, al              ; 柱面号
    mov     ax, word [bp-6]
    and     ax, 1
    mov     dh, al              ; 磁头号
    mov     dl, byte [BS_DrvNum]; 驱动器号
    mov     ax, si              ; 低八位是扇区数
.reading:
    mov     ah, 02h             ; 功能号
    int     13h
    jc      .reading

    ;mov     cx, word [bp-2]
    mov     dx, word [bp-4]
    leave
    ret


fn_find_file_in_root:
    ; 文件名地址传入si，长度十一个字节
    ; 在根目录区找到一个文件的32字节目录表项
    ; ------------------------------------------------
    ; 返回值：
    ; ax=0 -> 未找到
    ; ax=1 -> 找到了一个文件
    ; es:bx -> 指向根目录扇区中第一个匹配项的首个字节
    ; ------------------------------------------------

    ;push    cx
    ;push    si
    ;push    di
    ;push    bx
    push    bp
    mov     bp, sp
    sub     sp ,2

    ; 设置缓冲区地址
    mov     ax, SECT_BASE
    mov     es, ax
    mov     bx, SECT_OFFSET

    mov     cx, NUM_OF_ROOT_DIR_SECTS  ; 14个根目录扇区，循环十四次
    mov     di, START_OF_ROOT_DIR_SECT ; 根目录区起始扇区号
    mov     si, 1
.read_a_new_sector:
    call    fn_read_a_sector
    ; 将扇区读到缓冲区，从缓冲区取前11个字节，转换为大写后与file_name比较
    ; [bp-2]应该算是cpp里面函数中的static变量
    push    cx
    mov     cx, 16              ; 512/32=16个目录项
    mov     ax, 32
    mov     word [bp-2], ax     ; 用作bx自增
    mov     bx, 0

.read_a_new_entry:          ; 读取当前扇区中的下一个目录项
    call    fn_check_name
    cmp     ax, 1
    je      .find
    add     bx, word [bp-2]
    loop    .read_a_new_entry

    ; 当前扇区搜索完毕，恢复现场，准备读取下一个扇区
    inc     di
    xor     bx, bx              ; 处理单个扇区的时候更改了bx，需要初始化
    pop     cx                  ; cx同理
    loop    .read_a_new_sector
    xor     ax, ax

.find:
    sub     bx, NAME_LEN        ; 搜索完了以后bx指向文件名后面的地址，减去以后就是目录项地址
    leave
    ret

fn_check_name:
    ; 名字长度11个字节，从缓冲区(es:bx)取11个字节转化位大写后与file_name比较
    ; es和bx都需要作为参数传入，在fn_read_a_sector中处理bx的变化
    ; 完全一致返回1，否则返回0

    push    bp
    mov     bp, sp
    
    sub     sp, 3
    mov     word [bp-2], cx ; 用作偏移量
    mov     cx, NAME_LEN

.read_byte_in_buffer:
    push    bx
    mov     bx, 11
    sub     bx, cx
    mov     al, byte [file_name+bx]
    and     al, 0xdf
    mov     byte [bp-3], al
    pop     bx

    mov     al, byte [es:bx]
    and     al, 0xdf
    cmp     al, byte [bp-3]
    jne     .not_equal
    inc     bx
    loop    .read_byte_in_buffer
    mov     ax, 1
    jmp     .return

.not_equal:
    mov     ax, 0

.return:
    mov     cx, word [bp-2]
    leave
    ret

fn_load_file:
    ; 首簇号传入di(最大为2848,0xb20)
    ; 文件被写入地址传入es:bx
    ; 改变了ci，dx，但是没存

    push    bp
    mov     bp, sp

    sub     sp, 10
    mov     word [bp-2], dx
    mov     ax, 0xffff              ; 为了保证首次执行函数不会跳过读盘阶段
    mov     [bp-8], ax
    mov     ax, LOADER_BIAS
    mov     [bp-10], ax

    ; 这个FAT12比较逆天，需要一次读3个扇区才能凑齐整数个表项，估计要做一堆乘除法, 烦
.read_sect:
    push    di
    push    es
    push    bx
    add     di, 31                  ; 前两个簇不用(33-2)
    mov     ax, LOADER_BASE
    mov     es, ax
    mov     bx, [bp-10]
    mov     si ,1
    call    fn_read_a_sector
    add     bx, 512
    mov     [bp-10], bx
    pop     bx
    pop     es
    pop     di

    xor     dx, dx
    mov     ax, 1024
    mov     word [bp-4], ax
    mov     ax, di
    div     word [bp-4]             ; 商最大就是2,用8位乘法就好
    mov     word [bp-6], dx         ; 读取扇区中的相对簇号
    mov     word [bp-8], ax         ; 记录一下读扇区的情况，如果重复可以跳过读盘阶段

    mov     dl, 3                   ; 一次读3个扇区
    mul     dl
    add     ax, START_OF_FAT1_SECT  ; ax就是要读取的三个扇区的逻辑扇区号

    mov     di, ax
    mov     si, 3
    call    fn_read_a_sector

    ; 读盘完成，计算在读取扇区中的字节偏移量
.finish_reading_sect:
    push    bx

    ; 下面这一段找出fat项的位置
    ; 如果dx=0，di应该右移4位
    ; 如果dx=1，di应该和0x0fff作与运算
    ; 12这个位宽真他妈的反人类, 真他妈无法想象以前那些人都是怎么过来的...
    ; 读取这个垃圾FAT12表项跟他妈服刑一样
    xor     dx, dx
    mov     ax, [bp-6]
    mov     di, 2
    div     di
    mov     cl,3
    mul     cl
    add     bx, ax
    add     bx, dx
    mov     di, [es:bx]
    pop     bx

    cmp     dx, 1
    je      .shr
    and     di, 0x0fff
    jmp     .done

.shr:
    shr     di, 4

.done:
    cmp     di, 0xff0
    ja      .return
    jmp     .read_sect

.return:
    mov     dx, word [bp-2]
    leave
    ret

msg1:
    ;db "booting", 0

;error:
;    db "loader not found!!!", 0

success:
    db "booting...",0

file_name:
    db "loader  bin"

    times   510 - ($-$$) db 0
    dw      0xaa55
