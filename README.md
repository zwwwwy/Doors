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