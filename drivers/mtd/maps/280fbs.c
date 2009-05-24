/* 
 * FILE NAME : 280fbs.c 
 *  
 * Mapping for Intel Strataflash on the Motorola Force PrPMC280 
 * 
 * Joseph Barnett <jbarnett@motorola.com> 
 * 
 * Copyright 2004 Motorola Inc. 
 * 
 * This program is free software; you can redistribute  it and/or modify it 
 * under  the terms of  the GNU General  Public License as published by the 
 * Free Software Foundation;  either version 2 of the  License, or (at your 
 * option) any later version. 
 */ 
 
#include <linux/config.h> 
#include <linux/module.h> 
#include <linux/types.h> 
#include <linux/kernel.h> 
#include <linux/init.h> 
#include <asm/io.h> 
 
#include <linux/mtd/mtd.h> 
#include <linux/mtd/map.h> 
#include <linux/mtd/concat.h> 
#include <linux/mtd/partitions.h> 
#include <platforms/ppmc280.h>
 
#define PPMC280_CS0_NAME    "PPMC280 User Flash Bank 0" 
#define PPMC280_CS1_NAME    "PPMC280 User Flash Bank 1" 
 
#define BANK_CONCATINATION_SIZE (PPMC280_EXT_FLASH_SIZE_ACTUAL * 2) 
 
#define FW_IMG_SIZE              0x00100000 
#define FW_IMG_CFG_DATA_SIZE     0x00040000 
 
#define FW_IMG_BANK_A_OFFSET     0x00000000 
#define FW_IMG_BANK_B_OFFSET    (BANK_CONCATINATION_SIZE - FW_IMG_SIZE) 
#define FW_IMG_CFG_DATA_OFFSET   FW_IMG_SIZE 
     
#define OS_KERN_IMG_OFFSET      (FW_IMG_CFG_DATA_OFFSET + FW_IMG_CFG_DATA_SIZE) 
#define OS_KERN_IMG_SIZE         0x00400000 
 
#define OS_FILESYS_OFFSET       (OS_KERN_IMG_OFFSET + OS_KERN_IMG_SIZE) 
#define OS_FILESYS_SIZE         (FW_IMG_BANK_B_OFFSET - OS_FILESYS_OFFSET) 
 
static struct map_info ppmc280_map[] = { 
     { 
         .name = PPMC280_CS0_NAME, 
         .size = PPMC280_EXT_FLASH_SIZE_ACTUAL, 
         .bankwidth = 4, 
         .phys = PPMC280_EXT_FLASH_BASE_1 
     }, 
     { 
         .name = PPMC280_CS1_NAME, 
         .size = PPMC280_EXT_FLASH_SIZE_ACTUAL, 
         .bankwidth = 4, 
         .phys = PPMC280_EXT_FLASH_BASE_2 
     }, 
}; 
 
#define NUM_FLASH_BANKS	(sizeof(ppmc280_map)/sizeof(struct map_info)) 
 
static struct mtd_partition ppmc280_part[] = { 
    { 
        .name       = "Firmware Image A", 
        .offset     = FW_IMG_BANK_A_OFFSET, 
        .size       = FW_IMG_SIZE, 
    }, 
    { 
        .name       = "Firmware Configuration Data", 
        .offset     = FW_IMG_CFG_DATA_OFFSET, 
        .size       = FW_IMG_CFG_DATA_SIZE, 
        .mask_flags = MTD_WRITEABLE,            /* force read-only */ 
    }, 
    { 
        .name       = "Embedded OS Kernel Image", 
        .offset     = OS_KERN_IMG_OFFSET, 
        .size       = OS_KERN_IMG_SIZE, 
    }, 
    { 
        .name       = "Embedded OS File System", 
        .offset     = OS_FILESYS_OFFSET,  
        .size       = OS_FILESYS_SIZE, 
    }, 
    { 
        .name       = "Firmware Image B", 
        .offset     = FW_IMG_BANK_B_OFFSET, 
        .size       = FW_IMG_SIZE, 
    } 
}; 
 
static struct mtd_info *flash[NUM_FLASH_BANKS]; 
static struct mtd_info *chunk; 
 
static int __init init_ppmc280(void) 
{ 
    int                dev_cnt = 0, 
                       i       = 0; 
     
    for (i = 0; i < NUM_FLASH_BANKS; i++) { 
        printk(KERN_NOTICE "PPMC280 flash device: 0x%lx at 0x%lx\n", ppmc280_map[i].size, ppmc280_map[i].phys); 
        ppmc280_map[i].virt = ioremap_nocache(ppmc280_map[i].phys, ppmc280_map[i].size); 

        if (!ppmc280_map[i].virt) { 
            printk("Failed to ioremap_nocache\n"); 
            return -EIO; 
        } 
 
        simple_map_init(&ppmc280_map[i]); 
 
        flash[i]     = do_map_probe("cfi_probe",   &ppmc280_map[i]); 
        if(!flash[i]) 
            flash[i] = do_map_probe("jedec_probe", &ppmc280_map[i]); 
        if(!flash[i]) 
            flash[i] = do_map_probe("map_rom",     &ppmc280_map[i]); 
 
        if (flash[i]) { 
            flash[i]->owner = THIS_MODULE;  
            ++dev_cnt;  
        } else { 
            iounmap((void *)ppmc280_map[i].virt); 
        } 
    } 
    if(dev_cnt >= 2) { 
        /* Combine the two flash banks into a single MTD device & register it: */ 
        if(NULL != (chunk = mtd_concat_create(flash, 2, "PPMC280 Flash Banks 0 and 1"))) { 
            add_mtd_device(chunk);
            add_mtd_partitions(chunk, ppmc280_part, ARRAY_SIZE(ppmc280_part)); 
        } 
    } 
    return(dev_cnt ? 0 : -ENXIO); 
} 
 
static void __exit cleanup_ppmc280(void) 
{ 
    int i; 
    if (chunk) { 
        del_mtd_device(chunk); 
        mtd_concat_destroy(chunk); 
    } 
 
    for (i = 0; i < NUM_FLASH_BANKS; i++) { 
        if (flash[i]) 
            map_destroy(flash[i]); 

        if (ppmc280_map[i].virt) { 
            iounmap((void *)ppmc280_map[i].virt); 
            ppmc280_map[i].virt = 0; 
        } 
    } 
} 
 
module_init(init_ppmc280); 
module_exit(cleanup_ppmc280); 
 
MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Joseph Barnett <jbarnett@motorola.com>"); 
MODULE_DESCRIPTION("Flash Support for the Motorola ATCA SCX310X with the PPMC280"); 
