void* memset(void* dest, int ch, unsigned long count)
{

	unsigned long ch_q	  = ch * 0x0101010101010101;
	unsigned long count_q = count / 8;
	count %= 8;

	// 尽可能用四字填充，剩下用字节填充
	__asm__ __volatile__("cld\n\t"
						 "rep 	stosq\n\t"
						 "cmp	$0,%1\n\t"
						 "jz	done\n\t"

						 "xorq	%%rax, %%rax\n\t"
						 "movb	%0, %%al\n\t"
						 "movq 	%1, %%rcx\n\t"
						 "rep	stosb\n\t"

						 "done:\n\t" ::"r"((unsigned char)ch),
						 "r"(count), "a"(ch_q), "c"(count_q), "D"(dest)
						 : "memory");
	return dest;
}
