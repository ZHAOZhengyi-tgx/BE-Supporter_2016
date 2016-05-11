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

// Module: Motion Initialization for ACS controller
// 
// History
// YYYYMMDD  Author			Notes
// 20081101  Zhengyi		Created
// 20081231  Zhengyi        Released after tested by force control
// 20090129  ZhengyiJohn	Add Relax position
// 20090219  Zhengyi replace ServoController.h to AcsServo.h, follow ZhangDong's new release
// 20090508  Zhengyi        Add multiple blocks for parameter
// 20100730
// 20130215                 Add arBHCtrl
#include "stdafx.h"
#include "MtnInitAcs.h"
#include "mtnconfg.h"
#include <sys\stat.h>

#include "MtnWbDef.h"
extern AXIS_INFO_WIRE_BOND astAxisInfoWireBond[MAX_SERVO_AXIS_WIREBOND];

unsigned int mtn_wb_tune_get_max_sector_table_x(); // 20100730 from MtnWbTune.cpp
unsigned int mtn_wb_tune_get_max_sector_table_y(); // 20100730 from MtnWbTune.cpp

// For custermer's interface
#include "AcsServo.h" // "ServoController.h" // 20081009
//#include "MACDEF.h"
static char strTempReadFile[65536];

static char strServoMasterCfg[] = "SERVO_MASTER_CONFIG";
static char strServoTotalAxisLabel[] = "TOTAL_AXIS";
static char strServoCtrlBoardLabel[] = "CONTROLLER_BOARD";

static char strServoSettingCfg[] = "SERVO_SETTING_AXIS_";
static char strServoApplicNameLabel[] = "APPLICATION_NAME";
static char strServoAxisParaFilename[] = "PARA_FILE_PATH";
static char strServoAxisMtnTuneFilename[] = "MTN_TUNE_AXIS";
static char strServoAxisIndexLabel[] = "AXIS_ON_SERVO_BOARD";

static char strDebugMessage[512];

SERVO_AXIS_BLK stServoAxis_ACS[MAX_CTRL_AXIS_PER_SERVO_BOARD];
SAFETY_PARA_ACS stSafetyParaAxis_ACS[MAX_CTRL_AXIS_PER_SERVO_BOARD];
BASIC_PARA_ACS_AXIS stBasicParaAxis_ACS[MAX_CTRL_AXIS_PER_SERVO_BOARD];
SERVO_ACS stServoACS;
ACSC_PCI_SLOT gstSysPCICards[SYS_MAX_ACS_CARDS_PCI];
COMM_SETTINGS stServoControllerCommSet;

// to be port into class-member
int giFlagObtainedCards;
double dControllerInterruptTs_ms = 0.5; // dControllerInterruptTs_ms has a unit: ms

void sys_init_controller_ts(double dTs_ms)
{
	dControllerInterruptTs_ms = dTs_ms;
}
double sys_get_controller_ts()
{
	return dControllerInterruptTs_ms;
}

#include "MtnTesterResDef.h"

//char *pstrACS_SP_VarAdv[] =
//{
//	"DetectSw",
//	"ForceOffSw",
//	"ForceMode",
//	"SpringCompOn",
//	"PreDrvCmd",
//	"ContactDrvCmd",
//	"posCollLow",
//	"SpringCompGa",
//	"SpringCompGf",
//	"SpringOffset",
//	"SpringCompOut",
//	"JerkFFGa",
//	"JerkFFGf",
//	"KxaGa",
//	"KxaGf",
//	"KaxGa",
//	"KaxGf",
//	"LevelPE",
//	"LevelAIN"
//};
//--
//DetectSw 每 switch for collision detection.
//If 0: No detection, if 1: detection based on DOUT (current), if 2: detection
//based on PE (position)
//If 3: Detection based on AIN (force sensor)
//The switch will reset automatically after collision is detected.
//--
//ForceOffSw 每 if set to 1, the controller will switch from force mode back to
//position mode without any jump
//--
//ForceMode 每 flag that indicates if the axis is in force mode (=1) or encoder
//mode(=0)
//--
//SpringCompOn 每 flag to activate spring compensation
//--
//PreDrvCmd 每 force applied in the fractional time between collision detection
//till the next MPU cycle. Also used as threshold for DOUT criterion.
//--
//ContactDrvCmd 每 force is changed to this level after the fractional time
//--
//posCollLow - 24bits variable that store the position where collision was
//detected (20kHz latching, in DSP counts)
//--
//SpringCompGa 每 Spring compensation gain (currently multiplies the feedback
//position, may be changed to reference position). Signed value
//--
//SpringCompGf - Spring compensation gain factor (total gain= GA*2^(-GF)
//--
//SpringOffset 每 constant offset for spring compensation
//--
//SpringCompOut 每 variable that shows the spring compensation output
//--
//JerkFFGa - Jerk FF gain (signed value)
//--
//JerkFFGf - Jerk FF gain factor
//--
//KxaGa - Gain for XA inter-coupling compensation. Feedforward of "A"
//acceleration to "X" drive command (signed value)
//--
//KxaGf - Gain factor for XA inter-coupling compensation
//--
//KaxGa - Gain for AX inter-coupling compensation. Feedforward of "X"
//acceleration to "A" drive command (signed value)
//--
//KaxGf - Gain factor for AX inter-coupling compensation
//--
//LevelPE - Threshold for PE collision detection (positive value)
//--
//LevelAIN- Threshould for AIN collision detection (positive value)

// "SETSP(0, GETSPA(0,\"X_DZ_MIN\"), %d)"

// global REAL arMotorXProfile(4)(3); [Blk0:Blk3] [MaxVel, MaxAcc, MaxJerk]
// global REAL arMotorAProfile(5)(3); [Blk0:Blk4] [MaxVel, MaxAcc, MaxJerk]
// global REAL arACtrl(9)(4); [Blk0:Blk8] [KPV, KIV, KPP, SLAFF]
// Load Speed Profile, Table: arMotorXProfile; BondHead: arMotorAProfile
// Ctrl: arACtrl
//MTN_SPEED_PROFILE astWbTableXSpeedProfile[NUM_TABLE_SPEED_PROF_BLK];
//MTN_SPEED_PROFILE astWbTableYSpeedProfile[NUM_TABLE_SPEED_PROF_BLK];
//MTN_SPEED_PROFILE astWbBondHeadSpeedProfile[NUM_BONDHEAD_SPEED_PROF_BLK];
//CTRL_PARA_ACS	  astWbBondHeadServoCtrlPara[NUM_BONDHEAD_SERVO_CTRL_BLK];

static int iFlagCommuConnectionTypeACS = FLAG_NO_CONNECTION_ACS;

void sys_set_ctrl_communication_type(int iCommuniType)
{
	iFlagCommuConnectionTypeACS = iCommuniType;
}
int sys_get_ctrl_communication_type()
{
	return iFlagCommuConnectionTypeACS;
}

int mtn_tune_upload_wb_table_x_speed_profile_blk(HANDLE hCommunicationHandle, unsigned int uiBlk)
{
	int iRet = MTN_API_OK_ZERO;
	double dTempProfile[3];

	if(uiBlk < NUM_TABLE_SPEED_PROF_BLK && uiBlk < MAX_BLK_PARAMETER)
	{
		// Load default from ACS variables
		mtnapi_get_speed_profile(hCommunicationHandle, APP_X_TABLE_ACS_ID, 
			&stServoAxis_ACS[WB_AXIS_TABLE_X].stSpeedProfile[uiBlk], NULL);
		// Read from WB application
		if(acsc_ReadReal(hCommunicationHandle, ACSC_NONE, "arMotorXProfile", uiBlk, uiBlk, 0, 2,  dTempProfile, NULL)) // NON-Zero succeed;  // Global Variable, 20100726
		{
			if(dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1) // protection
			{
				stServoAxis_ACS[WB_AXIS_TABLE_X].stSpeedProfile[uiBlk].dMaxAcceleration = dTempProfile[1]; // astWbTableXSpeedProfile[uiBlk].dMaxAcceleration = ;
				stServoAxis_ACS[WB_AXIS_TABLE_X].stSpeedProfile[uiBlk].dMaxDeceleration = dTempProfile[1]; // astWbTableXSpeedProfile[uiBlk].dMaxDeceleration = dTempProfile[1];
				stServoAxis_ACS[WB_AXIS_TABLE_X].stSpeedProfile[uiBlk].dMaxJerk = dTempProfile[2];
				stServoAxis_ACS[WB_AXIS_TABLE_X].stSpeedProfile[uiBlk].dMaxVelocity = dTempProfile[0];
			}
		}
	}
	else
	{
		iRet = MTN_API_ERROR_EXCEED_MAX_BLK;
	}
	return iRet;
}


int mtn_tune_upload_wb_table_y_speed_profile_blk(HANDLE hCommunicationHandle, unsigned int uiBlk)
{
	int iRet = MTN_API_OK_ZERO;
	double dTempProfile[3];
	if(uiBlk < NUM_TABLE_SPEED_PROF_BLK && uiBlk < MAX_BLK_PARAMETER)
	{
		// Load default from ACS variables
		mtnapi_get_speed_profile(hCommunicationHandle, APP_Y_TABLE_ACS_ID, 
			&stServoAxis_ACS[WB_AXIS_TABLE_Y].stSpeedProfile[uiBlk], NULL);
		// Read from WB application
		int iFlagSuccessRead = 0;
		if(acsc_ReadReal(hCommunicationHandle, ACSC_NONE, "arMotorYProfile", uiBlk, uiBlk, 0, 2,  dTempProfile, NULL)) // NON-Zero succeed;  // Global Variable, 20100726, arMotorXProfile, 20110201
		{
			iFlagSuccessRead = 1;
		}
		else if(acsc_ReadReal(hCommunicationHandle, ACSC_NONE, "arMotorXProfile", uiBlk, uiBlk, 0, 2,  dTempProfile, NULL)) // NON-Zero succeed;  // Global Variable, 20100726, arMotorXProfile, 20110201
		{ // for version compatible with elder-WireBonder.exe before Ref25
			iFlagSuccessRead = 1;
		}

		if(iFlagSuccessRead == 1)
		{
			if(dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1) // protection
			{
				stServoAxis_ACS[WB_AXIS_TABLE_Y].stSpeedProfile[uiBlk].dMaxAcceleration = dTempProfile[1];
				stServoAxis_ACS[WB_AXIS_TABLE_Y].stSpeedProfile[uiBlk].dMaxDeceleration = dTempProfile[1];
				stServoAxis_ACS[WB_AXIS_TABLE_Y].stSpeedProfile[uiBlk].dMaxJerk = dTempProfile[2];
				stServoAxis_ACS[WB_AXIS_TABLE_Y].stSpeedProfile[uiBlk].dMaxVelocity = dTempProfile[0];
			}
		}
	}
	else
	{
		iRet = MTN_API_ERROR_EXCEED_MAX_BLK;
	}
	return iRet;
}

#include "MotAlgo_DLL.h"
int mtn_tune_upload_wb_bh_z_speed_profile_blk(HANDLE hCommunicationHandle, unsigned int uiBlk)
{
	int iRet = MTN_API_OK_ZERO;
	double dTempProfile[3];
	if(uiBlk < NUM_BONDHEAD_SPEED_PROF_BLK && uiBlk < MAX_BLK_PARAMETER)
	{
		// Load default from ACS variables
		mtnapi_get_speed_profile(hCommunicationHandle, sys_get_acs_axis_id_bnd_z(), 
			&stServoAxis_ACS[WB_AXIS_BOND_Z].stSpeedProfile[uiBlk], NULL);
 
		// Read from WB application
		if(acsc_ReadReal(hCommunicationHandle, ACSC_NONE, "arMotorAProfile", uiBlk, uiBlk, 0, 2,  dTempProfile, NULL)) // NON-Zero succeed; // Global Variable, 20100726
		{
			if(dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1) // protection
			{
				stServoAxis_ACS[WB_AXIS_BOND_Z].stSpeedProfile[uiBlk].dMaxAcceleration = dTempProfile[1];  // astWbBondHeadSpeedProfile[uiBlk]
				stServoAxis_ACS[WB_AXIS_BOND_Z].stSpeedProfile[uiBlk].dMaxDeceleration = dTempProfile[1];  // astWbBondHeadSpeedProfile[uiBlk]
				stServoAxis_ACS[WB_AXIS_BOND_Z].stSpeedProfile[uiBlk].dMaxJerk = dTempProfile[2]; // astWbBondHeadSpeedProfile[uiBlk]
				stServoAxis_ACS[WB_AXIS_BOND_Z].stSpeedProfile[uiBlk].dMaxVelocity = dTempProfile[0]; // astWbBondHeadSpeedProfile[uiBlk]
			}
		}
	}
	else
	{
		iRet = MTN_API_ERROR_EXCEED_MAX_BLK;
	}
	return iRet;
}
int mtn_tune_upload_wb_speed_profile(HANDLE hCommunicationHandle)
{
	int iRet = MTN_API_OK_ZERO;
//	double dTempProfile[3];
	int ii;
	for(ii = 0; ii<NUM_TABLE_SPEED_PROF_BLK; ii++)
	{
		iRet = mtn_tune_upload_wb_table_x_speed_profile_blk(hCommunicationHandle, ii);
		if(iRet != MTN_API_OK_ZERO) break;
		mtn_tune_upload_wb_table_y_speed_profile_blk(hCommunicationHandle, ii);
		if(iRet != MTN_API_OK_ZERO) break;
	}
	for(ii = 0; ii<NUM_BONDHEAD_SPEED_PROF_BLK; ii++)
	{
		if(iRet != MTN_API_OK_ZERO) break;
		mtn_tune_upload_wb_bh_z_speed_profile_blk(hCommunicationHandle, ii);
	}

	return iRet;
}

int mtn_tune_upload_wb_bh_z_servo_ctrl(HANDLE hCommunicationHandle, unsigned int uiBlk)
{
	int iRet = MTN_API_OK_ZERO;
	double dTempProfile[4];

	// 20110721
	mtn_api_get_spring_compensation_sp_para(hCommunicationHandle, &(stServoAxis_ACS[WB_AXIS_BOND_Z].stPosnCompensationWbBH));

	if(uiBlk < NUM_BONDHEAD_SERVO_CTRL_BLK && uiBlk < MAX_BLK_PARAMETER)
	{
		// Load default from ACS variables
		mtnapi_upload_servo_parameter_acs_per_axis(hCommunicationHandle, 
			stServoAxis_ACS[WB_AXIS_BOND_Z].uiAxisOnACS, // 20130127 sys_get_acs_axis_id_bnd_z(), 
			&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk]);
		// Read from WB application
		if(acsc_ReadReal(hCommunicationHandle, ACSC_NONE, "arACtrl", uiBlk, uiBlk, 0, 3,  dTempProfile, NULL))  // ;	  // Global Variable, 20100726
		{
			if(dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1) // protection
			{
				stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dVelocityLoopProportionalGain = dTempProfile[0]; // astWbBondHeadServoCtrlPara[uiBlk]
				stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dVelocityLoopIntegratorGain = dTempProfile[1];   // astWbBondHeadServoCtrlPara[uiBlk]
				stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dPositionLoopProportionalGain = dTempProfile[2];  // astWbBondHeadServoCtrlPara[uiBlk]
				stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dAccelerationFeedforward = dTempProfile[3];  // astWbBondHeadServoCtrlPara[uiBlk]
			}
		}
		else if(acsc_ReadReal(hCommunicationHandle, ACSC_NONE, "arBHCtrl", uiBlk, uiBlk, 0, 3,  dTempProfile, NULL)) // arBHCtrl, 20130215
		{
			if(dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1) // protection
			{
				stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dVelocityLoopProportionalGain = dTempProfile[0]; // astWbBondHeadServoCtrlPara[uiBlk]
				stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dVelocityLoopIntegratorGain = dTempProfile[1];   // astWbBondHeadServoCtrlPara[uiBlk]
				stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dPositionLoopProportionalGain = dTempProfile[2];  // astWbBondHeadServoCtrlPara[uiBlk]
				stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dAccelerationFeedforward = dTempProfile[3];  // astWbBondHeadServoCtrlPara[uiBlk]
			}
		}
	}
	else
	{
		iRet = MTN_API_ERROR_EXCEED_MAX_BLK;
	}

	return iRet;
}

int mtn_tune_upload_wb_tbl_x_servo_ctrl(HANDLE hCommunicationHandle, unsigned int uiBlk)
{
	int iRet = MTN_API_OK_ZERO;
	double dTempProfile[4];
	if(uiBlk < NUM_TABLE_X_SERVO_CTRL_BLK && uiBlk < MAX_BLK_PARAMETER)
	{
		// Load default from ACS variables
		mtnapi_upload_servo_parameter_acs_per_axis(hCommunicationHandle, 
			stServoAxis_ACS[WB_AXIS_TABLE_X].uiAxisOnACS, // 20130127, BugFix WB_AXIS_TABLE_X, 
			&stServoAxis_ACS[WB_AXIS_TABLE_X].stServoParaACS[uiBlk]);
		// Read from WB application
		if(acsc_ReadReal(hCommunicationHandle, ACSC_NONE, "arXCtrl", uiBlk, uiBlk, 0, 3,  dTempProfile, NULL)) // NON-Zero succeed;	  // Global Variable, 20100726
		{
			if(dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1) // protection
			{
				stServoAxis_ACS[WB_AXIS_TABLE_X].stServoParaACS[uiBlk].dVelocityLoopProportionalGain = dTempProfile[0]; // astWbBondHeadServoCtrlPara[uiBlk]
				stServoAxis_ACS[WB_AXIS_TABLE_X].stServoParaACS[uiBlk].dVelocityLoopIntegratorGain = dTempProfile[1];   // astWbBondHeadServoCtrlPara[uiBlk]
				stServoAxis_ACS[WB_AXIS_TABLE_X].stServoParaACS[uiBlk].dPositionLoopProportionalGain = dTempProfile[2];  // astWbBondHeadServoCtrlPara[uiBlk]
				stServoAxis_ACS[WB_AXIS_TABLE_X].stServoParaACS[uiBlk].dAccelerationFeedforward = dTempProfile[3];  // astWbBondHeadServoCtrlPara[uiBlk]
			}
		}
	}
	else
	{
		iRet = MTN_API_ERROR_EXCEED_MAX_BLK;
	}

	return iRet;
}

int mtn_tune_upload_wb_tbl_y_servo_ctrl(HANDLE hCommunicationHandle, unsigned int uiBlk)
{
	int iRet = MTN_API_OK_ZERO;
	double dTempProfile[4];
	if(uiBlk < NUM_TABLE_Y_SERVO_CTRL_BLK && uiBlk < MAX_BLK_PARAMETER)
	{
		// Load default from ACS variables
		mtnapi_upload_servo_parameter_acs_per_axis(hCommunicationHandle, 
			stServoAxis_ACS[WB_AXIS_TABLE_Y].uiAxisOnACS, // 20130127, WB_AXIS_TABLE_Y, 
			&stServoAxis_ACS[WB_AXIS_TABLE_Y].stServoParaACS[uiBlk]);
		// Read from WB application
		if(acsc_ReadReal(hCommunicationHandle, ACSC_NONE, "arYCtrl", uiBlk, uiBlk, 0, 3,  dTempProfile, NULL))  //;	  // Global Variable, 
		{
			if(dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1) // protection
			{
				stServoAxis_ACS[WB_AXIS_TABLE_Y].stServoParaACS[uiBlk].dVelocityLoopProportionalGain = dTempProfile[0]; // astWbBondHeadServoCtrlPara[uiBlk]
				stServoAxis_ACS[WB_AXIS_TABLE_Y].stServoParaACS[uiBlk].dVelocityLoopIntegratorGain = dTempProfile[1];   // astWbBondHeadServoCtrlPara[uiBlk]
				stServoAxis_ACS[WB_AXIS_TABLE_Y].stServoParaACS[uiBlk].dPositionLoopProportionalGain = dTempProfile[2];  // astWbBondHeadServoCtrlPara[uiBlk]
				stServoAxis_ACS[WB_AXIS_TABLE_Y].stServoParaACS[uiBlk].dAccelerationFeedforward = dTempProfile[3];  // astWbBondHeadServoCtrlPara[uiBlk]
			}
		}
	}
	else
	{
		iRet = MTN_API_ERROR_EXCEED_MAX_BLK;
	}

	return iRet;
}


//
int mtn_tune_upload_wb_servo_ctrl(HANDLE hCommunicationHandle)
{
	int iRet = MTN_API_OK_ZERO;
//	double dTempProfile[4];
	int ii;
	for(ii = 0; ii<NUM_BONDHEAD_SERVO_CTRL_BLK; ii++)
	{
		iRet = mtn_tune_upload_wb_bh_z_servo_ctrl(hCommunicationHandle, ii);
		if(iRet != MTN_API_OK_ZERO) break;
	}

	if(iRet != MTN_API_OK_ZERO) 
	{
	}
	else
	{
		for(ii = 0; ii<NUM_TABLE_X_SERVO_CTRL_BLK; ii++)
		{
			iRet = mtn_tune_upload_wb_tbl_x_servo_ctrl(hCommunicationHandle, ii);
			if(iRet != MTN_API_OK_ZERO) break;
		}
		//iRet = mtnapi_upload_servo_parameter_acs_per_axis(hCommunicationHandle, WB_AXIS_TABLE_X, 
		//		&(stServoACS.stpServoAxis_ACS[WB_AXIS_TABLE_X]->stServoParaACS[DEF_BLK_UPLOAD_PARA_FROM_CTRL])); // 20090508
	}

	if(iRet != MTN_API_OK_ZERO) 
	{
	}
	else
	{
		for(ii = 0; ii<NUM_TABLE_Y_SERVO_CTRL_BLK; ii++)
		{
			iRet = mtn_tune_upload_wb_tbl_y_servo_ctrl(hCommunicationHandle, ii);
			if(iRet != MTN_API_OK_ZERO) break;
		}
		//iRet = mtnapi_upload_servo_parameter_acs_per_axis(hCommunicationHandle, WB_AXIS_TABLE_Y, 
		//	&(stServoACS.stpServoAxis_ACS[WB_AXIS_TABLE_Y]->stServoParaACS[DEF_BLK_UPLOAD_PARA_FROM_CTRL])); // 20090508
	}

	return iRet;
}

int mtnapi_upload_wb_servo_speed_parameter_acs(HANDLE hCommunicationHandle)
{
	int iRet = MTN_API_OK_ZERO;

	iRet = mtn_tune_upload_wb_servo_ctrl(hCommunicationHandle);

	if(iRet != MTN_API_OK_ZERO) 
	{
	}
	else
	{
		iRet = mtn_tune_upload_wb_speed_profile(hCommunicationHandle);
	}
	return iRet;
}

//#ifdef __CHECK_CONFIRM__
#define DEF_BOARD_ID_ACS_BONDER_APP      0 
int mtnapi_download_bonder_z_servo_para_search_home()
{
	int iRet = MTN_API_OK_ZERO;
	iRet = mtnapi_download_servo_parameter_acs_per_axis(stServoACS.Handle, 
			stServoACS.stpServoAxis_ACS[WB_AXIS_BOND_Z]->uiAxisOnACS, // DEF_BOARD_ID_ACS_BONDER_APP
			&(stServoACS.stpServoAxis_ACS[WB_AXIS_BOND_Z]->stServoParaACS[DEF_BLK_ID_SERVO_PARA_Z_SEARCH_HOME])); // 20090508 DEF_BOARD_ID_ACS_BONDER_APP

	return iRet;
}
//#endif

int mtnapi_init_servo_control_para_acs(char *strFilename, HANDLE Handle) 
{
	int iRet = MTN_API_OK_ZERO;

	// 1. initialize with default parameter
	mtnapi_init_def_servo_acs();

	// 2. initialize from a file
	// 2-a. read the master config file
	iRet = mtnapi_init_master_config_acs(strFilename, Handle);
	if(iRet != MTN_API_OK_ZERO)
	{
		return iRet;
	}

	// 2-b.read parameter for each axis
	mtnapi_init_all_axis_parameter_from_file_no_label();
//	mtnapi_init_servo_parameter_from_file();
//	mtnapi_init_speed_parameter_from_file();
//	mtnapi_init_position_reg_from_file(); // 20090129

	// 3. download servo-parameter in the default block for each axis
	mtnapi_download_servo_control_para_all_axis_acs();

	return iRet;
}

static char strWbInitServoDefaultFilePath[256] = "C:\\WbData\\DefParaBase\\ServoMaster.ini";
char *pFilename = &strWbInitServoDefaultFilePath[0];
int mtnapi_init_wb_def_servo_control_para_acs(HANDLE hAcsHandle) 
{
	int iRet = mtnapi_init_servo_control_para_acs(strWbInitServoDefaultFilePath, hAcsHandle);  // pFilename, &strWbInitServoDefaultFilePath[0], hAcsHandle);

	return iRet;
}

int mtnapi_save_master_cfg_file()
{
	int iRet = MTN_API_OK_ZERO;
	unsigned int ii;
	FILE *fptr;
	fopen_s(&fptr, stServoACS.strFilename, "w");

	if(fptr != NULL)
	{
		fprintf(fptr, "\n\n[SERVO_MASTER_CONFIG]\n");
		fprintf(fptr, "TOTAL_AXIS = %d\n", stServoACS.uiTotalNumAxis);
		fprintf(fptr, "CONTROLLER_BOARD = %s\n\n", stServoACS.strCtrlBoardName);

		for(ii = 0; ii<stServoACS.uiTotalNumAxis; ii++)
		{
			fprintf(fptr, "[SERVO_SETTING_AXIS_%d]\n", ii + 1);
			fprintf(fptr, "APPLICATION_NAME = %s\n", stServoACS.stpServoAxis_ACS[ii]->strApplicationNameAxis);
			fprintf(fptr, "PARA_FILE_PATH = %s\n", stServoACS.stpServoAxis_ACS[ii]->strAxisServoParaFilename);
			fprintf(fptr, "MTN_TUNE_AXIS = %s\n", stServoACS.stpServoAxis_ACS[ii]->strAxisMtnServoTune);
			fprintf(fptr, "AXIS_ON_SERVO_BOARD = %d\n\n", stServoACS.stpServoAxis_ACS[ii]->uiAxisOnACS);
		}


		for(int ii = 0; ii <NUM_TOTAL_PROGRAM_BUFFER; ii ++)               // // 20090508
		{
			fprintf(fptr, "\n\n[ACS_PROG_CONFIG-%d]\n", ii);
			fprintf(fptr, "0x%x\n", stServoACS.stGlobalParaACS.aiProgramFlags[ii]);	// ProgramFlags
			fprintf(fptr, "%d\n", stServoACS.stGlobalParaACS.aiProgramRate[ii]);	// ProgramRate
			fprintf(fptr, "%d\n", stServoACS.stGlobalParaACS.aiProgramAutoRoutineRate[ii]);	// ProgramAutoRoutineRate
		}

		fclose(fptr);
	}
	else
	{
		iRet = MTN_API_ERROR_OPEN_FILE;
	}

	return iRet;
}

int mtnapi_save_servo_parameter_acs()
{
	int iRet = MTN_API_OK_ZERO;
	for(unsigned int ii = 0; ii<stServoACS.uiTotalNumAxis; ii++)
	{
		iRet = mtnapi_save_servo_parameter_acs_per_axis_no_label(ii); // mtnapi_save_servo_parameter_acs_per_axis(ii); 

		if(iRet != MTN_API_OK_ZERO)
		{
			break;
		}
	}
	return iRet;

}

void mtnapi_get_debug_message(char strDebugMessageInitPara[512])
{
	sprintf_s(strDebugMessageInitPara, 512, "%s", strDebugMessage);
}

int mtnapi_save_servo_parameter_acs_per_axis(unsigned int uiAxis)
{
	FILE *fptr;
	fopen_s(&fptr, stServoACS.stpServoAxis_ACS[uiAxis]->strAxisServoParaFilename, "w");

	for(int ii = 0; ii <MAX_BLK_PARAMETER; ii ++)               // // 20090508
	{
		fprintf(fptr, "\n\n[SERVO_PARA_BLK-%d]\n", ii);
		fprintf(fptr, "SLPKP = %15.9f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dPositionLoopProportionalGain);
		fprintf(fptr, "SLVKP = %15.9f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dVelocityLoopProportionalGain );
		fprintf(fptr, "SLVKI = %15.9f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dVelocityLoopIntegratorGain );
		fprintf(fptr, "SLVLI = %15.9f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dVelocityLoopIntegratorLimit );
		fprintf(fptr, "SLVSOF = %15.9f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dSecondOrderLowPassFilterBandwidth );
		fprintf(fptr, "SLVSOFD = %15.9f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dSecondOrderLowPassFilterDamping );
		fprintf(fptr, "SLVNFRQ = %15.9f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dNotchFilterFrequency );
		fprintf(fptr, "SLVNWID = %15.9f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dNotchFilterWidth );
		fprintf(fptr, "SLVNATT = %15.9f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dNotchFilterAttenuation );
		fprintf(fptr, "SLAFF = %15.9f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dAccelerationFeedforward );
		fprintf(fptr, "SLFRC = %15.9f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dDynamicFrictionFeedforward );
		//fprintf(fptr, "DCOM = %d\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].iConstantDriveCommand );
		//fprintf(fptr, "XCURI = %15.9f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dTorqueLimitIdle );
		//fprintf(fptr, "XCURV = %15.9f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dTorqueLimitMoving );
		//fprintf(fptr, "XRMS = %15.9f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dTorqueLimitRMS );
	}   // // 20090508

    // [SPEED_PROFILE]
	for(int ii = 0; ii <MAX_BLK_PARAMETER; ii ++)               // // 20090508
	{
		fprintf(fptr, "\n\n[SPEED_PROFILE_BLK-%d]\n", ii);
		fprintf(fptr, "MAX_VEL = %15.9f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[ii].dMaxVelocity );
		fprintf(fptr, "MAX_ACC = %15.9f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[ii].dMaxAcceleration );	
		fprintf(fptr, "MAX_DEC = %15.9f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[ii].dMaxDeceleration );
		fprintf(fptr, "MAX_KILL_DEC = %15.9f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[ii].dMaxKillDeceleration );
		fprintf(fptr, "MAX_JERK = %15.9f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[ii].dMaxJerk );
		fprintf(fptr, "PROF_TYPE = %d\n", stServoAxis_ACS[uiAxis].stSpeedProfile[ii].uiProfileType );
	}
	// [POSITION_REGISTRATION]  // 20090129
	fprintf(fptr, "\n\n[POSITION_REGISTRATION]\n");
	fprintf(fptr, "POSITIVE_LIMIT_POSN = %12.1f\n", stServoAxis_ACS[uiAxis].stServoPositionReg.dPositiveLimit);
	fprintf(fptr, "NEGATIVE_LIMIT_POSN = %12.1f\n", stServoAxis_ACS[uiAxis].stServoPositionReg.dNegativeLimit);
	fprintf(fptr, "RELAX_POSN = %12.1f\n", stServoAxis_ACS[uiAxis].stServoPositionReg.dRelaxPosition);

	fclose(fptr);

	return MTN_API_OK_ZERO;
}

int mtnapi_rename_bakup_servo_parameter_acs_per_axis(unsigned int uiAxis)
{
struct tm stTime;
__time64_t stLongTime;
char strFileNewName[512];

	_time64(&stLongTime);
	_localtime64_s(&stTime, &stLongTime);

	sprintf_s(strFileNewName, 512, "%s_%d-%d-%d_H%d-M%d.bak", stServoACS.stpServoAxis_ACS[uiAxis]->strAxisServoParaFilename,
		stTime.tm_year +1900, stTime.tm_mon +1, stTime.tm_mday, stTime.tm_hour, stTime.tm_min);
	if(rename(stServoACS.stpServoAxis_ACS[uiAxis]->strAxisServoParaFilename, strFileNewName))
	{
		return MTN_API_ERROR;
	}
	else
	{
		return MTN_API_OK_ZERO;
	}
}
// 
int mtnapi_save_servo_parameter_acs_per_axis_no_label(unsigned int uiAxis)
{
	FILE *fptr;
	fopen_s(&fptr, stServoACS.stpServoAxis_ACS[uiAxis]->strAxisServoParaFilename, "w");

	int iMaxNumBlk, nNumBlkUseByWbTuning;
	iMaxNumBlk = MAX_BLK_PARAMETER;

	for(int ii = 0; ii <iMaxNumBlk; ii ++)               // // 20090508
	{
		fprintf(fptr, "\n\n[SERVO_PARA_BLK-%d]\n", ii);
		fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dPositionLoopProportionalGain);  // SLPKP = 
		fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dVelocityLoopProportionalGain ); // SLVKP = 
		fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dVelocityLoopIntegratorGain );   // SLVKI = 
		fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dVelocityLoopIntegratorLimit );  // SLVLI = 
		fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dSecondOrderLowPassFilterBandwidth );  // SLVSOF = 
		fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dSecondOrderLowPassFilterDamping );    // SLVSOFD = 
		fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dNotchFilterFrequency );				// SLVNFRQ = 
		fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dNotchFilterWidth );					// SLVNWID = 
		fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dNotchFilterAttenuation );				// SLVNATT = 
		fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dAccelerationFeedforward );			// SLAFF = 
		fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dDynamicFrictionFeedforward );			// SLFRC = 

		/// 2013-01-
		switch (uiAxis)
		{
		case WB_AXIS_TABLE_X:
			nNumBlkUseByWbTuning = mtn_wb_tune_get_max_sector_table_x(); // 20100730
			break;
		case WB_AXIS_TABLE_Y:
			nNumBlkUseByWbTuning = mtn_wb_tune_get_max_sector_table_y(); // 20100730
			break;
		case WB_AXIS_BOND_Z:
			nNumBlkUseByWbTuning = NUM_BONDHEAD_SERVO_CTRL_BLK;
			break;
		case WB_AXIS_WIRE_CLAMP:
			nNumBlkUseByWbTuning = 1;
			break;
		default:
			nNumBlkUseByWbTuning = 0;
			break;
		}
		if(ii == nNumBlkUseByWbTuning - 1)
		{
			fprintf(fptr, "#### NOT USED\n");
		}

	}   // // 20090508

	switch (uiAxis)
	{
	case WB_AXIS_TABLE_X:
		iMaxNumBlk = NUM_TABLE_SPEED_PROF_BLK;
		break;
	case WB_AXIS_TABLE_Y:
		iMaxNumBlk = NUM_TABLE_SPEED_PROF_BLK;
		break;
	case WB_AXIS_BOND_Z:
		iMaxNumBlk = NUM_BONDHEAD_SPEED_PROF_BLK;
		break;
	case WB_AXIS_WIRE_CLAMP:
		iMaxNumBlk = 1;
		break;
	default:
		iMaxNumBlk = 0;
		break;
	}
    // [SPEED_PROFILE]
	if(uiAxis == WB_AXIS_BOND_Z)  // 20130111
	{
		for(int ii = 0; ii <iMaxNumBlk - 1; ii ++)
		{
				fprintf(fptr, "\n\n[SPEED_PROFILE_BLK-%d]\n", ii + 1);
				fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[ii].dMaxVelocity );					
				fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[ii].dMaxAcceleration );				
				fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[ii].dMaxDeceleration );				
				fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[ii].dMaxKillDeceleration );			
				fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[ii].dMaxJerk );						
				fprintf(fptr, "%d\n", stServoAxis_ACS[uiAxis].stSpeedProfile[ii].uiProfileType );						
		}
				fprintf(fptr, "\n\n[SPEED_PROFILE_BLK-0]\n");
				fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[iMaxNumBlk - 1].dMaxVelocity );					
				fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[iMaxNumBlk - 1].dMaxAcceleration );				
				fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[iMaxNumBlk - 1].dMaxDeceleration );				
				fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[iMaxNumBlk - 1].dMaxKillDeceleration );			
				fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[iMaxNumBlk - 1].dMaxJerk );						
				fprintf(fptr, "%d\n", stServoAxis_ACS[uiAxis].stSpeedProfile[iMaxNumBlk - 1].uiProfileType );						
	}
	else
	{
		for(int ii = 0; ii <iMaxNumBlk; ii ++)               // // 20090508
		{
			fprintf(fptr, "\n\n[SPEED_PROFILE_BLK-%d]\n", ii);
			fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[ii].dMaxVelocity );					// MAX_VEL = 
			fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[ii].dMaxAcceleration );				// MAX_ACC = 
			fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[ii].dMaxDeceleration );				// MAX_DEC = 
			fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[ii].dMaxKillDeceleration );			// MAX_KILL_DEC = 
			fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stSpeedProfile[ii].dMaxJerk );						// MAX_JERK = 
			fprintf(fptr, "%d\n", stServoAxis_ACS[uiAxis].stSpeedProfile[ii].uiProfileType );						// PROF_TYPE = 
		}
	}

	fprintf(fptr, "\n\n[SP_VAR]\n");  // 20110720
	fprintf(fptr, "%12.1f\n", stServoAxis_ACS[uiAxis].stServoParaACS[0].dJerkFf); // 20110720

	if(uiAxis == WB_AXIS_BOND_Z)  // 20110720
	{
		fprintf(fptr, "\n\n[SP_POSN_COMP]\n");  
		fprintf(fptr, "%12.8f\n", stServoAxis_ACS[uiAxis].stPosnCompensationWbBH.dPositionFactor);				// 
		fprintf(fptr, "%12.8f\n", stServoAxis_ACS[uiAxis].stPosnCompensationWbBH.dCtrlOutOffset);				// 
		fprintf(fptr, "%d\n", stServoAxis_ACS[uiAxis].stPosnCompensationWbBH.iFactor_SP_ACS);				// 
		fprintf(fptr, "%d\n", stServoAxis_ACS[uiAxis].stPosnCompensationWbBH.iGain_SP_ACS);				// 
		fprintf(fptr, "%d\n", stServoAxis_ACS[uiAxis].stPosnCompensationWbBH.iOffset_SP_ACS);				// 
		fprintf(fptr, "%d\n", stServoAxis_ACS[uiAxis].stPosnCompensationWbBH.iFlagEnable);				// 
		stServoAxis_ACS[uiAxis].stServoPositionReg.dRelaxPosition = stServoAxis_ACS[uiAxis].stPosnCompensationWbBH.iEncoderOffsetSP;
	} // 20110720

	fprintf(fptr, "\n\n[COMMON_SL]\n");  // 20110720
	fprintf(fptr, "%10.6f\n", stServoAxis_ACS[uiAxis].stServoParaACS[0].dCurrentLoopOffset); // 20110720

struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;

	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);
	fprintf(fptr, "\n\n[CLOCK_PRINT_YYYY-MM-DD_HH-mm]\n");  
	fprintf(fptr, "%d-%d-%d_%d-%d\n", 
		stpTime->tm_year + 1900, stpTime->tm_mon + 1, stpTime->tm_mday, 
		stpTime->tm_hour, stpTime->tm_min);  // 20110720

	// [POSITION_REGISTRATION]  // 20090129
	fprintf(fptr, "\n\n[POSITION_REGISTRATION]\n");
	fprintf(fptr, "%12.1f\n", stServoAxis_ACS[uiAxis].stServoPositionReg.dPositiveLimit);				// POSITIVE_LIMIT_POSN = 
	fprintf(fptr, "%12.1f\n", stServoAxis_ACS[uiAxis].stServoPositionReg.dNegativeLimit);				// NEGATIVE_LIMIT_POSN = 
	fprintf(fptr, "%12.1f\n", stServoAxis_ACS[uiAxis].stServoPositionReg.dRelaxPosition);				// RELAX_POSN = 

	fprintf(fptr, "\n\n[SAFETY_PARA]\n");
	fprintf(fptr, "%12.3f\n", stSafetyParaAxis_ACS[uiAxis].dPosnErrIdle);
	fprintf(fptr, "%12.3f\n", stSafetyParaAxis_ACS[uiAxis].dPosnErrVel);
	fprintf(fptr, "%12.3f\n", stSafetyParaAxis_ACS[uiAxis].dPosnErrAcc);
	fprintf(fptr, "%12.3f\n", stSafetyParaAxis_ACS[uiAxis].dCriticalPosnErrIdle);
	fprintf(fptr, "%12.3f\n", stSafetyParaAxis_ACS[uiAxis].dCriticalPosnErrVel);
	fprintf(fptr, "%12.3f\n", stSafetyParaAxis_ACS[uiAxis].dCriticalPosnErrAcc);
	fprintf(fptr, "%12.3f\n", stSafetyParaAxis_ACS[uiAxis].dDynamicBrakeThresholdVel);
	fprintf(fptr, "%12.3f\n", stSafetyParaAxis_ACS[uiAxis].dMaxVelX);
	fprintf(fptr, "%12.3f\n", stSafetyParaAxis_ACS[uiAxis].dMaxAccX);
	fprintf(fptr, "%12.3f\n", stSafetyParaAxis_ACS[uiAxis].dRMS_DrvCmdX);
	fprintf(fptr, "%12.3f\n", stSafetyParaAxis_ACS[uiAxis].dRMS_DrvCmdIdle);
	fprintf(fptr, "%12.3f\n", stSafetyParaAxis_ACS[uiAxis].dRMS_DrvCmdMtn);
	fprintf(fptr, "%12.3f\n", stSafetyParaAxis_ACS[uiAxis].dRMS_TimeConst);
	fprintf(fptr, "%12.3f\n", stSafetyParaAxis_ACS[uiAxis].dSoftwarePosnLimitLow);
	fprintf(fptr, "%12.3f\n", stSafetyParaAxis_ACS[uiAxis].dSoftwarePosnLimitUpp);

	fprintf(fptr, "\n\n[BASIC_CONFIG]\n");
	fprintf(fptr, "0x%x\n", stBasicParaAxis_ACS[uiAxis].iMotorFlags);	// MFLAGS
	fprintf(fptr, "0x%x\n", stBasicParaAxis_ACS[uiAxis].iAxisFlags);	// AFLAGS
	fprintf(fptr, "%12.3f\n", stBasicParaAxis_ACS[uiAxis].dBrakeOffTime);	// dBrakeOffTime
	fprintf(fptr, "%12.3f\n", stBasicParaAxis_ACS[uiAxis].dBrakeOnTime);	// dBrakeOnTime
	fprintf(fptr, "%12.3f\n", stBasicParaAxis_ACS[uiAxis].dEnableTime);		// dEnableTime
	fprintf(fptr, "%12.3f\n", stBasicParaAxis_ACS[uiAxis].dEncoderFactor);	// dEncoderFactor
	fprintf(fptr, "%12.3f\n", stBasicParaAxis_ACS[uiAxis].dSettlingTime);	// dSettlingTime
	fprintf(fptr, "%12.3f\n", stBasicParaAxis_ACS[uiAxis].dTargetRadix);	// dTargetRadix
	fprintf(fptr, "%d\n", stBasicParaAxis_ACS[uiAxis].iEncoderFreq);	// iEncoderFreq
	fprintf(fptr, "%d\n", stBasicParaAxis_ACS[uiAxis].iEncoderType);	// iEncoderType

	fclose(fptr);

	return MTN_API_OK_ZERO;
}
int mtnapi_init_servo_axis_para_acs(SERVO_AXIS_BLK *stpServoAxis_ACS)
{
	int iRet = MTN_API_OK_ZERO;
	char    buffer[BUFSIZ];
	char *strStopString;

	if(mtn_cfg_OpenConfigFile(stpServoAxis_ACS->strAxisServoParaFilename) != OPENOK)
	{
		iRet = MTN_API_ERROR_OPEN_FILE;
		goto LABEL_RETURN_INIT_SERVO_AXIS;
	}
	
	char strServoParaWithBlk[64];

	for(unsigned int uiBlk= 0; uiBlk < MAX_BLK_PARAMETER; uiBlk ++)   // 20090508
	{
		sprintf_s(strServoParaWithBlk, 64, "SERVO_PARA_BLK-%d", uiBlk);
		if(mtn_cfg_ReadConfigString(strServoParaWithBlk, "SLPKP", &buffer[0]) == READOK)
		{
			stpServoAxis_ACS->stServoParaACS[uiBlk].dPositionLoopProportionalGain = strtod(buffer, &strStopString);
			//sscanf(buffer,"%f", &(stpServoAxis_ACS->stServoParaACS[uiBlk].dPositionLoopProportionalGain));
		}
		else
		{
			sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
				__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto LABEL_RETURN_INIT_SERVO_AXIS;
		}

		if(mtn_cfg_ReadConfigString(strServoParaWithBlk, "SLVKP", &buffer[0]) == READOK)
		{
			stpServoAxis_ACS->stServoParaACS[uiBlk].dVelocityLoopProportionalGain = strtod(buffer, &strStopString);
		}
		else
		{
			sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
				__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto LABEL_RETURN_INIT_SERVO_AXIS;
		}

		if(mtn_cfg_ReadConfigString(strServoParaWithBlk, "SLVKI", &buffer[0]) == READOK)
		{
			stpServoAxis_ACS->stServoParaACS[uiBlk].dVelocityLoopIntegratorGain = strtod(buffer, &strStopString);
		}
		else
		{
			sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
				__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto LABEL_RETURN_INIT_SERVO_AXIS;
		}

		if(mtn_cfg_ReadConfigString(strServoParaWithBlk, "SLVLI", &buffer[0]) == READOK)
		{
			stpServoAxis_ACS->stServoParaACS[uiBlk].dVelocityLoopIntegratorLimit = strtod(buffer, &strStopString);
		}
		else
		{
			sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
				__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto LABEL_RETURN_INIT_SERVO_AXIS;
		}

		if(mtn_cfg_ReadConfigString(strServoParaWithBlk, "SLVSOF", &buffer[0]) == READOK)
		{
			stpServoAxis_ACS->stServoParaACS[uiBlk].dSecondOrderLowPassFilterBandwidth = strtod(buffer, &strStopString);
		}
		else
		{
			sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
				__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto LABEL_RETURN_INIT_SERVO_AXIS;
		}

		if(mtn_cfg_ReadConfigString(strServoParaWithBlk, "SLVSOFD", &buffer[0]) == READOK)
		{
			stpServoAxis_ACS->stServoParaACS[uiBlk].dSecondOrderLowPassFilterDamping = strtod(buffer, &strStopString);
		}
		else
		{
			sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
				__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto LABEL_RETURN_INIT_SERVO_AXIS;
		}

		if(mtn_cfg_ReadConfigString(strServoParaWithBlk, "SLVNFRQ", &buffer[0]) == READOK)
		{
			stpServoAxis_ACS->stServoParaACS[uiBlk].dNotchFilterFrequency = strtod(buffer, &strStopString);
		}
		else
		{
			sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
				__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto LABEL_RETURN_INIT_SERVO_AXIS;
		}

		if(mtn_cfg_ReadConfigString(strServoParaWithBlk, "SLVNWID", &buffer[0]) == READOK)
		{
			stpServoAxis_ACS->stServoParaACS[uiBlk].dNotchFilterWidth = strtod(buffer, &strStopString);
		}
		else
		{
			sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
				__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto LABEL_RETURN_INIT_SERVO_AXIS;
		}

		if(mtn_cfg_ReadConfigString(strServoParaWithBlk, "SLVNATT", &buffer[0]) == READOK)
		{
			stpServoAxis_ACS->stServoParaACS[uiBlk].dNotchFilterAttenuation = strtod(buffer, &strStopString);
		}
		else
		{
			sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
				__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto LABEL_RETURN_INIT_SERVO_AXIS;
		}

		if(mtn_cfg_ReadConfigString(strServoParaWithBlk, "SLAFF", &buffer[0]) == READOK)
		{
			stpServoAxis_ACS->stServoParaACS[uiBlk].dAccelerationFeedforward = strtod(buffer, &strStopString);
		}
		else
		{
			sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
				__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto LABEL_RETURN_INIT_SERVO_AXIS;
		}

		if(mtn_cfg_ReadConfigString(strServoParaWithBlk, "SLFRC", &buffer[0]) == READOK)
		{
			stpServoAxis_ACS->stServoParaACS[uiBlk].dDynamicFrictionFeedforward = strtod(buffer, &strStopString);
		}
		else
		{
			sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
				__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto LABEL_RETURN_INIT_SERVO_AXIS;
		}

		//if(mtn_cfg_ReadConfigString(strServoParaWithBlk, "DCOM", &buffer[0]) == READOK)
		//{
		//	stpServoAxis_ACS->stServoParaACS[uiBlk].iConstantDriveCommand = (int)strtod(buffer, &strStopString);
		//}
		//else
		//{
		//	sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
		//		__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
		//	iRet = MTN_API_ERROR_READ_FILE;
		//	goto LABEL_RETURN_INIT_SERVO_AXIS;
		//}

		//if(mtn_cfg_ReadConfigString(strServoParaWithBlk, "XCURI", &buffer[0]) == READOK)
		//{
		//	stpServoAxis_ACS->stServoParaACS[uiBlk].dTorqueLimitIdle = strtod(buffer, &strStopString);
		//}
		//else
		//{
		//	sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
		//		__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
		//	iRet = MTN_API_ERROR_READ_FILE;
		//	goto LABEL_RETURN_INIT_SERVO_AXIS;
		//}

		//if(mtn_cfg_ReadConfigString(strServoParaWithBlk, "XCURV", &buffer[0]) == READOK)
		//{
		//	stpServoAxis_ACS->stServoParaACS[uiBlk].dTorqueLimitMoving = strtod(buffer, &strStopString);
		//}
		//else
		//{
		//	sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
		//		__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
		//	iRet = MTN_API_ERROR_READ_FILE;
		//	goto LABEL_RETURN_INIT_SERVO_AXIS;
		//}

		//if(mtn_cfg_ReadConfigString(strServoParaWithBlk, "XRMS", &buffer[0]) == READOK)
		//{
		//	stpServoAxis_ACS->stServoParaACS[uiBlk].dTorqueLimitRMS = strtod(buffer, &strStopString);
		//}
		//else
		//{
		//	sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
		//		__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
		//	iRet = MTN_API_ERROR_READ_FILE;
		//	goto LABEL_RETURN_INIT_SERVO_AXIS;
		//}
	}

LABEL_RETURN_INIT_SERVO_AXIS:
	mtn_cfg_CloseConfigFile();

	return iRet;
}

int mtnapi_init_position_registration_axis_acs(SERVO_AXIS_BLK *stpServoAxis_ACS)
{
	int iRet = MTN_API_OK_ZERO;
	char    buffer[BUFSIZ];
	char *strStopString;

	if(mtn_cfg_OpenConfigFile(stpServoAxis_ACS->strAxisServoParaFilename) != OPENOK)
	{
		iRet = MTN_API_ERROR_OPEN_FILE;
		goto LABEL_RETURN_INIT_POSITION_REG;
	}
	
    if(mtn_cfg_ReadConfigString("POSITION_REGISTRATION", "POSITIVE_LIMIT_POSN", &buffer[0]) == READOK)
    {
		stpServoAxis_ACS->stServoPositionReg.dPositiveLimit = strtod(buffer, &strStopString);
    }
    else
    {
        sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
			__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
		iRet = MTN_API_ERROR_READ_FILE;
		goto LABEL_RETURN_INIT_POSITION_REG;
    }

    if(mtn_cfg_ReadConfigString("POSITION_REGISTRATION", "NEGATIVE_LIMIT_POSN", &buffer[0]) == READOK)
    {
		stpServoAxis_ACS->stServoPositionReg.dNegativeLimit = strtod(buffer, &strStopString);
    }
    else
    {
        sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
			__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
		iRet = MTN_API_ERROR_READ_FILE;
		goto LABEL_RETURN_INIT_POSITION_REG;
    }

    if(mtn_cfg_ReadConfigString("POSITION_REGISTRATION", "RELAX_POSN", &buffer[0]) == READOK)
    {
		stpServoAxis_ACS->stServoPositionReg.dRelaxPosition = strtod(buffer, &strStopString);
    }
    else
    {
        sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
			__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
		iRet = MTN_API_ERROR_READ_FILE;
		goto LABEL_RETURN_INIT_POSITION_REG;
    }

LABEL_RETURN_INIT_POSITION_REG:
	mtn_cfg_CloseConfigFile();

	return iRet;

}

int mtnapi_init_speed_para_axis_acs(SERVO_AXIS_BLK *stpServoAxis_ACS)
{
	int iRet = MTN_API_OK_ZERO;
	char    buffer[BUFSIZ];
	char *strStopString;

	if(mtn_cfg_OpenConfigFile(stpServoAxis_ACS->strAxisServoParaFilename) != OPENOK)
	{
		iRet = MTN_API_ERROR_OPEN_FILE;
		goto LABEL_RETURN_INIT_SPEED_PARA_AXIS;
	}
	
	char strSpeedParaWithBlk[64];

	for(unsigned int uiBlk= 0; uiBlk < MAX_BLK_PARAMETER; uiBlk ++)   // 20090508
	{
		sprintf_s(strSpeedParaWithBlk, 64, "SPEED_PROFILE_BLK-%d", uiBlk);

		if(mtn_cfg_ReadConfigString(strSpeedParaWithBlk, "MAX_VEL", &buffer[0]) == READOK)
		{
			stpServoAxis_ACS->stSpeedProfile[uiBlk].dMaxVelocity = strtod(buffer, &strStopString);
			//sscanf(buffer,"%f", &(stpServoAxis_ACS->stServoParaACS[uiBlk].dPositionLoopProportionalGain));
		}
		else
		{
			sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
				__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto LABEL_RETURN_INIT_SPEED_PARA_AXIS;
		}

		if(mtn_cfg_ReadConfigString(strSpeedParaWithBlk, "MAX_ACC", &buffer[0]) == READOK)
		{
			stpServoAxis_ACS->stSpeedProfile[uiBlk].dMaxAcceleration = strtod(buffer, &strStopString);
			//sscanf(buffer,"%f", &(stpServoAxis_ACS->stServoParaACS[uiBlk].dPositionLoopProportionalGain));
		}
		else
		{
			sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
				__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto LABEL_RETURN_INIT_SPEED_PARA_AXIS;
		}

		if(mtn_cfg_ReadConfigString(strSpeedParaWithBlk, "MAX_DEC", &buffer[0]) == READOK)
		{
			stpServoAxis_ACS->stSpeedProfile[uiBlk].dMaxDeceleration = strtod(buffer, &strStopString);
			//sscanf(buffer,"%f", &(stpServoAxis_ACS->stServoParaACS[uiBlk].dPositionLoopProportionalGain));
		}
		else
		{
			sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
				__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto LABEL_RETURN_INIT_SPEED_PARA_AXIS;
		}

		if(mtn_cfg_ReadConfigString(strSpeedParaWithBlk, "MAX_KILL_DEC", &buffer[0]) == READOK)
		{
			stpServoAxis_ACS->stSpeedProfile[uiBlk].dMaxKillDeceleration = strtod(buffer, &strStopString);
			//sscanf(buffer,"%f", &(stpServoAxis_ACS->stServoParaACS[uiBlk].dPositionLoopProportionalGain));
		}
		else
		{
			sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
				__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto LABEL_RETURN_INIT_SPEED_PARA_AXIS;
		}

		if(mtn_cfg_ReadConfigString(strSpeedParaWithBlk, "MAX_JERK", &buffer[0]) == READOK)
		{
			stpServoAxis_ACS->stSpeedProfile[uiBlk].dMaxJerk = strtod(buffer, &strStopString);
			//sscanf(buffer,"%f", &(stpServoAxis_ACS->stServoParaACS[uiBlk].dPositionLoopProportionalGain));
		}
		else
		{
			sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
				__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto LABEL_RETURN_INIT_SPEED_PARA_AXIS;
		}

		if(mtn_cfg_ReadConfigString(strSpeedParaWithBlk, "PROF_TYPE", &buffer[0]) == READOK)
		{
			stpServoAxis_ACS->stSpeedProfile[uiBlk].uiProfileType = (unsigned int)strtod(buffer, &strStopString);
			//sscanf(buffer,"%f", &(stpServoAxis_ACS->stServoParaACS[uiBlk].dPositionLoopProportionalGain));
		}
		else
		{
			sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: not read variable in file %s\n",
				__FILE__, __LINE__, stpServoAxis_ACS->strAxisServoParaFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto LABEL_RETURN_INIT_SPEED_PARA_AXIS;
		}

	}
LABEL_RETURN_INIT_SPEED_PARA_AXIS:
	mtn_cfg_CloseConfigFile();

	return iRet;

}

void mtnapi_init_master_struct_ptr()
{
	unsigned int ii;
	stServoACS.uiTotalNumAxis = MAX_CTRL_AXIS_PER_SERVO_BOARD;
	for( ii = 0; ii<stServoACS.uiTotalNumAxis; ii++)
	{
		stServoACS.stpServoAxis_ACS[ii] = &stServoAxis_ACS[ii];
		stServoACS.stpSafetyAxis_ACS[ii] = &stSafetyParaAxis_ACS[ii];
		stServoACS.stpBasicAxis_ACS[ii] = &stBasicParaAxis_ACS[ii];
	}
//	stServoACS.stpGlobalParaACS = &stGlobalParaACS;
}
//#include "MotAlgo_DLL.h"
void mtnapi_static_lib_init_all()
{
	// 1. initialize with default parameter
	mtnapi_init_master_struct_ptr();
	mtnapi_init_def_servo_acs();

	// 2. Application of Wb
	InitWireBondServoAxisName();

	// 3. Tuning Related
	mtn_tune_init_wb_bondhead_tuning_position_set();
	mtn_tune_init_wb_table_x_tuning_position_set();

	int iFlagMachType = get_sys_machine_type_flag();
	if(iFlagMachType == WB_MACH_TYPE_VLED_FORK || iFlagMachType == WB_STATION_XY_VERTICAL)
	{
		mtn_dll_init_def_para_search_index_vled_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
		mtn_tune_init_wb_table_y_tuning_vled_position_set();
	}
	else if(iFlagMachType == WB_MACH_TYPE_HORI_LED || iFlagMachType == WB_STATION_XY_TOP || iFlagMachType == BE_WB_HORI_20T_LED ||
		iFlagMachType == BE_WB_ONE_TRACK_18V_LED)
	{
		mtn_dll_init_def_para_search_index_hori_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
		if(iFlagMachType == BE_WB_ONE_TRACK_18V_LED)
		{
			mtn_tune_init_wb_table_y_tuning_vled_position_set();
		}
		else
		{
			mtn_tune_init_wb_table_y_tuning_hori_led_position_set();
		}
	}
	else
	{
		mtn_dll_init_def_para_search_index_13v_vled_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
		mtn_tune_init_wb_table_y_tuning_vled_position_set();
	}

}
// Initialize local memory for totoal servo board
// stServoACS: global variable
int mtnapi_init_master_config_acs(char *strFilename, HANDLE Handle)  
{
	char    buffer[BUFSIZ], strTempCfg[MTN_API_MAX_STRLEN_FEATURENAME];
	unsigned int uTemp;
	int retval;
	int iRet = MTN_API_OK_ZERO;

	// 1. Initialize communication handler
	stServoACS.Handle = Handle;

	// 2. initialize from a master config file
    if(mtn_cfg_OpenConfigFile(strFilename) != OPENOK)
    {
        sprintf_s(strDebugMessage, 512, "FATAL ERROR! [%s %d]: Unable to open Master Config file %s\n",
                        __FILE__, __LINE__, strFilename);
        iRet = MTN_API_ERROR_OPEN_FILE;
		return iRet;
    }
	sprintf_s(&stServoACS.strFilename[0], MTN_API_MAX_STRLEN_FILENAME, "%s", strFilename);

    if(mtn_cfg_ReadConfigString("SERVO_MASTER_CONFIG", "TOTAL_AXIS", &buffer[0]) == READOK)
    {
        retval = sscanf_s(buffer,"%d", &uTemp);
        if (retval != EOF && retval == 1)
        {
			stServoACS.uiTotalNumAxis = uTemp;
        }
    }
    else
    {
        sprintf_s(strDebugMessage, 512, "FATAL ERROR! [%s %d]: Total controller axis not specified in %s\n",
			__FILE__, __LINE__, strFilename);
        iRet = MTN_API_ERROR_READ_FILE;
		goto label_return_mtnapi_init_master_config_acs;

    }

    if(mtn_cfg_ReadConfigString(strServoMasterCfg, strServoCtrlBoardLabel, &buffer[0]) == READOK)
    {
        retval = sprintf_s(stServoACS.strCtrlBoardName, MTN_API_MAX_STRLEN_FEATURENAME, "%s", buffer);
    }
    else
    {
        sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: Controller board name not specified in %s\n",
			__FILE__, __LINE__, strFilename);
        iRet = MTN_API_ERROR_READ_FILE;
		goto label_return_mtnapi_init_master_config_acs;

    }

	unsigned int ii;

	for( ii = 0; ii<stServoACS.uiTotalNumAxis; ii++)
	{
		sprintf_s(strTempCfg, MTN_API_MAX_STRLEN_FEATURENAME, "%s%d", strServoSettingCfg, (ii + 1));
		if(mtn_cfg_ReadConfigString(strTempCfg, strServoApplicNameLabel, &buffer[0]) == READOK)
		{
			retval = sprintf_s(&stServoACS.stpServoAxis_ACS[ii]->strApplicationNameAxis[0], 
				MTN_API_MAX_STRLEN_FEATURENAME, "%s", buffer);
		}
		else
		{
	        sprintf_s(strDebugMessage, 512, "Warning! [%s %d]: Axis-%d application name not specified in %s\n",
			__FILE__, __LINE__, ii, strFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto label_return_mtnapi_init_master_config_acs;
		}
		if(mtn_cfg_ReadConfigString(strTempCfg, strServoAxisParaFilename, &buffer[0]) == READOK)
		{
			retval = sprintf_s(stServoACS.stpServoAxis_ACS[ii]->strAxisServoParaFilename, MTN_API_MAX_STRLEN_FEATURENAME, "%s", buffer);
		}
		else
		{
	        sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Axis-%d parameter filename not specified in %s\n",
			__FILE__, __LINE__, ii, strFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto label_return_mtnapi_init_master_config_acs;
		}

		// Servo Axis Motion Tuning Setting
		if(mtn_cfg_ReadConfigString(strTempCfg, strServoAxisMtnTuneFilename, &buffer[0]) == READOK)
		{
			retval = sprintf_s(stServoACS.stpServoAxis_ACS[ii]->strAxisMtnServoTune, MTN_API_MAX_STRLEN_FEATURENAME, "%s", buffer);
		}
		else
		{
	        sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Axis-%d motion tuning setting file not specified in %s\n",
			__FILE__, __LINE__, ii, strFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto label_return_mtnapi_init_master_config_acs;
		}

		if(mtn_cfg_ReadConfigString(strTempCfg, strServoAxisIndexLabel, &buffer[0]) == READOK)
		{
			retval = sscanf_s(buffer, "%d", &uTemp);

			if (retval != EOF && retval == 1)
			{
				stServoACS.stpServoAxis_ACS[ii]->uiAxisOnACS = uTemp;
			}
		}
		else
		{
	        sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Axis-%d's index mapping on servo-board not specified in %s\n",
			__FILE__, __LINE__, ii, strFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto label_return_mtnapi_init_master_config_acs;
		}
	}

label_return_mtnapi_init_master_config_acs:

	mtn_cfg_CloseConfigFile();
	return iRet;

}

void _mtnapi_read_axis_safety_para_acs_no_label(FILE *fptr, int iAxis)
{
	static char *strStopString;
	static char strTempReadSafetyPara[128];
	fscanf(fptr, "%s", strTempReadSafetyPara); 
	stSafetyParaAxis_ACS[iAxis].dPosnErrIdle = strtod(strTempReadSafetyPara, &strStopString);
//    fscanf(fptr, "%f", &stSafetyParaAxis_ACS[iAxis].dPosnErrIdle);
	fscanf(fptr, "%s", strTempReadSafetyPara); 
	stSafetyParaAxis_ACS[iAxis].dPosnErrVel = strtod(strTempReadSafetyPara, &strStopString);
//	fscanf(fptr, "%f", &stSafetyParaAxis_ACS[iAxis].dPosnErrVel);
	fscanf(fptr, "%s", strTempReadSafetyPara); 
	stSafetyParaAxis_ACS[iAxis].dPosnErrAcc = strtod(strTempReadSafetyPara, &strStopString);
//	fscanf(fptr, "%f", &stSafetyParaAxis_ACS[iAxis].dPosnErrAcc)
	fscanf(fptr, "%s", strTempReadSafetyPara); 
	stSafetyParaAxis_ACS[iAxis].dCriticalPosnErrIdle = strtod(strTempReadSafetyPara, &strStopString);
//	fscanf(fptr, "%f", &stSafetyParaAxis_ACS[iAxis].dCriticalPosnErrIdle);
	fscanf(fptr, "%s", strTempReadSafetyPara); 
	stSafetyParaAxis_ACS[iAxis].dCriticalPosnErrVel = strtod(strTempReadSafetyPara, &strStopString);
//	fscanf(fptr, "%f", &stSafetyParaAxis_ACS[iAxis].dCriticalPosnErrVel);
	fscanf(fptr, "%s", strTempReadSafetyPara); 
	stSafetyParaAxis_ACS[iAxis].dCriticalPosnErrAcc = strtod(strTempReadSafetyPara, &strStopString);
//	fscanf(fptr, "%f", &stSafetyParaAxis_ACS[iAxis].dCriticalPosnErrAcc);
	fscanf(fptr, "%s", strTempReadSafetyPara); 
	stSafetyParaAxis_ACS[iAxis].dDynamicBrakeThresholdVel = strtod(strTempReadSafetyPara, &strStopString);
//	fscanf(fptr, "%f", &stSafetyParaAxis_ACS[iAxis].dDynamicBrakeThresholdVel);
	fscanf(fptr, "%s", strTempReadSafetyPara); 
	stSafetyParaAxis_ACS[iAxis].dMaxVelX = strtod(strTempReadSafetyPara, &strStopString);
//	fscanf(fptr, "%f", &stSafetyParaAxis_ACS[iAxis].dMaxVelX);
	fscanf(fptr, "%s", strTempReadSafetyPara); 
	stSafetyParaAxis_ACS[iAxis].dMaxAccX = strtod(strTempReadSafetyPara, &strStopString);
//	fscanf(fptr, "%f", &stSafetyParaAxis_ACS[iAxis].dMaxAccX);
	fscanf(fptr, "%s", strTempReadSafetyPara); 
	stSafetyParaAxis_ACS[iAxis].dRMS_DrvCmdX = strtod(strTempReadSafetyPara, &strStopString);
//	fscanf(fptr, "%f", &stSafetyParaAxis_ACS[iAxis].dRMS_DrvCmdX);
	fscanf(fptr, "%s", strTempReadSafetyPara); 
	stSafetyParaAxis_ACS[iAxis].dRMS_DrvCmdIdle = strtod(strTempReadSafetyPara, &strStopString);
//	fscanf(fptr, "%f", &stSafetyParaAxis_ACS[iAxis].dRMS_DrvCmdIdle);
	fscanf(fptr, "%s", strTempReadSafetyPara); 
	stSafetyParaAxis_ACS[iAxis].dRMS_DrvCmdMtn = strtod(strTempReadSafetyPara, &strStopString);
//	fscanf(fptr, "%f", &stSafetyParaAxis_ACS[iAxis].dRMS_DrvCmdMtn);
	fscanf(fptr, "%s", strTempReadSafetyPara); 
	stSafetyParaAxis_ACS[iAxis].dRMS_TimeConst = strtod(strTempReadSafetyPara, &strStopString);
//	fscanf(fptr, "%f", &stSafetyParaAxis_ACS[iAxis].dRMS_TimeConst);
	fscanf(fptr, "%s", strTempReadSafetyPara); 
	stSafetyParaAxis_ACS[iAxis].dSoftwarePosnLimitLow = strtod(strTempReadSafetyPara, &strStopString);
//	fscanf(fptr, "%f", &stSafetyParaAxis_ACS[iAxis].dSoftwarePosnLimitLow);
	fscanf(fptr, "%s", strTempReadSafetyPara); 
	stSafetyParaAxis_ACS[iAxis].dSoftwarePosnLimitUpp = strtod(strTempReadSafetyPara, &strStopString);
//	fscanf(fptr, "%f", &stSafetyParaAxis_ACS[iAxis].dSoftwarePosnLimitUpp);
}

void _mtnapi_read_axis_basic_para_para_acs_no_label(FILE *fptr, int iAxis)
{
	fscanf(fptr, "%x\n", &stBasicParaAxis_ACS[iAxis].iMotorFlags);	// MFLAGS
	fscanf(fptr, "%x\n", &stBasicParaAxis_ACS[iAxis].iAxisFlags);	// AFLAGS
	fscanf(fptr, "%f\n", &stBasicParaAxis_ACS[iAxis].dBrakeOffTime);	// dBrakeOffTime
	fscanf(fptr, "%f\n", &stBasicParaAxis_ACS[iAxis].dBrakeOnTime);	// dBrakeOnTime
	fscanf(fptr, "%f\n", &stBasicParaAxis_ACS[iAxis].dEnableTime);		// dEnableTime
	fscanf(fptr, "%f\n", &stBasicParaAxis_ACS[iAxis].dEncoderFactor);	// dEncoderFactor
	fscanf(fptr, "%f\n", &stBasicParaAxis_ACS[iAxis].dSettlingTime);	// dSettlingTime
	fscanf(fptr, "%f\n", &stBasicParaAxis_ACS[iAxis].dTargetRadix);	// dTargetRadix
	fscanf(fptr, "%d\n", &stBasicParaAxis_ACS[iAxis].iEncoderFreq);	// iEncoderFreq
	fscanf(fptr, "%d\n", &stBasicParaAxis_ACS[iAxis].iEncoderType);	// iEncoderType
}

static char *strStopCharReadDouble;
void _mtnapi_read_axis_servo_para_acs_no_label(FILE *fptr, int iAxis, int iBlkId)
{
static char strTemp[65536];
	fscanf(fptr, "%s", strTemp);  // SLPKP = _s
	stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dPositionLoopProportionalGain = strtod(strTemp, &strStopCharReadDouble);

	fscanf(fptr, "%s", strTemp);   // SLVKP = 
	stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dVelocityLoopProportionalGain = strtod(strTemp, &strStopCharReadDouble); // fscanf_s(fptr, "%f\n", & );

	fscanf(fptr, "%s", strTemp);  // SLVKI = 
	stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dVelocityLoopIntegratorGain = strtod(strTemp, &strStopCharReadDouble); // fscanf_s(fptr, "%f\n", &stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dVelocityLoopIntegratorGain );   

	fscanf(fptr, "%s", strTemp);  // SLVLI = 
	stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dVelocityLoopIntegratorLimit = strtod(strTemp, &strStopCharReadDouble); // fscanf_s(fptr, "%f\n", &stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dVelocityLoopIntegratorLimit );  

	fscanf(fptr, "%s", strTemp);  // SLVSOF = 
	stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dSecondOrderLowPassFilterBandwidth = strtod(strTemp, &strStopCharReadDouble); // fscanf_s(fptr, "%f\n", &stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dSecondOrderLowPassFilterBandwidth );  

	fscanf(fptr, "%s", strTemp);  // SLVSOFD = 
	stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dSecondOrderLowPassFilterDamping = strtod(strTemp, &strStopCharReadDouble); // fscanf_s(fptr, "%f\n", &stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dSecondOrderLowPassFilterDamping );    

	fscanf(fptr, "%s", strTemp);  // SLVNFRQ = 
	stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dNotchFilterFrequency = strtod(strTemp, &strStopCharReadDouble); // fscanf_s(fptr, "%f\n", &stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dNotchFilterFrequency );				

	fscanf(fptr, "%s", strTemp);  // SLVNWID = 
	stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dNotchFilterWidth = strtod(strTemp, &strStopCharReadDouble); // fscanf_s(fptr, "%f\n", &stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dNotchFilterWidth );					

	fscanf(fptr, "%s", strTemp);  // SLVNATT = 
	stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dNotchFilterAttenuation = strtod(strTemp, &strStopCharReadDouble); // fscanf_s(fptr, "%f\n", &stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dNotchFilterAttenuation );				

	fscanf(fptr, "%s", strTemp);  // SLAFF = 
	stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dAccelerationFeedforward = strtod(strTemp, &strStopCharReadDouble); // fscanf_s(fptr, "%f\n", &stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dAccelerationFeedforward );			

	fscanf(fptr, "%s", strTemp);  // SLFRC = 
	stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dDynamicFrictionFeedforward = strtod(strTemp, &strStopCharReadDouble); // fscanf_s(fptr, "%f\n", &stServoAxis_ACS[iAxis].stServoParaACS[iBlkId].dDynamicFrictionFeedforward );			
}

void _mtnapi_read_axis_speed_para_acs_no_label(FILE *fptr, int iAxis, int iBlk)
{
static char strTemp[128];
	fscanf(fptr, "%s", strTemp);  // MAX_VEL = 
	stServoAxis_ACS[iAxis].stSpeedProfile[iBlk].dMaxVelocity = strtod(strTemp, &strStopCharReadDouble); // fscanf_s(fptr, "%f\n", & );					
	fscanf(fptr, "%s", strTemp);// MAX_ACC = 
	stServoAxis_ACS[iAxis].stSpeedProfile[iBlk].dMaxAcceleration = strtod(strTemp, &strStopCharReadDouble); // fscanf_s(fptr, "%f\n", & );				
	fscanf(fptr, "%s", strTemp);// MAX_DEC = 
	stServoAxis_ACS[iAxis].stSpeedProfile[iBlk].dMaxDeceleration = strtod(strTemp, &strStopCharReadDouble); // 	fscanf_s(fptr, "%f\n", & );				
	fscanf(fptr, "%s", strTemp);// MAX_KILL_DEC = 
	stServoAxis_ACS[iAxis].stSpeedProfile[iBlk].dMaxKillDeceleration = strtod(strTemp, &strStopCharReadDouble); // 	fscanf_s(fptr, "%f\n", & );			
	fscanf(fptr, "%s", strTemp);// MAX_JERK = 
	stServoAxis_ACS[iAxis].stSpeedProfile[iBlk].dMaxJerk  = strtod(strTemp, &strStopCharReadDouble); // 	fscanf_s(fptr, "%f\n", &);						
	fscanf(fptr, "%s", strTemp);// PROF_TYPE = 
	stServoAxis_ACS[iAxis].stSpeedProfile[iBlk].uiProfileType = (unsigned int)strtod(strTemp, &strStopCharReadDouble); // 	fscanf_s(fptr, "%d\n", & );						
}

int mtnapi_init_axis_para_acs_no_label(int iAxis, SERVO_AXIS_BLK *stpServoAxis_ACS)
{
	int iRet = MTN_API_OK_ZERO;
//	int iBlkId;
char strTemp[256];
	FILE *fptr;
	sprintf_s(strTemp, 256, "%s", stpServoAxis_ACS->strAxisServoParaFilename);
	fopen_s(&fptr, strTemp, "r"); // stpServoAxis_ACS->strAxisServoParaFilename, "r");

	if(fptr != NULL)
	{
		while(!feof(fptr))
		{
			fscanf(fptr, "%s", strTempReadFile); //  _s \n
			if(strcmp("[SAFETY_PARA]", strTempReadFile) == 0)
			{
				_mtnapi_read_axis_safety_para_acs_no_label(fptr, iAxis);
			}
			else if(strcmp("[BASIC_CONFIG]", strTempReadFile) == 0)
			{
				_mtnapi_read_axis_basic_para_para_acs_no_label(fptr, iAxis);
			}
			else if(strcmp("[SERVO_PARA_BLK-0]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_servo_para_acs_no_label(fptr, iAxis, 0);
			}		
			else if(strcmp("[SERVO_PARA_BLK-1]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_servo_para_acs_no_label(fptr, iAxis, 1);
			}		
			else if(strcmp("[SERVO_PARA_BLK-2]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_servo_para_acs_no_label(fptr, iAxis, 2);
			}		
			else if(strcmp("[SERVO_PARA_BLK-3]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_servo_para_acs_no_label(fptr, iAxis, 3);
			}		
			else if(strcmp("[SERVO_PARA_BLK-4]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_servo_para_acs_no_label(fptr, iAxis, 4);
			}		
			else if(strcmp("[SERVO_PARA_BLK-5]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_servo_para_acs_no_label(fptr, iAxis, 5);
			}		
			else if(strcmp("[SERVO_PARA_BLK-6]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_servo_para_acs_no_label(fptr, iAxis, 6);
			}		
			else if(strcmp("[SERVO_PARA_BLK-7]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_servo_para_acs_no_label(fptr, iAxis, 7);
			}		
			else if(strcmp("[SERVO_PARA_BLK-8]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_servo_para_acs_no_label(fptr, iAxis, 8);
			}		
			else if(strcmp("[SERVO_PARA_BLK-9]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_servo_para_acs_no_label(fptr, iAxis, 9);
			}		
			else if(strcmp("[SPEED_PROFILE_BLK-0]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_speed_para_acs_no_label(fptr, iAxis, 0);
			}		
			else if(strcmp("[SPEED_PROFILE_BLK-1]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_speed_para_acs_no_label(fptr, iAxis, 1);
			}		
			else if(strcmp("[SPEED_PROFILE_BLK-2]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_speed_para_acs_no_label(fptr, iAxis, 2);
			}		
			else if(strcmp("[SPEED_PROFILE_BLK-3]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_speed_para_acs_no_label(fptr, iAxis, 3);
			}		
			else if(strcmp("[SPEED_PROFILE_BLK-4]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_speed_para_acs_no_label(fptr, iAxis, 4);
			}		
			else if(strcmp("[SPEED_PROFILE_BLK-5]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_speed_para_acs_no_label(fptr, iAxis, 5);
			}		
			else if(strcmp("[SPEED_PROFILE_BLK-6]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_speed_para_acs_no_label(fptr, iAxis, 6);
			}		
			else if(strcmp("[SPEED_PROFILE_BLK-7]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_speed_para_acs_no_label(fptr, iAxis, 7);
			}		
			else if(strcmp("[SPEED_PROFILE_BLK-8]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_speed_para_acs_no_label(fptr, iAxis, 8);
			}		
			else if(strcmp("[SPEED_PROFILE_BLK-9]", strTempReadFile) == 0) 
			{
				_mtnapi_read_axis_speed_para_acs_no_label(fptr, iAxis, 9);
			}		

		}
		fclose(fptr);

	}
	else
	{
		iRet = MTN_API_ERROR_OPEN_FILE;
	}
	
	//for(int ii = 0; ii <MAX_BLK_PARAMETER; ii ++)               // // 20090508
	//{
	//	fprintf(fptr, "\n\n[SERVO_PARA_BLK-%d]\n", ii);
	//	fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dPositionLoopProportionalGain);  // SLPKP = 
	//	fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dVelocityLoopProportionalGain ); // SLVKP = 
	//	fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dVelocityLoopIntegratorGain );   // SLVKI = 
	//	fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dVelocityLoopIntegratorLimit );  // SLVLI = 
	//	fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dSecondOrderLowPassFilterBandwidth );  // SLVSOF = 
	//	fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dSecondOrderLowPassFilterDamping );    // SLVSOFD = 
	//	fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dNotchFilterFrequency );				// SLVNFRQ = 
	//	fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dNotchFilterWidth );					// SLVNWID = 
	//	fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dNotchFilterAttenuation );				// SLVNATT = 
	//	fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dAccelerationFeedforward );			// SLAFF = 
	//	fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dDynamicFrictionFeedforward );			// SLFRC = 
	//	//fprintf(fptr, "%d\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].iConstantDriveCommand );					// DCOM = 
	//	//fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dTorqueLimitIdle );					// XCURI = 
	//	//fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dTorqueLimitMoving );					// XCURV = 
	//	//fprintf(fptr, "%12.3f\n", stServoAxis_ACS[uiAxis].stServoParaACS[ii].dTorqueLimitRMS );						// XRMS = 
	//}   // // 20090508

    // [SPEED_PROFILE]
	//fprintf(fptr, "\n\n[SPEED_PROFILE]\n");

	// [POSITION_REGISTRATION]  // 20090129
	//fprintf(fptr, "\n\n[POSITION_REGISTRATION]\n");
	//fprintf(fptr, "%12.1f\n", stServoAxis_ACS[uiAxis].stServoPositionReg.dPositiveLimit);				// POSITIVE_LIMIT_POSN = 
	//fprintf(fptr, "%12.1f\n", stServoAxis_ACS[uiAxis].stServoPositionReg.dNegativeLimit);				// NEGATIVE_LIMIT_POSN = 
	//fprintf(fptr, "%12.1f\n", stServoAxis_ACS[uiAxis].stServoPositionReg.dRelaxPosition);				// RELAX_POSN = 

	return iRet;
}

int mtnapi_init_all_axis_parameter_from_file_no_label()
{
	int iRet = MTN_API_OK_ZERO;
	// Initialize each axis parameter
	for(unsigned int ii = 0; ii< stServoACS.uiTotalNumAxis; ii++)
	{
		iRet = mtnapi_init_axis_para_acs_no_label(ii, stServoACS.stpServoAxis_ACS[ii]);
		if(iRet != MTN_API_OK_ZERO)
		{
			break;
		}
	}
	return iRet;
}

int mtnapi_init_servo_parameter_from_file()
{
	int iRet = MTN_API_OK_ZERO;
	// Initialize each axis parameter
	for(unsigned int ii = 0; ii< stServoACS.uiTotalNumAxis; ii++)
	{
		iRet = mtnapi_init_servo_axis_para_acs(stServoACS.stpServoAxis_ACS[ii]);
		if(iRet != MTN_API_OK_ZERO)
		{
			break;
		}
	}
	return iRet;
}

int mtnapi_init_speed_parameter_from_file()
{
	int iRet = MTN_API_OK_ZERO;
	// Initialize each axis parameter
	for(unsigned int ii = 0; ii< stServoACS.uiTotalNumAxis; ii++)
	{
		iRet = mtnapi_init_speed_para_axis_acs(stServoACS.stpServoAxis_ACS[ii]);
		if(iRet != MTN_API_OK_ZERO)
		{
			break;
		}
	}
	return iRet;
}

// 20090129 
int mtnapi_init_position_reg_from_file()
{
	int iRet = MTN_API_OK_ZERO;
	// Initialize each axis parameter
	for(unsigned int ii = 0; ii< stServoACS.uiTotalNumAxis; ii++)
	{
		iRet = mtnapi_init_position_registration_axis_acs(stServoACS.stpServoAxis_ACS[ii]);
		if(iRet != MTN_API_OK_ZERO)
		{
			break;
		}
	}
	return iRet;
}

int mtnapi_upload_servo_parameter_acs(HANDLE Handle)
{
	int iRet = MTN_API_OK_ZERO;
	unsigned int ii;
	unsigned int uiAxisOnACS;

	for(ii = 0; ii<stServoACS.uiTotalNumAxis; ii++)
	{
		uiAxisOnACS = stServoACS.stpServoAxis_ACS[ii]->uiAxisOnACS;
		mtnapi_get_speed_profile(stServoControllerCommSet.Handle, uiAxisOnACS, 
				&stServoACS.stpServoAxis_ACS[ii]->stSpeedProfile[0], 0);

		iRet = mtnapi_upload_servo_parameter_acs_per_axis(stServoControllerCommSet.Handle, uiAxisOnACS, 
			&(stServoACS.stpServoAxis_ACS[ii]->stServoParaACS[DEF_BLK_UPLOAD_PARA_FROM_CTRL])); // 20090508
		if(iRet != MTN_API_OK_ZERO)
		{
			break;
		}
//
		iRet = mtnapi_upload_position_reg_acs_per_axis(stServoControllerCommSet.Handle, uiAxisOnACS, 
			&(stServoACS.stpServoAxis_ACS[ii]->stServoPositionReg)); // 20090129

		if(iRet != MTN_API_OK_ZERO)
		{
			break;
		}
//
		iRet = mtnapi_upload_safety_parameter_acs_per_axis(stServoControllerCommSet.Handle, uiAxisOnACS, 
			stServoACS.stpSafetyAxis_ACS[ii]); // 20090129

		if(iRet != MTN_API_OK_ZERO)
		{
			break;
		}
//
		iRet = mtnapi_upload_basic_parameter_acs_per_axis(stServoControllerCommSet.Handle, uiAxisOnACS, 
			stServoACS.stpBasicAxis_ACS[ii]); // 20090129

		if(iRet != MTN_API_OK_ZERO)
		{
			break;
		}

	}

	if(iRet == MTN_API_OK_ZERO)
	{
		iRet = mtnapi_upload_global_parameter_acs_per_axis(stServoControllerCommSet.Handle,  
			&(stServoACS.stGlobalParaACS)); // 20090129 // iRet = 
	}
	
	return iRet;
}

int mtnapi_upload_safety_parameter_acs_per_axis(HANDLE Handle, int iAxis, SAFETY_PARA_ACS *stpSafetyPara)
{
	double dTemp[8];
	int iRet= MTN_API_OK_ZERO;
//ERRI	= 1
	if(!acsc_ReadReal(Handle, ACSC_NONE, "ERRI", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's ERRI parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpSafetyPara->dPosnErrIdle = dTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "ERRV", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's ERRV parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpSafetyPara->dPosnErrVel = dTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "ERRA", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's ERRA parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpSafetyPara->dPosnErrAcc = dTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "CERRI", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's CERRI parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpSafetyPara->dCriticalPosnErrIdle = dTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "CERRV", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's CERRV parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpSafetyPara->dCriticalPosnErrVel = dTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "CERRA", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's CERRA parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpSafetyPara->dCriticalPosnErrAcc = dTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLLIMIT", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLLIMIT parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpSafetyPara->dSoftwarePosnLimitLow = dTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SRLIMIT", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SRLIMIT parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpSafetyPara->dSoftwarePosnLimitUpp = dTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "XVEL", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's XVEL parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpSafetyPara->dMaxVelX = dTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "XACC", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's XACC parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpSafetyPara->dMaxAccX = dTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "VELBRK", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's VELBRK parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpSafetyPara->dDynamicBrakeThresholdVel = dTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "XRMS", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's XRMS parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpSafetyPara->dRMS_DrvCmdX = dTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "XRMST", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's XRMST parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpSafetyPara->dRMS_TimeConst = dTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "XCURI", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's XCURI parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpSafetyPara->dRMS_DrvCmdIdle = dTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "XCURV", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's XCURV parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpSafetyPara->dRMS_DrvCmdMtn = dTemp[0];
	}

MTNAPI_UPLOAD_SAFETY_PARAMETER_ACS_RETURN:

	return iRet;

}

int mtnapi_download_safety_parameter_acs_per_axis(HANDLE Handle, int iAxis, SAFETY_PARA_ACS *stpSafetyPara)
{
	double dTemp[8];
	int iRet= MTN_API_OK_ZERO;
//ERRI	= 1
	dTemp[0] = stpSafetyPara->dPosnErrIdle;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "ERRI", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's ERRI parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
//
	dTemp[0] = stpSafetyPara->dPosnErrVel;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "ERRV", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's ERRV parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
//
	dTemp[0] = stpSafetyPara->dPosnErrAcc;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "ERRA", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's ERRA parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
//
	dTemp[0] = stpSafetyPara->dCriticalPosnErrIdle;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "CERRI", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's CERRI parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
//
	dTemp[0] = stpSafetyPara->dCriticalPosnErrVel;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "CERRV", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's CERRV parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
//
	dTemp[0] = stpSafetyPara->dCriticalPosnErrAcc;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "CERRA", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's CERRA parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
#ifdef __TBA__

//
	dTemp[0] = stpSafetyPara->dSoftwarePosnLimitLow;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLLIMIT", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLLIMIT parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
//
	dTemp[0] = stpSafetyPara->dSoftwarePosnLimitUpp;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SRLIMIT", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SRLIMIT parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
//
	dTemp[0] = stpSafetyPara->dMaxVelX;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "XVEL", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's XVEL parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
//
	dTemp[0] = stpSafetyPara->dMaxAccX;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "XACC", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's XACC parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
//
	dTemp[0] = stpSafetyPara->dDynamicBrakeThresholdVel ;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "VELBRK", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's VELBRK parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
#endif // __TBA__
//XCURI	= 13, torque limit during idle mode, for protection
	dTemp[0] = stpSafetyPara->dRMS_DrvCmdIdle;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "XCURI", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's XCURI parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}

//XCURV	= 14, torque limit
	dTemp[0] = stpSafetyPara->dRMS_DrvCmdMtn;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "XCURV", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's XCURV parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}

//XRMS	= 15, maximum current output RMS, also used in velocity loop test
	dTemp[0] = stpSafetyPara->dRMS_DrvCmdX;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "XRMS", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's XRMS parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}
//
	dTemp[0] = stpSafetyPara->dRMS_TimeConst;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "XRMST", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's XRMST parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SAFETY_PARAMETER_ACS_RETURN;
	}

MTNAPI_DOWNLOAD_SAFETY_PARAMETER_ACS_RETURN:

	return iRet;
}

int mtnapi_upload_basic_parameter_acs_per_axis(HANDLE Handle, int iAxis, BASIC_PARA_ACS_AXIS *stpBasicParaAxisACS)
{
	double dTemp[8];
	int iTemp[8];
	int iRet= MTN_API_OK_ZERO;
//MFLAGS	= 1
	if(!acsc_ReadInteger(Handle, ACSC_NONE, "MFLAGS", iAxis, iAxis, ACSC_NONE, ACSC_NONE, iTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's MFLAGS parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_BASIC_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpBasicParaAxisACS->iMotorFlags = iTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "EFAC", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's EFAC parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_BASIC_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpBasicParaAxisACS->dEncoderFactor = dTemp[0];
	}
//
	if(!acsc_ReadInteger(Handle, ACSC_NONE, "E_TYPE", iAxis, iAxis, ACSC_NONE, ACSC_NONE, iTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's E_TYPE parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_BASIC_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpBasicParaAxisACS->iEncoderType = iTemp[0];
	}
//
	if(!acsc_ReadInteger(Handle, ACSC_NONE, "E_FREQ", iAxis, iAxis, ACSC_NONE, ACSC_NONE, iTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's E_FREQ parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_BASIC_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpBasicParaAxisACS->iEncoderFreq = iTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "ENTIME", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's ENTIME parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_BASIC_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpBasicParaAxisACS->dEnableTime = dTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "BOFFTIME", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's BOFFTIME parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_BASIC_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpBasicParaAxisACS->dBrakeOffTime = dTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "BONTIME", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's BONTIME parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_BASIC_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpBasicParaAxisACS->dBrakeOnTime = dTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SETTLE", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SETTLE parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_BASIC_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpBasicParaAxisACS->dSettlingTime = dTemp[0];
	}
//
	if(!acsc_ReadReal(Handle, ACSC_NONE, "TARGRAD", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's TARGRAD parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_BASIC_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpBasicParaAxisACS->dTargetRadix = dTemp[0];
	}
//
	if(!acsc_ReadInteger(Handle, ACSC_NONE, "AFLAGS", iAxis, iAxis, ACSC_NONE, ACSC_NONE, iTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's AFLAGS parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_BASIC_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpBasicParaAxisACS->iAxisFlags = iTemp[0];
	}

MTNAPI_UPLOAD_BASIC_PARAMETER_ACS_RETURN:

	return iRet;
}

int mtnapi_upload_global_parameter_acs_per_axis(HANDLE Handle, BASIC_PARA_ACS_GLOBAL *stpProgramParaAxisACS)
{
	int iTemp[NUM_TOTAL_PROGRAM_BUFFER];
	int iRet= MTN_API_OK_ZERO;
	int ii;
//PFLAGS	= 1
	if(!acsc_ReadInteger(Handle, ACSC_NONE, "PFLAGS", 0, NUM_TOTAL_PROGRAM_BUFFER-1, ACSC_NONE, ACSC_NONE, iTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read PFLAGS  parameter\n",
			__FILE__, __LINE__);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_PROGRAM_PARAMETER_ACS_RETURN;
	}
	else
	{
		for(ii = 0; ii<NUM_TOTAL_PROGRAM_BUFFER; ii++)
		{
			stpProgramParaAxisACS->aiProgramFlags[ii] = iTemp[ii];
		}
	}
//
	if(!acsc_ReadInteger(Handle, ACSC_NONE, "PRATE", 0, NUM_TOTAL_PROGRAM_BUFFER-1, ACSC_NONE, ACSC_NONE, iTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read PRATE  parameter\n",
			__FILE__, __LINE__);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_PROGRAM_PARAMETER_ACS_RETURN;
	}
	else
	{
		for(ii = 0; ii<NUM_TOTAL_PROGRAM_BUFFER; ii++)
		{
			stpProgramParaAxisACS->aiProgramRate[ii] = iTemp[ii];
		}
	}
//
	if(!acsc_ReadInteger(Handle, ACSC_NONE, "ONRATE", 0, NUM_TOTAL_PROGRAM_BUFFER-1, ACSC_NONE, ACSC_NONE, iTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read ONRATE  parameter\n",
			__FILE__, __LINE__);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_PROGRAM_PARAMETER_ACS_RETURN;
	}
	else
	{
		for(ii = 0; ii<NUM_TOTAL_PROGRAM_BUFFER; ii++)
		{
			stpProgramParaAxisACS->aiProgramAutoRoutineRate[ii] = iTemp[ii];
		}
	}

MTNAPI_UPLOAD_PROGRAM_PARAMETER_ACS_RETURN:

	return iRet;
}

int mtnapi_get_acs_sp_id_str(int iAxis, int *piSP_Id, char strAxisSP[], int iStrLen)
{
	int iRet= MTN_API_OK_ZERO;
	switch(iAxis)
	{
	case 0:
		*piSP_Id = 0; 
		sprintf_s(strAxisSP, iStrLen, "X_");
		break;
	case 1:
		*piSP_Id = 1; 
		sprintf_s(strAxisSP, iStrLen, "X_");
		break;
	case 2:
		*piSP_Id = 2; 
		sprintf_s(strAxisSP, iStrLen, "X_");
		break;
	case 3:
		*piSP_Id = 3; 
		sprintf_s(strAxisSP, iStrLen, "X_");
		break;
	case 4:
		*piSP_Id = 0; 
		sprintf_s(strAxisSP, iStrLen, "A_");
		break;
	case 5:
		*piSP_Id = 1; 
		sprintf_s(strAxisSP, iStrLen, "A_");
		break;
	case 6:
		*piSP_Id = 2; 
		sprintf_s(strAxisSP, iStrLen, "A_");
		break;
	case 7:
		*piSP_Id = 3; 
		sprintf_s(strAxisSP, iStrLen, "A_");
		break;
	default:
		iRet = MTN_API_ERROR;
		break;
	}
	return iRet;
}

int mtnapi_download_acs_sp_parameter_k_x_from_a(HANDLE Handle, double dDecoupleK_x_From_a)
{
	int iRet= MTN_API_OK_ZERO;
	ACS_SP_VAR_FL stOutput24bFloat;

	acs_cvt_float_to_24b_int_gf(dDecoupleK_x_From_a, &stOutput24bFloat);
char strDownloadCmd[256];

//	int iSP_Id; char strAxisSP[16];

	if(iRet == MTN_API_OK_ZERO)
	{
		sprintf_s(strDownloadCmd, 256, "SETSPV(SP0:KxaGa,  %d)\n\r", stOutput24bFloat.iGain);
		if (!acsc_Send(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, NULL))
		{
			iRet = MTN_API_ERROR;
		}
	}

	if(iRet == MTN_API_OK_ZERO)
	{
		sprintf_s(strDownloadCmd, 256, "SETSPV(SP0:KxaGf,  %d)\n\r", stOutput24bFloat.iFactor);
		if (!acsc_Send(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, NULL))
		{
			iRet = MTN_API_ERROR;
		}
	}
	return iRet;
}

int mtnapi_download_acs_sp_parameter_k_a_from_x(HANDLE Handle, double dDecoupleK_a_From_x)
{
	int iRet= MTN_API_OK_ZERO;
	ACS_SP_VAR_FL stOutput24bFloat;

	acs_cvt_float_to_24b_int_gf(dDecoupleK_a_From_x, &stOutput24bFloat);
char strDownloadCmd[256];

//	int iSP_Id; char strAxisSP[16];

	if(iRet == MTN_API_OK_ZERO)
	{
		sprintf_s(strDownloadCmd, 256, "SETSPV(SP0:KaxGa,  %d)\n\r", stOutput24bFloat.iGain);
		if (!acsc_Send(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, NULL))
		{
			iRet = MTN_API_ERROR;
		}
	}

	if(iRet == MTN_API_OK_ZERO)
	{
		sprintf_s(strDownloadCmd, 256, "SETSPV(SP0:KaxGf,  %d)\n\r", stOutput24bFloat.iFactor);
		if (!acsc_Send(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, NULL))
		{
			iRet = MTN_API_ERROR;
		}
	}
	return iRet;
}

int mtnapi_download_acs_sp_parameter_jerk_ff(HANDLE Handle, int iAxis, double dJerkFf)
{
	int iRet= MTN_API_OK_ZERO;
	ACS_SP_VAR_FL stOutput24bFloat;

	acs_cvt_float_to_24b_int_gf(dJerkFf, &stOutput24bFloat);
char strDownloadCmd[256];

	int iSP_Id; char strAxisSP[16];

	iRet = mtnapi_get_acs_sp_id_str(iAxis, &iSP_Id, strAxisSP, 16);

	if(iRet == MTN_API_OK_ZERO)
	{
		sprintf_s(strDownloadCmd, 256, "SETSPV(SP%d:%sJerkFFGa,  %d)\n\r", iSP_Id, strAxisSP,
		stOutput24bFloat.iGain);
		if (!acsc_Send(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, NULL))
		{
			iRet = MTN_API_ERROR;
		}
	}

	if(iRet == MTN_API_OK_ZERO)
	{
		sprintf_s(strDownloadCmd, 256, "SETSPV(SP%d:%sJerkFFGf,  %d)\n\r", iSP_Id, strAxisSP,
			stOutput24bFloat.iFactor);
		if (!acsc_Send(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, NULL))
		{
			iRet = MTN_API_ERROR;
		}
	}
	return iRet;
}
static char *strStopString;

int mtnapi_upload_acs_sp_parameter_jerk_ff(HANDLE Handle, int iAxis, double *pdJerkFf)
{
	int iRet= MTN_API_OK_ZERO;
	ACS_SP_VAR_FL stOutput24bFloat;

char strDownloadCmd[256];
char strUploadMessage[256];
int iCountReceived;
int iTimeout = 50;
	int iSP_Id; char strAxisSP[16];
double dJerkFF_Ga, dJerkFF_Gf;

	iRet = mtnapi_get_acs_sp_id_str(iAxis, &iSP_Id, strAxisSP, 16);

	if(iRet == MTN_API_OK_ZERO)
	{
		sprintf_s(strDownloadCmd, 256, "?GETSPV(SP%d:%sJerkFFGa)\n\r", iSP_Id, strAxisSP); //,  %d
//		stOutput24bFloat.iGain);
		if(acsc_Transaction(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
		{
			if(iCountReceived >= 2)
			{
                strUploadMessage[iCountReceived] = '\0';
				dJerkFF_Ga = strtod(strUploadMessage, &strStopString); //sscanf(strUploadMessage, "%e", &dJerkFF_Ga);
			}
            else
            {
                dJerkFF_Gf = 0;
                iRet = MTN_API_ERROR;
            }
		}
		else
		{
			dJerkFF_Ga = 0;
            iRet = MTN_API_ERROR;
		}

	}

	if(iRet == MTN_API_OK_ZERO)
	{
		sprintf_s(strDownloadCmd, 256, "?GETSPV(SP%d:%sJerkFFGf)\n\r", iSP_Id, strAxisSP); //,  %d
//			stOutput24bFloat.iFactor);
		if(acsc_Transaction(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
		{
			if(iCountReceived >= 2)
			{
                strUploadMessage[iCountReceived] = '\0';
				dJerkFF_Gf = strtod(strUploadMessage, &strStopString); //sscanf(strUploadMessage, "%e", &dJerkFF_Gf);
			}
            else
            {
       			dJerkFF_Gf = 0;
                iRet = MTN_API_ERROR;
            }
		}
		else
		{
			dJerkFF_Gf = 0;
            iRet = MTN_API_ERROR;
		}
	}
//
	stOutput24bFloat.iFactor = (int)dJerkFF_Gf;
	stOutput24bFloat.iGain = (int)dJerkFF_Ga;
	acs_cvt_24b_int_gf_to_float(&stOutput24bFloat, pdJerkFf);
//	acs_cvt_float_to_24b_int_gf(dJerkFf, &stOutput24bFloat);

	return iRet;
}

int mtnapi_upload_acs_sp_parameter_decouple_x_from_a(HANDLE Handle, double *pdDecoupleX_from_A) // 
{
	int iRet= MTN_API_OK_ZERO;
	ACS_SP_VAR_FL stOutput24bFloat;

char strDownloadCmd[256];
char strUploadMessage[256];
int iCountReceived;
int iTimeout = 50;
//	int iSP_Id; char strAxisSP[16];
double dKxaGa, dKxaGf;

//	iRet = mtnapi_get_acs_sp_id_str(iAxis, &iSP_Id, strAxisSP, 16);

	if(iRet == MTN_API_OK_ZERO)
	{
		sprintf_s(strDownloadCmd, 256, "?GETSPV(SP0:KxaGa)\n\r"); //,  %d
//		stOutput24bFloat.iGain);
		if(acsc_Transaction(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
		{
			if(iCountReceived >= 2)
			{
                strUploadMessage[iCountReceived] = '\0';
				dKxaGa = strtod(strUploadMessage, &strStopString); //sscanf(strUploadMessage, "%e", &dKxaGa);
			}
            else
            {
                dKxaGf = 0;
                iRet = MTN_API_ERROR;
            }
		}
		else
		{
			dKxaGa = 0;
            iRet = MTN_API_ERROR;
		}

	}

	if(iRet == MTN_API_OK_ZERO)
	{
		sprintf_s(strDownloadCmd, 256, "?GETSPV(SP0:KxaGf)\n\r"); //,  %d
//			stOutput24bFloat.iFactor);
		if(acsc_Transaction(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
		{
			if(iCountReceived >= 2)
			{
                strUploadMessage[iCountReceived] = '\0';
				dKxaGf = strtod(strUploadMessage, &strStopString); //sscanf(strUploadMessage, "%e", &dKxaGf);
			}
            else
            {
       			dKxaGf = 0;
                iRet = MTN_API_ERROR;
            }
		}
		else
		{
			dKxaGf = 0;
            iRet = MTN_API_ERROR;
		}
	}
//
	stOutput24bFloat.iFactor = (int)dKxaGf;
	stOutput24bFloat.iGain = (int)dKxaGa;
	acs_cvt_24b_int_gf_to_float(&stOutput24bFloat, pdDecoupleX_from_A);
//	acs_cvt_float_to_24b_int_gf(dJerkFf, &stOutput24bFloat);

	return iRet;
}

int mtnapi_upload_acs_sp_parameter_decouple_a_from_x(HANDLE Handle, double *pdDecoupleA_from_X) // 
{
	int iRet= MTN_API_OK_ZERO;
	ACS_SP_VAR_FL stOutput24bFloat;

char strDownloadCmd[256];
char strUploadMessage[256];
int iCountReceived;
int iTimeout = 50;
//	int iSP_Id; char strAxisSP[16];
double dKaxGa, dKaxGf;

//	iRet = mtnapi_get_acs_sp_id_str(iAxis, &iSP_Id, strAxisSP, 16);

	if(iRet == MTN_API_OK_ZERO)
	{
		sprintf_s(strDownloadCmd, 256, "?GETSPV(SP0:KaxGa)\n\r"); //,  %d
//		stOutput24bFloat.iGain);
		if(acsc_Transaction(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
		{
			if(iCountReceived >= 2)
			{
                strUploadMessage[iCountReceived] = '\0';
				dKaxGa = strtod(strUploadMessage, &strStopString); //sscanf(strUploadMessage, "%e", &dKaxGa);
			}
            else
            {
                dKaxGf = 0;
                iRet = MTN_API_ERROR;
            }
		}
		else
		{
			dKaxGa = 0;
            iRet = MTN_API_ERROR;
		}

	}

	if(iRet == MTN_API_OK_ZERO)
	{
		sprintf_s(strDownloadCmd, 256, "?GETSPV(SP0:KaxGf)\n\r"); //,  %d
//			stOutput24bFloat.iFactor);
		if(acsc_Transaction(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
		{
			if(iCountReceived >= 2)
			{
                strUploadMessage[iCountReceived] = '\0';
				dKaxGf = strtod(strUploadMessage, &strStopString); //sscanf(strUploadMessage, "%e", &dKaxGf);
			}
            else
            {
       			dKaxGf = 0;
                iRet = MTN_API_ERROR;
            }
		}
		else
		{
			dKaxGf = 0;
            iRet = MTN_API_ERROR;
		}
	}
//
	stOutput24bFloat.iFactor = (int)dKaxGf;
	stOutput24bFloat.iGain = (int)dKaxGa;
	acs_cvt_24b_int_gf_to_float(&stOutput24bFloat, pdDecoupleA_from_X);
//	acs_cvt_float_to_24b_int_gf(dJerkFf, &stOutput24bFloat);

	return iRet;
}

int mtnapi_upload_servo_parameter_acs_v60_per_axis(HANDLE Handle, int iAxis, CTRL_PARA_ACS *stpServoPara)
{
	double dTemp[8];
	int iRet= MTN_API_OK_ZERO;
//SLPKP	= 1
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLPKP", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLPKP parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
	else
	{
		stpServoPara->dPositionLoopProportionalGain = dTemp[0];
	}

//SLVKP	= 2
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLVKP", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLVKP parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
	else
	{
		stpServoPara->dVelocityLoopProportionalGain = dTemp[0];
	}

//SLVKI	= 3
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLVKI", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLVKI parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
	else
	{
		stpServoPara->dVelocityLoopIntegratorGain = dTemp[0];
	}

//SLVLI	= 4
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLVLI", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLVLI parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
	else
	{
		stpServoPara->dVelocityLoopIntegratorLimit = dTemp[0];
	}

//SLVSOF	= 5
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLVSOF", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLVSOF parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
	else
	{
		stpServoPara->dSecondOrderLowPassFilterBandwidth = dTemp[0];
	}

//SLVSOFD	= 6
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLVSOFD", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLVSOFD parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
	else
	{
		stpServoPara->dSecondOrderLowPassFilterDamping = dTemp[0];
	}

//SLVNFRQ	= 7
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLVNFRQ", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLVNFRQ parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
	else
	{
		stpServoPara->dNotchFilterFrequency = dTemp[0];
	}

//SLVNWID	= 8
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLVNWID", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLVNWID parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
	else
	{
		stpServoPara->dNotchFilterWidth = dTemp[0];
	}

//SLVNATT	= 9
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLVNATT", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLVNATT parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
	else
	{
		stpServoPara->dNotchFilterAttenuation = dTemp[0];
	}

//SLAFF	= 10
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLAFF", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLAFF parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
	else
	{
		stpServoPara->dAccelerationFeedforward = dTemp[0];
	}

//SLFRC	= 11
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLFRC", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLFRC parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
	else
	{
		stpServoPara->dDynamicFrictionFeedforward = dTemp[0];
	}

//SLIOFFS, dCurrentLoopOffset, 20110802
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLIOFFS", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLIOFFS parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
	else
	{
		stpServoPara->dCurrentLoopOffset = dTemp[0];
	}

// IDC_EDIT_ACS_SLFRC
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLFRC", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLFRC parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
	else
	{
		stpServoPara->dVelocityFrictionPc = dTemp[0];
	}

// IDC_EDIT_ACS_SLFRCD
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLFRCD", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLFRCD parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
	else
	{
		stpServoPara->dVelocityFrictionMax = dTemp[0];
	}

// IDC_EDIT_ACS_SLPKI
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLPKI", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLPKI parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
	else
	{
		stpServoPara->dPositionIntegralGain = dTemp[0];
	}

// IDC_EDIT_ACS_SLPLI
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLPLI", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLPLI parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
	else
	{
		stpServoPara->dPositionIntegralLimit = dTemp[0];
	}

MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_V60_RETURN:
	return iRet;
}

int mtnapi_upload_servo_parameter_acs_per_axis(HANDLE Handle, int iAxis, CTRL_PARA_ACS *stpServoPara)
{
	double dTemp[8];
	int iRet= MTN_API_OK_ZERO;

	if(mtnapi_upload_servo_parameter_acs_v60_per_axis(Handle, iAxis, stpServoPara) != MTN_API_OK_ZERO)
	{
		iRet = MTN_API_ERROR_UPLOAD_DATA;
//		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_RETURN;
	}

	double dJerkFF;
	if(mtnapi_upload_acs_sp_parameter_jerk_ff(Handle, iAxis, &dJerkFF) == MTN_API_OK_ZERO)
	{
		stpServoPara->dJerkFf = dJerkFF;
	}

	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLDRA", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's DRA parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpServoPara->dDRA = dTemp[0];
	}

	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLDRX", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's DRA parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpServoPara->dDRX = dTemp[0];
	}

	// 20120921
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLVB0NF", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLVB0NF parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpServoPara->dBiquadNumFreq = dTemp[0];
	}

	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLVB0DF", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLVB0DF parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpServoPara->dBiquadDenFreq = dTemp[0];
	}

	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLVB0ND", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLVB0ND parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpServoPara->dBiquadNumDamp = dTemp[0];
	}

	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLVB0DD", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLVB0DD parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_RETURN;
	}
	else
	{
		stpServoPara->dBiquadDenDamp = dTemp[0];
	}

MTNAPI_UPLOAD_SERVO_PARAMETER_ACS_RETURN:

	return iRet;
}

int mtnapi_download_servo_parameter_acs_v60_per_axis(HANDLE Handle, int iAxis, CTRL_PARA_ACS *stpServoPara)
{
	double dTemp[8];
	int iRet= MTN_API_OK_ZERO;
//SLPKP	= 1
	dTemp[0] = stpServoPara->dPositionLoopProportionalGain;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLPKP", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLPKP parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}

//SLVKP	= 2
	dTemp[0] = stpServoPara->dVelocityLoopProportionalGain;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLVKP", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLVKP parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}

//SLVKI	= 3
	dTemp[0] = stpServoPara->dVelocityLoopIntegratorGain;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLVKI", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLVKI parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}

//SLVLI	= 4
	dTemp[0] = stpServoPara->dVelocityLoopIntegratorLimit;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLVLI", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLVLI parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
	
//SLAFF	= 10, acceleration feed forward compensation
	dTemp[0] = stpServoPara->dAccelerationFeedforward;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLAFF", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLAFF parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}

//SLIOFFS, dCurrentLoopOffset, 20110802
	dTemp[0] = stpServoPara->dCurrentLoopOffset;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLIOFFS", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLIOFFS parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
// IDC_EDIT_ACS_SLFRC
	dTemp[0] = stpServoPara->dVelocityFrictionPc;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLFRC", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLFRC parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
// IDC_EDIT_ACS_SLFRCD
	dTemp[0] = stpServoPara->dVelocityFrictionMax;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLFRCD", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLFRCD parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
#ifdef __ACS_SLP__
// IDC_EDIT_ACS_SLPKI
	dTemp[0] = stpServoPara->dPositionIntegralGain;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLPKI", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLPKI parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}

// IDC_EDIT_ACS_SLPLI
	dTemp[0] = stpServoPara->dPositionIntegralLimit;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLPLI", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLPLI parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_V60_RETURN;
	}
#endif  // __ACS_SLP__

MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_V60_RETURN:

	return iRet;
}

// 20090129
int mtnapi_download_servo_parameter_acs_per_axis(HANDLE Handle, int iAxis, CTRL_PARA_ACS *stpServoPara)
{
	double dTemp[8];
	int iRet= MTN_API_OK_ZERO;

	if(mtnapi_download_servo_parameter_acs_v60_per_axis(Handle, iAxis, stpServoPara) != MTN_API_OK_ZERO)
	{
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_RETURN;
	}
// JerkFF, 20110515
	mtnapi_download_acs_sp_parameter_jerk_ff(Handle, iAxis, stpServoPara->dJerkFf);

//DRA
	dTemp[0] = stpServoPara->dDRA;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLDRA", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLDRA parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_RETURN;
	}

//DRX
	dTemp[0] = stpServoPara->dDRX;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLDRX", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLDRX parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_RETURN;
	}


//SLFRC	= 11, friction feed forward
	dTemp[0] = stpServoPara->dDynamicFrictionFeedforward;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLFRC", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLFRC parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_RETURN;
	}

//SLVSOF	= 5
	dTemp[0] = stpServoPara->dSecondOrderLowPassFilterBandwidth;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLVSOF", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLVSOF parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_RETURN;
	}

//SLVSOFD	= 6
	dTemp[0] = stpServoPara->dSecondOrderLowPassFilterDamping;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLVSOFD", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLVSOFD parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_RETURN;
	}
//SLVNFRQ	= 7, Notch filter frequency
	dTemp[0] = stpServoPara->dNotchFilterFrequency;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLVNFRQ", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLVNFRQ parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_RETURN;
	}
	

//SLVNWID	= 8, Notch filter Freq Width
	dTemp[0] = stpServoPara->dNotchFilterWidth;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLVNWID", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLVNWID parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_RETURN;
	}
	

//SLVNATT	= 9, Notch filter attenuation
	dTemp[0] = stpServoPara->dNotchFilterAttenuation;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLVNATT", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLVNATT parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_RETURN;
	}

// Biquad, // 20120921
	dTemp[0] = stpServoPara->dBiquadDenDamp;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLVB0DD", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLVB0DD parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_RETURN;
	}

	dTemp[0] = stpServoPara->dBiquadDenFreq;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLVB0DF", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLVB0DF parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_RETURN;
	}

	dTemp[0] = stpServoPara->dBiquadNumDamp;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLVB0ND", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLVB0ND parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_RETURN;
	}

	dTemp[0] = stpServoPara->dBiquadNumFreq;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLVB0NF", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLVB0NF parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_RETURN;
	}
MTNAPI_DOWNLOAD_SERVO_PARAMETER_ACS_RETURN:

	return iRet;
}

int mtnapi_download_parameter_acs_axis(HANDLE stHandle, int iAxis, SERVO_AXIS_BLK *stpServoAxisBlk)
{
//	double dTemp[8];
	int iRet= MTN_API_OK_ZERO;

	iRet= mtnapi_download_servo_parameter_acs_per_axis(stHandle, iAxis, &(stpServoAxisBlk->stServoParaACS[DEF_BLK_DOWNLOAD_PARA_TO_CTRL])); 
	if(iRet == MTN_API_OK_ZERO)
	{
		iRet = mtnapi_set_speed_profile(stHandle, iAxis, &(stpServoAxisBlk->stSpeedProfile[0]), 0);
		iRet = mtnapi_download_position_reg_acs_per_axis(stHandle, iAxis, &(stpServoAxisBlk->stServoPositionReg));
	}
	
	return iRet;
}

int mtnapi_download_servo_all_axis_acs()
{
	int iRet = MTN_API_OK_ZERO;
	for(unsigned int ii = 0; ii<stServoACS.uiTotalNumAxis; ii++)
	{
		iRet = mtnapi_download_parameter_acs_axis(stServoACS.Handle, stServoACS.stpServoAxis_ACS[ii]->uiAxisOnACS, (stServoACS.stpServoAxis_ACS[ii])); 

		if(iRet != MTN_API_OK_ZERO)
		{
			break;
		}
	}
	return iRet;
}

#include "MtnWbDef.h"
#include "acs_buff_prog.h"

void mtn_b1w_write_para_bh_servo(HANDLE stCommHandle, MTN_TUNE_PARAMETER_SET *stpPara, int iObjSectionFlagB1W)
{
	double dTempProfile[4];
	dTempProfile[0] = stpPara->dVelLoopKP; //stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dVelocityLoopProportionalGain
	dTempProfile[1] = stpPara->dVelLoopKI; //stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dVelocityLoopIntegratorGain =
	dTempProfile[2] = stpPara->dPosnLoopKP; //stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dPositionLoopProportionalGain =
	dTempProfile[3] = stpPara->dAccFFC; //stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dAccelerationFeedforward = ;  

		if(!acsc_WriteReal(stCommHandle, ACSC_NONE, "arBHCtrl", iObjSectionFlagB1W, iObjSectionFlagB1W, 0, 3,  dTempProfile, NULL))// 20130215
		{
		}
	switch(iObjSectionFlagB1W)
	{
	case WB_BH_IDLE:
	case WB_BH_SRCH_HT:
	case WB_BH_LOOPING:
	case WB_BH_LOOP_TOP:
	case WB_BH_TRAJECTORY:
	case WB_BH_TAIL:
	case WB_BH_RESET:
	case WB_BH_1ST_CONTACT:
	case WB_BH_2ND_CONTACT:
		acsc_WriteReal(stCommHandle, ACSC_NONE, "arACtrl", iObjSectionFlagB1W, iObjSectionFlagB1W, 0, 3,  dTempProfile, NULL);	  // Global Variable, 20100726
		acsc_WriteReal(stCommHandle, BUFFER_ID_SEARCH_CONTACT, "arACtrl", iObjSectionFlagB1W, iObjSectionFlagB1W, 0, 3,  dTempProfile, NULL);	  // Global Variable, 20100726
		break;
	}
}

void mtn_b1w_read_para_bh_servo(HANDLE stCommHandle, MTN_TUNE_PARAMETER_SET *stpPara, int iObjSectionFlagB1W)
{
	double dTempProfile[4];

		if(!acsc_ReadReal(stCommHandle, ACSC_NONE, "arBHCtrl", iObjSectionFlagB1W, iObjSectionFlagB1W, 0, 3,  dTempProfile, NULL))  // ; // 20130215
		{
			// TBD, Legacy
			if(iObjSectionFlagB1W > WB_BH_2ND_CONTACT) iObjSectionFlagB1W = WB_BH_2ND_CONTACT;
			acsc_ReadReal(stCommHandle, ACSC_NONE, "arACtrl", iObjSectionFlagB1W, iObjSectionFlagB1W, 0, 3,  dTempProfile, NULL);	  // Global Variable, 20100726
		}
	//switch(iObjSectionFlagB1W)
	//{
	//case WB_BH_IDLE:
	//case WB_BH_SRCH_HT:
	//case WB_BH_LOOPING:
	//case WB_BH_LOOP_TOP:
	//case WB_BH_TRAJECTORY:
	//case WB_BH_TAIL:
	//case WB_BH_RESET:
	//case WB_BH_1ST_CONTACT:
	//case WB_BH_2ND_CONTACT:
	//	break;
	//}
	stpPara->dVelLoopKP = dTempProfile[0]; //stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dVelocityLoopProportionalGain
	stpPara->dVelLoopKI = dTempProfile[1]; //stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dVelocityLoopIntegratorGain =
	stpPara->dPosnLoopKP = dTempProfile[2]; //stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dPositionLoopProportionalGain =
	stpPara->dAccFFC = dTempProfile[3]; //stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dAccelerationFeedforward = ;  
}

int mtn_b1w_write_para_bh_servo_readve(HANDLE stCommHandle, MTN_TUNE_PARAMETER_SET *stpPara, int iObjSectionFlagB1W)
{
	int iRet = MTN_API_OK_ZERO, iCount =0;
	MTN_TUNE_PARAMETER_SET stWriteVar, stReadVerify;
	stWriteVar = *stpPara;
	stReadVerify = stWriteVar;
	mtn_b1w_write_para_bh_servo(stCommHandle, &stWriteVar, iObjSectionFlagB1W);
	if(stWriteVar.dAccFFC <=0 ||
		stWriteVar.dPosnLoopKP <= 0 ||
		stWriteVar.dVelLoopKI <= 0 ||
		stWriteVar.dVelLoopKP <= 0)
	{
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		CString cstrTemp; cstrTemp.Format(_T("Error, Invalid servo parameter: %4f, %4f, %4f, %4f"), stWriteVar.dVelLoopKP, stWriteVar.dVelLoopKI, stWriteVar.dPosnLoopKP, stWriteVar.dAccFFC);
		AfxMessageBox(cstrTemp);
		return iRet;
	}


	//stReadVerify.dSecOrdFilterFreq_Hz = stWriteVar.dSecOrdFilterFreq_Hz;
	//stReadVerify.dVelLoopLimitI = stWriteVar.dVelLoopLimitI;
	mtn_b1w_read_para_bh_servo(stCommHandle, &stReadVerify, iObjSectionFlagB1W);
	while( memcmp(&stWriteVar, &stReadVerify, sizeof(MTN_TUNE_PARAMETER_SET)) != 0) // if not equal
	{
		mtn_b1w_write_para_bh_servo(stCommHandle, &stWriteVar, iObjSectionFlagB1W);
		Sleep(5);
		mtn_b1w_read_para_bh_servo(stCommHandle, &stReadVerify, iObjSectionFlagB1W);
		iCount ++;
		if(iCount >= 3)
		{
			iRet = MTN_API_ERROR_DOWNLOAD_DATA;
			break;
		}
	}
	return iRet;
}

int mtnapi_download_servo_control_para_all_axis_acs()
{
	int iRet = MTN_API_OK_ZERO;
	for(unsigned int ii = 0; ii<stServoACS.uiTotalNumAxis; ii++)
	{
		iRet = mtnapi_download_servo_parameter_acs_per_axis(stServoACS.Handle, 
			stServoACS.stpServoAxis_ACS[ii]->uiAxisOnACS, 
			&(stServoACS.stpServoAxis_ACS[ii]->stServoParaACS[DEF_BLK_DOWNLOAD_PARA_TO_CTRL])); // 20090508

		if(iRet != MTN_API_OK_ZERO)
		{
			break;
		}
		iRet = mtnapi_download_safety_parameter_acs_per_axis(stServoACS.Handle, 
			stServoACS.stpServoAxis_ACS[ii]->uiAxisOnACS, stServoACS.stpSafetyAxis_ACS[ii]);
		if(iRet != MTN_API_OK_ZERO)
		{
			break;
		}

	}

// MTN_TUNE_PARAMETER_SET stTempTunePara;

	return iRet;
}

int mtnapi_upload_position_reg_acs_per_axis(HANDLE Handle, int iAxis, MTN_POSITION_REGISTRATION *stpRegPosition)
{
	double dTemp[8];
	int iRet= MTN_API_OK_ZERO;

//SLLIMIT = -
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SLLIMIT", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SLLIMIT parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_POSN_REG_ACS_RETURN;
	}
	else
	{
		stpRegPosition->dNegativeLimit= dTemp[0];
	}

//SRLIMIT = -
	if(!acsc_ReadReal(Handle, ACSC_NONE, "SRLIMIT", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Read Axis-%d's SRLIMIT parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_UPLOAD_DATA;
		goto MTNAPI_UPLOAD_POSN_REG_ACS_RETURN;
	}
	else
	{
		stpRegPosition->dPositiveLimit= dTemp[0];
	}

MTNAPI_UPLOAD_POSN_REG_ACS_RETURN:

	return iRet;
}

int mtnapi_download_position_reg_acs_per_axis(HANDLE Handle, int iAxis, MTN_POSITION_REGISTRATION *stpRegPosition)
{
	double dTemp[8];
	int iRet= MTN_API_OK_ZERO;

//SLLIMIT = -
	dTemp[0] = stpRegPosition->dNegativeLimit;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SLLIMIT", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SLLIMIT parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_POSN_REG_ACS_RETURN;
	}

//SRLIMIT = -
	dTemp[0] = stpRegPosition->dPositiveLimit;
	if(!acsc_WriteReal(Handle, ACSC_NONE, "SRLIMIT", iAxis, iAxis, ACSC_NONE, ACSC_NONE, dTemp, NULL))
	{ // error
		sprintf_s(strDebugMessage, 512, "Error! [%s %d]: Write Axis-%d's SRLIMIT parameter\n",
			__FILE__, __LINE__, iAxis);
		iRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto MTNAPI_DOWNLOAD_POSN_REG_ACS_RETURN;
	}

MTNAPI_DOWNLOAD_POSN_REG_ACS_RETURN:
	return iRet;
}

int mtn_api_get_relax_position_by_acs_axis(int iAxis, double *RelaxPosn)
{
	int iRet = MTN_API_ERR_INVALID_AXIS;

	for(unsigned int ii = 0; ii <stServoACS.uiTotalNumAxis; ii ++)
	{
		if(stServoACS.stpServoAxis_ACS[ii]->uiAxisOnACS == iAxis)
		{
			(*RelaxPosn) = stServoACS.stpServoAxis_ACS[ii]->stServoPositionReg.dRelaxPosition;
			iRet = MTN_API_OK_ZERO;
		}
	}

	return iRet;
}

void mtn_api_servo_off_all(HANDLE stCommHandle)
{
	int iMotorState, iAxisOnAcs;

	for(unsigned int ii=0; ii<stServoACS.uiTotalNumAxis; ii++)
	{
		iAxisOnAcs = stServoACS.stpServoAxis_ACS[ii]->uiAxisOnACS;
		mtnapi_get_motor_state(stCommHandle, 
				iAxisOnAcs, &iMotorState, 0);
		if(iMotorState & ACSC_MST_ENABLE) 
		{
			mtnapi_disable_motor(stCommHandle, iAxisOnAcs, 0);
		}
	}
}

// 
void sys_set_comm_default()
{
	stServoControllerCommSet.CommType = 0;
	stServoControllerCommSet.Handle = ACSC_INVALID; // (HANDLE)-1; // invalid communication channel, ACSC_INVALID;
	stServoControllerCommSet.ConnectionString = _T("NO");
	stServoControllerCommSet.SerialPort = 0;					//COM1
	stServoControllerCommSet.SerialRate = 7;					//115200
	stServoControllerCommSet.EthernetAddress = "10.0.0.100"; //default address
	stServoControllerCommSet.EthernetConnection = 1;			//Internet/Intranet
}
//#define __PROMPT_CONNECTION__
// Initialization with auto-detection for virtural
int sys_init_acs_communication()
{
	int iRet = MTN_API_ERROR_COMMUNICATION_WITH_CONTROLLER; // Initialize to be no communication
		// get all installed SPiiPlus PCI cards
	if (acsc_GetPCICards(gstSysPCICards, SYS_MAX_ACS_CARDS_PCI, &giFlagObtainedCards))
	{
		if (giFlagObtainedCards > 0)
		{
				theAcsServo.Initialization(ONLINE_MODE);
				stServoControllerCommSet.Handle = theAcsServo.GetServoHandle(); // From ZhangDong's Lib
//				stServoControllerCommSet.Handle = acsc_OpenCommPCI(gstSysPCICards[ giFlagObtainedCards - 1].SlotNumber);

				double flTimeClock_ms;
				ACSC_WAITBLOCK stWaitBlk;  // WriteReal from1 = 0, To1 = 0, write 1 variable, 20081118
				if (!acsc_ReadReal( stServoControllerCommSet.Handle, ACSC_NONE, "CTIME", 0, 0, ACSC_NONE, ACSC_NONE, &flTimeClock_ms, &stWaitBlk))
				sys_init_controller_ts(flTimeClock_ms);

				iRet = MTN_API_OK_ZERO;
		}
	}
	else if(AfxMessageBox(_T("Get connected with virtual controller"), MB_YESNO) == IDYES)
	{
		theAcsServo.Initialization(OFFLINE_MODE);
		stServoControllerCommSet.Handle = theAcsServo.GetServoHandle();
		if(stServoControllerCommSet.Handle != ACSC_INVALID)
		{
			iRet = MTN_API_OK_ZERO;
		}
	}
	return iRet;
}
// 20090129  set relax position
int sys_set_servo_relax_position(int iServoAxis, double dRelaxPosition)
{
	int iRet = MTN_API_OK_ZERO;

	if(iServoAxis >0 && ((unsigned) iServoAxis < stServoACS.uiTotalNumAxis))
	{
		stServoACS.stpServoAxis_ACS[iServoAxis]->stServoPositionReg.dRelaxPosition = dRelaxPosition;
	}
	else
	{
		iRet = MTN_API_ERR_INVALID_AXIS;
	}
	return iRet;
}

int mtnapi_download_acs_sp_parameter_damp_switch(HANDLE Handle, int iDampGA)
{
int iRet= MTN_API_OK_ZERO;
char strDownloadCmd[256];

		sprintf_s(strDownloadCmd, 256, "SETSPV(SP0:DampGA, %d)\n\r",iDampGA);
		if (!acsc_Send(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, NULL))
		{
			iRet = MTN_API_ERROR;
		}

	return iRet;
}

int mtnapi_upload_acs_sp_parameter_damp_switch(HANDLE Handle, int *iDampGA) 
{
	int iRet= MTN_API_OK_ZERO;
//	ACS_SP_VAR_FL stOutput24bFloat;

char strDownloadCmd[256];
char strUploadMessage[256];
int iCountReceived;
int iTimeout = 50;

	if(iRet == MTN_API_OK_ZERO)
	{
		sprintf_s(strDownloadCmd, 256, "?GETSPV(SP0:DampGA)\n\r"); //,  %d
//		stOutput24bFloat.iGain);
		if(acsc_Transaction(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
		{
			if(iCountReceived >= 2)
			{
                strUploadMessage[iCountReceived] = '\0';
				*iDampGA = (int)strtod(strUploadMessage, &strStopString); //sscanf(strUploadMessage, "%e", &dKxaGa);
			}
            else
            {
                *iDampGA = 0;
                iRet = MTN_API_ERROR;
            }
		}
		else
		{
			*iDampGA = 0;
            iRet = MTN_API_ERROR;
		}
	}

	return iRet;
}