/*
 *
 */
#include <linux/kernel.h>
#include <linux/errno.h>

#include <tmbslCore.h>

extern compActivateFunc_t tmbslMgrBoardActivateOfBoardId0x00081131;

extern int
tmboard_activate(void)
{
	tmErrorCode_t status=TM_OK;

	status=(*tmbslMgrBoardActivateOfBoardId0x00081131)(NULL);
	if(status!=TM_OK){
		return -EFAULT;
	}

	return 0;
}
