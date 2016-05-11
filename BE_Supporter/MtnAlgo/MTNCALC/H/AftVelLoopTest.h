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


#ifndef __AFT_VEL_LOOP_TEST__
#define __AFT_VEL_LOOP_TEST__

#define   LEN_ANALYZ_DRVCMD_FDACC    10

typedef struct
{
	int iAxisCtrlCard;
	double fMaxAbsVel;
	double fMaxAbsDAC;
	int iPositionUpper;
	int iPositionLower;
//	unsigned int uiDwell_ms;
	unsigned int uiLoopNum;
	double fDriveCmdRatioRMS;
	double fEncResolutionCntPerMM;
	double fPercentErrorThresholdSettleTime;
	char *strServoAxisNameWb;

	int iFlagSaveFile;
}AFT_VEL_LOOP_TEST_INPUT;

typedef struct
{
	double fMaxAcc;
	double fMaxDriveCmd;
	double fMinAcc;
	double fMinDriveCmd;

	double fAveMaxFeedBackAcc_mpss;
	double fMaxAccEstimateFullDAC_mpss;
	double fMinAccEstimateFullDAC_mpss;
	double fAveMaxAccEstimateFullDAC_mpss;
	double fExpectedAccAtRMS_mpss;

	/// velocity step-response performance, OS, ST, RT
	double fPositiveStepOverShoot;
	double fPositiveStepPercentOS;
	double fPositiveStepSettleTime;
	double fPositiveStepRiseTime;
	// 20120803
	double fSamplePositivePeakDrvCmd;
	double fSampleNegativePeakDrvCmd;

	double fNegativeStepOverShoot;
	double fNegativeStepPercentOS;
	double fNegativeStepSettleTime;
	double fNegativeStepRiseTime;

	double fPositiveReturnTime;
	double fNegativeReturnTime;
	double fPositiveReturnBottom;
	double fNegativeReturnPeak;

	//// Dynamic friction in constant velocity motion
	double fMeanDrvCmd_PositiveConstVel;
	double fStdDrvCmd_PositiveConstVel;
	double fMeanDrvCmd_NegativeConstVel;
	double fStdDrvCmd_NegativeConstVel;

	//// debug intermediate variables
	unsigned int idxMaxDrvCmd;
	unsigned int idxMaxFdAcc;
	unsigned int idxMinDrvCmd;
	unsigned int idxMinFdAcc;

	unsigned int idxPositiveStepCmdStart;
	unsigned int idxNegativeStepCmdStart;
	double fPositiveLevel;
	double fNegativeLevel;
	int idxOnePeriod;
	int nLengthPositiveCmd; 
	int nLengthNegativeCmd;
	unsigned int idxNegativePeakTime;
	unsigned int idxPositivePeakTime;

	//// Statistic method
double afMaxDriveCmd[LEN_ANALYZ_DRVCMD_FDACC]; // = afDac[0];
double afMinDriveCmd[LEN_ANALYZ_DRVCMD_FDACC]; // = afDac[0];
unsigned int aidxMaxDriveCmd[LEN_ANALYZ_DRVCMD_FDACC]; // = 0;
unsigned int aidxMinDriveCmd[LEN_ANALYZ_DRVCMD_FDACC]; // = 0;
double afMaxFdAcc[LEN_ANALYZ_DRVCMD_FDACC]; // = afFeedAcc[idxMaxDriveCmd];
int aidxMaxFdAcc[LEN_ANALYZ_DRVCMD_FDACC]; // = idxMaxDriveCmd;
double afMinFdAcc[LEN_ANALYZ_DRVCMD_FDACC]; // = afFeedAcc[idxMinDriveCmd];
int aidxMinFdAcc[LEN_ANALYZ_DRVCMD_FDACC]; // = idxMinDriveCmd;
double afMaxFdAccAtFullDriveCmd[LEN_ANALYZ_DRVCMD_FDACC];
double afMinFdAccAtFullDriveCmd[LEN_ANALYZ_DRVCMD_FDACC];
	unsigned int nTotalMaxPeak;
	unsigned int nTotalMinPeak;
char  strOutputFilename[256];
}AFT_VEL_LOOP_TEST_OUTPUT;

#define FILENAME_TRAJ_TEMP_DATA  "VLoopTest.m"

int mtn_aft_vel_loop_test(HANDLE hHandle, AFT_VEL_LOOP_TEST_INPUT *stpVelLoopTestInput, AFT_VEL_LOOP_TEST_OUTPUT *stpVelLoopTestOutput);
void mtn_aft_vel_loop_get_status(unsigned int *uiCurrLoopCount, int *iFlagServoNormal);
void mtn_aft_vel_loop_stop();
void aft_veloop_step_test_make_new_folder();

#define   MAX_NUM_CASE_VEL_STEP_GROUP_TEST		3000

#define MAX_NUM_AXIS_GROUP_VEL_STEP_TEST   8
typedef struct
{
	unsigned int usActualNumAxis;  // us - ui, 20100729
	unsigned int uiAxis_CtrlCardId[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
	int iAxisPosn[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
	double fMaxVelocityPercent;
	AFT_VEL_LOOP_TEST_INPUT stVelStepTestOnceInput;
}AFT_GROUP_VEL_STEP_TEST_INPUT;

typedef struct
{
	unsigned short usActualNumAxis;
	unsigned int uiAxis_CtrlCardId[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
	double dLowerLimitPosition_mm[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
	double dUpperLimitPosition_mm[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
	unsigned int uiNumPoints[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
	unsigned char ucFlagIsExciteAxis[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
	double fVelStepTestDistance_mm[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
	double fMaxVelocityPercent[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
	unsigned int uiLoopNum[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
	double fDriveCmdRatioRMS[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
	double fEncResolutionCntPerMM[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
	double fPercentErrorThresholdSettleTime[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
	char *strServoAxisNameWb[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
	double afPeakDriverCurrent[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
}AFT_GROUP_VEL_STEP_TEST_CONFIG;
void aft_vel_loop_test_init_default_cfg(AFT_GROUP_VEL_STEP_TEST_CONFIG *stpVelStepGroupTestConfig);
void aft_vel_loop_test_hori_bonder_init_default_cfg(AFT_GROUP_VEL_STEP_TEST_CONFIG *stpVelStepGroupTestConfig);
void aft_vel_loop_test_13v_init_default_cfg(AFT_GROUP_VEL_STEP_TEST_CONFIG *stpVelStepGroupTestConfig);
void aft_vel_loop_test_wb_18V_init_default_cfg(AFT_GROUP_VEL_STEP_TEST_CONFIG *stpVelStepGroupTestConfig);  // 

void aft_convert_group_vel_step_cfg_to_test_input(AFT_GROUP_VEL_STEP_TEST_CONFIG *stVelStepGroupTestConfig, 
											  AFT_GROUP_VEL_STEP_TEST_INPUT* stpVelStepGroupTestCase,
											  unsigned int *uiTotalTestCases);
void aft_vel_loop_set_bond_head_relax_position(int iPosn);
void aft_vel_loop_test_station_xy_init_VLED_cfg(AFT_GROUP_VEL_STEP_TEST_CONFIG *stpVelStepGroupTestConfig);
void aft_vel_loop_test_station_bh_init_default_cfg(AFT_GROUP_VEL_STEP_TEST_CONFIG *stpVelStepGroupTestConfig);

void aft_vel_loop_test_station_xy_init_HoriLED_cfg(AFT_GROUP_VEL_STEP_TEST_CONFIG *stpVelStepGroupTestConfig);

#endif // __AFT_VEL_LOOP_TEST__