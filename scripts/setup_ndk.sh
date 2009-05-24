#!/bin/sh

#
# arch/trimedia/bsp/intfs/
#
rm -f arch/trimedia/bsp/intfs/Itmosal 
rm -f arch/trimedia/bsp/intfs/ItmbslPhy

ln -s $NDK/sd/os/osgeneric/intfs/Itmosal arch/trimedia/bsp/intfs/
ln -s $NDK/sd/os/osgeneric/intfs/ItmbslPhy arch/trimedia/bsp/intfs/

#
# arch/trimedia/bsp/tmBootInfo/  
#
rm -f arch/trimedia/bsp/tmBootInfo/tmBootInfo.c 
rm -f arch/trimedia/bsp/tmBootInfo/tmBootInfoCommon.h 
rm -f arch/trimedia/bsp/tmBootInfo/tmMemUncached.c 
rm -f arch/trimedia/bsp/tmBootInfo/tmBootInfoUtil.c 
rm -f arch/trimedia/bsp/tmBootInfo/inc 

ln -s $NDK/sd/os/osgeneric/comps/tmBootInfo/src/tmBootInfo.c arch/trimedia/bsp/tmBootInfo/
ln -s $NDK/sd/os/osgeneric/comps/tmBootInfo/src/tmBootInfoCommon.h arch/trimedia/bsp/tmBootInfo/
ln -s $NDK/sd/os/osgeneric/comps/tmBootInfo/src/tmMemUncached.c arch/trimedia/bsp/tmBootInfo/
ln -s $NDK/sd/os/osgeneric/comps/tmBootInfo/src/tm_psos/tmBootInfoUtil.c arch/trimedia/bsp/tmBootInfo/
ln -s $NDK/sd/os/osgeneric/comps/tmBootInfo/inc arch/trimedia/bsp/tmBootInfo/

#
# arch/trimedia/bsp/tmbslCore/
#
rm -f arch/trimedia/bsp/tmbslCore/tmbslCore_vsnprintf.c
rm -f arch/trimedia/bsp/tmbslCore/tmbslCore_vsnprintf.h
rm -f arch/trimedia/bsp/tmbslCore/tmbslMgr.c
rm -f arch/trimedia/bsp/tmbslCore/tmbslOsSpecific.c
rm -f arch/trimedia/bsp/tmbslCore/inc

ln -s $NDK/sd/os/oshome/comps/tmbslCore/src/tmbslCore_vsnprintf.c arch/trimedia/bsp/tmbslCore/
ln -s $NDK/sd/os/oshome/comps/tmbslCore/src/tmbslCore_vsnprintf.h arch/trimedia/bsp/tmbslCore/
ln -s $NDK/sd/os/oshome/comps/tmbslCore/src/tmbslMgr.c arch/trimedia/bsp/tmbslCore/
cp -f $NDK/sd/os/oshome/comps/tmbslCore/src/tm_psos/tmbslOsSpecific.c arch/trimedia/bsp/tmbslCore/
ln -s $NDK/sd/os/oshome/comps/tmbslCore/inc arch/trimedia/bsp/tmbslCore/

#
# arch/trimedia/bsp/tmbslPnx1500/
#
rm -f arch/trimedia/bsp/tmbslPnx1500/inc

ln -s $NDK/sd/os/osinfra/comps/tmbslPnx1500/inc arch/trimedia/bsp/tmbslPnx1500/

#
# arch/trimedia/bsp/tmbslTmOnly/
#
rm -f arch/trimedia/bsp/tmbslTmOnly/dummy_tmtdosal.c 
rm -f arch/trimedia/bsp/tmbslTmOnly/tmbslTmOnly.c
rm -f arch/trimedia/bsp/tmbslTmOnly/tmbslTmOnly_Exception.s
rm -f arch/trimedia/bsp/tmbslTmOnly/tmbslTmOnly_OsSpecific.c
rm -f arch/trimedia/bsp/tmbslTmOnly/tmTmOnlyMmio.h
rm -f arch/trimedia/bsp/tmbslTmOnly/tmbslTmOnly.h

ln -s $NDK/sd/os/osinfra/comps/tmbslTmOnly/src/dummy_tmtdosal.c arch/trimedia/bsp/tmbslTmOnly/
ln -s $NDK/sd/os/osinfra/comps/tmbslTmOnly/src/tmbslTmOnly.c arch/trimedia/bsp/tmbslTmOnly/
ln -s $NDK/sd/os/osinfra/comps/tmbslTmOnly/src/tm_psos/tmbslTmOnly_Exception.s arch/trimedia/bsp/tmbslTmOnly/
ln -s $NDK/sd/os/osinfra/comps/tmbslTmOnly/src/tm_psos/tmbslTmOnly_OsSpecific.c arch/trimedia/bsp/tmbslTmOnly/
ln -s $NDK/sd/os/osinfra/comps/tmbslTmOnly/src/tmTmOnlyMmio.h arch/trimedia/bsp/tmbslTmOnly/
ln -s $NDK/sd/os/osinfra/comps/tmbslTmOnly/src/tmbslTmOnly.h arch/trimedia/bsp/tmbslTmOnly/

#
# arch/trimedia/bsp/tmhwPci/
#
rm -f arch/trimedia/bsp/tmhwPci/tmhwPci.c 
rm -f arch/trimedia/bsp/tmhwPci/tmhwPciInternal.h 
rm -f arch/trimedia/bsp/tmhwPci/tmhwPciIo.c 
rm -f arch/trimedia/bsp/tmhwPci/tmhwPciMemSpace.c
rm -f arch/trimedia/bsp/tmhwPci/inc

ln -s $NDK/sd/os/osgeneric/comps/tmhwPci/src/tmhwPci.c arch/trimedia/bsp/tmhwPci/
ln -s $NDK/sd/os/osgeneric/comps/tmhwPci/src/tmhwPciInternal.h arch/trimedia/bsp/tmhwPci/
ln -s $NDK/sd/os/osgeneric/comps/tmhwPci/src/tmhwPciIo.c arch/trimedia/bsp/tmhwPci/
ln -s $NDK/sd/os/osgeneric/comps/tmhwPci/src/tmhwPciMemSpace.c arch/trimedia/bsp/tmhwPci/
ln -s $NDK/sd/os/osgeneric/comps/tmhwPci/inc arch/trimedia/bsp/tmhwPci/

#
# arch/trimedia/bsp/tmhwGpio/
#
rm -f arch/trimedia/bsp/tmhwGpio/pnx1500/tmhwGpio.c 
rm -f arch/trimedia/bsp/tmhwGpio/inc

ln -s $NDK/sd/os/osgeneric/comps/tmhwGpio/src/pnx1500/tmhwGpio.c arch/trimedia/bsp/tmhwGpio/pnx1500/
ln -s $NDK/sd/os/osgeneric/comps/tmhwGpio/inc arch/trimedia/bsp/tmhwGpio/

#
# arch/trimedia/bsp/tmhwEth/
#
rm -f arch/trimedia/bsp/tmhwEth/tmhwEth.c 
rm -f arch/trimedia/bsp/tmhwEth/inc 

ln -s $NDK/sd/os/osgeneric/comps/tmhwEth/src/tmhwEth.c arch/trimedia/bsp/tmhwEth/
ln -s $NDK/sd/os/osgeneric/comps/tmhwEth/inc arch/trimedia/bsp/tmhwEth/

#
# arch/trimedia/bsp/tmhwXio/
#
rm -f arch/trimedia/bsp/tmhwXio/tmhwXio.c 
rm -f arch/trimedia/bsp/tmhwXio/tmhwXioInternal.h 
rm -f arch/trimedia/bsp/tmhwXio/tmhwXio_CfgLocal.h 
rm -f arch/trimedia/bsp/tmhwXio/tmhwXio_DvpCfg.c 
rm -f arch/trimedia/bsp/tmhwXio/tmhwXio_DvpNoBslCfg.c 
rm -f arch/trimedia/bsp/tmhwXio/tmvhPciXio0113_Reg.h 
rm -f arch/trimedia/bsp/tmhwXio/tmvhPciXioA051_Reg.h 
rm -f arch/trimedia/bsp/tmhwXio/inc

ln -s $NDK/sd/os/osgeneric/comps/tmhwXio/src/tmhwXio.c arch/trimedia/bsp/tmhwXio/
ln -s $NDK/sd/os/osgeneric/comps/tmhwXio/src/tmhwXioInternal.h arch/trimedia/bsp/tmhwXio/
ln -s $NDK/sd/os/osgeneric/comps/tmhwXio/cfg/tmhwXio_CfgLocal.h arch/trimedia/bsp/tmhwXio/
ln -s $NDK/sd/os/osgeneric/comps/tmhwXio/cfg/tmhwXio_DvpCfg.c arch/trimedia/bsp/tmhwXio/
ln -s $NDK/sd/os/osgeneric/comps/tmhwXio/cfg/tmhwXio_DvpNoBslCfg.c arch/trimedia/bsp/tmhwXio/
ln -s $NDK/sd/os/osgeneric/comps/tmhwXio/src/tmvhPciXio0113_Reg.h arch/trimedia/bsp/tmhwXio/
ln -s $NDK/sd/os/osgeneric/comps/tmhwXio/src/tmvhPciXioA051_Reg.h arch/trimedia/bsp/tmhwXio/
ln -s $NDK/sd/os/osgeneric/comps/tmhwXio/inc arch/trimedia/bsp/tmhwXio/

#
# arch/trimedia/bsp/tmStdTypes/
#
rm -f arch/trimedia/bsp/tmStdTypes/inc

ln -s $NDK/sd/os/osapi/types/tmStdTypes/inc arch/trimedia/bsp/tmStdTypes/

#
# arch/trimedia/bsp/tmbslPhyDp83847
#
rm -f arch/trimedia/bsp/tmbslPhyDp83847/tmbslPhyDp83847.c 
rm -f arch/trimedia/bsp/tmbslPhyDp83847/tmbslPhyDp83847RegBe.h 
rm -f arch/trimedia/bsp/tmbslPhyDp83847/tmbslPhyDp83847RegLe.h 
rm -f arch/trimedia/bsp/tmbslPhyDp83847/inc

ln -s $NDK/sd/os/osgeneric/comps/tmbslPhyDp83847/src/tmbslPhyDp83847.c arch/trimedia/bsp/tmbslPhyDp83847/
ln -s $NDK/sd/os/osgeneric/comps/tmbslPhyDp83847/src/tmbslPhyDp83847RegBe.h arch/trimedia/bsp/tmbslPhyDp83847/
ln -s $NDK/sd/os/osgeneric/comps/tmbslPhyDp83847/src/tmbslPhyDp83847RegLe.h arch/trimedia/bsp/tmbslPhyDp83847/
ln -s $NDK/sd/os/osgeneric/comps/tmbslPhyDp83847/inc arch/trimedia/bsp/tmbslPhyDp83847/

#
# arch/trimedia/board-mbe/
#
rm -f arch/trimedia/board-mbe/gmdsbslLcp_IntDrv.h 
rm -f arch/trimedia/board-mbe/gmdsbslLcp_CoreUtilities.h 
rm -f arch/trimedia/board-mbe/mdsbslLcp_OsSpecific.c 
rm -f arch/trimedia/board-mbe/mdsbslLcp_IntDrv.c 
rm -f arch/trimedia/board-mbe/mdsbslLcp_Vid.h 
rm -f arch/trimedia/board-mbe/mdsbslLcp_Vid.c 
rm -f arch/trimedia/board-mbe/mdsbslLcp.h 
rm -f arch/trimedia/board-mbe/mdsbslLcp.c 
rm -f arch/trimedia/board-mbe/mdsbslLcp_Aud.h 
rm -f arch/trimedia/board-mbe/mdsbslLcp_Aud.c 

ln -s $NDK/sd/os/osinfra/comps/mdsbslLcp/src/gmdsbslLcp_IntDrv.h arch/trimedia/board-mbe/
ln -s $NDK/sd/os/osinfra/comps/mdsbslLcp/src/gmdsbslLcp_CoreUtilities.h arch/trimedia/board-mbe/
ln -s $NDK/sd/os/osinfra/comps/mdsbslLcp/src/tm_psos/mdsbslLcp_OsSpecific.c arch/trimedia/board-mbe/
ln -s $NDK/sd/os/osinfra/comps/mdsbslLcp/src/tm_psos/mdsbslLcp_IntDrv.c arch/trimedia/board-mbe/
ln -s $NDK/sd/os/osinfra/comps/mdsbslLcp/src/mdsbslLcp_Vid.h arch/trimedia/board-mbe/
ln -s $NDK/sd/os/osinfra/comps/mdsbslLcp/src/mdsbslLcp_Vid.c arch/trimedia/board-mbe/
ln -s $NDK/sd/os/osinfra/comps/mdsbslLcp/src/mdsbslLcp.h arch/trimedia/board-mbe/
cp -f $NDK/sd/os/osinfra/comps/mdsbslLcp/src/mdsbslLcp.c arch/trimedia/board-mbe/
ln -s $NDK/sd/os/osinfra/comps/mdsbslLcp/src/mdsbslLcp_Aud.h arch/trimedia/board-mbe/
ln -s $NDK/sd/os/osinfra/comps/mdsbslLcp/src/mdsbslLcp_Aud.c arch/trimedia/board-mbe/

#
# arch/trimedia/board-sim/
#
rm -f arch/trimedia/board-sim/tmbslNull.c 
rm -f arch/trimedia/board-sim/tmbslNull.h 
rm -f arch/trimedia/board-sim/tmbslNull_IntDrv.c 
rm -f arch/trimedia/board-sim/tmbslNull_IntDrv.h 
rm -f arch/trimedia/board-sim/tmbslNull_Mmio.h 
rm -f arch/trimedia/board-sim/tmbslNull_OsSpecific.c 

ln -s $NDK/sd/os/osinfra/comps/tmbslNull/src/tmbslNull.c arch/trimedia/board-sim/
ln -s $NDK/sd/os/osinfra/comps/tmbslNull/inc/tmbslNull.h arch/trimedia/board-sim/
ln -s $NDK/sd/os/osinfra/comps/tmbslNull/src/tm_psos/tmbslNull_IntDrv.c arch/trimedia/board-sim/
ln -s $NDK/sd/os/osinfra/comps/tmbslNull/inc/tmbslNull_IntDrv.h arch/trimedia/board-sim/
ln -s $NDK/sd/os/osinfra/comps/tmbslNull/inc/tmbslNull_Mmio.h arch/trimedia/board-sim/
cp -f $NDK/sd/os/osinfra/comps/tmbslNull/src/tm_psos/tmbslNull_OsSpecific.c arch/trimedia/board-sim/

