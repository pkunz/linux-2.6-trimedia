#ifndef __NORMAL_DELAY_H
#define __NORMAL_DELAY_H

extern void __delay(int loops);
extern void udelay(unsigned long usecs);

extern void ndelay(unsigned long nsecs);
#define ndelay ndelay

#endif /* defined(__NORMAL_DELAY_H) */

