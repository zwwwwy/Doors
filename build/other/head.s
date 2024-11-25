# 0 "head.S"
# 0 "<built-in>"
# 0 "<命令行>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<命令行>" 2
# 1 "head.S"
.section .text
    .global _start
_start:

 mov $0x10, %ax
 mov %ax, %ds
 mov %ax, %es
 mov %ax, %fs
 mov %ax, %ss
 mov $0x7E00, %esp

    lgdt gdt_ptr(%rip)
    lidt idt_ptr(%rip)

 mov $0x10, %ax
 mov %ax, %ds
 mov %ax, %es
 mov %ax, %fs
 mov %ax, %gs
 mov %ax, %ss

 movq $0x7E00, %rsp

    movq $0x101000, %rax
    movq %rax, %cr3



    pushq $0x08 # 压入段选择子
    movq switch_seg(%rip), %rax
    pushq %rax # 压入返回地址
    iretq

    iretq # gas不支持长跳转和长调用

switch_seg:
    .quad entry64
entry64:
    movq $0x10, %rax
    movq %rax, %ds
    movq %rax, %es
    movq %rax, %gs
    movq %rax, %ss
    movq $0xffff800000007e00, %rsp

    movq go_to_kernel(%rip), %rax
    pushq $0x08
    pushq %rax
    iretq

go_to_kernel:
    .quad setup_kernel

# 页表
.align 8

# 下面的org应是以0x100000为基址
.org 0x1000
__PML4E:
    .quad 0x102007
    .fill 255,8,0
    .quad 0x102007 # 把0xffff800000000000也映射到0
    .fill 255,8,0

.org 0x2000
__PDPTE:
    .quad 0x103003
    .fill 511,8,0

.org 0x3000
__PDE:
    .quad 0x000083
    .quad 0x200083
    .quad 0x400083
    .quad 0x600083
    .quad 0x800083
    .quad 0xe0000083
    .quad 0xe0200083
    .quad 0xe0400083
    .quad 0xe0600083
    .quad 0xe0800083
    .quad 0xe0a00083
    .quad 0xe0c00083
    .quad 0xe0e00083
    .fill 499,8,0

.section .data

.global gdt_64
gdt_64:
    .quad 0

    # 64位内核代码段，offset=0x8
    .word 0x0000 # 段限长
    .word 0x0000 # 前15位段基址
    .word 0x9800 # 第2个数(8)是type，非一致性，不可读，未访问,9是p+dpl+s; dpl是第一个数中间的两位
    .word 0x0020 # 0000 0000 0010 0000

    # 64位内核数据段，offset=0x10
    .word 0x0000
    .word 0x0000
    .word 0x9200 # 向上扩展，可读写，未访问
    .word 0x0000

    # 64位用户代码段，offset=0x18。(dpl=3)
    .word 0x0000
    .word 0x0000
    .word 0xf800
    .word 0x0020

    # 64位用户数据段，offset=0x20。(dpl=3)
    .word 0x0000
    .word 0x0000
    .word 0xf200
    .word 0x0000

    # 32位内核代码段，offset=0x28
    .word 0xffff
    .word 0x0000
    .word 0x9a00 # a.type:保留
    .word 0x00cf

    # 32位内核数据段，offset=0x30
    .word 0xffff
    .word 0x0000
    .word 0x9200
    .word 0x00cf

    # TSS段，offset=0x38。十个八字节的0
    .fill 10,8,0
gdt_64_end:

gdt_ptr:
    .word gdt_64_end-gdt_64-1
    .quad gdt_64

    #idt-------------------------------------
.global idt_64
idt_64:
    .fill 512,8,0
idt_64_end:

idt_ptr:
    .word idt_64_end-idt_64-1
    .quad idt_64

    #tss------------------------------------
.global tss_64
tss_64:
    .fill 13,8,0
tss_64_end:

tss_ptr:
    .word tss_64_end-tss_64-1
    .quad tss_64
