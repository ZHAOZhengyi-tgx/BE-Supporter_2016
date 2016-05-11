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

// 20110111  Zhengyi Modify naming for the ACS applications

#pragma once
#ifndef __MTN_API_SEARCH_HOME__
#define __MTN_API_SEARCH_HOME__

#define MTN_API_AXIS_X_ON_ACS         0
#define MTN_API_AXIS_Y_ON_ACS         1
#define MTN_API_AXIS_Z_ON_ACS         4

#define SEARCH_HOME_MOTOR_ERROR        1
#define SEARCH_HOME_OUT_OF_DIST_RANGE  2
#define SEARCH_HOME_MULTIPLE_AXIS      3
#define SEARCH_HOME_ERROR_1ST_MOVE_DIST  4
// Default move-to position after XZY searching home
#define BE_WB_TABLE_POSN_X_START_BOND_Z_HOME   -138000
#define BE_WB_TABLE_POSN_Y_START_BOND_Z_HOME   39000
#define BE_WB_BH_FIRE_LEVEL					   5000
#define BE_WB_BH_FIRE_LEVEL_HORI_LED_1         100
#define BE_WB_TABLE_POSN_X_START_HORI_BONDER_Z_HOME   -55000
#define BE_WB_TABLE_POSN_X_START_1_CUP_BOND_Z_HOME   60000
#define BE_WB_TABLE_POSN_Y_START_1_CUP_BOND_Z_HOME   1000
typedef struct{
	int iAxisOnACS;
	double dMaxDistanceRangeProtection;
	double dMoveMaxVelInterSearch;

	double dMoveDistanceBeforeSearchLimit;
	double dVelJoggingLimit;
	double dPositionErrorThresholdSearchLimit;

	double dMoveDistanceBeforeSearchIndex1;
	double dVelJoggingIndex1;

	double dMoveDistanceBeforeSearchIndex2;
	double dVelJoggingIndex2;

	double dPosnErrThresHoldSettling;
	int iDebug;

	int iFlagVerifyByRepeatingIndex2;

}MTN_API_SEARCH_INDEX_INPUT;

typedef struct{
	double dIndexPositionFineSearch_1;
	double dIndexPositionFineSearch_2;
	int iErrorCode;
}MTN_API_SEARCH_INDEX_OUTPUT;

typedef struct{
	int iAxisOnACS;
	double dMaxDistanceRangeProtection;
	double dMoveMaxVelInterSearch;

	double dMoveDistanceBeforeSearchLimit;
	double dVelJoggingLimit;
	double dPositionErrorThresholdSearchLimit;

	double dPosnErrThresHoldSettling;

	double dMoveDistanceAfterSearchLimit;
}MTN_API_SEARCH_LIMIT_INPUT;

typedef struct{
	double dLimitPosition;
	int iErrorCode;
}MTN_API_SEARCH_LIMIT_OUTPUT;

typedef struct{
	int iAxisOnACS;

	double dMaxDistanceRangeProtection;
	double dMoveMaxVelInterSearch;

	double dMoveDistanceBeforeSearchIndex1;
	double dVelJoggingIndex1;

	double dMoveDistanceBeforeSearchIndex2;
	double dVelJoggingIndex2;

	double dPosnErrThresHoldSettling;
	int iDebug;

	int iFlagVerifyByRepeatingIndex2;

}MTN_API_SEARCH_HOME_FROM_LIMIT_INPUT;

typedef struct{
	double dIndexPositionFineSearch_1;
	double dIndexPositionFineSearch_2;
	int iErrorCode;
}MTN_API_SEARCH_HOME_FROM_LIMIT_OUTPUT;

#define FLAG_VERIFY_INDEX_BY_REPEATING_SEARCH  1

void mtn_init_def_para_search_index_vled_bonder_xyz(int iAxisX, int iAxisY, int iAxisZ);
int mtn_bonder_xyz_start_search_limit_go_home(HANDLE Handle, int iAxisX, int iAxisY, int iAxisZ);

void mtn_init_home_set_x_table_offset_startbond(double dOffsetTableX);
double mtn_init_home_get_x_table_offset_startbond();
void mtn_init_home_set_z_bondhead_firelevel(double dBondHeadFireLevel);
double mtn_init_home_get_z_bondhead_firelevel();

void mtn_init_home_set_y_table_offset_startbond(double dOffsetTableY);
double mtn_init_home_get_y_table_offset_startbond();

int mtn_api_search_home_acs(HANDLE mHandle, MTN_API_SEARCH_INDEX_INPUT *stpSearchHomeInputAPI, MTN_API_SEARCH_INDEX_OUTPUT *stpSearcihHomeOutputAPI);
int mtn_api_search_limit_acs(HANDLE mHandle, MTN_API_SEARCH_LIMIT_INPUT *stpSearchLimitInput, MTN_API_SEARCH_LIMIT_OUTPUT *stpSearchLimitOutput);
int mtn_api_search_home_from_limit_acs(HANDLE mHandle, MTN_API_SEARCH_HOME_FROM_LIMIT_INPUT *stpSearchHomeFromLimitInput, MTN_API_SEARCH_HOME_FROM_LIMIT_OUTPUT *stpSearchHomeFromLimitOutput);
int mtn_search_home_acs(HANDLE mHandle, MTN_SEARCH_INDEX_INPUT *stpSearchHomeInput, MTN_SEARCH_INDEX_OUTPUT *stpSearchHomeOutput);

#endif // __MTN_API_SEARCH_HOME__