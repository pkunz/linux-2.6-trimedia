/*
 * FILE NAME : amd29lv008.c
 * 
 * Mapping for AMD am29lv008 1Mx8 flash bank on the Motorola Force PrPMC280
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
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/config.h>
#include <asm/io.h>
#include <platforms/ppmc280.h>

#define PPMC280_BOOT_FLASH_NAME         "PPMC280 AMD Bootbank Flash"

#define BOOTLOADER_OFFSET         0x0
#define BOOTLOADER_SIZE           0x100000
#define BOOTLOADER_DATA_SIZE      0x100000

static struct mtd_info *bootbank;

static struct map_info ppmc280_amd_bootbank_map = {
    .name         = PPMC280_BOOT_FLASH_NAME,
    .size         = PPMC280_BOOT_FLASH_SIZE_ACTUAL,
    .bankwidth     = 1,
};

static struct mtd_partition prpmc280_amd_bootbank_part[] = {
    {
        .name       = "PPMC280 bootblock",
        .offset     = BOOTLOADER_OFFSET,
        .size       = BOOTLOADER_SIZE,
    }
};


int __init init_prpmc280_amd_bootbank(void)
{
    unsigned long long bootbank_flash_base;

    bootbank_flash_base = PPMC280_BOOT_FLASH_BASE;

    ppmc280_amd_bootbank_map.phys = bootbank_flash_base;
    ppmc280_amd_bootbank_map.virt = ioremap(bootbank_flash_base, ppmc280_amd_bootbank_map.size);
 
    if (!ppmc280_amd_bootbank_map.virt) {
        printk("Failed to ioremap AMD Boot Bank Flash\n");
        return -EIO;
    }

    simple_map_init(&ppmc280_amd_bootbank_map);

    bootbank = do_map_probe("jedec_probe", &ppmc280_amd_bootbank_map);
    if (bootbank) {
        bootbank->owner = THIS_MODULE;
        add_mtd_partitions(bootbank, prpmc280_amd_bootbank_part,
                    ARRAY_SIZE(prpmc280_amd_bootbank_part));
    } else {
        printk("Map Probe Failed for AMD Boot Bank Flash\n");
        return -ENXIO;
    }

    return 0;
}

static void __exit cleanup_prpmc280_amd_bootbank(void)
{
    if (bootbank) {
        del_mtd_partitions(bootbank);
        map_destroy(bootbank);
    }

    if (ppmc280_amd_bootbank_map.virt) {
        iounmap((void *)ppmc280_amd_bootbank_map.virt);
        ppmc280_amd_bootbank_map.virt = 0;
    }
}

module_init(init_prpmc280_amd_bootbank);
module_exit(cleanup_prpmc280_amd_bootbank);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joseph Barnett <jbarnett@motorola.com>");
MODULE_DESCRIPTION("AMD Flash Module for the Motorola PrPMC280 on the SCxB3101");

