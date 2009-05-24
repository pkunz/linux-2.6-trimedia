
#ifndef __ASM_OFFSETS 
#include <ops/custom_ops.h>
#endif 

#define barrier() 

#define __inline      inline 

#define __builtin_return_address(x)   RDREG(2)

#define __attribute__(x)
#define __attribute(x)
