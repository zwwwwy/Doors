__attribute__((naked)) void setup_kernel() {
  __asm__ volatile("movq $0x7E00, %rsp\n" // 设置 rsp 寄存器
                   "jmp kernel_start\n"   // 跳转到 main 函数
  );
}
void test(void);

void kernel_start(void) {
  test();
  while (1) {
  }
}

void test(void) { __asm__ volatile("movq $6, %%rax" ::); }
