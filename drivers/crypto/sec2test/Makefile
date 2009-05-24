#
# 2.6 Makefile for sec2x device test driver
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.

EXTRA_CFLAGS += -Idrivers/crypto/sec2

sec2drvTest-objs := testAll.o testAesa.o testRng.o testAfha.o \
	testAfhaLong.o testAFHActx.o testKEA.o testXOR.o testTLS.o testICV.o \
        testHmac.o testDES.o testipsec.o testPKHA.o ss_rsa.o morepkhaTest.o \
	hash_test.o ccmp.o ccmpdec.o testSG.o testAux.o dumpRegs.o dump.o

obj-$(CONFIG_MPC8349E_SEC20TEST)	+= sec2drvTest.o
obj-$(CONFIG_MPC854x_SEC21TEST)		+= sec2drvTest.o
