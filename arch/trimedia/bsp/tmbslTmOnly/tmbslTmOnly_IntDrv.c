/* 
 * Copyright (C) 2009 Gulessoft , Inc. 
 * Written by Guo Hongruan (guo.hongruan@gulessoft.com)
 *
 * Note: rewrite the IntDrv for tmbslTmOnly. It should never be used in linux kernel,
 *  	so just stubs.
 */

#if   ((TMFL_PNX_ID != 1500) && (TMFL_PNX_ID != 1700))
#error This component can only be built for PNX1500 and PNC1700 targets
#endif

#if   (TMFL_PNX_ID == 1500)
#include <tmbslPnx1500.h>               
#elif (TMFL_PNX_ID == 1700)
#include <tmbslPnx1700.h>              
#endif
#include "tmTmOnlyMmio.h"
#include "tmbslTmOnly.h"
#include <tmosal.h>                     
#include <tmbslInterrupt.h>             
#include <tmInterrupts.h>               

#include <tmFlags.h>                    
#include <tmSysCfg.h>                   

#if   (TMFL_PNX_ID == 1500)
#define tmbslTmOnly_IntDrvClearInt         tmbslPnx1500_IntDrvClearInt
#define tmbslTmOnly_IntDrvDeinit           tmbslPnx1500_IntDrvDeinit
#define tmbslTmOnly_IntDrvDelInstance      tmbslPnx1500_IntDrvDelInstance
#define tmbslTmOnly_IntDrvGetInstance      tmbslPnx1500_IntDrvGetInstance
#define tmbslTmOnly_IntDrvGetSwVersion     tmbslPnx1500_IntDrvGetSwVersion
#define tmbslTmOnly_IntDrvInit             tmbslPnx1500_IntDrvInit
#define tmbslTmOnly_IntDrvRaiseInt         tmbslPnx1500_IntDrvRaiseInt
#define tmbslTmOnly_IntDrvSetGlobalPrio    tmbslPnx1500_IntDrvSetGlobalPrio
#define tmbslTmOnly_IntDrvSetIntPrio       tmbslPnx1500_IntDrvSetIntPrio
#define tmbslTmOnly_IntDrvEnableInt        tmbslPnx1500_IntDrvEnableInt
#define tmbslTmOnly_VicNonIntHandler       tmbslPnx1500_VicNonIntHandler
#define tmbslTmOnly_VicIntHandler          tmbslPnx1500_VicIntHandler
#define tmbslTmOnly_VicIntHandler0         tmbslPnx1500_VicIntHandler0
#define tmbslTmOnly_VicIntHandler1         tmbslPnx1500_VicIntHandler1
#define tmbslTmOnly_VicIntHandler2         tmbslPnx1500_VicIntHandler2
#define tmbslTmOnly_VicIntHandler3         tmbslPnx1500_VicIntHandler3
#define tmbslTmOnly_VicIntHandler4         tmbslPnx1500_VicIntHandler4
#define tmbslTmOnly_VicIntHandler5         tmbslPnx1500_VicIntHandler5
#define tmbslTmOnly_VicIntHandler6         tmbslPnx1500_VicIntHandler6
#define tmbslTmOnly_VicIntHandler7         tmbslPnx1500_VicIntHandler7
#define tmbslTmOnly_VicIntHandler8         tmbslPnx1500_VicIntHandler8
#define tmbslTmOnly_VicIntHandler9         tmbslPnx1500_VicIntHandler9
#define tmbslTmOnly_VicIntHandler10        tmbslPnx1500_VicIntHandler10
#define tmbslTmOnly_VicIntHandler11        tmbslPnx1500_VicIntHandler11
#define tmbslTmOnly_VicIntHandler12        tmbslPnx1500_VicIntHandler12
#define tmbslTmOnly_VicIntHandler13        tmbslPnx1500_VicIntHandler13
#define tmbslTmOnly_VicIntHandler14        tmbslPnx1500_VicIntHandler14
#define tmbslTmOnly_VicIntHandler15        tmbslPnx1500_VicIntHandler15
#define tmbslTmOnly_VicIntHandler16        tmbslPnx1500_VicIntHandler16
#define tmbslTmOnly_VicIntHandler17        tmbslPnx1500_VicIntHandler17
#define tmbslTmOnly_VicIntHandler18        tmbslPnx1500_VicIntHandler18
#define tmbslTmOnly_VicIntHandler19        tmbslPnx1500_VicIntHandler19
#define tmbslTmOnly_VicIntHandler20        tmbslPnx1500_VicIntHandler20
#define tmbslTmOnly_VicIntHandler21        tmbslPnx1500_VicIntHandler21
#define tmbslTmOnly_VicIntHandler22        tmbslPnx1500_VicIntHandler22
#define tmbslTmOnly_VicIntHandler23        tmbslPnx1500_VicIntHandler23
#define tmbslTmOnly_VicIntHandler24        tmbslPnx1500_VicIntHandler24
#define tmbslTmOnly_VicIntHandler25        tmbslPnx1500_VicIntHandler25
#define tmbslTmOnly_VicIntHandler26        tmbslPnx1500_VicIntHandler26
#define tmbslTmOnly_VicIntHandler27        tmbslPnx1500_VicIntHandler27
#define tmbslTmOnly_VicIntHandler28        tmbslPnx1500_VicIntHandler28
#define tmbslTmOnly_VicIntHandler29        tmbslPnx1500_VicIntHandler29
#define tmbslTmOnly_VicIntHandler30        tmbslPnx1500_VicIntHandler30
#define tmbslTmOnly_VicIntHandler31        tmbslPnx1500_VicIntHandler31
#define tmbslTmOnly_VicIntHandler32        tmbslPnx1500_VicIntHandler32
#define tmbslTmOnly_VicIntHandler33        tmbslPnx1500_VicIntHandler33
#define tmbslTmOnly_VicIntHandler34        tmbslPnx1500_VicIntHandler34
#define tmbslTmOnly_VicIntHandler35        tmbslPnx1500_VicIntHandler35
#define tmbslTmOnly_VicIntHandler36        tmbslPnx1500_VicIntHandler36
#define tmbslTmOnly_VicIntHandler37        tmbslPnx1500_VicIntHandler37
#define tmbslTmOnly_VicIntHandler38        tmbslPnx1500_VicIntHandler38
#define tmbslTmOnly_VicIntHandler39        tmbslPnx1500_VicIntHandler39
#define tmbslTmOnly_VicIntHandler40        tmbslPnx1500_VicIntHandler40
#define tmbslTmOnly_VicIntHandler41        tmbslPnx1500_VicIntHandler41
#define tmbslTmOnly_VicIntHandler42        tmbslPnx1500_VicIntHandler42
#define tmbslTmOnly_VicIntHandler43        tmbslPnx1500_VicIntHandler43
#define tmbslTmOnly_VicIntHandler44        tmbslPnx1500_VicIntHandler44
#define tmbslTmOnly_VicIntHandler45        tmbslPnx1500_VicIntHandler45
#define tmbslTmOnly_VicIntHandler46        tmbslPnx1500_VicIntHandler46
#define tmbslTmOnly_VicIntHandler47        tmbslPnx1500_VicIntHandler47
#define tmbslTmOnly_VicIntHandler48        tmbslPnx1500_VicIntHandler48
#define tmbslTmOnly_VicIntHandler49        tmbslPnx1500_VicIntHandler49
#define tmbslTmOnly_VicIntHandler50        tmbslPnx1500_VicIntHandler50
#define tmbslTmOnly_VicIntHandler51        tmbslPnx1500_VicIntHandler51
#define tmbslTmOnly_VicIntHandler52        tmbslPnx1500_VicIntHandler52
#define tmbslTmOnly_VicIntHandler53        tmbslPnx1500_VicIntHandler53
#define tmbslTmOnly_VicIntHandler54        tmbslPnx1500_VicIntHandler54
#define tmbslTmOnly_VicIntHandler55        tmbslPnx1500_VicIntHandler55
#define tmbslTmOnly_VicIntHandler56        tmbslPnx1500_VicIntHandler56
#define tmbslTmOnly_VicIntHandler57        tmbslPnx1500_VicIntHandler57
#define tmbslTmOnly_VicIntHandler58        tmbslPnx1500_VicIntHandler58
#define tmbslTmOnly_VicIntHandler59        tmbslPnx1500_VicIntHandler59
#define tmbslTmOnly_VicIntHandler60        tmbslPnx1500_VicIntHandler60
#define tmbslTmOnly_VicIntHandler61        tmbslPnx1500_VicIntHandler61
#define tmbslTmOnly_VicIntHandler62        tmbslPnx1500_VicIntHandler62
#define tmbslTmOnly_VicIntHandler63        tmbslPnx1500_VicIntHandler63


#elif (TMFL_PNX_ID == 1700)
#define tmbslTmOnly_IntDrvClearInt         tmbslPnx1700_IntDrvClearInt
#define tmbslTmOnly_IntDrvDeinit           tmbslPnx1700_IntDrvDeinit
#define tmbslTmOnly_IntDrvDelInstance      tmbslPnx1700_IntDrvDelInstance
#define tmbslTmOnly_IntDrvGetInstance      tmbslPnx1700_IntDrvGetInstance
#define tmbslTmOnly_IntDrvGetSwVersion     tmbslPnx1700_IntDrvGetSwVersion
#define tmbslTmOnly_IntDrvInit             tmbslPnx1700_IntDrvInit
#define tmbslTmOnly_IntDrvRaiseInt         tmbslPnx1700_IntDrvRaiseInt
#define tmbslTmOnly_IntDrvSetGlobalPrio    tmbslPnx1700_IntDrvSetGlobalPrio
#define tmbslTmOnly_IntDrvSetIntPrio       tmbslPnx1700_IntDrvSetIntPrio
#define tmbslTmOnly_IntDrvEnableInt        tmbslPnx1700_IntDrvEnableInt
#define tmbslTmOnly_VicNonIntHandler       tmbslPnx1700_VicNonIntHandler
#define tmbslTmOnly_VicIntHandler          tmbslPnx1700_VicIntHandler
#define tmbslTmOnly_VicIntHandler0         tmbslPnx1700_VicIntHandler0
#define tmbslTmOnly_VicIntHandler1         tmbslPnx1700_VicIntHandler1
#define tmbslTmOnly_VicIntHandler2         tmbslPnx1700_VicIntHandler2
#define tmbslTmOnly_VicIntHandler3         tmbslPnx1700_VicIntHandler3
#define tmbslTmOnly_VicIntHandler4         tmbslPnx1700_VicIntHandler4
#define tmbslTmOnly_VicIntHandler5         tmbslPnx1700_VicIntHandler5
#define tmbslTmOnly_VicIntHandler6         tmbslPnx1700_VicIntHandler6
#define tmbslTmOnly_VicIntHandler7         tmbslPnx1700_VicIntHandler7
#define tmbslTmOnly_VicIntHandler8         tmbslPnx1700_VicIntHandler8
#define tmbslTmOnly_VicIntHandler9         tmbslPnx1700_VicIntHandler9
#define tmbslTmOnly_VicIntHandler10        tmbslPnx1700_VicIntHandler10
#define tmbslTmOnly_VicIntHandler11        tmbslPnx1700_VicIntHandler11
#define tmbslTmOnly_VicIntHandler12        tmbslPnx1700_VicIntHandler12
#define tmbslTmOnly_VicIntHandler13        tmbslPnx1700_VicIntHandler13
#define tmbslTmOnly_VicIntHandler14        tmbslPnx1700_VicIntHandler14
#define tmbslTmOnly_VicIntHandler15        tmbslPnx1700_VicIntHandler15
#define tmbslTmOnly_VicIntHandler16        tmbslPnx1700_VicIntHandler16
#define tmbslTmOnly_VicIntHandler17        tmbslPnx1700_VicIntHandler17
#define tmbslTmOnly_VicIntHandler18        tmbslPnx1700_VicIntHandler18
#define tmbslTmOnly_VicIntHandler19        tmbslPnx1700_VicIntHandler19
#define tmbslTmOnly_VicIntHandler20        tmbslPnx1700_VicIntHandler20
#define tmbslTmOnly_VicIntHandler21        tmbslPnx1700_VicIntHandler21
#define tmbslTmOnly_VicIntHandler22        tmbslPnx1700_VicIntHandler22
#define tmbslTmOnly_VicIntHandler23        tmbslPnx1700_VicIntHandler23
#define tmbslTmOnly_VicIntHandler24        tmbslPnx1700_VicIntHandler24
#define tmbslTmOnly_VicIntHandler25        tmbslPnx1700_VicIntHandler25
#define tmbslTmOnly_VicIntHandler26        tmbslPnx1700_VicIntHandler26
#define tmbslTmOnly_VicIntHandler27        tmbslPnx1700_VicIntHandler27
#define tmbslTmOnly_VicIntHandler28        tmbslPnx1700_VicIntHandler28
#define tmbslTmOnly_VicIntHandler29        tmbslPnx1700_VicIntHandler29
#define tmbslTmOnly_VicIntHandler30        tmbslPnx1700_VicIntHandler30
#define tmbslTmOnly_VicIntHandler31        tmbslPnx1700_VicIntHandler31
#define tmbslTmOnly_VicIntHandler32        tmbslPnx1700_VicIntHandler32
#define tmbslTmOnly_VicIntHandler33        tmbslPnx1700_VicIntHandler33
#define tmbslTmOnly_VicIntHandler34        tmbslPnx1700_VicIntHandler34
#define tmbslTmOnly_VicIntHandler35        tmbslPnx1700_VicIntHandler35
#define tmbslTmOnly_VicIntHandler36        tmbslPnx1700_VicIntHandler36
#define tmbslTmOnly_VicIntHandler37        tmbslPnx1700_VicIntHandler37
#define tmbslTmOnly_VicIntHandler38        tmbslPnx1700_VicIntHandler38
#define tmbslTmOnly_VicIntHandler39        tmbslPnx1700_VicIntHandler39
#define tmbslTmOnly_VicIntHandler40        tmbslPnx1700_VicIntHandler40
#define tmbslTmOnly_VicIntHandler41        tmbslPnx1700_VicIntHandler41
#define tmbslTmOnly_VicIntHandler42        tmbslPnx1700_VicIntHandler42
#define tmbslTmOnly_VicIntHandler43        tmbslPnx1700_VicIntHandler43
#define tmbslTmOnly_VicIntHandler44        tmbslPnx1700_VicIntHandler44
#define tmbslTmOnly_VicIntHandler45        tmbslPnx1700_VicIntHandler45
#define tmbslTmOnly_VicIntHandler46        tmbslPnx1700_VicIntHandler46
#define tmbslTmOnly_VicIntHandler47        tmbslPnx1700_VicIntHandler47
#define tmbslTmOnly_VicIntHandler48        tmbslPnx1700_VicIntHandler48
#define tmbslTmOnly_VicIntHandler49        tmbslPnx1700_VicIntHandler49
#define tmbslTmOnly_VicIntHandler50        tmbslPnx1700_VicIntHandler50
#define tmbslTmOnly_VicIntHandler51        tmbslPnx1700_VicIntHandler51
#define tmbslTmOnly_VicIntHandler52        tmbslPnx1700_VicIntHandler52
#define tmbslTmOnly_VicIntHandler53        tmbslPnx1700_VicIntHandler53
#define tmbslTmOnly_VicIntHandler54        tmbslPnx1700_VicIntHandler54
#define tmbslTmOnly_VicIntHandler55        tmbslPnx1700_VicIntHandler55
#define tmbslTmOnly_VicIntHandler56        tmbslPnx1700_VicIntHandler56
#define tmbslTmOnly_VicIntHandler57        tmbslPnx1700_VicIntHandler57
#define tmbslTmOnly_VicIntHandler58        tmbslPnx1700_VicIntHandler58
#define tmbslTmOnly_VicIntHandler59        tmbslPnx1700_VicIntHandler59
#define tmbslTmOnly_VicIntHandler60        tmbslPnx1700_VicIntHandler60
#define tmbslTmOnly_VicIntHandler61        tmbslPnx1700_VicIntHandler61
#define tmbslTmOnly_VicIntHandler62        tmbslPnx1700_VicIntHandler62
#define tmbslTmOnly_VicIntHandler63        tmbslPnx1700_VicIntHandler63
#endif

tmErrorCode_t
tmbslTmOnly_IntDrvInit (
		ptmbslIntInfoEntry_t pBslIntInfoTable,  
		UInt32               bslIntInfoCount    
		)
{
	return TM_OK;
} 

tmErrorCode_t
tmbslTmOnly_IntDrvDeinit (
		void
		)
{
	return TM_OK;
}

tmErrorCode_t
tmbslTmOnly_IntDrvGetSwVersion (
		ptmSWVersion_t  pIntDrvVersionInfo  
		)
{
	return (TM_OK);

}

tmErrorCode_t
tmbslTmOnly_IntDrvSetGlobalPrio (
		tmosalIntPriority_t osalIntPriority
		)
{
	return TM_OK;
} 

tmErrorCode_t
tmbslTmOnly_IntDrvGetInstance (
		String              intName,        
		tmosalIntHandle_t  *ppOsalIntInfo   
		)
{
	return (TMBSL_ERR_MGR_NO_COMPONENT);
}

tmErrorCode_t
tmbslTmOnly_IntDrvDelInstance (
		tmosalIntHandle_t   pOsalIntInfo    
		)
{
	return TM_OK;
}

tmErrorCode_t
tmbslTmOnly_IntDrvSetIntPrio (
		tmosalIntHandle_t   pOsalIntInfo,  
		tmosalIntPriority_t osalIntPriority
		)
{
	return TM_OK;
}

tmErrorCode_t
tmbslTmOnly_IntDrvEnableInt (
		tmosalIntHandle_t   pOsalIntInfo,   
		Bool                intEnable       
		)
{
	return TM_OK;
} 

tmErrorCode_t
tmbslTmOnly_IntDrvRaiseInt (
		tmosalIntHandle_t   pOsalIntInfo    
		)
{
	return TM_OK;
}


tmErrorCode_t
tmbslTmOnly_IntDrvClearInt (
		tmosalIntHandle_t   pOsalIntInfo   
		)
{
	return TM_OK;
}

pVoid 
tmbslTmOnly_VicIntHandler (            
		pVoid       inputParm 
		)
{
	return (Null);
}


pVoid 
tmbslTmOnly_VicNonIntHandler (       
		pVoid       inputParm
		)
{
	return (Null);
}


#define VIC_INTERRUPT_HANDLER(num) 

void tmbslTmOnly_VicIntHandler0(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(0);
}
void tmbslTmOnly_VicIntHandler1(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(1);
}
void tmbslTmOnly_VicIntHandler2(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(2);
}
void tmbslTmOnly_VicIntHandler3(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(3);
}
void tmbslTmOnly_VicIntHandler4(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(4);
}
void tmbslTmOnly_VicIntHandler5(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(5);
}
void tmbslTmOnly_VicIntHandler6(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(6);
}
void tmbslTmOnly_VicIntHandler7(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(7);
}
void tmbslTmOnly_VicIntHandler8(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(8);
}
void tmbslTmOnly_VicIntHandler9(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(9);
}
void tmbslTmOnly_VicIntHandler10(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(10);
}
void tmbslTmOnly_VicIntHandler11(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(11);
}
void tmbslTmOnly_VicIntHandler12(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(12);
}
void tmbslTmOnly_VicIntHandler13(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(13);
}
void tmbslTmOnly_VicIntHandler14(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(14);
}
void tmbslTmOnly_VicIntHandler15(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(15);
}
void tmbslTmOnly_VicIntHandler16(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(16);
}
void tmbslTmOnly_VicIntHandler17(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(17);
}
void tmbslTmOnly_VicIntHandler18(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(18);
}
void tmbslTmOnly_VicIntHandler19(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(19);
}
void tmbslTmOnly_VicIntHandler20(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(20);
}
void tmbslTmOnly_VicIntHandler21(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(21);
}
void tmbslTmOnly_VicIntHandler22(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(22);
}
void tmbslTmOnly_VicIntHandler23(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(23);
}
void tmbslTmOnly_VicIntHandler24(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(24);
}
void tmbslTmOnly_VicIntHandler25(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(25);
}
void tmbslTmOnly_VicIntHandler26(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(26);
}
void tmbslTmOnly_VicIntHandler27(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(27);
}
void tmbslTmOnly_VicIntHandler28(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(28);
}
void tmbslTmOnly_VicIntHandler29(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(29);
}
void tmbslTmOnly_VicIntHandler30(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(30);
}
void tmbslTmOnly_VicIntHandler31(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(31);
}
void tmbslTmOnly_VicIntHandler32(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(32);
}
void tmbslTmOnly_VicIntHandler33(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(33);
}
void tmbslTmOnly_VicIntHandler34(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(34);
}
void tmbslTmOnly_VicIntHandler35(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(35);
}
void tmbslTmOnly_VicIntHandler36(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(36);
}
void tmbslTmOnly_VicIntHandler37(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(37);
}
void tmbslTmOnly_VicIntHandler38(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(38);
}
void tmbslTmOnly_VicIntHandler39(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(39);
}
void tmbslTmOnly_VicIntHandler40(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(40);
}
void tmbslTmOnly_VicIntHandler41(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(41);
}
void tmbslTmOnly_VicIntHandler42(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(42);
}
void tmbslTmOnly_VicIntHandler43(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(43);
}
void tmbslTmOnly_VicIntHandler44(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(44);
}
void tmbslTmOnly_VicIntHandler45(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(45);
}
void tmbslTmOnly_VicIntHandler46(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(46);
}
void tmbslTmOnly_VicIntHandler47(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(47);
}
void tmbslTmOnly_VicIntHandler48(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(48);
}
void tmbslTmOnly_VicIntHandler49(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(49);
}
void tmbslTmOnly_VicIntHandler50(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(50);
}
void tmbslTmOnly_VicIntHandler51(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(51);
}
void tmbslTmOnly_VicIntHandler52(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(52);
}
void tmbslTmOnly_VicIntHandler53(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(53);
}
void tmbslTmOnly_VicIntHandler54(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(54);
}
void tmbslTmOnly_VicIntHandler55(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(55);
}
void tmbslTmOnly_VicIntHandler56(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(56);
}
void tmbslTmOnly_VicIntHandler57(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(57);
}
void tmbslTmOnly_VicIntHandler58(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(58);
}
void tmbslTmOnly_VicIntHandler59(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(59);
}
void tmbslTmOnly_VicIntHandler60(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(60);
}
void tmbslTmOnly_VicIntHandler61(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(61);
}
void tmbslTmOnly_VicIntHandler62(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(62);
}
void tmbslTmOnly_VicIntHandler63(void) {
#pragma TCS_handler
	VIC_INTERRUPT_HANDLER(63);
}
