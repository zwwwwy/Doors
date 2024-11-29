.section .text
    .global _start
_start:

	mov	    $0x10, %ax
	mov	    %ax, %ds
	mov	    %ax, %es
	mov	    %ax, %fs
	mov	    %ax, %ss
	mov	    $0x7e00, %esp

    lgdt    gdt_ptr(%rip)
    lidt    idt_ptr(%rip)

	mov	    $0x10, %ax
	mov	    %ax, %ds
	mov	    %ax, %es
	mov	    %ax, %fs
	mov	    %ax, %gs
	mov	    %ax, %ss
	movq	$0x7e00,	%rsp

    movq    $0x101000, %rax
    movq    %rax, %cr3

    pushq   $0x08          # 压入段选择子
    leaq    entry64(%rip), %rax
    pushq   %rax           # 压入返回地址
    iretq


entry64:
    movq    $0x10, %rax
    movq    %rax, %ds
    movq    %rax, %es
    movq    %rax, %gs
    movq    %rax, %ss
    movq    $0xffff800000007e00, %rsp

    # 初始化idt表的所有向量为int_not_define
    xorq    %rax, %rax                  # 中断门描述符
    leaq    int_not_define(%rip), %rdx       # int_not_define地址
    movl    $0x80000, %eax              # 段选择子
    movw    %dx, %ax                    # 段内偏移低16位
    andq    $0xffffffffffff0000, %rdx
    movw    $0x8e00, %dx
    movq    %rdx, %rcx
    shl     $32, %rcx
    xorq    %rcx, %rax
    shr     $32, %rdx    

    movq    $256, %rcx
    leaq    idt_64(%rip), %rdi 
write_idt:
    movq    %rax, (%rdi)
    movq    %rdx, 8(%rdi)
    addq    $16, %rdi
    loop    write_idt


    # 田宇代码，暂用
    #-------------------------------------------
setup_TSS64:
	leaq	tss_64(%rip),	%rdx
	xorq	%rax,	%rax
	xorq	%rcx,	%rcx
	movq	$0x89,	%rax
	shlq	$40,	%rax
	movl	%edx,	%ecx
	shrl	$24,	%ecx
	shlq	$56,	%rcx
	addq	%rcx,	%rax
	xorq	%rcx,	%rcx
	movl	%edx,	%ecx
	andl	$0xffffff,	%ecx
	shlq	$16,	%rcx
	addq	%rcx,	%rax
	addq	$103,	%rax
	leaq	gdt_64(%rip),	%rdi
	movq	%rax,	64(%rdi)
	shrq	$32,	%rdx
	movq	%rdx,	72(%rdi)

    leaq    setup_kernel(%rip), %rax
	pushq	$0x08
	pushq	%rax
	lretq
    #-------------------------------------------

    # 进入内核主程序
    leaq    setup_kernel(%rip), %rax
    pushq   $0x08
    pushq   %rax
    iretq


# 页表
.align  8

# 下面的org应是以0x100000为基址
.org    0x1000
__PML4E:
    .quad   0x102007
    .fill   255,8,0
    .quad   0x102007 # 把0xffff800000000000也映射到0
    .fill   255,8,0

.org    0x2000
__PDPTE:
    .quad   0x103003
    .fill   511,8,0

.org    0x3000
__PDE:
    .quad   0x000083
    .quad   0x200083
    .quad   0x400083
    .quad   0x600083
    .quad   0x800083
    .quad   0xa00083
    .quad   0xc00083
    .quad   0xe00083
    .quad   0xfd000083
    .quad   0xfd200083
    .quad   0xfd400083
    .quad   0xfd600083
    .quad   0xfd800083
    .quad   0xfda00083
    .quad   0xfdc00083
    .quad   0xfde00083
    .fill   496,8,0

.section .data

.global gdt_64
gdt_64:
    .quad   0

    # 64位内核代码段，offset=0x8
    .word   0x0000  # 段限长
    .word   0x0000  # 前15位段基址
    .word   0x9800  # 第2个数(8)是type，非一致性，不可读，未访问,9是p+dpl+s; dpl是第一个数中间的两位
    .word   0x0020  # 0000 0000 0010 0000

    # 64位内核数据段，offset=0x10
    .word   0x0000
    .word   0x0000
    .word   0x9200  # 向上扩展，可读写，未访问
    .word   0x0000

    # 64位用户代码段，offset=0x18。(dpl=3)
    .word   0x0000
    .word   0x0000
    .word   0xf800
    .word   0x0020

    # 64位用户数据段，offset=0x20。(dpl=3)
    .word   0x0000
    .word   0x0000
    .word   0xf200
    .word   0x0000

    # 32位内核代码段，offset=0x28
    .word   0xffff
    .word   0x0000
    .word   0x9a00  # a.type:保留
    .word   0x00cf

    # 32位内核数据段，offset=0x30
    .word   0xffff
    .word   0x0000
    .word   0x9200
    .word   0x00cf

    # TSS段，offset=0x38。十个八字节的0
    .fill   10,8,0
gdt_64_end:

gdt_ptr:
    .word   gdt_64_end-gdt_64-1
    .quad   gdt_64 

    #idt-------------------------------------
.global idt_64
idt_64:
    .fill   512,8,0  # 256,16,0实际的作用是256,8,0...
idt_64_end:
    
idt_ptr:
    .word   idt_64_end-idt_64-1
    .quad   idt_64

    #tss------------------------------------
.global tss_64
tss_64:
    .fill   13,8,0
tss_64_end:

tss_ptr:
    .word   tss_64_end-tss_64-1
    .quad   tss_64
