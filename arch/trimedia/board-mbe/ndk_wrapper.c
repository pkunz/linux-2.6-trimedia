/*
 *
 */
#include <linux/kernel.h>
#include <linux/kernel.h>
#include <linux/errno.h>

#ifdef CONFIG_PCI
#include <asm/pci.h>
#endif 

#include <tmbslCore.h>
#ifdef CONFIG_PCI
#include "tmhwPci.h"
#endif 

#ifdef CONFIG_PCI
//-----------------------------------------------------------------------------
// FUNCTION:    mdsbslMbe_PciInit
//
// DESCRIPTION: Enumerate the PCI bus by calling tmhwPciInit.
//
// RETURN: tmErrorCode_t: Status of operation
//
// NOTES: Uses the ioSpace values from tmhwPciTest.
//
//  The PCI bus needs to be enumerated in the DMA bsl so that
//  each PCI devices can call tmhwPciSearch device to find
//  the devices base address.
//-----------------------------------------------------------------------------

// Define memory where the PCI device list is stored - word aligned
static UInt32 gpMem[ 1024 ];

#undef MBE_PRINT_PCI_INFO

tmErrorCode_t
mdsbslMbe_PciInit(void)
{
	tmErrorCode_t status;
	tmhwPciRes_t pciRes;
	UInt32 nDevices;

	// the following values taken from tmhwPciTest.c
	pciRes.unitId           = 0;
	pciRes.pMem             = (UInt8*)gpMem;
	pciRes.cbMem            = sizeof gpMem;
	pciRes.ioSpaceStart     = PCIBIOS_MIN_IO;   // Begin of the PCI IO space window
	pciRes.ioSpaceEnd       = PCIBIOS_MAX_IO;   // End of the PCI IO space window
	pciRes.memSpaceStart    = PCIBIOS_MIN_MEM;   // Begin of the memory space window.
	pciRes.memSpaceEnd      = PCIBIOS_MAX_MEM; // End of the memory space window
	pciRes.bBigEndian       = TMFL_ENDIAN_LITTLE;
	pciRes.bReMapDevices    = True;         // Map PCI devices into the PCI address space ranges.
	pciRes.pfnTimer         = Null;         // pointer to timer routine
	// The minimum amount of time, in PCI cycles, that the bus master can
	// retain ownership of the bus whenever it initiates a new transaction
	pciRes.latencyTimer = 0x20;

	status = tmhwPciInit(&pciRes);

	if (status != TM_OK)
	{
		printk( "mdsbslMbe BSL: mdsbsl_PciInit failed.\n");
		return status;
	}

#ifdef MBE_PRINT_PCI_INFO
	{
		int i;
		tmhwPciDeviceInfo_t DevInfo;
		UInt32 baseAddressRegCount;

		nDevices = tmhwPciGetDeviceCount( pciRes.unitId );
		printk( "mdsbslMbe BSL: Found %d PCI devices.\n", nDevices);
		for ( i = 0; i < nDevices; i++ )
		{
			if ( tmhwPciSearchDevice( 0,
						PCI_SEARCH_PARAM_IGNORE,
						PCI_SEARCH_PARAM_IGNORE,
						i,
						&DevInfo )
			   )
			{
				break;
			}

			printk( "Device No. %x\n", i );
			printk( "Device Id. 0x%x\n", DevInfo.DeviceId );
			printk( "Vendor Id. 0x%x\n", DevInfo.VendorId );
			printk( "Class      0x%x\n", DevInfo.Class );
			printk( "SubClass   0x%x\n", DevInfo.SubClass );

			for ( baseAddressRegCount = 0; baseAddressRegCount < 6; baseAddressRegCount++ )
			{
				if ( baseAddressRegCount < DevInfo.ccBaseAddrReg )
				{
					printk( 
							"Base address %x is %x, size %d\n",
							baseAddressRegCount,
							DevInfo.pBaseAddrReg[ baseAddressRegCount ],
							DevInfo.pBaseAddrRegSizes[ baseAddressRegCount ] );
				}
			}
			printk(  "\n");
		}
	}
#endif

	return TM_OK;
}
#endif 

extern compActivateFunc_t tmbslMgrBoardActivateOfBoardId0x10021131;

extern int
tmboard_activate(void)
{
	tmErrorCode_t status=TM_OK;

	status=(*tmbslMgrBoardActivateOfBoardId0x10021131)(NULL);
	if(status!=TM_OK){
		return -EFAULT;
	}

#ifdef CONFIG_PCI
	// Delay here otherwise some mini PCI devices will not get enumerated because
	// they are not ready yet.
	microsleep(10000);
	status = mdsbslMbe_PciInit();
	if(status !=TM_OK){
		return -EFAULT;
	}
#endif 

	return 0;
}
