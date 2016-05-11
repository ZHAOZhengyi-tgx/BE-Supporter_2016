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



#ifndef  __MTN_AFT_H__
#define  __MTN_AFT_H__

#define MAX_BYTE_NAME_STRING      128
typedef struct
{
	double dDistLimitToLimit_mm;
	double dDistUppLimitToIndex_mm;
	double dDistLowLimitToIndex_mm;
	double dDistIndexUppLow_mm;
	int iDriverPolarity;
	
	double dExpectMaxAccPosDir_m_s;
	double dExpMaxAccMeanPosDir;
	double dExpMaxAccStdPosDir;

	double dExpectMaxAccNegDir_m_s;
	double dExpMaxAccMeanNegDir;
	double dExpMaxAccStdNegDir;

}AFT_OUTPUT_AXIS;

typedef struct
{
	double dDistLimitToLimit_cnt;
	double dDistUppLimitToIndex_cnt;
	double dDistLowLimitToIndex_cnt;
	double dDistIndexUppLow_cnt;
}AFT_CHECKING_AXIS;

// 0: pass
// 1: fail, > upp limit
// -1: fail, < low limit
typedef struct
{
	int iFlagFailLimitToLimit;
	int iFlagFailUppLimitToIndex;
	int iFlagFailLowLimitToIndex;
	int iFlagFailIndexUppLow;

	int iFlagExpMaxAccNegDir;
	int iFlagExpMaxAccPosDir;
}AFT_RESULT_AXIS;

typedef struct
{
	char strAxisName_cn[MAX_BYTE_NAME_STRING];
	char strAxisName_en[MAX_BYTE_NAME_STRING];
	double dDistLimitToLimit_mm_U;
	double dDistLimitToLimit_mm_L;
	double dDistUppLimitToIndex_mm_U;
	double dDistUppLimitToIndex_mm_L;
	double dDistLowLimitToIndex_mm_U;
	double dDistLowLimitToIndex_mm_L;
	double dDistIndexUppLow_mm_U;
	double dDistIndexUppLow_mm_L;

	double dExpectMaxAccPosDir_m_s_U;
	double dExpectMaxAccPosDir_m_s_L;

	double dExpectMaxAccNegDir_m_s_U;
	double dExpectMaxAccNegDir_m_s_L;

}AFT_SPEC_AXIS;

extern void aft_init_spec();
void aft_axis_calc_output_from_checking(int iAxisWb, AFT_CHECKING_AXIS *stpAFT_AxisChecking);
void aft_axis_verify_output_by_spec(int iAxisWb);

void mtn_aft_bakup_speed_profile(HANDLE hHandleACS);
void mtn_aft_restore_speed_profile(HANDLE hHandleACS);
void mtn_aft_init_random_burn_in_condition(HANDLE hHandleACS);

#include "AftVelLoopTest.h"
void aft_ana_vel_step_group_test(AFT_VEL_LOOP_TEST_INPUT *stpVelLoopTestInput, AFT_VEL_LOOP_TEST_OUTPUT *stpVelLoopTestOutput, unsigned int nTotalCase);

#endif   // __MTN_AFT_H__