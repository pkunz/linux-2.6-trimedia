#
#
#
EXTRA_CFLAGS = $(NDKCFLAGS) -Iarch/trimedia/bsp/tmhwXio/

ifeq ($(CONFIG_PNX1500),y)
EXTRA_CFLAGS += -DBUILDING_FOR=2
endif 

obj-y += tmhwXio.o tmhwXio_DvpCfg.o
