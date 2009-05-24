/*
 * Flash mapping for BCM7xxx boards
 *
 * Copyright (C) 2001 Broadcom Corporation
 *
 * THT				10-23-2002
 * Steven J. Hill	09-25-2001
 * Mark Huang		09-15-2001
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/config.h>
#include <linux/init.h>


#ifdef CONFIG_BCM93730
#define WINDOW_ADDR 0x1e000000

#elif defined(CONFIG_MIPS_BRCM97XXX)
extern unsigned long getPhysFlashBase(void);

#define WINDOW_ADDR getPhysFlashBase()

#else
#error ("bcm9xxxx-flash.c: Unsupported architecture\n")
#endif

#ifdef CONFIG_MIPS_BCM7315_BBX
/* 
 * The 7315BBX only has a 4MB flash
 */
#define WINDOW_SIZE 0x00400000	/* 4 MB flash */

/* 
 * All other 97XXX boards.  May have 2 flash chips, but we only use 1.
 * and since they are of different sizes, no interleaving.
 */
#elif defined(CONFIG_MIPS_BCM7038) || defined(CONFIG_MIPS_BCM7401)
#define WINDOW_SIZE 0x02000000	/* 32MB flash */
#define BUSWIDTH 2
#else

#define WINDOW_SIZE 0x01000000	/* 16MB flash */
#define BUSWIDTH 2
#endif






static struct mtd_info *bcm9XXXX_mtd;

#if 0 //def CONFIG_MTD_COMPLEX_MAPPINGS

__u8 bcm9XXXX_map_read8(struct map_info *map, unsigned long ofs)
{
	return __raw_readb(map->map_priv_1 + ofs);
}

__u16 bcm9XXXX_map_read16(struct map_info *map, unsigned long ofs)
{
	return __raw_readw(map->map_priv_1 + ofs);
}

__u32 bcm9XXXX_map_read32(struct map_info *map, unsigned long ofs)
{
	return __raw_readl(map->map_priv_1 + ofs);
}

void bcm9XXXX_map_copy_from(struct map_info *map, void *to, unsigned long from, ssize_t len)
{
	/* tht on the 7038 this cause a databus error, fixed in B0 rev */
	memcpy_fromio(to, map->map_priv_1 + from, len);
}

void bcm9XXXX_map_write8(struct map_info *map, __u8 d, unsigned long adr)
{
	__raw_writeb(d, map->map_priv_1 + adr);
	mb();
}

void bcm9XXXX_map_write16(struct map_info *map, __u16 d, unsigned long adr)
{
	__raw_writew(d, map->map_priv_1 + adr);
	mb();
}

void bcm9XXXX_map_write32(struct map_info *map, __u32 d, unsigned long adr)
{
	__raw_writel(d, map->map_priv_1 + adr);
	mb();
}

void bcm9XXXX_map_copy_to(struct map_info *map, unsigned long to, const void *from, ssize_t len)
{
	memcpy_toio(map->map_priv_1 + to, from, len);
}

#endif

struct map_info bcm9XXXX_map
= {
	name: "Broadcom 9xxxx mapped flash",
	size: WINDOW_SIZE,
	bankwidth: BUSWIDTH,
#if 0  // CONFIG_MTD_COMPLEX_MAPPINGS
	read8: bcm9XXXX_map_read8,
	read16: bcm9XXXX_map_read16,
	read32: bcm9XXXX_map_read32,
	copy_from: bcm9XXXX_map_copy_from,
	write8: bcm9XXXX_map_write8,
	write16: bcm9XXXX_map_write16,
	write32: bcm9XXXX_map_write32,
	copy_to: bcm9XXXX_map_copy_to
#endif 
};


static struct mtd_partition bcm9XXXX_parts[] = {
#ifdef CONFIG_BCM93730   /* BCM937xx STBs with PMON bootloader */
	{ name: "kernel",	offset: 0,		size: 2560*1024 }, 
			/* This size is 20*128K blocks. Although PMON use 128K blocks and
			 * will erase enough blocks to flash the kernel according to its size, 
			  * we must start rootfs on 256K boundary for mtd to work correctly 
			  */
	{ name: "rootfs",	offset: 2560*1024, 	size: 13696*1024 },
	{ name: "config",	offset: 16256*1024,	size: 128*1024 },

#elif defined( CONFIG_MIPS_BCM3560_WO_MIPS_INIT_FIX )
/* THT PR16701: 8/16/05 Obsoleted as the  MIPS init codes have been fixed.
 * This workaround is required for all 3560 CFE bootloaders built before this date:
 * Build Date: Mon Aug 15 14:34:08 PDT 2005 (rpereira@stb-irva-01)
 */
/* Temporarily created a skip zone to circumvent bad addr in flash addr space */
	{ name: "rootfs",	offset: 0,		    size: 10*1024*1024 },
	{ name: "skip", 	offset: 0x00A00000, size: 2*1024*1024 },
	{ name: "cfe",	    offset: 0x00C00000, size: 512*1024 },
	{ name: "vmlinux",	offset: 0x00C80000, size: 3582*1024 },
	{ name: "config",	offset: 0x00FFF800,	size: 144 },
	{ name: "nvram",	offset: 0x00FFF890,	size: 1904 },


#elif defined( CONFIG_MIPS_BCM7115 )  
/* PR9931: On the 7115 CFE boot-address is hardwired at 0 offset. */

	{ name: "cfe",	offset: 0, 		       size: 512*1024 },
	{ name: "rootfs",	offset: 0x00080000,     size: 12*1024*1024 },
	{ name: "vmlinux",	offset: 0x00C80000, size: 3582*1024 },
	{ name: "config",	offset: 0x00FFF800,	size: 144 },
	{ name: "nvram",	offset: 0x00FFF890,	size: 1904 },


#elif defined( CONFIG_MIPS_BCM7315_BBX )
	{ name: "cfe",	    offset: 0, 			size: 512*1024 },
	{ name: "vmlinux",	offset: 0x00080000, size: 3582*1024 },
	{ name: "config",	offset: 0x003FF800,	size: 144 },
	{ name: "nvram",	offset: 0x003FF890,	size: 1904 },

#elif defined( CONFIG_MIPS_BCM7038 ) || defined( CONFIG_MIPS_BCM7401 )
/* 32MB flash */
	{ name: "rootfs",	offset: 0,		    size: 28*1024*1024 },
	{ name: "cfe",	        offset: 0x01C00000, size: 512*1024 },
	{ name: "vmlinux",	offset: 0x01C80000, size: 3582*1024 },
	{ name: "config",	offset: 0x01FFF800,	size: 144 },
	{ name: "nvram",	offset: 0x01FFF890,	size: 1904 },




#elif defined(CONFIG_DP522) || defined(CONFIG_DP322)
        { name: "E*FlashFS",    offset: 0,              size:  2*1024*128},
        { name: "Application",  offset: 2*1024*128,     size: 29*1024*128},
        { name: "ATE",          offset: 31*1024*128,    size:  1*1024*128},
        { name: "BootLoader",   offset: 32*1024*128,    size:  1*1024*128},
        { name: "Kernel",       offset: 33*1024*128,    size:  7*1024*128},
        { name: "RootFS",       offset: 40*1024*128,    size: 10*1024*128},
        { name: "OpenTV",       offset: 50*1024*128,    size: 12*1024*128},
        { name: "BootRecovery", offset: 62*1024*128,    size:  2*1024*128}


#elif defined( CONFIG_MIPS_BRCM97XXX )
	{ name: "rootfs",	offset: 0,		    size: 12*1024*1024 },
	{ name: "cfe",	        offset: 0x00C00000, size: 512*1024 },
	{ name: "vmlinux",	offset: 0x00C80000, size: 3582*1024 },
	{ name: "config",	offset: 0x00FFF800,	size: 144 },
	{ name: "nvram",	offset: 0x00FFF890,	size: 1904 },

#else
#error ("bcm7xxx-flash.c: Unknown/Unsupported platform\n")
#endif
};


int __init init_bcm9XXXX_map(void)
{
	printk(KERN_NOTICE "BCM97XXX flash device: %x at %lx\n", WINDOW_SIZE, WINDOW_ADDR);
	bcm9XXXX_map.virt = (unsigned long)ioremap(WINDOW_ADDR, WINDOW_SIZE);

	if (!bcm9XXXX_map.virt) {
		printk("Failed to ioremap\n");
		return -EIO;
	}
	bcm9XXXX_mtd = do_map_probe("cfi_probe", &bcm9XXXX_map);
	if (!bcm9XXXX_mtd) {
		iounmap((void *)bcm9XXXX_map.virt);
		return -ENXIO;
	}
		
	add_mtd_partitions(bcm9XXXX_mtd, bcm9XXXX_parts, sizeof(bcm9XXXX_parts)/sizeof(bcm9XXXX_parts[0]));
	bcm9XXXX_mtd->owner = THIS_MODULE;
	return 0;
	

	
}

void __exit cleanup_bcm9XXXX_map(void)
{
	if (bcm9XXXX_mtd) {
		del_mtd_partitions(bcm9XXXX_mtd);
		map_destroy(bcm9XXXX_mtd);
	}
	if (bcm9XXXX_map.virt) {
		iounmap((void *)bcm9XXXX_map.virt);
		bcm9XXXX_map.virt = 0;
	}
}

module_init(init_bcm9XXXX_map);
module_exit(cleanup_bcm9XXXX_map);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Steven J. Hill <shill@broadcom.com>");
MODULE_DESCRIPTION("Broadcom 7xxx MTD map driver");
