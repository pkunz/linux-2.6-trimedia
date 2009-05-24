#ifndef _UM_ASM_FUTEX_H
#define _UM_ASM_FUTEX_H

#if 0 /* 2.6.18 */
#include <asm-generic/futex.h>
#else
#include <linux/futex.h>
#include <asm/errno.h>
#include <asm/system.h>
#include <asm/processor.h>
#include <asm/uaccess.h>

#include "asm/arch/futex.h"
#endif


#endif
