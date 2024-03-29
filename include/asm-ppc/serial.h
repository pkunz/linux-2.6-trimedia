/*
 * include/asm-ppc/serial.h
 */

#ifdef __KERNEL__
#ifndef __ASM_SERIAL_H__
#define __ASM_SERIAL_H__

#include <linux/config.h>

#if defined(CONFIG_EV64260)
#include <platforms/ev64260.h>
#elif defined(CONFIG_CHESTNUT)
#include <platforms/chestnut.h>
#elif defined(CONFIG_GEMINI)
#include <platforms/gemini_serial.h>
#elif defined(CONFIG_POWERPMC250)
#include <platforms/powerpmc250.h>
#elif defined(CONFIG_LOPEC)
#include <platforms/lopec.h>
#elif defined(CONFIG_MVME5100)
#include <platforms/mvme5100.h>
#elif defined(CONFIG_MCP905)
#include <platforms/mcp905.h>
#elif defined(CONFIG_PAL4)
#include <platforms/pal4_serial.h>
#elif defined(CONFIG_PRPMC750)
#include <platforms/prpmc750.h>
#elif defined(CONFIG_PRPMC800)
#include <platforms/prpmc800.h>
#elif defined(CONFIG_SANDPOINT)
#include <platforms/sandpoint.h>
#elif defined(CONFIG_TAIGA)
#include <platforms/taiga.h>
#elif defined(CONFIG_SPRUCE)
#include <platforms/spruce.h>
#elif defined(CONFIG_4xx)
#include <asm/ibm4xx.h>
#elif defined(CONFIG_83xx)
#include <asm/mpc83xx.h>
#elif defined(CONFIG_85xx)
#include <asm/mpc85xx.h>
#elif defined(CONFIG_RADSTONE_PPC7D)
#include <platforms/radstone_ppc7d.h>
#else

/*
 * XXX Assume for now it has PC-style ISA serial ports.
 * This is true for PReP and CHRP at least.
 */
#include <asm/pc_serial.h>

#if defined(CONFIG_MAC_SERIAL)
#define SERIAL_DEV_OFFSET	((_machine == _MACH_prep || _machine == _MACH_chrp) ? 0 : 2)
#endif

#endif /* !CONFIG_GEMINI and others */
#endif /* __ASM_SERIAL_H__ */
#endif /* __KERNEL__ */
