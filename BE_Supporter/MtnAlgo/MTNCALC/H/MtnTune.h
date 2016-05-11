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



#ifndef __MTN_TUNE__
#define __MTN_TUNE__


#include "MtnApi.h"
#include "GA.h"

#include "MtnWbDef.h"


// Servo Setup Procedure
#define SERVO_AFT_VERIFY_ENCODER			0
#define SERVO_AFT_VERIFY_OPEN_LOOP			1
#define SERVO_AFT_VERIFY_VELOCITY_LOOP		2
#define SERVO_AFT_VERIFY_SPECTRUM			3
#define SERVO_AFT_VERIFY_MOVE_TEST			4
//#define SERVO_AFT_VERIFY_RANDOM_POINTS		4
//#define SERVO_AFT_VERIFY_FIXED_PATTERN		5
#define SERVO_AFT_VERIFY_FORCE_CTRL			5
#define SERVO_AFT_VERIFY_LOOPING			6
#define SERVO_AFT_PASS						7

// Structures for individual test
#define MAX_SIGNED_DAC_16BIT   (32767.0)

////////// Pass Fail threshold for B1W-Tuning 
#define _B1W_TUNE_OBJ_TH_IDLE            500
#define _B1W_TUNE_OBJ_TH_1ST_SRCH        120
#define _B1W_TUNE_OBJ_TH_MOVE_1ST_SRCH   80
#define _B1W_TUNE_OBJ_TH_LOOPING         600
#define _B1W_TUNE_OBJ_TH_LOOPTOP         120
#define _B1W_TUNE_OBJ_TH_TRAJECTORY      400
#define _B1W_TUNE_OBJ_TH_TAIL            150
#define _B1W_TUNE_OBJ_TH_RESET           300
#define _B1W_TUNE_OBJ_TH_2ND_SRCH        120

typedef struct
{
	int idx1stMoveFrontStart;
	int idx1stMoveFrontEnd;
	int idx1stMoveBackStart;
	int idx1stMoveBackEnd;
	int idx2ndMoveFrontStart;
	int idx2ndMoveFrontEnd;
	int idx2ndMoveBackStart;
	int idx2ndMoveBackEnd;
}MTN_TIME_IDX_2_LOOP_MOVE;


typedef struct
{
	double	fMaxFrontMoveDPE;
	double	fDirFrontMove;
	double	fMaxBackMoveDPE;
	double	fDirBackMove;

	double	fMaxFrontOS; 
	double	fMaxFrontUS; 
	double	fCmdEndFrontOUS;
	double	fMaxBackOS; 
	double	fMaxBackUS; 
	double	fCmdEndBackOUS;

	double	fSettleTime1stFront;
	double	fSettleTime2ndFront;
	double	fSettleTime1stBack;
	double	fSettleTime2ndBack;

	double  fSettleTimeFrontDir;
	double  fSettleTimeBackDir;

	double	fRptErrorRMS;

	double  fRMS_StaticPE;

	double fPeakAbsDAC;
	double fPeakPercentDAC;
	double fRootMeanSquareDAC;
	double fRMS_Over32767_DAC;
}MTN_SERVO_TUNE_INDEX;

short qc_is_axis_not_safe(HANDLE hCommunicationHandle, int iAxis);
short qc_is_axis_still_acc_dec(HANDLE hCommunicationHandle, int iAxis);
//short qc_is_any_motor_still_moving(HANDLE hCommunicationHandle);
//short qc_is_any_motor_not_safe(HANDLE hCommunicationHandle);

extern char *mtn_api_get_acs_error_string();
extern int mtn_api_prompt_acs_error_message(HANDLE stCommHandleAcs);

extern short mtn_tune_init_array_ptr();
extern void mtn_tune_calc_time_idx_2_loop_move(MTN_TIME_IDX_2_LOOP_MOVE *stpTimeIdx2LoopMove);
extern void mtn_tune_calc_servo_perform_index(double fThresholdSettleTime, MTN_TIME_IDX_2_LOOP_MOVE *stpTimeIdx2LoopMove, MTN_SERVO_TUNE_INDEX *stpMtnServoTuneIndex);
extern void mtn_tune_initialize_class_var(double fThresholdSettleTime, 
								   HANDLE stAcsHandle, 
								   unsigned int uiCurrMoveAxis_ACS,
								   int iInterMoveDelay);


#define  __MTN_TUNE_NUM_GROUP_POINTS__			8
#define  __MTN_TUNE_OPTION_SLAFF__      1
#define  __MTN_TUNE_OPTION_VELLOOP__    2
#define  __MTN_TUNE_OPTION_POSNLOOP__   3
#define  __MTN_TUNE_OPTION_GA__         4

#define __MTN_TUNE_SLAFF_SUCCESS_RET          0
#define __MTN_TUNE_SLAFF_STAGE_FIND_RANGE__   1
#define __MTN_TUNE_SLAFF_STAGE_FINE_SEARCH__  2
#define __MTN_TUNE_SLAFF_ERROR_RET            3
#define __MTN_TUNE_AFF_ERROR_INITIAL_ZERO__   4

typedef struct
{
	double fInitSLAFF;
	double fWeightPositiveDPE;
	double fWeightNegativeDPE;
	double fWeightDiffDPE;
	double fWeightSettlingTime;
	double fWeightCmdEndOverUnderShoot;
	double fWeightRMS_DAC;
	double fWeightPeakDAC;
	int iMovePoint1;
	int iMovePoint2;
	int iFlagEnableDebug;

}MTN_TUNE_SEARCH_AFF_INPUT;

typedef struct
{
	double fServoLoopAFF;
	double fBestObj;
	int  iErrorCode;
}MTN_TUNE_SEARCH_AFF_OUTPUT;

////// Genenal Tuning for common motion
typedef struct
{
	double dEncCntUnit_um;
	double dSampleTime_ms;
	double dSettleThreshold_um;
}MTN_TUNE_ENC_TIME_CFG;

typedef struct
{
	double dMovePosition_1;
	double dMovePointion_2;
	int iIterMotionDelay_ms;
	MTN_SPEED_PROFILE stMtnSpeedProfile;

	double d2ndMovePosition_1;
	double d2ndMovePosition_2;
}MTN_TUNE_CONDITION;

typedef struct
{
	double fWeightPositiveDPE;
	double fWeightNegativeDPE;
	double fWeightDiffDPE;
	double fWeightSettlingTime;
	double fWeightCmdEndOverUnderShoot;
	double fPenaltyCmdEndOverUnderShoot;
	double fWeightRMS_DAC;
	double fWeightPeakDAC;
	double fWeightRMS_StaticPE;
	int iThemeType;   
	/// 0: X-Y Table, Settling Time as small as possible, 
	///				  RMS_DAC and Peak_DAC within range
	///
	/// 1: Bnd_Z, CmdEnd must be under shoot
	///				Settling Time as small as possible
	///				RMS_DAC and Peak_DAC within range
}MTN_TUNE_THEME;

#define MTN_TUNE_OBJ_LARGE_NUMBER   10000
typedef enum
{
	TUNE_THEME_OPT_MIN_DPE_DIFF_DPE = 0,
	TUNE_THEME_OPT_MIN_CEOUS = 1,
	TUNE_THEME_MIN_SETTLE_TIME = 2,
	TUNE_THEME_MIN_PEAK_DAC = 3,
	TUNE_THEME_MIN_RMS_DAC = 4,
	THEME_MIN_CEOUS_DPE = 5,
	THEME_MIN_CEOUS_SETTLE_TIME = 6,
	THEME_MIN_DPE_SETTLE_TIME = 7,
	THEME_MIN_DPE_W_DAC = 8,
	THEME_MIN_CEOUS_W_DAC = 9,
	THEME_MIN_S_TIME_W_DAC = 10,
	THEME_MIN_CEOUS_DPE_W_DAC = 11,
	THEME_MIN_CEOUS_SETTLE_TIME_W_DAC = 12,
	THEME_MIN_DPE_SETTLE_TIME_W_DAC = 13,
	THEME_MIN_CEOUS_BH_DPE_W_DAC = 14,
	THEME_MIN_CEOUS_BH_W_DAC = 15,
	THEME_WB_B1W = 16,
};
int mtn_tune_get_tune_theme_option();
void mtn_tune_set_tune_theme_option(int iOpt);

typedef struct
{
	double dSettleTime_ms;
	double dCmdEndOverUnderShoot_um;
	double dOverUnderShoot_um;
	double dMaxDPE_um;
	double dRptError;
	double dPeakDAC_pc; // unit is percent of maximum 16 bit signed-range
	double dRMS_DAC_pc;

}MTN_TUNE_SPECIFICATION;

//enum
//{
//	GA_PARA_IDX_PKP = 0;
//	GA_PARA_IDX_VKP = 1;
//	GA_PARA_IDX_VKI = 2;
//	GA_PARA_IDX_VLI = 3;
//	GA_PARA_IDX_AFF = 4;
//};

typedef struct
{
	double dPosnLoopKP;
	double dVelLoopKP;
	double dVelLoopKI;
	double dVelLoopLimitI;
	double dAccFFC;
	double dSecOrdFilterFreq_Hz;
	double dSecOrdFilterDamping;
	double dServoLoopDRA;
	double dServoLoopDRX;
	double dNotchFilterFrequency;
	double dNotchFilterWidth;
	double dNotchFilterAttenuation;
	double dDynamicFrictionFeedforward;
	double dJerkFf;
}MTN_TUNE_PARAMETER_SET;

typedef union 
{
	MTN_TUNE_PARAMETER_SET stMtnPara;
	double afGA_PopulaVar[sizeof(MTN_TUNE_PARAMETER_SET)/sizeof(double)];
}MTN_TUNE_PARA_UNION;

#define SERVO_PARA_TUNE_ALGO_COMPLETE_SEARCH   0
#define SERVO_PARA_TUNE_ALGO_1_DIM_AFT_1       1
#define SERVO_PARA_TUNE_ALGO_GENETIC		   2

typedef struct
{
	unsigned int uiPopulationSize;
	unsigned int uiMaxGeneration;
	double dStopingStd;
	double dCrossOverPercent;
	double dMutionPercent;
}TUNE_ALGO_SETTING;
// loop for each iteration , generation
// according to tuning theme, calculation TuningObj function, 
//					store all passed tuning cases, num. passed case ++
//					selection -> cross_over  -> mutation
// if StDev(population para) < dStopingStd; exit
// if iteration >= uiMaxGeneration exit
// loop else next iteration 
// exit:
//     if num. passed case > 1, pass, return best feasible solution in the passed cases
//	   else, fail, 

typedef struct
{
	TUNE_ALGO_SETTING       stTuningAlgoSetting;
	MTN_TUNE_PARA_UNION	stTuningParameterIni;
	MTN_TUNE_PARA_UNION	stTuningParameterUppBound;
	MTN_TUNE_PARA_UNION	stTuningParameterLowBound;
	MTN_TUNE_THEME			stTuningTheme;
	MTN_TUNE_CONDITION		stTuningCondition;
	MTN_TUNE_ENC_TIME_CFG	stTuningEncTimeConfig;
	MTN_TUNE_SPECIFICATION  stTuningPassSpecfication;
	HANDLE stCommHandle;
	int	iAxisTuning;
	int iDebugFlag;

}MTN_TUNE_GENETIC_INPUT;

// Error Code
#define MTN_TUNE_ERROR_OPEN_FILE   301

#define MAX_DATA_LEN_CAPTURE_IN_TUNING    6000

#define MAX_CASE_TUNING_HISTORY_BUFFER    500
#define MAX_CASE_FEASIBLE_B1W_PARAMETER   300

extern short qc_is_axis_still_acc_dec(HANDLE hCommunicationHandle, int iAxis);
extern short qc_is_axis_not_safe(HANDLE hCommunicationHandle, int iAxis);

///// Wb_Waveform performance calculation Tuning
#define MAX_LEN_FORCE_CTRL_MS		100
typedef struct
{
	double f1stBondMoveToSearchHeightOverUnderShoot;
	double fReverseHeightOverUnderShoot;
	double fKinkHeightOverUnderShoot;
	double fLoopTopOverUnderShoot;
	double f2ndBondMoveToSearchHeightOverUnderShoot;
	double fTailOverUnderShoot;
	double fEndFireLevelOverUnderShoot;
	double f1stBondForceCtrlPosnRipplePtoP;
	double f1stBondForceCtrlPosnRippleStd;
	double af1stBondForceCtrlFbPosn[MAX_LEN_FORCE_CTRL_MS];
	double f2ndBondForceCtrlPosnRipplePtoP;
	double f2ndBondForceCtrlPosnRippleStd;
	double af2ndBondForceCtrlFbPosn[MAX_LEN_FORCE_CTRL_MS];

	double af1stBondSearchFbVel[MAX_LEN_FORCE_CTRL_MS];
	double af2ndBondSearchFbVel[MAX_LEN_FORCE_CTRL_MS];

	double af1stBondSearchVelErr[MAX_LEN_FORCE_CTRL_MS];
	double af2ndBondSearchVelErr[MAX_LEN_FORCE_CTRL_MS];

//	double f1stBondSearchVelErrStdAfterSrchDelay;
	double f1stBondSearchVelErrP2PAfterSrchDelay;
	double f1stBondSearchFeedAccRMS;

//	double f2ndBondSearchVelErrStdAfterSrchDelay;
	double f2ndBondSearchVelErrP2PAfterSrchDelay;
	double f2ndBondSearchFeedAccRMS;

	double f1stBondVelErrAtMoveSrchHeight;
	double f2ndBondVelErrAtTrajEnd;

	int  iFlagIsFalseContact1stB;
	int  iFlagIsFalseContact2ndB;

	/////// Deformation Rate, Calculation
	double dRefDeformPosition1stB;
	double dRefDeformPosition2ndB;
	double adDeformPosn1stB[MAX_LEN_FORCE_CTRL_MS];
	double adDeformPosn2ndB[MAX_LEN_FORCE_CTRL_MS];

	////// Idle Settling RMS
	double dIdleRMS_5ms;
}BOND_HEAD_PERFORMANCE;

typedef struct
{
	MTN_TUNE_PARA_UNION stServoPara;
	MTN_SERVO_TUNE_INDEX stServoTuneIndex;
	int iFlagTuningIsFail;   // 0: pass, 1: Fail
	double dTuningObj;
	BOND_HEAD_PERFORMANCE stTuneBondHeadPerformanceB1W;
	MTN_TUNE_PARA_UNION stServoParaB1W[WB_BH_2ND_CONTACT+1];
	WB_TUNE_B1W_BH_OBJ stTuneB1wObjValues;
}MTN_TUNE_CASE;

#define POSN_SETTLE_TH_DURING_TUNING  20

typedef struct
{
	int iFlagTuningIsFail;   // 0: pass, 1: Fail
	double dTuningObj;
	MTN_TUNE_PARAMETER_SET stBestParameterSet;
	MTN_SERVO_TUNE_INDEX stResultResponse;
	double dInitialObj;
	MTN_SERVO_TUNE_INDEX stInitialResponse;
	BOND_HEAD_PERFORMANCE stInitialB1WPerformanceBH;

}MTN_TUNE_GENETIC_OUTPUT;

typedef struct
{
	int	idxStartMove1stBondSearchHeight	;
	int	idxEndMove1stBondSearchHeight	;
	int	idxStart1stBondForceCtrl	;
	int	idxEnd1stBondForceCtrl	;
	int	idxStartReverseHeight	;
	int	idxEndReverseHeight	;
	int	idxStartKinkHeight	;
	int	idxEndKinkHeight	;
	int	idxStartMoveLoopTop	;
	int	idxEndMoveLoopTop	;
	int	idxStartTrajectory	;
	int	idxEndTrajectory2ndBondSearchHeight	;
	int	idxStart2ndBondForceCtrl	;
	int	idxEnd2ndBondForceCtrl	;
	int	idxStartTail	;
	int	idxEndTail	;
	int	idxStartFireLevel	;
	int	idxEndFireLevel	;
	/// 
	int idxStart2ndBondSearchContact;

	/// Deformation rate calcuation
	int idxStartCalcDeform1stB;
	int idxStartCalcDeform2ndB;
}TIME_POINTS_OF_BONDHEAD_Z;

typedef struct
{
	double	fStartFireLevel_Z	;
	double	f1stBondSearchHeight_Z	;
	double	f1stBondContactPosn_Z	;
	double	fReverseHeightPosn_Z	;
	double	fKinkHeightPosn_Z	;
	double	fLoopTopPosn_Z	;
	double	f2ndBondSearchHeightPosn_Z	;
	double	f2ndBondContactPosn_Z	;
	double	fTailPosn_Z	;
	double	fEndFireLevel_Z	;
	double  f2ndBondStartSearchContact_Z;
	
}POSNS_OF_BONDHEAD_Z;

typedef struct
{
	int	idxStartMoveX1stBond	;
	int	idxStartMoveY1stBond	;
	int	idxEndMoveX_1stBondPosn	;
	int	idxEndMoveY_1stBondPosn	;
	int	idxStartReverseMoveX	;
	int	idxStartReverseMoveY	;
	int	idxEndReverseMoveX	;
	int	idxEndReverseMoveY	;
	int	idxStartTrajTo2ndBondX	;
	int	idxStartTrajTo2ndBondY	;
	int	idxEndTraj2ndBondPosnX	;
	int	idxEndTraj2ndBondPosnY	;
	int	idxStartMoveNextPR_X ;
	int	idxEndMoveNextPR_X ;
	int	idxStartMoveNextPR_Y ;
	int	idxEndMoveNextPR_Y ;
}TIME_POINTS_OF_TABLE;

typedef struct
{
	double	fStartPosition_X	;
	double	fStartPosition_Y	;
	double	f1stBondPosition_X	;
	double	f1stBondPosition_Y	;
	double	f1stBondReversePosition_X	;
	double	f1stBondReversePosition_Y	;
	double	fTrajEnd2ndBondPosn_X	;
	double	fTrajEnd2ndBondPosn_Y	;
	double  fMoveToNextPR_Posn_X;
	double  fMoveToNextPR_Posn_Y;
}POSNS_OF_TABLE;

#define MAX_LENGTH_TABLE_SETTLE		100
typedef struct
{
	double	afMaxDPE_MoveTo1stBondPosn_BTO[2]	;
	double	fCEOUS_MoveTo1stBondPosn_BTO	;
	double	afMaxDPE_MoveToKink1[2]	;
	double	fCEOUS_MoveToKink1	;
	double	afMaxDPE_TrajMoveTo2ndBond[2]	;
	double	fCEOUS_TrajMoveTo2ndBond	;
	double	afMaxDPE_MoveToNextPR[2]	;
	double	fCEOUS_MoveToNextPR	;
	double  afTablePosnErrSettle_MoveTo1stBond[MAX_LENGTH_TABLE_SETTLE];
	double  afTablePosnErrSettle_Kink1[MAX_LENGTH_TABLE_SETTLE];
	double  afTablePosnErrSettle_MoveTo2ndBond[MAX_LENGTH_TABLE_SETTLE];

	double  fTable_SettlingTimeMove1B;
	double  fTable_SettlingTimeMove2B;

	double  afTblPosnErrFromSrch1B[MAX_LENGTH_TABLE_SETTLE];
	double  afTblPosnErrFromSrch2B[MAX_LENGTH_TABLE_SETTLE];

	double  afTblPosnErrContact1B[MAX_LENGTH_TABLE_SETTLE];
	double  afTblPosnErrContact2B[MAX_LENGTH_TABLE_SETTLE];

	double  fTblPosnErrP2PContact1B;
	double  fTblMaxAbsPosnErrContact1B;
	double  fTblPosnErrP2PContact2B;
	double  fTblMaxAbsPosnErrContact2B;

}TABLE_WB_PERFORMANCE;

typedef struct
{
	BOND_HEAD_PERFORMANCE stBondHeadPerformance;
	TIME_POINTS_OF_BONDHEAD_Z stTimePointsOfBondHeadZ;
	POSNS_OF_BONDHEAD_Z stPosnsOfBondHeadZ;
	POSNS_OF_TABLE stPosnsOfTable;
	TIME_POINTS_OF_TABLE stTimePointsOfTable;
	TABLE_WB_PERFORMANCE stTableXPerformance;
	TABLE_WB_PERFORMANCE stTableYPerformance;
}WB_WAVEFORM_PERFORMANCE_CALC;

typedef struct
{
	int    idxStartMotion;
	int    idxEndMotion;
	double    fMotionTargetPosn;
	double    fMotionStartPosn;
}ONE_MOTION_INFO;

typedef struct
{
	int idxStartOneWireZ;
	int idxStartOneWireXY;
}START_ONE_WIRE_TIME_INFO;

typedef struct
{
	double dMaxRefVelMoveToSrchHeight;
	double dMaxRefAccMoveToSrchHeight;
	double dMaxRefJerkMoveToSrchHeight;
	double dMaxRefVelMoveReverseHeight;
	double dMaxRefAccMoveReverseHeight;
	double dMaxRefJerkMoveReverseHeight;
	double dMaxRefVelMoveReverseDist;
	double dMaxRefAccMoveReverseDist;
	double dMaxRefJerkMoveReverseDist;
	double dMaxRefVelMoveLoopTop;
	double dMaxRefAccMoveLoopTop;
	double dMaxRefJerkMoveLoopTop;
	double dMaxRefVelMoveTrajectory;
	double dMaxRefAccMoveTrajectory;
	double dMaxRefJerkMoveTrajectory;
	double dMaxRefVelMoveTail;
	double dMaxRefAccMoveTail;
	double dMaxRefJerkMoveTail;
	double dMaxRefVelMoveResetLevel;
	double dMaxRefAccMoveResetLevel;
	double dMaxRefJerkMoveResetLevel;

	double dSrchVel1stBond;
	double dSrchVel2ndBond;

}BOND_HEAD_SPEED_SETTING;

typedef struct
{
	double dMaxRefAccMove1stB_BTO;
	double dMaxRefJerkMove1stB_BTO;

	double dMaxRefAccReverseDist;
	double dMaxRefJerkReverseDist;

	double dMaxRefAccTraj;
	double dMaxRefJerkTraj;

}TABLE_SPEEDING_SETTING;

#define NUM_MOTION_AFTER_1ST_BOND_FORCE_WITHIN_ONE_WIRE    10
#define MAX_NU_ERROR_PULL_MOTION_PER_WIRE				10
typedef struct
{
	int  iFlagHasWireInfo;
	int  idxEndOneWireZ;
	int  idxEndOneWireXY;
	int  nTotalBondHeadZ_MotionAft1stBond;
	int  iFlagIsDryRun;
	int  iFlagBallBond;
	unsigned int nTimeB1W_cnt;
	unsigned int nTimeB1W_S2E_cnt;
	TIME_POINTS_OF_BONDHEAD_Z    stTimePointsOfBondHeadZ;
	POSNS_OF_BONDHEAD_Z    stPosnsOfBondHeadZ;
	ONE_MOTION_INFO    astBondHeadMotionAft1stBond[NUM_MOTION_AFTER_1ST_BOND_FORCE_WITHIN_ONE_WIRE];
	BOND_HEAD_PERFORMANCE    stBondHeadPerformance;
	POSNS_OF_TABLE    stPosnsOfTable;
	TIME_POINTS_OF_TABLE    stTimePointsOfTable;
	TABLE_WB_PERFORMANCE    stTableXPerformance;
	TABLE_WB_PERFORMANCE    stTableYPerformance;

	BOND_HEAD_SPEED_SETTING   stBondHeadSpeedSetting;
	TABLE_SPEEDING_SETTING stActSpeedTableX;
	TABLE_SPEEDING_SETTING stActSpeedTableY;

	//// 20111011
	double fBondHeadSetting_1stB_SrchSafeTH;
	double fBondHeadSetting_2ndB_SrchSafeTH;

	// Debug for error truncation
	ONE_MOTION_INFO stTableX_ErrPullMotion[MAX_NU_ERROR_PULL_MOTION_PER_WIRE];
	ONE_MOTION_INFO stTableY_ErrPullMotion[MAX_NU_ERROR_PULL_MOTION_PER_WIRE];
	int  nNumPullMotionX;
	int  nNumPullMotionY;

}WB_ONE_WIRE_PERFORMANCE_CALC;

typedef struct
{
	double dMin;
	double dMax;
	double dStd;
}MULTI_WIRE_PERFORM_STAT;

void mtn_tune_get_traj_points(WB_ONE_WIRE_PERFORMANCE_CALC *stpAnalyzeOneWire, 
							  double *adRefVelocityBondHeadZ_Traj, double *adRefVelocityTableX_Traj, double *adRefVelocityTableY_Traj, 
							  int *nTrajLen);

#define NUM_TOTAL_WIRE_IN_ONE_SCOPE  10
void mtn_tune_calc_one_wire_performance(START_ONE_WIRE_TIME_INFO *stpStartOneWireTimeInfo, WB_ONE_WIRE_PERFORMANCE_CALC *stpAnalyzeOneWire);
void mtn_tune_ana_wb_waveform(WB_ONE_WIRE_PERFORMANCE_CALC astWbOneWirePerformance[], int *nTotalNumWires);

#define WB_TUNE_B1W_SEGMENT_MOVE_SRCH_HEIGHT   1
#define WB_TUNE_B1W_SEGMENT_LOOPING			   2
#define WB_TUNE_B1W_SEGMENT_LOOP_TOP		   3
#define WB_TUNE_B1W_SEGMENT_TRAJ			   4
#define WB_TUNE_B1W_SEGMENT_TAIL			   5
#define WB_TUNE_B1W_SEGMENT_RESET_LEVEL		   6
/// Objective function for b1w tuning Z
double mtn_tune_calc_b1w_bh_performance(WB_ONE_WIRE_PERFORMANCE_CALC *stpAnalyzeOneWire, MTN_TUNE_THEME *stpTuneTheme, int iObjSectionFlagB1W);
double mtn_wb_tune_b1w_calc_bh_obj(TIME_POINTS_OF_BONDHEAD_Z *stpTimePointsOfBondHeadZ, int iObjSectionFlagB1W);
int mtn_wb_tune_b1w_check_is_feasible_parameter(double adObj[], WB_TUNE_B1W_BH_PASS_FLAG *stpWbTuneB1wPassFlag);
void mtn_wb_tune_b1w_init_feasible_set();
void mtn_wb_tune_b1w_save_feasible_set_file();
void mtn_wb_tune_b1w_record_feasible_parameter(WB_TUNE_B1W_BH_OBJ *stpWbTuneB1wObj, BOND_HEAD_PERFORMANCE  *stpBondHeadPerformance, 
											   int iObjSectionFlagB1W, MTN_TUNE_PARAMETER_SET *stpCurrTuneBondHeadParaB1W);

typedef struct
{
	MULTI_WIRE_PERFORM_STAT stBondHeadMove1stSearchHeight_CEOUS;
	MULTI_WIRE_PERFORM_STAT stBondHeadReverseHeight_CEOUS;
	MULTI_WIRE_PERFORM_STAT stBondHeadKink1_CEOUS;
	MULTI_WIRE_PERFORM_STAT stBondHeadLoopTop_CEOUS;
	MULTI_WIRE_PERFORM_STAT stBondHeadTrajectory_CEOUS;
	MULTI_WIRE_PERFORM_STAT stBondHeadTail_CEOUS;
	MULTI_WIRE_PERFORM_STAT stBondHeadResetFireLevel_CEOUS;
	
	MULTI_WIRE_PERFORM_STAT stBondHead1stContactPosn;
	MULTI_WIRE_PERFORM_STAT stBondHead2ndContactPosn;
	MULTI_WIRE_PERFORM_STAT stBondHead1stBondPosnRippleStd;
	MULTI_WIRE_PERFORM_STAT stBondHead2ndBondPosnRippleStd;
	//
	MULTI_WIRE_PERFORM_STAT stTableXMove1stBond_CEOUS;
	MULTI_WIRE_PERFORM_STAT stTableXKink1_CEOUS;
	MULTI_WIRE_PERFORM_STAT stTableXTrajMove2ndBond_CEOUS;

	// 
	MULTI_WIRE_PERFORM_STAT stTableYMove1stBond_CEOUS;
	MULTI_WIRE_PERFORM_STAT stTableYKink1_CEOUS;
	MULTI_WIRE_PERFORM_STAT stTableYTrajMove2ndBond_CEOUS;

	// 
	int nTotalWires;
	double dMeanTimeB1W_cnt;
	double dTableX1stBondPosnList[NUM_TOTAL_WIRE_IN_ONE_SCOPE];
	double dTableY1stBondPosnList[NUM_TOTAL_WIRE_IN_ONE_SCOPE];
	double dTableX2ndBondPosnList[NUM_TOTAL_WIRE_IN_ONE_SCOPE];
	double dTableY2ndBondPosnList[NUM_TOTAL_WIRE_IN_ONE_SCOPE];
	double dBondHead1stContactPosnList[NUM_TOTAL_WIRE_IN_ONE_SCOPE];
	double dBondHead2ndContactPosnList[NUM_TOTAL_WIRE_IN_ONE_SCOPE];
}MULTI_WIRE_WB_PERFORMANCE;
void mtn_tune_ana_wb_multi_wire_stat(MULTI_WIRE_WB_PERFORMANCE *stpWbMultiWirePerformStat, 
									 WB_ONE_WIRE_PERFORMANCE_CALC astWbOneWirePerformance[], int nTotalNumWires);
void mtn_tune_calc_bond_prog_by_one_wire_info(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbOneWireInfo);
void mtn_tune_get_waveform_data_ptr(double **pdRefPosnX, double **pdFeedPosnX, double **pdRefPosnY, double **pdFeedPosnY, 
									double **pdRefPosnZ, double **pdFeedPosnZ, double **pdWireClampCmd, double **pdMotorFlagZ); // 20120522

int mtn_wb_save_waveform_with_path(char *strNamePrefWithFolderPath);
int mtn_tune_save_waveform(char *strNamePrefix);
int mtn_wb_tune_save_group_tuning_parameter(char *strFilenamePrefix, char *strActualFilename, int iLenChar);
void mtn_wb_tune_consolidate_table_x_servo_para();
void mtn_wb_tune_consolidate_table_y_servo_para();

int mtn_debug_load_waveform_from_file(char *strFilenameWavefrom, unsigned int *uiTotalLen);
int mtn_debug_load_b1w_data_from_file(char *strFilenameWavefrom, unsigned int *uiTotalLen);
int mtn_tune_upload_latest_wb_waveform(HANDLE hCommunicationHandle);
void mtn_tune_calc_performance_wb_waveform(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc);
CString mtn_tune_output_performance_string(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc);
CString mtn_wb_tune_multi_wire_perform(MULTI_WIRE_WB_PERFORMANCE *stpWbMultiWirePerformStat);
CString mtn_wb_tune_multi_wire_motion_cmd_dist_acc(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc);
CString _mtn_tune_opt_wire_bh_srch_reset(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc);
CString _mtn_tune_opt_wire_bh_looping(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc);
CString _mtn_tune_opt_wire_xy_settling_1st_bond(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc);
CString _mtn_tune_opt_wire_xy_settling_2nd_bond(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc);
CString _mtn_tune_opt_wire_xy_settling(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc);
CString _mtn_tune_opt_wire_max_speed_bnd_z(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc);
CString _mtn_tune_opt_wire_max_speed_xy(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbWaveformPerformanceCalc);



typedef struct
{
int	iBufferDataWbTune_XY_SPEED_PROFILE_MOVE_TO_1ST_B	;
int	iBufferDataWbTune_X_CTRL_MOVE_TO_1ST_B	;
int	iBufferDataWbTune_Y_CTRL_MOVE_TO_1ST_B	;
int	iBufferDataWbTune_X_POSN_1ST_B	;
int	iBufferDataWbTune_Y_POSN_1ST_B	;
int	iBufferDataWbTune_XY_REMAIN_TIME_START_Z_1ST_B	;
int	iBufferDataWbTune_Z_SRCH_HEIGHT_1ST_B	;
int	iBufferDataWbTune_Z_SRCH_VEL_1ST_B	;
int	iBufferDataWbTune_Z_SRCH_THRESHOLD_PE_1ST_B	;
int	iBufferDataWbTune_Z_SRCH_PROT_FD_POSN_1ST_B	;
int	iBufferDataWbTune_Z_SRCH_PRE_CONTACT_FORCE_DCOM_1ST_B	;
int	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_1_TIME_1ST_B	;
int	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_1_DCOM_1ST_B	;
int	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_2_TIME_1ST_B	;
int	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_2_DCOM_1ST_B	;
int	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_3_TIME_1ST_B	;
int	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_3_DCOM_1ST_B	;
int	iBufferDataWbTune_Z_REVERSE_HEIGHT	;
int	iBufferDataWbTune_X_REVERSE_DIST	;
int	iBufferDataWbTune_Y_REVERSE_DIST	;
int	iBufferDataWbTune_Z_REVERSE_DIST	;
int	iBufferDataWbTune_Z_LOOP_TOP_DIST	;
int	iBufferDataWbTune_XYZ_TRAJ_LENGTH	;
int	iBufferDataWbTune_Z_SRCH_VEL_2ND_B	;
int	iBufferDataWbTune_Z_SRCH_DELAY_2ND_B	;
int	iBufferDataWbTune_DRY_RUN_FLAG	;
int	iBufferDataWbTune_Z_SRCH_THRESHOLD_PE_2ND_B	;
int	iBufferDataWbTune_Z_SRCH_PROT_FD_POSN_2ND_B	;
int	iBufferDataWbTune_Z_SRCH_PRE_CONTACT_FORCE_DCOM_2ND_B	;
int	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_1_TIME_2ND_B	;
int	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_1_DCOM_2ND_B	;
int	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_2_TIME_2ND_B	;
int	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_2_DCOM_2ND_B	;
int	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_3_TIME_2ND_B	;
int	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_3_DCOM_2ND_B	;
int	iBufferDataWbTune_Z_TAIL_DIST	;
int	iBufferDataWbTune_Z_RESET_LEVEL	;
int	iBufferDataWbTune_Z_SRCH_DELAY_1ST_B	;
}BUFFER_DATA_WB_TUNE;

typedef struct
{
	double dTuneObj1stBondMoveSrchHeight;
	double dTuneObjReverseHeight;
	double dTuneObjLooping;
	double dTuneObjLoopTop;
	double dTuneObjTraj2ndBondSrchHeight;
	double dTuneObjTail;
	double dTuneObjResetLevel;
}B1W_BH_TUNE_OBJ;

//// One entry for initialization
void mtn_dll_wb_tune_initialization();

//// Init configuration from file
int mtnapi_init_tune_para_bound();

//// Tuning Rountines, MtnWbTune.cpp
void mtn_wb_tune_update_tuning_condition(MTN_TUNE_CONDITION *stpTuningCondition, MTN_TUNE_ENC_TIME_CFG *stpTuningEncTimeConfig, int iWbAxisOpt, unsigned int uiCurrWireNo, unsigned int uiBlkMovePosnSet);
void mtn_wb_tune_init_def_servo_para_tune_input(MTN_TUNE_GENETIC_INPUT *stpMtnServoParaTuneInput, int iWbAxisOpt, 
												unsigned int uiCurrWireNo, unsigned int uiBlkMovePosnSet);

void mtn_wb_tune_load_servo_para_to_tune_output(MTN_TUNE_GENETIC_OUTPUT *stpMtnServoParaTuneOutput, int iWbAxisOpt, unsigned int uiCurrTableSector, unsigned int uiBlkMovePosnSet);
void mtn_wb_tune_load_servo_para_to_tune_ini(MTN_TUNE_GENETIC_INPUT *stpMtnServoParaTuneInput, int iWbAxisOpt, unsigned int uiCurrTableSector, unsigned int uiBlkMovePosnSet);


void mtn_tune_init_servo_para_tuning_config();
void mtn_tune_set_initial_tuning_para();
void mtn_wb_tune_init_def_servo_para_tune_input_no_condi(MTN_TUNE_GENETIC_INPUT *stpMtnServoParaTuneInput, int iWbAxisOpt);

unsigned int mtn_wb_tune_get_curr_wire_number();
void mtn_wb_tune_set_curr_wire_number(unsigned int uiWireNo);
unsigned int mtn_wb_tune_get_blk_move_posn_set();
void mtn_wb_tune_set_blk_move_posn_set(unsigned int uiBlkPosnSet);
int mtn_wb_tune_get_curr_wb_axis_servo_para_tuning();
void mtn_wb_tune_set_curr_wb_axis_servo_para_tuning(int iWbAxis);
unsigned int mtn_wb_tune_get_curr_sector_table();
void mtn_wb_tune_set_curr_sector_table(unsigned int uiCurrSector);

typedef struct
{
	double dPosition1;
	double dPosition2;
}MTN_TUNE_2POINTS_POSNS;

void mtn_tune_get_wb_bondhead_tuning_position_set(int idxWireNo, POSNS_OF_BONDHEAD_Z *stpPosnsOfBondHeadZ);
void mtn_tune_init_wb_bondhead_tuning_position_set();
void mtn_tune_get_bond_head_tuning_position(int idxWireNo, unsigned int uiBlk,  double *dPosn1, double *dPosn2);
void mtn_tune_get_table_x_tuning_position(int idxWireNo, unsigned int uiBlk,  double *dPosn1, double *dPosn2);
void mtn_tune_get_table_y_tuning_position(int idxWireNo, unsigned int uiBlk,  double *dPosn1, double *dPosn2);
void mtn_tune_get_wb_table_tuning_position_set(int idxWireNo, POSNS_OF_TABLE *stpPosnsOfTable);
void mtn_tune_init_wb_table_x_tuning_position_set();
void mtn_tune_init_wb_table_y_tuning_vled_position_set();
void mtn_tune_init_wb_table_y_tuning_hori_led_position_set();

unsigned int mtn_wb_tune_get_max_sector_table_x();
unsigned int mtn_wb_tune_get_max_sector_table_y();
unsigned int mtn_wb_tune_get_curr_tune_sector();
void mtn_wb_tune_set_max_sector_table_x(unsigned int uiMaxSector);

int AutoTuneBondHeadBlk();
int AutoTuneTableX_CurrentSectorBlk();
int AutoTuneTableY_CurrentSectorBlk();
int AutoTuneTableX_AllSectorBlk();
int AutoTuneTableY_AllSectorBlk();
CString AutoTuneBondHeadB1W();
CString AutoTuneBondHeadB1W_StopSrch();
// Flags
void mtn_wb_tune_b1w_set_flag_contact(int iFlag);
int mtn_wb_tune_b1w_get_flag_contact();
void mtn_wb_tune_b1w_set_flag_motion(int iFlag);
int mtn_wb_tune_b1w_get_flag_motion();
void mtn_wb_tune_b1w_set_enable_all_section();
void mtn_wb_tune_b1w_set_clear_all_section();

unsigned int mtn_wb_tune_get_num_total_wire();
void mtn_wb_tune_set_num_total_wire(unsigned int nNumWire);

#define  NUM_POINTS_ONE_DIM_MESH	9
#define  RESCALE_FACTOR_PREV_STEP_SIZE   2

double mtn_wb_checksum_para_b1w_out(MTN_TUNE_PARAMETER_SET *stpParsSetTuneOut);
int mtn_wb_verify_tune_out_b1w();
void mtn_wb_bond_head_z_update_acs_parameter_servo_struct_multi_blk_w_chksum(int nTotalBlkInAcs);

void mtn_tune_save_tuning_history(MTN_TUNE_GENETIC_INPUT *stpMtnTuneInput, MTN_TUNE_GENETIC_OUTPUT *stpMtnTuneOutput, int nTotalCases);
int mtn_wb_tune_bond_1_wire_bh(MTN_TUNE_GENETIC_INPUT *stpMtnTuneInput, MTN_TUNE_GENETIC_OUTPUT *stpMtnTuneOutput, 
								int iObjSectionFlagB1W);
int mtn_wb_tune_bh_b1w_stop_srch(MTN_TUNE_GENETIC_INPUT *stpMtnTuneInput, MTN_TUNE_GENETIC_OUTPUT *stpMtnTuneOutput, 
								int iObjSectionFlagB1W);
void mtn_wb_tune_b1w_stop();

int mtn_wb_tune_b1w_stop_srch_download_cfg();
int mtn_wb_tune_b1w_stop_srch_init_cfg(HANDLE m_hHandle);
int mtn_wb_tune_b1w_check_valid_search_height();
int mtn_wb_tune_b1w_update_search_height_posn_by_low_limit(HANDLE m_hHandle, int iLowLimit);

int mtn_wb_tune_b1w_update_search_height(HANDLE hAcsHandle);
void mtn_wb_tune_b1w_init_search_cfg();

int mtn_wb_tune_b1w_stop_srch_calc_bh_obj( MTN_SCOPE *stpScopeB1W, double adScopeDataB1W[],
										  WB_TUNE_B1W_BH_OBJ *stpWbTuneB1wObj);
int mtn_wb_tune_b1w_stop_srch_trig_once(HANDLE hAcsHandle);

	// 0: no tuning;
#define  SERVO_PARA_TUNING_ROUTINE_ONE_CASE		1
#define  SERVO_PARA_TUNING_ROUTINE_AUTO_FULL    2
#define  SERVO_PARA_TUNING_ROUTINE_AUTO_B1W     3

class CMtnTune :  public CObject
{
	DECLARE_DYNAMIC(CMtnTune)

protected:
	char cFlagDoingTuningThreadOneGroup;
	CWinThread* pMtnTuneWinThread;
	short mtn_save_tune_debug_data_to_file();
	FILE *fpDataTuneDebug;

	// SLAFF
	char cFlagSearchAFF_DoingGroupThread;
	CWinThread* pMtnTuneAFF_GroupWinThread; // , THREAD_PRIORITY_TIME_CRITICAL);
	char cFlagTuningStageSLAFF;  // Tuning Stage

	// Tuning by GA
	CWinThread* pMtnTuneGA_GroupWinThread;
	char cFlagTuningGA_DoingGroupThread;
	FILE * fpDataTuneGA;
	TUNE_ALGO_SETTING stTuneAlgoSetting;
	MTN_TUNE_THEME			stTuningTheme;
	MTN_TUNE_CONDITION		stTuningCondition;
	MTN_TUNE_ENC_TIME_CFG	stTuningEncTimeConfig;
	MTN_TUNE_SPECIFICATION  stTuningPassSpecfication;

	GA_CONFIG stGA_Config;
	GA_INPUT_BOUND_DBL stGA_Input;
#ifdef __GA_FLOATING_POINT
	GA_FloatingPoint mTuningGA_Setting;
#endif  // __GA_FLOATING_POINT

	HANDLE Handle;				// communication handle
	int iAxisTuning;

	int iMovePoint1;
	int iMovePoint2;
	int iDelayInterMove_ms;  // iIterMotionDelay_ms
	double fThresholdSettleTime;
	int iFlagEnableDebug;

//	double fWeightPositiveDPE;
//	double fWeightNegativeDPE;
//	double fWeightDiffDPE;

	void mtn_tune_start_2_points_move_with_data_collect();
	short OneAxis3MovePoints();

public:

	// Init
	void mtn_tune_initialize_class_var(double fThresholdSettleTime, HANDLE stAcsHandle, unsigned int uiCurrMoveAxis_ACS, int iInterMoveDelay);
	// Basic Moving function
	short OneAxis2PointsMoveOneCycle();

	// Tuning SLAFF
	double adGroupPointSLAFF[__MTN_TUNE_NUM_GROUP_POINTS__];
	double adObjGroupTuningSLAFF[__MTN_TUNE_NUM_GROUP_POINTS__];
	double adSortedObjGroupTuningSLAFF[__MTN_TUNE_NUM_GROUP_POINTS__];
	double adPointSLAFF_SortByObj[__MTN_TUNE_NUM_GROUP_POINTS__];

	// set private variables
	void mtn_tune_set_comm_handle(HANDLE stCommHandle); 
	void mtn_tune_set_tune_axis(int iTuneAxis);

	// 
	void mtn_tune_search_aff(MTN_TUNE_SEARCH_AFF_INPUT *stpTuneAffInput, MTN_TUNE_SEARCH_AFF_OUTPUT *stpTuneAffOutput);

	UINT mtn_tune_aff_groups_thread();
	void mtn_tune_one_group_by_thread();
	void mtn_tune_aff_start_groups_thread();
	void mtn_tune_aff_stop_groups_thread();
	char mtn_tune_aff_get_flag_doing_group_thread();

	// Tuning By GA
	void mtn_tune_ga_start_groups_thread();
	void mtn_tune_ga_stop_groups_thread();
	UINT mtn_tune_ga_groups_thread();
	char mtn_tune_ga_get_flag_doing_group_thread();
	void mtn_tune_ga_one_group();
	void mtn_tune_ga_init(MTN_TUNE_GENETIC_INPUT *stpMtnTuneGeneticInput);

	// Genetic thread of tuing one group of data, one population in genetic
	void mtn_tune_start_tuning_thread_one_group();
	void mtn_tune_stop_tuning_thread_one_group();
	UINT mtn_tuning_thread_one_group();

	// Tuning Status, loop counter
	int mtn_tune_get_group_loop_iter();
	unsigned int mtn_tune_get_total_num_iter_for_one_group();
	unsigned int mtn_tune_get_curr_iter_within_one_group();
	char mtn_tune_get_option();
	char mtn_tune_get_stage_group_thread();

	// 
	int mtn_tune_get_move_point_1(); //	int iMovePoint1;
	int mtn_tune_get_move_point_2(); //	int iMovePoint2;
	int mtn_tune_get_tune_axis(); // 

	// Obj Calc
	double CalculateObjByThemeIndex(MTN_TUNE_THEME *stpTuningTheme, MTN_SERVO_TUNE_INDEX  *stpMtnServoTuneIndex);
	double CMtnTune::CalculateObjByThemeIndex_DRA(MTN_TUNE_THEME *stpTuningTheme, MTN_SERVO_TUNE_INDEX  *stpMtnServoTuneIndex);

	// Complete mesh
	short mtn_tune_axis_complete_mesh(MTN_TUNE_GENETIC_INPUT *stpMtnTuneInput, MTN_TUNE_GENETIC_OUTPUT *stpMtnTuneOutput);
	char cFlagStopTuneCompleteMesh;
	void StopTuneCompleteMesh();
	void mtn_tune_get_status_axis_complete(unsigned int *idxCurrTune, unsigned int *nTotal, double *dCurrBest);

	// Tuning one axis after one
	void mtn_tune_get_status_axis_1by1(unsigned int *idxCurrTune, unsigned int *nTotal, double *dCurrBest);
	int CMtnTune::mtn_tune_get_best_idx_tune_1by1();

	short CMtnTune::mtn_tune_axis_one_by_one_para(MTN_TUNE_GENETIC_INPUT *stpMtnTuneInput, MTN_TUNE_GENETIC_OUTPUT *stpMtnTuneOutput);
	short CMtnTune::mtn_wb_tune_axis_sldra_jerk_ffc_para(MTN_TUNE_GENETIC_INPUT *stpMtnTuneInput, MTN_TUNE_GENETIC_OUTPUT *stpMtnTuneOutput);

	// Save Tuning history 
//	void mtn_tune_save_tuning_history(MTN_TUNE_GENETIC_INPUT *stpMtnTuneInput, MTN_TUNE_GENETIC_OUTPUT *stpMtnTuneOutput, int nTotalCases);
	// Thread status
	char mtn_tuning_get_flag_doing_thread_one_group();

private:
	char cFlagTuningOption;
	unsigned int uiTotalNumIterTunigThread;
	unsigned int uiCurrIterWithinTuningThread;
	int iTuningLoopIter;
	CTRL_PARA_ACS stAxisServoCtrlParaAtTuningACS;  // 20090930
	FILE *fpDataTuneAccFF;

	short mtn_tune_load_input_file(char *strFilename, HANDLE stCommHandle, int iAxisTuning, MTN_TUNE_GENETIC_INPUT *stpMtnTuneInput);


};

extern CMtnTune mMotionTuning;
#endif // __MTN_TUNE__