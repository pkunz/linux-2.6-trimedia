#include <linux/compiler-tmcc.h>

# define __builtin_expect(x, expected_value) 		(x)

#define __attribute_used__

/*
 * The attribute `pure' is not implemented in GCC versions earlier
 * than 2.96.
 */
# define __attribute_pure__	
# define __attribute_const__	

/* GCC 2.95.x/2.96 recognize __va_copy, but not va_copy. Actually later GCC's
 * define both va_copy and __va_copy, but the latter may go away, so limit this
 * to this header */
#define va_copy

