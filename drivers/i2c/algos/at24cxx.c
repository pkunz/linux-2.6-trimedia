/*******************************************************************************
**	FileName:	at24cxx.c
**	Author	:	D K Raghunandan Copyright (c) 2003
**	Date	:	11 Mar 2003
**	Notes	:	I2C driver for serial eeprom Atmel AT24Cx.

**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.

**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
********************************************************************************
**	$Author		:$
**	$Date		:$
*******************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/init.h>

#include <asm/mv64360.h>
#include <linux/i2c.h>
#include <linux/i2c-algo-mv64360.h>

#include "i2c-mv64360.h"

/*******************************************************************************
**		Defines
*******************************************************************************/

#define DEBUG

#ifdef DEBUG
#define	DBG( string, args... ) printk(KERN_INFO "i2c-eeprom:" string, ##args )
#else
#define	DBG( string, args... )
#endif

#define 	EEPROM_MAJOR 	42
#define 	GET_MAC_ADDRS	0x0
#define 	SET_MAC_ADDRS	0x1

#define 	MAC_OFFSET	18


/*******************************************************************************
**		Prototypes Functions
*******************************************************************************/

static struct i2c_driver eeprom_driver; 
struct i2c_client *eeprom_client;

static unsigned short ignore[] = { I2C_CLIENT_END };
static unsigned short normal_addr[] = { 0x54, I2C_CLIENT_END };

static struct i2c_client_address_data addr_data = {
	normal_i2c:		normal_addr,
	normal_i2c_range:	ignore,
	probe:			ignore,
	probe_range:		ignore,
	ignore:			ignore,
	ignore_range:		ignore,
	force:			ignore,
};


/*******************************************************************************
**		Driver Functions
*******************************************************************************/
static int eeprom_attach( struct i2c_adapter *adap, int addr, int unused )
{
	DBG("Attaching client.\n" ) ;
	
	eeprom_client = kmalloc( sizeof( *eeprom_client), GFP_KERNEL );
	if( !eeprom_client ){
		return -ENOMEM;
	}

	strcpy( eeprom_client->name, "AT24Cx" );
	eeprom_client->id = eeprom_driver.id;
	eeprom_client->flags = 0;
	eeprom_client->addr = addr;
	eeprom_client->adapter = adap;

	eeprom_client->driver = &eeprom_driver;
#if 0
	eeprom_client->data = NULL;
#endif

	return i2c_attach_client( eeprom_client );
}

static int eeprom_probe( struct i2c_adapter *adap )
{
	int err;
	err = i2c_probe( adap, &addr_data, eeprom_attach );
	if( err < 0 ){
		DBG("Probe failed.\n" );
		return -ENODEV;
	}

	return 0;
}

static int eeprom_detach( struct i2c_client *client )
{
	i2c_detach_client( client );
	kfree( client );
	return 0;
}

static int eeprom_command( struct i2c_client *client, uint cmd, void *arg )
{
	return 0;
}

/******************************************************************************/
static int eeprom_open( struct inode *inode, struct file *file )
{
//	MOD_INC_USE_COUNT;
	return 0;
}


static int eeprom_release( struct inode *inode, struct file *file )
{
//	MOD_DEC_USE_COUNT;
	return 0;
}

static ssize_t eeprom_write( struct file *file, const char *buf,
			     size_t count, loff_t *offset )
{
        int ret;
        char *tmp;
        struct i2c_client *client = eeprom_client;
	struct i2c_algo_mv64360_data *data ;

#ifdef DEBUG
        struct inode *inode = file->f_dentry->d_inode;
#endif /* DEBUG */
	
	/** First 18 bytes store MAC addresses of the #3 MV64360 
	 ** n/w ports 
	*/
	data = ( struct i2c_algo_mv64360_data *)client->adapter->algo_data ;	
	data->offset = (uint)(*offset) + MAC_OFFSET ;

        /* copy user space data to kernel space. */
        tmp = kmalloc(count,GFP_KERNEL);
        if (tmp==NULL)
                return -ENOMEM;
        if (copy_from_user(tmp,buf,count)) {
                kfree(tmp);
                return -EFAULT;
        }

        DBG("i2c-%d writing %d bytes.\n",MINOR(inode->i_rdev), count);
        ret = i2c_master_send(client,tmp,count);
        kfree(tmp);
        return ret;
}

static ssize_t eeprom_read( struct file *file, char *buf,
			    size_t count, loff_t *offset )
{
       char *tmp;
       int ret;


        struct i2c_client *client = eeprom_client;
	struct i2c_algo_mv64360_data *data ;

#ifdef DEBUG
        struct inode *inode = file->f_dentry->d_inode;
#endif /* DEBUG */
     	
	/** First 18 bytes store MAC addresses of the #3 MV64360 
	 ** n/w ports 
	*/
	data = ( struct i2c_algo_mv64360_data *)client->adapter->algo_data ;
        data->offset = (uint)(*offset) + MAC_OFFSET;

        /* copy user space data to kernel space. */
        tmp = kmalloc(count,GFP_KERNEL);
        if (tmp==NULL)
                return -ENOMEM;

        DBG("i2c-%d reading %d bytes.\n",MINOR(inode->i_rdev), count);

        ret = i2c_master_recv(client,tmp,count);
        if (ret >= 0)
                ret = copy_to_user(buf,tmp,count)?-EFAULT:ret;
        kfree(tmp);
        return ret;
}

static void set_mac_addr( u_char *buf )
{
	int count = 18 ;

        struct i2c_client *client = eeprom_client;
        struct i2c_algo_mv64360_data *data ;

	/* Offset of MAC Addr in EEPROM */
        data = ( struct i2c_algo_mv64360_data *)client->adapter->algo_data ;
        data->offset = 0;

        i2c_master_send(client,buf,count);
}

static void get_mac_addr( u_char *buf )
{
	int count = 18 ;

        struct i2c_client *client = eeprom_client;
        struct i2c_algo_mv64360_data *data ;

	/* Offset of MAC Addr in EEPROM */
        data = ( struct i2c_algo_mv64360_data *)client->adapter->algo_data ;
        data->offset = 0; 

        i2c_master_recv(client,buf,count);
}


static int eeprom_ioctl( struct inode *inode, struct file *file,
			 unsigned int cmd, unsigned long arg )
{
	switch( cmd ){ 
		case SET_MAC_ADDRS: 
				set_mac_addr((u_char*)arg ) ;
				break;
		
		case GET_MAC_ADDRS: 
				get_mac_addr((u_char*)arg ) ;
				break;
		default: 
				return -EINVAL ;
	}

	return 0;
}

/******************************************************************************
**		Exported Driver Data
******************************************************************************/
static struct i2c_driver eeprom_driver = {
	name:	"AT24Cxx",
	id:	I2C_DRIVERID_AT24Cxx,
	flags:	I2C_DF_NOTIFY,
	attach_adapter:	eeprom_probe,
	detach_client:	eeprom_detach,
	command:	eeprom_command
};

static struct file_operations eeprom_fops = {
	owner:		THIS_MODULE,
	llseek:		NULL,
	read:		eeprom_read,
	write:		eeprom_write,
	ioctl:		eeprom_ioctl,
	open:		eeprom_open,
	release:	eeprom_release,
};


/******************************************************************************
**		Initial Routine
******************************************************************************/

static int __init i2c_eeprom_init_module( void )
{
	int err;

	printk( KERN_INFO "Intializing AT24CXX eeprom driver.\n" );

	err = i2c_add_driver( &eeprom_driver );
	if( err ) {
		DBG("Unable to add driver , %s\n", eeprom_driver.name );
		return err;	
	}
	
	err = register_chrdev( EEPROM_MAJOR, "eeprom_chr_drv", &eeprom_fops );
	if( err ){
		DBG("Cant register eeprom chr driver.\n" );
		err = i2c_del_driver( &eeprom_driver );
		return err ;
	}
		
	return 0; 
}

module_init( i2c_eeprom_init_module ) ;

/******************************************************************************
**		Exit Routine
******************************************************************************/
static void i2c_eeprom_exit_module( void )
{
	printk( KERN_INFO "Unloading AT24CXX eeprom driver.\n" );
	unregister_chrdev( EEPROM_MAJOR, "eeprom_chr_drv" );

	i2c_del_driver( &eeprom_driver );
}

module_exit( i2c_eeprom_exit_module ) ;


#ifdef MODULE 
MODULE_AUTHOR("D K Raghunandan" );
MODULE_DESCRIPTION("I2C AT24CXX Driver" );
#endif
/* MODULE */



