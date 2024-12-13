#ifndef C_STRING_H
#define C_STRING_H

/*
 * 用于将dest以上的一片内存区域初始化为某个值
 * dest:  指向要填充的对象的指针
 * ch:    填充字节
 * count: 要填充的字节数
 */
void* memset(void* dest, int ch, unsigned long count);

#endif
