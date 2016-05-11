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

// 20090703 Add offset current to hold the axis in current position
// 20090908 Replace Sleep to be high_precision_sleep_ms

//// Module for pseudo-random binary sequence generator
//      and System spectrum analysis
#include "stdafx.h"
#include <math.h>

#include "stdlib.h"
#include "mtndefin.h"
#include "MtnApi.h"

#include "WinTiming.h"

// ACS buffer program
#include "acs_buff_prog.h"

#include "aftprbs.h"
// From MtnSysanaSpectgrum.cpp
extern int iFlagSpectrumSysAnaFlag;
extern char strSpectrumNewFolderName[128];
extern char cFlagEnableDebug;

UINT nTotalNumCasesGroupSpecTest;
UINT nCurrCaseIdxGroupSpecTest;

static short sCommandPRBS[MAX_PRBS_LEN];
static double pRealCmdPrbs[MAX_PRBS_LEN];
static double adSpectrumResponse[PRBS_FB_MAX_ARRAY_ROW * PRBS_FB_MAX_ARRAY_COL];
static unsigned short usActualUploadLen;

static double adVelScopeSP[PRBS_NUM_POINT_ARRAY_SPDC];
static unsigned short usActualUploadSPDC;

// Variables and routines for checking time-out
static LARGE_INTEGER liFreqOS_SpecTest; 
void mtn_cto_tick_start_time_u1s(LARGE_INTEGER liFreqOperSys);
bool mtn_cto_is_time_out_u1s(LARGE_INTEGER liFreqOperSys, unsigned int uiTimeOut_u1s);
unsigned int mtn_cto_get_start_cnt();
unsigned int mtn_cto_get_curr_cnt();

//
extern COMM_SETTINGS stServoControllerCommSet;
static double dIsrTime_ms = 1;

// Local debug text
static char strDebugText[128];
static double dMaxDacValueInController;

// CMtnSpectrumTestDlg dialog
HANDLE stCommHandleACS;				// communication handle

static int iFlagEnableDebugGroupPRBS = 0;
int aft_spectrum_get_debug_flag()
{
	return iFlagEnableDebugGroupPRBS;
}
void aft_spectrum_set_debug_flag(int iFlag)
{
	iFlagEnableDebugGroupPRBS = iFlag;
}

static char cFlagVelocityLoopExciting = 1;
char aft_spectrum_get_flag_velloop()
{
	return cFlagVelocityLoopExciting;
}
void aft_spectrum_set_flag_velocity_loop_excite(char cFlagExciteVelLoop)
{
	cFlagVelocityLoopExciting = cFlagExciteVelLoop;
}

static char cFlagCaptureDSP_Data = 0;
char aft_spectrum_get_flag_dsp_data()
{
	return cFlagCaptureDSP_Data;
}
void aft_spectrum_set_flag_dsp_data(char cFlag)
{
	cFlagCaptureDSP_Data = cFlag;
}

static BOOL mFlagStopSpectrumTestThread;
BOOL aft_spectrum_get_thread_flag_stop()
{
	return mFlagStopSpectrumTestThread;
}

void aft_spectrum_set_thread_flag_stop(int bFlagStop)
{
	mFlagStopSpectrumTestThread = bFlagStop;
}

// Variables for Bakup Parameter
static int iParaBakupFlag;
static MTN_SPEED_PROFILE stBakupParaSpeedProf[MAX_CTRL_AXIS_PER_SERVO_BOARD];
static MTN_SPEED_PROFILE stParaSpeedProfileAtSpecTest[MAX_CTRL_AXIS_PER_SERVO_BOARD];

static CTRL_PARA_ACS stBakupServoControlPara[MAX_CTRL_AXIS_PER_SERVO_BOARD];  // 20110720

static int iCurrOffsetHoldAxis[MAX_CTRL_AXIS_PER_SERVO_BOARD];

// [afSineOut, aTimePoint_at_Ts] = gen_sin_sweep_one_freq(Amp, Ts_ms, fFreq_Hz, nNumCycle)
//%
//% Ts_ms: is the sample time in the unit of milli-sec
//% fFreq_Hz: frequency, Hz
//
//nLen = ceil(2 * pi * nNumCycle * 1000/fFreq_Hz/Ts_ms);
//
//aTimePoint_at_Ts = 0:1:(nLen - 1);
//for ii = 1:1:nLen
//    afSineOut(ii) = Amp * sin(2 * pi * aTimePoint_at_Ts(ii) * fFreq_Hz * Ts_ms/1000);
//end
#define  __PI__    (3.1415926536)
short aft_sine_sweep_gen_excite_cmd_one(SINE_SWEEP_GEN_EXCITE_ONE_FREQ_IN *stpInputSineSwp, 
										SINE_SWEEP_GEN_EXCITE_ONE_FREQ_OUT *stpOutputSine)
{
double Amp =stpInputSineSwp->fAmplitude;
double Ts_ms = stpInputSineSwp->fTs_ms;
double fFreq_Hz = stpInputSineSwp->fFreq_Hz;
double nNumCycle = stpInputSineSwp->dNumCycle;
short sRet = MTN_API_OK_ZERO;

unsigned int uiLen, ii;

	uiLen = (unsigned int)(ceil(__PI__ * 2.0 * nNumCycle * 1000.0/fFreq_Hz/Ts_ms));

	if(uiLen > PRBS_FB_MAX_ARRAY_COL)
	{
		uiLen = PRBS_FB_MAX_ARRAY_COL;
	}
	for(ii = 0; ii<uiLen; ii++)
	{
		stpOutputSine->afTimePointTs[ii] = ii;
		stpOutputSine->afSineOut[ii] =  Amp * sin(2.0 * __PI__ * ii * fFreq_Hz * Ts_ms/1000.0);
	}
	stpOutputSine->uiLen = uiLen;

return sRet;
}
#include "MtnInitAcs.h"
short aft_spectrum_initialize()
{
	short sRet = MTN_API_OK_ZERO;

	stCommHandleACS = stServoControllerCommSet.Handle;
	dMaxDacValueInController = DAC_MAX_16BIT_SIGNED;
	cFlagEnableDebug = 0;
	usActualUploadLen = 0;

	// Get OS frequency
	QueryPerformanceFrequency(&liFreqOS_SpecTest); 

//	double dTempCTime[1]; 
//	if(!acsc_ReadReal(stCommHandleACS, ACSC_NONE, "CTIME", 0, 0, ACSC_NONE, ACSC_NONE, dTempCTime, NULL))
//	{
//		sprintf_s(strDebugText, 128, "Warning: error get Controller Time unit(ms), Error Code: %d", acsc_GetLastError());
//#ifndef __MOTALGO_DLL
//		if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
//#endif // __MOTALGO_DLL
//		sRet = MTN_API_ERR_START_FIRMWARE_PROG;
//	}
//	else
//	{
//		dIsrTime_ms = dTempCTime[0];
//	}
	dIsrTime_ms = sys_get_controller_ts();
	iParaBakupFlag = 0; // Initialize FALSE
	return sRet;
}
short aft_safe_exit_spectrum_test()
{
	short sRet = MTN_API_OK_ZERO;
	return sRet;
}

double aft_get_basic_time_unit()
{
	return dIsrTime_ms;
}

int aft_get_spectrum_test_bakup_flag()
{
	return iParaBakupFlag;
}
short aft_get_sample_period_ms(	double *pdSamplePeriod_ms)
{
	short sRet = MTN_API_OK_ZERO;
	if(!acsc_ReadReal(stCommHandleACS, 0, "S_DCP", 0, 0, 0, 0, pdSamplePeriod_ms, NULL))
	{
		sprintf_s(strDebugText, 128, "get S_DCP error, Error Code: %d", acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug)	AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = AFT_PRBS_ERR_UPLOAD_DATA;
	}
	return sRet;
}

void aft_backup_speedprofile_for_spec_axis(PRBS_GROUP_TEST_INPUT *stpGroupPrbsInput)
{
	unsigned int ii, uiAxis;
	for(ii = 0; ii < stpGroupPrbsInput->usActualNumAxis; ii++)
	{
		uiAxis = stpGroupPrbsInput->uiAxis_CtrlCardId[ii];
		mtnapi_get_speed_profile(stCommHandleACS, uiAxis, &stBakupParaSpeedProf[uiAxis], 0);
	}
	iParaBakupFlag = 1;
} // After bakup, it is TRUE

void aft_restore_speedprofile_for_spec_axis(PRBS_GROUP_TEST_INPUT *stpGroupPrbsInput)
{
	unsigned int ii, uiAxis;
	for(ii = 0; ii < stpGroupPrbsInput->usActualNumAxis; ii++)
	{
		uiAxis = stpGroupPrbsInput->uiAxis_CtrlCardId[ii];
		mtnapi_set_speed_profile(stCommHandleACS, uiAxis, &stBakupParaSpeedProf[uiAxis], 0);
	}
	iParaBakupFlag = 0;
}

short aft_set_smaller_speed(double dAccRatio, double dJerkRatio, PRBS_GROUP_TEST_INPUT *stpGroupPrbsInput)
{
	short sRet = MTN_API_OK_ZERO;
	unsigned int ii, uiAxis;
	if(iParaBakupFlag == 1 && abs(dAccRatio) < 1.0 && abs(dJerkRatio) < 1.0)
	{
		for(ii = 0; ii<stpGroupPrbsInput->usActualNumAxis; ii ++)
		{
			uiAxis = stpGroupPrbsInput->uiAxis_CtrlCardId[ii];
			memcpy(&stParaSpeedProfileAtSpecTest[uiAxis], &stBakupParaSpeedProf[uiAxis], sizeof(MTN_SPEED_PROFILE));
			stParaSpeedProfileAtSpecTest[uiAxis].dMaxAcceleration *= abs(dAccRatio);
			stParaSpeedProfileAtSpecTest[uiAxis].dMaxDeceleration *= abs(dAccRatio);
			stParaSpeedProfileAtSpecTest[uiAxis].dMaxKillDeceleration *= abs(dAccRatio);
			stParaSpeedProfileAtSpecTest[uiAxis].dMaxJerk *= abs(dJerkRatio);

			mtnapi_set_speed_profile(stCommHandleACS, uiAxis, &stParaSpeedProfileAtSpecTest[uiAxis], 0);

		}
	}
	else
	{
		sRet =  AFT_PRBS_ERR_NOT_VALID_PARAMETER;
	}
	return sRet;
}
#include "MtnInitAcs.h"
// 20110720
void aft_backup_servo_control_for_spec_axis(PRBS_GROUP_TEST_INPUT *stpGroupPrbsInput)
{
	unsigned int ii, uiAxis;
	for(ii = 0; ii < stpGroupPrbsInput->usActualNumAxis; ii++)
	{
		uiAxis = stpGroupPrbsInput->uiAxis_CtrlCardId[ii];
		mtnapi_upload_servo_parameter_acs_per_axis(stCommHandleACS, uiAxis, &stBakupServoControlPara[uiAxis]);
	}
} 

void aft_restore_servo_control_for_spec_axis(PRBS_GROUP_TEST_INPUT *stpGroupPrbsInput)
{
	unsigned int ii, uiAxis;
	for(ii = 0; ii < stpGroupPrbsInput->usActualNumAxis; ii++)
	{
		uiAxis = stpGroupPrbsInput->uiAxis_CtrlCardId[ii];
		mtnapi_download_servo_parameter_acs_per_axis(stCommHandleACS, uiAxis, &stBakupServoControlPara[uiAxis]);
	}
} // 20110720

// binary random number generation by MS-C library, maynot be as white as expected
short aft_gen_rand_array(unsigned int uiRandSeed, unsigned short usPrbsLen, short sPrbsAmp)
{
	short sRet = MTN_API_OK_ZERO;
	srand(uiRandSeed);

	if( usPrbsLen > MAX_PRBS_LEN)
	{
		sRet = AFT_PRBS_ERR_EXCEED_MAX_LEN;
		goto label_ret_aft_gen_rand_array;
	}
	else
	{
		unsigned int ii;
		int uiHalfRandMax = RAND_MAX/2; // Assume 7FFF, 32767/2 = 16384
		for(ii = 0; ii<usPrbsLen; ii++)
		{
			if(rand() >= uiHalfRandMax)
			{
				sCommandPRBS[ii] = sPrbsAmp;
			}
			else
			{
				sCommandPRBS[ii] = -sPrbsAmp;
			}
		}
	}

label_ret_aft_gen_rand_array:
	return sRet;

}

short aft_get_prbs_array(short *psCmdPrbs, unsigned short sLen)
{
	short sRet = MTN_API_OK_ZERO;
	if( sLen > MAX_PRBS_LEN)
	{
		sRet = AFT_PRBS_ERR_EXCEED_MAX_LEN;
		goto label_ret_aft_get_prbs_array;
	}
	else
	{
		unsigned short ii;
		for(ii=0; ii<sLen; ii++)
		{
			psCmdPrbs[ii] = sCommandPRBS[ii];
		}
	}

label_ret_aft_get_prbs_array:
	return sRet;

}

void aft_get_prbs_response_array(double adResponseArrayPrbs[], unsigned short *usRow, unsigned short *usCol)
{
	(*usRow) = PRBS_FB_MAX_ARRAY_ROW;
	(*usCol) = usActualUploadLen;

	int ii, jj;
	for(ii = 0; ii< PRBS_FB_MAX_ARRAY_ROW; ii ++)
	{
		for(jj = 0; jj < usActualUploadLen; jj++)
		{
			adResponseArrayPrbs[ii + jj * PRBS_FB_MAX_ARRAY_ROW] = adSpectrumResponse[ii * usActualUploadLen + jj];
		}
	}

}
void aft_get_prbs_sp_vel_response(double adResponseArraySP_VelPrbs[], unsigned short *usLen)
{
	*usLen = usActualUploadSPDC;

	int jj;
	for(jj = 0; jj < usActualUploadLen; jj++)
	{
		adResponseArraySP_VelPrbs[jj] = adVelScopeSP[jj];
	}
}
//global int iAFT_FlagMotion; ! 1: motion in close loop, 0: testing in open loop
//global int iAFT_PrbsLen;
//global int iAFT_PrbsSampleTime_ms;
//global int iAFT_Axis;
//GLOBAL REAL  rAFT_PrbsArray(10000);

short aft_download_prbs_array(unsigned short usCountLenPRBS)
{
	short sRet = MTN_API_OK_ZERO;
	// convert to double type
	for(short ii = 0; ii<usCountLenPRBS; ii ++)
	{
		pRealCmdPrbs[ii] = sCommandPRBS[ii] * 100.0/ dMaxDacValueInController; // meaning is percentage
	}

	if (!acsc_WriteReal(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "rAFT_PrbsArray", 0, 0, 0, usCountLenPRBS - 1, pRealCmdPrbs, NULL ))
	{
		sprintf_s(strDebugText, 128, "download PRBS array error, Error Code: %d", acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug)	AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = AFT_PRBS_ERR_DOWNLOAD_ERROR;
	}

	return sRet;
}

short aft_download_sine_sweep_array(SINE_SWEEP_GEN_EXCITE_ONE_FREQ_OUT *stpSineSweepOneExciteArray)
{
	short sRet = MTN_API_OK_ZERO;
	// convert to double type
	for(unsigned int ii = 0; ii<stpSineSweepOneExciteArray->uiLen; ii ++)
	{
		pRealCmdPrbs[ii] = stpSineSweepOneExciteArray->afSineOut[ii] * 100.0/ dMaxDacValueInController; // meaning is percentage
	}

	if (!acsc_WriteReal(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "rAFT_PrbsArray", 0, 0, 0, (int)(stpSineSweepOneExciteArray->uiLen - 1), pRealCmdPrbs, NULL ))
	{
		sprintf_s(strDebugText, 128, "download SineSweep array error, Error Code: %d", acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug)	AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = AFT_PRBS_ERR_DOWNLOAD_ERROR;
	}

	return sRet;
}

short aft_download_prbs_array_w_offset(unsigned short usCountLenPRBS, int iOffset)
{
	short sRet = MTN_API_OK_ZERO;
	// convert to double type
	for(short ii = 0; ii<usCountLenPRBS; ii ++)
	{
		pRealCmdPrbs[ii] = (iOffset + sCommandPRBS[ii]) * 100.0/ dMaxDacValueInController; // meaning is percentage
	}

	if (!acsc_WriteReal(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "rAFT_PrbsArray", 0, 0, 0, usCountLenPRBS - 1, pRealCmdPrbs, NULL ))
	{
		sprintf_s(strDebugText, 128, "download PRBS array error, Error Code: %d", acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug)	AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = AFT_PRBS_ERR_DOWNLOAD_ERROR;
	}

	return sRet;
}

// Flag to excite PRBS on which test point
// 0:  DAC -- current command
// 1;  VEL - Velocity command
// 


short aft_download_prbs_parameter(PRBS_TEST_CONDITION *stpPrbsTestInput)
{
short sRet = MTN_API_OK_ZERO;
int iSpectrumFlagGetDataSP[1]; iSpectrumFlagGetDataSP[0] = cFlagCaptureDSP_Data;
	//
	if (!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "iFlagDcSP", 0, 0, 0, 0, iSpectrumFlagGetDataSP, NULL ))
	{
		sprintf_s(strDebugText, 128, "download PRBS lengh error, Error Code: %d", acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug)			AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = AFT_PRBS_ERR_DOWNLOAD_ERROR;
		goto label_return_aft_download_prbs_parameter;
	}

	int iPRBSLen[1]; iPRBSLen[0] = stpPrbsTestInput->usCountLenPRBS; // must <= 4200
	if (!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "iAFT_PrbsLen", 0, 0, 0, 0, iPRBSLen, NULL ))
	{
		sprintf_s(strDebugText, 128, "download PRBS lengh error, Error Code: %d", acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug)			AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = AFT_PRBS_ERR_DOWNLOAD_ERROR;
		goto label_return_aft_download_prbs_parameter;
	}

	sRet = acs_bufprog_write_motion_flag_buff_8(ACS_BUFPROG_MTN_FLAG_OPEN_LOOP);
	if (sRet != MTN_API_OK_ZERO)
	{
		sprintf_s(strDebugText, 128, "download Motion Flag error, Error Code: %d", acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug)			AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = AFT_PRBS_ERR_DOWNLOAD_ERROR;
		goto label_return_aft_download_prbs_parameter;
	}

	int iTestAxis[1];  iTestAxis[0] = stpPrbsTestInput->uiExcitingAxis;
	if (!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "iAFT_Axis", 0, 0, 0, 0, iTestAxis, NULL ))
	{
		sprintf_s(strDebugText, 128, "download Test axis error, Error Code: %d", acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug)	AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = AFT_PRBS_ERR_DOWNLOAD_ERROR;
		goto label_return_aft_download_prbs_parameter;
	}

label_return_aft_download_prbs_parameter:
	return sRet;
}

short aft_start_prbs_excitation()
{
	short sRet = MTN_API_OK_ZERO;
	// Run buffer program for spectrum testing
//	acsc_WriteDPRAMInteger(stCommHandleACS, DPRAM_ADDR_START_MOVE_BUF_8, 1);
    // 20090909
	int iStartProgram_buffer8[1];  iStartProgram_buffer8[0] = 1;
	if (!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "iAFT_StartPathMotion_buffer8", 0, 0, 0, 0, iStartProgram_buffer8, NULL ))
	{
		sprintf_s(strDebugText, 128, "download starting PathMotion, Error Code: %d", acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug)	AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = AFT_PRBS_ERR_DOWNLOAD_ERROR;
	}

	return sRet;
}

short aft_start_vel_loop_prbs_excitation()
{
	short sRet = MTN_API_OK_ZERO;
	// Run buffer program for spectrum testing
//	acsc_WriteDPRAMInteger(stCommHandleACS, DPRAM_ADDR_START_MOVE_BUF_8, 1);
    // 20090909
	int iStartProgram_buffer9[1];  iStartProgram_buffer9[0] = 1;
	if (!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_VEL_TEST_PROF_MOVE_2, "iAFT_StartPathMotion_buffer9", 0, 0, 0, 0, iStartProgram_buffer9, NULL ))
	{
		sprintf_s(strDebugText, 128, "download starting VelLoop PWN, Error Code: %d", acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug)	AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = AFT_PRBS_ERR_DOWNLOAD_ERROR;
	}

	return sRet;
}
// 
short aft_get_prbs_response(unsigned short usCountLenPRBS)
{
	short sRet = MTN_API_OK_ZERO;

	if(usCountLenPRBS > PRBS_FB_MAX_ARRAY_COL)
	{
		usActualUploadLen = PRBS_FB_MAX_ARRAY_COL;
	}
	else
	{
		usActualUploadLen = usCountLenPRBS;
	}

	if(!acsc_ReadReal(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "rAFT_Scope", 0, PRBS_FB_MAX_ARRAY_ROW -1, 0, usActualUploadLen -1, adSpectrumResponse, NULL))
	{
		sprintf_s(strDebugText, 128, "upload spectrum-test response error, Error Code: %d", acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug)	AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = AFT_PRBS_ERR_UPLOAD_DATA;
	}
	return sRet;
}
// aScopeSP, adVelScopeSP, PRBS_NUM_POINT_ARRAY_SPDC
short aft_get_prbs_sp_data(unsigned short usCountLenPRBS_SPDC)
{
	short sRet = MTN_API_OK_ZERO;

	if(usCountLenPRBS_SPDC > PRBS_NUM_POINT_ARRAY_SPDC)
	{
		usActualUploadSPDC = PRBS_NUM_POINT_ARRAY_SPDC;
	}
	else
	{
		usActualUploadSPDC = usCountLenPRBS_SPDC;
	}

	if(!acsc_ReadReal(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "aScopeSP", 0, 0, 0, usActualUploadSPDC -1, adVelScopeSP, NULL))
	{
		sprintf_s(strDebugText, 128, "upload aScopeSP spectrum-test response error, Error Code: %d", acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug)	AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = AFT_PRBS_ERR_UPLOAD_DATA;
	}
	return sRet;
}
#include "MotAlgo_DLL.h"

#define EFSIKA_WB_Z_AXIS  4
static double pdZerosArray[8]; 
static int iFlagDoingTest;
short aft_prbs_excite_once(PRBS_TEST_CONDITION *stpPrbsTestInput)
{
static unsigned short usCountLenPRBS;
static unsigned short usAmplitudePRBS;
static unsigned short usFreqFactorPRBS;
static int iSelChannel;
short sRet = MTN_API_OK_ZERO;

	usCountLenPRBS = stpPrbsTestInput->usCountLenPRBS;
	usAmplitudePRBS = stpPrbsTestInput->usAmplitudePRBS;
	iSelChannel = stpPrbsTestInput->uiExcitingAxis;

	aft_gen_rand_array(stpPrbsTestInput->uiRandSeed, usCountLenPRBS, usAmplitudePRBS);

	// 20090703
	if(iSelChannel == sys_get_acs_axis_id_bnd_z() && 
		cFlagVelocityLoopExciting == OPEN_LOOP_SPECTRUM_TEST)  // 20120108  EFSIKA_WB_Z_AXIS 
	{
		int iHoldCurrent;
		iHoldCurrent = aft_get_hold_drive_output(iSelChannel);
		// download random array
		sRet = aft_download_prbs_array_w_offset(usCountLenPRBS, iHoldCurrent);
	}
	else
	{
		sRet = aft_download_prbs_array(usCountLenPRBS);
	}

	if(sRet != MTN_API_OK_ZERO)
	{
		goto label_return_aft_prbs_excite_once;
	}

	// download PRBS parameters
	sRet = aft_download_prbs_parameter(stpPrbsTestInput);
	if(sRet != MTN_API_OK_ZERO)
	{
		goto label_return_aft_prbs_excite_once;
	}
	
	// start excitation once
	if(cFlagVelocityLoopExciting == OPEN_LOOP_SPECTRUM_TEST)
	{
		aft_start_prbs_excitation();
	}
	else
	{	//VEL_LOOP_SPECTRUM_TEST
		aft_start_vel_loop_prbs_excitation();
	}

    UINT uiSleepDelay = (UINT)(dIsrTime_ms * stpPrbsTestInput->usFreqFactorPRBS * stpPrbsTestInput->usCountLenPRBS);

	Sleep(uiSleepDelay);

	if(cFlagVelocityLoopExciting == OPEN_LOOP_SPECTRUM_TEST)
	{
		// clear DCOM to 0, 2010Feb04
		acsc_ReadReal(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "DCOM", 0, 0, 0, 7, pdZerosArray, NULL);
		while(fabs(pdZerosArray[stpPrbsTestInput->uiExcitingAxis]) >= 0.01)
		{
       		Sleep(20);
			acsc_ReadReal(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "DCOM", 0, 0, 0, 7, pdZerosArray, NULL);

		}
	}
	else
	{
		// clear DCOM to 0, 2010Feb04
		acsc_ReadInteger(stCommHandleACS, BUFFER_ID_VEL_TEST_PROF_MOVE_2, "iFlagRunningBufferProg", 0, 0, 0, 0, &iFlagDoingTest, NULL);
		while(iFlagDoingTest)
		{
       		Sleep(20);
			acsc_ReadInteger(stCommHandleACS, BUFFER_ID_VEL_TEST_PROF_MOVE_2, "iFlagRunningBufferProg", 0, 0, 0, 0, &iFlagDoingTest, NULL);
		}
	}
	// 
	aft_get_prbs_response(usCountLenPRBS);
	if(1500 > uiSleepDelay)
	{
		Sleep(1500 - uiSleepDelay);  // Capture 30000 points at 20KHz, around 1500ms (1.5 sec)
	}
	aft_get_prbs_sp_data(PRBS_NUM_POINT_ARRAY_SPDC);

label_return_aft_prbs_excite_once:

	return sRet;

}

static SINE_SWEEP_GEN_EXCITE_ONE_FREQ_OUT stSineSweepOneExciteFreqOut;

short aft_sinsweep_excite_once(SINE_SWEEP_ONE_EXCITE *stpSineSweepInput)  // stpPrbsTestInput
{
static unsigned short usCountLenSineSweep;
static double dAmplitudeSineSweep;
static unsigned short usFreqFactorPRBS;
static int iSelChannel;
short sRet = MTN_API_OK_ZERO;

//stpSineSweepInput->stSineSweepOnceExciteFreq.fFreq_Hz = 
	aft_sine_sweep_gen_excite_cmd_one(&(stpSineSweepInput->stSineSweepOnceExciteFreq), &stSineSweepOneExciteFreqOut);
	
	usCountLenSineSweep = (unsigned short)stSineSweepOneExciteFreqOut.uiLen; // stpSineSweepInput->usCountLenSineSweep;
//	dAmplitudeSineSweep = stpSineSweepInput->dAmplitudeSineSweep;
	iSelChannel = stpSineSweepInput->uiExcitingAxis;

	sRet = aft_download_sine_sweep_array(&stSineSweepOneExciteFreqOut);
	if(sRet != MTN_API_OK_ZERO)
	{
		goto label_return_aft_sinsweep_excite_once;
	}

	PRBS_TEST_CONDITION stSpectrumTestCondition;
	stSpectrumTestCondition.uiExcitingAxis = stpSineSweepInput->uiExcitingAxis;
	stSpectrumTestCondition.usCountLenPRBS = (unsigned short)stSineSweepOneExciteFreqOut.uiLen;
	stSpectrumTestCondition.uiRandSeed = 0;
	stSpectrumTestCondition.usAmplitudePRBS = 1;
	stSpectrumTestCondition.usFreqFactorPRBS = 1;
	// download PRBS parameters
	sRet = aft_download_prbs_parameter(&stSpectrumTestCondition);
	if(sRet != MTN_API_OK_ZERO)
	{
		goto label_return_aft_sinsweep_excite_once;
	}
	
	// start excitation once
	//if(cFlagVelocityLoopExciting == OPEN_LOOP_SPECTRUM_TEST)
	//{
		aft_start_prbs_excitation();
	//}
	//else
	//{	//VEL_LOOP_SPECTRUM_TEST
	//	aft_start_vel_loop_prbs_excitation();
	//}

	UINT uiSleepDelay = (UINT)(stpSineSweepInput->stSineSweepOnceExciteFreq.fTs_ms) * stSineSweepOneExciteFreqOut.uiLen;

	Sleep(uiSleepDelay);

	if(cFlagVelocityLoopExciting == OPEN_LOOP_SPECTRUM_TEST)
	{
		// clear DCOM to 0, 2010Feb04
		acsc_ReadReal(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "SLIOFFS", 0, 0, 0, 7, pdZerosArray, NULL); // DCOM
		while(fabs(pdZerosArray[iSelChannel]) >= 0.01)
		{
       		Sleep(20);
			acsc_ReadReal(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "SLIOFFS", 0, 0, 0, 7, pdZerosArray, NULL);  // DCOM

		}
	}
	else
	{
		// clear DCOM to 0, 2010Feb04
		acsc_ReadInteger(stCommHandleACS, BUFFER_ID_VEL_TEST_PROF_MOVE_2, "iFlagRunningBufferProg", 0, 0, 0, 0, &iFlagDoingTest, NULL);
		while(iFlagDoingTest)
		{
       		Sleep(20);
			acsc_ReadInteger(stCommHandleACS, BUFFER_ID_VEL_TEST_PROF_MOVE_2, "iFlagRunningBufferProg", 0, 0, 0, 0, &iFlagDoingTest, NULL);
		}
	}
	// 
	if(1500 > uiSleepDelay)
	{
		Sleep(1500 - uiSleepDelay);  // Capture 30000 points at 20KHz, around 1500ms (1.5 sec)
	}
	aft_get_prbs_response(usCountLenSineSweep);
	aft_get_prbs_sp_data(PRBS_NUM_POINT_ARRAY_SPDC);

label_return_aft_sinsweep_excite_once:

	return sRet;
}

#define SPEC_TEST_TIME_OUT_SEC     10
#define NUM_SAMPLE_GET_IDLE_HOLD_CURRENT  100
int aft_get_hold_drive_output(int iAxisInCtrl)
{
double dHoldCurrent;
int ii = 0;
int iHoldCurrentTemp[1];
int iHoldCurrentAve;
//	int iMotorState;
	dHoldCurrent = 0;

//	mtnapi_get_motor_state(stCommHandleACS, iAxisInCtrl, &iMotorState, 0); // iMotorState & ACSC_MST_INPOS donot check settling
    while(ii < NUM_SAMPLE_GET_IDLE_HOLD_CURRENT)
	{
		acsc_ReadInteger(stCommHandleACS, NULL, "DOUT", iAxisInCtrl, iAxisInCtrl, 0, 0, &iHoldCurrentTemp[0], NULL);
		ii ++;
		dHoldCurrent = dHoldCurrent + iHoldCurrentTemp[0];
		Sleep(1);
	}
	iHoldCurrentAve = (int)(dHoldCurrent / NUM_SAMPLE_GET_IDLE_HOLD_CURRENT);

	return iHoldCurrentAve;
}
//#include "MotAlgo_DLL.h"
//extern char sys_acs_communication_get_flag_sc_udi();

short aft_group_prbs_move_and_excite_once(PRBS_GROUP_TEST_INPUT *stpGroupPrbsInput)
{
	short sRet = MTN_API_OK_ZERO;
	int iMotorState;
	unsigned int jj, uiCurrAxis;

	//
	for(jj=0; jj < stpGroupPrbsInput->usActualNumAxis; jj++)  
//	for(jj = stpGroupPrbsInput->usActualNumAxis - 1; jj>=0; jj--)  // 20110709
	{
		uiCurrAxis = stpGroupPrbsInput->uiAxis_CtrlCardId[jj];
		if(sys_acs_communication_get_flag_sc_udi() == 0)  // 20121203
		{
			if(uiCurrAxis != sys_get_acs_axis_id_bnd_z() && stpGroupPrbsInput->iFlagHasBondHeadApp == TRUE)   // 20110709
			{
				// Move BondHead first to safe position
				acsc_ToPoint(stCommHandleACS, 0, // start up the motion immediately
							sys_get_acs_axis_id_bnd_z(), 
							mtn_wb_init_bh_relax_position_from_sp(stCommHandleACS), // 20110806
							NULL);
				Sleep(500);

			}  // 20110709
		}
		acsc_ToPoint(stCommHandleACS, 0, // start up immediately the motion
			uiCurrAxis, stpGroupPrbsInput->iAxisPosn[jj], NULL);  

		// wait until motion ends
		mtnapi_get_motor_state(stCommHandleACS, uiCurrAxis, &iMotorState, 0);
    	mtn_cto_tick_start_time_u1s(liFreqOS_SpecTest); // uiTimeOutCntStart = GetPentiumTimeCount_per_100us(liFreqOS.QuadPart); // 20090508
		while(iMotorState & ACSC_MST_MOVE) //
		{
			if(uiCurrAxis == sys_get_acs_axis_id_bnd_z())  // 20110709
			{ // Needs to check the stability
				Sleep(50);
			}
			else
			{
				Sleep(2);
			}
			mtnapi_get_motor_state(stCommHandleACS, uiCurrAxis, &iMotorState, 0);
			// Check time-out
			if(mtn_cto_is_time_out_u1s(liFreqOS_SpecTest, SPEC_TEST_TIME_OUT_SEC))
			{
				sRet = AFT_SPEC_TEST_ERR_TIME_OUT;
				sprintf_s(strDebugText, 128, "Error Limit- Timeout: [CntStart: %d, CntCurr: %d, TimeOut: %d], Not Settling-1! [%s %d] \n",	
					mtn_cto_get_start_cnt(), mtn_cto_get_curr_cnt(), SPEC_TEST_TIME_OUT_SEC,__FILE__, __LINE__);
				goto label_return_aft_group_prbs_move_and_excite_once;
			} 
		}

		if(!(iMotorState & ACSC_MST_ENABLE))
		{
			sRet = AFT_SPEC_TEST_ERR_MOTOR_DISABLED;
		}
	}
#ifdef __WAIT_SETTLING__
	uiCurrAxis = stpGroupPrbsInput->stPrbsTestOnceInput.uiExcitingAxis;   // 20110709
	if(uiCurrAxis == sys_get_acs_axis_id_bnd_z())
	{ // Needs to check the stability
		if(mtnapi_wait_axis_settling(stCommHandleACS, uiCurrAxis, 20) != MTN_API_OK_ZERO)
		{
			sRet = MTN_API_ERROR_ACS_ERROR_SETTLING;//
			goto label_return_aft_group_prbs_move_and_excite_once;
		}
	}   // 20110709
#endif // __WAIT_SETTLING__
	sRet = aft_prbs_excite_once(&(stpGroupPrbsInput->stPrbsTestOnceInput));

	static char strFilename[256] ;
			// upload data
	sprintf_s(strFilename , 256, "%s\\PrbsRespAryCase_%d.m", strSpectrumNewFolderName, nCurrCaseIdxGroupSpecTest); // PrbsRespAryCase_%d.m", nCurrCaseIdxGroupSpecTest);
	aft_spec_test_dlg_save_response_memory_to_file(strFilename, stpGroupPrbsInput->stPrbsTestOnceInput);  // stpPrbsGroupTestCase[nCurrCaseIdxGroupSpecTest].stPrbsTestOnceInput

label_return_aft_group_prbs_move_and_excite_once:
	return sRet;

}

//
void aft_convert_group_prbs_cfg_to_test_input(SPECTRUM_SYS_ANA_GROUP_CONFIG stPRBS_GroupTestConfig, 
											  PRBS_GROUP_TEST_INPUT* stpPrbsGroupTestCase,
											  unsigned int uiRandSeed)
{
static	unsigned int uiAxis_CtrlCardId[MAX_NUM_AXIS_GROUP_SPECTRUM];
static	int iLowerLimitPosition[MAX_NUM_AXIS_GROUP_SPECTRUM];
static	int iUpperLimitPosition[MAX_NUM_AXIS_GROUP_SPECTRUM];
static	unsigned int uiNumPoints[MAX_NUM_AXIS_GROUP_SPECTRUM];
static	unsigned char ucFlagIsExciteAxis[MAX_NUM_AXIS_GROUP_SPECTRUM];
static	unsigned short usAxisAmpPrbs[MAX_NUM_AXIS_GROUP_SPECTRUM];
static	unsigned short usAxisPrbsLen[MAX_NUM_AXIS_GROUP_SPECTRUM];
static	unsigned short usAxisPrbsFreqFactor[MAX_NUM_AXIS_GROUP_SPECTRUM];
unsigned int ii, jj, iCase;
int iFlagHasBondHeadApp = FALSE;  // 20110719
	for(ii = 0; ii< MAX_NUM_AXIS_GROUP_SPECTRUM; ii++)
	{
		// 
		uiAxis_CtrlCardId[ii] = stPRBS_GroupTestConfig.uiAxis_CtrlCardId[ii];
		iLowerLimitPosition[ii] = stPRBS_GroupTestConfig.iLowerLimitPosition[ii];
		iUpperLimitPosition[ii] = stPRBS_GroupTestConfig.iUpperLimitPosition[ii];
		ucFlagIsExciteAxis[ii] = stPRBS_GroupTestConfig.ucFlagIsExciteAxis[ii];
		uiNumPoints [ii] = stPRBS_GroupTestConfig.uiNumPoints[ii];
		usAxisAmpPrbs[ii] = stPRBS_GroupTestConfig.usAxisAmpPrbs[ii];
		usAxisPrbsFreqFactor[ii] = stPRBS_GroupTestConfig.usAxisPrbsFreqFactor[ii];
		usAxisPrbsLen[ii] = stPRBS_GroupTestConfig.usAxisPrbsLen[ii];
		if(uiAxis_CtrlCardId[ii] == sys_get_acs_axis_id_bnd_z())  // 20110719
		{
			iFlagHasBondHeadApp = TRUE;
		}  // 20110719
	}

	static unsigned short nTotalEnvolvedAxis, nTotalExciteAxis, nTotalNumGroupPoints;
	static unsigned short nTotalNumCasesGroupPrbsTest;
	nTotalEnvolvedAxis = 0; nTotalExciteAxis= 0; nTotalNumCasesGroupPrbsTest = 1; nTotalNumGroupPoints = 1;
	// spanning for nTotalEnvolvedAxis
	static int aiExciteAxisListInGroup[MAX_NUM_AXIS_GROUP_SPECTRUM], aiEnvolvedAxisList[MAX_NUM_AXIS_GROUP_SPECTRUM], aiNumPointsActiveAxis[MAX_NUM_AXIS_GROUP_SPECTRUM];
	static int iDeltaDistance[MAX_NUM_AXIS_GROUP_SPECTRUM], iCurrAxis, iCurrPointIdInAxis, iCurrTestPosn[MAX_NUM_AXIS_GROUP_SPECTRUM];
	static int iCurrLowLimitEnvolveAxis[MAX_NUM_AXIS_GROUP_SPECTRUM];  // 20110719, iCurrUppLimitEnvolveAxis[MAX_NUM_AXIS_GROUP_SPECTRUM];
	iCurrAxis = MAX_CTRL_AXIS_PER_SERVO_BOARD; // dummy
	for(ii = 0; ii< MAX_NUM_AXIS_GROUP_SPECTRUM; ii++)
	{
			// init
		iDeltaDistance[ii] = 0;
		if(uiAxis_CtrlCardId[ii] < MAX_CTRL_AXIS_PER_SERVO_BOARD && uiNumPoints[ii] > 0)
		{
			nTotalNumGroupPoints = nTotalNumGroupPoints * uiNumPoints[ii];

			aiEnvolvedAxisList[nTotalEnvolvedAxis] = uiAxis_CtrlCardId[ii];
			iCurrTestPosn[nTotalEnvolvedAxis] = iLowerLimitPosition[ii];
			iCurrLowLimitEnvolveAxis[nTotalEnvolvedAxis] = iLowerLimitPosition[ii];
			aiNumPointsActiveAxis[nTotalEnvolvedAxis] = uiNumPoints[ii];
			if(uiNumPoints[ii] > 1)
			{
				iDeltaDistance[nTotalEnvolvedAxis] = (int)((iUpperLimitPosition[ii] - iLowerLimitPosition[ii])/ (uiNumPoints[ii] - 1.0));
			}
			else
			{
				iDeltaDistance[nTotalEnvolvedAxis] = (iUpperLimitPosition[ii] - iLowerLimitPosition[ii]) >> 1;
			}

			nTotalEnvolvedAxis ++;

			if(iCurrAxis == MAX_CTRL_AXIS_PER_SERVO_BOARD)
			{
				iCurrAxis = uiAxis_CtrlCardId[ii];

			}
		}
		if(ucFlagIsExciteAxis[ii] >0 && uiAxis_CtrlCardId[ii] < MAX_CTRL_AXIS_PER_SERVO_BOARD)
		{
			aiExciteAxisListInGroup[nTotalExciteAxis] = ii; // uiAxis_CtrlCardId[ii]
			nTotalExciteAxis = nTotalExciteAxis + 1;
		}

	}
	nTotalNumCasesGroupPrbsTest = nTotalExciteAxis * nTotalNumGroupPoints;

	iCase= 0;
	for(ii = 0; ii<nTotalNumCasesGroupPrbsTest; ii++)
	{
		for(jj = 0; jj < nTotalEnvolvedAxis; jj++)
		{
			stpPrbsGroupTestCase[ii].uiAxis_CtrlCardId[jj] = aiEnvolvedAxisList[jj];
		}

		stpPrbsGroupTestCase[ii].iFlagHasBondHeadApp = iFlagHasBondHeadApp;  // 20110719

	}

	iCurrPointIdInAxis = 0;
	static unsigned int iRemainder, iQuotient;

	for(ii = 0; ii< nTotalNumGroupPoints; ii ++)
	{

		// spanning nTotalEnvolvedAxis- dimensional space by Quotient and Remainder 
		iQuotient = ii;
		for(jj = 0; jj < nTotalEnvolvedAxis; jj ++)
		{
			iRemainder = iQuotient % aiNumPointsActiveAxis[jj];
			iCurrTestPosn[jj] = iRemainder * iDeltaDistance[jj] + iCurrLowLimitEnvolveAxis[jj];  // 20110719 correct BUG, iLowerLimitPosition[jj];

			iQuotient = iQuotient / aiNumPointsActiveAxis[jj];
		}

		for(jj = 0; jj < nTotalExciteAxis; jj++)
		{
			int iExciteAxis;
			stpPrbsGroupTestCase[iCase].usActualNumAxis = nTotalEnvolvedAxis;
			stpPrbsGroupTestCase[iCase].stPrbsTestOnceInput.uiRandSeed = uiRandSeed;
			iExciteAxis = aiExciteAxisListInGroup[jj];
			stpPrbsGroupTestCase[iCase].stPrbsTestOnceInput.uiExcitingAxis = uiAxis_CtrlCardId[iExciteAxis];
			stpPrbsGroupTestCase[iCase].stPrbsTestOnceInput.usAmplitudePRBS = stPRBS_GroupTestConfig.usAxisAmpPrbs[iExciteAxis];
			stpPrbsGroupTestCase[iCase].stPrbsTestOnceInput.usCountLenPRBS = stPRBS_GroupTestConfig.usAxisPrbsLen[iExciteAxis];
			stpPrbsGroupTestCase[iCase].stPrbsTestOnceInput.usFreqFactorPRBS = stPRBS_GroupTestConfig.usAxisPrbsFreqFactor[iExciteAxis];
			for(int ll = 0; ll<nTotalEnvolvedAxis; ll++)
			{
				stpPrbsGroupTestCase[iCase].iAxisPosn[ll] = iCurrTestPosn[ll];
			}
			iCase ++;
		}
	}

}

void aft_white_noise_spectrum_make_master_file(SPECTRUM_SYS_ANA_GROUP_CONFIG stPRBS_GroupTestConfig, 
											  PRBS_GROUP_TEST_INPUT* stpPrbsGroupTestCase)
{
    
unsigned int ii, jj;

	FILE *fpData;
char strFileFullPathname[256];
	sprintf_s(strFileFullPathname, 256, "%s\\PrbsGroupTestCases.txt", strSpectrumNewFolderName);
	fopen_s(&fpData, strFileFullPathname, "w");  // PrbsGroupTestCases.txt

	if(fpData != NULL)
	{
		fprintf(fpData, "fFreqBasic = %6.2f;\n", stPRBS_GroupTestConfig.dFreqControl_Hz);
		fprintf(fpData, "nTotalCaseGroupPrbs = %d;\n", nTotalNumCasesGroupSpecTest);
		fprintf(fpData, "MachineType = %d;\n", get_sys_machine_type_flag());
		for(ii = 0; ii<nTotalNumCasesGroupSpecTest; ii++)
		{
			fprintf(fpData, "%% SpectrumTest - Case %d / Total %d \n", ii , nTotalNumCasesGroupSpecTest);
			fprintf(fpData, "TotalAxis = %d;\n", stpPrbsGroupTestCase[ii].usActualNumAxis);  // stpVolatilePrbsGroupTestCase
			fprintf(fpData, "aAxisPositions = [");
			for(jj = 0; jj < (unsigned int)(stpPrbsGroupTestCase[ii].usActualNumAxis - 1); jj++)
			{
				fprintf(fpData, "%d, ", stpPrbsGroupTestCase[ii].iAxisPosn[jj]);
			}
			fprintf(fpData, "%d];\n ", stpPrbsGroupTestCase[ii].iAxisPosn[jj]);
			fprintf(fpData, "aAxisInCtrl = [");
			for(jj = 0; jj < (unsigned int)(stpPrbsGroupTestCase[ii].usActualNumAxis - 1); jj++)
			{
				fprintf(fpData, "%d, ", stpPrbsGroupTestCase[ii].uiAxis_CtrlCardId[jj]);
			}
			fprintf(fpData, "%d];\n ", stpPrbsGroupTestCase[ii].uiAxis_CtrlCardId[jj]);

			fprintf(fpData, "iExcitingAxis = %d;\n", stpPrbsGroupTestCase[ii].stPrbsTestOnceInput.uiExcitingAxis);
			fprintf(fpData, "iSeedRandomNumber = %d;\n", stpPrbsGroupTestCase[ii].stPrbsTestOnceInput.uiRandSeed);
			fprintf(fpData, "usBinaryAmplitude = %d;\n", stpPrbsGroupTestCase[ii].stPrbsTestOnceInput.usAmplitudePRBS);
			fprintf(fpData, "usSpectrumLength = %d; \n", stpPrbsGroupTestCase[ii].stPrbsTestOnceInput.usCountLenPRBS);
			fprintf(fpData, "usFrequencyFactor = %d; \n", stpPrbsGroupTestCase[ii].stPrbsTestOnceInput.usFreqFactorPRBS);
			fprintf(fpData, "\n");

		}
		// iPlotFlag
		fprintf(fpData, "iPlotFlag = %d \n", aft_spectrum_get_debug_flag());

		// Save the PC-memory
		fclose(fpData);
	}

}
static 	double 	mdRealPrbsResp[PRBS_FB_MAX_ARRAY_ROW * PRBS_FB_MAX_ARRAY_COL]; // matrix of 2-dim
static  double  adPrbsRespVelSP[PRBS_NUM_POINT_ARRAY_SPDC];

void aft_spec_test_dlg_save_response_memory_to_file(char *strFilename, PRBS_TEST_CONDITION stPrbsTestInput)
{
	unsigned short usRow;
	unsigned short usCol;
	// Get response array
	aft_get_prbs_response_array(&mdRealPrbsResp[0], &usRow, &usCol);

	unsigned short usLen;
	aft_get_prbs_sp_vel_response(adPrbsRespVelSP, &usLen);

	double dSamplePeriod_ms;
	aft_get_sample_period_ms(&dSamplePeriod_ms);


	// Save to file
	FILE *fpData;
	fopen_s(&fpData, strFilename, "w");

	if(fpData != NULL)
	{
		// Save the response
		fprintf(fpData, "%% Spectrum Test Condition\n");
		fprintf(fpData, "iExcitingAxis = %d;\n", stPrbsTestInput.uiExcitingAxis);
		fprintf(fpData, "iFlagVelLoopExcite = %d;\n", aft_spectrum_get_flag_velloop());
		fprintf(fpData, "iSeedRandomNumber = %d;\n", stPrbsTestInput.uiRandSeed);
		fprintf(fpData, "usBinaryAmplitude = %d;\n", stPrbsTestInput.usAmplitudePRBS);
		fprintf(fpData, "usSpectrumLength = %d; \n", stPrbsTestInput.usCountLenPRBS);
		fprintf(fpData, "usFrequencyFactor = %d; \n", stPrbsTestInput.usFreqFactorPRBS);
		fprintf(fpData, "dActuralSamplePeriod_ms = %4.2f; \n", dSamplePeriod_ms);
		fprintf(fpData, "dCTIME_ms = %4.2f; \n", aft_get_basic_time_unit());

		fprintf(fpData, "\n\n%% Pseudo-Random-Binary-Sequence Test Response\n");
		fprintf(fpData, "%% FdVel-1,  FdVel-2,  FdVel-3, Dac-1, Dac-2, Dac-3, FdPos-ExAx \n");
		fprintf(fpData, "matRespPRBS = [ ");
		int ii, iStartRow; iStartRow= 0;
		for(ii=0; ii<usCol; ii++)
		{
				fprintf(fpData, "%f, %f, %f, %f, %f, %f", //, %f
						mdRealPrbsResp[iStartRow], mdRealPrbsResp[ iStartRow + 1], mdRealPrbsResp[iStartRow + 2], 
						mdRealPrbsResp[3 + iStartRow], mdRealPrbsResp[4 + iStartRow], mdRealPrbsResp[5 + iStartRow]);
						// mdRealPrbsResp[6 + iStartRow]);
			iStartRow = iStartRow + PRBS_FB_MAX_ARRAY_ROW;

			if(ii == usCol - 1)
			{ // last row
				fprintf(fpData, "];\n");
			}
			else
			{  // otherwise
				fprintf(fpData, ";\n");
			}
		}

		fprintf(fpData, "adRespPosnSP = [ ");
		for(ii=0; ii<usLen; ii++)
		{
			//if( aft_spectrum_get_flag_dsp_data() == 0)
			//{			
			//	fprintf(fpData, "0");
			//}
			//else
			//{
				fprintf(fpData, "%f", adPrbsRespVelSP[ii]);
			//}
			if(ii == usLen - 1)
			{ // last row
				fprintf(fpData, "];\n");
			}
			else
			{  // otherwise
				fprintf(fpData, ";\n");
			}
		} // adPrbsRespVelSP, &usLen
		// Save the PC-memory

		fclose(fpData);
	}

}

void aft_spectrum_sinesweep_save_response_memory_to_file(char *strFilename, SINE_SWEEP_ONE_EXCITE *stpSineSweepOneExcite)  // stPrbsTestInput
{
	unsigned short usRow;
	unsigned short usCol;
	// Get response array
	aft_get_prbs_response_array(&mdRealPrbsResp[0], &usRow, &usCol);

	unsigned short usLen;
	aft_get_prbs_sp_vel_response(adPrbsRespVelSP, &usLen);

	double dSamplePeriod_ms;
	aft_get_sample_period_ms(&dSamplePeriod_ms);


	// Save to file
	FILE *fpData;
	fopen_s(&fpData, strFilename, "w");

	if(fpData != NULL)
	{
		// Save the response
		fprintf(fpData, "%% Spectrum Test Condition\n");
		fprintf(fpData, "iExcitingAxis = %d;\n", stpSineSweepOneExcite->uiExcitingAxis); // stPrbsTestInput.uiExcitingAxis);
		fprintf(fpData, "iFlagVelLoopExcite = %d;\n", aft_spectrum_get_flag_velloop());
		fprintf(fpData, "fAmplitude = %5.1f;\n", stpSineSweepOneExcite->stSineSweepOnceExciteFreq.fAmplitude);  // stPrbsTestInput.usAmplitudePRBS);
		fprintf(fpData, "dFreq_Hz = %5.1f; \n", stpSineSweepOneExcite->stSineSweepOnceExciteFreq.fFreq_Hz); // stPrbsTestInput.usCountLenPRBS);
		fprintf(fpData, "dActuralSamplePeriod_ms = %4.2f; \n", dSamplePeriod_ms);
		fprintf(fpData, "dCTIME_ms = %4.2f; \n", aft_get_basic_time_unit());

		fprintf(fpData, "\n\n%% Pseudo-Random-Binary-Sequence Test Response\n");
		fprintf(fpData, "%% FdVel-1,  FdVel-2,  FdVel-3, Dac-1, Dac-2, Dac-3, FdPosn-ExAx \n");
		fprintf(fpData, "matRespPRBS = [ ");
		int ii, iStartRow; iStartRow= 0;
		for(ii=0; ii<usCol; ii++)
		{
				fprintf(fpData, "%f, %f, %f, %f, %f, %f, %f",
						mdRealPrbsResp[iStartRow], mdRealPrbsResp[ iStartRow + 1], mdRealPrbsResp[iStartRow + 2], 
						mdRealPrbsResp[3 + iStartRow], mdRealPrbsResp[4 + iStartRow], mdRealPrbsResp[5 + iStartRow],
						mdRealPrbsResp[6 + iStartRow]);
			iStartRow = iStartRow + PRBS_FB_MAX_ARRAY_ROW;

			if(ii == usCol - 1)
			{ // last row
				fprintf(fpData, "];\n");
			}
			else
			{  // otherwise
				fprintf(fpData, ";\n");
			}
		}

		//fprintf(fpData, "adRespPosnSP = [ ");
		//for(ii=0; ii<usLen; ii++)
		//{
		//	fprintf(fpData, "%f", adPrbsRespVelSP[ii]);
		//	if(ii == usLen - 1)
		//	{ // last row
		//		fprintf(fpData, "];\n");
		//	}
		//	else
		//	{  // otherwise
		//		fprintf(fpData, ";\n");
		//	}
		//} // adPrbsRespVelSP, &usLen
		//// Save the PC-memory

		fclose(fpData);
	}

}
static unsigned int jCaseFreqSineSweep;
int aft_spectrum_get_sine_sweep_current_case_freq()
{
	return jCaseFreqSineSweep;
}

#include "MtnApi.h"
#include "WinTiming.h"

short aft_group_sine_sweep_move_and_excite_multi_freq(SINESWEEP_GROUP_TEST_INPUT *stpGroupSineSweepInput)
{
	static int aiFbPosnTestSpectrum[MAX_NUM_AXIS_GROUP_SPECTRUM];
	static double adFbPosnTestSpectrum[MAX_NUM_AXIS_GROUP_SPECTRUM];
	short sRet = MTN_API_OK_ZERO;
	int iMotorState;
	unsigned int jj, uiCurrAxis;
	for(jj=0; jj < stpGroupSineSweepInput->usActualNumAxis; jj++)
	{
		if(mFlagStopSpectrumTestThread == TRUE)
		{
			break; 
		}
		uiCurrAxis = stpGroupSineSweepInput->uiAxis_CtrlCardId[jj];
		acsc_ToPoint(stCommHandleACS, 0, // start up immediately the motion
			uiCurrAxis, stpGroupSineSweepInput->iAxisPosn[jj], NULL);
		aiFbPosnTestSpectrum[jj] = stpGroupSineSweepInput->iAxisPosn[jj];
		adFbPosnTestSpectrum[jj] = (double)aiFbPosnTestSpectrum[jj];

		// wait until motion ends
		mtnapi_get_motor_state(stCommHandleACS, uiCurrAxis, &iMotorState, 0);
    	mtn_cto_tick_start_time_u1s(liFreqOS_SpecTest); // uiTimeOutCntStart = GetPentiumTimeCount_per_100us(liFreqOS.QuadPart); // 20090508
		while(iMotorState & ACSC_MST_MOVE) //
		{
			Sleep(2);
			mtnapi_get_motor_state(stCommHandleACS, uiCurrAxis, &iMotorState, 0);
			// Check time-out
			if(mtn_cto_is_time_out_u1s(liFreqOS_SpecTest, SPEC_TEST_TIME_OUT_SEC))
			{
				sRet = AFT_SPEC_TEST_ERR_TIME_OUT;
				sprintf_s(strDebugText, 128, "Error Limit- Timeout: [CntStart: %d, CntCurr: %d, TimeOut: %d], Not Settling-1! [%s %d] \n",	
					mtn_cto_get_start_cnt(), mtn_cto_get_curr_cnt(), SPEC_TEST_TIME_OUT_SEC,__FILE__, __LINE__);
				goto label_return_aft_group_sine_sweep_move_and_excite_multi_freq;
			} 
		}

		if(!(iMotorState & ACSC_MST_ENABLE))
		{
			sRet = AFT_SPEC_TEST_ERR_MOTOR_DISABLED;
//			goto label_return_aft_group_sine_sweep_move_and_excite_multi_freq;
		}
	}
	// Save to file
struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;
	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);

	stpGroupSineSweepInput->stSineSweepExciteOnce.stSineSweepOnceExciteFreq.dNumCycle = 10;
	stpGroupSineSweepInput->stSineSweepExciteOnce.stSineSweepOnceExciteFreq.fAmplitude = stpGroupSineSweepInput->stSinSweepAxisCfg.dAmpSineSweepRatio /100.0 * dMaxDacValueInController;
	stpGroupSineSweepInput->stSineSweepExciteOnce.stSineSweepOnceExciteFreq.fTs_ms = 0.5;
//	static SINE_SWEEP_ONE_EXCITE stSineSweepOneExcite;
char strFileFullPathname[512];
	sprintf_s(strFileFullPathname, 512, "%s\\MasterSineSwp_%d.%d.%d.H%d.M%d.txt", strSpectrumNewFolderName,
		stpTime->tm_year +1900, stpTime->tm_mon +1, stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min);  // strSpectrumNewFolderName
	aft_spectrum_sinesweep_save_master_cfg(strFileFullPathname, stpGroupSineSweepInput);
int ii;
static char strFilename[256];

	for(jCaseFreqSineSweep=0; jCaseFreqSineSweep<stpGroupSineSweepInput->stSinSweepAxisCfg.uiTotalCaseFreq; jCaseFreqSineSweep++) // 20110502
	{
		for(ii=0; ii<stpGroupSineSweepInput->usActualNumAxis; ii++)
		{
			uiCurrAxis = stpGroupSineSweepInput->uiAxis_CtrlCardId[ii];
			if(uiCurrAxis != stpGroupSineSweepInput->stSineSweepExciteOnce.uiExcitingAxis)
			{
				mtnapi_disable_motor(stCommHandleACS, uiCurrAxis, 0);
				mtnapi_get_fb_position(stCommHandleACS, uiCurrAxis, &adFbPosnTestSpectrum[ii], 0);
				aiFbPosnTestSpectrum[ii] = (int)adFbPosnTestSpectrum[ii];
			}			
		}
		Sleep(100); // 20120808
		stpGroupSineSweepInput->stSineSweepExciteOnce.stSineSweepOnceExciteFreq.fFreq_Hz = stpGroupSineSweepInput->stSinSweepAxisCfg.pdFreqArray[jCaseFreqSineSweep];		// 20110502

		if(mFlagStopSpectrumTestThread == TRUE) // External Stop
		{
			break; 
		}
		else
		{
			if(aft_sinsweep_excite_once(&(stpGroupSineSweepInput->stSineSweepExciteOnce)) != MTN_API_OK_ZERO)
			{
				mFlagStopSpectrumTestThread = TRUE;
				break;   // Error Stop
			}
		}

		for(ii=0; ii<stpGroupSineSweepInput->usActualNumAxis; ii++)
		{
			uiCurrAxis = stpGroupSineSweepInput->uiAxis_CtrlCardId[ii];
			if(uiCurrAxis != stpGroupSineSweepInput->stSineSweepExciteOnce.uiExcitingAxis)
			{
				mtnapi_enable_motor(stCommHandleACS, uiCurrAxis, 0);
//				Sleep(100);
				acsc_ToPoint(stCommHandleACS, 0, // start up immediately the motion
					uiCurrAxis, aiFbPosnTestSpectrum[ii], NULL);

			}
		}
		//// upload data
		sprintf_s(strFilename , 256, "%s\\SineSweepRespAryCase_%d.m", strSpectrumNewFolderName, jCaseFreqSineSweep); // 20110502 // nCurrCaseIdxGroupSpecTest); // PrbsRespAryCase_%d.m", nCurrCaseIdxGroupSpecTest);
		aft_spectrum_sinesweep_save_response_memory_to_file(strFilename, &stpGroupSineSweepInput->stSineSweepExciteOnce); // stpPrbsGroupTestCase[nCurrCaseIdxGroupSpecTest].stPrbsTestOnceInput);
		
	}

label_return_aft_group_sine_sweep_move_and_excite_multi_freq: // _aft_group_prbs_move_and_excite_once:
	return sRet;

}

void aft_spectrum_sinesweep_save_master_cfg(char strFilename[], SINESWEEP_GROUP_TEST_INPUT *stpGroupSineSweepInput)
{
	FILE *fpData;

	fopen_s(&fpData, strFilename, "w");

	int iTotalFreqCases  = stpGroupSineSweepInput->stSinSweepAxisCfg.uiTotalCaseFreq;

	if(fpData != NULL)
	{
		int ii;
		for(ii = 0; ii<stpGroupSineSweepInput->usActualNumAxis; ii++)
		{
			fprintf(fpData, "stSineSweepCfg.iAxisIdCtrlCard(%d) = %d;\n", ii+1, stpGroupSineSweepInput->uiAxis_CtrlCardId[ii]);
			fprintf(fpData, "stSineSweepCfg.iAxisPosn(%d) = %d;\n", ii+1, stpGroupSineSweepInput->iAxisPosn[ii]);
		}
		fprintf(fpData, "iExciteAxisCtrlCard = %d;\n", stpGroupSineSweepInput->stSineSweepExciteOnce.uiExcitingAxis);
		fprintf(fpData, "fBasicSampleTime_ms = %6.2f;\n", stpGroupSineSweepInput->stSineSweepExciteOnce.stSineSweepOnceExciteFreq.fTs_ms); //.);
		fprintf(fpData, "nTotalCaseSineSweep = %d;\n", iTotalFreqCases);
		fprintf(fpData, "MachineType = %d;\n", get_sys_machine_type_flag());

		for(ii = 0; ii<iTotalFreqCases; ii++)
		{
			double dFreqHzAtCase;
			fprintf(fpData, "%% SineSweepTest - Case %d / Total %d \n", ii , iTotalFreqCases);
			fprintf(fpData, "astSineSweepCase(%d).fAmplitudeRatio16bDAC = %6.1f;\n", ii+1, stpGroupSineSweepInput->stSinSweepAxisCfg.dAmpSineSweepRatio);
			fprintf(fpData, "astSineSweepCase(%d).usSpectrumLength = %d; \n", ii+1, 4200);

			dFreqHzAtCase = stpGroupSineSweepInput->stSinSweepAxisCfg.pdFreqArray[ii];
			fprintf(fpData, "astSineSweepCase(%d).fFreqHz = %5.1f; \n", ii+1, dFreqHzAtCase);
			// Estimate
			fprintf(fpData, "astSineSweepCase(%d).usExciteLength = %d; \n", ii+1,
				(int)(stpGroupSineSweepInput->stSineSweepExciteOnce.stSineSweepOnceExciteFreq.dNumCycle / dFreqHzAtCase * 1000
						/ stpGroupSineSweepInput->stSineSweepExciteOnce.stSineSweepOnceExciteFreq.fTs_ms));
			fprintf(fpData, "astSineSweepCase(%d).strOneExcitationResponse = 'SineSweepRespAryCase_%d.m'; \n", ii+1, ii);

			fprintf(fpData, "\n");

		}

		// iPlotFlag = 
		fprintf(fpData, "iPlotFlag = %d; \n", aft_spectrum_get_debug_flag());

		// Save the PC-memory
		fclose(fpData);
	}
}