/*
 * $Id: mc-2.6.8.1.patch,v 1.2 2005/02/08 17:15:49 hpham Exp $
 * 
 * Mapping for Ebony user flash
 *
 * Matt Porter <mporter@mvista.com>
 *
 * Copyright 2002 MontaVista Software Inc.
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
#include <platforms/85xx/gda8500.h>

static struct map_info gda8500_map = {
	.name =		"Flash",
	.size =		0x00800000,
	.phys =     	0xFF800000,
	.bankwidth =	2,
	.set_vpp  = 0,
};

static struct mtd_partition gda8500_partitions[] = {
    {
        name:   "jffs2",
        offset: 0x00000000,
        size:   0x00680000,
    },
    {
        name:   "kernel",
        offset: 0x00680000,
        size:   0x00100000,
    },
    {
        name:   "uboot",
        offset: 0x00780000,
        size:   0x00080000,
    }
};

static struct mtd_info *mymtd;

int __init init_gda8500(void)
{
    gda8500_map.virt = (unsigned long)ioremap(gda8500_map.phys, 
                                              gda8500_map.size);
    if (!gda8500_map.virt) {
        printk("Failed to ioremap\n");
        return -EIO;
    }

    simple_map_init(&gda8500_map);

    mymtd = NULL;
    mymtd = do_map_probe("cfi_probe", &gda8500_map);
    if (mymtd) {
        mymtd->owner = THIS_MODULE;

        add_mtd_partitions(mymtd, gda8500_partitions, 
                           ARRAY_SIZE(gda8500_partitions));

        return 0;
    }

    iounmap((void *)gda8500_map.virt);
    return -ENXIO;
}

static void __exit cleanup_gda8500(void)
{
    if (mymtd) {
        del_mtd_partitions(mymtd);
        map_destroy(mymtd);
    }

    if (gda8500_map.virt) {
        iounmap((void *)gda8500_map.virt);
    }
}

module_init(init_gda8500);
module_exit(cleanup_gda8500);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Christopher P. Hayden <chayden@mc.com>");
MODULE_DESCRIPTION("MTD map and partitions for GDA RMC8500 MPC8540 boards");
