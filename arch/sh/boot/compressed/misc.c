/*
 * arch/sh/boot/compressed/misc.c
 *
 * This is a collection of several routines from gzip-1.0.3
 * adapted for Linux.
 *
 * Adapted for SH by Stuart Menefy, Aug 1999
 *
 * Modified to use standard LinuxSH BIOS by Greg Banks 7Jul2000
 */

#include <linux/config.h>
#include <asm/uaccess.h>
#ifdef CONFIG_SH_STANDARD_BIOS
#include <asm/sh_bios.h>
#endif

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

extern char input_data[];
extern int input_len;

static long bytes_out = 0;
static uch *output_data;
static unsigned long output_ptr = 0;

int puts(const char *);

extern int _text;		/* Defined in vmlinux.lds.S */
extern int _end;
static unsigned long free_mem_ptr;
static unsigned long free_mem_end_ptr;

#define HEAP_SIZE             0x10000

#include "../../../../lib/inflate.c"

#ifdef CONFIG_SH_STANDARD_BIOS
size_t strlen(const char *s)
{
	int i = 0;

	while (*s++)
		i++;
	return i;
}

int puts(const char *s)
{
	int len = strlen(s);
	sh_bios_console_write(s, len);
	return len;
}
#else
int puts(const char *s)
{
	/* This should be updated to use the sh-sci routines */
	return 0;
}
#endif

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

#define STACK_SIZE (4096)
long user_stack [STACK_SIZE];
long* stack_start = &user_stack[STACK_SIZE];

void decompress_kernel(void)
{
	output_data = 0;
	output_ptr = (unsigned long)&_text+0x20001000;
	free_mem_ptr = (unsigned long)&_end;
	free_mem_end_ptr = free_mem_ptr + HEAP_SIZE;

	puts("Uncompressing Linux... ");
	gunzip(input_data, input_len, NULL);
	puts("Ok, booting the kernel.\n");
}
