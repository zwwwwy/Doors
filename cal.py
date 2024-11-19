def lba_to_chs(lba, hpc, spt):
    c = lba // (hpc * spt)
    h = (lba // spt) % hpc
    s = (lba % spt) + 1
    return c, h, s

# 示例参数
lba = int(input("逻辑扇区号"))  # 逻辑扇区号
hpc = 16     # 每个柱面的磁头数
spt = 63     # 每个磁头的扇区数

cylinder, head, sector = lba_to_chs(lba, hpc, spt)
print(f"柱面号: {cylinder}, 磁头号: {head}, 扇区号: {sector}")
