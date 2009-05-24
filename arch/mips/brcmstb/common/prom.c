/*
 * arch/mips/brcm/prom.c
 *
 * Copyright (C) 2001 Broadcom Corporation
 *                    Steven J. Hill <shill@broadcom.com>
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
 * Set up kernel arguments and memory. Currently 32MB is reserved
 * for the 70XX and other Broadcom chip drivers.
 *
 * 11-29-2001   SJH    Created
 * 12-23-2005   RYH    Add bcmrac support
 */
#include <linux/ctype.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <asm/bootinfo.h>

/* RYH */
unsigned int par_val = 0x00;	/* for RAC Mode setting, 0x00-Disabled, 0xD4-I&D Enabled, 0x94-I Only */
unsigned int par_val2 = 0x00;	/* for RAC Cacheable Space setting */

//#define TEST_ROUNDROBIN_DISABLE

unsigned long get_RAM_size(void);

#ifdef CONFIG_MIPS_BCM7400
#define DEFAULT_KARGS " rw console=uart,mmio,0x10400b00,115200n8"

#else
#define DEFAULT_KARGS " rw console=ttyS0,115200"
#endif


static uint
bcm_atoi(const char *s)
{
	uint n;

	n = 0;

	while (isdigit(*s))
		n = (n * 10) + *s++ - '0';
	return (n);
}




#ifdef CONFIG_MIPS_BRCM97XXX
#include "asm/brcmstb/common/cfe_call.h"	/* CFE interface */

extern void uart_init(unsigned long uClock);
void uart_puts(const char *s);
extern void InitEbi (void);

/* RYH */
extern void set_debug_traps(void);
extern void breakpoint(void);
//cmdEntry_t rootEntry;

char cfeBootParms[CFE_CMDLINE_BUFLEN]; 
#endif
//char arcs_cmdline[COMMAND_LINE_SIZE];



/*
  * Munges cmdArg, and append the console= string if its not there
  */
static void
appendConsoleArg(char* cmdArg)
{
	char* pConsole = NULL;

	pConsole = strstr(cmdArg, "console=");
	if (!pConsole) {
		if ((strlen(cmdArg) + strlen(DEFAULT_KARGS)) <COMMAND_LINE_SIZE) {
		    strcat(cmdArg, DEFAULT_KARGS);
	    }
		else {
			uart_puts("***** WARNINGS: No console= appended to kernel args: args is too long");
		}
	}
	return;
}

static int
isRootSpecified(char* cmdArg)
{
	char* pRoot = NULL;

	if (NULL != cmdArg && cmdArg[0] != '\0') {
		pRoot = strstr(cmdArg, "root=");
	}
	return (NULL != pRoot);
}



void __init prom_init(void)
{

#ifdef CONFIG_MIPS_BRCM97XXX
	int hasCfeParms = 0;
	int res = -1;
	char msg[COMMAND_LINE_SIZE];
	unsigned long board_RAM_size = 0;	//Updated by get_RAM_size();;
	extern void determineBootFromFlashOrRom(void);


#endif

#ifdef CONFIG_KGDB_BRCM  /* Older KGDB. Skip. SYY */
	debugInit();
#else
	uart_init(27000000);
#endif
	uart_puts("HI WORLD!!!\n");

#ifdef CONFIG_MIPS_BCM7315_BBX
	*(volatile unsigned long *)(0xfffe7008) = 0x1b800000;
	*(volatile unsigned long *)(0xfffe700c) = 0x00000c91;
#endif

#if defined(CONFIG_MIPS_BCM7115) || defined(CONFIG_MIPS_BCM7112) || \
     defined(CONFIG_MIPS_BCM7314) || defined( CONFIG_MIPS_BCM7315 ) || \
     defined(CONFIG_MIPS_BCM7317) || defined( CONFIG_MIPS_BCM7318 ) || \
     defined(CONFIG_MIPS_BCM7315_BBX) || defined(CONFIG_MIPS_BCM7110) ||\
     defined(CONFIG_MIPS_BCM7111) || defined(CONFIG_MIPS_BCM7312)

	*((volatile unsigned long *)0xfffe0900) = 0x7fff;

#ifndef CONFIG_MIPS_BCM7312

	*(volatile unsigned char *)(0xfffe8008) = 0x00;
	*(volatile unsigned char *)(0xfffe009d) = 0x00;
	*(volatile unsigned char *)(0xfffe0092) = 0x00;
	*(volatile unsigned char *)(0xfffe0093) = 0x55;
	*(volatile unsigned char *)(0xfffe0094) = 0x01;
	*(volatile unsigned char *)(0xfffe0095) = 0xaa;
	*(volatile unsigned char *)(0xfffe009a) = 0x80;
#endif

#if 0
/* THT 7/28/03 Now done in CFE */
#ifdef CONFIG_MIPS_BCM7110
	*(volatile unsigned long *)(0xffe00028) = 0x10fa;
#endif
#if defined( CONFIG_MIPS_BCM7315 ) || defined ( CONFIG_MIPS_BCM7315_BBX )
	*(volatile unsigned long *)(0xffe00028) = 0x30f8;
#endif
#if defined( CONFIG_MIPS_BCM7115 ) || defined ( CONFIG_MIPS_BCM7112 )
	*(volatile unsigned long *)(0xffe00028) = 0x11f5;
#endif
#endif

	//GPIO1 set to output and high to force the drive out of reset. 
	// we need to move this into some borad specific dirs.
	*((volatile unsigned char *)0xfffe0057) &= 0xfd;
	*((volatile unsigned char *)0xfffe0053) |= 0x02;
	// we need to move this into some borad specific dirs.

	//InitEbi();
	//uart_puts("init Ebi\n");
#endif

	/* Fill in platform information */
	mips_machgroup = MACH_GROUP_BRCM;
#ifdef CONFIG_MIPS_BCM3560	
	mips_machtype  = MACH_BRCM_3560;
#endif
#ifdef CONFIG_MIPS_BCM7115	
	mips_machtype  = MACH_BRCM_7115;
#endif
#ifdef CONFIG_MIPS_BCM7112	
	mips_machtype  = MACH_BRCM_7112;
#endif

#ifdef CONFIG_MIPS_BCM7110	
	mips_machtype  = MACH_BRCM_7110;
#endif
#ifdef CONFIG_MIPS_BCM7111
	mips_machtype  = MACH_BRCM_7111;
#endif
#ifdef CONFIG_MIPS_BCM7320	
	mips_machtype  = MACH_BRCM_7320;
/*
** Put this here until CFE does it,ISB bus timer
*/
        *((volatile unsigned long *)0xbafe0900) = 0x7fff;

#endif
#ifdef CONFIG_MIPS_BCM7314	
	mips_machtype  = MACH_BRCM_7314;
#endif
#ifdef CONFIG_MIPS_BCM7315	
	mips_machtype  = MACH_BRCM_7315;
#endif
#ifdef CONFIG_MIPS_BCM7317	
	mips_machtype  = MACH_BRCM_7317;
#endif
#ifdef CONFIG_MIPS_BCM7318	
	mips_machtype  = MACH_BRCM_7318;
#endif
#ifdef CONFIG_MIPS_BCM7319	
		mips_machtype  = MACH_BRCM_7319;
       *((volatile unsigned long *)0xbafe0900) = 0x7fff;
#endif
#ifdef CONFIG_MIPS_BCM7328	
		mips_machtype  = MACH_BRCM_7328;
       *((volatile unsigned long *)0xbafe0900) = 0x7fff;
#endif
#ifdef CONFIG_MIPS_BCM7038	
		mips_machtype  = MACH_BRCM_7038;

#ifdef TEST_ROUNDROBIN_DISABLE
// TBD: Change this to be "if chip revision is C0 or later"
		/*
		 * Testing 1,2,3: PR11804: Disabling RoundRobin in 7038 to see
		 * if it can withstand the transfer rate (bit 6)
		 */
		*((volatile unsigned long *)0xb01061b4) &= 0xffffffbf;
#endif
#endif
#ifdef CONFIG_MIPS_BCM7327	
	mips_machtype  = MACH_BRCM_7327;
       *((volatile unsigned long *)0xbafe0900) = 0x7fff;
#endif
#ifdef CONFIG_MIPS_BCM7329	
	mips_machtype  = MACH_BRCM_7329;
       *((volatile unsigned long *)0xbafe0900) = 0x7fff;
#endif
#ifdef CONFIG_MIPS_BCM7401	
		mips_machtype  = MACH_BRCM_7401;
#endif
#ifdef CONFIG_MIPS_BCM7400
		mips_machtype  = MACH_BRCM_7400;
#endif


	/* Kernel arguments */
#ifdef CONFIG_MIPS_BRCM97XXX
/* For the 97xxx series STB, process CFE boot parms */

	res = get_cfe_boot_parms(cfeBootParms);
	hasCfeParms = !res;

	/* RYH - RAC */
	{
	  char	str1[32], str2[32] = "0x";
	  char *cp, *sp;

	  sprintf(msg, "cfeBootParms ===> %s\n", cfeBootParms);
	  uart_puts(msg);
	  cp = strstr( cfeBootParms, "bcmrac=" );
	  if( cp ) { 
		if ( strchr(cfeBootParms, ',') ) {
			for( cp+=strlen("bcmrac="),sp=str1; *cp != ','; *sp++=*cp++ );
			*sp = '\0';
			strcat( str2, ++cp );
			sprintf(msg, "STR1 : %s    STR2 : %s\n", str1, str2);
		        uart_puts(msg);
			sscanf( str1, "%u", &par_val );
			sscanf( str2, "%u", &par_val2 );
			if (par_val2 == 0x00) par_val2 = (get_RAM_size()-1) & 0xffff0000;
		} else {
			sprintf(msg, "RAC Cacheable Space is set to default...\n");
		        uart_puts(msg);
			sscanf( cp+=strlen("bcmrac="), "%i", &par_val );
			par_val2 = (get_RAM_size()-1) & 0xffff0000;
		}
	  }
	  else {
		par_val = 0x00;		/* set to default */
		par_val2 = (get_RAM_size()-1) & 0xffff0000;
	  }
	}

	/* Just accept whatever specified in BOOT_FLAGS as kernel options, unless root= is NOT specified */
	if (hasCfeParms && isRootSpecified(cfeBootParms)) {
//sprintf(msg, "after get_cfe_boot_parms, res=0, BootParmAddr=%08x,bootParms=%s\n",
//&cfeBootParms[0],cfeBootParms);
//uart_puts(msg);

		strcpy(arcs_cmdline, cfeBootParms);
		appendConsoleArg(arcs_cmdline);
	}
	else /* The old ways of doing it, as root is not specified on the command line */
		
#endif
	{
	/* Gets here when one of the following is true
	  * - CFE is not the boot loader, or
	  * - CFE is the boot loader, but no option is specified or
	  * - CFE option is specified, but does not say root=
	  */

#ifdef CONFIG_BLK_DEV_INITRD
		/*
		 * tht: Things like init=xxxx wants to be first on command line,
		 * kernel arg parse logic depends on that
		 */
		if (hasCfeParms) {
			strcpy(arcs_cmdline, cfeBootParms);
			strcat(arcs_cmdline, DEFAULT_KARGS);
			hasCfeParms = 0; // Suppress further processing
		}
		else {
			strcpy(arcs_cmdline, DEFAULT_KARGS);
		}

#elif defined(CONFIG_CMDLINE)
		char* p;

#ifdef CONFIG_MIPS_BRCM97XXX

		/*
		  * if root= is not on the command line, but user specified something else, 
		  * tag it on.  Some command like init=xxx wants to be first, as kernel
		  * arg parse logic depends on that.
		  */
		if (hasCfeParms && !isRootSpecified(cfeBootParms)) {
			strcpy(arcs_cmdline, cfeBootParms);
			strcat(arcs_cmdline, " ");
			strcat(arcs_cmdline, CONFIG_CMDLINE);
		}
		else
			strcpy(arcs_cmdline, CONFIG_CMDLINE);
#else
		strcpy(arcs_cmdline, CONFIG_CMDLINE);
#endif
		uart_puts("Default command line = \n");
		uart_puts(CONFIG_CMDLINE);
		uart_puts("\n");
		p = &arcs_cmdline[0];
		while (p != NULL && *p != '\0') {
			if (!isspace(*p))
				break;
			p++;
		}
		if (p == NULL || *p == '\0') {
			uart_puts("Defaulting to boot from HD\n");
			/* Default is to boot from HD */
			strcpy(arcs_cmdline,
				"root=/dev/hda1" DEFAULT_KARGS);
		}
		else {
			/* Make sure that the boot params specify a console */
			appendConsoleArg(arcs_cmdline);
		}
		
#else /* No CONFIG_CMDLINE, and not Initrd */
	/* Default is to boot from HD */
		strcpy(arcs_cmdline,
			"root=/dev/hda1" DEFAULT_KARGS);
#endif /* No CONFIG_CMDLINE */



	} /* End else no root= option is specified */

	uart_puts("Kernel boot options: ");
	uart_puts(arcs_cmdline);
	uart_puts("\r\n");

	{
		/*
		  * Support  mem=nn[KMG] on command line
		  */
		char* p = (char*) arcs_cmdline;
		char* q = NULL;
		const char* sizep = NULL;
		int i, foundKeyword = 0, foundNumber = 0, foundUnit = 0, done = 0;
		unsigned int size = 0, unitShift = 0;
		unsigned int ramSizeMB;

		for (i = 0; i < strlen(p) - 6 && !done; i++) {
//sprintf(msg, "i=%d\n", i);
//uart_puts(msg);
			if (0 == strncmp(&p[i], "mem=", 4)) {
				/* Found key, now read in value */
				foundKeyword = 1;

//uart_puts("while\n");
				for (sizep = q = &p[i+4];*q != '\0' && !done; q++) {
					if (isdigit(*q)) {
						foundNumber = 1;
						continue;
					}

					if (foundNumber) {
//uart_puts("found number\n");

						switch (*q) {
						case 'k':
						case 'K':
							unitShift = 10; /* KB shift value*/
							foundUnit = 1;
							done = 1;
							break;
						case 'm':
						case 'M':
							unitShift = 20; /* MB shift value */
							foundUnit = 1;
							done = 1;
//uart_puts("found unit M\n");
//sprintf(msg, "q=%x\n", q);
//uart_puts(msg);

							break;
						case 'g':
						case 'G':
							/* Probably too big */
							unitShift = 30; /* GB shift value */
							foundUnit = 1;
							done = 1;
							break;
						default:
							done = 1;
							break;
						} 
					}
				}
			} 
		} 

		if (foundNumber) {
			if (foundUnit) {
				size = bcm_atoi(sizep);

				sprintf(msg, "Using %d %cB for memory\n", size, *(q-1));
				uart_puts(msg);

				/*
				 * PR17093: Remove mem=xxM from command line to prevent parse_mem_cmdline()
				 * from processing it, and thus causing mmap() to fail.
				 */
				if (i > 0) {
					i--; /* Rewind one space due to for loop */
				}
			    while (i >= 0 && p != NULL && p[i] != '\0' && !isspace(p[i])) {
					p[i] = ' ';
					i++;
			    }
				/*
				uart_puts("After processing mem=xxM, k_arg is :\n<");
				uart_puts(arcs_cmdline);
				uart_puts(">\n");
				*/
			}
			else {
				uart_puts("Syntax: mem=nn[KMG] Option ignored : No unit specified\n");
			}
		}
		else if (foundKeyword) {
			uart_puts("Syntax: mem=nn[KMG] Option ignored : No size specified\n");
		}

		board_RAM_size = get_RAM_size();
		ramSizeMB = board_RAM_size >> 20;

		if (ramSizeMB <= 32) {
#ifdef CONFIG_BLK_DEV_INITRD
			/* 
			  * Initrd kernels on STB with 32 MB RAM
			  */
			if (foundNumber && foundUnit) {
				if (size <= ramSizeMB && size > 0) {
					/* Already output size above */
				} 
				else {
					uart_puts("Invalid size ignored, using default value\n");
					size = 24;
					unitShift = 20;
				}
			}
			else {
				uart_puts("Using default 24MB RAM for STBs\n");
					size = 24;
					unitShift = 20;
			}
 
#else
			/* 
			  * Regular kernels on STBs with 32 MB RAM
			  */
			if (foundNumber && foundUnit) {
				if (size <= 32 && size > 0) {
				/* Already output size above */
				} 
				else {
					uart_puts("Invalid size ignored, using default value of 16MB\n");
					size = 20; // Was 16 in 2.4.25
					unitShift = 20;
				}
			}
			else {
				uart_puts("Using default 20MB, can be overwritten by mem=xxM\n");
				size = 20; // Was 16 in 2.4.25
				unitShift = 20;
			}

#endif
		}
		else {
			/* 
			  * Kernels on STBs with larger than 32MB, we only use 32MB RAM for the kernel
			  */

	  		if (foundNumber && foundUnit) {
				if (size <= ramSizeMB && size > 0) {
				/* Already output size above */
				} 
				else {
					uart_puts("Invalid size ignored, using default value of 32MB\n");
					size = 32;
					unitShift = 20;
				}
			}
			else {
				uart_puts("Using 32MB for kernel, can be overwritten by passing mem=xxM\n");
				size = 32;
				unitShift = 20;
			}
		}
		
		/* Assert size and unit not 0 */
		add_memory_region(0, size << unitShift, BOOT_MEM_RAM);

		/* Register the reserved upper memory, in order to allow kernel to cache them */
		if (size < ramSizeMB) {
			add_memory_region(size << unitShift, (ramSizeMB-size) << unitShift, BOOT_MEM_RAM);
		}
		

	}

	

#if defined (CONFIG_MIPS_BRCM97XXX) 
	(void) determineBootFromFlashOrRom();
#endif /* if BCM97xxx boards */
//	uart_puts("<--prom_init\r\n");
}
const char *get_system_type(void)
{
        return "BCM97xxx Settop Platform";
}


void __init prom_free_prom_memory(void) {}
