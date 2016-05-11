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
// 20100819 ??

#include "stdafx.h"
#include "MtnApi.h"
#include "WinTiming.h"

#define MATRIX_COL_2       2
#define MATRIX_ROW_2       2
#define MATRIX_LEN         (MATRIX_COL_2 * MATRIX_ROW_2)
static	char strMessageDebug[64];

#define MAX_NUM_POINTS_MOVE_TO_POSN		100
#define MAX_NUM_POINTS_SCOPE_DATA		2000

void acs_cvt_float_to_24b_int_gf(double fInput, ACS_SP_VAR_FL *stpOutput);  // extern MOTALGO_DLL_API 
// static double afDrvOutRaw = stPosnCmpn(ii).Data(1:nLenDrvOut, 2);
//static double fMeanDrvOutRaw[MAX_NUM_POINTS_MOVE_TO_POSN]; // = mean(afDrvOutRaw);
//static double fMaxDrvOutRaw[MAX_NUM_POINTS_MOVE_TO_POSN]; //  = max(afDrvOutRaw);
//static double fMinDrvOutRaw[MAX_NUM_POINTS_MOVE_TO_POSN]; //  = min(afDrvOutRaw);

static double afDrvOutRaw[MAX_NUM_POINTS_SCOPE_DATA];
static double afValidDrvOut[MAX_NUM_POINTS_SCOPE_DATA];
extern double f_get_max(double *afIn, int nLen);
extern double f_get_min(double *afIn, int nLen);
extern double f_get_mean(double *afIn, int nLen);
extern COMM_SETTINGS stServoControllerCommSet;

#include "math.h"

#include "MotAlgo_DLL.h"
#include "MtnWbDef.h"
MTN_TUNE_POSN_COMPENSATION stPosnCompensationTune ={10, 2000, 7000, 0, 0};
MTUNE_OUT_POSN_COMPENSATION stPosnCompensationTuneOutput;
int iFlagInitialize1stTimeOnce = FALSE;
void mtn_tune_init_position_compensation_condition()
{
	if(iFlagInitialize1stTimeOnce == FALSE)
	{
		iFlagInitialize1stTimeOnce = TRUE;
		if(get_sys_machine_type_flag() == WB_MACH_TYPE_VLED_MAGAZINE) // Only for old bond-head
		{
			stPosnCompensationTune.dLowerPosnLimit = BE_WB_Z_BOND_LOW_LIMIT_CNT_1CUP; //
			stPosnCompensationTune.dUpperPosnLimit = BE_WB_Z_BOND_UPP_LIMIT_CNT_1CUP;
			stPosnCompensationTune.uiTotalPoints = 6;
		}
		else  // WB_MACH_TYPE_HORI_LED
		{  // Default
			stPosnCompensationTune.dLowerPosnLimit = BE_WB_Z_BOND_LOW_LIMIT_CNT_HORI_LED; // -7000;
			stPosnCompensationTune.dUpperPosnLimit = BE_WB_Z_BOND_UPP_LIMIT_CNT_HORI_LED;
			stPosnCompensationTune.uiTotalPoints = 6;
		}

		stPosnCompensationTune.dPositionFactor = 0;
		stPosnCompensationTune.dCtrlOutOffset = 0;
		stPosnCompensationTune.uiGetStartingDateLen = 200;
		stPosnCompensationTune.uiSleepInterAction_ms = 250;
	}
}

CWinThread* m_pWinThreadWbCalcBH_PosnSpringCompensate;
int iFlagStopThreadPositionCompensate = TRUE;

int mtn_tune_get_flag_stopping_position_compensation_thread()
{
	return iFlagStopThreadPositionCompensate;
}
void mtn_tune_stop_position_compensation_thread()
{
	if (m_pWinThreadWbCalcBH_PosnSpringCompensate)
	{
		iFlagStopThreadPositionCompensate = TRUE;
		WaitForSingleObject(m_pWinThreadWbCalcBH_PosnSpringCompensate->m_hThread, 1000);
		// delete m_pWinThreadSpecTest;
		m_pWinThreadWbCalcBH_PosnSpringCompensate = NULL;
	}
}

UINT mtn_tune_position_compensation_thread()
{
	while(iFlagStopThreadPositionCompensate == FALSE)
	{
		mtune_position_compensation(stServoControllerCommSet.Handle, sys_get_acs_axis_id_bnd_z(), &stPosnCompensationTune, &stPosnCompensationTuneOutput);

		iFlagStopThreadPositionCompensate = TRUE;
	}
	return 0;
}

// Position Compensation Thread
UINT mtn_tune_start_position_compensation_thread( LPVOID pParam )
{
	return mtn_tune_position_compensation_thread();
}

void mtn_thread_trigger_start_position_compensation()
{
	if(iFlagStopThreadPositionCompensate == TRUE)
	{
		iFlagStopThreadPositionCompensate = FALSE;
		//
		m_pWinThreadWbCalcBH_PosnSpringCompensate = AfxBeginThread(mtn_tune_start_position_compensation_thread, 0);
		SetPriorityClass(m_pWinThreadWbCalcBH_PosnSpringCompensate->m_hThread, REALTIME_PRIORITY_CLASS);

		m_pWinThreadWbCalcBH_PosnSpringCompensate->m_bAutoDelete = FALSE;
	}
}
int mtune_position_compensation(HANDLE Handle, int iAxis, MTN_TUNE_POSN_COMPENSATION *stpPosnCompensationTune, MTUNE_OUT_POSN_COMPENSATION *stpOutputPosnCompensationTune)
{
	double *dTunePosition, *dCtrlOutAtPosition, dTempSumPosn, dTempSumCtrlOut;
	double fMeanDrvOutRaw, fMaxDrvOutRaw, fMinDrvOutRaw, fDistToMin, fDistToMax, fDistToMean; // 20100714
	int nValidData; // 20100714
	unsigned int uiTotalPoints, ii, jj, uiGetStartingDateLen, uiSleepInterAction_ms;
	double *dMatrixSq; // 2 by 2 matrix
	double *dRHS, *dResult;      // 2
	char cErrorCode[256];
	int iRet;
	FILE *fpData = NULL;
	if(stpPosnCompensationTune->iDebug == 1)
	{
		fopen_s(&fpData, stpPosnCompensationTune->strDataFileName, "w");
	}

	mtn_api_disable_spring_compensation_sp_para(Handle); // 20110710
	
	uiGetStartingDateLen = stpPosnCompensationTune->uiGetStartingDateLen;
	uiSleepInterAction_ms = stpPosnCompensationTune->uiSleepInterAction_ms;

	uiTotalPoints = stpPosnCompensationTune->uiTotalPoints;
	dTunePosition = (double*)calloc(uiTotalPoints, sizeof(double));
	dCtrlOutAtPosition = (double*)calloc(uiTotalPoints, sizeof(double));
	dMatrixSq = (double*)calloc(MATRIX_LEN, sizeof(double));
	dRHS = (double*)calloc(MATRIX_COL_2, sizeof(double));
	dResult = (double*)calloc(MATRIX_COL_2, sizeof(double));

	for(ii = 0; ii<uiTotalPoints; ii++)
	{
		dTunePosition[ii] = stpPosnCompensationTune->dLowerPosnLimit + 
			ii * (stpPosnCompensationTune->dUpperPosnLimit - stpPosnCompensationTune->dLowerPosnLimit)
			   / (uiTotalPoints - 1);
	}
    // Prepare variables
	gstSystemScope.uiDataLen = 2000;
	gstSystemScope.uiNumData = 2;  // Bug Fixed????, actually 2 array, 20100819
	mtnscope_declare_var_on_controller(Handle);
	mtnscope_set_acsc_var_collecting_posncompn(iAxis);
	if(fpData != NULL)
	{
		fprintf(fpData, "%% ACSC Controller\n%% Variable Name:");
		ii = 0;
		while(strACSC_VarName[ii] != '\0')
		{
			if(strACSC_VarName[ii] != '\r')
			{
				fprintf(fpData, "%c", strACSC_VarName[ii]);
			}
			ii ++;
		}
		fprintf(fpData, "\n");
	}

	for(ii = 0; ii<uiTotalPoints; ii++)
	{
		if(stpPosnCompensationTune->iDebug >= 1)
		{
			sprintf_s(strMessageDebug, 64, "Axis: %d, ToPoints:%8.2f", iAxis, dTunePosition[ii]);
//			AfxMessageBox(strMessageDebug);
		}
		acsc_ToPoint(Handle, 0, // start up immediately the motion
			iAxis, dTunePosition[ii], NULL);
		Sleep(uiSleepInterAction_ms);
		if (!acsc_CollectB(Handle, 0, // system data collection
			gstrScopeArrayName, // name of data collection array
			gstSystemScope.uiDataLen, // number of samples to be collected
			1, // sampling period 1 millisecond
			strACSC_VarName, // variables to be collected
			NULL)
			)
		{
			printf("transaction error: %d\n", acsc_GetLastError());
		}
		Sleep(gstSystemScope.uiDataLen/2); // 20100714

		if (!acsc_ReadReal( Handle, ACSC_NONE, gstrScopeDataVarName, 0, 1, 0, gstSystemScope.uiDataLen - 1, gdScopeCollectData, NULL))
		{
			sprintf_s(cErrorCode, "transaction error: %d\n", acsc_GetLastError());
		}

		if(fpData != NULL)
		{
			fprintf(fpData, "%%Data at point %d \n", ii);
			fprintf(fpData, "stPosnCmpn(%d).Data = [", ii + 1);
		}
		for(jj = 0; jj<gstSystemScope.uiDataLen; jj++)
		{
			// For z-encoder inverse polarity
			gdScopeCollectData[gstSystemScope.uiDataLen + jj] = - gdScopeCollectData[gstSystemScope.uiDataLen + jj];
			if(fpData != NULL)
			{
					if(jj == gstSystemScope.uiDataLen - 1)  // last data with ];
					{
						fprintf(fpData, "%8.2f, %8.2f];\n\n", gdScopeCollectData[jj], gdScopeCollectData[gstSystemScope.uiDataLen + jj]);
					}
					else
					{
						fprintf(fpData, "%8.2f, %8.2f\n", gdScopeCollectData[jj], gdScopeCollectData[gstSystemScope.uiDataLen + jj]);
					}
			}
		}

		dTempSumPosn = 0;
		for(jj=0; jj<uiGetStartingDateLen; jj++)
		{
			dTempSumPosn += gdScopeCollectData[jj];
		}
		dTunePosition[ii] = dTempSumPosn/uiGetStartingDateLen;

#define LEN_ANALYZE_RAW_DATA   500
		dTempSumCtrlOut=0;		
		for(jj=0; jj<LEN_ANALYZE_RAW_DATA; jj++)
		{
			dTempSumCtrlOut += gdScopeCollectData[gstSystemScope.uiDataLen + jj];
			afDrvOutRaw[jj] = gdScopeCollectData[gstSystemScope.uiDataLen + jj];
		}

		fMeanDrvOutRaw = dTempSumCtrlOut/(double)LEN_ANALYZE_RAW_DATA;
		fMaxDrvOutRaw = f_get_max(afDrvOutRaw, LEN_ANALYZE_RAW_DATA);
		fMinDrvOutRaw = f_get_min(afDrvOutRaw, LEN_ANALYZE_RAW_DATA);
		 
		// nValidData
		nValidData = 0;
		for(jj=0; jj<MAX_NUM_POINTS_SCOPE_DATA; jj++)
		{
			afValidDrvOut[jj] = 0;
		}
		for(jj=0; jj<LEN_ANALYZE_RAW_DATA; jj++)
		{
			fDistToMin = fabs(afDrvOutRaw[jj] - fMinDrvOutRaw);
			fDistToMax = fabs(afDrvOutRaw[jj] - fMaxDrvOutRaw);
			fDistToMean = fabs(afDrvOutRaw[jj] - fMeanDrvOutRaw);
			if((fDistToMean < fDistToMin ) && (fDistToMean < fDistToMax))
			{
				afValidDrvOut[nValidData] = afDrvOutRaw[jj];
				nValidData = nValidData + 1;
			}
		}
		 dCtrlOutAtPosition[ii] = f_get_min(afValidDrvOut, nValidData);
	}

	// Save debug file
	if(fpData != NULL)
	{
		fprintf(fpData,"\n");
		fprintf(fpData, "nDataGroup = length(stPosnCmpn);\n");
		fprintf(fpData, "for ii = 1:1:nDataGroup\n");
		fprintf(fpData, "    PosnFb(ii) = mean(stPosnCmpn(ii).Data(1:%d, 1));\n", uiGetStartingDateLen);
		fprintf(fpData, "    CtrlOut(ii) = mean(stPosnCmpn(ii).Data(1:%d, 2));\n", uiGetStartingDateLen);
		fprintf(fpData, "end\n");
		fprintf(fpData, "figure(1);\n");
		fprintf(fpData, "plot(PosnFb, CtrlOut)\n");
		fprintf(fpData, "hold on;\n");
		fprintf(fpData, "plot(PosnFb, CtrlOut, '*')\n");
		fprintf(fpData, "Matrix(:,1) = PosnFb';\n");
		fprintf(fpData, "Matrix(:,2) = ones(size(PosnFb'));\n");
		fprintf(fpData, "PosnCompensatePara = Matrix\\CtrlOut';\n");
		fprintf(fpData, "PosnFactor = PosnCompensatePara(1)\n");
		fprintf(fpData, "Offset = PosnCompensatePara(2)\n");
		fprintf(fpData, "\n");

		fprintf(fpData, "figure(2);\n");
		for(ii=0; ii<uiTotalPoints; ii++)
		{
			fprintf(fpData, "dValidCtrlOut(%d) = %f;\n", ii+1, dCtrlOutAtPosition[ii]);
		}
		fprintf(fpData, "plot(PosnFb, dValidCtrlOut, '*')\n");
		fprintf(fpData, "\n");
		fprintf(fpData, "\n");

		fprintf(fpData, "wb_calc_position_compensate\n");
	}
	// Calculation
	for(ii = 0; ii<uiTotalPoints; ii++)
	{
		dMatrixSq[0 * MATRIX_COL_2 + 0] += dTunePosition[ii] * dTunePosition[ii];
		dMatrixSq[0 * MATRIX_COL_2 + 1] += dTunePosition[ii];
		dRHS[0] += dTunePosition[ii] * dCtrlOutAtPosition[ii];
		dRHS[1] += dCtrlOutAtPosition[ii];
	}
	dMatrixSq[1 * MATRIX_COL_2 + 0] = dMatrixSq[0 * MATRIX_COL_2 + 1];
	dMatrixSq[1 * MATRIX_COL_2 + 1] = uiTotalPoints;
	if(brinv(dMatrixSq, MATRIX_COL_2) == 0)
	{
		dResult[0] = 0;
		dResult[1] = 0;
		sprintf_s(strMessageDebug, 64, "Matrix singular, %s, %d", __FILE__, __LINE__);
		if(stpPosnCompensationTune->iDebug >= 1)
		{
//			AfxMessageBox(strMessageDebug);
		}
		if(fpData != NULL)
		{
			fprintf(fpData, "strMessageDebug\n");
		}
		iRet = MTN_API_ERROR_NOT_SUPPORT_VENDOR;
	}
	else
	{
		brmul(dMatrixSq, dRHS, MATRIX_COL_2, MATRIX_ROW_2, 1, dResult);
		stpOutputPosnCompensationTune->dPositionFactor = dResult[0];
		stpOutputPosnCompensationTune->dCtrlOutOffset = dResult[1];

		ACS_SP_VAR_FL stOutputACS_FL_SP;
		acs_cvt_float_to_24b_int_gf(stpOutputPosnCompensationTune->dPositionFactor, &stOutputACS_FL_SP);

		// Read encoder offset
		int iEncoderOffset;
		acsc_ReadInteger(Handle, NULL, "EOFFS", iAxis, iAxis, 0, 0, &iEncoderOffset, NULL);

		stpOutputPosnCompensationTune->iFactor_SP_ACS = stOutputACS_FL_SP.iFactor;
		stpOutputPosnCompensationTune->iGain_SP_ACS = -stOutputACS_FL_SP.iGain;
		stpOutputPosnCompensationTune->iOffset_SP_ACS = (int)(stpOutputPosnCompensationTune->dCtrlOutOffset / (-stpOutputPosnCompensationTune->dPositionFactor)) - iEncoderOffset;

		if(fpData != NULL)
		{
			fprintf(fpData, "%% Calculation By C Library\n");
			fprintf(fpData, "PosnFactorByC = %10.6f\n", dResult[0]);
			fprintf(fpData, "OffsetByC = %10.6f\n", dResult[1]);
			fprintf(fpData, "EncoderOffset = %d\n", iEncoderOffset);
			fprintf(fpData, "ACS_FL_Factor = %d\n", stpOutputPosnCompensationTune->iFactor_SP_ACS);
			fprintf(fpData, "ACS_FL_Gain = %d\n", stpOutputPosnCompensationTune->iGain_SP_ACS);
			fprintf(fpData, "ACS_FL_Offset = %d\n", stpOutputPosnCompensationTune->iOffset_SP_ACS);
		}

		mtn_api_set_spring_compensation_sp_para(Handle, stpOutputPosnCompensationTune, fpData);

		mtn_api_enable_spring_compensation_sp_para(Handle); // 20110710
		if(fpData != NULL)
		{			
			fclose(fpData);
		}
		iRet = MTN_API_OK_ZERO;
	}
// "SETSPV(SP0:SpringOffset, %d - EOFFS4)", 
	//

	free(dMatrixSq);
	free(dRHS);
	free(dResult);
	free(dTunePosition);
	free(dCtrlOutAtPosition);

	return iRet;
}


