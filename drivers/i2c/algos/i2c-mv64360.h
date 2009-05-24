/******************************************************************************
**	FileName	:	i2c-mv64360.h
**	Author		:	D K Raghunandan
**	Date		:	11 Mar 2003
**	Notes		:	I2C MV64360 Discovery defines.
**			Copyright ( c ) 2003 Force Computers India Pvt Ltd
*******************************************************************************
**	$Author		:$
**	$Date		:$
******************************************************************************/
#ifndef	I2C_MV64360_H
#define I2C_MV64360_H

/******************************************************************************
**	MV64360 I2C Registers
******************************************************************************/

#define	I2C_SLAVE_ADDR			0xC000
#define I2C_EXT_SLAVE_ADDR		0xC040
#define I2C_DATA			0xC004
#define I2C_CONTROL			0xC008
#define I2C_STATUS_BAUD_RATE		0xC00C
#define I2C_SOFT_RESET			0xC01C

/* Control Reg Bit defines */
#define ACK				( 1 << 2 )
#define IFLG				( 1 << 3 )
#define STO				( 1 << 4 )
#define STA				( 1 << 5 )
#define I2CEN				( 1 << 6 )

/* M and N Values for SYSCLK 100 MHz */
#define M                               0x2
#define N                               0x7


#define GT_I2C_ADDRESS			0x08
#define WRITE_BIT			0x0
#define READ_BIT			0x1

#define	I2C_DELAY			2000 


/* I2C Status Codes */

#define I2CBUS_ERROR						0x00
#define I2C_START_TRA						0x08
#define I2C_REP_START_TRA					0x08
#define ADDR_WB_TRA_ACK						0x18
#define ADDR_WB_TRA_NACK					0x20
#define MASTER_DATA_TRA_ACK					0x28
#define MASTER_DATA_TRA_NACK					0x30 
#define MASTER_LOST_ARBTR_DURING_ADR_DATA			0x38
#define ADDR_RB_TRA_ACK						0x40
#define ADDR_RB_TRA_NACK					0x48
#define MASTER_READ_ACK_TRA					0x50
#define MASTER_READ_NACK_TRA					0x58
#define SLAVE_RECD_ADDR_ACK					0x60
#define MASTER_LOST_ARBTR_DURING_ADR_TRA_SLAVEW_ACK_TRA		0x68
#define GEN_CALL_RECD_ACK_TRA					0x70
#define MASTER_LOST_ARBTR_DURING_ADR_TRA_GCA_RECD_ACK_TRA	0x78
#define SLAVE_RECD_WRT_DATA_AFTER_ADR_ACK_TRA			0x80
#define SLAVE_RECD_WRT_DATA_AFTER_ADR_ACK_NTRA			0x88
#define SLAVE_RECD_WRT_DATA_AFTER_GC_ACK_TRA			0x90
#define SLAVE_RECD_WRT_DATA_AFTER_GC_ACK_NTRA			0x98
#define SLAVE_RECD_STOP_REP_START				0xA0
#define SLAVE_RECD_ADDR_RB_ACK_TRA				0xA8
#define MASTER_LOST_ARBTR_DURING_ADR_TRA_SLAVER_ACK_TRA		0xB0
#define SLAVE_TRA_READ_DATA_ACK_RECD				0xB8
#define SLAVE_TRA_READ_DATA_ACK_NOT_RECD			0xC0
#define	SLAVE_TRA_LOST_READ_BYTE_ACK_RECD			0xC8

/* Type defines */
typedef unsigned int bool ;

/* Prototypes */
bool i2c_word_write( u_char, uint,unsigned int, u_char *);
bool i2c_word_read( u_char, uint, unsigned int, u_char *);

bool i2c_byte_write( u_char, u_char,unsigned int, u_char *);
bool i2c_byte_read( u_char, u_char, unsigned int, u_char *);

bool i2c_write( u_char,u_char*, unsigned int);
bool i2c_read( u_char, u_char* , unsigned int );

bool i2c_dummy_word_write( u_char , uint);
bool i2c_dummy_byte_write(u_char , unsigned int);

void i2c_rtc_read( u_char * ) ;
void i2c_rtc_write( u_char * ) ;

#endif
/* I2C_MV64360_H */

