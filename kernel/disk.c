#include "disk.h"
#include "../lib/io.h"
#include "printk.h"

unsigned long read_sectors(unsigned int lba, unsigned int num, unsigned short* buffer, unsigned int hd_no)
{
	unsigned char current_num;
	unsigned int  next_lba = lba;
	unsigned char mask8;
	unsigned char hd_status;
	unsigned int  read_times;
	do
	{
		lba = next_lba;
		if (num > 255)
		{
			current_num = 255;
			num -= 255;
			next_lba += 255;
		}
		else
		{
			current_num = num;
			num			= 0;
		}
		// do
		// {
		//     hd_status = io_in8(0x1f7);
		// } while ((hd_status & DISK_BUSY) || (!(hd_status & DISK_READY)));
		unsigned char mask = io_in8(0x1f7);
		printk("damn:%d\n", mask);
		// 把28位lba写入4个8位端口
		io_out8(0x1f2, current_num);
		mask8 = ((unsigned char)-1) & lba;
		io_out8(0x1f3, mask8);
		mask8 = ((unsigned char)-1) & (lba >> 8);
		io_out8(0x1f4, mask8);
		mask8 = ((unsigned char)-1) & (lba >> 16);
		io_out8(0x1f5, mask8);
		mask8 = (0b00001111) & (lba >> 24);
		mask8 = mask8 | (hd_no << 4) | (0b11100000);
		io_out8(0x1f6, mask8);
		read_times = current_num << 8; // *=256

		io_out8(0x1f7, 0x20); // 0x20表示读盘，0x30表示写盘
		for (int i = 0; i < read_times; ++i)
		{
			*(buffer++) = io_in16(0x1f0);
		}

	} while (num);
	return 1;
}
