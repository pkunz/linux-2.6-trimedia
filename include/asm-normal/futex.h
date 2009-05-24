#ifndef _ASM_NORMAL_FUTEX_H
#define _ASM_NORMAL_FUTEX_H

#ifdef __KERNEL__

#include <linux/futex.h>

extern int futex_atomic_op_inuser (int encoded_op, int __user *uaddr);

#endif
#endif /*_ASM_NORMAL_FUTEX_H */

