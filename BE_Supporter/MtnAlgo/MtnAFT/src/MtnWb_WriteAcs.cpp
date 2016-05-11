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


#include "stdafx.h"
#include "MtnInitAcs.h"
#include "mtnconfg.h"
#include <sys\stat.h>

#include "MtnWbDef.h"
#include "MotAlgo_DLL.h"

int mtn_wb_acs_download_bh_z_servo_ctrl(HANDLE hCommunicationHandle, unsigned int uiBlk)
{
int iRet = MTN_API_OK_ZERO;
double dTempProfile[4];
MTN_TUNE_PARAMETER_SET stTuneParaSet;

	// Only in running mode with BL-DSP
//	mtn_api_get_spring_compensation_sp_para(hCommunicationHandle, &(stServoAxis_ACS[WB_AXIS_BOND_Z].stPosnCompensationWbBH));

	if(uiBlk < NUM_BONDHEAD_SERVO_CTRL_BLK && uiBlk < MAX_BLK_PARAMETER)
	{
				dTempProfile[0] = stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dVelocityLoopProportionalGain; 
				dTempProfile[1] = stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dVelocityLoopIntegratorGain;   
				dTempProfile[2] = stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dPositionLoopProportionalGain; 
				dTempProfile[3] = stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dAccelerationFeedforward;  

		// write  default from ACS variables
		if(uiBlk == 0 && 
			dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1)
		{
			mtnapi_download_servo_parameter_acs_per_axis(hCommunicationHandle, sys_get_acs_axis_id_bnd_z(), 
				&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk]);
		}

		// Read from WB application
		if(dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1) // protection
		{
			if(acsc_WriteReal(hCommunicationHandle, ACSC_NONE, "arACtrl", uiBlk, uiBlk, 0, 3,  dTempProfile, NULL))  // ;	  // Global Variable, 20100726
			{
			}
		}
		stTuneParaSet.dAccFFC = stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dAccelerationFeedforward;
		stTuneParaSet.dVelLoopKP = stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dVelocityLoopProportionalGain;
		stTuneParaSet.dVelLoopKI = stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dVelocityLoopIntegratorGain;
		stTuneParaSet.dPosnLoopKP = stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[uiBlk].dPositionLoopProportionalGain;
		mtn_b1w_write_para_bh_servo(hCommunicationHandle, &stTuneParaSet, uiBlk); //20130422
	}
	else
	{
		iRet = MTN_API_ERROR_EXCEED_MAX_BLK;
	}

	return iRet;
}

int mtn_wb_acs_download_tbl_x_servo_ctrl(HANDLE hCommunicationHandle, unsigned int uiBlk)
{
	int iRet = MTN_API_OK_ZERO;
	double dTempProfile[4];
	if(uiBlk < NUM_TABLE_X_SERVO_CTRL_BLK && uiBlk < MAX_BLK_PARAMETER)
	{
		dTempProfile[0]	= stServoAxis_ACS[WB_AXIS_TABLE_X].stServoParaACS[uiBlk].dVelocityLoopProportionalGain; 
		dTempProfile[1] = stServoAxis_ACS[WB_AXIS_TABLE_X].stServoParaACS[uiBlk].dVelocityLoopIntegratorGain;   
		dTempProfile[2]	= stServoAxis_ACS[WB_AXIS_TABLE_X].stServoParaACS[uiBlk].dPositionLoopProportionalGain;  
		dTempProfile[3]	= stServoAxis_ACS[WB_AXIS_TABLE_X].stServoParaACS[uiBlk].dAccelerationFeedforward;  

		// Load default from ACS variables
		if(uiBlk == 0 && 
			dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1)
		{
			mtnapi_download_servo_parameter_acs_per_axis(hCommunicationHandle, stServoAxis_ACS[WB_AXIS_TABLE_X].uiAxisOnACS,//WB_AXIS_TABLE_X, // 20120801
				&stServoAxis_ACS[WB_AXIS_TABLE_X].stServoParaACS[uiBlk]);
		}
		// Read from WB application
		if(dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1) // protection
		{
			acsc_WriteReal(hCommunicationHandle, ACSC_NONE, "arXCtrl", uiBlk, uiBlk, 0, 3,  dTempProfile, NULL);
		}
	}
	else
	{
		iRet = MTN_API_ERROR_EXCEED_MAX_BLK;
	}

	return iRet;
}


int mtn_wb_acs_download_tbl_y_servo_ctrl(HANDLE hCommunicationHandle, unsigned int uiBlk)
{
	int iRet = MTN_API_OK_ZERO;
	double dTempProfile[4];
	if(uiBlk < NUM_TABLE_Y_SERVO_CTRL_BLK && uiBlk < MAX_BLK_PARAMETER)
	{
		dTempProfile[0] = stServoAxis_ACS[WB_AXIS_TABLE_Y].stServoParaACS[uiBlk].dVelocityLoopProportionalGain; // astWbBondHeadServoCtrlPara[uiBlk]
		dTempProfile[1] = stServoAxis_ACS[WB_AXIS_TABLE_Y].stServoParaACS[uiBlk].dVelocityLoopIntegratorGain;   // astWbBondHeadServoCtrlPara[uiBlk]
		dTempProfile[2] = stServoAxis_ACS[WB_AXIS_TABLE_Y].stServoParaACS[uiBlk].dPositionLoopProportionalGain;  // astWbBondHeadServoCtrlPara[uiBlk]
		dTempProfile[3] = stServoAxis_ACS[WB_AXIS_TABLE_Y].stServoParaACS[uiBlk].dAccelerationFeedforward;  // astWbBondHeadServoCtrlPara[uiBlk]

		// Load default from ACS variables
		if(uiBlk == 0 && 
			dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1)
		{
			mtnapi_download_servo_parameter_acs_per_axis(hCommunicationHandle, stServoAxis_ACS[WB_AXIS_TABLE_Y].uiAxisOnACS, // 20120801
				&stServoAxis_ACS[WB_AXIS_TABLE_Y].stServoParaACS[uiBlk]);
		}
		// Read from WB application
		if(dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1) // protection
		{
			acsc_WriteReal(hCommunicationHandle, ACSC_NONE, "arYCtrl", uiBlk, uiBlk, 0, 3,  dTempProfile, NULL);  //;	  // Global Variable, 
		}
	}
	else
	{
		iRet = MTN_API_ERROR_EXCEED_MAX_BLK;
	}

	return iRet;
}

int mtn_wb_acs_download_servo_ctrl(HANDLE hCommunicationHandle)
{
	int iRet = MTN_API_OK_ZERO;
//	double dTempProfile[4];
	int ii;
	for(ii = 0; ii<NUM_BONDHEAD_SERVO_CTRL_BLK; ii++)
	{
		iRet = mtn_wb_acs_download_bh_z_servo_ctrl(hCommunicationHandle, ii);

		if(iRet != MTN_API_OK_ZERO) break;
	}

	if(iRet != MTN_API_OK_ZERO) 
	{
	}
	else
	{
		for(ii = 0; ii<NUM_TABLE_X_SERVO_CTRL_BLK; ii++)
		{
			iRet = mtn_wb_acs_download_tbl_x_servo_ctrl(hCommunicationHandle, ii);
			if(iRet != MTN_API_OK_ZERO) break;
		}
	}

	if(iRet != MTN_API_OK_ZERO) 
	{
	}
	else
	{
		for(ii = 0; ii<NUM_TABLE_Y_SERVO_CTRL_BLK; ii++)
		{
			iRet = mtn_wb_acs_download_tbl_y_servo_ctrl(hCommunicationHandle, ii);
			if(iRet != MTN_API_OK_ZERO) break;
		}
	}

	return iRet;
}
///////////////////// Download Servo with structure input
int mtn_wb_acs_download_tbl_x_servo_ctrl_struct(HANDLE hCommunicationHandle, unsigned int uiBlk, 
												CTRL_PARA_ACS *stpServoParaACS)
{
	int iRet = MTN_API_OK_ZERO;
	double dTempProfile[4];
	if(uiBlk < NUM_TABLE_X_SERVO_CTRL_BLK && uiBlk < MAX_BLK_PARAMETER)
	{
		dTempProfile[0]	= stpServoParaACS->dVelocityLoopProportionalGain; 
		dTempProfile[1] = stpServoParaACS->dVelocityLoopIntegratorGain;   
		dTempProfile[2]	= stpServoParaACS->dPositionLoopProportionalGain;  
		dTempProfile[3]	= stpServoParaACS->dAccelerationFeedforward;  

		// Load default from ACS variables
		if(uiBlk == 0 && 
			dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1)
		{
			mtnapi_download_servo_parameter_acs_per_axis(hCommunicationHandle, 
				stServoAxis_ACS[WB_AXIS_TABLE_X].uiAxisOnACS, // 20130127, BugFix, WB_AXIS_TABLE_X, 
				stpServoParaACS);
		}
		// Read from WB application
		if(dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1) // protection
		{
			acsc_WriteReal(hCommunicationHandle, ACSC_NONE, "arXCtrl", uiBlk, uiBlk, 0, 3,  dTempProfile, NULL);
		}
	}
	else
	{
		iRet = MTN_API_ERROR_EXCEED_MAX_BLK;
	}

	return iRet;
}

int mtn_wb_acs_download_tbl_y_servo_ctrl_struct(HANDLE hCommunicationHandle, unsigned int uiBlk,
										 CTRL_PARA_ACS *stpServoParaACS)
{
	int iRet = MTN_API_OK_ZERO;
	double dTempProfile[4];
	if(uiBlk < NUM_TABLE_Y_SERVO_CTRL_BLK && uiBlk < MAX_BLK_PARAMETER)
	{
		dTempProfile[0] = stpServoParaACS->dVelocityLoopProportionalGain; // astWbBondHeadServoCtrlPara[uiBlk]
		dTempProfile[1] = stpServoParaACS->dVelocityLoopIntegratorGain;   // astWbBondHeadServoCtrlPara[uiBlk]
		dTempProfile[2] = stpServoParaACS->dPositionLoopProportionalGain;  // astWbBondHeadServoCtrlPara[uiBlk]
		dTempProfile[3] = stpServoParaACS->dAccelerationFeedforward;  // astWbBondHeadServoCtrlPara[uiBlk]

		// Load default from ACS variables
		if(uiBlk == 0 && 
			dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1)
		{
			mtnapi_download_servo_parameter_acs_per_axis(hCommunicationHandle, 
				stServoAxis_ACS[WB_AXIS_TABLE_Y].uiAxisOnACS, // 20130127, BugFix,WB_AXIS_TABLE_Y, 
				stpServoParaACS);
		}
		// Read from WB application
		if(dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1) // protection
		{
			acsc_WriteReal(hCommunicationHandle, ACSC_NONE, "arYCtrl", uiBlk, uiBlk, 0, 3,  dTempProfile, NULL);  //;	  // Global Variable, 
		}
	}
	else
	{
		iRet = MTN_API_ERROR_EXCEED_MAX_BLK;
	}

	return iRet;
}

////////////////////////////////////////// Speed Profile
int mtn_wb_acs_download_table_x_speed_profile_blk(HANDLE hCommunicationHandle, unsigned int uiBlk)
{
	int iRet = MTN_API_OK_ZERO;
	double dTempProfile[3];

	if(uiBlk < NUM_TABLE_SPEED_PROF_BLK && uiBlk < MAX_BLK_PARAMETER)
	{
		dTempProfile[1] = stServoAxis_ACS[WB_AXIS_TABLE_X].stSpeedProfile[uiBlk].dMaxAcceleration; // astWbTableXSpeedProfile[uiBlk].dMaxAcceleration = ;
		dTempProfile[1]	= stServoAxis_ACS[WB_AXIS_TABLE_X].stSpeedProfile[uiBlk].dMaxDeceleration; // astWbTableXSpeedProfile[uiBlk].dMaxDeceleration = dTempProfile[1];
		dTempProfile[2] = stServoAxis_ACS[WB_AXIS_TABLE_X].stSpeedProfile[uiBlk].dMaxJerk;
		dTempProfile[0] = stServoAxis_ACS[WB_AXIS_TABLE_X].stSpeedProfile[uiBlk].dMaxVelocity;
		// Load default from ACS variables
		if(uiBlk == 3 && dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1)
		{
			mtnapi_set_speed_profile(hCommunicationHandle, 
				stServoAxis_ACS[WB_AXIS_TABLE_X].uiAxisOnACS, // 20130127, BugFix,APP_X_TABLE_ACS_ID, 
				&stServoAxis_ACS[WB_AXIS_TABLE_X].stSpeedProfile[uiBlk], NULL);
		}
		// Read from WB application
		if(dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1) // NON-Zero succeed;  // Global Variable, 20100726
		{
			acsc_WriteReal(hCommunicationHandle, ACSC_NONE, "arMotorXProfile", uiBlk, uiBlk, 0, 2,  dTempProfile, NULL);
		}
	}
	else
	{
		iRet = MTN_API_ERROR_EXCEED_MAX_BLK;
	}
	return iRet;
}


int mtn_wb_acs_download_table_y_speed_profile_blk(HANDLE hCommunicationHandle, unsigned int uiBlk)
{
	int iRet = MTN_API_OK_ZERO;
	double dTempProfile[3];
	if(uiBlk < NUM_TABLE_SPEED_PROF_BLK && uiBlk < MAX_BLK_PARAMETER)
	{
		dTempProfile[1] = stServoAxis_ACS[WB_AXIS_TABLE_Y].stSpeedProfile[uiBlk].dMaxAcceleration;
		dTempProfile[1] = stServoAxis_ACS[WB_AXIS_TABLE_Y].stSpeedProfile[uiBlk].dMaxDeceleration;
		dTempProfile[2] = stServoAxis_ACS[WB_AXIS_TABLE_Y].stSpeedProfile[uiBlk].dMaxJerk;
		dTempProfile[0] = stServoAxis_ACS[WB_AXIS_TABLE_Y].stSpeedProfile[uiBlk].dMaxVelocity;

		// Load default from ACS variables
		if(uiBlk == NUM_TABLE_SPEED_PROF_BLK - 1)
		{
			mtnapi_set_speed_profile(hCommunicationHandle, 
				stServoAxis_ACS[WB_AXIS_TABLE_Y].uiAxisOnACS, // 20130127, BugFix, APP_Y_TABLE_ACS_ID, 
				&stServoAxis_ACS[WB_AXIS_TABLE_Y].stSpeedProfile[uiBlk], NULL);
		}
		// Write to WB application
		if(dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1) // protection
		{
			int iFlagSuccessWrite = 0;
			if(acsc_WriteReal(hCommunicationHandle, ACSC_NONE, "arMotorYProfile", uiBlk, uiBlk, 0, 2,  dTempProfile, NULL)) // NON-Zero succeed;  // Global Variable, 20100726, arMotorXProfile, 20110201
			{
				iFlagSuccessWrite = 1;
			}
			else if(acsc_WriteReal(hCommunicationHandle, ACSC_NONE, "arMotorXProfile", uiBlk, uiBlk, 0, 2,  dTempProfile, NULL)) // NON-Zero succeed;  // Global Variable, 20100726, arMotorXProfile, 20110201
			{ // for version compatible with elder-WireBonder.exe before Ref25
				iFlagSuccessWrite = 1;
			}
		}
	}
	else
	{
		iRet = MTN_API_ERROR_EXCEED_MAX_BLK;
	}
	return iRet;
}
int mtn_wb_acs_download_bh_z_speed_profile_blk(HANDLE hCommunicationHandle, unsigned int uiBlk)
{
	int iRet = MTN_API_OK_ZERO;
	double dTempProfile[3];
	if(uiBlk < NUM_BONDHEAD_SPEED_PROF_BLK && uiBlk < MAX_BLK_PARAMETER)
	{
		dTempProfile[1] = stServoAxis_ACS[WB_AXIS_BOND_Z].stSpeedProfile[uiBlk].dMaxAcceleration;  // astWbBondHeadSpeedProfile[uiBlk]
		dTempProfile[1] = stServoAxis_ACS[WB_AXIS_BOND_Z].stSpeedProfile[uiBlk].dMaxDeceleration;  // astWbBondHeadSpeedProfile[uiBlk]
		dTempProfile[2] = stServoAxis_ACS[WB_AXIS_BOND_Z].stSpeedProfile[uiBlk].dMaxJerk; // astWbBondHeadSpeedProfile[uiBlk]
		dTempProfile[0] = stServoAxis_ACS[WB_AXIS_BOND_Z].stSpeedProfile[uiBlk].dMaxVelocity; // astWbBondHeadSpeedProfile[uiBlk]

		// Load default from ACS variables
		if(uiBlk == NUM_BONDHEAD_SPEED_PROF_BLK - 1)
		{
			mtnapi_set_speed_profile(hCommunicationHandle, 
				stServoAxis_ACS[WB_AXIS_BOND_Z].uiAxisOnACS, // 20130127, BugFix, sys_get_acs_axis_id_bnd_z(), 
				&stServoAxis_ACS[WB_AXIS_BOND_Z].stSpeedProfile[uiBlk], NULL);
		} 
		// Read from WB application
		if(dTempProfile[1] > 1 && dTempProfile[0] >1 && dTempProfile[2] >1) // NON-Zero succeed; // Global Variable, 20100726
		{
			acsc_WriteReal(hCommunicationHandle, ACSC_NONE, "arMotorAProfile", uiBlk, uiBlk, 0, 2,  dTempProfile, NULL);
		}
	}
	else
	{
		iRet = MTN_API_ERROR_EXCEED_MAX_BLK;
	}
	return iRet;
}
int mtn_wb_acs_download_speed_profile(HANDLE hCommunicationHandle)
{
	int iRet = MTN_API_OK_ZERO;
//	double dTempProfile[3];
	int ii;
	for(ii = 0; ii<NUM_TABLE_SPEED_PROF_BLK; ii++)
	{
		iRet = mtn_wb_acs_download_table_x_speed_profile_blk(hCommunicationHandle, ii);
		if(iRet != MTN_API_OK_ZERO) break;
		mtn_wb_acs_download_table_y_speed_profile_blk(hCommunicationHandle, ii);
		if(iRet != MTN_API_OK_ZERO) break;
	}
	for(ii = 0; ii<NUM_BONDHEAD_SPEED_PROF_BLK; ii++)
	{
		if(iRet != MTN_API_OK_ZERO) break;
		mtn_wb_acs_download_bh_z_speed_profile_blk(hCommunicationHandle, ii);
	}

	return iRet;
}

////////////////////// Control and Speed
int mtn_wb_download_acs_servo_speed_parameter_acs(HANDLE hCommunicationHandle)
{
	int iRet = MTN_API_OK_ZERO;

	iRet = mtn_wb_acs_download_servo_ctrl(hCommunicationHandle);

	if(iRet != MTN_API_OK_ZERO) 
	{
	}
	else
	{
		iRet = mtn_wb_acs_download_speed_profile(hCommunicationHandle);
	}
	return iRet;
}
