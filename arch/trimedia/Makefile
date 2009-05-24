#
# arch/trimedia/Makefile
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
#

NDKCFLAGS 	:= 
NDKDEFS 	:= -DTMFL_DVP4_BUILD -DTM_CPU -DTMFL_CPU=TMFL_CPU_IS_TM -DTMFL_OS=TMFL_OS_IS_LINUX
NDKINCS 	:= -Iarch/trimedia/bsp/tmStdTypes/inc/ \
		-Iarch/trimedia/bsp/tmbslCore/inc/ \
		-Iarch/trimedia/bsp/tmhwPci/inc/ \
		-Iarch/trimedia/bsp/tmBootInfo/inc/ \
		-Iarch/trimedia/bsp/tmbslPnx1500/inc/ \
		-Iarch/trimedia/bsp/tmhwGpio/inc/ \
		-Iarch/trimedia/bsp/tmhwXio/inc/ \
		-Iarch/trimedia/bsp/tmhwEth/inc/ \
		-Iarch/trimedia/bsp/tmbslPhyDp83847/inc/ \
		-Iarch/trimedia/bsp/intfs/Itmosal/inc \
		-Iarch/trimedia/bsp/intfs/ItmbslPhy/inc \
		-Iarch/trimedia/bsp/intfs/generated \


#
# Define the TMFL_PNX_ID macro.
#
TMFL_TARGET=PNX0000
TMFL_PNX_ID=9999
ifeq ($(CONFIG_TM1300),y)
TMFL_PNX_ID=1300
endif
ifeq ($(CONFIG_PNX1500),y)
TMFL_PNX_ID=1500
TMFL_TARGET=PNX1500
endif
ifeq ($(CONFIG_PNX1700),y)
TMFL_PNX_ID=1700
TMFL_TARGET=PNX1700
endif
ifeq ($(CONFIG_PNX1005),y)
TMFL_PNX_ID=1005
endif
#
# Correct TMFL_PNX_ID if use trimedia simulator
#
ifeq ($(CONFIG_TTISIM),y)
TMFL_PNX_ID=0000
endif
ifeq ($(CONFIG_OLD_TMSIM),y)
TMFL_PNX_ID=0000
endif

NDKDEFS += -DTMFL_PNX_ID=$(TMFL_PNX_ID) -D$(TMFL_TARGET)

#
# Define the endian flags
#
ifeq ($(CONFIG_BIG_ENDIAN),y)
NDKDEFS += -DTMFL_ENDIAN=TMFL_ENDIAN_BIG
endif
ifeq ($(CONFIG_LITTLE_ENDIAN),y)
NDKDEFS += -DTMFL_ENDIAN=TMFL_ENDIAN_LITTLE
endif

#
# Define TMFL_REL flags and compile flags
#
ifeq ($(CONFIG_TMTGTREL_RETIAL),y)
NDKDEFS += -DTMFL_REL=TMFL_REL_RETAIL
NDKCFLAGS += -O3 -DNDEBUG -DNO_DP
endif 
ifeq ($(CONFIG_TMTGTREL_TRACE),y)
NDKDEFS += -DTMFL_REL=TMFL_REL_TRACE
NDKCFLAGS += -O3 -DTMFL_TRACE 
endif 
ifeq ($(CONFIG_TMTGTREL_ASSERT),y)
NDKDEFS += -DTMFL_REL=TMFL_REL_ASSERT
NDKCFLAGS += -O3 -DNO_DP
endif 
ifeq ($(CONFIG_TMTGTREL_DEBUG),y)
NDKDEFS += -DTMFL_REL=TMFL_REL_DEBUG
NDKCFLAGS += -g -O1 -DDEBUG
endif 

NDKCFLAGS += $(NDKDEFS) $(NDKINCS)

export NDKCFLAGS

LDFLAGS 	:= -chain _init_setup_list,_initcall_1_list,_initcall_2_list,_initcall_3_list,_initcall_4_list,_initcall_5_list,_initcall_6_list,_initcall_7_list,_con_initcall_list,_exitcall_list,___kernel_param_list
ifdef CONFIG_PCI_QUIRKS
LDFLAGS += -chain ___pci_fixup_early_list,___pci_fixup_header_list,___pci_fixup_final_list,___pci_fixup_enable_list
endif 

board-$(CONFIG_SIM1300) 	:= board-sim
board-$(CONFIG_SIM1500) 	:= board-sim
board-$(CONFIG_SIM1700) 	:= board-sim
board-$(CONFIG_SIM1005) 	:= board-sim
board-$(CONFIG_MBE1500) 	:= board-mbe
board-$(CONFIG_LCP1500) 	:= board-lcp
BOARD  := $(board-y)

target-$(CONFIG_TM1300) 	:= tm1
target-$(CONFIG_PNX1500) 	:= pnx1500
target-$(CONFIG_PNX1700) 	:= pnx1700
target-$(CONFIG_PNX1005) 	:= pnx1005
TARGET := $(target-y)

host-$(CONFIG_OLD_TMSIM)   	:= tmsim
host-$(CONFIG_TTISIM)        	:= ttisim
host-$(CONFIG_NOHOST)   	:= nohost
host-$(CONFIG_WINNT)   		:= winnt
HOST   := $(host-y)

endian-$(CONFIG_LITTLE_ENDIAN)  := -el
endian-$(CONFIG_BIG_ENDIAN) 	:= -eb
ENDIAN := $(endian-y)

float-$(CONFIG_FP32)   		:= -fp32
float-$(CONFIG_FP64) 		:= -fp64
FLOAT  := $(float-y)

boot-$(CONFIG_DYNLOAD) 		:= "-btype dynboot"
BOOTTYPE := $(boot-y) 

#
# Some CFLAG additions based on specific CPU type.
#
cflags-y = -host $(HOST) -target $(TARGET) $(ENDIAN) $(FLOAT)

ifndef CONFIG_TRIMEDIA_DP
cflags-y += -DNO_DP
endif

AFLAGS += $(cflags-y) -x

CFLAGS += $(cflags-y)
LDFLAGS_vmlinux += -symbolrename _memchr=_linux_memchr \
	-symbolrename _memmove=_linux_memmove \
	-symbolrename _memcmp=_linux_memcmp \
	-symbolrename _memset=_linux_memset \
	-symbolrename _strlen=_linux_strlen \
	-symbolrename _sprintf=_linux_sprintf \
	-symbolrename _sscanf=_linux_sscanf \
	-symbolrename _strcat=_linux_strcat \
	-symbolrename _strchr=_linux_strchr \
	-symbolrename _strcmp=_linux_strcmp \
	-symbolrename _strcpy=_linux_strcpy \
	-symbolrename _strcspn=_linux_strcspn \
	-symbolrename _strncat=_linux_strncat \
	-symbolrename _strncmp=_linux_strncmp \
	-symbolrename _strncpy=_linux_strncpy \
	-symbolrename _strpbrk=_linux_strpbrk \
	-symbolrename _strrchr=_linux_strrchr \
	-symbolrename _strspn=_linux_strspn \
	-symbolrename _strstr=_linux_strstr \
	-symbolrename _vsprintf=_linux_vsprintf \
	-symbolrename __ctype=_linux__ctype\
	-symbolrename _vsnprintf=_linux_vsnprintf\
	-symbolrename _mktime=_linux_mktime 

core-y	+= arch/trimedia/boot/ \
	   arch/trimedia/kernel/ \
	   arch/trimedia/mm/ \
	   arch/trimedia/bsp/ \
	   arch/trimedia/$(BOARD)/ \
	   arch/trimedia/target/$(TARGET)/
	 
libs-y	+= arch/trimedia/lib/

INCDIR := target-$(target-y)

include/asm-trimedia/.arch: $(wildcard include/config/arch/*.h) include/config/MARKER
	@echo '  SYMLINK include/asm-trimedia/target -> include/asm-trimedia/$(INCDIR)'
ifneq ($(KBUILD_SRC),)
	$(Q)mkdir -p include/asm-trimedia
	$(Q)ln -fsn $(srctree)/include/asm-trimedia/$(INCDIR) include/asm-trimedia/target
else
	$(Q)ln -fsn $(INCDIR) include/asm-trimedia/target
endif
	@touch $@

archprepare: maketools

.PHONY: maketools FORCE
maketools: include/linux/version.h include/asm-trimedia/.arch FORCE

CLEAN_FILES += include/asm-trimedia/target include/asm-trimedia/.arch

boot := arch/trimedia/boot

all: TMLinux 

.NOTPARALLEL: TMLinux 

TMLinux: vmlinux
	$(Q)$(MAKE) $(build)=$(boot) $(boot)/$@ TARGET=$(TARGET) HOST=$(HOST) FLOAT=$(FLOAT) ENDIAN=$(ENDIAN) BOOTTYPE=$(BOOTTYPE)

archclean:
	$(Q)$(MAKE) $(clean)=$(boot) clean

define  archhelp
  echo 	 '* libtmlinux.a -Dynamic boot kernel image for TriMedia (arch/$(ARCH)/boot/libtmlinux.a)' 
endef 
