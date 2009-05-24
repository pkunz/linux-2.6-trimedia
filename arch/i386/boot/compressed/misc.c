/*
 * misc.c
 * 
 * This is a collection of several routines from gzip-1.0.3 
 * adapted for Linux.
 *
 * puts by Nick Holloway 1993, better puts by Martin Mares 1995
 * High loaded stuff by Hans Lermen & Werner Almesberger, Feb. 1996
 */

#include <linux/linkage.h>
#include <linux/vmalloc.h>
#include <linux/tty.h>
#include <asm/io.h>
#include <asm/page.h>

/*
 * gzip declarations
 */

/*
 * Why do we do this? Don't ask me..
 *
 * Incomprehensible are the ways of bootloaders.
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

/*
 * This is set up by the setup-routine at boot-time
 */
static unsigned char *real_mode; /* Pointer to real-mode data */

#define RM_EXT_MEM_K   (*(unsigned short *)(real_mode + 0x2))
#ifndef STANDARD_MEMORY_BIOS_CALL
#define RM_ALT_MEM_K   (*(unsigned long *)(real_mode + 0x1e0))
#endif
#define RM_SCREEN_INFO (*(struct screen_info *)(real_mode+0))

extern char input_data[];
extern int input_len;

static long bytes_out = 0;
static uch *output_data;
static unsigned long output_ptr = 0;

static void putstr(const char *);

extern int end;
static long free_mem_ptr = (long)&end;
static long free_mem_end_ptr;

#define INPLACE_MOVE_ROUTINE  0x1000
#define LOW_BUFFER_START      0x2000
#define LOW_BUFFER_MAX       0x90000
#define HEAP_SIZE             0x3000
static unsigned int low_buffer_end, low_buffer_size;
static int high_loaded =0;
static uch *high_buffer_start /* = (uch *)(((ulg)&end) + HEAP_SIZE)*/;

static char *vidmem = (char *)0xb8000;
static int vidport;
static int lines, cols;

#ifdef CONFIG_X86_NUMAQ
static void * xquad_portio = NULL;
#endif

#include "../../../../lib/inflate.c"

static void scroll(void)
{
	int i;

	for (i = 0; i < (lines - 1) * cols * 2; i++)
		vidmem[i] = vidmem[i + cols * 2];

	for ( i = ( lines - 1 ) * cols * 2; i < lines * cols * 2; i += 2 )
		vidmem[i] = ' ';
}

static void putstr(const char *s)
{
	int x,y,pos;
	char c;

	x = RM_SCREEN_INFO.orig_x;
	y = RM_SCREEN_INFO.orig_y;

	while ( ( c = *s++ ) != '\0' ) {
		if ( c == '\n' ) {
			x = 0;
			if ( ++y >= lines ) {
				scroll();
				y--;
			}
		} else {
			vidmem [ ( x + cols * y ) * 2 ] = c; 
			if ( ++x >= cols ) {
				x = 0;
				if ( ++y >= lines ) {
					scroll();
					y--;
				}
			}
		}
	}

	RM_SCREEN_INFO.orig_x = x;
	RM_SCREEN_INFO.orig_y = y;

	pos = (x + cols * y) * 2;	/* Update cursor position */
	outb_p(14, vidport);
	outb_p(0xff & (pos >> 9), vidport+1);
	outb_p(15, vidport);
	outb_p(0xff & (pos >> 1), vidport+1);
}

/* ===========================================================================
 * Write the output window window[0..outcnt-1] and update bytes_out.
 * (Used for the decompressed data only.)
 */
static void flush_window_low(void)
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

static void flush_window_high(void)
{
    unsigned n;
    uch *in,  ch;
    in = window;
    for (n = 0; n < outcnt; n++) {
	ch = *output_data++ = *in++;
	if ((ulg)output_data == low_buffer_end) output_data=high_buffer_start;
    }

    bytes_out += (ulg)outcnt;
    outcnt = 0;
}

static void flush_window(void)
{
	if (high_loaded) flush_window_high();
	else flush_window_low();
}

static void error(char *x)
{
	putstr("\n\n");
	putstr(x);
	putstr("\n\n -- System halted");

	while(1);	/* Halt */
}

#define STACK_SIZE (4096)

long user_stack [STACK_SIZE];

struct {
	long * a;
	short b;
	} stack_start = { & user_stack [STACK_SIZE] , __BOOT_DS };

static void setup_normal_output_buffer(void)
{
#ifdef STANDARD_MEMORY_BIOS_CALL
	if (RM_EXT_MEM_K < 1024) error("Less than 2MB of memory");
#else
	if ((RM_ALT_MEM_K > RM_EXT_MEM_K ? RM_ALT_MEM_K : RM_EXT_MEM_K) < 1024) error("Less than 2MB of memory");
#endif
	output_data = (char *)__PHYSICAL_START; /* Normally Points to 1M */
	free_mem_end_ptr = (long)real_mode;
}

struct moveparams {
	uch *low_buffer_start;  int lcount;
	uch *high_buffer_start; int hcount;
};

static void setup_output_buffer_if_we_run_high(struct moveparams *mv)
{
	high_buffer_start = (uch *)(((ulg)&end) + HEAP_SIZE);
#ifdef STANDARD_MEMORY_BIOS_CALL
	if (RM_EXT_MEM_K < (3*1024)) error("Less than 4MB of memory");
#else
	if ((RM_ALT_MEM_K > RM_EXT_MEM_K ? RM_ALT_MEM_K : RM_EXT_MEM_K) <
			(3*1024))
		error("Less than 4MB of memory");
#endif	
	mv->low_buffer_start = output_data = (char *)LOW_BUFFER_START;
	low_buffer_end = ((unsigned int)real_mode > LOW_BUFFER_MAX
	  ? LOW_BUFFER_MAX : (unsigned int)real_mode) & ~0xfff;
	low_buffer_size = low_buffer_end - LOW_BUFFER_START;
	high_loaded = 1;
	free_mem_end_ptr = (long)high_buffer_start;
	if ( (__PHYSICAL_START + low_buffer_size) > ((ulg)high_buffer_start)) {
		high_buffer_start = (uch *)(__PHYSICAL_START + low_buffer_size);
		mv->hcount = 0; /* say: we need not to move high_buffer */
	}
	else mv->hcount = -1;
	mv->high_buffer_start = high_buffer_start;
}

static void close_output_buffer_if_we_run_high(struct moveparams *mv)
{
	if (bytes_out > low_buffer_size) {
		mv->lcount = low_buffer_size;
		if (mv->hcount)
			mv->hcount = bytes_out - low_buffer_size;
	} else {
		mv->lcount = bytes_out;
		mv->hcount = 0;
	}
}

asmlinkage int decompress_kernel(struct moveparams *mv, void *rmode)
{
	real_mode = rmode;

	if (RM_SCREEN_INFO.orig_video_mode == 7) {
		vidmem = (char *) 0xb0000;
		vidport = 0x3b4;
	} else {
		vidmem = (char *) 0xb8000;
		vidport = 0x3d4;
	}

	lines = RM_SCREEN_INFO.orig_video_lines;
	cols = RM_SCREEN_INFO.orig_video_cols;

	if (free_mem_ptr < 0x100000) setup_normal_output_buffer();
	else setup_output_buffer_if_we_run_high(mv);

	putstr("Uncompressing Linux... ");
	gunzip(input_data, input_len, NULL);
	putstr("Ok, booting the kernel.\n");
	if (high_loaded) close_output_buffer_if_we_run_high(mv);
	return high_loaded;
}
