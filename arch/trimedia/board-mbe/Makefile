#
# Makefile for arch/trimedia/board-sim/
#

EXTRA_CFLAGS += $(NDKCFLAGS) -DTMFL_LCP_EXCLUDE_NANDFLASH -DTMFL_LCP_EXCLUDE_NORFLASH -DTMFL_LCP_EXCLUDE_AUDIO -DTMFL_LCP_EXCLUDE_VIDEO -Iarch/trimedia/board-mbe/

obj-y += mdsbslLcp.o  mdsbslLcp_IntDrv.o  mdsbslLcp_OsSpecific.o ndk_wrapper.o
