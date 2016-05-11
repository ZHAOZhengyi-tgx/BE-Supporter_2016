// MtnDialog_FbMonitor.cpp : implementation file
// History
// YYYYMMDD Author  Notes
// 20081030 Zhengyi Testing EFO, Trigger by ACS controller OUT0.0 (DigitalOutput0-bit0)
//                  Detection by PCI Interrupt Event 0
// 20090119 Zhengyi Add updating index
// 20090824			Add EFO Monitor by 1739U
// 20091121			E-Torch Burn-in , trig by 1739U
#include "stdafx.h"
#include "MtnDialog_FbMonitor.h"
#include "MtnApi.h"
#include "MtnInitAcs.h"
#include "WinTiming.h"
#include <MMSystem.h>

#define INTER_TRIG_FB_DELAY_MS  (40)
#define DELAY_SWITCH_BSD_AFTER_EFO  (20)

#include "MtnTesterResDef.h"
extern void mtnapi_get_debug_message_home_acs(char strDebugMessageHomeAcs[512]);
//extern char sys_acs_communication_get_flag_sc_udi();

HANDLE stCommHandle_DlgFbMonitor_ACS;	// communication handle
int iFlagMachineType = WB_MACH_TYPE_VLED_FORK;

// 20090119
#include "MtnSearchHome.h"
#include "MtnApiSearchHome.h"
static unsigned int uiCountIndex; 
static int iIndexStateCurr;
static int iFlagInitSearchHomeParameter;

static MTN_API_SEARCH_INDEX_INPUT stSearchHomeInputAPI[8];
static MTN_API_SEARCH_INDEX_OUTPUT stSearchHomeOutputAPI[8];
static MTN_API_SEARCH_LIMIT_INPUT stSearchLimitInputAPI[8];
static MTN_API_SEARCH_LIMIT_OUTPUT stSearchLimitOutputAPI[8];
static MTN_API_SEARCH_HOME_FROM_LIMIT_INPUT stSearchHomeFromLimitInputAPI[8];
static MTN_API_SEARCH_HOME_FROM_LIMIT_OUTPUT stSearchHomeFromLimitOutputAPI[8];

static MTN_SEARCH_LIMIT_INPUT stSearchLimitInput[8];
static MTN_SEARCH_LIMIT_OUTPUT stSearchLimitOutput[8];
static MTN_SEARCH_HOME_FROM_LIMIT_INPUT stSearchHomeFromLimitInput[8];
static MTN_SEARCH_HOME_FROM_LIMIT_OUTPUT stSearchHomeFromLimitOutput[8];
static MTN_SEARCH_INDEX_INPUT stSearchIndexInput[8];
static MTN_SEARCH_INDEX_OUTPUT stSearchIndexOutput[8];

int mtester_fb_search_home_acs_axis(HANDLE stCommHandleACS, int iAcsAxis)
{
	return mtn_search_home_acs(stCommHandleACS, &stSearchIndexInput[iAcsAxis], &stSearchIndexOutput[iAcsAxis]);
}

#define BYTE_EFO_GOOD_NG_FB_13V_13T        2
#define BYTE_EFO_GOOD_NG_FB_ORI_LOT_1    4

static  unsigned short usDigitalIO1739_ReadByteValueEFO_GoodNG;
static  unsigned short usByteRead1739U_EFO_GoodNG = BYTE_EFO_GOOD_NG_FB_ORI_LOT_1; // BYTE_EFO_GOOD_NG_FB_13V_13T;
static  unsigned short usBitPattern1739U_EFO_GoodNG = 0x80;
unsigned int uiTotalCountTriggerEFO_BSD = 72000;

unsigned short FbMonitorDlg_get_address_read_EFO_good_ng()
{
	return usByteRead1739U_EFO_GoodNG;
}

// UI counter
static unsigned int uiCountTriggerEFO;
static unsigned int uiCountSuccessEFO;
static unsigned int uiCountFailTrigEFO;
static unsigned int uiCountTotalEFO;
static unsigned int uiCountTrigEFO_1739U;
#include "MotAlgo_DLL.h"

static char cFlagEnableTestDualFreeAirBall = 0;  // 20121018
char mtest_fb_check_get_flag_enable_dual_fab()
{
	return cFlagEnableTestDualFreeAirBall;
}

void mtest_fb_init_search_limit_home_parameter_vled_bonder()
{
	// EFO
	    usByteRead1739U_EFO_GoodNG = BYTE_EFO_GOOD_NG_FB_ORI_LOT_1;  // 20110504
	// For Table-X
		stSearchIndexInput[0].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_TBL;
		stSearchIndexInput[0].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_TBL_X;
		stSearchIndexInput[0].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_TBL_X;
		stSearchIndexInput[0].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_TBL_X;
		stSearchIndexInput[0].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_X;
		stSearchIndexInput[0].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
		stSearchIndexInput[0].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_X;
		stSearchIndexInput[0].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_TBL;
		stSearchIndexInput[0].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_TBL;
		stSearchIndexInput[0].iAxisOnACS = 0;  // ACS_CARD_AXIS_X
		stSearchIndexInput[0].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_X;
		stSearchIndexInput[0].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
		stSearchIndexInput[0].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_X;
		stSearchIndexInput[0].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;
	// For Table-Y
		stSearchIndexInput[1].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_TBL;
		stSearchIndexInput[1].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_TBL_Y;
		stSearchIndexInput[1].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_TBL_Y;
		stSearchIndexInput[1].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_TBL_Y;
		stSearchIndexInput[1].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_Y;
		stSearchIndexInput[1].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
		stSearchIndexInput[1].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_Y;
		stSearchIndexInput[1].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_TBL;
		stSearchIndexInput[1].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_TBL;
		stSearchIndexInput[1].iAxisOnACS = 1;  // ACS_CARD_AXIS_Y
		stSearchIndexInput[1].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_Y;
		stSearchIndexInput[1].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
		stSearchIndexInput[1].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_Y;
		stSearchIndexInput[1].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;
	// For BondHead-Z
		stSearchIndexInput[4].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_Z;
		stSearchIndexInput[4].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_Z;
		stSearchIndexInput[4].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_Z;
		stSearchIndexInput[4].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_Z;
		stSearchIndexInput[4].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_Z;
		stSearchIndexInput[4].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
		stSearchIndexInput[4].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_Z;
		stSearchIndexInput[4].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_Z;
		stSearchIndexInput[4].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_Z;
		if(sys_acs_communication_get_flag_sc_udi() == TRUE) // 20130228
		{
			stSearchIndexInput[4].iAxisOnACS = ACS_CARD_AXIS_Z;   // 20110111, iAxisZ;
		}
		else
		{
			stSearchIndexInput[4].iAxisOnACS = 4;
		}
		stSearchIndexInput[4].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_Z;
		stSearchIndexInput[4].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
		stSearchIndexInput[4].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_Z;
		stSearchIndexInput[4].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;

		stSearchIndexInput[0].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;
		stSearchIndexInput[1].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;
		stSearchIndexInput[4].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;
}

void mtest_fb_init_search_limit_home_parameter_13v_bonder()
{
	// EFO
	    usByteRead1739U_EFO_GoodNG = BYTE_EFO_GOOD_NG_FB_13V_13T;  // 20110504
	// For Table-X
		stSearchIndexInput[0].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_TBL;
		stSearchIndexInput[0].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_TBL_X;
		stSearchIndexInput[0].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_TBL_X;
		stSearchIndexInput[0].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_TBL_X;
		stSearchIndexInput[0].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_X;
		stSearchIndexInput[0].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
		stSearchIndexInput[0].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_X;
		stSearchIndexInput[0].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_TBL;
		stSearchIndexInput[0].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_TBL;
		stSearchIndexInput[0].iAxisOnACS = 0;  // ACS_CARD_AXIS_X
		stSearchIndexInput[0].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_X;
		stSearchIndexInput[0].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
		stSearchIndexInput[0].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_X;
		stSearchIndexInput[0].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;
	// For Table-Y
		stSearchIndexInput[1].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_TBL;
		stSearchIndexInput[1].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_TBL_Y;
		stSearchIndexInput[1].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_TBL_Y;
		stSearchIndexInput[1].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_TBL_Y;
		stSearchIndexInput[1].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_Y;
		stSearchIndexInput[1].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
		stSearchIndexInput[1].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_Y;
		stSearchIndexInput[1].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_TBL;
		stSearchIndexInput[1].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_TBL;
		stSearchIndexInput[1].iAxisOnACS = 1;  // ACS_CARD_AXIS_Y
		stSearchIndexInput[1].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_Y;
		stSearchIndexInput[1].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
		stSearchIndexInput[1].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_Y;
		stSearchIndexInput[1].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;
	// For BondHead-Z
		stSearchIndexInput[4].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_Z;
		stSearchIndexInput[4].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_Z;
		stSearchIndexInput[4].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_Z;
		stSearchIndexInput[4].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_Z;
		stSearchIndexInput[4].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_Z;
		stSearchIndexInput[4].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
		stSearchIndexInput[4].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_Z;
		stSearchIndexInput[4].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_Z;
		stSearchIndexInput[4].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_Z;
		if(sys_acs_communication_get_flag_sc_udi() == TRUE) // 20130228
		{
			stSearchIndexInput[4].iAxisOnACS = ACS_CARD_AXIS_Z;   // 20110111, iAxisZ;
		}
		else
		{
			stSearchIndexInput[4].iAxisOnACS = 4;
		}
		stSearchIndexInput[4].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_Z;
		stSearchIndexInput[4].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
		stSearchIndexInput[4].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_Z;
		stSearchIndexInput[4].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;

		stSearchIndexInput[0].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;
		stSearchIndexInput[1].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;
		stSearchIndexInput[4].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;
}

void mtest_fb_init_search_limit_home_parameter_hori_bonder()
{
	// EFO
	    usByteRead1739U_EFO_GoodNG = BYTE_EFO_GOOD_NG_FB_13V_13T;  // 20110504
	// For Table-X
		stSearchIndexInput[1].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_TBL;
		stSearchIndexInput[1].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_TBL_X;
		stSearchIndexInput[1].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_TBL_X;
		stSearchIndexInput[1].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_TBL_X;
		stSearchIndexInput[1].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_X;
		stSearchIndexInput[1].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
		stSearchIndexInput[1].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_X;
		stSearchIndexInput[1].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_TBL;
		stSearchIndexInput[1].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_TBL;
		stSearchIndexInput[1].iAxisOnACS = ACS_CARD_AXIS_Y;  // 20110111
		stSearchIndexInput[1].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_X_HORI_BONDER;
		stSearchIndexInput[1].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
		stSearchIndexInput[1].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_X;
		stSearchIndexInput[1].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;
	// For Table-Y
		stSearchIndexInput[0].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_TBL;
		stSearchIndexInput[0].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_TBL_Y;
		stSearchIndexInput[0].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_TBL_Y;
		stSearchIndexInput[0].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_TBL_Y;
		stSearchIndexInput[0].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_Y;
		stSearchIndexInput[0].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
		stSearchIndexInput[0].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_Y;
		stSearchIndexInput[0].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_TBL;
		stSearchIndexInput[0].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_TBL;
		stSearchIndexInput[0].iAxisOnACS = ACS_CARD_AXIS_X;  // 20110111
		stSearchIndexInput[0].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_Y;
		stSearchIndexInput[0].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
		stSearchIndexInput[0].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_Y;
		stSearchIndexInput[0].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;
	// For BondHead-Z
		stSearchIndexInput[4].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_Z;
		stSearchIndexInput[4].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_Z;
		stSearchIndexInput[4].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_Z;
		stSearchIndexInput[4].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_Z;
		stSearchIndexInput[4].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_Z;
		stSearchIndexInput[4].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
		stSearchIndexInput[4].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_Z;
		stSearchIndexInput[4].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_Z;
		stSearchIndexInput[4].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_Z;
		if(sys_acs_communication_get_flag_sc_udi() == TRUE) // 20130228
		{
			stSearchIndexInput[4].iAxisOnACS = ACS_CARD_AXIS_Z;   // 20110111, iAxisZ;
		}
		else
		{
			stSearchIndexInput[4].iAxisOnACS = 4;  // ACS_CARD_AXIS_A
		}
		stSearchIndexInput[4].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_Z;
		stSearchIndexInput[4].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
		stSearchIndexInput[4].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_Z;
		stSearchIndexInput[4].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;

		stSearchIndexInput[0].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;
		stSearchIndexInput[1].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;
		stSearchIndexInput[4].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;
}


#include "driver.h"
void MtnDialog_FbMonitor::CheckEfoFbBy1739U_WbAddr()
{
	if(lDrvHandleEFO_Monitor_1739U != NULL)
	{		
		static PT_DioReadPortByte ptDioReadPortByteEFO_GoodNG;
		ptDioReadPortByteEFO_GoodNG.port = usByteRead1739U_EFO_GoodNG; // 0x4;  // Byte-4, from 0
		ptDioReadPortByteEFO_GoodNG.value = (USHORT far *)&usDigitalIO1739_ReadByteValueEFO_GoodNG;

		DRV_DioReadPortByte(lDrvHandleEFO_Monitor_1739U,
					(LPT_DioReadPortByte)&ptDioReadPortByteEFO_GoodNG);

//		int iBitEFO_GoodNG_By1739 = 2^7;
		if(usDigitalIO1739_ReadByteValueEFO_GoodNG & usBitPattern1739U_EFO_GoodNG)  // bit 7 from 0
		{  // 1: fail
			uiCountFailTrigEFO ++;
		}
		else
		{  // 0: success
			uiCountSuccessEFO ++;
		}
		uiCountTotalEFO ++;

	}
}

static char strTextTemp[128];
static char *strStopString;
static	CString cstrEditText;

static char strDebugText[1024];
extern char strDebugErrorMessageHomeACS[];
extern COMM_SETTINGS stServoControllerCommSet;

unsigned short usDigitalIO_PortTrigEFO_Byte;   // 20091121

unsigned short usDigitalIO_PortInputBytePort, usDigitalIO_PortOutputBytePort;
unsigned short usDigitalIO1739_ReadByteValue, usDigitalIO1739_ReadByteOutputValue;
unsigned short usDigitalIO1739_WriteByteReg;

static 	int iSelChannel = 0;

// 20100405
static	LARGE_INTEGER liFreqOS_FbMonitorDlg; 
static	unsigned int uiTimerFreqFactor_10KHz = 10;

// MtnDialog_FbMonitor dialog
IMPLEMENT_DYNAMIC(MtnDialog_FbMonitor, CDialog)

MtnDialog_FbMonitor::MtnDialog_FbMonitor( CWnd* pParent /*=NULL*/)
	: CDialog(MtnDialog_FbMonitor::IDD, pParent)
{
//	Handle = stServoControllerCommSet.Handle;
	m_bStart_ACS_TriggerEFO_CheckBy1739 = FALSE;
	iOut0[0] = 0;

	dblFbPosition = 0;
	dblFbVelocity = 0;
	dblFbAcceleration = 0;
	dblFbForce = 0;
	iFbIndex = 0;
	iFbLimit = 0;
	iDebug = 0;
	cstrFbPosition.Format("%f", dblFbPosition);
	cstrFbVelocity.Format("%f", dblFbVelocity);

	// Thread handling
	m_pWinThread = NULL;
	m_fStopMonitorThread = TRUE; // FALSE;
	iMonitorFlag = 0;

//	mtnapi_get_motor_state(Handle, iSelChannel, &iMotorState, iDebug);
	// UI variables
	cFlagEnable_Group_Servo_ACS_ChannelFbMonitor = 1;
	cFlagEnable_Group_ACS_OUT_MtnDialog_FbMonitor = 1;

	// 1739U Init
	lDrvHandleDigitalInput_IO1739U = (LONG)NULL;
	lDrvHandleDigitalOutput_IO1739U = (LONG)NULL;
	lDrvHandleEFO_Monitor_1739U = (LONG)NULL;
}

MtnDialog_FbMonitor::~MtnDialog_FbMonitor()
{
	StopMonitorThread();
#ifndef NO_IO_PCI_CARD
//	theEFO_Monitor.EFO_stop_monitor(); // 20081030
#endif // NO_IO_PCI_CARD
}

#include "EfoMonitor1739U.h"
// 20090824, add EFO_Monitor_1739U
EFO_MONITOR_1739U mEfoMonitor1739U;
static char  cFlagEnable_Group_1739burn_Efo207;
static char  cFlagEnable_Group_EFO207_Fb_CheckBy_1739U_Level;

static char  cFlagEnable_Group_ACS_ServoHome_MtnDialog_FbMonitor = 0;

static char  cFlagEnable_Group_Setting_ACS_ServoHome_MtnDialog_FbMonitor = 0;

#undef MOTALGO_DLL_EXPORTS
//#include "MotAlgo_DLL.h"

void MtnDialog_FbMonitor::InitUI_ComboChannelSel()
{
	CComboBox *pSelectChannelCombo = (CComboBox*) GetDlgItem(IDC_CHANNEL_SEL_COMBO);
	pSelectChannelCombo->ResetContent();
	pSelectChannelCombo->InsertString(0, "X");
	pSelectChannelCombo->InsertString(1, "Y");
	pSelectChannelCombo->InsertString(2, "Z");
	pSelectChannelCombo->InsertString(3, "T");
	pSelectChannelCombo->InsertString(4, "A");
	pSelectChannelCombo->InsertString(5, "B");
	pSelectChannelCombo->InsertString(6, "C");
	pSelectChannelCombo->InsertString(7, "D");
	pSelectChannelCombo->SetCurSel(iSelChannel);

	cstrChannelNum.Format("%d", iSelChannel);
	GetDlgItem(IDC_CHANNEL_NUM)->SetWindowText(cstrChannelNum);
}
void MtnDialog_FbMonitor::InitVarMachineType(int iFlagMachType)
{
	if(iFlagMachType == WB_STATION_EFO_BSD)
	{
		cFlagEnable_Group_1739burn_Efo207 = 1;
		usDigitalIO_PortOutputBytePort = 3;
		usDigitalIO_PortInputBytePort = 2;
	}
	else if(iFlagMachType == WB_STATION_USG_LIGHTING_PR)
	{
		usDigitalIO_PortOutputBytePort = 0;
		usDigitalIO_PortInputBytePort = 0;
	}
	else
	{
		usDigitalIO_PortOutputBytePort = 0;
		usDigitalIO_PortInputBytePort = 0;
	}
}

BOOL MtnDialog_FbMonitor::OnInitDialog()
{
	stCommHandle_DlgFbMonitor_ACS = stServoControllerCommSet.Handle;
	// Initialize local variables 20110803
	iFlagMachineType = get_sys_machine_type_flag();
	InitVarMachineType(iFlagMachineType);

	InitUI_ComboChannelSel();
	iMotorState = 0;
	if(stCommHandle_DlgFbMonitor_ACS != ACSC_INVALID)
	{
		mtnapi_get_motor_state(stCommHandle_DlgFbMonitor_ACS, iSelChannel, &iMotorState, iDebug);
	}
	if(iMotorState & ACSC_MST_ENABLE) 
	{
		// now is enable
		CString cstrEnaDisMotorButton = "Disable";
		GetDlgItem(IDC_ENABLE_MOTOR)->SetWindowText(cstrEnaDisMotorButton);
	}
	else
	{
		CString cstrEnaDisMotorButton = "Enable";
		GetDlgItem(IDC_ENABLE_MOTOR)->SetWindowText(cstrEnaDisMotorButton);
	}
	if(stCommHandle_DlgFbMonitor_ACS != ACSC_INVALID)
	{
		acsc_ResetIndexState(stCommHandle_DlgFbMonitor_ACS, iSelChannel, ACSC_IST_IND, NULL); 				// Clear index state, 20090119 Handle
	}

	// set language option
	SetUserInterfaceLanguage(get_sys_language_option());
	//

	if(iFlagInitSearchHomeParameter == 0)
	{
		iFlagInitSearchHomeParameter = 1;

		if(iFlagMachineType == WB_MACH_TYPE_VLED_FORK)
		{
			mtest_fb_init_search_limit_home_parameter_vled_bonder();
			mtn_init_def_para_search_index_vled_bonder_xyz(_EFSIKA_TABLE_X_AXIS, _EFSIKA_TABLE_Y_AXIS, _EFSIKA_BOND_Z_AXIS);
		}
		else if(iFlagMachineType == WB_MACH_TYPE_HORI_LED || iFlagMachineType == BE_WB_HORI_20T_LED)  // 20120731
		{
			mtest_fb_init_search_limit_home_parameter_hori_bonder();
			mtn_dll_init_def_para_search_index_hori_bonder_xyz(_EFSIKA_TABLE_X_AXIS, _EFSIKA_TABLE_Y_AXIS, _EFSIKA_BOND_Z_AXIS);
		}
		else if(iFlagMachineType  == BE_WB_ONE_TRACK_18V_LED)  // 20120801
		{
			mtest_fb_init_search_limit_home_parameter_hori_bonder();
			mtn_dll_init_def_para_search_index_18v_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
		}
		else // WB_MACH_TYPE_ONE_TRACK_13V_LED, 20110504
		{
			mtest_fb_init_search_limit_home_parameter_13v_bonder();
			mtn_dll_init_def_para_search_index_13v_vled_bonder_xyz(_EFSIKA_TABLE_X_AXIS, _EFSIKA_TABLE_Y_AXIS, _EFSIKA_BOND_Z_AXIS);
		}
	}
	UpdateSearchHomeEdit();

	// Init 1739U Card related
	DialogInitDigitalIO_1739U();
	DialogInitDigitalIO_7248(); // 20120823

	// Enable Grouping UIs
	UI_Enable_Group_1739burn_Efo207(cFlagEnable_Group_1739burn_Efo207);
	((CButton*) GetDlgItem(IDC_CHECK_EFO_MONITOR_1739U_DEBUG_FLAG))->SetCheck(0);
	OnBnClickedCheckEfoMonitor1739uDebugFlag();

	UI_Enable_Group_EFO207_Fb_CheckBy_1739U_Level(cFlagEnable_Group_EFO207_Fb_CheckBy_1739U_Level);
	UI_Enable_Group_ACS_OUT_MtnDialog_FbMonitor(cFlagEnable_Group_ACS_OUT_MtnDialog_FbMonitor);
	UI_Enable_Group_ACS_ServoHome_MtnDialog_FbMonitor(cFlagEnable_Group_ACS_ServoHome_MtnDialog_FbMonitor);

	((CButton*) GetDlgItem(IDC_CHECK_ENA_FLAG_BURN_ETORCH_1739))->SetCheck(cFlagEnable_Group_1739burn_Efo207);
	((CButton*) GetDlgItem(IDC_CHECK_ENA_EFO207_FB_CHECK_BY1739_LEVEL))->SetCheck(cFlagEnable_Group_EFO207_Fb_CheckBy_1739U_Level);
	((CButton*) GetDlgItem(IDC_CHECK_ENA_ACS_IO))->SetCheck(cFlagEnable_Group_ACS_OUT_MtnDialog_FbMonitor);
	((CButton*) GetDlgItem(IDC_CHECK_FLAG_ENA_HOME_ACS_SERVO_CH))->SetCheck(cFlagEnable_Group_ACS_ServoHome_MtnDialog_FbMonitor);
	((CButton*)GetDlgItem(IDC_CHECK_FB_MONITOR_DLG_TEST_DUAL_FAB))->SetCheck(cFlagEnableTestDualFreeAirBall); // 20121018

	UI_Enable_Group_Servo_ACS_FbMonitorDlg(cFlagEnable_Group_Servo_ACS_ChannelFbMonitor);  // 20110726
	((CButton*) GetDlgItem(IDC_CHECK_FLAG_ENA_ACS_CH_MONITOR))->SetCheck(cFlagEnable_Group_Servo_ACS_ChannelFbMonitor); 
	// Set Servo Control UI
	UpdateServoCtrlUI();

	// Ratio Period of Trigger EFO
	UI_UpdateTriggerEFO_RatioPeriod();

	// USG select channel
	InitUI_ComboSelectUSG_Channel();
	//// 20100405
	//QueryPerformanceFrequency(&liFreqOS_FbMonitorDlg);
	//timeBeginPeriod(1);
	//RunMonitorThread();

#ifndef NO_IO_PCI_CARD
//	theEFO_Monitor.EFO_start_monitor(); // 20081030
#endif // NO_IO_PCI_CARD
	return CDialog::OnInitDialog();
}

void MtnDialog_FbMonitor::UI_UpdateTriggerEFO_RatioPeriod()
{
	int iSliderPosn = (int)(uiTimerFreqFactor_10KHz * 100.0/10 /INTER_TRIG_FB_DELAY_MS);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_FB_DLG_TRIG_EFO_RATIO_PERIOD))->SetPos(iSliderPosn);
	CString cstrTemp; cstrTemp.Format("%d %%", iSliderPosn);
	GetDlgItem(IDC_STATIC_FB_DLG_TRIG_EFO_RATIO_PERIOD)->SetWindowTextA(cstrTemp);
}
void MtnDialog_FbMonitor::ShowWindow(int nCmdShow)
{
	if(m_fStopMonitorThread == TRUE)
	{
		QueryPerformanceFrequency(&liFreqOS_FbMonitorDlg);
		timeBeginPeriod(1);
		RunMonitorThread();
	}
	UpdateServoCtrlUI();
	// Ratio Period of Trigger EFO
	UI_UpdateTriggerEFO_RatioPeriod();

	// Ratio Period of Trigger EFO
	UpdateSearchHomeEdit();

	// Init 1739U Card related
	DialogInitDigitalIO_1739U();

	CDialog::ShowWindow(nCmdShow);
}

void MtnDialog_FbMonitor::UpdateSearchHomeEdit()
{
	cstrEditText.Format("%d", stSearchIndexInput[iSelChannel].uiFreqFactor_10KHz_Detecting);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ)->SetWindowText(cstrEditText);

	cstrEditText.Format("%8.0f", stSearchIndexInput[iSelChannel].stSpeedProfileMove.dMaxVelocity);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_VEL)->SetWindowText(cstrEditText);
	cstrEditText.Format("%8.0f", stSearchIndexInput[iSelChannel].stSpeedProfileMove.dMaxAcceleration);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_Acc)->SetWindowText(cstrEditText);
	cstrEditText.Format("%8.0f", stSearchIndexInput[iSelChannel].stSpeedProfileMove.dMaxJerk);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_JERK)->SetWindowText(cstrEditText);
	cstrEditText.Format("%8.0f", stSearchIndexInput[iSelChannel].stSpeedProfileMove.dMaxDeceleration);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_DEC)->SetWindowText(cstrEditText);

	cstrEditText.Format("%6.0f", stSearchIndexInput[iSelChannel].dMaxDistanceRangeProtection);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_DETECT_MAX_DIST_PROT)->SetWindowText(cstrEditText);
	cstrEditText.Format("%6.0f", stSearchIndexInput[iSelChannel].dMoveDistanceBeforeSearchLimit);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT)->SetWindowText(cstrEditText);
	cstrEditText.Format("%6.0f", stSearchIndexInput[iSelChannel].dVelJoggingLimit);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT)->SetWindowText(cstrEditText);
	cstrEditText.Format("%4.0f", stSearchIndexInput[iSelChannel].dPositionErrorThresholdSearchLimit);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_PE_TH_SEARCH_LIMIT)->SetWindowText(cstrEditText);
	cstrEditText.Format("%6.0f", stSearchIndexInput[iSelChannel].dMoveDistanceBeforeSearchIndex1);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1)->SetWindowText(cstrEditText);
	cstrEditText.Format("%6.0f", stSearchIndexInput[iSelChannel].dMoveDistanceBeforeSearchIndex2);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2)->SetWindowText(cstrEditText);
	cstrEditText.Format("%6.0f", stSearchIndexInput[iSelChannel].dVelJoggingIndex1);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1)->SetWindowText(cstrEditText);
	cstrEditText.Format("%6.0f", stSearchIndexInput[iSelChannel].dVelJoggingIndex2);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_2)->SetWindowText(cstrEditText);
	cstrEditText.Format("%6.0f", stSearchIndexInput[iSelChannel].dPosnErrThresHoldSettling);
	GetDlgItem(IDC_SEARCH_HOME_POSN_ERR_SETTLE)->SetWindowText(cstrEditText);

	stSearchIndexInput[iSelChannel].iDebug = ((CButton *)GetDlgItem(IDC_CHECK_DEBUG_SEARCH_INDEX))->GetCheck();
}
void MtnDialog_FbMonitor::OnClose()
{
	StopMonitorThread();
	Sleep(1000);
	CDialog::OnCancel();
}

void MtnDialog_FbMonitor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHANNEL_SEL_COMBO, combSelectChannel);

	cstrChannelNum.Format("%d", iSelChannel);
	GetDlgItem(IDC_CHANNEL_NUM)->SetWindowText(cstrChannelNum);

	// update the feedback monitor
	if(stCommHandle_DlgFbMonitor_ACS != ACSC_INVALID)
	{
		mtnapi_get_fb_position(stCommHandle_DlgFbMonitor_ACS, iSelChannel, &dblFbPosition, iDebug);  // Handle
		cstrFbPosition.Format("%f", dblFbPosition);
		GetDlgItem(IDC_FB_POSITION)->SetWindowText(cstrFbPosition);
	}

	cstrMotionStatus.Format("0x%X", iMotorState);
	GetDlgItem(IDC_FB_MOTOR_STATUS)->SetWindowText(cstrMotionStatus);

	GetDlgItem(IDC_FB_VELOCITY)->SetWindowText(cstrFbVelocity);
	GetDlgItem(IDC_FB_ACCELERATION)->SetWindowText(cstrFbAcceleration);
	GetDlgItem(IDC_FB_FORCE)->SetWindowText(cstrFbForce);
	GetDlgItem(IDC_FB_INDEX)->SetWindowText(cstrFbIndex);
	GetDlgItem(IDC_FB_LIMIT)->SetWindowText(cstrFbLimit);

	// , 
	//	(CEdit *)GetDlgItem[IDC_FB_POSITION_TEXT]->Text = 
	// DDX_Control(pDX, IDC_FB_POSITION_TEXT, cFbPositionEdit);
}


BEGIN_MESSAGE_MAP(MtnDialog_FbMonitor, CDialog)
	ON_CBN_SELCHANGE(IDC_CHANNEL_SEL_COMBO, &MtnDialog_FbMonitor::OnCbnSelchangeChannelSelCombo)
//	ON_EN_CHANGE(IDC_FB_POSITION_TEXT, &MtnDialog_FbMonitor::OnEnChangeFbPositionText)
ON_BN_CLICKED(IDC_ENABLE_MOTOR, &MtnDialog_FbMonitor::OnBnClickedEnableMotor)
ON_BN_CLICKED(IDC_FB_MONITOR_START, &MtnDialog_FbMonitor::OnBnClickedFbMonitorStart)
ON_BN_CLICKED(IDC_CHECK_FB_ACS_OUT0, &MtnDialog_FbMonitor::OnBnClickedCheckOut0)
ON_EN_CHANGE(IDC_EDIT_SEARCH_HOME_MOVE_VEL, &MtnDialog_FbMonitor::OnEnChangeEditSearchHomeMoveVel)
ON_EN_CHANGE(IDC_EDIT_SEARCH_HOME_MOVE_Acc, &MtnDialog_FbMonitor::OnEnChangeEditSearchHomeMoveAcc)
ON_EN_CHANGE(IDC_EDIT_SEARCH_HOME_MOVE_DEC, &MtnDialog_FbMonitor::OnEnChangeEditSearchHomeMoveDec)
ON_EN_CHANGE(IDC_EDIT_SEARCH_HOME_MOVE_JERK, &MtnDialog_FbMonitor::OnEnChangeEditSearchHomeMoveJerk)
ON_EN_CHANGE(IDC_EDIT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ, &MtnDialog_FbMonitor::OnEnChangeEditSearchHomeDetectFreqFactor10khz)
ON_EN_CHANGE(IDC_EDIT_SEARCH_HOME_DETECT_MAX_DIST_PROT, &MtnDialog_FbMonitor::OnEnChangeEditSearchHomeDetectMaxDistProt)
ON_EN_CHANGE(IDC_EDIT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT, &MtnDialog_FbMonitor::OnEnChangeEditSearchHomeMoveDistSearchLimit)
ON_EN_CHANGE(IDC_EDIT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT, &MtnDialog_FbMonitor::OnEnChangeEditSearchHomeJogVelSearchLimit)
ON_EN_CHANGE(IDC_EDIT_SEARCH_HOME_PE_TH_SEARCH_LIMIT, &MtnDialog_FbMonitor::OnEnChangeEditSearchHomePeThSearchLimit)
ON_EN_CHANGE(IDC_EDIT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1, &MtnDialog_FbMonitor::OnEnChangeEditSearchHomeMoveDistBfSearchIndex1)
ON_EN_CHANGE(IDC_EDIT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1, &MtnDialog_FbMonitor::OnEnChangeEditSearchHomeJogVelSearchIndex1)
ON_EN_CHANGE(IDC_EDIT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2, &MtnDialog_FbMonitor::OnEnChangeEditSearchHomeMoveDistBfSearchIndex2)
ON_EN_CHANGE(IDC_EDIT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_2, &MtnDialog_FbMonitor::OnEnChangeEditSearchHomeJogVelSearchIndex2)
ON_BN_CLICKED(IDC_BUTTON_AXIS_GO_HOME, &MtnDialog_FbMonitor::OnBnClickedButtonAxisGoHome)
ON_BN_CLICKED(IDC_CHECK_DEBUG_SEARCH_INDEX, &MtnDialog_FbMonitor::OnBnClickedCheckDebugSearchIndex)
ON_EN_CHANGE(IDC_SEARCH_HOME_POSN_ERR_SETTLE, &MtnDialog_FbMonitor::OnEnChangeSearchHomePosnErrSettle)
ON_BN_CLICKED(IDC_BUTTON_AXIS_GO_LIMIT, &MtnDialog_FbMonitor::OnBnClickedButtonAxisGoLimit)
ON_BN_CLICKED(IDC_BUTTON_AXIS_GO_HOME_FROM_LIMIT, &MtnDialog_FbMonitor::OnBnClickedButtonAxisGoHomeFromLimit)
ON_BN_CLICKED(IDC_BUTTON_AXIS_API_GO_LIMIT_AND_HOME, &MtnDialog_FbMonitor::OnBnClickedButtonAxisApiGoLimitAndHome)
ON_BN_CLICKED(IDC_BUTTON_AXIS_API_GO_LIMIT, &MtnDialog_FbMonitor::OnBnClickedButtonAxisApiGoLimit)
ON_BN_CLICKED(IDC_BUTTON_AXIS_API_GO_HOME_FROM_LIMIT, &MtnDialog_FbMonitor::OnBnClickedButtonAxisApiGoHomeFromLimit)
ON_BN_CLICKED(IDC_CHECK_SEARCH_INDEX_VERIFY_BY_REPEATING, &MtnDialog_FbMonitor::OnBnClickedCheckSearchIndexVerifyByRepeating)
ON_BN_CLICKED(IDC_BUTTON_BONDER_HOME_ALL, &MtnDialog_FbMonitor::OnBnClickedButtonBonderHomeAll)
ON_BN_CLICKED(IDC_CHECK_ACS_OUT1, &MtnDialog_FbMonitor::OnBnClickedCheckAcsOut1)
ON_BN_CLICKED(IDC_CHECK_ACS_OUT2, &MtnDialog_FbMonitor::OnBnClickedCheckAcsOut2)
ON_BN_CLICKED(IDC_CHECK_ACS_OUT3, &MtnDialog_FbMonitor::OnBnClickedCheckAcsOut3)
ON_CBN_SELCHANGE(IDC_COMBO_DIGITAL_INPUT_CARD_LIST, &MtnDialog_FbMonitor::OnCbnSelchangeComboDigitalIo1739List)
ON_CBN_SELCHANGE(IDC_COMBO_DIGITAL_IO_SELECT_INPUT_BYTE, &MtnDialog_FbMonitor::OnCbnSelchangeComboDigitalIoSelectInputByte)
ON_CBN_SELCHANGE(IDC_COMBO_DIGITAL_IO_SELECT_OUTPUT_BYTE, &MtnDialog_FbMonitor::OnCbnSelchangeComboDigitalIoSelectOutputByte)
ON_BN_CLICKED(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_7, &MtnDialog_FbMonitor::OnBnClickedButtonDigitalIoOutputBit7)
ON_BN_CLICKED(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_6, &MtnDialog_FbMonitor::OnBnClickedButtonDigitalIoOutputBit6)
ON_BN_CLICKED(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_5, &MtnDialog_FbMonitor::OnBnClickedButtonDigitalIoOutputBit5)
ON_BN_CLICKED(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_4, &MtnDialog_FbMonitor::OnBnClickedButtonDigitalIoOutputBit4)
ON_BN_CLICKED(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_3, &MtnDialog_FbMonitor::OnBnClickedButtonDigitalIoOutputBit3)
ON_BN_CLICKED(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_2, &MtnDialog_FbMonitor::OnBnClickedButtonDigitalIoOutputBit2)
ON_BN_CLICKED(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_1, &MtnDialog_FbMonitor::OnBnClickedButtonDigitalIoOutputBit1)
ON_BN_CLICKED(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_0, &MtnDialog_FbMonitor::OnBnClickedButtonDigitalIoOutputBit0)
ON_CBN_SELCHANGE(IDC_COMBO_DIGITAL_OUTPUT_CARD_LIST, &MtnDialog_FbMonitor::OnCbnSelchangeComboDigitalOutput1739List)
ON_CBN_SELCHANGE(IDC_COMBO_EFO_MONITOR_USE_1739U_CARD, &MtnDialog_FbMonitor::OnCbnSelchangeComboEfoMonitorUse1739uCard)
ON_BN_CLICKED(IDC_BUTTON_FB_MONITOR_EFO_GOODNG_BY_1739U, &MtnDialog_FbMonitor::OnBnClickedButtonFbMonitorEfoGoodngBy1739u)
ON_BN_CLICKED(IDC_CHECK_EFO_MONITOR_1739U_DEBUG_FLAG, &MtnDialog_FbMonitor::OnBnClickedCheckEfoMonitor1739uDebugFlag)
ON_BN_CLICKED(IDC_BUTTON_FB_MONITOR_HOME_ALL_API, &MtnDialog_FbMonitor::OnBnClickedButtonFbMonitorHomeAllApi)
ON_CBN_SELCHANGE(IDC_COMBO_EFO_TRIG_USE_1739U_BYTE, &MtnDialog_FbMonitor::OnCbnSelchangeComboEfoTrigUse1739uByte)
ON_BN_CLICKED(IDC_CHECK_FLAG_ENA_ACS_CH_MONITOR, &MtnDialog_FbMonitor::OnBnClickedCheckFlagEnaAcsChMonitor)
ON_BN_CLICKED(IDC_CHECK_FLAG_ENA_HOME_ACS_SERVO_CH, &MtnDialog_FbMonitor::OnBnClickedCheckFlagEnaHomeAcsServoCh)
ON_BN_CLICKED(IDC_CHECK_ENA_ACS_IO, &MtnDialog_FbMonitor::OnBnClickedCheckEnaAcsIo)
ON_BN_CLICKED(IDC_CHECK_ENA_FLAG_BURN_ETORCH_1739, &MtnDialog_FbMonitor::OnBnClickedCheckEnaFlagBurnEtorch1739)
ON_BN_CLICKED(IDC_CHECK_ENA_EFO207_FB_CHECK_BY1739_LEVEL, &MtnDialog_FbMonitor::OnBnClickedCheckEnaEfo207FbCheckBy1739Level)
ON_CBN_SELCHANGE(IDC_COMBO_DLG_FB_SELECT_WH_MOD, &MtnDialog_FbMonitor::OnCbnSelchangeComboDlgFbSelectWhMod)
ON_BN_CLICKED(IDC_CHECK_FB_ACS_OUT8, &MtnDialog_FbMonitor::OnBnClickedCheckFbAcsOut8)
ON_BN_CLICKED(IDC_CHECK_FB_ACS_OUT9, &MtnDialog_FbMonitor::OnBnClickedCheckFbAcsOut9)
ON_BN_CLICKED(IDC_CHECK_FB_ACS_OUT10, &MtnDialog_FbMonitor::OnBnClickedCheckFbAcsOut10)
ON_BN_CLICKED(IDC_CHECK_FB_ACS_OUT11, &MtnDialog_FbMonitor::OnBnClickedCheckFbAcsOut11)
ON_BN_CLICKED(IDC_CHECK_FB_ACS_OUT4, &MtnDialog_FbMonitor::OnBnClickedCheckFbAcsOut4)
ON_BN_CLICKED(IDC_CHECK_FB_ACS_OUT5, &MtnDialog_FbMonitor::OnBnClickedCheckFbAcsOut5)
ON_BN_CLICKED(IDC_CHECK_FB_ACS_OUT6, &MtnDialog_FbMonitor::OnBnClickedCheckFbAcsOut6)
ON_BN_CLICKED(IDC_CHECK_FB_ACS_OUT7, &MtnDialog_FbMonitor::OnBnClickedCheckFbAcsOut7)
ON_CBN_SELCHANGE(IDC_COMBO_DLG_FB_SELECT_MACH_TYPE, &MtnDialog_FbMonitor::OnCbnSelchangeComboDlgFbSelectMachType)
ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_FB_DLG_TRIG_EFO_RATIO_PERIOD, &MtnDialog_FbMonitor::OnNMReleasedcaptureSliderFbDlgTrigEfoRatioPeriod)
ON_EN_KILLFOCUS(IDC_EDIT_FB_MONITOR_EFO_BSD_BY_1739U_TOTAL_CNT, &MtnDialog_FbMonitor::OnEnKillfocusEditFbMonitorEfoBsdBy1739uTotalCnt)
ON_CBN_SELCHANGE(IDC_COMBO_SELECT_OUTPUT_USG_STATION, &MtnDialog_FbMonitor::OnCbnSelchangeComboSelectOutputUsgStation)
ON_BN_CLICKED(IDC_CHECK_FB_MONITOR_DLG_TEST_DUAL_FAB, &MtnDialog_FbMonitor::OnBnClickedCheckFbMonitorDlgTestDualFab)
END_MESSAGE_MAP()

//WM_SHOWWINDOW MSG_HANDLER(, &MtnDialog_FbMonitor::OnShowWindow) 

// MtnDialog_FbMonitor message handlers
void MtnDialog_FbMonitor::OnCbnSelchangeChannelSelCombo()
{
	// TODO: Add your control notification handler code here
	CDialog::UpdateData(FALSE); // From view to the class

	iSelChannel = (int)combSelectChannel.GetCurSel();
	// update the feedback monitor
//	mtnapi_get_fb_position(Handle, iSelChannel, &dblFbPosition, iDebug);

	cstrChannelNum.Format("%d", iSelChannel);
	GetDlgItem(IDC_CHANNEL_NUM)->SetWindowText(cstrChannelNum);
	//cstrFbVelocity.Format("%f", dblFbVelocity);
	//GetDlgItem(IDC_FB_VELOCITY)->SetWindowText(cstrFbVelocity);

	mtnapi_get_motor_state(stCommHandle_DlgFbMonitor_ACS, iSelChannel, &iMotorState, iDebug);  // Handle
	if(iMotorState & ACSC_MST_ENABLE) 
	{
		// now is enable
		CString cstrEnaDisMotorButton = "Disable";
		GetDlgItem(IDC_ENABLE_MOTOR)->SetWindowText(cstrEnaDisMotorButton);
	}
	else
	{
		CString cstrEnaDisMotorButton = "Enable";
		GetDlgItem(IDC_ENABLE_MOTOR)->SetWindowText(cstrEnaDisMotorButton);
	}
	cstrMotionStatus.Format("0x%X", iMotorState);
	GetDlgItem(IDC_FB_MOTOR_STATUS)->SetWindowText(cstrMotionStatus);

	uiCountIndex = 0; // 20090119
	UpdateSearchHomeEdit();
}

void MtnDialog_FbMonitor::OnBnClickedEnableMotor()
{
	// TODO: Add your control notification handler code here

	mtnapi_get_motor_state(stCommHandle_DlgFbMonitor_ACS, iSelChannel, &iMotorState, iDebug);  // Handle
	if(iMotorState & ACSC_MST_ENABLE) 
	{
		// now is enable, to disable it
		mtnapi_disable_motor(stCommHandle_DlgFbMonitor_ACS, iSelChannel, iDebug);
	}
	else
	{
		// now is disabled, to enable it
		mtnapi_enable_motor(stCommHandle_DlgFbMonitor_ACS, iSelChannel, iDebug);
	}
}

UINT MonitorThreadProc( LPVOID pParam )
{
    MtnDialog_FbMonitor* pObject = (MtnDialog_FbMonitor *)pParam;
	return pObject->MonitorThread(); 	
}
static char strEFO_Flag[128];


void MtnDialog_FbMonitor::MonitorUpdateAcsFeedback()
{
	double dIndexPositionCurr;
	double dRelaxPosn;
		// Analog input, contact force
	int aiAnalogInput[16]; // 20081112
//		if (!acsc_ReadInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "AIN", 0, 15, ACSC_NONE, ACSC_NONE, &aiAnalogInput[0], NULL ))
//		{
#ifdef __DEBUG_ADC__
			sprintf_s(strDebugText, 128, "Error Read AIN, Error Code: %d", acsc_GetLastError());
			AfxMessageBox(strDebugText);
#endif // __DEBUG_ADC__
//		}
		acsc_GetAnalogInput(stCommHandle_DlgFbMonitor_ACS, 0, &aiAnalogInput[0], 0);
		cstrFbForce.Format("%d", aiAnalogInput[0]); // iMonitorFlag * 1000 +
		GetDlgItem(IDC_FB_FORCE)->SetWindowText(cstrFbForce);

		mtnapi_get_fb_position(stCommHandle_DlgFbMonitor_ACS, iSelChannel, &dblFbPosition, iDebug);  // Handle
		cstrFbPosition.Format("%8.2f", dblFbPosition); // iMonitorFlag * 1000 +
		GetDlgItem(IDC_FB_POSITION)->SetWindowText(cstrFbPosition);

		mtnapi_get_fb_velocity(stCommHandle_DlgFbMonitor_ACS, iSelChannel, &dblFbVelocity, iDebug);  // Handle
		cstrFbVelocity.Format("%8.2f", dblFbVelocity);
		GetDlgItem(IDC_FB_VELOCITY)->SetWindowText(cstrFbVelocity);

		cstrMotionStatus.Format("0x%X", iMotorState);
		GetDlgItem(IDC_FB_MOTOR_STATUS)->SetWindowText(cstrMotionStatus);

		if (!acsc_ReadInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, &iOut0[0], NULL ))  // Handle
		{
			sprintf_s(strDebugText, 128, "Error Read OUT0, Error Code: %d", acsc_GetLastError());
//			AfxMessageBox(_T("strDebugText"));
		}
		if( (iOut0[0] & 0x1) == 1)
		{
			((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT0))->SetCheck(TRUE);
		}
		else
		{
			((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT0))->SetCheck(FALSE);
		}
		if( iOut0[0] & 0x2)
		{
			((CButton *)GetDlgItem(IDC_CHECK_ACS_OUT1))->SetCheck(TRUE);
		}
		else
		{
			((CButton *)GetDlgItem(IDC_CHECK_ACS_OUT1))->SetCheck(FALSE);
		}
		if( iOut0[0] & 0x4)
		{
			((CButton *)GetDlgItem(IDC_CHECK_ACS_OUT2))->SetCheck(TRUE);
		}
		else
		{
			((CButton *)GetDlgItem(IDC_CHECK_ACS_OUT2))->SetCheck(FALSE);
		}
		if( iOut0[0] & 0x8)
		{
			((CButton *)GetDlgItem(IDC_CHECK_ACS_OUT3))->SetCheck(TRUE);
		}
		else
		{
			((CButton *)GetDlgItem(IDC_CHECK_ACS_OUT3))->SetCheck(FALSE);
		}

		if( iOut0[0] & ACS_OUT0_OR_SET_BIT4)
		{
			((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT4))->SetCheck(TRUE);
		}
		else
		{
			((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT4))->SetCheck(FALSE);
		}
		if( iOut0[0] & ACS_OUT0_OR_SET_BIT5)
		{
			((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT5))->SetCheck(TRUE);
		}
		else
		{
			((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT5))->SetCheck(FALSE);
		}
		if( iOut0[0] & ACS_OUT0_OR_SET_BIT6)
		{
			((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT6))->SetCheck(TRUE);
		}
		else
		{
			((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT6))->SetCheck(FALSE);
		}
		if( iOut0[0] & ACS_OUT0_OR_SET_BIT7)
		{
			((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT7))->SetCheck(TRUE);
		}
		else
		{
			((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT7))->SetCheck(FALSE);
		}

		if( iOut0[0] & ACS_OUT0_OR_SET_BIT8)
		{
			((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT8))->SetCheck(TRUE);
		}
		else
		{
			((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT8))->SetCheck(FALSE);
		}
		if( iOut0[0] & ACS_OUT0_OR_SET_BIT9)
		{
			((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT9))->SetCheck(TRUE);
		}
		else
		{
			((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT9))->SetCheck(FALSE);
		}
		if( iOut0[0] & ACS_OUT0_OR_SET_BIT10)
		{
			((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT10))->SetCheck(TRUE);
		}
		else
		{
			((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT10))->SetCheck(FALSE);
		}
		if( iOut0[0] & ACS_OUT0_OR_SET_BIT11)
		{
			((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT11))->SetCheck(TRUE);
		}
		else
		{
			((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT11))->SetCheck(FALSE);
		}


		// index count 20090119
		acsc_GetIndexState(stCommHandle_DlgFbMonitor_ACS, iSelChannel, &iIndexStateCurr, NULL);
		if(iIndexStateCurr & ACSC_IST_IND)
		{
			uiCountIndex ++;
			acsc_ResetIndexState(stCommHandle_DlgFbMonitor_ACS, iSelChannel, ACSC_IST_IND, NULL); 				// Clear index state
		}
		cstrFbIndex.Format("%d", uiCountIndex);
		GetDlgItem(IDC_FB_INDEX)->SetWindowText(cstrFbIndex);

        acsc_ReadReal(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "IND", iSelChannel, iSelChannel, 0, 0, &dIndexPositionCurr, NULL);
		cstrFbIndex.Format("%6.0f", dIndexPositionCurr);
		GetDlgItem(IDC_FB_INDEX_POSITION)->SetWindowText(cstrFbIndex);

		if(mtn_api_get_relax_position_by_acs_axis(iSelChannel, &dRelaxPosn) == MTN_API_OK_ZERO)
		{
			sprintf_s(strTextTemp, 128, "%12.1f", dRelaxPosn);
			GetDlgItem(IDC_EDIT_SEARCH_HOME_OUT_DIST_INDEX_RELAX)->SetWindowText(_T(strTextTemp)); //20090129
		}
		else
		{
			sprintf_s(strTextTemp, 128, "NoAxis");
			GetDlgItem(IDC_EDIT_SEARCH_HOME_OUT_DIST_INDEX_RELAX)->SetWindowText(_T(strTextTemp)); //20090129
		}

	mtnapi_get_motor_state(stCommHandle_DlgFbMonitor_ACS, iSelChannel, &iMotorState, iDebug); 
	if(iMotorState & ACSC_MST_ENABLE) 
	{
		// now is enable
		CString cstrEnaDisMotorButton = "Disable";
		GetDlgItem(IDC_ENABLE_MOTOR)->SetWindowText(cstrEnaDisMotorButton);
	}
	else
	{
		CString cstrEnaDisMotorButton = "Enable";
		GetDlgItem(IDC_ENABLE_MOTOR)->SetWindowText(cstrEnaDisMotorButton);
	}

}

// IDC_CHECK_ENA_EFO207_FB_CHECK_BY1739_LEVEL
void MtnDialog_FbMonitor::OnBnClickedCheckEnaEfo207FbCheckBy1739Level()
{
	cFlagEnable_Group_EFO207_Fb_CheckBy_1739U_Level = ((CButton*) GetDlgItem(IDC_CHECK_ENA_EFO207_FB_CHECK_BY1739_LEVEL))->GetCheck();
	UI_Enable_Group_EFO207_Fb_CheckBy_1739U_Level(cFlagEnable_Group_EFO207_Fb_CheckBy_1739U_Level);
	uiTimerFreqFactor_10KHz = 5; UI_UpdateTriggerEFO_RatioPeriod();

	UI_OpenBoard1739CheckEFO_GoodNG();
}

extern BOOL para_get_flag_initialization(); // From MtnParameterDlg.cpp
#include "DigitalIO.h"  // 20081030
//#include "efo_monitor.h"
extern CDigitalIO theDigitalIO;

UINT MtnDialog_FbMonitor::MonitorThread()
{
	// every 100 ms communication history is updated
//	int Received;
	static	CString tmpString;
	static int iFlagLanguageOpt;
	iFlagLanguageOpt = get_sys_language_option();

	while (!m_fStopMonitorThread)
	{
		// 
		if(theDigitalIO.m_IoCardInfo.sNumOfDevices < 1)  // 20110620
		{
			GetDlgItem(IDC_COMBO_DIGITAL_INPUT_CARD_LIST)->EnableWindow(FALSE);
			GetDlgItem(IDC_COMBO_DIGITAL_OUTPUT_CARD_LIST)->EnableWindow(FALSE);
			GetDlgItem(IDC_COMBO_EFO_MONITOR_USE_1739U_CARD)->EnableWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDC_COMBO_DIGITAL_INPUT_CARD_LIST)->EnableWindow(TRUE);
			GetDlgItem(IDC_COMBO_DIGITAL_OUTPUT_CARD_LIST)->EnableWindow(TRUE);
			GetDlgItem(IDC_COMBO_EFO_MONITOR_USE_1739U_CARD)->EnableWindow(TRUE);
		}

		// ACS Feedback Monitor
		if(stCommHandle_DlgFbMonitor_ACS != ACSC_INVALID)
		{
			MonitorUpdateAcsFeedback();
		}

		// DigitalIO-1739U and 7248
		MonitorUpdateDigitalInputStatic(); // 20120823 MonitorUpdateDigitalIO1739Status();

		//CRect rectNow;
		//GetClientRect(&rectNow);
		//// Update the member variable, indicating the plot frame
		//m_iX_LeftBottom = rectNow.left;
		//m_iY_LeftBottom = rectNow.bottom;
		//m_iX_RightTop = rectNow.right;
		//m_iY_RightTop = rectNow.top;

		// Trigger by ACS and feedback by 1739U
		ThreadUpdateFrom_EFO_By_ACS_1739U();


		// Trigger and feedback both by 1739U
		ThreadUpdateFromEFO_BSD_By1739U(iFlagLanguageOpt);

		Sleep((int)(INTER_TRIG_FB_DELAY_MS));
		
		//IDC_STATIC_EFO_SUCCESS
		//
		sprintf_s(strEFO_Flag, 128, "EFO Count Trig: %d", uiCountTriggerEFO);
		GetDlgItem(IDC_STATIC_EFO_COUNT)->SetWindowTextA(_T(strEFO_Flag));
		sprintf_s(strEFO_Flag, 128, "OK: %d(B:%d)", uiCountSuccessEFO, usByteRead1739U_EFO_GoodNG); // lDrvHandleEFO_Monitor_1739U, 
		GetDlgItem(IDC_STATIC_EFO_SUCCESS)->SetWindowTextA(_T(strEFO_Flag));
		// IDC_STATIC_EFO_FAIL
		sprintf_s(strEFO_Flag, 128, "Fail: %d (D:%d, P:%d)", uiCountFailTrigEFO, usDigitalIO1739_ReadByteValueEFO_GoodNG, usBitPattern1739U_EFO_GoodNG);
		GetDlgItem(IDC_STATIC_EFO_FAIL)->SetWindowTextA(_T(strEFO_Flag));

	}
	return 0;
}

#define EFO_BURN_IN_PASS_THREAD_FAIL_COUNT      10
void MtnDialog_FbMonitor::ThreadUpdateFromEFO_BSD_By1739U(int iLanguageOption)
{
static int iCountFailureEFO_GoodNG;
CString tmpString;
	if(mEfoMonitor1739U.m_bRunEFO_MonitorThread1739U == TRUE)
	{
		tmpString.Format("%d", mEfoMonitor1739U.EFO_Monitor_1739U_GetSuccessCounter());
		GetDlgItem(IDC_STATIC_EFO_MONITOR_1739U_SUCCESS_COUNT)->SetWindowTextA(tmpString);

		iCountFailureEFO_GoodNG = (mEfoMonitor1739U.EFO_Monitor_1739U_GetTriggerCounter() - mEfoMonitor1739U.EFO_Monitor_1739U_GetSuccessCounter());
		tmpString.Format("%d", iCountFailureEFO_GoodNG);
		GetDlgItem(IDC_STATIC_FB_EFO_MONITOR_1739U_FAIL_COUNT)->SetWindowTextA(tmpString);
		tmpString.Format("%d", mEfoMonitor1739U.EFO_Monitor_1739U_GetTriggerCounter());
		GetDlgItem(IDC_STATIC_EFO_MONITOR_1739U_TRIG_COUNT)->SetWindowTextA(tmpString);

		if(mEfoMonitor1739U.EFO_Monitor_1739U_GetTriggerCounter() >= uiTotalCountTriggerEFO_BSD)
		{
			//mEfoMonitor1739U.EFO_StopMonitor_1739U();
			OnBnClickedButtonFbMonitorEfoGoodngBy1739u();
			Sleep(100);
//			char str
			if(iCountFailureEFO_GoodNG > EFO_BURN_IN_PASS_THREAD_FAIL_COUNT)
			{ // FAIL
				if(iLanguageOption == LANGUAGE_UI_EN)
				{
					AfxMessageBox(_T("EFO E-Torch BurnIn Fail, check connection"));
				}
				else
				{
					AfxMessageBox(_T("电打火烧球错误"));
				}
			}
			else
			{
				if(iLanguageOption == LANGUAGE_UI_EN)
				{
					AfxMessageBox(_T("EFO E-Torch BurnIn Pass"));
				}
				else
				{
					AfxMessageBox(_T("电打火烧球检测通过"));
				}
			}
		}
	}
}

void MtnDialog_FbMonitor::ThreadUpdateFrom_EFO_By_ACS_1739U()
{
		if(m_bStart_ACS_TriggerEFO_CheckBy1739 == TRUE)
		{
			ACS_TriggerEFO_ThenReset();
			uiCountTriggerEFO ++;
			Sleep(DELAY_SWITCH_BSD_AFTER_EFO); // At least 3 ms
			CheckEfoFbBy1739U_WbAddr();

			ACS_TriggerBSD_Relay();

			if(cFlagEnableTestDualFreeAirBall) // 20121018
			{
				ACS_SwitchDualFAB();
			}
		}
}

void MtnDialog_FbMonitor::RunMonitorThread()
{
	m_fStopMonitorThread = FALSE;
	m_pWinThread = AfxBeginThread(MonitorThreadProc, this);
	SetPriorityClass(m_pWinThread->m_hThread, REALTIME_PRIORITY_CLASS);

	m_pWinThread->m_bAutoDelete = FALSE;
}

void MtnDialog_FbMonitor::StopMonitorThread()
{
	if (m_pWinThread)
	{
		m_fStopMonitorThread = TRUE;
		WaitForSingleObject(m_pWinThread->m_hThread, 1000);
//		delete m_pWinThread;
		m_pWinThread = NULL;
	}
}

void MtnDialog_FbMonitor::OnBnClickedFbMonitorStart()
{
	if(	m_bStart_ACS_TriggerEFO_CheckBy1739 == FALSE)
	{
		m_bStart_ACS_TriggerEFO_CheckBy1739 = TRUE;
		GetDlgItem(IDC_FB_MONITOR_START)->SetWindowTextA(_T("StopTrigEFO"));
	}
	else
	{
		m_bStart_ACS_TriggerEFO_CheckBy1739 = FALSE;
		GetDlgItem(IDC_FB_MONITOR_START)->SetWindowTextA(_T("StartTrigEFO"));
	}

}
void MtnDialog_FbMonitor::ACS_TriggerBSD_Relay()
{
	int iOut1[1];
	// Trigger once, low
	if (!acsc_ReadInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, &iOut1[0], NULL ))  // Handle
	{
		sprintf_s(strDebugText, 128, "Error Read OUT0, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
	}

	iOut1[0] = iOut1[0] | 2;
	if (!acsc_WriteInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, iOut1, NULL ))  // 
	{
		sprintf_s(strDebugText, 128, "Error write OUT0, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
		return;
	}
	Sleep(6); // 1 ms, cannot be too long, 20100405

	iOut1[0] = iOut1[0] & 0xFFFFFFFD;
	if (!acsc_WriteInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, iOut1, NULL ))
	{
		sprintf_s(strDebugText, 128, "Error write OUT0, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
		return;
	}
}

void MtnDialog_FbMonitor::ACS_SwitchDualFAB()  // 20121018
{
	if (!acsc_ReadInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, &iOut0[0], NULL ))  // Handle
	{
		sprintf_s(strDebugText, 128, "Error Read OUT0, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
	}


	if((iOut0[0] & 0x8) == 0x8)
	{
		iOut0[0] = iOut0[0] & 0xFFFFFFF7;
	}
	else
	{
		iOut0[0] = iOut0[0] | 0x8;
	}
	if (!acsc_WriteInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, iOut0, NULL ))  // 
	{
		sprintf_s(strDebugText, 128, "Error write OUT0, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
		return;
	}
} // 20121018

void MtnDialog_FbMonitor::ACS_TriggerEFO_ThenReset()
{
    // 20121018
	if (!acsc_ReadInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, &iOut0[0], NULL ))  // Handle
	{
		sprintf_s(strDebugText, 128, "Error Read OUT0, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
	}
	// Trigger once, high
	iOut0[0] |= 1;  // 20121018
	if (!acsc_WriteInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, iOut0, NULL ))  // 
	{
		sprintf_s(strDebugText, 128, "Error write OUT0, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
		return;
	}
//	Sleep(1); // 1 ms, cannot be too long, 20100405
unsigned int uiCounterLast = GetPentiumTimeCount_per_100us(liFreqOS_FbMonitorDlg.QuadPart); // timeGetTime();
unsigned int uiCounterCurr = uiCounterLast; // timeGetTime();
		
	while((uiCounterCurr - uiCounterLast < uiTimerFreqFactor_10KHz) 
			||	( (double) uiCounterCurr + UINT_MAX - uiCounterLast < uiTimerFreqFactor_10KHz 
			     && (double)uiCounterLast + uiTimerFreqFactor_10KHz <= UINT_MAX
				 )
		  ) // if within uiTimerFreqFactor_10KHz * 0.1mSec 
	{	
		Sleep(0);
		uiCounterCurr = GetPentiumTimeCount_per_100us(liFreqOS_FbMonitorDlg.QuadPart);
	}

	iOut0[0] &=  0xFFFFFFFE;  // 20121018
	if (!acsc_WriteInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, iOut0, NULL ))
	{
		sprintf_s(strDebugText, 128, "Error write OUT0, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
		return;
	}

}
// IDC_EDIT_SEARCH_HOME_MOVE_VEL
void MtnDialog_FbMonitor::OnEnChangeEditSearchHomeMoveVel()
{
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_VEL)->GetWindowTextA( &strTextTemp[0], 128);
	stSearchIndexInput[iSelChannel].stSpeedProfileMove.dMaxVelocity = strtod(strTextTemp, &strStopString);
}
// IDC_EDIT_SEARCH_HOME_MOVE_Acc
void MtnDialog_FbMonitor::OnEnChangeEditSearchHomeMoveAcc()
{
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_Acc)->GetWindowTextA( &strTextTemp[0], 128);
	stSearchIndexInput[iSelChannel].stSpeedProfileMove.dMaxAcceleration = strtod(strTextTemp, &strStopString);
}
// IDC_EDIT_SEARCH_HOME_MOVE_DEC
void MtnDialog_FbMonitor::OnEnChangeEditSearchHomeMoveDec()
{
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_DEC)->GetWindowTextA( &strTextTemp[0], 128);
	stSearchIndexInput[iSelChannel].stSpeedProfileMove.dMaxDeceleration = strtod(strTextTemp, &strStopString);
}
// IDC_EDIT_SEARCH_HOME_MOVE_JERK
void MtnDialog_FbMonitor::OnEnChangeEditSearchHomeMoveJerk()
{
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_JERK)->GetWindowTextA( &strTextTemp[0], 128);
	stSearchIndexInput[iSelChannel].stSpeedProfileMove.dMaxJerk = strtod(strTextTemp, &strStopString);
}
// IDC_EDIT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ
void MtnDialog_FbMonitor::OnEnChangeEditSearchHomeDetectFreqFactor10khz()
{
	GetDlgItem(IDC_EDIT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ)->GetWindowTextA( &strTextTemp[0], 128);
	sscanf(strTextTemp, "%d", &stSearchIndexInput[iSelChannel].uiFreqFactor_10KHz_Detecting);
}
// IDC_EDIT_SEARCH_HOME_DETECT_MAX_DIST_PROT
void MtnDialog_FbMonitor::OnEnChangeEditSearchHomeDetectMaxDistProt()
{
	GetDlgItem(IDC_EDIT_SEARCH_HOME_DETECT_MAX_DIST_PROT)->GetWindowTextA( &strTextTemp[0], 128);
	stSearchIndexInput[iSelChannel].dMaxDistanceRangeProtection = strtod(strTextTemp, &strStopString);
}
// IDC_EDIT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT
void MtnDialog_FbMonitor::OnEnChangeEditSearchHomeMoveDistSearchLimit()
{
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT)->GetWindowTextA( &strTextTemp[0], 128);
	stSearchIndexInput[iSelChannel].dMoveDistanceBeforeSearchLimit = strtod(strTextTemp, &strStopString);
}
// IDC_EDIT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT
void MtnDialog_FbMonitor::OnEnChangeEditSearchHomeJogVelSearchLimit()
{
	GetDlgItem(IDC_EDIT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT)->GetWindowTextA( &strTextTemp[0], 128);
	stSearchIndexInput[iSelChannel].dVelJoggingLimit = strtod(strTextTemp, &strStopString);
}
// IDC_EDIT_SEARCH_HOME_PE_TH_SEARCH_LIMIT
void MtnDialog_FbMonitor::OnEnChangeEditSearchHomePeThSearchLimit()
{
	GetDlgItem(IDC_EDIT_SEARCH_HOME_PE_TH_SEARCH_LIMIT)->GetWindowTextA( &strTextTemp[0], 128);
	stSearchIndexInput[iSelChannel].dPositionErrorThresholdSearchLimit = strtod(strTextTemp, &strStopString);
}
// IDC_EDIT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1
void MtnDialog_FbMonitor::OnEnChangeEditSearchHomeMoveDistBfSearchIndex1()
{
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1)->GetWindowTextA( &strTextTemp[0], 128);
	stSearchIndexInput[iSelChannel].dMoveDistanceBeforeSearchIndex1 = strtod(strTextTemp, &strStopString);
}
// IDC_EDIT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1
void MtnDialog_FbMonitor::OnEnChangeEditSearchHomeJogVelSearchIndex1()
{
	GetDlgItem(IDC_EDIT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1)->GetWindowTextA( &strTextTemp[0], 128);
	stSearchIndexInput[iSelChannel].dVelJoggingIndex1 = strtod(strTextTemp, &strStopString);
}
// IDC_EDIT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2
void MtnDialog_FbMonitor::OnEnChangeEditSearchHomeMoveDistBfSearchIndex2()
{
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2)->GetWindowTextA( &strTextTemp[0], 128);
	stSearchIndexInput[iSelChannel].dMoveDistanceBeforeSearchIndex2 = strtod(strTextTemp, &strStopString);
}
// IDC_EDIT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_2
void MtnDialog_FbMonitor::OnEnChangeEditSearchHomeJogVelSearchIndex2()
{
	GetDlgItem(IDC_EDIT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_2)->GetWindowTextA( &strTextTemp[0], 128);
	stSearchIndexInput[iSelChannel].dVelJoggingIndex2 = strtod(strTextTemp, &strStopString);
}

// IDC_BUTTON_AXIS_GO_HOME  // output // IDC_EDIT_SEARCH_HOME_OUT_LIMIT_DIST_INDEX_1 // IDC_EDIT_SEARCH_HOME_OUT_LIMIT_DIST_INDEX_2
//#undef MOTALGO_DLL_EXPORTS
//#include "MotAlgo_DLL.h"

void MtnDialog_FbMonitor::OnBnClickedButtonAxisGoHome()
{
	if(mtester_fb_search_home_acs_axis(stCommHandle_DlgFbMonitor_ACS, iSelChannel) != MTN_API_OK_ZERO)
	{
//		mtnapi_get_debug_message_home_acs(strDebugMessage);
		sprintf_s(strDebugText, 1024, "Error Code: %d - %s", stSearchIndexOutput[iSelChannel].iErrorCode, strDebugErrorMessageHomeACS);
		AfxMessageBox(strDebugText);
	}
	else
	{
		double dAveDistBtnLimitIndex = (stSearchIndexOutput[iSelChannel].dDistanceFromIndex1ToLimit + stSearchIndexOutput[iSelChannel].dDistanceFromIndex2ToLimit)/2;
		cstrEditText.Format("%6.0f", dAveDistBtnLimitIndex);
		GetDlgItem(IDC_EDIT_SEARCH_HOME_OUT_LIMIT_DIST_INDEX_1)->SetWindowText(cstrEditText);
		cstrEditText.Format("%6.0f", stSearchIndexOutput[iSelChannel].dIndexPositionFineSearch_1);
		GetDlgItem(IDC_EDIT_SEARCH_HOME_OUT_INDEX_1)->SetWindowText(cstrEditText);
		cstrEditText.Format("%6.0f", stSearchIndexOutput[iSelChannel].dIndexPositionFineSearch_2);
		GetDlgItem(IDC_EDIT_SEARCH_HOME_OUT_INDEX_2)->SetWindowText(cstrEditText);
	}
}

// IDC_CHECK_DEBUG_SEARCH_INDEX
void MtnDialog_FbMonitor::OnBnClickedCheckDebugSearchIndex()
{
	cFlagEnable_Group_Setting_ACS_ServoHome_MtnDialog_FbMonitor = ((CButton *)GetDlgItem(IDC_CHECK_DEBUG_SEARCH_INDEX))->GetCheck();
    stSearchIndexInput[iSelChannel].iDebug = cFlagEnable_Group_Setting_ACS_ServoHome_MtnDialog_FbMonitor;
	UI_Enable_Group_Setting_ACS_ServoHome_MtnDialog_FbMonitor(
		cFlagEnable_Group_Setting_ACS_ServoHome_MtnDialog_FbMonitor 
		&& cFlagEnable_Group_ACS_ServoHome_MtnDialog_FbMonitor);
}
// IDC_SEARCH_HOME_POSN_ERR_SETTLE
void MtnDialog_FbMonitor::OnEnChangeSearchHomePosnErrSettle()
{
	GetDlgItem(IDC_SEARCH_HOME_POSN_ERR_SETTLE)->GetWindowTextA( &strTextTemp[0], 128);
	stSearchIndexInput[iSelChannel].dPosnErrThresHoldSettling = strtod(strTextTemp, &strStopString);
}

// IDC_BUTTON_AXIS_GO_LIMIT
void MtnDialog_FbMonitor::OnBnClickedButtonAxisGoLimit()
{
	stSearchLimitInput[iSelChannel].dMaxDistanceRangeProtection = stSearchIndexInput[iSelChannel].dMaxDistanceRangeProtection;
	stSearchLimitInput[iSelChannel].dMoveDistanceAfterSearchLimit = stSearchIndexInput[iSelChannel].dMoveDistanceBeforeSearchIndex1/2; // Before search index, is usually search limit
	stSearchLimitInput[iSelChannel].dMoveDistanceBeforeSearchLimit = stSearchIndexInput[iSelChannel].dMoveDistanceBeforeSearchLimit;
	stSearchLimitInput[iSelChannel].dPositionErrorThresholdSearchLimit = stSearchIndexInput[iSelChannel].dPositionErrorThresholdSearchLimit;
	stSearchLimitInput[iSelChannel].dPosnErrThresHoldSettling = stSearchIndexInput[iSelChannel].dPosnErrThresHoldSettling;
	stSearchLimitInput[iSelChannel].dVelJoggingLimit = stSearchIndexInput[iSelChannel].dVelJoggingLimit;
	stSearchLimitInput[iSelChannel].iAxisOnACS = stSearchIndexInput[iSelChannel].iAxisOnACS;  // 20100111, iSelChannel;
	stSearchLimitInput[iSelChannel].iDebug = stSearchIndexInput[iSelChannel].iDebug;
	stSearchLimitInput[iSelChannel].stSpeedProfileMove = stSearchIndexInput[iSelChannel].stSpeedProfileMove;
	stSearchLimitInput[iSelChannel].uiFreqFactor_10KHz_Detecting = stSearchIndexInput[iSelChannel].uiFreqFactor_10KHz_Detecting;

	if(mtn_search_limit_acs(stCommHandle_DlgFbMonitor_ACS, &stSearchLimitInput[iSelChannel], &stSearchLimitOutput[iSelChannel]) == MTN_API_ERROR)
	{
//		mtnapi_get_debug_message_home_acs(strDebugMessage);
		sprintf_s(strDebugText, 1024, "Error Code: %d - %s", stSearchLimitOutput[iSelChannel].iErrorCode, strDebugErrorMessageHomeACS);
		AfxMessageBox(strDebugText);
	}
	else
	{
		double dAveDistBtnLimitIndex = stSearchLimitOutput[iSelChannel].dLimitPosition;
		cstrEditText.Format("%6.0f", dAveDistBtnLimitIndex);
		GetDlgItem(IDC_EDIT_SEARCH_HOME_OUT_LIMIT_DIST_INDEX_1)->SetWindowText(cstrEditText);
	}

}
// IDC_BUTTON_AXIS_GO_HOME_FROM_LIMIT
void MtnDialog_FbMonitor::OnBnClickedButtonAxisGoHomeFromLimit()
{
	stSearchHomeFromLimitInput[iSelChannel].dMaxDistanceRangeProtection = stSearchIndexInput[iSelChannel].dMaxDistanceRangeProtection;
	stSearchHomeFromLimitInput[iSelChannel].dMoveDistanceBeforeSearchIndex1 = stSearchIndexInput[iSelChannel].dMoveDistanceBeforeSearchIndex1/2;
	stSearchHomeFromLimitInput[iSelChannel].dMoveDistanceBeforeSearchIndex2 = stSearchIndexInput[iSelChannel].dMoveDistanceBeforeSearchIndex2;
	stSearchHomeFromLimitInput[iSelChannel].dPosnErrThresHoldSettling = stSearchIndexInput[iSelChannel].dPosnErrThresHoldSettling;
	stSearchHomeFromLimitInput[iSelChannel].dVelJoggingIndex1 = stSearchIndexInput[iSelChannel].dVelJoggingIndex1;
	stSearchHomeFromLimitInput[iSelChannel].dVelJoggingIndex2 = stSearchIndexInput[iSelChannel].dVelJoggingIndex2;
	stSearchHomeFromLimitInput[iSelChannel].iDebug = stSearchIndexInput[iSelChannel].iDebug;
	stSearchHomeFromLimitInput[iSelChannel].stSpeedProfileMove = stSearchIndexInput[iSelChannel].stSpeedProfileMove;
	stSearchHomeFromLimitInput[iSelChannel].iAxisOnACS = stSearchIndexInput[iSelChannel].iAxisOnACS;  // 20100111, iSelChannel;
	stSearchHomeFromLimitInput[iSelChannel].uiFreqFactor_10KHz_Detecting = stSearchIndexInput[iSelChannel].uiFreqFactor_10KHz_Detecting;

	if(mtn_search_home_from_limit_acs(stCommHandle_DlgFbMonitor_ACS, &stSearchHomeFromLimitInput[iSelChannel], &stSearchHomeFromLimitOutput[iSelChannel]) == MTN_API_ERROR)
	{
//		mtnapi_get_debug_message_home_acs(strDebugMessage);
		sprintf_s(strDebugText, 128, "Error Code: %d - %s", stSearchHomeFromLimitOutput[iSelChannel].iErrorCode, strDebugErrorMessageHomeACS);
		AfxMessageBox(strDebugText);
	}
	else
	{
		double dAveDistBtnLimitIndex = (stSearchHomeFromLimitOutput[iSelChannel].dDistanceFromIndex1ToLimit + stSearchHomeFromLimitOutput[iSelChannel].dDistanceFromIndex2ToLimit)/2;
		cstrEditText.Format("%6.0f", dAveDistBtnLimitIndex);
		GetDlgItem(IDC_EDIT_SEARCH_HOME_OUT_LIMIT_DIST_INDEX_1)->SetWindowText(cstrEditText);
		cstrEditText.Format("%6.0f", stSearchHomeFromLimitOutput[iSelChannel].dIndexPositionFineSearch_1);
		GetDlgItem(IDC_EDIT_SEARCH_HOME_OUT_INDEX_1)->SetWindowText(cstrEditText);
		cstrEditText.Format("%6.0f", stSearchHomeFromLimitOutput[iSelChannel].dIndexPositionFineSearch_2);
		GetDlgItem(IDC_EDIT_SEARCH_HOME_OUT_INDEX_2)->SetWindowText(cstrEditText);
	}
	
}

// IDC_BUTTON_AXIS_API_GO_LIMIT_AND_HOME
void MtnDialog_FbMonitor::OnBnClickedButtonAxisApiGoLimitAndHome()
{
	stSearchHomeInputAPI[iSelChannel].dMaxDistanceRangeProtection = stSearchIndexInput[iSelChannel].dMaxDistanceRangeProtection;
	stSearchHomeInputAPI[iSelChannel].dMoveDistanceBeforeSearchIndex1 = stSearchIndexInput[iSelChannel].dMoveDistanceBeforeSearchIndex1 ;
	stSearchHomeInputAPI[iSelChannel].dMoveDistanceBeforeSearchIndex2 = stSearchIndexInput[iSelChannel].dMoveDistanceBeforeSearchIndex2 ;
	stSearchHomeInputAPI[iSelChannel].dMoveDistanceBeforeSearchLimit = stSearchIndexInput[iSelChannel].dMoveDistanceBeforeSearchLimit;
	stSearchHomeInputAPI[iSelChannel].dMoveMaxVelInterSearch = stSearchIndexInput[iSelChannel].stSpeedProfileMove.dMaxVelocity ;
	stSearchHomeInputAPI[iSelChannel].dPositionErrorThresholdSearchLimit = stSearchIndexInput[iSelChannel].dPositionErrorThresholdSearchLimit ;
	stSearchHomeInputAPI[iSelChannel].dPosnErrThresHoldSettling = stSearchIndexInput[iSelChannel].dPosnErrThresHoldSettling ;
	stSearchHomeInputAPI[iSelChannel].dVelJoggingIndex1 = stSearchIndexInput[iSelChannel].dVelJoggingIndex1 ;
	stSearchHomeInputAPI[iSelChannel].dVelJoggingIndex2 = stSearchIndexInput[iSelChannel].dVelJoggingIndex2 ;
	stSearchHomeInputAPI[iSelChannel].dVelJoggingLimit = stSearchIndexInput[iSelChannel].dVelJoggingLimit ;
	stSearchHomeInputAPI[iSelChannel].iAxisOnACS = stSearchIndexInput[iSelChannel].iAxisOnACS;  // 20100111, iSelChannel ;
	stSearchHomeInputAPI[iSelChannel].iDebug = stSearchIndexInput[iSelChannel].iDebug ;

	if(mtn_api_search_home_acs(stCommHandle_DlgFbMonitor_ACS, &stSearchHomeInputAPI[iSelChannel], &stSearchHomeOutputAPI[iSelChannel]) == MTN_API_ERROR)
	{
//		mtnapi_get_debug_message_home_acs(strDebugMessage);
		sprintf_s(strDebugText, 1024, "Error Code: %d - %s", stSearchHomeOutputAPI[iSelChannel].iErrorCode, strDebugErrorMessageHomeACS);
		AfxMessageBox(strDebugText);
	}
	else
	{
		cstrEditText.Format("%6.0f", stSearchHomeOutputAPI[iSelChannel].dIndexPositionFineSearch_1);
		GetDlgItem(IDC_EDIT_SEARCH_HOME_OUT_INDEX_1)->SetWindowText(cstrEditText);
		cstrEditText.Format("%6.0f", stSearchHomeOutputAPI[iSelChannel].dIndexPositionFineSearch_2);
		GetDlgItem(IDC_EDIT_SEARCH_HOME_OUT_INDEX_2)->SetWindowText(cstrEditText);
	}
}

// IDC_BUTTON_AXIS_API_GO_LIMIT
void MtnDialog_FbMonitor::OnBnClickedButtonAxisApiGoLimit()
{
	stSearchLimitInputAPI[iSelChannel].dMaxDistanceRangeProtection = stSearchIndexInput[iSelChannel].dMaxDistanceRangeProtection;
	stSearchLimitInputAPI[iSelChannel].dMoveDistanceAfterSearchLimit = stSearchIndexInput[iSelChannel].dMoveDistanceBeforeSearchIndex1 / 2;
	stSearchLimitInputAPI[iSelChannel].dMoveDistanceBeforeSearchLimit = stSearchIndexInput[iSelChannel].dMoveDistanceBeforeSearchLimit;
	stSearchLimitInputAPI[iSelChannel].dMoveMaxVelInterSearch = stSearchIndexInput[iSelChannel].stSpeedProfileMove.dMaxVelocity;
	stSearchLimitInputAPI[iSelChannel].dPositionErrorThresholdSearchLimit = stSearchIndexInput[iSelChannel].dPositionErrorThresholdSearchLimit;
	stSearchLimitInputAPI[iSelChannel].dPosnErrThresHoldSettling = stSearchIndexInput[iSelChannel].dPosnErrThresHoldSettling;
	stSearchLimitInputAPI[iSelChannel].dVelJoggingLimit = stSearchIndexInput[iSelChannel].dVelJoggingLimit;
	stSearchLimitInputAPI[iSelChannel].iAxisOnACS = stSearchIndexInput[iSelChannel].iAxisOnACS;  // 20100111, iSelChannel;

	if(mtn_api_search_limit_acs(stCommHandle_DlgFbMonitor_ACS, &stSearchLimitInputAPI[iSelChannel], &stSearchLimitOutputAPI[iSelChannel]) == MTN_API_ERROR)
	{
//		mtnapi_get_debug_message_home_acs(strDebugMessage);
		sprintf_s(strDebugText, 128, "Error Code: %d - %s", stSearchLimitOutputAPI[iSelChannel].iErrorCode, strDebugErrorMessageHomeACS);
		AfxMessageBox(strDebugText);
	}
	else
	{
		double dAveDistBtnLimitIndex = stSearchLimitOutputAPI[iSelChannel].dLimitPosition;
		cstrEditText.Format("%6.0f", dAveDistBtnLimitIndex);
		GetDlgItem(IDC_EDIT_SEARCH_HOME_OUT_LIMIT_DIST_INDEX_1)->SetWindowText(cstrEditText);
	}
}
// IDC_BUTTON_AXIS_API_GO_HOME_FROM_LIMIT
void MtnDialog_FbMonitor::OnBnClickedButtonAxisApiGoHomeFromLimit()
{
	stSearchHomeFromLimitInputAPI[iSelChannel].dMaxDistanceRangeProtection = stSearchIndexInput[iSelChannel].dMaxDistanceRangeProtection;
	stSearchHomeFromLimitInputAPI[iSelChannel].dMoveDistanceBeforeSearchIndex1 = stSearchIndexInput[iSelChannel].dMoveDistanceBeforeSearchIndex1 / 2;
	stSearchHomeFromLimitInputAPI[iSelChannel].dMoveDistanceBeforeSearchIndex2 = stSearchIndexInput[iSelChannel].dMoveDistanceBeforeSearchIndex2;
	stSearchHomeFromLimitInputAPI[iSelChannel].dMoveMaxVelInterSearch = stSearchIndexInput[iSelChannel].stSpeedProfileMove.dMaxVelocity;
	stSearchHomeFromLimitInputAPI[iSelChannel].dPosnErrThresHoldSettling = stSearchIndexInput[iSelChannel].dPosnErrThresHoldSettling;
	stSearchHomeFromLimitInputAPI[iSelChannel].dVelJoggingIndex1 = stSearchIndexInput[iSelChannel].dVelJoggingIndex1;
	stSearchHomeFromLimitInputAPI[iSelChannel].dVelJoggingIndex2 = stSearchIndexInput[iSelChannel].dVelJoggingIndex2;
	stSearchHomeFromLimitInputAPI[iSelChannel].iAxisOnACS = stSearchIndexInput[iSelChannel].iAxisOnACS;  // 20100111, 
	stSearchHomeFromLimitInputAPI[iSelChannel].iDebug = stSearchIndexInput[iSelChannel].iDebug;

	if(mtn_api_search_home_from_limit_acs(stCommHandle_DlgFbMonitor_ACS, &stSearchHomeFromLimitInputAPI[iSelChannel], &stSearchHomeFromLimitOutputAPI[iSelChannel]) == MTN_API_ERROR)
	{
//		mtnapi_get_debug_message_home_acs(strDebugMessage);
		sprintf_s(strDebugText, 128, "Error Code: %d - %s", stSearchHomeFromLimitOutputAPI[iSelChannel].iErrorCode, strDebugErrorMessageHomeACS);
		AfxMessageBox(strDebugText);
	}
	else
	{
		cstrEditText.Format("%6.0f", stSearchHomeFromLimitOutputAPI[iSelChannel].dIndexPositionFineSearch_1);
		GetDlgItem(IDC_EDIT_SEARCH_HOME_OUT_INDEX_1)->SetWindowText(cstrEditText);
		cstrEditText.Format("%6.0f", stSearchHomeFromLimitOutputAPI[iSelChannel].dIndexPositionFineSearch_2);
		GetDlgItem(IDC_EDIT_SEARCH_HOME_OUT_INDEX_2)->SetWindowText(cstrEditText);
	}
	
}
// IDC_CHECK_SEARCH_INDEX_VERIFY_BY_REPEATING
void MtnDialog_FbMonitor::OnBnClickedCheckSearchIndexVerifyByRepeating()
{
	stSearchIndexInput[iSelChannel].iFlagVerifyByRepeatingIndex2 = ((CButton *)GetDlgItem(IDC_CHECK_SEARCH_INDEX_VERIFY_BY_REPEATING))->GetCheck();
}


void MtnDialog_FbMonitor::OnBnClickedButtonBonderHomeAll()
{
	mtn_bonder_xyz_start_search_limit_go_home(stCommHandle_DlgFbMonitor_ACS, _EFSIKA_TABLE_X_AXIS, _EFSIKA_TABLE_Y_AXIS, _EFSIKA_BOND_Z_AXIS);
}

// IDC_CHECK_ACS_OUT1
//int iOut1[1], iOut2[1], iOut3[1];


void MtnDialog_FbMonitor::SetUserInterfaceLanguage(int iLanguageOption)
{
	if(iLanguageOption == LANGUAGE_UI_EN)
	{
		GetDlgItem(IDC_GROUP_LABEL_DIGITAL_IO_1739U)->SetWindowTextA(_T("DigitalIO-1739U"));
		GetDlgItem(IDC_STATIC_DIGITAL_INPUT_CARD_1739U)->SetWindowTextA(_T("DI Cards"));
		GetDlgItem(IDC_STATIC_DIGITAL_INPUT_BYTE_1739U)->SetWindowTextA(_T("Input Byte"));
		GetDlgItem(IDC_STATIC_DIGITAL_OUTPUT_CARD_1739U)->SetWindowTextA(_T("DO Cards"));
		GetDlgItem(IDC_STATIC_DIGITAL_OUTPUT_BYTE_1739U)->SetWindowTextA(_T("Output Byte"));

		GetDlgItem(IDC_GROUP_LABEL_ACS_IO)->SetWindowTextA(_T("ServoIO-ACS"));
		GetDlgItem(IDC_GROUP_LABEL_SERVO_CHANNEL)->SetWindowTextA(_T("ServoAxisFeedbk-ACS"));
		GetDlgItem(IDC_STATIC_FB_MONITOR_CHANNEL)->SetWindowTextA(_T("Channel"));
		GetDlgItem(IDC_LABEL_FB_POSITION)->SetWindowTextA(_T("Position"));
		GetDlgItem(IDC_LABEL_FB_FORCE)->SetWindowTextA(_T("Force"));
		GetDlgItem(IDC_LABEL_FB_LIMIT_SENSOR)->SetWindowTextA(_T("Limit"));
		GetDlgItem(IDC_LABEL_FB_VELOCITY)->SetWindowTextA(_T("Velocity"));
		GetDlgItem(IDC_LABEL_FB_ACCELERATION)->SetWindowTextA(_T("Acceleration"));
		GetDlgItem(IDC_LABEL_FB_INDEX)->SetWindowTextA(_T("IdxCount"));
		GetDlgItem(IDC_STATIC_FB_MONITOR_INDEX_POSITION)->SetWindowTextA(_T("IdxPosition"));
		GetDlgItem(IDC_ENABLE_MOTOR)->SetWindowTextA(_T("Enable"));
		GetDlgItem(IDC_GROUP_LABEL_SEARCH_HOME)->SetWindowTextA(_T("Search Limit/Home Above Axis"));
		GetDlgItem(IDC_BUTTON_FB_MONITOR_HOME_ALL_API)->SetWindowTextA(_T("HomeAllAPI"));

		GetDlgItem(IDC_CHECK_ENA_EFO207_FB_CHECK_BY1739_LEVEL)->SetWindowTextA(_T("EFO Fb Check"));
		GetDlgItem(IDC_CHECK_ENA_ACS_IO)->SetWindowTextA(_T("Do ACS-OUT"));
		GetDlgItem(IDC_CHECK_ENA_FLAG_BURN_ETORCH_1739)->SetWindowTextA(_T("Do BurnE-Torch_1739"));
		GetDlgItem(IDC_CHECK_FLAG_ENA_HOME_ACS_SERVO_CH)->SetWindowTextA(_T("Do ACS-Homing"));

		GetDlgItem(IDC_BUTTON_AXIS_API_GO_LIMIT_AND_HOME)->SetWindowTextA(_T("API L+M"));
		GetDlgItem(IDC_BUTTON_AXIS_API_GO_LIMIT)->SetWindowTextA(_T("Go Limit"));
		GetDlgItem(IDC_BUTTON_AXIS_API_GO_HOME_FROM_LIMIT)->SetWindowTextA(_T("Hom f.Lmt"));
		GetDlgItem(IDC_BUTTON_BONDER_HOME_ALL)->SetWindowTextA(_T("WB_Init"));

		GetDlgItem(IDC_IDC_STATIC_LABEL_1739BURN_TRIG_EFO_BIT)->SetWindowTextA(_T("TrigEFO: 0"));                // 20121026
		GetDlgItem(IDC_IDC_STATIC_LABEL_1739BURN_TRIG_RELAY_ExorB_BIT)->SetWindowTextA(_T("TrigExorB: 4"));
		GetDlgItem(IDC_IDC_STATIC_LABEL_1739BURN_TRIG_RELAY_DualFAB_BIT)->SetWindowTextA(_T("DualFAB: 5"));		
		GetDlgItem(IDC_IDC_STATIC_LABEL_1739BURN_TRIG_BSD)->SetWindowTextA(_T("TrigBSD: 7"));                    // 20121026

		GetDlgItem(IDC_BUTTON_FB_MONITOR_EFO_GOODNG_BY_1739U)->SetWindowTextA(_T("Auto.EFO&BSD"));
		
		
	}
	else
	{
		GetDlgItem(IDC_GROUP_LABEL_DIGITAL_IO_1739U)->SetWindowTextA(_T("开关量-1739U"));
		GetDlgItem(IDC_STATIC_DIGITAL_INPUT_CARD_1739U)->SetWindowTextA(_T("开关入卡"));
		GetDlgItem(IDC_STATIC_DIGITAL_INPUT_BYTE_1739U)->SetWindowTextA(_T("开关入字節"));
		GetDlgItem(IDC_STATIC_DIGITAL_OUTPUT_CARD_1739U)->SetWindowTextA(_T("开关出卡"));
		GetDlgItem(IDC_STATIC_DIGITAL_OUTPUT_BYTE_1739U)->SetWindowTextA(_T("开关出字節"));
		GetDlgItem(IDC_GROUP_LABEL_ACS_IO)->SetWindowTextA(_T("开关量-ACS"));
		GetDlgItem(IDC_GROUP_LABEL_SERVO_CHANNEL)->SetWindowTextA(_T("伺服控制反馈"));
		GetDlgItem(IDC_STATIC_FB_MONITOR_CHANNEL)->SetWindowTextA(_T("伺服通道"));
		GetDlgItem(IDC_LABEL_FB_POSITION)->SetWindowTextA(_T("位置"));
		GetDlgItem(IDC_LABEL_FB_FORCE)->SetWindowTextA(_T("压力"));
		GetDlgItem(IDC_LABEL_FB_LIMIT_SENSOR)->SetWindowTextA(_T("极限"));
		GetDlgItem(IDC_LABEL_FB_VELOCITY)->SetWindowTextA(_T("速度"));
		GetDlgItem(IDC_LABEL_FB_ACCELERATION)->SetWindowTextA(_T("加速度"));
		GetDlgItem(IDC_LABEL_FB_INDEX)->SetWindowTextA(_T("刻标次数"));
		GetDlgItem(IDC_STATIC_FB_MONITOR_INDEX_POSITION)->SetWindowTextA(_T("刻标位置"));
		GetDlgItem(IDC_ENABLE_MOTOR)->SetWindowTextA(_T("马达开关"));
		GetDlgItem(IDC_GROUP_LABEL_SEARCH_HOME)->SetWindowTextA(_T("伺服通道找寻原点"));
		GetDlgItem(IDC_BUTTON_FB_MONITOR_HOME_ALL_API)->SetWindowTextA(_T("XYZ回零"));
		GetDlgItem(IDC_CHECK_ENA_EFO207_FB_CHECK_BY1739_LEVEL)->SetWindowTextA(_T("执行电打火反馈检测"));
		GetDlgItem(IDC_CHECK_ENA_ACS_IO)->SetWindowTextA(_T("执行控制卡ACS-开关量输出"));
		GetDlgItem(IDC_CHECK_ENA_FLAG_BURN_ETORCH_1739)->SetWindowTextA(_T("执行1739U烤机打火杆"));
		GetDlgItem(IDC_CHECK_FLAG_ENA_HOME_ACS_SERVO_CH)->SetWindowTextA(_T("执行伺服马达归零"));

		GetDlgItem(IDC_BUTTON_AXIS_API_GO_LIMIT_AND_HOME)->SetWindowTextA(_T("回零"));
		GetDlgItem(IDC_BUTTON_AXIS_API_GO_LIMIT)->SetWindowTextA(_T("去极限"));
		GetDlgItem(IDC_BUTTON_AXIS_API_GO_HOME_FROM_LIMIT)->SetWindowTextA(_T("极限归零"));
		GetDlgItem(IDC_BUTTON_BONDER_HOME_ALL)->SetWindowTextA(_T("焊线机初始化"));

		GetDlgItem(IDC_IDC_STATIC_LABEL_1739BURN_TRIG_EFO_BIT)->SetWindowTextA(_T("触发打火: 0"));          // 20121026
		GetDlgItem(IDC_IDC_STATIC_LABEL_1739BURN_TRIG_RELAY_ExorB_BIT)->SetWindowTextA(_T("火异或测: 4"));
		GetDlgItem(IDC_IDC_STATIC_LABEL_1739BURN_TRIG_BSD)->SetWindowTextA(_T("触发检测: 7"));

		GetDlgItem(IDC_IDC_STATIC_LABEL_1739BURN_TRIG_RELAY_DualFAB_BIT)->SetWindowTextA(_T("双球切换: 5")); // 20121026

		GetDlgItem(IDC_BUTTON_FB_MONITOR_EFO_GOODNG_BY_1739U)->SetWindowTextA(_T("自动打火检测"));
	}
}


#include "MtnTesterResDef.h"
#include "MtnApiSearchHome.h"

void MtnDialog_FbMonitor::OnBnClickedButtonFbMonitorHomeAllApi()
{
static double dPositionTableX_StartBonding_HomeZ ;

	// Init default parameter
	if(iFlagMachineType == WB_MACH_TYPE_VLED_FORK)
	{
		mtn_init_def_para_search_index_vled_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
		dPositionTableX_StartBonding_HomeZ = BE_WB_TABLE_POSN_X_START_BOND_Z_HOME;
	}
	else
	{
		mtn_dll_init_def_para_search_index_hori_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
		dPositionTableX_StartBonding_HomeZ = BE_WB_TABLE_POSN_X_START_HORI_BONDER_Z_HOME;
	}

	// IDC_BUTTON_FB_MONITOR_HOME_ALL_API
	if(mtn_axis_search_limit(stCommHandle_DlgFbMonitor_ACS, sys_get_acs_axis_id_bnd_z()) == MTN_API_ERROR)
	{
		AfxMessageBox("Search Limit Error Bonder-Z");
		return ;
	}

	// (2) Home Y: Positive Limit, motor size, far away from lead-frame
	if(mtn_axis_search_limit_go_home(stCommHandle_DlgFbMonitor_ACS, APP_Y_TABLE_ACS_ID) == MTN_API_ERROR)
	{
		AfxMessageBox("Search Limit & Home Error Table-X");
		return ;
	}

	// (3) Home X: 
	if(mtn_axis_search_limit_go_home(stCommHandle_DlgFbMonitor_ACS, APP_X_TABLE_ACS_ID) == MTN_API_ERROR)
	{
		AfxMessageBox("Search Limit & Home Error Table-Y");
		return ;
	}

	// (3.5) X Table goto Start Bondering Position,  
	if(! acsc_ToPoint(stCommHandle_DlgFbMonitor_ACS, 0, // start up the motion immediately
			APP_X_TABLE_ACS_ID, dPositionTableX_StartBonding_HomeZ, NULL) )
	{
		CString cstrTemp;
		cstrTemp.Format("Error X goto start-bonding position, s.t. Z home down! [%s %d] \n",__FILE__, __LINE__);
		AfxMessageBox(cstrTemp);
		return;
	}

	// (4) Home Z
	if(mtn_axis_go_home_from_limit(stCommHandle_DlgFbMonitor_ACS, sys_get_acs_axis_id_bnd_z()) == MTN_API_ERROR)
	{
		AfxMessageBox("Search Home Error Bonder-Z");
		return ;
	}

	// (4.5) Z Goto FireLevel
static double dPositionBondHead_FireLevel = BE_WB_BH_FIRE_LEVEL;
	if(! acsc_ToPoint(stCommHandle_DlgFbMonitor_ACS, 0, // start up the motion immediately
			sys_get_acs_axis_id_bnd_z(), dPositionBondHead_FireLevel, NULL) )
	{
		CString cstrTemp;
		cstrTemp.Format("Error Z goto FireLevel! [%s %d] \n",__FILE__, __LINE__);
		AfxMessageBox(cstrTemp);
		return;
	}
}

// IDC_CHECK_FLAG_ENA_ACS_CH_MONITOR
void MtnDialog_FbMonitor::OnBnClickedCheckFlagEnaAcsChMonitor()
{
	cFlagEnable_Group_Servo_ACS_ChannelFbMonitor = 
				((CButton*) GetDlgItem(IDC_CHECK_FLAG_ENA_ACS_CH_MONITOR))->GetCheck();

	UI_Enable_Group_Servo_ACS_FbMonitorDlg(cFlagEnable_Group_Servo_ACS_ChannelFbMonitor);
}


// IDC_CHECK_FLAG_ENA_HOME_ACS_SERVO_CH
void MtnDialog_FbMonitor::OnBnClickedCheckFlagEnaHomeAcsServoCh()
{
	cFlagEnable_Group_ACS_ServoHome_MtnDialog_FbMonitor = 
		((CButton*) GetDlgItem(IDC_CHECK_FLAG_ENA_HOME_ACS_SERVO_CH))->GetCheck();
	UI_Enable_Group_ACS_ServoHome_MtnDialog_FbMonitor(cFlagEnable_Group_ACS_ServoHome_MtnDialog_FbMonitor);
}

void MtnDialog_FbMonitor::UI_Enable_Group_ACS_ServoHome_MtnDialog_FbMonitor(BOOL bEnableFlag)
{
	UI_Enable_Group_Setting_ACS_ServoHome_MtnDialog_FbMonitor(bEnableFlag && cFlagEnable_Group_Setting_ACS_ServoHome_MtnDialog_FbMonitor);
	UI_Enable_Group_Normal_ACS_ServoHome_MtnDialog_FbMonitor(bEnableFlag);
}

// IDC_CHECK_ENA_FLAG_BURN_ETORCH_1739
void MtnDialog_FbMonitor::OnBnClickedCheckEnaFlagBurnEtorch1739()
{
	cFlagEnable_Group_1739burn_Efo207 = ((CButton*) GetDlgItem(IDC_CHECK_ENA_FLAG_BURN_ETORCH_1739))->GetCheck();
	UI_Enable_Group_1739burn_Efo207(cFlagEnable_Group_1739burn_Efo207);
}

void MtnDialog_FbMonitor::UI_Enable_Group_1739burn_Efo207(BOOL bEnableFlag)
{
	GetDlgItem(IDC_STATIC_LABEL_1739BURN_EFO207_FAIL_COUNT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_LABEL_1739BURN_EFO207_GOOD_COUNT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_LABEL_1739BURN_EFO207_TRIG_COUNT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_EFO_MONITOR_1739U_DEBUG_FLAG)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_COMBO_EFO_MONITOR_USE_1739U_CARD)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_FB_MONITOR_EFO_GOODNG_BY_1739U)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_FB_EFO_MONITOR_1739U_FAIL_COUNT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_EFO_MONITOR_1739U_SUCCESS_COUNT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_EFO_MONITOR_1739U_TRIG_COUNT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_COMBO_EFO_TRIG_USE_1739U_BYTE)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_GROUP_1739BURN_EFO207)->ShowWindow(bEnableFlag);

	GetDlgItem(IDC_IDC_STATIC_LABEL_1739BURN_TRIG_EFO_BIT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_IDC_STATIC_LABEL_1739BURN_TRIG_RELAY_ExorB_BIT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_IDC_STATIC_LABEL_1739BURN_TRIG_BSD)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_IDC_STATIC_LABEL_1739BURN_TRIG_RELAY_DualFAB_BIT)->ShowWindow(bEnableFlag); // 20121026
	
	// Total Counter
	GetDlgItem(IDC_EDIT_FB_MONITOR_EFO_BSD_BY_1739U_TOTAL_CNT)->ShowWindow(bEnableFlag);
	UpdateIntToEdit(IDC_EDIT_FB_MONITOR_EFO_BSD_BY_1739U_TOTAL_CNT, (int)uiTotalCountTriggerEFO_BSD);

}

void MtnDialog_FbMonitor::UI_Enable_Group_EFO207_Fb_CheckBy_1739U_Level(BOOL bEnableFlag)
{
	GetDlgItem(IDC_GROUP_LABEL_EFO_FB_STATUS)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_EFO_COUNT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_EFO_SUCCESS)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_EFO_FAIL)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_COMBO_EFO_MONITOR_USE_1739U_CARD)->ShowWindow(bEnableFlag);
}

// IDC_GROUP_LABEL_ACS_IO
// IDC_CHECK_FB_ACS_OUT0
// IDC_CHECK_ACS_OUT1
// IDC_CHECK_ACS_OUT2
// IDC_CHECK_ACS_OUT3
// IDC_FB_MONITOR_START
// IDC_CHECK_ENA_ACS_IO
void MtnDialog_FbMonitor::OnBnClickedCheckEnaAcsIo()
{
	cFlagEnable_Group_ACS_OUT_MtnDialog_FbMonitor = ((CButton*) GetDlgItem(IDC_CHECK_ENA_ACS_IO))->GetCheck();
	UI_Enable_Group_ACS_OUT_MtnDialog_FbMonitor(cFlagEnable_Group_ACS_OUT_MtnDialog_FbMonitor);
}

#define BE_WB_LED_WH_ON_LOADER     0
#define BE_WB_LED_WH_BOND_TRACK    1
#define BE_WB_LED_WH_OFF_LOADER    2
#define BE_WB_LED_WH_IO_OTHERS     3

char *astrWB_WorkHolderVLedForkModuleNameLabel_en[] =  {
	"OnLoader",
	"BondTrack",
	"OffLoader",
	"Others"};

char *astrWB_WorkHolderVLedForkModuleNameLabel_cn[] =  {
	"上料",
	"焊线轨道",
	"下料",
	"其它"};

// VerLED-Fork DualTrackClamp
extern char *astrWB_WorkHolderVLedForkBondTrackNameLabel_en[];
extern char *astrWB_WorkHolderVLedForkBondTrackNameLabel_cn[];
extern char *astrWB_WorkHolderVLedForkBondTrackCardAddress[];
extern unsigned int nMaxWB_WorkHolderVLedForkIO_BondTrack;

extern char *astrWB_WorkHolderVLedForkOnloaderNameLabel_en[];
extern char *astrWB_WorkHolderVLedForkOnloaderNameLabel_cn[];
extern char *astrWB_WorkHolderVLedForkOnloaderCardAddress[];
extern unsigned int nMaxWB_WorkHolderVLedForkIO_OnLoader;

extern char *astrWB_WorkHolderVLedForkOffLoaderNameLabel_en[];
extern char *astrWB_WorkHolderVLedForkOffLoaderNameLabel_cn[];
extern char *astrWB_WorkHolderVLedForkOffLoaderCardAddress[];
extern unsigned int nMaxWB_WorkHolderVLedForkIO_OffLoader;

extern unsigned int nMaxWB_WorkHolderVLedForkIO_Others;
extern char *astrWB_WorkHolderVLedForkOthersNameLabel_en[];
extern char *astrWB_WorkHolderVLedForkOthersNameLabel_cn[];
extern char *astrWB_WorkHolderVLedForkOthersCardAddress[];

extern char *astrMachineTypeNameLabel_en[];
extern char *astrMachineTypeNameLabel_cn[];


static int iSelWB_WorkHolderVLedForkGuideTextModule = BE_WB_LED_WH_BOND_TRACK;
// IDC_EDIT_DLG_FB_MONITOR_WH_IO_GUIDE
void MtnDialog_FbMonitor::UI_InitWH_IOTextGuide(BOOL bFlagShow)
{
	CComboBox* pComboSelectWorkHolderModule = ((CComboBox*) GetDlgItem(IDC_COMBO_DLG_FB_SELECT_WH_MOD));
	pComboSelectWorkHolderModule->ResetContent();
	if(get_sys_language_option() == LANGUAGE_UI_EN)
	{
		pComboSelectWorkHolderModule->InsertString(BE_WB_LED_WH_ON_LOADER, _T(astrWB_WorkHolderVLedForkModuleNameLabel_en[BE_WB_LED_WH_ON_LOADER]));
		pComboSelectWorkHolderModule->InsertString(BE_WB_LED_WH_BOND_TRACK, _T(astrWB_WorkHolderVLedForkModuleNameLabel_en[BE_WB_LED_WH_BOND_TRACK]));
		pComboSelectWorkHolderModule->InsertString(BE_WB_LED_WH_OFF_LOADER, _T(astrWB_WorkHolderVLedForkModuleNameLabel_en[BE_WB_LED_WH_OFF_LOADER]));
		pComboSelectWorkHolderModule->InsertString(BE_WB_LED_WH_IO_OTHERS, _T(astrWB_WorkHolderVLedForkModuleNameLabel_en[BE_WB_LED_WH_IO_OTHERS]));
	}
	else
	{
		pComboSelectWorkHolderModule->InsertString(BE_WB_LED_WH_ON_LOADER, _T(astrWB_WorkHolderVLedForkModuleNameLabel_cn[BE_WB_LED_WH_ON_LOADER]));
		pComboSelectWorkHolderModule->InsertString(BE_WB_LED_WH_BOND_TRACK, _T(astrWB_WorkHolderVLedForkModuleNameLabel_cn[BE_WB_LED_WH_BOND_TRACK]));
		pComboSelectWorkHolderModule->InsertString(BE_WB_LED_WH_OFF_LOADER, _T(astrWB_WorkHolderVLedForkModuleNameLabel_cn[BE_WB_LED_WH_OFF_LOADER]));
		pComboSelectWorkHolderModule->InsertString(BE_WB_LED_WH_IO_OTHERS, _T(astrWB_WorkHolderVLedForkModuleNameLabel_cn[BE_WB_LED_WH_IO_OTHERS]));
	}
	pComboSelectWorkHolderModule->SetCurSel(iSelWB_WorkHolderVLedForkGuideTextModule);

	CComboBox* pComboSelectMachineType = ((CComboBox*) GetDlgItem(IDC_COMBO_DLG_FB_SELECT_MACH_TYPE));
	pComboSelectMachineType->ResetContent();
	if(get_sys_language_option() == LANGUAGE_UI_EN) // IDC_COMBO_DLG_FB_SELECT_MACH_TYPE
	{
		pComboSelectMachineType->InsertString(WB_MACH_TYPE_VLED_MAGAZINE, _T(astrMachineTypeNameLabel_en[WB_MACH_TYPE_VLED_MAGAZINE]));
		pComboSelectMachineType->InsertString(WB_MACH_TYPE_VLED_FORK, _T(astrMachineTypeNameLabel_en[WB_MACH_TYPE_VLED_FORK]));
		pComboSelectMachineType->InsertString(WB_MACH_TYPE_HORI_LED, _T(astrMachineTypeNameLabel_en[WB_MACH_TYPE_HORI_LED]));
	}
	else
	{
		pComboSelectMachineType->InsertString(WB_MACH_TYPE_VLED_MAGAZINE, _T(astrMachineTypeNameLabel_cn[WB_MACH_TYPE_VLED_MAGAZINE]));
		pComboSelectMachineType->InsertString(WB_MACH_TYPE_VLED_FORK, _T(astrMachineTypeNameLabel_cn[WB_MACH_TYPE_VLED_FORK]));
		pComboSelectMachineType->InsertString(WB_MACH_TYPE_HORI_LED, _T(astrMachineTypeNameLabel_cn[WB_MACH_TYPE_HORI_LED]));
	}
	pComboSelectMachineType->SetCurSel(iFlagMachineType);

	if(bFlagShow == TRUE)
	{
		GetDlgItem(IDC_EDIT_DLG_FB_MONITOR_WH_IO_GUIDE)->SetWindowPos(NULL, 300, 200, 450, 290, SWP_NOMOVE);
		UI_UpdateTextGuideWH_IO(iSelWB_WorkHolderVLedForkGuideTextModule);
	}
	else
	{
		GetDlgItem(IDC_EDIT_DLG_FB_MONITOR_WH_IO_GUIDE)->ShowWindow(FALSE);
	}
	GetDlgItem(IDC_EDIT_DLG_FB_MONITOR_WH_IO_GUIDE)->ShowWindow(bFlagShow);
	GetDlgItem(IDC_COMBO_DLG_FB_SELECT_WH_MOD)->ShowWindow(bFlagShow);
}

// OneTrack VerLED
extern char *astrWB_WorkHolder_13V_OthersCardAddress[];
extern char *astrWB_WorkHolder_13V_OthersNameLabel_cn[];
extern char *astrWB_WorkHolder_13V_OthersNameLabel_en[];
extern unsigned int nMaxWB_WorkHolder_13V_IO_Others;
extern char *astrWB_WorkHolder_13V_BondTrackCardAddress[];
extern char *astrWB_WorkHolder_13V_BondTrackNameLabel_cn[];
extern char *astrWB_WorkHolder_13V_BondTrackNameLabel_en[];
extern unsigned int nMaxWB_WorkHolder_13V_IO_BondTrack;

// One-Cup
CString MtnDialog_FbMonitor::GuideWH_IO_GetText_OneTrackVLED(int iModuleId)
{
	CString cstrWbLedWB_WorkHolderOneTrackVLedForkConnectGuide;
	unsigned int ii;

	switch(iModuleId)
	{
	case BE_WB_LED_WH_IO_OTHERS:
		if(get_sys_language_option() == LANGUAGE_UI_EN)
		{
			cstrWbLedWB_WorkHolderOneTrackVLedForkConnectGuide.Format("%s, %s\r\n", 
				astrWB_WorkHolder_13V_OthersNameLabel_en[0], astrWB_WorkHolder_13V_OthersCardAddress[0]);
			for(ii = 1; ii<nMaxWB_WorkHolder_13V_IO_Others; ii++)
			{
				cstrWbLedWB_WorkHolderOneTrackVLedForkConnectGuide.AppendFormat("%s, %s\r\n", 
					astrWB_WorkHolder_13V_OthersNameLabel_en[ii], astrWB_WorkHolder_13V_OthersCardAddress[ii]);
			}

		}
		else
		{
			cstrWbLedWB_WorkHolderOneTrackVLedForkConnectGuide.Format("%s, %s\r\n", 
				astrWB_WorkHolder_13V_OthersNameLabel_cn[0], astrWB_WorkHolder_13V_OthersCardAddress[0]);
			for(ii = 1; ii<nMaxWB_WorkHolder_13V_IO_Others; ii++)
			{
				cstrWbLedWB_WorkHolderOneTrackVLedForkConnectGuide.AppendFormat("%s, %s\r\n", 
					astrWB_WorkHolder_13V_OthersNameLabel_cn[ii], astrWB_WorkHolder_13V_OthersCardAddress[ii]);
			}
		}
		break;
	case BE_WB_LED_WH_ON_LOADER:
	case BE_WB_LED_WH_BOND_TRACK:
	case BE_WB_LED_WH_OFF_LOADER:
		if(get_sys_language_option() == LANGUAGE_UI_EN)
		{
			cstrWbLedWB_WorkHolderOneTrackVLedForkConnectGuide.Format("%s, %s\r\n", 
				astrWB_WorkHolder_13V_BondTrackNameLabel_en[0], astrWB_WorkHolder_13V_BondTrackCardAddress[0]);
			for(ii = 1; ii<nMaxWB_WorkHolder_13V_IO_BondTrack; ii++)
			{
				cstrWbLedWB_WorkHolderOneTrackVLedForkConnectGuide.AppendFormat("%s, %s\r\n", 
					astrWB_WorkHolder_13V_BondTrackNameLabel_en[ii], astrWB_WorkHolder_13V_BondTrackCardAddress[ii]);
			}

		}
		else
		{
			cstrWbLedWB_WorkHolderOneTrackVLedForkConnectGuide.Format("%s, %s\r\n", 
				astrWB_WorkHolder_13V_BondTrackNameLabel_cn[0], astrWB_WorkHolder_13V_BondTrackCardAddress[0]);
			for(ii = 1; ii<nMaxWB_WorkHolder_13V_IO_BondTrack; ii++)
			{
				cstrWbLedWB_WorkHolderOneTrackVLedForkConnectGuide.AppendFormat("%s, %s\r\n", 
					astrWB_WorkHolder_13V_BondTrackNameLabel_cn[ii], astrWB_WorkHolder_13V_BondTrackCardAddress[ii]);
			}
		}
		break;
	}

	return cstrWbLedWB_WorkHolderOneTrackVLedForkConnectGuide;
}

CString MtnDialog_FbMonitor::GuideWH_IO_GetText_VLED(int iModuleId)
{
	CString cstrWbLedWB_WorkHolderVLedForkConnectGuide;
	unsigned int ii;

	switch(iModuleId)
	{
	case BE_WB_LED_WH_ON_LOADER:
		if(get_sys_language_option() == LANGUAGE_UI_EN)
		{
			cstrWbLedWB_WorkHolderVLedForkConnectGuide.Format("%s, %s\r\n", astrWB_WorkHolderVLedForkOnloaderNameLabel_en[0], astrWB_WorkHolderVLedForkOnloaderCardAddress[0]);
			for(ii = 1; ii<nMaxWB_WorkHolderVLedForkIO_OnLoader; ii++)
			{
				cstrWbLedWB_WorkHolderVLedForkConnectGuide.AppendFormat("%s, %s\r\n", astrWB_WorkHolderVLedForkOnloaderNameLabel_en[ii], astrWB_WorkHolderVLedForkOnloaderCardAddress[ii]);
			}

		}
		else
		{
			cstrWbLedWB_WorkHolderVLedForkConnectGuide.Format("%s, %s\r\n", astrWB_WorkHolderVLedForkOnloaderNameLabel_cn[0], astrWB_WorkHolderVLedForkOnloaderCardAddress[0]);
			for(ii = 1; ii<nMaxWB_WorkHolderVLedForkIO_OnLoader; ii++)
			{
				cstrWbLedWB_WorkHolderVLedForkConnectGuide.AppendFormat("%s, %s\r\n", astrWB_WorkHolderVLedForkOnloaderNameLabel_cn[ii], astrWB_WorkHolderVLedForkOnloaderCardAddress[ii]);
			}
		}
		break;
	case BE_WB_LED_WH_BOND_TRACK:
		if(get_sys_language_option() == LANGUAGE_UI_EN)
		{
			cstrWbLedWB_WorkHolderVLedForkConnectGuide.Format("%s, %s\r\n", astrWB_WorkHolderVLedForkBondTrackNameLabel_en[0], astrWB_WorkHolderVLedForkBondTrackCardAddress[0]);
			for(ii = 1; ii<nMaxWB_WorkHolderVLedForkIO_BondTrack; ii++)
			{
				cstrWbLedWB_WorkHolderVLedForkConnectGuide.AppendFormat("%s, %s\r\n", astrWB_WorkHolderVLedForkBondTrackNameLabel_en[ii], astrWB_WorkHolderVLedForkBondTrackCardAddress[ii]);
			}

		}
		else
		{
			cstrWbLedWB_WorkHolderVLedForkConnectGuide.Format("%s, %s\r\n", astrWB_WorkHolderVLedForkBondTrackNameLabel_cn[0], astrWB_WorkHolderVLedForkBondTrackCardAddress[0]);
			for(ii = 1; ii<nMaxWB_WorkHolderVLedForkIO_BondTrack; ii++)
			{
				cstrWbLedWB_WorkHolderVLedForkConnectGuide.AppendFormat("%s, %s\r\n", astrWB_WorkHolderVLedForkBondTrackNameLabel_cn[ii], astrWB_WorkHolderVLedForkBondTrackCardAddress[ii]);
			}
		}
		break;
	case BE_WB_LED_WH_OFF_LOADER:
		if(get_sys_language_option() == LANGUAGE_UI_EN)
		{
			cstrWbLedWB_WorkHolderVLedForkConnectGuide.Format("%s, %s\r\n", astrWB_WorkHolderVLedForkOffLoaderNameLabel_en[0], astrWB_WorkHolderVLedForkOffLoaderCardAddress[0]);
			for(ii = 1; ii<nMaxWB_WorkHolderVLedForkIO_OffLoader; ii++)
			{
				cstrWbLedWB_WorkHolderVLedForkConnectGuide.AppendFormat("%s, %s\r\n", astrWB_WorkHolderVLedForkOffLoaderNameLabel_en[ii], astrWB_WorkHolderVLedForkOffLoaderCardAddress[ii]);
			}

		}
		else
		{
			cstrWbLedWB_WorkHolderVLedForkConnectGuide.Format("%s, %s\r\n", astrWB_WorkHolderVLedForkOffLoaderNameLabel_cn[0], astrWB_WorkHolderVLedForkOffLoaderCardAddress[0]);
			for(ii = 1; ii<nMaxWB_WorkHolderVLedForkIO_OffLoader; ii++)
			{
				cstrWbLedWB_WorkHolderVLedForkConnectGuide.AppendFormat("%s, %s\r\n", astrWB_WorkHolderVLedForkOffLoaderNameLabel_cn[ii], astrWB_WorkHolderVLedForkOffLoaderCardAddress[ii]);
			}
		}
		break;
	case BE_WB_LED_WH_IO_OTHERS:
		if(get_sys_language_option() == LANGUAGE_UI_EN)
		{
			cstrWbLedWB_WorkHolderVLedForkConnectGuide.Format("%s, %s\r\n", astrWB_WorkHolderVLedForkOthersNameLabel_en[0], astrWB_WorkHolderVLedForkOthersCardAddress[0]);
			for(ii = 1; ii<nMaxWB_WorkHolderVLedForkIO_Others; ii++)
			{
				cstrWbLedWB_WorkHolderVLedForkConnectGuide.AppendFormat("%s, %s\r\n", astrWB_WorkHolderVLedForkOthersNameLabel_en[ii], astrWB_WorkHolderVLedForkOthersCardAddress[ii]);
			}

		}
		else
		{
			cstrWbLedWB_WorkHolderVLedForkConnectGuide.Format("%s, %s\r\n", astrWB_WorkHolderVLedForkOthersNameLabel_cn[0], astrWB_WorkHolderVLedForkOthersCardAddress[0]);
			for(ii = 1; ii<nMaxWB_WorkHolderVLedForkIO_Others; ii++)
			{
				cstrWbLedWB_WorkHolderVLedForkConnectGuide.AppendFormat("%s, %s\r\n", astrWB_WorkHolderVLedForkOthersNameLabel_cn[ii], astrWB_WorkHolderVLedForkOthersCardAddress[ii]);
			}
		}
		break;
	default:
		break;
	}

	return cstrWbLedWB_WorkHolderVLedForkConnectGuide;
}

extern unsigned int nMaxBE_WB13T_WorkHolder_IO_BondTrack;
extern char *astrBE_WB13T_WorkHolder_BondTrackNameLabel_en[];
extern char *astrBE_WB13T_WorkHolder_BondTrackNameLabel_cn[];
extern unsigned int nMaxBE_WB13T_WorkHolder_IO_OnLoader;
extern char *astrBE_WB13T_WorkHolder_OnloaderNameLabel_en[];
extern char *astrBE_WB13T_WorkHolder_OnloaderNameLabel_cn[];
extern unsigned int nMaxBE_WB13T_WorkHolder_IO_OffLoader;
extern char *astrBE_WB13T_WorkHolder_OffloaderNameLabel_en[];
extern char *astrBE_WB13T_WorkHolder_OffloaderNameLabel_cn[];
extern unsigned int nMaxBE_WB13T_WorkHolder_IO_Others;
extern char *astrBE_WB13T_WorkHolder_OthersNameLabel_en[];
extern char *astrBE_WB13T_WorkHolder_OthersNameLabel_cn[];

extern char *astrBE_WB13T_WorkHolder_OnloaderCardAddress[];
extern char *astrBE_WB13T_WorkHolder_BondTrackCardAddress[];
extern char *astrBE_WB13T_WorkHolder_OffLoaderCardAddress[];
extern char *astrBE_WB13T_WorkHolder_OthersCardAddress[];

CString MtnDialog_FbMonitor::GuideWH_IO_GetText_HoriLed(int iModuleId)
{
	CString cstrWbLedBE_WB13T_WorkHolder_ConnectGuide;
	unsigned int ii;

	switch(iModuleId)
	{
	case BE_WB_LED_WH_ON_LOADER:
		if(get_sys_language_option() == LANGUAGE_UI_EN)
		{
			cstrWbLedBE_WB13T_WorkHolder_ConnectGuide.Format("%s, %s\r\n", astrBE_WB13T_WorkHolder_OnloaderNameLabel_en[0], astrBE_WB13T_WorkHolder_OnloaderCardAddress[0]);
			for(ii = 1; ii<nMaxBE_WB13T_WorkHolder_IO_OnLoader; ii++)
			{
				cstrWbLedBE_WB13T_WorkHolder_ConnectGuide.AppendFormat("%s, %s\r\n", astrBE_WB13T_WorkHolder_OnloaderNameLabel_en[ii], astrBE_WB13T_WorkHolder_OnloaderCardAddress[ii]);
			}

		}
		else
		{
			cstrWbLedBE_WB13T_WorkHolder_ConnectGuide.Format("%s, %s\r\n", astrBE_WB13T_WorkHolder_OnloaderNameLabel_cn[0], astrBE_WB13T_WorkHolder_OnloaderCardAddress[0]);
			for(ii = 1; ii<nMaxBE_WB13T_WorkHolder_IO_OnLoader; ii++)
			{
				cstrWbLedBE_WB13T_WorkHolder_ConnectGuide.AppendFormat("%s, %s\r\n", astrBE_WB13T_WorkHolder_OnloaderNameLabel_cn[ii], astrBE_WB13T_WorkHolder_OnloaderCardAddress[ii]);
			}
		}
		break;
	case BE_WB_LED_WH_BOND_TRACK:
		if(get_sys_language_option() == LANGUAGE_UI_EN)
		{
			cstrWbLedBE_WB13T_WorkHolder_ConnectGuide.Format("%s, %s\r\n", astrBE_WB13T_WorkHolder_BondTrackNameLabel_en[0], astrBE_WB13T_WorkHolder_BondTrackCardAddress[0]);
			for(ii = 1; ii<nMaxBE_WB13T_WorkHolder_IO_BondTrack; ii++)
			{
				cstrWbLedBE_WB13T_WorkHolder_ConnectGuide.AppendFormat("%s, %s\r\n", astrBE_WB13T_WorkHolder_BondTrackNameLabel_en[ii], astrBE_WB13T_WorkHolder_BondTrackCardAddress[ii]);
			}

		}
		else
		{
			cstrWbLedBE_WB13T_WorkHolder_ConnectGuide.Format("%s, %s\r\n", astrBE_WB13T_WorkHolder_BondTrackNameLabel_cn[0], astrBE_WB13T_WorkHolder_BondTrackCardAddress[0]);
			for(ii = 1; ii<nMaxBE_WB13T_WorkHolder_IO_BondTrack; ii++)
			{
				cstrWbLedBE_WB13T_WorkHolder_ConnectGuide.AppendFormat("%s, %s\r\n", astrBE_WB13T_WorkHolder_BondTrackNameLabel_cn[ii], astrBE_WB13T_WorkHolder_BondTrackCardAddress[ii]);
			}
		}
		break;
	case BE_WB_LED_WH_OFF_LOADER:
		if(get_sys_language_option() == LANGUAGE_UI_EN)
		{
			cstrWbLedBE_WB13T_WorkHolder_ConnectGuide.Format("%s, %s\r\n", astrBE_WB13T_WorkHolder_OffloaderNameLabel_en[0], astrBE_WB13T_WorkHolder_OffLoaderCardAddress[0]);
			for(ii = 1; ii<nMaxBE_WB13T_WorkHolder_IO_OffLoader; ii++)
			{
				cstrWbLedBE_WB13T_WorkHolder_ConnectGuide.AppendFormat("%s, %s\r\n", astrBE_WB13T_WorkHolder_OffloaderNameLabel_en[ii], astrBE_WB13T_WorkHolder_OffLoaderCardAddress[ii]);
			}

		}
		else
		{
			cstrWbLedBE_WB13T_WorkHolder_ConnectGuide.Format("%s, %s\r\n", astrBE_WB13T_WorkHolder_OffloaderNameLabel_cn[0], astrBE_WB13T_WorkHolder_OffLoaderCardAddress[0]);
			for(ii = 1; ii<nMaxBE_WB13T_WorkHolder_IO_OffLoader; ii++)
			{
				cstrWbLedBE_WB13T_WorkHolder_ConnectGuide.AppendFormat("%s, %s\r\n", astrBE_WB13T_WorkHolder_OffloaderNameLabel_cn[ii], astrBE_WB13T_WorkHolder_OffLoaderCardAddress[ii]);
			}
		}
		break;
	case BE_WB_LED_WH_IO_OTHERS:
		if(get_sys_language_option() == LANGUAGE_UI_EN)
		{
			cstrWbLedBE_WB13T_WorkHolder_ConnectGuide.Format("%s, %s\r\n", astrBE_WB13T_WorkHolder_OthersNameLabel_en[0], astrBE_WB13T_WorkHolder_OthersCardAddress[0]);
			for(ii = 1; ii<nMaxBE_WB13T_WorkHolder_IO_Others; ii++)
			{
				cstrWbLedBE_WB13T_WorkHolder_ConnectGuide.AppendFormat("%s, %s\r\n", astrBE_WB13T_WorkHolder_OthersNameLabel_en[ii], astrBE_WB13T_WorkHolder_OthersCardAddress[ii]);
			}

		}
		else
		{
			cstrWbLedBE_WB13T_WorkHolder_ConnectGuide.Format("%s, %s\r\n", astrBE_WB13T_WorkHolder_OthersNameLabel_cn[0], astrBE_WB13T_WorkHolder_OthersCardAddress[0]);
			for(ii = 1; ii<nMaxBE_WB13T_WorkHolder_IO_Others; ii++)
			{
				cstrWbLedBE_WB13T_WorkHolder_ConnectGuide.AppendFormat("%s, %s\r\n", astrBE_WB13T_WorkHolder_OthersNameLabel_cn[ii], astrBE_WB13T_WorkHolder_OthersCardAddress[ii]);
			}
		}
		break;
	default:
		break;
	}

	return cstrWbLedBE_WB13T_WorkHolder_ConnectGuide;
}

void MtnDialog_FbMonitor::UI_UpdateTextGuideWH_IO(int iModuleId)
{
CString cstrWbLedWB_WorkHolderConnectGuide;

	switch(iFlagMachineType)
	{
	case WB_MACH_TYPE_VLED_FORK:
		cstrWbLedWB_WorkHolderConnectGuide = GuideWH_IO_GetText_VLED(iModuleId);
		break;
	case WB_MACH_TYPE_HORI_LED:
	case BE_WB_HORI_20T_LED:  // 20120826
		cstrWbLedWB_WorkHolderConnectGuide = GuideWH_IO_GetText_HoriLed(iModuleId);
		break;
	case WB_MACH_TYPE_VLED_MAGAZINE:
		cstrWbLedWB_WorkHolderConnectGuide.Format(_T(""));
		break;
	case WB_MACH_TYPE_ONE_TRACK_13V_LED:
	case BE_WB_ONE_TRACK_18V_LED:  // 20120731
		cstrWbLedWB_WorkHolderConnectGuide = GuideWH_IO_GetText_OneTrackVLED(iModuleId);
		break;
	}
	GetDlgItem(IDC_EDIT_DLG_FB_MONITOR_WH_IO_GUIDE)->SetWindowTextA(cstrWbLedWB_WorkHolderConnectGuide);
}

// IDC_COMBO_DLG_FB_SELECT_WH_MOD
void MtnDialog_FbMonitor::OnCbnSelchangeComboDlgFbSelectWhMod()
{
	int iPrevSel = iSelWB_WorkHolderVLedForkGuideTextModule;
	iSelWB_WorkHolderVLedForkGuideTextModule = ((CComboBox*) GetDlgItem(IDC_COMBO_DLG_FB_SELECT_WH_MOD))->GetCurSel();
	if(iPrevSel != iSelWB_WorkHolderVLedForkGuideTextModule)
	{
		UI_UpdateTextGuideWH_IO(iSelWB_WorkHolderVLedForkGuideTextModule);
	}
}


// IDC_COMBO_DLG_FB_SELECT_MACH_TYPE
void MtnDialog_FbMonitor::OnCbnSelchangeComboDlgFbSelectMachType()
{
	int iPrevSel = iFlagMachineType;
	iFlagMachineType = ((CComboBox*)GetDlgItem(IDC_COMBO_DLG_FB_SELECT_MACH_TYPE))->GetCurSel();
	if(iPrevSel != iFlagMachineType)
	{
		UI_UpdateTextGuideWH_IO(iSelWB_WorkHolderVLedForkGuideTextModule);

		if(iFlagMachineType == WB_MACH_TYPE_VLED_FORK)
		{
			mtest_fb_init_search_limit_home_parameter_vled_bonder();
			mtn_init_def_para_search_index_vled_bonder_xyz(_EFSIKA_TABLE_X_AXIS, _EFSIKA_TABLE_Y_AXIS, _EFSIKA_BOND_Z_AXIS);
		}
		else
		{
			mtest_fb_init_search_limit_home_parameter_hori_bonder();
			mtn_dll_init_def_para_search_index_hori_bonder_xyz(_EFSIKA_TABLE_X_AXIS, _EFSIKA_TABLE_Y_AXIS, _EFSIKA_BOND_Z_AXIS);
		}
		UpdateSearchHomeEdit();
	}
}
// IDC_STATIC_FB_DLG_TRIG_EFO_RATIO_PERIOD
// IDC_SLIDER_FB_DLG_TRIG_EFO_RATIO_PERIOD
void MtnDialog_FbMonitor::OnNMReleasedcaptureSliderFbDlgTrigEfoRatioPeriod(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int iSliderPosn = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_FB_DLG_TRIG_EFO_RATIO_PERIOD))->GetPos();
	uiTimerFreqFactor_10KHz = (unsigned int)(iSliderPosn * 10.0 /100.0 * INTER_TRIG_FB_DELAY_MS);
	if(uiTimerFreqFactor_10KHz < 5)
	{
		uiTimerFreqFactor_10KHz = 5;
		iSliderPosn = (int)(uiTimerFreqFactor_10KHz * 100.0 /10 /INTER_TRIG_FB_DELAY_MS );
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER_FB_DLG_TRIG_EFO_RATIO_PERIOD))->SetPos(iSliderPosn);
	}
	else if(iSliderPosn > 80)
	{
		iSliderPosn = 80;
		uiTimerFreqFactor_10KHz = (unsigned int)(iSliderPosn * 10.0 /100.0 * INTER_TRIG_FB_DELAY_MS);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER_FB_DLG_TRIG_EFO_RATIO_PERIOD))->SetPos(iSliderPosn);
	}
	CString cstrTemp;
	cstrTemp.Format("%d %%", iSliderPosn);
	GetDlgItem(IDC_STATIC_FB_DLG_TRIG_EFO_RATIO_PERIOD)->SetWindowTextA(cstrTemp);

	*pResult = 0;
}

void MtnDialog_FbMonitor::ReadUnsignedIntegerFromEdit(int nResId, unsigned int *uiValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", uiValue);
}

void MtnDialog_FbMonitor::UpdateIntToEdit(int nResId, int iValue)
{
	static char tempChar[32];
	sprintf_s(tempChar, 32, "%d", iValue);
	GetDlgItem(nResId)->SetWindowTextA(_T(tempChar));
}

// IDC_EDIT_FB_MONITOR_EFO_BSD_BY_1739U_TOTAL_CNT
void MtnDialog_FbMonitor::OnEnKillfocusEditFbMonitorEfoBsdBy1739uTotalCnt()
{
//	GetDlgItem(IDC_EDIT_FB_MONITOR_EFO_BSD_BY_1739U_TOTAL_CNT)->GetWindowTextA( &strTextTemp[0], 128);
	ReadUnsignedIntegerFromEdit(IDC_EDIT_FB_MONITOR_EFO_BSD_BY_1739U_TOTAL_CNT, &uiTotalCountTriggerEFO_BSD);
}

#define DIGIT_IO_1739_USG_CHANNEL_1    0
#define DIGIT_IO_1739_USG_CHANNEL_2    1
// IDC_COMBO_SELECT_OUTPUT_USG_STATION
void MtnDialog_FbMonitor::OnCbnSelchangeComboSelectOutputUsgStation()
{
	CComboBox* pComboSelectUSG_Channel = ((CComboBox*) GetDlgItem(IDC_COMBO_SELECT_OUTPUT_USG_STATION));
	int iSelUSG_Channel = pComboSelectUSG_Channel->GetCurSel();
	// 
	 
	if(iSelUSG_Channel == DIGIT_IO_1739_USG_CHANNEL_1)
	{
		usDigitalIO_PortOutputBytePort = 0;
	}
	else if(iSelUSG_Channel == DIGIT_IO_1739_USG_CHANNEL_2)
	{
		usDigitalIO_PortOutputBytePort = 3;
	}
	CComboBox *pDigitalIO1739UComboOutput = (CComboBox*) GetDlgItem(IDC_COMBO_DIGITAL_IO_SELECT_OUTPUT_BYTE);
	pDigitalIO1739UComboOutput->SetCurSel(usDigitalIO_PortOutputBytePort);
}

// 
void MtnDialog_FbMonitor::InitUI_ComboSelectUSG_Channel()
{
	// IDC_COMBO_SELECT_OUTPUT_USG_STATION
	CComboBox* pComboSelectUSG_Channel = ((CComboBox*) GetDlgItem(IDC_COMBO_SELECT_OUTPUT_USG_STATION));
	pComboSelectUSG_Channel->ResetContent();	

	int iFlagMachType = get_sys_machine_type_flag();

	if(iFlagMachType == WB_STATION_USG_LIGHTING_PR)  // machine type dependency Item-14
	{
		pComboSelectUSG_Channel->EnableWindow(TRUE);
		pComboSelectUSG_Channel->InsertString(DIGIT_IO_1739_USG_CHANNEL_1, _T("USG_Chn-1"));
		pComboSelectUSG_Channel->InsertString(DIGIT_IO_1739_USG_CHANNEL_2, _T("USG_Chn-2"));

		//hide other UI parts, NOT related with USG-Lighting Station
		GetDlgItem(IDC_COMBO_DLG_FB_SELECT_MACH_TYPE)->ShowWindow(FALSE);
		GetDlgItem(IDC_COMBO_DLG_FB_SELECT_WH_MOD)->ShowWindow(FALSE);
		
	}
	else
	{
		pComboSelectUSG_Channel->EnableWindow(FALSE);
		pComboSelectUSG_Channel->InsertString(DIGIT_IO_1739_USG_CHANNEL_1, _T("USG_Chn-1"));
	}
	pComboSelectUSG_Channel->SetCurSel(DIGIT_IO_1739_USG_CHANNEL_1);
}

// IDC_CHECK_FB_MONITOR_DLG_TEST_DUAL_FAB , 20121018
void MtnDialog_FbMonitor::OnBnClickedCheckFbMonitorDlgTestDualFab()
{
	cFlagEnableTestDualFreeAirBall = ((CButton*)GetDlgItem(IDC_CHECK_FB_MONITOR_DLG_TEST_DUAL_FAB))->GetCheck();
}
