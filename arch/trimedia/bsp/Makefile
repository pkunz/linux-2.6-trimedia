#
# Makefile for arch/trimedia/bsp/
#

EXTRA_CFLAGS += $(NDKCFLAGS)

obj-y += tmBootInfo/ tmbslCore/  

ifneq ($(CONFIG_TTISIM),y)
ifneq ($(CONFIG_OLD_TTISIM),y)
obj-y += tmhwXio/ tmhwGpio/ tmbslTmOnly/ tmbslPhyDp83847/
obj-$(CONFIG_PCI) += tmhwPci/
obj-$(CONFIG_NXP_LAN100) += tmhwEth/ 
endif 
endif 
