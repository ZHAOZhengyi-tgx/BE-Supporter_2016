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

#include "MtnTune.h"
#include "MtnInitAcs.h"
#include "GA.h"

#include "acs_buff_prog.h"
static BUFFER_DATA_WB_TUNE stBufferDataEmuB1W;
static WB_ONE_WIRE_PERFORMANCE_CALC astWbEmuB1WPerformance[NUM_TOTAL_WIRE_IN_ONE_SCOPE];
// WB_ONE_WIRE_PERFORMANCE_CALC  stBestB1W_Performance[NUM_TOTAL_WIRE_IN_ONE_SCOPE]; // 20110523
// BOND_HEAD_PERFORMANCE    stBondHeadPerformanceBestB1W_;   // 20110523
extern WB_ONE_WIRE_PERFORMANCE_CALC astWbOneWirePerformance[NUM_TOTAL_WIRE_IN_ONE_SCOPE]; // 20110529
extern MTN_TUNE_CASE astMotionTuneHistory[MAX_CASE_TUNING_HISTORY_BUFFER];
extern void mtn_tune_round_parameter_to_nearest_1(MTN_TUNE_PARAMETER_SET *stpMtnParaRoundIn, MTN_TUNE_PARAMETER_SET *stpMtnParaRoundOut);
extern MTN_TUNE_CASE astMotionTuneFeasibleParameterB1W[MAX_CASE_FEASIBLE_B1W_PARAMETER];
extern int nFeasibleParameter;

char cFlagStopTuneB1W;
void mtn_wb_tune_b1w_stop()
{
	cFlagStopTuneB1W = TRUE;
}

extern unsigned int idxCurrCase1By1, nTotalCases1By1;
extern double dBestTuneObj1By1;


WB_TUNE_B1W_STOP_SRCH_CFG stWbTuneB1W_StopSrch;
#include "AcsServo.h"
#include "MotAlgo_DLL.h"

int mtn_wb_tune_b1w_stop_srch_init_cfg(HANDLE hAcsHandle)
{
	int iRet = MTN_API_OK_ZERO;

	if(acs_buffprog_get_num_lines_at_buff(DEF_WB_BOND_PROGRAM_IN_BUFF_IDX) > 1)
	{
		int iSrchHt;
		if(!acsc_ReadInteger(hAcsHandle, DEF_WB_BOND_PROGRAM_IN_BUFF_IDX, "IntData", 0, 0, 3, 3, &iSrchHt, NULL ))
		{
			iRet = MTN_API_ERROR_ACS_BUFF_PROG;
		}
		else
		{
			stWbTuneB1W_StopSrch.iSrchHeightPosn = iSrchHt;
		}

		int iSrchVel;
		if(!acsc_ReadInteger(hAcsHandle, DEF_WB_BOND_PROGRAM_IN_BUFF_IDX, "IntData", 0, 0, 4, 4, &iSrchVel, NULL ))
		{
			iRet = MTN_API_ERROR_ACS_BUFF_PROG;
		}
		else
		{
			if(iSrchVel == 0) {iSrchVel = DEF_SRCH_VELOCITY;} // 20120802, protection in case WireBonder is in Dry-run mode
			stWbTuneB1W_StopSrch.iSrchVelocity = iSrchVel;
		}

		int iTailDist;
		if(!acsc_ReadInteger(hAcsHandle, DEF_WB_BOND_PROGRAM_IN_BUFF_IDX, "IntData", 0, 0, 31, 31, &iTailDist, NULL ))
		{
			iRet = MTN_API_ERROR_ACS_BUFF_PROG;
		}
		else
		{
			stWbTuneB1W_StopSrch.iTailDistance = iTailDist;
		}

		int iResetPosn;
		if(!acsc_ReadInteger(hAcsHandle, DEF_WB_BOND_PROGRAM_IN_BUFF_IDX, "IntData", 0, 0, 32, 32, &iResetPosn, NULL ))
		{
			iRet = MTN_API_ERROR_ACS_BUFF_PROG;
		}
		else
		{
			stWbTuneB1W_StopSrch.iResetPosn = iResetPosn;
		}
	}
	else
	{
		iRet = mtn_wb_tune_b1w_update_search_height(hAcsHandle);
		mtn_wb_tune_b1w_init_search_cfg();
//		stWbTuneB1W_StopSrch.

	}
	return iRet;
}
#define DEF_SEARCH_HEIGHT_ABOVE_CONTACT_LEVEL    500
int mtn_wb_tune_b1w_update_search_height(HANDLE hAcsHandle)
{
	int iRet = MTN_API_OK_ZERO;
	double dLowLimitPosn;

		int iContactPosnReg;
		if(!acsc_ReadInteger(hAcsHandle, BUFFER_ID_SEARCH_CONTACT, "CONTACT_POSN_REG", 0, 0, 0, 0, &iContactPosnReg, NULL ))
		{
			iRet = MTN_API_ERROR_ACS_BUFF_PROG;
		}
		else
		{
			if(iContactPosnReg < 0)
			{
				stWbTuneB1W_StopSrch.iSrchHeightPosn = iContactPosnReg + DEF_SEARCH_HEIGHT_ABOVE_CONTACT_LEVEL; // 20120726
			}
			else
			{
				iRet = MTN_API_ERROR_ACS_BUFF_PROG;
			}
		}
		if(iRet == MTN_API_ERROR_ACS_BUFF_PROG)
		{
			if(aft_teach_lower_limit(hAcsHandle, sys_get_acs_axis_id_bnd_z(), 	0, &dLowLimitPosn) != MTN_API_OK_ZERO)
			{
				iRet = MTN_API_ERROR_DOWNLOAD_DATA;
			}
			else
			{
				stWbTuneB1W_StopSrch.iSrchHeightPosn = (int)dLowLimitPosn + DEF_SEARCH_HEIGHT_ABOVE_CONTACT_LEVEL; // 20120726
			}
		}
	return iRet;
}

void mtn_wb_tune_b1w_init_search_cfg()
{
#define DEF_RESET_LEVEL_ABOVE_SEARCH_HEIGHT      7500
		if(theAcsServo.GetServoOperationMode() == ONLINE_MODE)
		{
//			stWbTuneB1W_StopSrch.iResetPosn = (int)mtn_wb_init_bh_relax_position_from_sp(hAcsHandle); // stOutputPosnCompensationTune.iEncoderOffsetSP;
			stWbTuneB1W_StopSrch.iResetPosn = stWbTuneB1W_StopSrch.iSrchHeightPosn + DEF_RESET_LEVEL_ABOVE_SEARCH_HEIGHT; // 20120727
			int iPosnUpperLimitBH = (int)mtn_wb_get_bh_upper_limit_position() - 500;
			if(stWbTuneB1W_StopSrch.iResetPosn > iPosnUpperLimitBH)
			{
				stWbTuneB1W_StopSrch.iResetPosn = iPosnUpperLimitBH;
			}

		}
		else
		{
			stWbTuneB1W_StopSrch.iResetPosn = 5000;
		}

		stWbTuneB1W_StopSrch.iSrchVelocity = DEF_SRCH_VELOCITY;
		stWbTuneB1W_StopSrch.iTailDistance = DEF_TAIL_DIST;
		stWbTuneB1W_StopSrch.i2ndSrchHeightPosn = stWbTuneB1W_StopSrch.iSrchHeightPosn;
		stWbTuneB1W_StopSrch.i2ndSrchVelocity = DEF_SRCH_VELOCITY;
		stWbTuneB1W_StopSrch.iKinkHeight = 100;
		stWbTuneB1W_StopSrch.iLoopTop = 1000;
		stWbTuneB1W_StopSrch.iRevHeight = 400;
}
int mtn_wb_tune_b1w_check_valid_search_height()
{
	if( stWbTuneB1W_StopSrch.iSrchHeightPosn == DEF_SEARCH_HEIGHT_ABOVE_CONTACT_LEVEL || stWbTuneB1W_StopSrch.iSrchHeightPosn == 0)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

int mtn_wb_tune_b1w_update_search_height_posn_by_low_limit(HANDLE hAcsHandle, int iLowLimit)
{
	int iRet = MTN_API_OK_ZERO;
	if(!acsc_WriteInteger(hAcsHandle, BUFFER_ID_SEARCH_CONTACT, "CONTACT_POSN_REG", 0, 0, 0, 0, &iLowLimit, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	stWbTuneB1W_StopSrch.iSrchHeightPosn = DEF_SEARCH_HEIGHT_ABOVE_CONTACT_LEVEL + iLowLimit;

	return iRet;
}

int mtn_wb_tune_b1w_stop_srch_download_cfg()
{
	int iRet = MTN_API_OK_ZERO;

	acs_set_search_height_posn(stWbTuneB1W_StopSrch.iSrchHeightPosn);
	acs_set_search_velocity(stWbTuneB1W_StopSrch.iSrchVelocity);
//	acs_set_search_stop_position(stWbTuneB1W_StopSrch.);
	acs_set_search_tune_bnd_z_tail_dist(stWbTuneB1W_StopSrch.iTailDistance);
	acs_set_search_tune_bnd_z_reset_posn(stWbTuneB1W_StopSrch.iResetPosn);

	acs_set_search_tune_bnd_z_reverse_height(stWbTuneB1W_StopSrch.iRevHeight);

	return iRet;
}

double adScopeCollectDataB1W[30000];
double *pRPOS; 
double *pPE; 
double *pRVEL;
double *pFVEL; 
double *pDCOM;
double *pDOUT;
double adVelErr_cnt_p_ms[1000];

extern double fRefPosnZ[LEN_UPLOAD_ARRAY];
extern double fPosnErrZ[LEN_UPLOAD_ARRAY];
extern double fRefVelZ[LEN_UPLOAD_ARRAY];
extern double fFeedVelZ[LEN_UPLOAD_ARRAY];

int mtn_wb_tune_b1w_stop_srch_trig_once(HANDLE hAcsHandle)
{
	int iRet;
	iRet = MTN_API_OK_ZERO;
	while(qc_is_axis_still_acc_dec(hAcsHandle, ACSC_AXIS_A))
	{
		Sleep(10); 	//Sleep(2);
	}
	Sleep(10);
	mtn_run_srch_contact_b1w(hAcsHandle);
	while(qc_is_axis_still_acc_dec(hAcsHandle, ACSC_AXIS_A))
	{
		Sleep(10); 	//Sleep(2);
	}

	MTN_SCOPE stScopeB1W;
	stScopeB1W.uiDataLen = 1000;
	stScopeB1W.uiNumData = 7;
	stScopeB1W.dSamplePeriod_ms = sys_get_controller_ts();

	if (!acsc_ReadReal( hAcsHandle, BUFFER_ID_SEARCH_CONTACT, "rAFT_Scope", 0, stScopeB1W.uiNumData-1, 0, stScopeB1W.uiDataLen - 1, adScopeCollectDataB1W, NULL))
	{
		iRet = MTN_API_ERROR_UPLOAD_DATA;
	}
	if (!acsc_ReadReal( hAcsHandle, BUFFER_ID_SEARCH_CONTACT, "rAFT_Scope", 0, 0, 0, LEN_UPLOAD_ARRAY - 1, fRefPosnZ, NULL))
	{
		iRet = MTN_API_ERROR_UPLOAD_DATA;
	}
	if (!acsc_ReadReal( hAcsHandle, BUFFER_ID_SEARCH_CONTACT, "rAFT_Scope", 1, 1, 0, LEN_UPLOAD_ARRAY - 1, fPosnErrZ, NULL))
	{
		iRet = MTN_API_ERROR_UPLOAD_DATA;
	}
	if (!acsc_ReadReal( hAcsHandle, BUFFER_ID_SEARCH_CONTACT, "rAFT_Scope", 2, 2, 0, LEN_UPLOAD_ARRAY - 1, fRefVelZ, NULL))
	{
		iRet = MTN_API_ERROR_UPLOAD_DATA;
	}

	double dCountPerSec = 1000.0; // / sys_get_controller_ts();
	int ii;
	for(ii=0; ii<LEN_UPLOAD_ARRAY; ii++)
	{
		fRefVelZ[ii] /= dCountPerSec;
	}
	if (!acsc_ReadReal( hAcsHandle, BUFFER_ID_SEARCH_CONTACT, "rAFT_Scope", 3, 3, 0, LEN_UPLOAD_ARRAY - 1, fFeedVelZ, NULL))
	{
		iRet = MTN_API_ERROR_UPLOAD_DATA;
	}
	for(ii=0; ii<LEN_UPLOAD_ARRAY; ii++)
	{
		fFeedVelZ[ii] /= dCountPerSec;
	}

	return iRet;
}

extern double f_get_mean(double *afIn, int nLen);
extern double f_get_std(double *afIn, double fMean, int nLen);
extern double f_get_abs_max(double *afIn, int nLen);
extern double f_get_min(double *afIn, int nLen);
extern double f_get_max(double *afIn, int nLen);
extern double f_get_rms_max_err_w_offset(double *afIn, int nLen, double dOffset);

int mtn_wb_tune_b1w_stop_srch_calc_bh_obj( MTN_SCOPE *stpScopeB1W, double adScopeDataB1W[],
										  WB_TUNE_B1W_BH_OBJ *stpWbTuneB1wObj)
{

	TIME_POINTS_OF_BONDHEAD_Z stTimePointsOfBondHeadZ;

	memset(&stTimePointsOfBondHeadZ, 0, sizeof(stTimePointsOfBondHeadZ));

	pRPOS = fRefPosnZ;//&adScopeDataB1W[0];
	pPE   = fPosnErrZ;//&adScopeDataB1W[stpScopeB1W->uiDataLen];
	pRVEL = fRefVelZ;//&adScopeDataB1W[stpScopeB1W->uiDataLen * 2];
	pFVEL = fFeedVelZ;//&adScopeDataB1W[stpScopeB1W->uiDataLen * 3];
	pDCOM = &adScopeDataB1W[stpScopeB1W->uiDataLen * 4];
	pDOUT = &adScopeDataB1W[stpScopeB1W->uiDataLen * 5];

	//memcpy(fRefPosnZ, pRPOS, LEN_UPLOAD_ARRAY * sizeof(double));
	//memcpy(fPosnErrZ, pPE, LEN_UPLOAD_ARRAY * sizeof(double));
	//memcpy(fRefVelZ, pRVEL, LEN_UPLOAD_ARRAY * sizeof(double));
	//memcpy(fFeedVelZ, pFVEL, LEN_UPLOAD_ARRAY * sizeof(double));

	double dCountPerSec = 1000.0; // / sys_get_controller_ts();
	unsigned int ii;
	double dFbAcc[LEN_UPLOAD_ARRAY];
	for(ii=0; ii<LEN_UPLOAD_ARRAY - 1; ii++)
	{
		dFbAcc[ii] = pFVEL[ii] - pFVEL[ii-1];
	}
	//for(ii = 0; ii< LEN_UPLOAD_ARRAY; ii++)
	//{
	//	fFeedVelZ[ii] = fFeedVelZ[ii]/dCountPerSec;
	//	fRefVelZ[ii] = fRefVelZ[ii]/dCountPerSec;
	//}
	unsigned int idxStartMoveSrchHt, idxEndMoveSrchHt, idxStartSearch, idxStopSearch, idxStartMoveTail, idxEndMoveTail, idxEndMoveReset;
	unsigned int idxStartLooping, idxEndLooping, idxStartLoopTop, idxEndLoopTop, idxStartTraj, idxEndTraj, idxStart2ndSrch, idxEnd2ndSrch;
	int iFlagStartTraj;

	double dStartPosn;

	idxStartLooping = idxEndLooping = idxStartLoopTop = idxEndLoopTop = idxStartTraj = idxEndTraj = idxStart2ndSrch = idxEnd2ndSrch = 0;
	idxStartMoveSrchHt = idxEndMoveSrchHt = idxStartSearch = idxStartMoveTail = idxEndMoveTail = idxStopSearch = idxEndMoveReset = 0;
	iFlagStartTraj = 0;
	for(ii=0; ii<LEN_UPLOAD_ARRAY; ii++)
	{
		if(idxStartMoveSrchHt == 0
			&& pRVEL[ii + 1] < -0.5
			&& fabs(pRVEL[ii]) < 0.5
			)
		{
			idxStartMoveSrchHt = ii+1;
			dStartPosn = pRPOS[ii];
		}
		if( pRVEL[ii] < 0 
			&& (fabs(pRVEL[ii + 2] - pRVEL[ii + 1])< 0.01 || fabs(pRVEL[ii + 1]) <= 0.5)  //pRPOS[ii] <= stWbTuneB1W_StopSrch.iSrchHeightPosn 
			&& 	idxEndMoveSrchHt == 0
			&&  idxStartMoveSrchHt != 0)
		{
			idxEndMoveSrchHt = ii + 1; 
		}
		if( fabs(pRVEL[ii + 1] - pRVEL[ii]) < 0.5 
			&& 	fabs(pRVEL[ii] * dCountPerSec - stWbTuneB1W_StopSrch.iSrchVelocity ) < 0.01 
			&&  idxStartSearch == 0
			&&  idxEndMoveSrchHt != 0)
		{
			idxStartSearch = ii;
		}
		if(idxStopSearch == 0
			&& idxStartSearch !=0
			&& fabs(pRVEL[ii]) < 0.5 )
		{
			idxStopSearch = ii;
			break;
		}
	}

	if(idxStartSearch == 0) { idxStartSearch = idxEndMoveSrchHt; idxStopSearch = idxStartSearch + 12;} // 20120802

	for(ii=idxStopSearch; ii<LEN_UPLOAD_ARRAY; ii++)
	{
		if(idxStartLooping == 0 &&
			pRVEL[ii] > 0 && 
			pRVEL[ii- 1] <=0
			)
		{
			idxStartLooping = ii;
		}
		if(idxStartLooping != 0 &&
			iFlagStartTraj == 0 &&
			(pRVEL[ii] <= -1E2  )  // 1E5
			// || pRPOS[ii+2] <= stWbTuneB1W_StopSrch.i2ndSrchHeightPosn)
		  )
		{
			iFlagStartTraj = 1;
			//if(pRPOS[ii] <= stWbTuneB1W_StopSrch.i2ndSrchHeightPosn)
			//{
			//	idxEndTraj = ii;
			//}
		}
		if(idxEndTraj == 0 && iFlagStartTraj != 0 &&  pRVEL[ii] < 0 
			&& (pRVEL[ii + 1] == 0 || fabs(pRVEL[ii + 1] - pRVEL[ii + 2])<0.5)
			)//pRPOS[ii] <= stWbTuneB1W_StopSrch.i2ndSrchHeightPosn)
		{
			idxEndTraj = ii;
			break;
		}
	}

	for(ii = idxEndTraj; ii>= idxStopSearch; ii--)
	{
		if(idxStartTraj == 0 && 
			(pRVEL[ii-1] >= 0 && pRVEL[ii] < 0)
			)
		{
			idxStartTraj = ii;
		}
		if(idxEndLoopTop == 0 && idxStartTraj != 0 &&
			(pRVEL[ii-1] > 0 && pRVEL[ii] <= 0 )
			)
		{
			idxEndLoopTop = ii;
		}
		if(idxStartLoopTop == 0 && idxEndLoopTop != 0 &&
			( (pRVEL[ii-1] <= 0 && pRVEL[ii] > 0 )
			 || (pRVEL[ii-1] <= pRVEL[ii] && pRVEL[ii-1] <= pRVEL[ii - 2]) 
			 )
			 )
		{
			idxStartLoopTop = ii;
			idxEndLooping = ii;
		}

		if( idxEndLooping != 0
		    && idxStartLooping == 0
			&& fabs(pRVEL[ii]) < 0.5
			&& pRVEL[ii+1] > 0.5)
		{
			idxStartLooping = ii;
			break;
		}
	}

	for(ii=idxEndTraj; ii<LEN_UPLOAD_ARRAY; ii++)
	{
// 20120512
		if(idxStart2ndSrch == 0 
			&& 	fabs(pRVEL[ii] * dCountPerSec - stWbTuneB1W_StopSrch.i2ndSrchVelocity ) < 0.01 )
		{
			idxStart2ndSrch = ii;
		}
		if(idxEnd2ndSrch == 0
			&& idxStart2ndSrch !=0
			&& fabs(pRVEL[ii]) < 0.5 )
		{
			idxEnd2ndSrch = ii;
		}

// 20120512
		if(idxEnd2ndSrch != 0  // idxStopSearch
		    && idxStartMoveTail == 0
			&& fabs(pRVEL[ii]) < 0.5
			&& pRVEL[ii+1] > 0.5)
		{
			idxStartMoveTail = ii;
		}
		if( idxStartMoveTail != 0
			&& idxEndMoveTail == 0
			&& pRVEL[ii] > 0.5 
			&& fabs(pRVEL[ii + 1] ) < 0.5)
		{
			idxEndMoveTail = ii + 1;
		}
		if(idxEndMoveTail != 0
			&& idxEndMoveReset == 0
			&& pRVEL[ii] > 0.5
			&& fabs(pRVEL[ii + 1] ) < 0.5
			&& fabs(pRPOS[ii + 1] - dStartPosn) < 0.5
			)
			 // && fabs(pRPOS[ii] - stWbTuneB1W_StopSrch.iResetPosn ) < 0.5 )
		{
			idxEndMoveReset = ii + 1;
			break;
		}
	}
	if(idxStart2ndSrch == 0) {idxStart2ndSrch = idxEndTraj; idxEnd2ndSrch = idxStart2ndSrch + 12; } // 20120802

	stTimePointsOfBondHeadZ.idxStartMove1stBondSearchHeight = idxStartMoveSrchHt;
	stTimePointsOfBondHeadZ.idxEndMove1stBondSearchHeight = idxEndMoveSrchHt;

	stTimePointsOfBondHeadZ.idxStart1stBondForceCtrl = idxStopSearch;
	stTimePointsOfBondHeadZ.idxEnd1stBondForceCtrl = idxStopSearch;

	stTimePointsOfBondHeadZ.idxStartMoveLoopTop = idxStartLoopTop;
	stTimePointsOfBondHeadZ.idxEndMoveLoopTop = idxEndLoopTop;
	stTimePointsOfBondHeadZ.idxStartReverseHeight = idxStartLooping;
	stTimePointsOfBondHeadZ.idxEndReverseHeight = idxEndLooping;
	stTimePointsOfBondHeadZ.idxStartKinkHeight = idxEndLooping;
	stTimePointsOfBondHeadZ.idxEndKinkHeight = idxEndLooping;

	stTimePointsOfBondHeadZ.idxStartTrajectory = idxStartTraj;
	stTimePointsOfBondHeadZ.idxEndTrajectory2ndBondSearchHeight = idxStart2ndSrch;
	stTimePointsOfBondHeadZ.idxEndTrajectory2ndBondSearchHeight = idxEndTraj;
	stTimePointsOfBondHeadZ.idxStart2ndBondSearchContact = idxStart2ndSrch;
	stTimePointsOfBondHeadZ.idxStart2ndBondForceCtrl = idxEnd2ndSrch;
	stTimePointsOfBondHeadZ.idxEnd2ndBondForceCtrl = idxEnd2ndSrch;

	stTimePointsOfBondHeadZ.idxStartTail = idxStartMoveTail;
	stTimePointsOfBondHeadZ.idxEndTail = idxEndMoveTail;
	stTimePointsOfBondHeadZ.idxStartFireLevel = idxEndMoveTail + 1;
	stTimePointsOfBondHeadZ.idxEndFireLevel = idxEndMoveReset;

	////
	for(ii = 0; ii< idxEndMoveReset + 100; ii++)
	{
		adVelErr_cnt_p_ms[ii] = (pRVEL[ii] - pFVEL[ii]); ///dCountPerSec;  // 1000.0;
	}

	stpWbTuneB1wObj->dObj[WB_BH_SRCH_HT] = fabs(pPE[idxEndMoveSrchHt]) + fabs(adVelErr_cnt_p_ms[idxEndMoveSrchHt]);
	stpWbTuneB1wObj->dObj[WB_BH_SRCH_HT] += fabs(pPE[idxEndMoveSrchHt + 1]) + fabs(adVelErr_cnt_p_ms[idxEndMoveSrchHt + 1]);
	stpWbTuneB1wObj->dObj[WB_BH_SRCH_HT] += fabs(pPE[idxEndMoveSrchHt + 2]) + fabs(adVelErr_cnt_p_ms[idxEndMoveSrchHt + 2]);

	stpWbTuneB1wObj->dObj[WB_BH_SRCH_HT] += fabs(pPE[idxEndMoveSrchHt - 1]) + fabs(adVelErr_cnt_p_ms[idxEndMoveSrchHt - 1]);
	//stpWbTuneB1wObj->dObj[WB_BH_SRCH_HT] += fabs(pPE[idxEndMoveSrchHt - 2]) + fabs(adVelErr_cnt_p_ms[idxEndMoveSrchHt - 2]);
	//stpWbTuneB1wObj->dObj[WB_BH_SRCH_HT] += fabs(pPE[idxEndMoveSrchHt - 3]) + fabs(adVelErr_cnt_p_ms[idxEndMoveSrchHt - 3]);
	//stpWbTuneB1wObj->dObj[WB_BH_SRCH_HT] += fabs(pPE[idxEndMoveSrchHt - 4]) + fabs(adVelErr_cnt_p_ms[idxEndMoveSrchHt - 4]);
	//stpWbTuneB1wObj->dObj[WB_BH_SRCH_HT] += fabs(pPE[idxEndMoveSrchHt - 5]) + fabs(adVelErr_cnt_p_ms[idxEndMoveSrchHt - 5]);

	int nSrchLen = idxStopSearch - idxStartSearch;
	double dMeanVelErr = f_get_mean(&adVelErr_cnt_p_ms[idxStartSearch], nSrchLen ); // +8 - 8 // 20121108
	double dStdVelErr = f_get_std(&adVelErr_cnt_p_ms[idxStartSearch], dMeanVelErr, nSrchLen ); // +8 - 8 // 20121108
	stpWbTuneB1wObj->dObj[WB_BH_SRCH_HT] += dStdVelErr; //// 20120802


	double dVelErrP2P = f_get_max(&adVelErr_cnt_p_ms[idxStartSearch], nSrchLen ) - f_get_min(&adVelErr_cnt_p_ms[idxStartSearch], nSrchLen); // 20121108
	double dMaxPosnErr =  f_get_abs_max(&pPE[idxStartSearch], nSrchLen); // 20121108
	double dRmsPosnErrWithOffset = f_get_rms_max_err_w_offset(&pPE[idxStartSearch], nSrchLen, 0.0);  //// 20121108

	double dStdFbAcc = f_get_std(&dFbAcc[idxStartSearch ], 0, nSrchLen); // 20121108
	stpWbTuneB1wObj->dObj[WB_BH_1ST_CONTACT] = dRmsPosnErrWithOffset * dRmsPosnErrWithOffset * 2.0   // 20120802
						+ dStdFbAcc * dStdFbAcc;   // EXPECT_POSN_ERR_FB_LEAD_CMD_SRCH_CONTACT
//									+ dVelErrP2P/2.0 * dVelErrP2P / 2.0
//									+ dMeanVelErr * dMeanVelErr;


	stpWbTuneB1wObj->dObj[WB_BH_TAIL] = fabs(pPE[idxEndMoveTail]);
	stpWbTuneB1wObj->dObj[WB_BH_TAIL] += fabs(pPE[idxEndMoveTail + 1] );
	if(pFVEL[idxEndMoveTail] < 0)
	{
		stpWbTuneB1wObj->dObj[WB_BH_TAIL] = stpWbTuneB1wObj->dObj[WB_BH_TAIL] + fabs(pFVEL[idxEndMoveTail]) + fabs(pPE[idxEndMoveTail]);  //1000
	}
	if(pFVEL[idxEndMoveTail + 1] < 0)
	{
		stpWbTuneB1wObj->dObj[WB_BH_TAIL] = stpWbTuneB1wObj->dObj[WB_BH_TAIL] + fabs(pFVEL[idxEndMoveTail + 1]) + fabs(pPE[idxEndMoveTail + 1] ); //1000
	}

	stpWbTuneB1wObj->dObj[WB_BH_RESET] = fabs(pPE[idxEndMoveReset]) + fabs(adVelErr_cnt_p_ms[idxEndMoveReset]); //1000
	stpWbTuneB1wObj->dObj[WB_BH_RESET] += fabs(pPE[idxEndMoveReset-1]) + fabs(adVelErr_cnt_p_ms[idxEndMoveReset-1]); //1000
	stpWbTuneB1wObj->dObj[WB_BH_RESET] += fabs(pPE[idxEndMoveReset-2]) + fabs(adVelErr_cnt_p_ms[idxEndMoveReset-2]); //1000
	stpWbTuneB1wObj->dObj[WB_BH_RESET] += fabs(pPE[idxEndMoveReset+1]) + fabs(adVelErr_cnt_p_ms[idxEndMoveReset+1]); //1000
	stpWbTuneB1wObj->dObj[WB_BH_RESET] += fabs(pPE[idxEndMoveReset+2]) + fabs(adVelErr_cnt_p_ms[idxEndMoveReset+2]); //1000
//	stpWbTuneB1wObj->dObj[WB_BH_2ND_CONTACT] = ;

//	double dMeanVelErr = f_get_mean(&adVelErr_cnt_p_ms[idxEndMoveReset], 8);
	double dVelErrRMS = f_get_std(&adVelErr_cnt_p_ms[idxEndMoveReset ], 0, 40);  // 20120802
	double dPosnErrRMS = f_get_std(&pPE[idxEndMoveReset ], 0, 40);  // 20120802
	stpWbTuneB1wObj->dObj[WB_BH_IDLE] = dVelErrRMS + dPosnErrRMS;

	stpWbTuneB1wObj->dObj[WB_BH_TRAJECTORY] = fabs(pPE[idxEndTraj]) + fabs(adVelErr_cnt_p_ms[idxEndTraj]);
	stpWbTuneB1wObj->dObj[WB_BH_TRAJECTORY] += fabs(pPE[idxEndTraj + 1]) + fabs(adVelErr_cnt_p_ms[idxEndTraj + 1]);
	stpWbTuneB1wObj->dObj[WB_BH_TRAJECTORY] += fabs(pPE[idxEndTraj + 2]) + fabs(adVelErr_cnt_p_ms[idxEndTraj + 2]);
	stpWbTuneB1wObj->dObj[WB_BH_TRAJECTORY] += fabs(pPE[idxEndTraj + 3]) + fabs(adVelErr_cnt_p_ms[idxEndTraj + 3]);
	stpWbTuneB1wObj->dObj[WB_BH_TRAJECTORY] += fabs(pPE[idxEndTraj - 1]) + fabs(adVelErr_cnt_p_ms[idxEndTraj - 1]);
//	stpWbTuneB1wObj->dObj[WB_BH_TRAJECTORY] += fabs(pPE[idxEndTraj - 2]) + fabs(adVelErr_cnt_p_ms[idxEndTraj - 2]);
	//stpWbTuneB1wObj->dObj[WB_BH_TRAJECTORY] += fabs(pPE[idxEndTraj - 3]) + fabs(adVelErr_cnt_p_ms[idxEndTraj - 3]);
	//stpWbTuneB1wObj->dObj[WB_BH_TRAJECTORY] += fabs(pPE[idxEndTraj - 4]) + fabs(adVelErr_cnt_p_ms[idxEndTraj - 4]);
	//stpWbTuneB1wObj->dObj[WB_BH_TRAJECTORY] += fabs(pPE[idxEndTraj - 5]) + fabs(adVelErr_cnt_p_ms[idxEndTraj - 5]);

	stpWbTuneB1wObj->dObj[WB_BH_LOOP_TOP] = fabs(pPE[idxEndLoopTop]) + fabs(adVelErr_cnt_p_ms[idxEndLoopTop]);
	stpWbTuneB1wObj->dObj[WB_BH_LOOP_TOP] += fabs(pPE[idxEndLoopTop+1]) + fabs(adVelErr_cnt_p_ms[idxEndLoopTop+1]);
	stpWbTuneB1wObj->dObj[WB_BH_LOOP_TOP] += fabs(pPE[idxEndLoopTop+2]) + fabs(adVelErr_cnt_p_ms[idxEndLoopTop+2]);
//	stpWbTuneB1wObj->dObj[WB_BH_LOOP_TOP] += fabs(pPE[idxEndLoopTop+3]) + fabs(adVelErr_cnt_p_ms[idxEndLoopTop+3]);
//	stpWbTuneB1wObj->dObj[WB_BH_LOOP_TOP] += fabs(pPE[idxEndLoopTop+4]) + fabs(adVelErr_cnt_p_ms[idxEndLoopTop+4]);
//WB_TUNE_B1W_BH_OBJ stWbTuneB1wObj;  // 
//    memset(&stWbTuneB1wObj, 0, sizeof(stWbTuneB1wObj));
//	stpWbTuneB1wObj->dObj[WB_BH_SRCH_HT] = mtn_wb_tune_b1w_calc_bh_obj(&stTimePointsOfBondHeadZ, WB_BH_SRCH_HT); // 
//	stpWbTuneB1wObj->dObj[WB_BH_1ST_CONTACT] = mtn_wb_tune_b1w_calc_bh_obj(&stTimePointsOfBondHeadZ, WB_BH_1ST_CONTACT); // 
//	stpWbTuneB1wObj->dObj[WB_BH_TAIL] = mtn_wb_tune_b1w_calc_bh_obj(&stTimePointsOfBondHeadZ, WB_BH_TAIL); // 
//	stpWbTuneB1wObj->dObj[WB_BH_RESET] = mtn_wb_tune_b1w_calc_bh_obj(&stTimePointsOfBondHeadZ, WB_BH_RESET); // 
//	stpWbTuneB1wObj->dObj[WB_BH_IDLE] = mtn_wb_tune_b1w_calc_bh_obj(&stTimePointsOfBondHeadZ, WB_BH_IDLE); // 
//	stpWbTuneB1wObj->dObj[WB_BH_TRAJECTORY] = mtn_wb_tune_b1w_calc_bh_obj(&stTimePointsOfBondHeadZ, WB_BH_TRAJECTORY); // 

	nSrchLen = idxEnd2ndSrch - idxEndTraj;
	dMeanVelErr = f_get_mean(&adVelErr_cnt_p_ms[idxEndTraj], nSrchLen); // 20121108
	dStdVelErr = f_get_std(&adVelErr_cnt_p_ms[idxEndTraj], dMeanVelErr, nSrchLen); // 20121108
	dVelErrP2P = f_get_max(&adVelErr_cnt_p_ms[idxEndTraj], nSrchLen ) - f_get_min(&adVelErr_cnt_p_ms[idxEndTraj], nSrchLen); // 20121108
	dMaxPosnErr = f_get_abs_max(&pPE[idxEndTraj], nSrchLen);
	dRmsPosnErrWithOffset = f_get_rms_max_err_w_offset(&pPE[idxStartSearch], nSrchLen, 0.0);  //// 20121108

	dStdFbAcc = f_get_std(&dFbAcc[idxEndTraj], 0, nSrchLen); // 20121108
	stpWbTuneB1wObj->dObj[WB_BH_2ND_CONTACT] = dRmsPosnErrWithOffset * dRmsPosnErrWithOffset * 2.0 // 20120802
		+ dStdFbAcc * dStdFbAcc; // dMaxPosnErr  // EXPECT_POSN_ERR_FB_LEAD_CMD_SRCH_CONTACT
//									+ dVelErrP2P/2.0 * dVelErrP2P / 2.0
//									+ dMeanVelErr * dMeanVelErr;

	// 20120802
	stpWbTuneB1wObj->dObj[WB_BH_LOOP_TOP] += stpWbTuneB1wObj->dObj[WB_BH_TRAJECTORY]/10.0;
	stpWbTuneB1wObj->dObj[WB_BH_TRAJECTORY] += stpWbTuneB1wObj->dObj[WB_BH_2ND_CONTACT]/10.0;
	// 20120802

	// 20120512
	stpWbTuneB1wObj->dObj[WB_BH_LOOPING] = mtn_wb_tune_b1w_calc_bh_obj(&stTimePointsOfBondHeadZ, WB_BH_LOOPING); // 
//	stpWbTuneB1wObj->dObj[WB_BH_LOOP_TOP] = mtn_wb_tune_b1w_calc_bh_obj(&stTimePointsOfBondHeadZ, WB_BH_LOOP_TOP); // 
//	stpWbTuneB1wObj->dObj[WB_BH_2ND_CONTACT] = mtn_wb_tune_b1w_calc_bh_obj(&stTimePointsOfBondHeadZ, WB_BH_2ND_CONTACT); // 

	return MTN_API_OK_ZERO;
}

#define MAX_CASE_B1W_EACH_PARA  3
extern int idxBestParaSet;

int mtn_wb_tune_bond_1_wire_bh(MTN_TUNE_GENETIC_INPUT *stpMtnTuneInput, MTN_TUNE_GENETIC_OUTPUT *stpMtnTuneOutput, 
								int iObjSectionFlagB1W)
{
//int nTotalNumWire;

HANDLE stCommHandle = stpMtnTuneInput->stCommHandle;
MTN_TUNE_THEME			*stpTuningTheme = &stpMtnTuneInput->stTuningTheme;
TUNE_ALGO_SETTING       *stpTuningAlgoSetting = &stpMtnTuneInput->stTuningAlgoSetting;
MTN_TUNE_PARA_UNION		*stpTuningParameterIni = &stpMtnTuneInput->stTuningParameterIni;
MTN_TUNE_PARA_UNION		*stpTuningParameterUppBound = &stpMtnTuneInput->stTuningParameterUppBound;
MTN_TUNE_PARA_UNION		*stpTuningParameterLowBound = &stpMtnTuneInput->stTuningParameterLowBound;
MTN_TUNE_CONDITION		*stpTuningCondition = &stpMtnTuneInput->stTuningCondition;
MTN_TUNE_ENC_TIME_CFG	*stpTuningEncTimeConfig = &stpMtnTuneInput->stTuningEncTimeConfig;
MTN_TUNE_SPECIFICATION  *stpTuningPassSpecfication = &stpMtnTuneInput->stTuningPassSpecfication;
int	iAxisTuningACS = stpMtnTuneInput->iAxisTuning;
double adParaPosnKP[2 * NUM_POINTS_ONE_DIM_MESH], dStepPosnKP;
double adParaVelKP[2 * NUM_POINTS_ONE_DIM_MESH], dStepVelKP;
double adParaVelKI[2 * NUM_POINTS_ONE_DIM_MESH], dStepVelKI;
double adParaAccFFC[2 * NUM_POINTS_ONE_DIM_MESH], dStepAccFFC;
double adParaJerkFFC[2 * NUM_POINTS_ONE_DIM_MESH], dStepJerkFFC;  // 20110529
int ii, jj, kk, ll, nActualPointsTunePKP, nActualPointsTuneVKP, nActualPointsTuneAccFF, nActualPointsTuneVKI, nActualPointsTuneJerkFF, cc;  // mm, 
double cTempObjCaseInB1W;
short sRet = MTN_API_OK_ZERO;
// int idxBestParaSet;

	// Backup and relax the safety parameters
	SAFETY_PARA_ACS stSafetyParaBak, stSafetyParaCurr;
	mtnapi_upload_safety_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stSafetyParaBak);
	stSafetyParaCurr = stSafetyParaBak;
	stSafetyParaCurr.dCriticalPosnErrAcc = 5000;
	stSafetyParaCurr.dCriticalPosnErrIdle = 5000;	
	stSafetyParaCurr.dCriticalPosnErrVel = 1000;
	stSafetyParaCurr.dRMS_DrvCmdX = stSafetyParaCurr.dRMS_DrvCmdX * 2;
	stSafetyParaCurr.dRMS_DrvCmdIdle = stSafetyParaCurr.dRMS_DrvCmdIdle * 2;
	stSafetyParaCurr.dRMS_DrvCmdMtn = 100;  // 20120717, must release to 100% for motion tuning
	mtnapi_download_safety_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stSafetyParaCurr);

	// Backup and relax Safety parameter for WireClamp
	SAFETY_PARA_ACS stSafetyParaBakWCL, stSafetyParaCurrWCL;
	mtnapi_upload_safety_parameter_acs_per_axis(stCommHandle, sys_get_acs_axis_id_wire_clamp(), &stSafetyParaBakWCL);
	stSafetyParaCurrWCL = stSafetyParaBakWCL;
	stSafetyParaCurrWCL.dRMS_DrvCmdX = stSafetyParaBakWCL.dRMS_DrvCmdX * 2;
	if(stSafetyParaCurrWCL.dRMS_DrvCmdX > 100)
	{
		stSafetyParaCurrWCL.dRMS_DrvCmdX = 100;
	}
	stSafetyParaCurrWCL.dRMS_TimeConst = stSafetyParaBakWCL.dRMS_TimeConst * 6;
	mtnapi_download_safety_parameter_acs_per_axis(stCommHandle, sys_get_acs_axis_id_wire_clamp(), &stSafetyParaCurrWCL);

//int iFlagDoingTest;
	//Clear the tuning history buffer
	idxCurrCase1By1 = idxBestParaSet = 0;
	memset(astMotionTuneHistory, 0, sizeof(MTN_TUNE_CASE) * MAX_CASE_TUNING_HISTORY_BUFFER);

	// Init Parameter
	MTN_TUNE_PARAMETER_SET stCurrTuneBondHeadParaB1W = stpTuningParameterIni->stMtnPara;
	// Start EmuB1W
	if(mtn_b1w_write_para_bh_servo_readve(stCommHandle, &stCurrTuneBondHeadParaB1W, iObjSectionFlagB1W) != MTN_API_OK_ZERO)
	{
		sRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto label_mtn_wb_tune_bond_1_wire_bh;
	}

int uiTotalNumPointsOneDim = NUM_POINTS_ONE_DIM_MESH;

	if(iObjSectionFlagB1W == WB_BH_1ST_CONTACT
		|| iObjSectionFlagB1W == WB_BH_2ND_CONTACT)
	{
		uiTotalNumPointsOneDim = 2 * NUM_POINTS_ONE_DIM_MESH;
	}

	for(cc = 0, cTempObjCaseInB1W=0; cc<MAX_CASE_B1W_EACH_PARA; cc++)
	{
		//if(mtnapi_wait_axis_settling(stCommHandle, iAxisTuningACS, POSN_SETTLE_TH_DURING_TUNING) != MTN_API_OK_ZERO)
		//{
		//	sRet = MTN_API_ERROR_ACS_ERROR_SETTLING;//
		//	goto label_mtn_wb_tune_bond_1_wire_bh;
		//}
		if(acs_bufprog_start_buff_b1w_(&astWbEmuB1WPerformance[cc]) != MTN_API_OK_ZERO)
		{
			sRet = MTN_API_ERROR_UPLOAD_DATA;//
			goto label_mtn_wb_tune_bond_1_wire_bh;
		}
		cTempObjCaseInB1W = cTempObjCaseInB1W + mtn_tune_calc_b1w_bh_performance(&astWbEmuB1WPerformance[cc], stpTuningTheme, iObjSectionFlagB1W);
	}
	astMotionTuneHistory[idxCurrCase1By1].dTuningObj = cTempObjCaseInB1W/MAX_CASE_B1W_EACH_PARA;

	stpMtnTuneOutput->dInitialObj = astMotionTuneHistory[idxCurrCase1By1].dTuningObj; // mtn_tune_calc_b1w_bh_performance(&astWbEmuB1WPerformance[0], stpTuningTheme, iObjSectionFlagB1W);
	stpMtnTuneOutput->stInitialB1WPerformanceBH = astWbEmuB1WPerformance[0].stBondHeadPerformance;
	dBestTuneObj1By1 = stpMtnTuneOutput->dInitialObj;

//	astMotionTuneHistory[idxCurrCase1By1].dTuningObj = mtn_tune_calc_b1w_bh_performance(&astWbEmuB1WPerformance[0], stpTuningTheme, iObjSectionFlagB1W);
	astMotionTuneHistory[idxCurrCase1By1].stTuneBondHeadPerformanceB1W = astWbEmuB1WPerformance[0].stBondHeadPerformance;
	astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara = stCurrTuneBondHeadParaB1W;

	// Init Parameter and step size
MTN_TUNE_PARA_UNION	stCurrTuneParaUppBound, stCurrTuneParaLowBound;
	stCurrTuneParaUppBound.stMtnPara = stpTuningParameterUppBound->stMtnPara;
	stCurrTuneParaLowBound.stMtnPara = stpTuningParameterLowBound->stMtnPara;
	dStepPosnKP = (stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP - stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP)/(uiTotalNumPointsOneDim - 1);
	dStepVelKP = (stCurrTuneParaUppBound.stMtnPara.dVelLoopKP - stCurrTuneParaLowBound.stMtnPara.dVelLoopKP)/(uiTotalNumPointsOneDim - 1);
	dStepVelKI = (stCurrTuneParaUppBound.stMtnPara.dVelLoopKI - stCurrTuneParaLowBound.stMtnPara.dVelLoopKI)/(uiTotalNumPointsOneDim - 1);
	dStepAccFFC = (stCurrTuneParaUppBound.stMtnPara.dAccFFC - stCurrTuneParaLowBound.stMtnPara.dAccFFC)/(uiTotalNumPointsOneDim - 1);
	dStepJerkFFC = (stCurrTuneParaUppBound.stMtnPara.dJerkFf - stCurrTuneParaLowBound.stMtnPara.dJerkFf)/(uiTotalNumPointsOneDim - 1);

	for(ii =0; ii<uiTotalNumPointsOneDim; ii++)
	{
		adParaPosnKP[ii] = stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP + dStepPosnKP * ii;
		adParaVelKP[ii] = stCurrTuneParaLowBound.stMtnPara.dVelLoopKP + dStepVelKP * ii;
		adParaVelKI[ii] = stCurrTuneParaLowBound.stMtnPara.dVelLoopKI + dStepVelKI * ii;
		adParaAccFFC[ii] = stCurrTuneParaLowBound.stMtnPara.dAccFFC + dStepAccFFC * ii;
		adParaJerkFFC[ii] = stCurrTuneParaLowBound.stMtnPara.dJerkFf + dStepJerkFFC * ii; // 20110529
	}

	nTotalCases1By1 = stpTuningAlgoSetting->uiMaxGeneration * 5 * uiTotalNumPointsOneDim; //10000; // NUM_POINTS_ONE_DIM_MESH * NUM_POINTS_ONE_DIM_MESH * NUM_POINTS_ONE_DIM_MESH * NUM_POINTS_ONE_DIM_MESH;
	if(nTotalCases1By1 > MAX_CASE_TUNING_HISTORY_BUFFER)
	{
		nTotalCases1By1 = MAX_CASE_TUNING_HISTORY_BUFFER;
	}

unsigned int iCurrLoop =0;
	nActualPointsTunePKP = nActualPointsTuneVKP = nActualPointsTuneAccFF = nActualPointsTuneJerkFF = nActualPointsTuneVKI = uiTotalNumPointsOneDim;

	///////////// Check upper and lower boundary // 20110529
	// AccFF
	if(stCurrTuneBondHeadParaB1W.dAccFFC > stCurrTuneParaUppBound.stMtnPara.dAccFFC)
	{
		stCurrTuneBondHeadParaB1W.dAccFFC = stCurrTuneParaUppBound.stMtnPara.dAccFFC;
	}
	if(stCurrTuneBondHeadParaB1W.dAccFFC < stCurrTuneParaLowBound.stMtnPara.dAccFFC)
	{
		stCurrTuneBondHeadParaB1W.dAccFFC = stCurrTuneParaLowBound.stMtnPara.dAccFFC;
	}
	if(fabs(stCurrTuneParaUppBound.stMtnPara.dAccFFC - stCurrTuneParaLowBound.stMtnPara.dAccFFC) <= 1.5)
	{
		stCurrTuneBondHeadParaB1W.dAccFFC = (stCurrTuneParaUppBound.stMtnPara.dAccFFC + stCurrTuneParaLowBound.stMtnPara.dAccFFC)/2.0;
		nActualPointsTuneAccFF = 0;
	}

	// PosnLoopKP
	if(stCurrTuneBondHeadParaB1W.dPosnLoopKP > stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP)
	{
		stCurrTuneBondHeadParaB1W.dPosnLoopKP = stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP;
	}
	if(stCurrTuneBondHeadParaB1W.dPosnLoopKP < stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP)
	{
		stCurrTuneBondHeadParaB1W.dPosnLoopKP = stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP;
	}
	if(fabs(stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP - stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP) <= 1.5)
	{
		stCurrTuneBondHeadParaB1W.dPosnLoopKP = (stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP + stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP)/2.0;
		nActualPointsTunePKP = 0;
	}

	// dVelLoopKI
	if(stCurrTuneBondHeadParaB1W.dVelLoopKI > stCurrTuneParaUppBound.stMtnPara.dVelLoopKI)
	{
		stCurrTuneBondHeadParaB1W.dVelLoopKI = stCurrTuneParaUppBound.stMtnPara.dVelLoopKI;
	}
	if(stCurrTuneBondHeadParaB1W.dVelLoopKI < stCurrTuneParaLowBound.stMtnPara.dVelLoopKI)
	{
		stCurrTuneBondHeadParaB1W.dVelLoopKI = stCurrTuneParaLowBound.stMtnPara.dVelLoopKI;
	}
	if(fabs(stCurrTuneParaUppBound.stMtnPara.dVelLoopKI - stCurrTuneParaLowBound.stMtnPara.dVelLoopKI) <= 1.5)
	{
		stCurrTuneBondHeadParaB1W.dVelLoopKI = (stCurrTuneParaUppBound.stMtnPara.dVelLoopKI + stCurrTuneParaLowBound.stMtnPara.dVelLoopKI)/2.0;
		nActualPointsTuneVKI = 0;
	}

	// dVelLoopKP
	if(stCurrTuneBondHeadParaB1W.dVelLoopKP > stCurrTuneParaUppBound.stMtnPara.dVelLoopKP)
	{
		stCurrTuneBondHeadParaB1W.dVelLoopKP = stCurrTuneParaUppBound.stMtnPara.dVelLoopKP;
	}
	if(stCurrTuneBondHeadParaB1W.dVelLoopKP < stCurrTuneParaLowBound.stMtnPara.dVelLoopKP)
	{
		stCurrTuneBondHeadParaB1W.dVelLoopKP = stCurrTuneParaLowBound.stMtnPara.dVelLoopKP;
	}
	if(fabs(stCurrTuneParaUppBound.stMtnPara.dVelLoopKP - stCurrTuneParaLowBound.stMtnPara.dVelLoopKP) <= 1.5)
	{
		stCurrTuneBondHeadParaB1W.dVelLoopKP = (stCurrTuneParaUppBound.stMtnPara.dVelLoopKP + stCurrTuneParaLowBound.stMtnPara.dVelLoopKP)/2.0;
		nActualPointsTuneVKP = 1;
	}
	// JerkFF
	if(stCurrTuneBondHeadParaB1W.dJerkFf > stCurrTuneParaUppBound.stMtnPara.dJerkFf)  
	{
		stCurrTuneBondHeadParaB1W.dJerkFf = stCurrTuneParaUppBound.stMtnPara.dJerkFf;
	}
	if(stCurrTuneBondHeadParaB1W.dJerkFf < stCurrTuneParaLowBound.stMtnPara.dJerkFf) 
	{
		stCurrTuneBondHeadParaB1W.dJerkFf = stCurrTuneParaLowBound.stMtnPara.dJerkFf;
	}
	if(fabs(stCurrTuneParaUppBound.stMtnPara.dJerkFf - stCurrTuneParaLowBound.stMtnPara.dJerkFf) <= 1.5
		|| iObjSectionFlagB1W != WB_BH_SRCH_HT)
	{
		stCurrTuneBondHeadParaB1W.dJerkFf = (stCurrTuneParaUppBound.stMtnPara.dJerkFf + stCurrTuneParaLowBound.stMtnPara.dJerkFf)/2.0;
		nActualPointsTuneJerkFF = 0;
	} // 20110529

	while( (nActualPointsTuneAccFF >= 1 || nActualPointsTunePKP >= 1 || nActualPointsTuneVKI >= 1 || nActualPointsTuneVKP >= 1 || nActualPointsTuneJerkFF >= 1)  // 20110529
		&& (idxCurrCase1By1 < nTotalCases1By1) && (iCurrLoop < stpTuningAlgoSetting->uiMaxGeneration) )
	{

		//// VKI
		for(jj = 0; jj<nActualPointsTuneVKI; jj++)
		{
			stCurrTuneBondHeadParaB1W.dVelLoopKI= adParaVelKI[jj];
			if(mtn_b1w_write_para_bh_servo_readve(stCommHandle, &stCurrTuneBondHeadParaB1W, iObjSectionFlagB1W) != MTN_API_OK_ZERO)
			{
				sRet = MTN_API_ERROR_DOWNLOAD_DATA;
				goto label_mtn_wb_tune_bond_1_wire_bh;
			}
			// Execute B1W
			for(cc = 0, cTempObjCaseInB1W=0; cc<MAX_CASE_B1W_EACH_PARA; cc++)
			{
				if(mtnapi_wait_axis_settling(stCommHandle, iAxisTuningACS, POSN_SETTLE_TH_DURING_TUNING) != MTN_API_OK_ZERO)
				{
					sRet = MTN_API_ERROR_ACS_ERROR_SETTLING;//
					goto label_mtn_wb_tune_bond_1_wire_bh;
				}
				if(acs_bufprog_start_buff_b1w_(&astWbEmuB1WPerformance[cc]) != MTN_API_OK_ZERO)
				{
					sRet = MTN_API_ERROR_UPLOAD_DATA;//
					goto label_mtn_wb_tune_bond_1_wire_bh;
				}
				cTempObjCaseInB1W = cTempObjCaseInB1W + mtn_tune_calc_b1w_bh_performance(&astWbEmuB1WPerformance[cc], stpTuningTheme, iObjSectionFlagB1W);
			}
			astMotionTuneHistory[idxCurrCase1By1].dTuningObj = cTempObjCaseInB1W/MAX_CASE_B1W_EACH_PARA;

			astMotionTuneHistory[idxCurrCase1By1].stTuneBondHeadPerformanceB1W = astWbEmuB1WPerformance[0].stBondHeadPerformance;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara = stCurrTuneBondHeadParaB1W;

			if(idxCurrCase1By1 ==0)
			{
				dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
				idxBestParaSet = idxCurrCase1By1;
				astWbOneWirePerformance[0].stBondHeadPerformance = astMotionTuneHistory[idxBestParaSet].stTuneBondHeadPerformanceB1W;  // 20110523

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

			if(idxCurrCase1By1 >= nTotalCases1By1 || cFlagStopTuneB1W == TRUE || qc_is_axis_not_safe(stCommHandle, iAxisTuningACS))
			{
				goto label_mtn_wb_tune_bond_1_wire_bh;
			}
		}
		stCurrTuneBondHeadParaB1W.dVelLoopKI = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara.dVelLoopKI;
		if(stCurrTuneParaLowBound.stMtnPara.dVelLoopKI < (stCurrTuneBondHeadParaB1W.dVelLoopKI - RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKI))
		{
			stCurrTuneParaLowBound.stMtnPara.dVelLoopKI = stCurrTuneBondHeadParaB1W.dVelLoopKI - RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKI;
		}
		if(stCurrTuneParaLowBound.stMtnPara.dVelLoopKI <= 0)
		{
			stCurrTuneParaLowBound.stMtnPara.dVelLoopKI = stpTuningParameterLowBound->stMtnPara.dVelLoopKI;
		}
		if(stCurrTuneParaUppBound.stMtnPara.dVelLoopKI > (stCurrTuneBondHeadParaB1W.dVelLoopKI + RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKI))
		{
			stCurrTuneParaUppBound.stMtnPara.dVelLoopKI = (stCurrTuneBondHeadParaB1W.dVelLoopKI + RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKI);
		}

		dStepVelKI = (stCurrTuneParaUppBound.stMtnPara.dVelLoopKI - stCurrTuneParaLowBound.stMtnPara.dVelLoopKI)/(uiTotalNumPointsOneDim - 1);
		if(fabs(dStepVelKI) > 10)
		{
			nActualPointsTuneVKI = uiTotalNumPointsOneDim;
			for(ii =0; ii<uiTotalNumPointsOneDim; ii++)
			{
				adParaVelKI[ii] = stCurrTuneParaLowBound.stMtnPara.dVelLoopKI + dStepVelKI * ii;
			}
		}
		else
		{
			nActualPointsTuneVKI = 0;
		}
		astWbOneWirePerformance[0].stBondHeadPerformance = astMotionTuneHistory[idxBestParaSet].stTuneBondHeadPerformanceB1W;  /// 20110523

		if(iObjSectionFlagB1W != WB_BH_2ND_CONTACT
			&& iObjSectionFlagB1W != WB_BH_1ST_CONTACT
			&& iObjSectionFlagB1W != WB_BH_IDLE)  //// Skip tunning parameter for 1st and 2nd contact, 20101002
		{
			// AccFFC
			for(kk = 0; kk<nActualPointsTuneAccFF; kk++)
			{
				stCurrTuneBondHeadParaB1W.dAccFFC = adParaAccFFC[kk];
				if(mtn_b1w_write_para_bh_servo_readve(stCommHandle, &stCurrTuneBondHeadParaB1W, iObjSectionFlagB1W) != MTN_API_OK_ZERO)
				{
					sRet = MTN_API_ERROR_DOWNLOAD_DATA;
					goto label_mtn_wb_tune_bond_1_wire_bh;
				}
				// Execute B1W
				for(cc = 0, cTempObjCaseInB1W=0; cc<MAX_CASE_B1W_EACH_PARA; cc++)
				{
					if(mtnapi_wait_axis_settling(stCommHandle, iAxisTuningACS, POSN_SETTLE_TH_DURING_TUNING) != MTN_API_OK_ZERO)
					{
						sRet = MTN_API_ERROR_ACS_ERROR_SETTLING;//
						goto label_mtn_wb_tune_bond_1_wire_bh;
					}
					if(acs_bufprog_start_buff_b1w_(&astWbEmuB1WPerformance[cc]) != MTN_API_OK_ZERO)
					{
						sRet = MTN_API_ERROR_UPLOAD_DATA;//
						goto label_mtn_wb_tune_bond_1_wire_bh;
					}
					cTempObjCaseInB1W = cTempObjCaseInB1W + mtn_tune_calc_b1w_bh_performance(&astWbEmuB1WPerformance[cc], stpTuningTheme, iObjSectionFlagB1W);
				}
				astMotionTuneHistory[idxCurrCase1By1].dTuningObj = cTempObjCaseInB1W/MAX_CASE_B1W_EACH_PARA;

				astMotionTuneHistory[idxCurrCase1By1].stTuneBondHeadPerformanceB1W = astWbEmuB1WPerformance[0].stBondHeadPerformance;
				astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara = stCurrTuneBondHeadParaB1W;

				if(dBestTuneObj1By1 > astMotionTuneHistory[idxCurrCase1By1].dTuningObj)
				{
					dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
					idxBestParaSet = idxCurrCase1By1;
				}
				idxCurrCase1By1++;
				if(idxCurrCase1By1 >= nTotalCases1By1 || cFlagStopTuneB1W == TRUE || qc_is_axis_not_safe(stCommHandle, iAxisTuningACS))
				{
					goto label_mtn_wb_tune_bond_1_wire_bh;
				}
			}
			stCurrTuneBondHeadParaB1W.dAccFFC = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara.dAccFFC;
			if(stCurrTuneParaLowBound.stMtnPara.dAccFFC < (stCurrTuneBondHeadParaB1W.dAccFFC - RESCALE_FACTOR_PREV_STEP_SIZE * dStepAccFFC))
			{
				stCurrTuneParaLowBound.stMtnPara.dAccFFC = stCurrTuneBondHeadParaB1W.dAccFFC - RESCALE_FACTOR_PREV_STEP_SIZE * dStepAccFFC;
			}
			if(stCurrTuneParaLowBound.stMtnPara.dAccFFC <= 0)
			{
				stCurrTuneParaLowBound.stMtnPara.dAccFFC = stpTuningParameterLowBound->stMtnPara.dAccFFC;
			}

			if(stCurrTuneParaUppBound.stMtnPara.dAccFFC > (stCurrTuneBondHeadParaB1W.dAccFFC + RESCALE_FACTOR_PREV_STEP_SIZE * dStepAccFFC))
			{
				stCurrTuneParaUppBound.stMtnPara.dAccFFC = (stCurrTuneBondHeadParaB1W.dAccFFC + RESCALE_FACTOR_PREV_STEP_SIZE * dStepAccFFC);
			}
			dStepAccFFC = (stCurrTuneParaUppBound.stMtnPara.dAccFFC - stCurrTuneParaLowBound.stMtnPara.dAccFFC)/(uiTotalNumPointsOneDim - 1);
			if(fabs(dStepAccFFC) > 2)
			{
				nActualPointsTuneAccFF = uiTotalNumPointsOneDim;
				for(ii =0; ii<uiTotalNumPointsOneDim; ii++)
				{
					adParaAccFFC[ii] = stCurrTuneParaLowBound.stMtnPara.dAccFFC + dStepAccFFC * ii;
				}
			}
			else
			{
				nActualPointsTuneAccFF = 0;
			}

			//////  JerkFF, 20110529
			if(iObjSectionFlagB1W == WB_BH_SRCH_HT)
			{
				for(kk = 0; kk<nActualPointsTuneJerkFF; kk++)
				{
					stCurrTuneBondHeadParaB1W.dJerkFf = adParaJerkFFC[kk];
					if(mtn_b1w_write_para_bh_servo_readve(stCommHandle, &stCurrTuneBondHeadParaB1W, iObjSectionFlagB1W) != MTN_API_OK_ZERO)
					{
						sRet = MTN_API_ERROR_DOWNLOAD_DATA;
						goto label_mtn_wb_tune_bond_1_wire_bh;
					}
					// Execute B1W
					for(cc = 0, cTempObjCaseInB1W=0; cc<MAX_CASE_B1W_EACH_PARA; cc++)
					{
						if(mtnapi_wait_axis_settling(stCommHandle, iAxisTuningACS, POSN_SETTLE_TH_DURING_TUNING) != MTN_API_OK_ZERO)
						{
							sRet = MTN_API_ERROR_ACS_ERROR_SETTLING;//
							goto label_mtn_wb_tune_bond_1_wire_bh;
						}
						if(acs_bufprog_start_buff_b1w_(&astWbEmuB1WPerformance[cc]) != MTN_API_OK_ZERO)
						{
							sRet = MTN_API_ERROR_UPLOAD_DATA;//
							goto label_mtn_wb_tune_bond_1_wire_bh;
						}
						cTempObjCaseInB1W = cTempObjCaseInB1W + mtn_tune_calc_b1w_bh_performance(&astWbEmuB1WPerformance[cc], stpTuningTheme, iObjSectionFlagB1W);
					}
					astMotionTuneHistory[idxCurrCase1By1].dTuningObj = cTempObjCaseInB1W/MAX_CASE_B1W_EACH_PARA;

					astMotionTuneHistory[idxCurrCase1By1].stTuneBondHeadPerformanceB1W = astWbEmuB1WPerformance[0].stBondHeadPerformance;
					astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara = stCurrTuneBondHeadParaB1W;

					if(dBestTuneObj1By1 > astMotionTuneHistory[idxCurrCase1By1].dTuningObj)
					{
						dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
						idxBestParaSet = idxCurrCase1By1;
					}
					idxCurrCase1By1++;
					if(idxCurrCase1By1 >= nTotalCases1By1 || cFlagStopTuneB1W == TRUE || qc_is_axis_not_safe(stCommHandle, iAxisTuningACS))
					{
						goto label_mtn_wb_tune_bond_1_wire_bh;
					}
				}
				stCurrTuneBondHeadParaB1W.dJerkFf = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara.dJerkFf;
				if(stCurrTuneParaLowBound.stMtnPara.dJerkFf < (stCurrTuneBondHeadParaB1W.dJerkFf - RESCALE_FACTOR_PREV_STEP_SIZE * dStepJerkFFC))
				{
					stCurrTuneParaLowBound.stMtnPara.dJerkFf = stCurrTuneBondHeadParaB1W.dJerkFf - RESCALE_FACTOR_PREV_STEP_SIZE * dStepJerkFFC;
				}
				if(stCurrTuneParaLowBound.stMtnPara.dJerkFf <= 0)
				{
					stCurrTuneParaLowBound.stMtnPara.dJerkFf = stpTuningParameterLowBound->stMtnPara.dJerkFf;
				}

				if(stCurrTuneParaUppBound.stMtnPara.dJerkFf > (stCurrTuneBondHeadParaB1W.dJerkFf + RESCALE_FACTOR_PREV_STEP_SIZE * dStepJerkFFC))
				{
					stCurrTuneParaUppBound.stMtnPara.dJerkFf = (stCurrTuneBondHeadParaB1W.dJerkFf + RESCALE_FACTOR_PREV_STEP_SIZE * dStepJerkFFC);
				}
				dStepJerkFFC = (stCurrTuneParaUppBound.stMtnPara.dJerkFf - stCurrTuneParaLowBound.stMtnPara.dJerkFf)/(uiTotalNumPointsOneDim - 1);
				if(fabs(dStepJerkFFC) > 10)
				{
					nActualPointsTuneJerkFF = uiTotalNumPointsOneDim;
					for(ii =0; ii<uiTotalNumPointsOneDim; ii++)
					{
						adParaJerkFFC[ii] = stCurrTuneParaLowBound.stMtnPara.dJerkFf + dStepJerkFFC * ii;
					}
				}
				else
				{
					nActualPointsTuneJerkFF = 0;
				}
			}

		}
		astWbOneWirePerformance[0].stBondHeadPerformance = astMotionTuneHistory[idxBestParaSet].stTuneBondHeadPerformanceB1W;  /// 20110523

		// PosnKP
		for(ll = 0; ll<nActualPointsTunePKP; ll++)
		{
			stCurrTuneBondHeadParaB1W.dPosnLoopKP = adParaPosnKP[ll];
			if(stCurrTuneBondHeadParaB1W.dPosnLoopKP < stpTuningParameterLowBound->stMtnPara.dPosnLoopKP)
			{
				stCurrTuneBondHeadParaB1W.dPosnLoopKP = stpTuningParameterLowBound->stMtnPara.dPosnLoopKP;
			}
			if(mtn_b1w_write_para_bh_servo_readve(stCommHandle, &stCurrTuneBondHeadParaB1W, iObjSectionFlagB1W) != MTN_API_OK_ZERO)
			{
				CString cstrTemp; cstrTemp.Format(_T("Error Parameter: %4f, %4f, %4f, %4f, %4f, %4f, %4f, %4f, %4f"), adParaPosnKP[0], adParaPosnKP[1], adParaPosnKP[2],
					adParaPosnKP[3], adParaPosnKP[4], adParaPosnKP[5], adParaPosnKP[6], adParaPosnKP[7], adParaPosnKP[8]);
				AfxMessageBox(cstrTemp);

				sRet = MTN_API_ERROR_DOWNLOAD_DATA;
				goto label_mtn_wb_tune_bond_1_wire_bh;
			}
			// Execute B1W
			for(cc = 0, cTempObjCaseInB1W=0; cc<MAX_CASE_B1W_EACH_PARA; cc++)
			{
				if(mtnapi_wait_axis_settling(stCommHandle, iAxisTuningACS, POSN_SETTLE_TH_DURING_TUNING) != MTN_API_OK_ZERO)
				{
					sRet = MTN_API_ERROR_ACS_ERROR_SETTLING;//
					goto label_mtn_wb_tune_bond_1_wire_bh;
				}
				if(acs_bufprog_start_buff_b1w_(&astWbEmuB1WPerformance[cc]) != MTN_API_OK_ZERO)
				{
					sRet = MTN_API_ERROR_UPLOAD_DATA;//
					goto label_mtn_wb_tune_bond_1_wire_bh;
				}
				cTempObjCaseInB1W = cTempObjCaseInB1W + mtn_tune_calc_b1w_bh_performance(&astWbEmuB1WPerformance[cc], stpTuningTheme, iObjSectionFlagB1W);
			}
			astMotionTuneHistory[idxCurrCase1By1].dTuningObj = cTempObjCaseInB1W/MAX_CASE_B1W_EACH_PARA;

			astMotionTuneHistory[idxCurrCase1By1].stTuneBondHeadPerformanceB1W = astWbEmuB1WPerformance[0].stBondHeadPerformance;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara = stCurrTuneBondHeadParaB1W;

			if(dBestTuneObj1By1 > astMotionTuneHistory[idxCurrCase1By1].dTuningObj)
			{
				dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
				idxBestParaSet = idxCurrCase1By1;
			}
			idxCurrCase1By1++;

			if(idxCurrCase1By1 >= nTotalCases1By1 || cFlagStopTuneB1W == TRUE || qc_is_axis_not_safe(stCommHandle, iAxisTuningACS))
			{
				goto label_mtn_wb_tune_bond_1_wire_bh;
			}
		}
		stCurrTuneBondHeadParaB1W.dPosnLoopKP = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara.dPosnLoopKP;
		if(stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP < (stCurrTuneBondHeadParaB1W.dPosnLoopKP - RESCALE_FACTOR_PREV_STEP_SIZE * dStepPosnKP))
		{
			stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP = stCurrTuneBondHeadParaB1W.dPosnLoopKP - RESCALE_FACTOR_PREV_STEP_SIZE * dStepPosnKP;
		}
		if(stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP <= 0)
		{
			stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP = stpTuningParameterLowBound->stMtnPara.dPosnLoopKP;
		}
		if(stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP > (stCurrTuneBondHeadParaB1W.dPosnLoopKP + RESCALE_FACTOR_PREV_STEP_SIZE * dStepPosnKP))
		{
			stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP = (stCurrTuneBondHeadParaB1W.dPosnLoopKP + RESCALE_FACTOR_PREV_STEP_SIZE * dStepPosnKP);
		}
		dStepPosnKP = (stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP - stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP)/(uiTotalNumPointsOneDim - 1);
		if(dStepPosnKP > 10)
		{
			nActualPointsTunePKP = uiTotalNumPointsOneDim;
			for(ii =0; ii<uiTotalNumPointsOneDim; ii++)
			{
				adParaPosnKP[ii] = stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP + dStepPosnKP * ii;
			}
		}
		else
		{
			nActualPointsTunePKP = 0;
		}

		astWbOneWirePerformance[0].stBondHeadPerformance = astMotionTuneHistory[idxBestParaSet].stTuneBondHeadPerformanceB1W;  /// 20110523
		// End one loop
		iCurrLoop ++;
	}

label_mtn_wb_tune_bond_1_wire_bh:

	stpMtnTuneOutput->dTuningObj = dBestTuneObj1By1;

	stpMtnTuneOutput->stResultResponse = astMotionTuneHistory[idxBestParaSet].stServoTuneIndex;
	// Tuning pass (iFlagTuningIsFail = FALSE ) iff the obj is better than initial value  <=> (dTuningObj < dInitialObj)
	if(stpMtnTuneOutput->dTuningObj < stpMtnTuneOutput->dInitialObj)
	{
		stpMtnTuneOutput->iFlagTuningIsFail = FALSE;
		// Failure is FALSE, meaning PASS, download the best parameter set
		mtn_b1w_write_para_bh_servo_readve(stCommHandle, &stpMtnTuneOutput->stBestParameterSet, iObjSectionFlagB1W); // stCurrTuneBondHeadParaB1W
		stpMtnTuneOutput->stBestParameterSet = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara; // Only update when TuneOut better than init
		mtn_tune_round_parameter_to_nearest_1(&astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara, &stpMtnTuneOutput->stBestParameterSet);  // APP_Z_BOND_ACS_I // 	// Parameter rounding

	}
	else
	{
		stpMtnTuneOutput->iFlagTuningIsFail = TRUE;
	}

	// Restore the safety parameters
	mtnapi_download_safety_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stSafetyParaBak);
	mtnapi_download_safety_parameter_acs_per_axis(stCommHandle, sys_get_acs_axis_id_wire_clamp(), &stSafetyParaBakWCL);

	if(stpMtnTuneInput->iDebugFlag == TRUE)
	{
		mtn_tune_save_tuning_history(stpMtnTuneInput, stpMtnTuneOutput, idxCurrCase1By1);
	}

	return sRet;
}

#include "ForceCali.h"
#define FILE_NAME_SEARCH_CONTACT_INIT_B1W  "SrchContact_InitB1W.m"
#define FILE_NAME_SEARCH_CONTACT_FINAL  "SrchContact_FinalB1W.m"

void mtn_wb_b1w_tune_check_upp_low_bound( MTN_TUNE_PARAMETER_SET *stpTuningPara,
										MTN_TUNE_PARA_UNION	*stpTuneParaUppBound,	
										MTN_TUNE_PARA_UNION	 *stpTuneParaLowBound)
{
	///////////// Check upper and lower boundary // 20110529
	// AccFF
	if(stpTuningPara->dAccFFC > stpTuneParaUppBound->stMtnPara.dAccFFC)
	{
		stpTuningPara->dAccFFC = stpTuneParaUppBound->stMtnPara.dAccFFC;
	}
	if(stpTuningPara->dAccFFC < stpTuneParaLowBound->stMtnPara.dAccFFC)
	{
		stpTuningPara->dAccFFC = stpTuneParaLowBound->stMtnPara.dAccFFC;
	}
	// PosnLoopKP
	if(stpTuningPara->dPosnLoopKP > stpTuneParaUppBound->stMtnPara.dPosnLoopKP)
	{
		stpTuningPara->dPosnLoopKP = stpTuneParaUppBound->stMtnPara.dPosnLoopKP;
	}
	if(stpTuningPara->dPosnLoopKP < stpTuneParaLowBound->stMtnPara.dPosnLoopKP)
	{
		stpTuningPara->dPosnLoopKP = stpTuneParaLowBound->stMtnPara.dPosnLoopKP;
	}
	// dVelLoopKI
	if(stpTuningPara->dVelLoopKI > stpTuneParaUppBound->stMtnPara.dVelLoopKI)
	{
		stpTuningPara->dVelLoopKI = stpTuneParaUppBound->stMtnPara.dVelLoopKI;
	}
	if(stpTuningPara->dVelLoopKI < stpTuneParaLowBound->stMtnPara.dVelLoopKI)
	{
		stpTuningPara->dVelLoopKI = stpTuneParaLowBound->stMtnPara.dVelLoopKI;
	}
	// dVelLoopKP
	if(stpTuningPara->dVelLoopKP > stpTuneParaUppBound->stMtnPara.dVelLoopKP)
	{
		stpTuningPara->dVelLoopKP = stpTuneParaUppBound->stMtnPara.dVelLoopKP;
	}
	if(stpTuningPara->dVelLoopKP < stpTuneParaLowBound->stMtnPara.dVelLoopKP)
	{
		stpTuningPara->dVelLoopKP = stpTuneParaLowBound->stMtnPara.dVelLoopKP;
	}
	// JerkFF
	if(stpTuningPara->dJerkFf > stpTuneParaUppBound->stMtnPara.dJerkFf)  
	{
		stpTuningPara->dJerkFf = stpTuneParaUppBound->stMtnPara.dJerkFf;
	}
	if(stpTuningPara->dJerkFf < stpTuneParaLowBound->stMtnPara.dJerkFf) 
	{
		stpTuningPara->dJerkFf = stpTuneParaLowBound->stMtnPara.dJerkFf;
	}
}


int mtn_wb_tune_bh_b1w_stop_srch(MTN_TUNE_GENETIC_INPUT *stpMtnTuneInput, MTN_TUNE_GENETIC_OUTPUT *stpMtnTuneOutput, 
								int iObjSectionFlagB1W)
{
//int nTotalNumWire;

HANDLE stCommHandle = stpMtnTuneInput->stCommHandle;
MTN_TUNE_THEME			*stpTuningTheme = &stpMtnTuneInput->stTuningTheme;
TUNE_ALGO_SETTING       *stpTuningAlgoSetting = &stpMtnTuneInput->stTuningAlgoSetting;
MTN_TUNE_PARA_UNION		*stpTuningParameterIni = &stpMtnTuneInput->stTuningParameterIni;
MTN_TUNE_PARA_UNION		*stpTuningParameterUppBound = &stpMtnTuneInput->stTuningParameterUppBound;
MTN_TUNE_PARA_UNION		*stpTuningParameterLowBound = &stpMtnTuneInput->stTuningParameterLowBound;
MTN_TUNE_CONDITION		*stpTuningCondition = &stpMtnTuneInput->stTuningCondition;
MTN_TUNE_ENC_TIME_CFG	*stpTuningEncTimeConfig = &stpMtnTuneInput->stTuningEncTimeConfig;
MTN_TUNE_SPECIFICATION  *stpTuningPassSpecfication = &stpMtnTuneInput->stTuningPassSpecfication;
int	iAxisTuningACS = stpMtnTuneInput->iAxisTuning;
double adParaPosnKP[2 * NUM_POINTS_ONE_DIM_MESH], dStepPosnKP;
double adParaVelKP[2 * NUM_POINTS_ONE_DIM_MESH], dStepVelKP;
double adParaVelKI[2 * NUM_POINTS_ONE_DIM_MESH], dStepVelKI;
double adParaAccFFC[2 * NUM_POINTS_ONE_DIM_MESH], dStepAccFFC;
double adParaJerkFFC[2 * NUM_POINTS_ONE_DIM_MESH], dStepJerkFFC;  // 20110529
int ii, jj, kk, ll, nActualPointsTunePKP, nActualPointsTuneVKP, nActualPointsTuneAccFF, nActualPointsTuneVKI, nActualPointsTuneJerkFF, cc;  // mm, 
double cTempObjCaseInB1W;
short sRet = MTN_API_OK_ZERO;

	// Backup and relax the safety parameters
	SAFETY_PARA_ACS stSafetyParaBak, stSafetyParaCurr;
	mtnapi_upload_safety_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stSafetyParaBak);
	stSafetyParaCurr = stSafetyParaBak;
	stSafetyParaCurr.dCriticalPosnErrAcc *= 32;
	stSafetyParaCurr.dCriticalPosnErrIdle *= 8;	
	stSafetyParaCurr.dCriticalPosnErrVel *= 16;
	stSafetyParaCurr.dRMS_DrvCmdX = stSafetyParaCurr.dRMS_DrvCmdX * 2;
	stSafetyParaCurr.dRMS_DrvCmdIdle = stSafetyParaCurr.dRMS_DrvCmdIdle * 2;
	stSafetyParaCurr.dRMS_DrvCmdMtn  = 100;  // 20120717, must release to 100% for motion tuning
	mtnapi_download_safety_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stSafetyParaCurr);

	mtn_wb_tune_b1w_stop_srch_init_cfg(stCommHandle);
	mtn_wb_tune_b1w_stop_srch_download_cfg();

//int iFlagDoingTest;
	//Clear the tuning history buffer
	idxCurrCase1By1 = idxBestParaSet = 0;
	memset(astMotionTuneHistory, 0, sizeof(MTN_TUNE_CASE) * MAX_CASE_TUNING_HISTORY_BUFFER);

	// Init Parameter
	MTN_TUNE_PARAMETER_SET stCurrTuneBondHeadParaB1W = stpTuningParameterIni->stMtnPara;
	// Start EmuB1W
	if(mtn_b1w_write_para_bh_servo_readve(stCommHandle, &stCurrTuneBondHeadParaB1W, iObjSectionFlagB1W) != MTN_API_OK_ZERO)
	{
		sRet = MTN_API_ERROR_DOWNLOAD_DATA;
		goto label_mtn_wb_tune_bh_b1w_stop_srch;
	}
int uiTotalNumPointsOneDim = NUM_POINTS_ONE_DIM_MESH;

	if(iObjSectionFlagB1W == WB_BH_1ST_CONTACT
		|| iObjSectionFlagB1W == WB_BH_2ND_CONTACT)
	{
		uiTotalNumPointsOneDim = 2 * NUM_POINTS_ONE_DIM_MESH;
	}
WB_TUNE_B1W_BH_OBJ stWbTuneB1wObj;
//	
	TEACH_CONTACT_INPUT stTeachContactParameter;
	acsc_upload_search_contact_parameter(stCommHandle, &stTeachContactParameter);

	for(cc = 0, cTempObjCaseInB1W=0; cc<MAX_CASE_B1W_EACH_PARA; cc++)
	{
		//if(mtnapi_wait_axis_settling(stCommHandle, iAxisTuningACS, POSN_SETTLE_TH_DURING_TUNING) != MTN_API_OK_ZERO)
		//{
		//	sRet = MTN_API_ERROR_ACS_ERROR_SETTLING;//
		//	goto label_mtn_wb_tune_bh_b1w_stop_srch;
		//}
		mtn_wb_tune_b1w_stop_srch_trig_once(stCommHandle);
		while(qc_is_axis_still_acc_dec(stCommHandle, iAxisTuningACS))
		{
			Sleep(10); 	//Sleep(2);
		}
		mtn_wb_tune_b1w_stop_srch_calc_bh_obj(&gstSystemScope, &gdScopeCollectData[0], &stWbTuneB1wObj);

		cTempObjCaseInB1W = cTempObjCaseInB1W + stWbTuneB1wObj.dObj[iObjSectionFlagB1W]; //mtn_tune_calc_b1w_bh_performance(&astWbEmuB1WPerformance[cc], stpTuningTheme, iObjSectionFlagB1W);
	}
	astMotionTuneHistory[idxCurrCase1By1].dTuningObj = cTempObjCaseInB1W/MAX_CASE_B1W_EACH_PARA;

	gstSystemScope.uiNumData = 7;
	gstSystemScope.uiDataLen = 3000;
	gstSystemScope.dSamplePeriod_ms = 1;
	mtn_tune_contact_save_data(stCommHandle, &gstSystemScope, stTeachContactParameter.iAxis, 
				FILE_NAME_SEARCH_CONTACT_INIT_B1W, "%% ACSC Controller, Axis- %d: RPOS(AXIS), PE(AXIS), RVEL(AXIS), FVEL(AXIS), DCOM(AXIS), DOUT(AXIS), SLAFF(AXIS) \n");

	stpMtnTuneOutput->dInitialObj = astMotionTuneHistory[idxCurrCase1By1].dTuningObj; // mtn_tune_calc_b1w_bh_performance(&astWbEmuB1WPerformance[0], stpTuningTheme, iObjSectionFlagB1W);
	stpMtnTuneOutput->stInitialB1WPerformanceBH = astWbEmuB1WPerformance[0].stBondHeadPerformance;
	dBestTuneObj1By1 = stpMtnTuneOutput->dInitialObj;

//	astMotionTuneHistory[idxCurrCase1By1].dTuningObj = mtn_tune_calc_b1w_bh_performance(&astWbEmuB1WPerformance[0], stpTuningTheme, iObjSectionFlagB1W);
	astMotionTuneHistory[idxCurrCase1By1].stTuneBondHeadPerformanceB1W = astWbEmuB1WPerformance[0].stBondHeadPerformance;
	astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara = stCurrTuneBondHeadParaB1W;

	// Init Parameter and step size
MTN_TUNE_PARA_UNION	stCurrTuneParaUppBound, stCurrTuneParaLowBound;
	stCurrTuneParaUppBound.stMtnPara = stpTuningParameterUppBound->stMtnPara;
	stCurrTuneParaLowBound.stMtnPara = stpTuningParameterLowBound->stMtnPara;
	dStepPosnKP = (stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP - stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP)/(uiTotalNumPointsOneDim - 1);
	dStepVelKP = (stCurrTuneParaUppBound.stMtnPara.dVelLoopKP - stCurrTuneParaLowBound.stMtnPara.dVelLoopKP)/(uiTotalNumPointsOneDim - 1);
	dStepVelKI = (stCurrTuneParaUppBound.stMtnPara.dVelLoopKI - stCurrTuneParaLowBound.stMtnPara.dVelLoopKI)/(uiTotalNumPointsOneDim - 1);
	dStepAccFFC = (stCurrTuneParaUppBound.stMtnPara.dAccFFC - stCurrTuneParaLowBound.stMtnPara.dAccFFC)/(uiTotalNumPointsOneDim - 1);
	dStepJerkFFC = (stCurrTuneParaUppBound.stMtnPara.dJerkFf - stCurrTuneParaLowBound.stMtnPara.dJerkFf)/(uiTotalNumPointsOneDim - 1);

	for(ii =0; ii<uiTotalNumPointsOneDim; ii++)
	{
		adParaPosnKP[ii] = stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP + dStepPosnKP * ii;
		adParaVelKP[ii] = stCurrTuneParaLowBound.stMtnPara.dVelLoopKP + dStepVelKP * ii;
		adParaVelKI[ii] = stCurrTuneParaLowBound.stMtnPara.dVelLoopKI + dStepVelKI * ii;
		adParaAccFFC[ii] = stCurrTuneParaLowBound.stMtnPara.dAccFFC + dStepAccFFC * ii;
		adParaJerkFFC[ii] = stCurrTuneParaLowBound.stMtnPara.dJerkFf + dStepJerkFFC * ii; // 20110529
	}

	nTotalCases1By1 = stpTuningAlgoSetting->uiMaxGeneration * 5 * uiTotalNumPointsOneDim; //10000; // NUM_POINTS_ONE_DIM_MESH * NUM_POINTS_ONE_DIM_MESH * NUM_POINTS_ONE_DIM_MESH * NUM_POINTS_ONE_DIM_MESH;
	if(nTotalCases1By1 > MAX_CASE_TUNING_HISTORY_BUFFER)
	{
		nTotalCases1By1 = MAX_CASE_TUNING_HISTORY_BUFFER;
	}

unsigned int iCurrLoop =0;
	nActualPointsTunePKP = nActualPointsTuneVKP = nActualPointsTuneAccFF = nActualPointsTuneJerkFF = nActualPointsTuneVKI = uiTotalNumPointsOneDim;

	// Check with upper and lower bound
	mtn_wb_b1w_tune_check_upp_low_bound( &stCurrTuneBondHeadParaB1W, 
		&stCurrTuneParaUppBound,	&stCurrTuneParaLowBound);

	// Check tuning iterations
	if(fabs(stCurrTuneParaUppBound.stMtnPara.dAccFFC - stCurrTuneParaLowBound.stMtnPara.dAccFFC) <= 1.5)
	{
		stCurrTuneBondHeadParaB1W.dAccFFC = (stCurrTuneParaUppBound.stMtnPara.dAccFFC + stCurrTuneParaLowBound.stMtnPara.dAccFFC)/2.0;
		nActualPointsTuneAccFF = 0;
	}

	if(fabs(stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP - stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP) <= 1.5)
	{
		stCurrTuneBondHeadParaB1W.dPosnLoopKP = (stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP + stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP)/2.0;
		nActualPointsTunePKP = 0;
	}

	if(fabs(stCurrTuneParaUppBound.stMtnPara.dVelLoopKI - stCurrTuneParaLowBound.stMtnPara.dVelLoopKI) <= 1.5)
	{
		stCurrTuneBondHeadParaB1W.dVelLoopKI = (stCurrTuneParaUppBound.stMtnPara.dVelLoopKI + stCurrTuneParaLowBound.stMtnPara.dVelLoopKI)/2.0;
		nActualPointsTuneVKI = 0;
	}

	if(fabs(stCurrTuneParaUppBound.stMtnPara.dVelLoopKP - stCurrTuneParaLowBound.stMtnPara.dVelLoopKP) <= 1.5)
	{
		stCurrTuneBondHeadParaB1W.dVelLoopKP = (stCurrTuneParaUppBound.stMtnPara.dVelLoopKP + stCurrTuneParaLowBound.stMtnPara.dVelLoopKP)/2.0;
		nActualPointsTuneVKP = 1;
	}
	if(fabs(stCurrTuneParaUppBound.stMtnPara.dJerkFf - stCurrTuneParaLowBound.stMtnPara.dJerkFf) <= 1.5
		|| iObjSectionFlagB1W != WB_BH_SRCH_HT)
	{
		stCurrTuneBondHeadParaB1W.dJerkFf = (stCurrTuneParaUppBound.stMtnPara.dJerkFf + stCurrTuneParaLowBound.stMtnPara.dJerkFf)/2.0;
		nActualPointsTuneJerkFF = 0;
	} // 20110529

WB_TUNE_B1W_BH_PASS_FLAG stWbTuneB1wPassFlag;

	while( (nActualPointsTuneAccFF >= 1 || nActualPointsTunePKP >= 1 || nActualPointsTuneVKI >= 1 || nActualPointsTuneVKP >= 1 || nActualPointsTuneJerkFF >= 1)  // 20110529
		&& (idxCurrCase1By1 < nTotalCases1By1) && (iCurrLoop < stpTuningAlgoSetting->uiMaxGeneration) )
	{
		// VKP
		for(ii = 0; ii<nActualPointsTuneVKP; ii++)
		{
			stCurrTuneBondHeadParaB1W.dVelLoopKP = adParaVelKP[ii];
			if(mtn_b1w_write_para_bh_servo_readve(stCommHandle, &stCurrTuneBondHeadParaB1W, iObjSectionFlagB1W) != MTN_API_OK_ZERO)
			{
				sRet = MTN_API_ERROR_DOWNLOAD_DATA;
				goto label_mtn_wb_tune_bh_b1w_stop_srch;
			}
			// Execute B1W, cTempObjCaseInB1W
			for(cc = 0, cTempObjCaseInB1W=0; cc<MAX_CASE_B1W_EACH_PARA; cc++)
			{
				//if(mtnapi_wait_axis_settling(stCommHandle, iAxisTuningACS, POSN_SETTLE_TH_DURING_TUNING) != MTN_API_OK_ZERO)
				//{
				//	sRet = MTN_API_ERROR_ACS_ERROR_SETTLING;//
				//	goto label_mtn_wb_tune_bh_b1w_stop_srch;
				//}
				mtn_wb_tune_b1w_stop_srch_trig_once(stCommHandle);
				while(qc_is_axis_still_acc_dec(stCommHandle, iAxisTuningACS))
				{
					Sleep(10); 	//Sleep(2);
				}
				mtn_wb_tune_b1w_stop_srch_calc_bh_obj(&gstSystemScope, &gdScopeCollectData[0], &stWbTuneB1wObj);

				cTempObjCaseInB1W = cTempObjCaseInB1W + stWbTuneB1wObj.dObj[iObjSectionFlagB1W]; //mtn_tune_calc_b1w_bh_performance(&astWbEmuB1WPerformance[cc], stpTuningTheme, iObjSectionFlagB1W);
			}
			astMotionTuneHistory[idxCurrCase1By1].dTuningObj = cTempObjCaseInB1W/MAX_CASE_B1W_EACH_PARA;

			astMotionTuneHistory[idxCurrCase1By1].stTuneBondHeadPerformanceB1W = astWbEmuB1WPerformance[0].stBondHeadPerformance;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara = stCurrTuneBondHeadParaB1W;
			astMotionTuneHistory[idxCurrCase1By1].stTuneB1wObjValues = stWbTuneB1wObj;

			if(mtn_wb_tune_b1w_check_is_feasible_parameter(&stWbTuneB1wObj.dObj[0], &stWbTuneB1wPassFlag) == TRUE)
			{
				mtn_wb_tune_b1w_record_feasible_parameter(&stWbTuneB1wObj, &astWbEmuB1WPerformance[0].stBondHeadPerformance, 
											   iObjSectionFlagB1W, &stCurrTuneBondHeadParaB1W);
			}

			if(idxCurrCase1By1 ==0)
			{
				dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
				idxBestParaSet = idxCurrCase1By1;
				astWbOneWirePerformance[0].stBondHeadPerformance = astMotionTuneHistory[idxBestParaSet].stTuneBondHeadPerformanceB1W;  // 20110523

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
			if(idxCurrCase1By1 >= nTotalCases1By1 || cFlagStopTuneB1W == TRUE || qc_is_axis_not_safe(stCommHandle, iAxisTuningACS))
			{
				goto label_mtn_wb_tune_bh_b1w_stop_srch;
			}
		}
		stCurrTuneBondHeadParaB1W.dVelLoopKP = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara.dVelLoopKP;
		if(stCurrTuneParaLowBound.stMtnPara.dVelLoopKP < (stCurrTuneBondHeadParaB1W.dVelLoopKP - RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKP))
		{
			stCurrTuneParaLowBound.stMtnPara.dVelLoopKP = stCurrTuneBondHeadParaB1W.dVelLoopKP - RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKP;
		}
		if(stCurrTuneParaLowBound.stMtnPara.dVelLoopKP <= 0)
		{
			stCurrTuneParaLowBound.stMtnPara.dVelLoopKP = stpTuningParameterLowBound->stMtnPara.dVelLoopKP;
		}
		if(stCurrTuneParaUppBound.stMtnPara.dVelLoopKP > (stCurrTuneBondHeadParaB1W.dVelLoopKP + RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKP))
		{
			stCurrTuneParaUppBound.stMtnPara.dVelLoopKP = (stCurrTuneBondHeadParaB1W.dVelLoopKP + RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKP);
		}

		dStepVelKP = (stCurrTuneParaUppBound.stMtnPara.dVelLoopKP - stCurrTuneParaLowBound.stMtnPara.dVelLoopKP)/(uiTotalNumPointsOneDim - 1);
		if(fabs(dStepVelKP) > 0.5) // 20120727
		{
			nActualPointsTuneVKP = uiTotalNumPointsOneDim;
			for(ii =0; ii<uiTotalNumPointsOneDim; ii++)
			{
				adParaVelKP[ii] = stCurrTuneParaLowBound.stMtnPara.dVelLoopKP  + dStepVelKP * ii;
			}
		}
		else
		{
			nActualPointsTuneVKP = 0;
		}

		//// VKI
		for(jj = 0; jj<nActualPointsTuneVKI; jj++)
		{
			stCurrTuneBondHeadParaB1W.dVelLoopKI= adParaVelKI[jj];
			if(mtn_b1w_write_para_bh_servo_readve(stCommHandle, &stCurrTuneBondHeadParaB1W, iObjSectionFlagB1W) != MTN_API_OK_ZERO)
			{
				sRet = MTN_API_ERROR_DOWNLOAD_DATA;
				goto label_mtn_wb_tune_bh_b1w_stop_srch;
			}
			// Execute B1W
			for(cc = 0, cTempObjCaseInB1W=0; cc<MAX_CASE_B1W_EACH_PARA; cc++)
			{
				//if(mtnapi_wait_axis_settling(stCommHandle, iAxisTuningACS, POSN_SETTLE_TH_DURING_TUNING) != MTN_API_OK_ZERO)
				//{
				//	sRet = MTN_API_ERROR_ACS_ERROR_SETTLING;//
				//	goto label_mtn_wb_tune_bh_b1w_stop_srch;
				//}
				mtn_wb_tune_b1w_stop_srch_trig_once(stCommHandle);
				while(qc_is_axis_still_acc_dec(stCommHandle, iAxisTuningACS))
				{
					Sleep(100); 	//Sleep(2);
				}
				mtn_wb_tune_b1w_stop_srch_calc_bh_obj(&gstSystemScope, &gdScopeCollectData[0], &stWbTuneB1wObj);

				cTempObjCaseInB1W = cTempObjCaseInB1W + stWbTuneB1wObj.dObj[iObjSectionFlagB1W]; //mtn_tune_calc_b1w_bh_performance(&astWbEmuB1WPerformance[cc], stpTuningTheme, iObjSectionFlagB1W);
			}
			astMotionTuneHistory[idxCurrCase1By1].dTuningObj = cTempObjCaseInB1W/MAX_CASE_B1W_EACH_PARA;
			astMotionTuneHistory[idxCurrCase1By1].stTuneBondHeadPerformanceB1W = astWbEmuB1WPerformance[0].stBondHeadPerformance;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara = stCurrTuneBondHeadParaB1W;
			astMotionTuneHistory[idxCurrCase1By1].stTuneB1wObjValues = stWbTuneB1wObj;

			if(dBestTuneObj1By1 > astMotionTuneHistory[idxCurrCase1By1].dTuningObj)
			{
				dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
				idxBestParaSet = idxCurrCase1By1;
			}
			idxCurrCase1By1++;

			if(mtn_wb_tune_b1w_check_is_feasible_parameter(&stWbTuneB1wObj.dObj[0], &stWbTuneB1wPassFlag) == TRUE)
			{
				mtn_wb_tune_b1w_record_feasible_parameter(&stWbTuneB1wObj, &astWbEmuB1WPerformance[0].stBondHeadPerformance, 
											   iObjSectionFlagB1W, &stCurrTuneBondHeadParaB1W);
			}

			if(idxCurrCase1By1 >= nTotalCases1By1 || cFlagStopTuneB1W == TRUE || qc_is_axis_not_safe(stCommHandle, iAxisTuningACS))
			{
				goto label_mtn_wb_tune_bh_b1w_stop_srch;
			}
		}
		stCurrTuneBondHeadParaB1W.dVelLoopKI = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara.dVelLoopKI;
		if(stCurrTuneParaLowBound.stMtnPara.dVelLoopKI < (stCurrTuneBondHeadParaB1W.dVelLoopKI - RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKI))
		{
			stCurrTuneParaLowBound.stMtnPara.dVelLoopKI = stCurrTuneBondHeadParaB1W.dVelLoopKI - RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKI;
		}
		if(stCurrTuneParaLowBound.stMtnPara.dVelLoopKI <= 0)
		{
			stCurrTuneParaLowBound.stMtnPara.dVelLoopKI = stpTuningParameterLowBound->stMtnPara.dVelLoopKI;
		}
		if(stCurrTuneParaUppBound.stMtnPara.dVelLoopKI > (stCurrTuneBondHeadParaB1W.dVelLoopKI + RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKI))
		{
			stCurrTuneParaUppBound.stMtnPara.dVelLoopKI = (stCurrTuneBondHeadParaB1W.dVelLoopKI + RESCALE_FACTOR_PREV_STEP_SIZE * dStepVelKI);
		}

		dStepVelKI = (stCurrTuneParaUppBound.stMtnPara.dVelLoopKI - stCurrTuneParaLowBound.stMtnPara.dVelLoopKI)/(uiTotalNumPointsOneDim - 1);
		if(fabs(dStepVelKI) > 1) // 20120727
		{
			nActualPointsTuneVKI = uiTotalNumPointsOneDim;
			for(ii =0; ii<uiTotalNumPointsOneDim; ii++)
			{
				adParaVelKI[ii] = stCurrTuneParaLowBound.stMtnPara.dVelLoopKI + dStepVelKI * ii;
			}
		}
		else
		{
			nActualPointsTuneVKI = 0;
		}
		astWbOneWirePerformance[0].stBondHeadPerformance = astMotionTuneHistory[idxBestParaSet].stTuneBondHeadPerformanceB1W;  /// 20110523

		if(iObjSectionFlagB1W != WB_BH_2ND_CONTACT
			&& iObjSectionFlagB1W != WB_BH_1ST_CONTACT
			&& iObjSectionFlagB1W != WB_BH_IDLE)  //// Skip tunning parameter for 1st and 2nd contact, 20101002
		{
			// AccFFC
			for(kk = 0; kk<nActualPointsTuneAccFF; kk++)
			{
				stCurrTuneBondHeadParaB1W.dAccFFC = adParaAccFFC[kk];
				if(mtn_b1w_write_para_bh_servo_readve(stCommHandle, &stCurrTuneBondHeadParaB1W, iObjSectionFlagB1W) != MTN_API_OK_ZERO)
				{
					sRet = MTN_API_ERROR_DOWNLOAD_DATA;
					goto label_mtn_wb_tune_bh_b1w_stop_srch;
				}
				// Execute B1W
				for(cc = 0, cTempObjCaseInB1W=0; cc<MAX_CASE_B1W_EACH_PARA; cc++)
				{
					//if(mtnapi_wait_axis_settling(stCommHandle, iAxisTuningACS, POSN_SETTLE_TH_DURING_TUNING) != MTN_API_OK_ZERO)
					//{
					//	sRet = MTN_API_ERROR_ACS_ERROR_SETTLING;//
					//	goto label_mtn_wb_tune_bh_b1w_stop_srch;
					//}
					mtn_wb_tune_b1w_stop_srch_trig_once(stCommHandle);
					while(qc_is_axis_still_acc_dec(stCommHandle, iAxisTuningACS))
					{
						Sleep(100); 	//Sleep(2);
					}
					mtn_wb_tune_b1w_stop_srch_calc_bh_obj(&gstSystemScope, &gdScopeCollectData[0], &stWbTuneB1wObj);

					cTempObjCaseInB1W = cTempObjCaseInB1W + stWbTuneB1wObj.dObj[iObjSectionFlagB1W]; //mtn_tune_calc_b1w_bh_performance(&astWbEmuB1WPerformance[cc], stpTuningTheme, iObjSectionFlagB1W);
				}
				astMotionTuneHistory[idxCurrCase1By1].dTuningObj = cTempObjCaseInB1W/MAX_CASE_B1W_EACH_PARA;

				astMotionTuneHistory[idxCurrCase1By1].stTuneBondHeadPerformanceB1W = astWbEmuB1WPerformance[0].stBondHeadPerformance;
				astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara = stCurrTuneBondHeadParaB1W;
				astMotionTuneHistory[idxCurrCase1By1].stTuneB1wObjValues = stWbTuneB1wObj;

				if(dBestTuneObj1By1 > astMotionTuneHistory[idxCurrCase1By1].dTuningObj)
				{
					dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
					idxBestParaSet = idxCurrCase1By1;
				}
				idxCurrCase1By1++;

				if(mtn_wb_tune_b1w_check_is_feasible_parameter(&stWbTuneB1wObj.dObj[0], &stWbTuneB1wPassFlag) == TRUE)
				{
					mtn_wb_tune_b1w_record_feasible_parameter(&stWbTuneB1wObj, &astWbEmuB1WPerformance[0].stBondHeadPerformance, 
											   iObjSectionFlagB1W, &stCurrTuneBondHeadParaB1W);
				}

				if(idxCurrCase1By1 >= nTotalCases1By1 || cFlagStopTuneB1W == TRUE || qc_is_axis_not_safe(stCommHandle, iAxisTuningACS))
				{
					goto label_mtn_wb_tune_bh_b1w_stop_srch;
				}
			}
			stCurrTuneBondHeadParaB1W.dAccFFC = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara.dAccFFC;
			if(stCurrTuneParaLowBound.stMtnPara.dAccFFC < (stCurrTuneBondHeadParaB1W.dAccFFC - RESCALE_FACTOR_PREV_STEP_SIZE * dStepAccFFC))
			{
				stCurrTuneParaLowBound.stMtnPara.dAccFFC = stCurrTuneBondHeadParaB1W.dAccFFC - RESCALE_FACTOR_PREV_STEP_SIZE * dStepAccFFC;
			}
			if(stCurrTuneParaLowBound.stMtnPara.dAccFFC <= 0)
			{
				stCurrTuneParaLowBound.stMtnPara.dAccFFC = stpTuningParameterLowBound->stMtnPara.dAccFFC;
			}

			if(stCurrTuneParaUppBound.stMtnPara.dAccFFC > (stCurrTuneBondHeadParaB1W.dAccFFC + RESCALE_FACTOR_PREV_STEP_SIZE * dStepAccFFC))
			{
				stCurrTuneParaUppBound.stMtnPara.dAccFFC = (stCurrTuneBondHeadParaB1W.dAccFFC + RESCALE_FACTOR_PREV_STEP_SIZE * dStepAccFFC);
			}
			dStepAccFFC = (stCurrTuneParaUppBound.stMtnPara.dAccFFC - stCurrTuneParaLowBound.stMtnPara.dAccFFC)/(uiTotalNumPointsOneDim - 1);
			if(fabs(dStepAccFFC) >= 1)  // 20120727
			{
				nActualPointsTuneAccFF = uiTotalNumPointsOneDim;
				for(ii =0; ii<uiTotalNumPointsOneDim; ii++)
				{
					adParaAccFFC[ii] = stCurrTuneParaLowBound.stMtnPara.dAccFFC + dStepAccFFC * ii;
				}
			}
			else
			{
				nActualPointsTuneAccFF = 0;
			}
			//////  JerkFF, 20110529
			if(iObjSectionFlagB1W == WB_BH_SRCH_HT)
			{
				for(kk = 0; kk<nActualPointsTuneJerkFF; kk++)
				{
					stCurrTuneBondHeadParaB1W.dJerkFf = adParaJerkFFC[kk];
					//if(mtn_b1w_write_para_bh_servo_readve(stCommHandle, &stCurrTuneBondHeadParaB1W, iObjSectionFlagB1W) != MTN_API_OK_ZERO)
					//{
					//	sRet = MTN_API_ERROR_DOWNLOAD_DATA;
					//	goto label_mtn_wb_tune_bh_b1w_stop_srch;
					//}
					mtnapi_download_acs_sp_parameter_jerk_ff(stCommHandle, iAxisTuningACS, stCurrTuneBondHeadParaB1W.dJerkFf);  // 20110524

					// Execute B1W
					for(cc = 0, cTempObjCaseInB1W=0; cc<MAX_CASE_B1W_EACH_PARA; cc++)
					{
						//if(mtnapi_wait_axis_settling(stCommHandle, iAxisTuningACS, POSN_SETTLE_TH_DURING_TUNING) != MTN_API_OK_ZERO)
						//{
						//	sRet = MTN_API_ERROR_ACS_ERROR_SETTLING;//
						//	goto label_mtn_wb_tune_bh_b1w_stop_srch;
						//}
						mtn_wb_tune_b1w_stop_srch_trig_once(stCommHandle);
						while(qc_is_axis_still_acc_dec(stCommHandle, iAxisTuningACS))
						{
							Sleep(100); 	//Sleep(2);
						}
						mtn_wb_tune_b1w_stop_srch_calc_bh_obj(&gstSystemScope, &gdScopeCollectData[0], &stWbTuneB1wObj);

						cTempObjCaseInB1W = cTempObjCaseInB1W + stWbTuneB1wObj.dObj[iObjSectionFlagB1W]; //mtn_tune_calc_b1w_bh_performance(&astWbEmuB1WPerformance[cc], stpTuningTheme, iObjSectionFlagB1W);
					}
					astMotionTuneHistory[idxCurrCase1By1].dTuningObj = cTempObjCaseInB1W/MAX_CASE_B1W_EACH_PARA;

					astMotionTuneHistory[idxCurrCase1By1].stTuneBondHeadPerformanceB1W = astWbEmuB1WPerformance[0].stBondHeadPerformance;
					astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara = stCurrTuneBondHeadParaB1W;
					astMotionTuneHistory[idxCurrCase1By1].stTuneB1wObjValues = stWbTuneB1wObj;

					if(dBestTuneObj1By1 > astMotionTuneHistory[idxCurrCase1By1].dTuningObj)
					{
						dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
						idxBestParaSet = idxCurrCase1By1;
					}
					idxCurrCase1By1++;

					if(mtn_wb_tune_b1w_check_is_feasible_parameter(&stWbTuneB1wObj.dObj[0], &stWbTuneB1wPassFlag) == TRUE)
					{
						mtn_wb_tune_b1w_record_feasible_parameter(&stWbTuneB1wObj, &astWbEmuB1WPerformance[0].stBondHeadPerformance, 
											   iObjSectionFlagB1W, &stCurrTuneBondHeadParaB1W);
					}

					if(idxCurrCase1By1 >= nTotalCases1By1 || cFlagStopTuneB1W == TRUE || qc_is_axis_not_safe(stCommHandle, iAxisTuningACS))
					{
						goto label_mtn_wb_tune_bh_b1w_stop_srch;
					}
				}
				stCurrTuneBondHeadParaB1W.dJerkFf = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara.dJerkFf;
				if(stCurrTuneParaLowBound.stMtnPara.dJerkFf < (stCurrTuneBondHeadParaB1W.dJerkFf - RESCALE_FACTOR_PREV_STEP_SIZE * dStepJerkFFC))
				{
					stCurrTuneParaLowBound.stMtnPara.dJerkFf = stCurrTuneBondHeadParaB1W.dJerkFf - RESCALE_FACTOR_PREV_STEP_SIZE * dStepJerkFFC;
				}
				if(stCurrTuneParaLowBound.stMtnPara.dJerkFf <= 0)
				{
					stCurrTuneParaLowBound.stMtnPara.dJerkFf = stpTuningParameterLowBound->stMtnPara.dJerkFf;
				}

				if(stCurrTuneParaUppBound.stMtnPara.dJerkFf > (stCurrTuneBondHeadParaB1W.dJerkFf + RESCALE_FACTOR_PREV_STEP_SIZE * dStepJerkFFC))
				{
					stCurrTuneParaUppBound.stMtnPara.dJerkFf = (stCurrTuneBondHeadParaB1W.dJerkFf + RESCALE_FACTOR_PREV_STEP_SIZE * dStepJerkFFC);
				}
				dStepJerkFFC = (stCurrTuneParaUppBound.stMtnPara.dJerkFf - stCurrTuneParaLowBound.stMtnPara.dJerkFf)/(uiTotalNumPointsOneDim - 1);
				if(fabs(dStepJerkFFC) > 1) // 20120727
				{
					nActualPointsTuneJerkFF = uiTotalNumPointsOneDim;
					for(ii =0; ii<uiTotalNumPointsOneDim; ii++)
					{
						adParaJerkFFC[ii] = stCurrTuneParaLowBound.stMtnPara.dJerkFf + dStepJerkFFC * ii;
					}
				}
				else
				{
					nActualPointsTuneJerkFF = 0;
				}
			}
		}
		astWbOneWirePerformance[0].stBondHeadPerformance = astMotionTuneHistory[idxBestParaSet].stTuneBondHeadPerformanceB1W;  /// 20110523

		// PosnKP
		for(ll = 0; ll<nActualPointsTunePKP; ll++)
		{
			stCurrTuneBondHeadParaB1W.dPosnLoopKP = adParaPosnKP[ll];
			if(stCurrTuneBondHeadParaB1W.dPosnLoopKP < stpTuningParameterLowBound->stMtnPara.dPosnLoopKP)
			{
				stCurrTuneBondHeadParaB1W.dPosnLoopKP = stpTuningParameterLowBound->stMtnPara.dPosnLoopKP;
			}
			if(mtn_b1w_write_para_bh_servo_readve(stCommHandle, &stCurrTuneBondHeadParaB1W, iObjSectionFlagB1W) != MTN_API_OK_ZERO)
			{
				CString cstrTemp; cstrTemp.Format(_T("Error Parameter: %4f, %4f, %4f, %4f, %4f, %4f, %4f, %4f, %4f"), adParaPosnKP[0], adParaPosnKP[1], adParaPosnKP[2],
					adParaPosnKP[3], adParaPosnKP[4], adParaPosnKP[5], adParaPosnKP[6], adParaPosnKP[7], adParaPosnKP[8]);
				AfxMessageBox(cstrTemp);

				sRet = MTN_API_ERROR_DOWNLOAD_DATA;
				goto label_mtn_wb_tune_bh_b1w_stop_srch;
			}
			// Execute B1W
			for(cc = 0, cTempObjCaseInB1W=0; cc<MAX_CASE_B1W_EACH_PARA; cc++)
			{
				//if(mtnapi_wait_axis_settling(stCommHandle, iAxisTuningACS, POSN_SETTLE_TH_DURING_TUNING) != MTN_API_OK_ZERO)
				//{
				//	sRet = MTN_API_ERROR_ACS_ERROR_SETTLING;//
				//	goto label_mtn_wb_tune_bh_b1w_stop_srch;
				//}
				mtn_wb_tune_b1w_stop_srch_trig_once(stCommHandle);
				while(qc_is_axis_still_acc_dec(stCommHandle, iAxisTuningACS))
				{
					Sleep(100); 	//Sleep(2);
				}
				mtn_wb_tune_b1w_stop_srch_calc_bh_obj(&gstSystemScope, &gdScopeCollectData[0], &stWbTuneB1wObj);
				cTempObjCaseInB1W = cTempObjCaseInB1W + stWbTuneB1wObj.dObj[iObjSectionFlagB1W]; //mtn_tune_calc_b1w_bh_performance(&astWbEmuB1WPerformance[cc], stpTuningTheme, iObjSectionFlagB1W);
			}
			astMotionTuneHistory[idxCurrCase1By1].dTuningObj = cTempObjCaseInB1W/MAX_CASE_B1W_EACH_PARA;

			astMotionTuneHistory[idxCurrCase1By1].stTuneBondHeadPerformanceB1W = astWbEmuB1WPerformance[0].stBondHeadPerformance;
			astMotionTuneHistory[idxCurrCase1By1].stServoPara.stMtnPara = stCurrTuneBondHeadParaB1W;
			astMotionTuneHistory[idxCurrCase1By1].stTuneB1wObjValues = stWbTuneB1wObj;

			if(dBestTuneObj1By1 > astMotionTuneHistory[idxCurrCase1By1].dTuningObj)
			{
				dBestTuneObj1By1 = astMotionTuneHistory[idxCurrCase1By1].dTuningObj;
				idxBestParaSet = idxCurrCase1By1;
			}
			idxCurrCase1By1++;

			if(mtn_wb_tune_b1w_check_is_feasible_parameter(&stWbTuneB1wObj.dObj[0], &stWbTuneB1wPassFlag) == TRUE)
			{
				mtn_wb_tune_b1w_record_feasible_parameter(&stWbTuneB1wObj, &astWbEmuB1WPerformance[0].stBondHeadPerformance, 
											   iObjSectionFlagB1W, &stCurrTuneBondHeadParaB1W);
			}
			if(idxCurrCase1By1 >= nTotalCases1By1 || cFlagStopTuneB1W == TRUE || qc_is_axis_not_safe(stCommHandle, iAxisTuningACS))
			{
				goto label_mtn_wb_tune_bh_b1w_stop_srch;
			}
		}
		stCurrTuneBondHeadParaB1W.dPosnLoopKP = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara.dPosnLoopKP;
		if(stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP < (stCurrTuneBondHeadParaB1W.dPosnLoopKP - RESCALE_FACTOR_PREV_STEP_SIZE * dStepPosnKP))
		{
			stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP = stCurrTuneBondHeadParaB1W.dPosnLoopKP - RESCALE_FACTOR_PREV_STEP_SIZE * dStepPosnKP;
		}
		if(stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP <= 0)
		{
			stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP = stpTuningParameterLowBound->stMtnPara.dPosnLoopKP;
		}
		if(stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP > (stCurrTuneBondHeadParaB1W.dPosnLoopKP + RESCALE_FACTOR_PREV_STEP_SIZE * dStepPosnKP))
		{
			stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP = (stCurrTuneBondHeadParaB1W.dPosnLoopKP + RESCALE_FACTOR_PREV_STEP_SIZE * dStepPosnKP);
		}
		dStepPosnKP = (stCurrTuneParaUppBound.stMtnPara.dPosnLoopKP - stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP)/(uiTotalNumPointsOneDim - 1);
		if(dStepPosnKP > 1) // 20120727
		{
			nActualPointsTunePKP = uiTotalNumPointsOneDim;
			for(ii =0; ii<uiTotalNumPointsOneDim; ii++)
			{
				adParaPosnKP[ii] = stCurrTuneParaLowBound.stMtnPara.dPosnLoopKP + dStepPosnKP * ii;
			}
		}
		else
		{
			nActualPointsTunePKP = 0;
		}

		astWbOneWirePerformance[0].stBondHeadPerformance = astMotionTuneHistory[idxBestParaSet].stTuneBondHeadPerformanceB1W;  /// 20110523
		// End one loop
		iCurrLoop ++;
	}

label_mtn_wb_tune_bh_b1w_stop_srch:

	stpMtnTuneOutput->dTuningObj = dBestTuneObj1By1;

			gstSystemScope.uiNumData = 7;
			gstSystemScope.uiDataLen = 3000;
			gstSystemScope.dSamplePeriod_ms = 1;
			mtn_tune_contact_save_data(stCommHandle, &gstSystemScope, stTeachContactParameter.iAxis, 
				FILE_NAME_SEARCH_CONTACT_FINAL, "%% ACSC Controller, Axis- %d: RPOS(AXIS), PE(AXIS), RVEL(AXIS), FVEL(AXIS), DCOM(AXIS), DOUT(AXIS), SLAFF(AXIS) \n ");

	stpMtnTuneOutput->stResultResponse = astMotionTuneHistory[idxBestParaSet].stServoTuneIndex;
	// Tuning pass (iFlagTuningIsFail = FALSE ) iff the obj is better than initial value  <=> (dTuningObj < dInitialObj)
	if(stpMtnTuneOutput->dTuningObj < stpMtnTuneOutput->dInitialObj)
	{
		stpMtnTuneOutput->iFlagTuningIsFail = FALSE;
		// Failure is FALSE, meaning PASS, download the best parameter set
		stpMtnTuneOutput->stBestParameterSet = astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara; // Only update when TuneOut better than init
		mtn_tune_round_parameter_to_nearest_1(&astMotionTuneHistory[idxBestParaSet].stServoPara.stMtnPara, &stpMtnTuneOutput->stBestParameterSet);  // APP_Z_BOND_ACS_I // 	// Parameter rounding

		mtn_b1w_write_para_bh_servo_readve(stCommHandle, &stpMtnTuneOutput->stBestParameterSet, iObjSectionFlagB1W); // stCurrTuneBondHeadParaB1W
		//		mtn_b1w_write_para_bh_servo(
	}
	else
	{
		stpMtnTuneOutput->iFlagTuningIsFail = TRUE;
	}

	// Restore the safety parameters
	mtnapi_download_safety_parameter_acs_per_axis(stCommHandle, iAxisTuningACS, &stSafetyParaBak);

	if(stpMtnTuneInput->iDebugFlag == TRUE)
	{
		mtn_tune_save_tuning_history(stpMtnTuneInput, stpMtnTuneOutput, idxCurrCase1By1);
	}

	return sRet;
}

#ifdef __B1W_STOP_SRCH__
#endif