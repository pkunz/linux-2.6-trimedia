#ifndef CT_SWIC_H_
#define CT_SWIC_H_

/* define this if you disabled in ctdriver.c */
#ifdef CONFIG_ARM
#define CT_SWIC_BUFFER_NOCACHE
#endif

typedef struct Q_Tag {
  u32 tag ;
  u32 a_time ;
} Q_Tag ;

#define HOOK_BUFFER_SIZE  2048  /* must be a power of two */

struct Hook_Buffer {
  int head ;
  volatile int tail ;
  Q_Tag buffer[HOOK_BUFFER_SIZE] ;
} ;

#endif
