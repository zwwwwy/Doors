__attribute__((naked)) void setup_kernel() {
  __asm__ volatile("movq  $0xffff800000007e00, %rsp\n\t"
                   "movq  $0x0010, %rax\n\t"
                   "movq  %rax, %ss\n\t"
                   "jmp kernel_start\n\t");
}
void test(void);

void kernel_start(void) {
  test();
  while (1) {
  }
}

void test(void) { __asm__ volatile("movq $6, %%rax" ::); }
