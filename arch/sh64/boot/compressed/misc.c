/*
 * arch/shmedia/boot/compressed/misc.c
 *
 * This is a collection of several routines from gzip-1.0.3
 * adapted for Linux.
 *
 * Adapted for SHmedia from sh by Stuart Menefy, May 2002
 */

#include <linux/config.h>
#include <asm/uaccess.h>

/* cache.c */
#define CACHE_ENABLE      0
#define CACHE_DISABLE     1
int cache_control(unsigned int command);

/*
 * gzip declarations
 */

typedef unsigned char uch;
typedef unsigned short ush;
typedef unsigned long ulg;

#define WSIZE 0x8000		/* Window size must be at least 32k, */
				/* and a power of two */

static uch window[WSIZE];	/* Sliding window buffer */
static unsigned outcnt = 0;	/* bytes in output buffer */

static void flush_window(void);
static void error(char *m);

extern char input_data[];
extern int input_len;

static long bytes_out = 0;
static uch *output_data;
static unsigned long output_ptr = 0;

static void puts(const char *);

extern int _text;		/* Defined in vmlinux.lds.S */
extern int _end;
static unsigned long free_mem_ptr;
static unsigned long free_mem_end_ptr;

#define HEAP_SIZE             0x10000

#include "../../../../lib/inflate.c"

void puts(const char *s)
{
}

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

	bytes_out += (ulg) outcnt;
	output_ptr += (ulg) outcnt;
	outcnt = 0;
	puts(".");
}

static void error(char *x)
{
	puts("\n\n");
	puts(x);
	puts("\n\n -- System halted");

	while (1) ;		/* Halt */
}

#define STACK_SIZE (4096)
long __attribute__ ((aligned(8))) user_stack[STACK_SIZE];
long *stack_start = &user_stack[STACK_SIZE];

void decompress_kernel(void)
{
	output_data = (uch *) (CONFIG_MEMORY_START + 0x2000);
	free_mem_ptr = (unsigned long) &_end;
	free_mem_end_ptr = free_mem_ptr + HEAP_SIZE;

	puts("Uncompressing Linux... ");
	cache_control(CACHE_ENABLE);
	gunzip(input_data, input_len, NULL);
	puts("\n");

#if 0
	/* When booting from ROM may want to do something like this if the
	 * boot loader doesn't.
	 */

	/* Set up the parameters and command line */
	{
		volatile unsigned int *parambase =
		    (int *) (CONFIG_MEMORY_START + 0x1000);

		parambase[0] = 0x1;	/* MOUNT_ROOT_RDONLY */
		parambase[1] = 0x0;	/* RAMDISK_FLAGS */
		parambase[2] = 0x0200;	/* ORIG_ROOT_DEV */
		parambase[3] = 0x0;	/* LOADER_TYPE */
		parambase[4] = 0x0;	/* INITRD_START */
		parambase[5] = 0x0;	/* INITRD_SIZE */
		parambase[6] = 0;

		strcpy((char *) ((int) parambase + 0x100),
		       "console=ttySC0,38400");
	}
#endif

	puts("Ok, booting the kernel.\n");

	cache_control(CACHE_DISABLE);
}
