//The MIT License (MIT)
//
//Copyright (c) 2016 ZHAOZhengyi-tgx
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
// (c)All right reserved, sg.LongRenE@gmail.com

// 20101021  Make Difference in Horizontal Bonder
#include "stdafx.h"

#include "MtnApi.h"
#include "MtnSearchHome.h"
#include "MtnApiSearchHome.h"
#include "MtnInitAcs.h"
#include "MotAlgo_DLL.h"

// Following functions belongs to application layer
extern char sys_acs_communication_get_flag_sc_udi();

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

static double dBakupRightLimitPosn[8];
static double dBakupLeftLimitPosn[8];

static double dPositionTableX_StartBonding_HomeZ;
static double dPositionTableY_StartBonding_HomeZ;
static double dPositionBondHead_FireLevel;

// 20110711
static double dMechanicalUppLimitX;
static double dMechanicalUppLimitY;
static double dMechanicalUppLimitZ;
static double dMechanicalRelaxPositionZ;  // 20110806

double mtn_wb_get_bh_upper_limit_position()
{
	return dMechanicalUppLimitZ;
}

double mtn_wb_get_bh_relax_position()
{
	return dMechanicalRelaxPositionZ;
}

double mtn_wb_init_bh_relax_position_from_sp(HANDLE Handle)
{
	MTUNE_OUT_POSN_COMPENSATION stOutputPosnCompensationTune;
	mtn_api_get_spring_compensation_sp_para(Handle, &stOutputPosnCompensationTune);
	dMechanicalRelaxPositionZ = (double)stOutputPosnCompensationTune.iEncoderOffsetSP;

	return dMechanicalRelaxPositionZ;
}

//// Machine type : MechaTronics Configuration
char *pstrMechaTronicsConfig[] =
{
	"WB_VLED_MAGAZINE",
	"WB_VLED_FORK_",
	"WB_HORI_LED_",
	"WB_ONET_TRACK_V_LED",
};

#include "MtnTesterResDef.h"
static int iFlagSysMachineType = WB_MACH_TYPE_ONE_TRACK_13V_LED; // 20110514, WB_MACH_TYPE_VLED_FORK;
int get_sys_machine_type_flag()
{
	return iFlagSysMachineType;
}
void mtn_wb_dll_set_sys_machine_type(int iMachCfg)
{
	iFlagSysMachineType = iMachCfg;
}

static unsigned int uiSerialNumberMachine = 0;
unsigned int get_sys_machine_serial_num()
{
	return uiSerialNumberMachine;
}
#include <math.h>
int set_sys_machine_serial_num(unsigned int uiMachineSn)
{
	int iRet = MTN_API_OK_ZERO;
	if((double)uiMachineSn < pow(2.0, 30))
	{
		uiSerialNumberMachine = uiMachineSn;
	}
	else
	{
		iRet = MTN_API_ERROR;
	}
	return iRet;
}

#include <stdio.h>

// MtnTesterEntry dialog
#define _DEF_FILE_PATH_NAME_MACHINE_CFG   "C:\\WbData\\McConfig.dat"
int mtn_api_load_machine_config(int *piMachCfg)
{
	int iRet = MTN_API_OK_ZERO;

	*piMachCfg = WB_MACH_TYPE_ONE_TRACK_13V_LED;  // by default
	FILE *fptrMachCfg;
	fopen_s(&fptrMachCfg, _DEF_FILE_PATH_NAME_MACHINE_CFG, "r");
	if(fptrMachCfg != NULL)
	{
		int iTempCfg;
		fscanf(fptrMachCfg, "%d", &iTempCfg);
		switch(iTempCfg)
		{
		case 0:
			*piMachCfg = WB_MACH_TYPE_ONE_TRACK_13V_LED;// 3, By default, WB_MACH_TYPE_VLED_FORK;  
			break;
		case 1:
			*piMachCfg = WB_MACH_TYPE_VLED_FORK; // WB_MACH_TYPE_VLED_MAGAZINE;  // 0;
			break;
		case 2:
			*piMachCfg = WB_MACH_TYPE_HORI_LED;  // 2;
			break;
		case 3:
			*piMachCfg = BE_WB_ONE_TRACK_18V_LED;  // ;10
			break;
		case 4:
			*piMachCfg = BE_WB_HORI_20T_LED; // 11
			break;
		case 1001:  // Station XY-Vertical
			*piMachCfg = WB_STATION_XY_VERTICAL;
			break;
		case 1101:  // Station XY-Top
			*piMachCfg = WB_STATION_XY_TOP;
			break;
		case 1002:  // BH
			*piMachCfg = WB_STATION_BH;
			break;
		case 1003:  // EFO, BSD, USG
			*piMachCfg = WB_STATION_EFO_BSD;
			break;
		case 1004:  // USG Lighting
			*piMachCfg = WB_STATION_USG_LIGHTING_PR;
			break;

		}
		if(feof(fptrMachCfg) == 0)   // 20111013
		{
			fscanf(fptrMachCfg, "%d", &uiSerialNumberMachine);  // 20111013
			if((double)uiSerialNumberMachine >= pow(2.0, 30))
			{
				uiSerialNumberMachine = 0;
			}
		}
		fclose(fptrMachCfg);
	}
	else
	{
//		*piMachCfg = 2;  // Default is TopBonding, 20110404
		iRet = MTN_API_ERR_FILE_PTR;
	}
	return iRet;
}

// iMechTronicsType = Machine Type
int aft_get_mechatr_axis_from_ctrl_axis(int iSysAnaAxis_CtrlCardAx, int iMechTronicsType)
{
	int iWbAppAxis = WB_AXIS_TABLE_Y;
	if(iMechTronicsType == WB_MACH_TYPE_HORI_LED ||
		iMechTronicsType == WB_STATION_XY_TOP ||
		iMechTronicsType == BE_WB_ONE_TRACK_18V_LED ||  // 20120826
		iMechTronicsType == BE_WB_HORI_20T_LED)  // 20120826
	{
		switch(iSysAnaAxis_CtrlCardAx)
		{
		case ACS_CARD_AXIS_X:
			iWbAppAxis = WB_AXIS_TABLE_Y;
			break;
		case ACS_CARD_AXIS_Y:
			iWbAppAxis = WB_AXIS_TABLE_X;
			break;
		case ACS_CARD_AXIS_A:
			iWbAppAxis = WB_AXIS_BOND_Z;
			break;
		case ACS_CARD_AXIS_B:
			iWbAppAxis = WB_AXIS_WIRE_CLAMP;
			break;
		default :
			iWbAppAxis = WB_AXIS_TABLE_Y;
			break;
		}
	}
	else
	{
		switch(iSysAnaAxis_CtrlCardAx)
		{
		case ACS_CARD_AXIS_X:
			iWbAppAxis = WB_AXIS_TABLE_X;
			break;
		case ACS_CARD_AXIS_Y:
			iWbAppAxis = WB_AXIS_TABLE_Y;
			break;
		case ACS_CARD_AXIS_A:
			iWbAppAxis = WB_AXIS_BOND_Z;
			break;
		case ACS_CARD_AXIS_B:
			iWbAppAxis = WB_AXIS_WIRE_CLAMP;
			break;
		default :
			iWbAppAxis = WB_AXIS_TABLE_Y;
			break;
		}
	}
	return iWbAppAxis;
}

void mtn_dll_music_logo_normal_start()
{
	Beep(261, 1000); Beep(294, 1000); Beep(329, 1000); // Logo Normal Start 1-2-3, 20120217
}
void mtn_dll_music_logo_normal_stop()
{
	Beep(329, 1000), Beep(294, 1000); Beep(261, 1000);  // Logo Normal Exit 3-2-1, 20120217
}

void mtn_dll_init_home_set_x_table_offset_startbond(double dOffsetTableX)
{
	dPositionTableX_StartBonding_HomeZ = dOffsetTableX;
}

double mtn_dll_init_home_get_x_table_offset_startbond()
{
	return dPositionTableX_StartBonding_HomeZ;
}
// Offset Y
void mtn_dll_init_home_set_y_table_offset_startbond(double dOffsetTableY)
{
	dPositionTableY_StartBonding_HomeZ = dOffsetTableY;
}

double mtn_dll_init_home_get_y_table_offset_startbond()
{
	return dPositionTableY_StartBonding_HomeZ;
}

void mtn_dll_init_home_set_z_bondhead_firelevel(double dBondHeadFireLevel)
{
	dPositionBondHead_FireLevel = dBondHeadFireLevel;
}

double mtn_dll_init_home_get_z_bondhead_firelevel()
{
	return dPositionBondHead_FireLevel;
}
void mtn_dll_init_def_para_search_index_vled_bonder_xyz(int iAxisX, int iAxisY, int iAxisZ)
{
	stSearchIndexInput[iAxisX].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_TBL;
	stSearchIndexInput[iAxisX].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_TBL_X;
	stSearchIndexInput[iAxisX].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_TBL_X;
	stSearchIndexInput[iAxisX].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_TBL_X;
	stSearchIndexInput[iAxisX].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_X;
	stSearchIndexInput[iAxisX].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
	stSearchIndexInput[iAxisX].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_X;
	stSearchIndexInput[iAxisX].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_TBL_X;   // 20110111
	stSearchIndexInput[iAxisX].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_TBL;
	stSearchIndexInput[iAxisX].iAxisOnACS = ACS_CARD_AXIS_X;  // 20110111 iAxisX;
	stSearchIndexInput[iAxisX].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_X;
	stSearchIndexInput[iAxisX].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2_XY_RENISHAW;
	// 20121116, DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
	stSearchIndexInput[iAxisX].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_X;
	stSearchIndexInput[iAxisX].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;

	stSearchIndexInput[iAxisY].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_TBL;
	stSearchIndexInput[iAxisY].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_TBL_Y;
	stSearchIndexInput[iAxisY].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_TBL_Y;
	stSearchIndexInput[iAxisY].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_TBL_Y;
	stSearchIndexInput[iAxisY].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_Y;
	stSearchIndexInput[iAxisY].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
	stSearchIndexInput[iAxisY].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_Y;
	stSearchIndexInput[iAxisY].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_TBL;
	stSearchIndexInput[iAxisY].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_TBL;
	stSearchIndexInput[iAxisY].iAxisOnACS = ACS_CARD_AXIS_Y;  // 20110111, iAxisY;
	stSearchIndexInput[iAxisY].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_Y;
	stSearchIndexInput[iAxisY].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2_XY_RENISHAW;
	// 20121116, DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
	stSearchIndexInput[iAxisY].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_Y;
	stSearchIndexInput[iAxisY].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;

	stSearchIndexInput[iAxisZ].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_Z;
	stSearchIndexInput[iAxisZ].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_Z;
	stSearchIndexInput[iAxisZ].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_Z;
	stSearchIndexInput[iAxisZ].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_Z;
	stSearchIndexInput[iAxisZ].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_Z;
	stSearchIndexInput[iAxisZ].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
	stSearchIndexInput[iAxisZ].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_Z;
	stSearchIndexInput[iAxisZ].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_Z;
	stSearchIndexInput[iAxisZ].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_Z;
	if(sys_acs_communication_get_flag_sc_udi() == TRUE) // 20130228
	{
		stSearchIndexInput[iAxisZ].iAxisOnACS = ACS_CARD_AXIS_Z;   // 20110111, iAxisZ;
	}
	else
	{
		stSearchIndexInput[iAxisZ].iAxisOnACS = ACS_CARD_AXIS_A;   // 20110111, iAxisZ;
	}
	stSearchIndexInput[iAxisZ].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_Z;
	stSearchIndexInput[iAxisZ].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
	stSearchIndexInput[iAxisZ].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_Z;
	stSearchIndexInput[iAxisZ].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;

		stSearchIndexInput[iAxisX].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;
		stSearchIndexInput[iAxisY].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;
		stSearchIndexInput[iAxisZ].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;

	dPositionTableX_StartBonding_HomeZ = BE_WB_TABLE_POSN_X_START_BOND_Z_HOME;
	dPositionTableY_StartBonding_HomeZ = BE_WB_TABLE_POSN_Y_START_BOND_Z_HOME;
	dPositionBondHead_FireLevel = BE_WB_BH_FIRE_LEVEL;
	//if(fabs(dMechanicalUppLimitZ) <= 0.5)
	//{
	//	dMechanicalUppLimitZ = BE_WB_BH_FIRE_LEVEL;  // 20110711
	//}
}

void mtn_dll_init_def_para_search_index_13v_vled_bonder_xyz(int iAxisX, int iAxisY, int iAxisZ)
{

	stSearchIndexInput[iAxisX].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_TBL;
	stSearchIndexInput[iAxisX].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_TBL_X;
	stSearchIndexInput[iAxisX].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_TBL_X;
	stSearchIndexInput[iAxisX].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_TBL_X;
	stSearchIndexInput[iAxisX].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_X;
	stSearchIndexInput[iAxisX].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
	stSearchIndexInput[iAxisX].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_X;
	stSearchIndexInput[iAxisX].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_TBL_X;   // 20110111 20000; // 20110519, 
	stSearchIndexInput[iAxisX].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_TBL;
	stSearchIndexInput[iAxisX].iAxisOnACS = ACS_CARD_AXIS_X;  // 20110111 iAxisX;
	stSearchIndexInput[iAxisX].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_X;
	stSearchIndexInput[iAxisX].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2_XY_RENISHAW;
	// 20121116, DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
	stSearchIndexInput[iAxisX].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_X;
	stSearchIndexInput[iAxisX].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;

	stSearchIndexInput[iAxisY].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_TBL;
	stSearchIndexInput[iAxisY].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_TBL_Y;
	stSearchIndexInput[iAxisY].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_TBL_Y;
	stSearchIndexInput[iAxisY].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_TBL_Y;
	stSearchIndexInput[iAxisY].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_Y;
	stSearchIndexInput[iAxisY].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
	stSearchIndexInput[iAxisY].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_Y;
	stSearchIndexInput[iAxisY].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_TBL;
	stSearchIndexInput[iAxisY].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_TBL;
	stSearchIndexInput[iAxisY].iAxisOnACS = ACS_CARD_AXIS_Y;  // 20110111, iAxisY;
	stSearchIndexInput[iAxisY].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_Y;
	stSearchIndexInput[iAxisY].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2_XY_RENISHAW;
	// 20121116, DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
	stSearchIndexInput[iAxisY].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_Y;
	stSearchIndexInput[iAxisY].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;

	stSearchIndexInput[iAxisZ].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_Z;
	stSearchIndexInput[iAxisZ].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_Z;
	stSearchIndexInput[iAxisZ].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_Z;
	stSearchIndexInput[iAxisZ].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_Z;
	stSearchIndexInput[iAxisZ].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_Z;
	stSearchIndexInput[iAxisZ].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
	stSearchIndexInput[iAxisZ].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_Z;
	stSearchIndexInput[iAxisZ].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_Z;
	stSearchIndexInput[iAxisZ].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_Z;
	if(sys_acs_communication_get_flag_sc_udi() == TRUE) // 20130228
	{
		stSearchIndexInput[iAxisZ].iAxisOnACS = ACS_CARD_AXIS_Z;   // 20110111, iAxisZ;
	}
	else
	{
		stSearchIndexInput[iAxisZ].iAxisOnACS = ACS_CARD_AXIS_A;   // 20110111, iAxisZ;
	}
	stSearchIndexInput[iAxisZ].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_Z;
	stSearchIndexInput[iAxisZ].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
	stSearchIndexInput[iAxisZ].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_Z;
	stSearchIndexInput[iAxisZ].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;

		stSearchIndexInput[iAxisX].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;
		stSearchIndexInput[iAxisY].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;
		stSearchIndexInput[iAxisZ].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;

	dPositionTableX_StartBonding_HomeZ = BE_WB_TABLE_POSN_X_START_1_CUP_BOND_Z_HOME;
	dPositionTableY_StartBonding_HomeZ = BE_WB_TABLE_POSN_Y_START_1_CUP_BOND_Z_HOME;  // BE_WB_TABLE_POSN_Y_START_BOND_Z_HOME; // 20111018, 
	dPositionBondHead_FireLevel = BE_WB_BH_FIRE_LEVEL;

}

void mtn_dll_init_def_para_search_index_hori_bonder_xyz(int iAxisX, int iAxisY, int iAxisZ)
{

	stSearchIndexInput[iAxisX].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_TBL;
	stSearchIndexInput[iAxisX].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_TBL_X;
	stSearchIndexInput[iAxisX].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_TBL_X;
	stSearchIndexInput[iAxisX].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_TBL_X;
	stSearchIndexInput[iAxisX].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_X;
	stSearchIndexInput[iAxisX].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
	stSearchIndexInput[iAxisX].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_X;
	stSearchIndexInput[iAxisX].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_TBL;
	stSearchIndexInput[iAxisX].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_TBL;
	stSearchIndexInput[iAxisX].iAxisOnACS = ACS_CARD_AXIS_Y;  // 20110111, iAxisX;
	stSearchIndexInput[iAxisX].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_X_HORI_BONDER;  // 20101021
	stSearchIndexInput[iAxisX].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2_XY_RENISHAW;
	// 20121116, DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
	stSearchIndexInput[iAxisX].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_X;
	stSearchIndexInput[iAxisX].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;

	stSearchIndexInput[iAxisY].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_TBL;
	stSearchIndexInput[iAxisY].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_TBL_Y;
	stSearchIndexInput[iAxisY].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_TBL_Y;
	stSearchIndexInput[iAxisY].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_TBL_Y;
	stSearchIndexInput[iAxisY].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_Y;
	stSearchIndexInput[iAxisY].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
	stSearchIndexInput[iAxisY].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_Y;
	stSearchIndexInput[iAxisY].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_TBL;
	stSearchIndexInput[iAxisY].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_TBL;
	stSearchIndexInput[iAxisY].iAxisOnACS = ACS_CARD_AXIS_X;  // 20110111
	stSearchIndexInput[iAxisY].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_Y;
	stSearchIndexInput[iAxisY].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2_XY_RENISHAW;
	// 20121116, DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
	stSearchIndexInput[iAxisY].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_Y;
	stSearchIndexInput[iAxisY].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;

	stSearchIndexInput[iAxisZ].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_Z;
	stSearchIndexInput[iAxisZ].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_Z;
	stSearchIndexInput[iAxisZ].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_Z;
	stSearchIndexInput[iAxisZ].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_Z;
	stSearchIndexInput[iAxisZ].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_Z;
	stSearchIndexInput[iAxisZ].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
	stSearchIndexInput[iAxisZ].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_Z;
	stSearchIndexInput[iAxisZ].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_Z;
	stSearchIndexInput[iAxisZ].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_Z;
	if(sys_acs_communication_get_flag_sc_udi() == TRUE) // 20130228
	{
		stSearchIndexInput[iAxisZ].iAxisOnACS = ACS_CARD_AXIS_Z;   // 20110111, iAxisZ;
	}
	else
	{
		stSearchIndexInput[iAxisZ].iAxisOnACS = ACS_CARD_AXIS_A;   // 20110111, iAxisZ;
	}
	stSearchIndexInput[iAxisZ].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_Z;
	stSearchIndexInput[iAxisZ].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
	stSearchIndexInput[iAxisZ].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_Z;
	stSearchIndexInput[iAxisZ].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;

		stSearchIndexInput[iAxisX].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;
		stSearchIndexInput[iAxisY].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;
		stSearchIndexInput[iAxisZ].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;

	dPositionTableX_StartBonding_HomeZ = BE_WB_TABLE_POSN_X_START_HORI_BONDER_Z_HOME;   // 20101021
	dPositionTableY_StartBonding_HomeZ = BE_WB_TABLE_POSN_Y_START_BOND_Z_HOME;
	dPositionBondHead_FireLevel = BE_WB_BH_FIRE_LEVEL_HORI_LED_1;   // 20110625
	//if(fabs(dMechanicalUppLimitZ) <= 0.5)
	//{
	//	dMechanicalUppLimitZ = BE_WB_BH_FIRE_LEVEL_HORI_LED_1;  // 20110711
	//}
}

void mtn_dll_init_def_para_search_index_18v_bonder_xyz(int iAxisX, int iAxisY, int iAxisZ)
{

	stSearchIndexInput[iAxisX].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_TBL;
	stSearchIndexInput[iAxisX].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_TBL_X;
	stSearchIndexInput[iAxisX].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_TBL_X;
	stSearchIndexInput[iAxisX].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_TBL_X;
	stSearchIndexInput[iAxisX].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_X;
	stSearchIndexInput[iAxisX].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
	stSearchIndexInput[iAxisX].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_X;
	stSearchIndexInput[iAxisX].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_TBL;
	stSearchIndexInput[iAxisX].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_TBL;
	stSearchIndexInput[iAxisX].iAxisOnACS = ACS_CARD_AXIS_Y;  // 20110111, iAxisX;
	stSearchIndexInput[iAxisX].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_X_HORI_BONDER;  // 20101021
	stSearchIndexInput[iAxisX].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
	stSearchIndexInput[iAxisX].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_X;
	stSearchIndexInput[iAxisX].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;

	stSearchIndexInput[iAxisY].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_TBL;
	stSearchIndexInput[iAxisY].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_TBL_Y;
	stSearchIndexInput[iAxisY].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_TBL_Y;
	stSearchIndexInput[iAxisY].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_TBL_Y;
	stSearchIndexInput[iAxisY].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_Y;
	stSearchIndexInput[iAxisY].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
	stSearchIndexInput[iAxisY].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_Y;
	stSearchIndexInput[iAxisY].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_TBL;
	stSearchIndexInput[iAxisY].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_TBL;
	stSearchIndexInput[iAxisY].iAxisOnACS = ACS_CARD_AXIS_X;  // 20110111
	stSearchIndexInput[iAxisY].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_Y;
	stSearchIndexInput[iAxisY].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2_XY_RENISHAW;
	// 20121116, DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
	stSearchIndexInput[iAxisY].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_Y;
	stSearchIndexInput[iAxisY].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;

	stSearchIndexInput[iAxisZ].stSpeedProfileMove.dMaxVelocity = DEFAULT_SEARCH_HOME_MOVE_VEL_Z;
	stSearchIndexInput[iAxisZ].stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_Z;
	stSearchIndexInput[iAxisZ].stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_Z;
	stSearchIndexInput[iAxisZ].stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_Z;
	stSearchIndexInput[iAxisZ].dMaxDistanceRangeProtection = DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_Z;
	stSearchIndexInput[iAxisZ].uiFreqFactor_10KHz_Detecting = DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ;
	stSearchIndexInput[iAxisZ].dMoveDistanceBeforeSearchLimit = DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_Z;
	stSearchIndexInput[iAxisZ].dVelJoggingLimit = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_Z;
	stSearchIndexInput[iAxisZ].dPositionErrorThresholdSearchLimit = DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_Z;
	if(sys_acs_communication_get_flag_sc_udi() == TRUE) // 20130228
	{
		stSearchIndexInput[iAxisZ].iAxisOnACS = ACS_CARD_AXIS_Z;   // 20110111, iAxisZ;
	}
	else
	{
		stSearchIndexInput[iAxisZ].iAxisOnACS = ACS_CARD_AXIS_A;   // 20110111, iAxisZ;
	}
	stSearchIndexInput[iAxisZ].dMoveDistanceBeforeSearchIndex1 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_Z;
	stSearchIndexInput[iAxisZ].dMoveDistanceBeforeSearchIndex2 = DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2_XY_RENISHAW;
	// 20121116, DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2;
	stSearchIndexInput[iAxisZ].dVelJoggingIndex1 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_Z;
	stSearchIndexInput[iAxisZ].dVelJoggingIndex2 = DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX;

		stSearchIndexInput[iAxisX].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;
		stSearchIndexInput[iAxisY].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;
		stSearchIndexInput[iAxisZ].dPosnErrThresHoldSettling = DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH;

	dPositionTableX_StartBonding_HomeZ = 0;   // 20101021
	dPositionTableY_StartBonding_HomeZ = 10000;
	dPositionBondHead_FireLevel = BE_WB_BH_FIRE_LEVEL_HORI_LED_1;   // 20110625


}

extern MOTALGO_DLL_API int mtn_dll_api_search_home_acs(HANDLE mHandle, MTN_API_SEARCH_INDEX_INPUT *stpSearchHomeInputAPI, MTN_API_SEARCH_INDEX_OUTPUT *stpSearcihHomeOutputAPI);
extern MOTALGO_DLL_API int mtn_dll_api_search_limit_acs(HANDLE mHandle, MTN_API_SEARCH_LIMIT_INPUT *stpSearchLimitInput, MTN_API_SEARCH_LIMIT_OUTPUT *stpSearchLimitOutput);
extern MOTALGO_DLL_API int mtn_dll_api_search_home_from_limit_acs(HANDLE mHandle, MTN_API_SEARCH_HOME_FROM_LIMIT_INPUT *stpSearchHomeFromLimitInput, MTN_API_SEARCH_HOME_FROM_LIMIT_OUTPUT *stpSearchHomeFromLimitOutput);
extern MOTALGO_DLL_API int mtn_dll_search_home_acs(HANDLE mHandle, MTN_SEARCH_INDEX_INPUT *stpSearchHomeInput, MTN_SEARCH_INDEX_OUTPUT *stpSearchHomeOutput);

void mtn_enlarge_acs_axis_software_limit_(HANDLE Handle, int iAxis)
{
	// Hardcoded [-800mm, 800mm];
//	double dAbsFactor = fabs(dFactor);
	mtn_api_set_position_upp_lmt(Handle, iAxis, 16E8);
	mtn_api_set_position_low_lmt(Handle, iAxis, -16E8);
}

// 
void mtn_wb_bakup_software_limit(HANDLE Handle, int iAxisX, int iAxisY, int iAxisZ)
{
	mtn_api_get_position_upp_lmt(Handle, iAxisX, &dBakupRightLimitPosn[iAxisX]);
	mtn_api_get_position_low_lmt(Handle, iAxisX, &dBakupLeftLimitPosn[iAxisX]);

	mtn_api_get_position_upp_lmt(Handle, iAxisY, &dBakupRightLimitPosn[iAxisY]);
	mtn_api_get_position_low_lmt(Handle, iAxisY, &dBakupLeftLimitPosn[iAxisY]);

	mtn_api_get_position_upp_lmt(Handle, iAxisZ, &dBakupRightLimitPosn[iAxisZ]);
	mtn_api_get_position_low_lmt(Handle, iAxisZ, &dBakupLeftLimitPosn[iAxisZ]);
}

void mtn_wb_restore_software_limit(HANDLE Handle, int iAxisX, int iAxisY, int iAxisZ)
{
	mtn_api_set_position_upp_lmt(Handle, iAxisX, dBakupRightLimitPosn[iAxisX]);
	mtn_api_set_position_low_lmt(Handle, iAxisX, dBakupLeftLimitPosn[iAxisX]);

	mtn_api_set_position_upp_lmt(Handle, iAxisY, dBakupRightLimitPosn[iAxisY]);
	mtn_api_set_position_low_lmt(Handle, iAxisY, dBakupLeftLimitPosn[iAxisY]);

	mtn_api_set_position_upp_lmt(Handle, iAxisZ, dBakupRightLimitPosn[iAxisZ]);
	mtn_api_set_position_low_lmt(Handle, iAxisZ, dBakupLeftLimitPosn[iAxisZ]);
}

void mtn_wb_enlarge_software_limit_by_(HANDLE Handle, int iAxisX, int iAxisY, int iAxisZ, double dFactor)
{
	double dAbsFactor = fabs(dFactor);
	mtn_api_set_position_upp_lmt(Handle, iAxisX, fabs(dAbsFactor * dBakupRightLimitPosn[iAxisX]));
	mtn_api_set_position_low_lmt(Handle, iAxisX, -fabs(dAbsFactor * dBakupLeftLimitPosn[iAxisX]));

	mtn_api_set_position_upp_lmt(Handle, iAxisY, fabs(dAbsFactor * dBakupRightLimitPosn[iAxisY]));
	mtn_api_set_position_low_lmt(Handle, iAxisY, -fabs(dAbsFactor * dBakupLeftLimitPosn[iAxisY]));

	mtn_api_set_position_upp_lmt(Handle, iAxisZ, fabs(dAbsFactor * dBakupRightLimitPosn[iAxisZ]));
	mtn_api_set_position_low_lmt(Handle, iAxisZ, -fabs(dAbsFactor * dBakupLeftLimitPosn[iAxisZ]));
}

void mtn_bakup_acs_axis_software_limit(HANDLE Handle, int iAxis)
{
	mtn_api_get_position_upp_lmt(Handle, iAxis, &dBakupRightLimitPosn[iAxis]);
	mtn_api_get_position_low_lmt(Handle, iAxis, &dBakupLeftLimitPosn[iAxis]);
}

void mtn_restore_acs_axis_software_limit(HANDLE Handle, int iAxis)
{
	mtn_api_set_position_upp_lmt(Handle, iAxis, dBakupRightLimitPosn[iAxis]);
	mtn_api_set_position_low_lmt(Handle, iAxis, dBakupLeftLimitPosn[iAxis]);
}

void mtn_enlarge_acs_axis_software_limit_by_(HANDLE Handle, int iAxis, double dFactor)
{
	double dAbsFactor = fabs(dFactor);
	mtn_api_set_position_upp_lmt(Handle, iAxis, fabs(dAbsFactor * dBakupRightLimitPosn[iAxis]));
	mtn_api_set_position_low_lmt(Handle, iAxis, -fabs(dAbsFactor * dBakupLeftLimitPosn[iAxis]));
}


static SAFETY_PARA_ACS astSafetyParaBak[ACS_CARD_AXIS_D + 1], astSafetyParaCurr[ACS_CARD_AXIS_D + 1];
void mtn_wb_bakup_servo_protect_limit(HANDLE Handle, int iIdAcsAxisX, int iIdAcsAxisY, int iIdAcsAxisZ)
{
	mtnapi_upload_safety_parameter_acs_per_axis(Handle, iIdAcsAxisX, &astSafetyParaBak[0]);
	mtnapi_upload_safety_parameter_acs_per_axis(Handle, iIdAcsAxisY, &astSafetyParaBak[1]);
	mtnapi_upload_safety_parameter_acs_per_axis(Handle, iIdAcsAxisZ, &astSafetyParaBak[2]);
}

void mtn_wb_enlarge_servo_protect_limit(HANDLE Handle, int iIdAcsAxisX, int iIdAcsAxisY, int iIdAcsAxisZ, double dEnlargeFactor)
{
	int ii;
	for(ii = 0; ii< 3; ii++)
	{
		astSafetyParaCurr[ii] = astSafetyParaBak[ii];
		astSafetyParaCurr[ii].dCriticalPosnErrAcc = astSafetyParaCurr[ii].dCriticalPosnErrAcc * dEnlargeFactor ;
		astSafetyParaCurr[ii].dCriticalPosnErrIdle = astSafetyParaCurr[ii].dCriticalPosnErrIdle * dEnlargeFactor;	
		astSafetyParaCurr[ii].dCriticalPosnErrVel = astSafetyParaCurr[ii].dCriticalPosnErrVel * dEnlargeFactor;

		astSafetyParaCurr[ii].dRMS_DrvCmdMtn = astSafetyParaCurr[ii].dRMS_DrvCmdMtn * dEnlargeFactor;
		astSafetyParaCurr[ii].dRMS_TimeConst = astSafetyParaCurr[ii].dRMS_TimeConst * dEnlargeFactor;  // Enlarge time const, 20110901
		// DONOT enlarge following security protection
		astSafetyParaCurr[ii].dRMS_DrvCmdX = astSafetyParaCurr[ii].dRMS_DrvCmdX ;
		astSafetyParaCurr[ii].dRMS_DrvCmdIdle = astSafetyParaCurr[ii].dRMS_DrvCmdIdle ;
	}

	mtnapi_download_safety_parameter_acs_per_axis(Handle, iIdAcsAxisX, &astSafetyParaCurr[0]);
	mtnapi_download_safety_parameter_acs_per_axis(Handle, iIdAcsAxisY, &astSafetyParaCurr[1]);
	mtnapi_download_safety_parameter_acs_per_axis(Handle, iIdAcsAxisZ, &astSafetyParaCurr[2]);

}

void mtn_wb_enlarge_servo_axis_protect_limit(HANDLE Handle, int iIdAcsAxis, double dEnlargeFactor)
{
//	int ii;
		astSafetyParaCurr[iIdAcsAxis] = astSafetyParaBak[iIdAcsAxis];
		astSafetyParaCurr[iIdAcsAxis].dCriticalPosnErrAcc = astSafetyParaCurr[iIdAcsAxis].dCriticalPosnErrAcc * dEnlargeFactor ;
		astSafetyParaCurr[iIdAcsAxis].dCriticalPosnErrIdle = astSafetyParaCurr[iIdAcsAxis].dCriticalPosnErrIdle * dEnlargeFactor;	
		astSafetyParaCurr[iIdAcsAxis].dCriticalPosnErrVel = astSafetyParaCurr[iIdAcsAxis].dCriticalPosnErrVel * dEnlargeFactor;

		astSafetyParaCurr[iIdAcsAxis].dRMS_DrvCmdMtn = astSafetyParaCurr[iIdAcsAxis].dRMS_DrvCmdMtn * dEnlargeFactor;
		astSafetyParaCurr[iIdAcsAxis].dRMS_TimeConst = astSafetyParaCurr[iIdAcsAxis].dRMS_TimeConst * dEnlargeFactor;  // Enlarge time const, 20110901
		// DONOT enlarge following security protection
		astSafetyParaCurr[iIdAcsAxis].dRMS_DrvCmdX = astSafetyParaCurr[iIdAcsAxis].dRMS_DrvCmdX ;
		astSafetyParaCurr[iIdAcsAxis].dRMS_DrvCmdIdle = astSafetyParaCurr[iIdAcsAxis].dRMS_DrvCmdIdle ;
//		astSafetyParaCurr[iIdAcsAxis].dSoftwarePosnLimitLow = astSafetyParaCurr[iIdAcsAxis].dSoftwarePosnLimitLow * dEnlargeFactor; // 20120706
//		astSafetyParaCurr[iIdAcsAxis].dSoftwarePosnLimitUpp = astSafetyParaCurr[iIdAcsAxis].dSoftwarePosnLimitUpp * dEnlargeFactor; // 20120706

	mtnapi_download_safety_parameter_acs_per_axis(Handle, iIdAcsAxis, &astSafetyParaCurr[iIdAcsAxis]);

}

void mtn_wb_restore_servo_protect_limit(HANDLE Handle, int iIdAcsAxisX, int iIdAcsAxisY, int iIdAcsAxisZ)
{
	mtnapi_download_safety_parameter_acs_per_axis(Handle, iIdAcsAxisX, &astSafetyParaBak[0]);
	mtnapi_download_safety_parameter_acs_per_axis(Handle, iIdAcsAxisY, &astSafetyParaBak[1]);
	mtnapi_download_safety_parameter_acs_per_axis(Handle, iIdAcsAxisZ, &astSafetyParaBak[2]);
}

int mtn_dll_axis_search_limit(HANDLE Handle, int iAxis)
{
	stSearchLimitInput[iAxis].dMaxDistanceRangeProtection = stSearchIndexInput[iAxis].dMaxDistanceRangeProtection;
	stSearchLimitInput[iAxis].dMoveDistanceAfterSearchLimit = stSearchIndexInput[iAxis].dMoveDistanceBeforeSearchIndex1/2; // Before search index, is usually search limit
	stSearchLimitInput[iAxis].dMoveDistanceBeforeSearchLimit = stSearchIndexInput[iAxis].dMoveDistanceBeforeSearchLimit;
	stSearchLimitInput[iAxis].dPositionErrorThresholdSearchLimit = stSearchIndexInput[iAxis].dPositionErrorThresholdSearchLimit;
	stSearchLimitInput[iAxis].dPosnErrThresHoldSettling = stSearchIndexInput[iAxis].dPosnErrThresHoldSettling;
	stSearchLimitInput[iAxis].dVelJoggingLimit = stSearchIndexInput[iAxis].dVelJoggingLimit;
	stSearchLimitInput[iAxis].iAxisOnACS = stSearchIndexInput[iAxis].iAxisOnACS;  //// 20110111, iAxis;
	stSearchLimitInput[iAxis].iDebug = stSearchIndexInput[iAxis].iDebug;
	stSearchLimitInput[iAxis].stSpeedProfileMove = stSearchIndexInput[iAxis].stSpeedProfileMove;
	stSearchLimitInput[iAxis].uiFreqFactor_10KHz_Detecting = stSearchIndexInput[iAxis].uiFreqFactor_10KHz_Detecting;

	return mtn_search_limit_acs(Handle, &stSearchLimitInput[iAxis], &stSearchLimitOutput[iAxis]); 
}

int mtn_dll_axis_go_home_from_limit(HANDLE Handle, int iAxis)
{
	stSearchHomeFromLimitInput[iAxis].dMaxDistanceRangeProtection = stSearchIndexInput[iAxis].dMaxDistanceRangeProtection;
	stSearchHomeFromLimitInput[iAxis].dMoveDistanceBeforeSearchIndex1 = stSearchIndexInput[iAxis].dMoveDistanceBeforeSearchIndex1/2;
	stSearchHomeFromLimitInput[iAxis].dMoveDistanceBeforeSearchIndex2 = stSearchIndexInput[iAxis].dMoveDistanceBeforeSearchIndex2;
	stSearchHomeFromLimitInput[iAxis].dPosnErrThresHoldSettling = stSearchIndexInput[iAxis].dPosnErrThresHoldSettling;
	stSearchHomeFromLimitInput[iAxis].dVelJoggingIndex1 = stSearchIndexInput[iAxis].dVelJoggingIndex1;
	stSearchHomeFromLimitInput[iAxis].dVelJoggingIndex2 = stSearchIndexInput[iAxis].dVelJoggingIndex2;
	stSearchHomeFromLimitInput[iAxis].iDebug = stSearchIndexInput[iAxis].iDebug;
	stSearchHomeFromLimitInput[iAxis].stSpeedProfileMove = stSearchIndexInput[iAxis].stSpeedProfileMove;
	stSearchHomeFromLimitInput[iAxis].iAxisOnACS = stSearchIndexInput[iAxis].iAxisOnACS;  // 20110111, iAxis;
	stSearchHomeFromLimitInput[iAxis].uiFreqFactor_10KHz_Detecting = stSearchIndexInput[iAxis].uiFreqFactor_10KHz_Detecting;

	return mtn_search_home_from_limit_acs(Handle, &stSearchHomeFromLimitInput[iAxis], &stSearchHomeFromLimitOutput[iAxis]);
}

int mtn_dll_axis_search_limit_go_home(HANDLE Handle, int iAxis)
{
int iCtrlAxisACS;
	// (1) Lift up Z, search limit
	// Bakup parameter
	iCtrlAxisACS = stSearchIndexInput[iAxis].iAxisOnACS;  // 20110923

	// Backup and enlarge limit protection
 	mtn_bakup_acs_axis_software_limit(Handle, iCtrlAxisACS);
//	mtn_enlarge_acs_axis_software_limit_by_(Handle, iCtrlAxisACS, 4.0);  // 20110829, // 20110923
	mtn_enlarge_acs_axis_software_limit_(Handle, iCtrlAxisACS); // 2012July6

	// Protection limit, 20110902
	mtnapi_upload_safety_parameter_acs_per_axis(Handle, iCtrlAxisACS, &astSafetyParaBak[iCtrlAxisACS]);
	mtn_wb_enlarge_servo_axis_protect_limit(Handle, iCtrlAxisACS, 4.0); // 20110923

	int iRet = mtn_dll_search_home_acs(Handle, &stSearchIndexInput[iAxis], &stSearchIndexOutput[iAxis]);

	// 20110711
	if(iAxis == sys_get_acs_axis_id_bnd_z())
	{
		dMechanicalUppLimitZ = fabs(stSearchIndexOutput[iAxis].dDistanceFromIndex1ToLimit);
		MTUNE_OUT_POSN_COMPENSATION stOutputPosnCompensationTune;
		mtn_api_get_spring_compensation_sp_para(Handle, &stOutputPosnCompensationTune);
		dMechanicalRelaxPositionZ = (double)stOutputPosnCompensationTune.iEncoderOffsetSP;

	}
	// Restore // 20110923
	mtn_restore_acs_axis_software_limit(Handle, iCtrlAxisACS);
	// Protection limit, 20110902
	mtnapi_download_safety_parameter_acs_per_axis(Handle, iCtrlAxisACS, &astSafetyParaBak[iCtrlAxisACS]);

	return iRet;
}

int mtn_wb_table_servo_go_to_safety_position(HANDLE stCommHandle, int iAxisACS_TableX, int iAxisACS_TableY)
{
	int iRet = MTN_API_OK_ZERO;

	int iMotorState;
	// (3.5) Y Table, X Table goto Start Bondering Position,  
	mtnapi_get_motor_state(stCommHandle, 
				iAxisACS_TableY, &iMotorState, 0);
	if(iMotorState & ACSC_MST_ENABLE) 
	{
		if(! acsc_ToPoint(stCommHandle, 0, // start up the motion immediately
				iAxisACS_TableY, dPositionTableY_StartBonding_HomeZ, NULL) )
		{
			iRet = MTN_API_ERROR;
			goto label_mtn_wb_table_servo_go_to_safety_position;
		}
	}
	// 
	mtnapi_get_motor_state(stCommHandle, 
				iAxisACS_TableX, &iMotorState, 0);
	if(iMotorState & ACSC_MST_ENABLE) 
	{
		if(! acsc_ToPoint(stCommHandle, 0, // start up the motion immediately
				iAxisACS_TableX, dPositionTableX_StartBonding_HomeZ, NULL) )
		{
			iRet = MTN_API_ERROR;
			goto label_mtn_wb_table_servo_go_to_safety_position;
		}
	}

	// Until fully stopped and no error
	while(mtn_qc_is_axis_still_moving(stCommHandle, iAxisACS_TableY) == TRUE)
	{
		Sleep(1);
	}
	if(mtn_qc_is_axis_locked_safe(stCommHandle, iAxisACS_TableY) == FALSE)
	{
		iRet = MTN_API_ERROR;
		goto label_mtn_wb_table_servo_go_to_safety_position;
	}
	while(mtn_qc_is_axis_still_moving(stCommHandle, iAxisACS_TableX) == TRUE)
	{
		Sleep(1);
	}
	if(mtn_qc_is_axis_locked_safe(stCommHandle, iAxisACS_TableX) == FALSE)
	{
		iRet = MTN_API_ERROR;
		goto label_mtn_wb_table_servo_go_to_safety_position;
	}

label_mtn_wb_table_servo_go_to_safety_position:
	return iRet;
}
static MTN_SPEED_PROFILE stMtnHomeBakupParaSpeedProf[MAX_CTRL_AXIS_PER_SERVO_BOARD];
void mtn_wb_home_bakup_speed_parameter_acs(HANDLE Handle)
{
	mtnapi_get_speed_profile(Handle, ACSC_AXIS_X, &stMtnHomeBakupParaSpeedProf[ACSC_AXIS_X], 0);
	mtnapi_get_speed_profile(Handle, ACSC_AXIS_Y, &stMtnHomeBakupParaSpeedProf[ACSC_AXIS_Y], 0);
	mtnapi_get_speed_profile(Handle, ACSC_AXIS_A, &stMtnHomeBakupParaSpeedProf[ACSC_AXIS_A], 0);
}

void mtn_wb_home_restore_speed_parameter_acs(HANDLE Handle)
{
	mtnapi_set_speed_profile(Handle, ACSC_AXIS_X, &stMtnHomeBakupParaSpeedProf[ACSC_AXIS_X], 0);
	mtnapi_set_speed_profile(Handle, ACSC_AXIS_Y, &stMtnHomeBakupParaSpeedProf[ACSC_AXIS_Y], 0);
	mtnapi_set_speed_profile(Handle, ACSC_AXIS_A, &stMtnHomeBakupParaSpeedProf[ACSC_AXIS_A], 0);
}

int mtn_dll_bonder_xyz_start_search_limit_go_home(HANDLE Handle, int iAxisX, int iAxisY, int iAxisZ)
{
	int iRet = MTN_API_OK_ZERO;
int iCtrlAxisACS_X, iCtrlAxisACS_Y, iCtrlAxisACS_BondHeadZ;
	// (1) Lift up Z, search limit
	// Bakup parameter
	iCtrlAxisACS_X = stSearchIndexInput[iAxisX].iAxisOnACS;
	iCtrlAxisACS_Y = stSearchIndexInput[iAxisY].iAxisOnACS;
	iCtrlAxisACS_BondHeadZ = stSearchIndexInput[iAxisZ].iAxisOnACS;

CTRL_PARA_ACS  stBakupCtrlParameter_Z;
	iRet = mtnapi_upload_servo_parameter_acs_v60_per_axis(Handle, iCtrlAxisACS_BondHeadZ, &stBakupCtrlParameter_Z); // mtnapi_upload_servo_parameter_acs_per_axis
	if(iRet != MTN_API_OK_ZERO)
	{
		goto label_return_bonder_xyz_start_search_limit_go_home;
	}
//  Backup Software Limit
	mtn_wb_bakup_software_limit(Handle, iCtrlAxisACS_X, iCtrlAxisACS_Y, iCtrlAxisACS_BondHeadZ);
//	mtn_wb_enlarge_software_limit_by_(Handle, iCtrlAxisACS_X, iCtrlAxisACS_Y, iCtrlAxisACS_BondHeadZ, 10.0); // 2012July6
	mtn_enlarge_acs_axis_software_limit_(Handle, iCtrlAxisACS_X);
	mtn_enlarge_acs_axis_software_limit_(Handle, iCtrlAxisACS_Y);
	mtn_enlarge_acs_axis_software_limit_(Handle, iCtrlAxisACS_BondHeadZ);

	mtn_wb_home_bakup_speed_parameter_acs(Handle);
//  Backup Servo Protection limit
	mtn_wb_bakup_servo_protect_limit(Handle, iCtrlAxisACS_X, iCtrlAxisACS_Y, iCtrlAxisACS_BondHeadZ);
//  Enlarge protection limit
	mtn_wb_enlarge_servo_protect_limit(Handle, iCtrlAxisACS_X, iCtrlAxisACS_Y, iCtrlAxisACS_BondHeadZ, 8.0);

	// download special parameter
//	if(mtnapi_download_bonder_z_servo_para_search_home() != MTN_API_OK_ZERO)
//	{
//	}
	static int iFlagMachType;
	if(iFlagMachType != WB_STATION_XY_VERTICAL
			&& iFlagMachType != WB_STATION_XY_TOP
			&& iFlagMachType != WB_STATION_WIRE_CLAMP) // 20110914
	{
		if(mtn_dll_axis_search_limit(Handle, iAxisZ) == MTN_API_ERROR)
		{
			iRet = MTN_API_ERROR;
			goto label_return_bonder_xyz_start_search_limit_go_home;
		}
		// 20110711
		dMechanicalUppLimitZ = stSearchLimitOutput[iAxisZ].dLimitPosition;
		if(dPositionBondHead_FireLevel > dMechanicalUppLimitZ)
		{
			dPositionBondHead_FireLevel = dMechanicalUppLimitZ;
		}
	}
	// (2) Home Y: Positive Limit, motor size, far away from lead-frame
	if(mtn_dll_axis_search_limit_go_home(Handle, iAxisY) == MTN_API_ERROR)
	{
		iRet = MTN_API_ERROR;
		goto label_return_bonder_xyz_start_search_limit_go_home;
	}
	// 20110711
//	dMechanicalUppLimitY = stSearchLimitOutput[iAxisY].dLimitPosition;

	// 2.5  Y Table, goto Start Bondering Position, 
	if(! acsc_ToPoint(Handle, 0, // start up the motion immediately
			iCtrlAxisACS_Y, dPositionTableY_StartBonding_HomeZ, NULL) )
	{
		iRet = MTN_API_ERROR;
		goto label_return_bonder_xyz_start_search_limit_go_home;
	}

	// Until fully stopped and no error
	while(mtn_qc_is_axis_still_moving(Handle, iCtrlAxisACS_Y) == TRUE)  // iAxisY
	{
		Sleep(1);
	}
	if(mtn_qc_is_axis_locked_safe(Handle, iCtrlAxisACS_Y) == FALSE)  // iAxisY
	{
		iRet = MTN_API_ERROR;
		goto label_return_bonder_xyz_start_search_limit_go_home;
	}

	// (3) Home X: 
	if(mtn_dll_axis_search_limit_go_home(Handle, iAxisX) == MTN_API_ERROR)
	{
		iRet = MTN_API_ERROR;
		goto label_return_bonder_xyz_start_search_limit_go_home;
	}
	// 20110711
//	dMechanicalUppLimitX = stSearchLimitOutput[iAxisX].dLimitPosition;

	// (3.5) X Table goto Start Bondering Position,  
	if(! acsc_ToPoint(Handle, 0, // start up the motion immediately
			iCtrlAxisACS_X, dPositionTableX_StartBonding_HomeZ, NULL) )
	{
		iRet = MTN_API_ERROR;
		goto label_return_bonder_xyz_start_search_limit_go_home;
	}
	Sleep(2000);  // 20110915
	while(mtn_qc_is_axis_still_moving(Handle, iCtrlAxisACS_X) == TRUE)  // iAxisX
	{
		Sleep(1000);  // 20110915
	}
	if(mtn_qc_is_axis_locked_safe(Handle, iCtrlAxisACS_X) == FALSE)  // iAxisX
	{
		iRet = MTN_API_ERROR;
		goto label_return_bonder_xyz_start_search_limit_go_home;
	}

	// (4) Home Z
	if(iFlagMachType != WB_STATION_XY_VERTICAL
			&& iFlagMachType != WB_STATION_XY_TOP
			&& iFlagMachType != WB_STATION_WIRE_CLAMP) // 20110914
	{
		if(mtn_dll_axis_go_home_from_limit(Handle, iAxisZ) == MTN_API_ERROR)
		{
			iRet =  MTN_API_ERROR;
			goto label_return_bonder_xyz_start_search_limit_go_home;
		}
		else
		{
			iRet =  MTN_API_OK_ZERO;
		}
		// 20110711  // stSearchHomeFromLimitOutput, stSearchIndexOutput[iAxis].dDistanceFromIndex1ToLimit)
	//	dMechanicalUppLimitZ = fabs(stSearchHomeFromLimitOutput[iAxisZ].dDistanceFromIndex1ToLimit); // - stSearchIndexOutput[iAxisZ].dDistanceFromIndex2ToLimit; // dDistanceFromIndex1ToLimit;
	//  should read relax position from SP variable,
		MTUNE_OUT_POSN_COMPENSATION stOutputPosnCompensationTune;
		mtn_api_get_spring_compensation_sp_para(Handle, &stOutputPosnCompensationTune);
		dMechanicalRelaxPositionZ = (double)stOutputPosnCompensationTune.iEncoderOffsetSP;

		// (4.5) Z Goto FireLevel
		if(! acsc_ToPoint(Handle, 0, // start up the motion immediately
				iCtrlAxisACS_BondHeadZ, 
				dMechanicalRelaxPositionZ, //dMechanicalUppLimitZ, 20110807
				NULL) )  // dPositionBondHead_FireLevel, // 20110711
		{
	//		stpSearchLimitOutput->iErrorCode = SEARCH_HOME_MOTOR_ERROR;
			iRet = MTN_API_ERROR;
			goto label_return_bonder_xyz_start_search_limit_go_home;
		}
	} // 20110914
label_return_bonder_xyz_start_search_limit_go_home:
	// restore parameter
	mtnapi_download_servo_parameter_acs_v60_per_axis(Handle, iCtrlAxisACS_BondHeadZ, &stBakupCtrlParameter_Z);
	mtn_wb_restore_software_limit(Handle, iCtrlAxisACS_X, iCtrlAxisACS_Y, iCtrlAxisACS_BondHeadZ);
	mtn_wb_home_restore_speed_parameter_acs(Handle); /// 20110720
	//
	mtn_wb_restore_servo_protect_limit(Handle, iCtrlAxisACS_X, iCtrlAxisACS_Y, iCtrlAxisACS_BondHeadZ);

	if(iRet == MTN_API_ERROR)
	{
		mtnapi_disable_motor(Handle, iAxisX, 0);
		mtnapi_disable_motor(Handle, iAxisY, 0);
		mtnapi_disable_motor(Handle, iAxisZ, 0);
	}
	return iRet;
}

/********************       The following buffer program is from ACS controller.  **************/
// Following function is for the application layer, s.t. it has minimum parameters to set
// This is for table applications
// (1) Move a distance,
// (2) Search limit by position error
// (3) Move a distance,
// (4) Search index1
// (5) Clear feedback position
// (6) Move a distance
// (7) Fine search index2 1st time
// (8) Move a distance
// (9) Fine search index2 2nd time
int mtn_dll_api_search_home_acs(HANDLE mHandle, 
							MTN_API_SEARCH_INDEX_INPUT *stpSearchHomeInputAPI, 
							MTN_API_SEARCH_INDEX_OUTPUT *stpSearchHomeOutputAPI)
{
	MTN_SEARCH_INDEX_INPUT stSearchHomeInput;
	MTN_SEARCH_INDEX_OUTPUT stSearchHomeOutput;
//	int iAxis = stpSearchHomeInputAPI->iAxisOnACS;
	int iRet = MTN_API_OK_ZERO;; // return value

	// Backup parameter and download special parameter for searching home, 20090512
	CTRL_PARA_ACS  stBakupCtrlParameter;
	iRet = mtnapi_upload_servo_parameter_acs_per_axis(mHandle, stpSearchHomeInputAPI->iAxisOnACS, &stBakupCtrlParameter);
//	if(stpSearchHomeInputAPI->iAxisOnACS == MTN_API_AXIS_Z_ON_ACS)
//	{
//		mtnapi_download_bonder_z_servo_para_search_home();
//	}

	stSearchHomeInput.stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_CNT_SEC;
	stSearchHomeInput.stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_CNT_SEC;
	stSearchHomeInput.stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_CNT_SEC;
	stSearchHomeInput.stSpeedProfileMove.dMaxKillDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_CNT_SEC;
	stSearchHomeInput.stSpeedProfileMove.dStepperDriveVelocity = 0;  // Reserve for later use
	stSearchHomeInput.stSpeedProfileMove.dStepperStartVelocity = 0;  // Reserve for later use
	stSearchHomeInput.stSpeedProfileMove.uiProfileType = 0; // Reserve for later use

	stSearchHomeInput.stSpeedProfileMove.dMaxVelocity = stpSearchHomeInputAPI->dMoveMaxVelInterSearch;

	stSearchHomeInput.dMaxDistanceRangeProtection = stpSearchHomeInputAPI->dMaxDistanceRangeProtection;
	stSearchHomeInput.dMoveDistanceBeforeSearchIndex1 = stpSearchHomeInputAPI->dMoveDistanceBeforeSearchIndex1;
	stSearchHomeInput.dMoveDistanceBeforeSearchIndex2 = stpSearchHomeInputAPI->dMoveDistanceBeforeSearchIndex2;
	stSearchHomeInput.dMoveDistanceBeforeSearchLimit = stpSearchHomeInputAPI->dMoveDistanceBeforeSearchLimit;
	stSearchHomeInput.dPositionErrorThresholdSearchLimit = stpSearchHomeInputAPI->dPositionErrorThresholdSearchLimit;
	stSearchHomeInput.dPosnErrThresHoldSettling = stpSearchHomeInputAPI->dPosnErrThresHoldSettling;
	stSearchHomeInput.dVelJoggingIndex1 = stpSearchHomeInputAPI->dVelJoggingIndex1;
	stSearchHomeInput.dVelJoggingIndex2 = stpSearchHomeInputAPI->dVelJoggingIndex2;
	stSearchHomeInput.dVelJoggingLimit = stpSearchHomeInputAPI->dVelJoggingLimit;
	stSearchHomeInput.iDebug = stpSearchHomeInputAPI->iDebug;
	stSearchHomeInput.uiFreqFactor_10KHz_Detecting = 1;  // By default
	stSearchHomeInput.iAxisOnACS = stpSearchHomeInputAPI->iAxisOnACS; // Reserve for later use
	stSearchHomeInput.iFlagVerifyByRepeatingIndex2 = 0; // Reserve for later use

	iRet = mtn_dll_search_home_acs(mHandle, &stSearchHomeInput, &stSearchHomeOutput);

	stpSearchHomeOutputAPI->dIndexPositionFineSearch_1 = stSearchHomeOutput.dIndexPositionFineSearch_1;
	stpSearchHomeOutputAPI->dIndexPositionFineSearch_2 = stSearchHomeOutput.dIndexPositionFineSearch_2;
	stpSearchHomeOutputAPI->iErrorCode = stSearchHomeOutput.iErrorCode;

	// Restore the backuped parameter, 20090512
	mtnapi_download_servo_parameter_acs_per_axis(mHandle, stpSearchHomeInputAPI->iAxisOnACS, &stBakupCtrlParameter);

	return iRet;
}

// Search limit, 
// for z-applications
int mtn_dll_api_search_limit_acs(HANDLE mHandle, 
							 MTN_API_SEARCH_LIMIT_INPUT *stpSearchLimitInput, 
							 MTN_API_SEARCH_LIMIT_OUTPUT *stpSearchLimitOutput)
{
//	int iAxis = stpSearchLimitInput->iAxisOnACS;
	int iRet = MTN_API_OK_ZERO;; // return value

	MTN_SEARCH_LIMIT_INPUT stSearchLimitInput;
	MTN_SEARCH_LIMIT_OUTPUT stSearchLimitOutput;

	// Backup parameter and download special parameter for searching home, 20090512
	CTRL_PARA_ACS  stBakupCtrlParameter;
	iRet = mtnapi_upload_servo_parameter_acs_per_axis(mHandle, stpSearchLimitInput->iAxisOnACS, &stBakupCtrlParameter);
//	if(stpSearchLimitInput->iAxisOnACS == MTN_API_AXIS_Z_ON_ACS)
//	{
//		mtnapi_download_bonder_z_servo_para_search_home();
//	}

	stSearchLimitInput.stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_CNT_SEC;
	stSearchLimitInput.stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_CNT_SEC;
	stSearchLimitInput.stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_CNT_SEC;
	stSearchLimitInput.stSpeedProfileMove.dMaxKillDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_CNT_SEC;
	stSearchLimitInput.stSpeedProfileMove.dStepperDriveVelocity = 0;
	stSearchLimitInput.stSpeedProfileMove.dStepperStartVelocity = 0;
	stSearchLimitInput.stSpeedProfileMove.uiProfileType = 0;
	stSearchLimitInput.stSpeedProfileMove.dMaxVelocity = stpSearchLimitInput->dMoveMaxVelInterSearch;

	stSearchLimitInput.dMaxDistanceRangeProtection = stpSearchLimitInput->dMaxDistanceRangeProtection;
	stSearchLimitInput.dMoveDistanceAfterSearchLimit = stpSearchLimitInput->dMoveDistanceAfterSearchLimit;
	stSearchLimitInput.dMoveDistanceBeforeSearchLimit = stpSearchLimitInput->dMoveDistanceBeforeSearchLimit;
	stSearchLimitInput.dPositionErrorThresholdSearchLimit = stpSearchLimitInput->dPositionErrorThresholdSearchLimit;
	stSearchLimitInput.dPosnErrThresHoldSettling = stpSearchLimitInput->dPosnErrThresHoldSettling;
	stSearchLimitInput.dVelJoggingLimit = stpSearchLimitInput->dVelJoggingLimit;
	stSearchLimitInput.iAxisOnACS = stpSearchLimitInput->iAxisOnACS;
	stSearchLimitInput.iDebug = 0;
	stSearchLimitInput.uiFreqFactor_10KHz_Detecting = 1;

	iRet = mtn_search_limit_acs(mHandle, &stSearchLimitInput, &stSearchLimitOutput);

	stpSearchLimitOutput->dLimitPosition = stSearchLimitOutput.dLimitPosition;
	stpSearchLimitOutput->iErrorCode = stSearchLimitOutput.iErrorCode;

	// Restore the backuped parameter, 20090512
	mtnapi_download_servo_parameter_acs_per_axis(mHandle, stpSearchLimitInput->iAxisOnACS, &stBakupCtrlParameter);

	return iRet;

}
// Search index from limit area
// for z-applications in XYZ start-up
int mtn_dll_api_search_home_from_limit_acs(HANDLE mHandle, 
									   MTN_API_SEARCH_HOME_FROM_LIMIT_INPUT *stpSearchHomeFromLimitInput, 
									   MTN_API_SEARCH_HOME_FROM_LIMIT_OUTPUT *stpSearchHomeFromLimitOutput)
{
//	int iAxis = stpSearchHomeFromLimitInput->iAxisOnACS;
	int iRet = MTN_API_OK_ZERO; // return value

	MTN_SEARCH_HOME_FROM_LIMIT_INPUT stSearchHomeFromLimitInput; 
	MTN_SEARCH_HOME_FROM_LIMIT_OUTPUT stSearchHomeFromLimitOutput;

	// Backup parameter and download special parameter for searching home, 20090512
	CTRL_PARA_ACS  stBakupCtrlParameter;
	iRet = mtnapi_upload_servo_parameter_acs_per_axis(mHandle, stpSearchHomeFromLimitInput->iAxisOnACS, &stBakupCtrlParameter);
//	if(stpSearchHomeFromLimitInput->iAxisOnACS == MTN_API_AXIS_Z_ON_ACS)
//	{
//		mtnapi_download_bonder_z_servo_para_search_home();
//	}

	stSearchHomeFromLimitInput.stSpeedProfileMove.dMaxAcceleration = DEFAULT_SEARCH_HOME_MOVE_ACC_CNT_SEC;
	stSearchHomeFromLimitInput.stSpeedProfileMove.dMaxDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_CNT_SEC;
	stSearchHomeFromLimitInput.stSpeedProfileMove.dMaxJerk = DEFAULT_SEARCH_HOME_MOVE_JERK_CNT_SEC;
	stSearchHomeFromLimitInput.stSpeedProfileMove.dMaxKillDeceleration = DEFAULT_SEARCH_HOME_MOVE_DEC_CNT_SEC;
	stSearchHomeFromLimitInput.stSpeedProfileMove.dStepperDriveVelocity = 0;
	stSearchHomeFromLimitInput.stSpeedProfileMove.dStepperStartVelocity = 0;
	stSearchHomeFromLimitInput.stSpeedProfileMove.uiProfileType = 0;
	stSearchHomeFromLimitInput.stSpeedProfileMove.dMaxVelocity = stpSearchHomeFromLimitInput->dMoveMaxVelInterSearch;

	stSearchHomeFromLimitInput.dMaxDistanceRangeProtection = stpSearchHomeFromLimitInput->dMaxDistanceRangeProtection;
	stSearchHomeFromLimitInput.dMoveDistanceBeforeSearchIndex1 = stpSearchHomeFromLimitInput->dMoveDistanceBeforeSearchIndex1;
	stSearchHomeFromLimitInput.dMoveDistanceBeforeSearchIndex2 = stpSearchHomeFromLimitInput->dMoveDistanceBeforeSearchIndex2;
	stSearchHomeFromLimitInput.dPosnErrThresHoldSettling = stpSearchHomeFromLimitInput->dPosnErrThresHoldSettling;
	stSearchHomeFromLimitInput.dVelJoggingIndex1 = stpSearchHomeFromLimitInput->dVelJoggingIndex1;
	stSearchHomeFromLimitInput.dVelJoggingIndex2 = stpSearchHomeFromLimitInput->dVelJoggingIndex2;
	stSearchHomeFromLimitInput.iAxisOnACS = stpSearchHomeFromLimitInput->iAxisOnACS;
	stSearchHomeFromLimitInput.iDebug = stpSearchHomeFromLimitInput->iDebug;
	stSearchHomeFromLimitInput.iFlagVerifyByRepeatingIndex2 = 0;
	stSearchHomeFromLimitInput.uiFreqFactor_10KHz_Detecting = 1;

	iRet = mtn_search_home_from_limit_acs(mHandle, 
						&stSearchHomeFromLimitInput,
						&stSearchHomeFromLimitOutput);

	stpSearchHomeFromLimitOutput->dIndexPositionFineSearch_1 = stSearchHomeFromLimitOutput.dIndexPositionFineSearch_1;
	stpSearchHomeFromLimitOutput->dIndexPositionFineSearch_2 = stSearchHomeFromLimitOutput.dIndexPositionFineSearch_2;
	stpSearchHomeFromLimitOutput->iErrorCode = stSearchHomeFromLimitOutput.iErrorCode;

	// Restore the backuped parameter, 20090512
	mtnapi_download_servo_parameter_acs_per_axis(mHandle, stpSearchHomeFromLimitInput->iAxisOnACS, &stBakupCtrlParameter);

	return iRet;
}

////////////////////////////////////////
// Search limit -> Search index
int mtn_dll_search_home_acs(HANDLE mHandle, MTN_SEARCH_INDEX_INPUT *stpSearchHomeInput, MTN_SEARCH_INDEX_OUTPUT *stpSearchHomeOutput)
{
	int iRet = MTN_API_OK_ZERO;; // return value
//	int iAxis = stpSearchHomeInput->iAxisOnACS; // servo axis for searching index
//	double dblRefPosition;  // Axis Reference position

	MTN_SEARCH_LIMIT_INPUT stSearchLimitInput;
	MTN_SEARCH_LIMIT_OUTPUT stSearchLimitOutput;

	stSearchLimitInput.iDebug = stpSearchHomeInput->iDebug;
	stSearchLimitInput.iAxisOnACS = stpSearchHomeInput->iAxisOnACS;
	stSearchLimitInput.dMaxDistanceRangeProtection = stpSearchHomeInput->dMaxDistanceRangeProtection;
	stSearchLimitInput.dMoveDistanceBeforeSearchLimit = stpSearchHomeInput->dMoveDistanceBeforeSearchLimit;
	stSearchLimitInput.dMoveDistanceAfterSearchLimit = stpSearchHomeInput->dMoveDistanceBeforeSearchIndex1 / 2.0; //0; 20090311
	stSearchLimitInput.dPositionErrorThresholdSearchLimit = stpSearchHomeInput->dPositionErrorThresholdSearchLimit;
	stSearchLimitInput.dPosnErrThresHoldSettling = stpSearchHomeInput->dPosnErrThresHoldSettling;
	stSearchLimitInput.dVelJoggingLimit = stpSearchHomeInput->dVelJoggingLimit;
	stSearchLimitInput.stSpeedProfileMove = stpSearchHomeInput->stSpeedProfileMove;
	stSearchLimitInput.uiFreqFactor_10KHz_Detecting = stpSearchHomeInput->uiFreqFactor_10KHz_Detecting;

	iRet = mtn_search_limit_acs(mHandle, 
							 &stSearchLimitInput, 
							 &stSearchLimitOutput); // stSearchLimitOutput[stpSearchHomeInput->iAxisOnACS]);  // 
	
	if(iRet != MTN_API_OK_ZERO)
	{
		stpSearchHomeOutput->iErrorCode = stSearchLimitOutput.iErrorCode;
		goto label_return_search_home_acs;
	}


	MTN_SEARCH_HOME_FROM_LIMIT_INPUT stSearchHomeFromLimitInput;
	MTN_SEARCH_HOME_FROM_LIMIT_OUTPUT stSearchHomeFromLimitOutput;

	stSearchHomeFromLimitInput.dMaxDistanceRangeProtection = stpSearchHomeInput->dMaxDistanceRangeProtection;
	stSearchHomeFromLimitInput.dMoveDistanceBeforeSearchIndex1 = stpSearchHomeInput->dMoveDistanceBeforeSearchIndex1 / 2.0;  // 20090311
	stSearchHomeFromLimitInput.dMoveDistanceBeforeSearchIndex2 = stpSearchHomeInput->dMoveDistanceBeforeSearchIndex2;
	stSearchHomeFromLimitInput.dPosnErrThresHoldSettling = stpSearchHomeInput->dPosnErrThresHoldSettling;
	stSearchHomeFromLimitInput.dVelJoggingIndex1 = stpSearchHomeInput->dVelJoggingIndex1;
	stSearchHomeFromLimitInput.dVelJoggingIndex2 = stpSearchHomeInput->dVelJoggingIndex2;
	stSearchHomeFromLimitInput.iAxisOnACS = stpSearchHomeInput->iAxisOnACS;
	stSearchHomeFromLimitInput.iDebug = stpSearchHomeInput->iDebug;
	stSearchHomeFromLimitInput.stSpeedProfileMove = stpSearchHomeInput->stSpeedProfileMove;
	stSearchHomeFromLimitInput.uiFreqFactor_10KHz_Detecting = stpSearchHomeInput->uiFreqFactor_10KHz_Detecting;
	stSearchHomeFromLimitInput.iFlagVerifyByRepeatingIndex2 = stpSearchHomeInput->iFlagVerifyByRepeatingIndex2;

	iRet = mtn_search_home_from_limit_acs(mHandle, 
									   &stSearchHomeFromLimitInput, 
									   &stSearchHomeFromLimitOutput);

	if(iRet != MTN_API_OK_ZERO)
	{
		stpSearchHomeOutput->iErrorCode = stSearchHomeFromLimitOutput.iErrorCode;
	}
	else
	{
		stpSearchHomeOutput->dDistanceFromIndex1ToLimit = stSearchHomeFromLimitOutput.dDistanceFromIndex1ToLimit;
		stpSearchHomeOutput->dDistanceFromIndex2ToLimit = stSearchHomeFromLimitOutput.dDistanceFromIndex2ToLimit;
		stpSearchHomeOutput->dIndexPositionFineSearch_1 = stSearchHomeFromLimitOutput.dIndexPositionFineSearch_1;
		stpSearchHomeOutput->dIndexPositionFineSearch_2 = stSearchHomeFromLimitOutput.dIndexPositionFineSearch_2;
		stpSearchHomeOutput->iErrorCode = 0;
	}
label_return_search_home_acs:

	return iRet;

}

///////////////////////////
static int iCurrAxisLimitHome;
void mtn_dll_home_set_current_axis(int iAxis)
{
	iCurrAxisLimitHome = iAxis;
}
static HANDLE stServoCtrlHandle;
void mtn_dll_home_set_acs_ctrl_handle(HANDLE hHandle)
{
	stServoCtrlHandle = hHandle;
}

CWinThread* m_pWinThreadMtnHome;
int iFlagStopThreadMtnHome = TRUE;

int mtn_dll_home_get_flag_stopping_thread()
{
	return iFlagStopThreadMtnHome;
}
void mtn_dll_home_stop_thread()
{
	if (m_pWinThreadMtnHome)
	{
		iFlagStopThreadMtnHome = TRUE;
		WaitForSingleObject(m_pWinThreadMtnHome->m_hThread, 1000);
		// delete m_pWinThreadSpecTest;
		m_pWinThreadMtnHome = NULL;
	}
}

UINT mtn_dll_home_axis_thread()
{
	while(iFlagStopThreadMtnHome == FALSE)
	{

		mtn_dll_axis_search_limit_go_home(stServoCtrlHandle, iCurrAxisLimitHome);
		iFlagStopThreadMtnHome = TRUE;
	}
	return 0;
}

// Home by Thread
UINT mtn_dll_start_axis_home_thread( LPVOID pParam )
{
	return mtn_dll_home_axis_thread();
}

void mtn_dll_trigger_axis_home_thread()
{
	if(iFlagStopThreadMtnHome == TRUE)
	{
		iFlagStopThreadMtnHome = FALSE;
		//
		m_pWinThreadMtnHome = AfxBeginThread(mtn_dll_start_axis_home_thread, 0);  // mtn_dll_home_axis_thread
		SetPriorityClass(m_pWinThreadMtnHome->m_hThread, REALTIME_PRIORITY_CLASS);

		m_pWinThreadMtnHome->m_bAutoDelete = FALSE;
	}
}

UINT mtn_dll_home_wb_xyz_thread()
{
	while(iFlagStopThreadMtnHome == FALSE)
	{
		//mtn_dll_axis_search_limit_go_home(stServoCtrlHandle, iCurrAxisLimitHome);
		mtn_dll_bonder_xyz_start_search_limit_go_home(stServoCtrlHandle, APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
		iFlagStopThreadMtnHome = TRUE;
	}
	return 0;
}

// Home by Thread
UINT mtn_dll_start_home_wb_xyz_thread( LPVOID pParam )
{
	return mtn_dll_home_wb_xyz_thread();
}

void mtn_dll_trigger_wb_xyz_home_thread()
{
	if(iFlagStopThreadMtnHome == TRUE)
	{
		iFlagStopThreadMtnHome = FALSE;
		//
		m_pWinThreadMtnHome = AfxBeginThread(mtn_dll_start_home_wb_xyz_thread, 0);  // mtn_dll_home_axis_thread
		SetPriorityClass(m_pWinThreadMtnHome->m_hThread, REALTIME_PRIORITY_CLASS);

		m_pWinThreadMtnHome->m_bAutoDelete = FALSE;
	}
}
