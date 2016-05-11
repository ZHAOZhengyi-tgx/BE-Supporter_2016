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
#include "math.h"

#include "MtnAft.h"
#include "MtnTune.h"
#include "MtnWbDef.h"


char *strAFT_Result_cn[] =
{
	"不通过！！！小于下限",
	"通过：）",
	"大于上限"
};

char *strAFT_Result_en[] =
{
	"Fail: < Low",
	"Pass",
	"Fail: > Upp"
};

AFT_OUTPUT_AXIS  astAssemblyFuncTestAxisOut[MAX_SERVO_AXIS_WIREBOND];
AFT_SPEC_AXIS    astAssemblyFuncTestAxisSpec[MAX_SERVO_AXIS_WIREBOND];
AFT_RESULT_AXIS  astAssemblyFuncTestAxisResult[MAX_SERVO_AXIS_WIREBOND];
extern AXIS_INFO_WIRE_BOND astAxisInfoWireBond[MAX_SERVO_AXIS_WIREBOND];

static MTN_SPEED_PROFILE stBakSpeedProfileACS_X;
static MTN_SPEED_PROFILE stBakSpeedProfileACS_Y;
static MTN_SPEED_PROFILE stBakSpeedProfileACS_A;

void mtn_aft_bakup_speed_profile(HANDLE hHandleACS)
{
	mtnapi_get_speed_profile(hHandleACS, ACS_CARD_AXIS_X, &stBakSpeedProfileACS_X, 0);
	mtnapi_get_speed_profile(hHandleACS, ACS_CARD_AXIS_Y, &stBakSpeedProfileACS_Y, 0);
	mtnapi_get_speed_profile(hHandleACS, ACS_CARD_AXIS_A, &stBakSpeedProfileACS_A, 0);
}

void mtn_aft_init_random_burn_in_condition(HANDLE hHandleACS)
{
	// Backup
	mtn_aft_bakup_speed_profile(hHandleACS);

	// According to the machine type
	MTN_SPEED_PROFILE stSpeedProfileACS_X;
	MTN_SPEED_PROFILE stSpeedProfileACS_Y;
	MTN_SPEED_PROFILE stSpeedProfileACS_A;

	stSpeedProfileACS_X.dMaxAcceleration = 40.0E6;
	stSpeedProfileACS_X.dMaxDeceleration = 40.0E6;
	stSpeedProfileACS_X.dMaxJerk = 12000.0E6;
	stSpeedProfileACS_X.dMaxVelocity = 195000.0;
	stSpeedProfileACS_X.dMaxKillDeceleration = 10.0E6;
	mtnapi_set_speed_profile(hHandleACS, ACS_CARD_AXIS_X, &stSpeedProfileACS_X, 0);

	stSpeedProfileACS_Y.dMaxAcceleration = 20.0E6;
	stSpeedProfileACS_Y.dMaxDeceleration = 20.0E6;
	stSpeedProfileACS_Y.dMaxJerk = 12000.0E6;
	stSpeedProfileACS_Y.dMaxVelocity = 195000.0;
	stSpeedProfileACS_Y.dMaxKillDeceleration = 10.0E6;
	mtnapi_set_speed_profile(hHandleACS, ACS_CARD_AXIS_Y, &stSpeedProfileACS_Y, 0);

	stSpeedProfileACS_A.dMaxAcceleration = 200E6;
	stSpeedProfileACS_A.dMaxDeceleration = 200E6;
	stSpeedProfileACS_A.dMaxJerk = 60000E6;
	stSpeedProfileACS_A.dMaxVelocity = 195000.0;
	stSpeedProfileACS_A.dMaxKillDeceleration = 10.0E6;
	mtnapi_set_speed_profile(hHandleACS, ACS_CARD_AXIS_A, &stSpeedProfileACS_A, 0);

}

void mtn_aft_restore_speed_profile(HANDLE hHandleACS)
{
	mtnapi_set_speed_profile(hHandleACS, ACS_CARD_AXIS_X, &stBakSpeedProfileACS_X, 0);
	mtnapi_set_speed_profile(hHandleACS, ACS_CARD_AXIS_Y, &stBakSpeedProfileACS_Y, 0);
	mtnapi_set_speed_profile(hHandleACS, ACS_CARD_AXIS_A, &stBakSpeedProfileACS_A, 0);
}


CString aft_gen_report_string()
{
	CString cstrTemp;
	int iAxisCurr;

	cstrTemp.Format("%%%%%%%% BE WireBonder AFT (Assembly Functional Test) Report \r\n");
	cstrTemp.AppendFormat("%%%%%%%% Section-1: Travel Range and Index Distance \r\n");

	for(iAxisCurr = WB_AXIS_TABLE_X; iAxisCurr <= WB_AXIS_BOND_Z; iAxisCurr ++)
	{
		cstrTemp.AppendFormat("%% ------ Axis: %s \r\n", astAssemblyFuncTestAxisSpec[iAxisCurr].strAxisName_en);
		if(astAssemblyFuncTestAxisOut[iAxisCurr].dDistLimitToLimit_mm == 0 ||
			astAssemblyFuncTestAxisOut[iAxisCurr].dDistLowLimitToIndex_mm == 0 ||
			astAssemblyFuncTestAxisOut[iAxisCurr].dDistUppLimitToIndex_mm == 0 ||
			astAssemblyFuncTestAxisOut[iAxisCurr].dDistIndexUppLow_mm == 0)
		{
			cstrTemp.AppendFormat("NOT YET to do AFT\r\n\r\n");
		}
		else
		{
			cstrTemp.AppendFormat("DistLimitToLimit_mm = %8.2f; %% [%8.2f, %8.2f] --- %s %s\r\n", 
				astAssemblyFuncTestAxisOut[iAxisCurr].dDistLimitToLimit_mm,
				astAssemblyFuncTestAxisSpec[iAxisCurr].dDistLimitToLimit_mm_L, astAssemblyFuncTestAxisSpec[iAxisCurr].dDistLimitToLimit_mm_U,
				strAFT_Result_en[astAssemblyFuncTestAxisResult[iAxisCurr].iFlagFailLimitToLimit + 1],
				strAFT_Result_cn[astAssemblyFuncTestAxisResult[iAxisCurr].iFlagFailLimitToLimit + 1]);

			cstrTemp.AppendFormat("DistLowLimitToIndex_mm = -%4.2f; %% [%8.2f, %8.2f] --- %s %s\r\n", 
				astAssemblyFuncTestAxisOut[iAxisCurr].dDistLowLimitToIndex_mm,
				astAssemblyFuncTestAxisSpec[iAxisCurr].dDistLowLimitToIndex_mm_L, astAssemblyFuncTestAxisSpec[iAxisCurr].dDistLowLimitToIndex_mm_U,
				strAFT_Result_en[astAssemblyFuncTestAxisResult[iAxisCurr].iFlagFailLowLimitToIndex + 1],
				strAFT_Result_cn[astAssemblyFuncTestAxisResult[iAxisCurr].iFlagFailLowLimitToIndex + 1]);

			cstrTemp.AppendFormat("DistUppLimitToIndex_mm = %8.2f; %% [%8.2f, %8.2f] --- %s %s \r\n", 
				astAssemblyFuncTestAxisOut[iAxisCurr].dDistUppLimitToIndex_mm,
				astAssemblyFuncTestAxisSpec[iAxisCurr].dDistUppLimitToIndex_mm_L, astAssemblyFuncTestAxisSpec[iAxisCurr].dDistUppLimitToIndex_mm_U,
				strAFT_Result_en[astAssemblyFuncTestAxisResult[iAxisCurr].iFlagFailUppLimitToIndex + 1],
				strAFT_Result_cn[astAssemblyFuncTestAxisResult[iAxisCurr].iFlagFailUppLimitToIndex + 1]);

			cstrTemp.AppendFormat("DistIndexUppLow_mm = %8.4f; %% [%8.4f, %8.4f] --- %s %s\r\n\r\n", 
				astAssemblyFuncTestAxisOut[iAxisCurr].dDistIndexUppLow_mm,
				astAssemblyFuncTestAxisSpec[iAxisCurr].dDistIndexUppLow_mm_L, astAssemblyFuncTestAxisSpec[iAxisCurr].dDistIndexUppLow_mm_U,
				strAFT_Result_en[astAssemblyFuncTestAxisResult[iAxisCurr].iFlagFailIndexUppLow + 1],
				strAFT_Result_cn[astAssemblyFuncTestAxisResult[iAxisCurr].iFlagFailIndexUppLow + 1]);

		}
	}
	cstrTemp.AppendFormat("%%%%%%%% Section-2: Maximum Expected Acc (m/s/s) \r\n");
	for(iAxisCurr = WB_AXIS_TABLE_X; iAxisCurr <= WB_AXIS_BOND_Z; iAxisCurr ++)
	{
		cstrTemp.AppendFormat("%% ------ Axis: %s \r\n", astAssemblyFuncTestAxisSpec[iAxisCurr].strAxisName_en);
		if(astAssemblyFuncTestAxisOut[iAxisCurr].dExpectMaxAccNegDir_m_s == 0 ||
			astAssemblyFuncTestAxisOut[iAxisCurr].dExpectMaxAccPosDir_m_s == 0)
		{
			cstrTemp.AppendFormat("NOT YET to do AFT\r\n\r\n");
		}
		else
		{
			cstrTemp.AppendFormat("ExpectMaxAccPosDir_m_s = %8.1f; %% [%8.1f, %8.1f] --- %s %s\r\n", 
				astAssemblyFuncTestAxisOut[iAxisCurr].dExpectMaxAccPosDir_m_s,
				astAssemblyFuncTestAxisSpec[iAxisCurr].dExpectMaxAccPosDir_m_s_L, astAssemblyFuncTestAxisSpec[iAxisCurr].dExpectMaxAccPosDir_m_s_U,
				strAFT_Result_en[astAssemblyFuncTestAxisResult[iAxisCurr].iFlagExpMaxAccPosDir + 1],
				strAFT_Result_cn[astAssemblyFuncTestAxisResult[iAxisCurr].iFlagExpMaxAccPosDir + 1]);
			cstrTemp.AppendFormat("ExpectMaxAccNegDir_m_s = %8.1f; %% [%8.1f, %8.1f] --- %s %s\r\n", 
				astAssemblyFuncTestAxisOut[iAxisCurr].dExpectMaxAccNegDir_m_s,
				astAssemblyFuncTestAxisSpec[iAxisCurr].dExpectMaxAccNegDir_m_s_L, astAssemblyFuncTestAxisSpec[iAxisCurr].dExpectMaxAccNegDir_m_s_U,
				strAFT_Result_en[astAssemblyFuncTestAxisResult[iAxisCurr].iFlagExpMaxAccNegDir + 1],
				strAFT_Result_cn[astAssemblyFuncTestAxisResult[iAxisCurr].iFlagExpMaxAccNegDir + 1]);

			cstrTemp.AppendFormat("ExpectMaxAccPosDir_mean_std = [%8.1f, %8.1f]; \r\n", 
				astAssemblyFuncTestAxisOut[iAxisCurr].dExpMaxAccMeanPosDir, astAssemblyFuncTestAxisOut[iAxisCurr].dExpMaxAccStdPosDir);
			cstrTemp.AppendFormat("ExpectMaxAccNegDir_mean_std = [%8.1f, %8.1f]; \r\n\r\n", 
				astAssemblyFuncTestAxisOut[iAxisCurr].dExpMaxAccMeanNegDir, astAssemblyFuncTestAxisOut[iAxisCurr].dExpMaxAccStdNegDir);
		}
	}
	return cstrTemp;
}

void aft_axis_calc_output_from_checking(int iAxisWb, AFT_CHECKING_AXIS *stpAFT_AxisChecking)
{
	astAssemblyFuncTestAxisOut[iAxisWb].dDistLimitToLimit_mm = 
		stpAFT_AxisChecking->dDistLimitToLimit_cnt/astAxisInfoWireBond[iAxisWb].afEncoderResolution_cnt_p_mm;
	astAssemblyFuncTestAxisOut[iAxisWb].dDistLowLimitToIndex_mm = 
		stpAFT_AxisChecking->dDistLowLimitToIndex_cnt/astAxisInfoWireBond[iAxisWb].afEncoderResolution_cnt_p_mm;
	astAssemblyFuncTestAxisOut[iAxisWb].dDistUppLimitToIndex_mm = 
		stpAFT_AxisChecking->dDistUppLimitToIndex_cnt/astAxisInfoWireBond[iAxisWb].afEncoderResolution_cnt_p_mm;
	astAssemblyFuncTestAxisOut[iAxisWb].dDistIndexUppLow_mm = 
		stpAFT_AxisChecking->dDistIndexUppLow_cnt/astAxisInfoWireBond[iAxisWb].afEncoderResolution_cnt_p_mm;
}

static unsigned int anTotalCaseVelStepTestPerAxis[MAX_SERVO_AXIS_WIREBOND];
static double madExpectMaxAccPosDir[MAX_SERVO_AXIS_WIREBOND][MAX_NUM_CASE_VEL_STEP_GROUP_TEST];
static double madExpectMaxAccNegDir[MAX_SERVO_AXIS_WIREBOND][MAX_NUM_CASE_VEL_STEP_GROUP_TEST];
extern double f_get_mean(double *afIn, int nLen);
extern double f_get_std(double *afIn, double fMean, int nLen);

void aft_ana_vel_step_group_test(AFT_VEL_LOOP_TEST_INPUT *stpVelLoopTestInput, AFT_VEL_LOOP_TEST_OUTPUT *stpVelLoopTestOutput, unsigned int nTotalCase)
{
	unsigned int ii;
	for(ii=0; ii<MAX_SERVO_AXIS_WIREBOND; ii++)
	{
		anTotalCaseVelStepTestPerAxis[ii] = 0;
	}

	int iAxisCtrlCardCurrCase, iAxisWbCurrCase;
	for(ii = 0; ii < nTotalCase; ii++)
	{
		iAxisCtrlCardCurrCase = stpVelLoopTestInput[ii].iAxisCtrlCard;
		switch(iAxisCtrlCardCurrCase)
		{
		case _EFSIKA_TABLE_X_AXIS:
			iAxisWbCurrCase = WB_AXIS_TABLE_X;
			break;
		case _EFSIKA_TABLE_Y_AXIS:
			iAxisWbCurrCase = WB_AXIS_TABLE_Y;
			break;
		case _EFSIKA_BOND_Z_AXIS:
			iAxisWbCurrCase = WB_AXIS_BOND_Z;
			break;
		}
		madExpectMaxAccPosDir[iAxisWbCurrCase][anTotalCaseVelStepTestPerAxis[iAxisWbCurrCase]] = stpVelLoopTestOutput[ii].fMaxAccEstimateFullDAC_mpss;
		madExpectMaxAccNegDir[iAxisWbCurrCase][anTotalCaseVelStepTestPerAxis[iAxisWbCurrCase]] = stpVelLoopTestOutput[ii].fMinAccEstimateFullDAC_mpss;

		if(anTotalCaseVelStepTestPerAxis[iAxisWbCurrCase] == 0)
		{
			astAssemblyFuncTestAxisOut[iAxisWbCurrCase].dExpectMaxAccPosDir_m_s = stpVelLoopTestOutput[ii].fMaxAccEstimateFullDAC_mpss;
			astAssemblyFuncTestAxisOut[iAxisWbCurrCase].dExpectMaxAccNegDir_m_s = stpVelLoopTestOutput[ii].fMinAccEstimateFullDAC_mpss;
		}
		else
		{
			if(astAssemblyFuncTestAxisOut[iAxisWbCurrCase].dExpectMaxAccPosDir_m_s > madExpectMaxAccPosDir[iAxisWbCurrCase][anTotalCaseVelStepTestPerAxis[iAxisWbCurrCase]])
			{
				astAssemblyFuncTestAxisOut[iAxisWbCurrCase].dExpectMaxAccPosDir_m_s = madExpectMaxAccPosDir[iAxisWbCurrCase][anTotalCaseVelStepTestPerAxis[iAxisWbCurrCase]];
			}
			if(astAssemblyFuncTestAxisOut[iAxisWbCurrCase].dExpectMaxAccNegDir_m_s > madExpectMaxAccNegDir[iAxisWbCurrCase][anTotalCaseVelStepTestPerAxis[iAxisWbCurrCase]])
			{
				astAssemblyFuncTestAxisOut[iAxisWbCurrCase].dExpectMaxAccNegDir_m_s = madExpectMaxAccNegDir[iAxisWbCurrCase][anTotalCaseVelStepTestPerAxis[iAxisWbCurrCase]];
			}
		}
		anTotalCaseVelStepTestPerAxis[iAxisWbCurrCase] ++;
	}

	for(iAxisWbCurrCase = WB_AXIS_TABLE_X; iAxisWbCurrCase <= WB_AXIS_BOND_Z; iAxisWbCurrCase ++)
	{
		astAssemblyFuncTestAxisOut[iAxisWbCurrCase].dExpMaxAccMeanPosDir = f_get_mean(&madExpectMaxAccPosDir[iAxisWbCurrCase][0], anTotalCaseVelStepTestPerAxis[iAxisWbCurrCase]);
		astAssemblyFuncTestAxisOut[iAxisWbCurrCase].dExpMaxAccStdPosDir = f_get_std(&madExpectMaxAccPosDir[iAxisWbCurrCase][0], astAssemblyFuncTestAxisOut[iAxisWbCurrCase].dExpMaxAccMeanPosDir, anTotalCaseVelStepTestPerAxis[iAxisWbCurrCase]);

		astAssemblyFuncTestAxisOut[iAxisWbCurrCase].dExpMaxAccMeanNegDir = f_get_mean(&madExpectMaxAccNegDir[iAxisWbCurrCase][0], anTotalCaseVelStepTestPerAxis[iAxisWbCurrCase]);
		astAssemblyFuncTestAxisOut[iAxisWbCurrCase].dExpMaxAccStdNegDir = f_get_std(&madExpectMaxAccNegDir[iAxisWbCurrCase][0], astAssemblyFuncTestAxisOut[iAxisWbCurrCase].dExpMaxAccMeanNegDir, anTotalCaseVelStepTestPerAxis[iAxisWbCurrCase]);

	}

}


void aft_axis_verify_output_by_spec(int iAxisWb)
{
	if(astAssemblyFuncTestAxisOut[iAxisWb].dDistLimitToLimit_mm > astAssemblyFuncTestAxisSpec[iAxisWb].dDistLimitToLimit_mm_U)
	{
		astAssemblyFuncTestAxisResult[iAxisWb].iFlagFailLimitToLimit = 1;
	}
	else if(astAssemblyFuncTestAxisOut[iAxisWb].dDistLimitToLimit_mm < astAssemblyFuncTestAxisSpec[iAxisWb].dDistLimitToLimit_mm_L)
	{
		astAssemblyFuncTestAxisResult[iAxisWb].iFlagFailLimitToLimit = -1;
	}
	else
	{
		astAssemblyFuncTestAxisResult[iAxisWb].iFlagFailLimitToLimit = 0;
	}

	if(astAssemblyFuncTestAxisOut[iAxisWb].dDistIndexUppLow_mm > astAssemblyFuncTestAxisSpec[iAxisWb].dDistIndexUppLow_mm_U)
	{
		astAssemblyFuncTestAxisResult[iAxisWb].iFlagFailIndexUppLow = 1;
	}
	else if(astAssemblyFuncTestAxisOut[iAxisWb].dDistIndexUppLow_mm < astAssemblyFuncTestAxisSpec[iAxisWb].dDistIndexUppLow_mm_L)
	{
		astAssemblyFuncTestAxisResult[iAxisWb].iFlagFailIndexUppLow = -1;
	}
	else
	{
		astAssemblyFuncTestAxisResult[iAxisWb].iFlagFailIndexUppLow = 0;
	}

	if(-astAssemblyFuncTestAxisOut[iAxisWb].dDistLowLimitToIndex_mm > astAssemblyFuncTestAxisSpec[iAxisWb].dDistLowLimitToIndex_mm_U)
	{
		astAssemblyFuncTestAxisResult[iAxisWb].iFlagFailLowLimitToIndex = 1;
	}
	else if(-astAssemblyFuncTestAxisOut[iAxisWb].dDistLowLimitToIndex_mm < astAssemblyFuncTestAxisSpec[iAxisWb].dDistLowLimitToIndex_mm_L)
	{
		astAssemblyFuncTestAxisResult[iAxisWb].iFlagFailLowLimitToIndex = -1;
	}
	else
	{
		astAssemblyFuncTestAxisResult[iAxisWb].iFlagFailLowLimitToIndex = 0;
	}

	if(astAssemblyFuncTestAxisOut[iAxisWb].dDistUppLimitToIndex_mm > astAssemblyFuncTestAxisSpec[iAxisWb].dDistUppLimitToIndex_mm_U)
	{
		astAssemblyFuncTestAxisResult[iAxisWb].iFlagFailUppLimitToIndex = 1;
	}
	else if(astAssemblyFuncTestAxisOut[iAxisWb].dDistUppLimitToIndex_mm < astAssemblyFuncTestAxisSpec[iAxisWb].dDistUppLimitToIndex_mm_L)
	{
		astAssemblyFuncTestAxisResult[iAxisWb].iFlagFailUppLimitToIndex = -1;
	}
	else
	{
		astAssemblyFuncTestAxisResult[iAxisWb].iFlagFailUppLimitToIndex = 0;
	}
	//// Expected Max Acc
	if(astAssemblyFuncTestAxisOut[iAxisWb].dExpectMaxAccNegDir_m_s > astAssemblyFuncTestAxisSpec[iAxisWb].dExpectMaxAccNegDir_m_s_U)
	{
		astAssemblyFuncTestAxisResult[iAxisWb].iFlagExpMaxAccNegDir = 1;
	}
	else if(astAssemblyFuncTestAxisOut[iAxisWb].dExpectMaxAccNegDir_m_s < astAssemblyFuncTestAxisSpec[iAxisWb].dExpectMaxAccNegDir_m_s_L)
	{
		astAssemblyFuncTestAxisResult[iAxisWb].iFlagExpMaxAccNegDir = -1;
	}
	else
	{
		astAssemblyFuncTestAxisResult[iAxisWb].iFlagExpMaxAccNegDir = 0;
	}

	if(astAssemblyFuncTestAxisOut[iAxisWb].dExpectMaxAccPosDir_m_s > astAssemblyFuncTestAxisSpec[iAxisWb].dExpectMaxAccPosDir_m_s_U)
	{
		astAssemblyFuncTestAxisResult[iAxisWb].iFlagExpMaxAccPosDir = 1;
	}
	else if(astAssemblyFuncTestAxisOut[iAxisWb].dExpectMaxAccPosDir_m_s < astAssemblyFuncTestAxisSpec[iAxisWb].dExpectMaxAccPosDir_m_s_L)
	{
		astAssemblyFuncTestAxisResult[iAxisWb].iFlagExpMaxAccPosDir = -1;
	}
	else
	{
		astAssemblyFuncTestAxisResult[iAxisWb].iFlagExpMaxAccPosDir = 0;
	}

}

#include "MotAlgo_DLL.h"
void aft_init_spec_wb13v()
{
	int ii;
	for(ii=0; ii<MAX_SERVO_AXIS_WIREBOND; ii++)
	{
		sprintf_s(astAssemblyFuncTestAxisSpec[ii].strAxisName_cn, MAX_BYTE_NAME_STRING, 
			"%s", astAxisInfoWireBond[ii].strAxisNameCn);
		sprintf_s(astAssemblyFuncTestAxisSpec[ii].strAxisName_en, MAX_BYTE_NAME_STRING,
			"%s", astAxisInfoWireBond[ii].strAxisNameEn);
	}
	// X-Table
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dDistLimitToLimit_mm_L = 96;
//		LIMIT_TO_LIMIT_TABLE_X / astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm - TRAVEL_DIST_RANGE_ERR_TOL_MM;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dDistLimitToLimit_mm_U = 108;
//		LIMIT_TO_LIMIT_TABLE_X / astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm + TRAVEL_DIST_RANGE_ERR_UPP_TOL_MM;

	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dDistLowLimitToIndex_mm_L = -19;
//		fabs((double)TRAVEL_LOW_LIMIT_TABLE_X) / astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm - TRAVEL_DIST_RANGE_ERR_TOL_MM;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dDistLowLimitToIndex_mm_U = -6;
//		fabs((double)TRAVEL_LOW_LIMIT_TABLE_X) / astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm + TRAVEL_DIST_RANGE_ERR_UPP_TOL_MM;

	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dDistUppLimitToIndex_mm_L = 85;
//		fabs((double)TRAVEL_UPP_LIMIT_TABLE_X) / astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm - TRAVEL_DIST_RANGE_ERR_TOL_MM;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dDistUppLimitToIndex_mm_U = 88;
//		fabs((double)TRAVEL_UPP_LIMIT_TABLE_X) / astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm + TRAVEL_DIST_RANGE_ERR_UPP_TOL_MM;

	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dDistIndexUppLow_mm_L = 0.01;
//		fabs((double)(DIST_CNT_INDEX_EDGES - TOL_CNT_INDEX_EDGES)) / astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dDistIndexUppLow_mm_U = 0.02;
//		fabs((double)(DIST_CNT_INDEX_EDGES + TOL_CNT_INDEX_EDGES)) / astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dExpectMaxAccPosDir_m_s_L = MAX_EXPECT_ACC_TABLE_X_L;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dExpectMaxAccPosDir_m_s_U = MAX_EXPECT_ACC_TABLE_X_U;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dExpectMaxAccNegDir_m_s_L = MAX_EXPECT_ACC_TABLE_X_L;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dExpectMaxAccNegDir_m_s_U = MAX_EXPECT_ACC_TABLE_X_U;

	// Y-Table
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dDistLimitToLimit_mm_L = 50;
//		LIMIT_TO_LIMIT_TABLE_Y / astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm - TRAVEL_DIST_RANGE_ERR_TOL_MM;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dDistLimitToLimit_mm_U = 56;
//		LIMIT_TO_LIMIT_TABLE_Y / astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm + TRAVEL_DIST_RANGE_ERR_UPP_TOL_MM;

	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dDistLowLimitToIndex_mm_L = -21;
//		fabs((double)TRAVEL_LOW_LIMIT_TABLE_Y) / astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm - TRAVEL_DIST_RANGE_ERR_TOL_MM;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dDistLowLimitToIndex_mm_U = -16;
//		fabs((double)TRAVEL_LOW_LIMIT_TABLE_Y) / astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm + TRAVEL_DIST_RANGE_ERR_UPP_TOL_MM;

	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dDistUppLimitToIndex_mm_L = 33;
//		fabs((double)TRAVEL_UPP_LIMIT_TABLE_Y) / astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm - TRAVEL_DIST_RANGE_ERR_TOL_MM;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dDistUppLimitToIndex_mm_U = 36;
//		fabs((double)TRAVEL_UPP_LIMIT_TABLE_Y) / astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm + TRAVEL_DIST_RANGE_ERR_UPP_TOL_MM;

	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dDistIndexUppLow_mm_L = 0.01;
//		fabs((double)(DIST_CNT_INDEX_EDGES - TOL_CNT_INDEX_EDGES)) / astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dDistIndexUppLow_mm_U = 0.02;
//		fabs((double)(DIST_CNT_INDEX_EDGES + TOL_CNT_INDEX_EDGES)) / astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dExpectMaxAccPosDir_m_s_L = MAX_EXPECT_ACC_TABLE_Y_L;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dExpectMaxAccPosDir_m_s_U = MAX_EXPECT_ACC_TABLE_Y_U;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dExpectMaxAccNegDir_m_s_L = MAX_EXPECT_ACC_TABLE_Y_L;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dExpectMaxAccNegDir_m_s_U = MAX_EXPECT_ACC_TABLE_Y_U;

	// Bond-Z
	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dDistLimitToLimit_mm_L = 10;
//		LIMIT_TO_LIMIT_BOND_HEAD_Z / astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm - TRAVEL_DIST_RANGE_ERR_TOL_MM;
	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dDistLimitToLimit_mm_U = 14;
//		LIMIT_TO_LIMIT_BOND_HEAD_Z / astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm + TRAVEL_DIST_RANGE_ERR_UPP_TOL_MM;

	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dDistLowLimitToIndex_mm_L = -12;
//		fabs((double)TRAVEL_LOW_LIMIT_BOND_HEAD_Z) / astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm - TRAVEL_DIST_RANGE_ERR_TOL_MM;
	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dDistLowLimitToIndex_mm_U = -6;
//		fabs((double)TRAVEL_LOW_LIMIT_BOND_HEAD_Z) / astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm + TRAVEL_DIST_RANGE_ERR_UPP_TOL_MM;

	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dDistUppLimitToIndex_mm_L = 0;
//		fabs((double)TRAVEL_UPP_LIMIT_BOND_HEAD_Z) / astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm - TRAVEL_DIST_RANGE_ERR_TOL_MM;
	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dDistUppLimitToIndex_mm_U = 4;
//		fabs((double)TRAVEL_UPP_LIMIT_BOND_HEAD_Z) / astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm + TRAVEL_DIST_RANGE_ERR_UPP_TOL_MM;

	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dDistIndexUppLow_mm_L = 0.026;
//		fabs((double)(DIST_CNT_INDEX_EDGES - TOL_CNT_INDEX_EDGES)) / astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm;
	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dDistIndexUppLow_mm_U = 0.042;
//		fabs((double)(DIST_CNT_INDEX_EDGES + TOL_CNT_INDEX_EDGES)) / astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm;

	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dExpectMaxAccPosDir_m_s_L = MAX_EXPECT_ACC_BOND_HEAD_Z_L;
	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dExpectMaxAccPosDir_m_s_U = MAX_EXPECT_ACC_BOND_HEAD_Z_U;
	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dExpectMaxAccNegDir_m_s_L = MAX_EXPECT_ACC_BOND_HEAD_Z_L;
	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dExpectMaxAccNegDir_m_s_U = MAX_EXPECT_ACC_BOND_HEAD_Z_U;

}

void aft_init_spec_wb13t_hori_led()
{
	int ii;
	for(ii=0; ii<MAX_SERVO_AXIS_WIREBOND; ii++)
	{
		sprintf_s(astAssemblyFuncTestAxisSpec[ii].strAxisName_cn, MAX_BYTE_NAME_STRING, 
			"%s", astAxisInfoWireBond[ii].strAxisNameCn);
		sprintf_s(astAssemblyFuncTestAxisSpec[ii].strAxisName_en, MAX_BYTE_NAME_STRING,
			"%s", astAxisInfoWireBond[ii].strAxisNameEn);
	}
	// X-Table
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dDistLimitToLimit_mm_L = 57;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dDistLimitToLimit_mm_U = 60;

	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dDistLowLimitToIndex_mm_L = -37;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dDistLowLimitToIndex_mm_U = -35;

	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dDistUppLimitToIndex_mm_L = 21;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dDistUppLimitToIndex_mm_U = 23;

	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dDistIndexUppLow_mm_L = 0.01;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dDistIndexUppLow_mm_U = 0.02;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dExpectMaxAccPosDir_m_s_L = MAX_EXPECT_ACC_WB13T_TABLE_X_L;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dExpectMaxAccPosDir_m_s_U = MAX_EXPECT_ACC_WB13T_TABLE_X_U;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dExpectMaxAccNegDir_m_s_L = MAX_EXPECT_ACC_WB13T_TABLE_X_L;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_X].dExpectMaxAccNegDir_m_s_U = MAX_EXPECT_ACC_WB13T_TABLE_X_U;

	// Y-Table
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dDistLimitToLimit_mm_L = 60;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dDistLimitToLimit_mm_U = 65;

	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dDistLowLimitToIndex_mm_L = -33;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dDistLowLimitToIndex_mm_U = -29;

	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dDistUppLimitToIndex_mm_L = 30;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dDistUppLimitToIndex_mm_U = 32;

	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dDistIndexUppLow_mm_L = 0.01;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dDistIndexUppLow_mm_U = 0.02;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dExpectMaxAccPosDir_m_s_L = MAX_EXPECT_ACC_WB13T_TABLE_Y_L;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dExpectMaxAccPosDir_m_s_U = MAX_EXPECT_ACC_WB13T_TABLE_Y_U;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dExpectMaxAccNegDir_m_s_L = MAX_EXPECT_ACC_WB13T_TABLE_Y_L;
	astAssemblyFuncTestAxisSpec[WB_AXIS_TABLE_Y].dExpectMaxAccNegDir_m_s_U = MAX_EXPECT_ACC_WB13T_TABLE_Y_U;

	// Bond-Z
	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dDistLimitToLimit_mm_L = 10;
//		LIMIT_TO_LIMIT_BOND_HEAD_Z / astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm - TRAVEL_DIST_RANGE_ERR_TOL_MM;
	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dDistLimitToLimit_mm_U = 14;
//		LIMIT_TO_LIMIT_BOND_HEAD_Z / astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm + TRAVEL_DIST_RANGE_ERR_UPP_TOL_MM;

	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dDistLowLimitToIndex_mm_L = -12;
//		fabs((double)TRAVEL_LOW_LIMIT_BOND_HEAD_Z) / astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm - TRAVEL_DIST_RANGE_ERR_TOL_MM;
	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dDistLowLimitToIndex_mm_U = -6;
//		fabs((double)TRAVEL_LOW_LIMIT_BOND_HEAD_Z) / astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm + TRAVEL_DIST_RANGE_ERR_UPP_TOL_MM;

	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dDistUppLimitToIndex_mm_L = 0;
//		fabs((double)TRAVEL_UPP_LIMIT_BOND_HEAD_Z) / astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm - TRAVEL_DIST_RANGE_ERR_TOL_MM;
	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dDistUppLimitToIndex_mm_U = 4;
//		fabs((double)TRAVEL_UPP_LIMIT_BOND_HEAD_Z) / astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm + TRAVEL_DIST_RANGE_ERR_UPP_TOL_MM;

	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dDistIndexUppLow_mm_L = 0.026;
//		fabs((double)(DIST_CNT_INDEX_EDGES - TOL_CNT_INDEX_EDGES)) / astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm;
	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dDistIndexUppLow_mm_U = 0.042;
//		fabs((double)(DIST_CNT_INDEX_EDGES + TOL_CNT_INDEX_EDGES)) / astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm;

	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dExpectMaxAccPosDir_m_s_L = MAX_EXPECT_ACC_WB13T_BOND_HEAD_Z_L;
	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dExpectMaxAccPosDir_m_s_U = MAX_EXPECT_ACC_WB13T_BOND_HEAD_Z_U;
	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dExpectMaxAccNegDir_m_s_L = MAX_EXPECT_ACC_WB13T_BOND_HEAD_Z_L;
	astAssemblyFuncTestAxisSpec[WB_AXIS_BOND_Z].dExpectMaxAccNegDir_m_s_U = MAX_EXPECT_ACC_WB13T_BOND_HEAD_Z_U;

}

void aft_init_spec()
{
	int iFlagMachType = get_sys_machine_type_flag();  // machine type dependency Item-10

	if(iFlagMachType == WB_MACH_TYPE_ONE_TRACK_13V_LED  || iFlagMachType == WB_STATION_XY_VERTICAL)  // iFlagSysMachineType
	{
		aft_init_spec_wb13v();
	}
	else if(iFlagMachType == WB_MACH_TYPE_HORI_LED || iFlagMachType == WB_STATION_XY_TOP)
	{
		aft_init_spec_wb13t_hori_led();
	}
	else  // WB_MACH_TYPE_VLED_FORK
	{
		aft_init_spec_wb13v();
	}

}
