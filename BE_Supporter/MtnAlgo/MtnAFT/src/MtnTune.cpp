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


// 20101015  Consider RMS of static position error, 
#include "stdafx.h"

#include "MtnTune.h"

#include "math.h"
#include "MtnInitAcs.h"

static	double *afFeedPosn;
static	double *afRefPosn;
static	double *afFeedVel;
static	double *afRefVel;
static	double *afDac;
static	double *afPosnErr;

static	double afDataBuffer[MAX_DATA_LEN_CAPTURE_IN_TUNING];

char *strTuningOption[] = {
	"NULL",
	"ACC_FF",
	"VEL_LOOP",
	"POSN LOOP"
};

char *strTuningStageGroupThread[] = {
	"AFF_OK_Ret",
	"AFF_GetRge",
	"AFF_FnSrch",
	"AFF_Err_Ret",
	"Err_Init_Zero"
};

extern MTN_SCOPE gstSystemScope;
extern double gdScopeCollectData[];

CMtnTune mMotionTuning;

// to resolved externals
IMPLEMENT_DYNAMIC(CMtnTune, CObject)

//extern MTN_TUNE_GENETIC_INPUT astMtnTuneInput[MAX_SERVO_AXIS_WIREBOND][MAX_BLK_PARAMETER]; 
//
//extern MTN_TUNE_GENETIC_INPUT astMtnTuneSectorInputTableX[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];
//extern MTN_TUNE_GENETIC_INPUT astMtnTuneSectorInputTableY[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];

//extern MTN_TUNE_GENETIC_OUTPUT astMtnTuneOutput[MAX_SERVO_AXIS_WIREBOND][MAX_BLK_PARAMETER];
//extern MTN_TUNE_GENETIC_OUTPUT astMtnTuneSectorOutputTableX[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];
//extern MTN_TUNE_GENETIC_OUTPUT astMtnTuneSectorOutputTableY[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];



short mtn_tune_init_array_ptr()
{
	short sRet = MTN_API_OK_ZERO;
	int nDataLen = gstSystemScope.uiDataLen;

	if(nDataLen > MAX_DATA_LEN_CAPTURE_IN_TUNING)
	{
		afFeedPosn = (double*) calloc(nDataLen, sizeof(double));
		if(afFeedPosn == NULL)
		{
			sRet = MTN_API_ERROR_MEMORY;
			goto label_mtn_tune_init_array_ptr;
		}
	}
	else
	{
		afFeedPosn = afDataBuffer;
	}

	int ii = 0;
	afPosnErr = &gdScopeCollectData[0];
	ii += nDataLen;
	afRefPosn = &gdScopeCollectData[ii];
	ii += nDataLen;
	afFeedVel = &gdScopeCollectData[ii];
	ii += nDataLen;
	afRefVel = &gdScopeCollectData[ii];
	ii += nDataLen;
	afDac = &gdScopeCollectData[ii];

	for(ii = 0; ii< nDataLen; ii++)
	{
		afFeedPosn[ii] = afRefPosn[ii] - afPosnErr[ii];
	}

label_mtn_tune_init_array_ptr:
	return sRet;
}


short qc_is_axis_still_acc_dec(HANDLE hCommunicationHandle, int iAxis)
{
	short sRet = MTN_API_OK_ZERO;
	int iTempMotorState;
	mtnapi_get_motor_state(hCommunicationHandle, iAxis, &iTempMotorState, NULL);
	if(iTempMotorState & ACSC_MST_ACC ||
		iTempMotorState & ACSC_MST_MOVE) //
	{
		sRet = 1;
	}
	return sRet;
}

short qc_is_axis_not_safe(HANDLE hCommunicationHandle, int iAxis)
{
	short sRet = MTN_API_OK_ZERO;
	int iTempMotorState;
	mtnapi_get_motor_state(hCommunicationHandle, iAxis, &iTempMotorState, NULL);
	if(iTempMotorState & ACSC_MST_ENABLE) //
	{
		sRet = MTN_API_OK_ZERO;
	}
	else
	{
		sRet = 1;
	}
	return sRet;
}

static int iMtnErrorCodeACS;
static char strMtnErrorStrACS[2048];   // 20100305
static int iActualNumByteReceivedMtnError;
static int iCurrStrLen;
char *mtn_api_get_acs_error_string()
{
	return strMtnErrorStrACS;
}

int mtn_api_prompt_acs_error_message(HANDLE stCommHandleAcs)
{
//CString cstrTemp;
char strTemp[256];
int iRet = MTN_API_OK_ZERO;

//	cstrTemp.Format(_T("ACS Error. "));
	sprintf_s(strMtnErrorStrACS, 2048, "ACS Error. "); iCurrStrLen = (int)strlen(strMtnErrorStrACS);
	acsc_GetMotorError(stCommHandleAcs, ACSC_AXIS_X, &iMtnErrorCodeACS, NULL);
	if(iMtnErrorCodeACS != 0)
	{
		iRet |= iMtnErrorCodeACS;
		acsc_GetErrorString(stCommHandleAcs,iMtnErrorCodeACS,// error code
			strTemp,// buffer for the error explanation
			255,// available buffer length
			&iActualNumByteReceivedMtnError// number of actually received bytes
			);
		iCurrStrLen = (int)strlen(strMtnErrorStrACS);
		sprintf_s(strMtnErrorStrACS, 2048 - iCurrStrLen, "X:%d (%s), ", iMtnErrorCodeACS, strTemp); iCurrStrLen = (int)strlen(strMtnErrorStrACS); //		cstrTemp.AppendFormat("X:%d (%s), ", iMtnErrorCodeACS, strMtnErrorStrACS);
	}

	acsc_GetMotorError(stCommHandleAcs, ACSC_AXIS_Y, &iMtnErrorCodeACS, NULL);
	if(iMtnErrorCodeACS != 0)
	{
		iRet |= iMtnErrorCodeACS;
		acsc_GetErrorString(stCommHandleAcs,iMtnErrorCodeACS,// error code
			strTemp,// buffer for the error explanation
			255,// available buffer length
			&iActualNumByteReceivedMtnError// number of actually received bytes
			);
			sprintf_s(strMtnErrorStrACS, 2048 - iCurrStrLen, "Y:%d (%s), ", iMtnErrorCodeACS, strTemp); iCurrStrLen = (int)strlen(strMtnErrorStrACS); //		cstrTemp.AppendFormat("Y:%d (%s), ", iMtnErrorCodeACS, strMtnErrorStrACS);
	}

	acsc_GetMotorError(stCommHandleAcs, ACSC_AXIS_A, &iMtnErrorCodeACS, NULL);
	if(iMtnErrorCodeACS != 0)
	{
		iRet |= iMtnErrorCodeACS;
		acsc_GetErrorString(stCommHandleAcs,iMtnErrorCodeACS,// error code
			strTemp,// buffer for the error explanation
			255,// available buffer length
			&iActualNumByteReceivedMtnError// number of actually received bytes
			);
			sprintf_s(strMtnErrorStrACS, 2048 - iCurrStrLen, "Z:%d (%s), ", iMtnErrorCodeACS, strTemp); iCurrStrLen = (int)strlen(strMtnErrorStrACS); //		cstrTemp.AppendFormat("Z:%d (%s), ", iMtnErrorCodeACS, strMtnErrorStrACS);
	}
	acsc_GetMotorError(stCommHandleAcs, ACSC_AXIS_B, &iMtnErrorCodeACS, NULL);
	if(iMtnErrorCodeACS != 0)
	{
		iRet |= iMtnErrorCodeACS;
		acsc_GetErrorString(stCommHandleAcs,iMtnErrorCodeACS,// error code
			strTemp,// buffer for the error explanation
			255,// available buffer length
			&iActualNumByteReceivedMtnError// number of actually received bytes
			);
			sprintf_s(strMtnErrorStrACS, 2048 - iCurrStrLen, "W:%d (%s), ", iMtnErrorCodeACS, strTemp); iCurrStrLen = (int)strlen(strMtnErrorStrACS); //		cstrTemp.AppendFormat("W:%d (%s), ", iMtnErrorCodeACS, strMtnErrorStrACS);
	}

	for(int ii=0; ii<10; ii++)
	{
		acsc_GetProgramError(stCommHandleAcs, ii, &iMtnErrorCodeACS, NULL);
		if(iMtnErrorCodeACS != 0)
		{
//			iRet |= iMtnErrorCodeACS;
			acsc_GetErrorString(stCommHandleAcs,iMtnErrorCodeACS,// error code
				strTemp,// buffer for the error explanation
				255,// available buffer length
				&iActualNumByteReceivedMtnError// number of actually received bytes
				);
			sprintf_s(strMtnErrorStrACS, 2048 - iCurrStrLen, "Prog-%d:%d (%s), ", ii, iMtnErrorCodeACS, strTemp); iCurrStrLen = (int)strlen(strMtnErrorStrACS); //			cstrTemp.AppendFormat("Prog-%d:%d (%s), ", ii, iMtnErrorCodeACS, strMtnErrorStrACS);
		}
	}

	if(iMtnErrorCodeACS = acsc_GetLastError())
	{
		iRet |= iMtnErrorCodeACS;
		acsc_GetErrorString(stCommHandleAcs,// communication handle
			iMtnErrorCodeACS,// error code
			strTemp,// buffer for the error explanation
			255,// available buffer length
			&iActualNumByteReceivedMtnError// number of actually received bytes
			);
//			cstrTemp = _T(strMtnErrorStrACS);
			sprintf_s(strMtnErrorStrACS, 2048 - iCurrStrLen, " ErrorCode-%d (%s)", iMtnErrorCodeACS, strTemp); //			cstrTemp.AppendFormat(" ErrorCode-%d", iMtnErrorCodeACS);
	}

	return iRet;
//	AfxMessageBox(cstrTemp);  // , MB_OKCANCEL
}

void mtn_tune_calc_time_idx_2_loop_move(MTN_TIME_IDX_2_LOOP_MOVE *stpTimeIdx2LoopMove)
{

stpTimeIdx2LoopMove->idx1stMoveFrontStart = 0;
stpTimeIdx2LoopMove->idx1stMoveFrontEnd = 0;
stpTimeIdx2LoopMove->idx1stMoveBackStart = 0;
stpTimeIdx2LoopMove->idx1stMoveBackEnd = 0;
stpTimeIdx2LoopMove->idx2ndMoveFrontStart = 0;
stpTimeIdx2LoopMove->idx2ndMoveFrontEnd = 0;
stpTimeIdx2LoopMove->idx2ndMoveBackStart = 0;
stpTimeIdx2LoopMove->idx2ndMoveBackEnd = 0;
int ii, nDataLen;
nDataLen = gstSystemScope.uiDataLen;

	for( ii = 0; ii < nDataLen; ii ++)
	{
		if( stpTimeIdx2LoopMove->idx1stMoveFrontStart == 0 && fabs(afRefVel[ii]) > 1.0)
		{
			stpTimeIdx2LoopMove->idx1stMoveFrontStart = ii;
		}
		if( stpTimeIdx2LoopMove->idx1stMoveFrontStart != 0 && fabs(afRefVel[ii]) < 1.0 && stpTimeIdx2LoopMove->idx1stMoveFrontEnd == 0)
		{
			stpTimeIdx2LoopMove->idx1stMoveFrontEnd = ii;
		}
		if( stpTimeIdx2LoopMove->idx1stMoveFrontEnd != 0 && fabs(afRefVel[ii]) > 1.0 && stpTimeIdx2LoopMove->idx1stMoveBackStart == 0)
		{
			stpTimeIdx2LoopMove->idx1stMoveBackStart = ii;
		}
		if( stpTimeIdx2LoopMove->idx1stMoveBackStart != 0 && fabs(afRefVel[ii]) < 1.0 && stpTimeIdx2LoopMove->idx1stMoveBackEnd == 0)
		{
			stpTimeIdx2LoopMove->idx1stMoveBackEnd = ii;
		}

		if( stpTimeIdx2LoopMove->idx1stMoveBackEnd != 0 && fabs(afRefVel[ii]) > 1.0 && stpTimeIdx2LoopMove->idx2ndMoveFrontStart == 0)
		{
			stpTimeIdx2LoopMove->idx2ndMoveFrontStart = ii;
		}
		if( stpTimeIdx2LoopMove->idx2ndMoveFrontStart != 0 && fabs(afRefVel[ii]) < 1.0 && stpTimeIdx2LoopMove->idx2ndMoveFrontEnd == 0)
		{
			stpTimeIdx2LoopMove->idx2ndMoveFrontEnd = ii;
		}
		if( stpTimeIdx2LoopMove->idx2ndMoveFrontEnd != 0 && fabs(afRefVel[ii]) > 1.0 && stpTimeIdx2LoopMove->idx2ndMoveBackStart == 0)
		{
			stpTimeIdx2LoopMove->idx2ndMoveBackStart = ii;
		}
		if( stpTimeIdx2LoopMove->idx2ndMoveBackStart != 0 && fabs(afRefVel[ii]) < 1.0 && stpTimeIdx2LoopMove->idx2ndMoveBackEnd == 0)
		{
			stpTimeIdx2LoopMove->idx2ndMoveBackEnd = ii;
			break;
		}
	}

}

double f_sign(double dInput)
{
	if(dInput >= 0)
	{
		return 1.0;
	}
	else
	{
		return -1.0;
	}
}

void mtn_tune_calc_servo_perform_index(double fThresholdSettleTime, MTN_TIME_IDX_2_LOOP_MOVE *stpTimeIdx2LoopMove, MTN_SERVO_TUNE_INDEX *stpMtnServoTuneIndex)
{
int ii;
int nDataLen = gstSystemScope.uiDataLen;

////// Maximum Dynamic Position Error: MaxDPE
	double fMaxFrontMoveDPE = 0;
	for(ii = stpTimeIdx2LoopMove->idx1stMoveFrontStart-1; ii<stpTimeIdx2LoopMove->idx1stMoveFrontEnd; ii++)
	{
		if(fabs(afPosnErr[ii]) > fMaxFrontMoveDPE )
		{
			fMaxFrontMoveDPE = fabs(afPosnErr[ii]);
		}
	}

	for(ii = stpTimeIdx2LoopMove->idx2ndMoveFrontStart-1; ii<stpTimeIdx2LoopMove->idx2ndMoveFrontEnd; ii++)
	{
		if(fabs(afPosnErr[ii]) > fMaxFrontMoveDPE)
		{
			fMaxFrontMoveDPE = fabs(afPosnErr[ii]);
		}
	}

	stpMtnServoTuneIndex->fDirFrontMove = f_sign(afRefVel[(int)((stpTimeIdx2LoopMove->idx1stMoveFrontStart + stpTimeIdx2LoopMove->idx1stMoveFrontEnd)/2.0)]);
	stpMtnServoTuneIndex->fMaxFrontMoveDPE = fMaxFrontMoveDPE * stpMtnServoTuneIndex->fDirFrontMove;

	double fMaxBackMoveDPE = 0;
	for(ii = stpTimeIdx2LoopMove->idx1stMoveBackStart-1; ii< stpTimeIdx2LoopMove->idx1stMoveBackEnd; ii++)
	{
		if(fabs(afPosnErr[ii]) > fMaxBackMoveDPE)
		{
			fMaxBackMoveDPE = fabs(afPosnErr[ii]);
		}
	}
	for(ii = stpTimeIdx2LoopMove->idx2ndMoveBackStart; ii < stpTimeIdx2LoopMove->idx2ndMoveBackEnd; ii++)
	{
		if(fabs(afPosnErr[ii]) > fMaxBackMoveDPE)
		{
			fMaxBackMoveDPE = fabs(afPosnErr[ii]);
		}
	}
	stpMtnServoTuneIndex->fDirBackMove = f_sign(afRefVel[(int)((stpTimeIdx2LoopMove->idx1stMoveBackStart + stpTimeIdx2LoopMove->idx1stMoveBackEnd)/2.0)]);
	stpMtnServoTuneIndex->fMaxBackMoveDPE = fMaxBackMoveDPE * stpMtnServoTuneIndex->fDirBackMove;

// 	aMaxDPE = [fMaxFrontMoveDPE, fMaxBackMoveDPE]

	double fDirFrontMove = stpMtnServoTuneIndex->fDirFrontMove;
	double fDirBackMove = stpMtnServoTuneIndex->fDirBackMove;
/////// Over/Under Shoot, Settling Time
double fMaxFrontOS = 0; 
double fMaxFrontUS = 0; 
int idxMaxFrontOS = 0;
int idxMaxFrontUS = 0;
double fCmdEndFrontOUS = 0;
double fSettleTime1stFront = 0;
double fSettleTime2ndFront = 0;
double fMaxBackOS = 0; 
double fMaxBackUS = 0; 
int idxMaxBackOS = 0;
int idxMaxBackUS = 0;
double fCmdEndBackOUS = 0;
double fSettleTime1stBack = 0;
double fSettleTime2ndBack = 0;

	for( ii = stpTimeIdx2LoopMove->idx1stMoveFrontEnd; ii <stpTimeIdx2LoopMove->idx1stMoveBackStart; ii++)
	{
		if(fMaxFrontOS > fDirFrontMove * afPosnErr[ii] && (fabs(afPosnErr[ii]) > fThresholdSettleTime))
		{
			fMaxFrontOS = fDirFrontMove * afPosnErr[ii];
			idxMaxFrontOS = ii;
		}
		if(fMaxFrontUS < fDirFrontMove * afPosnErr[ii] && (fabs(afPosnErr[ii]) > fThresholdSettleTime))
		{
			fMaxFrontUS = fDirFrontMove * afPosnErr[ii];
			idxMaxFrontUS = ii;
		}
		if(fabs(afPosnErr[ii]) >= fThresholdSettleTime)
		{
			fSettleTime1stFront = ii - stpTimeIdx2LoopMove->idx1stMoveFrontEnd;
		}
	}
	if(idxMaxFrontOS == 0)
	{
		fCmdEndFrontOUS = fMaxFrontUS;
	}
	else if( idxMaxFrontUS == 0)
	{
		fCmdEndFrontOUS = fMaxFrontOS;
	}
	else
	{
		if( idxMaxFrontOS < idxMaxFrontUS)
		{
			fCmdEndFrontOUS = fMaxFrontOS;
		}
		else
		{
			fCmdEndFrontOUS = fMaxFrontUS;
		}
	}

	for( ii = stpTimeIdx2LoopMove->idx2ndMoveFrontEnd; ii <stpTimeIdx2LoopMove->idx2ndMoveBackStart; ii++)
	{
		if(fMaxFrontOS > fDirFrontMove * afPosnErr[ii] && (fabs(afPosnErr[ii]) > fThresholdSettleTime))
		{
			fMaxFrontOS = fDirFrontMove * afPosnErr[ii];
		}
		if(fMaxFrontUS < fDirFrontMove * afPosnErr[ii] && (fabs(afPosnErr[ii]) > fThresholdSettleTime))
		{
			fMaxFrontUS = fDirFrontMove * afPosnErr[ii];
		}
		if(fabs(afPosnErr[ii]) >= fThresholdSettleTime)
		{
			fSettleTime2ndFront = ii - stpTimeIdx2LoopMove->idx2ndMoveFrontEnd;
		}
	}
	// For search-delay >= 8, 20101015, must match
#define LEN_CALC_RMS_STATIC_PE   8  
double fSumSq_StaticPE = 0;
int iSettleStart = (int)(fSettleTime1stFront + stpTimeIdx2LoopMove->idx1stMoveFrontEnd);
int iLenCalcRMS_1stFront = stpTimeIdx2LoopMove->idx1stMoveBackStart - stpTimeIdx2LoopMove->idx1stMoveFrontEnd;

	for(ii = stpTimeIdx2LoopMove->idx1stMoveFrontEnd; ii < stpTimeIdx2LoopMove->idx1stMoveBackStart; ii++)
	{
		fSumSq_StaticPE = fSumSq_StaticPE + (afPosnErr[ii] * afPosnErr[ii]);
	}  // 20101015
//////  back
	for( ii = stpTimeIdx2LoopMove->idx1stMoveBackEnd; ii <stpTimeIdx2LoopMove->idx2ndMoveFrontStart; ii++)
	{
		if( fMaxBackOS > fDirBackMove * afPosnErr[ii] && (fabs(afPosnErr[ii]) > fThresholdSettleTime))
		{
			fMaxBackOS = fDirBackMove * afPosnErr[ii];
			idxMaxBackOS = ii;
		}
		if( fMaxBackUS < fDirBackMove * afPosnErr[ii] && (fabs(afPosnErr[ii]) > fThresholdSettleTime))
		{
			fMaxBackUS = fDirBackMove * afPosnErr[ii];
			idxMaxBackUS = ii;
		}
		if( fabs(afPosnErr[ii]) >= fThresholdSettleTime)
		{
			fSettleTime1stBack = ii - stpTimeIdx2LoopMove->idx1stMoveBackEnd;
		}
	}
	if( idxMaxBackOS == 0)
	{
		fCmdEndBackOUS = fMaxBackUS;
	}
	else if( idxMaxBackUS == 0)
	{
		fCmdEndBackOUS = fMaxBackOS;
	}
	else
	{
		if(idxMaxBackOS < idxMaxBackUS)
		{
			fCmdEndBackOUS = fMaxBackOS;
		}
		else
		{
			fCmdEndBackOUS = fMaxBackUS;
		}
	}

	for(ii = stpTimeIdx2LoopMove->idx2ndMoveBackEnd; ii <nDataLen; ii++)
	{
		if( fMaxBackOS > fDirBackMove * afPosnErr[ii] && (fabs(afPosnErr[ii]) > fThresholdSettleTime))
		{
			fMaxBackOS = fDirBackMove * afPosnErr[ii];
		}
		if( fMaxBackUS < fDirBackMove * afPosnErr[ii] && (fabs(afPosnErr[ii]) > fThresholdSettleTime))
		{
			fMaxBackUS = fDirBackMove * afPosnErr[ii];
		}
		if(fabs(afPosnErr[ii]) >= fThresholdSettleTime)
		{
			fSettleTime2ndBack = ii - stpTimeIdx2LoopMove->idx2ndMoveBackEnd;
		}
	}
	iSettleStart = (int)(fSettleTime2ndBack + stpTimeIdx2LoopMove->idx2ndMoveBackEnd);   // 20101015
	int iLenCalcRMS_1stBack = stpTimeIdx2LoopMove->idx2ndMoveFrontStart - stpTimeIdx2LoopMove->idx1stMoveBackEnd;

	for(ii = stpTimeIdx2LoopMove->idx1stMoveBackEnd ; ii < stpTimeIdx2LoopMove->idx2ndMoveFrontStart; ii++)
	{
		fSumSq_StaticPE = fSumSq_StaticPE + (afPosnErr[ii] * afPosnErr[ii]);
	}   // 20101015
	stpMtnServoTuneIndex->fRMS_StaticPE = sqrt(fSumSq_StaticPE / (iLenCalcRMS_1stBack + iLenCalcRMS_1stFront));

	stpMtnServoTuneIndex->fCmdEndBackOUS = fCmdEndBackOUS;
	stpMtnServoTuneIndex->fCmdEndFrontOUS = fCmdEndFrontOUS;
	stpMtnServoTuneIndex->fMaxBackOS = fMaxBackOS;
	stpMtnServoTuneIndex->fMaxBackUS = fMaxBackUS;
	stpMtnServoTuneIndex->fMaxFrontOS = fMaxFrontOS;
	stpMtnServoTuneIndex->fMaxFrontUS = fMaxFrontUS;
	stpMtnServoTuneIndex->fSettleTime1stBack = fSettleTime1stBack;
	stpMtnServoTuneIndex->fSettleTime1stFront = fSettleTime1stFront;
	stpMtnServoTuneIndex->fSettleTime2ndBack = fSettleTime2ndBack;
	stpMtnServoTuneIndex->fSettleTime2ndFront = fSettleTime2ndFront;

	stpMtnServoTuneIndex->fSettleTimeBackDir = (fSettleTime1stBack>=fSettleTime2ndBack)?(fSettleTime1stBack):(fSettleTime2ndBack);
	stpMtnServoTuneIndex->fSettleTimeFrontDir = (fSettleTime1stFront>=fSettleTime2ndFront)?(fSettleTime1stFront):(fSettleTime2ndFront);

/////// Repeatability Error
int n1stMoveLenFront = stpTimeIdx2LoopMove->idx1stMoveFrontEnd - stpTimeIdx2LoopMove->idx1stMoveFrontStart;
int n2ndMoveLenFront = stpTimeIdx2LoopMove->idx2ndMoveFrontEnd - stpTimeIdx2LoopMove->idx2ndMoveFrontStart;
int nRptLenMoveFront = (n1stMoveLenFront < n2ndMoveLenFront) ?(n1stMoveLenFront) : (n2ndMoveLenFront) ;

int n1stMoveLenBack = stpTimeIdx2LoopMove->idx1stMoveBackEnd - stpTimeIdx2LoopMove->idx1stMoveBackStart;
int n2ndMoveLenBack = stpTimeIdx2LoopMove->idx2ndMoveBackEnd - stpTimeIdx2LoopMove->idx2ndMoveBackStart;
int nRptLenMoveBack = (n1stMoveLenBack < n2ndMoveLenBack) ? (n1stMoveLenBack) : (n2ndMoveLenBack  );

double fRptErrSOS = 0;
	for(ii = 0; ii< nRptLenMoveFront; ii++)
	{
		fRptErrSOS = fRptErrSOS + pow((afPosnErr[stpTimeIdx2LoopMove->idx1stMoveFrontStart + ii - 1] - afPosnErr[stpTimeIdx2LoopMove->idx2ndMoveFrontStart + ii - 1]), 
										2.0);
	}
	for(ii = 0; ii< nRptLenMoveBack; ii++)
	{
		fRptErrSOS = fRptErrSOS + pow((afPosnErr[stpTimeIdx2LoopMove->idx1stMoveBackStart + ii - 1] - afPosnErr[stpTimeIdx2LoopMove->idx2ndMoveBackStart + ii - 1]), 
										2.0);
	}

	stpMtnServoTuneIndex->fRptErrorRMS = sqrt(fRptErrSOS / (nRptLenMoveFront + nRptLenMoveBack));

//////// RMS-DAC and Peak-DAC
double fPeakAbsDAC, fSumOfSquare, fRootMeanSquareDAC, fRMS_Over32767_DAC, fPeakPercentDAC, fDacTemp;
	fPeakAbsDAC = fabs(afDac[0]);
	fSumOfSquare = 0;
	for(ii = 0; ii< stpTimeIdx2LoopMove->idx2ndMoveBackEnd; ii++) //  1: idx2ndMoveBackEnd
	{
		fDacTemp = afDac[ii];
		fSumOfSquare = fSumOfSquare +  fDacTemp * fDacTemp;
		if (fPeakAbsDAC < fabs(fDacTemp))
		{
			fPeakAbsDAC = fabs(fDacTemp);
		}
	}
	fRootMeanSquareDAC = sqrt(fSumOfSquare/ ((double)stpTimeIdx2LoopMove->idx2ndMoveBackEnd));
	fRMS_Over32767_DAC = fRootMeanSquareDAC/MAX_SIGNED_DAC_16BIT;
	fPeakPercentDAC = fPeakAbsDAC/MAX_SIGNED_DAC_16BIT;

	stpMtnServoTuneIndex->fPeakAbsDAC = fPeakAbsDAC;
	stpMtnServoTuneIndex->fPeakPercentDAC = fPeakPercentDAC;
	stpMtnServoTuneIndex->fRootMeanSquareDAC = fRootMeanSquareDAC;
	stpMtnServoTuneIndex->fRMS_Over32767_DAC = fRMS_Over32767_DAC;

}

void mtn_tune_search_vel_loop_parameter()
{
	// Given initial value of velocity loop parameters, KP, KI, LI, SOF_FREQ 

	// Do velocity loop step-test, Front-back move for velocity step-response, upload data, calculate index
		// Get 

	// Get initial value of AFF, DOUT/ACC
	// Get maximum expected ACC
}

void CMtnTune::mtn_tune_initialize_class_var(double fThresholdSettleTime, 
								   HANDLE stAcsHandle, 
								   unsigned int uiCurrMoveAxis_ACS,
								   int iInterMoveDelay)
{
	// Motion Tuning Class
	fThresholdSettleTime = fThresholdSettleTime;
	iDelayInterMove_ms = iInterMoveDelay;
	mtn_tune_set_comm_handle(stAcsHandle);
	mtn_tune_set_tune_axis(uiCurrMoveAxis_ACS);
	stTuningTheme.fWeightDiffDPE = 1.0;
	stTuningTheme.fWeightNegativeDPE = 1.0;
	stTuningTheme.fWeightPositiveDPE = 1.0;
}

UINT MtnTune_TuningThreadProcOneGroup( LPVOID pParam )
{
    CMtnTune* pObject = (CMtnTune *)pParam;
	return pObject->mtn_tuning_thread_one_group(); 	
}

void CMtnTune::mtn_tune_set_comm_handle(HANDLE stCommHandle)
{
	Handle = stCommHandle;
}
void CMtnTune::mtn_tune_set_tune_axis(int iTuneAxis)
{
	iAxisTuning = iTuneAxis;
}

char CMtnTune::mtn_tuning_get_flag_doing_thread_one_group()
{
	return cFlagDoingTuningThreadOneGroup;
}

void CMtnTune::mtn_tune_start_tuning_thread_one_group()
{
	cFlagDoingTuningThreadOneGroup = TRUE;
	pMtnTuneWinThread = AfxBeginThread(MtnTune_TuningThreadProcOneGroup, this); // , THREAD_PRIORITY_TIME_CRITICAL);
	pMtnTuneWinThread->m_bAutoDelete = FALSE;

}

int CMtnTune::mtn_tune_get_tune_axis()
{
	return iAxisTuning;
}

int CMtnTune::mtn_tune_get_move_point_1()
{
	return iMovePoint1;
}
int CMtnTune::mtn_tune_get_move_point_2()
{
	return iMovePoint2;
}

void CMtnTune::mtn_tune_stop_tuning_thread_one_group()
{
	if(pMtnTuneWinThread)
	{
		WaitForSingleObject(pMtnTuneWinThread->m_hThread, 2000);
		pMtnTuneWinThread = NULL;
	}
}
//#define __DEBUG__
short CMtnTune::OneAxis2PointsMoveOneCycle()
{
	static int iTempMotorState;
	short sRet = 0;

	Sleep(iDelayInterMove_ms); 	//Sleep(iDelayInterMove_ms);

	acsc_ToPoint(Handle, 0, // start up immediately the motion
					iAxisTuning, iMovePoint1, NULL);

	int ii = 5;
//	char strFilenameDbg[128];
	while(qc_is_axis_still_acc_dec(Handle, iAxisTuning))
	{
#ifdef __DEBUG__
	int iTempMotorState;
	mtnapi_get_motor_state(Handle, iAxisTuning, &iTempMotorState, NULL);
	FILE * fptr;
	sprintf(strFilenameDbg, "TuneDbg_%d.txt", ii); ii++;
    fopen_s(&fptr, strFilenameDbg, "w");
	fprintf(fptr, "Handle = %d\n", Handle);
	fprintf(fptr, "iAxisTuning = %d\n", iAxisTuning);
	fprintf(fptr, "iTempMotorState = %d\n", iTempMotorState);
	fclose(fptr);
#endif  // __DEBUG__
		Sleep(2); 	//Sleep(2);
		if(qc_is_axis_not_safe(Handle, iAxisTuning))
		{
			sRet = MTN_API_ERROR; // error happens
			goto label_ret_one_axis_2_points_move_cycle;
		}
	}

	Sleep(iDelayInterMove_ms); 	//Sleep(iDelayInterMove_ms);
	acsc_ToPoint(Handle, 0, // start up immediately the motion
					iAxisTuning, iMovePoint2, NULL);
	mtnapi_get_motor_state(Handle, iAxisTuning, &iTempMotorState, NULL);

	while(qc_is_axis_still_acc_dec(Handle, iAxisTuning))
	{
		Sleep(2); 	//Sleep(2);
		if(qc_is_axis_not_safe(Handle, iAxisTuning))
		{
			sRet = MTN_API_ERROR; // error happens
			goto label_ret_one_axis_2_points_move_cycle;
		}
	}

label_ret_one_axis_2_points_move_cycle:
	return sRet;
}
short CMtnTune::OneAxis3MovePoints()
{
	static int iTempMotorState;
	short sRet = 0;

	return sRet;
}

#define FILE_NAME_PREFIX_TUNING   "SrvTune"
short CMtnTune::mtn_save_tune_debug_data_to_file()
{
	short sRet = MTN_API_OK_ZERO;
	unsigned int ii, jj;
	if(fpDataTuneDebug != NULL)
	{
	//	fprintf(fpDataTuneDebug, "%% ACSC Controller, %s\n\n", strACSC_VarName);
		mtnapi_upload_servo_parameter_acs_per_axis(Handle, iAxisTuning, &(stAxisServoCtrlParaAtTuningACS));

		fprintf(fpDataTuneDebug, "%% ACS_Axis - %d \n", iAxisTuning );
		fprintf(fpDataTuneDebug, "%% CurrentAFFC = %6.0f \n", stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward);
		fprintf(fpDataTuneDebug, "%% PE RPOS FVEL RVEL DOUT\n");
		fprintf(fpDataTuneDebug, "TrajData = [");
		for(ii = 0; ii<gstSystemScope.uiDataLen; ii++)
		{
			for(jj = 0; jj<gstSystemScope.uiNumData; jj++)
			{
				if(jj == gstSystemScope.uiNumData - 1)
				{	
					fprintf(fpDataTuneDebug, "%8.2f", gdScopeCollectData[jj* gstSystemScope.uiDataLen + ii]);
				}
				else
				{
					fprintf(fpDataTuneDebug, "%8.2f,  ", gdScopeCollectData[jj* gstSystemScope.uiDataLen + ii]);
				}
			}
			if(ii == (gstSystemScope.uiDataLen - 1))
			{
				fprintf(fpDataTuneDebug, "];\n");
			}
			else
			{
				fprintf(fpDataTuneDebug, "\n");
			}
		}
	}


	return sRet;
}

static MTN_TIME_IDX_2_LOOP_MOVE stMtnTimeIdx2LoopMove;
static MTN_SERVO_TUNE_INDEX  stMtnServoTuneIndex;

static int iTuneThemeOption = TUNE_THEME_OPT_MIN_DPE_DIFF_DPE;
int mtn_tune_get_tune_theme_option()
{
	return iTuneThemeOption;
}
void mtn_tune_set_tune_theme_option(int iOpt)
{
	iTuneThemeOption = iOpt;
}

UINT CMtnTune::mtn_tuning_thread_one_group()
{
	// Setup scope for motion
	while(cFlagDoingTuningThreadOneGroup && (uiCurrIterWithinTuningThread < uiTotalNumIterTunigThread))
	{

		if(iFlagEnableDebug)
		{
			static char strFilenameTuning[128];
			sprintf_s(strFilenameTuning, 128, "%s_SLAFF_%d.m", FILE_NAME_PREFIX_TUNING, uiCurrIterWithinTuningThread);

			fopen_s(&fpDataTuneDebug, strFilenameTuning, "w");
		}

		// Set Parameter
		switch(cFlagTuningOption)
		{
		case __MTN_TUNE_OPTION_SLAFF__:
			stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward = adGroupPointSLAFF[uiCurrIterWithinTuningThread];
			break;
		case __MTN_TUNE_OPTION_VELLOOP__:
			break;
		case __MTN_TUNE_OPTION_POSNLOOP__:
			break;
		default:
			break;
		}
		mtnapi_download_servo_parameter_acs_per_axis(Handle, iAxisTuning, &(stAxisServoCtrlParaAtTuningACS));
		Sleep(10);

		// Start Data Capture
		if (!acsc_CollectB(Handle, 0, // system data collection
				gstrScopeArrayName, // name of data collection array
				gstSystemScope.uiDataLen, // number of samples to be collected
				1, // sampling period 1 millisecond
				strACSC_VarName, // variables to be collected
				NULL)
			)
		{
			printf("Collecting data, transaction error: %d\n", acsc_GetLastError());
//				sRet = MTN_API_ERROR; // error happens
		}
	// for each set of parameter
		// Move Servo-Axis, Front-Back 2 times	// Upload data, Calculate index
		OneAxis2PointsMoveOneCycle();
		OneAxis2PointsMoveOneCycle();


		// Upload data, calculate index
		mtnscope_upload_acsc_data(Handle);
		if(iFlagEnableDebug)
		{
			if(fpDataTuneDebug != NULL)
			{
				mtn_save_tune_debug_data_to_file();
				fclose(fpDataTuneDebug);
			}
		}
		// init data buffer point and position error array
		mtn_tune_init_array_ptr();
		mtn_tune_calc_time_idx_2_loop_move(&stMtnTimeIdx2LoopMove);
		// Calculate servo tuning index
		mtn_tune_calc_servo_perform_index(fThresholdSettleTime, &stMtnTimeIdx2LoopMove, &stMtnServoTuneIndex);

		// Calculate Objective Function
		switch(cFlagTuningOption)
		{
		case __MTN_TUNE_OPTION_SLAFF__:
			if(iTuneThemeOption == TUNE_THEME_OPT_MIN_DPE_DIFF_DPE)
			{
			adObjGroupTuningSLAFF[uiCurrIterWithinTuningThread] = fabs(stMtnServoTuneIndex.fMaxBackMoveDPE) * stTuningTheme.fWeightPositiveDPE 
				+ fabs(stMtnServoTuneIndex.fMaxFrontMoveDPE) * stTuningTheme.fWeightNegativeDPE 
				+ fabs(stMtnServoTuneIndex.fMaxBackMoveDPE + stMtnServoTuneIndex.fMaxFrontMoveDPE) * stTuningTheme.fWeightDiffDPE;
			}
			else
			{
				adObjGroupTuningSLAFF[uiCurrIterWithinTuningThread] = 
				(fabs(stMtnServoTuneIndex.fCmdEndFrontOUS) + fabs(stMtnServoTuneIndex.fCmdEndFrontOUS)) * stTuningTheme.fWeightCmdEndOverUnderShoot;
			}
			break;
		case __MTN_TUNE_OPTION_VELLOOP__:
			break;
		case __MTN_TUNE_OPTION_POSNLOOP__:
			break;
		default:
			break;
		}
		uiCurrIterWithinTuningThread ++;
	}

	if(uiCurrIterWithinTuningThread >= uiTotalNumIterTunigThread)
	{
//		pMtnTuneWinThread = NULL;
		cFlagDoingTuningThreadOneGroup = FALSE;
	}
	return 0;
}

int func_compare_double_for_sorting(const void *pdIn1, const void *pdIn2)
{
	int iRet = 0;
	double dIn1, dIn2;

	dIn1 = *(double*) pdIn1;
	dIn2 = *(double*) pdIn2;
	if(dIn1 < dIn2)
	{
		iRet = -1;
	}
	else if(dIn1 > dIn2)
	{
		iRet = 1;
	}
	else
	{
		iRet = 0;
	}
	return iRet;
}

static	double dLowerPointSLAFF, dUpperPointSLAFF;
static	double dDeltaSLAFF;
static  double dCurrentSLAFF, dOriginalSLAFF_Bak;
static  MTN_TUNE_SEARCH_AFF_OUTPUT  *stpMtnTuneSearchAffOutByThread;

unsigned int CMtnTune::mtn_tune_get_total_num_iter_for_one_group()
{
	return uiTotalNumIterTunigThread;
}

unsigned int CMtnTune::mtn_tune_get_curr_iter_within_one_group()
{
	return uiCurrIterWithinTuningThread;
}
int CMtnTune::mtn_tune_get_group_loop_iter()
{
	return iTuningLoopIter;
}
char CMtnTune::mtn_tune_get_option()
{
	return cFlagTuningOption;
}

void CMtnTune::mtn_tune_one_group_by_thread() 
{
	int ii, jj;

	dDeltaSLAFF = (dUpperPointSLAFF - dLowerPointSLAFF)/(__MTN_TUNE_NUM_GROUP_POINTS__  - 1);

	fprintf_s(fpDataTuneAccFF,"SERVO_ACC_FFC_%d = [", iTuningLoopIter);
	for(ii = 0; ii<__MTN_TUNE_NUM_GROUP_POINTS__; ii ++)
	{
		adGroupPointSLAFF[ii] = dLowerPointSLAFF + dDeltaSLAFF * ii ;
		// Write to file, iTuningLoopIter
		if(ii< __MTN_TUNE_NUM_GROUP_POINTS__ - 1)
		{
			fprintf_s(fpDataTuneAccFF, "%6.0f, ", adGroupPointSLAFF[ii]);
		}
		else
		{
			fprintf_s(fpDataTuneAccFF, "%6.0f];\n ", adGroupPointSLAFF[ii]);
		}
	}

	cFlagTuningOption = __MTN_TUNE_OPTION_SLAFF__;
	uiTotalNumIterTunigThread = __MTN_TUNE_NUM_GROUP_POINTS__;
	uiCurrIterWithinTuningThread = 0;
	mtn_tune_start_tuning_thread_one_group();
	Sleep(100);

	while(cFlagDoingTuningThreadOneGroup )
	{
		Sleep(1000);
	}

	// Get the resulting Obj, save to file
	fprintf_s(fpDataTuneAccFF,"aOriginalObjArray_%d = [", iTuningLoopIter);
	for(ii = 0; ii<__MTN_TUNE_NUM_GROUP_POINTS__; ii ++)
	{
		// init for sorting
		adSortedObjGroupTuningSLAFF[ii] = adObjGroupTuningSLAFF[ii];
		// Write to file, iTuningLoopIter
		if(ii< __MTN_TUNE_NUM_GROUP_POINTS__ - 1)
		{
			fprintf_s(fpDataTuneAccFF, "%6.0f, ", adObjGroupTuningSLAFF[ii]);
		}
		else
		{
			fprintf_s(fpDataTuneAccFF, "%6.0f];\n", adObjGroupTuningSLAFF[ii]);
		}
	}
	qsort(adSortedObjGroupTuningSLAFF, __MTN_TUNE_NUM_GROUP_POINTS__, sizeof(double), func_compare_double_for_sorting);
	fprintf_s(fpDataTuneAccFF,"aSortedObj_%d = [", iTuningLoopIter);
	for( ii = 0; ii< __MTN_TUNE_NUM_GROUP_POINTS__; ii ++)
	{
		for(jj = 0; jj<__MTN_TUNE_NUM_GROUP_POINTS__; jj++)
		{
			if(fabs(adSortedObjGroupTuningSLAFF[ii] - adObjGroupTuningSLAFF[jj]) < 1E-6)
			{
				adPointSLAFF_SortByObj[ii] = adGroupPointSLAFF[jj];
				jj = __MTN_TUNE_NUM_GROUP_POINTS__; //continue;
			}
		}
		// Write to file, iTuningLoopIter
		if(ii< __MTN_TUNE_NUM_GROUP_POINTS__ - 1)
		{
			fprintf_s(fpDataTuneAccFF, "%6.0f, ", adSortedObjGroupTuningSLAFF[ii]);
		}
		else
		{
			fprintf_s(fpDataTuneAccFF, "%6.0f];\n", adSortedObjGroupTuningSLAFF[ii]);
		}
	}
	fprintf_s(fpDataTuneAccFF,"aACC_FFC_Points_SortByObj_%d = [", iTuningLoopIter);
	for( ii = 0; ii< __MTN_TUNE_NUM_GROUP_POINTS__; ii ++)
	{
		// Write to file, iTuningLoopIter
		if(ii< __MTN_TUNE_NUM_GROUP_POINTS__ - 1)
		{
			fprintf_s(fpDataTuneAccFF, "%6.0f, ", adPointSLAFF_SortByObj[ii]);
		}
		else
		{
			fprintf_s(fpDataTuneAccFF, "%6.0f];\n", adPointSLAFF_SortByObj[ii]);
		}
	}
}

#define MTN_TUNING_SLAFF_SAVE_FINE   "MtnTuneAff.txt"

void CMtnTune::mtn_tune_search_aff(MTN_TUNE_SEARCH_AFF_INPUT *stpTuneAffInput, MTN_TUNE_SEARCH_AFF_OUTPUT *stpTuneAffOutput)
{
	dCurrentSLAFF = stpTuneAffInput->fInitSLAFF;

	// Set upload variables
	mtnscope_set_acsc_var_collecting_move(iAxisTuning);
	// upload parameter, init
	mtnapi_upload_servo_parameter_acs_per_axis(Handle, iAxisTuning, &stAxisServoCtrlParaAtTuningACS); // 20090831
	dOriginalSLAFF_Bak = stAxisServoCtrlParaAtTuningACS.dAccelerationFeedforward;

	if(fabs(dCurrentSLAFF) < 1E-6)
	{
		stpTuneAffOutput->iErrorCode = __MTN_TUNE_AFF_ERROR_INITIAL_ZERO__;
		stpTuneAffOutput->fBestObj = adSortedObjGroupTuningSLAFF[0];
		stpTuneAffOutput->fServoLoopAFF = dOriginalSLAFF_Bak;
		goto label_mtn_tune_search_aff;
	}
	stTuningTheme.fWeightPositiveDPE = stpTuneAffInput->fWeightPositiveDPE;
	stTuningTheme.fWeightNegativeDPE = stpTuneAffInput->fWeightNegativeDPE;
	stTuningTheme.fWeightDiffDPE     = stpTuneAffInput->fWeightDiffDPE ;
	stTuningTheme.fWeightCmdEndOverUnderShoot = 2.0 * stpTuneAffInput->fWeightPositiveDPE;
	stTuningTheme.fPenaltyCmdEndOverUnderShoot = 5.0 * stpTuneAffInput->fWeightPositiveDPE ;

	iMovePoint1 = stpTuneAffInput->iMovePoint1;
	iMovePoint2 = stpTuneAffInput->iMovePoint2;
	iFlagEnableDebug = stpTuneAffInput->iFlagEnableDebug;

	stpMtnTuneSearchAffOutByThread = stpTuneAffOutput;
	// Given initial value, AFF, searching range is [AFF/2, AFF*2], generate 8 points
	//

	mtn_tune_aff_start_groups_thread();

label_mtn_tune_search_aff:

	return;
}

UINT MtnTune_TuningAFF_GroupThread( LPVOID pParam )
{
    CMtnTune* pObject = (CMtnTune *)pParam;
	return pObject->mtn_tune_aff_groups_thread(); 	
}

void CMtnTune::mtn_tune_aff_start_groups_thread()
{
	cFlagSearchAFF_DoingGroupThread = TRUE;
	pMtnTuneAFF_GroupWinThread = AfxBeginThread(MtnTune_TuningAFF_GroupThread, this); // , THREAD_PRIORITY_TIME_CRITICAL);
	pMtnTuneAFF_GroupWinThread->m_bAutoDelete = FALSE;
}

void CMtnTune::mtn_tune_aff_stop_groups_thread()
{
	if(pMtnTuneAFF_GroupWinThread)
	{
		WaitForSingleObject(pMtnTuneAFF_GroupWinThread->m_hThread, 2000);
		pMtnTuneAFF_GroupWinThread = NULL;
	}
	cFlagSearchAFF_DoingGroupThread = FALSE;
}

char CMtnTune::mtn_tune_aff_get_flag_doing_group_thread()
{
	return cFlagSearchAFF_DoingGroupThread;
}

char CMtnTune::mtn_tune_get_stage_group_thread()
{
	return cFlagTuningStageSLAFF;
}

UINT CMtnTune::mtn_tune_aff_groups_thread()
{

	cFlagTuningStageSLAFF = __MTN_TUNE_SLAFF_STAGE_FIND_RANGE__;

	fopen_s(&fpDataTuneAccFF, MTN_TUNING_SLAFF_SAVE_FINE, "w");
	fprintf_s(fpDataTuneAccFF, "%%%% Tuning ACC-FFC\n");
	fprintf_s(fpDataTuneAccFF, "%%%% Rough Tuning to Find Range: Loop-Iteration: %d\n", iTuningLoopIter);

	iTuningLoopIter = 1;
	while(cFlagTuningStageSLAFF == __MTN_TUNE_SLAFF_STAGE_FIND_RANGE__ && cFlagSearchAFF_DoingGroupThread == TRUE)
	{
		dLowerPointSLAFF = fabs(dCurrentSLAFF)/2.0;
		dUpperPointSLAFF = fabs(dCurrentSLAFF) * 2.0;

		// linear devide the tuning range [], start tuing-thread, get objective func value, sort, save data to file
		mtn_tune_one_group_by_thread();

		// find the best point
		dCurrentSLAFF = adPointSLAFF_SortByObj[0];
		// Next iteration
		if(dCurrentSLAFF > dLowerPointSLAFF && dCurrentSLAFF < dUpperPointSLAFF)  // 		// if the best point within the range, goto next step, fine tuning
		{  // exit the loop
			cFlagTuningStageSLAFF = __MTN_TUNE_SLAFF_STAGE_FINE_SEARCH__;
		}
		else // else the best point on the boundary, continue to search the range
		{ 
			iTuningLoopIter ++;
			fprintf_s(fpDataTuneAccFF, "%%%% RoughTuing to Find Range: Loop-Iteration: %d\n", iTuningLoopIter);
		}
	}

	// Fine tune the AFF parameter by genetic algorithm
double d2ndOptimalAFF, d3rdOptimalAFF_NegSign2nd = 0;
	iTuningLoopIter = 1;
	fprintf_s(fpDataTuneAccFF, "\n\n%%%% Fine-Tuing to Find Range: Loop-Iteration: %d\n", iTuningLoopIter);
	dLowerPointSLAFF = fabs(dCurrentSLAFF) * 0.5;
	dUpperPointSLAFF = fabs(dCurrentSLAFF) * 1.5;

	while(cFlagTuningStageSLAFF == __MTN_TUNE_SLAFF_STAGE_FINE_SEARCH__ && cFlagSearchAFF_DoingGroupThread == TRUE)
	{
		// linear devide the tuning range [], start tuing-thread, get objective func value, sort, save data to file
		mtn_tune_one_group_by_thread();

		// find the best point
		dCurrentSLAFF = adPointSLAFF_SortByObj[0];
		d2ndOptimalAFF = adPointSLAFF_SortByObj[1];

		for(int ii=2; ii<__MTN_TUNE_NUM_GROUP_POINTS__; ii++)
		{
			if((adPointSLAFF_SortByObj[ii] - dCurrentSLAFF) * (d2ndOptimalAFF - dCurrentSLAFF) < 0)
			{
				d3rdOptimalAFF_NegSign2nd = adPointSLAFF_SortByObj[ii];
				ii = __MTN_TUNE_NUM_GROUP_POINTS__;
			}
		}
		// on the boundary or abnormal data
		if(d3rdOptimalAFF_NegSign2nd ==0 || fabs(dCurrentSLAFF - dLowerPointSLAFF) < 1E-6 || fabs(dCurrentSLAFF - dUpperPointSLAFF) < 1E-6)
		{
			cFlagTuningStageSLAFF = __MTN_TUNE_SLAFF_ERROR_RET;
			stpMtnTuneSearchAffOutByThread->fServoLoopAFF = dOriginalSLAFF_Bak;

			break;
		}
		// calculate for next loop
		dLowerPointSLAFF = (d2ndOptimalAFF < d3rdOptimalAFF_NegSign2nd)?(d2ndOptimalAFF):(d3rdOptimalAFF_NegSign2nd);
		dUpperPointSLAFF = (d2ndOptimalAFF > d3rdOptimalAFF_NegSign2nd)?(d2ndOptimalAFF):(d3rdOptimalAFF_NegSign2nd);

		if(fabs(dUpperPointSLAFF - dLowerPointSLAFF) < 1)
		{  // normal exit
			cFlagTuningStageSLAFF = __MTN_TUNE_SLAFF_SUCCESS_RET;
			stpMtnTuneSearchAffOutByThread->fServoLoopAFF = dCurrentSLAFF;

			break;
		}
		else
		{
			iTuningLoopIter ++;
			fprintf_s(fpDataTuneAccFF, "%%%% Fine-Tuing to Find Range: Loop-Iteration: %d\n", iTuningLoopIter);
		}

	}

	fclose(fpDataTuneAccFF);
	mtn_tune_stop_tuning_thread_one_group(); // stop thread of one group

	cFlagSearchAFF_DoingGroupThread = FALSE;

	return 0;
}