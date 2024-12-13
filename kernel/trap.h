#ifndef C_TRAP_H
#define C_TRAP_H

typedef struct {
	unsigned char x[16];
} idt_element;
extern idt_element idt_64[];

/*
 * n:		  中断号
 * ist:		  ist区的值，为0表示采用保护模式的段切换方式
 * func_addr: 中断处理函数地址
 */
#define SET_INT_GATE_DPL_0(n, ist, func_addr) set_gate(idt_64 + n, ist, 0x8e, func_addr)

/*
 * idt_element:	要设定的门的地址
 * ist: 		ist区域的值
 * attr: 		八位，p/dpl/0/type
 * func_addr: 	中断处理函数地址
 */
void set_gate(idt_element* idt_element, unsigned char ist, unsigned short attr, void* func_addr);

void __stack_chk_fail(void);
void blue_screen();

void divide_fault_handler();
void debug_fault_handler();
void nmi_int_handler();
void breakPoint_trap_handler();
void overFlow_trap_handler();
void boundsCheck_fault_handler();
void invalidOpcode_fault_handler();
void deviceNotAvailable_fault_handler();
void doubleFault_abort_handler();
void coprocessorSegmentOverrun_fault_handler();
void invalidTSS_fault_handler();
void segmentNotPresent_fault_handler();
void stackSegmentFault_fault_handler();
void generalProtectionFault_fault_handler();
void pageFault_fault_handler();
// 15Intel保留
void floatPointError_fault_handler();
void alignmentCheck_fault_handler();
void machineCheck_abort_handler();
void SIMDFloatException_fault_handler();
// 20-31Intel保留
#endif
