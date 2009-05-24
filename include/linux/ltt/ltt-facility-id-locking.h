#ifndef _LTT_FACILITY_ID_LOCKING_H_
#define _LTT_FACILITY_ID_LOCKING_H_

#ifdef CONFIG_LTT
#include <linux/ltt-facilities.h>

/****  facility handle  ****/

extern ltt_facility_t ltt_facility_locking_51595CB2;
extern ltt_facility_t ltt_facility_locking;


/****  event index  ****/

enum locking_event {
	event_locking_spin_lock_wait,
	event_locking_spin_lock_try,
	event_locking_spin_lock_entry,
	event_locking_spin_lock_exit,
	event_locking_irq_save,
	event_locking_irq_restore,
	event_locking_irq_disable,
	event_locking_irq_enable,
	facility_locking_num_events
};

#endif //CONFIG_LTT
#endif //_LTT_FACILITY_ID_LOCKING_H_
