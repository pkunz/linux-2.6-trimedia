#define DEBG(x)
#define DEBG1(x)
/* inflate.c -- Not copyrighted 1992 by Mark Adler
 * version c10p1, 10 January 1993
 *
 * Adapted for booting Linux by Hannu Savolainen 1993
 * based on gzip-1.0.3
 *
 * Nicolas Pitre <nico@cam.org>, 1999/04/14 :
 *   Little mods for all variable to reside either into rodata or bss
 *   segments by marking constant variables with 'const' and
 *   initializing all the others at run-time only. This allows for the
 *   kernel uncompressor to run directly from Flash or ROM memory on
 *   embedded systems.
 */

/*
   Inflate deflated (PKZIP's method 8 compressed) data.  The compression
   method searches for as much of the current string of bytes (up to a
   length of 258) in the previous 32 K bytes.  If it doesn't find any
   matches (of at least length 3), it codes the next byte.  Otherwise, it
   codes the length of the matched string and its distance backwards from
   the current position.  There is a single Huffman code that codes both
   single bytes (called "literals") and match lengths.  A second Huffman
   code codes the distance information, which follows a length code.  Each
   length or distance code actually represents a base value and a number
   of "extra" (sometimes zero) bits to get to add to the base value.  At
   the end of each deflated block is a special end-of-block (EOB) literal/
   length code.  The decoding process is basically: get a literal/length
   code; if EOB then done; if a literal, emit the decoded byte; if a
   length then get the distance and emit the referred-to bytes from the
   sliding window of previously emitted data.

   There are (currently) three kinds of inflate blocks: stored, fixed, and
   dynamic.  The compressor deals with some chunk of data at a time, and
   decides which method to use on a chunk-by-chunk basis.  A chunk might
   typically be 32 K or 64 K.  If the chunk is incompressible, then the
   "stored" method is used.  In this case, the bytes are simply stored as
   is, eight bits per byte, with none of the above coding.  The bytes are
   preceded by a count, since there is no longer an EOB code.

   If the data is compressible, then either the fixed or dynamic methods
   are used.  In the dynamic method, the compressed data is preceded by
   an encoding of the literal/length and distance Huffman codes that are
   to be used to decode this block.  The representation is itself Huffman
   coded, and so is preceded by a description of that code.  These code
   descriptions take up a little space, and so for small blocks, there is
   a predefined set of codes, called the fixed codes.  The fixed method is
   used if the block codes up smaller that way (usually for quite small
   chunks), otherwise the dynamic method is used.  In the latter case, the
   codes are customized to the probabilities in the current block, and so
   can code it much better than the pre-determined fixed codes.

   The Huffman codes themselves are decoded using a multi-level table
   lookup, in order to maximize the speed of decoding plus the speed of
   building the decoding tables.  See the comments below that precede the
   lbits and dbits tuning parameters.
 */

/*
   Notes beyond the 1.93a appnote.txt:

   1. Distance pointers never point before the beginning of the output
      stream.
   2. Distance pointers can point back across blocks, up to 32k away.
   3. There is an implied maximum of 7 bits for the bit length table and
      15 bits for the actual data.
   4. If only one code exists, then it is encoded using one bit.  (Zero
      would be more efficient, but perhaps a little confusing.)  If two
      codes exist, they are coded using one bit each (0 and 1).
   5. There is no way of sending zero distance codes--a dummy must be
      sent if there are none.  (History: a pre 2.0 version of PKZIP would
      store blocks with no distance codes, but this was discovered to be
      too harsh a criterion.)  Valid only for 1.93a.  2.04c does allow
      zero distance codes, which is sent as one code of zero bits in
      length.
   6. There are up to 286 literal/length codes.  Code 256 represents the
      end-of-block.  Note however that the static length tree defines
      288 codes just to fill out the Huffman codes.  Codes 286 and 287
      cannot be used though, since there is no length base or extra bits
      defined for them.  Similarly, there are up to 30 distance codes.
      However, static trees define 32 codes (all 5 bits) to fill out the
      Huffman codes, but the last two had better not show up in the data.
   7. Unzip can check dynamic Huffman blocks for complete code sets.
      The exception is that a single code would not be complete (see #4).
   8. The five bits following the block type is really the number of
      literal codes sent minus 257.
   9. Length codes 8,16,16 are interpreted as 13 length codes of 8 bits
      (1+6+6).  Therefore, to output three times the length, you output
      three codes (1+1+1), whereas to output four times the same length,
      you only need two codes (1+3).  Hmm.
  10. In the tree reconstruction algorithm, Code = Code + Increment
      only if BitLength(i) is not zero.  (Pretty obvious.)
  11. Correction: 4 Bits: # of Bit Length codes - 4     (4 - 19)
  12. Note: length code 284 can represent 227-258, but length code 285
      really is 258.  The last length deserves its own, short code
      since it gets used a lot in very redundant files.  The length
      258 is special since 258 - 3 (the min match length) is 255.
  13. The literal/length and distance code bit lengths are read as a
      single stream of lengths.  It is possible (and advantageous) for
      a repeat code (16, 17, or 18) to go across the boundary between
      the two sets of lengths.
 */
#include <linux/compiler.h>

#ifndef INIT
#define INIT
#endif

#include <asm/types.h>

#ifndef NO_INFLATE_MALLOC
/* A trivial malloc implementation, adapted from
 *  malloc by Hannu Savolainen 1993 and Matthias Urlichs 1994
 */

static unsigned long malloc_ptr;
static int malloc_count;

static void *malloc(int size)
{
	void *p;

	if (size <0)
		error("Malloc error");
	if (!malloc_ptr)
		malloc_ptr = free_mem_ptr;

	malloc_ptr = (malloc_ptr + 3) & ~3;	/* Align */

	p = (void *)malloc_ptr;
	malloc_ptr += size;

	if (malloc_ptr >= free_mem_end_ptr)
		error("Out of memory");

	malloc_count++;
	return p;
}

static void free(void *where)
{
	malloc_count--;
	if (!malloc_count)
		malloc_ptr = free_mem_ptr;
}
#else
#define malloc(a) kmalloc(a, GFP_KERNEL)
#define free(a) kfree(a)
#endif

static u32 crc_32_tab[256];
#define CRCPOLY_LE 0xedb88320

/* Huffman code lookup table entry--this entry is four bytes for machines
   that have 16-bit pointers (e.g. PC's in the small or medium model).
   Valid extra bits are 0..13.  e == 15 is EOB (end of block), e == 16
   means that v is a literal, 16 < e < 32 means that v is a pointer to
   the next table, which codes e - 16 bits, and lastly e == 99 indicates
   an unused code.  If a code with e == 99 is looked up, this implies an
   error in the data. */
struct huft {
	union {
		u16 val; /* literal, length base, or distance base */
		struct huft *next; /* pointer to next level of table */
	};
	u8 extra; /* number of extra bits or operation */
	u8 bits; /* number of bits in this code or subcode */
};

struct iostate {
	u8 *window, *ibuf;
	int ipos, isize, itotal, opos, osize, bits;
	void (*fill)(u8 *ibuf, int len);
	u32 buf, crc;
};

/* Function prototypes */
static int INIT huft_build(unsigned *, unsigned, unsigned,
			  const u16 *, const u16 *, struct huft **, int *);
static int INIT huft_free(struct huft *);
static int INIT inflate_codes(struct iostate *, struct huft *, struct huft *,
			 int, int);
static int INIT inflate_stored(struct iostate *);
static int INIT inflate_fixed(struct iostate *);
static int INIT inflate_dynamic(struct iostate *);
static int INIT inflate_block(struct iostate *, int *);
static int INIT inflate(struct iostate *);

/* The inflate algorithm uses a sliding 32 K byte window on the uncompressed
   stream to find repeated byte strings.  This is implemented here as a
   circular buffer.  The index is updated simply by incrementing and then
   ANDing with 0x7fff (32K-1). */
/* It is left to other modules to supply the 32 K area.  It is assumed
   to be usable as if it were declared "u8 window[32768];" or as just
   "u8 *window;" and then malloc'ed in the latter case.  The definition
   must be in unzip.h, included above. */

static void flush_output(struct iostate *io)
{
	int i;

	for (i = 0; i < io->opos; i++)
		io->crc = crc_32_tab[(io->window[i] ^ (int)io->crc) & 0xff]
			^ (io->crc >> 8);

	outcnt = io->opos;
	flush_window();
	io->opos = 0;
}

static void put_byte(struct iostate *io, u8 byte)
{
	io->window[io->opos++] = byte;
	if (io->opos == io->osize)
		flush_output(io);
}

static void copy_bytes(struct iostate *io, int len, int dist)
{
	int part, pos = io->opos - dist;

	do {
		pos &= io->osize - 1;
		part = min(len, io->osize - max(pos, io->opos));
		len -= part;

		while (part--)
			io->window[io->opos++] = io->window[pos++];

		if (io->opos == io->osize)
			flush_output(io);
	} while (len);
}

/* Tables for deflate from PKZIP's appnote.txt. */

/* Order of the bit length code lengths */
static const unsigned border[] = {
	16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
};

/* Copy lengths for literal codes 257..285 */
static const u16 cplens[] = {
	3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
	35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0
};

/* Extra bits for literal codes 257..285
 * note: see note #13 above about the 258 in this list.
 * 99==invalid
 */
static const u16 cplext[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
	3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 99, 99
};

/* Copy offsets for distance codes 0..29 */
static const u16 cpdist[] = {
	1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
	257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
	8193, 12289, 16385, 24577
};

/* Extra bits for distance codes */
static const u16 cpdext[] = {
	0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
	7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
	12, 12, 13, 13
};

/* Inlines for inflate() bit peeking and grabbing.
   The usage is:

        x = readbits(io, j);
	dumpbits(io, j);

	x = pullbits(io, j);

   where readbits makes sure that b has at least j bits in it, and
   dumpbits removes the bits from b, while k tracks the number of bits
   in b.

   If we assume that EOB will be the longest code, then we will never
   ask for bits that are beyond the end of the stream. So, readbits
   should not read any more bytes than are needed to meet the request.
   Then no bytes need to be "returned" to the buffer at the end of the
   last block.

   However, this assumption is not true for fixed blocks--the EOB code
   is 7 bits, but the other literal/length codes can be 8 or 9 bits.
   (The EOB code is shorter than other codes because fixed blocks are
   generally short.  So, while a block always has an EOB, many other
   literal/length codes have a significantly lower probability of
   showing up at all.)  However, by making the first table have a
   lookup of seven bits, the EOB code will be found in that first
   lookup, and so will not require that too many bits be pulled from
   the stream.
 */

static u32 readbyte(struct iostate *io)
{
	if (io->ipos == io->isize) {
		io->fill(io->ibuf, io->isize);
		io->itotal += io->ipos;
		io->ipos = 0;
	}

	return io->ibuf[io->ipos++];
}

static inline u32 readbits(struct iostate *io, int n)
{
	for( ; io->bits < n; io->bits += 8)
		io->buf |= readbyte(io) << io->bits;
	return io->buf & ((1 << n) - 1);
}

static inline void dumpbits(struct iostate *io, int n)
{
	io->buf >>= n;
	io->bits -= n;
}

static inline u32 pullbits(struct iostate *io, int n)
{
	u32 r = readbits(io, n);
	dumpbits(io, n);
	return r;
}

static inline void popbytes(struct iostate *io)
{
	while (io->bits >= 8) {
		io->bits -= 8;
		io->ipos--;
	}
}

/*
   Huffman code decoding is performed using a multi-level table lookup.
   The fastest way to decode is to simply build a lookup table whose
   size is determined by the longest code.  However, the time it takes
   to build this table can also be a factor if the data being decoded
   is not very long.  The most common codes are necessarily the
   shortest codes, so those codes dominate the decoding time, and hence
   the speed.  The idea is you can have a shorter table that decodes the
   shorter, more probable codes, and then point to subsidiary tables for
   the longer codes.  The time it costs to decode the longer codes is
   then traded against the time it takes to make longer tables.

   This results of this trade are in the variables lbits and dbits
   below.  lbits is the number of bits the first level table for literal/
   length codes can decode in one step, and dbits is the same thing for
   the distance codes.  Subsequent tables are also less than or equal to
   those sizes.  These values may be adjusted either when all of the
   codes are shorter than that, in which case the longest code length in
   bits is used, or when the shortest code is *longer* than the requested
   table size, in which case the length of the shortest code in bits is
   used.

   There are two different values for the two tables, since they code a
   different number of possibilities each.  The literal/length table
   codes 286 possible values, or in a flat code, a little over eight
   bits.  The distance table codes 30 possible values, or a little less
   than five bits, flat.  The optimum values for speed end up being
   about one bit more than those, so lbits is 8+1 and dbits is 5+1.
   The optimum values may differ though from machine to machine, and
   possibly even between compilers.  Your mileage may vary.
 */

static const int lbits = 9;	/* bits in base literal/length lookup table */
static const int dbits = 6;	/* bits in base distance lookup table */

/* If BMAX needs to be larger than 16, then h and x[] should be u32. */
#define BMAX 16		/* maximum bit length of any code (16 for explode) */
#define N_MAX 288	/* maximum number of codes in any set */

/*
 * huft-build - build a huffman decoding table
 * @b: code lengths in bits (all assumed <= BMAX)
 * @n: number of codes (assumed <= N_MAX)
 * @s: number of simple-valued codes (0..s-1)
 * @d: list of base values for non-simple codes
 * @e: list of extra bits for non-simple codes
 * @t: returns pointer to starting table
 * @m: maximum lookup bits, returns actual
 *
 * Given a list of code lengths and a maximum table size, make a set
 * of tables to decode that set of codes. Return zero on success, one
 * if the given code set is incomplete (the tables are still built in
 * this case), two if the input is invalid (all zero length codes or
 * an oversubscribed set of lengths), and three if not enough
 * memory.
 */
static int INIT huft_build(unsigned *b, unsigned n, unsigned s, const u16 * d,
		      const u16 * e, struct huft **t, int *m)
{
	unsigned a;		/* counter for codes of length k */
	unsigned c[BMAX + 1];	/* bit length count table */
	unsigned f;		/* i repeats in table every f entries */
	int g;			/* maximum code length */
	int h;			/* table level */
	unsigned i;	/* counter, current code */
	unsigned j;	/* counter */
	int k;		/* number of bits in current code */
	int l;			/* bits per table (returned in m) */
	unsigned *p;	/* pointer into c[], b[], or v[] */
	struct huft *q;	/* points to current table */
	struct huft r;		/* table entry for structure assignment */
	struct huft *u[BMAX];	/* table stack */
	unsigned v[N_MAX];	/* values in order of bit length */
	int w;		/* bits before this table == (l * h) */
	unsigned x[BMAX + 1];	/* bit offsets, then code stack */
	unsigned *xp;		/* pointer into x */
	int y;			/* number of dummy codes added */
	unsigned z;		/* number of entries in current table */

	DEBG("huft1 ");

	for (i = 0; i < BMAX + 1; i++)
		c[i] = 0;

	/* Generate counts for each bit length */
	p = b;
	i = n;
	do {
		c[*p]++;	/* assume all entries <= BMAX */
		p++;
	} while (--i);

	if (c[0] == n) {	/* null input--all zero length codes */
		*t = 0;
		*m = 0;
		return 2;
	}

	DEBG("huft2 ");

	/* Find minimum and maximum length, bound *m by those */
	l = *m;
	for (j = 1; j <= BMAX; j++)
		if (c[j])
			break;
	k = j; /* minimum code length */

	if ((unsigned)l < j)
		l = j;
	for (i = BMAX; i; i--)
		if (c[i])
			break;
	g = i; /* maximum code length */

	if ((unsigned)l > i)
		l = i;
	*m = l;

	DEBG("huft3 ");

	/* Adjust last length count to fill out codes, if needed */
	for (y = 1 << j; j < i; j++, y <<= 1) {
		y -= c[j];
		if (y < 0)
			return 2; /* bad input: more codes than bits */
	}

	y -= c[i];
	if (y < 0)
		return 2;
	c[i] += y;

	DEBG("huft4 ");

	/* Generate starting offsets into the value table for each length */
	x[1] = j = 0;
	p = c + 1;
	xp = x + 2;
	/* note that i == g from above */
	while (--i) {
		j += *p;
		*xp = j;
		p++;
		xp++;
	}

	DEBG("huft5 ");

	/* Make a table of values in order of bit lengths */
	p = b;
	i = 0;
	do {
		j = *p++;
		if (j)
			v[x[j]++] = i;
	} while (++i < n);

	n = x[g];                   /* set n to length of v */

	DEBG("h6 ");

	/* Generate the Huffman codes and for each, make the table entries */
	x[0] = i = 0; /* first Huffman code is zero */
	p = v; /* grab values in bit order */
	h = -1; /* no tables yet--level -1 */
	w = -l;	/* bits decoded == (l * h) */
	u[0] = NULL; /* just to keep compilers happy */
	q = NULL; /* ditto */
	z = 0; /* ditto */
	DEBG("h6a ");

	/* go through the bit lengths (k already is bits in shortest code) */
	for (; k <= g; k++) {
		DEBG("h6b ");
		a = c[k];
		while (a--) {
			DEBG("h6b1 ");
			/* i is the Huffman code of length k for value *p */
			/* make tables up to required level */
			while (k > w + l) {
				DEBG1("1 ");
				h++;
				w += l;	/* previous table always l bits */

				/* compute min size <= l bits */
				/* upper limit on table size */
				z = min(l, g - w);

				/* try a k-w bit table */
				j = k - w;
				f = 1 << j;
				if (f > a + 1) {
					/* too few codes for k-w bit table */
					DEBG1("2 ");
					/* deduct codes from patterns left */
					f -= a + 1;
					xp = c + k;
					/* try smaller tables up to z bits */
					if (j < z) {
						/* enough codes for j bits? */
						while (++j < z) {
							f <<= 1;
							if (f <= *++xp)
								break;
							/* deduct from pats */
							f -= *xp;
						}
					}
    				}

				DEBG1("3 ");
				/* table entries for j-bit table */
				z = 1 << j;

				/* allocate and link in new table */
				q = malloc((z + 1) * sizeof(struct huft));
				if (!q) {
					if (h)
						huft_free(u[0]);
					return 3;	/* not enough memory */
				}

				DEBG1("4 ");
				*t = q + 1; /* link to list for huft_free */
				t = &q->next;
				*t = NULL;
				u[h] = ++q;	/* table starts after link */

				DEBG1("5 ");
				/* connect to last table, if there is one */
				if (h) {
					/* save pattern for backing up */
					x[h] = i;
					/* bits to dump before this table */
					r.bits = (u8)l;
					/* bits in this table */
					r.extra = (u8)(16 + j);
					/* pointer to this table */
					r.next = q;
					/* (get around Turbo C bug) */
					j = i >> (w - l);
					/* connect to last table */
					u[h - 1][j] = r;
				}
				DEBG1("6 ");
			}
			DEBG("h6c ");

			/* set up table entry in r */
			r.bits = (u8)(k - w);
			if (p >= v + n)
				r.extra = 99; /* out of values--invalid code */
			else if (*p < s) {
				/* 256 is end-of-block code */
				r.extra = (u8)(*p < 256 ? 16 : 15);
				/* simple code is just the value */
				r.val = (u16)(*p);
				/* one compiler does not like *p++ */
				p++;
			} else {
				/* non-simple--look up in lists */
				r.extra = (u8)e[*p - s];
				r.val = d[*p++ - s];
			}
			DEBG("h6d ");

			/* fill code-like entries with r */
			f = 1 << (k - w);
			for (j = i >> w; j < z; j += f)
				q[j] = r;

			/* backwards increment the k-bit code i */
			for (j = 1 << (k - 1); i & j; j >>= 1)
				i ^= j;
			i ^= j;

			/* backup over finished tables */
			while ((i & ((1 << w) - 1)) != x[h]) {
				h--;	/* don't need to update q */
				w -= l;
			}
			DEBG("h6e ");
		}
		DEBG("h6f ");
	}

	DEBG("huft7 ");

	/* Return true (1) if we were given an incomplete table */
	return y && g != 1;
}

/*
 * huft_free - free a huffman table
 * @t: table to free
 *
 * Free the malloc'ed tables built by huft_build(), which makes a
 * linked list of the tables it made, with the links in a dummy first
 * entry of each table.
 */
static int INIT huft_free(struct huft *t)
{
	struct huft *q;

	/* Go through list, freeing from the malloced (t[-1]) address. */
	while (t) {
		t -= 1;
		q = t->next;
		free(t);
		t = q;
	}
	return 0;
}

/*
 * inflate_codes - decompress the codes in a deflated block
 * @io: current i/o state
 * @tl: literal/length decoder tables
 * @td: distance decoder tables
 * @bl: number of bits decoded by tl
 * @bd: number of bits decoded by td
 *
 * inflate (decompress) the codes in a deflated (compressed) block.
 * Return an error code or zero if it all goes ok.
 */
static int INIT inflate_codes(struct iostate *io, struct huft *tl, struct huft *td,
			 int bl, int bd)
{
	unsigned len, dist;
	struct huft *t;		/* pointer to table entry */

	/* inflate the coded data */
	for (;;) {		/* do until end of block */
		t = tl + readbits(io, bl);
		while (t->extra > 16) {
			if (t->extra == 99)
				return 1;
			dumpbits(io, t->bits);
			t = &t->next[readbits(io, t->extra - 16)];
		}
		dumpbits(io, t->bits);
		if (t->extra == 16) {	/* then it's a literal */
			put_byte(io, t->val);
		} else {	/* it's an EOB or a length */
			/* exit if end of block */
			if (t->extra == 15)
				break;

			/* get length of block to copy */
			len = t->val + pullbits(io, t->extra);

			/* decode distance of block to copy */
			t = td + readbits(io, bd);
			while (t->extra > 16) {
				if (t->extra == 99)
					return 1;
				dumpbits(io, t->bits);
				t = &t->next[readbits(io, t->extra - 16)];
			}
			dumpbits(io, t->bits);

			dist = t->val + pullbits(io, t->extra);
			copy_bytes(io, len, dist);
		}
	}

	return 0;
}

/* inflate_stored - "decompress" an inflated type 0 (stored) block.
 * @io: current i/o state
 */
static int INIT inflate_stored(struct iostate *io)
{
	unsigned n;		/* number of bytes in block */

	DEBG("<stor");

	/* go to byte boundary */
	dumpbits(io, io->bits & 7);

	/* get the length and its complement */
	n = pullbits(io, 16);
	if (n != (~pullbits(io, 16) & 0xffff))
		return 1;	/* error in compressed data */

	/* read and output the compressed data */
	while (n--)
		put_byte(io, readbyte(io));

	DEBG(">");
	return 0;
}


/* inflate_fixed - decompress a block with fixed Huffman codes
 * @io: current i/o state
 *
 * decompress an inflated type 1 (fixed Huffman codes) block. We
 * should either replace this with a custom decoder, or at least
 * precompute the Huffman tables.
 *
 * We use `noinline' here to prevent gcc-3.5 from using too much stack space
 */
static int noinline INIT inflate_fixed(struct iostate *io)
{
	int i;			/* temporary variable */
	struct huft *tl;	/* literal/length code table */
	struct huft *td;	/* distance code table */
	int bl;			/* lookup bits for tl */
	int bd;			/* lookup bits for td */
	unsigned l[N_MAX];	/* length list for huft_build */

	DEBG("<fix");

	/* set up literal table */
	for (i = 0; i < 144; i++)
		l[i] = 8;
	for (; i < 256; i++)
		l[i] = 9;
	for (; i < 280; i++)
		l[i] = 7;
	for (; i < N_MAX; i++)	/* make a complete, but wrong code set */
		l[i] = 8;
	bl = 7;
	if ((i = huft_build(l, N_MAX, 257, cplens, cplext, &tl, &bl)))
		return i;

	/* set up distance table */
	for (i = 0; i < 30; i++)	/* make an incomplete code set */
		l[i] = 5;
	bd = 5;
	if ((i = huft_build(l, 30, 0, cpdist, cpdext, &td, &bd)) > 1) {
		huft_free(tl);

		DEBG(">");
		return i;
	}

	/* decompress until an end-of-block code */
	if (inflate_codes(io, tl, td, bl, bd))
		return 1;

	/* free the decoding tables, return */
	huft_free(tl);
	huft_free(td);
	return 0;
}


/* inflate_dynamic - decompress a type 2 (dynamic Huffman codes) block.
 *
 * We use `noinline' here to prevent gcc-3.5 from using too much stack space
 */
static int noinline INIT inflate_dynamic(struct iostate *io)
{
	int i;			/* temporary variables */
	unsigned j;
	unsigned l;		/* last length */
	unsigned n;		/* number of lengths to get */
	struct huft *tl;	/* literal/length code table */
	struct huft *td;	/* distance code table */
	int bl;			/* lookup bits for tl */
	int bd;			/* lookup bits for td */
	unsigned nb;		/* number of bit length codes */
	unsigned nl;		/* number of literal/length codes */
	unsigned nd;		/* number of distance codes */
	unsigned ll[286 + 30];	/* literal/length and distance code lengths */

	DEBG("<dyn");

	/* read in table lengths */
	nl = 257 + pullbits(io, 5); /* number of literal/length codes */
	nd = 1 + pullbits(io, 5); /* number of distance codes */
	nb = 4 + pullbits(io, 4); /* number of bit length codes */
	if (nl > 286 || nd > 30)
		return 1;	/* bad lengths */

	DEBG("dyn1 ");

	/* read in bit-length-code lengths */
	for (j = 0; j < nb; j++)
		ll[border[j]] = pullbits(io, 3);
	for (; j < 19; j++)
		ll[border[j]] = 0;

	DEBG("dyn2 ");

	/* build decoding table for trees--single level, 7 bit lookup */
	bl = 7;
	if ((i = huft_build(ll, 19, 19, 0, 0, &tl, &bl))) {
		if (i == 1)
			huft_free(tl);
		return i;	/* incomplete code set */
	}

	DEBG("dyn3 ");

	/* read in literal and distance code lengths */
	n = nl + nd;
	i = l = 0;
	while ((unsigned)i < n) {
		td = tl + readbits(io, bl);
		dumpbits(io, td->bits);
		j = td->val;
		if (j < 16)	/* length of code in bits (0..15) */
			ll[i++] = l = j;	/* save last length in l */
		else if (j == 16) {	/* repeat last length 3 to 6 times */
			j = 3 + pullbits(io, 2);
			if ((unsigned)i + j > n)
				return 1;
			while (j--)
				ll[i++] = l;
		} else if (j == 17) {	/* 3 to 10 zero length codes */
			j = 3 + pullbits(io, 3);
			if ((unsigned)i + j > n)
				return 1;
			while (j--)
				ll[i++] = 0;
			l = 0;
		} else {	/* j == 18: 11 to 138 zero length codes */
			j = 11 + pullbits(io, 7);
			if ((unsigned)i + j > n)
				return 1;
			while (j--)
				ll[i++] = 0;
			l = 0;
		}
	}

	DEBG("dyn4 ");

	/* free decoding table for trees */
	huft_free(tl);

	DEBG("dyn5 ");

	DEBG("dyn5a ");

	/* build the decoding tables for literal/length and distance codes */
	bl = lbits;
	if ((i = huft_build(ll, nl, 257, cplens, cplext, &tl, &bl))) {
		DEBG("dyn5b ");
		if (i == 1) {
			error("incomplete literal tree");
			huft_free(tl);
		}
		return i;	/* incomplete code set */
	}
	DEBG("dyn5c ");
	bd = dbits;
	if ((i = huft_build(ll + nl, nd, 0, cpdist, cpdext, &td, &bd))) {
		DEBG("dyn5d ");
		if (i == 1) {
			error("incomplete distance tree");
			huft_free(td);
		}
		huft_free(tl);
		return i;	/* incomplete code set */
	}

	DEBG("dyn6 ");

	/* decompress until an end-of-block code */
	if (inflate_codes(io, tl, td, bl, bd))
		return 1;

	DEBG("dyn7 ");

	/* free the decoding tables, return */
	huft_free(tl);
	huft_free(td);

	DEBG(">");
	return 0;
}

/* inflate_block - decompress a deflated block
 * @io: current i/o state
 * @e: last block flag
 */
static int INIT inflate_block(struct iostate *io, int *e)
{
	unsigned t;		/* block type */

	DEBG("<blk");

	*e = pullbits(io, 1); /* read in last block bit */
	t = pullbits(io, 2); /* read in block type */

	/* inflate that block type */
	if (t == 2)
		return inflate_dynamic(io);
	if (t == 0)
		return inflate_stored(io);
	if (t == 1)
		return inflate_fixed(io);

	DEBG(">");

	/* bad block type */
	return 2;
}

/* inflate - decompress an inflated entry
 * @io: current i/o state
 */
static int INIT inflate(struct iostate *io)
{
	int e;			/* last block flag */
	int r;			/* result code */

	/* decompress until the last block */
	do {
		r = inflate_block(io, &e);
		if (r)
			return r;
	} while (!e);

	popbytes(io);
	flush_output(io);
	return 0;
}

/**********************************************************************
 *
 * The following are support routines for inflate.c
 *
 **********************************************************************/

static void INIT makecrc(void)
{
	unsigned i, j;
	u32 c = 1;

	for (i = 128; i; i >>= 1) {
		c = (c >> 1) ^ ((c & 1) ? CRCPOLY_LE : 0);
		for (j = 0; j < 256; j += 2 * i)
			crc_32_tab[i + j] = c ^ crc_32_tab[j];
	}
}

/* gzip flag byte */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ASCII text */
#define CONTINUATION 0x02 /* bit 1 set: continuation of multi-part gzip file */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define ENCRYPTED    0x20 /* bit 5 set: file is encrypted */
#define RESERVED     0xC0 /* bit 6,7:   reserved */

/*
 * gunzip - do the uncompression!
 * @ibuf: input character pool
 * @isize: size of pool
 * @fill: function to fill the input pool
 */
static int INIT gunzip(u8 *ibuf, int isize, void (*fill)(u8 *buf, int size))
{
	u8 flags;
	unsigned char magic[2];	/* magic header */
	char method;
	u32 orig_crc;
	u32 orig_len;
	int res;
	struct iostate io;

	io.window = window;
	io.osize = WSIZE;
	io.opos = io.bits = io.buf = io.ipos = io.itotal = 0;
	io.crc = 0xffffffffUL;
	io.isize = isize;
	io.ibuf = ibuf;
	io.fill = fill;

	if (fill)
		fill(ibuf, isize);

	makecrc(); /* initialize the CRC table */

	magic[0] = readbyte(&io);
	magic[1] = readbyte(&io);
	method = readbyte(&io);

	if (magic[0] != 037 || ((magic[1] != 0213) && (magic[1] != 0236))) {
		error("bad gzip magic numbers");
		return -1;
	}

	/* We only support method #8, DEFLATED */
	if (method != 8) {
		error("internal error, invalid method");
		return -1;
	}

	flags = readbyte(&io);
	if (flags & ENCRYPTED) {
		error("Input is encrypted");
		return -1;
	}
	if (flags & CONTINUATION) {
		error("Multi part input");
		return -1;
	}
	if (flags & RESERVED) {
		error("Input has invalid flags");
		return -1;
	}
	readbyte(&io); /* skip timestamp */
	readbyte(&io);
	readbyte(&io);
	readbyte(&io);

	readbyte(&io); /* Ignore extra flags */
	readbyte(&io); /* Ignore OS type */

	if (flags & EXTRA_FIELD) {
		unsigned len = readbyte(&io);
		len |= readbyte(&io) << 8;
		while (len--)
			readbyte(&io);
	}

	/* Discard original file name if it was truncated */
	if (flags & ORIG_NAME)
		while (readbyte(&io))
			;

	/* Discard file comment if any */
	if (flags & COMMENT)
		while (readbyte(&io))
			;

	/* Decompress */
	if ((res = inflate(&io))) {
		switch (res) {
		case 0:
			break;
		case 1:
			error("invalid compressed format (err=1)");
			break;
		case 2:
			error("invalid compressed format (err=2)");
			break;
		case 3:
			error("out of memory");
			break;
		case 4:
			error("out of input data");
			break;
		default:
			error("invalid compressed format (other)");
		}
		return -1;
	}

	/* Get the crc and original length
	 * uncompressed input size modulo 2^32
	 */
	orig_crc = readbyte(&io);
	orig_crc |= readbyte(&io) << 8;
	orig_crc |= readbyte(&io) << 16;
	orig_crc |= readbyte(&io) << 24;

	orig_len = readbyte(&io);
	orig_len |= readbyte(&io) << 8;
	orig_len |= readbyte(&io) << 16;
	orig_len |= readbyte(&io) << 24;

	/* Validate decompression */
	if (orig_crc != ~io.crc) {
		error("crc error");
		return -1;
	}
	if (orig_len != bytes_out) {
		error("length error");
		return -1;
	}

	return io.itotal + io.ipos;
}
