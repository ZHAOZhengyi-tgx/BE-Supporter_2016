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


// 20100929 Check upper boundary
// 20101002 Skip tunning parameter for 1st and 2nd contact, 
// 20101015 Consider RMS of static position error, 

#include "stdafx.h"

#include "MtnTune.h"

#include "math.h"
#include "MtnInitAcs.h"
#include "GA.h"

#include "MotAlgo_DLL.h"

UINT MtnTune_GA_GroupThread( LPVOID pParam )
{
    CMtnTune* pObject = (CMtnTune *)pParam;
	return pObject->mtn_tune_ga_groups_thread(); 	
}

void CMtnTune::mtn_tune_ga_start_groups_thread()
{
	cFlagTuningGA_DoingGroupThread = TRUE;
	pMtnTuneGA_GroupWinThread = AfxBeginThread(MtnTune_GA_GroupThread, this); // , THREAD_PRIORITY_TIME_CRITICAL);
	pMtnTuneGA_GroupWinThread->m_bAutoDelete = FALSE;
}

void CMtnTune::mtn_tune_ga_stop_groups_thread()
{
	if(pMtnTuneGA_GroupWinThread)
	{
		WaitForSingleObject(pMtnTuneGA_GroupWinThread->m_hThread, 2000);
		pMtnTuneGA_GroupWinThread = NULL;
	}
	cFlagTuningGA_DoingGroupThread = FALSE;
}

char CMtnTune::mtn_tune_ga_get_flag_doing_group_thread()
{
	return cFlagTuningGA_DoingGroupThread;
}

void CMtnTune::mtn_tune_ga_init(MTN_TUNE_GENETIC_INPUT *stpMtnTuneGeneticInput)
{
	DWORD ii;

	stTuneAlgoSetting = stpMtnTuneGeneticInput->stTuningAlgoSetting;

	stGA_Config.fPerCrossOver= stTuneAlgoSetting.dCrossOverPercent;
	stGA_Config.fEpsilonStopVar = stTuneAlgoSetting.dStopingStd;
	stGA_Config.fMutionPercent = stTuneAlgoSetting.dMutionPercent;
	stGA_Config.nDimVar = (unsigned int) sizeof(MTN_TUNE_PARAMETER_SET)/sizeof(double);
	stGA_Config.nPopSize = stTuneAlgoSetting.uiPopulationSize ;

	for(ii=0; ii<stGA_Config.nDimVar; ii++)
	{
		stGA_Input.afMaxX[ii] = stpMtnTuneGeneticInput->stTuningParameterUppBound.afGA_PopulaVar[ii];
		stGA_Input.afMinX[ii] = stpMtnTuneGeneticInput->stTuningParameterLowBound.afGA_PopulaVar[ii];
	}
#ifdef __GA_FLOATING_POINT
	mTuningGA_Setting.ga_init_cfg_bound(&stGA_Config, &stGA_Input);
#endif // __GA_FLOATING_POINT

	stTuningTheme     = stpMtnTuneGeneticInput->stTuningTheme;
	stTuningCondition = stpMtnTuneGeneticInput->stTuningCondition;
	stTuningEncTimeConfig    = stpMtnTuneGeneticInput->stTuningEncTimeConfig;
	stTuningPassSpecfication = stpMtnTuneGeneticInput->stTuningPassSpecfication;

	fThresholdSettleTime = stTuningEncTimeConfig.dSettleThreshold_um/stTuningEncTimeConfig.dEncCntUnit_um;
	iMovePoint1 = (int)stTuningCondition.dMovePosition_1;				// communication handle
	iMovePoint2 = (int)stTuningCondition.dMovePointion_2;
	iDelayInterMove_ms = stTuningCondition.iIterMotionDelay_ms;

	Handle = stpMtnTuneGeneticInput->stCommHandle;
	iAxisTuning = stpMtnTuneGeneticInput->iAxisTuning;

}

void CMtnTune::mtn_tune_ga_one_group() 
{
//	int ii, jj;

	cFlagTuningOption = __MTN_TUNE_OPTION_GA__;
	uiTotalNumIterTunigThread = stTuneAlgoSetting.uiPopulationSize;
	uiCurrIterWithinTuningThread = 0;

	// Get the resulting Obj, save to file
}

#define MTN_TUNING_GA_SAVE_FILE   "MtnTuneGA.txt"
UINT CMtnTune::mtn_tune_ga_groups_thread()
{
unsigned int uiTuningGenGA;
	fopen_s(&fpDataTuneGA, MTN_TUNING_GA_SAVE_FILE, "w");
	fprintf_s(fpDataTuneGA, "%%%% Tuning GA\n");

	// Initialize first population To tune parameter by genetic algorithm
	uiTuningGenGA = 1;

	while(uiTuningGenGA <= stTuneAlgoSetting.uiMaxGeneration && cFlagTuningGA_DoingGroupThread == TRUE)
	{
		// For one group of parameter, exec the motion and get obj
		// mtn_tune_one_group_by_thread();

		// Record whatever parameters, whose performance meet the spec

		// Selection, CrossOver and Mutation to generate parameters for next gen

		uiTuningGenGA ++;
	}
	fclose(fpDataTuneGA);

	cFlagTuningGA_DoingGroupThread = FALSE;

	return 0;
}

short CMtnTune::mtn_tune_load_input_file(char *strFilename, HANDLE stCommHandle, int iAxisTuning, MTN_TUNE_GENETIC_INPUT *stpMtnTuneInput)
{
	short sRet = MTN_API_OK_ZERO;
	FILE *fptr;

	fopen_s(&fptr, strFilename, "w");
	if(fptr == NULL)
	{
		sRet = MTN_TUNE_ERROR_OPEN_FILE;
		goto label_mtn_tune_load_input_file_ret;
	}

	fclose(fptr);

label_mtn_tune_load_input_file_ret:
	return sRet;
}

void CMtnTune::StopTuneCompleteMesh()
{
	cFlagStopTuneCompleteMesh = TRUE;
}

double CMtnTune::CalculateObjByThemeIndex_DRA(MTN_TUNE_THEME *stpTuningTheme, MTN_SERVO_TUNE_INDEX  *stpMtnServoTuneIndex)
{
	// fRMS_StaticPE
	return stpMtnServoTuneIndex->fRMS_StaticPE;
}

double CMtnTune::CalculateObjByThemeIndex(MTN_TUNE_THEME *stpTuningTheme, MTN_SERVO_TUNE_INDEX  *stpMtnServoTuneIndex)
{
double dObj;
	dObj = MTN_TUNE_OBJ_LARGE_NUMBER;
	switch(stpTuningTheme->iThemeType)
	{
	case TUNE_THEME_OPT_MIN_DPE_DIFF_DPE:
		dObj = stpTuningTheme->fWeightDiffDPE * fabs(stpMtnServoTuneIndex->fMaxBackMoveDPE + stpMtnServoTuneIndex->fMaxFrontMoveDPE)
			+ stpTuningTheme->fWeightNegativeDPE * fabs(stpMtnServoTuneIndex->fMaxBackMoveDPE)
			+ stpTuningTheme->fWeightPositiveDPE * fabs(stpMtnServoTuneIndex->fMaxFrontMoveDPE);
		break;
	case TUNE_THEME_OPT_MIN_CEOUS:
		dObj = stpTuningTheme->fWeightCmdEndOverUnderShoot * (fabs(stpMtnServoTuneIndex->fCmdEndFrontOUS) + fabs(stpMtnServoTuneIndex->fCmdEndBackOUS));
		break;
	case TUNE_THEME_MIN_SETTLE_TIME:
		dObj = stpTuningTheme->fWeightSettlingTime * 
			(stpMtnServoTuneIndex->fSettleTime1stBack + stpMtnServoTuneIndex->fSettleTime1stFront + stpMtnServoTuneIndex->fSettleTime2ndFront + stpMtnServoTuneIndex->fSettleTime2ndBack);
		break;
	case TUNE_THEME_MIN_PEAK_DAC:
		dObj = stpTuningTheme->fWeightPeakDAC * fabs(stpMtnServoTuneIndex->fPeakPercentDAC);
		break;
	case TUNE_THEME_MIN_RMS_DAC:
		dObj = stpTuningTheme->fWeightRMS_DAC * fabs(stpMtnServoTuneIndex->fRMS_Over32767_DAC);
		break;
	case THEME_MIN_CEOUS_DPE: //  = 5,
		dObj = stpTuningTheme->fWeightDiffDPE * fabs(stpMtnServoTuneIndex->fMaxBackMoveDPE + stpMtnServoTuneIndex->fMaxFrontMoveDPE)
			+ stpTuningTheme->fWeightNegativeDPE * fabs(stpMtnServoTuneIndex->fMaxBackMoveDPE)
			+ stpTuningTheme->fWeightPositiveDPE * fabs(stpMtnServoTuneIndex->fMaxFrontMoveDPE)
			+ stpTuningTheme->fWeightCmdEndOverUnderShoot * (fabs(stpMtnServoTuneIndex->fCmdEndFrontOUS) + fabs(stpMtnServoTuneIndex->fCmdEndBackOUS));
		break;
	case THEME_MIN_CEOUS_SETTLE_TIME: // = 6,
		dObj = stpTuningTheme->fWeightCmdEndOverUnderShoot * (fabs(stpMtnServoTuneIndex->fCmdEndFrontOUS) + fabs(stpMtnServoTuneIndex->fCmdEndBackOUS))
			+ stpTuningTheme->fWeightSettlingTime * 
			(stpMtnServoTuneIndex->fSettleTime1stBack + stpMtnServoTuneIndex->fSettleTime1stFront + stpMtnServoTuneIndex->fSettleTime2ndFront + stpMtnServoTuneIndex->fSettleTime2ndBack);
		break;
	case THEME_MIN_DPE_SETTLE_TIME: // = 7,
		dObj = stpTuningTheme->fWeightDiffDPE * fabs(stpMtnServoTuneIndex->fMaxBackMoveDPE + stpMtnServoTuneIndex->fMaxFrontMoveDPE)
			+ stpTuningTheme->fWeightNegativeDPE * fabs(stpMtnServoTuneIndex->fMaxBackMoveDPE)
			+ stpTuningTheme->fWeightSettlingTime * 
			(stpMtnServoTuneIndex->fSettleTime1stBack + stpMtnServoTuneIndex->fSettleTime1stFront + stpMtnServoTuneIndex->fSettleTime2ndFront + stpMtnServoTuneIndex->fSettleTime2ndBack);
		break;
	case THEME_MIN_DPE_W_DAC: // = 8,
		dObj = stpTuningTheme->fWeightDiffDPE * fabs(stpMtnServoTuneIndex->fMaxBackMoveDPE + stpMtnServoTuneIndex->fMaxFrontMoveDPE)
			+ stpTuningTheme->fWeightNegativeDPE * fabs(stpMtnServoTuneIndex->fMaxBackMoveDPE)
			+ stpTuningTheme->fWeightPositiveDPE * fabs(stpMtnServoTuneIndex->fMaxFrontMoveDPE)
			+ stpTuningTheme->fWeightPeakDAC * fabs(stpMtnServoTuneIndex->fPeakPercentDAC * 100)
			+ stpTuningTheme->fWeightRMS_DAC * fabs(stpMtnServoTuneIndex->fRMS_Over32767_DAC * 100);
		break;
	case THEME_MIN_CEOUS_W_DAC: // = 9,
		dObj = stpTuningTheme->fWeightCmdEndOverUnderShoot * (fabs(stpMtnServoTuneIndex->fCmdEndFrontOUS) + fabs(stpMtnServoTuneIndex->fCmdEndBackOUS))
			+ stpTuningTheme->fWeightPeakDAC * fabs(stpMtnServoTuneIndex->fPeakPercentDAC * 100)
			+ stpTuningTheme->fWeightRMS_DAC * fabs(stpMtnServoTuneIndex->fRMS_Over32767_DAC * 100);
		break;
	case THEME_MIN_S_TIME_W_DAC: // = 10,
		dObj = stpTuningTheme->fWeightSettlingTime * 
			(stpMtnServoTuneIndex->fSettleTime1stBack + stpMtnServoTuneIndex->fSettleTime1stFront + stpMtnServoTuneIndex->fSettleTime2ndFront + stpMtnServoTuneIndex->fSettleTime2ndBack)
			+ stpTuningTheme->fWeightPeakDAC * fabs(stpMtnServoTuneIndex->fPeakPercentDAC * 100)
			+ stpTuningTheme->fWeightRMS_DAC * fabs(stpMtnServoTuneIndex->fRMS_Over32767_DAC * 100);
		break;
	case THEME_MIN_CEOUS_DPE_W_DAC: // = 11,
		dObj = stpTuningTheme->fWeightDiffDPE * fabs(stpMtnServoTuneIndex->fMaxBackMoveDPE + stpMtnServoTuneIndex->fMaxFrontMoveDPE)
			+ stpTuningTheme->fWeightNegativeDPE * fabs(stpMtnServoTuneIndex->fMaxBackMoveDPE)
			+ stpTuningTheme->fWeightPositiveDPE * fabs(stpMtnServoTuneIndex->fMaxFrontMoveDPE)
			+ stpTuningTheme->fWeightCmdEndOverUnderShoot * (fabs(stpMtnServoTuneIndex->fCmdEndFrontOUS) + fabs(stpMtnServoTuneIndex->fCmdEndBackOUS))
			+ stpTuningTheme->fWeightPeakDAC * fabs(stpMtnServoTuneIndex->fPeakPercentDAC * 100)
			+ stpTuningTheme->fWeightRMS_DAC * fabs(stpMtnServoTuneIndex->fRMS_Over32767_DAC * 100)
			+ stpTuningTheme->fWeightRMS_StaticPE * pow(stpMtnServoTuneIndex->fRMS_StaticPE, 4.0); //  * stpMtnServoTuneIndex->fRMS_StaticPE * stpMtnServoTuneIndex->fRMS_StaticPE * stpMtnServoTuneIndex->fRMS_StaticPE;  // 20101015
		break;
	case THEME_MIN_CEOUS_SETTLE_TIME_W_DAC: // = 12,
		dObj = stpTuningTheme->fWeightCmdEndOverUnderShoot * (fabs(stpMtnServoTuneIndex->fCmdEndFrontOUS) + fabs(stpMtnServoTuneIndex->fCmdEndBackOUS))
			+ stpTuningTheme->fWeightSettlingTime * 
			(stpMtnServoTuneIndex->fSettleTime1stBack + stpMtnServoTuneIndex->fSettleTime1stFront + stpMtnServoTuneIndex->fSettleTime2ndFront + stpMtnServoTuneIndex->fSettleTime2ndBack)
			+ stpTuningTheme->fWeightPeakDAC * fabs(stpMtnServoTuneIndex->fPeakPercentDAC * 100)
			+ stpTuningTheme->fWeightRMS_DAC * fabs(stpMtnServoTuneIndex->fRMS_Over32767_DAC * 100)
			+ stpTuningTheme->fWeightRMS_StaticPE * pow(stpMtnServoTuneIndex->fRMS_StaticPE, 4.0);
		break;
	case THEME_MIN_DPE_SETTLE_TIME_W_DAC: // = 13,
		dObj = stpTuningTheme->fWeightDiffDPE * fabs(stpMtnServoTuneIndex->fMaxBackMoveDPE + stpMtnServoTuneIndex->fMaxFrontMoveDPE)
			+ stpTuningTheme->fWeightNegativeDPE * fabs(stpMtnServoTuneIndex->fMaxBackMoveDPE)
			+ stpTuningTheme->fWeightSettlingTime * 
			(stpMtnServoTuneIndex->fSettleTime1stBack + stpMtnServoTuneIndex->fSettleTime1stFront + stpMtnServoTuneIndex->fSettleTime2ndFront + stpMtnServoTuneIndex->fSettleTime2ndBack)
			+ stpTuningTheme->fWeightPeakDAC * fabs(stpMtnServoTuneIndex->fPeakPercentDAC * 100)
			+ stpTuningTheme->fWeightRMS_DAC * fabs(stpMtnServoTuneIndex->fRMS_Over32767_DAC * 100)
			+ stpTuningTheme->fWeightRMS_StaticPE * pow(stpMtnServoTuneIndex->fRMS_StaticPE, 4.0); 
		break;
	case THEME_MIN_CEOUS_BH_DPE_W_DAC: // = 14,
		dObj = 0;
		if(stpMtnServoTuneIndex->fCmdEndFrontOUS < 0)
			dObj = dObj + fabs(stpMtnServoTuneIndex->fCmdEndFrontOUS) * stpTuningTheme->fPenaltyCmdEndOverUnderShoot;
		else
			dObj = dObj + stpMtnServoTuneIndex->fCmdEndFrontOUS * stpTuningTheme->fWeightCmdEndOverUnderShoot;

		if(stpMtnServoTuneIndex->fCmdEndBackOUS < 0)
			dObj = dObj + fabs(stpMtnServoTuneIndex->fCmdEndBackOUS) * stpTuningTheme->fPenaltyCmdEndOverUnderShoot;
		else
			dObj = dObj + stpMtnServoTuneIndex->fCmdEndBackOUS * stpTuningTheme->fWeightCmdEndOverUnderShoot;

		dObj = dObj + stpTuningTheme->fWeightDiffDPE * fabs(stpMtnServoTuneIndex->fMaxBackMoveDPE + stpMtnServoTuneIndex->fMaxFrontMoveDPE)
			+ stpTuningTheme->fWeightNegativeDPE * fabs(stpMtnServoTuneIndex->fMaxBackMoveDPE)
			+ stpTuningTheme->fWeightPositiveDPE * fabs(stpMtnServoTuneIndex->fMaxFrontMoveDPE)
			+ stpTuningTheme->fWeightPeakDAC * fabs(stpMtnServoTuneIndex->fPeakPercentDAC * 100)
			+ stpTuningTheme->fWeightRMS_DAC * fabs(stpMtnServoTuneIndex->fRMS_Over32767_DAC * 100);
		break;
	case THEME_MIN_CEOUS_BH_W_DAC: // = 15,
		dObj = 0;
		if(stpMtnServoTuneIndex->fCmdEndFrontOUS < 0)
			dObj = dObj + fabs(stpMtnServoTuneIndex->fCmdEndFrontOUS) * stpTuningTheme->fPenaltyCmdEndOverUnderShoot;
		else
			dObj = dObj + stpMtnServoTuneIndex->fCmdEndFrontOUS * stpTuningTheme->fWeightCmdEndOverUnderShoot;

		if(stpMtnServoTuneIndex->fCmdEndBackOUS < 0)
			dObj = dObj + fabs(stpMtnServoTuneIndex->fCmdEndBackOUS) * stpTuningTheme->fPenaltyCmdEndOverUnderShoot;
		else
			dObj = dObj + stpMtnServoTuneIndex->fCmdEndBackOUS * stpTuningTheme->fWeightCmdEndOverUnderShoot;

		dObj = dObj + stpTuningTheme->fWeightPeakDAC * fabs(stpMtnServoTuneIndex->fPeakPercentDAC * 100)
			+ stpTuningTheme->fWeightRMS_DAC * fabs(stpMtnServoTuneIndex->fRMS_Over32767_DAC * 100);
		break;
	default:
		break;
	}
	return dObj;
}

static int idxCurrCaseCompleteTune, nTotalCasesCompleteTune;
static double dBestTuneObjCompleteTune;
void CMtnTune::mtn_tune_get_status_axis_complete(unsigned int *idxCurrTune, unsigned int *nTotal, double *dCurrBest)
{
	*idxCurrTune = idxCurrCaseCompleteTune;
	*nTotal = nTotalCasesCompleteTune;
	*dCurrBest = dBestTuneObjCompleteTune;
}

MTN_TUNE_CASE astMotionTuneHistory[MAX_CASE_TUNING_HISTORY_BUFFER];

short CMtnTune::mtn_tune_axis_complete_mesh(MTN_TUNE_GENETIC_INPUT *stpMtnTuneInput, MTN_TUNE_GENETIC_OUTPUT *stpMtnTuneOutput)
{
	TUNE_ALGO_SETTING       *stpTuningAlgoSetting = &stpMtnTuneInput->stTuningAlgoSetting;
	MTN_TUNE_PARA_UNION		*stpTuningParameterIni = &stpMtnTuneInput->stTuningParameterIni;
	MTN_TUNE_PARA_UNION		*stpTuningParameterUppBound = &stpMtnTuneInput->stTuningParameterUppBound;
	MTN_TUNE_PARA_UNION		*stpTuningParameterLowBound = &stpMtnTuneInput->stTuningParameterLowBound;
	MTN_TUNE_THEME			*stpTuningTheme = &stpMtnTuneInput->stTuningTheme;
	MTN_TUNE_CONDITION		*stpTuningCondition = &stpMtnTuneInput->stTuningCondition;
	MTN_TUNE_ENC_TIME_CFG	*stpTuningEncTimeConfig = &stpMtnTuneInput->stTuningEncTimeConfig;
	MTN_TUNE_SPECIFICATION  *stpTuningPassSpecfication = &stpMtnTuneInput->stTuningPassSpecfication;
	HANDLE stCommHandle = stpMtnTuneInput->stCommHandle;
	int	iAxisTuningACS = stpMtnTuneInput->iAxisTuning;
	double adParaPosnKP[NUM_POINTS_ONE_DIM_MESH], dStepPosnKP;
	double adParaVelKP[NUM_POINTS_ONE_DIM_MESH], dStepVelKP;
	double adParaVelKI[NUM_POINTS_ONE_DIM_MESH], dStepVelKI;
	double adParaAccFFC[NUM_POINTS_ONE_DIM_MESH], dStepAccFFC;
	int ii, jj, kk, ll;
	short sRet = MTN_API_OK_ZERO;
	int idxBestParaSet;

	CTRL_PARA_ACS stAxisServoCtrlParaBak, stAxisServoCtrlParaAtTuningACS;

	cFlagStopTuneCompleteMesh = FALSE;
	mtnapi_upload_servo_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stAxisServoCtrlParaBak);
	//Clear the tuning history buffer
	memset(astMotionTuneHistory, 0, sizeof(MTN_TUNE_CASE) * MAX_CASE_TUNING_HISTORY_BUFFER);

	SAFETY_PARA_ACS stSafetyParaBak, stSafetyParaCurr;
	mtnapi_upload_safety_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stSafetyParaBak);
	stSafetyParaCurr = stSafetyParaBak;
	stSafetyParaCurr.dCriticalPosnErrAcc = 5000;
	stSafetyParaCurr.dCriticalPosnErrIdle = 1000;	
	stSafetyParaCurr.dCriticalPosnErrVel = 1000;
	mtnapi_download_safety_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stSafetyParaCurr);

	stAxisServoCtrlParaAtTuningACS = stAxisServoCtrlParaBak;
	stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain = stpTuningParameterLowBound->stMtnPara.dPosnLoopKP;

	dStepPosnKP = (stpTuningParameterUppBound->stMtnPara.dPosnLoopKP - stpTuningParameterLowBound->stMtnPara.dPosnLoopKP)/(NUM_POINTS_ONE_DIM_MESH - 1);
	dStepVelKP = (stpTuningParameterUppBound->stMtnPara.dVelLoopKP - stpTuningParameterLowBound->stMtnPara.dVelLoopKP)/(NUM_POINTS_ONE_DIM_MESH - 1);
	dStepVelKI = (stpTuningParameterUppBound->stMtnPara.dVelLoopKI - stpTuningParameterLowBound->stMtnPara.dVelLoopKI)/(NUM_POINTS_ONE_DIM_MESH - 1);
	dStepAccFFC = (stpTuningParameterUppBound->stMtnPara.dAccFFC - stpTuningParameterLowBound->stMtnPara.dAccFFC)/(NUM_POINTS_ONE_DIM_MESH - 1);

	for(ii =0; ii<NUM_POINTS_ONE_DIM_MESH; ii++)
	{
		adParaPosnKP[ii] = stpTuningParameterLowBound->stMtnPara.dPosnLoopKP + dStepPosnKP * ii;
		adParaVelKP[ii] = stpTuningParameterLowBound->stMtnPara.dVelLoopKP + dStepVelKP * ii;
		adParaVelKI[ii] = stpTuningParameterLowBound->stMtnPara.dVelLoopKI + dStepVelKI * ii;
		adParaAccFFC[ii] = stpTuningParameterLowBound->stMtnPara.dAccFFC + dStepAccFFC * ii;
	}
	// Set upload variables
	mtnscope_set_acsc_var_collecting_move(iAxisTuningACS);

double fThresholdSettleTime; //  = stpTuningEncTimeConfig->dSettleThreshold_um / stpTuningEncTimeConfig->dEncCntUnit_um;
int iInterMoveDelay = stpTuningCondition->iIterMotionDelay_ms;
MTN_TIME_IDX_2_LOOP_MOVE stMtnTimeIdx2LoopMove;
MTN_SERVO_TUNE_INDEX  stMtnServoTuneIndex;

	if(stpTuningEncTimeConfig->dEncCntUnit_um > 1E-6)
	{
		fThresholdSettleTime = 10;
	}
	else
	{
		fThresholdSettleTime = stpTuningEncTimeConfig->dSettleThreshold_um / stpTuningEncTimeConfig->dEncCntUnit_um;
	}
	mtn_tune_initialize_class_var(fThresholdSettleTime, stCommHandle, iAxisTuningACS, iInterMoveDelay);
	mtnapi_set_speed_profile(stCommHandle, iAxisTuningACS,	&stpTuningCondition->stMtnSpeedProfile, NULL);
	iMovePoint1 = (int)stpTuningCondition->dMovePosition_1;
	iMovePoint2 = (int)stpTuningCondition->dMovePointion_2;
	OneAxis2PointsMoveOneCycle(); // Move once, get ready to move to 1st point
	// Move to get initial performance
	mtn_tune_start_2_points_move_with_data_collect();
	// Calculate time index
	mtn_tune_calc_time_idx_2_loop_move(&stMtnTimeIdx2LoopMove);
	// Calculate servo tuning index
	mtn_tune_calc_servo_perform_index(fThresholdSettleTime, &stMtnTimeIdx2LoopMove, &stMtnServoTuneIndex);
	stpMtnTuneOutput->stInitialResponse = stMtnServoTuneIndex;
	stpMtnTuneOutput->dInitialObj = CalculateObjByThemeIndex(stpTuningTheme, &stMtnServoTuneIndex);
	
	idxCurrCaseCompleteTune = idxBestParaSet = 0;
	nTotalCasesCompleteTune = NUM_POINTS_ONE_DIM_MESH * NUM_POINTS_ONE_DIM_MESH * NUM_POINTS_ONE_DIM_MESH * NUM_POINTS_ONE_DIM_MESH;
	for(ii = 0; ii<NUM_POINTS_ONE_DIM_MESH; ii++)
	{
		for(jj = 0; jj<NUM_POINTS_ONE_DIM_MESH; jj++)
		{
			for(kk = 0; kk<NUM_POINTS_ONE_DIM_MESH; kk++)
			{
				for(ll = 0; ll<NUM_POINTS_ONE_DIM_MESH; ll++)
				{
					stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain = adParaVelKP[ii];
					stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain = adParaVelKI[jj];
					stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward = adParaAccFFC[kk];
					stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain = adParaPosnKP[ll];

					mtnapi_download_servo_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &(stAxisServoCtrlParaAtTuningACS));
					Sleep(10);

					mtn_tune_start_2_points_move_with_data_collect();
					// Calculate time index
					mtn_tune_calc_time_idx_2_loop_move(&stMtnTimeIdx2LoopMove);
					// Calculate servo tuning index
					mtn_tune_calc_servo_perform_index(fThresholdSettleTime, &stMtnTimeIdx2LoopMove, &stMtnServoTuneIndex);

					astMotionTuneHistory[idxCurrCaseCompleteTune].dTuningObj = CalculateObjByThemeIndex(stpTuningTheme, &stMtnServoTuneIndex);
					astMotionTuneHistory[idxCurrCaseCompleteTune].stServoPara.stMtnPara.dVelLoopKP = adParaVelKP[ii];
					astMotionTuneHistory[idxCurrCaseCompleteTune].stServoPara.stMtnPara.dVelLoopKI = adParaVelKI[jj];
					astMotionTuneHistory[idxCurrCaseCompleteTune].stServoPara.stMtnPara.dAccFFC = adParaAccFFC[kk];
					astMotionTuneHistory[idxCurrCaseCompleteTune].stServoPara.stMtnPara.dPosnLoopKP = adParaPosnKP[ll];
					astMotionTuneHistory[idxCurrCaseCompleteTune].stServoTuneIndex = stMtnServoTuneIndex;

					if(idxCurrCaseCompleteTune ==0)
					{
						dBestTuneObjCompleteTune = astMotionTuneHistory[idxCurrCaseCompleteTune].dTuningObj;
						idxBestParaSet = idxCurrCaseCompleteTune;
					}
					else
					{
						if(dBestTuneObjCompleteTune > astMotionTuneHistory[idxCurrCaseCompleteTune].dTuningObj)
						{
							dBestTuneObjCompleteTune = astMotionTuneHistory[idxCurrCaseCompleteTune].dTuningObj;
							idxBestParaSet = idxCurrCaseCompleteTune;
						}
					}
					idxCurrCaseCompleteTune++;

					if(cFlagStopTuneCompleteMesh == TRUE || qc_is_axis_not_safe(stCommHandle, iAxisTuningACS))
					{
						goto label_mtn_tune_axis_complete_mesh;
					}
				}
			}
		}
	}

label_mtn_tune_axis_complete_mesh:

	stpMtnTuneOutput->dTuningObj = dBestTuneObjCompleteTune;
	stpMtnTuneOutput->iFlagTuningIsFail = 0;
	stpMtnTuneOutput->stBestParameterSet = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara;
	stpMtnTuneOutput->stResultResponse = astMotionTuneHistory[idxBestParaSet].stServoTuneIndex;

	stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward = stpMtnTuneOutput->stBestParameterSet.dAccFFC;
	stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain = stpMtnTuneOutput->stBestParameterSet.dPosnLoopKP;
	stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain = stpMtnTuneOutput->stBestParameterSet.dVelLoopKI;
	stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain = stpMtnTuneOutput->stBestParameterSet.dVelLoopKP;

	mtnapi_download_servo_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &(stAxisServoCtrlParaAtTuningACS));
	mtnapi_download_safety_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stSafetyParaBak);
//	mtnapi_download_servo_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stAxisServoCtrlParaBak);

	if(stpMtnTuneInput->iDebugFlag == TRUE)
	{
		mtn_tune_save_tuning_history(stpMtnTuneInput, stpMtnTuneOutput, idxCurrCaseCompleteTune);
	}

	return sRet;
}

////////
unsigned int idxCurrCase1By1, nTotalCases1By1;
double dBestTuneObj1By1;
int idxBestParaSet;

void CMtnTune::mtn_tune_get_status_axis_1by1(unsigned int *idxCurrTune, unsigned int *nTotal, double *dCurrBest)
{
	*idxCurrTune = idxCurrCase1By1;
	*nTotal = nTotalCases1By1;
	*dCurrBest = dBestTuneObj1By1;
}
int CMtnTune::mtn_tune_get_best_idx_tune_1by1()
{
	return idxBestParaSet;
}

double f_get_round(double fIn)
{
	double dOut;

	if(fIn > 0)
	{
		dOut = (double)((int)(fIn + 0.5));
	}
	else
	{
		dOut = floor(fIn + 0.5);
	}
	return dOut;
}
void mtn_tune_round_parameter_to_nearest_10(MTN_TUNE_PARAMETER_SET *stpMtnParaRoundIn, MTN_TUNE_PARAMETER_SET *stpMtnParaRoundOut)
{
	stpMtnParaRoundOut->dVelLoopKP = f_get_round(stpMtnParaRoundIn->dVelLoopKP/10) * 10;
	stpMtnParaRoundOut->dVelLoopKI = f_get_round(stpMtnParaRoundIn->dVelLoopKI/10) * 10;
	stpMtnParaRoundOut->dPosnLoopKP = f_get_round(stpMtnParaRoundIn->dPosnLoopKP/10) * 10;
	stpMtnParaRoundOut->dAccFFC = f_get_round(stpMtnParaRoundIn->dAccFFC/10) * 10;

	stpMtnParaRoundOut->dVelLoopLimitI = f_get_round(stpMtnParaRoundIn->dVelLoopLimitI/10) * 10;
	stpMtnParaRoundOut->dSecOrdFilterFreq_Hz = f_get_round(stpMtnParaRoundIn->dSecOrdFilterFreq_Hz/10) * 10;
}

void mtn_tune_round_parameter_to_nearest_1(MTN_TUNE_PARAMETER_SET *stpMtnParaRoundIn, MTN_TUNE_PARAMETER_SET *stpMtnParaRoundOut)
{
	stpMtnParaRoundOut->dVelLoopKP = f_get_round(stpMtnParaRoundIn->dVelLoopKP);
	stpMtnParaRoundOut->dVelLoopKI = f_get_round(stpMtnParaRoundIn->dVelLoopKI);
	stpMtnParaRoundOut->dPosnLoopKP = f_get_round(stpMtnParaRoundIn->dPosnLoopKP);
	stpMtnParaRoundOut->dAccFFC = f_get_round(stpMtnParaRoundIn->dAccFFC);

	stpMtnParaRoundOut->dVelLoopLimitI = f_get_round(stpMtnParaRoundIn->dVelLoopLimitI);
	stpMtnParaRoundOut->dSecOrdFilterFreq_Hz = f_get_round(stpMtnParaRoundIn->dSecOrdFilterFreq_Hz);
}

#define  TUNE_PARA_STOP_RANGE_TABLE_XY_VKP    5
#define  TUNE_PARA_STOP_RANGE_TABLE_XY_VKI    10
#define  TUNE_PARA_STOP_RANGE_TABLE_XY_AFF    10
#define  TUNE_PARA_STOP_RANGE_TABLE_XY_PKP    10
#include "MtnWbDef.h"
extern char strWbTuningNewFolderName[];


// One after one dimension
short CMtnTune::mtn_tune_axis_one_by_one_para(MTN_TUNE_GENETIC_INPUT *stpMtnTuneInput, MTN_TUNE_GENETIC_OUTPUT *stpMtnTuneOutput)
{
	TUNE_ALGO_SETTING       *stpTuningAlgoSetting = &stpMtnTuneInput->stTuningAlgoSetting;
	MTN_TUNE_PARA_UNION		*stpTuningParameterIni = &stpMtnTuneInput->stTuningParameterIni;
	MTN_TUNE_PARA_UNION		*stpTuningParameterUppBound = &stpMtnTuneInput->stTuningParameterUppBound;
	MTN_TUNE_PARA_UNION		*stpTuningParameterLowBound = &stpMtnTuneInput->stTuningParameterLowBound;
	MTN_TUNE_THEME			*stpTuningTheme = &stpMtnTuneInput->stTuningTheme;
	MTN_TUNE_CONDITION		*stpTuningCondition = &stpMtnTuneInput->stTuningCondition;
	MTN_TUNE_ENC_TIME_CFG	*stpTuningEncTimeConfig = &stpMtnTuneInput->stTuningEncTimeConfig;
	MTN_TUNE_SPECIFICATION  *stpTuningPassSpecfication = &stpMtnTuneInput->stTuningPassSpecfication;
	HANDLE stCommHandle = stpMtnTuneInput->stCommHandle;
	int	iAxisTuningACS = stpMtnTuneInput->iAxisTuning;
	double adParaPosnKP[NUM_POINTS_ONE_DIM_MESH], dStepPosnKP;
	double adParaVelKP[NUM_POINTS_ONE_DIM_MESH], dStepVelKP;
	double adParaVelKI[NUM_POINTS_ONE_DIM_MESH], dStepVelKI;
	double adParaAccFFC[NUM_POINTS_ONE_DIM_MESH], dStepAccFFC;
	int ii, jj, kk, ll, nActualPointsTunePKP, nActualPointsTuneVKP, nActualPointsTuneAccFF, nActualPointsTuneVKI;
	short sRet = MTN_API_OK_ZERO;
	int idxBestParaSet;

	CTRL_PARA_ACS stAxisServoCtrlParaBak, stAxisServoCtrlParaAtTuningACS;
	SAFETY_PARA_ACS stSafetyParaBak, stSafetyParaCurr;
	mtnapi_upload_safety_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stSafetyParaBak);
	stSafetyParaCurr = stSafetyParaBak;
	stSafetyParaCurr.dCriticalPosnErrAcc = 5000;
	stSafetyParaCurr.dCriticalPosnErrIdle = 1000;	
	stSafetyParaCurr.dCriticalPosnErrVel = 1000;
	stSafetyParaCurr.dRMS_DrvCmdX = stSafetyParaCurr.dRMS_DrvCmdX * 2;
	stSafetyParaCurr.dRMS_DrvCmdIdle = stSafetyParaCurr.dRMS_DrvCmdIdle * 2;
	stSafetyParaCurr.dRMS_DrvCmdMtn = 100;  // 20120717, must release to 100% for motion tuning
	mtnapi_download_safety_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stSafetyParaCurr);

	cFlagStopTuneCompleteMesh = FALSE;
	mtnapi_upload_servo_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stAxisServoCtrlParaBak);
	//Clear the tuning history buffer
	memset(astMotionTuneHistory, 0, sizeof(MTN_TUNE_CASE) * MAX_CASE_TUNING_HISTORY_BUFFER);

	stAxisServoCtrlParaAtTuningACS = stAxisServoCtrlParaBak;
	stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain = stpTuningParameterLowBound->stMtnPara.dPosnLoopKP;

	MTN_TUNE_PARA_UNION	stCurrTuneParaUppBound, stCurrTuneParaLowBound;

	stCurrTuneParaUppBound.stMtnPara = stpTuningParameterUppBound->stMtnPara;
	stCurrTuneParaLowBound.stMtnPara = stpTuningParameterLowBound->stMtnPara;
	dStepPosnKP = (stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP - stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP)/(NUM_POINTS_ONE_DIM_MESH - 1);
	dStepVelKP = (stCurrTuneParaUppBound.stMtnPara.dVelLoopKP - stCurrTuneParaLowBound.stMtnPara.dVelLoopKP)/(NUM_POINTS_ONE_DIM_MESH - 1);
	dStepVelKI = (stCurrTuneParaUppBound.stMtnPara.dVelLoopKI - stCurrTuneParaLowBound.stMtnPara.dVelLoopKI)/(NUM_POINTS_ONE_DIM_MESH - 1);
	dStepAccFFC = (stCurrTuneParaUppBound.stMtnPara.dAccFFC - stCurrTuneParaLowBound.stMtnPara.dAccFFC)/(NUM_POINTS_ONE_DIM_MESH - 1);

	for(ii =0; ii<NUM_POINTS_ONE_DIM_MESH; ii++)
	{
		adParaPosnKP[ii] = stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP + dStepPosnKP * ii;
		adParaVelKP[ii] = stCurrTuneParaLowBound.stMtnPara.dVelLoopKP + dStepVelKP * ii;
		adParaVelKI[ii] = stCurrTuneParaLowBound.stMtnPara.dVelLoopKI + dStepVelKI * ii;
		adParaAccFFC[ii] = stCurrTuneParaLowBound.stMtnPara.dAccFFC + dStepAccFFC * ii;
	}
	// Set upload variables
	mtnscope_set_acsc_var_collecting_move(iAxisTuningACS);

double fThresholdSettleTime; //  = stpTuningEncTimeConfig->dSettleThreshold_um / stpTuningEncTimeConfig->dEncCntUnit_um;
int iInterMoveDelay = stpTuningCondition->iIterMotionDelay_ms;
MTN_TIME_IDX_2_LOOP_MOVE stMtnTimeIdx2LoopMove;
MTN_SERVO_TUNE_INDEX  stMtnServoTuneIndex;

	if(stpTuningEncTimeConfig->dEncCntUnit_um > 1E-6)
	{
		fThresholdSettleTime = 10;
	}
	else
	{
		fThresholdSettleTime = stpTuningEncTimeConfig->dSettleThreshold_um / stpTuningEncTimeConfig->dEncCntUnit_um;
	}
	mtn_tune_initialize_class_var(fThresholdSettleTime, stCommHandle, iAxisTuningACS, iInterMoveDelay);
	mtnapi_set_speed_profile(stCommHandle, iAxisTuningACS,	&stpTuningCondition->stMtnSpeedProfile, NULL); // 20110426
	iMovePoint1 = (int)stpTuningCondition->dMovePosition_1;
	iMovePoint2 = (int)stpTuningCondition->dMovePointion_2;
	OneAxis2PointsMoveOneCycle(); // Move once, get ready to move to 1st point
	// Move to get initial performance
	mtn_tune_start_2_points_move_with_data_collect();
	// Calculate time index
	mtn_tune_calc_time_idx_2_loop_move(&stMtnTimeIdx2LoopMove);
	// Calculate servo tuning index
	mtn_tune_calc_servo_perform_index(fThresholdSettleTime, &stMtnTimeIdx2LoopMove, &stMtnServoTuneIndex);
	stpMtnTuneOutput->stInitialResponse = stMtnServoTuneIndex;
	stpMtnTuneOutput->dInitialObj = CalculateObjByThemeIndex(stpTuningTheme, &stMtnServoTuneIndex);

	static char strMoveWaveformFilename[128];
	struct tm stTime;
	struct tm *stpTime = &stTime;
	__time64_t stLongTime;

	if(stpMtnTuneInput->iDebugFlag == 1)
	{
		_time64(&stLongTime);
		_localtime64_s(stpTime, &stLongTime);
		sprintf_s(strMoveWaveformFilename, 128, "%s\\TuneInit_Ctrl_%d_%d%d%d_%d%d%d_wvfm.m", 
			strWbTuningNewFolderName, iAxisTuningACS, stpTime->tm_year +1900, stpTime->tm_mon +1, 
					stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, stpTime->tm_sec); // 20120626

		mtn_save_curr_axis_traj_data_to_filename(strMoveWaveformFilename, iAxisTuningACS); // 
	}

	idxCurrCase1By1 = idxBestParaSet = 0;
	nTotalCases1By1 = stpTuningAlgoSetting->uiMaxGeneration * 4 * NUM_POINTS_ONE_DIM_MESH; //10000; // NUM_POINTS_ONE_DIM_MESH * NUM_POINTS_ONE_DIM_MESH * NUM_POINTS_ONE_DIM_MESH * NUM_POINTS_ONE_DIM_MESH;
	if(nTotalCases1By1 > MAX_CASE_TUNING_HISTORY_BUFFER)
	{
		nTotalCases1By1 = MAX_CASE_TUNING_HISTORY_BUFFER;
	}

	// Check upper boundary, 20100929
	if(stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward > stCurrTuneParaUppBound.stMtnPara.dAccFFC)
	{
		stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward = stCurrTuneParaUppBound.stMtnPara.dAccFFC;
	}
	if(stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain > stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP)
	{
		stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain = stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP;
	}
	if(stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain > stCurrTuneParaUppBound.stMtnPara.dVelLoopKI)
	{
		stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain = stCurrTuneParaUppBound.stMtnPara.dVelLoopKI;
	}
	if(stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain > stCurrTuneParaUppBound.stMtnPara.dVelLoopKP)
	{
		stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain = stCurrTuneParaUppBound.stMtnPara.dVelLoopKP;
	} // 20100929

unsigned int iCurrLoop =0;
	nActualPointsTunePKP = nActualPointsTuneVKP = nActualPointsTuneAccFF = nActualPointsTuneVKI = NUM_POINTS_ONE_DIM_MESH;
	
	while( (nActualPointsTunePKP >= 1 || nActualPointsTuneVKP >= 1 || nActualPointsTuneAccFF >= 1 || nActualPointsTuneVKI >= 1) 
		&& (idxCurrCase1By1 < nTotalCases1By1) && (iCurrLoop < stpTuningAlgoSetting->uiMaxGeneration) )
	{
		// VKP
		for(ii = 0; ii<nActualPointsTuneVKP; ii++)
		{
			stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain = adParaVelKP[ii];

			mtnapi_download_servo_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &(stAxisServoCtrlParaAtTuningACS));
			Sleep(10);

			mtn_tune_start_2_points_move_with_data_collect();
			// Calculate time index
			mtn_tune_calc_time_idx_2_loop_move(&stMtnTimeIdx2LoopMove);
			// Calculate servo tuning index
			mtn_tune_calc_servo_perform_index(fThresholdSettleTime, &stMtnTimeIdx2LoopMove, &stMtnServoTuneIndex);

			astMotionTuneHistory[idxCurrCase1By1].dTuningObj = CalculateObjByThemeIndex(stpTuningTheme, &stMtnServoTuneIndex);
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dVelLoopKP = stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dVelLoopKI = stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dAccFFC = stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dPosnLoopKP = stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoTuneIndex = stMtnServoTuneIndex;

			if(idxCurrCase1By1 ==0)
			{
				dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
				idxBestParaSet = idxCurrCase1By1;
			}
			else
			{
				if(dBestTuneObj1By1 > astMotionTuneHistory[idxCurrCase1By1].dTuningObj)
				{
					dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
					idxBestParaSet = idxCurrCase1By1;
				}
			}
			idxCurrCase1By1++;

			if(idxCurrCase1By1 >= nTotalCases1By1 || cFlagStopTuneCompleteMesh == TRUE || qc_is_axis_not_safe(stCommHandle, iAxisTuningACS))
			{
				goto label_mtn_tune_axis_one_by_one_para;
			}
		}
		stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara.dVelLoopKP;
		if(stCurrTuneParaLowBound.stMtnPara.dVelLoopKP < (stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain - RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKP))
		{
			stCurrTuneParaLowBound.stMtnPara.dVelLoopKP = stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain - RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKP;
		}
		if(stCurrTuneParaUppBound.stMtnPara.dVelLoopKP > (stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain + RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKP))
		{
			stCurrTuneParaUppBound.stMtnPara.dVelLoopKP = (stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain + RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKP);
		}

		dStepVelKP = (stCurrTuneParaUppBound.stMtnPara.dVelLoopKP - stCurrTuneParaLowBound.stMtnPara.dVelLoopKP)/(NUM_POINTS_ONE_DIM_MESH - 1);
		if(fabs(dStepVelKP) > TUNE_PARA_STOP_RANGE_TABLE_XY_VKP)
		{
			nActualPointsTuneVKP = NUM_POINTS_ONE_DIM_MESH;
			for(ii =0; ii<NUM_POINTS_ONE_DIM_MESH; ii++)
			{
				adParaVelKP[ii] = stCurrTuneParaLowBound.stMtnPara.dVelLoopKP  + dStepVelKP * ii;
			}
		}
		else
		{
			nActualPointsTuneVKP = 0;
		}

		// AccFFC
		for(kk = 0; kk<nActualPointsTuneAccFF; kk++)
		{
			stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward = adParaAccFFC[kk];
			mtnapi_download_servo_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &(stAxisServoCtrlParaAtTuningACS));
			Sleep(10);

			mtn_tune_start_2_points_move_with_data_collect();
			// Calculate time index
			mtn_tune_calc_time_idx_2_loop_move(&stMtnTimeIdx2LoopMove);
			// Calculate servo tuning index
			mtn_tune_calc_servo_perform_index(fThresholdSettleTime, &stMtnTimeIdx2LoopMove, &stMtnServoTuneIndex);

			astMotionTuneHistory[idxCurrCase1By1].dTuningObj = CalculateObjByThemeIndex(stpTuningTheme, &stMtnServoTuneIndex);
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dVelLoopKP = stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dVelLoopKI = stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dAccFFC = stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dPosnLoopKP = stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoTuneIndex = stMtnServoTuneIndex;

			if(dBestTuneObj1By1 > astMotionTuneHistory[idxCurrCase1By1].dTuningObj)
			{
					dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
					idxBestParaSet = idxCurrCase1By1;
			}
			idxCurrCase1By1++;

			if(idxCurrCase1By1 >= nTotalCases1By1 || cFlagStopTuneCompleteMesh == TRUE || qc_is_axis_not_safe(stCommHandle, iAxisTuningACS))
			{
				goto label_mtn_tune_axis_one_by_one_para;
			}
		}
		stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara.dAccFFC;
		if(stCurrTuneParaLowBound.stMtnPara.dAccFFC < (stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward - RESCALE_FACTOR_PREV_STEP_SIZE * dStepAccFFC))
		{
			stCurrTuneParaLowBound.stMtnPara.dAccFFC = stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward - RESCALE_FACTOR_PREV_STEP_SIZE * dStepAccFFC;
		}
		if(stCurrTuneParaUppBound.stMtnPara.dAccFFC > (stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward + RESCALE_FACTOR_PREV_STEP_SIZE * dStepAccFFC))
		{
			stCurrTuneParaUppBound.stMtnPara.dAccFFC = (stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward + RESCALE_FACTOR_PREV_STEP_SIZE * dStepAccFFC);
		}
		dStepAccFFC = (stCurrTuneParaUppBound.stMtnPara.dAccFFC - stCurrTuneParaLowBound.stMtnPara.dAccFFC)/(NUM_POINTS_ONE_DIM_MESH - 1);
		if(fabs(dStepAccFFC) > TUNE_PARA_STOP_RANGE_TABLE_XY_AFF)
		{
			nActualPointsTuneAccFF = NUM_POINTS_ONE_DIM_MESH;
			for(ii =0; ii<NUM_POINTS_ONE_DIM_MESH; ii++)
			{
				adParaAccFFC[ii] = stCurrTuneParaLowBound.stMtnPara.dAccFFC + dStepAccFFC * ii;
			}
		}
		else
		{
			nActualPointsTuneAccFF = 0;
		}

		//// VKI
		for(jj = 0; jj<nActualPointsTuneVKI; jj++)
		{
			stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain = adParaVelKI[jj];
			mtnapi_download_servo_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &(stAxisServoCtrlParaAtTuningACS));
			Sleep(10);

			mtn_tune_start_2_points_move_with_data_collect();
			// Calculate time index
			mtn_tune_calc_time_idx_2_loop_move(&stMtnTimeIdx2LoopMove);
			// Calculate servo tuning index
			mtn_tune_calc_servo_perform_index(fThresholdSettleTime, &stMtnTimeIdx2LoopMove, &stMtnServoTuneIndex);

			astMotionTuneHistory[idxCurrCase1By1].dTuningObj = CalculateObjByThemeIndex(stpTuningTheme, &stMtnServoTuneIndex);
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dVelLoopKP = stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dVelLoopKI = stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dAccFFC = stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dPosnLoopKP = stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoTuneIndex = stMtnServoTuneIndex;

			if(dBestTuneObj1By1 > astMotionTuneHistory[idxCurrCase1By1].dTuningObj)
			{
					dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
					idxBestParaSet = idxCurrCase1By1;
			}
			idxCurrCase1By1++;

			if(idxCurrCase1By1 >= nTotalCases1By1 || cFlagStopTuneCompleteMesh == TRUE || qc_is_axis_not_safe(stCommHandle, iAxisTuningACS))
			{
				goto label_mtn_tune_axis_one_by_one_para;
			}
		}
		stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara.dVelLoopKI;
		if(stCurrTuneParaLowBound.stMtnPara.dVelLoopKI < (stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain - RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKI))
		{
			stCurrTuneParaLowBound.stMtnPara.dVelLoopKI = stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain - RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKI;
		}
		if(stCurrTuneParaUppBound.stMtnPara.dVelLoopKI > (stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain + RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKI))
		{
			stCurrTuneParaUppBound.stMtnPara.dVelLoopKI = (stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain + RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKI);
		}

		dStepVelKI = (stCurrTuneParaUppBound.stMtnPara.dVelLoopKI - stCurrTuneParaLowBound.stMtnPara.dVelLoopKI)/(NUM_POINTS_ONE_DIM_MESH - 1);
		if(fabs(dStepVelKI) > TUNE_PARA_STOP_RANGE_TABLE_XY_VKI)
		{
			nActualPointsTuneVKI = NUM_POINTS_ONE_DIM_MESH;
			for(ii =0; ii<NUM_POINTS_ONE_DIM_MESH; ii++)
			{
				adParaVelKI[ii] = stCurrTuneParaLowBound.stMtnPara.dVelLoopKI + dStepVelKI * ii;
			}
		}
		else
		{
			nActualPointsTuneVKI = 0;
		}

		// PosnKP
		for(ll = 0; ll<nActualPointsTunePKP; ll++)
		{
			stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain = adParaPosnKP[ll];
			mtnapi_download_servo_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &(stAxisServoCtrlParaAtTuningACS));
			Sleep(10);

			mtn_tune_start_2_points_move_with_data_collect();
			// Calculate time index
			mtn_tune_calc_time_idx_2_loop_move(&stMtnTimeIdx2LoopMove);
			// Calculate servo tuning index
			mtn_tune_calc_servo_perform_index(fThresholdSettleTime, &stMtnTimeIdx2LoopMove, &stMtnServoTuneIndex);

			astMotionTuneHistory[idxCurrCase1By1].dTuningObj = CalculateObjByThemeIndex(stpTuningTheme, &stMtnServoTuneIndex);
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dVelLoopKP = stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dVelLoopKI = stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dAccFFC = stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dPosnLoopKP = stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoTuneIndex = stMtnServoTuneIndex;

			if(dBestTuneObj1By1 > astMotionTuneHistory[idxCurrCase1By1].dTuningObj)
			{
					dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
					idxBestParaSet = idxCurrCase1By1;
			}
			idxCurrCase1By1++;

			if(idxCurrCase1By1 >= nTotalCases1By1 || cFlagStopTuneCompleteMesh == TRUE || qc_is_axis_not_safe(stCommHandle, iAxisTuningACS))
			{
				goto label_mtn_tune_axis_one_by_one_para;
			}
		}
		stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara.dPosnLoopKP;
		if(stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP < (stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain - RESCALE_FACTOR_PREV_STEP_SIZE * dStepPosnKP))
		{
			stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP = stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain - RESCALE_FACTOR_PREV_STEP_SIZE * dStepPosnKP;
		}
		if(stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP > (stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain + RESCALE_FACTOR_PREV_STEP_SIZE * dStepPosnKP))
		{
			stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP = (stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain + RESCALE_FACTOR_PREV_STEP_SIZE * dStepPosnKP);
		}
		dStepPosnKP = (stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP - stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP)/(NUM_POINTS_ONE_DIM_MESH - 1);
		if(fabs(dStepPosnKP) > TUNE_PARA_STOP_RANGE_TABLE_XY_PKP)
		{
			nActualPointsTunePKP = NUM_POINTS_ONE_DIM_MESH;
			for(ii =0; ii<NUM_POINTS_ONE_DIM_MESH; ii++)
			{
				adParaPosnKP[ii] = stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP + dStepPosnKP * ii;
			}
		}
		else
		{
			nActualPointsTunePKP = 0;
		}

		// End one loop
		iCurrLoop ++;
	}

label_mtn_tune_axis_one_by_one_para:

// 	AfxMessageBox(_T("Before ending"));

	stpMtnTuneOutput->dTuningObj = dBestTuneObj1By1;

//	stpMtnTuneOutput->stBestParameterSet = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara;
	stpMtnTuneOutput->stResultResponse = astMotionTuneHistory[idxBestParaSet].stServoTuneIndex;
	if(iAxisTuningACS == APP_X_TABLE_ACS_ID|| iAxisTuningACS == APP_Y_TABLE_ACS_ID)
	{
		mtn_tune_round_parameter_to_nearest_10(&astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara, &stpMtnTuneOutput->stBestParameterSet);
	}
	else if (iAxisTuningACS ==  sys_get_acs_axis_id_bnd_z())
	{
		mtn_tune_round_parameter_to_nearest_1(&astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara, &stpMtnTuneOutput->stBestParameterSet);
	}

	if(stpMtnTuneInput->iDebugFlag == 1)
	{
		stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain = stpMtnTuneOutput->stBestParameterSet.dVelLoopKP;
		stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain = stpMtnTuneOutput->stBestParameterSet.dVelLoopKI;
		stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain = stpMtnTuneOutput->stBestParameterSet.dPosnLoopKP;
		stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward = stpMtnTuneOutput->stBestParameterSet.dAccFFC;
		mtnapi_download_servo_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &(stAxisServoCtrlParaAtTuningACS));
		Sleep(100);

		mtn_tune_start_2_points_move_with_data_collect();
		// Calculate time index
		mtn_tune_calc_time_idx_2_loop_move(&stMtnTimeIdx2LoopMove);
		// Calculate servo tuning index
		mtn_tune_calc_servo_perform_index(fThresholdSettleTime, &stMtnTimeIdx2LoopMove, &stMtnServoTuneIndex);

		stpMtnTuneOutput->dTuningObj = CalculateObjByThemeIndex(stpTuningTheme, &stMtnServoTuneIndex);
	

		_time64(&stLongTime);
		_localtime64_s(stpTime, &stLongTime);
		sprintf_s(strMoveWaveformFilename, 128, "%s\\TuneOut_Ctrl_%d_%d%d%d_%d%d%d_wvfm.m", 
			strWbTuningNewFolderName, iAxisTuningACS, stpTime->tm_year +1900, stpTime->tm_mon +1, 
					stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, stpTime->tm_sec);  // 20120626

		mtn_save_curr_axis_traj_data_to_filename(strMoveWaveformFilename, iAxisTuningACS); // 
	}

	// Tuning pass (iFlagTuningIsFail = FALSE ) iff the obj is better than initial value  <=> (dTuningObj < dInitialObj)
	if(stpMtnTuneOutput->dTuningObj < stpMtnTuneOutput->dInitialObj)
	{
		stpMtnTuneOutput->iFlagTuningIsFail = FALSE;
	}
	else
	{
		stpMtnTuneOutput->iFlagTuningIsFail = TRUE;
	}

	mtnapi_download_safety_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stSafetyParaBak);

	if(stpMtnTuneInput->iDebugFlag == TRUE)
	{
		mtn_tune_save_tuning_history(stpMtnTuneInput, stpMtnTuneOutput, idxCurrCase1By1);
	}

	mtnapi_download_servo_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stAxisServoCtrlParaBak);
	return sRet;

}


void CMtnTune::mtn_tune_start_2_points_move_with_data_collect()
{
	// Start Data Capture
#ifdef __PREV__
	if (!acsc_CollectB(Handle, 0, // system data collection
			gstrScopeArrayName, // name of data collection array
			gstSystemScope.uiDataLen, // number of samples to be collected
			1, // sampling period 1 millisecond
			strACSC_VarName, // variables to be collected
			NULL)
		)
#else
	if (!acsc_CollectB(Handle, 0, // system data collection
			"TuningScopeData(5)(2000)", // name of data collection array
			2000, // number of samples to be collected
			1, // sampling period 1 millisecond
			strACSC_VarName, // variables to be collected
			NULL)
		)
#endif
	{
		//printf("Collecting data, transaction error: %d\n", acsc_GetLastError());
//				sRet = MTN_API_ERROR; // error happens
	}
// for each set of parameter
	// Move Servo-Axis, Front-Back 2 times	// Upload data, Calculate index
#ifdef __DEBUG__
	FILE * fptr;
    fopen_s(&fptr, "TuneDbg2.txt", "w");
	fprintf(fptr, "iAxisTuning = %d\n", iAxisTuning);
	fprintf(fptr, "iMovePoint1 = %d\n", iMovePoint1);
	fprintf(fptr, "iMovePoint2 = %d\n", iMovePoint2);
	fclose(fptr);
#endif  // __DEBUG__
	OneAxis2PointsMoveOneCycle();
	OneAxis2PointsMoveOneCycle();

	// Upload data
#ifdef __PREV__
	mtnscope_upload_acsc_data(Handle);
#else
	mtnscope_tuning_upload_acsc_data(Handle);
#endif
	mtn_tune_init_array_ptr();
}


// Tuning Table One after one dimension
short CMtnTune::mtn_wb_tune_axis_sldra_jerk_ffc_para(MTN_TUNE_GENETIC_INPUT *stpMtnTuneInput, MTN_TUNE_GENETIC_OUTPUT *stpMtnTuneOutput)
{
	TUNE_ALGO_SETTING       *stpTuningAlgoSetting = &stpMtnTuneInput->stTuningAlgoSetting;
	MTN_TUNE_PARA_UNION		*stpTuningParameterIni = &stpMtnTuneInput->stTuningParameterIni;
	MTN_TUNE_PARA_UNION		*stpTuningParameterUppBound = &stpMtnTuneInput->stTuningParameterUppBound;
	MTN_TUNE_PARA_UNION		*stpTuningParameterLowBound = &stpMtnTuneInput->stTuningParameterLowBound;
	MTN_TUNE_THEME			*stpTuningTheme = &stpMtnTuneInput->stTuningTheme;
	MTN_TUNE_CONDITION		*stpTuningCondition = &stpMtnTuneInput->stTuningCondition;
	MTN_TUNE_ENC_TIME_CFG	*stpTuningEncTimeConfig = &stpMtnTuneInput->stTuningEncTimeConfig;
	MTN_TUNE_SPECIFICATION  *stpTuningPassSpecfication = &stpMtnTuneInput->stTuningPassSpecfication;
	HANDLE stCommHandle = stpMtnTuneInput->stCommHandle;
	int	iAxisTuningACS = stpMtnTuneInput->iAxisTuning;
	double adParaSLDRA[NUM_POINTS_ONE_DIM_MESH], dStepSLDRA;
	double adParaSLDRX[NUM_POINTS_ONE_DIM_MESH], dStepSLDRX;
	double adParaJerkFFC[NUM_POINTS_ONE_DIM_MESH], dStepJerkFFC;
	int ii, jj, kk; // , nActualPointsTunePKP, nActualPointsTuneVKP, nActualPointsTuneAccFF, nActualPointsTuneVKI;
	int nActualPointsTuneDRA, nActualPointsTuneDRX, nActualPointsTuneJerkFFC;  // 20110515
	short sRet = MTN_API_OK_ZERO;
	int idxBestParaSet;

	CTRL_PARA_ACS stAxisServoCtrlParaBak, stAxisServoCtrlParaAtTuningACS;
	// Bakup safety parameter, and enlarge the parameter range
	SAFETY_PARA_ACS stSafetyParaBak, stSafetyParaCurr;
	mtnapi_upload_safety_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stSafetyParaBak);
	stSafetyParaCurr = stSafetyParaBak;
	stSafetyParaCurr.dCriticalPosnErrAcc = 5000;
	stSafetyParaCurr.dCriticalPosnErrIdle = 1000;	
	stSafetyParaCurr.dCriticalPosnErrVel = 1000;
	stSafetyParaCurr.dRMS_DrvCmdX = stSafetyParaCurr.dRMS_DrvCmdX * 2;
	stSafetyParaCurr.dRMS_DrvCmdIdle = stSafetyParaCurr.dRMS_DrvCmdIdle * 2;
	stSafetyParaCurr.dRMS_DrvCmdMtn = 100;  // 20120717, must release to 100% for motion tuning
	mtnapi_download_safety_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stSafetyParaCurr);

	cFlagStopTuneCompleteMesh = FALSE;
	mtnapi_upload_servo_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stAxisServoCtrlParaBak);
	//Clear the tuning history buffer
	memset(astMotionTuneHistory, 0, sizeof(MTN_TUNE_CASE) * MAX_CASE_TUNING_HISTORY_BUFFER);

	stAxisServoCtrlParaAtTuningACS = stAxisServoCtrlParaBak;
	stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain = stpTuningParameterLowBound->stMtnPara.dPosnLoopKP;

	MTN_TUNE_PARA_UNION	stCurrTuneParaUppBound, stCurrTuneParaLowBound;

	stCurrTuneParaUppBound.stMtnPara = stpTuningParameterUppBound->stMtnPara;
	stCurrTuneParaLowBound.stMtnPara = stpTuningParameterLowBound->stMtnPara;
	dStepSLDRA = (stCurrTuneParaUppBound.stMtnPara.dServoLoopDRA - stCurrTuneParaLowBound.stMtnPara.dServoLoopDRA)/(NUM_POINTS_ONE_DIM_MESH - 1);
	dStepSLDRX = (stCurrTuneParaUppBound.stMtnPara.dServoLoopDRX - stCurrTuneParaLowBound.stMtnPara.dServoLoopDRX)/(NUM_POINTS_ONE_DIM_MESH - 1);
	dStepJerkFFC = (stCurrTuneParaUppBound.stMtnPara.dJerkFf - stCurrTuneParaLowBound.stMtnPara.dJerkFf)/(NUM_POINTS_ONE_DIM_MESH - 1);  // 20110515

	for(ii =0; ii<NUM_POINTS_ONE_DIM_MESH; ii++)
	{
		adParaSLDRA[ii] = stCurrTuneParaLowBound.stMtnPara.dServoLoopDRA + dStepSLDRA * ii;
		adParaSLDRX[ii] = stCurrTuneParaLowBound.stMtnPara.dServoLoopDRX + dStepSLDRX * ii;
		adParaJerkFFC[ii] = stCurrTuneParaLowBound.stMtnPara.dJerkFf + dStepJerkFFC * ii; // 20110515
	}
	// Set upload variables
	mtnscope_set_acsc_var_collecting_move(iAxisTuningACS);

double fThresholdSettleTime; //  = stpTuningEncTimeConfig->dSettleThreshold_um / stpTuningEncTimeConfig->dEncCntUnit_um;
int iInterMoveDelay = stpTuningCondition->iIterMotionDelay_ms;
MTN_TIME_IDX_2_LOOP_MOVE stMtnTimeIdx2LoopMove;
MTN_SERVO_TUNE_INDEX  stMtnServoTuneIndex;

	if(stpTuningEncTimeConfig->dEncCntUnit_um > 1E-6)
	{
		fThresholdSettleTime = 10;
	}
	else
	{
		fThresholdSettleTime = stpTuningEncTimeConfig->dSettleThreshold_um / stpTuningEncTimeConfig->dEncCntUnit_um;
	}
	mtn_tune_initialize_class_var(fThresholdSettleTime, stCommHandle, iAxisTuningACS, iInterMoveDelay);
	iMovePoint1 = (int)stpTuningCondition->dMovePosition_1;
	iMovePoint2 = (int)stpTuningCondition->dMovePointion_2;
	OneAxis2PointsMoveOneCycle(); // Move once, get ready to move to 1st point
	// Move to get initial performance
	mtn_tune_start_2_points_move_with_data_collect();
	// Calculate time index
	mtn_tune_calc_time_idx_2_loop_move(&stMtnTimeIdx2LoopMove);
	// Calculate servo tuning index
	mtn_tune_calc_servo_perform_index(fThresholdSettleTime, &stMtnTimeIdx2LoopMove, &stMtnServoTuneIndex);
	stpMtnTuneOutput->stInitialResponse = stMtnServoTuneIndex;
	stpMtnTuneOutput->dInitialObj = CalculateObjByThemeIndex_DRA(stpTuningTheme, &stMtnServoTuneIndex);

	static char strMoveWaveformFilename[512];
	struct tm stTime;
	struct tm *stpTime = &stTime;
	__time64_t stLongTime;

	if(stpMtnTuneInput->iDebugFlag == 1)
	{
		_time64(&stLongTime);
		_localtime64_s(stpTime, &stLongTime);
		sprintf_s(strMoveWaveformFilename, 512, "%s\\TuneDRA_Init_Ctrl_%d_%d%d%d_%d%d%d_wvfm.m", 
			strWbTuningNewFolderName,   // 20120613
			iAxisTuningACS, stpTime->tm_year +1900, stpTime->tm_mon +1, 
					stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, stpTime->tm_sec);

		mtn_save_curr_axis_traj_data_to_filename(strMoveWaveformFilename, iAxisTuningACS); // 
	}

	idxCurrCase1By1 = idxBestParaSet = 0;
	nTotalCases1By1 = stpTuningAlgoSetting->uiMaxGeneration * 2 * NUM_POINTS_ONE_DIM_MESH; //10000; // NUM_POINTS_ONE_DIM_MESH * NUM_POINTS_ONE_DIM_MESH * NUM_POINTS_ONE_DIM_MESH * NUM_POINTS_ONE_DIM_MESH;
	if(nTotalCases1By1 > MAX_CASE_TUNING_HISTORY_BUFFER)
	{
		nTotalCases1By1 = MAX_CASE_TUNING_HISTORY_BUFFER;
	}


unsigned int iCurrLoop =0;
	//nActualPointsTunePKP = nActualPointsTuneVKP = nActualPointsTuneAccFF = nActualPointsTuneVKI = NUM_POINTS_ONE_DIM_MESH;
	nActualPointsTuneDRA = nActualPointsTuneDRX = nActualPointsTuneJerkFFC = NUM_POINTS_ONE_DIM_MESH;  // 20110515
	// set SLDRX to be the middle for the initial point
	stAxisServoCtrlParaAtTuningACS.dDRX = (stCurrTuneParaUppBound.stMtnPara.dServoLoopDRX + stCurrTuneParaLowBound.stMtnPara.dServoLoopDRX)/2.0;
	stAxisServoCtrlParaAtTuningACS.dJerkFf = (stCurrTuneParaUppBound.stMtnPara.dJerkFf + stCurrTuneParaLowBound.stMtnPara.dJerkFf)/2.0;  // 20110515
	
	while( (nActualPointsTuneDRA >=1  || nActualPointsTuneDRX >= 1 || nActualPointsTuneJerkFFC >=1) //( (nActualPointsTunePKP >= 1 || nActualPointsTuneVKP >= 1 || nActualPointsTuneAccFF >= 1 || nActualPointsTuneVKI >= 1) 
		&& (idxCurrCase1By1 < nTotalCases1By1) && (iCurrLoop < stpTuningAlgoSetting->uiMaxGeneration) )
	{
		// DRA
		for(ii = 0; ii<nActualPointsTuneDRA; ii++)
		{
			stAxisServoCtrlParaAtTuningACS.dDRA = adParaSLDRA[ii];

			mtnapi_download_servo_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &(stAxisServoCtrlParaAtTuningACS));
			Sleep(10);

			mtn_tune_start_2_points_move_with_data_collect();
			// Calculate time index
			mtn_tune_calc_time_idx_2_loop_move(&stMtnTimeIdx2LoopMove);
			// Calculate servo tuning index
			mtn_tune_calc_servo_perform_index(fThresholdSettleTime, &stMtnTimeIdx2LoopMove, &stMtnServoTuneIndex);

			astMotionTuneHistory[idxCurrCase1By1].dTuningObj = CalculateObjByThemeIndex_DRA(stpTuningTheme, &stMtnServoTuneIndex);
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dVelLoopKP = stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dVelLoopKI = stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dAccFFC = stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dPosnLoopKP = stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dServoLoopDRA = stAxisServoCtrlParaAtTuningACS.dDRA;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dServoLoopDRX = stAxisServoCtrlParaAtTuningACS.dDRX;
			astMotionTuneHistory[idxCurrCase1By1].stServoTuneIndex = stMtnServoTuneIndex;

			if(idxCurrCase1By1 ==0)
			{
				dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
				idxBestParaSet = idxCurrCase1By1;
			}
			else
			{
				if(dBestTuneObj1By1 > astMotionTuneHistory[idxCurrCase1By1].dTuningObj)
				{
					dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
					idxBestParaSet = idxCurrCase1By1;
				}
			}
			idxCurrCase1By1++;

			if(idxCurrCase1By1 >= nTotalCases1By1 || cFlagStopTuneCompleteMesh == TRUE || qc_is_axis_not_safe(stCommHandle, iAxisTuningACS))
			{
				goto label_mtn_tune_axis_one_by_one_para;
			}
		}
		stAxisServoCtrlParaAtTuningACS.dDRA = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara.dServoLoopDRA;
		if(stCurrTuneParaLowBound.stMtnPara.dServoLoopDRA < (stAxisServoCtrlParaAtTuningACS.dDRA - RESCALE_FACTOR_PREV_STEP_SIZE * dStepSLDRA))
		{
			stCurrTuneParaLowBound.stMtnPara.dServoLoopDRA = stAxisServoCtrlParaAtTuningACS.dDRA - RESCALE_FACTOR_PREV_STEP_SIZE * dStepSLDRA;
		}
		if(stCurrTuneParaUppBound.stMtnPara.dServoLoopDRA > (stAxisServoCtrlParaAtTuningACS.dDRA + RESCALE_FACTOR_PREV_STEP_SIZE * dStepSLDRA))
		{
			stCurrTuneParaUppBound.stMtnPara.dServoLoopDRA = (stAxisServoCtrlParaAtTuningACS.dDRA + RESCALE_FACTOR_PREV_STEP_SIZE * dStepSLDRA);
		}

		dStepSLDRA = (stCurrTuneParaUppBound.stMtnPara.dServoLoopDRA - stCurrTuneParaLowBound.stMtnPara.dServoLoopDRA)/(NUM_POINTS_ONE_DIM_MESH - 1);
		if(fabs(dStepSLDRA) > 1)
		{
			nActualPointsTuneDRA = NUM_POINTS_ONE_DIM_MESH;
			for(ii =0; ii<NUM_POINTS_ONE_DIM_MESH; ii++)
			{
				adParaSLDRA[ii] = stCurrTuneParaLowBound.stMtnPara.dServoLoopDRA  + dStepSLDRA * ii;
			}
		}
		else
		{
			nActualPointsTuneDRA = 0;
		}

		// DRX
		for(jj = 0; jj<nActualPointsTuneDRX; jj++)
		{
			stAxisServoCtrlParaAtTuningACS.dDRX = adParaSLDRX[jj];

			mtnapi_download_servo_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &(stAxisServoCtrlParaAtTuningACS));
			Sleep(10);

			mtn_tune_start_2_points_move_with_data_collect();
			// Calculate time index
			mtn_tune_calc_time_idx_2_loop_move(&stMtnTimeIdx2LoopMove);
			// Calculate servo tuning index
			mtn_tune_calc_servo_perform_index(fThresholdSettleTime, &stMtnTimeIdx2LoopMove, &stMtnServoTuneIndex);

			astMotionTuneHistory[idxCurrCase1By1].dTuningObj = CalculateObjByThemeIndex_DRA(stpTuningTheme, &stMtnServoTuneIndex);
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dVelLoopKP = stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dVelLoopKI = stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dAccFFC = stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dPosnLoopKP = stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dServoLoopDRA = stAxisServoCtrlParaAtTuningACS.dDRA;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dServoLoopDRX = stAxisServoCtrlParaAtTuningACS.dDRX;
			astMotionTuneHistory[idxCurrCase1By1].stServoTuneIndex = stMtnServoTuneIndex;

			if(idxCurrCase1By1 ==0)
			{
				dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
				idxBestParaSet = idxCurrCase1By1;
			}
			else
			{
				if(dBestTuneObj1By1 > astMotionTuneHistory[idxCurrCase1By1].dTuningObj)
				{
					dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
					idxBestParaSet = idxCurrCase1By1;
				}
			}
			idxCurrCase1By1++;

			if(idxCurrCase1By1 >= nTotalCases1By1 || cFlagStopTuneCompleteMesh == TRUE || qc_is_axis_not_safe(stCommHandle, iAxisTuningACS))
			{
				goto label_mtn_tune_axis_one_by_one_para;
			}
		}
		stAxisServoCtrlParaAtTuningACS.dDRX = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara.dServoLoopDRX;
		if(stCurrTuneParaLowBound.stMtnPara.dServoLoopDRX < (stAxisServoCtrlParaAtTuningACS.dDRX - RESCALE_FACTOR_PREV_STEP_SIZE * dStepSLDRX))
		{
			stCurrTuneParaLowBound.stMtnPara.dServoLoopDRX = stAxisServoCtrlParaAtTuningACS.dDRX - RESCALE_FACTOR_PREV_STEP_SIZE * dStepSLDRX;
		}
		if(stCurrTuneParaUppBound.stMtnPara.dServoLoopDRX > (stAxisServoCtrlParaAtTuningACS.dDRX + RESCALE_FACTOR_PREV_STEP_SIZE * dStepSLDRX))
		{
			stCurrTuneParaUppBound.stMtnPara.dServoLoopDRX = (stAxisServoCtrlParaAtTuningACS.dDRX + RESCALE_FACTOR_PREV_STEP_SIZE * dStepSLDRX);
		}

		dStepSLDRX = (stCurrTuneParaUppBound.stMtnPara.dServoLoopDRX - stCurrTuneParaLowBound.stMtnPara.dServoLoopDRX)/(NUM_POINTS_ONE_DIM_MESH - 1);
		if(fabs(dStepSLDRX) > 10)
		{
			nActualPointsTuneDRX = NUM_POINTS_ONE_DIM_MESH;
			for(jj =0; jj<NUM_POINTS_ONE_DIM_MESH; jj++)
			{
				adParaSLDRX[jj] = stCurrTuneParaLowBound.stMtnPara.dServoLoopDRX  + dStepSLDRX * jj;
			}
		}
		else
		{
			nActualPointsTuneDRX = 0;
		}

		// JerkFFC, 20110515
		for(kk = 0; kk<nActualPointsTuneJerkFFC; kk++)
		{
			stAxisServoCtrlParaAtTuningACS.dJerkFf = adParaJerkFFC[kk];

			mtnapi_download_servo_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &(stAxisServoCtrlParaAtTuningACS));
			Sleep(10);

			mtn_tune_start_2_points_move_with_data_collect();
			// Calculate time index
			mtn_tune_calc_time_idx_2_loop_move(&stMtnTimeIdx2LoopMove);
			// Calculate servo tuning index
			mtn_tune_calc_servo_perform_index(fThresholdSettleTime, &stMtnTimeIdx2LoopMove, &stMtnServoTuneIndex);

			astMotionTuneHistory[idxCurrCase1By1].dTuningObj = CalculateObjByThemeIndex_DRA(stpTuningTheme, &stMtnServoTuneIndex);
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dVelLoopKP = stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dVelLoopKI = stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dAccFFC = stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dPosnLoopKP = stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dServoLoopDRA = stAxisServoCtrlParaAtTuningACS.dDRA;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dServoLoopDRX = stAxisServoCtrlParaAtTuningACS.dDRX;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara.dJerkFf = stAxisServoCtrlParaAtTuningACS.dJerkFf; // 20110515
			astMotionTuneHistory[idxCurrCase1By1].stServoTuneIndex = stMtnServoTuneIndex;

			if(idxCurrCase1By1 ==0)
			{
				dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
				idxBestParaSet = idxCurrCase1By1;
			}
			else
			{
				if(dBestTuneObj1By1 > astMotionTuneHistory[idxCurrCase1By1].dTuningObj)
				{
					dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
					idxBestParaSet = idxCurrCase1By1;
				}
			}
			idxCurrCase1By1++;

			if(idxCurrCase1By1 >= nTotalCases1By1 || cFlagStopTuneCompleteMesh == TRUE || qc_is_axis_not_safe(stCommHandle, iAxisTuningACS))
			{
				goto label_mtn_tune_axis_one_by_one_para;
			}
		}
		stAxisServoCtrlParaAtTuningACS.dJerkFf = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara.dJerkFf;
		if(stCurrTuneParaLowBound.stMtnPara.dJerkFf < (stAxisServoCtrlParaAtTuningACS.dJerkFf - RESCALE_FACTOR_PREV_STEP_SIZE * dStepJerkFFC))
		{
			stCurrTuneParaLowBound.stMtnPara.dJerkFf = stAxisServoCtrlParaAtTuningACS.dJerkFf - RESCALE_FACTOR_PREV_STEP_SIZE * dStepJerkFFC;
		}
		if(stCurrTuneParaLowBound.stMtnPara.dJerkFf < 0)
		{
			stCurrTuneParaLowBound.stMtnPara.dJerkFf = 0;
		}
		if(stCurrTuneParaUppBound.stMtnPara.dJerkFf > (stAxisServoCtrlParaAtTuningACS.dJerkFf + RESCALE_FACTOR_PREV_STEP_SIZE * dStepJerkFFC))
		{
			stCurrTuneParaUppBound.stMtnPara.dJerkFf = (stAxisServoCtrlParaAtTuningACS.dJerkFf + RESCALE_FACTOR_PREV_STEP_SIZE * dStepJerkFFC);
		}

		dStepJerkFFC = (stCurrTuneParaUppBound.stMtnPara.dJerkFf - stCurrTuneParaLowBound.stMtnPara.dJerkFf)/(NUM_POINTS_ONE_DIM_MESH - 1);
		if(fabs(dStepJerkFFC) > 10)
		{
			nActualPointsTuneJerkFFC = NUM_POINTS_ONE_DIM_MESH;
			for(kk =0; kk<NUM_POINTS_ONE_DIM_MESH; kk++)
			{
				adParaJerkFFC[kk] = stCurrTuneParaLowBound.stMtnPara.dJerkFf  + dStepJerkFFC * kk;
			}
		}
		else
		{
			nActualPointsTuneJerkFFC = 0;
		}

		// End one loop
		iCurrLoop ++;
	}

label_mtn_tune_axis_one_by_one_para:

	stpMtnTuneOutput->dTuningObj = dBestTuneObj1By1;

	stpMtnTuneOutput->stBestParameterSet = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara;
	stpMtnTuneOutput->stResultResponse = astMotionTuneHistory[idxBestParaSet].stServoTuneIndex;
	if(iAxisTuningACS == APP_X_TABLE_ACS_ID|| iAxisTuningACS == APP_Y_TABLE_ACS_ID)
	{
		mtn_tune_round_parameter_to_nearest_10(&astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara, &stpMtnTuneOutput->stBestParameterSet);
	}
	else if (iAxisTuningACS ==  sys_get_acs_axis_id_bnd_z())
	{
		mtn_tune_round_parameter_to_nearest_1(&astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara, &stpMtnTuneOutput->stBestParameterSet);
	}

	if(stpMtnTuneInput->iDebugFlag == 1)
	{
		stAxisServoCtrlParaAtTuningACS.dVelocityLoopProportionalGain = stpMtnTuneOutput->stBestParameterSet.dVelLoopKP;
		stAxisServoCtrlParaAtTuningACS.dVelocityLoopIntegratorGain = stpMtnTuneOutput->stBestParameterSet.dVelLoopKI;
		stAxisServoCtrlParaAtTuningACS.dPositionLoopProportionalGain = stpMtnTuneOutput->stBestParameterSet.dPosnLoopKP;
		stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward = stpMtnTuneOutput->stBestParameterSet.dAccFFC;
		stAxisServoCtrlParaAtTuningACS.dDRA = stpMtnTuneOutput->stBestParameterSet.dServoLoopDRA;
		stAxisServoCtrlParaAtTuningACS.dDRX = stpMtnTuneOutput->stBestParameterSet.dServoLoopDRX;
		mtnapi_download_servo_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &(stAxisServoCtrlParaAtTuningACS));
		Sleep(100);

		mtn_tune_start_2_points_move_with_data_collect();
		// Calculate time index
		mtn_tune_calc_time_idx_2_loop_move(&stMtnTimeIdx2LoopMove);
		// Calculate servo tuning index
		mtn_tune_calc_servo_perform_index(fThresholdSettleTime, &stMtnTimeIdx2LoopMove, &stMtnServoTuneIndex);

		stpMtnTuneOutput->dTuningObj = CalculateObjByThemeIndex_DRA(stpTuningTheme, &stMtnServoTuneIndex);
	

		_time64(&stLongTime);
		_localtime64_s(stpTime, &stLongTime);
		sprintf_s(strMoveWaveformFilename, 128, "%s\\TuneOutDRA_Ctrl_%d_%d%d%d_%d%d%d_wvfm.m", 
			strWbTuningNewFolderName,   // 20120613
			iAxisTuningACS, stpTime->tm_year +1900, stpTime->tm_mon +1, 
					stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, stpTime->tm_sec);

		mtn_save_curr_axis_traj_data_to_filename(strMoveWaveformFilename, iAxisTuningACS); // 
	}

	// Tuning pass (iFlagTuningIsFail = FALSE ) iff the obj is better than initial value  <=> (dTuningObj < dInitialObj)
	if(stpMtnTuneOutput->dTuningObj < stpMtnTuneOutput->dInitialObj)
	{
		stpMtnTuneOutput->iFlagTuningIsFail = FALSE;
	}
	else
	{
		stpMtnTuneOutput->iFlagTuningIsFail = TRUE;
	}

	mtnapi_download_safety_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stSafetyParaBak);

	if(stpMtnTuneInput->iDebugFlag == TRUE)
	{
		mtn_tune_save_tuning_history(stpMtnTuneInput, stpMtnTuneOutput, idxCurrCase1By1);
	}

	stAxisServoCtrlParaAtTuningACS.dDRA = stpMtnTuneOutput->stBestParameterSet.dServoLoopDRA;
	stAxisServoCtrlParaAtTuningACS.dDRX = stpMtnTuneOutput->stBestParameterSet.dServoLoopDRX;
	mtnapi_download_servo_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stAxisServoCtrlParaAtTuningACS);  //stAxisServoCtrlParaBak);
	return sRet;

}
#ifdef  __TBA__
#endif // __TBA__