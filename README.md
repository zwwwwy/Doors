### 内存结构
前1MB附近内存空间功能如下:  
| 名称                             | linear address                        |
| -------------------------------- | ------------------------------------- |
| 中断向量表                       | `0x00000`                             |
| BIOS数据区                       | `0x00400`                             |
| 主内存区                         | `0x00500`                             |
| 空                               | `0x00500`->`0x07bff`                  |
| 引导程序                         | `0x07c00`                             |
| 引导扇区加载后的sp               | `0x07e00`(向下扩展，将引导扇区设为栈) |
| 空                               | `0x07e00`->`0x0ffff`                  |
| 保护模式下kernel暂存区           | `0x10000`->`0x6ffff` (注)             |
| 进入IA-32e时的临时页表           | `0x70000`->`0x72fff`                  |
| 预留                             | `0x73000`->`0x7fdff`                  |
| 实模式下FAT32表项暂存区(512字节) | `0x7fe00`->`0x7ffff`                  |
| 实模式下文件暂存区               | `0x80000`->`0x8ffff`                  |
| VBE信息(256字节)                 | `0x90000`->`0x900ff`                  |
| 显示模式信息(256字节)            | `0x90100`->`0x901ff`                  |
| 内存信息(256字节，暂未用完)      | `0x90200`->`0x902ff`                  |
| 预留                             | `0x90200`->`0x90fff`                  |
| loader程序                       | `0x91000`->`0x9ffff`                  |
| 显示缓冲区                       | `0xa0000`->`0xbffff`                  |
| ROM拓展空间                      | `0xc0000`->`0xdffff`                  |
| 扩展BIOS映射(影子内存)           | `0xe0000`->`0xeffff`                  |
| BIOS映射                         | `0xf0000`->`0xfffff`                  |
| 内核主程序                       | `0x100000`->`0x15ffff` (注)           |

注：在loader中，先把缓存的kernel移到1MB处，然后再设置临时页表，所以可以把暂存区的空间扩展至`0x10000`->`0x7fdff`，如果主程序空间不够可以临时在这里扩展一下，不过需要考虑临时页表的越界问题。loader中相关常量：`TMP_KERNEL_START`、`TMP_KERNEL_END`、`KERNEL_SIZE`。  
  
进入内核主程序后改变或新增的内存空间功能如下：  
| 名称                             | linear address(低地址表示)                 |
| -------------------------------- | ------------------------------------------ |
| RSP                              | `0x10000`                                  |
| IO缓冲区                         | `0x10000->0x11000`                         |
| 主内存区                         | `0x100000->0xffffff`                       |
| 显存区                           | `0x1000000->0x1ffffff`(约能装两个1920*1080)|

注：`0xffff8`开头的线性地址与不带前缀的0起始的线性地址被映射到一起。  

### 硬盘结构
| 属性             | 值                      |
|------------------|-------------------------|
| 文件结构         | FAT32                   |
| 柱面数           | 203                     |
| 磁头数           | 16                      |
| 每磁道扇区数     | 63                      |
| 每扇区字节数     | 512                     |
| 总扇区数         | 203 * 16 * 63 = 203904  |
| 每簇扇区数       | 1                       |
| FAT表长          | 1574                    |
| FAT表数          | 2                       |

| 描述                | 偏移 | 扇区范围          |
|---------------------|------|-------------------|
| 引导扇区            | 0    | 0                 |
| 引导扇区备份        | 6    | 6                 |
| 保留扇区            | NULL | 0 - 31            |
| FAT表1              | 32   | 32 - 1605         |
| FAT表2              | 1606 | 1606 - 3179       |
| 数据区              | 3180 | 3180 - end        |

FAT32中各个目录和文件等价，根目录的起始簇为2，从3180扇区开始  
下面是从逻辑扇区计算出c、h、s的公式:  
柱面号 = 逻辑扇区号 // (每柱面磁头数 * 每磁头扇区数)  
磁头号 = (逻辑扇区号 // 每磁头扇区数) % 每柱面磁头数  
扇区号 = (逻辑扇区号 % 每磁头扇区数) + 1  

