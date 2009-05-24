#include <asm/hardware/arm_timer.h>
#include <asm/mach/map.h>
#include <asm/mach/mmc.h>
#include <asm/system.h>
/*
 * Declaring macros specific to the pl031 hardware
 */
#define OFFSET_DATA_REGISTER 0x000
#define OFFSET_MATCH_REGISTER 0x004
#define OFFSET_LOAD_REGISTER 0x008
#define OFFSET_CONTROL_REGISTER 0x00c
#define OFFSET_INTERRUPT_MASK_S_R_REGISTER 0x010
#define OFFSET_RAW_INTERRUPT_STATUS_REGISTER 0x014
#define OFFSET_MASKED_INTERRUPT_STATUS_REGISTER 0x018
#define OFFSET_INTERRUPT_CLEAR_REGISTER 0x01c

#define PL031_INTERRUPT_MASK_BIT 0
#define PL031_INTERRUPT_UNMASK_BIT 1
#define PL031_INTERRUPT_CLEAR_BIT 1
#define PL031_START_COUNT 1
#define PL031_IRQ 10
/*
 * pl031 misc driver with minor 137
 */
#define PL031_MINOR 137

/*
 *Structure to store the result for the various registers.
 */
struct pl031_rtc {
	unsigned long rtcdr;	/* data register */
	unsigned long rtcmr;	/* match register */
	unsigned long rtclr;	/* load register */
	unsigned long rtccr;	/* control register */
	unsigned long rtcimsc;	/* Interrupt mask set register */
	unsigned long rtcrisr;	/* raw interrupt status register */
};
