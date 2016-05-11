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

// Module: Motion Application Programming Interface
// 
// History
// YYYYMMDD  Author			Notes
// 20090116  Zhengyi        Created pseudo-code from ACS controller buffer program
// 20090119  ZhengyiJohn    Testing
// 20090120  ZhengyiJohn	Prototyping with ZhangDong
// 20090311  ZhengyiJohn    Add protection for very small distance motion
// 20090508  ZhengyiJohn    Add different parameter, time-out protecting for search
// 20090512  ZhengyiJohn	Add bakup and restore parameter in api functions
#include "stdafx.h"
#include <math.h>

#include "MtnApi.h"
#include "MtnSearchHome.h"
#include "MtnApiSearchHome.h"
#include "MtnInitAcs.h"


#undef MOTALGO_DLL_EXPORTS
#include "MotAlgo_DLL.h"

// Offset X
void mtn_init_home_set_x_table_offset_startbond(double dOffsetTableX)
{
	mtn_dll_init_home_set_x_table_offset_startbond(dOffsetTableX); // dPositionTableX_StartBonding_HomeZ = dOffsetTableX;
}

double mtn_init_home_get_x_table_offset_startbond()
{
	return mtn_dll_init_home_get_x_table_offset_startbond(); // dPositionTableX_StartBonding_HomeZ;
}
// Offset Y
void mtn_init_home_set_y_table_offset_startbond(double dOffsetTableY)
{
	mtn_dll_init_home_set_y_table_offset_startbond(dOffsetTableY); // dPositionTableY_StartBonding_HomeZ = dOffsetTableY;
}

double mtn_init_home_get_y_table_offset_startbond()
{
	return mtn_dll_init_home_get_y_table_offset_startbond(); // dPositionTableY_StartBonding_HomeZ;
}

void mtn_init_home_set_z_bondhead_firelevel(double dBondHeadFireLevel)
{
	mtn_dll_init_home_set_z_bondhead_firelevel(dBondHeadFireLevel); // dPositionBondHead_FireLevel = dBondHeadFireLevel;
}

double mtn_init_home_get_z_bondhead_firelevel()
{
	return mtn_dll_init_home_get_z_bondhead_firelevel(); // dPositionBondHead_FireLevel;
}

void mtn_init_def_para_search_index_vled_bonder_xyz(int iAxisX, int iAxisY, int iAxisZ)
{
	mtn_dll_init_def_para_search_index_vled_bonder_xyz(iAxisX, iAxisY, iAxisZ);
}


//#ifdef __RELEASE_MORE__
#include "MtnSearchHome.h"
#include "MtnApiSearchHome.h"

extern MOTALGO_DLL_API int mtn_dll_api_search_home_acs(HANDLE mHandle, MTN_API_SEARCH_INDEX_INPUT *stpSearchHomeInputAPI, MTN_API_SEARCH_INDEX_OUTPUT *stpSearcihHomeOutputAPI);
extern MOTALGO_DLL_API int mtn_dll_api_search_limit_acs(HANDLE mHandle, MTN_API_SEARCH_LIMIT_INPUT *stpSearchLimitInput, MTN_API_SEARCH_LIMIT_OUTPUT *stpSearchLimitOutput);
extern MOTALGO_DLL_API int mtn_dll_api_search_home_from_limit_acs(HANDLE mHandle, MTN_API_SEARCH_HOME_FROM_LIMIT_INPUT *stpSearchHomeFromLimitInput, MTN_API_SEARCH_HOME_FROM_LIMIT_OUTPUT *stpSearchHomeFromLimitOutput);
extern MOTALGO_DLL_API int mtn_dll_search_home_acs(HANDLE mHandle, MTN_SEARCH_INDEX_INPUT *stpSearchHomeInput, MTN_SEARCH_INDEX_OUTPUT *stpSearchHomeOutput);

// extern MOTALGO_DLL_API void mtn_init_def_para_search_index_vled_bonder_xyz(int iAxisX, int iAxisY, int iAxisZ);
// extern MOTALGO_DLL_API int mtn_bonder_xyz_start_search_limit_go_home(HANDLE Handle, int iAxisX, int iAxisY, int iAxisZ);
//#endif  // __RELEASE_MORE__


int mtn_api_search_home_acs(HANDLE mHandle, 
							MTN_API_SEARCH_INDEX_INPUT *stpSearchHomeInputAPI, 
							MTN_API_SEARCH_INDEX_OUTPUT *stpSearcihHomeOutputAPI)
{
	return mtn_dll_api_search_home_acs(mHandle, 
							stpSearchHomeInputAPI, 
							stpSearcihHomeOutputAPI);
}

int mtn_api_search_limit_acs(HANDLE mHandle, MTN_API_SEARCH_LIMIT_INPUT *stpSearchLimitInput, MTN_API_SEARCH_LIMIT_OUTPUT *stpSearchLimitOutput)
{
	return mtn_dll_api_search_limit_acs(mHandle, 
		stpSearchLimitInput, 
		stpSearchLimitOutput);
}

int mtn_api_search_home_from_limit_acs(HANDLE mHandle, MTN_API_SEARCH_HOME_FROM_LIMIT_INPUT *stpSearchHomeFromLimitInput, MTN_API_SEARCH_HOME_FROM_LIMIT_OUTPUT *stpSearchHomeFromLimitOutput)
{
	return mtn_dll_api_search_home_from_limit_acs(mHandle, 
		stpSearchHomeFromLimitInput, 
		stpSearchHomeFromLimitOutput);
}

int mtn_search_home_acs(HANDLE mHandle, MTN_SEARCH_INDEX_INPUT *stpSearchHomeInput, MTN_SEARCH_INDEX_OUTPUT *stpSearchHomeOutput)
{
	return mtn_dll_search_home_acs(mHandle, 
		stpSearchHomeInput, 
		stpSearchHomeOutput);
}

int mtn_axis_search_limit(HANDLE Handle, int iAxis)
{
	return mtn_dll_axis_search_limit(Handle, iAxis);
}

int mtn_axis_go_home_from_limit(HANDLE Handle, int iAxis)
{
	return mtn_dll_axis_go_home_from_limit(Handle, iAxis);
}

int mtn_axis_search_limit_go_home(HANDLE Handle, int iAxis)
{
	return mtn_dll_axis_search_limit_go_home(Handle, iAxis); // mtn_search_home_acs(Handle, &stSearchIndexInput[iAxis], &stSearchIndexOutput[iAxis]);
}

int mtn_bonder_xyz_start_search_limit_go_home(HANDLE Handle, int iAxisX, int iAxisY, int iAxisZ)
{
	int iRet = mtn_dll_bonder_xyz_start_search_limit_go_home(Handle, iAxisX, iAxisY, iAxisZ);

	return iRet;
}
//	// Backup the current speed profile
//	mtnapi_get_speed_profile(mHandle, iAxis, &stSpeedProfileBak, 0);
//
//	// setup new profile for moving within this function
//	mtnapi_set_speed_profile(mHandle, iAxis, &(stpSearchHomeInput->stSpeedProfileMove), 0);
//
//	// Enable the motor if it is not enabled
//	mtnapi_get_motor_state(mHandle, iAxis, &iMotorState, 0);
//	if(iMotorState & ACSC_MST_ENABLE) 
//	{
//		// now is enabled, OK
//	}
//	else
//	{ // Motor is not enabled, we have to enable it first
//		if(mtnapi_enable_motor(mHandle, iAxis, 0) != MTN_API_OK_ZERO)
//		{
//			stpSearchHomeOutput->iErrorCode = SEARCH_HOME_MOTOR_ERROR;
//			iRet = MTN_API_ERROR;
//			goto label_return_search_home_acs;
//		}
//	}
//
//	// Move to a distance before searching limit
//	mtnapi_get_ref_position(mHandle, iAxis, &dblRefPosition, 0);
//
//	double dMoveDistBeforeSearch = stpSearchHomeInput->dMoveDistanceBeforeSearchLimit;
//	double dTargetPositionBeforeSearchLimit = dblRefPosition + dMoveDistBeforeSearch;
//
//	if(stpSearchHomeInput->iDebug == 1)
//	{
////		AfxMessageBox(_T("Move for a distance before searching limit"));
//	}
//	if(stpSearchHomeInput->iDebug == 1)
//	{
//		acsc_CollectB(mHandle, 0, // system data collection
//				gstrScopeArrayName, // name of data collection array
//				gstSystemScope.uiDataLen, // number of samples to be collected
//				fSampleTime_ms, // sampling period 1 millisecond
//				strACSC_VarName, // variables to be collected
//				NULL);
//	}
//
//	if(! acsc_ToPoint(mHandle, 0, // start up the motion immediately
//			iAxis, dTargetPositionBeforeSearchLimit, NULL) )
//	{
//		stpSearchHomeOutput->iErrorCode = SEARCH_HOME_MOTOR_ERROR;
//		iRet = MTN_API_ERROR;
//		goto label_return_search_home_acs;
//	}
//	if(fpData != NULL)
//	{
//		fprintf(fpData, "%% MoveToPointBeforeSearchLimit = %8.2f\n", dTargetPositionBeforeSearchLimit);
//		fprintf(fpData, "%% ScopeSampleTime_ms = %8.4f \n", fSampleTime_ms);
//		MTN_SPEED_PROFILE stSpeedProfileTemp;
//		mtnapi_get_speed_profile(mHandle, iAxis, &stSpeedProfileTemp, 0);
//		fprintf(fpData, "%%  MaxVel = %8.2f\n", stSpeedProfileTemp.dMaxVelocity);
//		fprintf(fpData, "%%  MaxAcc = %8.2f\n", stSpeedProfileTemp.dMaxAcceleration);
//		fprintf(fpData, "%%  MaxJerk = %8.2f\n", stSpeedProfileTemp.dMaxJerk);
//		fprintf(fpData, "%%  MaxDec = %8.2f\n", stSpeedProfileTemp.dMaxDeceleration);
//
//	}	
//	// Wait axis to settle
//	double dPositionErrorFb;
//	double dPosnErrTH_Settle = stpSearchHomeInput->dPosnErrThresHoldSettling;
//	mtn_api_get_position_err(mHandle, iAxis, &dPositionErrorFb);
//	while(fabs(dPositionErrorFb) >= dPosnErrTH_Settle)
//	{
//		Sleep(10);
//		mtn_api_get_position_err(mHandle, iAxis, &dPositionErrorFb);
//	}
//
//	// search mechanical limit by jogging
//	mtnapi_get_ref_position(mHandle, iAxis, &dRegPositionBeforeSearching, 0);
//
//	double dJogVelocity; // search velocity. NO wait command. Axis the searching axis.
//	if(dMoveDistBeforeSearch > 0 )
//	{
//		dJogVelocity = fabs(stpSearchHomeInput->dVelJoggingLimit) * ACSC_POSITIVE_DIRECTION; //  ;
//	}
//	else
//	{
//		dJogVelocity = fabs(stpSearchHomeInput->dVelJoggingLimit) * ACSC_NEGATIVE_DIRECTION; //  ;
//	}
//	acsc_SetVelocity(mHandle, iAxis, fabs(dJogVelocity), NULL); // setup the jogging velocity
//	acsc_Jog(mHandle, iFlags, iAxis, dJogVelocity, NULL); // Immediate start
//
//	QueryPerformanceFrequency(&liFreqOS); // Get OS Frequency
//
//	// detection of the position error
//	unsigned int uiCounterLast = GetPentiumTimeCount_per_100us(liFreqOS.QuadPart); // timeGetTime();
//	unsigned int uiCounterCurr;
//
//	mtn_api_get_position_err(mHandle, iAxis, &dPositionErrorFb);
//
//	if(	uiFlagIsDetectingByHighFreq == 1) 
//	{
//		// semaphore variable to protect s.t. the following routine could be executed no more than once at any time
//		stpSearchHomeOutput->iErrorCode = SEARCH_HOME_MULTIPLE_AXIS;
//		iRet = MTN_API_ERROR;
//		goto label_return_search_home_acs;
//	}
//	else
//	{
//		uiFlagIsDetectingByHighFreq = 1;
//	}
//	while(fabs(dPositionErrorFb) <= dPositionErrorTH)
//	{
//		uiCounterCurr = GetPentiumTimeCount_per_100us(liFreqOS.QuadPart); // Register the current OS time
//		if(uiCounterCurr - uiCounterLast >= uiFreqFactor_10KHz 
//			||	( (double) uiCounterCurr + UINT_MAX - uiCounterLast >= uiFreqFactor_10KHz 
//			     && (double)uiCounterLast + uiFreqFactor_10KHz > UINT_MAX
//				 )
//		   )
//		{   // Get position error
//			mtn_api_get_position_err(mHandle, iAxis, &dPositionErrorFb);
//
//			// update the counter
//			uiCounterLast = uiCounterCurr;
//
//			// Get the current feedback position for limit protection
//			mtnapi_get_fb_position(mHandle, iAxis, &dFeedbackPositionCurrent, 0);
//			if(fabs(dFeedbackPositionCurrent - dRegPositionBeforeSearching) >= dMaxDistRangeProt)
//			{
//				stpSearchHomeOutput->iErrorCode = SEARCH_HOME_OUT_OF_DIST_RANGE;
//				iRet = MTN_API_ERROR;
//				goto label_return_search_home_acs;
//			}
//		}
//		else
//		{
//			Sleep(0);
//		}
//	}
//	acsc_Halt(mHandle, iAxis, NULL);
//	uiFlagIsDetectingByHighFreq = 0; // reset the semaphore variable
//	// Wait axis to settle
//	mtn_api_get_position_err(mHandle, iAxis, &dPositionErrorFb);
//	while(fabs(dPositionErrorFb) >= dPosnErrTH_Settle)
//	{
//		Sleep(10);
//		mtn_api_get_position_err(mHandle, iAxis, &dPositionErrorFb);
//	}
//
//	// record the limit position
//	mtnapi_get_fb_position(mHandle, iAxis, &dRegPositionLimit, 0);
//

// Following is another implementation by Spii+

// It serves as a guide-line for the function of searching limit and then searching home
// firmware buffer program to perform searching index
//! The program executes the following sequence:
//! - Move to the left limit switch.
//! - Wait for the left limit release 
//! - Move to the encoder index.
//! - Set the axis origin to the position of index.
//! - Move to the origin. 
//GLOBAL INT AXIS        ! Define a global variable named "AXIS"
//AXIS=0                 ! Define the axis name (0=X axis, 1=Y axis...)
//VEL(AXIS)= 2000        ! Set maximum velocity
//ACC(AXIS)= 100000      ! Set acceleration
//DEC(AXIS)= 100000      ! Set deceleration
//JERK(AXIS)= 20000000   ! Set jerk
//KDEC(AXIS)= 200000     ! Set kill deceleration
//FDEF(AXIS).#LL=0       ! Disable the axis left limit default response
//FDEF(AXIS).#RL=0       ! Disable the axis right limit default response
//ENABLE (AXIS)          ! Enable the axis drive
//
//JOG (AXIS),-           ! Move to the left limit switch 
//TILL FAULT(AXIS).#LL   ! Wait for the left limit switch activation
//  ! Can be written also as "TILL ABS(X_PE)>???" when no limit switches exist - only a hard stop.
//JOG (AXIS),+           ! Move to the encoder index
//TILL ^FAULT(AXIS).#LL  ! Wait for the left limit release
//IST(AXIS).#IND=0       ! Reset the index flag - activate index circuit
//TILL IST(AXIS).#IND    ! Wait for crossing the index
//SET FPOS(AXIS)=FPOS(AXIS)-IND(AXIS)  ! Set axis origin to the position of index = zero
//PTP (AXIS),0           ! Move to the origin
//FDEF(AXIS).#LL=1       ! Enable the axis left limit default response
//FDEF(AXIS).#RL=1       ! Enable the axis right limit default response
//STOP         
