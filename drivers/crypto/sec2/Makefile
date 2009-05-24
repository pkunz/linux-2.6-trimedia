#
# 2.6 Makefile for sec2x device
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.


#EXTRA_CFLAGS += -DDBG

sec2drv-objs := sec2_init.o sec2_cha.o sec2_dpd.o sec2_io.o sec2_ioctl.o sec2_request.o sec2isr.o sec2_sctrMap.o

obj-$(CONFIG_MPC8349E_SEC20)	+= sec2drv.o
obj-$(CONFIG_MPC854x_SEC21)	+= sec2drv.o
