__attribute__((naked)) void setup_kernel() {
  __asm__ volatile("movq  $0xffff800000007e00, %rsp\n\t"
                   "movq  $0x0010, %rax\n\t"
                   "movq  %rax, %ss\n\t"
                   "jmp kernel_start\n\t");
}

void kernel_start(void) {
  int *diplay_ptr = (int *)0xa00000;
  int i;

  for (i = 0; i < 1440 * 90; i++) {
    *diplay_ptr = (int)0x00ff0000;
    diplay_ptr += 1;
  }
  /*for (i = 0; i < 1440 * 20; i++) {*/
  /*  *diplay_ptr = (int)0x0000ff00;*/
  /*  diplay_ptr += 1;*/
  /*}*/
  while (1) {
  };
}
