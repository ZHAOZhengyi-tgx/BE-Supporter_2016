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

#include "MtnTune.h"
#include "MtnWbDef.h"
#include "MtnInitAcs.h"

#include "math.h" // fabs(

#define PASS_FAIL_TH_BND_Z_PERFORM_IDLE                   3000
#define PASS_FAIL_TH_BND_Z_PERFORM_1ST_CONTACT            10.0
#define PASS_FAIL_TH_BND_Z_PERFORM_SRCH_HT                3000
#define PASS_FAIL_TH_BND_Z_PERFORM_LOOPING                3000
#define PASS_FAIL_TH_BND_Z_PERFORM_LOOP_TOP               3000
#define PASS_FAIL_TH_BND_Z_PERFORM_TRAJECTORY             3000
#define PASS_FAIL_TH_BND_Z_PERFORM_TAIL                   3000
#define PASS_FAIL_TH_BND_Z_PERFORM_RESET                  600
#define PASS_FAIL_TH_BND_Z_PERFORM_2ND_CONTACT            10.0

#define PASS_FAIL_TH_BND_Z_PERFORM_DEFORM_BNDING          (20.0)
#define PASS_FAIL_TH_BND_Z_PERFORM_DEFORM_DRVIN           (50.0)

// Peak to Peak Position Error 2um, 4cnt@ 0.5um/count
#define PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_P2P          4
#define PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_STD          6

extern double fRefVelZ[LEN_UPLOAD_ARRAY];
extern double fPosnErrZ[LEN_UPLOAD_ARRAY];
extern double fFeedVelZ[LEN_UPLOAD_ARRAY];
extern AXIS_INFO_WIRE_BOND astAxisInfoWireBond[MAX_SERVO_AXIS_WIREBOND];
extern MTN_TUNE_GENETIC_INPUT astMtnTuneInput[MAX_SERVO_AXIS_WIREBOND][MAX_BLK_PARAMETER]; 
char *astrTextContactOrNot[] =
{
	"ActBnd",
	"DryRun"
};  // 20110531

char *astrTextBondWireOrBall[] =
{
	"Wire",
	"Ball"
}; // 20120915

char *astrTextFlagIsPass[] =
{
	"F:(",
	"P:)"
};  // 20110531

static int idxStartSearch, idxEndSearch;

static double dObjBondHead[WB_BH_2ND_CONTACT + 1];

void _mtn_tune_update_calc_bh_obj_values(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc)
{
	dObjBondHead[WB_BH_SRCH_HT] = 
		mtn_tune_calc_b1w_bh_performance(stpWbWaveformPerformanceCalc, &astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_SRCH_HT].stTuningTheme, WB_BH_SRCH_HT);
	dObjBondHead[WB_BH_1ST_CONTACT] = 
		mtn_tune_calc_b1w_bh_performance(stpWbWaveformPerformanceCalc, &astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_1ST_CONTACT].stTuningTheme, WB_BH_1ST_CONTACT);
	dObjBondHead[WB_BH_LOOPING] = 
		mtn_tune_calc_b1w_bh_performance(stpWbWaveformPerformanceCalc, &astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_LOOPING].stTuningTheme, WB_BH_LOOPING);
	dObjBondHead[WB_BH_LOOP_TOP] = 
		mtn_tune_calc_b1w_bh_performance(stpWbWaveformPerformanceCalc, &astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_LOOP_TOP].stTuningTheme, WB_BH_LOOP_TOP);
	dObjBondHead[WB_BH_TRAJECTORY] = 
		mtn_tune_calc_b1w_bh_performance(stpWbWaveformPerformanceCalc, &astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_TRAJECTORY].stTuningTheme, WB_BH_TRAJECTORY);
	dObjBondHead[WB_BH_TAIL] = 
		mtn_tune_calc_b1w_bh_performance(stpWbWaveformPerformanceCalc, &astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_TAIL].stTuningTheme, WB_BH_TAIL);
	dObjBondHead[WB_BH_RESET] = 
		mtn_tune_calc_b1w_bh_performance(stpWbWaveformPerformanceCalc, &astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_RESET].stTuningTheme, WB_BH_RESET);
	dObjBondHead[WB_BH_2ND_CONTACT] = 
		mtn_tune_calc_b1w_bh_performance(stpWbWaveformPerformanceCalc, &astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_2ND_CONTACT].stTuningTheme, WB_BH_2ND_CONTACT);
}

CString _mtn_tune_opt_wire_bh_srch_reset(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc)
{
BOND_HEAD_PERFORMANCE *stpBondHeadPerformance = &(stpWbWaveformPerformanceCalc->stBondHeadPerformance);
TIME_POINTS_OF_BONDHEAD_Z *stpTimePointsOfBondHeadZ = &(stpWbWaveformPerformanceCalc->stTimePointsOfBondHeadZ);
POSNS_OF_BONDHEAD_Z *stpPosnsOfBondHeadZ = &(stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ);
WB_TUNE_B1W_BH_PASS_FLAG stWbTuneB1wPassFlag;  // 20120512

CString csTemp;
int ii, iFlagIsFeasibleParameter;  // 20120512
	double dCtrlSampleTime_ms = sys_get_controller_ts();


	_mtn_tune_update_calc_bh_obj_values(stpWbWaveformPerformanceCalc);
	iFlagIsFeasibleParameter = mtn_wb_tune_b1w_check_is_feasible_parameter(dObjBondHead, &stWbTuneB1wPassFlag);  // 20120512

	// aPosn_1stSearchHeight = 
	csTemp.Format("Mv1stSrcHt_OUS_Obj_cFrom_cTo= [%3.0f,  %4.0f, %4.0f, %4.0f]; %% %s @ Dist_um =%3.0f, tDur_ms = %2.1f, Time [%d, %d]; \r\n", // 20120107
		stpBondHeadPerformance->f1stBondMoveToSearchHeightOverUnderShoot, dObjBondHead[WB_BH_SRCH_HT], 
		stpPosnsOfBondHeadZ->fStartFireLevel_Z, stpPosnsOfBondHeadZ->f1stBondSearchHeight_Z, 
		astrTextFlagIsPass[stWbTuneB1wPassFlag.aiTuneB1W_PassFlag[WB_BH_SRCH_HT]],  // 20120512
		(stpPosnsOfBondHeadZ->f1stBondSearchHeight_Z - stpPosnsOfBondHeadZ->fStartFireLevel_Z ) /astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm * 1000.0,  
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfBondHeadZ->idxStartMove1stBondSearchHeight - stpTimePointsOfBondHeadZ->idxEndMove1stBondSearchHeight),
		stpTimePointsOfBondHeadZ->idxStartMove1stBondSearchHeight, stpTimePointsOfBondHeadZ->idxEndMove1stBondSearchHeight);
//    csTemp.AppendFormat("fOverUnderShoot = %8.1f, dObj = %8.1f \r\n", stpBondHeadPerformance->f1stBondMoveToSearchHeightOverUnderShoot, dObjBondHead[WB_BH_SRCH_HT]);

	csTemp.AppendFormat("Traj2ndB_fOUS_dObj_cFrom_cTo = [%3.0f, %4.0f, %4.0f, %4.0f]; %% %s @ Dist_um =%3.0f, tDur_ms = %2.1f, Time [%d, %d]; \r\n",
		stpBondHeadPerformance->f2ndBondMoveToSearchHeightOverUnderShoot, dObjBondHead[WB_BH_TRAJECTORY], 
		stpPosnsOfBondHeadZ->fLoopTopPosn_Z, stpPosnsOfBondHeadZ->f2ndBondSearchHeightPosn_Z,
		astrTextFlagIsPass[stWbTuneB1wPassFlag.aiTuneB1W_PassFlag[WB_BH_TRAJECTORY]],  // 20120512
		(stpPosnsOfBondHeadZ->f2ndBondSearchHeightPosn_Z - stpPosnsOfBondHeadZ->fLoopTopPosn_Z)/astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm * 1000.0, // 20120107
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfBondHeadZ->idxStartTrajectory - stpTimePointsOfBondHeadZ->idxEndTrajectory2ndBondSearchHeight), 
		stpTimePointsOfBondHeadZ->idxStartTrajectory, stpTimePointsOfBondHeadZ->idxEndTrajectory2ndBondSearchHeight);
//    csTemp.AppendFormat("fOverUnderShoot = %8.1f, dObj = %8.1f  \r\n", stpBondHeadPerformance->f2ndBondMoveToSearchHeightOverUnderShoot, dObjBondHead[WB_BH_TRAJECTORY]);

///	csTemp.AppendFormat("%%%% BondHead Search Property\r\n");
	idxStartSearch = stpTimePointsOfBondHeadZ->idxEndMove1stBondSearchHeight;
	idxEndSearch = stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl;
	csTemp.AppendFormat("f1stSearchSpd = %5.1f, aErr_P2P_AccRMS = [%4.1f, %3.1f]; %% Time [%d, %d], dObj=%6.1f , tDur_ms = %2.1f;\r\n", 
		fRefVelZ[(idxStartSearch + idxEndSearch )/2],
		stpBondHeadPerformance->f1stBondSearchVelErrP2PAfterSrchDelay, stpBondHeadPerformance->f1stBondSearchFeedAccRMS,
		idxStartSearch, idxEndSearch, dObjBondHead[WB_BH_1ST_CONTACT],
		dCtrlSampleTime_ms * fabs((double)idxStartSearch - idxEndSearch)); // idxStart2ndBondSearchContact
	csTemp.AppendFormat("f1stContactTH = %2.0f; af1stSrchPosnErr = [%2.0f,", 
		stpWbWaveformPerformanceCalc->fBondHeadSetting_1stB_SrchSafeTH, fPosnErrZ[idxStartSearch]);
	for(ii= idxStartSearch+1; ii< idxEndSearch; ii++)
	{
		csTemp.AppendFormat("%2.0f,", fPosnErrZ[ii]);
	}
	csTemp.AppendFormat("%2.0f];\r\n", fPosnErrZ[idxEndSearch]);

	csTemp.AppendFormat("af1stSrchVelErr_fm_%d = [%4.1f,", (idxStartSearch-1), fRefVelZ[idxStartSearch-1] - fFeedVelZ[idxStartSearch-1]);  // 20110802
	for(ii= idxStartSearch; ii< idxEndSearch; ii++)
	{
		csTemp.AppendFormat("%4.1f,", fRefVelZ[ii] - fFeedVelZ[ii]);
	}
	csTemp.AppendFormat("%4.1f];\r\n", fRefVelZ[idxEndSearch] - fFeedVelZ[idxEndSearch]);
 

	idxStartSearch = stpTimePointsOfBondHeadZ->idxStart2ndBondSearchContact, idxEndSearch = stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl;
	csTemp.AppendFormat("f2ndSearch: Spd = %5.1f, Err(P2P,RMS-Acc) = (%4.1f, %3.1f); %% Time [%d, %d], dObj=%6.1f, tDur_ms = %2.1f;  \r\n", 
		fRefVelZ[(idxStartSearch + idxEndSearch )/2],
		stpBondHeadPerformance->f2ndBondSearchVelErrP2PAfterSrchDelay, stpBondHeadPerformance->f2ndBondSearchFeedAccRMS,
		stpTimePointsOfBondHeadZ->idxStart2ndBondSearchContact, stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl, 
		dObjBondHead[WB_BH_2ND_CONTACT],
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl - stpTimePointsOfBondHeadZ->idxStart2ndBondSearchContact)); // idxStart2ndBondSearchContact
	csTemp.AppendFormat("f2ndContactTH = %2.0f; af2ndSrchPosnErr = [%2.0f,", 
		stpWbWaveformPerformanceCalc->fBondHeadSetting_2ndB_SrchSafeTH, fPosnErrZ[idxStartSearch]);
	for(ii= idxStartSearch+1; ii< idxEndSearch; ii++)
	{
		csTemp.AppendFormat("%2.0f,", fPosnErrZ[ii]);
	}
	csTemp.AppendFormat("%2.0f];\r\n", fPosnErrZ[idxEndSearch]);

	csTemp.AppendFormat("af2ndSrchVelErr_fm_%d = [%4.1f,", (idxStartSearch-1), fRefVelZ[idxStartSearch-1] - fFeedVelZ[idxStartSearch-1]);  // 20110802
	for(ii= idxStartSearch; ii< idxEndSearch; ii++)
	{
		csTemp.AppendFormat("%4.1f,", fRefVelZ[ii] - fFeedVelZ[ii]);
	}
	csTemp.AppendFormat("%4.1f];\r\n", fRefVelZ[idxEndSearch] - fFeedVelZ[idxEndSearch]);


/////////////////////////////  
	double dMaxDeform1stB = 0, dMaxDeform2ndB = 0;
	for(ii = stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl; ii < stpTimePointsOfBondHeadZ->idxEnd1stBondForceCtrl; ii++)
	{
		if(dMaxDeform1stB > stpBondHeadPerformance->adDeformPosn1stB[ii - stpTimePointsOfBondHeadZ->idxStartCalcDeform1stB])
		{
			dMaxDeform1stB = stpBondHeadPerformance->adDeformPosn1stB[ii - stpTimePointsOfBondHeadZ->idxStartCalcDeform1stB];
		}
	}
	csTemp.AppendFormat("%%%% BondHead Force Ctrl Performance Point\r\n");
	csTemp.AppendFormat("aTimeDeform_DrvInBond_1stB = [%3.0f, %3.0f, %3.0f, %3.0f] %% %s\r\n", 
		(stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl - stpTimePointsOfBondHeadZ->idxStartCalcDeform1stB)/2.0, stpBondHeadPerformance->adDeformPosn1stB[0],
		(stpTimePointsOfBondHeadZ->idxEnd1stBondForceCtrl - stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl)/2.0 - 1, fabs(dMaxDeform1stB),
				astrTextFlagIsPass[stWbTuneB1wPassFlag.aiTuneB1W_PassFlag[WB_BH_1ST_CONTACT]]  // 20120512
				);

	for(ii = stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl; ii < stpTimePointsOfBondHeadZ->idxEnd2ndBondForceCtrl; ii++)
	{
		if(dMaxDeform2ndB > stpBondHeadPerformance->adDeformPosn2ndB[ii - stpTimePointsOfBondHeadZ->idxStartCalcDeform2ndB])
		{
			dMaxDeform2ndB = stpBondHeadPerformance->adDeformPosn2ndB[ii - stpTimePointsOfBondHeadZ->idxStartCalcDeform2ndB];
		}
	}
	csTemp.AppendFormat("aTimeDeform_DrvInBond_2ndB = [%3.0f, %3.0f, %3.0f, %3.0f] %% %s\r\n", 
		(stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl - stpTimePointsOfBondHeadZ->idxStartCalcDeform2ndB)/2.0, stpBondHeadPerformance->adDeformPosn2ndB[0],
		(stpTimePointsOfBondHeadZ->idxEnd2ndBondForceCtrl - stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl)/2.0 - 1, fabs(dMaxDeform2ndB),
		astrTextFlagIsPass[stWbTuneB1wPassFlag.aiTuneB1W_PassFlag[WB_BH_2ND_CONTACT]]  // 20120512
		);

// #define PASS_FAIL_TH_BND_Z_PERFORM_IDLE                   3000


	if(fabs(dObjBondHead[WB_BH_SRCH_HT]) > PASS_FAIL_TH_BND_Z_PERFORM_SRCH_HT)
	{;
	}

	if(fabs(dObjBondHead[WB_BH_1ST_CONTACT]) > PASS_FAIL_TH_BND_Z_PERFORM_1ST_CONTACT)
	{;
	}
	if(fabs(dObjBondHead[WB_BH_LOOPING]) > PASS_FAIL_TH_BND_Z_PERFORM_LOOPING)
	{;
	}
	if(fabs(dObjBondHead[WB_BH_LOOP_TOP]) > PASS_FAIL_TH_BND_Z_PERFORM_LOOP_TOP)
	{;
	}
	if(fabs(dObjBondHead[WB_BH_TRAJECTORY] )> PASS_FAIL_TH_BND_Z_PERFORM_TRAJECTORY)
	{;
	}
	if(fabs(dObjBondHead[WB_BH_TAIL]) > PASS_FAIL_TH_BND_Z_PERFORM_TAIL)
	{;
	}
	if(fabs(dObjBondHead[WB_BH_RESET] )> PASS_FAIL_TH_BND_Z_PERFORM_RESET)
	{
		csTemp.AppendFormat("%%%%FAIL: RESET MOTION,  \r\n", fabs(dObjBondHead[WB_BH_RESET]), PASS_FAIL_TH_BND_Z_PERFORM_RESET);  
	}
	if(fabs(dObjBondHead[WB_BH_2ND_CONTACT]) > PASS_FAIL_TH_BND_Z_PERFORM_2ND_CONTACT)
	{;
	}

	if(fabs(dMaxDeform1stB) > PASS_FAIL_TH_BND_Z_PERFORM_DEFORM_BNDING)
	{
		csTemp.AppendFormat("%%%%FAIL: 1stBonding Deform, %4.1f > %3.0f \r\n", fabs(dMaxDeform1stB), PASS_FAIL_TH_BND_Z_PERFORM_DEFORM_BNDING);  
	}
	if(fabs(dMaxDeform2ndB) > PASS_FAIL_TH_BND_Z_PERFORM_DEFORM_BNDING)
	{
		csTemp.AppendFormat("%%%%FAIL: 2ndBonding Deform, %4.1f > %3.0f \r\n", fabs(dMaxDeform2ndB), PASS_FAIL_TH_BND_Z_PERFORM_DEFORM_BNDING);  
	}

	if(fabs(stpBondHeadPerformance->adDeformPosn1stB[0]) > PASS_FAIL_TH_BND_Z_PERFORM_DEFORM_DRVIN)
	{
		csTemp.AppendFormat("%%%%FAIL: 1stDrvIn Deform %4.1f > %3.0f \r\n", fabs(stpBondHeadPerformance->adDeformPosn1stB[0]), PASS_FAIL_TH_BND_Z_PERFORM_DEFORM_DRVIN);  
	}
	if(fabs(stpBondHeadPerformance->adDeformPosn2ndB[0]) > PASS_FAIL_TH_BND_Z_PERFORM_DEFORM_DRVIN)
	{
		csTemp.AppendFormat("%%%%FAIL: 2ndDrvIn Deform %4.1f > %3.0f \r\n", fabs(stpBondHeadPerformance->adDeformPosn2ndB[0]), PASS_FAIL_TH_BND_Z_PERFORM_DEFORM_DRVIN);  
	}

	csTemp.AppendFormat("af1BDfrmFrm_%d = [%3.0f,", stpTimePointsOfBondHeadZ->idxStartCalcDeform1stB, stpBondHeadPerformance->adDeformPosn1stB[0]);
	ii = 1;
	while(ii < (stpTimePointsOfBondHeadZ->idxEnd1stBondForceCtrl - stpTimePointsOfBondHeadZ->idxStartCalcDeform1stB)) // 20111025
		//(fabs(stpBondHeadPerformance->adDeformPosn1stB[ii]) > 0.0001) 
		//|| (fabs(stpBondHeadPerformance->adDeformPosn1stB[ii + 1]) > 0.0001)
		//|| (fabs(stpBondHeadPerformance->adDeformPosn1stB[ii + 2]) > 0.0001)
		//|| (fabs(stpBondHeadPerformance->adDeformPosn1stB[ii + 3]) > 0.0001)
		//|| (fabs(stpBondHeadPerformance->adDeformPosn1stB[ii + 4]) > 0.0001)
		//)    /// 20110526
	{
		csTemp.AppendFormat("%3.0f,", stpBondHeadPerformance->adDeformPosn1stB[ii]);
		ii ++;
	}
	csTemp.AppendFormat("0]; \r\n");  /// 20110526
    csTemp.AppendFormat("af1stBondForce_P2P_Std = [%8.2f, %8.2f]; %% @ Time [%d, %d]\r\n", stpBondHeadPerformance->f1stBondForceCtrlPosnRipplePtoP, 
		stpBondHeadPerformance->f1stBondForceCtrlPosnRippleStd, 
		stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl, stpTimePointsOfBondHeadZ->idxEnd1stBondForceCtrl);

	csTemp.AppendFormat("af2BDfrmFrm_%d = [%3.0f,", stpTimePointsOfBondHeadZ->idxStartCalcDeform2ndB, stpBondHeadPerformance->adDeformPosn2ndB[0]);
	ii = 1;
	while(ii < (stpTimePointsOfBondHeadZ->idxEnd2ndBondForceCtrl - stpTimePointsOfBondHeadZ->idxStartCalcDeform2ndB)) // (fabs(stpBondHeadPerformance->adDeformPosn2ndB[ii]) > 0.0001) 
		//|| (fabs(stpBondHeadPerformance->adDeformPosn2ndB[ii + 1]) > 0.0001)
		//|| (fabs(stpBondHeadPerformance->adDeformPosn2ndB[ii + 2]) > 0.0001)
		//|| (fabs(stpBondHeadPerformance->adDeformPosn2ndB[ii + 3]) > 0.0001)
		//|| (fabs(stpBondHeadPerformance->adDeformPosn2ndB[ii + 4]) > 0.0001)
		//|| (fabs(stpBondHeadPerformance->adDeformPosn2ndB[ii + 5]) > 0.0001)
		//|| (fabs(stpBondHeadPerformance->adDeformPosn2ndB[ii + 6]) > 0.0001)
		//)/// 20110526
	{
		csTemp.AppendFormat("%3.0f,", stpBondHeadPerformance->adDeformPosn2ndB[ii]);
		ii ++;
	}
	csTemp.AppendFormat("0]; \r\n");  /// 20110526

	/////// Bonding ContactForceControl, minimize the ripple for contact without ball
    csTemp.AppendFormat("af2ndBondForce_P2P_Std = [%8.2f, %8.2f]; %% @ Time [%d, %d] , tDur_ms = %2.1f; \r\n", stpBondHeadPerformance->f2ndBondForceCtrlPosnRipplePtoP, 
		stpBondHeadPerformance->f2ndBondForceCtrlPosnRippleStd,
		stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl, stpTimePointsOfBondHeadZ->idxEnd2ndBondForceCtrl,
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl - stpTimePointsOfBondHeadZ->idxEnd2ndBondForceCtrl));

	return csTemp;
}

CString _mtn_tune_opt_wire_bh_looping(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc)
{
BOND_HEAD_PERFORMANCE *stpBondHeadPerformance = &(stpWbWaveformPerformanceCalc->stBondHeadPerformance);
TIME_POINTS_OF_BONDHEAD_Z *stpTimePointsOfBondHeadZ = &(stpWbWaveformPerformanceCalc->stTimePointsOfBondHeadZ);
POSNS_OF_BONDHEAD_Z *stpPosnsOfBondHeadZ = &(stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ);
TIME_POINTS_OF_TABLE *stpTimePointsOfTable = &(stpWbWaveformPerformanceCalc->stTimePointsOfTable);

WB_TUNE_B1W_BH_PASS_FLAG stWbTuneB1wPassFlag;  // 20120512

CString csTemp;
int ii, iFlagIsFeasibleParameter;  // 20120512
	double dCtrlSampleTime_ms = sys_get_controller_ts();
	_mtn_tune_update_calc_bh_obj_values(stpWbWaveformPerformanceCalc);
	iFlagIsFeasibleParameter = mtn_wb_tune_b1w_check_is_feasible_parameter(dObjBondHead, &stWbTuneB1wPassFlag);  // 20120512

    csTemp.AppendFormat("%%%% BondHead Tail-Reset Performance\r\n");
	csTemp.AppendFormat("Tail_fOUS_dObj_cFrom_cTo= [%3.0f, %3.0f, %4.1f, %4.1f]; %% %s @ Dist_um =%3.0f, tDur_ms = %2.1f, aTime =[%d, %d];  \r\n", // 20120107
		stpBondHeadPerformance->fTailOverUnderShoot, dObjBondHead[WB_BH_TAIL], 
		stpPosnsOfBondHeadZ->f2ndBondContactPosn_Z, stpPosnsOfBondHeadZ->fTailPosn_Z,
		astrTextFlagIsPass[stWbTuneB1wPassFlag.aiTuneB1W_PassFlag[WB_BH_TAIL]], 
		(stpPosnsOfBondHeadZ->fTailPosn_Z - stpPosnsOfBondHeadZ->f2ndBondContactPosn_Z)/astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm * 1000.0, // 20120107
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfBondHeadZ->idxStartTail - stpTimePointsOfBondHeadZ->idxEndTail ), 
		stpTimePointsOfBondHeadZ->idxStartTail, stpTimePointsOfBondHeadZ->idxEndTail);
//    csTemp.AppendFormat("fOverUnderShoot = %8.1f, dObj = %8.1f  \r\n", stpBondHeadPerformance->fTailOverUnderShoot, dObjBondHead[WB_BH_TAIL]);

	csTemp.AppendFormat("ResetFL_fOUS_dObj_cFrom_cTo = [%3.0f, %3.0f, %4.1f, %4.1f]; %% %s @ Dist_um =%3.0f, tDur_ms = %2.1f, Time [%d, %d];  \r\n",  // 20120107
		stpBondHeadPerformance->fEndFireLevelOverUnderShoot, dObjBondHead[WB_BH_RESET], 
		stpPosnsOfBondHeadZ->fTailPosn_Z, stpPosnsOfBondHeadZ->fEndFireLevel_Z,
		astrTextFlagIsPass[stWbTuneB1wPassFlag.aiTuneB1W_PassFlag[WB_BH_RESET]], 
		(stpPosnsOfBondHeadZ->fEndFireLevel_Z - stpPosnsOfBondHeadZ->fTailPosn_Z)/astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm * 1000.0, // 20120107
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfBondHeadZ->idxStartFireLevel- stpTimePointsOfBondHeadZ->idxEndFireLevel ),
		stpTimePointsOfBondHeadZ->idxStartFireLevel, stpTimePointsOfBondHeadZ->idxEndFireLevel);
//    csTemp.AppendFormat("fOverUnderShoot = %8.1f, dObj = %8.1f; \r\n", stpBondHeadPerformance->fEndFireLevelOverUnderShoot, dObjBondHead[WB_BH_RESET]);
	int idxEndFireLevel = stpTimePointsOfBondHeadZ->idxEndFireLevel;
	csTemp.AppendFormat("aIdlePosnErr = [%3.1f, ", fPosnErrZ[idxEndFireLevel]); // 20120107
	for(ii = 1; ii< 20; ii++)
	{
		csTemp.AppendFormat("%3.1f, ", fPosnErrZ[idxEndFireLevel + ii]); // 20120107
	}
	csTemp.AppendFormat("%3.1f]\r\n", fPosnErrZ[idxEndFireLevel + 20]); // 20120107

    csTemp.AppendFormat("aReverseHeight_OUS_Obj_aPosnFromTo = [%3.0f, %4.0f, %4.0f, %4.0f]; %% %s @ Time [%d, %d], tDur_ms = %2.1f; \r\n", 
		stpBondHeadPerformance->fReverseHeightOverUnderShoot, dObjBondHead[WB_BH_LOOPING],
		stpPosnsOfBondHeadZ->f1stBondContactPosn_Z, stpPosnsOfBondHeadZ->fReverseHeightPosn_Z, 
		astrTextFlagIsPass[stWbTuneB1wPassFlag.aiTuneB1W_PassFlag[WB_BH_LOOPING]], 
		stpTimePointsOfBondHeadZ->idxStartReverseHeight, stpTimePointsOfBondHeadZ->idxEndReverseHeight,
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfBondHeadZ->idxStartReverseHeight - stpTimePointsOfBondHeadZ->idxEndReverseHeight ));
//    csTemp.AppendFormat("fOverUnderShoot = %3.0f, dObj = %4.0f  \r\n", stpBondHeadPerformance->fReverseHeightOverUnderShoot, dObjBondHead[WB_BH_LOOPING]);

    csTemp.AppendFormat("aPosn_KinkHeight = [%4.0f, %4.0f]; %% @ Time [%d, %d], tDur_ms = %2.1f; \r\n", stpPosnsOfBondHeadZ->fReverseHeightPosn_Z, stpPosnsOfBondHeadZ->fKinkHeightPosn_Z, 
		stpTimePointsOfBondHeadZ->idxStartKinkHeight, stpTimePointsOfBondHeadZ->idxEndKinkHeight,
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfBondHeadZ->idxStartKinkHeight - stpTimePointsOfBondHeadZ->idxEndKinkHeight));
    csTemp.AppendFormat("fOverUnderShoot = %4.0f \r\n", stpBondHeadPerformance->fKinkHeightOverUnderShoot);

    csTemp.AppendFormat("aLoopTop_OUS_Obj_PosnFrmTo = [%4.0f, %4.0f, %4.0f, %4.0f]; %% %s @ Dist_um =%3.0f, tDur_ms = %2.1f, Time [%d, %d]; \r\n",  // 20120107
		stpBondHeadPerformance->fLoopTopOverUnderShoot, dObjBondHead[WB_BH_LOOP_TOP], 
		stpPosnsOfBondHeadZ->fKinkHeightPosn_Z, stpPosnsOfBondHeadZ->fLoopTopPosn_Z,
		astrTextFlagIsPass[stWbTuneB1wPassFlag.aiTuneB1W_PassFlag[WB_BH_LOOP_TOP]], 
		(stpPosnsOfBondHeadZ->fLoopTopPosn_Z - stpPosnsOfBondHeadZ->fKinkHeightPosn_Z) /astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm * 1000.0,
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfBondHeadZ->idxStartMoveLoopTop - stpTimePointsOfBondHeadZ->idxEndMoveLoopTop), 
		stpTimePointsOfBondHeadZ->idxStartMoveLoopTop, stpTimePointsOfBondHeadZ->idxEndMoveLoopTop);
//    csTemp.AppendFormat("fOverUnderShoot = %4.0f, dObj = %4.0f  \r\n", stpBondHeadPerformance->fLoopTopOverUnderShoot, dObjBondHead[WB_BH_LOOP_TOP]);

	/// Looping Position Error count by table start reverse movint
	int idxTableStartRd = (stpTimePointsOfTable->idxStartReverseMoveX + stpTimePointsOfBondHeadZ->idxStartReverseHeight)/2;
	int idxBondHeadStartLoopTop = stpTimePointsOfBondHeadZ->idxStartMoveLoopTop;
	csTemp.AppendFormat("LoopingPosnErr = [%2.0f,", fPosnErrZ[idxTableStartRd - 1]);
	for(ii= idxTableStartRd; ii< idxBondHeadStartLoopTop; ii++)
	{
		csTemp.AppendFormat("%2.0f,", fPosnErrZ[ii]);
	}
	csTemp.AppendFormat("%2.0f];\r\n", fPosnErrZ[idxBondHeadStartLoopTop]);

	csTemp.AppendFormat("LoopingFbVel = [%2.0f,", fFeedVelZ[idxTableStartRd - 1]);
	for(ii= idxTableStartRd; ii< idxBondHeadStartLoopTop; ii++)
	{
		csTemp.AppendFormat("%2.0f,", fFeedVelZ[ii]);
	}
	csTemp.AppendFormat("%2.0f];\r\n", fFeedVelZ[idxBondHeadStartLoopTop]);

	int idxBndEndMoveToLoopTop, idxBndStartTraj;
	idxBndEndMoveToLoopTop = stpTimePointsOfBondHeadZ->idxEndMoveLoopTop;
	idxBndStartTraj = stpTimePointsOfBondHeadZ->idxStartTrajectory;
	csTemp.AppendFormat("LoopTopSettlingPosnErr = [%2.0f,", fPosnErrZ[idxBndEndMoveToLoopTop - 1]);
	for(ii= idxBndEndMoveToLoopTop; ii< idxBndStartTraj; ii++)
	{
		csTemp.AppendFormat("%2.0f,", fPosnErrZ[ii]);
	}
	csTemp.AppendFormat("%2.0f];\r\n", fFeedVelZ[idxBndStartTraj]);
	csTemp.AppendFormat("LoopTopSettlingVelErr = [%2.0f,", fFeedVelZ[idxBndEndMoveToLoopTop - 1]);
	for(ii= idxBndEndMoveToLoopTop; ii< idxBndStartTraj; ii++)
	{
		csTemp.AppendFormat("%2.0f,", fFeedVelZ[ii]);
	}
	csTemp.AppendFormat("%2.0f];\r\n", fFeedVelZ[idxBndStartTraj]);

	return csTemp;
}

CString _mtn_tune_opt_wire_xy_settling_1st_bond(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc)
{
//BOND_HEAD_PERFORMANCE *stpBondHeadPerformance = &(stpWbWaveformPerformanceCalc->stBondHeadPerformance);
TIME_POINTS_OF_BONDHEAD_Z *stpTimePointsOfBondHeadZ = &(stpWbWaveformPerformanceCalc->stTimePointsOfBondHeadZ);

POSNS_OF_BONDHEAD_Z *stpPosnsOfBondHeadZ = &(stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ);
POSNS_OF_TABLE *stpPosnsOfTable = &(stpWbWaveformPerformanceCalc->stPosnsOfTable);
TIME_POINTS_OF_TABLE *stpTimePointsOfTable = &(stpWbWaveformPerformanceCalc->stTimePointsOfTable);
TABLE_WB_PERFORMANCE *stpTableXPerformance = &(stpWbWaveformPerformanceCalc->stTableXPerformance);
TABLE_WB_PERFORMANCE *stpTableYPerformance = &(stpWbWaveformPerformanceCalc->stTableYPerformance);

CString csTemp;
int ii;
	double dCtrlSampleTime_ms = sys_get_controller_ts();
	_mtn_tune_update_calc_bh_obj_values(stpWbWaveformPerformanceCalc);
	int nMaxIdxTblSettling = stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl - stpTimePointsOfBondHeadZ->idxEndMove1stBondSearchHeight + 5;

///// XY Table Settling For 1st Bond
//int ii;
	csTemp.AppendFormat("%%%%1st Bond XY-Table Settling \r\n");
	idxStartSearch = stpTimePointsOfBondHeadZ->idxEndMove1stBondSearchHeight; // After SearchDelay of 6 ms
	idxEndSearch = stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl;
	csTemp.AppendFormat("%%f1stSearchSpeed_Time [%d, %d], tDur_ms = %2.1f; \r\n", 
		idxStartSearch, idxEndSearch,
		dCtrlSampleTime_ms * fabs((double) idxStartSearch - idxEndSearch)); // idxStart2ndBondSearchContact
    csTemp.AppendFormat("af1stBondTime_sample = [%d, %d]\r\n", 
		stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl, stpTimePointsOfBondHeadZ->idxEnd1stBondForceCtrl);
	// X
    csTemp.AppendFormat("TBL_X_MOVE_TO_1ST_B = [%6.0f, %6.0f]; %% @ Time [%d, %d], Dist_mm = %3.1f, tDur_ms = %2.1f; \r\n",  // 20120105
		stpPosnsOfTable->fStartPosition_X, stpPosnsOfTable->f1stBondPosition_X, 
		stpTimePointsOfTable->idxStartMoveX1stBond, stpTimePointsOfTable->idxEndMoveX_1stBondPosn,
		fabs(stpPosnsOfTable->fStartPosition_X - stpPosnsOfTable->f1stBondPosition_X)/ astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm,
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfTable->idxStartMoveX1stBond - stpTimePointsOfTable->idxEndMoveX_1stBondPosn ));
	csTemp.AppendFormat("afPosnErrTable_X_MoveTo1stBond = [%5.1f, ", stpTableXPerformance->afTablePosnErrSettle_MoveTo1stBond[0]);
	for(ii = 0; ii<MAX_LENGTH_TABLE_SETTLE - 1; ii++)
	{
		csTemp.AppendFormat("%5.1f, ", stpTableXPerformance->afTablePosnErrSettle_MoveTo1stBond[ii]);
	}
	csTemp.AppendFormat("%5.1f];\r\n", stpTableXPerformance->afTablePosnErrSettle_MoveTo1stBond[MAX_LENGTH_TABLE_SETTLE - 1]);


	//// 20111130, Table.X
	csTemp.AppendFormat("afPosnErrTblX_FromSrch1B_%d = [%5.1f, ", 
		stpTimePointsOfBondHeadZ->idxEndMove1stBondSearchHeight, stpTableXPerformance->afTblPosnErrFromSrch1B[0]);
	for(ii = 0; ii<nMaxIdxTblSettling ; ii++)
	{
		csTemp.AppendFormat("%5.1f, ", stpTableXPerformance->afTblPosnErrFromSrch1B[ii]);
	}
	csTemp.AppendFormat("%5.1f];\r\n", stpTableXPerformance->afTblPosnErrFromSrch1B[nMaxIdxTblSettling]);

	csTemp.AppendFormat("afPosnErrTblX_Contact1B_%d = [%5.1f, ", 
		stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl, stpTableXPerformance->afTblPosnErrContact1B[0]);
	nMaxIdxTblSettling = stpTimePointsOfBondHeadZ->idxEnd1stBondForceCtrl - stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl + 5;
	for(ii = 0; ii<nMaxIdxTblSettling ; ii++)
	{
		csTemp.AppendFormat("%5.1f, ", stpTableXPerformance->afTblPosnErrContact1B[ii]);
	}
	csTemp.AppendFormat("%5.1f];\r\n", stpTableXPerformance->afTblPosnErrContact1B[nMaxIdxTblSettling]);
	csTemp.AppendFormat("afPosnErrTblX_Contact1B_P2P_MaxAbs = [%5.1f, %5.1f]; \r\n", 
		stpTableXPerformance->fTblPosnErrP2PContact1B, stpTableXPerformance->fTblMaxAbsPosnErrContact1B);

	if(fabs(stpTableXPerformance->fTblPosnErrP2PContact1B) <= PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_P2P )  //// 20120214
//		&& fabs(stpTableXPerformance->fTblMaxAbsPosnErrContact1B) <= PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_STD )
	{
		csTemp.AppendFormat("%%%%PASS :) \r\n");  
	}
	else
	{
		csTemp.AppendFormat("%%%%FAIL !!!  Spec. < [%4.1f, %4.1f] \r\n", 
			(double)PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_P2P, (double)PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_STD);  
	}  //// 20120214
#define __OFFSET_BH_AFTER_FORCE_START_XY_SETTLING__  10
	// stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl + __OFFSET_BH_AFTER_FORCE_START_XY_SETTLING__
	for(ii = (stpTimePointsOfTable->idxStartReverseMoveX - stpTimePointsOfTable->idxEndMoveX_1stBondPosn); ii>= 0; ii --)  // 20120716
	{
		if(fabs(stpTableXPerformance->afTablePosnErrSettle_MoveTo1stBond[ii]) > PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_P2P/2)
		{
			stpTableXPerformance->fTable_SettlingTimeMove1B = ii/2.0;
			break;
		}
	}
	csTemp.AppendFormat("X1B_SettlingTime_ms = %4.1f;\r\n", stpTableXPerformance->fTable_SettlingTimeMove1B);

	// Y
    csTemp.AppendFormat("TBL_Y_MOVE_TO_1ST_B = [%6.0f, %6.0f]; %% @ Time [%d, %d], Dist_mm = %3.1f, tDur_ms = %2.1f; \r\n", 
		stpPosnsOfTable->fStartPosition_Y, stpPosnsOfTable->f1stBondPosition_Y,
		stpTimePointsOfTable->idxStartMoveY1stBond, stpTimePointsOfTable->idxEndMoveY_1stBondPosn,
		fabs(stpPosnsOfTable->fStartPosition_Y - stpPosnsOfTable->f1stBondPosition_Y)/ astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm,
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfTable->idxStartMoveY1stBond - stpTimePointsOfTable->idxEndMoveY_1stBondPosn ));
	csTemp.AppendFormat("afPosnErrTable_Y_MoveTo1stBond = [%5.1f, ", stpTableYPerformance->afTablePosnErrSettle_MoveTo1stBond[0]);
	for(ii = 0; ii<MAX_LENGTH_TABLE_SETTLE - 1; ii++)
	{
		csTemp.AppendFormat("%5.1f, ", stpTableYPerformance->afTablePosnErrSettle_MoveTo1stBond[ii]);
	}
	csTemp.AppendFormat("%5.1f];\r\n", stpTableYPerformance->afTablePosnErrSettle_MoveTo1stBond[MAX_LENGTH_TABLE_SETTLE - 1]);

	//// 20111130, Table.Y
	csTemp.AppendFormat("afPosnErrTblY_FromSrch1B_%d = [%5.1f, ", 
		stpTimePointsOfBondHeadZ->idxEndMove1stBondSearchHeight, stpTableYPerformance->afTblPosnErrFromSrch1B[0]);
	nMaxIdxTblSettling = stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl - stpTimePointsOfBondHeadZ->idxEndMove1stBondSearchHeight + 5;
	for(ii = 0; ii<nMaxIdxTblSettling ; ii++)
	{
		csTemp.AppendFormat("%5.1f, ", stpTableYPerformance->afTblPosnErrFromSrch1B[ii]);
	}
	csTemp.AppendFormat("%5.1f];\r\n", stpTableYPerformance->afTblPosnErrFromSrch1B[nMaxIdxTblSettling]);

	csTemp.AppendFormat("afPosnErrTblY_Contact1B_%d = [%5.1f, ", 
		stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl, stpTableYPerformance->afTblPosnErrContact1B[0]);
	nMaxIdxTblSettling = stpTimePointsOfBondHeadZ->idxEnd1stBondForceCtrl - stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl + 5;
	for(ii = 0; ii<nMaxIdxTblSettling ; ii++)
	{
		csTemp.AppendFormat("%5.1f, ", stpTableYPerformance->afTblPosnErrContact1B[ii]);
	}
	csTemp.AppendFormat("%5.1f];\r\n", stpTableYPerformance->afTblPosnErrContact1B[nMaxIdxTblSettling]);
	csTemp.AppendFormat("afPosnErrTblY_Contact1B_P2P_MaxAbs = [%5.1f, %5.1f]; \r\n", 
		stpTableYPerformance->fTblPosnErrP2PContact1B, stpTableYPerformance->fTblMaxAbsPosnErrContact1B);
	if(fabs(stpTableYPerformance->fTblPosnErrP2PContact1B) <= PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_P2P )                   //// 20120214
//		&& fabs(stpTableYPerformance->fTblMaxAbsPosnErrContact1B) <= PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_STD )
	{
		csTemp.AppendFormat("%%%%PASS :) \r\n");  
	}
	else
	{
		csTemp.AppendFormat("%%%%FAIL !!!  Spec. < [%4.1f, %4.1f] \r\n", (double)PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_P2P, (double)PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_STD);  
	}  //// 20120214  (stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl + __OFFSET_BH_AFTER_FORCE_START_XY_SETTLING__
	for(ii = (stpTimePointsOfTable->idxStartReverseMoveY - stpTimePointsOfTable->idxEndMoveX_1stBondPosn); ii>= 0; ii --)  // 20120716
	{
		if(fabs(stpTableYPerformance->afTablePosnErrSettle_MoveTo1stBond[ii]) > PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_P2P/2)
		{
			stpTableYPerformance->fTable_SettlingTimeMove1B = ii/2.0;
			break;
		}
	}
	csTemp.AppendFormat("Y1B_SettlingTime_ms = %4.1f;\r\n", stpTableYPerformance->fTable_SettlingTimeMove1B);

	return csTemp;
}

CString _mtn_tune_opt_wire_xy_settling_2nd_bond(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc)
{
//BOND_HEAD_PERFORMANCE *stpBondHeadPerformance = &(stpWbWaveformPerformanceCalc->stBondHeadPerformance);
TIME_POINTS_OF_BONDHEAD_Z *stpTimePointsOfBondHeadZ = &(stpWbWaveformPerformanceCalc->stTimePointsOfBondHeadZ);

POSNS_OF_BONDHEAD_Z *stpPosnsOfBondHeadZ = &(stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ);
POSNS_OF_TABLE *stpPosnsOfTable = &(stpWbWaveformPerformanceCalc->stPosnsOfTable);
TIME_POINTS_OF_TABLE *stpTimePointsOfTable = &(stpWbWaveformPerformanceCalc->stTimePointsOfTable);
TABLE_WB_PERFORMANCE *stpTableXPerformance = &(stpWbWaveformPerformanceCalc->stTableXPerformance);
TABLE_WB_PERFORMANCE *stpTableYPerformance = &(stpWbWaveformPerformanceCalc->stTableYPerformance);

CString csTemp;
int ii;
	double dCtrlSampleTime_ms = sys_get_controller_ts();
	_mtn_tune_update_calc_bh_obj_values(stpWbWaveformPerformanceCalc);
	int nMaxIdxTblSettling;

	// XY-Table settling 2nd Bond
	csTemp.AppendFormat("\r\n%%%%2nd Bond XY-Table Settling \r\n");
	idxStartSearch = stpTimePointsOfBondHeadZ->idxStart2ndBondSearchContact, idxEndSearch = stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl;
	csTemp.AppendFormat("f2ndSearchSpeed_cnt_sample = %5.1f; %% Time [%d, %d], tDur_ms = %2.1f; \r\n", 
		fRefVelZ[(idxStartSearch + idxEndSearch )/2],
		stpTimePointsOfBondHeadZ->idxStart2ndBondSearchContact, stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl,
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfBondHeadZ->idxStart2ndBondSearchContact - stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl)); // idxStart2ndBondSearchContact
    csTemp.AppendFormat("af2ndBondTime_sample = [%d, %d]\r\n", 
		stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl, stpTimePointsOfBondHeadZ->idxEnd2ndBondForceCtrl);
	// X
    csTemp.AppendFormat("WB_TBL_X_TRAJ_TO_2ND_B = [%6.0f, %6.0f]; %% @ Time [%d, %d], tDur_ms = %2.1f; \r\n", stpPosnsOfTable->f1stBondReversePosition_X, stpPosnsOfTable->fTrajEnd2ndBondPosn_X, 
		stpTimePointsOfTable->idxStartTrajTo2ndBondX, stpTimePointsOfTable->idxEndTraj2ndBondPosnX,
		dCtrlSampleTime_ms * fabs((double) stpTimePointsOfTable->idxStartTrajTo2ndBondX - stpTimePointsOfTable->idxEndTraj2ndBondPosnX));
	csTemp.AppendFormat("afPosnErrTable_X_MoveTo2ndBond = [%5.1f, ", stpTableXPerformance->afTablePosnErrSettle_MoveTo2ndBond[0]);
#define __SRCH_HT_AFTER_6MS_DELAY__    12
	for(ii = 0; ii<MAX_LENGTH_TABLE_SETTLE - 1; ii++) // __SRCH_HT_AFTER_6MS_DELAY__, 20130127
	{
		csTemp.AppendFormat("%5.1f, ", stpTableXPerformance->afTablePosnErrSettle_MoveTo2ndBond[ii]);
	}
	csTemp.AppendFormat("%5.1f];\r\n", stpTableXPerformance->afTablePosnErrSettle_MoveTo2ndBond[MAX_LENGTH_TABLE_SETTLE - 1]);
	//// 20111130,
	csTemp.AppendFormat("afPosnErrTblX_FromSrch2B_%d = [%5.1f, ", 
		stpTimePointsOfBondHeadZ->idxEndTrajectory2ndBondSearchHeight + __SRCH_HT_AFTER_6MS_DELAY__, stpTableXPerformance->afTblPosnErrFromSrch2B[0]);
	nMaxIdxTblSettling = stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl - stpTimePointsOfBondHeadZ->idxEndTrajectory2ndBondSearchHeight + 5;
	for(ii = 0; ii<nMaxIdxTblSettling ; ii++)  // __SRCH_HT_AFTER_6MS_DELAY__
	{
		csTemp.AppendFormat("%5.1f, ", stpTableXPerformance->afTblPosnErrFromSrch2B[ii]);
	}
	csTemp.AppendFormat("%5.1f];\r\n", stpTableXPerformance->afTblPosnErrFromSrch2B[nMaxIdxTblSettling]);

	csTemp.AppendFormat("afPosnErrTblX_Contact2B_%d = [%5.1f, ", 
		stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl, stpTableXPerformance->afTblPosnErrContact2B[0]);
	nMaxIdxTblSettling = stpTimePointsOfBondHeadZ->idxEnd2ndBondForceCtrl - stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl + 5;
	for(ii = 0; ii<nMaxIdxTblSettling ; ii++)
	{
		csTemp.AppendFormat("%5.1f, ", stpTableXPerformance->afTblPosnErrContact2B[ii]);
	}
	csTemp.AppendFormat("%5.1f];\r\n", stpTableXPerformance->afTblPosnErrContact2B[nMaxIdxTblSettling]);
	csTemp.AppendFormat("afPosnErrTblX_Contact2B_P2P_MaxAbs = [%5.1f, %5.1f]; \r\n", 
		stpTableXPerformance->fTblPosnErrP2PContact2B, stpTableXPerformance->fTblMaxAbsPosnErrContact2B);
	if(fabs(stpTableXPerformance->fTblPosnErrP2PContact2B) <= PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_P2P)   //// 20120214
//		&& fabs(stpTableXPerformance->fTblMaxAbsPosnErrContact2B) <= PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_STD )
	{
		csTemp.AppendFormat("%%%%PASS :) \r\n");  
	}
	else
	{
		csTemp.AppendFormat("%%%%FAIL !!!  Spec. < [%4.1f, %4.1f] \r\n",  
			(double)PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_P2P, 
			(double)PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_STD);  
	}  //// 20120214
	for(ii = (stpTimePointsOfBondHeadZ->idxStartTail - stpTimePointsOfTable->idxEndTraj2ndBondPosnX); ii>= 0; ii --)  // 20120716
	{
		if(fabs(stpTableXPerformance->afTblPosnErrFromSrch2B[ii]) > PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_P2P)
		{
			stpTableXPerformance->fTable_SettlingTimeMove2B = ii/2.0;
			break;
		} // afTblPosnErrContact2B
	}
	csTemp.AppendFormat("X2B_SettlingTime_ms = %4.1f;\r\n", stpTableXPerformance->fTable_SettlingTimeMove2B);

	// Y
    csTemp.AppendFormat("\r\nWB_TBL_Y_TRAJ_TO_2ND_B = [%6.0f, %6.0f]; %% @ Time [%d, %d], tDur_ms = %2.1f; \r\n", 
		stpPosnsOfTable->f1stBondReversePosition_Y, stpPosnsOfTable->fTrajEnd2ndBondPosn_Y,
		stpTimePointsOfTable->idxStartTrajTo2ndBondY, stpTimePointsOfTable->idxEndTraj2ndBondPosnY,
		dCtrlSampleTime_ms * fabs((double) stpTimePointsOfTable->idxStartTrajTo2ndBondY - stpTimePointsOfTable->idxEndTraj2ndBondPosnY));
	csTemp.AppendFormat("afPosnErrTable_Y_MoveTo2ndBond = [%5.1f, ", stpTableYPerformance->afTablePosnErrSettle_MoveTo2ndBond[0]);
	for(ii = 0; ii<MAX_LENGTH_TABLE_SETTLE - 1; ii++)
	{
		csTemp.AppendFormat("%5.1f, ", stpTableYPerformance->afTablePosnErrSettle_MoveTo2ndBond[ii]);
	}
	csTemp.AppendFormat("%5.1f];\r\n", stpTableYPerformance->afTablePosnErrSettle_MoveTo2ndBond[MAX_LENGTH_TABLE_SETTLE - 1]);

	//// 20111130, Table.Y
	csTemp.AppendFormat("afPosnErrTblY_FromSrch2B_%d = [%5.1f, ", 
		stpTimePointsOfBondHeadZ->idxEndTrajectory2ndBondSearchHeight, stpTableYPerformance->afTblPosnErrFromSrch2B[0]);
	nMaxIdxTblSettling = stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl - stpTimePointsOfBondHeadZ->idxEndTrajectory2ndBondSearchHeight + 5;
	for(ii = 0; ii<nMaxIdxTblSettling ; ii++)
	{
		csTemp.AppendFormat("%5.1f, ", stpTableYPerformance->afTblPosnErrFromSrch2B[ii]);
	}
	csTemp.AppendFormat("%5.1f];\r\n", stpTableYPerformance->afTblPosnErrFromSrch2B[nMaxIdxTblSettling]);

	csTemp.AppendFormat("afPosnErrTblY_Contact2B_%d = [%5.1f, ", 
		stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl, stpTableYPerformance->afTblPosnErrContact2B[0]);
	nMaxIdxTblSettling = stpTimePointsOfBondHeadZ->idxEnd2ndBondForceCtrl - stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl + 5;
	for(ii = 0; ii<nMaxIdxTblSettling ; ii++)
	{
		csTemp.AppendFormat("%5.1f, ", stpTableYPerformance->afTblPosnErrContact2B[ii]);
	}
	csTemp.AppendFormat("%5.1f];\r\n", stpTableYPerformance->afTblPosnErrContact2B[nMaxIdxTblSettling]);
	csTemp.AppendFormat("afPosnErrTblY_Contact2B_P2P_MaxAbs = [%5.1f, %5.1f]; \r\n", 
		stpTableYPerformance->fTblPosnErrP2PContact2B, stpTableYPerformance->fTblMaxAbsPosnErrContact2B);
	if(fabs(stpTableYPerformance->fTblPosnErrP2PContact2B) <= PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_P2P)   //// 20120214
//		&& fabs(stpTableYPerformance->fTblMaxAbsPosnErrContact2B) <= PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_STD )
	{
		csTemp.AppendFormat("%%%%PASS :) \r\n");  
	}
	else
	{
		csTemp.AppendFormat("%%%%FAIL !!!  Spec. < [%4.1f, %4.1f] \r\n",  
			(double)PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_P2P, 
			(double)PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_STD);  
	}  //// 20120214
	for(ii = (stpTimePointsOfBondHeadZ->idxStartTail - stpTimePointsOfTable->idxEndTraj2ndBondPosnY); ii>= 0; ii --)  // 20120716
	{
		if(fabs(stpTableYPerformance->afTblPosnErrFromSrch2B[ii]) > PASS_FAIL_TH_BND_XY_PERFORM_SETTLING_P2P)
		{
			stpTableYPerformance->fTable_SettlingTimeMove2B = ii/2.0;
			break;
		} // afTblPosnErrContact2B
	}
	csTemp.AppendFormat("Y2B_SettlingTime_ms = %4.1f;\r\n", stpTableYPerformance->fTable_SettlingTimeMove2B);

	return csTemp;
}

CString _mtn_tune_opt_wire_xy_settling(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc)
{
CString csTemp;

	csTemp.Append(_mtn_tune_opt_wire_xy_settling_1st_bond(stpWbWaveformPerformanceCalc));
	csTemp.Append(_mtn_tune_opt_wire_xy_settling_2nd_bond(stpWbWaveformPerformanceCalc));
	return csTemp;
}

CString _mtn_tune_opt_wire_max_speed_bnd_z(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc)
{
	CString csTemp;
	double dCtrlSampleTime_ms = sys_get_controller_ts();
	BOND_HEAD_SPEED_SETTING   *stpBondHeadSpeedSetting = &stpWbWaveformPerformanceCalc->stBondHeadSpeedSetting;
	double dBndHeadZ_EncRes_mm = astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm;

	double dFactorBH_Z_Acc_Fr_cnt_ms_to_si = (1E+6)/dBndHeadZ_EncRes_mm/1000/dCtrlSampleTime_ms/dCtrlSampleTime_ms;
	double dFactorBH_Z_Jerk_Fr_cnt_ms_to_si = (1E+9)/dBndHeadZ_EncRes_mm/1000/dCtrlSampleTime_ms/dCtrlSampleTime_ms/dCtrlSampleTime_ms;

	//////// Actual Max Speed
	csTemp.AppendFormat("%%%% Actual Maximum Speed BH\r\n");
	csTemp.AppendFormat("aMaxAcc_Jerk_SrchHeight_si_m_s = [%8.1f, %8.1f]\r\n", 
		stpBondHeadSpeedSetting->dMaxRefAccMoveToSrchHeight * dFactorBH_Z_Acc_Fr_cnt_ms_to_si, 
		stpBondHeadSpeedSetting->dMaxRefJerkMoveToSrchHeight *dFactorBH_Z_Jerk_Fr_cnt_ms_to_si);
	csTemp.AppendFormat("aMaxAcc_Jerk_RevHeight_si_m_s = [%8.1f, %8.1f]\r\n", 
		stpBondHeadSpeedSetting->dMaxRefAccMoveReverseHeight * dFactorBH_Z_Acc_Fr_cnt_ms_to_si, 
		stpBondHeadSpeedSetting->dMaxRefJerkMoveReverseHeight *dFactorBH_Z_Jerk_Fr_cnt_ms_to_si);
	csTemp.AppendFormat("aMaxAcc_Jerk_RevDist_si_m_s = [%8.1f, %8.1f]\r\n", 
		stpBondHeadSpeedSetting->dMaxRefAccMoveReverseDist * dFactorBH_Z_Acc_Fr_cnt_ms_to_si, 
		stpBondHeadSpeedSetting->dMaxRefJerkMoveReverseDist *dFactorBH_Z_Jerk_Fr_cnt_ms_to_si);
	csTemp.AppendFormat("aMaxAcc_Jerk_LoopTop_si_m_s = [%8.1f, %8.1f]\r\n", 
		stpBondHeadSpeedSetting->dMaxRefAccMoveLoopTop * dFactorBH_Z_Acc_Fr_cnt_ms_to_si, 
		stpBondHeadSpeedSetting->dMaxRefJerkMoveLoopTop *dFactorBH_Z_Jerk_Fr_cnt_ms_to_si);
	csTemp.AppendFormat("aMaxAcc_Jerk_Traj_si_m_s = [%8.1f, %8.1f]\r\n", 
		stpBondHeadSpeedSetting->dMaxRefAccMoveTrajectory * dFactorBH_Z_Acc_Fr_cnt_ms_to_si, 
		stpBondHeadSpeedSetting->dMaxRefJerkMoveTrajectory *dFactorBH_Z_Jerk_Fr_cnt_ms_to_si);
	csTemp.AppendFormat("aMaxAcc_Jerk_Tail_si_m_s = [%8.1f, %8.1f]\r\n", 
		stpBondHeadSpeedSetting->dMaxRefAccMoveTail * dFactorBH_Z_Acc_Fr_cnt_ms_to_si, 
		stpBondHeadSpeedSetting->dMaxRefJerkMoveTail *dFactorBH_Z_Jerk_Fr_cnt_ms_to_si);
	csTemp.AppendFormat("aMaxAcc_Jerk_Reset_si_m_s = [%8.1f, %8.1f]\r\n\r\n", 
		stpBondHeadSpeedSetting->dMaxRefAccMoveResetLevel * dFactorBH_Z_Acc_Fr_cnt_ms_to_si, 
		stpBondHeadSpeedSetting->dMaxRefJerkMoveResetLevel *dFactorBH_Z_Jerk_Fr_cnt_ms_to_si);


	return csTemp;
}

CString _mtn_tune_opt_wire_max_speed_xy(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc)
{
	CString csTemp;
	double dCtrlSampleTime_ms = sys_get_controller_ts();
TABLE_SPEEDING_SETTING *stpActSpeedTableX = &stpWbWaveformPerformanceCalc->stActSpeedTableX;
TABLE_SPEEDING_SETTING *stpActSpeedTableY = &stpWbWaveformPerformanceCalc->stActSpeedTableY;
POSNS_OF_TABLE *stpPosnsOfTable = &stpWbWaveformPerformanceCalc->stPosnsOfTable;  // 20120724
	double dTableX_EncRes_mm = astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm;
	double dTableY_EncRes_mm = astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm;

	double dFactorTbl_X_Acc_Fr_cnt_ms_to_si = (1E+6)/dTableX_EncRes_mm/1000/dCtrlSampleTime_ms/dCtrlSampleTime_ms;
	double dFactorTbl_Y_Acc_Fr_cnt_ms_to_si = (1E+6)/dTableY_EncRes_mm/1000/dCtrlSampleTime_ms/dCtrlSampleTime_ms;
	double dFactorTbl_X_Jerk_Fr_cnt_ms_to_si = (1E+9)/dTableX_EncRes_mm/1000/dCtrlSampleTime_ms/dCtrlSampleTime_ms/dCtrlSampleTime_ms;
	double dFactorTbl_Y_Jerk_Fr_cnt_ms_to_si = (1E+9)/dTableY_EncRes_mm/1000/dCtrlSampleTime_ms/dCtrlSampleTime_ms/dCtrlSampleTime_ms;


	csTemp.AppendFormat("%%%% Actual Maximum Speed Table-X\r\n");
	csTemp.AppendFormat("aMaxAJ_m_s_Dmm_Move1stB_ = [%8.1f, %8.1f, %5.2f]\r\n", 
		stpActSpeedTableX->dMaxRefAccMove1stB_BTO * dFactorTbl_X_Acc_Fr_cnt_ms_to_si, 
		stpActSpeedTableX->dMaxRefJerkMove1stB_BTO * dFactorTbl_X_Jerk_Fr_cnt_ms_to_si,
		fabs(stpPosnsOfTable->fStartPosition_X - stpPosnsOfTable->f1stBondPosition_X)/ dTableX_EncRes_mm);
	csTemp.AppendFormat("aMaxAJ_m_s_Dmm_RevDist_ = [%8.1f, %8.1f, %5.2f]\r\n", 
		stpActSpeedTableX->dMaxRefAccReverseDist * dFactorTbl_X_Acc_Fr_cnt_ms_to_si, 
		stpActSpeedTableX->dMaxRefJerkReverseDist * dFactorTbl_X_Jerk_Fr_cnt_ms_to_si,
		fabs(stpPosnsOfTable->f1stBondReversePosition_X - stpPosnsOfTable->f1stBondPosition_X)/ dTableX_EncRes_mm);
	csTemp.AppendFormat("aMaxAJ_m_s_Dmm_Traj_ = [%8.1f, %8.1f, %5.2f]\r\n", 
		stpActSpeedTableX->dMaxRefAccTraj * dFactorTbl_X_Acc_Fr_cnt_ms_to_si, 
		stpActSpeedTableX->dMaxRefJerkTraj * dFactorTbl_X_Jerk_Fr_cnt_ms_to_si,
		fabs(stpPosnsOfTable->f1stBondReversePosition_X - stpPosnsOfTable->fTrajEnd2ndBondPosn_X)/ dTableX_EncRes_mm);

	csTemp.AppendFormat("%%%% Actual Maximum Speed Table-Y\r\n");
	csTemp.AppendFormat("aMaxAJ_m_s_Dmm_Move1stB_ = [%8.2f, %8.2f, %5.2f]\r\n", 
		stpActSpeedTableY->dMaxRefAccMove1stB_BTO * dFactorTbl_Y_Acc_Fr_cnt_ms_to_si, 
		stpActSpeedTableY->dMaxRefJerkMove1stB_BTO * dFactorTbl_Y_Jerk_Fr_cnt_ms_to_si,
		fabs(stpPosnsOfTable->fStartPosition_Y - stpPosnsOfTable->f1stBondPosition_Y)/ dTableX_EncRes_mm);
	csTemp.AppendFormat("aMaxAJ_m_s_Dmm_RevDist_ = [%8.2f, %8.2f, %5.2f]\r\n", 
		stpActSpeedTableY->dMaxRefAccReverseDist * dFactorTbl_Y_Acc_Fr_cnt_ms_to_si, 
		stpActSpeedTableY->dMaxRefJerkReverseDist * dFactorTbl_Y_Jerk_Fr_cnt_ms_to_si,
		fabs(stpPosnsOfTable->f1stBondReversePosition_Y - stpPosnsOfTable->f1stBondPosition_Y)/ dTableX_EncRes_mm);
	csTemp.AppendFormat("aMaxAJ_m_s_Dmm_Traj_ = [%8.2f, %8.2f, %5.2f]\r\n", 
		stpActSpeedTableY->dMaxRefAccTraj * dFactorTbl_Y_Acc_Fr_cnt_ms_to_si, 
		stpActSpeedTableY->dMaxRefJerkTraj * dFactorTbl_Y_Jerk_Fr_cnt_ms_to_si,
		fabs(stpPosnsOfTable->f1stBondReversePosition_Y - stpPosnsOfTable->fTrajEnd2ndBondPosn_Y)/ dTableX_EncRes_mm);

	return csTemp;
}

CString mtn_tune_output_performance_string(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc)
{
BOND_HEAD_PERFORMANCE *stpBondHeadPerformance = &(stpWbWaveformPerformanceCalc->stBondHeadPerformance);
TIME_POINTS_OF_BONDHEAD_Z *stpTimePointsOfBondHeadZ = &(stpWbWaveformPerformanceCalc->stTimePointsOfBondHeadZ);
POSNS_OF_BONDHEAD_Z *stpPosnsOfBondHeadZ = &(stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ);
POSNS_OF_TABLE *stpPosnsOfTable = &(stpWbWaveformPerformanceCalc->stPosnsOfTable);
TIME_POINTS_OF_TABLE *stpTimePointsOfTable = &(stpWbWaveformPerformanceCalc->stTimePointsOfTable);
TABLE_WB_PERFORMANCE *stpTableXPerformance = &(stpWbWaveformPerformanceCalc->stTableXPerformance);
TABLE_WB_PERFORMANCE *stpTableYPerformance = &(stpWbWaveformPerformanceCalc->stTableYPerformance);

	double dCtrlSampleTime_ms = sys_get_controller_ts();
//int ii;

///	csTemp.Format("%%%%BondHead Points @ Time Point, w Performance O/U_shoot\r\n");

CString csTemp;

	csTemp.AppendFormat("nB1W_Time_ms = %3.0f; %%FalseContact: 1B-%d, 2B-%d, %% Time [%d, %d];\r\n", 
		stpWbWaveformPerformanceCalc->nTimeB1W_S2E_cnt * dCtrlSampleTime_ms, 
		stpBondHeadPerformance->iFlagIsFalseContact1stB, stpBondHeadPerformance->iFlagIsFalseContact2ndB, // 20110529
		stpWbWaveformPerformanceCalc->stTimePointsOfBondHeadZ.idxStartMove1stBondSearchHeight,
		stpWbWaveformPerformanceCalc->idxEndOneWireZ);

	csTemp.AppendFormat("%s", _mtn_tune_opt_wire_bh_srch_reset(stpWbWaveformPerformanceCalc).GetString());

	csTemp.AppendFormat("%%%% BondHead Looping\r\n%s", _mtn_tune_opt_wire_bh_looping(stpWbWaveformPerformanceCalc).GetString());

	csTemp.Append(_mtn_tune_opt_wire_xy_settling(stpWbWaveformPerformanceCalc));

	//// Table Performance
    csTemp.AppendFormat("%%%% Table-X Points @ Time Point, CEOUS: Command End Over-Under Shoot, DPE: Dynamic Position Error\r\n");
////
    csTemp.AppendFormat("Tbl_X_MOVE_BTO = [%6.0f, %6.0f]; %% @ Time [%d, %d], Dist_mm = %3.1f, tDur_ms = %2.1f; \r\n", 
		stpPosnsOfTable->fStartPosition_X, stpPosnsOfTable->f1stBondPosition_X, 
		stpTimePointsOfTable->idxStartMoveX1stBond, stpTimePointsOfTable->idxEndMoveX_1stBondPosn,
		fabs(stpPosnsOfTable->fStartPosition_X - stpPosnsOfTable->f1stBondPosition_X)/astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm,
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfTable->idxStartMoveX1stBond - stpTimePointsOfTable->idxEndMoveX_1stBondPosn ));
    csTemp.AppendFormat("afMaxMinDPE = [%6.0f, %6.0f]; \r\n", stpTableXPerformance->afMaxDPE_MoveTo1stBondPosn_BTO[0], stpTableXPerformance->afMaxDPE_MoveTo1stBondPosn_BTO[1]);
    csTemp.AppendFormat("fCEOUS = %4.0f \r\n",  stpTableXPerformance->fCEOUS_MoveTo1stBondPosn_BTO);
//// 
    csTemp.AppendFormat("Tbl_X_MOVE_REVERSE_DIST = [%6.0f, %6.0f]; %% @ Time [%d, %d], Dist_mm = %3.1f, tDur_ms = %2.1f; \r\n", 
		stpPosnsOfTable->f1stBondPosition_X, stpPosnsOfTable->f1stBondReversePosition_X, 
		stpTimePointsOfTable->idxStartReverseMoveX, stpTimePointsOfTable->idxEndReverseMoveX,
		fabs(stpPosnsOfTable->f1stBondPosition_X - stpPosnsOfTable->f1stBondReversePosition_X)/astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm, 
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfTable->idxStartReverseMoveX - stpTimePointsOfTable->idxEndReverseMoveX));
    csTemp.AppendFormat("afMaxMinDPE = [%4.0f, %4.0f]; \r\n", stpTableXPerformance->afMaxDPE_MoveToKink1[0], stpTableXPerformance->afMaxDPE_MoveToKink1[1]);
    csTemp.AppendFormat("fCEOUS = %4.0f; \r\n",  stpTableXPerformance->fCEOUS_MoveToKink1);
////    
    csTemp.AppendFormat("Tbl_X_MOVE_TRAJ = [%6.0f, %6.0f]; %% @ Time [%d, %d], Dist_mm = %3.1f, tDur_ms = %2.1f; \r\n", 
		stpPosnsOfTable->f1stBondReversePosition_X, stpPosnsOfTable->fTrajEnd2ndBondPosn_X, 
		stpTimePointsOfTable->idxStartTrajTo2ndBondX, stpTimePointsOfTable->idxEndTraj2ndBondPosnX,
		fabs(stpPosnsOfTable->f1stBondReversePosition_X - stpPosnsOfTable->fTrajEnd2ndBondPosn_X)/astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm, 
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfTable->idxStartTrajTo2ndBondX - stpTimePointsOfTable->idxEndTraj2ndBondPosnX));
    csTemp.AppendFormat("afMaxMinDPE = [%4.0f, %4.0f]; \r\n", stpTableXPerformance->afMaxDPE_TrajMoveTo2ndBond[0], stpTableXPerformance->afMaxDPE_TrajMoveTo2ndBond[1]);
    csTemp.AppendFormat("fCEOUS = %4.0f; \r\n",  stpTableXPerformance->fCEOUS_TrajMoveTo2ndBond);
//// 
    csTemp.AppendFormat("Tbl_X_MOVE_NEXT_PR = [%6.0f, %6.0f]; %% @ Time [%d, %d], tDur_ms = %2.1f; \r\n", 
		stpPosnsOfTable->fTrajEnd2ndBondPosn_X, stpPosnsOfTable->fMoveToNextPR_Posn_X, 
		stpTimePointsOfTable->idxStartMoveNextPR_X, stpTimePointsOfTable->idxEndMoveNextPR_X,
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfTable->idxStartMoveNextPR_X - stpTimePointsOfTable->idxEndMoveNextPR_X));
    csTemp.AppendFormat("afMaxMinDPE = [%4.0f, %4.0f]; \r\n", stpTableXPerformance->afMaxDPE_MoveToNextPR[0], stpTableXPerformance->afMaxDPE_MoveToNextPR[1]);
    csTemp.AppendFormat("fCEOUS = %4.0f; \r\n",  stpTableXPerformance->fCEOUS_MoveToNextPR);
//// 

    csTemp.AppendFormat("%%%% Table-Y Points @ Time Point, CEOUS: Command End Over-Under Shoot, DPE: Dynamic Positio Error\r\n");
//// 
    csTemp.AppendFormat("Tbl_Y_MOVE_BTO = [%6.0f, %6.0f]; %% @ Time [%d, %d], Dist_mm = %3.1f, tDur_ms = %2.1f; \r\n", 
		stpPosnsOfTable->fStartPosition_Y, stpPosnsOfTable->f1stBondPosition_Y,
		stpTimePointsOfTable->idxStartMoveY1stBond, stpTimePointsOfTable->idxEndMoveY_1stBondPosn,
		fabs(stpPosnsOfTable->fStartPosition_Y - stpPosnsOfTable->f1stBondPosition_Y)/ astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm,
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfTable->idxStartMoveY1stBond - stpTimePointsOfTable->idxEndMoveY_1stBondPosn));
    csTemp.AppendFormat("afMaxMinDPE = [%4.0f, %4.0f]; \r\n", stpTableYPerformance->afMaxDPE_MoveTo1stBondPosn_BTO[0], stpTableYPerformance->afMaxDPE_MoveTo1stBondPosn_BTO[1]);
    csTemp.AppendFormat("fCEOUS = %4.0f; \r\n",  stpTableYPerformance->fCEOUS_MoveTo1stBondPosn_BTO);
//// 
    csTemp.AppendFormat("Tbl_Y_MOVE_REVERSE_DIST = [%6.0f, %6.0f]; %% @ Time [%d, %d], Dist_mm = %3.1f, tDur_ms = %2.1f; \r\n", 
		stpPosnsOfTable->f1stBondPosition_Y, stpPosnsOfTable->f1stBondReversePosition_Y,
		stpTimePointsOfTable->idxStartReverseMoveY, stpTimePointsOfTable->idxEndReverseMoveY,
		fabs(stpPosnsOfTable->f1stBondPosition_Y - stpPosnsOfTable->f1stBondReversePosition_Y)/astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm,
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfTable->idxStartReverseMoveY - stpTimePointsOfTable->idxEndReverseMoveY));
    csTemp.AppendFormat("afMaxMinDPE = [%4.0f, %4.0f]; \r\n", stpTableYPerformance->afMaxDPE_MoveToKink1[0], stpTableYPerformance->afMaxDPE_MoveToKink1[1]);
    csTemp.AppendFormat("fCEOUS = %4.0f; \r\n",  stpTableYPerformance->fCEOUS_MoveToKink1);
//// 
    csTemp.AppendFormat("Tbl_Y_MOVE_TRAJ = [%6.0f, %6.0f]; %% @ Time [%d, %d], Dist_mm = %3.1f, tDur_ms = %2.1f; \r\n", 
		stpPosnsOfTable->f1stBondReversePosition_Y, stpPosnsOfTable->fTrajEnd2ndBondPosn_Y,
		stpTimePointsOfTable->idxStartTrajTo2ndBondY, stpTimePointsOfTable->idxEndTraj2ndBondPosnY,
		fabs(stpPosnsOfTable->f1stBondReversePosition_Y - stpPosnsOfTable->fTrajEnd2ndBondPosn_Y)/astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm,
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfTable->idxStartTrajTo2ndBondY - stpTimePointsOfTable->idxEndTraj2ndBondPosnY));
    csTemp.AppendFormat("afMaxMinDPE = [%4.0f, %4.0f]; \r\n", stpTableYPerformance->afMaxDPE_TrajMoveTo2ndBond[0], stpTableYPerformance->afMaxDPE_TrajMoveTo2ndBond[1]);
    csTemp.AppendFormat("fCEOUS = %4.0f \r\n",  stpTableYPerformance->fCEOUS_TrajMoveTo2ndBond);
////
    csTemp.AppendFormat("Tbl_Y_MOVE_NEXT_PR = [%6.0f, %6.0f]; %% @ Time [%d, %d], tDur_ms = %2.1f; \r\n", stpPosnsOfTable->fTrajEnd2ndBondPosn_Y, stpPosnsOfTable->fMoveToNextPR_Posn_Y,
		stpTimePointsOfTable->idxStartMoveNextPR_Y, stpTimePointsOfTable->idxEndMoveNextPR_Y,
		dCtrlSampleTime_ms * fabs((double) stpTimePointsOfTable->idxStartMoveNextPR_Y- stpTimePointsOfTable->idxEndMoveNextPR_Y));
    csTemp.AppendFormat("afMaxMinDPE = [%4.0f, %4.0f]; \r\n", stpTableYPerformance->afMaxDPE_MoveToNextPR[0], stpTableYPerformance->afMaxDPE_MoveToNextPR[1]);
    csTemp.AppendFormat("fCEOUS = %4.0f; \r\n\r\n",  stpTableYPerformance->fCEOUS_MoveToNextPR);


	csTemp.Append(_mtn_tune_opt_wire_max_speed_bnd_z(stpWbWaveformPerformanceCalc));
	csTemp.Append(_mtn_tune_opt_wire_max_speed_xy(stpWbWaveformPerformanceCalc));

	csTemp.AppendFormat("\r\n\r\n");

	return csTemp;
}

CString mtn_wb_tune_multi_wire_perform(MULTI_WIRE_WB_PERFORMANCE *stpWbMultiWirePerformStat)
{
CString csTemp;

	csTemp.Format("%%Multi Wire Statistics: BH-(BondHead) CeousMMS-(CEOUS_min_max_std) \r\n");
	csTemp.AppendFormat("stBH_Move1stSearchHeight_CeousMMS = [%6.1f, %6.1f, %6.1f]\r\n", 
		stpWbMultiWirePerformStat->stBondHeadMove1stSearchHeight_CEOUS.dMin, stpWbMultiWirePerformStat->stBondHeadMove1stSearchHeight_CEOUS.dMax,
		stpWbMultiWirePerformStat->stBondHeadMove1stSearchHeight_CEOUS.dStd);

	csTemp.AppendFormat("stBH_ReverseHeight_CeousMMS = [%6.1f, %6.1f, %6.1f]\r\n", 
		stpWbMultiWirePerformStat->stBondHeadReverseHeight_CEOUS.dMin, stpWbMultiWirePerformStat->stBondHeadReverseHeight_CEOUS.dMax,
		stpWbMultiWirePerformStat->stBondHeadReverseHeight_CEOUS.dStd);

	csTemp.AppendFormat("stBH_Kink1_CeousMMS = [%6.1f, %6.1f, %6.1f]\r\n", 
		stpWbMultiWirePerformStat->stBondHeadKink1_CEOUS.dMin, stpWbMultiWirePerformStat->stBondHeadKink1_CEOUS.dMax,
		stpWbMultiWirePerformStat->stBondHeadKink1_CEOUS.dStd);

	csTemp.AppendFormat("stBH_LoopTop_CeousMMS = [%6.1f, %6.1f, %6.1f]\r\n", 
		stpWbMultiWirePerformStat->stBondHeadLoopTop_CEOUS.dMin, stpWbMultiWirePerformStat->stBondHeadLoopTop_CEOUS.dMax,
		stpWbMultiWirePerformStat->stBondHeadLoopTop_CEOUS.dStd);

	csTemp.AppendFormat("stBH_Trajectory_CeousMMS = [%6.1f, %6.1f, %6.1f]\r\n", 
		stpWbMultiWirePerformStat->stBondHeadTrajectory_CEOUS.dMin, stpWbMultiWirePerformStat->stBondHeadTrajectory_CEOUS.dMax,
		stpWbMultiWirePerformStat->stBondHeadTrajectory_CEOUS.dStd);

	csTemp.AppendFormat("stBH_Tail_CeousMMS = [%6.1f, %6.1f, %6.1f]\r\n", 
		stpWbMultiWirePerformStat->stBondHeadTail_CEOUS.dMin, stpWbMultiWirePerformStat->stBondHeadTail_CEOUS.dMax,
		stpWbMultiWirePerformStat->stBondHeadTail_CEOUS.dStd);

	csTemp.AppendFormat("stBH_ResetFireLevel_CeousMMS = [%6.1f, %6.1f, %6.1f]\r\n", 
		stpWbMultiWirePerformStat->stBondHeadResetFireLevel_CEOUS.dMin, stpWbMultiWirePerformStat->stBondHeadResetFireLevel_CEOUS.dMax,
		stpWbMultiWirePerformStat->stBondHeadResetFireLevel_CEOUS.dStd);

	csTemp.AppendFormat("stBH_1stContactPosn_min_max_std = [%6.1f, %6.1f, %6.1f]\r\n", 
		stpWbMultiWirePerformStat->stBondHead1stContactPosn.dMin, stpWbMultiWirePerformStat->stBondHead1stContactPosn.dMax,
		stpWbMultiWirePerformStat->stBondHead1stContactPosn.dStd);

	csTemp.AppendFormat("stBH_1stBondPosnRippleStd_min_max_std = [%6.1f, %6.1f, %6.1f]\r\n", 
		stpWbMultiWirePerformStat->stBondHead1stBondPosnRippleStd.dMin, stpWbMultiWirePerformStat->stBondHead1stBondPosnRippleStd.dMax,
		stpWbMultiWirePerformStat->stBondHead1stBondPosnRippleStd.dStd);

	csTemp.AppendFormat("stBH_2ndContactPosn_min_max_std = [%6.1f, %6.1f, %6.1f]\r\n", 
		stpWbMultiWirePerformStat->stBondHead2ndContactPosn.dMin, stpWbMultiWirePerformStat->stBondHead2ndContactPosn.dMax,
		stpWbMultiWirePerformStat->stBondHead2ndContactPosn.dStd);

	csTemp.AppendFormat("stBH_2ndBondPosnRippleStd_min_max_std = [%6.1f, %6.1f, %6.1f]\r\n", 
		stpWbMultiWirePerformStat->stBondHead2ndBondPosnRippleStd.dMin, stpWbMultiWirePerformStat->stBondHead2ndBondPosnRippleStd.dMax,
		stpWbMultiWirePerformStat->stBondHead2ndBondPosnRippleStd.dStd);

	// Table-X
	csTemp.AppendFormat("stTableXMove1stBond_CeousMMS = [%6.1f, %6.1f, %6.1f]\r\n", 
		stpWbMultiWirePerformStat->stTableXMove1stBond_CEOUS.dMin, stpWbMultiWirePerformStat->stTableXMove1stBond_CEOUS.dMax,
		stpWbMultiWirePerformStat->stTableXMove1stBond_CEOUS.dStd);

	csTemp.AppendFormat("stTableXKink1_CeousMMS = [%6.1f, %6.1f, %6.1f]\r\n", 
		stpWbMultiWirePerformStat->stTableXKink1_CEOUS.dMin, stpWbMultiWirePerformStat->stTableXKink1_CEOUS.dMax,
		stpWbMultiWirePerformStat->stTableXKink1_CEOUS.dStd);

	csTemp.AppendFormat("stTableXTrajMove2ndBond_CeousMMS = [%6.1f, %6.1f, %6.1f]\r\n", 
		stpWbMultiWirePerformStat->stTableXTrajMove2ndBond_CEOUS.dMin, stpWbMultiWirePerformStat->stTableXTrajMove2ndBond_CEOUS.dMax,
		stpWbMultiWirePerformStat->stTableXTrajMove2ndBond_CEOUS.dStd);

	// Table-Y
	csTemp.AppendFormat("stTableYMove1stBond_CeousMMS = [%6.1f, %6.1f, %6.1f]\r\n", 
		stpWbMultiWirePerformStat->stTableYMove1stBond_CEOUS.dMin, stpWbMultiWirePerformStat->stTableYMove1stBond_CEOUS.dMax,
		stpWbMultiWirePerformStat->stTableYMove1stBond_CEOUS.dStd);

	csTemp.AppendFormat("stTableYKink1_CeousMMS = [%6.1f, %6.1f, %6.1f]\r\n", 
		stpWbMultiWirePerformStat->stTableYKink1_CEOUS.dMin, stpWbMultiWirePerformStat->stTableYKink1_CEOUS.dMax,
		stpWbMultiWirePerformStat->stTableYKink1_CEOUS.dStd);

	csTemp.AppendFormat("stTableYTrajMove2ndBond_CeousMMS = [%6.1f, %6.1f, %6.1f]\r\n", 
		stpWbMultiWirePerformStat->stTableYTrajMove2ndBond_CEOUS.dMin, stpWbMultiWirePerformStat->stTableYTrajMove2ndBond_CEOUS.dMax,
		stpWbMultiWirePerformStat->stTableYTrajMove2ndBond_CEOUS.dStd);

	// Position list - [TableX, TableY]
	csTemp.AppendFormat("%% a1stBondPosn_Wire# = [X, Y]; a2ndBondPosn_Wire# = [X, Y]; \r\n");
	for(int ii=0; ii<stpWbMultiWirePerformStat->nTotalWires; ii++)
	{
		csTemp.AppendFormat("a1stBondPosn_Wire%d = [%8.1f, %8.1f]; a2ndBondPosn_Wire%d = [%8.1f, %8.1f]; \r\n",
			ii, stpWbMultiWirePerformStat->dTableX1stBondPosnList[ii], stpWbMultiWirePerformStat->dTableY1stBondPosnList[ii],
			ii, stpWbMultiWirePerformStat->dTableX2ndBondPosnList[ii], stpWbMultiWirePerformStat->dTableY2ndBondPosnList[ii]);
	}
	csTemp.AppendFormat("\r\n");

	return csTemp;
}

CString mtn_wb_tune_multi_wire_motion_cmd_dist_acc(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc)
{
CString csTemp;
TIME_POINTS_OF_TABLE *stpTimePointsOfTable = &(stpWbWaveformPerformanceCalc->stTimePointsOfTable);

	double dCtrlSampleTime_ms = sys_get_controller_ts();

BOND_HEAD_SPEED_SETTING   *stpBondHeadSpeedSetting = &stpWbWaveformPerformanceCalc->stBondHeadSpeedSetting;
TABLE_SPEEDING_SETTING *stpActSpeedTableX = &stpWbWaveformPerformanceCalc->stActSpeedTableX;
TABLE_SPEEDING_SETTING *stpActSpeedTableY = &stpWbWaveformPerformanceCalc->stActSpeedTableY;
	double dTableX_EncRes_mm = astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm;
	double dTableY_EncRes_mm = astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm;
	double dBndHeadZ_EncRes_mm = astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm;

	double dFactorBH_Z_Acc_Fr_cnt_ms_to_si = (1E+6)/dBndHeadZ_EncRes_mm/1000/dCtrlSampleTime_ms/dCtrlSampleTime_ms;
	double dFactorTbl_X_Acc_Fr_cnt_ms_to_si = (1E+6)/dTableX_EncRes_mm/1000/dCtrlSampleTime_ms/dCtrlSampleTime_ms;
	double dFactorTbl_Y_Acc_Fr_cnt_ms_to_si = (1E+6)/dTableY_EncRes_mm/1000/dCtrlSampleTime_ms/dCtrlSampleTime_ms;
	double dFactorBH_Z_Jerk_Fr_cnt_ms_to_si = (1E+9)/dBndHeadZ_EncRes_mm/1000/dCtrlSampleTime_ms/dCtrlSampleTime_ms/dCtrlSampleTime_ms;
	double dFactorTbl_X_Jerk_Fr_cnt_ms_to_si = (1E+9)/dTableX_EncRes_mm/1000/dCtrlSampleTime_ms/dCtrlSampleTime_ms/dCtrlSampleTime_ms;
	double dFactorTbl_Y_Jerk_Fr_cnt_ms_to_si = (1E+9)/dTableY_EncRes_mm/1000/dCtrlSampleTime_ms/dCtrlSampleTime_ms/dCtrlSampleTime_ms;

	csTemp.Format("%%----XXX---- \r\n aMaxAcc_Jerk_si_m_s = [Acc, Jerk], \r\n aPosn_FromTo_Dist = [PosnFrom, PosnTo, Dist] \r\n");

	//////// Actual Max Speed
	csTemp.AppendFormat("%%%% Actual Maximum Speed BH\r\n");
	csTemp.AppendFormat("%%----SrchHeight---- \r\n");
	csTemp.AppendFormat("aMaxAcc_Jerk_si_m_s = [%8.1f, %8.1f]\r\n", 
		stpBondHeadSpeedSetting->dMaxRefAccMoveToSrchHeight * dFactorBH_Z_Acc_Fr_cnt_ms_to_si, 
		stpBondHeadSpeedSetting->dMaxRefJerkMoveToSrchHeight *dFactorBH_Z_Jerk_Fr_cnt_ms_to_si);
	csTemp.AppendFormat("aPosn_FromTo_Dist_um = [%6.1f, %6.1f, %6.1f]\r\n",
		stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fEndFireLevel_Z,
		stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.f1stBondSearchHeight_Z,
		(stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.f1stBondSearchHeight_Z - stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fEndFireLevel_Z));
	csTemp.AppendFormat("\r\n");

	csTemp.AppendFormat("%%----RevHeight---- \r\n");
	csTemp.AppendFormat("aMaxAcc_Jerk_si_m_s = [%8.1f, %8.1f]\r\n", 
		stpBondHeadSpeedSetting->dMaxRefAccMoveReverseHeight * dFactorBH_Z_Acc_Fr_cnt_ms_to_si, 
		stpBondHeadSpeedSetting->dMaxRefJerkMoveReverseHeight *dFactorBH_Z_Jerk_Fr_cnt_ms_to_si);
	csTemp.AppendFormat("aPosn_FromTo_Dist_um = [%6.1f, %6.1f, %6.1f]\r\n",
		stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.f1stBondContactPosn_Z,
		stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fReverseHeightPosn_Z,
		(stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fReverseHeightPosn_Z - stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.f1stBondContactPosn_Z) 
		/ astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm * 1000);
	csTemp.AppendFormat("\r\n");

	csTemp.AppendFormat("%%----RevDist---- \r\n");
	csTemp.AppendFormat("aMaxAcc_Jerk_RevDist_si_m_s = [%8.1f, %8.1f]\r\n", 
		stpBondHeadSpeedSetting->dMaxRefAccMoveReverseDist * dFactorBH_Z_Acc_Fr_cnt_ms_to_si, 
		stpBondHeadSpeedSetting->dMaxRefJerkMoveReverseDist *dFactorBH_Z_Jerk_Fr_cnt_ms_to_si);
	csTemp.AppendFormat("aPosn_FromTo_Dist_um = [%6.1f, %6.1f, %6.1f]\r\n",
		stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fReverseHeightPosn_Z,
		stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fKinkHeightPosn_Z,
		(stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fKinkHeightPosn_Z - stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fReverseHeightPosn_Z) 
			/ astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm * 1000);
	csTemp.AppendFormat("\r\n");

	csTemp.AppendFormat("%%----LoopTop---- \r\n");
	csTemp.AppendFormat("aMaxAcc_Jerk_LoopTop_si_m_s = [%8.1f, %8.1f]\r\n", 
		stpBondHeadSpeedSetting->dMaxRefAccMoveLoopTop * dFactorBH_Z_Acc_Fr_cnt_ms_to_si, 
		stpBondHeadSpeedSetting->dMaxRefJerkMoveLoopTop *dFactorBH_Z_Jerk_Fr_cnt_ms_to_si);
	csTemp.AppendFormat("aPosn_FromTo_Dist_um = [%6.1f, %6.1f, %6.1f]\r\n",
		stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fKinkHeightPosn_Z,
		stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fLoopTopPosn_Z,
		(stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fLoopTopPosn_Z - stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fKinkHeightPosn_Z)
			/ astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm * 1000);
	csTemp.AppendFormat("\r\n");

	csTemp.AppendFormat("%%----Traj---- \r\n");
	csTemp.AppendFormat("aMaxAcc_Jerk_Traj_si_m_s = [%8.1f, %8.1f]\r\n", 
		stpBondHeadSpeedSetting->dMaxRefAccMoveTrajectory * dFactorBH_Z_Acc_Fr_cnt_ms_to_si, 
		stpBondHeadSpeedSetting->dMaxRefJerkMoveTrajectory *dFactorBH_Z_Jerk_Fr_cnt_ms_to_si);
	csTemp.AppendFormat("aPosn_FromTo_Dist_um = [%6.1f, %6.1f, %6.1f]\r\n",
		stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fLoopTopPosn_Z,
		stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.f2ndBondSearchHeightPosn_Z,
		(stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.f2ndBondSearchHeightPosn_Z - stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fLoopTopPosn_Z)
			/ astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm * 1000);
	csTemp.AppendFormat("\r\n");

	csTemp.AppendFormat("%%----Tail---- \r\n");
	csTemp.AppendFormat("aMaxAcc_Jerk_Tail_si_m_s = [%8.1f, %8.1f]\r\n", 
		stpBondHeadSpeedSetting->dMaxRefAccMoveTail * dFactorBH_Z_Acc_Fr_cnt_ms_to_si, 
		stpBondHeadSpeedSetting->dMaxRefJerkMoveTail *dFactorBH_Z_Jerk_Fr_cnt_ms_to_si);
	csTemp.AppendFormat("aPosn_FromTo_Dist_um = [%6.1f, %6.1f, %6.1f]\r\n",
		stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.f2ndBondContactPosn_Z,
		stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fTailPosn_Z,
		(stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fTailPosn_Z - stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.f2ndBondContactPosn_Z)
			/ astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm * 1000);
	csTemp.AppendFormat("\r\n");

	csTemp.AppendFormat("%%----Reset---- \r\n");
	csTemp.AppendFormat("aMaxAcc_Jerk_Reset_si_m_s = [%8.1f, %8.1f]\r\n", 
		stpBondHeadSpeedSetting->dMaxRefAccMoveResetLevel * dFactorBH_Z_Acc_Fr_cnt_ms_to_si, 
		stpBondHeadSpeedSetting->dMaxRefJerkMoveResetLevel *dFactorBH_Z_Jerk_Fr_cnt_ms_to_si);
	csTemp.AppendFormat("aPosn_FromTo_Dist_um = [%6.1f, %6.1f, %6.1f]\r\n",
		stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fTailPosn_Z,
		stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fEndFireLevel_Z,
		(stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fEndFireLevel_Z - stpWbWaveformPerformanceCalc->stPosnsOfBondHeadZ.fTailPosn_Z)
			/ astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm * 1000);
	csTemp.AppendFormat("\r\n");

	csTemp.AppendFormat("%%%% Actual Maximum Speed Table-X \r\n aPosn_FromTo_Dist = [PosnFrom, PosnTo, Dist]\r\n");
	csTemp.AppendFormat("%%----Move1stB_BTO_---- \r\n");
	csTemp.AppendFormat("aMaxAcc_Jerk_si_m_s_ms = [%8.1f, %8.1f, %4.1f]\r\n", 
		stpActSpeedTableX->dMaxRefAccMove1stB_BTO * dFactorTbl_X_Acc_Fr_cnt_ms_to_si, 
		stpActSpeedTableX->dMaxRefJerkMove1stB_BTO * dFactorTbl_X_Jerk_Fr_cnt_ms_to_si,
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfTable->idxStartMoveX1stBond - stpTimePointsOfTable->idxEndMoveX_1stBondPosn ));

//////////////////////////////////////////
	csTemp.AppendFormat("aPosn_FromTo_Dist_mm = [%8.1f, %8.1f, %8.3f]; \r\n",
		stpWbWaveformPerformanceCalc->stPosnsOfTable.fStartPosition_X,	stpWbWaveformPerformanceCalc->stPosnsOfTable.f1stBondPosition_X,
		(stpWbWaveformPerformanceCalc->stPosnsOfTable.f1stBondPosition_X - stpWbWaveformPerformanceCalc->stPosnsOfTable.fStartPosition_X)
			/ astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm);

	csTemp.AppendFormat("%%----RevDist_---- \r\n");
	csTemp.AppendFormat("aMaxAcc_Jerk_si_m_s = [%8.1f, %8.1f]\r\n", 
		stpActSpeedTableX->dMaxRefAccReverseDist * dFactorTbl_X_Acc_Fr_cnt_ms_to_si, 
		stpActSpeedTableX->dMaxRefJerkReverseDist * dFactorTbl_X_Jerk_Fr_cnt_ms_to_si);
	csTemp.AppendFormat("aPosn_FromTo_Dist_mm = [%8.1f, %8.1f, %8.3f]; \r\n",
		stpWbWaveformPerformanceCalc->stPosnsOfTable.f1stBondPosition_X, stpWbWaveformPerformanceCalc->stPosnsOfTable.f1stBondReversePosition_X, 
		(stpWbWaveformPerformanceCalc->stPosnsOfTable.f1stBondReversePosition_X - stpWbWaveformPerformanceCalc->stPosnsOfTable.f1stBondPosition_X)
			/ astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm);

	csTemp.AppendFormat("%%----Traj_---- \r\n");
	csTemp.AppendFormat("aMaxAcc_Jerk_si_m_s = [%8.1f, %8.1f, %4.1f]\r\n", 
		stpActSpeedTableX->dMaxRefAccTraj * dFactorTbl_X_Acc_Fr_cnt_ms_to_si, 
		stpActSpeedTableX->dMaxRefJerkTraj * dFactorTbl_X_Jerk_Fr_cnt_ms_to_si,
		dCtrlSampleTime_ms * fabs((double) stpTimePointsOfTable->idxStartTrajTo2ndBondX - stpTimePointsOfTable->idxEndTraj2ndBondPosnX));
	csTemp.AppendFormat("aPosn_FromTo_Dist_mm = [%8.1f, %8.1f, %8.1f]; \r\n\r\n",
		stpWbWaveformPerformanceCalc->stPosnsOfTable.f1stBondReversePosition_X, stpWbWaveformPerformanceCalc->stPosnsOfTable.fTrajEnd2ndBondPosn_X,
		(stpWbWaveformPerformanceCalc->stPosnsOfTable.fTrajEnd2ndBondPosn_X - stpWbWaveformPerformanceCalc->stPosnsOfTable.f1stBondReversePosition_X)
			/ astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm);

	csTemp.AppendFormat("%%%% Actual Maximum Speed Table-Y\r\n");
	csTemp.AppendFormat("%%----Move1stB_BTO_---- \r\n");
	csTemp.AppendFormat("aMaxAcc_Jerk_si_m_s = [%8.2f, %8.2f, %4.1f]\r\n", 
		stpActSpeedTableY->dMaxRefAccMove1stB_BTO * dFactorTbl_Y_Acc_Fr_cnt_ms_to_si, 
		stpActSpeedTableY->dMaxRefJerkMove1stB_BTO * dFactorTbl_Y_Jerk_Fr_cnt_ms_to_si,
		dCtrlSampleTime_ms * fabs((double)stpTimePointsOfTable->idxStartMoveY1stBond - stpTimePointsOfTable->idxEndMoveY_1stBondPosn ));

	csTemp.AppendFormat("aPosn_FromTo_Dist_mm = [%8.1f, %8.1f, %8.3f]; \r\n",
		stpWbWaveformPerformanceCalc->stPosnsOfTable.fStartPosition_Y, stpWbWaveformPerformanceCalc->stPosnsOfTable.f1stBondPosition_Y, 
		(stpWbWaveformPerformanceCalc->stPosnsOfTable.f1stBondPosition_Y - stpWbWaveformPerformanceCalc->stPosnsOfTable.fStartPosition_Y)
			/ astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm);

	csTemp.AppendFormat("%%----RevDist_---- \r\n");
	csTemp.AppendFormat("aMaxAcc_Jerk_si_m_s = [%8.2f, %8.2f]\r\n", 
		stpActSpeedTableY->dMaxRefAccReverseDist * dFactorTbl_Y_Acc_Fr_cnt_ms_to_si, 
		stpActSpeedTableY->dMaxRefJerkReverseDist * dFactorTbl_Y_Jerk_Fr_cnt_ms_to_si);
	csTemp.AppendFormat("aPosn_FromTo_Dist_um = [%8.1f, %8.1f, %8.3f]; \r\n",
		stpWbWaveformPerformanceCalc->stPosnsOfTable.f1stBondPosition_Y, stpWbWaveformPerformanceCalc->stPosnsOfTable.f1stBondReversePosition_Y, 
		(stpWbWaveformPerformanceCalc->stPosnsOfTable.f1stBondReversePosition_Y - stpWbWaveformPerformanceCalc->stPosnsOfTable.f1stBondPosition_Y)
			/ astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm);

	csTemp.AppendFormat("%%----Traj_---- \r\n");
	csTemp.AppendFormat("aMaxAcc_Jerk_si_m_s = [%8.2f, %8.2f, %4.1f]\r\n", 
		stpActSpeedTableY->dMaxRefAccTraj * dFactorTbl_Y_Acc_Fr_cnt_ms_to_si, 
		stpActSpeedTableY->dMaxRefJerkTraj * dFactorTbl_Y_Jerk_Fr_cnt_ms_to_si,
		dCtrlSampleTime_ms * fabs((double) stpTimePointsOfTable->idxStartTrajTo2ndBondY - stpTimePointsOfTable->idxEndTraj2ndBondPosnY));
	csTemp.AppendFormat("aPosn_FromTo_Dist_mm = [%8.1f, %8.1f, %8.3f]; \r\n",
		stpWbWaveformPerformanceCalc->stPosnsOfTable.f1stBondReversePosition_Y, stpWbWaveformPerformanceCalc->stPosnsOfTable.fTrajEnd2ndBondPosn_Y,
		(stpWbWaveformPerformanceCalc->stPosnsOfTable.fTrajEnd2ndBondPosn_Y - stpWbWaveformPerformanceCalc->stPosnsOfTable.f1stBondReversePosition_Y)
			/ astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm);

	csTemp.AppendFormat("\r\n");
	return csTemp;
}

#include "MtnTune.h"
extern CMtnTune mMotionTuning;
//extern char mtn_wb_tune_thread_get_flag_running();
extern int idxTuningBlockBondHead;
extern int idxTuningBlockTable;
extern int iFlagDebug_SaveFile_UIProtPass;
extern int iFlagTableSectorTuning;
extern unsigned int idxCurrSectorTable;
//////////////////////////////
extern int iFlagIsActualContact;
extern int iFlagTuningMotionZ_B1W;
/////
extern int iFlagB1W_TuneIdle;
extern int iFlagB1W_TuneSrch1B;
extern int iFlagB1W_TuneMoveTo1B;
extern int iFlagB1W_TuneLooping ;
extern int iFlagB1W_TuneLoopTop ;
extern int iFlagB1W_TuneTraj2B ;
extern int iFlagB1W_TuneTail ;
extern int iFlagB1W_TuneReset;
extern int iFlagB1W_TuneSrch2B;

extern MTN_TUNE_GENETIC_INPUT astMtnTuneInput[MAX_SERVO_AXIS_WIREBOND][MAX_BLK_PARAMETER]; 
extern MTN_TUNE_GENETIC_OUTPUT astMtnTuneOutput[MAX_SERVO_AXIS_WIREBOND][MAX_BLK_PARAMETER];

extern MTN_TUNE_GENETIC_INPUT astMtnTuneSectorInputTableX[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];
extern MTN_TUNE_GENETIC_OUTPUT astMtnTuneSectorOutputTableX[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];
extern MTN_TUNE_GENETIC_INPUT astMtnTuneSectorInputTableY[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];
extern MTN_TUNE_GENETIC_OUTPUT astMtnTuneSectorOutputTableY[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];

extern char *astrPassOrFail[2];  // defined from MtnWbTune.cpp

#include "MotAlgo_DLL.h"

CString AutoTuneBondHeadB1W()
{
		CString cstrTemp;
int iFlagTuneIdleIsFail;

	mtn_wb_tune_b1w_init_feasible_set();
	mtn_tune_save_waveform("BefTune");

			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneIdle == TRUE)
			{
				idxTuningBlockBondHead = WB_BH_IDLE; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				mtn_wb_tune_bond_1_wire_bh(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				iFlagTuneIdleIsFail = astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail;
//				cstrTemp.Format("IDLE: %s; ", astrPassOrFail[]);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneMoveTo1B == TRUE) // 20110521, iFlagTuningMotionZ_B1W
			{
				idxTuningBlockBondHead = WB_BH_SRCH_HT; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				mtn_wb_tune_bond_1_wire_bh(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				cstrTemp.Format("Srch_Ht: %s; ", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail]);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneSrch1B == TRUE)  // iFlagIsActualContact == TRUE && 
			{
				idxTuningBlockBondHead = WB_BH_1ST_CONTACT; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead);astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				mtn_wb_tune_bond_1_wire_bh(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				cstrTemp.AppendFormat("1st_Contact: %s; ", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail]);
			}

			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneLooping == TRUE) // 20110521) iFlagTuningMotionZ_B1W
			{
				idxTuningBlockBondHead = WB_BH_LOOPING; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead);astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				mtn_wb_tune_bond_1_wire_bh(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				cstrTemp.AppendFormat("Looping: %s; ", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail]);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneLoopTop == TRUE) // 20110521) iFlagTuningMotionZ_B1W
			{
				idxTuningBlockBondHead = WB_BH_LOOP_TOP; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				mtn_wb_tune_bond_1_wire_bh(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				cstrTemp.AppendFormat("LoopTop: %s; ", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail]);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneTraj2B == TRUE) // 20110521)  iFlagTuningMotionZ_B1W
			{
				idxTuningBlockBondHead = WB_BH_TRAJECTORY; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				mtn_wb_tune_bond_1_wire_bh(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				cstrTemp.AppendFormat("Traj: %s; ", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail]);
			}

			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneSrch2B == TRUE)  // iFlagIsActualContact
			{
				idxTuningBlockBondHead = WB_BH_2ND_CONTACT; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead);astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				mtn_wb_tune_bond_1_wire_bh(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				cstrTemp.AppendFormat("2nd_Contact: %s; ", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail]);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneTail == TRUE) // 20110521)  iFlagTuningMotionZ_B1W
			{
				idxTuningBlockBondHead = WB_BH_TAIL; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				mtn_wb_tune_bond_1_wire_bh(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				cstrTemp.AppendFormat("Tail: %s; ", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail]);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneReset == TRUE) // 20110521)  iFlagTuningMotionZ_B1W
			{
				idxTuningBlockBondHead = WB_BH_RESET; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				mtn_wb_tune_bond_1_wire_bh(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				cstrTemp.AppendFormat("Reset: %s; ", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail]);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneIdle == TRUE) // 20110521)  // iFlagTuningMotionZ_B1W
			{
				idxTuningBlockBondHead = WB_BH_IDLE; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				mtn_wb_tune_bond_1_wire_bh(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				iFlagTuneIdleIsFail = iFlagTuneIdleIsFail * astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail; // 20110512
				cstrTemp.Format("IDLE: %s; ", astrPassOrFail[iFlagTuneIdleIsFail]);
			}
			// Need to tuning setting parameters, WB_BH_IDLE

			// if iFlagTuningContact, WB_BH_1ST_CONTACT
			// WB_BH_2ND_CONTACT

			mtn_wb_tune_b1w_save_feasible_set_file();
			mtn_tune_save_waveform("TuneOut");
// Verification if OK
			return cstrTemp;

}
// 			mMotionTuning.mtn_tune_set_tune_axis(sys_get_acs_axis_id_bnd_z()); mtn_wb_tune_set_curr_wb_axis_servo_para_tuning(WB_AXIS_BOND_Z);
CString AutoTuneBondHeadB1W_StopSrch()
{
		CString cstrTemp;
int iFlagTuneIdleIsFail;
MTN_TUNE_PARA_UNION stBestParameterSetTuneOut;
int iFlagUpdateInitPara = 0;
	mtn_wb_tune_b1w_init_feasible_set();

			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneIdle == TRUE)
			{
				idxTuningBlockBondHead = WB_BH_IDLE; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				mtn_wb_tune_bh_b1w_stop_srch(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				if(astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail == 0)
				{
					stBestParameterSetTuneOut.stMtnPara = astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stBestParameterSet;
					iFlagUpdateInitPara = 1;
				}
				iFlagTuneIdleIsFail = astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail;
//				cstrTemp.Format("IDLE: %s; ", astrPassOrFail[]);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneMoveTo1B == TRUE) // 20110521, iFlagTuningMotionZ_B1W
			{
				idxTuningBlockBondHead = WB_BH_SRCH_HT; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
//				if(iFlagUpdateInitPara == 1) astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stTuningParameterIni.stMtnPara = stBestParameterSetTuneOut.stMtnPara;
				mtn_wb_tune_bh_b1w_stop_srch(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				if(astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail == 0)
				{
					stBestParameterSetTuneOut.stMtnPara = astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stBestParameterSet;
					
					iFlagUpdateInitPara = 1;
				}
				cstrTemp.Format("Srch_Ht: %s; ", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail]);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneSrch1B == TRUE)  // iFlagIsActualContact == TRUE && 
			{
				idxTuningBlockBondHead = WB_BH_1ST_CONTACT; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead);astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
//				if(iFlagUpdateInitPara == 1) astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stTuningParameterIni.stMtnPara = stBestParameterSetTuneOut.stMtnPara;
				mtn_wb_tune_bh_b1w_stop_srch(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				if(astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail == 0)
				{
					stBestParameterSetTuneOut.stMtnPara = astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stBestParameterSet;
					iFlagUpdateInitPara = 1;
				}
				cstrTemp.AppendFormat("1st_Contact: %s; ", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail]);
			}

			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneLooping == TRUE) // 20110521) iFlagTuningMotionZ_B1W
			{
				idxTuningBlockBondHead = WB_BH_LOOPING; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead);astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
//				if(iFlagUpdateInitPara == 1) astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stTuningParameterIni.stMtnPara = stBestParameterSetTuneOut.stMtnPara;
				mtn_wb_tune_bh_b1w_stop_srch(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				if(astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail == 0)
				{
					stBestParameterSetTuneOut.stMtnPara = astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stBestParameterSet;
					iFlagUpdateInitPara = 1;
				}
				cstrTemp.AppendFormat("Looping: %s; ", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail]);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneLoopTop == TRUE) // 20110521) iFlagTuningMotionZ_B1W
			{
				idxTuningBlockBondHead = WB_BH_LOOP_TOP; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
//				if(iFlagUpdateInitPara == 1) astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stTuningParameterIni.stMtnPara = stBestParameterSetTuneOut.stMtnPara;
				mtn_wb_tune_bh_b1w_stop_srch(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				if(astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail == 0)
				{
					stBestParameterSetTuneOut.stMtnPara = astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stBestParameterSet;
					iFlagUpdateInitPara = 1;
				}
				cstrTemp.AppendFormat("LoopTop: %s; ", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail]);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneTraj2B == TRUE) // 20110521)  iFlagTuningMotionZ_B1W
			{
				idxTuningBlockBondHead = WB_BH_TRAJECTORY; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
//				if(iFlagUpdateInitPara == 1) astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stTuningParameterIni.stMtnPara = stBestParameterSetTuneOut.stMtnPara;
				mtn_wb_tune_bh_b1w_stop_srch(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				if(astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail == 0)
				{
					stBestParameterSetTuneOut.stMtnPara = astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stBestParameterSet;
					iFlagUpdateInitPara = 1;
				}
				cstrTemp.AppendFormat("Traj: %s; ", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail]);
			}

			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneSrch2B == TRUE)  // iFlagIsActualContact
			{
				idxTuningBlockBondHead = WB_BH_2ND_CONTACT; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead);astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
//				if(iFlagUpdateInitPara == 1) astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stTuningParameterIni.stMtnPara = stBestParameterSetTuneOut.stMtnPara;
				mtn_wb_tune_bh_b1w_stop_srch(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				if(astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail == 0)
				{
					stBestParameterSetTuneOut.stMtnPara = astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stBestParameterSet;
					iFlagUpdateInitPara = 1;
				}
				cstrTemp.AppendFormat("2nd_Contact: %s; ", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail]);
			}
//
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneTail == TRUE) // 20110521)  iFlagTuningMotionZ_B1W
			{
				idxTuningBlockBondHead = WB_BH_TAIL; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
//				if(iFlagUpdateInitPara == 1) astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stTuningParameterIni.stMtnPara = stBestParameterSetTuneOut.stMtnPara;
				mtn_wb_tune_bh_b1w_stop_srch(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				if(astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail == 0)
				{
					stBestParameterSetTuneOut.stMtnPara = astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stBestParameterSet;
					iFlagUpdateInitPara = 1;
				}
				cstrTemp.AppendFormat("Tail: %s; ", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail]);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneReset == TRUE) // 20110521)  iFlagTuningMotionZ_B1W
			{
				idxTuningBlockBondHead = WB_BH_RESET; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
//				if(iFlagUpdateInitPara == 1) astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stTuningParameterIni.stMtnPara = stBestParameterSetTuneOut.stMtnPara;
				mtn_wb_tune_bh_b1w_stop_srch(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				if(astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail == 0)
				{
					stBestParameterSetTuneOut.stMtnPara = astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stBestParameterSet;
					iFlagUpdateInitPara = 1;
				}
				cstrTemp.AppendFormat("Reset: %s; ", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail]);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iFlagB1W_TuneIdle == TRUE) // 20110521)  // iFlagTuningMotionZ_B1W
			{
				idxTuningBlockBondHead = WB_BH_IDLE; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
//				if(iFlagUpdateInitPara == 1) astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stTuningParameterIni.stMtnPara = stBestParameterSetTuneOut.stMtnPara;
				mtn_wb_tune_bh_b1w_stop_srch(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], idxTuningBlockBondHead);
				iFlagTuneIdleIsFail = iFlagTuneIdleIsFail * astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail; // 20110512
				if(astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iFlagTuningIsFail == 0)
				{
					stBestParameterSetTuneOut.stMtnPara = astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].stBestParameterSet;
					iFlagUpdateInitPara = 1;
				}
				cstrTemp.Format("IDLE: %s; ", astrPassOrFail[iFlagTuneIdleIsFail]);
			} // 20120802, AutoTuning B1W

			mtn_wb_tune_b1w_save_feasible_set_file();
			return cstrTemp;

}

//// ACS-System Error Code and string
static int iErrorCodeACS;
char strErrorStrACS[256];   // 20100305
int iActualNumByteReceived;
void mtn_api_prompt_acs_error_code(HANDLE stCommHandleAcs)
{
CString cstrTemp;
int iFlagHasError = 0;
//int ii;
	cstrTemp.Format(_T("ACS Error. "));
	acsc_GetMotorError(stCommHandleAcs, ACSC_AXIS_X, &iErrorCodeACS, NULL);
	if(iErrorCodeACS != 0)
	{
		acsc_GetErrorString(stCommHandleAcs,iErrorCodeACS,// error code
			strErrorStrACS,// buffer for the error explanation
			255,// available buffer length
			&iActualNumByteReceived// number of actually received bytes
			);
		cstrTemp.AppendFormat("X:%d (%s) ", iErrorCodeACS, strErrorStrACS);
		cstrTemp.AppendFormat("\r\n");
		iFlagHasError = 1;
	}

	acsc_GetMotorError(stCommHandleAcs, ACSC_AXIS_Y, &iErrorCodeACS, NULL);
	if(iErrorCodeACS != 0)
	{
		acsc_GetErrorString(stCommHandleAcs,iErrorCodeACS,// error code
			strErrorStrACS,// buffer for the error explanation
			255,// available buffer length
			&iActualNumByteReceived// number of actually received bytes
			);
		cstrTemp.AppendFormat("Y:%d (%s) ", iErrorCodeACS, strErrorStrACS);
		cstrTemp.AppendFormat("\r\n");
		iFlagHasError = 1;
	}

	acsc_GetMotorError(stCommHandleAcs, ACSC_AXIS_A, &iErrorCodeACS, NULL);
	if(iErrorCodeACS != 0)
	{
		acsc_GetErrorString(stCommHandleAcs,iErrorCodeACS,// error code
			strErrorStrACS,// buffer for the error explanation
			255,// available buffer length
			&iActualNumByteReceived// number of actually received bytes
			);
		cstrTemp.AppendFormat("Z:%d (%s) ", iErrorCodeACS, strErrorStrACS);
		cstrTemp.AppendFormat("\r\n");
		iFlagHasError = 1;
	}

	acsc_GetMotorError(stCommHandleAcs, ACSC_AXIS_B, &iErrorCodeACS, NULL);
	if(iErrorCodeACS != 0)
	{
		acsc_GetErrorString(stCommHandleAcs,iErrorCodeACS,// error code
			strErrorStrACS,// buffer for the error explanation
			255,// available buffer length
			&iActualNumByteReceived// number of actually received bytes
			);
		cstrTemp.AppendFormat("W:%d (%s) ", iErrorCodeACS, strErrorStrACS);
		cstrTemp.AppendFormat("\r\n");
		iFlagHasError = 1;
	}
int aiProgramErrLine[12];
	acsc_ReadInteger(stCommHandleAcs, ACSC_NONE, "PERL", 0, NUM_TOTAL_PROGRAM_BUFFER-1, ACSC_NONE, ACSC_NONE, aiProgramErrLine, NULL );
	for(int ii=0; ii<NUM_TOTAL_PROGRAM_BUFFER; ii++)
	{
		acsc_GetProgramError(stCommHandleAcs, ii, &iErrorCodeACS, NULL);
		if(iErrorCodeACS != 0)
		{

			acsc_GetErrorString(stCommHandleAcs,iErrorCodeACS,// error code
				strErrorStrACS,// buffer for the error explanation
				255,// available buffer length
				&iActualNumByteReceived// number of actually received bytes
				);
			
			cstrTemp.AppendFormat("Prog-%d:%d (%s), L-%d", ii, iErrorCodeACS, strErrorStrACS, aiProgramErrLine[ii]);
		}
		cstrTemp.AppendFormat("\r\n");
	}

	if(iErrorCodeACS = acsc_GetLastError())
	{
		acsc_GetErrorString(stCommHandleAcs,// communication handle
			iErrorCodeACS,// error code
			strErrorStrACS,// buffer for the error explanation
			255,// available buffer length
			&iActualNumByteReceived// number of actually received bytes
			);
//			cstrTemp = _T(strErrorStrACS);
			cstrTemp.AppendFormat("  ErrorCode-%d", iErrorCodeACS);
		iFlagHasError = 1;
	}
	if(		iFlagHasError == 1)
	{
		AfxMessageBox(cstrTemp);  // , MB_OKCANCEL
	}
}