/*
 * arch/m32r/boot/compressed/misc.c
 *
 * This is a collection of several routines from gzip-1.0.3
 * adapted for Linux.
 *
 * Adapted for SH by Stuart Menefy, Aug 1999
 *
 * 2003-02-12:	Support M32R by Takeo Takahashi
 * 		This is based on arch/sh/boot/compressed/misc.c.
 */

#include <linux/config.h>
#include <linux/string.h>

/*
 * gzip declarations
 */

typedef unsigned char  uch;
typedef unsigned short ush;
typedef unsigned long  ulg;

#define WSIZE 0x8000		/* Window size must be at least 32k, */
				/* and a power of two */

static uch window[WSIZE];    /* Sliding window buffer */
static unsigned outcnt = 0;  /* bytes in output buffer */

static void flush_window(void);
static void error(char *m);

static unsigned char *input_data;
static int input_len;

static long bytes_out = 0;
static uch *output_data;
static unsigned long output_ptr = 0;

#include "m32r_sio.c"

static unsigned long free_mem_ptr;
static unsigned long free_mem_end_ptr;

#define HEAP_SIZE             0x10000

#include "../../../../lib/inflate.c"

/* ===========================================================================
 * Write the output window window[0..outcnt-1] and update bytes_out.
 * (Used for the decompressed data only.)
 */
static void flush_window(void)
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

static void error(char *x)
{
	puts("\n\n");
	puts(x);
	puts("\n\n -- System halted");

	while(1);	/* Halt */
}

/* return decompressed size */
void
decompress_kernel(int mmu_on, unsigned char *zimage_data,
		  unsigned int zimage_len, unsigned long heap)
{
	output_data = (unsigned char *)CONFIG_MEMORY_START + 0x2000
		+ (mmu_on ? 0x80000000 : 0);
	free_mem_ptr = heap;
	free_mem_end_ptr = free_mem_ptr + HEAP_SIZE;
	input_data = zimage_data;
	input_len = zimage_len;

	puts("Uncompressing Linux... ");
	gunzip(input_data, input_len, NULL);
	puts("Ok, booting the kernel.\n");
}
