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

// 20100429 XY table max acc
// 20110111  Zhengyi Modify naming for the ACS applications

#pragma once
#ifndef __MTN_SEARCH_HOME__
#define __MTN_SEARCH_HOME__

#include "MtnApi.h"

typedef struct
{
	int iAxisOnACS;
	unsigned int uiFreqFactor_10KHz_Detecting;
	double dMaxDistanceRangeProtection;
	MTN_SPEED_PROFILE stSpeedProfileMove;
	double dMoveDistanceBeforeSearchLimit;
	double dVelJoggingLimit;
	double dPositionErrorThresholdSearchLimit;

	double dMoveDistanceBeforeSearchIndex1;
	double dVelJoggingIndex1;

	double dMoveDistanceBeforeSearchIndex2;
	double dVelJoggingIndex2;

	double dPosnErrThresHoldSettling;
	int iDebug;
	double dDriveCmdThresholdProtection; // 20120927

	int iFlagVerifyByRepeatingIndex2;

}MTN_SEARCH_INDEX_INPUT;

typedef struct
{
	double dDistanceFromIndex1ToLimit;
	double dDistanceFromIndex2ToLimit;
	double dIndexPositionFineSearch_1;
	double dIndexPositionFineSearch_2;

	int iErrorCode;
}MTN_SEARCH_INDEX_OUTPUT;

typedef struct
{
	int iAxisOnACS;
	unsigned int uiFreqFactor_10KHz_Detecting;
	double dMaxDistanceRangeProtection;
	MTN_SPEED_PROFILE stSpeedProfileMove;
	double dMoveDistanceBeforeSearchLimit;
	double dVelJoggingLimit;
	double dPositionErrorThresholdSearchLimit;

	double dDriveCmdThresholdProtection; // 20120927

	double dMoveDistanceAfterSearchLimit;

	double dPosnErrThresHoldSettling;
	int iDebug;

}MTN_SEARCH_LIMIT_INPUT;

typedef struct
{	double dLimitPosition;
	int iErrorCode;
}MTN_SEARCH_LIMIT_OUTPUT;

typedef struct{
	int iAxisOnACS;
	unsigned int uiFreqFactor_10KHz_Detecting;
	double dMaxDistanceRangeProtection;
	MTN_SPEED_PROFILE stSpeedProfileMove;

	double dMoveDistanceBeforeSearchIndex1;
	double dVelJoggingIndex1;

	double dMoveDistanceBeforeSearchIndex2;
	double dVelJoggingIndex2;

	double dPosnErrThresHoldSettling;
	int iDebug;

	int iFlagVerifyByRepeatingIndex2;

}MTN_SEARCH_HOME_FROM_LIMIT_INPUT;

typedef struct{
	double dIndexPositionFineSearch_1;
	double dIndexPositionFineSearch_2;
	double dDistanceFromIndex1ToLimit;
	double dDistanceFromIndex2ToLimit;
	int iErrorCode;
}MTN_SEARCH_HOME_FROM_LIMIT_OUTPUT;

#define SEARCH_HOME_DEBUG_FILENAME      "SrchHome.m"
#define SEARCH_LIMIT_DEBUG_FILENAME     "SrchLmt.m"
extern int mtn_search_limit_acs(HANDLE mHandle, MTN_SEARCH_LIMIT_INPUT *stpSearchLimitInput, MTN_SEARCH_LIMIT_OUTPUT *stpSearchLimitOutput);
extern int mtn_search_home_from_limit_acs(HANDLE mHandle, MTN_SEARCH_HOME_FROM_LIMIT_INPUT *stpSearchHomeFromLimitInput, MTN_SEARCH_HOME_FROM_LIMIT_OUTPUT *stpSearchHomeFromLimitOutput);

// Integrated functions
int mtn_axis_search_limit(HANDLE Handle, int iAxis);
int mtn_axis_go_home_from_limit(HANDLE Handle, int iAxis);
int mtn_axis_search_limit_go_home(HANDLE Handle, int iAxis);

//Port to DLL
//extern int mtn_search_home_acs(HANDLE mHandle, MTN_SEARCH_INDEX_INPUT *stpSearchHomeInput, MTN_SEARCH_INDEX_OUTPUT *stpSearchHomeOutput);

#define DEFAULT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ   1

#define DEFAULT_SEARCH_HOME_MOVE_ACC_CNT_SEC  1E+8
#define DEFAULT_SEARCH_HOME_MOVE_DEC_CNT_SEC  1E+8
#define DEFAULT_SEARCH_HOME_MOVE_JERK_CNT_SEC  5E+8

#define DEFAULT_SEARCH_HOME_MOVE_VEL_Z  1E+5
#define DEFAULT_SEARCH_HOME_MOVE_ACC_Z  1E+8
#define DEFAULT_SEARCH_HOME_MOVE_DEC_Z  1E+8
#define DEFAULT_SEARCH_HOME_MOVE_JERK_Z  5E+8

#define DEFAULT_SEARCH_HOME_MOVE_VEL_TBL  1E+5
// X: 45 m/s/s
#define DEFAULT_SEARCH_HOME_MOVE_ACC_TBL_X  6E+7
#define DEFAULT_SEARCH_HOME_MOVE_DEC_TBL_X  6E+7
#define DEFAULT_SEARCH_HOME_MOVE_JERK_TBL_X  2E+8

// Y: 23 m/s/s, 2E6 cnt/m, 15m/s/s
#define DEFAULT_SEARCH_HOME_MOVE_ACC_TBL_Y  3E+7
#define DEFAULT_SEARCH_HOME_MOVE_DEC_TBL_Y  3E+7
#define DEFAULT_SEARCH_HOME_MOVE_JERK_TBL_Y  2E+8

#define DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_Z   17000
#define DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_X   380000
#define DEFAULT_SEARCH_HOME_DETECT_MAX_DIST_PROT_Y   120000

#define DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_X   10
#define DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_Y   10
#define DEFAULT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT_Z   10


#define DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_TBL   50000
// 20110910
#define DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_TBL     30
#define DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_TBL_X 40000
// 20110519 from 100000

// 20110807
#define DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT_Z     5000
#define DEFAULT_SEARCH_HOME_PE_TH_SEARCH_LIMIT_Z       15

#define DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_Z  -50
#define DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_Z     -1000

#define DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_X  -5000
// 20110922
#define DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_X        -20000
// 20110519
#define DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_X_HORI_BONDER  -5000

#define DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1_Y  -5000
#define DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1_Y       -6000

#define DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2  500
// 20121116
#define DEFAULT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2_XY_RENISHAW  2000

#define DEFAULT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX    800

#define DEFAULT_SEARCH_HOME_POSITION_SETTLE_TH   20

#endif // __MTN_SEARCH_HOME__
