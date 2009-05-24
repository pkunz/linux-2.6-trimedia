/******************************************************************************
**	FileNam	:	i2c-algo-mv64360.h
**	Author	:	D K Raghunandan
**	Date	:	11 Mar 2003
**	Notes	:	I2C driver algorithms for MV64360 Discovery Adapter.
**		Copyright ( c ) 2003
*******************************************************************************
**	$Author		:$
**	$Date		:$
******************************************************************************/
#ifndef	I2C_ALGO_MV64360_H
#define I2C_ALGO_MV64360_H

#include <linux/i2c.h>

struct i2c_algo_mv64360_data {
	void *data;

	/* local settings */
	int udelay;
	int mdelay;
	int timeout;
	unsigned int offset ;
};

int i2c_mv64360_add_bus( struct i2c_adapter * );
int i2c_mv64360_del_bus( struct i2c_adapter * );

#endif
/* I2C_ALGO_MV64360_H */
