/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/init.h>
#include <linux/ide.h>
#include <linux/config.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/delay.h>

#include <tmhwXio.h>

typedef struct pcixio_ide_regs
{
	unsigned char filler01[0x38-0x00];
	union _altStatusDevControl {            		// 0x38
		unsigned char altStatus;
		unsigned char devControl;
	} altStatusDevControl;
	unsigned char filler02[0x40-0x39];
	unsigned short data;                    			// 0x40
	unsigned char filler03[0x44-0x42];
	union _errFeature {                     		// 0x44
		unsigned char err;
		unsigned char feature;
	} errFeature;
	unsigned char filler04[0x48-0x45];
	unsigned char secCount;                 		// 0x48
	unsigned char filler05[0x4c-0x49];
	unsigned char secNumber;                		// 0x4C
	unsigned char filler06[0x50-0x4D];
	unsigned char cylLow;                   		// 0x50
	unsigned char filler07[0x54-0x51];
	unsigned char cylHigh;                  		// 0x54
	unsigned char filler08[0x58-0x55];
	unsigned char drvHead;                  		// 0x58
	unsigned char filler09[0x5C-0x59];
	union _statusCommand {
		volatile unsigned char status;
		volatile unsigned char command;
	} statusCommand;                        		// 0x5C
};

static u8 ideRegRead8(u32 addr)
{
#if        !defined(TMBSL_IDEXIO_USE_XIOHWAPI) && !defined(TMBSL_IDEXIO_USE_XIODEVLIB)
	/*
	   Direct xio
	   */
	return(
#if        defined(YES_XIO_BYTE_READ_WRITE)
			*(u8 *) addr
#else   // defined(YES_XIO_BYTE_READ_WRITE)
#if        (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
			(u8) TMBSL_IDEXIO_SWAP_UINT16(*(u16 *) addr)
#else   // (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
			(u8) *(u16 *) addr
#endif  // (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
#endif  // defined(YES_XIO_BYTE_READ_WRITE)
	      );

#else   // !defined(TMBSL_IDEXIO_USE_XIOHWAPI) && !defined(TMBSL_IDEXIO_USE_XIODEVLIB)

	u32 t;

#if        defined(TMBSL_IDEXIO_USE_XIOHWAPI)

	tmhwXioRead(tmUnit0, addr, &t, tmhwXioEnable1stByte, 10);

#else   // defined(TMBSL_IDEXIO_USE_XIOHWAPI)

	tmdlXioRead(gXioInstance, addr, &t, tmdlXioEnable1stByte, 10);

#endif  // defined(TMBSL_IDEXIO_USE_XIOHWAPI)

	return((u8) t);

#endif  // !defined(TMBSL_IDEXIO_USE_XIOHWAPI) && !defined(TMBSL_IDEXIO_USE_XIODEVLIB)
}


//-----------------------------------------------------------------------------
// FUNCTION:    ideRegRead16:
//
// DESCRIPTION: Read 16 bits from an IDE register
//
// RETURN:      The u16 value read
//
// NOTES:
//-----------------------------------------------------------------------------

static  u16
ideRegRead16(
		u32 addr                 // address of IDE register to read
	    )
{
#if        !defined(TMBSL_IDEXIO_USE_XIOHWAPI) && !defined(TMBSL_IDEXIO_USE_XIODEVLIB)

	return(*(u16 *) addr);

#else   // !defined(TMBSL_IDEXIO_USE_XIOHWAPI) && !defined(TMBSL_IDEXIO_USE_XIODEVLIB)

	u32 t;

#if        defined(TMBSL_IDEXIO_USE_XIOHWAPI)

	tmhwXioRead(
			tmUnit0,
			addr,
			&t,
			(tmhwXioByteEnable_t)(tmhwXioEnable1stByte | tmhwXioEnable2ndByte),
			10
		   );

#else   // defined(TMBSL_IDEXIO_USE_XIOHWAPI)

	tmdlXioRead(
			gXioInstance,
			addr,
			&t,
			(tmdlXioByteEnable_t)(tmdlXioEnable1stByte | tmdlXioEnable2ndByte),
			10
		   );

#endif  // defined(TMBSL_IDEXIO_USE_XIOHWAPI)

#if        (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
	t = TMBSL_IDEXIO_SWAP_UINT16(t);
#endif   //(TMFL_ENDIAN == TMFL_ENDIAN_BIG)

	return((u16) t);

#endif  // !defined(TMBSL_IDEXIO_USE_XIOHWAPI) && !defined(TMBSL_IDEXIO_USE_XIODEVLIB)
}

static u32
ideRegRead32(
		u32 addr                 // address of IDE register to read
	    )
{
#if        !defined(TMBSL_IDEXIO_USE_XIOHWAPI) && !defined(TMBSL_IDEXIO_USE_XIODEVLIB)

	return(*(u32 *) addr);

#else   // !defined(TMBSL_IDEXIO_USE_XIOHWAPI) && !defined(TMBSL_IDEXIO_USE_XIODEVLIB)

	u32 t;

#if        defined(TMBSL_IDEXIO_USE_XIOHWAPI)

	tmhwXioRead(
			tmUnit0,
			addr,
			&t,
			(tmhwXioByteEnable_t)(tmhwXioEnable1stByte | tmhwXioEnable2ndByte 
				| tmhwXioEnable3thBtye | tmhwXioEnable4thBtye),
			10
		   );

#else   // defined(TMBSL_IDEXIO_USE_XIOHWAPI)

	tmdlXioRead(
			gXioInstance,
			addr,
			&t,
			(tmhwXioByteEnable_t)(tmhwXioEnable1stByte | tmhwXioEnable2ndByte 
				| tmhwXioEnable3thBtye | tmhwXioEnable4thBtye),
			10
		   );

#endif  // defined(TMBSL_IDEXIO_USE_XIOHWAPI)

#if        (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
	t = TMBSL_IDEXIO_SWAP_UINT32(t);
#endif   //(TMFL_ENDIAN == TMFL_ENDIAN_BIG)

	return((u32) t);

#endif  // !defined(TMBSL_IDEXIO_USE_XIOHWAPI) && !defined(TMBSL_IDEXIO_USE_XIODEVLIB)
}

//-----------------------------------------------------------------------------
// FUNCTION:    ideRegWrite8:
//
// DESCRIPTION: Write 8 bits to an IDE register
//
// RETURN:
//
// NOTES:
//-----------------------------------------------------------------------------

static  void
ideRegWrite8(
		u8 val,                   // value to write
		u32 addr                // address of IDE register to read
	    )
{
#if        !defined(TMBSL_IDEXIO_USE_XIOHWAPI) && !defined(TMBSL_IDEXIO_USE_XIODEVLIB)

#if        defined(YES_XIO_BYTE_READ_WRITE)
	*(u8 *) addr = val;
#else   // defined(YES_XIO_BYTE_READ_WRITE)
#if        (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
	*(u16 *) addr = TMBSL_IDEXIO_SWAP_UINT16((u16) val);
#else   // (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
	*(u16 *) addr = (u16) val;
#endif  // (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
#endif  // defined(YES_XIO_BYTE_READ_WRITE)

#else   // !defined(TMBSL_IDEXIO_USE_XIOHWAPI) && !defined(TMBSL_IDEXIO_USE_XIODEVLIB)

#if        (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
	val = TMBSL_IDEXIO_SWAP_UINT16(val);
#endif   //(TMFL_ENDIAN == TMFL_ENDIAN_BIG)

#if        defined(TMBSL_IDEXIO_USE_XIOHWAPI)

	tmhwXioWrite(
			tmUnit0,
			addr,
			val,
#if        defined(YES_XIO_BYTE_READ_WRITE)
			tmhwXioEnable1stByte,
#else   // defined(YES_XIO_BYTE_READ_WRITE)
			(tmhwXioByteEnable_t)(tmhwXioEnable1stByte | tmhwXioEnable2ndByte),
#endif  // defined(YES_XIO_BYTE_READ_WRITE)
			10
		    );

#else   // defined(TMBSL_IDEXIO_USE_XIOHWAPI)

	tmdlXioWrite(
			gXioInstance,
			addr,
			val,
#if        defined(YES_XIO_BYTE_READ_WRITE)
			tmhwXioEnable1stByte,
#else   // defined(YES_XIO_BYTE_READ_WRITE)
			(tmdlXioByteEnable_t)(tmdlXioEnable1stByte | tmdlXioEnable2ndByte),
#endif  // defined(YES_XIO_BYTE_READ_WRITE)
			10
		    );

#endif  // defined(TMBSL_IDEXIO_USE_XIOHWAPI)
#endif  // !defined(TMBSL_IDEXIO_USE_XIOHWAPI) && !defined(TMBSL_IDEXIO_USE_XIODEVLIB)
}

static void ideRegWrite8Sync(ide_drive_t* drive, u8 val, u32 addr)
{
#if        (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
	val = TMBSL_IDEXIO_SWAP_UINT16(val);
#endif   //(TMFL_ENDIAN == TMFL_ENDIAN_BIG)

	tmhwXioWrite(
			tmUnit0,
			addr,
			val,
			(tmhwXioByteEnable_t)(tmhwXioEnable1stByte | tmhwXioEnable2ndByte),
			10
		    );
}

//-----------------------------------------------------------------------------
// FUNCTION:    ideRegWrite16:
//
// DESCRIPTION: Write 16 bits to an IDE register
//
// RETURN:
//
// NOTES:
//-----------------------------------------------------------------------------

static  void
ideRegWrite16(
		u16 val,                  // value to write
		u32 addr                // address of IDE register to read
	     )
{
#if        !defined(TMBSL_IDEXIO_USE_XIOHWAPI) && !defined(TMBSL_IDEXIO_USE_XIODEVLIB)

	*(u16 *) addr = val;

#else   // !defined(TMBSL_IDEXIO_USE_XIOHWAPI) && !defined(TMBSL_IDEXIO_USE_XIODEVLIB)

#if        (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
	val = TMBSL_IDEXIO_SWAP_UINT16(val);
#endif   //(TMFL_ENDIAN == TMFL_ENDIAN_BIG)

#if        defined(TMBSL_IDEXIO_USE_XIOHWAPI)

	tmhwXioWrite(
			tmUnit0,
			addr,
			val,
			(tmhwXioByteEnable_t)(tmhwXioEnable1stByte | tmhwXioEnable2ndByte),
			10
		    );

#else   // defined(TMBSL_IDEXIO_USE_XIOHWAPI)

	tmdlXioWrite(
			gXioInstance,
			addr,
			val,
			(tmdlXioByteEnable_t)(tmdlXioEnable1stByte | tmdlXioEnable2ndByte),
			10
		    );

#endif  // defined(TMBSL_IDEXIO_USE_XIOHWAPI)
#endif  // !defined(TMBSL_IDEXIO_USE_XIOHWAPI) && !defined(TMBSL_IDEXIO_USE_XIODEVLIB)
}

static  void
ideRegWrite32(
		u32 val,                 // value to write
		u32 addr                 // address of IDE register to read
	     )
{
#if        !defined(TMBSL_IDEXIO_USE_XIOHWAPI) && !defined(TMBSL_IDEXIO_USE_XIODEVLIB)

	*(u32 *) addr = val;

#else   // !defined(TMBSL_IDEXIO_USE_XIOHWAPI) && !defined(TMBSL_IDEXIO_USE_XIODEVLIB)

#if        (TMFL_ENDIAN == TMFL_ENDIAN_BIG)
	val = TMBSL_IDEXIO_SWAP_UINT32(val);
#endif   //(TMFL_ENDIAN == TMFL_ENDIAN_BIG)

#if        defined(TMBSL_IDEXIO_USE_XIOHWAPI)

	tmhwXioWrite(
			tmUnit0,
			addr,
			val,
			(tmhwXioByteEnable_t)(tmhwXioEnable1stByte | tmhwXioEnable2ndByte 
				| tmhwXioEnable3thByte | tmhwXioEnable4thByte),
			10
		    );

#else   // defined(TMBSL_IDEXIO_USE_XIOHWAPI)

	tmdlXioWrite(
			gXioInstance,
			addr,
			val,
			(tmhwXioByteEnable_t)(tmhwXioEnable1stByte | tmhwXioEnable2ndByte 
				| tmhwXioEnable3thByte | tmhwXioEnable4thByte),
			10
		    );

#endif  // defined(TMBSL_IDEXIO_USE_XIOHWAPI)
#endif  // !defined(TMBSL_IDEXIO_USE_XIOHWAPI) && !defined(TMBSL_IDEXIO_USE_XIODEVLIB)
}

static void ideRegReadS16(u32 port, void *addr, u32 count)
{
	while (count--) {
		*(u16 *)addr = ideRegRead16(port);
		addr = (void*)((unsigned long)addr + 2);
	}
}

static void ideRegReadS32(u32 port, void *addr, u32 count)
{
	while (count--) {
		*(u32 *)addr = ideRegRead32(port);
		addr = (void*)((unsigned long)addr + 4);
	}
}

static void ideRegWriteS16(u32 port, void *addr, u32 count)
{
	while (count--) {
		ideRegWrite16(*(u16 *)addr, port);
		addr = (void*)((unsigned long)addr + 2);
	}
}

static void ideRegWriteS32(u32 port, void *addr, u32 count)
{
	while (count--) {
		ideRegWrite32(*(u32 *)addr, port);
		addr = (void*)((unsigned long)addr + 4);
	}
}

static void trimedia_ide_register_setup(hw_regs_t *hw, tmhwXioProfileSetup_t* xioProfile)
{
	int i;
	struct pcixio_ide_regs* ide_regs = (struct pcixio_ide_regs*)(xio_region_start + (xioProfile->offset << 23));

	memset(hw, 0, sizeof(hw_regs_t)); 

	hw->io_ports [IDE_DATA_OFFSET]              = (unsigned long)&(ide_regs->data);
	hw->io_ports [IDE_ERROR_OFFSET]             = (unsigned long)&(ide_regs->errFeature.err);
	hw->io_ports [IDE_NSECTOR_OFFSET]           = (unsigned long)&(ide_regs->secCount);
	hw->io_ports [IDE_SECTOR_OFFSET]      	    = (unsigned long)&(ide_regs->secNumber);
	hw->io_ports [IDE_LCYL_OFFSET]       	    = (unsigned long)&(ide_regs->cylLow);
	hw->io_ports [IDE_HCYL_OFFSET]              = (unsigned long)&(ide_regs->cylHigh);
	hw->io_ports [IDE_SELECT_OFFSET]        = (unsigned long)&(ide_regs->drvHead);
	hw->io_ports [IDE_STATUS_OFFSET]            = (unsigned long)&(ide_regs->statusCommand.status);
	hw->io_ports [IDE_CONTROL_OFFSET]     = (unsigned long)&(ide_regs->altStatusDevControl.devControl);

	hw->irq = 26;  /*GPIO_10*/
	hw->dma = NO_DMA;
	/*hw->ack_intr = trimedia_ide_ack_intr;*/
	hw->chipset = ide_generic;
}

static void trimedia_ide_hwif_setup(ide_hwif_t *hwif)
{
	default_hwif_mmiops(hwif);

	hwif->mmio  = 2;

	hwif->OUTB	= ideRegWrite8;
	/*FIXME: how to make sure sync.*/
	hwif->OUTBSYNC	= ideRegWrite8Sync;
	hwif->INB	= ideRegRead8;

	hwif->OUTW	= ideRegWrite16;
	hwif->OUTSW	= ideRegWriteS16;
	hwif->INW	= ideRegRead16;
	hwif->INSW	= ideRegReadS16;

	hwif->OUTL	= ideRegWrite32;
	hwif->OUTSL	= ideRegWriteS32;
	hwif->INL	= ideRegRead32;
	hwif->INSL	= ideRegReadS32;
}

#define XIO_MAX_PROFS  5

void __init trimedia_ide_init(void)
{
	int xioSelect = 0;
	tmErrorCode_t ret = TM_OK;

	for(xioSelect = 0 ; xioSelect < XIO_MAX_PROFS; xioSelect++){
		hw_regs_t ide_regs;
		ide_hwif_t *hwif = NULL;
		int idx = 0;
		tmhwXioProfileSetup_t xioProfile;	

		memset(&ide_regs,0,sizeof(ide_regs));
		memset(&xioProfile,0,sizeof(xioProfile));

		ret = tmhwXioGetProfile(0,xioSelect,&xioProfile);
		if((ret == TM_OK) 
				&& (xioProfile.deviceType == tmhwXioIDE)
				&& (xioProfile.enabled == True)) {

			trimedia_ide_register_setup(&ide_regs,&xioProfile);

			idx = ide_register_hw(&ide_regs, &hwif);
			if(idx == -1){
				printk(KERN_ERR "ide-trimedia: IDE I/F register failed!\n");
				continue;
			}

			trimedia_ide_hwif_setup(hwif);

			printk(KERN_INFO "ide%d: TriMedia PCIXIO IDE controller registered\n",idx);
		}
	}	

	return;
}
