#################################################################################
#
#   Marvell Serial ATA Linux Driver
#   Copyright 2004 
#   Marvell International Ltd.
#
# This software program (the "Program") is distributed by Marvell International
# ltd. under the terms of the GNU General Public License Version 2, June 1991 
# (the "License").  You may use, redistribute and/or modify this Program in 
# accordance with the terms and conditions of the License, a copy of which is
# available along with the Program in the license.txt file or by writing to the 
# Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
# MA 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
#
# THE PROGRAM IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE 
# IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE
# EXPRESSLY DISCLAIMED.  The License provides additional details about this 
# warranty disclaimer.
#
# For more information about the Program or the License as it applies to the
# Program, please contact Marvell International Ltd. via its affiliate, Marvell
# Semiconductor, Inc., 700 First Avenue, Sunnyvale, CA 94010
#
#################################################################################

#EXTRA_CFLAGS                   += -DDEBUG=1 -DMV_LOG_DEBUG -DLINUX -Idrivers/scsi
EXTRA_CFLAGS                   += -DLINUX -Idrivers/scsi
obj-$(CONFIG_SCSI_MVSATA)      += mvsata.o
mvsata-y	               += mvLinuxIalLib.o \
                                  mvLinuxIalHt.o \
                                  mvLinuxIalOs.o \
                                  mvIALCommon.o \
                                  mvIALCommonUtils.o \
                                  mvLinuxIalSmart.o \
                                  mvSata.o \
                                  mvStorageDev.o \
                                  mvLog.o \
                                  mvScsiAtaLayer.o
