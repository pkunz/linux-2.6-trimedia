/*
 * misc.c
 * 
 * This is a collection of several routines from gzip-1.0.3 
 * adapted for Linux.
 *
 * Modified for ARM Linux by Russell King
 *
 * Nicolas Pitre <nico@visuaide.com>  1999/04/14 :
 *  For this code to run directly from Flash, all constant variables must
 *  be marked with 'const' and all other variables initialized at run-time 
 *  only.  This way all non constant variables will end up in the bss segment,
 *  which should point to addresses in RAM and cleared to 0 on start.
 *  This allows for a much quicker boot time.
 *
 * Modified for Alpha, from the ARM version, by Jay Estabrook 2003.
 */

#include <linux/kernel.h>

#include <asm/uaccess.h>

#define puts		srm_printk
extern long srm_printk(const char *, ...)
     __attribute__ ((format (printf, 1, 2)));

/*
 * gzip delarations
 */

typedef unsigned char  uch;
typedef unsigned short ush;
typedef unsigned long  ulg;

#define WSIZE 0x8000		/* Window size must be at least 32k, */
				/* and a power of two */

static uch *window;		/* Sliding window buffer */
static unsigned outcnt;		/* bytes in output buffer */

static void flush_window(void);
static void error(char *m);

static char *input_data;
static int  input_data_size;

static uch *output_data;
static ulg output_ptr;
static ulg bytes_out;

extern int end;
static ulg free_mem_ptr;
static ulg free_mem_ptr_end;

#define HEAP_SIZE 0x2000

#include "../../../lib/inflate.c"

/* ===========================================================================
 * Write the output window window[0..outcnt-1] and update bytes_out.
 * (Used for the decompressed data only.)
 */
void flush_window(void)
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
/*	puts("."); */
}

static void error(char *x)
{
	puts("\n\n");
	puts(x);
	puts("\n\n -- System halted");

	while(1);	/* Halt */
}

unsigned int
decompress_kernel(void *output_start,
		  void *input_start,
		  size_t ksize,
		  size_t kzsize)
{
	output_data		= (uch *)output_start;
	input_data		= (uch *)input_start;
	input_data_size		= kzsize; /* use compressed size */

	/* FIXME FIXME FIXME */
	free_mem_ptr		= (ulg)output_start + ksize;
	free_mem_ptr_end	= (ulg)output_start + ksize + 0x200000;
	/* FIXME FIXME FIXME */

	/* put in temp area to reduce initial footprint */
	window = malloc(WSIZE);

/*	puts("Uncompressing Linux..."); */
	gunzip(input_data, input_data_size, NULL);
/*	puts(" done, booting the kernel.\n"); */
	return output_ptr;
}
