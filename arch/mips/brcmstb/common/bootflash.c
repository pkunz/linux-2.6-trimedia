/*
 * Flash Base address for BCM97xxx boards
 *
 * Copyright (C) 2002,2003,2004,2005 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * THT 10/22/2002
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/config.h>

/*
 * The way the file bcm97xxx.h are defined, it would require a compile
 * time switch in order to determine whether we boot from flash or ROM.
 * This simple QRY test can test for us, as we query the CFE boot address
 * to see if it responds to the CFI 'QRY' test.
 */
 
#ifndef CODE_IN_FLASH
#define CODE_IN_FLASH
#endif

#ifdef CONFIG_MIPS_BCM3560
#include <asm/brcmstb/common/brcmstb.h>
#define WINDOW_ADDR 0x1f000000

#elif CONFIG_MIPS_BCM7319
#include <asm/brcmstb/brcm97319/bcm97319.h>
#define WINDOW_ADDR 0x1f000000

#elif defined( CONFIG_MIPS_BCM7328 )
#include <asm/brcmstb/brcm97328/bcm97328.h>
#define WINDOW_ADDR 0x1f000000

#elif defined(CONFIG_MIPS_BCM7320)
#include <asm/brcmstb/brcm97320/bcm97320.h>
/* Assume ROM daughter card present, 
 * will play with CS0 when it is not (boot from flash) 
 */
#define WINDOW_ADDR PHYS_FLASH_CS0FLASH_BASE	/*0x1e800000*/

#elif defined(CONFIG_MIPS_BCM7112) || defined(CONFIG_MIPS_BCM7115)
/*
 * The 7115 never boots from flash, but its brother, the 7112 does.
 * so this is really for the 7112
 */
#include <asm/brcmstb/brcm97115/bcm97115.h>
#define WINDOW_ADDR 0x1f000000			/*0x1a000000 for 7115*/

#elif defined(CONFIG_MIPS_BCM7110)
#include <asm/brcmstb/brcm97110/bcm97110.h>
#define WINDOW_ADDR PHYS_FLASH_BASE			/*0x1f000000*/

#elif defined(CONFIG_MIPS_BCM7111)
#include <asm/brcmstb/brcm97111/bcm97111.h>
#define WINDOW_ADDR PHYS_FLASH_BASE			/*0x1f000000*/

#elif defined(CONFIG_MIPS_BCM7312)
#include <asm/brcmstb/brcm97312/bcm97312.h>
#define WINDOW_ADDR PHYS_CS0_DEV_BASE			/*0x1f000000*/

#elif defined(CONFIG_MIPS_BCM7315_BBX) 
#include <asm/brcmstb/brcm97315bbx/bcm97315bbx.h>
#define WINDOW_ADDR PHYS_FLASH_BASE			/* 0x1FC00000 */

#elif defined(CONFIG_MIPS_BCM7314) 
#include <asm/brcmstb/brcm97314/bcm97314.h>
#define WINDOW_ADDR PHYS_FLASH1_BASE

#elif defined(CONFIG_MIPS_BCM7315) 
#include <asm/brcmstb/brcm97315/bcm97315.h>
#define WINDOW_ADDR PHYS_FLASH1_BASE

#elif defined(CONFIG_MIPS_BCM7317) 
#include <asm/brcmstb/brcm97317/bcm97317.h>
#define WINDOW_ADDR PHYS_FLASH1_BASE

#elif defined(CONFIG_MIPS_BCM7318)
#include <asm/brcmstb/brcm97318/bcm97318.h>
#define WINDOW_ADDR PHYS_CS0_DEV_BASE			/*0x1f000000*/

#elif defined(CONFIG_MIPS_BCM7038)
/* Both A0 and B0 revs */
#include <asm/brcmstb/common/brcmstb.h>
#define WINDOW_ADDR 0x1e000000

#elif defined(CONFIG_MIPS_BCM7327)
#include <asm/brcmstb/common/brcmstb.h>
#define WINDOW_ADDR PHYS_CS0_DEV_BASE			/* 0x1f000000 */

#elif defined(CONFIG_MIPS_BCM7329)
#include <asm/brcmstb/common/brcmstb.h>
#define WINDOW_ADDR PHYS_CS0_DEV_BASE			/* 0x1f000000 */

#elif defined(CONFIG_MIPS_BCM7401)
#include <asm/brcmstb/common/brcmstb.h>
#define WINDOW_ADDR 0x1e000000

#endif

static const unsigned long FLASH_FLASH_BASE = WINDOW_ADDR;
extern const unsigned long ROM_FLASH_BASE;


static unsigned long RT_PHYS_FLASH_BASE = WINDOW_ADDR;

unsigned long 
getPhysFlashBase(void)
{
	return RT_PHYS_FLASH_BASE;
}
EXPORT_SYMBOL(getPhysFlashBase);

#define BOOT_LOADER_ENTRY 0xbfc00000



#if defined (CONFIG_MIPS_BRCM97XXX)
/*
 * Determine whether CFE was booted from Flash or ROM 
 */
void 
determineBootFromFlashOrRom(void)
{
	char msg[60];
	
	
	unsigned short   query[3];
	volatile unsigned short * queryaddr;

	/* Enter query mode */
	*(volatile unsigned char *)(BOOT_LOADER_ENTRY | (0x55 << 1)) = 0x98;

	queryaddr = (volatile unsigned short *)(BOOT_LOADER_ENTRY | (0x10 << 1));

	query[0] = *queryaddr++;
	query[1] = *queryaddr++;
	query[2] = *queryaddr;

	/* Go back to read-array mode */
	*(volatile unsigned short *)(BOOT_LOADER_ENTRY | (0x55 << 1)) = 0xFFFF;

#if (!defined( CONFIG_MIPS_BCM7110 ) || defined( CONFIG_MIPS_BCM7110_DSG))
	if( query[0] == 0x51 &&     /* Q */
	   	query[1] == 0x52 &&     /* R */
	   	query[2] == 0x59  )    /* Y */
#else
	/*
	 * The 7110 (Not the 7110-DSG) has 2 8bit flash chips that are interleaved.  Rather than using the CFI_probe routine which
	 * does this test taking interleave into account, for all instances and purposes, this should
	 * be enough
	 */
	if( query[0] == 0x5151 &&     /* Q */
	   	query[1] == 0x5252 &&     /* R */
	   	query[2] == 0x5959  )    /* Y */
#endif
	{	
		RT_PHYS_FLASH_BASE = FLASH_FLASH_BASE;
		sprintf(msg, "**********BOOTEDFROMFLASH, Base=%08lx\n", RT_PHYS_FLASH_BASE);
		uart_puts(msg);
	
	} else {

		
		RT_PHYS_FLASH_BASE = ROM_FLASH_BASE;
		sprintf(msg, "**********BOOTEDFROMROM, Base=%08lx\n", RT_PHYS_FLASH_BASE);
		uart_puts(msg);
	}

}


#endif /* if BCM97xxx boards */
