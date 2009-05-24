#ifndef _LTT_FACILITY_LOADER_STACK_ARCH_H_
#define _LTT_FACILITY_LOADER_STACK_ARCH_H_

#ifdef CONFIG_LTT

#include <linux/ltt-facilities.h>
#include <asm/ltt/ltt-facility-id-stack_arch_i386.h>

ltt_facility_t	ltt_facility_stack_arch;
ltt_facility_t	ltt_facility_stack_arch_C67264DD;

#define LTT_FACILITY_SYMBOL		ltt_facility_stack_arch
#define LTT_FACILITY_CHECKSUM_SYMBOL	ltt_facility_stack_arch_C67264DD
#define LTT_FACILITY_CHECKSUM		0xC67264DD
#define LTT_FACILITY_NAME		"stack_arch"
#define LTT_FACILITY_NUM_EVENTS	facility_stack_arch_num_events

#endif //CONFIG_LTT

#endif //_LTT_FACILITY_LOADER_STACK_ARCH_H_
