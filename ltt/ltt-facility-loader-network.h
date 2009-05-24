#ifndef _LTT_FACILITY_LOADER_NETWORK_H_
#define _LTT_FACILITY_LOADER_NETWORK_H_

#ifdef CONFIG_LTT

#include <linux/ltt-facilities.h>
#include <linux/ltt/ltt-facility-id-network.h>

ltt_facility_t	ltt_facility_network;
ltt_facility_t	ltt_facility_network_8B91EC6C;

#define LTT_FACILITY_SYMBOL		ltt_facility_network
#define LTT_FACILITY_CHECKSUM_SYMBOL	ltt_facility_network_8B91EC6C
#define LTT_FACILITY_CHECKSUM		0x8B91EC6C
#define LTT_FACILITY_NAME		"network"
#define LTT_FACILITY_NUM_EVENTS	facility_network_num_events

#endif //CONFIG_LTT

#endif //_LTT_FACILITY_LOADER_NETWORK_H_
