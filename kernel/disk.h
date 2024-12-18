#ifndef C_DISK_H
#define C_DISK_H

#define DISK_BUSY 1 << 7
#define DISK_READY 1 << 3

/*
 * 使用lba28读取扇区
 * lba:     逻辑扇区号
 * num:     读取扇区数
 * buffer:  要写入的地址空间
 * hd_no:   0为主硬盘，1为从硬盘
 */
unsigned long read_sectors(unsigned int lba, unsigned int num, unsigned short* buffer, unsigned int hd_no);
#endif
