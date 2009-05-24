/*
 * misc.c
 *
 * $Id: misc.c,v 1.6 2003/10/27 08:04:31 starvik Exp $
 * 
 * This is a collection of several routines from gzip-1.0.3 
 * adapted for Linux.
 *
 * puts by Nick Holloway 1993, better puts by Martin Mares 1995
 * adoptation for Linux/CRIS Axis Communications AB, 1999
 * 
 */

/* where the piggybacked kernel image expects itself to live.
 * it is the same address we use when we network load an uncompressed
 * image into DRAM, and it is the address the kernel is linked to live
 * at by vmlinux.lds.S
 */

#define KERNEL_LOAD_ADR 0x40004000

#include <linux/config.h>

#include <linux/types.h>
#include <asm/arch/svinto.h>

/*
 * gzip declarations
 */

typedef unsigned char  uch;
typedef unsigned short ush;
typedef unsigned long  ulg;

#define WSIZE 0x8000		/* Window size must be at least 32k, */
				/* and a power of two */

static uch window[WSIZE];    /* Sliding window buffer */
unsigned compsize; /* compressed size, used by head.S */
static unsigned outcnt = 0;  /* bytes in output buffer */

static void flush_window(void);
static void error(char *m);

extern char *input_data;  /* lives in head.S */

static long bytes_out = 0;
static uch *output_data;
static unsigned long output_ptr = 0;

static void puts(const char *);

/* the "heap" is put directly after the BSS ends, at end */
  
extern int end;
static long free_mem_ptr = (long)&end;
static long free_mem_end_ptr = 0xffffffff;

#include "../../../../../lib/inflate.c"

/* decompressor info and error messages to serial console */

static void
puts(const char *s)
{
#ifndef CONFIG_ETRAX_DEBUG_PORT_NULL
	while(*s) {
#ifdef CONFIG_ETRAX_DEBUG_PORT0
		while(!(*R_SERIAL0_STATUS & (1 << 5))) ;
		*R_SERIAL0_TR_DATA = *s++;
#endif
#ifdef CONFIG_ETRAX_DEBUG_PORT1
		while(!(*R_SERIAL1_STATUS & (1 << 5))) ;
		*R_SERIAL1_TR_DATA = *s++;
#endif
#ifdef CONFIG_ETRAX_DEBUG_PORT2
		while(!(*R_SERIAL2_STATUS & (1 << 5))) ;
		*R_SERIAL2_TR_DATA = *s++;
#endif
#ifdef CONFIG_ETRAX_DEBUG_PORT3
		while(!(*R_SERIAL3_STATUS & (1 << 5))) ;
		*R_SERIAL3_TR_DATA = *s++;
#endif
	}
#endif
}

/* ===========================================================================
 * Write the output window window[0..outcnt-1] and update bytes_out.
 * (Used for the decompressed data only.)
 */

static void
flush_window()
{
    unsigned n;
    uch *in, *out, ch;

    in = window;
    out = &output_data[output_ptr];
    for (n = 0; n < outcnt; n++)
	    ch = *out++ = *in++;

    bytes_out += (ulg)outcnt;
    output_ptr += (ulg)outcnt;
    outcnt = 0;
}

static void
error(char *x)
{
	puts("\n\n");
	puts(x);
	puts("\n\n -- System halted\n");

	while(1);	/* Halt */
}

void
setup_normal_output_buffer()
{
	output_data = (char *)KERNEL_LOAD_ADR;
}

void
decompress_kernel()
{
	char revision;
	
	/* input_data is set in head.S */
	inbuf = input_data;

#ifdef CONFIG_ETRAX_DEBUG_PORT0
	*R_SERIAL0_XOFF = 0;
	*R_SERIAL0_BAUD = 0x99;
	*R_SERIAL0_TR_CTRL = 0x40;
#endif
#ifdef CONFIG_ETRAX_DEBUG_PORT1
	*R_SERIAL1_XOFF = 0;
	*R_SERIAL1_BAUD = 0x99;
	*R_SERIAL1_TR_CTRL = 0x40;
#endif
#ifdef CONFIG_ETRAX_DEBUG_PORT2
	*R_GEN_CONFIG = 0x08;
	*R_SERIAL2_XOFF = 0;
	*R_SERIAL2_BAUD = 0x99;
	*R_SERIAL2_TR_CTRL = 0x40;
#endif
#ifdef CONFIG_ETRAX_DEBUG_PORT3
	*R_GEN_CONFIG = 0x100;
	*R_SERIAL3_XOFF = 0;
	*R_SERIAL3_BAUD = 0x99;
	*R_SERIAL3_TR_CTRL = 0x40;
#endif

	setup_normal_output_buffer();

	__asm__ volatile ("move vr,%0" : "=rm" (revision));
	if (revision < 10)
	{
		puts("You need an ETRAX 100LX to run linux 2.6\n");
		while(1);
	}

	puts("Uncompressing Linux...\n");
	compsize = gunzip(input_data, 0x7fffffff, NULL);
	puts("Done. Now booting the kernel.\n");
}
