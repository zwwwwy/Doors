#ifndef C_INIT_H
#define C_INIT_H
#include "info.h"
#include "pcb.h"
void init_display();
void init_buffer();
void init_trap();
void init_memory();
void init_irq();
void init_disk_controller();
void init_pcb();
void init_task_struct(task_struct* tsk);
#endif
