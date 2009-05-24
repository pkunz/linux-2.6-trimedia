#
# Makefile for arch/trimedia/board-sim/
#

EXTRA_CFLAGS += $(NDKCFLAGS) -DTMFL_OS_IS_PSOS=1 -Iarch/trimedia/board-sim/

ifeq ($(CONFIG_PNX1005),y)
EXTRA_CFLAGS += -D__TCS_tm3282Minimal__=1
endif 

obj-y += tmbslNull.o tmbslNull_OsSpecific.o tmbslNull_IntDrv.o ndk_wrapper.o
