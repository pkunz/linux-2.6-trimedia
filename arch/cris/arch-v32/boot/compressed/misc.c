/*
 * misc.c
 *
 * $Id: misc.c,v 1.8 2005/04/24 18:34:29 starvik Exp $
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
#include <asm/arch/hwregs/reg_rdwr.h>
#include <asm/arch/hwregs/reg_map.h>
#include <asm/arch/hwregs/ser_defs.h>

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

extern char *input_data;  /* lives in head.S */

static long bytes_out = 0;
static uch *output_data;
static unsigned long output_ptr = 0;

static void puts(const char *);

/* the "heap" is put directly after the BSS ends, at end */

extern int _end;
static long free_mem_ptr = (long)&_end;
static long free_mem_end_ptr = 0xffffffff;

#include "../../../../../lib/inflate.c"

/* decompressor info and error messages to serial console */

static inline void
serout(const char *s, reg_scope_instances regi_ser)
{
	reg_ser_rs_stat_din rs;
	reg_ser_rw_dout dout = {.data = *s};

	do {
		rs = REG_RD(ser, regi_ser, rs_stat_din);
	}
	while (!rs.tr_rdy);/* Wait for tranceiver. */

	REG_WR(ser, regi_ser, rw_dout, dout);
}

static void
puts(const char *s)
{
#ifndef CONFIG_ETRAX_DEBUG_PORT_NULL
	while (*s) {
#ifdef CONFIG_ETRAX_DEBUG_PORT0
		serout(s, regi_ser0);
#endif
#ifdef CONFIG_ETRAX_DEBUG_PORT1
		serout(s, regi_ser1);
#endif
#ifdef CONFIG_ETRAX_DEBUG_PORT2
		serout(s, regi_ser2);
#endif
#ifdef CONFIG_ETRAX_DEBUG_PORT3
		serout(s, regi_ser3);
#endif
		*s++;
	}
/* CONFIG_ETRAX_DEBUG_PORT_NULL */
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

static inline void
serial_setup(reg_scope_instances regi_ser)
{
	reg_ser_rw_xoff xoff;
	reg_ser_rw_tr_ctrl tr_ctrl;
	reg_ser_rw_rec_ctrl rec_ctrl;
	reg_ser_rw_tr_baud_div tr_baud;
	reg_ser_rw_rec_baud_div rec_baud;

	/* Turn off XOFF. */
	xoff = REG_RD(ser, regi_ser, rw_xoff);

	xoff.chr = 0;
	xoff.automatic = regk_ser_no;

	REG_WR(ser, regi_ser, rw_xoff, xoff);

	/* Set baudrate and stopbits. */
	tr_ctrl = REG_RD(ser, regi_ser, rw_tr_ctrl);
	rec_ctrl = REG_RD(ser, regi_ser, rw_rec_ctrl);
	tr_baud = REG_RD(ser, regi_ser, rw_tr_baud_div);
	rec_baud = REG_RD(ser, regi_ser, rw_rec_baud_div);

	tr_ctrl.stop_bits = 1;	/* 2 stop bits. */

	/*
	 * The baudrate setup is a bit fishy, but in the end the tranceiver is
	 * set to 4800 and the receiver to 115200. The magic value is
	 * 29.493 MHz.
	 */
	tr_ctrl.base_freq = regk_ser_f29_493;
	rec_ctrl.base_freq = regk_ser_f29_493;
	tr_baud.div = (29493000 / 8) / 4800;
	rec_baud.div = (29493000 / 8) / 115200;

	REG_WR(ser, regi_ser, rw_tr_ctrl, tr_ctrl);
	REG_WR(ser, regi_ser, rw_tr_baud_div, tr_baud);
	REG_WR(ser, regi_ser, rw_rec_ctrl, rec_ctrl);
	REG_WR(ser, regi_ser, rw_rec_baud_div, rec_baud);
}

void
decompress_kernel()
{
	char revision;

	/* input_data is set in head.S */
	inbuf = input_data;

#ifdef CONFIG_ETRAX_DEBUG_PORT0
	serial_setup(regi_ser0);
#endif
#ifdef CONFIG_ETRAX_DEBUG_PORT1
	serial_setup(regi_ser1);
#endif
#ifdef CONFIG_ETRAX_DEBUG_PORT2
	serial_setup(regi_ser2);
#endif
#ifdef CONFIG_ETRAX_DEBUG_PORT3
	serial_setup(regi_ser3);
#endif

	setup_normal_output_buffer();

	__asm__ volatile ("move $vr,%0" : "=rm" (revision));
	if (revision < 32)
	{
		puts("You need an ETRAX FS to run Linux 2.6/crisv32.\n");
		while(1);
	}

	puts("Uncompressing Linux...\n");
	gunzip(input_data, 0x7fffffff, NULL);
	puts("Done. Now booting the kernel.\n");
}
