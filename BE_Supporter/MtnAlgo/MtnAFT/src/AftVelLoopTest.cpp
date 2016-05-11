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

// History
// 20100331   Comments
//			acsc_SetVelocity(hHandle, iAxisCtrlCard, fabs(dJogVelocity), NULL); // setup the maximum velocity for profile gen
//			acsc_SetVelocityImm(hHandle, iAxisCtrlCard, dJogVelocity, NULL); // immediate, let the controller do the smoothing

#include <stdafx.h>
#include <math.h>
#include "MtnInitAcs.h"
#include "MtnTune.h"
#include "AftVelLoopTest.h"
#include "MtnWbDef.h"

extern void aft_vel_loop_test_save_data_to_file(AFT_VEL_LOOP_TEST_INPUT *stpVelLoopTestInput, AFT_VEL_LOOP_TEST_OUTPUT *stpVelLoopTestOutput);
extern int aft_vel_loop_test_download_condition(HANDLE stCommHandleACS, AFT_VEL_LOOP_TEST_INPUT *stpVelLoopTestInput);
extern int aft_vel_loop_test_get_current_loop(HANDLE stCommHandleACS, unsigned int *uiCurrIter);

static unsigned int uiCurrLoop = 0;
static int iFlagServoCtrlNoErr = 1;
static int iFlagStopVelLoopTest = FALSE;

extern	double *afRefPosn;
extern	double *afFeedVel;
extern	double *afRefVel;
extern	double *afFeedAcc;
extern	double *afDac;
//static	double *afPosnErr;

// #include "XLEzAutomation.h"
// extern CXLEzAutomation * m_pXLS_ServoTuningDlg;
extern void aft_vel_test_port_data_to_extern_xls_file();
#include <direct.h>

#include "MotAlgo_DLL.h"
// 20110013
extern char *astrMachineTypeNameLabel_en[];

#include "MtnSearchHome.h"
short aft_teach_upper_limit(HANDLE hHandle, int iAxisOnACS, int iDebug,double *dRetUppLimitPosn)
{
	short sRet = MTN_API_OK_ZERO;
	MTN_SEARCH_LIMIT_INPUT stSearchLimitInput;
	MTN_SEARCH_LIMIT_OUTPUT stSearchLimitOutput;

	stSearchLimitInput.iAxisOnACS = iAxisOnACS;
	if(iAxisOnACS == 0 || iAxisOnACS == 1)
	{
		stSearchLimitInput.dVelJoggingLimit = fabs(20000.0);
		stSearchLimitInput.dDriveCmdThresholdProtection = 0.33; // 20120927
	}
	else
	{
		stSearchLimitInput.dVelJoggingLimit = fabs(5000.0);
		stSearchLimitInput.dDriveCmdThresholdProtection = 0.25; // 20120927
	}
	stSearchLimitInput.dMaxDistanceRangeProtection = 1E10;;
	stSearchLimitInput.dMoveDistanceAfterSearchLimit = -1000.0; // Before search index, is usually search limit
	stSearchLimitInput.dMoveDistanceBeforeSearchLimit = 0;
	stSearchLimitInput.dPositionErrorThresholdSearchLimit = 30.0;
	stSearchLimitInput.dPosnErrThresHoldSettling = 20.0;
	stSearchLimitInput.iDebug = iDebug;
	stSearchLimitInput.stSpeedProfileMove.dMaxAcceleration = 1E6;
	stSearchLimitInput.stSpeedProfileMove.dMaxDeceleration = 1E6;
	stSearchLimitInput.stSpeedProfileMove.dMaxJerk = 1E8;
	stSearchLimitInput.stSpeedProfileMove.dMaxKillDeceleration = 1E6;
	stSearchLimitInput.stSpeedProfileMove.dMaxVelocity = 1E6;
	stSearchLimitInput.stSpeedProfileMove.uiProfileType = 0;
	stSearchLimitInput.uiFreqFactor_10KHz_Detecting = 1;

	sRet = mtn_search_limit_acs(hHandle, &stSearchLimitInput, &stSearchLimitOutput);

	*dRetUppLimitPosn = stSearchLimitOutput.dLimitPosition;

	return sRet;
}

short aft_teach_lower_limit(HANDLE hHandle, int iAxisOnACS, int iDebug, double *dRetLowLimitPosn)
{
	short sRet = MTN_API_OK_ZERO;
	MTN_SEARCH_LIMIT_INPUT stSearchLimitInput;
	MTN_SEARCH_LIMIT_OUTPUT stSearchLimitOutput;

	stSearchLimitInput.iAxisOnACS = iAxisOnACS;
	if(iAxisOnACS == 0 || iAxisOnACS == 1)
	{
		stSearchLimitInput.dVelJoggingLimit = -fabs(20000.0);
		stSearchLimitInput.dMoveDistanceBeforeSearchLimit = -1000.0;
		stSearchLimitInput.dDriveCmdThresholdProtection = 0.33; // 20120927
	}
	else
	{
		stSearchLimitInput.dVelJoggingLimit = -fabs(5000.0);
		stSearchLimitInput.dMoveDistanceBeforeSearchLimit = -1000.0;
		stSearchLimitInput.dDriveCmdThresholdProtection = 0.25; // 20120927
	}
	
	stSearchLimitInput.dMaxDistanceRangeProtection = 1E10;;
	stSearchLimitInput.dMoveDistanceAfterSearchLimit = 1000.0; // Before search index, is usually search limit
	stSearchLimitInput.dPositionErrorThresholdSearchLimit = 50.0;	// 20121220
	stSearchLimitInput.dPosnErrThresHoldSettling = 50.0;			// 20121220
	stSearchLimitInput.iDebug = iDebug;
	stSearchLimitInput.stSpeedProfileMove.dMaxAcceleration = 1E6;
	stSearchLimitInput.stSpeedProfileMove.dMaxDeceleration = 1E6;
	stSearchLimitInput.stSpeedProfileMove.dMaxJerk = 1E7;			// 20121220
	stSearchLimitInput.stSpeedProfileMove.dMaxKillDeceleration = 1E6;
	stSearchLimitInput.stSpeedProfileMove.dMaxVelocity = 1E6;
	stSearchLimitInput.stSpeedProfileMove.uiProfileType = 0;
	stSearchLimitInput.uiFreqFactor_10KHz_Detecting = 1;

	sRet = mtn_search_limit_acs(hHandle, &stSearchLimitInput, &stSearchLimitOutput);

	*dRetLowLimitPosn = stSearchLimitOutput.dLimitPosition;

	return sRet;
}

char strVeLoopTestNewFolderName[128];
void aft_veloop_step_test_make_new_folder()
{
struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;
	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);
int iTempMachineType = get_sys_machine_type_flag();

	sprintf_s(strVeLoopTestNewFolderName, 128, "D:\\MT\\%s-%d_VeloopStepTest_%d.%d.%d.H%dm%d",  // 20111013, 20111201, Add MT
				astrMachineTypeNameLabel_en[iTempMachineType],
				get_sys_machine_serial_num(),
				stpTime->tm_year +1900, stpTime->tm_mon +1, stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min);

	_mkdir(strVeLoopTestNewFolderName);
}

short aft_vel_loop_tune_init_array_ptr()
{
	short sRet = MTN_API_OK_ZERO;
	int nDataLen = gstSystemScope.uiDataLen;

	int ii = 0;
	afRefPosn = &gdScopeCollectData[0];
	ii += nDataLen;
	afFeedVel = &gdScopeCollectData[ii];
	ii += nDataLen;
	afRefVel = &gdScopeCollectData[ii];
	ii += nDataLen;
	afFeedAcc = &gdScopeCollectData[ii];
	ii += nDataLen;
	afDac = &gdScopeCollectData[ii];

	return sRet;
}

// MFLAGS, bit 13, driver polarity
void mtn_aft_vel_loop_stop()
{
	iFlagStopVelLoopTest = TRUE;
}

void mtn_aft_vel_loop_get_status(unsigned int *uiCurrLoopCount, int *iFlagServoNormal)
{
	*uiCurrLoopCount = uiCurrLoop;
	*iFlagServoNormal = iFlagServoCtrlNoErr;
}

double mtn_aft_find_1st_max_peak(double dInput[], unsigned int nLen, unsigned int *idxPeak)
{
	double dRet = 0;

	for(unsigned int ii=1; ii<nLen - 1; ii++)
	{
		if((dInput[ii] > dInput[ii - 1]) &&(dInput[ii] >= dInput[ii + 1]))
		{
			dRet = dInput[ii];
			*idxPeak = ii;
		}
	}
	return dRet;
}

double mtn_aft_find_1st_min_peak(double dInput[], unsigned int nLen, unsigned int *idxPeak)
{
	double dRet = 0;

	for(unsigned int ii=1; ii<nLen - 1; ii++)
	{
		if((dInput[ii] < dInput[ii - 1]) &&(dInput[ii] <= dInput[ii + 1]))
		{
			dRet = dInput[ii];
			*idxPeak = ii;
		}
	}
	return dRet;
}
extern double f_get_mean(double *afIn, int nLen);
extern double f_get_std(double *afIn, double fMean, int nLen);
extern double f_get_abs_max(double *afIn, int nLen);
extern double f_get_min(double *afIn, int nLen);
extern double f_get_rms_max_err_w_offset(double *afIn, int nLen, double dOffset);

static double dSampleTime_ms;
	// 20120710
static double dMaxVelACS;

#define VEL_STEP_TEST_SKIP_SAMPLE       10
#define MAX_DRV_COMMAND_16_BIT_DAC		32767
int mtn_aft_vel_loop_calc_output(AFT_VEL_LOOP_TEST_INPUT *stpVelLoopTestInput, AFT_VEL_LOOP_TEST_OUTPUT *stpVelLoopTestOutput)
{
	int iRet = MTN_API_OK_ZERO;
	static unsigned int ii;

	//// Calculate velocity rise-time (RT), overshoot (OS), settle time(ST)
// nTimeLen = length(afFeedVel); 
unsigned int idxPositiveStepCmdStart = 0;
unsigned int idxNegativeStepCmdStart = 0;
double fPositiveLevel = 0;
double fNegativeLevel = 0;
int idxOnePeriod = 0;
int nLengthPositiveCmd = 0, nLengthNegativeCmd=0;
	for( ii = VEL_STEP_TEST_SKIP_SAMPLE; ii < gstSystemScope.uiDataLen - 1; ii++) // 1:nLenTime_ms
	{
		if( (idxNegativeStepCmdStart != 0)
			&& (idxPositiveStepCmdStart != 0)
			&& ((afRefVel[ii] * afRefVel[ii+1]) <0))
		{
			idxOnePeriod = ii;
			break;
		}
		if(afRefVel[ii] < 0 && afRefVel[ii+1] >0) //// && (idxPositiveStepCmdStart == 0)
		{
			idxPositiveStepCmdStart = ii;
			fPositiveLevel = afRefVel[ii+2];
		}
		if(afRefVel[ii] > 0 && afRefVel[ii+1] <0) //// && (idxNegativeStepCmdStart == 0)
		{
			idxNegativeStepCmdStart = ii;
			fNegativeLevel = afRefVel[ii+2];
		}
	}

	if(idxNegativeStepCmdStart == 0 || idxPositiveStepCmdStart == 0)
	{
		iRet = MTN_CALC_ERROR;
		goto label_return_mtn_aft_vel_loop_calc_output;
	}
	// nHalfPeriodStepResp = abs(idxNegativeStepCmdStart - idxPositiveStepCmdStart); // 20120803
	if (idxNegativeStepCmdStart > idxPositiveStepCmdStart)
	{
		nLengthPositiveCmd = idxNegativeStepCmdStart - idxPositiveStepCmdStart - 10;
		nLengthNegativeCmd = idxOnePeriod - idxNegativeStepCmdStart - 10;
	}
	else
	{
		nLengthNegativeCmd = idxPositiveStepCmdStart - idxNegativeStepCmdStart - 10;
		nLengthPositiveCmd = idxOnePeriod - idxPositiveStepCmdStart - 10;
	}

	// 20120803
	double fErrorThresholdSettleTime = stpVelLoopTestInput->fPercentErrorThresholdSettleTime * stpVelLoopTestInput->fMaxAbsVel;

	//////// Positive OS, ST, RT
double fPositiveStepOverShoot = 0;
double fPositiveStepPercentOS = 0;
double fPositiveStepSettleTime = 0;
double fPositiveStepRiseTime = 0;
unsigned int idxPositivePeakTime = 0;
	for( ii = (idxPositiveStepCmdStart+1); ii < (unsigned int)(nLengthPositiveCmd + idxPositiveStepCmdStart); ii++)
	{
		if( ((afFeedVel[ii+1] - afFeedVel[ii]) * (afFeedVel[ii] - afFeedVel[ii-1])) <0 
			&& afFeedVel[ii] > 0) // 20110727
		{
			idxPositivePeakTime = ii;
			fPositiveStepOverShoot = afFeedVel[ii];
		}
		if(idxPositivePeakTime != 0)
		{
			break;
		}
	}
	fPositiveStepPercentOS = (fPositiveStepOverShoot - fPositiveLevel) / (fabs(fPositiveLevel - fNegativeLevel));
	fPositiveStepRiseTime = idxPositivePeakTime - idxPositiveStepCmdStart;
	for(ii = (nLengthPositiveCmd + idxPositiveStepCmdStart - 1); ii>(unsigned int)(idxPositiveStepCmdStart + fPositiveStepRiseTime); ii--)
	{
		if (fabs(afFeedVel[ii+1] - afRefVel[ii+1]) > fErrorThresholdSettleTime)
		{
			fPositiveStepSettleTime = ii - idxPositiveStepCmdStart - fPositiveStepRiseTime;
			break;
		}
	}
	// 20120803
double fPositiveReturnBottom = 0;
double fNegativeReturnPeak = 0;
unsigned int idxPositiveReturnBottom = 0, idxNegativeReturnPeak = 0;
	idxPositiveReturnBottom = nLengthPositiveCmd + idxPositiveStepCmdStart;
	for( ii = (idxPositivePeakTime+1); ii < (unsigned int)(nLengthPositiveCmd + idxPositiveStepCmdStart); ii++)
	{
		if( afFeedVel[ii] < afRefVel[ii])
		{
			fPositiveReturnBottom = afFeedVel[ii];
			idxPositiveReturnBottom = ii;
			break;
		}
	}

	////////// Negative  OS, ST, RT
double fNegativeStepOverShoot = 0;
double fNegativeStepPercentOS = 0;
double fNegativeStepSettleTime = 0;
double fNegativeStepRiseTime = 0;
unsigned int idxNegativePeakTime = 0;
	for( ii = (idxNegativeStepCmdStart+1); ii< (unsigned int)(nLengthNegativeCmd + idxNegativeStepCmdStart); ii++)
	{
		if( ((afFeedVel[ii+1] - afFeedVel[ii]) * (afFeedVel[ii] - afFeedVel[ii-1])) <0 
			&& afFeedVel[ii] < 0)  // 20110727
		{
			idxNegativePeakTime = ii;
			fNegativeStepOverShoot = afFeedVel[ii];
		}
		if( idxNegativePeakTime != 0)
		{
			break;
		}
	}
	fNegativeStepPercentOS = (fNegativeStepOverShoot - fNegativeLevel) / (fabs(fPositiveLevel - fNegativeLevel));
	fNegativeStepRiseTime = idxNegativePeakTime - idxNegativeStepCmdStart;
	for( ii = (unsigned int)(idxNegativeStepCmdStart + fNegativeStepRiseTime); ii< (unsigned int)(nLengthNegativeCmd + idxNegativeStepCmdStart - 1) ; ii++)
	{
		if( (fabs(afFeedVel[ii] - afRefVel[ii]) > fErrorThresholdSettleTime) 
			&& (fabs(afFeedVel[ii+1] - afRefVel[ii+1]) <= fErrorThresholdSettleTime)
			)
		{
			fNegativeStepSettleTime = ii - idxNegativeStepCmdStart - fNegativeStepRiseTime;
		}
	}

	// 20120803
	idxNegativeReturnPeak = nLengthNegativeCmd + idxNegativeStepCmdStart;
	for( ii = (idxNegativePeakTime+1); ii < (unsigned int)(nLengthNegativeCmd + idxNegativeStepCmdStart); ii++)
	{
		if( afFeedVel[ii] > afRefVel[ii])
		{
			fNegativeReturnPeak = afFeedVel[ii+1];
			idxNegativeReturnPeak = ii;
			break;
		}
	}

//////// Maximum Minimum DrvCmd and FdAcc
	double fMaxDriveCmd = fabs(afDac[idxPositiveStepCmdStart]); // 20120509
	double fMinDriveCmd = -fabs(afDac[idxNegativeStepCmdStart]);// 20120509
	unsigned int idxMaxDriveCmd = idxPositiveStepCmdStart;      // 20120509
	unsigned int idxMinDriveCmd = idxNegativeStepCmdStart;      // 20120509
	double fMaxFdAcc = fabs(afFeedAcc[idxMaxDriveCmd]); // 20120509
	int idxMaxFdAcc = idxMaxDriveCmd;
	double fMinFdAcc = -fabs(afFeedAcc[idxMinDriveCmd]);   // 20120509
	int idxMinFdAcc = idxMinDriveCmd;

	for( ii = (idxPositiveStepCmdStart+1); ii < (unsigned int)(nLengthPositiveCmd + idxPositiveStepCmdStart); ii++) // 1:nLenTime_ms
	{
		if( fabs(afDac[ii]) > fMaxDriveCmd 
			|| ( fabs(afDac[ii]) >= 32766 && (fabs(afDac[ii - 1]) >= 32766 || fabs(afDac[ii - 2]) >= 32766) ) // 20110916
			)
		{
			fMaxDriveCmd = fabs(afDac[ii]);
			idxMaxDriveCmd = ii;
		}
	}

	for( ii = (idxNegativeStepCmdStart+1); ii< (unsigned int)(nLengthNegativeCmd + idxNegativeStepCmdStart); ii++)
	{
		if( -fabs(afDac[ii]) < fMinDriveCmd
			|| ( -fabs(afDac[ii]) <= -32766 && (-fabs(afDac[ii - 1]) <= - 32766 || -fabs(afDac[ii - 2]) <= - 32766))  // 20110916
			)
		{
			fMinDriveCmd = -fabs(afDac[ii]);
			idxMinDriveCmd = ii;
		}
	}

	// 20120803
	int nSamplePositivePeakDrvCmd = 0, nSampleNegativePeakDrvCmd = 0;
	for( ii = (idxPositiveStepCmdStart+1); ii <= idxPositivePeakTime + 10; ii++) // 1:nLenTime_ms
	{
		if(fabs(fabs(afDac[ii]) - fabs(fMaxDriveCmd)) <= 1.5)
		{
			nSamplePositivePeakDrvCmd ++;
		}
	}

	for( ii = (idxNegativeStepCmdStart+1); ii <= idxNegativePeakTime + 10; ii++) // 1:nLenTime_ms
	{
		if(fabs(fabs(afDac[ii]) - fabs(fMinDriveCmd)) <= 1.5)
		{
			nSampleNegativePeakDrvCmd ++;
		}
	}	// 20120803

//	int nLenAnalyzeMaxFdAcc = gstSystemScope.uiDataLen/10;
	for( ii = idxMaxDriveCmd-1; ii < (unsigned int)(nLengthPositiveCmd + idxMaxDriveCmd); ii++) // 1:nLenTime_ms
	{
		if(fabs(afFeedAcc[ii]) > fMaxFdAcc)  // 20120509
		{
			fMaxFdAcc = fabs(afFeedAcc[ii]); // 20120509
			idxMaxFdAcc = ii;
		}
	}
	for( ii = idxMinDriveCmd-1; ii < (unsigned int)(nLengthNegativeCmd + idxMinDriveCmd); ii++) // 1:nLenTime_ms
	{
		if(-fabs(afFeedAcc[ii]) < fMinFdAcc) // 20120509
		{
			fMinFdAcc = -fabs(afFeedAcc[ii]); // 20120509
			idxMinFdAcc = ii;
		}
	}

/// Analyze by array and statistics
#define   LEN_FIND_PEAK_ACC_AFTER_DAC   3
static	double afMaxDriveCmd[LEN_ANALYZ_DRVCMD_FDACC]; // = afDac[0];
static	double afMinDriveCmd[LEN_ANALYZ_DRVCMD_FDACC]; // = afDac[0];
static	unsigned int aidxMaxDriveCmd[LEN_ANALYZ_DRVCMD_FDACC]; // = 0;
static	unsigned int aidxMinDriveCmd[LEN_ANALYZ_DRVCMD_FDACC]; // = 0;
static	double afMaxFdAcc[LEN_ANALYZ_DRVCMD_FDACC]; // = afFeedAcc[idxMaxDriveCmd];
static	int aidxMaxFdAcc[LEN_ANALYZ_DRVCMD_FDACC]; // = idxMaxDriveCmd;
static	double afMinFdAcc[LEN_ANALYZ_DRVCMD_FDACC]; // = afFeedAcc[idxMinDriveCmd];
static	int aidxMinFdAcc[LEN_ANALYZ_DRVCMD_FDACC]; // = idxMinDriveCmd;
static  double afMaxFdAccAtFullDriveCmd[LEN_ANALYZ_DRVCMD_FDACC];
static  double afMinFdAccAtFullDriveCmd[LEN_ANALYZ_DRVCMD_FDACC];
	unsigned int nTotalMaxPeak = 0;
	unsigned int nTotalMinPeak = 0;
double dTempPeak;
unsigned int idxTempPeakAftwards;
	for(ii = 0; ii<LEN_ANALYZ_DRVCMD_FDACC; ii++)
	{
		afMaxDriveCmd[ii] = 0;
		afMinDriveCmd[ii] = 0;
		aidxMaxDriveCmd[ii] = 0;
		aidxMinDriveCmd[ii] = 0;
		afMaxFdAcc[ii] = 0;
		aidxMaxFdAcc[ii] = 0;
		afMinFdAcc[ii] = 0;
		aidxMinFdAcc[ii] = 0;
		afMaxFdAccAtFullDriveCmd[ii] = 0;
		afMinFdAccAtFullDriveCmd[ii] = 0;
	}
// 20120726
#define __DEF_PEAK_ACC_NOISE__     3E3
	for( ii = (idxPositiveStepCmdStart+1); ii < (unsigned int)(nLengthPositiveCmd + idxPositiveStepCmdStart); ii++) // 1:nLenTime_ms
	{
		if( (afFeedAcc[ii] > afFeedAcc[ii - 1]) && (afFeedAcc[ii] > afFeedAcc[ii + 1]) && (afFeedAcc[ii] > __DEF_PEAK_ACC_NOISE__))
		{
			afMaxFdAcc[nTotalMaxPeak] = afFeedAcc[ii];
			aidxMaxFdAcc[nTotalMaxPeak] = ii;

			dTempPeak = mtn_aft_find_1st_max_peak(&afDac[ii-1], LEN_FIND_PEAK_ACC_AFTER_DAC, &idxTempPeakAftwards); // 20100921
			if(dTempPeak > 1)
			{
				afMaxDriveCmd[nTotalMaxPeak] = dTempPeak;
				aidxMaxDriveCmd[nTotalMaxPeak] = idxTempPeakAftwards + ii;
				afMaxFdAccAtFullDriveCmd[nTotalMaxPeak] = afMaxFdAcc[nTotalMaxPeak] * MAX_DRV_COMMAND_16_BIT_DAC/afMaxDriveCmd[nTotalMaxPeak];
				nTotalMaxPeak = nTotalMaxPeak + 1;
				if( nTotalMaxPeak >= LEN_ANALYZ_DRVCMD_FDACC) break;
			}
			else
			{
			}
		}
	}
	for( ii = (idxNegativeStepCmdStart+1); ii< (unsigned int)(nLengthNegativeCmd + idxNegativeStepCmdStart); ii++)
	{
		if( (afFeedAcc[ii] < afFeedAcc[ii - 1]) && (afFeedAcc[ii] < afFeedAcc[ii + 1]) && (afFeedAcc[ii] < -__DEF_PEAK_ACC_NOISE__) )
		{
			afMinFdAcc[nTotalMinPeak] = afFeedAcc[ii];
			aidxMinFdAcc[nTotalMinPeak] = ii;

			dTempPeak = mtn_aft_find_1st_min_peak(&afDac[ii-1], LEN_FIND_PEAK_ACC_AFTER_DAC, &idxTempPeakAftwards); // 20100921
			if(dTempPeak < -1)
			{
				afMinDriveCmd[nTotalMinPeak] = dTempPeak;
				aidxMinDriveCmd[nTotalMinPeak] = idxTempPeakAftwards + ii;
				afMinFdAccAtFullDriveCmd[nTotalMinPeak] = afMinFdAcc[nTotalMinPeak] *(MAX_DRV_COMMAND_16_BIT_DAC)/afMinDriveCmd[nTotalMinPeak];
				nTotalMinPeak = nTotalMinPeak + 1;
				if( nTotalMinPeak >= LEN_ANALYZ_DRVCMD_FDACC) break;
			}
			else
			{
			}
		}
	}

	double fEncResolutionCntPerMM = stpVelLoopTestInput->fEncResolutionCntPerMM;
	double fDriveCmdRatioRMS = stpVelLoopTestInput->fDriveCmdRatioRMS;


	stpVelLoopTestOutput->fMaxDriveCmd = fMaxDriveCmd;
	stpVelLoopTestOutput->fMinDriveCmd = fMinDriveCmd;
	stpVelLoopTestOutput->fMaxAcc = fMaxFdAcc;
	stpVelLoopTestOutput->fMinAcc = fMinFdAcc;
	stpVelLoopTestOutput->fAveMaxFeedBackAcc_mpss = (fabs(stpVelLoopTestOutput->fMaxAcc) + fabs(stpVelLoopTestOutput->fMinAcc))/2.0 / fEncResolutionCntPerMM / 1000; // / fEncResolutionCntPerMM / 1000;
//	if(stpVelLoopTestInput->iAxisCtrlCard == _EFSIKA_TABLE_X_AXIS || stpVelLoopTestInput->iAxisCtrlCard == _EFSIKA_TABLE_Y_AXIS)
//	{
		stpVelLoopTestOutput->fMaxAccEstimateFullDAC_mpss =  fabs(fMaxFdAcc/fMaxDriveCmd * MAX_DRV_COMMAND_16_BIT_DAC / fEncResolutionCntPerMM / 1000); //f_get_abs_max(afMaxFdAccAtFullDriveCmd, nTotalMaxPeak) / fEncResolutionCntPerMM / 1000; //  // f_get_abs_max
		stpVelLoopTestOutput->fMinAccEstimateFullDAC_mpss =  fabs(fMinFdAcc/fMaxDriveCmd * (-MAX_DRV_COMMAND_16_BIT_DAC) / fEncResolutionCntPerMM / 1000); // f_get_abs_max(afMinFdAccAtFullDriveCmd, nTotalMinPeak) / fEncResolutionCntPerMM / 1000; //  // f_get_abs_max
	//}
	//else
	//{
	//	stpVelLoopTestOutput->fMaxAccEstimateFullDAC_mpss =  fabs(afMaxFdAccAtFullDriveCmd[0]) / fEncResolutionCntPerMM / 1000; //f_get_abs_max(afMaxFdAccAtFullDriveCmd, nTotalMaxPeak) / fEncResolutionCntPerMM / 1000; // fMaxFdAcc/fMaxDriveCmd * MAX_DRV_COMMAND_16_BIT_DAC // f_get_mean // afMaxFdAccAtFullDriveCmd[0], f_get_abs_max(, nTotalMaxPeak)
	//	stpVelLoopTestOutput->fMinAccEstimateFullDAC_mpss =  fabs(afMinFdAccAtFullDriveCmd[0]) / fEncResolutionCntPerMM / 1000; // f_get_abs_max(afMinFdAccAtFullDriveCmd, nTotalMinPeak) / fEncResolutionCntPerMM / 1000; // fMinFdAcc/fMaxDriveCmd * (-MAX_DRV_COMMAND_16_BIT_DAC) // f_get_abs_max // f_get_mean(, nTotalMinPeak) f_get_abs_max(, nTotalMinPeak)
	//}
	stpVelLoopTestOutput->fAveMaxAccEstimateFullDAC_mpss = (fabs(stpVelLoopTestOutput->fMaxAccEstimateFullDAC_mpss) + fabs(stpVelLoopTestOutput->fMinAccEstimateFullDAC_mpss))/ 2.0; 

	stpVelLoopTestOutput->idxMaxDrvCmd = idxMaxDriveCmd;
	stpVelLoopTestOutput->idxMaxFdAcc = idxMaxFdAcc;
	stpVelLoopTestOutput->idxMinDrvCmd = idxMinDriveCmd;
	stpVelLoopTestOutput->idxMinFdAcc = idxMinFdAcc;

	stpVelLoopTestOutput->fExpectedAccAtRMS_mpss = stpVelLoopTestOutput->fAveMaxAccEstimateFullDAC_mpss * fDriveCmdRatioRMS * 2;

	stpVelLoopTestOutput->nTotalMinPeak = nTotalMinPeak;
	stpVelLoopTestOutput->nTotalMaxPeak = nTotalMaxPeak;
	for(ii = 0; ii<nTotalMinPeak; ii++)
	{
		stpVelLoopTestOutput->afMinDriveCmd[ii] = afMinDriveCmd[ii];
		stpVelLoopTestOutput->aidxMinDriveCmd[ii] = aidxMinDriveCmd[ii]; // = 0;
		stpVelLoopTestOutput->afMinFdAcc[ii] = afMinFdAcc[ii]; // = afFeedAcc[idxMinDriveCmd];
		stpVelLoopTestOutput->aidxMinFdAcc[ii] = aidxMinFdAcc[ii]; // = idxMinDriveCmd;
		stpVelLoopTestOutput->afMinFdAccAtFullDriveCmd[ii] = afMinFdAccAtFullDriveCmd[ii];

	}
	for(ii = 0; ii<nTotalMaxPeak; ii++)
	{
		stpVelLoopTestOutput->afMaxDriveCmd[ii] = afMaxDriveCmd[ii]; // = afDac[0];
		stpVelLoopTestOutput->aidxMaxDriveCmd[ii] = aidxMaxDriveCmd[ii]; // = 0;
		stpVelLoopTestOutput->afMaxFdAcc[ii] = afMaxFdAcc[ii]; // = afFeedAcc[idxMaxDriveCmd];
		stpVelLoopTestOutput->aidxMaxFdAcc[ii] = aidxMaxFdAcc[ii]; // = idxMaxDriveCmd;
		stpVelLoopTestOutput->afMaxFdAccAtFullDriveCmd[ii] = afMaxFdAccAtFullDriveCmd[ii];
	}

//////// Dynamic friction in constant velocity motion
int idxPositiveMoveConstVel = idxPositiveStepCmdStart + (int)(fPositiveStepRiseTime * 2 ); // + fPositiveStepSettleTime
int idxStartPositiveSkip = (int)(idxPositiveMoveConstVel + (nLengthPositiveCmd - (fPositiveStepRiseTime * 2)) * 0.1); // + fPositiveStepSettleTime
int nLenPositiveConstVel = (int)((nLengthPositiveCmd - (fPositiveStepRiseTime * 2)) * 0.8);  // + fPositiveStepSettleTime
double fMeanDrvCmd_PositiveConstVel = 0;

	for(ii = idxStartPositiveSkip; ii< (unsigned int)(idxStartPositiveSkip + nLenPositiveConstVel); ii++)
	{
		fMeanDrvCmd_PositiveConstVel += afDac[ii];
	}
	fMeanDrvCmd_PositiveConstVel = fMeanDrvCmd_PositiveConstVel/nLenPositiveConstVel;

double fStdDrvCmd_PositiveConstVel = 0;
	for(ii = idxStartPositiveSkip; ii< (unsigned int)(idxStartPositiveSkip + nLenPositiveConstVel); ii++)
	{
		fStdDrvCmd_PositiveConstVel += (afDac[ii] - fMeanDrvCmd_PositiveConstVel) * (afDac[ii] - fMeanDrvCmd_PositiveConstVel);
	}
	fStdDrvCmd_PositiveConstVel = sqrt(fStdDrvCmd_PositiveConstVel/nLenPositiveConstVel);


int idxNegativeMoveConstVel = idxNegativeStepCmdStart + (int)(fNegativeStepRiseTime * 2 ); // + fNegativeStepSettleTime
int idxStartNegativeSkip = (int)(idxNegativeMoveConstVel + (nLengthNegativeCmd - (fNegativeStepRiseTime * 2)) * 0.1);  // + fNegativeStepSettleTime
int nLenNegativeConstVel = (int)((nLengthNegativeCmd - (fNegativeStepRiseTime * 2 )) * 0.8);  // + fNegativeStepSettleTime
double fMeanDrvCmd_NegativeConstVel = 0;

	for(ii = idxStartNegativeSkip; ii< (unsigned int)(idxStartNegativeSkip + nLenNegativeConstVel); ii++)
	{
		fMeanDrvCmd_NegativeConstVel += afDac[ii];
	}
	fMeanDrvCmd_NegativeConstVel = fMeanDrvCmd_NegativeConstVel/(double)nLenNegativeConstVel;

double fStdDrvCmd_NegativeConstVel = 0;
	for(ii = idxStartNegativeSkip; ii< (unsigned int)(idxStartNegativeSkip + nLenNegativeConstVel); ii++)
	{
		fStdDrvCmd_NegativeConstVel += (afDac[ii] - fMeanDrvCmd_NegativeConstVel) * (afDac[ii] - fMeanDrvCmd_NegativeConstVel);
	}
	fStdDrvCmd_NegativeConstVel = sqrt(fStdDrvCmd_NegativeConstVel/(double)nLenNegativeConstVel);

	stpVelLoopTestOutput->fPositiveStepOverShoot    = fPositiveStepOverShoot   ;
	stpVelLoopTestOutput->fPositiveStepPercentOS    = fPositiveStepPercentOS   ;
	stpVelLoopTestOutput->fPositiveStepSettleTime   = fPositiveStepSettleTime * dSampleTime_ms ;
	stpVelLoopTestOutput->fPositiveStepRiseTime     = fPositiveStepRiseTime   * dSampleTime_ms ;
	stpVelLoopTestOutput->fNegativeStepOverShoot    = fNegativeStepOverShoot   ;
	stpVelLoopTestOutput->fNegativeStepPercentOS    = fNegativeStepPercentOS   ;
	stpVelLoopTestOutput->fNegativeStepSettleTime   = fNegativeStepSettleTime * dSampleTime_ms ;
	stpVelLoopTestOutput->fNegativeStepRiseTime     = fNegativeStepRiseTime   * dSampleTime_ms ;

	stpVelLoopTestOutput->fMeanDrvCmd_PositiveConstVel = fMeanDrvCmd_PositiveConstVel;
	stpVelLoopTestOutput->fStdDrvCmd_PositiveConstVel = fStdDrvCmd_PositiveConstVel;
	stpVelLoopTestOutput->fMeanDrvCmd_NegativeConstVel = fMeanDrvCmd_NegativeConstVel;
	stpVelLoopTestOutput->fStdDrvCmd_NegativeConstVel = fStdDrvCmd_NegativeConstVel;

	stpVelLoopTestOutput->idxPositiveStepCmdStart = idxPositiveStepCmdStart;
	stpVelLoopTestOutput->idxNegativeStepCmdStart = idxNegativeStepCmdStart;
	stpVelLoopTestOutput->fPositiveLevel = fPositiveLevel;
	stpVelLoopTestOutput->fNegativeLevel = fNegativeLevel;
	stpVelLoopTestOutput->idxOnePeriod = idxOnePeriod;
	stpVelLoopTestOutput->nLengthPositiveCmd = nLengthPositiveCmd;
	stpVelLoopTestOutput->nLengthNegativeCmd = nLengthNegativeCmd;
	stpVelLoopTestOutput->idxPositivePeakTime = idxPositivePeakTime;
	stpVelLoopTestOutput->idxNegativePeakTime = idxNegativePeakTime;

	// 20120803
	stpVelLoopTestOutput->fPositiveReturnTime = (idxPositiveReturnBottom - idxPositivePeakTime) * dSampleTime_ms ;
	stpVelLoopTestOutput->fNegativeReturnTime = (idxNegativeReturnPeak - idxNegativePeakTime) * dSampleTime_ms;
	stpVelLoopTestOutput->fPositiveReturnBottom = fPositiveReturnBottom;
	stpVelLoopTestOutput->fNegativeReturnPeak = fNegativeReturnPeak;

	stpVelLoopTestOutput->fSamplePositivePeakDrvCmd = dSampleTime_ms * nSamplePositivePeakDrvCmd;
	stpVelLoopTestOutput->fSampleNegativePeakDrvCmd = dSampleTime_ms * nSampleNegativePeakDrvCmd;

label_return_mtn_aft_vel_loop_calc_output:
	return iRet;
}

#include "aftprbs.h"
#include "acs_buff_prog.h"
int mtn_aft_vel_loop_test(HANDLE hHandle, AFT_VEL_LOOP_TEST_INPUT *stpVelLoopTestInput, AFT_VEL_LOOP_TEST_OUTPUT *stpVelLoopTestOutput)
{
int iPositionLower, iPositionUpper;
int iAxisCtrlCard = stpVelLoopTestInput->iAxisCtrlCard;
int iRet = MTN_API_OK_ZERO; // return value
CTRL_PARA_ACS stBakupServoPara, stServoParaVelLoopTest;
MTN_SPEED_PROFILE stBakupSpeedProfile, stVelTuningSpeedProfile;

	// Set upload variables
	aft_vel_loop_tune_init_array_ptr(); // mtn_tune_init_array_ptr();
	mtnscope_set_acsc_var_collecting_velloop_test(stpVelLoopTestInput->iAxisCtrlCard); //mtnscope_set_acsc_var_collecting_move(stpVelLoopTestInput->iAxisCtrlCard);
	iFlagStopVelLoopTest = FALSE;
	iFlagServoCtrlNoErr = TRUE;

	// verify input, iPositionLower < iPositionUpper
    if(stpVelLoopTestInput->iPositionLower > stpVelLoopTestInput->iPositionUpper)
	{
		iPositionLower = stpVelLoopTestInput->iPositionUpper;
		iPositionUpper = stpVelLoopTestInput->iPositionLower;
	}
	else
	{
		iPositionLower = stpVelLoopTestInput->iPositionLower;
		iPositionUpper = stpVelLoopTestInput->iPositionUpper;
	}

	// if fMaxAbsVel<0, fMaxAbsVel = fabs(fMaxAbsVel)
	if(stpVelLoopTestInput->fMaxAbsVel < 0)
	{
		stpVelLoopTestInput->fMaxAbsVel = fabs(stpVelLoopTestInput->fMaxAbsVel);
	}

	// if fMaxAbsDAC<0, fMaxAbsDAC = fabs(fMaxAbsDAC)
	if(stpVelLoopTestInput->fMaxAbsDAC < 0)
	{
		stpVelLoopTestInput->fMaxAbsDAC = fabs(stpVelLoopTestInput->fMaxAbsDAC);
	}

	// bakup XRMS-DAC, set up fMaxAbsDAC
	mtnapi_upload_servo_parameter_acs_per_axis(hHandle, iAxisCtrlCard, &stBakupServoPara);
	mtnapi_get_speed_profile(hHandle, iAxisCtrlCard, &stBakupSpeedProfile,  0); // Immediately se
	mtnapi_get_speed_profile(hHandle, iAxisCtrlCard, &stVelTuningSpeedProfile,  0); // 
	if(stpVelLoopTestInput->iAxisCtrlCard == _EFSIKA_TABLE_X_AXIS || stpVelLoopTestInput->iAxisCtrlCard == _EFSIKA_TABLE_Y_AXIS) // 20120725
	{
		stVelTuningSpeedProfile.dMaxAcceleration = stpVelLoopTestInput->fMaxAbsVel * 1000 * 20 * 2 *100; // can change max velocity to min velocity, 20 KHz, 
		stVelTuningSpeedProfile.dMaxJerk = stVelTuningSpeedProfile.dMaxAcceleration * 1000 * 20 * 2 *100; // can change max acc to min velocity, 20 KHz, 
	}
	else // BH hard-code, to match 4 time of 150g, 1500 * 4 = 6E3 * 1E6
		// Jerk 4 * 4 of 1500km/s^3 = 24000E3 * 1E6 = 24E12
	{
		stVelTuningSpeedProfile.dMaxAcceleration = 6E9;
		stVelTuningSpeedProfile.dMaxJerk = 2E12;
	}
	stVelTuningSpeedProfile.dMaxDeceleration = stVelTuningSpeedProfile.dMaxAcceleration;
	mtnapi_set_speed_profile(hHandle, iAxisCtrlCard, &stVelTuningSpeedProfile,  0); // set speed profile with larger range

	// Bakup safety parameter
	//SAFETY_PARA_ACS stSafetyParaBak, stSafetyParaCurr;
	//mtnapi_upload_safety_parameter_acs_per_axis(hHandle, iAxisCtrlCard, &stSafetyParaBak);
	//stSafetyParaCurr = stSafetyParaBak;
	//stSafetyParaCurr.dCriticalPosnErrAcc = 5000;
	//stSafetyParaCurr.dCriticalPosnErrIdle = 1000;	
	//stSafetyParaCurr.dCriticalPosnErrVel = 1000;
	//stSafetyParaCurr.dRMS_DrvCmdX = TABLE_X_RMS_DRVCMDX;  // 2013
	//stSafetyParaCurr.dRMS_DrvCmdIdle = 100;
	//stSafetyParaCurr.dRMS_DrvCmdMtn = 100;  // 20120717, must release to 100% for motion tuning
	//mtnapi_download_safety_parameter_acs_per_axis(hHandle, iAxisCtrlCard, &stSafetyParaCurr);

	// Disable Position loop
	mtnapi_upload_servo_parameter_acs_per_axis(hHandle, iAxisCtrlCard, &stServoParaVelLoopTest);
	stServoParaVelLoopTest.dPositionLoopProportionalGain = 0;
	mtnapi_download_servo_parameter_acs_per_axis(hHandle, iAxisCtrlCard, &stServoParaVelLoopTest);

	// if motor is not enabled, enable it
static int iMotorState;
	mtnapi_get_motor_state(hHandle, iAxisCtrlCard, &iMotorState, 0);
	if(!(iMotorState & ACSC_MST_ENABLE)) 
	{
		// now is disabled, to enable it
		mtnapi_enable_motor(hHandle, iAxisCtrlCard,	0);
	}

#ifdef  __MOTION_BY_C_COMMAND__
static int iFlags = 0;
double dFbPosition, dJogVelocity = 0;
	mtnapi_get_fb_position(hHandle, iAxisCtrlCard, &dFbPosition, NULL);
	// if current feedback position > iPositionUpper, set immediate velocity to - fMaxAbsVel, wait until hit < iPositionLower, set immediate velocity to fMaxAbsVel
int iFlagProtectInitialOutRange = 0;
	if(dFbPosition > stpVelLoopTestInput->iPositionUpper)
	{
		dJogVelocity = -stpVelLoopTestInput->fMaxAbsVel;
	}
	// elseif current feedback position < iPositionLower, set immediate velocity to fMaxAbsVel, wait until hit > iPositionUpper
	else //if(dFbPosition < stpVelLoopTestInput->iPositionLower)
	{
		dJogVelocity = stpVelLoopTestInput->fMaxAbsVel;
	}
	acsc_Jog(hHandle, ACSC_AMF_VELOCITY, iAxisCtrlCard, dJogVelocity, NULL); // Immediate start

	// Start Data Capture
	if (!acsc_CollectB(hHandle, 0, // system data collection
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
#define _VEL_LOOP_SLEEP_MS_CHANGING_DIR   20
unsigned int uiChangeDirNum=0;
int nMilliSecondSleepByTransition = _VEL_LOOP_SLEEP_MS_CHANGING_DIR;
	nMilliSecondSleepByTransition = (int)fabs((double)(stpVelLoopTestInput->iPositionUpper - stpVelLoopTestInput->iPositionLower) / stpVelLoopTestInput->fMaxAbsVel * 1000/2.0);
	uiCurrLoop = 0;
	iFlagServoCtrlNoErr = 1;
	while((uiCurrLoop < stpVelLoopTestInput->uiLoopNum) && iFlagServoCtrlNoErr && (iFlagStopVelLoopTest == FALSE))
	{
	// loop uiLoopNum or kbhit
	// set immediate velocity to - fMaxAbsVel
		mtnapi_get_fb_position(hHandle, iAxisCtrlCard, &dFbPosition, NULL);
		if((dFbPosition > stpVelLoopTestInput->iPositionUpper) && (dJogVelocity > 0)) 	  // wait until hit > iPositionUpper
		{
			dJogVelocity = -stpVelLoopTestInput->fMaxAbsVel;
			acsc_Jog(hHandle, ACSC_AMF_VELOCITY, iAxisCtrlCard, dJogVelocity, NULL); // Immediate start, iFlags
			uiChangeDirNum++;
			Sleep(nMilliSecondSleepByTransition);
			if((uiChangeDirNum % 2 == 0))
			{
				uiCurrLoop ++;
			}
		}
		else if((dFbPosition < stpVelLoopTestInput->iPositionLower)  && (dJogVelocity < 0)) // wait until hit < iPositionLower
		{
			dJogVelocity = stpVelLoopTestInput->fMaxAbsVel;
			acsc_Jog(hHandle, ACSC_AMF_VELOCITY, iAxisCtrlCard, dJogVelocity, NULL); // Immediate start
			uiChangeDirNum++;
			Sleep(nMilliSecondSleepByTransition); // _VEL_LOOP_SLEEP_MS_CHANGING_DIR);
			if((uiChangeDirNum % 2 == 0))
			{
				uiCurrLoop ++;
			}
		}
		else 
		{
		}


		if((uiChangeDirNum % 6 == 0) && 
			(uiCurrLoop >=2 ) && (uiCurrLoop < (stpVelLoopTestInput->uiLoopNum - 6)))
		{
			mtnscope_upload_acsc_data(hHandle);
			aft_vel_loop_tune_init_array_ptr(); // mtn_tune_init_array_ptr();
			aft_vel_test_port_data_to_extern_xls_file();
			if (!acsc_CollectB(hHandle, 0, // system data collection
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
		}
		else
		{
			Sleep(_VEL_LOOP_SLEEP_MS_CHANGING_DIR); // nMilliSecondSleepByTransition);
		}

	// collection data every 2 loops, data analysis, set trigger data collection
	// set immediate velocity to fMaxAbsVel
	
		if(qc_is_axis_not_safe(hHandle, iAxisCtrlCard)) 
		{
			// now is disabled, to enable it
			iFlagServoCtrlNoErr = 0;
			iRet = MTN_API_ERROR;
		}

	}
	acsc_Halt(hHandle, iAxisCtrlCard, NULL);
#else
	acs_run_buffer_prog_prbs_prof_cfg_move();
	acs_bufprog_write_motion_flag_buff_8(ACS_BUFPROG_MTN_FLAG_VELOOP_STEP);
	aft_vel_loop_test_download_condition(hHandle, stpVelLoopTestInput);
	aft_start_prbs_excitation();

	Sleep(100);
	unsigned int iCurrIter;
	aft_vel_loop_test_get_current_loop(hHandle, &iCurrIter);

	while(iCurrIter < stpVelLoopTestInput->uiLoopNum)
	{
		Sleep(200);
		aft_vel_loop_test_get_current_loop(hHandle, &iCurrIter);
		uiCurrLoop = iCurrIter;

		// Protection any cut-off
		if(mtn_qc_is_axis_locked_safe(hHandle, iAxisCtrlCard) == FALSE)
		{
			iRet = MTN_API_ERROR;
			goto label_return_run_vel_step_test_once;
		}
	}
	mtnapi_download_servo_parameter_acs_per_axis(hHandle, iAxisCtrlCard, &stBakupServoPara);  // 20130203
	mtnapi_set_speed_profile(hHandle, iAxisCtrlCard, &stBakupSpeedProfile,  0); // 20130203

	Sleep(500);
	acsc_Halt(hHandle, iAxisCtrlCard, NULL);
	iFlagStopVelLoopTest = TRUE;
//	iFlagServoCtrlNoErr = TRUE;

// Upload data
//			mtnscope_upload_acsc_data(hHandle);
	gstSystemScope.uiDataLen = 4200;
	gstSystemScope.uiNumData = 5;
	if (!acsc_ReadReal(hHandle, ACSC_NONE, "rAFT_Scope", 0, gstSystemScope.uiNumData-1, 0, gstSystemScope.uiDataLen - 1, gdScopeCollectData, NULL))
	{
		iRet = MTN_API_ERROR_UPLOAD_DATA;
	}
	aft_vel_loop_tune_init_array_ptr(); // mtn_tune_init_array_ptr();
	// dSampleTime_ms
	double dTempCTime[1]; 
	if (!acsc_ReadReal(hHandle, ACSC_NONE, "CTIME", 0, 0, 0, 0, dTempCTime, NULL))
	{
		iRet = MTN_API_ERROR_UPLOAD_DATA;
	}
	dSampleTime_ms = dTempCTime[0];
	if (!acsc_ReadReal(hHandle, ACSC_NONE, "XVEL", stpVelLoopTestInput->iAxisCtrlCard, stpVelLoopTestInput->iAxisCtrlCard, 0, 0, &dMaxVelACS, NULL))
	{
		iRet = MTN_API_ERROR_UPLOAD_DATA;
	}  // 20120710
//			aft_vel_test_port_data_to_extern_xls_file();
#endif // __MOTION_BY_C_COMMAND__

	// 
	int iRetCalcOutput;
	if(iRet == MTN_API_OK_ZERO)
	{
		// Check the driver polarity
		if(	(stpVelLoopTestInput->iAxisCtrlCard == sys_get_acs_axis_id_bnd_z()  ) 
			|| (stpVelLoopTestInput->iAxisCtrlCard == APP_Z_BOND_ACS_SC_UDI_ID  ))   // 20100921 || mtn_api_get_flag_inv_drv_polarity(hHandle, stpVelLoopTestInput->iAxisCtrlCard) != 0
		{
			for(unsigned int ii=0; ii< gstSystemScope.uiDataLen; ii++)
			{
				afDac[ii] = - afDac[ii];
			}
		}

		iRetCalcOutput = mtn_aft_vel_loop_calc_output(stpVelLoopTestInput, stpVelLoopTestOutput);
	}
	// 20120220
#define DEF_MAXIMUM_SETTLE_TIME    15
	int iSaveFileFlag = stpVelLoopTestInput->iFlagSaveFile;
	if(stpVelLoopTestOutput->fNegativeStepSettleTime >= DEF_MAXIMUM_SETTLE_TIME
		|| stpVelLoopTestOutput->fPositiveStepSettleTime >= DEF_MAXIMUM_SETTLE_TIME
		|| iRetCalcOutput != MTN_API_OK_ZERO)
	{
		stpVelLoopTestInput->iFlagSaveFile = 1;
	}
	aft_vel_loop_test_save_data_to_file(stpVelLoopTestInput, stpVelLoopTestOutput);
	stpVelLoopTestInput->iFlagSaveFile = iSaveFileFlag; // 20120220

//	aft_vel_test_port_data_to_extern_xls_file();
//201301
	// Restore the bakup profile
label_return_run_vel_step_test_once:
	mtnapi_set_speed_profile(hHandle, iAxisCtrlCard, &stBakupSpeedProfile,  0);
	mtnapi_download_servo_parameter_acs_per_axis(hHandle, iAxisCtrlCard, &stBakupServoPara);
//	mtnapi_download_safety_parameter_acs_per_axis(hHandle, iAxisCtrlCard, &stSafetyParaBak);

	// restore the XRMS-DAC
	acs_stop_buffer_prog_prbs_prof_cfg_move();

	return iRet;
}

int aft_vel_loop_test_get_current_loop(HANDLE stCommHandleACS, unsigned int *uiCurrIter)
{
	int iRet = MTN_API_OK_ZERO;
	int iTempUpload;
	if(!acsc_ReadInteger(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "iCurrLoop", 0, 0, 0, 0, &iTempUpload, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}
	else
	{
		*uiCurrIter = (unsigned int)iTempUpload;
	}
	return iRet;
}

int aft_vel_loop_test_download_condition(HANDLE stCommHandleACS, AFT_VEL_LOOP_TEST_INPUT *stpVelLoopTestInput)
{
	int iRet = MTN_API_OK_ZERO;
//	"rUppPosnLimit = 5000; rLowPosnLimit = -5000;\n",  // 20110727
//	"rAbsJogVel = 50000;\n",   // 20110727
	double dTempDownload;
	dTempDownload = (double)stpVelLoopTestInput->iPositionUpper;
	if(!acsc_WriteReal(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "rUppPosnLimit", 0, 0, 0, 0, &dTempDownload, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	dTempDownload = (double)stpVelLoopTestInput->iPositionLower;
	if(!acsc_WriteReal(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "rLowPosnLimit", 0, 0, 0, 0, &dTempDownload, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	dTempDownload = (double)stpVelLoopTestInput->fMaxAbsVel;
	if(!acsc_WriteReal(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "rAbsJogVel", 0, 0, 0, 0, &dTempDownload, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	int iTempDownload;
	iTempDownload = stpVelLoopTestInput->iAxisCtrlCard;
	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "iAFT_Axis", 0, 0, 0, 0, &iTempDownload, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}
	iTempDownload = stpVelLoopTestInput->uiLoopNum;
	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "nNumLoops", 0, 0, 0, 0, &iTempDownload, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	return iRet;
}

void aft_vel_loop_test_save_data_to_file(AFT_VEL_LOOP_TEST_INPUT *stpVelLoopTestInput, AFT_VEL_LOOP_TEST_OUTPUT *stpVelLoopTestOutput)
{
short sRet = MTN_API_OK_ZERO;
//unsigned int ii;
FILE *fpDataTraj;
static	char strFilename[256];
struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;

	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);
	sprintf_s(strFilename, 256, "%s\\%s_%d_%d_%d_H%dm%d_%s", strVeLoopTestNewFolderName,
		stpVelLoopTestInput->strServoAxisNameWb, 
		stpTime->tm_year + 1900, stpTime->tm_mon + 1, stpTime->tm_mday, 
		stpTime->tm_hour, stpTime->tm_min,
		FILENAME_TRAJ_TEMP_DATA);
	if(stpVelLoopTestInput->iFlagSaveFile == TRUE)
	{
		fopen_s(&fpDataTraj, strFilename, "w");  // FILENAME_TRAJ_TEMP_DATA
	}

	if(fpDataTraj != NULL && stpVelLoopTestInput->iFlagSaveFile == TRUE)
	{
		strcpy_s(stpVelLoopTestOutput->strOutputFilename, 128, strFilename);
	//	fprintf(fpDataTraj, "%% ACSC Controller, %s\n\n", strACSC_VarName);
		fprintf(fpDataTraj, "YYYYMMDDHHMMSS = [%d, %d, %d, %d, %d, %d]\n", stpTime->tm_year + 1900, stpTime->tm_mon + 1, 
				stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, stpTime->tm_sec);
		fprintf(fpDataTraj, "StepVel = %6.1f\n", stpVelLoopTestInput->fMaxAbsVel);

		fprintf(fpDataTraj, "MaxPosAccEstimateFullDAC = %8.1f %%(m/s/s) \n", stpVelLoopTestOutput->fMaxAccEstimateFullDAC_mpss);
		fprintf(fpDataTraj, "MaxNegAccEstimateFullDAC = %8.1f %%(m/s/s) \n", stpVelLoopTestOutput->fMinAccEstimateFullDAC_mpss);
		fprintf(fpDataTraj, "MaxAccEstimateFullDAC = %8.1f %%(m/s/s) \n", stpVelLoopTestOutput->fAveMaxAccEstimateFullDAC_mpss);
		fprintf(fpDataTraj, "MaxFbAcc = %8.1f %%(m/s/s) \n", stpVelLoopTestOutput->fAveMaxFeedBackAcc_mpss);
		fprintf(fpDataTraj, "ExpAccAtRMS = %8.1f %%(m/s/s) \n\n", stpVelLoopTestOutput->fExpectedAccAtRMS_mpss);
		fprintf(fpDataTraj, "%%% Velocity Performance \n");
		fprintf(fpDataTraj, "fPositiveStepOverShoot = %8.1f \n", stpVelLoopTestOutput->fPositiveStepOverShoot);
		fprintf(fpDataTraj, "fPositiveStepPercentOS = %8.3f \n", stpVelLoopTestOutput->fPositiveStepPercentOS);
		fprintf(fpDataTraj, "fPositiveStepSettleTime = %8.1f \n", stpVelLoopTestOutput->fPositiveStepSettleTime);
		fprintf(fpDataTraj, "fPositiveStepRiseTime = %8.1f \n", stpVelLoopTestOutput->fPositiveStepRiseTime);
		fprintf(fpDataTraj, "fNegativeStepOverShoot = %8.1f \n", stpVelLoopTestOutput->fNegativeStepOverShoot);
		fprintf(fpDataTraj, "fNegativeStepPercentOS = %8.3f \n", stpVelLoopTestOutput->fNegativeStepPercentOS);
		fprintf(fpDataTraj, "fNegativeStepSettleTime = %8.1f \n", stpVelLoopTestOutput->fNegativeStepSettleTime);
		fprintf(fpDataTraj, "fNegativeStepRiseTime = %8.1f \n", stpVelLoopTestOutput->fNegativeStepRiseTime);

		fprintf(fpDataTraj, "fMeanDrvCmd_PosiNegConstVel = [%8.1f, %8.1f] \n", stpVelLoopTestOutput->fMeanDrvCmd_PositiveConstVel, 
			stpVelLoopTestOutput->fMeanDrvCmd_NegativeConstVel);
		fprintf(fpDataTraj, "fStdDrvCmd_PosiNegConstVel = [%8.1f, %8.1f] \n\n", stpVelLoopTestOutput->fStdDrvCmd_PositiveConstVel, 
			stpVelLoopTestOutput->fStdDrvCmd_NegativeConstVel);

		fprintf(fpDataTraj, "idxMaxDrvCmd = %d; \n", stpVelLoopTestOutput->idxMaxDrvCmd);
		fprintf(fpDataTraj, "idxMaxFdAcc = %d; \n", stpVelLoopTestOutput->idxMaxFdAcc);
		fprintf(fpDataTraj, "idxMinDrvCmd = %d; \n", stpVelLoopTestOutput->idxMinDrvCmd);
		fprintf(fpDataTraj, "idxMinFdAcc = %d; \n", stpVelLoopTestOutput->idxMinFdAcc);

		fprintf(fpDataTraj, "idxPositiveStepCmdStart = %d; \n", stpVelLoopTestOutput->idxPositiveStepCmdStart);
		fprintf(fpDataTraj, "idxNegativeStepCmdStart = %d; \n", stpVelLoopTestOutput->idxNegativeStepCmdStart);
		fprintf(fpDataTraj, "fPositiveLevel = %8.1f; \n", stpVelLoopTestOutput->fPositiveLevel);
		fprintf(fpDataTraj, "fNegativeLevel = %8.1f; \n", stpVelLoopTestOutput->fNegativeLevel);
		fprintf(fpDataTraj, "nLengthPositiveCmd = %d;\n", stpVelLoopTestOutput->nLengthPositiveCmd);
		fprintf(fpDataTraj, "nLengthNegativeCmd = %d; \n", stpVelLoopTestOutput->nLengthNegativeCmd);
		fprintf(fpDataTraj, "idxOnePeriod = %d; \n", stpVelLoopTestOutput->idxOnePeriod);
		fprintf(fpDataTraj, "idxPositivePeakTime = %d;\n", stpVelLoopTestOutput->idxPositivePeakTime);
		fprintf(fpDataTraj, "idxNegativePeakTime = %d; \n\n", stpVelLoopTestOutput->idxNegativePeakTime);

		// 20120804
		fprintf(fpDataTraj, "fSamplePositivePeakDrvCmd = %3.1f; \n", stpVelLoopTestOutput->fSamplePositivePeakDrvCmd);
		fprintf(fpDataTraj, "fSampleNegativePeakDrvCmd = %3.1f; \n\n", stpVelLoopTestOutput->fSampleNegativePeakDrvCmd);

		unsigned int ii;
		if(stpVelLoopTestInput->iAxisCtrlCard == _EFSIKA_BOND_Z_AXIS)
		{
		fprintf(fpDataTraj, "%%%% Statistic Method\n");
		// total min peak
		unsigned int nTotalMinPeak = stpVelLoopTestOutput->nTotalMinPeak, nTotalMaxPeak = stpVelLoopTestOutput->nTotalMaxPeak;
		fprintf(fpDataTraj, "nTotalMinPeak = %d; \n", stpVelLoopTestOutput->nTotalMinPeak);

		if(stpVelLoopTestOutput->nTotalMinPeak >= 1)
		{
			fprintf(fpDataTraj, "afMinDriveCmd = [");
			for(ii = 0; ii<stpVelLoopTestOutput->nTotalMinPeak - 1; ii++)
			{
				fprintf(fpDataTraj, "%8.2f, ", stpVelLoopTestOutput->afMinDriveCmd[ii]);
			}
			fprintf(fpDataTraj, "%8.2f]; \n ", stpVelLoopTestOutput->afMinDriveCmd[ii]);

			fprintf(fpDataTraj, "aidxMinDriveCmd = [");
			for(ii = 0; ii<stpVelLoopTestOutput->nTotalMinPeak - 1; ii++)
			{
				fprintf(fpDataTraj, "%d, ", stpVelLoopTestOutput->aidxMinDriveCmd[ii]);
			}
			fprintf(fpDataTraj, "%d]; \n ", stpVelLoopTestOutput->aidxMinDriveCmd[ii]);

			fprintf(fpDataTraj, "aidxMinFdAcc = [");
			for(ii = 0; ii<stpVelLoopTestOutput->nTotalMinPeak - 1; ii++)
			{
				fprintf(fpDataTraj, "%d, ", stpVelLoopTestOutput->aidxMinFdAcc[ii]);
			}
			fprintf(fpDataTraj, "%d]; \n ", stpVelLoopTestOutput->aidxMinFdAcc[ii]);

			fprintf(fpDataTraj, "afMinFdAcc = [");
			for(ii = 0; ii<stpVelLoopTestOutput->nTotalMinPeak - 1; ii++)
			{
				fprintf(fpDataTraj, "%8.2f, ", stpVelLoopTestOutput->afMinFdAcc[ii]);
			}
			fprintf(fpDataTraj, "%8.2f]; \n ", stpVelLoopTestOutput->afMinFdAcc[ii]);

			fprintf(fpDataTraj, "afMinFdAccAtFullDriveCmd = [");
			for(ii = 0; ii<stpVelLoopTestOutput->nTotalMinPeak - 1; ii++)
			{
				fprintf(fpDataTraj, "%8.2f, ", stpVelLoopTestOutput->afMinFdAccAtFullDriveCmd[ii]);
			}
			fprintf(fpDataTraj, "%8.2f]; \n ", stpVelLoopTestOutput->afMinFdAccAtFullDriveCmd[ii]);
		}

		// total max peak
		fprintf(fpDataTraj, "nTotalMaxPeak = %d; \n", stpVelLoopTestOutput->nTotalMaxPeak);
		if(stpVelLoopTestOutput->nTotalMaxPeak >= 1)
		{
			fprintf(fpDataTraj, "afMaxDriveCmd = [");
			for(ii = 0; ii<stpVelLoopTestOutput->nTotalMaxPeak- 1; ii++)
			{
				fprintf(fpDataTraj, "%8.2f, ", stpVelLoopTestOutput->afMaxDriveCmd[ii]);
			}
			fprintf(fpDataTraj, "%8.2f]; \n ", stpVelLoopTestOutput->afMaxDriveCmd[ii]);
			fprintf(fpDataTraj, "aidxMaxDriveCmd = [");
			for(ii = 0; ii<stpVelLoopTestOutput->nTotalMaxPeak- 1; ii++)
			{
				fprintf(fpDataTraj, "%d, ", stpVelLoopTestOutput->aidxMaxDriveCmd[ii]);
			}
			fprintf(fpDataTraj, "%d]; \n ", stpVelLoopTestOutput->aidxMaxDriveCmd[ii]);

			fprintf(fpDataTraj, "aidxMaxFdAcc = [");
			for(ii = 0; ii<stpVelLoopTestOutput->nTotalMaxPeak- 1; ii++)
			{
				fprintf(fpDataTraj, "%d, ", stpVelLoopTestOutput->aidxMaxFdAcc[ii]);
			}
			fprintf(fpDataTraj, "%d]; \n ", stpVelLoopTestOutput->aidxMaxFdAcc[ii]);

			fprintf(fpDataTraj, "afMaxFdAcc = [");
			for(ii = 0; ii<stpVelLoopTestOutput->nTotalMaxPeak- 1; ii++)
			{
				fprintf(fpDataTraj, "%8.2f, ", stpVelLoopTestOutput->afMaxFdAcc[ii]);
			}
			fprintf(fpDataTraj, "%8.2f]; \n ", stpVelLoopTestOutput->afMaxFdAcc[ii]);

			fprintf(fpDataTraj, "afMaxFdAccAtFullDriveCmd = [");
			for(ii = 0; ii<stpVelLoopTestOutput->nTotalMaxPeak- 1; ii++)
			{
				fprintf(fpDataTraj, "%8.2f, ", stpVelLoopTestOutput->afMaxFdAccAtFullDriveCmd[ii]);
			}
			fprintf(fpDataTraj, "%8.2f]; \n ", stpVelLoopTestOutput->afMaxFdAccAtFullDriveCmd[ii]);
			}
		}


		fprintf(fpDataTraj, "%% RPOS FVEL RVEL FACC DOUT\n");
		fprintf(fpDataTraj, "TrajData = [");
		for(ii = 0; ii<gstSystemScope.uiDataLen; ii++)
		{
			fprintf(fpDataTraj, "%8.2f, %8.2f, %8.2f, %8.2f, %8.2f", 
				afRefPosn[ii], afFeedVel[ii], afRefVel[ii], afFeedAcc[ii], afDac[ii]);

			if(ii == (gstSystemScope.uiDataLen - 1))
			{
				fprintf(fpDataTraj, "];\n");
			}
			else
			{
				fprintf(fpDataTraj, "\n");
			}
		}

		fclose(fpDataTraj);
	}

}

int mtn_api_tuning_velocity_loop()
{
int iRet = MTN_API_OK_ZERO; // return value


	return iRet;
}
static int iBondHeadRelaxPosition;
void aft_vel_loop_set_bond_head_relax_position(int iPosn)
{
	iBondHeadRelaxPosition = iPosn;
}

#include "MtnTesterResDef.h"
extern char *astrWireBondServoAxisNameEn[];
extern AXIS_INFO_WIRE_BOND astAxisInfoWireBond[MAX_SERVO_AXIS_WIREBOND];

void aft_vel_loop_test_init_default_cfg(AFT_GROUP_VEL_STEP_TEST_CONFIG *stpVelStepGroupTestConfig)
{
	int ii;
	for(ii = 0; ii<MAX_NUM_AXIS_GROUP_VEL_STEP_TEST; ii++)
	{
		stpVelStepGroupTestConfig->uiAxis_CtrlCardId[ii] = MAX_CTRL_AXIS_PER_SERVO_BOARD;
	}

	stpVelStepGroupTestConfig->usActualNumAxis = 3;

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[0] = APP_X_TABLE_ACS_ID;
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[0] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[0] = 10; // in count 20000; 
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[0] = -69; // in count -138000;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[0] = 69; // in count 138000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[0] = 1;
	stpVelStepGroupTestConfig->uiLoopNum[0] = 10;
	stpVelStepGroupTestConfig->uiNumPoints[0] = 3; // 20;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[0] = 0.15;
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[0] = 0.25;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[0] = 
		astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[0] = astrWireBondServoAxisNameEn[0];

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[1] = APP_Y_TABLE_ACS_ID;
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[1] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[1] = 10; // in count 20000;
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[1] = -10; // in count -20000;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[1] = 20; // in count 40000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[1] = 1;
	stpVelStepGroupTestConfig->uiLoopNum[1] = 10;
	stpVelStepGroupTestConfig->uiNumPoints[1] = 3;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[1] = 0.10;
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[1] = 0.25;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[1] = 
		astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[1] = astrWireBondServoAxisNameEn[1];

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[2] = sys_get_acs_axis_id_bnd_z();
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[2] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[2] = 3; // in count 2000;
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[2] = iBondHeadRelaxPosition - 1;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[2] = iBondHeadRelaxPosition + 3; // in count 8000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[2] = 1;
	stpVelStepGroupTestConfig->uiLoopNum[2] = 10;
	stpVelStepGroupTestConfig->uiNumPoints[2] = 2;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[2] = 0.01;  // 20120717
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[2] = 0.2;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[2] = //1000;WB_AXIS_BOND_Z
		astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[2] = astrWireBondServoAxisNameEn[2];

	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_TABLE_X] = 10.0;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_TABLE_Y] = 10.0;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_BOND_Z] = 10.9;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_WIRE_CLAMP] = 2.0;
}

void aft_vel_loop_test_13v_init_default_cfg(AFT_GROUP_VEL_STEP_TEST_CONFIG *stpVelStepGroupTestConfig)
{
	int ii;
	for(ii = 0; ii<MAX_NUM_AXIS_GROUP_VEL_STEP_TEST; ii++)
	{
		stpVelStepGroupTestConfig->uiAxis_CtrlCardId[ii] = MAX_CTRL_AXIS_PER_SERVO_BOARD;
	}

	stpVelStepGroupTestConfig->usActualNumAxis = 3;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_TABLE_X] = 10.0;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_TABLE_Y] = 16.0;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_BOND_Z] = 10.9;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_WIRE_CLAMP] = 2.0;

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[0] = APP_X_TABLE_ACS_ID;
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[0] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[0] = 8; // in count 2500; 
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[0] = 5; // in count 2000;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[0] = 35; // in count 24000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[0] = 1;
	stpVelStepGroupTestConfig->uiLoopNum[0] = 11;
	stpVelStepGroupTestConfig->uiNumPoints[0] = 2; // 20;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[0] = 0.15;
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[0] = 0.25;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[0] = ///2000;
		astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[0] = astrWireBondServoAxisNameEn[0];

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[1] = APP_Y_TABLE_ACS_ID;
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[1] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[1] = 8; // in count 20000;
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[1] = 0; //  Special for 18V
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[1] = 20; // in count 40000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[1] = 1;
	stpVelStepGroupTestConfig->uiLoopNum[1] = 11;
	stpVelStepGroupTestConfig->uiNumPoints[1] = 2;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[1] = 0.10;
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[1] = 0.25;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[1] = ///2000;
		astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[1] = astrWireBondServoAxisNameEn[1];

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[2] = sys_get_acs_axis_id_bnd_z();
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[2] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[2] = 3; // in count 2000;
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[2] = -6; // iBondHeadRelaxPosition - 1;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[2] = 2;  // iBondHeadRelaxPosition + 3; // in count 8000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[2] = 1;
	stpVelStepGroupTestConfig->uiLoopNum[2] = 11;
	stpVelStepGroupTestConfig->uiNumPoints[2] = 2;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[2] = 0.01;  // 20120717
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[2] = 0.2;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[2] = //1000;
		astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[2] = astrWireBondServoAxisNameEn[2];
}

void aft_vel_loop_test_wb_18V_init_default_cfg(AFT_GROUP_VEL_STEP_TEST_CONFIG *stpVelStepGroupTestConfig)
{
	int ii;
	for(ii = 0; ii<MAX_NUM_AXIS_GROUP_VEL_STEP_TEST; ii++)
	{
		stpVelStepGroupTestConfig->uiAxis_CtrlCardId[ii] = MAX_CTRL_AXIS_PER_SERVO_BOARD;
	}
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_TABLE_X] = 10.0;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_TABLE_Y] = 8.0;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_BOND_Z] = 10.9;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_WIRE_CLAMP] = 2.0;

	stpVelStepGroupTestConfig->usActualNumAxis = 3;

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[0] = APP_Y_TABLE_ACS_ID;
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[0] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[0] = 8; // in count 2500; 
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[0] = 5; // in count 2000;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[0] = 30; // in count 24000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[0] = 1;
	stpVelStepGroupTestConfig->uiLoopNum[0] = 11;
	stpVelStepGroupTestConfig->uiNumPoints[0] = 2; // 20;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[0] = 0.15; // Special for 18V
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[0] = 0.25;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[0] = ///2000;
		astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[0] = astrWireBondServoAxisNameEn[0];

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[1] = APP_X_TABLE_ACS_ID;
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[1] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[1] = 6; // in count 12000;
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[1] = 8; // in count  16000;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[1] = 26; // in count 52000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[1] = 1;
	stpVelStepGroupTestConfig->uiLoopNum[1] = 11;
	stpVelStepGroupTestConfig->uiNumPoints[1] = 2;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[1] = 0.15; // Special for 18V
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[1] = 0.25;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[1] = ///2000;
		astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[1] = astrWireBondServoAxisNameEn[1];

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[2] = sys_get_acs_axis_id_bnd_z();
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[2] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[2] = 3; // in count 2000;
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[2] = -6; // iBondHeadRelaxPosition - 1;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[2] = 1;  // iBondHeadRelaxPosition + 3; // in count 8000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[2] = 1;
	stpVelStepGroupTestConfig->uiLoopNum[2] = 11;
	stpVelStepGroupTestConfig->uiNumPoints[2] = 2;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[2] = 0.05; // Special for 18V
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[2] = 0.2;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[2] = //1000;
		astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[2] = astrWireBondServoAxisNameEn[2];
}
void aft_vel_loop_test_station_xy_init_VLED_cfg(AFT_GROUP_VEL_STEP_TEST_CONFIG *stpVelStepGroupTestConfig)
{
	int ii;
	for(ii = 0; ii<MAX_NUM_AXIS_GROUP_VEL_STEP_TEST; ii++)
	{
		stpVelStepGroupTestConfig->uiAxis_CtrlCardId[ii] = MAX_CTRL_AXIS_PER_SERVO_BOARD;
	}

	stpVelStepGroupTestConfig->usActualNumAxis = 2;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_TABLE_X] = 10.0;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_TABLE_Y] = 16.0;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_BOND_Z] = 10.9;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_WIRE_CLAMP] = 2.0;

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[0] = APP_X_TABLE_ACS_ID;
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[0] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[0] = 8; // in count 2500; 
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[0] = 5; // in count 2000;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[0] = 35; // in count 24000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[0] = 1;
	stpVelStepGroupTestConfig->uiLoopNum[0] = 11;
	stpVelStepGroupTestConfig->uiNumPoints[0] = 2; // 20;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[0] = 0.15;
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[0] = 0.25;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[0] = //2000;
		astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[0] = astrWireBondServoAxisNameEn[0];

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[1] = APP_Y_TABLE_ACS_ID;
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[1] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[1] = 8; // in count 20000;
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[1] = -10; // in count -20000;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[1] = 20; // in count 40000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[1] = 1;
	stpVelStepGroupTestConfig->uiLoopNum[1] = 11;
	stpVelStepGroupTestConfig->uiNumPoints[1] = 2;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[1] = 0.10;
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[1] = 0.25;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[1] = //2000;
		astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[1] = astrWireBondServoAxisNameEn[1];

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[2] = MAX_CTRL_AXIS_PER_SERVO_BOARD;
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[2] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[2] = 3; // in count 2000;
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[2] = iBondHeadRelaxPosition - 1;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[2] = 2; // 20111223 iBondHeadRelaxPosition + 3; // in count 8000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[2] = 0;
	stpVelStepGroupTestConfig->uiLoopNum[2] = 11;
	stpVelStepGroupTestConfig->uiNumPoints[2] = 0;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[2] = 0.01;  // 20120717
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[2] = 0.2;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[2] = //1000;
		astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[2] = astrWireBondServoAxisNameEn[2];
}

void aft_vel_loop_test_station_xy_init_HoriLED_cfg(AFT_GROUP_VEL_STEP_TEST_CONFIG *stpVelStepGroupTestConfig)
{
	int ii;
	for(ii = 0; ii<MAX_NUM_AXIS_GROUP_VEL_STEP_TEST; ii++)
	{
		stpVelStepGroupTestConfig->uiAxis_CtrlCardId[ii] = MAX_CTRL_AXIS_PER_SERVO_BOARD;
	}

	stpVelStepGroupTestConfig->usActualNumAxis = 2;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_TABLE_X] = 10.0;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_TABLE_Y] = 16.0;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_BOND_Z] = 10.9;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_WIRE_CLAMP] = 2.0;

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[0] = APP_Y_TABLE_ACS_ID;
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[0] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[0] = 8; // in count 2500; 
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[0] = -20; // in count -40000;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[0] = 20; // in count 40000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[0] = 1;
	stpVelStepGroupTestConfig->uiLoopNum[0] = 11;
	stpVelStepGroupTestConfig->uiNumPoints[0] = 2; // 20;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[0] = 0.15;
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[0] = 0.25;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[0] = //2000;
		astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[0] = astrWireBondServoAxisNameEn[0];

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[1] = APP_X_TABLE_ACS_ID;
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[1] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[1] = 8; // in count 20000;
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[1] = -28; // in count -56000;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[1] = 28; // in count 24000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[1] = 1;
	stpVelStepGroupTestConfig->uiLoopNum[1] = 11;
	stpVelStepGroupTestConfig->uiNumPoints[1] = 2;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[1] = 0.15;
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[1] = 0.25;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[1] = //2000;
		astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[1] = astrWireBondServoAxisNameEn[1];

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[2] = MAX_CTRL_AXIS_PER_SERVO_BOARD;
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[2] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[2] = 3; // in count 2000;
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[2] = iBondHeadRelaxPosition - 1;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[2] = 2; // 20111223 // iBondHeadRelaxPosition + 3; // in count 8000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[2] = 0;
	stpVelStepGroupTestConfig->uiLoopNum[2] = 11;
	stpVelStepGroupTestConfig->uiNumPoints[2] = 0;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[2] = 0.05;  // 20120717
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[2] = 0.2;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[2] = //1000;
		astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[2] = astrWireBondServoAxisNameEn[2];
}

// Only Testing BH, all axis name is Bnd.Z, 20111222
void aft_vel_loop_test_station_bh_init_default_cfg(AFT_GROUP_VEL_STEP_TEST_CONFIG *stpVelStepGroupTestConfig)
{
	int ii;
	for(ii = 0; ii<MAX_NUM_AXIS_GROUP_VEL_STEP_TEST; ii++)
	{
		stpVelStepGroupTestConfig->uiAxis_CtrlCardId[ii] = MAX_CTRL_AXIS_PER_SERVO_BOARD;
	}

	stpVelStepGroupTestConfig->usActualNumAxis = 2;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_TABLE_X] = 10.0;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_TABLE_Y] = 10.0;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_BOND_Z] = 10.9;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_WIRE_CLAMP] = 2.0;

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[0] = MAX_CTRL_AXIS_PER_SERVO_BOARD;
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[0] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[0] = 8; // in count 2500; 
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[0] = 5; // in count 2000;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[0] = 35; // in count 24000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[0] = 0;
	stpVelStepGroupTestConfig->uiLoopNum[0] = 11;
	stpVelStepGroupTestConfig->uiNumPoints[0] = 0; // 20;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[0] = 0.15;
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[0] = 0.25;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[0] = //1000;						// 20111222
		astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[0] = astrWireBondServoAxisNameEn[2];  // 20111222

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[1] = MAX_CTRL_AXIS_PER_SERVO_BOARD;
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[1] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[1] = 8; // in count 20000;
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[1] = -10; // in count -20000;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[1] = 20; // in count 40000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[1] = 0;
	stpVelStepGroupTestConfig->uiLoopNum[1] = 11;
	stpVelStepGroupTestConfig->uiNumPoints[1] = 0;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[1] = 0.10;
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[1] = 0.25;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[1] = //2000;
		astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[1] = astrWireBondServoAxisNameEn[1];

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[2] = sys_get_acs_axis_id_bnd_z();
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[2] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[2] = 3; // in count 2000;
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[2] = iBondHeadRelaxPosition - 2;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[2] = 2; // 20111223iBondHeadRelaxPosition + 3; // in count 8000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[2] = 1;
	stpVelStepGroupTestConfig->uiLoopNum[2] = 11;
	stpVelStepGroupTestConfig->uiNumPoints[2] = 2;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[2] = 0.01;  // 20120717
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[2] = 0.2;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[2] = //1000;
		astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[2] = astrWireBondServoAxisNameEn[2];
}

void aft_vel_loop_test_hori_bonder_init_default_cfg(AFT_GROUP_VEL_STEP_TEST_CONFIG *stpVelStepGroupTestConfig)
{
	int ii;
	for(ii = 0; ii<MAX_NUM_AXIS_GROUP_VEL_STEP_TEST; ii++)
	{
		stpVelStepGroupTestConfig->uiAxis_CtrlCardId[ii] = MAX_CTRL_AXIS_PER_SERVO_BOARD;
	}

	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_TABLE_X] = 10.0;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_TABLE_Y] = 10.0;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_BOND_Z] = 10.9;
	stpVelStepGroupTestConfig->afPeakDriverCurrent[WB_AXIS_WIRE_CLAMP] = 2.0;
	stpVelStepGroupTestConfig->usActualNumAxis = 3;

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[0] = APP_Y_TABLE_ACS_ID;
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[0] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[0] = 8; // in count 20000; 
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[0] = -18; // 69; // in count -138000;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[0] = 18;  // 69; // in count 138000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[0] = 1;
	stpVelStepGroupTestConfig->uiLoopNum[0] = 11;
	stpVelStepGroupTestConfig->uiNumPoints[0] = 3; // 20;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[0] = 0.15;
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[0] = 0.25;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[0] = //2000;
		astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[0] = astrWireBondServoAxisNameEn[0];

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[1] = APP_X_TABLE_ACS_ID;
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[1] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[1] = 8; // in count 20000;
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[1] = -20; // in count -40000;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[1] = 20; // in count 40000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[1] = 1;
	stpVelStepGroupTestConfig->uiLoopNum[1] = 11;
	stpVelStepGroupTestConfig->uiNumPoints[1] = 3;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[1] = 0.15;
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[1] = 0.25;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[1] = //2000;
		astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[1] = astrWireBondServoAxisNameEn[1];

	stpVelStepGroupTestConfig->uiAxis_CtrlCardId[2] = sys_get_acs_axis_id_bnd_z();
	stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[2] = 0.05;
	stpVelStepGroupTestConfig->fVelStepTestDistance_mm[2] = 3; // in count 2000;
	stpVelStepGroupTestConfig->dLowerLimitPosition_mm[2] = iBondHeadRelaxPosition - 1;
	stpVelStepGroupTestConfig->dUpperLimitPosition_mm[2] = 2; // 20111223iBondHeadRelaxPosition + 3; // in count 8000;
	stpVelStepGroupTestConfig->ucFlagIsExciteAxis[2] = 1;
	stpVelStepGroupTestConfig->uiLoopNum[2] = 10;
	stpVelStepGroupTestConfig->uiNumPoints[2] = 2;
	stpVelStepGroupTestConfig->fMaxVelocityPercent[2] = 0.05;  // 20120717
	stpVelStepGroupTestConfig->fDriveCmdRatioRMS[2] = 0.2;
	stpVelStepGroupTestConfig->fEncResolutionCntPerMM[2] = //1000;
		astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm;
	stpVelStepGroupTestConfig->strServoAxisNameWb[2] = astrWireBondServoAxisNameEn[2];
}

void aft_convert_group_vel_step_cfg_to_test_input(AFT_GROUP_VEL_STEP_TEST_CONFIG *stpVelStepGroupTestConfig, 
											  AFT_GROUP_VEL_STEP_TEST_INPUT* stpVelStepGroupTestCase,
											  unsigned int *uiTotalTestCases)
{
static	unsigned int uiAxis_CtrlCardId[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
static	int iLowerLimitPosition[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
static	int iUpperLimitPosition[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
static	unsigned int uiNumPoints[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
static	unsigned char ucFlagIsExciteAxis[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
static	double fVelStepTestDistance[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
static	double fMaxVelocityPercent[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
static	int uiLoopNum[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
static	double fDriveCmdRatioRMS[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
static	double fEncResolutionCntPerMM[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
static	double fPercentErrorThresholdSettleTime[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
static int iCurrAxisVelStepTestDistance[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
int ii, jj, iCase;
	for(ii = 0; ii< MAX_NUM_AXIS_GROUP_VEL_STEP_TEST; ii++)
	{
		// 
		uiAxis_CtrlCardId[ii] = stpVelStepGroupTestConfig->uiAxis_CtrlCardId[ii];
		fEncResolutionCntPerMM[ii] = stpVelStepGroupTestConfig->fEncResolutionCntPerMM[ii];
		//
		iLowerLimitPosition[ii] = (int)(stpVelStepGroupTestConfig->dLowerLimitPosition_mm[ii] * fEncResolutionCntPerMM[ii]);
		iUpperLimitPosition[ii] = (int)(stpVelStepGroupTestConfig->dUpperLimitPosition_mm[ii] * fEncResolutionCntPerMM[ii]);
		fVelStepTestDistance[ii] = stpVelStepGroupTestConfig->fVelStepTestDistance_mm[ii] * fEncResolutionCntPerMM[ii];
		// 
		ucFlagIsExciteAxis[ii] = stpVelStepGroupTestConfig->ucFlagIsExciteAxis[ii];
		uiNumPoints [ii] = stpVelStepGroupTestConfig->uiNumPoints[ii];
		fMaxVelocityPercent[ii] = stpVelStepGroupTestConfig->fMaxVelocityPercent[ii];
		uiLoopNum[ii] = stpVelStepGroupTestConfig->uiLoopNum[ii];
		fDriveCmdRatioRMS[ii] = stpVelStepGroupTestConfig->fDriveCmdRatioRMS[ii];
		fPercentErrorThresholdSettleTime[ii] = stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[ii];
	}

	static unsigned short nTotalEnvolvedAxis, nTotalExciteAxis, nTotalNumGroupPoints;
	static unsigned short nTotalNumCasesGroupVelStepTest;
	nTotalEnvolvedAxis = 0; nTotalExciteAxis= 0; nTotalNumCasesGroupVelStepTest = 1; nTotalNumGroupPoints = 1;
	// spanning for nTotalEnvolvedAxis
	static int aiExciteAxisListInGroup[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST], aiEnvolvedAxisList[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST], aiNumPointsActiveAxis[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
	static double dDeltaDistance[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST], iCurrTestPosn[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];
	static int iCurrLowLimitPosn[MAX_NUM_AXIS_GROUP_VEL_STEP_TEST];   // 20110722
	int iCurrAxis, iCurrPointIdInAxis;
	iCurrAxis = MAX_CTRL_AXIS_PER_SERVO_BOARD; // dummy
	for(ii = 0; ii< MAX_NUM_AXIS_GROUP_VEL_STEP_TEST; ii++)
	{
		aiEnvolvedAxisList[ii] = MAX_CTRL_AXIS_PER_SERVO_BOARD;
	}
	for(ii = 0; ii< MAX_NUM_AXIS_GROUP_VEL_STEP_TEST; ii++)
	{
			// init
		dDeltaDistance[ii] = 0;
		if(uiAxis_CtrlCardId[ii] < MAX_CTRL_AXIS_PER_SERVO_BOARD && uiNumPoints[ii] > 0)
		{
			nTotalNumGroupPoints = nTotalNumGroupPoints * uiNumPoints[ii];

			aiEnvolvedAxisList[nTotalEnvolvedAxis] = uiAxis_CtrlCardId[ii];
			iCurrTestPosn[nTotalEnvolvedAxis] = iLowerLimitPosition[ii] + (int)(fVelStepTestDistance[ii]);
			aiNumPointsActiveAxis[nTotalEnvolvedAxis] = uiNumPoints[ii];
			if(uiNumPoints[ii] > 1)
			{
				dDeltaDistance[nTotalEnvolvedAxis] = (iUpperLimitPosition[ii] - iLowerLimitPosition[ii] - fVelStepTestDistance[ii] *2)/ (uiNumPoints[ii] - 1.0);
			}
			else
			{
				dDeltaDistance[nTotalEnvolvedAxis] = (iUpperLimitPosition[ii] - iLowerLimitPosition[ii])/2.0;
			}
			iCurrLowLimitPosn[nTotalEnvolvedAxis] = iLowerLimitPosition[ii];  // 20110722
			iCurrAxisVelStepTestDistance[nTotalEnvolvedAxis] =  (int)(fVelStepTestDistance[ii]); // 20110722

			nTotalEnvolvedAxis ++;

			if(iCurrAxis == MAX_CTRL_AXIS_PER_SERVO_BOARD)
			{
				iCurrAxis = uiAxis_CtrlCardId[ii];

			}
		}
		if(ucFlagIsExciteAxis[ii] >0 && uiAxis_CtrlCardId[ii] < MAX_CTRL_AXIS_PER_SERVO_BOARD)
		{
			aiExciteAxisListInGroup[nTotalExciteAxis] = ii; // uiAxis_CtrlCardId[ii]
			nTotalExciteAxis = nTotalExciteAxis + 1;
		}
	}
	stpVelStepGroupTestConfig->usActualNumAxis = nTotalEnvolvedAxis;
	nTotalNumCasesGroupVelStepTest = nTotalExciteAxis * nTotalNumGroupPoints;

	*uiTotalTestCases = nTotalNumCasesGroupVelStepTest;

	iCase= 0;
	for(ii = 0; ii<nTotalNumCasesGroupVelStepTest; ii++)
	{
		for(jj = 0; jj < nTotalEnvolvedAxis; jj++)
		{
			stpVelStepGroupTestCase[ii].uiAxis_CtrlCardId[jj] = aiEnvolvedAxisList[jj];
		}
		for(jj = nTotalEnvolvedAxis; jj < MAX_NUM_AXIS_GROUP_VEL_STEP_TEST; jj++)
		{
			stpVelStepGroupTestCase[ii].uiAxis_CtrlCardId[jj] = MAX_CTRL_AXIS_PER_SERVO_BOARD;
		}
	}

	iCurrPointIdInAxis = 0;
	static unsigned int iRemainder, iQuotient;
	int iExciteAxis;

	for(ii = 0; ii< nTotalNumGroupPoints; ii ++)
	{

		// spanning nTotalEnvolvedAxis- dimensional space by Quotient and Remainder 
		iQuotient = ii;
		for(jj = 0; jj < nTotalEnvolvedAxis; jj ++)
		{
			iRemainder = iQuotient % aiNumPointsActiveAxis[jj];
			iCurrTestPosn[jj] = (int)(iRemainder * dDeltaDistance[jj] + iCurrLowLimitPosn[jj] + iCurrAxisVelStepTestDistance[jj]); 
			// fVelStepTestDistance[jj]);  // BUG if using iLowerLimitPosition 

			iQuotient = iQuotient / aiNumPointsActiveAxis[jj];
		}

		for(jj = 0; jj < nTotalExciteAxis; jj++)
		{
			stpVelStepGroupTestCase[iCase].usActualNumAxis = nTotalEnvolvedAxis;
			iExciteAxis = aiExciteAxisListInGroup[jj];
			stpVelStepGroupTestCase[iCase].stVelStepTestOnceInput.iAxisCtrlCard = uiAxis_CtrlCardId[iExciteAxis];
			stpVelStepGroupTestCase[iCase].stVelStepTestOnceInput.fDriveCmdRatioRMS = stpVelStepGroupTestConfig->fDriveCmdRatioRMS[iExciteAxis];
			stpVelStepGroupTestCase[iCase].stVelStepTestOnceInput.fEncResolutionCntPerMM = stpVelStepGroupTestConfig->fEncResolutionCntPerMM[iExciteAxis];
			stpVelStepGroupTestCase[iCase].stVelStepTestOnceInput.fMaxAbsDAC = 32767; // stpVelStepGroupTestConfig->[iExciteAxis];
			stpVelStepGroupTestCase[iCase].fMaxVelocityPercent = stpVelStepGroupTestConfig->fMaxVelocityPercent[iExciteAxis];
			stpVelStepGroupTestCase[iCase].stVelStepTestOnceInput.fPercentErrorThresholdSettleTime = stpVelStepGroupTestConfig->fPercentErrorThresholdSettleTime[iExciteAxis];
			stpVelStepGroupTestCase[iCase].stVelStepTestOnceInput.uiLoopNum = stpVelStepGroupTestConfig->uiLoopNum[iExciteAxis];
			stpVelStepGroupTestCase[iCase].stVelStepTestOnceInput.strServoAxisNameWb = stpVelStepGroupTestConfig->strServoAxisNameWb[iExciteAxis];
			stpVelStepGroupTestCase[iCase].stVelStepTestOnceInput.iPositionLower = (int)iCurrTestPosn[iExciteAxis] - (int)((fVelStepTestDistance[iExciteAxis])/2);
			stpVelStepGroupTestCase[iCase].stVelStepTestOnceInput.iPositionUpper = (int)iCurrTestPosn[iExciteAxis] + (int)((fVelStepTestDistance[iExciteAxis])/2);
			for(int ll = 0; ll<nTotalEnvolvedAxis; ll++)
			{
				stpVelStepGroupTestCase[iCase].iAxisPosn[ll] = (int)(iCurrTestPosn[ll]);
			}
			iCase ++;
		}

	}

}

