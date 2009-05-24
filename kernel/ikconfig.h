#ifndef _IKCONFIG_H
#define _IKCONFIG_H
/*
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 * NON INFRINGEMENT.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * 
 * This file is generated automatically by scripts/mkconfigs. Do not edit.
 *
 */
static char const ikconfig_config[] __attribute__((unused)) = 
"CONFIG_BEGIN=n\n\
CONFIG_MIPS=y\n\
# CONFIG_MIPS64 is not set\n\
# CONFIG_64BIT is not set\n\
CONFIG_MIPS32=y\n\
# CONFIG_EXPERIMENTAL is not set\n\
CONFIG_CLEAN_COMPILE=y\n\
CONFIG_STANDALONE=y\n\
CONFIG_BROKEN_ON_SMP=y\n\
CONFIG_SWAP=y\n\
CONFIG_SYSVIPC=y\n\
# CONFIG_BSD_PROCESS_ACCT is not set\n\
CONFIG_SYSCTL=y\n\
# CONFIG_AUDIT is not set\n\
CONFIG_LOG_BUF_SHIFT=14\n\
# CONFIG_HOTPLUG is not set\n\
CONFIG_IKCONFIG=y\n\
CONFIG_IKCONFIG_PROC=y\n\
CONFIG_EMBEDDED=y\n\
CONFIG_KALLSYMS=y\n\
CONFIG_FUTEX=y\n\
CONFIG_EPOLL=y\n\
CONFIG_IOSCHED_NOOP=y\n\
CONFIG_IOSCHED_AS=y\n\
CONFIG_IOSCHED_DEADLINE=y\n\
CONFIG_IOSCHED_CFQ=y\n\
# CONFIG_CC_OPTIMIZE_FOR_SIZE is not set\n\
# CONFIG_MODULES is not set\n\
# CONFIG_MACH_JAZZ is not set\n\
# CONFIG_MACH_VR41XX is not set\n\
# CONFIG_TOSHIBA_JMR3927 is not set\n\
# CONFIG_MACH_DECSTATION is not set\n\
# CONFIG_MIPS_IVR is not set\n\
# CONFIG_LASAT is not set\n\
# CONFIG_MIPS_ITE8172 is not set\n\
# CONFIG_MIPS_ATLAS is not set\n\
# CONFIG_MIPS_MALTA is not set\n\
# CONFIG_MOMENCO_OCELOT is not set\n\
# CONFIG_MOMENCO_OCELOT_G is not set\n\
# CONFIG_MOMENCO_OCELOT_C is not set\n\
CONFIG_MOMENCO_JAGUAR_ATX=y\n\
CONFIG_JAGUAR_DMALOW=y\n\
# CONFIG_PMC_YOSEMITE is not set\n\
# CONFIG_DDB5476 is not set\n\
# CONFIG_DDB5477 is not set\n\
# CONFIG_NEC_OSPREY is not set\n\
# CONFIG_SGI_IP22 is not set\n\
# CONFIG_SOC_AU1X00 is not set\n\
# CONFIG_SNI_RM200_PCI is not set\n\
# CONFIG_TOSHIBA_RBTX4927 is not set\n\
CONFIG_RWSEM_GENERIC_SPINLOCK=y\n\
CONFIG_HAVE_DEC_LOCK=y\n\
CONFIG_DMA_NONCOHERENT=y\n\
CONFIG_LIMITED_DMA=y\n\
# CONFIG_CPU_LITTLE_ENDIAN is not set\n\
CONFIG_IRQ_CPU=y\n\
CONFIG_IRQ_CPU_RM7K=y\n\
CONFIG_MIPS_MV64340=y\n\
CONFIG_SWAP_IO_SPACE=y\n\
CONFIG_BOOT_ELF32=y\n\
CONFIG_MIPS_L1_CACHE_SHIFT=5\n\
# CONFIG_FB is not set\n\
# CONFIG_CPU_MIPS32 is not set\n\
# CONFIG_CPU_MIPS64 is not set\n\
# CONFIG_CPU_R3000 is not set\n\
# CONFIG_CPU_TX39XX is not set\n\
# CONFIG_CPU_VR41XX is not set\n\
# CONFIG_CPU_R4300 is not set\n\
# CONFIG_CPU_R4X00 is not set\n\
# CONFIG_CPU_TX49XX is not set\n\
# CONFIG_CPU_R5000 is not set\n\
# CONFIG_CPU_R5432 is not set\n\
# CONFIG_CPU_R6000 is not set\n\
# CONFIG_CPU_NEVADA is not set\n\
# CONFIG_CPU_R8000 is not set\n\
# CONFIG_CPU_R10000 is not set\n\
# CONFIG_CPU_RM7000 is not set\n\
CONFIG_CPU_RM9000=y\n\
# CONFIG_CPU_SB1 is not set\n\
CONFIG_PAGE_SIZE_4KB=y\n\
# CONFIG_PAGE_SIZE_8KB is not set\n\
# CONFIG_PAGE_SIZE_16KB is not set\n\
# CONFIG_PAGE_SIZE_64KB is not set\n\
CONFIG_BOARD_SCACHE=y\n\
CONFIG_RM7000_CPU_SCACHE=y\n\
CONFIG_CPU_HAS_PREFETCH=y\n\
# CONFIG_64BIT_PHYS_ADDR is not set\n\
# CONFIG_CPU_ADVANCED is not set\n\
CONFIG_CPU_HAS_LLSC=y\n\
CONFIG_CPU_HAS_LLDSCD=y\n\
CONFIG_CPU_HAS_SYNC=y\n\
CONFIG_HIGHMEM=y\n\
# CONFIG_SMP is not set\n\
# CONFIG_PREEMPT is not set\n\
# CONFIG_DEBUG_SPINLOCK_SLEEP is not set\n\
CONFIG_HW_HAS_PCI=y\n\
CONFIG_PCI=y\n\
CONFIG_PCI_LEGACY_PROC=y\n\
CONFIG_PCI_NAMES=y\n\
CONFIG_MMU=y\n\
CONFIG_BINFMT_ELF=y\n\
# CONFIG_BINFMT_MISC is not set\n\
CONFIG_TRAD_SIGNALS=y\n\
# CONFIG_BINFMT_IRIX is not set\n\
# CONFIG_MTD is not set\n\
# CONFIG_PARPORT is not set\n\
# CONFIG_BLK_DEV_FD is not set\n\
# CONFIG_BLK_CPQ_DA is not set\n\
# CONFIG_BLK_CPQ_CISS_DA is not set\n\
# CONFIG_BLK_DEV_DAC960 is not set\n\
# CONFIG_BLK_DEV_LOOP is not set\n\
# CONFIG_BLK_DEV_NBD is not set\n\
# CONFIG_BLK_DEV_CARMEL is not set\n\
# CONFIG_BLK_DEV_RAM is not set\n\
# CONFIG_LBD is not set\n\
# CONFIG_IDE is not set\n\
# CONFIG_SCSI is not set\n\
# CONFIG_MD is not set\n\
# CONFIG_IEEE1394 is not set\n\
# CONFIG_I2O is not set\n\
CONFIG_NET=y\n\
# CONFIG_PACKET is not set\n\
# CONFIG_NETLINK_DEV is not set\n\
CONFIG_UNIX=y\n\
# CONFIG_NET_KEY is not set\n\
CONFIG_INET=y\n\
# CONFIG_IP_MULTICAST is not set\n\
# CONFIG_IP_ADVANCED_ROUTER is not set\n\
CONFIG_IP_PNP=y\n\
# CONFIG_IP_PNP_DHCP is not set\n\
CONFIG_IP_PNP_BOOTP=y\n\
# CONFIG_IP_PNP_RARP is not set\n\
# CONFIG_NET_IPIP is not set\n\
# CONFIG_NET_IPGRE is not set\n\
# CONFIG_SYN_COOKIES is not set\n\
# CONFIG_INET_AH is not set\n\
# CONFIG_INET_ESP is not set\n\
# CONFIG_INET_IPCOMP is not set\n\
# CONFIG_NETFILTER is not set\n\
# CONFIG_BRIDGE is not set\n\
# CONFIG_VLAN_8021Q is not set\n\
# CONFIG_DECNET is not set\n\
# CONFIG_LLC2 is not set\n\
# CONFIG_IPX is not set\n\
# CONFIG_ATALK is not set\n\
# CONFIG_NET_SCHED is not set\n\
# CONFIG_NET_PKTGEN is not set\n\
# CONFIG_NETPOLL is not set\n\
# CONFIG_NET_POLL_CONTROLLER is not set\n\
# CONFIG_HAMRADIO is not set\n\
# CONFIG_IRDA is not set\n\
# CONFIG_BT is not set\n\
CONFIG_NETDEVICES=y\n\
# CONFIG_DUMMY is not set\n\
# CONFIG_BONDING is not set\n\
# CONFIG_EQUALIZER is not set\n\
# CONFIG_TUN is not set\n\
# CONFIG_ARCNET is not set\n\
CONFIG_NET_ETHERNET=y\n\
CONFIG_MII=y\n\
# CONFIG_HAPPYMEAL is not set\n\
# CONFIG_SUNGEM is not set\n\
# CONFIG_NET_VENDOR_3COM is not set\n\
# CONFIG_NET_TULIP is not set\n\
# CONFIG_HP100 is not set\n\
CONFIG_NET_PCI=y\n\
# CONFIG_PCNET32 is not set\n\
# CONFIG_AMD8111_ETH is not set\n\
# CONFIG_ADAPTEC_STARFIRE is not set\n\
# CONFIG_DGRS is not set\n\
CONFIG_EEPRO100=y\n\
# CONFIG_EEPRO100_PIO is not set\n\
# CONFIG_E100 is not set\n\
# CONFIG_FEALNX is not set\n\
# CONFIG_NATSEMI is not set\n\
# CONFIG_NE2K_PCI is not set\n\
# CONFIG_8139TOO is not set\n\
# CONFIG_SIS900 is not set\n\
# CONFIG_EPIC100 is not set\n\
# CONFIG_SUNDANCE is not set\n\
# CONFIG_TLAN is not set\n\
# CONFIG_VIA_RHINE is not set\n\
# CONFIG_ACENIC is not set\n\
# CONFIG_DL2K is not set\n\
# CONFIG_E1000 is not set\n\
# CONFIG_NS83820 is not set\n\
# CONFIG_HAMACHI is not set\n\
# CONFIG_R8169 is not set\n\
# CONFIG_SK98LIN is not set\n\
# CONFIG_TIGON3 is not set\n\
CONFIG_MV64340_ETH=y\n\
CONFIG_MV64340_ETH_0=y\n\
CONFIG_MV64340_ETH_1=y\n\
CONFIG_MV64340_ETH_2=y\n\
# CONFIG_IXGB is not set\n\
# CONFIG_S2IO is not set\n\
# CONFIG_TR is not set\n\
# CONFIG_NET_RADIO is not set\n\
# CONFIG_WAN is not set\n\
# CONFIG_FDDI is not set\n\
# CONFIG_PPP is not set\n\
# CONFIG_SLIP is not set\n\
# CONFIG_ISDN is not set\n\
# CONFIG_PHONE is not set\n\
# CONFIG_INPUT is not set\n\
# CONFIG_GAMEPORT is not set\n\
CONFIG_SOUND_GAMEPORT=y\n\
# CONFIG_SERIO is not set\n\
# CONFIG_SERIO_I8042 is not set\n\
# CONFIG_VT is not set\n\
# CONFIG_SERIAL_NONSTANDARD is not set\n\
CONFIG_SERIAL_8250=y\n\
CONFIG_SERIAL_8250_CONSOLE=y\n\
CONFIG_SERIAL_8250_NR_UARTS=4\n\
# CONFIG_SERIAL_8250_EXTENDED is not set\n\
CONFIG_SERIAL_CORE=y\n\
CONFIG_SERIAL_CORE_CONSOLE=y\n\
CONFIG_UNIX98_PTYS=y\n\
CONFIG_LEGACY_PTYS=y\n\
CONFIG_LEGACY_PTY_COUNT=256\n\
# CONFIG_QIC02_TAPE is not set\n\
# CONFIG_IPMI_HANDLER is not set\n\
# CONFIG_WATCHDOG is not set\n\
# CONFIG_RTC is not set\n\
# CONFIG_GEN_RTC is not set\n\
# CONFIG_DTLK is not set\n\
# CONFIG_R3964 is not set\n\
# CONFIG_APPLICOM is not set\n\
# CONFIG_FTAPE is not set\n\
# CONFIG_AGP is not set\n\
# CONFIG_DRM is not set\n\
# CONFIG_RAW_DRIVER is not set\n\
# CONFIG_I2C is not set\n\
# CONFIG_VIDEO_DEV is not set\n\
# CONFIG_DVB is not set\n\
# CONFIG_SOUND is not set\n\
# CONFIG_USB is not set\n\
# CONFIG_USB_GADGET is not set\n\
# CONFIG_EXT2_FS is not set\n\
# CONFIG_EXT3_FS is not set\n\
# CONFIG_JBD is not set\n\
# CONFIG_REISERFS_FS is not set\n\
# CONFIG_JFS_FS is not set\n\
# CONFIG_XFS_FS is not set\n\
# CONFIG_MINIX_FS is not set\n\
# CONFIG_ROMFS_FS is not set\n\
# CONFIG_QUOTA is not set\n\
# CONFIG_AUTOFS_FS is not set\n\
# CONFIG_AUTOFS4_FS is not set\n\
# CONFIG_ISO9660_FS is not set\n\
# CONFIG_UDF_FS is not set\n\
# CONFIG_FAT_FS is not set\n\
# CONFIG_NTFS_FS is not set\n\
CONFIG_PROC_FS=y\n\
CONFIG_PROC_KCORE=y\n\
CONFIG_SYSFS=y\n\
# CONFIG_DEVPTS_FS_XATTR is not set\n\
# CONFIG_TMPFS is not set\n\
# CONFIG_HUGETLB_PAGE is not set\n\
CONFIG_RAMFS=y\n\
# CONFIG_HFSPLUS_FS is not set\n\
# CONFIG_CRAMFS is not set\n\
# CONFIG_VXFS_FS is not set\n\
# CONFIG_HPFS_FS is not set\n\
# CONFIG_QNX4FS_FS is not set\n\
# CONFIG_SYSV_FS is not set\n\
# CONFIG_UFS_FS is not set\n\
CONFIG_NFS_FS=y\n\
# CONFIG_NFS_V3 is not set\n\
# CONFIG_NFSD is not set\n\
CONFIG_ROOT_NFS=y\n\
CONFIG_LOCKD=y\n\
# CONFIG_EXPORTFS is not set\n\
CONFIG_SUNRPC=y\n\
# CONFIG_SMB_FS is not set\n\
# CONFIG_CIFS is not set\n\
# CONFIG_NCP_FS is not set\n\
# CONFIG_CODA_FS is not set\n\
# CONFIG_PARTITION_ADVANCED is not set\n\
CONFIG_MSDOS_PARTITION=y\n\
# CONFIG_NLS is not set\n\
CONFIG_CROSSCOMPILE=y\n\
CONFIG_CMDLINE=\"\"\n\
# CONFIG_DEBUG_KERNEL is not set\n\
# CONFIG_SECURITY is not set\n\
# CONFIG_CRYPTO is not set\n\
# CONFIG_CRC32 is not set\n\
# CONFIG_LIBCRC32C is not set\n\
CONFIG_END=n\n";
#endif /* _IKCONFIG_H */
