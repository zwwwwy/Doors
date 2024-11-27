#include "printk.h"
void __stack_chk_fail(void) {
	printk("stack fail.\n");
	while (1)
		;
}
