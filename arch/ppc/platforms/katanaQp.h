/*
 * arch/ppc/platforms/katanaQp.h
 * 
 * Definitions for Artesyn KatanaQP board
 *
 * Tim Montgomery <timm@artesyncp.com> 
 *
 * Based on code done by Rabeeh Khoury - rabeeh@galileo.co.il
 * Based on code done by Mark A. Greer <mgreer@mvista.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __PPC_PLATFORMS_KATANAQP_H
#define __PPC_PLATFORMS_KATANAQP_H

/*
 * CPU Physical Memory Map setup.
 */

#define KATANAQP_BOOT_WINDOW_BASE         0xff800000
#define KATANAQP_INTERNAL_SRAM_BASE       0xf8400000
#define KATANAQP_CPLD_BASE                0xf8200000
#define KATANAQP_BRIDGE_REG_BASE          0xf8100000
#define KATANAQP_SOCKET_BASE              0xf8000000
#define KATANAQP_SOLDERED_FLASH_BASE      0xe8000000

#define KATANAQP_SOLDERED_FLASH_SIZE_ACTUAL 0x02000000	/* soldered FLASH */
#define KATANAQP_SOCKETED_FLASH_SIZE_ACTUAL 0x00080000	/* PLCC socket */
#define KATANAQP_CPLD_SIZE_ACTUAL           0x00020000	/* CPLD space */
#define KATANAQP_BOOT_WINDOW_SIZE_ACTUAL    0x00800000	/* boot window */

#define KATANAQP_BOOT_WINDOW_SIZE max(MV64360_WINDOW_SIZE_MIN,  \
                  KATANAQP_BOOT_WINDOW_SIZE_ACTUAL)

#define KATANAQP_SOLDERED_FLASH_SIZE     max(MV64360_WINDOW_SIZE_MIN,  \
                  KATANAQP_SOLDERED_FLASH_SIZE_ACTUAL)

#define KATANAQP_SOCKETED_FLASH_SIZE     max(MV64360_WINDOW_SIZE_MIN,  \
                  KATANAQP_SOCKETED_FLASH_SIZE_ACTUAL)

#define KATANAQP_CPLD_SIZE     max(MV64360_WINDOW_SIZE_MIN,  \
                  KATANAQP_CPLD_SIZE_ACTUAL)

#define KATANAQP_INTERNAL_SRAM_SIZE 0x40000	/* 2Mbits internal SRAM size */

/****************************************************************************
 * The KatanaQp uses both MV64460 PCI buses.
 *
 * PCI Interface 0 is 64bit PCIX attached to the PCI Express Bridge
 *  0xc0000000-0xc0ffffff - I/O Space    (32MB)
 *  0x80000000-0x8fffffff - Memory Space (256MB)
 *
 * PCI Interface 1 is 32bit PCI attached to GigE switch and four PMC sites
 *  0xc1000000-0xc1ffffff - I/O Space        (32MB)
 *  0x90000000-0x9fffffff - Memory Space - Window 0 (256MB)
 *  0xa0000000-0xbfffffff - Memory Space - Window 1 (512MB)
 ****************************************************************************
 */

#define KATANAQP_PCI0_IO_START_PROC_ADDR          0xc0000000
#define KATANAQP_PCI0_IO_START_PCI_ADDR           0x00000000
#define KATANAQP_PCI0_IO_SIZE                     0x01000000

#define KATANAQP_PCI0_MEM_START_PROC_ADDR         0x80000000
#define KATANAQP_PCI0_MEM_START_PCI_HI_ADDR       0x00000000
#define KATANAQP_PCI0_MEM_START_PCI_LO_ADDR       0x80000000
#define KATANAQP_PCI0_MEM_SIZE                    0x10000000

#define KATANAQP_PCI1_IO_START_PROC_ADDR          0xc1000000
#define KATANAQP_PCI1_IO_START_PCI_ADDR           0x01000000
#define KATANAQP_PCI1_IO_SIZE                     0x01000000

#define KATANAQP_PCI1_MEM0_START_PROC_ADDR        0x90000000
#define KATANAQP_PCI1_MEM0_START_PCI_HI_ADDR      0x00000000
#define KATANAQP_PCI1_MEM0_START_PCI_LO_ADDR      0x90000000
#define KATANAQP_PCI1_MEM0_SIZE                   0x10000000
#define KATANAQP_PCI1_MEM1_START_PROC_ADDR        0xa0000000
#define KATANAQP_PCI1_MEM1_START_PCI_HI_ADDR      0x00000000
#define KATANAQP_PCI1_MEM1_START_PCI_LO_ADDR      0xa0000000
#define KATANAQP_PCI1_MEM1_SIZE                   0x20000000

#define MV64x60_MPP_CNTL2_MPP23_GPIO 0xf0000000
#define MV64x60_MPP_CNTL2_MPP22_GPIO 0x0f000000
#define MV64x60_MPP_CNTL2_MPP21_GPIO 0x00f00000
#define MV64x60_MPP_CNTL2_MPP19_GPIO 0x0000f000
#define MV64x60_MPP_CNTL2_MPP18_GPIO 0x00000f00

#define MV64x60_MPP_CNTL3_MPP31_GPIO 0xf0000000
#define MV64x60_MPP_CNTL3_MPP30_GPIO 0x0f000000
#define MV64x60_MPP_CNTL3_MPP27_GPIO 0x0000f000
#define MV64x60_MPP_CNTL3_MPP26_GPIO 0x00000f00
#define MV64x60_MPP_CNTL3_MPP25_GPIO 0x000000f0

/*
 * Board-specific interrupt vectors
 * 
 * PCI0 INTA --> MPP19
 * PCI0 INTB --> MPP21
 * PCI0 INTC --> MPP22 
 * PCI0 INTD --> MPP25
 * 
 * PCI1 INTA --> MPP26
 * PCI1 INTB --> MPP27
 * PCI1 INTC --> MPP30
 * PCI1 INTD --> MPP31
 *
 *    PERR/SERR --> MPP23
 * NMI Watchdog --> MPP18
 */

#define KATANAQP_PCI0_INTA_IRQ    64+19
#define KATANAQP_PCI0_INTB_IRQ    64+21
#define KATANAQP_PCI0_INTC_IRQ    64+22
#define KATANAQP_PCI0_INTD_IRQ    64+25
#define KATANAQP_PCI1_INTA_IRQ    64+26
#define KATANAQP_PCI1_INTB_IRQ    64+27
#define KATANAQP_PCI1_INTC_IRQ    64+30
#define KATANAQP_PCI1_INTD_IRQ    64+31

#define KATANAQP_PERR_SERR_IRQ    64+18
#define KATANAQP_NMI_WATCHDOG_IRQ 64+23

#define KATANAQP_MV64460_EEPROM_ADDR             0x52
#define KATANAQP_MONITOR_EEPROM_ADDR             0x53
#define KATANAQP_NV_MON_PARAMS_SIZE              0x5D8
#define KATANAQP_NV_MON_PARAMS_OFFSET            0x1800

/*******************************************************************
 * CPLD offsets
 *******************************************************************/

#define KATANAQP_CPLD_RER             0x0000	/* Reset enable */
#define KATANAQP_CPLD_RCR             0x0004	/* Reset command */
#define KATANAQP_CPLD_CPUE            0x0008	/* 2nd CPU enable */
#define KATANAQP_CPLD_PRER            0x000c	/* PCI reset enable */
#define KATANAQP_CPLD_IER0            0x0010	/* Int enable 0 */
#define KATANAQP_CPLD_IER1            0x0014	/* Int enable 1 */
#define KATANAQP_CPLD_IPR0            0x0018	/* Int pending 0 */
#define KATANAQP_CPLD_IPR1            0x001c	/* Int pending 1 */
#define KATANAQP_CPLD_HVR             0x0020	/* Harwdare version */
#define KATANAQP_CPLD_PVR             0x0024	/* PLD version */
#define KATANAQP_CPLD_JSR             0x0028	/* Jumper setting */
#define KATANAQP_CPLD_PSR             0x0030	/* PCI status */
#define KATANAQP_CPLD_LEDE            0x0034	/* LED */
#define KATANAQP_CPLD_PCR0            0x0038	/* CPU0 PLL config */
#define KATANAQP_CPLD_PCR1            0x003c	/* CPU1 PLL config */
#define KATANAQP_CPLD_CCR             0x0040	/* CPU configuration */
#define KATANAQP_CPLD_HCR             0x0044	/* Hardware config */
#define KATANAQP_CPLD_BDRR            0x0048	/* Boot device redirection */
#define KATANAQP_CPLD_J2CR            0x0050	/* J20 config */
#define KATANAQP_CPLD_CTCR            0x0054	/* CT control */
#define KATANAQP_CPLD_I2CR            0x0058	/* I2C interface */
#define KATANAQP_CPLD_SCR             0x005c	/* Switch control */

#define KATANAQP_CPLD_SUPERSET_PLLA   0x0100
#define KATANAQP_CPLD_SUPERSET_PLLB   0x0180

#define KATANAQP_CPLD_RER_INITACT     0x80
#define KATANAQP_CPLD_RER_SWHR        0x40
#define KATANAQP_CPLD_RER_WDE         0x20
#define KATANAQP_CPLD_RER_COPSR       0x10
#define KATANAQP_CPLD_RER_COPHR       0x08
#define KATANAQP_CPLD_RER_PCIR        0x04
#define KATANAQP_CPLD_RER_IPMCR       0x02
#define KATANAQP_CPLD_RER_FPPB        0x01

#define KATANAQP_CPLD_RCR_CPUHR       0x80
#define KATANAQP_CPLD_RCR_J20CR       0x40
#define KATANAQP_CPLD_RCR_PCIR        0x10
#define KATANAQP_CPLD_RCR_I2CR        0x04
#define KATANAQP_CPLD_RCR_FLR         0x02
#define KATANAQP_CPLD_RCR_GBR         0x01

#define KATANAQP_CPLD_CPUE_CPUE       0x01

#define KATANAQP_CPLD_PRER_PMC4RE     0x80
#define KATANAQP_CPLD_PRER_PMC3RE     0x40
#define KATANAQP_CPLD_PRER_PMC2RE     0x20
#define KATANAQP_CPLD_PRER_PMC1RE     0x10
#define KATANAQP_CPLD_PRER_SWRE       0x08
#define KATANAQP_CPLD_PRER_MVRE       0x04
#define KATANAQP_CPLD_PRER_PCI0RE     0x01

#define KATANAQP_CPLD_IER0_E2IM       0x10
#define KATANAQP_CPLD_IER0_E1IM       0x08
#define KATANAQP_CPLD_IER0_E0IM       0x04
#define KATANAQP_CPLD_IER0_PLLAIM     0x02
#define KATANAQP_CPLD_IER0_PLLBIM     0x01

#define KATANAQP_CPLD_IER1_PCI1SM     0x08
#define KATANAQP_CPLD_IER1_PCI1PM     0x04
#define KATANAQP_CPLD_IER1_PCI0SM     0x02
#define KATANAQP_CPLD_IER1_PCI0PM     0x01

#define KATANAQP_CPLD_IPR0_E2I        0x10
#define KATANAQP_CPLD_IPR0_E1I        0x08
#define KATANAQP_CPLD_IPR0_E0I        0x04
#define KATANAQP_CPLD_IPR0_PLLAI      0x02
#define KATANAQP_CPLD_IPR0_PLLBI      0x01

#define KATANAQP_CPLD_IPR1_PCI1S      0x08
#define KATANAQP_CPLD_IPR1_PCI1P      0x04
#define KATANAQP_CPLD_IPR1_PCI0S      0x02
#define KATANAQP_CPLD_IPR1_PCI0P      0x01

#define KATANAQP_CPLD_JSR_SROM        0x80
#define KATANAQP_CPLD_JSR_CPU         0x40
#define KATANAQP_CPLD_JSR_BOOT_SKT    0x20
#define KATANAQP_CPLD_JSR_SMHC        0x08
#define KATANAQP_CPLD_JSR_JMP2        0x04
#define KATANAQP_CPLD_JSR_JMP1        0x02

#define KATANAQP_CPLD_PSR_PMCM        0x02
#define KATANAQP_CPLD_PSR_PCIE        0x01

#define KATANAQP_CPLD_LEDR_CPU0R      0x80
#define KATANAQP_CPLD_LEDR_CPU0G      0x40
#define KATANAQP_CPLD_LEDR_CPU1R      0x20
#define KATANAQP_CPLD_LEDR_CPU1G      0x10
#define KATANAQP_CPLD_LEDR_DB3        0x08
#define KATANAQP_CPLD_LEDR_DB2        0x04
#define KATANAQP_CPLD_LEDR_DB1        0x02
#define KATANAQP_CPLD_LEDR_DB0        0x01

#define KATANAQP_CPLD_PCR0_PLLCF           0x80
#define KATANAQP_CPLD_PCR0_PLL_CONFIG_MASK 0x1f

#define KATANAQP_CPLD_PCR1_PLLCF           0x80
#define KATANAQP_CPLD_PCR2_PLL_CONFIG_MASK 0x1f

#define KATANAQP_CPLD_CCR_SCF1        0x40
#define KATANAQP_CPLD_CCR_SCF0        0x20
#define KATANAQP_CPLD_CCR_PLLC0       0x10
#define KATANAQP_CPLD_CCR_PLLC1       0x08
#define KATANAQP_CPLD_CCR_PLLC2       0x04
#define KATANAQP_CPLD_CCR_PLLC3       0x02
#define KATANAQP_CPLD_CCR_PLLC4       0x01

#define KATANAQP_CPLD_CCR_SCF_MASK    0x60
#define KATANAQP_CPLD_CCR_SCF_133     0x00
#define KATANAQP_CPLD_CCR_SCF_167     0x20
#define KATANAQP_CPLD_CCR_SCF_200     0x40

#define KATANAQP_CPLD_BDRR_BR         0x80
#define KATANAQP_CPLD_BDRR_IPMCFS     0x20
#define KATANAQP_CPLD_BDRR_BSJ        0x10
#define KATANAQP_CPLD_BDRR_BDS        0x04
#define KATANAQP_CPLD_BDRR_BDF1       0x02
#define KATANAQP_CPLD_BDRR_BDF0       0x01

#define KATANAQP_CPLD_J2CR_CLK1AE     0x20
#define KATANAQP_CPLD_J2CR_CLK1BE     0x10
#define KATANAQP_CPLD_J2CR_CLK2AE     0x08
#define KATANAQP_CPLD_J2CR_CLK2BE     0x04
#define KATANAQP_CPLD_J2CR_CLK3AE     0x02
#define KATANAQP_CPLD_J2CR_CLK3BE     0x01

#define KATANAQP_CPLD_CTCR_LAMS       0x04
#define KATANAQP_CPLD_CTCR_LBMS       0x02
#define KATANAQP_CPLD_CTCR_LCBE       0x01

#define KATANAQP_CPLD_I2CR_I2C2D      0x20
#define KATANAQP_CPLD_I2CR_I2C2C      0x10
#define KATANAQP_CPLD_I2CR_I2C1D      0x02
#define KATANAQP_CPLD_I2CR_I2C1C      0x01

#define KATANAQP_CPLD_SCR_SDID        0x10
#define KATANAQP_CPLD_SCR_SDE         0x02
#define KATANAQP_CPLD_SCR_SSI         0x01

#define PCI_VENDOR_ID_ARTESYN       0x1223
#define PCI_DEVICE_ID_KATANAQP      0x0049

#define	KATANAQP_DEFAULT_BAUD			9600
#define	KATANAQP_MPSC_CLK_SRC			8	/* TCLK */
#define	KATANAQP_MPSC_CLK_FREQ			133333333	/* 133.3333... MHz */

#define	KATANAQP_ETH0_PHY_ADDR			9
#define	KATANAQP_ETH1_PHY_ADDR			8
#define	KATANAQP_ETH2_PHY_ADDR			6

#define KATANAQP_ETH_TX_QUEUE_SIZE		800
#define KATANAQP_ETH_RX_QUEUE_SIZE		400

#define KATANAQP_MTD_MONITOR_SIZE		0x100000

#define KATANAQP_ETH_PORT_SERIAL_CONTROL_DEFAULT_VALUE           \
                MV643XX_ETH_FORCE_LINK_PASS              |       \
                MV643XX_ETH_DISABLE_AUTO_NEG_FOR_DUPLX   |       \
                MV643XX_ETH_DISABLE_AUTO_NEG_FOR_FLOW_CTRL |    \
                MV643XX_ETH_ADV_SYMMETRIC_FLOW_CTRL     |       \
                MV643XX_ETH_FORCE_FC_MODE_NO_PAUSE_DIS_TX |     \
                MV643XX_ETH_FORCE_BP_MODE_NO_JAM        |       \
                (1<<9)  /* reserved */                  |       \
                MV643XX_ETH_DO_NOT_FORCE_LINK_FAIL      |       \
                MV643XX_ETH_RETRANSMIT_16_ATTEMPTS      |       \
                MV643XX_ETH_DISABLE_AUTO_NEG_SPEED_GMII  |       \
                MV643XX_ETH_DTE_ADV_0                   |       \
                MV643XX_ETH_DISABLE_AUTO_NEG_BYPASS     |       \
                MV643XX_ETH_AUTO_NEG_NO_CHANGE          |       \
                MV643XX_ETH_MAX_RX_PACKET_9700BYTE      |       \
                MV643XX_ETH_CLR_EXT_LOOPBACK            |       \
                MV643XX_ETH_SET_FULL_DUPLEX_MODE        |       \
                MV643XX_ETH_ENABLE_FLOW_CTRL_TX_RX_IN_FULL_DUPLEX

#define	KATANAQP_ETH_PORT_CONFIG_EXTEND_VALUE		\
	ETH_SPAN_BPDU_PACKETS_AS_NORMAL		|	\
	ETH_PARTITION_DISABLE

#define	GT_ETH_IPG_INT_RX(value)			\
	((value & 0x3fff) << 8)

#define	KATANAQP_ETH_PORT_SDMA_CONFIG_VALUE		\
	ETH_RX_BURST_SIZE_4_64BIT		|	\
	GT_ETH_IPG_INT_RX(0)			|	\
	ETH_TX_BURST_SIZE_4_64BIT

#define	KATANAQP_ETH_PORT_SERIAL_CONTROL_VALUE		\
	ETH_FORCE_LINK_PASS			|	\
	ETH_ENABLE_AUTO_NEG_FOR_DUPLX		|	\
	ETH_DISABLE_AUTO_NEG_FOR_FLOW_CTRL	|	\
	ETH_ADV_SYMMETRIC_FLOW_CTRL		|	\
	ETH_FORCE_FC_MODE_NO_PAUSE_DIS_TX	|	\
	ETH_FORCE_BP_MODE_NO_JAM		|	\
	BIT9					|	\
	ETH_DO_NOT_FORCE_LINK_FAIL		|	\
	ETH_RETRANSMIT_16_ATTEMPTS		|	\
	ETH_ENABLE_AUTO_NEG_SPEED_GMII		|	\
	ETH_DTE_ADV_0				|	\
	ETH_DISABLE_AUTO_NEG_BYPASS		|	\
	ETH_AUTO_NEG_NO_CHANGE			|	\
	ETH_MAX_RX_PACKET_9700BYTE		|	\
	ETH_CLR_EXT_LOOPBACK			|	\
	ETH_SET_FULL_DUPLEX_MODE		|	\
	ETH_ENABLE_FLOW_CTRL_TX_RX_IN_FULL_DUPLEX

/**
 * katanaQp_bus_freq
 *
 * This routine reads board configuration register zero to determine the
 * correct bus speed.
 */

static unsigned long katanaQp_bus_freq(void __iomem *cpld_base)
{
	u8 ccr;

	ccr = in_8((volatile char *)(cpld_base + KATANAQP_CPLD_CCR));

	switch (ccr & KATANAQP_CPLD_CCR_SCF_MASK) {
	case KATANAQP_CPLD_CCR_SCF_200:
		return (200000000);
		break;

	case KATANAQP_CPLD_CCR_SCF_167:
		return (166666666);
		break;

	case KATANAQP_CPLD_CCR_SCF_133:
		return (133333333);
		break;

	default:
		return (133333333);
		break;
	}
}
#endif				/* __PPC_PLATFORMS_KATANAQP_H */
