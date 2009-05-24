/* 
 * 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/kernel.h>

#include <mmio.h>

void RseExceptionIlstoTest(void)
{
    // try to write outside any aperture (DRAM, MMIO, aperture1)
    // assume there is some space between the apertures
    // so write just above TM32_DRMA_HI
    // on system where DRAM isn't mapped to 0, you can also try to write to 0

    UInt32 start = 0;

    printk("Apertures\n");
    printk("\tDRAM: 0x%x-0x%x\n", MMIO(TM32_DRAM_LO), MMIO(TM32_DRAM_HI));
    printk("\tMMIO: 0x%x-0x%x\n", _MMIO_base, (UInt32)_MMIO_base+0x200000);  // MMIO is 2 MB, always
    printk("\tAperture1: 0x%x-0x%x\n", MMIO(TM32_APERT1_LO), MMIO(TM32_APERT1_HI));

    for (start = 0; ; start+=0x100000)
    {
        // check whether the address is in any of the appertures
        if ( (start >= MMIO(TM32_DRAM_LO)) && (start < MMIO(TM32_DRAM_HI)) )
        {
            // in DRAM, skip address
            continue;
        }
        if ( (start >= (UInt32)_MMIO_base) && (start < ((UInt32)_MMIO_base+0x200000)) )
        {
            // in MMIO, skip address
            continue;
        }
        if ( (start >= MMIO(TM32_APERT1_LO)) && (start < MMIO(TM32_APERT1_HI)) )
        {
            // in aperture1, skip address
            continue;
        }
        // found a valid address
        break;
    }
    //start = (char*)MMIO(TM32_DRAM_HI)+8;
    printk("Trying to write in unmapped memory: 0x%x\n", start);

    // RSE exception is installed automatically by tmbslPnx1500 (or tmbslTmOnly in new NDK)

    *(char*)start = 5;

    printk("Should never see this message\n");

    // note that there are \other reasons for the  ILSTO to trigger:
    // -

}
void RseExceptionIlstmTest(void)
{
    // - partial store in MMIO (only supports 32 bit store)
    UInt16* ptr=(UInt16*)(_MMIO_base);

    printk("Trying access 16 bit at 0x%x\n", ptr);
    *ptr=0x1234;
    printk("Should never see this message\n");
}
