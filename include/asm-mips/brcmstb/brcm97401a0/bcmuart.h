/***********************************************************************/
/*                                                                     */
/*   MODULE:  bcmuart.h                                                */
/*   DATE:    02/15/02                                                 */
/*   PURPOSE: Definitions for UART block                               */
/*                                                                     */
/***********************************************************************/
#ifndef _BCMUART_H
#define _BCMUART_H

#include "bcmmips.h"
#include "boardcfg.h"
#include "bchp_common.h"
//#include "bchp_uarta.h"
#include "bchp_uartb.h"
#include "bchp_uartc.h"

#if !defined _ASMLANGUAGE
#ifdef __cplusplus
extern "C" {
#endif
#endif

/* UART register base addresses */
//#define UARTA_ADR_BASE   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_UARTA_RCVSTAT)
#define UARTA_ADR_BASE   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_UARTB_RCVSTAT)
#define UARTB_ADR_BASE   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_UARTB_RCVSTAT)
#define UARTC_ADR_BASE   BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_UARTC_RCVSTAT)

/* UART registers */
#define UART_RXSTAT     	0x00
#define UART_RXDATA     	0x04
#define UART_CONTROL    	0x0c
#define UART_BAUDHI     	0x10
#define UART_BAUDLO     	0x14
#define UART_TXSTAT     	0x18
#define UART_TXDATA     	0x1c

/* RXSTAT bit definitions */
#define PARERR				0x20
#define FRAMEERR			0x10
#define OVERRUNERR			0x08
#define RXDATARDY			0x04
#define RXINTEN				0x02
/* CONTROL bit definitions */
#define BITM8				0x10
#define	PAREN				0x08
#define	TXEN				0x04
#define	RXEN				0x02
#define	PODD				0x01
/* TXSTAT bit definitions */
#define	TXINTEN				0x04
#define	TXIDLE				0x02
#define	TXDREGEMT			0x01


#if !defined _ASMLANGUAGE

/**********************************************************************
  Uart Register Structure
 **********************************************************************/
typedef struct UartChannel {
  volatile unsigned long rxstat;
  volatile unsigned long rxdata;
  volatile unsigned long unused;
  volatile unsigned long control;
  volatile unsigned long baudh;
  /* When divide SysClk/2/(1+baudword) we should get 32*bit-rate
   */
  volatile unsigned long baudl;
  volatile unsigned long txstat;
  volatile unsigned long txdata;
} UartChannel;

#define UARTA ((volatile UartChannel *) UARTA_ADR_BASE)
#define UARTB ((volatile UartChannel *) UARTB_ADR_BASE)



#endif /* _ASMLANGUAGE */

/******************************************************************
 * Baud Rate Table
 * XTALFREQ / baud rate / 16
 ******************************************************************/
#define BAUD_VAL(x)     (((XTALFREQ/8/(x) + 1)/2) - 1)
#define BAUD_VAL_HI(x)  ((BAUD_VAL(x) >> 8) & 0xff)
#define BAUD_VAL_LO(x)  (BAUD_VAL(x) & 0xff)

#define BAUD_1200_HI    BAUD_VAL_HI(1200)
#define BAUD_1200_LO    BAUD_VAL_LO(1200)
#define BAUD_2400_HI    BAUD_VAL_HI(2400)
#define BAUD_2400_LO    BAUD_VAL_LO(2400)
#define BAUD_4800_HI    BAUD_VAL_HI(4800)
#define BAUD_4800_LO    BAUD_VAL_LO(4800)
#define BAUD_9600_HI    BAUD_VAL_HI(9600)
#define BAUD_9600_LO    BAUD_VAL_LO(9600)
#define BAUD_19200_HI   BAUD_VAL_HI(19200)
#define BAUD_19200_LO   BAUD_VAL_LO(19200)
#define BAUD_38400_HI   BAUD_VAL_HI(38400)
#define BAUD_38400_LO   BAUD_VAL_LO(38400)
#define BAUD_57600_HI   BAUD_VAL_HI(57600)
#define BAUD_57600_LO   BAUD_VAL_LO(57600)
#define BAUD_115200_HI  BAUD_VAL_HI(115200)
#define BAUD_115200_LO  BAUD_VAL_LO(115200)



#if !defined _ASMLANGUAGE
#ifdef __cplusplus
}
#endif
#endif

#endif  /* _BCMUART_H */
