/*******************************************************************************
**	FileName:	i2c-algo-mv64360.c
**	Author	:	D K Raghunandan Copyright ( c ) 2003
**	Date	:	11 Mar 2003
**	Notes	:	I2C driver algorithms for MV64360 Discovery Adapter.
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.

**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
********************************************************************************
**	$Author	:$
**	$Date	:$
*******************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/types.h>

#include <asm/mv64x60.h>
#include <asm/ppc_asm.h>
#include <linux/i2c.h>
#include <linux/i2c-algo-mv64360.h>

#include "i2c-mv64360.h"

/*******************************************************************************
**		Defines
*******************************************************************************/
#undef	DEBUG

#ifdef	DEBUG
#define	DBG( string, args... ) printk(KERN_INFO "i2c-algo:" string, ##args )
#else
#define	DBG( string, args... )
#endif

#define	true	1 
#define	false	0 

extern struct mv64x60_handle bh;

#define gt_read(reg)	mv64x60_read(&bh, reg)
#define gt_write(reg,value)	mv64x60_write(&bh, reg, value)

uint eeprom_type;

/*******************************************************************************
**		Prototype Functions
*******************************************************************************/
bool	isIntFlagSet( void );
void	clearIntFlag( void );
int	get_eeprom_size( u_char );


/******************************************************************************
**		Utility Functions
******************************************************************************/
/******************************************************************************
**  Function	:	get_eeprom_size
**  Description	:	Detects dynamically the size of the EEPROM using a
**			simple technique of using the 16-bit EEPROMWrite 
**			and the 8-bitEEPROMWrite routines.
**  Inputs	:	I2C Address
**  Output	:	EEPROM TYPE                                                       
******************************************************************************/
int get_eeprom_size( u_char devadd )
{
	u_char dat,val;
	u_char buffer[2];     
	int eeprom_size = -1;

	dat = 0xdd;
	buffer[0] = dat;
	
	DBG("Detecting EEPROM device ......\n" ) ;

	if( i2c_word_write( devadd, 0x1230, 1, buffer )){
		if( i2c_byte_read( devadd, 0x12, 2, buffer ))
		{
			val = buffer[1];
		
			if( val == dat)
				eeprom_size = 2;
			else
				eeprom_size = 64; 
		} 
		else
			DBG("i2c_byte_read failed. \n" ) ;
	}
	else
		DBG("\nUnable to detect EEPROM Device\n");
	return( eeprom_size );
}

/******************************************************************************
**  Function	:     clearIntFlag
******************************************************************************/
void clearIntFlag( )
{
	uint temp;

	temp = gt_read( I2C_CONTROL );
	gt_write( I2C_CONTROL, temp & 0xfffffff7 );
}

/******************************************************************************
**  Function	:	isIntFlagSet.
**
**  Description	:	Checks wether the interrupt flag bit of the 
**			Control Register is set or not.
** Inputs	:	None.
**
** Outputs	:	True if set else false
******************************************************************************/
bool isIntFlagSet()
{
	uint temp;

	DBG("Waiting for the Interrupt flag to be set .....\n");

	temp = gt_read( I2C_CONTROL );

	if(( temp & 0x8 ) == 0x8 )
		return true;
	else
	return false;
}

/******************************************************************************
** Function     : i2c_byte_write                                               
** Description  : This routine does page write in to AT24C02A serial EEPROM at 
**                a specified offset                                           
** Inputs       : devAdd      - EEPROM hardware address                        
**                writeoffset - page offset                                    
**                noBytes     - Number of bytes to be written                  
**                regFile1    - Array of data to be written                    
** Outputs      : true  - If successful                                        
**                false - If not successful                                    
******************************************************************************/ 
bool i2c_byte_write( u_char devAdd, u_char writeoffset, 
		     uint noBytes,  u_char *regFile1 ) 
{
	u_char  regFile2[9];
	uint    i = 0, j = 0;
	bool    ret = true;

	if(( noBytes < 0 ) || ( noBytes > 256 )){
		DBG("Only 0-256 Bytes can be written.\n");
		return( false );
	}	

	gt_write( I2C_SOFT_RESET, 0x0 );
	udelay( I2C_DELAY );

	for( i = 0, j = 1; i < noBytes; i++ ){

		regFile2[j]=regFile1[i];

		if(( j == 8 - ( writeoffset % 8 )) || 
		   ( i == ( noBytes - 1 ))){

			udelay( I2C_DELAY ); 

			regFile2[0] = writeoffset;
			gt_write( I2C_SOFT_RESET, 0x0 );

			udelay( I2C_DELAY );   

			if( i2c_write ( devAdd, regFile2, j + 1 )){
				DBG("Burning of a Page over\n");
			}
			else{
				DBG("Page burning Failed...\n");
				ret = false;
				break;
			}

			j = 0;
			writeoffset = writeoffset + 8;
			/* To go to the next page*/
			writeoffset = writeoffset & 0xfffffff8; 
		}

		j++;
	}/* End for ... */
	return( ret );
}

/******************************************************************************
** Function     : i2c_word_write                                               
** Description  : This routine does page write in to AT24C02A 
**		  serial EEPROM at a specified offset                                           
** Inputs       : devAdd      - EEPROM hardware address                        
**                writeoffset - page offset                                    
**                noBytes     - Number of bytes to be written                  
**                regFile1    - Array of data to be written                    
** Outputs      : true  - If successful                                        
**                false - If not successful                                    
******************************************************************************/
bool i2c_word_write( u_char devAdd, uint writeoffset, 
		     uint noBytes,  u_char *regFile1 )
{
	u_char	regFile2[34];
	uint	i = 0, j = 0;
	bool	ret = true;

	/* 13-bit address for 64k EEPROM*/ 
	writeoffset = writeoffset & 0x1fff;      
                             
	if(( noBytes < 0 ) || ( noBytes > 8192 )){
		DBG("Only 0-8192 Bytes can be written.\n");
		return( false );
	}	

	gt_write( I2C_SOFT_RESET, 0x0 );

	udelay( I2C_DELAY );

	for( i = 0,j = 2; i < noBytes; i++ ){

		regFile2[j]=regFile1[i];
		if(( j == ( 33 - ( writeoffset % 32 ))) || 
		   ( i == (noBytes-1))){

			udelay( I2C_DELAY );

			regFile2[0] = ( writeoffset & 0xff00 ) >> 8;
			regFile2[1] = writeoffset & 0x00ff;

			gt_write( I2C_SOFT_RESET, 0x0 );
			udelay( I2C_DELAY );   

			if( i2c_write( devAdd, regFile2, j + 1 )){ 
				DBG("Burning of a Page over\n");
			}
			else {
				DBG("Page burning Failed...\n");
				ret = false;
				break;
			}
			
			j = 1;
			writeoffset = writeoffset + 0x0020;                  
			writeoffset = writeoffset & 0xffffffe0;
		}
	
		j++;
	}
	return( ret );
}


/******************************************************************************
** Function    : i2c_write                                                   
**                                                                           
** Description : This is the lowest level driver to write 
**		 to a slave I2C device
** Inputs      : devAdd  - Slave device hardware address.                      
**               regFile - Array of data to be sent to the slave device        
**               noBytes - Number of bytes to be sent.(present in the array)   
** Output      : true  - if successful                                         
**               false - if not successful                                     
******************************************************************************/
bool i2c_write( u_char devAdd, u_char* regFile, uint noBytes )
{
	uint i2cstatus, temp = 0x0, count = 0, data = 0;
	bool ret= false;

	udelay( I2C_DELAY );
	data = (( M << 3 ) | N ) ;
	gt_write( I2C_STATUS_BAUD_RATE, data ); 
                     
	gt_write( I2C_CONTROL, 0x60 );           
	while( !isIntFlagSet() );                   

	i2cstatus = gt_read( I2C_STATUS_BAUD_RATE );

	if(( i2cstatus == 0x08) || ( i2cstatus == 0x10 )){

		udelay( I2C_DELAY );
		gt_write( I2C_DATA, devAdd | 0x00 ); 
		clearIntFlag();

		while( !isIntFlagSet() );

		i2cstatus = gt_read( I2C_STATUS_BAUD_RATE );
        
		switch( i2cstatus ){
			case I2CBUS_ERROR:
				DBG("Bus Error!!!\n");
				temp = gt_read( I2C_CONTROL );
				gt_write( I2C_CONTROL,temp | 0x10 );
				udelay( I2C_DELAY );
				clearIntFlag();
				return ret;

			case ADDR_WB_TRA_ACK:
				udelay( I2C_DELAY );

				/***** Data Write ******/
				for ( count=0; count < noBytes; count++ ){
					gt_write( I2C_DATA, regFile[count] );
					clearIntFlag();
					while( !isIntFlagSet() );

					i2cstatus = gt_read( I2C_STATUS_BAUD_RATE );
					switch( i2cstatus ){
						case MASTER_DATA_TRA_ACK:
							udelay( I2C_DELAY );
							ret= true;
							break;

						case MASTER_DATA_TRA_NACK:
							DBG("MASTER_DATA_TRA_NACK\n" );
							ret = false;
							break;

						case MASTER_LOST_ARBTR_DURING_ADR_DATA:
							DBG("MASTER_LOST_ARBTR_DURING_ADR_DATA\n" );
							ret = false;
							break;

						default:
							DBG("Error, I2C status %x\n",i2cstatus);
							ret = false;
							break;
					}
					if ( ret == false )
					break;
			} /* End for ... */

			udelay( I2C_DELAY );
			temp = gt_read( I2C_CONTROL );
			gt_write(I2C_CONTROL, temp | 0x10);
			clearIntFlag();
			return ret;

			case ADDR_WB_TRA_NACK:
				temp = gt_read( I2C_CONTROL );
				gt_write(I2C_CONTROL, temp | 0x10);
				clearIntFlag();
				gt_write(I2C_CONTROL,0x0);
				return ret;
		}/* End Switch... */
	}
	else{
		DBG("I2C Start Condition Fails!!!\nI2C Status = %x\n",i2cstatus);
		temp = gt_read( I2C_CONTROL );
		gt_write( I2C_CONTROL, temp | 0x10 );
		clearIntFlag();
		gt_write( I2C_CONTROL, 0x00 );
		return ret;
	}
	return ret;	    
}

/******************************************************************************
** Function    : i2c_dummy_byte_write
** Description : This routine does "Dummy write" to the EEPROM device
**               specifically, for random read and sequencial read  
** Inputs      : devAdd  - EEPROM device hardware address
**               roffset - Offset from where the next read command would 
**                        like to read from.
** Output      : true - If successful
**               false - If not successful in issuing the "dummy write"
******************************************************************************/
bool i2c_dummy_byte_write( u_char devAdd, unsigned int roffset )
{
	uint i2cStatus, TempData = 0x0;
	bool retval= false;

	gt_write( I2C_STATUS_BAUD_RATE, 0x2c ); 

	/* Start bit & enable bit*/
	gt_write( I2C_CONTROL, 0x60 ); 
	while( !isIntFlagSet() );

	i2cStatus = gt_read( I2C_STATUS_BAUD_RATE );
    
	if( i2cStatus==0x08 ){
		gt_write( I2C_DATA, devAdd | 0x00); 
		clearIntFlag();

		while( !isIntFlagSet() );

		i2cStatus = gt_read( I2C_STATUS_BAUD_RATE );
        
		switch( i2cStatus ){
			case I2CBUS_ERROR:
				DBG("Bus error!!!\n");
				TempData = gt_read( I2C_CONTROL );
				gt_write(I2C_CONTROL,TempData & 0x10 );/*generate stop bit*/
				clearIntFlag();
				return retval;

			case ADDR_WB_TRA_ACK:
				gt_write(I2C_DATA,roffset); /* Word Address */
				clearIntFlag();
				while(!isIntFlagSet());
				i2cStatus = gt_read( I2C_STATUS_BAUD_RATE );

				switch( i2cStatus ){
					case MASTER_DATA_TRA_ACK:
						retval= true;
						break;
					case MASTER_DATA_TRA_NACK:
						DBG("MASTER_DATA_TRA_NACK\n" );
						retval = false;
						break;
					case MASTER_LOST_ARBTR_DURING_ADR_DATA:
						DBG("MASTER_LOST_ARBTR_DURING_ADR_DATA\n");
						retval = false;
						break;
				}
				return retval;
            
			case ADDR_WB_TRA_NACK:
				gt_write( I2C_CONTROL, TempData & 0x10 );
				clearIntFlag();
				udelay( I2C_DELAY );
				gt_write( I2C_CONTROL, 0x0 );
				return retval;
		}
	}
	else
	{
		DBG("I2C Re-Start Condition Fails.....\n");
		gt_write(I2C_CONTROL,TempData & 0x10);	/* generate stop bit */
		udelay( I2C_DELAY );
		gt_write(I2C_CONTROL,0x0);
		return retval;
	}
    return retval;
}


/******************************************************************************
** Function    :  i2c_dummy_word_write
** Description : This routine does "Dummy write" to the EEPROM device
**               specifically, for random read and sequencial read  
** Inputs      : devAdd  - EEPROM device hardware address
**               roffset - Offset from where the next read command would 
**                        like to read from.
** Output      : true - If successful
**               false - If not successful in issuing the "dummy write"
******************************************************************************/
bool i2c_dummy_word_write( u_char devAdd, uint roffset )
{
	uint i2cStatus, TempData = 0x0;
	bool retval= false;
	uint roffset1 = roffset;

	gt_write( I2C_STATUS_BAUD_RATE, 0x2c );

	/* Start bit & enable bit*/
	gt_write( I2C_CONTROL, 0x60 ); 

	while( !isIntFlagSet() );

	i2cStatus = gt_read( I2C_STATUS_BAUD_RATE );

	if( i2cStatus == 0x08 ){
		DBG("I2C Dummy Start Condition is succuessful\n");
		DBG("Writes the Dummy Slave Address as :%x\n",devAdd);

		gt_write(I2C_DATA,devAdd | 0x00); 
		clearIntFlag();

		while( !isIntFlagSet() );

		i2cStatus = gt_read( I2C_STATUS_BAUD_RATE );

		switch( i2cStatus )
		{
			case I2CBUS_ERROR:
				DBG("Bus error!!!\n");
				TempData = gt_read( I2C_CONTROL );
				gt_write( I2C_CONTROL, TempData & 0x10);
				clearIntFlag( );
				return retval;

			case ADDR_WB_TRA_ACK:
				DBG("The Offset : %x is being written\n",roffset);
				roffset =( roffset & 0xff00 ) >> 8;
				gt_write( I2C_DATA, roffset ); 
				clearIntFlag();

				while( !isIntFlagSet() );

				i2cStatus = gt_read( I2C_STATUS_BAUD_RATE );

			switch( i2cStatus )
			{
				case MASTER_DATA_TRA_ACK:
					roffset1 = roffset1 & 0x00ff;
					gt_write( I2C_DATA, roffset1 );

					clearIntFlag();
					while( !isIntFlagSet() );
					i2cStatus = gt_read( I2C_STATUS_BAUD_RATE );

					switch( i2cStatus )
					{
					       case MASTER_DATA_TRA_ACK:
						    retval = true;
						    break;  
					       case MASTER_DATA_TRA_NACK:
						    retval=false;
						    break;
					       case MASTER_LOST_ARBTR_DURING_ADR_DATA:
						    retval=false;
						    break; 
                    			}
					return retval;
				   case MASTER_DATA_TRA_NACK:
					retval = false;
					break;
				   case MASTER_LOST_ARBTR_DURING_ADR_DATA:
					retval = false;
					break;
			}
			return retval;
			case ADDR_WB_TRA_NACK:
				DBG("ADDR_WB_TRA_NACK\n");
				/*generate stop bit*/
				gt_write(I2C_CONTROL,TempData & 0x10);
				clearIntFlag();
				udelay( I2C_DELAY );
				gt_write(I2C_CONTROL,0x0);
			return retval;
		}
	}
	else{
		DBG("I2C Re-Start Condition Fails.....\n");
		/* generate stop bit */
		gt_write(I2C_CONTROL,TempData & 0x10);	
		udelay( I2C_DELAY );
		gt_write(I2C_CONTROL,0x0);
		return retval;
	}
    return retval;
}

/******************************************************************************
**				READ Functions
******************************************************************************/
/******************************************************************************
** Function      :  i2c_byte_read                                             
** Description   : This routine reads from AT24C02A serial EEPROM at          
**                 a specified offset                                         
** Inputs        : devAdd     - EEPROM hardware device address                
**                 readoffset - page offset                                   
**                noBytes    - Number of bytes to be read                     
**                 regFile    - Array where the read data has to be stored     
** Outputs       : true  - If successful                                       
**                 false - If not successful                                   
******************************************************************************/ 
bool i2c_byte_read( u_char devAdd, u_char readoffset,
		    uint noBytes,  u_char *regFile )
{ 
	bool ret;

	if(( noBytes < 0 ) || ( noBytes > 256 )){
		DBG("Only 0-256 Bytes can be read.\n");
		return( false );
	}

	udelay( I2C_DELAY * 2 );
	gt_write( I2C_SOFT_RESET, 0x0 ); 
	udelay( I2C_DELAY );

	if( i2c_dummy_byte_write( devAdd, readoffset )){
		DBG("Dummy write successful\n");
		if( i2c_read( devAdd, regFile, noBytes))
			ret = true;
		else
			ret = false;
	}
	else{ 
		DBG("Dummy write NOT successful!!!\n");
		ret = false;
	}
	return( ret );
}	

/******************************************************************************
** Function      :  i2c_word_read                                              
** Description   : This routine reads from AT24C02A serial EEPROM at           
**                 a specified offset                                          
** Inputs        : devAdd     - EEPROM hardware device address                 
**                 readoffset - page offset                                    
**                 noBytes    - Number of bytes to be read                     
**                 regFile    - Array where the read data has to be stored     
** Outputs       : true  - If successful                                       
**                 false - If not successful                                   
******************************************************************************/ 
bool i2c_word_read( u_char devAdd, uint readoffset,
		    uint noBytes,  u_char *regFile )
{ 
	bool ret;
	/* 64k EEPROM requires 13-bit addrress */
	readoffset = readoffset & 0x1fff;   

	if(( noBytes < 0 ) || ( noBytes > 8192 )){
		DBG("Only 0-8192 Bytes can be read.\n");
		return( false );
	}	
	udelay( I2C_DELAY * 2 );
	gt_write( I2C_SOFT_RESET, 0x0 ); 
	udelay( I2C_DELAY );

	if( i2c_dummy_word_write( devAdd, readoffset )){
		DBG("Dummy write successful\n");
		if( i2c_read( devAdd, regFile, noBytes ))
			ret = true;
		else
			ret = false;
	}
	else{ 
		DBG("Dummy write NOT successful!!!\n");
		ret = false;
	}
	return( ret );
}	
	
/******************************************************************************
** Function    : i2c_read 
**
** Description : This is the lowest driver to read from the slave I2C device.
** Inputs      : devAdd  - Slave I2c Device hardware address
**               regFile - Array to store read data from slave
**               index   - Number of bytes to read from slave
** Output      : true  - If successful   
**               false - If no successful
******************************************************************************/
bool i2c_read( u_char devAdd, u_char* regFile, uint index )
{
	uint	Status, Temp = 0x0, Count = 0, data = 0;
	bool	Ret= false;

	DBG("i2c_read() - 0x%x devaddr\n", devAdd ) ; 
	data = (( M << 3 ) | N ) ;
	gt_write( I2C_STATUS_BAUD_RATE, data );
	gt_write( I2C_CONTROL, 0x64 );

	udelay( I2C_DELAY );

	while( !isIntFlagSet() );

	Status = gt_read( I2C_STATUS_BAUD_RATE );
	DBG("Status = 0x%x \n", Status );

	if(( Status == 0x10) || ( Status == 0x08 ))
	{
		DBG("I2C Start Restart ok.\n" ) ;

		gt_write(I2C_DATA,devAdd | 0x01);
                                         
		clearIntFlag();
		while( !( isIntFlagSet() ));

		Status = gt_read( I2C_STATUS_BAUD_RATE );

		switch( Status ) 
		{
			case I2CBUS_ERROR:
				DBG("Bus Error!!!\n");
				Temp = gt_read( I2C_CONTROL);
				gt_write( I2C_CONTROL, Temp & 0x10);
				udelay( I2C_DELAY );
				clearIntFlag();
				return Ret;

			case ADDR_RB_TRA_ACK:
				DBG("In ADDR_RB_TRA_ACK... \n" ) ;
				clearIntFlag(); 
				udelay( I2C_DELAY * 2 );

				for( Count = 0; Count < index; Count++ )
				{
					Status = gt_read( I2C_STATUS_BAUD_RATE );
					switch( Status )
					{
						case MASTER_READ_ACK_TRA:
						DBG("MASTER_READ_ACK_TRA \n " ) ;
						regFile[Count] = gt_read( I2C_DATA );                   
						clearIntFlag();  
						udelay( I2C_DELAY * 5 );
						Ret = true;
						break;

						case MASTER_READ_NACK_TRA:
						DBG("MASTER_READ_NACK_TRA\n");
						Ret = false;
						clearIntFlag();
						udelay( I2C_DELAY );
						break;

						default:
						DBG("Failed to receive Data\n");
						Ret = false;
						clearIntFlag(); 
						udelay( I2C_DELAY );
						break;
					}
				} /*End of For loop */

				Temp = gt_read( I2C_CONTROL );
				gt_write( I2C_CONTROL, Temp & 0xfff0 );
				udelay( I2C_DELAY );

				gt_write( I2C_CONTROL, Temp & 0x10 ); 
				clearIntFlag();
				udelay( I2C_DELAY );
				gt_write( I2C_CONTROL, 0x0 );
				return Ret;

			case ADDR_RB_TRA_NACK:
				DBG("ADDR_RB_TRA_NACK\n");
				gt_write(I2C_CONTROL,Temp & 0x10);
				clearIntFlag();
				udelay( I2C_DELAY );
				gt_write( I2C_CONTROL, 0x0 );
				return Ret;

		}
	}
	else 
	{
		DBG("I2C Start/Re-start Condition Fails.....\n");
		gt_write(I2C_CONTROL,Temp & 0x10); /* generate stop bit */
		clearIntFlag();
		udelay( I2C_DELAY );
		gt_write( I2C_CONTROL, 0x0 );
		return Ret;
	}
	return Ret;
}

/******************************************************************************
**			RTC Functions	
******************************************************************************/
bool i2c_rtc_dummy_write( u_char devAdd, uint roffset )
{
	uint i2cStatus, TempData = 0x0;
	bool retval= false;

	gt_write( I2C_STATUS_BAUD_RATE, 0x2c );

	gt_write( I2C_CONTROL, 0x60 ); 

	while( !isIntFlagSet() );

	i2cStatus = gt_read( I2C_STATUS_BAUD_RATE );

	if( i2cStatus == 0x08 ){
		gt_write( I2C_DATA, devAdd | 0x00 ); 
		clearIntFlag();
		while(!isIntFlagSet());

		i2cStatus = gt_read( I2C_STATUS_BAUD_RATE );
		switch( i2cStatus ){
			case I2CBUS_ERROR:
				DBG("Bus error!!!\n");
				TempData = gt_read( I2C_CONTROL );
				gt_write( I2C_CONTROL, TempData & 0x10 );
				clearIntFlag();
				return retval;

			case ADDR_WB_TRA_ACK:
				gt_write( I2C_DATA, roffset ); /* Word Address */
				clearIntFlag();
				while(!isIntFlagSet());
				i2cStatus = gt_read( I2C_STATUS_BAUD_RATE );
				switch( i2cStatus ){
					case MASTER_DATA_TRA_ACK:
						retval= true;
						break;
					case MASTER_DATA_TRA_NACK:
						retval = false;
						break;
					case MASTER_LOST_ARBTR_DURING_ADR_DATA:
						retval = false;
						break;
				}
				return retval;
			case ADDR_WB_TRA_NACK:
				DBG("ADDR_WB_TRA_NACK\n");
				gt_write(I2C_CONTROL, TempData & 0x10 );
				clearIntFlag();
				udelay( I2C_DELAY );
				gt_write(I2C_CONTROL,0x0);
				return retval;
		}
	}
	else{
		DBG("I2C Re-Start Condition Fails.....\n");
		gt_write(I2C_CONTROL,TempData & 0x10);
		udelay( I2C_DELAY );
		gt_write(I2C_CONTROL,0x0);
		return retval;
	}
	return retval;
}

/******************************************************************************
** Function    :	i2c_rtc_read 
** Description : This function is to get the time and date from the RTC device. 
**		 The slave address of RTC is always fixed(by the vendor) 
**		to 0xa0.The format of date/time is "hh:mm:ss:dd:mm:yy:ce:da"
**		In this format the data would be read and stored in the buffer 
**		passed to the routine.
** Inputs      : buffer - array where the date/time has to be stored.
**
** Output      : true  - If successful.
**               false - If not successsful.
*******************************************************************************/
void i2c_rtc_read( u_char *buffer )
{
	u_char regfile[20], regFile1[4];
	int flag = 0;

	do{
		flag=0;
		udelay( I2C_DELAY );
		gt_write(I2C_SOFT_RESET,0x0); 
		udelay( I2C_DELAY );


		if(i2c_rtc_dummy_write( 0xa0, 0xbf )){                         
			i2c_read( 0xa0, regfile, 8 );         
			buffer[0] = regfile[2]; /*hour*/
			buffer[1] = regfile[1]; /*min*/
			buffer[2] = regfile[0]; /*seconds*/
			buffer[3] = regfile[3]; /*date*/
			buffer[4] = regfile[4]; /*month*/
			buffer[5] = regfile[6]; /*year*/
			buffer[6] = regfile[5]; /*day*/
		}

		else{
			DBG("RTCdummy write unsuccessfull\n");
		}
    
    
    		gt_write(I2C_SOFT_RESET,0x0); 
    		udelay( I2C_DELAY );   
     
		if( i2c_rtc_dummy_write( 0xa0, 0x93 )){                        
			i2c_read( 0xa0, regFile1, 1 );
			buffer[7]=regFile1[0];        
		}
		else{
			DBG("RTCdummy write unsuccessfull\n");
		}

		gt_write(I2C_SOFT_RESET,0x0); 
		udelay( I2C_DELAY );   /* Restart the I2C controller */

		if(i2c_rtc_dummy_write( 0xa0, 0xbf )){                         
			i2c_read( 0xa0, regfile, 8 );         
			if (buffer[5]!=regfile[6]) /*year*/
			flag=1;
		}
		else{
			DBG("RTCdummy write unsuccessfull\n");
		} 
	}/*end do*/
	while( flag == 1 ); 
         
}

/******************************************************************************
** Function    : i2c_rtc_write 
**
** Description : This function is to set the time and date of the RTC device. 
**		 The slave address of RTC is always fixed(by the vendor) 
**		 to 0xa0.The format of date/time is "hh:mm:ss:dd:mm:yy:ce:da"
**		 In this format the data would be sent and stored in the slave.
** Inputs      : buffer - array where the date/time to be set is stored.
**
** Output      : true  - If successful.
**               false - If not successsful.
*******************************************************************************/
void i2c_rtc_write( u_char *buffer ) 
{
	u_char regfile[20]; 

	regfile[3] = buffer[0]; /*hour*/
	regfile[2] = buffer[1]; /*minutes*/ 

	regfile[1] = buffer[2]; /*seconds*/
	regfile[4] = buffer[3];  /*date*/
	regfile[5] = buffer[4]; /*month*/
	regfile[7] = buffer[5]; /*year*/
	regfile[6] = buffer[6]; /*day*/
	regfile[8] = 0x00;      

	regfile[0] = 0xbe;

	udelay( I2C_DELAY ); 
	gt_write(I2C_SOFT_RESET,0x0);
	udelay( I2C_DELAY );   /* Restart the I2C controller */

	if (!i2c_write( 0xa0, regfile, 9 )){
		DBG("Burst write failed\n");
	} 

	udelay( I2C_DELAY );   
	gt_write( I2C_SOFT_RESET, 0x0 );
	udelay( I2C_DELAY );   

	regfile[0] = 0x93;
	regfile[1] = buffer[7];

	if (!i2c_write( 0xa0, regfile, 2 )){
		DBG("Century write failed\n");
	} 
}

/******************************************************************************/

static int mv64360_init_i2c_core( void )
{
	eeprom_type = get_eeprom_size( 0xA8 ) ;
	printk("AT24C%d EEPROM detected.\n", eeprom_type ) ;
	return 0;
}

/******************************************************************************
**		Device Algorithm Functions
******************************************************************************/
static int i2c_mv64360_xfer( struct i2c_adapter *adap, struct i2c_msg msg[],
			     int num )
{
	struct i2c_algo_mv64360_data *adap_data = adap->algo_data ;
	struct i2c_msg *pmsg ;
	int i = 0 ;
	u_char addr ;
	u_char offset = adap_data->offset ;
	
	pmsg = &msg[i] ;
	addr = ( pmsg->addr << 1 ) ;	
	
	DBG("Master Xfer: msg.flags = 0x%x, addr = 0x%x\n", 
			 msg[i].flags, addr );
	/* Read */
	if( pmsg->flags & I2C_M_RD ){
		switch( eeprom_type )
		{
			case 2:		i2c_byte_read( addr,  offset, 
						       pmsg->len, pmsg->buf ) ;
					break;

			case 64: 	i2c_word_read( addr, offset, 
						       (uint) pmsg->len, pmsg->buf ) ;
					break;

			default:	printk("Read not implemented.\n" ) ;
					break;
		}
			
	}
	/* Write */
	else {
		DBG("Writing...\n" );
		switch( eeprom_type )	
	{
			case 2:		i2c_byte_write( addr, offset, 
							pmsg->len, pmsg->buf );
					break;
	
			case 64:	i2c_word_write( addr, offset, 
							pmsg->len, pmsg->buf );
					break;
			
			default:	printk("Write not implemented.\n" );
					break;
		}
	}
					
	return pmsg->len;
}

static int algo_control( struct i2c_adapter *adap, uint cmd, ulong arg )
{
	return 0;
}

static u32 i2c_mv64360_func( struct i2c_adapter *adap )
{
	return ( I2C_FUNC_SMBUS_EMUL |
		 I2C_FUNC_10BIT_ADDR |
		 I2C_FUNC_PROTOCOL_MANGLING );
}

/******************************************************************************
**		Exported Algorithm Data
******************************************************************************/

static struct i2c_algorithm i2c_mv64360_algo = {
//	name: "MV64360 I2C Algorithm",	/* older kernels */
//	id: I2C_ALGO_EXP,		/* older kernels */
	master_xfer: i2c_mv64360_xfer,
	smbus_xfer: NULL,
	slave_send: NULL,
	slave_recv: NULL,
	algo_control: algo_control,
	functionality: i2c_mv64360_func,
};


/******************************************************************************
**		Registering fucntions to load algorithms at runtime
******************************************************************************/

int i2c_mv64360_add_bus( struct i2c_adapter *adap )
{
	DBG("Algorithm for MV64360 I2C %s registered.\n", adap->name );	
	
//	adap->id |= i2c_mv64360_algo.id ;	/* older kernels */
	adap->algo = &i2c_mv64360_algo ;

	adap->timeout = 100;
	adap->retries = 3;
		
	/* Now perform the MV64360 I2C initialization as mentioned 
	   in the Data sheet */
	mv64360_init_i2c_core( ) ;	

	/* Add the Adapter */
	
	i2c_add_adapter( adap );
	
	return 0;
}

int i2c_mv64360_del_bus( struct i2c_adapter *adap )
{
	int res;
	
	if(( res = i2c_del_adapter( adap )) < 0 ){ 
	
		return res;
	}
	DBG("Delete Bus %s.\n", adap->name );
	
	return 0;
}

/*******************************************************************************
**              Adapter Functions
*******************************************************************************/
static int i2c_mv64360_reg( struct i2c_client *client )
{
        DBG("Register Client, %s \n", client->name );
        return 0;
}

static int i2c_mv64360_unreg( struct i2c_client *client )
{
        DBG("Unregister Client, %s \n", client->name );
        return 0;
}

static struct i2c_algo_mv64360_data mv64360_data ;

/******************************************************************************
**              Exported Adapter Data
******************************************************************************/
static struct i2c_adapter i2c_mv64360_adap = {
        name: "MV64360",
        id: 0x00,
        algo: NULL, 
        algo_data: &mv64360_data,
        client_register: &i2c_mv64360_reg,
        client_unregister: &i2c_mv64360_unreg,
};


/******************************************************************************
**		Initial Routine
******************************************************************************/

static int __init i2c_mv64360_init_module( void )
{
	printk( KERN_INFO "i2c-algo-mv64360: I2C MV64360 Module\n"); 

	if( i2c_mv64360_add_bus( &i2c_mv64360_adap ) < 0 ){ 
		printk("i2c-algo-mv64360: Unable to register with I2C.\n" );
		return -ENODEV;
	}

	return 0; 
}

module_init( i2c_mv64360_init_module ) ;

/******************************************************************************
**		Exit Routine
******************************************************************************/
static void i2c_mv64360_exit_module( void )
{
	printk( KERN_INFO "Unloading I2C MV64360 module.\n" );
	i2c_mv64360_del_bus( &i2c_mv64360_adap );
}

module_exit( i2c_mv64360_exit_module ) ;


EXPORT_SYMBOL( i2c_rtc_read );
EXPORT_SYMBOL( i2c_rtc_write );


#ifdef MODULE 
MODULE_AUTHOR("D K Raghunandan" );
MODULE_DESCRIPTION("I2C Bus MV64360 Algorithm" );
#endif
/* MODULE */



