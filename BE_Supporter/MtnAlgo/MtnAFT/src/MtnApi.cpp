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
#include "MtnApi.h"
#include "math.h"

BOOL mtn_qc_is_axis_still_moving(HANDLE hCommunicationHandle, int iAxis)
{
	int iMotorStateReg;
	mtnapi_get_motor_state(hCommunicationHandle, iAxis, &iMotorStateReg, NULL);
	if(iMotorStateReg & ACSC_MST_MOVE)  // 
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL mtn_qc_is_axis_locked_safe(HANDLE hCommunicationHandle, int iAxis)
{
	int iMotorStateReg;
	mtnapi_get_motor_state(hCommunicationHandle, iAxis, &iMotorStateReg, NULL);
	if(iMotorStateReg & ACSC_MST_ENABLE)  // 
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void acs_cvt_float_to_24b_int_gf(double fInput, ACS_SP_VAR_FL *stpOutput)
{
	double dAbsInput = fabs(fInput);
	int iBigInt = ACS_SP_BIG_INT;
	double dRatio;
	int iFactor24b, iGain24b;

	if(dAbsInput > ACS_SP_BIG_INT)
	{
		dRatio = dAbsInput / ACS_SP_BIG_INT;
		iFactor24b = (int)ceil(log(dRatio)/log(2.0));
		//ceil(log((abs(fInput)/iBigInt));
		iGain24b = (int)(fInput / pow(2.0, (double)iFactor24b));
	}
	else
	{
		dRatio = ACS_SP_BIG_INT/dAbsInput;
		iFactor24b = - (int)(floor(log(dRatio)/log(2.0)));
		iGain24b = (int)(fInput * (pow(2.0,  -iFactor24b)));
	}
	stpOutput->iFactor = -iFactor24b;
	stpOutput->iGain = iGain24b;
}
// GA*2^(-GF)
void acs_cvt_24b_int_gf_to_float(ACS_SP_VAR_FL *stpInput, double *dOutput)
{
//	double dOutput; // = fabs(fInput);

	*dOutput = stpInput->iGain * pow(2.0, (- stpInput->iFactor));
}

// mtn_api is faster interface
void mtn_api_get_position_err(HANDLE mHandle, int iAxis, double *pdPositionErrorFb)
{
	// Get current position error feedback
	acsc_ReadReal(mHandle, ACSC_NONE, "PE", iAxis, iAxis, 0, 0, pdPositionErrorFb, NULL);
}

// 20120927
void mtn_api_get_drv_cmd(HANDLE mHandle, int iAxis, double *pdDrvCmdPc)
{
	// Get current position error feedback
	acsc_ReadReal(mHandle, ACSC_NONE, "DCOM", iAxis, iAxis, 0, 0, pdDrvCmdPc, NULL);
}

void mtn_api_get_motor_flag(HANDLE mHandle, int iAxis, int *piMotorFlag)
{
	acsc_ReadInteger(mHandle, ACSC_NONE, "MFLAGS", iAxis, iAxis, 0, 0, piMotorFlag, NULL);
}

#define  ACS_BIT_DRV_POLARITY_MOTOR_FLAG  0x2000
int mtn_api_get_flag_inv_drv_polarity(HANDLE mHandle, int iAxis)
{
	int iMotorFlag;
	mtn_api_get_motor_flag(mHandle, iAxis, &iMotorFlag);

	int iFlagInvDrivePolarity;
	iFlagInvDrivePolarity = iMotorFlag & ACS_BIT_DRV_POLARITY_MOTOR_FLAG;

	return iFlagInvDrivePolarity;
}

// mtnapi has slower and more standard interface
int mtnapi_get_fb_position(HANDLE Handle, int Axis, double* pdlbFbPosition, int iDebug)
{
//	ACSC_WAITBLOCK  stACSCWait;

	acsc_GetFPosition(Handle, Axis, pdlbFbPosition, NULL); // &stACSCWait);
  
	return MTN_API_OK_ZERO;
}

int mtnapi_get_fb_velocity(HANDLE Handle, int Axis, double* pdlbFbVelocity, int iDebug)
{
	acsc_GetFVelocity(Handle, Axis, pdlbFbVelocity, NULL); 
	return MTN_API_OK_ZERO;
}

int mtnapi_get_ref_velocity(HANDLE Handle, int Axis, double* pdlbRefVelocity, int iDebug)
{
	acsc_GetRVelocity(Handle, Axis, pdlbRefVelocity, NULL); 
	return MTN_API_OK_ZERO;
}

int mtnapi_get_ref_position(HANDLE Handle, int Axis, double* pdlbRefPosition, int iDebug)
{
	acsc_GetRPosition(Handle, Axis, pdlbRefPosition, NULL); 
	return MTN_API_OK_ZERO;
}

int mtnapi_get_motor_state(HANDLE Handle, int Axis, int* piState, int iDebug)
{
//	ACSC_WAITBLOCK  stACSCWait;

	acsc_GetMotorState(Handle, Axis, piState, NULL); // &stACSCWait);

	return MTN_API_OK_ZERO;
}

int mtnapi_enable_motor(HANDLE Handle, int Axis, int iDebug)
{
//	ACSC_WAITBLOCK  stACSCWait;
	acsc_Enable(Handle, Axis, NULL); // &stACSCWait);
	return MTN_API_OK_ZERO;
}

int mtnapi_disable_motor(HANDLE Handle, int Axis, int iDebug)
{
//	ACSC_WAITBLOCK  stACSCWait;
	acsc_Disable(Handle, Axis, NULL); // &stACSCWait);
	return MTN_API_OK_ZERO;
}

static int iMtnApiMotorFlags;
static int *piMtnApiMotorFlag = &iMtnApiMotorFlags;
int mtnapi_enable_motor_current_mode(HANDLE Handle, int iAxisACS)
{
	acsc_ReadInteger(Handle, 0, "MFLAGS", 
			iAxisACS, iAxisACS, 0, 0, piMtnApiMotorFlag, 0); // DOUT

	iMtnApiMotorFlags = iMtnApiMotorFlags | OR_SET_BIT_1;
	iMtnApiMotorFlags = iMtnApiMotorFlags & AND_CLEAR_BIT_17;

	acsc_WriteInteger(Handle, 0, "MFLAGS", 
			iAxisACS, iAxisACS, 0, 0, piMtnApiMotorFlag, 0); // DOUT
	return MTN_API_OK_ZERO;

}
int mtnapi_disable_motor_current_mode(HANDLE Handle, int iAxisACS)
{
	acsc_ReadInteger(Handle, 0, "MFLAGS", 
			iAxisACS, iAxisACS, 0, 0, piMtnApiMotorFlag, 0); // DOUT

	iMtnApiMotorFlags = iMtnApiMotorFlags & AND_CLEAR_BIT_1;
	iMtnApiMotorFlags = iMtnApiMotorFlags | OR_SET_BIT_17;

	acsc_WriteInteger(Handle, 0, "MFLAGS", 
			iAxisACS, iAxisACS, 0, 0, piMtnApiMotorFlag, 0); // DOUT
	return MTN_API_OK_ZERO;
}
//int mtnapi_enable_motors(HANDLE Handle, int Axis, int iDebug)
//{
//	ACSC_WAITBLOCK  stACSCWait;
//	acsc_EnableM(Handle, Axis, NULL); //&stACSCWait);
//}

int mtnapi_set_speed_profile(HANDLE Handle, int Axis, MTN_SPEED_PROFILE* stpSpeedProfile, int iDebug)
{
	acsc_SetVelocity(Handle, Axis, stpSpeedProfile->dMaxVelocity, NULL);
	acsc_SetAcceleration(Handle, Axis, stpSpeedProfile->dMaxAcceleration, NULL);
	acsc_SetDeceleration(Handle, Axis, stpSpeedProfile->dMaxDeceleration, NULL);
	acsc_SetJerk(Handle, Axis, stpSpeedProfile->dMaxJerk, NULL);
	acsc_SetKillDeceleration(Handle, Axis, stpSpeedProfile->dMaxKillDeceleration, NULL);
	return MTN_API_OK_ZERO;
}

#define MTN_PROFILE_3_POLY     0
#define MTN_PROFILE_CYCLOIDAL  1
#define MTN_PROFILE_7_POLY     2
#define MTN_PROFILE_SAM1_4567   3
#define MTN_PROFILE_SAM2_4567   4
#define MTN_PROFILE_NON_SYM    5

int mtnapi_get_speed_profile(HANDLE Handle, int Axis, MTN_SPEED_PROFILE* stpSpeedProfile, int iDebug)
{
	acsc_GetVelocity(Handle, Axis, &stpSpeedProfile->dMaxVelocity, NULL);
	acsc_GetAcceleration(Handle, Axis, &stpSpeedProfile->dMaxAcceleration, NULL);
	acsc_GetDeceleration(Handle, Axis, &stpSpeedProfile->dMaxDeceleration, NULL);
	acsc_GetJerk(Handle, Axis, &stpSpeedProfile->dMaxJerk, NULL);
	acsc_GetKillDeceleration(Handle, Axis, &stpSpeedProfile->dMaxKillDeceleration, NULL);
	stpSpeedProfile->uiProfileType = MTN_PROFILE_3_POLY;
	return MTN_API_OK_ZERO;
}

int mtnapi_axis_acs_move_to(HANDLE Handle, int iAxis, int iPosnTo)
{
	return acsc_ToPoint(Handle, 0, // start up immediately the motion
					iAxis, iPosnTo, NULL);
}
int mtnapi_axis_profile_move_to(HANDLE Handle, int iAxis, MTN_SPEED_PROFILE* stpSpeedProfile, int iPosnTo)
{
	mtnapi_set_speed_profile(Handle, iAxis, stpSpeedProfile, 0);
	return acsc_ToPoint(Handle, 0, // start up immediately the motion
					iAxis, iPosnTo, NULL);
}

void mtn_api_get_position_upp_lmt(HANDLE mHandle, int iAxis, double *pdPositionUppLmt)
{
	// Get current position error feedback
	acsc_ReadReal(mHandle, ACSC_NONE, "SRLIMIT", iAxis, iAxis, 0, 0, pdPositionUppLmt, NULL);
}
void mtn_api_get_position_low_lmt(HANDLE mHandle, int iAxis, double *pdPositionLowLmt)
{
	// Get current position error feedback
	acsc_ReadReal(mHandle, ACSC_NONE, "SLLIMIT", iAxis, iAxis, 0, 0, pdPositionLowLmt, NULL);
}

void mtn_api_set_position_upp_lmt(HANDLE mHandle, int iAxis, double dPositionUppLmt)
{
	// Get current position error feedback
	acsc_WriteReal(mHandle, ACSC_NONE, "SRLIMIT", iAxis, iAxis, 0, 0, &dPositionUppLmt, NULL);
}
void mtn_api_set_position_low_lmt(HANDLE mHandle, int iAxis, double dPositionLowLmt)
{
	// Get current position error feedback
	acsc_WriteReal(mHandle, ACSC_NONE, "SLLIMIT", iAxis, iAxis, 0, 0, &dPositionLowLmt, NULL);
}

int mtn_api_set_spring_compensation_sp_para(HANDLE Handle, MTUNE_OUT_POSN_COMPENSATION *stpOutputPosnCompensationTune, FILE *fpData)
{
char strDownloadCmd[256];
int iRet = MTN_API_OK_ZERO;

/// Download to DSP variables
		sprintf_s(strDownloadCmd, 256, "SETSPV(SP0:SpringOffset, %d)\n\r", stpOutputPosnCompensationTune->iOffset_SP_ACS);
//		strlen(strDownloadCmd);
		if (!acsc_Send(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, NULL))
		{
			if(fpData != NULL)
			{
				fprintf(fpData, "Error Download Offset\n");
			}
			iRet = MTN_API_ERROR;
		}
		else
		{
			if(fpData != NULL)
			{
				fprintf(fpData, "%s", strDownloadCmd);
			}
		}

		sprintf_s(strDownloadCmd, 256, "SETSPV(SP0:SpringCompGa, %d)\n\r", stpOutputPosnCompensationTune->iGain_SP_ACS);
		if (!acsc_Send(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, NULL))
		{
			if(fpData != NULL)
			{
				fprintf(fpData, "Error Download Ga\n");
			}
			iRet = MTN_API_ERROR;
		}
		else
		{
			if(fpData != NULL)
			{
				fprintf(fpData, "%s", strDownloadCmd);
			}
		}

		sprintf_s(strDownloadCmd, 256, "SETSPV(SP0:SpringCompGf, %d)\n\r", stpOutputPosnCompensationTune->iFactor_SP_ACS);
		if (!acsc_Send(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, NULL))
		{
			if(fpData != NULL)
			{
				fprintf(fpData, "Error Download Factor\n");
			}
			iRet = MTN_API_ERROR;
		}
		else
		{
			if(fpData != NULL)
			{
				fprintf(fpData, "%s", strDownloadCmd);
			}
		}

	return iRet;
}
int mtn_api_enable_spring_compensation_sp_para(HANDLE Handle)
{
char strDownloadCmd[256];
int iRet = MTN_API_OK_ZERO;

	sprintf_s(strDownloadCmd, 256, "SETSPV(SP0:SpringCompOn, 1)\n\r"); // , stpOutputPosnCompensationTune->iFactor_SP_ACS);
	if (!acsc_Send(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, NULL))
	{
		iRet = MTN_API_ERROR;
	}
	return iRet;
}

int mtn_api_disable_spring_compensation_sp_para(HANDLE Handle)
{
char strDownloadCmd[256];
int iRet = MTN_API_OK_ZERO;

	sprintf_s(strDownloadCmd, 256, "SETSPV(SP0:SpringCompOn, 0)\n\r"); // , stpOutputPosnCompensationTune->iFactor_SP_ACS);
	if (!acsc_Send(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, NULL))
	{
		iRet = MTN_API_ERROR;
	}
	return iRet;
}

// , FILE *fpData
static char *strStopString;
#include "MtnWbDef.h"
#include "MotAlgo_DLL.h"

int mtn_api_get_spring_compensation_sp_para(HANDLE Handle, MTUNE_OUT_POSN_COMPENSATION *stpOutputPosnCompensationTune)
{
char strDownloadCmd[256];
int iRet = MTN_API_OK_ZERO;

char strUploadMessage[256];
int iCountReceived;
ACS_SP_VAR_FL stOutput24bFloat;

/// Download to DSP variables
	sprintf_s(strDownloadCmd, 256, "?GETSPV(SP0:SpringOffset)\n\r");
//		strlen(strDownloadCmd);
	if (!acsc_Transaction(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		iRet = MTN_API_ERROR;
	}
	else
	{
		//stpOutputPosnCompensationTune->iOffset_SP_ACS
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			stpOutputPosnCompensationTune->iOffset_SP_ACS = (int)strtod(strUploadMessage, &strStopString);
		}
		else
		{
			stpOutputPosnCompensationTune->iOffset_SP_ACS = 0;
		}
	}

	sprintf_s(strDownloadCmd, 256, "?GETSPV(SP0:SpringCompGa)\n\r");
	if (!acsc_Transaction(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		iRet = MTN_API_ERROR;
	}
	else
	{ // stpOutputPosnCompensationTune->iGain_SP_ACS
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			stpOutputPosnCompensationTune->iGain_SP_ACS = (int)strtod(strUploadMessage, &strStopString);
		}
		else
		{
			stpOutputPosnCompensationTune->iGain_SP_ACS = 0;
		}
	}

	sprintf_s(strDownloadCmd, 256, "?GETSPV(SP0:SpringCompGf)\n\r");
	if (!acsc_Transaction(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		iRet = MTN_API_ERROR;
	}
	else
	{ // stpOutputPosnCompensationTune->iFactor_SP_ACS
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			stpOutputPosnCompensationTune->iFactor_SP_ACS = (int)strtod(strUploadMessage, &strStopString);
		}
		else
		{
			stpOutputPosnCompensationTune->iFactor_SP_ACS = 0;
		}
	}

		//ACS_SP_VAR_FL stOutputACS_FL_SP;
		//acs_cvt_float_to_24b_int_gf(stpOutputPosnCompensationTune->dPositionFactor, &stOutputACS_FL_SP);
		//stpOutputPosnCompensationTune->iFactor_SP_ACS = stOutputACS_FL_SP.iFactor;
		//stpOutputPosnCompensationTune->iGain_SP_ACS = -stOutputACS_FL_SP.iGain;
	stOutput24bFloat.iFactor = stpOutputPosnCompensationTune->iFactor_SP_ACS;
	stOutput24bFloat.iGain = - stpOutputPosnCompensationTune->iGain_SP_ACS;  // Note there is negative, 20110709
	acs_cvt_24b_int_gf_to_float(&stOutput24bFloat, &(stpOutputPosnCompensationTune->dPositionFactor));

	sprintf_s(strDownloadCmd, 256, "?GETSPV(SP0:SpringCompOn)\n\r"); // , );
	if (!acsc_Transaction(Handle, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		iRet = MTN_API_ERROR;
	}
	else
	{ // 
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			stpOutputPosnCompensationTune->iFlagEnable = (int)strtod(strUploadMessage, &strStopString);
		}
		else
		{
			stpOutputPosnCompensationTune->iFlagEnable = 0;
		}
	}

	//// Read encoder offset
	int iEncoderOffset;
	acsc_ReadInteger(Handle, NULL, "EOFFS", sys_get_acs_axis_id_bnd_z(), sys_get_acs_axis_id_bnd_z(), 0, 0, &iEncoderOffset, NULL);
	stpOutputPosnCompensationTune->iEncoderOffsetSP = iEncoderOffset;

	//stpOutputPosnCompensationTune->iOffset_SP_ACS = (int)(stpOutputPosnCompensationTune->dCtrlOutOffset / (-stpOutputPosnCompensationTune->dPositionFactor)) - iEncoderOffset;
	stpOutputPosnCompensationTune->dCtrlOutOffset = (stpOutputPosnCompensationTune->iOffset_SP_ACS + iEncoderOffset) * ( - stpOutputPosnCompensationTune->dPositionFactor);

	return iRet;
}

#include "acs_buff_prog.h"
// iAutoTuneFlagBnd_A
int acs_read_srch_contact_buffer_auto_tune_flag(HANDLE Handle, int *piFlagAutoTune)
{
int iRet = MTN_API_OK_ZERO;
int iTempRead;
	if (!acsc_ReadInteger(Handle, BUFFER_ID_SEARCH_CONTACT, "iAutoTuneFlagBnd_A", 
		0, 0, ACSC_NONE, ACSC_NONE, &iTempRead, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}
	else
	{
		(*piFlagAutoTune ) = iTempRead;
	}

	return iRet;
}

int acs_write_srch_contact_buffer_auto_tune_flag(HANDLE Handle, int iFlagAutoTune)
{
int iRet = MTN_API_OK_ZERO;
int iTempWrite = iFlagAutoTune;
	if (!acsc_WriteInteger(Handle, BUFFER_ID_SEARCH_CONTACT, "iAutoTuneFlagBnd_A", 
		0, 0, ACSC_NONE, ACSC_NONE, &iTempWrite, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	return iRet;
}

int mtn_run_srch_contact_b1w(HANDLE m_hHandle)
{
	int iRet;
//	MTN_SPEED_PROFILE stSpeedProfileBak_Z;

	//acsc_WriteDPRAMInteger(m_hHandle, DPRAM_ADDR_START_BUF_7, 1);
	acs_write_srch_contact_buffer_auto_tune_flag(m_hHandle, 1); // 20120203
	int iTempTrigger;
	iTempTrigger = 1;
	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iAFT_StartSearchContact_buffer7", 0, 0, ACSC_NONE, ACSC_NONE, &iTempTrigger, NULL ))
	{
		iRet = MTN_API_ERROR; 
	}
	Sleep(50);
	int iFlagFinishedSearch = 0; // iAFT_DoneSearchContact_buffer7 @ DPRAM_ADDR_DONE_SEARCH_CONTACT
//	acsc_ReadDPRAMInteger(m_hHandle, DPRAM_ADDR_DONE_SEARCH_CONTACT, &iFlagFinishedSearch);
	acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iAFT_DoneSearchContact_buffer7", 0, 0, ACSC_NONE, ACSC_NONE, &iFlagFinishedSearch, NULL );
	if(iFlagFinishedSearch == 0)
	{
		Sleep(10);
		acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iAFT_DoneSearchContact_buffer7", 0, 0, ACSC_NONE, ACSC_NONE, &iFlagFinishedSearch, NULL );
	}

	return iRet;
}


#define MAX_LOOP_COUNT_WAITING_SETTLING  1000
int mtn_acs_wait_axis_settle(HANDLE m_hHandle, int iAcsAxis)
{
	int iRet = MTN_API_OK_ZERO;

	double dPrevFbPosition, dCurrFbPosition; //, adPositionSample[32]; // 20121224
	double dSumFbPosition;
	int ii, nCountLoop;

	mtnapi_get_fb_position(m_hHandle, iAcsAxis, &dPrevFbPosition, 0);
	dSumFbPosition = 0;
	for(ii = 0; ii<32; ii++)
	{
		mtnapi_get_fb_position(m_hHandle, iAcsAxis, &dCurrFbPosition, 0);
		Sleep(5);
		dSumFbPosition = dSumFbPosition + dCurrFbPosition;
	}
	dCurrFbPosition = dSumFbPosition/32;

	nCountLoop = 0;
	while((fabs(dCurrFbPosition - dPrevFbPosition) > 5 ) && nCountLoop < MAX_LOOP_COUNT_WAITING_SETTLING)
	{
		dPrevFbPosition = dCurrFbPosition;
		dSumFbPosition = 0;
		for(ii = 0; ii<32; ii++)
		{
			mtnapi_get_fb_position(m_hHandle, iAcsAxis, &dCurrFbPosition, 0);
			Sleep(5);
			dSumFbPosition = dSumFbPosition + dCurrFbPosition;
		}
		dCurrFbPosition = dSumFbPosition/32;
		nCountLoop = nCountLoop + 1;
	}

	if(nCountLoop >= MAX_LOOP_COUNT_WAITING_SETTLING)
	{
		iRet = MTN_API_ERROR_SETTLING;
	}
	
	return iRet;

}
