#ifndef C_PCB_H
#define C_PCB_H

#include "../lib/list.h"
#include "cpu.h"
#include "memory.h"

// 2^15, 32kB
#define STACK_SIZE 32768

#define KERNEL_CS (0x08)
#define KERNEL_DS (0x10)

#define USER_CS (0x28)
#define USER_DS (0x30)

#define CLONE_FS (1 << 0)
#define CLONE_FILES (1 << 1)
#define CLONE_SIGNAL	(1 << 2

#define PF_KTHREAD (1 << 0)
#define TASK_RUNNING (1 << 0)
#define TASK_INTERRUPTIBLE (1 << 1)
#define TASK_UNINTERRUPTIBLE (1 << 2)
#define TASK_ZOMBIE (1 << 3)
#define TASK_STOPPED (1 << 4)

typedef struct mm_struct
{
	pml4t_t* pgd; // 页表指针

	unsigned long start_code;
	unsigned long end_code;

	unsigned long start_data;
	unsigned long end_data;

	unsigned long start_rodata; // 只读数据段
	unsigned long end_rodata;

	unsigned long start_brk; // 堆
	unsigned long end_brk;

	unsigned long start_stack;
} mm_struct;

typedef struct task_struct
{
	struct task_list list;	// 双向链表，用于连接各个pcb
	volatile long	 state; // 运行态、停止态、可中断态
	unsigned long	 flags; // 进程标志：进程、线程、内核线程

	struct mm_struct*	  mm;		  // 内存空间分布结构体，记录内存页表和程序段信息
	struct thread_struct* thread;	  // 进程切换时保留的状态信息
	unsigned long		  addr_limit; // 进程地址空间范围

	long pid;
	long counter; // 进程可用时间片
	long signal;  // 进程持有的信号
	long priority;

} task_struct;

typedef struct thread_struct
{
	unsigned long rsp0; // 内核层栈基地址
	unsigned long rip;
	unsigned long rsp; // 内核层当前栈指针
	unsigned long fs;
	unsigned long gs;
	unsigned long cr2;
	unsigned long trap_nr;
	unsigned long error_code;
} thread_struct;

typedef union task_union
{
	struct task_struct task;

	unsigned long stack[STACK_SIZE / sizeof(unsigned long)];
} __attribute__((aligned(8))) task_union;

#define INIT_TASK(tsk)                                                                                                 \
	{.state		 = TASK_UNINTERRUPTIBLE,                                                                               \
	 .flags		 = PF_KTHREAD,                                                                                         \
	 .mm		 = &init_mm,                                                                                           \
	 .thread	 = &init_thread,                                                                                       \
	 .addr_limit = 0xffff800000000000,                                                                                 \
	 .pid		 = 0,                                                                                                  \
	 .counter	 = 1,                                                                                                  \
	 .signal	 = 0,                                                                                                  \
	 .priority	 = 0}

#endif
