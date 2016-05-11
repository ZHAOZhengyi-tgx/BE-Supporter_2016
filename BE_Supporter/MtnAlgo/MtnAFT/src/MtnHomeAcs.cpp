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

// 20090908 Replace Sleep to be high_precision_sleep_ms
#include "stdafx.h"
#include <math.h>

#include "MtnApi.h"
#include "MtnSearchHome.h"
#include "MtnApiSearchHome.h"
#include "WinTiming.h"

#define  __SEARCH_LIMIT_STOP_DISPLAY__
#define __SEARCH_INDEX_STOP_DISPLAY__

static LARGE_INTEGER liFreqOS; 
static int uiFlagIsDetectingByHighFreq;
extern int GetPentiumTimeCount_per_100us(unsigned __int64 frequency);

void mtn_cto_tick_start_time_u1s(LARGE_INTEGER liFreqOperSys);
bool mtn_cto_is_time_out_u1s(LARGE_INTEGER liFreqOperSys, unsigned int uiTimeOut_u1s);
unsigned int mtn_cto_get_start_cnt();
unsigned int mtn_cto_get_curr_cnt();

char strDebugErrorMessageHomeACS[512];

#define TIMEOUT_CNT_20SEC_SEARCH_LIMIT  (200000)
static unsigned int uiSearchWaitingTimeOut_u1s = 15; // Default time-out 15 sec for settling detection
void mtn_api_set_waiting_timeout_u1s(unsigned int uiTimeOut_u1s)
{
	uiSearchWaitingTimeOut_u1s = uiTimeOut_u1s;
}
unsigned mtn_api_get_waiting_timeout_u1s()
{
	return uiSearchWaitingTimeOut_u1s;
}

//static char strDebugErrorMessageHomeACSHomeAcs[512];
void mtnapi_get_debug_message_home_acs(char strDebugMessageHomeAcs[512])
{
	sprintf_s(strDebugMessageHomeAcs, 512, "%s", strDebugErrorMessageHomeACS);
}

void mtnapi_get_debug_message_ptr_home_acs(char *ptrDebugMessageHomeAcs)
{
	ptrDebugMessageHomeAcs = &strDebugErrorMessageHomeACS[0];
}

////////////////////////////////////////
// mtn_search_limit_acs
////////////////////////////////////////
// search limit
int mtn_acs_2dir_search_limit(HANDLE mHandle, 
							 MTN_SEARCH_LIMIT_INPUT *stpSearchLimitInput, 
							 MTN_SEARCH_LIMIT_OUTPUT *stpSearchLimitOutput)
{
	int iAxisOnACS = stpSearchLimitInput->iAxisOnACS;
	int iRet = MTN_API_OK_ZERO;; // return value
	double dblRefPosition;  // Axis Reference position
	int iMotorState; // Axis Motor Status
	int iFlags = 0; // start up immediately the jog
	MTN_SPEED_PROFILE stSpeedProfileBak;
	FILE *fpData = NULL;
	double fSampleTime_ms = 1.0;
	
	if(stpSearchLimitInput->iDebug == 1)
	{
		fopen_s(&fpData, SEARCH_LIMIT_DEBUG_FILENAME, "w");

		gstSystemScope.uiDataLen = 2000;
		gstSystemScope.uiNumData = 5;
		mtnscope_declare_var_on_controller(mHandle);
		mtnscope_set_acsc_var_collecting_searchindex(iAxisOnACS);
	}

	// Backup the current speed profile
	mtnapi_get_speed_profile(mHandle, iAxisOnACS, &stSpeedProfileBak, 0);

	// setup new profile for moving within this function
	mtnapi_set_speed_profile(mHandle, iAxisOnACS, &(stpSearchLimitInput->stSpeedProfileMove), 0);

	// Get OS Frequency
	QueryPerformanceFrequency(&liFreqOS); 

	// Enable the motor if it is not enabled
	mtnapi_get_motor_state(mHandle, iAxisOnACS, &iMotorState, 0);
	if(iMotorState & ACSC_MST_ENABLE) 
	{
		// now is enabled, OK
	}
	else
	{ // Motor is not enabled, we have to enable it first
		if(mtnapi_enable_motor(mHandle, iAxisOnACS, 0) != MTN_API_OK_ZERO)
		{
			stpSearchLimitOutput->iErrorCode = SEARCH_HOME_MOTOR_ERROR;
			iRet = MTN_API_ERROR;
			sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Limit- Motor Cannot Enable! [%s %d] \n",	__FILE__, __LINE__);

			goto label_return_acs_2dir_search_limit;
		}
	}

	// Move to a distance before searching limit
	mtnapi_get_ref_position(mHandle, iAxisOnACS, &dblRefPosition, 0);

	double dMoveDistBeforeSearch = stpSearchLimitInput->dMoveDistanceBeforeSearchLimit;
	double dTargetPositionBeforeSearchLimit = dblRefPosition + dMoveDistBeforeSearch;

#ifdef __SEARCH_LIMIT_STOP_DISPLAY__
	if(stpSearchLimitInput->iDebug == 1)
	{
//		CString cstrTemp;
		//cstrTemp.Format( "Move for a distance %6.1f, to %6.1f, before searching limit", 
		//	dMoveDistBeforeSearch, dTargetPositionBeforeSearchLimit); // cstrTemp.Format((char *), sprintf_s(strDebugErrorMessageHomeACS, 512,
		AfxMessageBox(_T("Move for a distance, before searching limit")); // cstrTemp); // _T(strDebugErrorMessageHomeACS)); _T(strDebugErrorMessageHomeACS)
	}
#endif // __SEARCH_LIMIT_STOP_DISPLAY__
	if(stpSearchLimitInput->iDebug == 1)
	{
		acsc_CollectB(mHandle, 0, // system data collection
				gstrScopeArrayName, // name of data collection array
				gstSystemScope.uiDataLen, // number of samples to be collected
				(int)fSampleTime_ms, // sampling period 1 millisecond
				strACSC_VarName, // variables to be collected
				NULL);
	}

	if(! acsc_ToPoint(mHandle, 0, // start up the motion immediately
			iAxisOnACS, dTargetPositionBeforeSearchLimit, NULL) )
	{
		stpSearchLimitOutput->iErrorCode = SEARCH_HOME_ERROR_1ST_MOVE_DIST;
		iRet = MTN_API_ERROR;
		sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Limit- Moving A Distance before searching limit! [%s %d] \n",
			__FILE__, __LINE__);
		goto label_return_acs_2dir_search_limit;
	}
	if(fpData != NULL)
	{
		fprintf(fpData, "%% MoveToPointBeforeSearchLimit = %8.2f\n", dTargetPositionBeforeSearchLimit);
		fprintf(fpData, "%% ScopeSampleTime_ms = %8.4f \n", fSampleTime_ms);
		MTN_SPEED_PROFILE stSpeedProfileTemp;
		mtnapi_get_speed_profile(mHandle, iAxisOnACS, &stSpeedProfileTemp, 0);
		fprintf(fpData, "%%  MaxVel = %8.2f\n", stSpeedProfileTemp.dMaxVelocity);
		fprintf(fpData, "%%  MaxAcc = %8.2f\n", stSpeedProfileTemp.dMaxAcceleration);
		fprintf(fpData, "%%  MaxJerk = %8.2f\n", stSpeedProfileTemp.dMaxJerk);
		fprintf(fpData, "%%  MaxDec = %8.2f\n", stSpeedProfileTemp.dMaxDeceleration);

	}	
	mtn_cto_tick_start_time_u1s(liFreqOS); // uiTimeOutCntStart = GetPentiumTimeCount_per_100us(liFreqOS.QuadPart); // 20090508
	mtnapi_get_motor_state(mHandle, iAxisOnACS, &iMotorState, 0);
	while(iMotorState & ACSC_MST_MOVE) // iFlagDoneMotionBuff8 == 0) // 
	{
		Sleep(2);
		mtnapi_get_motor_state(mHandle, iAxisOnACS, &iMotorState, 0);
		
		if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
		{
			stpSearchLimitOutput->iErrorCode = MTN_API_ERROR_SEARCH_TIMEOUT;
			iRet = MTN_API_ERROR;
			sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Limit- Timeout: [CntStart: %d, CntCurr: %d, TimeOut: %d], Not Settling-1! [%s %d] \n",	
				mtn_cto_get_start_cnt(), mtn_cto_get_curr_cnt(), uiSearchWaitingTimeOut_u1s,__FILE__, __LINE__);
			goto label_return_acs_2dir_search_limit;
		} // 20090508
	}

	// Wait axis to settle
	double dPositionErrorFb;
	double dPosnErrTH_Settle = stpSearchLimitInput->dPosnErrThresHoldSettling;
	mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
	mtn_cto_tick_start_time_u1s(liFreqOS);  // 20090508 		if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
	while(fabs(dPositionErrorFb) >= dPosnErrTH_Settle)
	{
		Sleep(10);
		mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
		if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
		{
			stpSearchLimitOutput->iErrorCode = MTN_API_ERROR_SEARCH_TIMEOUT;
			iRet = MTN_API_ERROR;
			sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Limit- Timeout: [CntStart: %d, CntCurr: %d, TimeOut: %d], Settling-2 ! [%s %d] \n",	
				mtn_cto_get_start_cnt(), mtn_cto_get_curr_cnt(), uiSearchWaitingTimeOut_u1s, __FILE__, __LINE__);
			goto label_return_acs_2dir_search_limit;
		} // 20090508
	}
#ifdef __SEARCH_LIMIT_STOP_DISPLAY__
	if(stpSearchLimitInput->iDebug == 1)
	{
		AfxMessageBox(_T("After Moving for a distance and settle down"));
	}
#endif // __SEARCH_LIMIT_STOP_DISPLAY__
	// search mechanical limit by jogging
	double dRegPositionBeforeSearching;
	mtnapi_get_ref_position(mHandle, iAxisOnACS, &dRegPositionBeforeSearching, 0);

	double dJogVelocity; // search velocity. NO wait command. Axis the searching axis.
	if(dMoveDistBeforeSearch >= 0 )
	{
		dJogVelocity = fabs(stpSearchLimitInput->dVelJoggingLimit) * ACSC_POSITIVE_DIRECTION; //  ;
	}
	else
	{
		dJogVelocity = fabs(stpSearchLimitInput->dVelJoggingLimit) * ACSC_NEGATIVE_DIRECTION; //  ;
	}
	acsc_SetVelocity(mHandle, iAxisOnACS, fabs(dJogVelocity), NULL); // setup the jogging velocity
	acsc_Jog(mHandle, iFlags, iAxisOnACS, dJogVelocity, NULL); // Immediate start

	QueryPerformanceFrequency(&liFreqOS); // Get OS Frequency

	// detection of the position error
	unsigned int uiCounterLast = GetPentiumTimeCount_per_100us(liFreqOS.QuadPart); // timeGetTime();
	unsigned int uiCounterCurr;

	unsigned int uiFreqFactor_10KHz = stpSearchLimitInput->uiFreqFactor_10KHz_Detecting;
	double dPositionErrorTH = stpSearchLimitInput->dPositionErrorThresholdSearchLimit;
	double dFeedbackPositionCurrent; //
	double dMaxDistRangeProt = stpSearchLimitInput->dMaxDistanceRangeProtection;
	mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);

#ifdef __SEARCH_LIMIT_STOP_DISPLAY__
	if(stpSearchLimitInput->iDebug == 1)
	{
		AfxMessageBox(_T("After Jogging before detecting"));
	}
#endif //__SEARCH_LIMIT_STOP_DISPLAY__
	if(	uiFlagIsDetectingByHighFreq == 1) 
	{
		// semaphore variable to protect s.t. the following routine could be executed no more than once at any time
		stpSearchLimitOutput->iErrorCode = SEARCH_HOME_MULTIPLE_AXIS;
		iRet = MTN_API_ERROR;
		sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Limit- Semaphar, Multiple Axis Searching! [%s %d] \n",	__FILE__, __LINE__);
		goto label_return_acs_2dir_search_limit;
	}
	else
	{
		uiFlagIsDetectingByHighFreq = 1;
	}
	while(fabs(dPositionErrorFb) <= dPositionErrorTH)
	{
		uiCounterCurr = GetPentiumTimeCount_per_100us(liFreqOS.QuadPart); // Register the current OS time
		if(uiCounterCurr - uiCounterLast >= uiFreqFactor_10KHz 
			||	( (double) uiCounterCurr + UINT_MAX - uiCounterLast >= uiFreqFactor_10KHz 
			     && (double)uiCounterLast + uiFreqFactor_10KHz > UINT_MAX
				 )
		   )
		{   // Get position error
			mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);

			// update the counter
			uiCounterLast = uiCounterCurr;

			// Get the current feedback position for limit protection
			mtnapi_get_fb_position(mHandle, iAxisOnACS, &dFeedbackPositionCurrent, 0);
			if(fabs(dFeedbackPositionCurrent - dRegPositionBeforeSearching) >= dMaxDistRangeProt)
			{
				stpSearchLimitOutput->iErrorCode = SEARCH_HOME_OUT_OF_DIST_RANGE;
				iRet = MTN_API_ERROR;
				sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Limit- Exceed Maximum Dist! [%s %d] \n",	__FILE__, __LINE__);
				goto label_return_acs_2dir_search_limit;
			}
		}
		else
		{
			Sleep(0);
		}
		// check time-out    // 20090508
		if( (uiCounterCurr - uiCounterLast > TIMEOUT_CNT_20SEC_SEARCH_LIMIT)
			||	( (double) uiCounterCurr + UINT_MAX - uiCounterLast >= TIMEOUT_CNT_20SEC_SEARCH_LIMIT 
			     && (double)uiCounterLast + TIMEOUT_CNT_20SEC_SEARCH_LIMIT > UINT_MAX
				 )
				 )
		{
			stpSearchLimitOutput->iErrorCode = MTN_API_ERROR_SEARCH_TIMEOUT;
			iRet = MTN_API_ERROR;
			sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Limit- Not Get Limit-3! [%s %d] \n",	__FILE__, __LINE__);
			goto label_return_acs_2dir_search_limit;
		} // 20090508
	}
	acsc_Halt(mHandle, iAxisOnACS, NULL);
	uiFlagIsDetectingByHighFreq = 0; // reset the semaphore variable
	// Wait axis to settle
	//mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
	//while(fabs(dPositionErrorFb) >= dPosnErrTH_Settle)
	//{
	//	high_precision_sleep_ms(10);
	//	mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
	//}
#ifdef __SEARCH_LIMIT_STOP_DISPLAY__
	if(stpSearchLimitInput->iDebug == 1)
	{
		AfxMessageBox(_T("After detecting limit"));
	}
#endif // __SEARCH_LIMIT_STOP_DISPLAY__
	// record the limit position
	double dRegPositionLimit;
	mtnapi_get_ref_position(mHandle, iAxisOnACS, &dRegPositionLimit, 0);
	stpSearchLimitOutput->dLimitPosition = dRegPositionLimit;

	// move for a distance from limit area
	mtnapi_set_speed_profile(mHandle, iAxisOnACS, &(stpSearchLimitInput->stSpeedProfileMove), 0);
	dMoveDistBeforeSearch = stpSearchLimitInput->dMoveDistanceAfterSearchLimit;
	double dTargetPositionAfterSearchLimit = dRegPositionLimit + dMoveDistBeforeSearch;

	if( fabs(dMoveDistBeforeSearch) >= 1.0)
	{
		if(!acsc_ToPoint(mHandle, 0, // start up immediately the motion
				iAxisOnACS, dTargetPositionAfterSearchLimit, NULL) )
		{
			stpSearchLimitOutput->iErrorCode = SEARCH_HOME_MOTOR_ERROR;
			iRet = MTN_API_ERROR;
			sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Search Limit - Moving After SearchLimit! [%s %d] \n",	__FILE__, __LINE__);
			goto label_return_acs_2dir_search_limit;
		}
		// wait motion to complete
		mtnapi_get_motor_state(mHandle, iAxisOnACS, &iMotorState, 0);
		mtn_cto_tick_start_time_u1s(liFreqOS);  // 20090508 		if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
		while(iMotorState & ACSC_MST_MOVE) // iFlagDoneMotionBuff8 == 0) // 
		{
			Sleep(2);
			mtnapi_get_motor_state(mHandle, iAxisOnACS, &iMotorState, 0);
			if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
			{
				stpSearchLimitOutput->iErrorCode = MTN_API_ERROR_SEARCH_TIMEOUT;
				iRet = MTN_API_ERROR;
				sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Limit- Timeout: [CntStart: %d, CntCurr: %d, TimeOut: %d], Not Settling-4! [%s %d] \n",
					mtn_cto_get_start_cnt(), mtn_cto_get_curr_cnt(), uiSearchWaitingTimeOut_u1s, __FILE__, __LINE__);
				goto label_return_acs_2dir_search_limit;
			} // 20090508
		}
		// Wait axis to settle
		mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
		mtn_cto_tick_start_time_u1s(liFreqOS);  // 20090508 		if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
		while(fabs(dPositionErrorFb) >= dPosnErrTH_Settle)
		{
			Sleep(10);
			mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
			if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
			{
				stpSearchLimitOutput->iErrorCode = MTN_API_ERROR_SEARCH_TIMEOUT;
				iRet = MTN_API_ERROR;
				sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Limit- Timeout: [CntStart: %d, CntCurr: %d, TimeOut: %d], Not Settling-5! [%s %d] \n",	
					mtn_cto_get_start_cnt(), mtn_cto_get_curr_cnt(), uiSearchWaitingTimeOut_u1s, __FILE__, __LINE__);
				goto label_return_acs_2dir_search_limit;
			} // 20090508
		}
	}
#ifdef __SEARCH_LIMIT_STOP_DISPLAY__
	if(stpSearchLimitInput->iDebug == 1)
	{
		AfxMessageBox(_T("After moving a distance and settling"));
	}
#endif // __SEARCH_LIMIT_STOP_DISPLAY__
	if(fpData != NULL)
	{
		acsc_ReadReal(mHandle, ACSC_NONE, gstrScopeDataVarName, 0, gstSystemScope.uiNumData - 1, 0, gstSystemScope.uiDataLen - 1, gdScopeCollectData, NULL);
		fprintf(fpData, "%% Search Limit Debug Data, Axis: %d\n", iAxisOnACS);
		fprintf(fpData, "matSearchHomeData = [ %% RPOS, PE, RVEL, IND, DOUT\n");
		for(unsigned int ii = 0; ii< gstSystemScope.uiDataLen; ii++)
		{
			if(ii < gstSystemScope.uiDataLen - 1)
			{
				fprintf(fpData, "%8.1f, %8.1f, %8.2f, %8.0f, %8.0f\n", 
					gdScopeCollectData[ii], gdScopeCollectData[gstSystemScope.uiDataLen + ii], gdScopeCollectData[gstSystemScope.uiDataLen *2 + ii],
					gdScopeCollectData[gstSystemScope.uiDataLen *3 + ii], gdScopeCollectData[gstSystemScope.uiDataLen *4 + ii]);
			}
			else
			{
				fprintf(fpData, "%8.1f, %8.1f, %8.2f, %8.0f, %8.0f];\n", 
					gdScopeCollectData[ii], gdScopeCollectData[gstSystemScope.uiDataLen + ii], gdScopeCollectData[gstSystemScope.uiDataLen *2 + ii],
					gdScopeCollectData[gstSystemScope.uiDataLen *3 + ii], gdScopeCollectData[gstSystemScope.uiDataLen *4 + ii]);
			}
		}
	}

label_return_acs_2dir_search_limit:

	// restore the bakuped speed profile
	mtnapi_set_speed_profile(mHandle, iAxisOnACS, &stSpeedProfileBak, 0);
	uiFlagIsDetectingByHighFreq = 0; // reset the semaphore variable
	if(fpData != NULL)
	{
		fclose(fpData);
	}

	return iRet;
}

// 20120927
#define MIN_PROT_DRV_CMD_RMS   (0.2)
#define MAX_PROT_DRV_CMD_RMS   (0.5)
// 20120927

////////////////////////////////////////
// NOT released function
////////////////////////////////////////
// search limit
int mtn_search_limit_acs(HANDLE mHandle, 
							 MTN_SEARCH_LIMIT_INPUT *stpSearchLimitInput, 
							 MTN_SEARCH_LIMIT_OUTPUT *stpSearchLimitOutput)
{
	int iAxisOnACS = stpSearchLimitInput->iAxisOnACS;
	int iRet = MTN_API_OK_ZERO;; // return value
static	double dblRefPosition;  // Axis Reference position
	int iMotorState; // Axis Motor Status
	int iFlags = 0; // start up immediately the jog
	MTN_SPEED_PROFILE stSpeedProfileBak;
	FILE *fpData = NULL;
	double fSampleTime_ms = 1.0;
	// 20120927
	if(stpSearchLimitInput->dDriveCmdThresholdProtection < MIN_PROT_DRV_CMD_RMS)
	{
		stpSearchLimitInput->dDriveCmdThresholdProtection = MIN_PROT_DRV_CMD_RMS;
	}
	else if(stpSearchLimitInput->dDriveCmdThresholdProtection > MAX_PROT_DRV_CMD_RMS)
	{
		stpSearchLimitInput->dDriveCmdThresholdProtection = MAX_PROT_DRV_CMD_RMS;
	}
	// 20120927

	if(stpSearchLimitInput->iDebug == 1)
	{
		fopen_s(&fpData, SEARCH_LIMIT_DEBUG_FILENAME, "w");

		gstSystemScope.uiDataLen = 2000;
		gstSystemScope.uiNumData = 5;
		mtnscope_declare_var_on_controller(mHandle);
		mtnscope_set_acsc_var_collecting_searchindex(iAxisOnACS);
	}

	// Backup the current speed profile
	mtnapi_get_speed_profile(mHandle, iAxisOnACS, &stSpeedProfileBak, 0);

	// setup new profile for moving within this function
	mtnapi_set_speed_profile(mHandle, iAxisOnACS, &(stpSearchLimitInput->stSpeedProfileMove), 0);

	// Get OS Frequency
	QueryPerformanceFrequency(&liFreqOS); 

	// Enable the motor if it is not enabled
	mtnapi_get_motor_state(mHandle, iAxisOnACS, &iMotorState, 0);
	if(iMotorState & ACSC_MST_ENABLE) 
	{
		// now is enabled, OK
	}
	else
	{ // Motor is not enabled, we have to enable it first
		if(mtnapi_enable_motor(mHandle, iAxisOnACS, 0) != MTN_API_OK_ZERO)
		{
			stpSearchLimitOutput->iErrorCode = SEARCH_HOME_MOTOR_ERROR;
			iRet = MTN_API_ERROR;
			sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Limit- Motor Cannot Enable! [%s %d] \n",	__FILE__, __LINE__);

			goto label_return_search_limit_acs;
		}
	}

	// Move to a distance before searching limit
	mtnapi_get_ref_position(mHandle, iAxisOnACS, &dblRefPosition, 0);

static	double dMoveDistBeforeSearch;
	dMoveDistBeforeSearch = stpSearchLimitInput->dMoveDistanceBeforeSearchLimit;
static	double dTargetPositionBeforeSearchLimit;
	dTargetPositionBeforeSearchLimit = dblRefPosition + dMoveDistBeforeSearch;

#ifdef __SEARCH_LIMIT_STOP_DISPLAY__
	if(stpSearchLimitInput->iDebug == 1)
	{
//		CString cstrTempHomeACS;
		//cstrTemp.Format( "Move for a distance %6.1f, to %6.1f, before searching limit", 
		//	dMoveDistBeforeSearch, dTargetPositionBeforeSearchLimit); // cstrTemp.Format((char *), sprintf_s(strDebugErrorMessageHomeACS, 512,
//		cstrTempHomeACS.Format("Axis - %d, TargetPosn: %8.1f", iAxisOnACS, dTargetPositionBeforeSearchLimit);
//		AfxMessageBox(cstrTempHomeACS);

		sprintf_s(strDebugErrorMessageHomeACS, 512, "Axis- %d Move for a distance, to %6.1f, before searching limit",
			iAxisOnACS, dTargetPositionBeforeSearchLimit);
		AfxMessageBox(LPCTSTR(strDebugErrorMessageHomeACS));
//		AfxMessageBox(_T(strDebugErrorMessageHomeACS)); // cstrTemp); // _T(strDebugErrorMessageHomeACS)); _T(strDebugErrorMessageHomeACS)
	}
#endif // __SEARCH_LIMIT_STOP_DISPLAY__
	if(stpSearchLimitInput->iDebug == 1)
	{
		acsc_CollectB(mHandle, 0, // system data collection
				gstrScopeArrayName, // name of data collection array
				gstSystemScope.uiDataLen, // number of samples to be collected
				(int)fSampleTime_ms, // sampling period 1 millisecond
				strACSC_VarName, // variables to be collected
				NULL);
	}

	if(! acsc_ToPoint(mHandle, 0, // start up the motion immediately
			iAxisOnACS, dTargetPositionBeforeSearchLimit, NULL) )
	{
		stpSearchLimitOutput->iErrorCode = SEARCH_HOME_ERROR_1ST_MOVE_DIST;
		iRet = MTN_API_ERROR;
		sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Limit- Moving A Distance before searching limit! [%s %d] \n",
			__FILE__, __LINE__);
		goto label_return_search_limit_acs;
	}
	if(fpData != NULL)
	{
		fprintf(fpData, "%% MoveToPointBeforeSearchLimit = %8.2f\n", dTargetPositionBeforeSearchLimit);
		fprintf(fpData, "%% ScopeSampleTime_ms = %8.4f \n", fSampleTime_ms);
		MTN_SPEED_PROFILE stSpeedProfileTemp;
		mtnapi_get_speed_profile(mHandle, iAxisOnACS, &stSpeedProfileTemp, 0);
		fprintf(fpData, "%%  MaxVel = %8.2f\n", stSpeedProfileTemp.dMaxVelocity);
		fprintf(fpData, "%%  MaxAcc = %8.2f\n", stSpeedProfileTemp.dMaxAcceleration);
		fprintf(fpData, "%%  MaxJerk = %8.2f\n", stSpeedProfileTemp.dMaxJerk);
		fprintf(fpData, "%%  MaxDec = %8.2f\n", stSpeedProfileTemp.dMaxDeceleration);

	}	
	mtn_cto_tick_start_time_u1s(liFreqOS); // uiTimeOutCntStart = GetPentiumTimeCount_per_100us(liFreqOS.QuadPart); // 20090508
	mtnapi_get_motor_state(mHandle, iAxisOnACS, &iMotorState, 0);
	while(iMotorState & ACSC_MST_MOVE) // iFlagDoneMotionBuff8 == 0) // 
	{
		Sleep(2);
		mtnapi_get_motor_state(mHandle, iAxisOnACS, &iMotorState, 0);
		
		if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
		{
			stpSearchLimitOutput->iErrorCode = MTN_API_ERROR_SEARCH_TIMEOUT;
			iRet = MTN_API_ERROR;
			sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Limit- Timeout: [CntStart: %d, CntCurr: %d, TimeOut: %d], Not Settling-1! [%s %d] \n",	
				mtn_cto_get_start_cnt(), mtn_cto_get_curr_cnt(), uiSearchWaitingTimeOut_u1s,__FILE__, __LINE__);
			goto label_return_search_limit_acs;
		} // 20090508
	}

	double dRightLimitPosn, dLeftLimitPosn, dRefPosition; // 20130203
	mtn_api_get_position_upp_lmt(mHandle, iAxisOnACS, &dRightLimitPosn);
	mtn_api_get_position_low_lmt(mHandle, iAxisOnACS, &dLeftLimitPosn);

	// Wait axis to settle
	double dPositionErrorFb;
	double dPosnErrTH_Settle = stpSearchLimitInput->dPosnErrThresHoldSettling;
	mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
	mtn_cto_tick_start_time_u1s(liFreqOS);  // 20090508 		if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
	while(fabs(dPositionErrorFb) >= dPosnErrTH_Settle)
	{
		Sleep(10);
		mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
		if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
		{
			stpSearchLimitOutput->iErrorCode = MTN_API_ERROR_SEARCH_TIMEOUT;
			iRet = MTN_API_ERROR;
			sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Limit- Timeout: [CntStart: %d, CntCurr: %d, TimeOut: %d], Settling-2 ! [%s %d] \n",	
				mtn_cto_get_start_cnt(), mtn_cto_get_curr_cnt(), uiSearchWaitingTimeOut_u1s, __FILE__, __LINE__);
			goto label_return_search_limit_acs;
		} // 20090508
	}
#ifdef __SEARCH_LIMIT_STOP_DISPLAY__
	if(stpSearchLimitInput->iDebug == 1)
	{
		AfxMessageBox(_T("After Moving for a distance and settle down"));
	}
#endif // __SEARCH_LIMIT_STOP_DISPLAY__
	// search mechanical limit by jogging
	double dRegPositionBeforeSearching;
	mtnapi_get_ref_position(mHandle, iAxisOnACS, &dRegPositionBeforeSearching, 0);

	double dJogVelocity; // search velocity. NO wait command. Axis the searching axis.
	if(dMoveDistBeforeSearch >= 0 )
	{
		dJogVelocity = fabs(stpSearchLimitInput->dVelJoggingLimit) * ACSC_POSITIVE_DIRECTION; //  ;
	}
	else
	{
		dJogVelocity = fabs(stpSearchLimitInput->dVelJoggingLimit) * ACSC_NEGATIVE_DIRECTION; //  ;
	}
	acsc_SetVelocity(mHandle, iAxisOnACS, fabs(dJogVelocity), NULL); // setup the jogging velocity
	acsc_Jog(mHandle, iFlags, iAxisOnACS, dJogVelocity, NULL); // Immediate start

	QueryPerformanceFrequency(&liFreqOS); // Get OS Frequency

	// detection of the position error
	unsigned int uiCounterLast = GetPentiumTimeCount_per_100us(liFreqOS.QuadPart); // timeGetTime();
	unsigned int uiCounterCurr;

	unsigned int uiFreqFactor_10KHz = stpSearchLimitInput->uiFreqFactor_10KHz_Detecting;
	double dPositionErrorTH = stpSearchLimitInput->dPositionErrorThresholdSearchLimit;
	double dFeedbackPositionCurrent; //
	double dMaxDistRangeProt = stpSearchLimitInput->dMaxDistanceRangeProtection;
	mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);

#ifdef __SEARCH_LIMIT_STOP_DISPLAY__
	if(stpSearchLimitInput->iDebug == 1)
	{
		AfxMessageBox(_T("After Jogging before detecting"));
	}
#endif //__SEARCH_LIMIT_STOP_DISPLAY__
	if(	uiFlagIsDetectingByHighFreq == 1) 
	{
		// semaphore variable to protect s.t. the following routine could be executed no more than once at any time
		stpSearchLimitOutput->iErrorCode = SEARCH_HOME_MULTIPLE_AXIS;
		iRet = MTN_API_ERROR;
		sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Limit- Semaphar, Multiple Axis Searching! [%s %d] \n",	__FILE__, __LINE__);
		goto label_return_search_limit_acs;
	}
	else
	{
		uiFlagIsDetectingByHighFreq = 1;
	}
// 20120927
double dDrvCmdPc; int nCounterDrvCmdGT_Th = 0;
#define COUNTER_DRV_CMD_GT_RMS     20
// 20120927
	while(fabs(dPositionErrorFb) <= dPositionErrorTH)
	{
		uiCounterCurr = GetPentiumTimeCount_per_100us(liFreqOS.QuadPart); // Register the current OS time
		if(uiCounterCurr - uiCounterLast >= uiFreqFactor_10KHz 
			||	( (double) uiCounterCurr + UINT_MAX - uiCounterLast >= uiFreqFactor_10KHz 
			     && (double)uiCounterLast + uiFreqFactor_10KHz > UINT_MAX
				 )
		   )
		{   // Get position error
			mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);

			// update the counter
			uiCounterLast = uiCounterCurr;

			// Get the current feedback position for limit protection
			mtnapi_get_fb_position(mHandle, iAxisOnACS, &dFeedbackPositionCurrent, 0);
			if(fabs(dFeedbackPositionCurrent - dRegPositionBeforeSearching) >= dMaxDistRangeProt)
			{
				stpSearchLimitOutput->iErrorCode = SEARCH_HOME_OUT_OF_DIST_RANGE;
				iRet = MTN_API_ERROR;
				sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Limit- Exceed Maximum Dist! [%s %d] \n",	__FILE__, __LINE__);
				goto label_return_search_limit_acs;
			}
			// Get the current DrvCmd // 20120927
			mtn_api_get_drv_cmd(mHandle, iAxisOnACS, &dDrvCmdPc);
			if(fabs(dDrvCmdPc) >= stpSearchLimitInput->dDriveCmdThresholdProtection)
			{
				nCounterDrvCmdGT_Th ++;
			}
			else
			{
				nCounterDrvCmdGT_Th = 0;
			}
			if(nCounterDrvCmdGT_Th >= COUNTER_DRV_CMD_GT_RMS)
			{
				break;
			}
			// 20120927
			mtnapi_get_ref_position(mHandle, iAxisOnACS, &dRefPosition, 0); // 20130203
			if(dRefPosition > (dRightLimitPosn - 10) 
				|| dRefPosition < (dLeftLimitPosn +10) )
			{
				break;
			}// 20130203

		}
		else
		{
			Sleep(0);
		}
		// check time-out    // 20090508
		if( (uiCounterCurr - uiCounterLast > TIMEOUT_CNT_20SEC_SEARCH_LIMIT)
			||	( (double) uiCounterCurr + UINT_MAX - uiCounterLast >= TIMEOUT_CNT_20SEC_SEARCH_LIMIT 
			     && (double)uiCounterLast + TIMEOUT_CNT_20SEC_SEARCH_LIMIT > UINT_MAX
				 )
				 )
		{
			stpSearchLimitOutput->iErrorCode = MTN_API_ERROR_SEARCH_TIMEOUT;
			iRet = MTN_API_ERROR;
			sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Limit- Not Get Limit-3! [%s %d] \n",	__FILE__, __LINE__);
			goto label_return_search_limit_acs;
		} // 20090508
	}
	acsc_Halt(mHandle, iAxisOnACS, NULL);
	uiFlagIsDetectingByHighFreq = 0; // reset the semaphore variable
	// Wait axis to settle
	//mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
	//while(fabs(dPositionErrorFb) >= dPosnErrTH_Settle)
	//{
	//	high_precision_sleep_ms(10);
	//	mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
	//}
#ifdef __SEARCH_LIMIT_STOP_DISPLAY__
	if(stpSearchLimitInput->iDebug == 1)
	{
		AfxMessageBox(_T("After detecting limit"));
	}
#endif // __SEARCH_LIMIT_STOP_DISPLAY__
	// record the limit position
	double dRegPositionLimit;
	mtnapi_get_ref_position(mHandle, iAxisOnACS, &dRegPositionLimit, 0);
	stpSearchLimitOutput->dLimitPosition = dRegPositionLimit;

	// move for a distance from limit area
	mtnapi_set_speed_profile(mHandle, iAxisOnACS, &(stpSearchLimitInput->stSpeedProfileMove), 0);
	dMoveDistBeforeSearch = stpSearchLimitInput->dMoveDistanceAfterSearchLimit;
	double dTargetPositionAfterSearchLimit = dRegPositionLimit + dMoveDistBeforeSearch;

	if( fabs(dMoveDistBeforeSearch) >= 1.0)
	{
		if(!acsc_ToPoint(mHandle, 0, // start up immediately the motion
				iAxisOnACS, dTargetPositionAfterSearchLimit, NULL) )
		{
			stpSearchLimitOutput->iErrorCode = SEARCH_HOME_MOTOR_ERROR;
			iRet = MTN_API_ERROR;
			sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Search Limit - Moving After SearchLimit! [%s %d] \n",	__FILE__, __LINE__);
			goto label_return_search_limit_acs;
		}
		// wait motion to complete
		mtnapi_get_motor_state(mHandle, iAxisOnACS, &iMotorState, 0);
		mtn_cto_tick_start_time_u1s(liFreqOS);  // 20090508 		if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
		while(iMotorState & ACSC_MST_MOVE) // iFlagDoneMotionBuff8 == 0) // 
		{
			Sleep(2);
			mtnapi_get_motor_state(mHandle, iAxisOnACS, &iMotorState, 0);
			if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
			{
				stpSearchLimitOutput->iErrorCode = MTN_API_ERROR_SEARCH_TIMEOUT;
				iRet = MTN_API_ERROR;
				sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Limit- Timeout: [CntStart: %d, CntCurr: %d, TimeOut: %d], Not Settling-4! [%s %d] \n",
					mtn_cto_get_start_cnt(), mtn_cto_get_curr_cnt(), uiSearchWaitingTimeOut_u1s, __FILE__, __LINE__);
				goto label_return_search_limit_acs;
			} // 20090508
		}
		// Wait axis to settle
		mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
		mtn_cto_tick_start_time_u1s(liFreqOS);  // 20090508 		if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
		while(fabs(dPositionErrorFb) >= dPosnErrTH_Settle)
		{
			Sleep(10);
			mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
			if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
			{
				stpSearchLimitOutput->iErrorCode = MTN_API_ERROR_SEARCH_TIMEOUT;
				iRet = MTN_API_ERROR;
				sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Limit- Timeout: [CntStart: %d, CntCurr: %d, TimeOut: %d], Not Settling-5! [%s %d] \n",	
					mtn_cto_get_start_cnt(), mtn_cto_get_curr_cnt(), uiSearchWaitingTimeOut_u1s, __FILE__, __LINE__);
				goto label_return_search_limit_acs;
			} // 20090508
		}
	}
#ifdef __SEARCH_LIMIT_STOP_DISPLAY__
	if(stpSearchLimitInput->iDebug == 1)
	{
		AfxMessageBox(_T("After moving a distance and settling"));
	}
#endif // __SEARCH_LIMIT_STOP_DISPLAY__
	if(fpData != NULL)
	{
		acsc_ReadReal(mHandle, ACSC_NONE, gstrScopeDataVarName, 0, gstSystemScope.uiNumData - 1, 0, gstSystemScope.uiDataLen - 1, gdScopeCollectData, NULL);
		fprintf(fpData, "%% Search Limit Debug Data, Axis: %d\n", iAxisOnACS);
		fprintf(fpData, "matSearchHomeData = [ %% RPOS, PE, RVEL, IND, DOUT\n");
		for(unsigned int ii = 0; ii< gstSystemScope.uiDataLen; ii++)
		{
			if(ii < gstSystemScope.uiDataLen - 1)
			{
				fprintf(fpData, "%8.1f, %8.1f, %8.2f, %8.0f, %8.0f\n", 
					gdScopeCollectData[ii], gdScopeCollectData[gstSystemScope.uiDataLen + ii], gdScopeCollectData[gstSystemScope.uiDataLen *2 + ii],
					gdScopeCollectData[gstSystemScope.uiDataLen *3 + ii], gdScopeCollectData[gstSystemScope.uiDataLen *4 + ii]);
			}
			else
			{
				fprintf(fpData, "%8.1f, %8.1f, %8.2f, %8.0f, %8.0f];\n", 
					gdScopeCollectData[ii], gdScopeCollectData[gstSystemScope.uiDataLen + ii], gdScopeCollectData[gstSystemScope.uiDataLen *2 + ii],
					gdScopeCollectData[gstSystemScope.uiDataLen *3 + ii], gdScopeCollectData[gstSystemScope.uiDataLen *4 + ii]);
			}
		}
	}

label_return_search_limit_acs:

	// restore the bakuped speed profile
	mtnapi_set_speed_profile(mHandle, iAxisOnACS, &stSpeedProfileBak, 0);
	uiFlagIsDetectingByHighFreq = 0; // reset the semaphore variable
	if(fpData != NULL)
	{
		fclose(fpData);
	}

	return iRet;
}

// Search index from limit area 
int mtn_search_home_from_limit_acs(HANDLE mHandle, 
									   MTN_SEARCH_HOME_FROM_LIMIT_INPUT *stpSearchHomeFromLimitInput, 
									   MTN_SEARCH_HOME_FROM_LIMIT_OUTPUT *stpSearchHomeFromLimitOutput)
{
	int iAxisOnACS = stpSearchHomeFromLimitInput->iAxisOnACS;
	int iRet = MTN_API_OK_ZERO; // return value
//	double dblRefPosition;  // Axis Reference position
	int iMotorState; // Axis Motor Status
	int iFlags = 0; // start up immediately the jog
	MTN_SPEED_PROFILE stSpeedProfileBak;
	FILE *fpData = NULL;
	double fSampleTime_ms = 2.0;
	// Get OS Frequency
	QueryPerformanceFrequency(&liFreqOS); 

	// Backup the current speed profile
	mtnapi_get_speed_profile(mHandle, iAxisOnACS, &stSpeedProfileBak, 0);

	// setup new profile for moving within this function
	mtnapi_set_speed_profile(mHandle, iAxisOnACS, &(stpSearchHomeFromLimitInput->stSpeedProfileMove), 0);

	// Enable the motor if it is not enabled
	mtnapi_get_motor_state(mHandle, iAxisOnACS, &iMotorState, 0);
	if(iMotorState & ACSC_MST_ENABLE) 
	{
		// now is enabled, OK
	}
	else
	{ // Motor is not enabled, we have to enable it first
		if(mtnapi_enable_motor(mHandle, iAxisOnACS, 0) != MTN_API_OK_ZERO)
		{
			stpSearchHomeFromLimitOutput->iErrorCode = SEARCH_HOME_MOTOR_ERROR;
			iRet = MTN_API_ERROR;
			sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Searching Index- Cannot Enable Motor! [%s %d] \n",	__FILE__, __LINE__);
			goto label_return_search_home_from_limit_acs;
		}
	}

	if(stpSearchHomeFromLimitInput->iDebug == 1)
	{
		fopen_s(&fpData, SEARCH_HOME_DEBUG_FILENAME, "w");

		gstSystemScope.uiDataLen = 2000;
		gstSystemScope.uiNumData = 5;
		mtnscope_declare_var_on_controller(mHandle);
		mtnscope_set_acsc_var_collecting_searchindex(iAxisOnACS);
	}

	if(stpSearchHomeFromLimitInput->iDebug == 1)
	{
		acsc_CollectB(mHandle, 0, // system data collection
				gstrScopeArrayName, // name of data collection array
				gstSystemScope.uiDataLen, // number of samples to be collected
				(int)fSampleTime_ms, // sampling period 1 millisecond
				strACSC_VarName, // variables to be collected
				NULL);
	}
	double dRegPositionLimit; //  = stSearchLimitOutput.dLimitPosition;
	mtnapi_get_ref_position(mHandle, iAxisOnACS, &dRegPositionLimit, 0); // Assume the initial position is limit

	QueryPerformanceFrequency(&liFreqOS); // Get OS Frequency

	double dRegPositionBeforeSearching;
	double dMoveDistBeforeSearch;
	double dJogVelocity; // search velocity. NO wait command. Axis the searching axis.
	unsigned int uiCounterLast = GetPentiumTimeCount_per_100us(liFreqOS.QuadPart); // timeGetTime();
	unsigned int uiCounterCurr;
	double dPositionErrorFb;
	double dPosnErrTH_Settle = stpSearchHomeFromLimitInput->dPosnErrThresHoldSettling;
	unsigned int uiFreqFactor_10KHz = stpSearchHomeFromLimitInput->uiFreqFactor_10KHz_Detecting;
	double dFeedbackPositionCurrent; //
	double dMaxDistRangeProt = stpSearchHomeFromLimitInput->dMaxDistanceRangeProtection;

	// move for a distance before 1st search index
	mtnapi_set_speed_profile(mHandle, iAxisOnACS, &(stpSearchHomeFromLimitInput->stSpeedProfileMove), 0);
	dMoveDistBeforeSearch = stpSearchHomeFromLimitInput->dMoveDistanceBeforeSearchIndex1;
	double dTargetPositionBeforeSearchIndex = dRegPositionLimit + dMoveDistBeforeSearch;

#ifdef __SEARCH_INDEX_STOP_DISPLAY__
	if(stpSearchHomeFromLimitInput->iDebug == 1)
	{
		AfxMessageBox(_T("Move for a distance before searching index"));
	}
#endif // __SEARCH_INDEX_STOP_DISPLAY__
	if(!acsc_ToPoint(mHandle, 0, // start up immediately the motion
			iAxisOnACS, dTargetPositionBeforeSearchIndex, NULL) )
	{
		stpSearchHomeFromLimitOutput->iErrorCode = SEARCH_HOME_MOTOR_ERROR;
		iRet = MTN_API_ERROR;
		sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Searching Index- Moving-1! [%s %d] \n",	__FILE__, __LINE__);
		goto label_return_search_home_from_limit_acs;
	}
	if(fpData != NULL)
	{
		fprintf(fpData, "%% MoveToPointBeforeSearchLimit = %8.2f\n", dTargetPositionBeforeSearchIndex);
		fprintf(fpData, "%% ScopeSampleTime_ms = %8.4f \n", fSampleTime_ms);
		MTN_SPEED_PROFILE stSpeedProfileTemp;
		mtnapi_get_speed_profile(mHandle, iAxisOnACS, &stSpeedProfileTemp, 0);
		fprintf(fpData, "%%  MaxVel = %8.2f\n", stSpeedProfileTemp.dMaxVelocity);
		fprintf(fpData, "%%  MaxAcc = %8.2f\n", stSpeedProfileTemp.dMaxAcceleration);
		fprintf(fpData, "%%  MaxJerk = %8.2f\n", stSpeedProfileTemp.dMaxJerk);
		fprintf(fpData, "%%  MaxDec = %8.2f\n", stSpeedProfileTemp.dMaxDeceleration);

	}	
	// wait motion to complete
	mtnapi_get_motor_state(mHandle, iAxisOnACS, &iMotorState, 0);
	mtn_cto_tick_start_time_u1s(liFreqOS); // if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s)) // 20090508
	while(iMotorState & ACSC_MST_MOVE) // iFlagDoneMotionBuff8 == 0) // 
	{
		Sleep(2);
		mtnapi_get_motor_state(mHandle, iAxisOnACS, &iMotorState, 0);
	}
	// Wait axis to settle
	mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
	mtn_cto_tick_start_time_u1s(liFreqOS); // if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s)) // 20090508
	while(fabs(dPositionErrorFb) >= dPosnErrTH_Settle)
	{
		Sleep(2);
		mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
	}

	// Clear index state
	acsc_ResetIndexState(mHandle, iAxisOnACS, ACSC_IST_IND, NULL);

	// search 1st index by jogging, high speed
	mtnapi_get_ref_position(mHandle, iAxisOnACS, &dRegPositionBeforeSearching, 0);

#ifdef __SEARCH_INDEX_STOP_DISPLAY__
	if(stpSearchHomeFromLimitInput->iDebug == 1)
	{
		AfxMessageBox(_T("Jogging searching index"));
	}
#endif // __SEARCH_INDEX_STOP_DISPLAY__
	if(dMoveDistBeforeSearch >= 0 )
	{
		dJogVelocity =  fabs(stpSearchHomeFromLimitInput->dVelJoggingIndex1) * ACSC_POSITIVE_DIRECTION;
	}
	else
	{
		dJogVelocity =  fabs(stpSearchHomeFromLimitInput->dVelJoggingIndex1) * ACSC_NEGATIVE_DIRECTION;
	}
	acsc_SetVelocity(mHandle, iAxisOnACS, fabs(dJogVelocity), NULL); // setup the jogging velocity
	acsc_Jog(mHandle, iFlags, iAxisOnACS, dJogVelocity, NULL); // Immediate start

	if(fpData != NULL)
	{
		fprintf(fpData, "\n");
		fprintf(fpData, "%% MoveToPointBeforeSearchIndex_1 = %8.2f\n", dTargetPositionBeforeSearchIndex);
		MTN_SPEED_PROFILE stSpeedProfileTemp;
		mtnapi_get_speed_profile(mHandle, iAxisOnACS, &stSpeedProfileTemp, 0);
		fprintf(fpData, "%%  MaxVel = %8.2f\n", stSpeedProfileTemp.dMaxVelocity);
		fprintf(fpData, "%%  MaxAcc = %8.2f\n", stSpeedProfileTemp.dMaxAcceleration);
		fprintf(fpData, "%%  MaxJerk = %8.2f\n", stSpeedProfileTemp.dMaxJerk);
		fprintf(fpData, "%%  MaxDec = %8.2f\n", stSpeedProfileTemp.dMaxDeceleration);
		fprintf(fpData, "%% JoggingVel = %8.2f\n", dJogVelocity); // 

	}	

	uiCounterLast = GetPentiumTimeCount_per_100us(liFreqOS.QuadPart); // Register the current OS time
	if(	uiFlagIsDetectingByHighFreq == 1) 
	{
		// semaphore variable to protect s.t. the following routine could be executed no more than once at any time
		stpSearchHomeFromLimitOutput->iErrorCode = SEARCH_HOME_MULTIPLE_AXIS;
		iRet = MTN_API_ERROR;
		sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Searching Index- Semaphore Multi-axis! [%s %d] \n",	__FILE__, __LINE__);
		goto label_return_search_home_from_limit_acs;
	}
	else
	{
		uiFlagIsDetectingByHighFreq = 1;
	}
	int iIndexStateCurr; // Current index status
	acsc_GetIndexState(mHandle, iAxisOnACS, &iIndexStateCurr, NULL);

	uiCounterLast = GetPentiumTimeCount_per_100us(liFreqOS.QuadPart); // Register the current OS time
	while(!(iIndexStateCurr & ACSC_IST_IND))
	{
		uiCounterCurr = GetPentiumTimeCount_per_100us(liFreqOS.QuadPart); // timeGetTime();
		if(uiCounterCurr - uiCounterLast >= uiFreqFactor_10KHz 
			||	( (double) uiCounterCurr + UINT_MAX - uiCounterLast >= uiFreqFactor_10KHz 
			     && (double)uiCounterLast + uiFreqFactor_10KHz > UINT_MAX
				 )
		   )
		{
			// Get current position error feedback
			acsc_GetIndexState(mHandle, iAxisOnACS, &iIndexStateCurr, NULL);

			// update the counter
			uiCounterLast = uiCounterCurr;

			// Get the current feedback position for limit protection
			mtnapi_get_fb_position(mHandle, iAxisOnACS, &dFeedbackPositionCurrent, 0);
			if(fabs(dFeedbackPositionCurrent - dRegPositionBeforeSearching) >= dMaxDistRangeProt)
			{
				stpSearchHomeFromLimitOutput->iErrorCode = SEARCH_HOME_OUT_OF_DIST_RANGE;
				iRet = MTN_API_ERROR;
				sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Searching Index- Exceed maximum distance prot! [%s %d] \n",	__FILE__, __LINE__);
				goto label_return_search_home_from_limit_acs;
			}
		}
		else
		{
			Sleep(0);
		}
	}
	acsc_Halt(mHandle, iAxisOnACS, NULL);
	uiFlagIsDetectingByHighFreq = 0;
	// Wait axis to settle
	mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
	mtn_cto_tick_start_time_u1s(liFreqOS); // if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s)) // 20090508
	while(fabs(dPositionErrorFb) >= dPosnErrTH_Settle)
	{
		Sleep(2);
		mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
		//if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
		//{
		//	stpSearchHomeFromLimitOutput->iErrorCode = MTN_API_ERROR_SEARCH_TIMEOUT;
		//	iRet = MTN_API_ERROR;
		//	sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Searching Index- Not Settling-3! [%s %d] \n",	__FILE__, __LINE__);
		//	goto label_return_search_home_from_limit_acs;
		//} // 20090508
	}

	// Read index position
	double dIndexPositionCurr; // Current Index Position
	acsc_ReadReal(mHandle, ACSC_NONE, "IND", iAxisOnACS, iAxisOnACS, 0, 0, &dIndexPositionCurr, NULL);
	double dRegCurrPosition;
	stpSearchHomeFromLimitOutput->dDistanceFromIndex1ToLimit = dRegPositionLimit - dIndexPositionCurr;
	mtnapi_get_ref_position(mHandle, iAxisOnACS, &dRegCurrPosition, 0);
	dRegCurrPosition = dRegCurrPosition - dIndexPositionCurr;
	acsc_SetFPosition(mHandle, iAxisOnACS, dRegCurrPosition, 0);

	// move for a distance before 2nd search index
	mtnapi_set_speed_profile(mHandle, iAxisOnACS, &(stpSearchHomeFromLimitInput->stSpeedProfileMove), 0);
	dMoveDistBeforeSearch = stpSearchHomeFromLimitInput->dMoveDistanceBeforeSearchIndex2;
	dTargetPositionBeforeSearchIndex = dMoveDistBeforeSearch;

#ifdef __SEARCH_INDEX_STOP_DISPLAY__
	if(stpSearchHomeFromLimitInput->iDebug == 1)
	{
		AfxMessageBox(_T("Move after search index-1"));
	}
#endif // __SEARCH_INDEX_STOP_DISPLAY__
	if(! acsc_ToPoint(mHandle, 0, // start up immediately the motion
			iAxisOnACS, dTargetPositionBeforeSearchIndex, NULL) )
	{
		stpSearchHomeFromLimitOutput->iErrorCode = SEARCH_HOME_MOTOR_ERROR;
		iRet = MTN_API_ERROR;
		sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Searching Index- Moving -2! [%s %d] \n",	__FILE__, __LINE__);
		goto label_return_search_home_from_limit_acs;
	}
	// wait motion to complete
	mtnapi_get_motor_state(mHandle, iAxisOnACS, &iMotorState, 0);
	mtn_cto_tick_start_time_u1s(liFreqOS); // if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s)) // 20090508
	while(iMotorState & ACSC_MST_MOVE) // iFlagDoneMotionBuff8 == 0) // 
	{
		Sleep(2);
		mtnapi_get_motor_state(mHandle, iAxisOnACS, &iMotorState, 0);
		//if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
		//{
		//	stpSearchHomeFromLimitOutput->iErrorCode = MTN_API_ERROR_SEARCH_TIMEOUT;
		//	iRet = MTN_API_ERROR;
		//	sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Searching Index- Not Settling-4! [%s %d] \n",	__FILE__, __LINE__);
		//	goto label_return_search_home_from_limit_acs;
		//} // 20090508
	}
	// Wait axis to settle
	Sleep(2);
	mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
	mtn_cto_tick_start_time_u1s(liFreqOS); // if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s)) // 20090508
	while(fabs(dPositionErrorFb) >= dPosnErrTH_Settle)
	{
		Sleep(2);
		mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
		//if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s)) 
		//{
		//	stpSearchHomeFromLimitOutput->iErrorCode = MTN_API_ERROR_SEARCH_TIMEOUT;
		//	iRet = MTN_API_ERROR;
		//	sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Searching Index- Not Settling-5! [%s %d] \n",	__FILE__, __LINE__);
		//	goto label_return_search_home_from_limit_acs;
		//} // 20090508
	}

	// search 2nd index by jogging, low speed
	mtnapi_get_ref_position(mHandle, iAxisOnACS, &dRegPositionBeforeSearching, 0);

	dJogVelocity = stpSearchHomeFromLimitInput->dVelJoggingIndex2;
	acsc_SetVelocity(mHandle, iAxisOnACS, fabs(dJogVelocity), NULL); // setup the jogging velocity

	// Clear index state
	acsc_ResetIndexState(mHandle, iAxisOnACS, ACSC_IST_IND, NULL);
	if(dRegPositionBeforeSearching > 0)
	{
		acsc_Jog(mHandle, iFlags, iAxisOnACS, ACSC_NEGATIVE_DIRECTION, NULL);
	}
	else
	{
		acsc_Jog(mHandle, iFlags, iAxisOnACS, ACSC_POSITIVE_DIRECTION, NULL);
	}
#ifdef __SEARCH_INDEX_STOP_DISPLAY__
	if(stpSearchHomeFromLimitInput->iDebug == 1)
	{
		AfxMessageBox(_T("Jogging search index-1"));
	}
#endif // __SEARCH_INDEX_STOP_DISPLAY__

	if(fpData != NULL)
	{
		fprintf(fpData, "\n");
		fprintf(fpData, "%% MoveToPointBeforeSearchIndex_2 = %8.2f\n", dTargetPositionBeforeSearchIndex);
		MTN_SPEED_PROFILE stSpeedProfileTemp;
		mtnapi_get_speed_profile(mHandle, iAxisOnACS, &stSpeedProfileTemp, 0);
		fprintf(fpData, "%%  MaxVel = %8.2f\n", stSpeedProfileTemp.dMaxVelocity);
		fprintf(fpData, "%%  MaxAcc = %8.2f\n", stSpeedProfileTemp.dMaxAcceleration);
		fprintf(fpData, "%%  MaxJerk = %8.2f\n", stSpeedProfileTemp.dMaxJerk);
		fprintf(fpData, "%%  MaxDec = %8.2f\n", stSpeedProfileTemp.dMaxDeceleration);
		fprintf(fpData, "%% JoggingVel = %8.2f\n", dJogVelocity); // 
		fprintf(fpData, "%% RegFeedbackPosnBeforeSearchIdx2 = %8.1f\n", dRegPositionBeforeSearching);
	}	

	if(	uiFlagIsDetectingByHighFreq == 1) 
	{
		// semaphore variable to protect s.t. the following routine could be executed no more than once at any time
		stpSearchHomeFromLimitOutput->iErrorCode = SEARCH_HOME_MULTIPLE_AXIS;
		iRet = MTN_API_ERROR;
		sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Searching Index- Semaphore-MultipleAxis! [%s %d] \n",	__FILE__, __LINE__);
		goto label_return_search_home_from_limit_acs;
	}
	else
	{
		uiFlagIsDetectingByHighFreq = 1;
	}
	acsc_GetIndexState(mHandle, iAxisOnACS, &iIndexStateCurr, NULL);

	uiCounterLast = GetPentiumTimeCount_per_100us(liFreqOS.QuadPart); // Register the current OS time
	while(!(iIndexStateCurr & ACSC_IST_IND))
	{
		uiCounterCurr = GetPentiumTimeCount_per_100us(liFreqOS.QuadPart); // timeGetTime();
		if(uiCounterCurr - uiCounterLast >= uiFreqFactor_10KHz 
			||	( (double) uiCounterCurr + UINT_MAX - uiCounterLast >= uiFreqFactor_10KHz 
			     && (double)uiCounterLast + uiFreqFactor_10KHz > UINT_MAX
				 )
		   )
		{
			// Get current position error feedback
			acsc_GetIndexState(mHandle, iAxisOnACS, &iIndexStateCurr, NULL);

			// update the counter
			uiCounterLast = uiCounterCurr;

			// Get the current feedback position for limit protection
			mtnapi_get_fb_position(mHandle, iAxisOnACS, &dFeedbackPositionCurrent, 0);
			if(fabs(dFeedbackPositionCurrent - dRegPositionBeforeSearching) >= dMaxDistRangeProt)
			{
				stpSearchHomeFromLimitOutput->iErrorCode = SEARCH_HOME_OUT_OF_DIST_RANGE;
				iRet = MTN_API_ERROR;
				sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Searching Index- Exceed maximum dist prot! [%s %d] \n",	__FILE__, __LINE__);
				goto label_return_search_home_from_limit_acs;
			}
		}
		else
		{
			Sleep(0);
		}
	}
	acsc_Halt(mHandle, iAxisOnACS, NULL);
	// Wait axis to settle
	Sleep(2);
	mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
	mtn_cto_tick_start_time_u1s(liFreqOS); // if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s)) // 20090508
	while(fabs(dPositionErrorFb) >= dPosnErrTH_Settle)
	{
		Sleep(2);
		mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
		//if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
		//{
		//	stpSearchHomeFromLimitOutput->iErrorCode = MTN_API_ERROR_SEARCH_TIMEOUT;
		//	iRet = MTN_API_ERROR;
		//	sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Searching Index- Not Settling-6! [%s %d] \n",	__FILE__, __LINE__);
		//	goto label_return_search_home_from_limit_acs;
		//} // 20090508
	}

	double dIndexPositionFineSearch_1; // Current Index Position
	double dIndexPositionFineSearch_2;
	// read index position
	acsc_ReadReal(mHandle, ACSC_NONE, "IND", iAxisOnACS, iAxisOnACS, 0, 0, &dIndexPositionFineSearch_1, NULL);
	mtnapi_get_ref_position(mHandle, iAxisOnACS, &dRegCurrPosition, 0);
	stpSearchHomeFromLimitOutput->dDistanceFromIndex1ToLimit = stpSearchHomeFromLimitOutput->dDistanceFromIndex1ToLimit + dRegCurrPosition - dIndexPositionFineSearch_1;
	dRegCurrPosition = dRegCurrPosition - dIndexPositionFineSearch_1;
	acsc_SetFPosition(mHandle, iAxisOnACS, dRegCurrPosition, 0);
	uiFlagIsDetectingByHighFreq = 0; // reset the semaphore variable
	dIndexPositionFineSearch_2 = dIndexPositionFineSearch_1;

	if( stpSearchHomeFromLimitInput->iFlagVerifyByRepeatingIndex2 == FLAG_VERIFY_INDEX_BY_REPEATING_SEARCH)
	{
		////////////////////////////////////////////////////////
		// Re-search index position by Index2 parameters
		////////////////////////////////////////////////////////
		// move for a distance before 3rd search index
		mtnapi_set_speed_profile(mHandle, iAxisOnACS, &(stpSearchHomeFromLimitInput->stSpeedProfileMove), 0);
		dMoveDistBeforeSearch = stpSearchHomeFromLimitInput->dMoveDistanceBeforeSearchIndex2;
		dTargetPositionBeforeSearchIndex = dMoveDistBeforeSearch;

		if(! acsc_ToPoint(mHandle, 0, // start up immediately the motion
				iAxisOnACS, dTargetPositionBeforeSearchIndex, NULL) )
		{
			stpSearchHomeFromLimitOutput->iErrorCode = SEARCH_HOME_MOTOR_ERROR;
			iRet = MTN_API_ERROR;
			sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Searching Index- Moving! [%s %d] \n",	__FILE__, __LINE__);
			goto label_return_search_home_from_limit_acs;
		}
		// wait motion to complete
		mtnapi_get_motor_state(mHandle, iAxisOnACS, &iMotorState, 0);
		mtn_cto_tick_start_time_u1s(liFreqOS); // if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s)) // 20090508
		while(iMotorState & ACSC_MST_MOVE) // iFlagDoneMotionBuff8 == 0) // 
		{
			Sleep(2);
			mtnapi_get_motor_state(mHandle, iAxisOnACS, &iMotorState, 0);
			//if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
			//{
			//	stpSearchHomeFromLimitOutput->iErrorCode = MTN_API_ERROR_SEARCH_TIMEOUT;
			//	iRet = MTN_API_ERROR;
			//	sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Searching Index- Not Settling-6! [%s %d] \n",	__FILE__, __LINE__);
			//	goto label_return_search_home_from_limit_acs;
			//} // 20090508
		}
		// Wait axis to settle
		Sleep(2);
		mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
		mtn_cto_tick_start_time_u1s(liFreqOS); // if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s)) // 20090508
		while(fabs(dPositionErrorFb) >= dPosnErrTH_Settle)
		{
			Sleep(2);
			mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
			//if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
			//{
			//	stpSearchHomeFromLimitOutput->iErrorCode = MTN_API_ERROR_SEARCH_TIMEOUT;
			//	iRet = MTN_API_ERROR;
			//	sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Searching Index- Not Settling-7! [%s %d] \n",	__FILE__, __LINE__);
			//	goto label_return_search_home_from_limit_acs;
			//} // 20090508
		}

		// search 3rd index by jogging, low speed
		mtnapi_get_ref_position(mHandle, iAxisOnACS, &dRegPositionBeforeSearching, 0);

		dJogVelocity = stpSearchHomeFromLimitInput->dVelJoggingIndex2;
		acsc_SetVelocity(mHandle, iAxisOnACS, fabs(dJogVelocity), NULL); // setup the jogging velocity

		// Clear index state
		acsc_ResetIndexState(mHandle, iAxisOnACS, ACSC_IST_IND, NULL);
		if(dRegPositionBeforeSearching > 0)
		{
			acsc_Jog(mHandle, iFlags, iAxisOnACS, ACSC_NEGATIVE_DIRECTION, NULL);
		}
		else
		{
			acsc_Jog(mHandle, iFlags, iAxisOnACS, ACSC_POSITIVE_DIRECTION, NULL);
		}

		if(fpData != NULL)
		{
			fprintf(fpData, "\n");
			fprintf(fpData, "%% MoveToPointBeforeSearchIndex_3 = %8.2f\n", dTargetPositionBeforeSearchIndex);
			MTN_SPEED_PROFILE stSpeedProfileTemp;
			mtnapi_get_speed_profile(mHandle, iAxisOnACS, &stSpeedProfileTemp, 0);
			fprintf(fpData, "%%  MaxVel = %8.2f\n", stSpeedProfileTemp.dMaxVelocity);
			fprintf(fpData, "%%  MaxAcc = %8.2f\n", stSpeedProfileTemp.dMaxAcceleration);
			fprintf(fpData, "%%  MaxJerk = %8.2f\n", stSpeedProfileTemp.dMaxJerk);
			fprintf(fpData, "%%  MaxDec = %8.2f\n", stSpeedProfileTemp.dMaxDeceleration);
			fprintf(fpData, "%% JoggingVel = %8.2f\n", dJogVelocity); // 
			fprintf(fpData, "%% RegFeedbackPosnBeforeSearchIdx3 = %8.1f\n", dRegPositionBeforeSearching);
		}	

		if(	uiFlagIsDetectingByHighFreq == 1) 
		{
			// semaphore variable to protect s.t. the following routine could be executed no more than once at any time
			stpSearchHomeFromLimitOutput->iErrorCode = SEARCH_HOME_MULTIPLE_AXIS;
			iRet = MTN_API_ERROR;
			sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Searching Index- Semaphore3-Multi-Axis, ! [%s %d] \n",	__FILE__, __LINE__);
			goto label_return_search_home_from_limit_acs;
		}
		else
		{
			uiFlagIsDetectingByHighFreq = 1;
		}
		acsc_GetIndexState(mHandle, iAxisOnACS, &iIndexStateCurr, NULL);

		uiCounterLast = GetPentiumTimeCount_per_100us(liFreqOS.QuadPart); // Register the current OS time
		while(!(iIndexStateCurr & ACSC_IST_IND))
		{
			uiCounterCurr = GetPentiumTimeCount_per_100us(liFreqOS.QuadPart); // timeGetTime();
			if(uiCounterCurr - uiCounterLast >= uiFreqFactor_10KHz 
				||	( (double) uiCounterCurr + UINT_MAX - uiCounterLast >= uiFreqFactor_10KHz 
					 && (double)uiCounterLast + uiFreqFactor_10KHz > UINT_MAX
					 )
			   )
			{
				// Get current position error feedback
				acsc_GetIndexState(mHandle, iAxisOnACS, &iIndexStateCurr, NULL);

				// update the counter
				uiCounterLast = uiCounterCurr;

				// Get the current feedback position for limit protection
				mtnapi_get_fb_position(mHandle, iAxisOnACS, &dFeedbackPositionCurrent, 0);
				if(fabs(dFeedbackPositionCurrent - dRegPositionBeforeSearching) >= dMaxDistRangeProt)
				{
					stpSearchHomeFromLimitOutput->iErrorCode = SEARCH_HOME_OUT_OF_DIST_RANGE;
					iRet = MTN_API_ERROR;
					sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Searching Index- Exceed Maximum Dist Prot! [%s %d] \n",	__FILE__, __LINE__);
					goto label_return_search_home_from_limit_acs;
				}
			}
			else
			{
				Sleep(0);
			}
		}
		acsc_Halt(mHandle, iAxisOnACS, NULL);
		// Wait axis to settle
		Sleep(2);
		mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
		mtn_cto_tick_start_time_u1s(liFreqOS); // if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s)) // 20090508
		while(fabs(dPositionErrorFb) >= dPosnErrTH_Settle)
		{
			Sleep(2);
			mtn_api_get_position_err(mHandle, iAxisOnACS, &dPositionErrorFb);
			//if(mtn_cto_is_time_out_u1s(liFreqOS, uiSearchWaitingTimeOut_u1s))
			//{
			//	stpSearchHomeFromLimitOutput->iErrorCode = MTN_API_ERROR_SEARCH_TIMEOUT;
			//	iRet = MTN_API_ERROR;
			//	sprintf_s(strDebugErrorMessageHomeACS, 512, "Error Searching Index- Not Settling-6! [%s %d] \n",	__FILE__, __LINE__);
			//	goto label_return_search_home_from_limit_acs;
			//} // 20090508
		}

		// read index position
		acsc_ReadReal(mHandle, ACSC_NONE, "IND", iAxisOnACS, iAxisOnACS, 0, 0, &dIndexPositionFineSearch_2, NULL);
		mtnapi_get_ref_position(mHandle, iAxisOnACS, &dRegCurrPosition, 0);
		stpSearchHomeFromLimitOutput->dDistanceFromIndex2ToLimit = stpSearchHomeFromLimitOutput->dDistanceFromIndex1ToLimit + dRegCurrPosition - dIndexPositionFineSearch_2;
		dRegCurrPosition = dRegCurrPosition - dIndexPositionFineSearch_2;
		acsc_SetFPosition(mHandle, iAxisOnACS, dRegCurrPosition, 0);
	}

	stpSearchHomeFromLimitOutput->dIndexPositionFineSearch_1 = dIndexPositionFineSearch_1;
	stpSearchHomeFromLimitOutput->dIndexPositionFineSearch_2 = dIndexPositionFineSearch_2;

	if(fpData != NULL)
	{
		acsc_ReadReal(mHandle, ACSC_NONE, gstrScopeDataVarName, 0, gstSystemScope.uiNumData - 1, 0, gstSystemScope.uiDataLen - 1, gdScopeCollectData, NULL);
		fprintf(fpData, "%% Search Home Debug Data, Axis: %d\n", iAxisOnACS);
		fprintf(fpData, "matSearchHomeData = [ %% RPOS, PE, RVEL, IND, DOUT\n");
		for(unsigned int ii = 0; ii< gstSystemScope.uiDataLen; ii++)
		{
			if(ii < gstSystemScope.uiDataLen - 1)
			{
				fprintf(fpData, "%8.1f, %8.1f, %8.2f, %8.0f, %8.0f\n", 
					gdScopeCollectData[ii], gdScopeCollectData[gstSystemScope.uiDataLen + ii], gdScopeCollectData[gstSystemScope.uiDataLen *2 + ii],
					gdScopeCollectData[gstSystemScope.uiDataLen *3 + ii], gdScopeCollectData[gstSystemScope.uiDataLen *4 + ii]);
			}
			else
			{
				fprintf(fpData, "%8.1f, %8.1f, %8.2f, %8.0f, %8.0f];\n", 
					gdScopeCollectData[ii], gdScopeCollectData[gstSystemScope.uiDataLen + ii], gdScopeCollectData[gstSystemScope.uiDataLen *2 + ii],
					gdScopeCollectData[gstSystemScope.uiDataLen *3 + ii], gdScopeCollectData[gstSystemScope.uiDataLen *4 + ii]);
			}
		}
		fprintf(fpData, "DistanceFromLimitToIndex1 = %8.2f\n", stpSearchHomeFromLimitOutput->dDistanceFromIndex1ToLimit);
		fprintf(fpData, "DistanceFromLimitToIndex2 = %8.2f\n", stpSearchHomeFromLimitOutput->dDistanceFromIndex2ToLimit);
		fprintf(fpData, "DiffDistance_From2Indexes_ToLimit = DistanceFromLimitToIndex1 - DistanceFromLimitToIndex2 \n");
		fprintf(fpData, "dIndexPositionFineSearch_1 = %8.2f\n", stpSearchHomeFromLimitOutput->dIndexPositionFineSearch_1);
		fprintf(fpData, "dIndexPositionFineSearch_2 = %8.2f\n", stpSearchHomeFromLimitOutput->dIndexPositionFineSearch_2);
		fprintf(fpData, "DiffIndexPosition_2FineSearch = dIndexPositionFineSearch_1 - dIndexPositionFineSearch_2\n");
	}


label_return_search_home_from_limit_acs:
	if(fpData != NULL)
	{
		fclose(fpData);
	}

	// restore the bakuped speed profile
	mtnapi_set_speed_profile(mHandle, iAxisOnACS, &stSpeedProfileBak, 0);
	uiFlagIsDetectingByHighFreq = 0; // reset the semaphore variable

	return iRet;
}


