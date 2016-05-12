
#include "stdafx.h"
#include "mtndefin.h"

int brinv(double a[],int n); // return -  1: OK, 0: error
// Matrix multiplication a * b = c, a: m by n, b: n by k, c: m by k
void brmul(double *a, double *b, int m, int n, int k, double *c);

RANGE_DRIVE_OUT astRangeDriveOut[MAX_DRIVE_OUT_RANGE_CASES];
char *strDriveRangeText_OLT[MAX_DRIVE_OUT_RANGE_CASES] = 
{
	"1",
	"2",
	"5",
	"10",
	"20",
	"50",
	"100"
};


///////// 
#define __TOTAL_POINTS_FORCE_RATIO_CALI__	5
#define MATRIX_COL_2						2

int aiForceRatioCalibrationFbAdc[__TOTAL_POINTS_FORCE_RATIO_CALI__];
double adForceRatioCalibrationFbGram[__TOTAL_POINTS_FORCE_RATIO_CALI__];

double adMatrixSq[4]; // 2 by 2 matrix
double adRHS[2];      // 2
double adResult[2];

int mpi_calibrate_force_ratio(double *dRetForceRatio)
{
	int iRet = MTN_API_OK_ZERO;

	int ii;
	for(ii = 1; ii<__TOTAL_POINTS_FORCE_RATIO_CALI__; ii++)
	{
		if( aiForceRatioCalibrationFbAdc[ii] < aiForceRatioCalibrationFbAdc[ii - 1])
		{
			aiForceRatioCalibrationFbAdc[ii] = aiForceRatioCalibrationFbAdc[ii - 1];
			adForceRatioCalibrationFbGram[ii] = adForceRatioCalibrationFbGram[ii - 1];

			iRet = MTN_CALC_FORCE_SENSOR_ERROR;

		}
	}

	for(ii = 0; ii<4; ii++)
	{
		adMatrixSq[ii] = 0;	
	}
	for(ii =0; ii<2; ii++)
	{
		adRHS[ii] = 0;
	}
	for(ii = 0; ii<__TOTAL_POINTS_FORCE_RATIO_CALI__; ii++)
	{
		adMatrixSq[0 * MATRIX_COL_2 + 0] += aiForceRatioCalibrationFbAdc[ii] * aiForceRatioCalibrationFbAdc[ii];
		adMatrixSq[0 * MATRIX_COL_2 + 1] += aiForceRatioCalibrationFbAdc[ii];
		adRHS[0] += adForceRatioCalibrationFbGram[ii] * aiForceRatioCalibrationFbAdc[ii];
		adRHS[1] += adForceRatioCalibrationFbGram[ii];
	}
	adMatrixSq[1 * MATRIX_COL_2 + 0] = adMatrixSq[0 * MATRIX_COL_2 + 1];
	adMatrixSq[1 * MATRIX_COL_2 + 1] = __TOTAL_POINTS_FORCE_RATIO_CALI__;

	adResult[0] = 0; // Initialization
	if(brinv(adMatrixSq, MATRIX_COL_2) == 0)
	{
		adResult[1] = 0;
		iRet = MTN_CALC_ERROR;
	}
	else
	{
		brmul(adMatrixSq, adRHS, MATRIX_COL_2, MATRIX_COL_2, 1, adResult);
	}
	*dRetForceRatio = adResult[0]; // mtn_get_force_ratio()

	return iRet;
}

#include "acsc.h"
#include "math.h"
int mtnapi_enable_motor(HANDLE Handle, int Axis, int iDebug);
int mtnapi_disable_motor(HANDLE Handle, int Axis, int iDebug);
double mtn_wb_init_bh_relax_position_from_sp(HANDLE Handle);
int mtnapi_get_fb_position(HANDLE Handle, int Axis, double* FPosition, int iDebug);

#define  AND_CLEAR_BIT_1    0xFFFFFFFD
#define  OR_SET_BIT_1    0x2
#define  AND_CLEAR_BIT_17    0xFFFDFFFF
#define  OR_SET_BIT_17    0x20000

void mtn_acs_convert_from_current_to_position_loop(HANDLE m_hHandle, int iAxisACS)
{
static int iMotorFlags;
		acsc_ReadInteger(m_hHandle, 0, "MFLAGS", 	iAxisACS, iAxisACS, 0, 0, &iMotorFlags, 0); // DOUT
		iMotorFlags = iMotorFlags & AND_CLEAR_BIT_1;
		iMotorFlags = iMotorFlags | OR_SET_BIT_17;
		acsc_WriteInteger(m_hHandle, 0, "MFLAGS", iAxisACS, iAxisACS, 0, 0, &iMotorFlags, 0); // DOUT
		mtnapi_disable_motor(m_hHandle, iAxisACS, 0);

		mtnapi_enable_motor(m_hHandle, iAxisACS, 0);

		Sleep(200);

int iAxisSafePosition;
		iAxisSafePosition = (int)mtn_wb_init_bh_relax_position_from_sp(m_hHandle);
		acsc_ToPoint(m_hHandle, 0, // start up immediately the motion
		iAxisACS, iAxisSafePosition, NULL);

}

///////// Routine of Checking polarity of servo-loop,
/// positive current -> positive force -> needs to give position respons in encoder
int mtn_acs_routine_check_axis_force_feedback_direction(HANDLE stCommHandleACS, int iAcsAxis)
{
int iMotorFlags;
int iRet;
static int iDebug = 0;
//int iMotorState;
	// Set to current loop 
//	mtnapi_get_motor_state(stCommHandleACS, astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, 
//		&iMotorState, iDebug);
		acsc_ReadInteger(stCommHandleACS, 0, "MFLAGS", 
			iAcsAxis,iAcsAxis, 0, 0, &iMotorFlags, 0); // DOUT
		//	OpenLoop DAC mode, MFLAGS(iAFT_Axis).1=1; MFLAGS(iAFT_Axis).17=0;\n",
			// now is disabled, to enable it
			iMotorFlags = iMotorFlags | OR_SET_BIT_1;
			iMotorFlags = iMotorFlags & AND_CLEAR_BIT_17;
		acsc_WriteInteger(stCommHandleACS, 0, "MFLAGS", 
			iAcsAxis,iAcsAxis, 0, 0, &iMotorFlags, 0); // DOUT

		Sleep(50);
		iRet = mtnapi_enable_motor(stCommHandleACS, iAcsAxis, iDebug);
		// enable driver
#define __TOTAL_INCREAMENT_CHECK_POLARITY__  10
#define __TOLERANCE_POSITION_CHANGE_COUNT__  100
#define __TOLERANCE_POSITION_CHANGE_COUNT_TABLE__  500

double dTolerancePositionChange;
double dFbPosition, dFbNewPosition;
//double dFbPositionIncreament[__TOTAL_INCREAMENT_CHECK_POLARITY__];
double dDrvCmdIncrement, dCurrDrvCmd;	
int ii, jj;

	if(iAcsAxis == 0 || iAcsAxis == 1) // Table using larger tolerance
	{
		dTolerancePositionChange = __TOLERANCE_POSITION_CHANGE_COUNT_TABLE__;
		dDrvCmdIncrement = 15.0 / __TOTAL_INCREAMENT_CHECK_POLARITY__;
	}
	else
	{
		dTolerancePositionChange = __TOLERANCE_POSITION_CHANGE_COUNT__;
		dDrvCmdIncrement = 10.0 / __TOTAL_INCREAMENT_CHECK_POLARITY__;
	}
	dCurrDrvCmd = 0.1;  // Initial Force

	mtnapi_get_fb_position(stCommHandleACS, iAcsAxis, &dFbPosition, 0);
	for(ii = 0; ii < __TOTAL_INCREAMENT_CHECK_POLARITY__ - 1; ii++)
	{
		acsc_WriteReal(stCommHandleACS, 0, "DCOM", 	iAcsAxis,iAcsAxis, 0, 0, &dCurrDrvCmd, 0); // DOUT
		Sleep(50);
		for(jj = 0; jj<20; jj++)
		{
			mtnapi_get_fb_position(stCommHandleACS, iAcsAxis, &dFbNewPosition, 0);
			if(fabs(dFbNewPosition - dFbPosition) > dTolerancePositionChange)
			{
				goto label_check_polarity_position_has_movement;
			}
			Sleep(50);
		}
		mtnapi_get_fb_position(stCommHandleACS, iAcsAxis, &dFbNewPosition, 0);
		if(fabs(dFbNewPosition - dFbPosition) > dTolerancePositionChange)
		{
			goto label_check_polarity_position_has_movement;
		}
		else
		{
			dCurrDrvCmd = dCurrDrvCmd + dDrvCmdIncrement;
		}
	}
label_check_polarity_position_has_movement:
	dCurrDrvCmd = 0;
	acsc_WriteReal(stCommHandleACS, 0, "DCOM", 	iAcsAxis,iAcsAxis, 0, 0, &dCurrDrvCmd, 0); // DOUT
		// try 3 times
		//    record current feedback position
		//    ramp output upto 5% DAC, for 20 ms, record feedback position increament,
		//    output 0 DAC,
		//    
		//    ramp output upto -5% DAC, for 20 ms, record feedback position increament, (should be -)
		//    output 0 DAC
		//    

		// disable driver
		acsc_ReadInteger(stCommHandleACS, 0, "MFLAGS", 	iAcsAxis,iAcsAxis, 0, 0, &iMotorFlags, 0); // DOUT
		iMotorFlags = iMotorFlags & AND_CLEAR_BIT_1;
		iMotorFlags = iMotorFlags | OR_SET_BIT_17;
		acsc_WriteInteger(stCommHandleACS, 0, "MFLAGS", iAcsAxis,iAcsAxis, 0, 0, &iMotorFlags, 0); // DOUT
		mtnapi_disable_motor(stCommHandleACS, iAcsAxis, iDebug);

	if(dFbNewPosition >= dFbPosition + dTolerancePositionChange)
	{
		iRet = MTN_API_OK_ZERO;
	}
	else if(dFbNewPosition <= dFbPosition - dTolerancePositionChange)
	{
		iRet = MTN_API_ERROR_WRONG_POLARITY;
	}
	else
	{
		iRet = MTN_API_ERROR_DRIVER_NO_FORCE;
	}

	return iRet;
}

char *pstrDetectionSwitch[] =
{
	"NONE",
	"DOUT",
	"PE",
	"AIN"
};
char *pstrSearchVelSelection[] =
{
	"0.1",
	"0.2",
	"0.3",
	"0.4",
	"0.5",
	"0.6",
	"0.7",
	"0.8",
	"0.9",
	"1",
	"1.1",
	"1.2",
	"1.3",
	"1.4",
	"1.5",
	"1.6",
	"1.7",
	"1.8",
	"1.9",
	"2.0",
	"2.1",
	"2.2",
	"2.3",
	"2.4",
	"2.5",
	"2.6",
	"2.7",
	"2.8",
	"2.9",
	"3.0",
	"3.1",
	"3.2",
	"3.3",
	"3.4",
	"3.5",
	"3.6",
	"3.7",
	"3.8",
	"3.9",
	"4",
	"4.1",
	"4.2",
	"4.3",
	"4.4",
	"4.5",
	"4.6",
	"4.7",
	"4.8",
	"4.9",
	"5",
	"5.1",
	"5.2",
	"5.3",
	"5.4",
	"5.5",
	"5.6",
	"5.7",
	"5.8",
	"5.9",
	"6"
};
char *pstrSearchTH_PE[] =
{
	"6",
	"12",
	"18",
	"24",
	"30",
	"36",
	"42",
	"48",
	"54",
	"60",
	"66",
	"72",
	"78"
};

char *pstrSwitchDamp[] =
{
	"5",
	"10",
	"15",
	"20",
	"25",
	"30",
	"35",
	"40",
	"45",
	"50",
	"55",
	"60",
	"65",
	"70"
};

char *pstrPreImpactForce[] =
{
	"0",
	"20",
	"40",
	"60",
	"80",
	"100",
	"120",
	"140",
	"160",
	"180",
	"200"
};

#include "stdio.h"
extern MTN_SCOPE gstSystemScope;
extern double gdScopeCollectData[];

void mtn_debug_print_to_file(FILE *fpData, SEARCH_CONTACT_AND_FORCE_CONTROL_INPUT *stpSearchContactAndForceControl, TEACH_CONTACT_OUTPUT *stpTeachContactResult)
{

	unsigned int ii, jj;
	FORCE_BLOCK_ACS *stpForceControlBlk;
	stpForceControlBlk = stpSearchContactAndForceControl->stpForceBlkPara;

	fprintf(fpData, "iCountactPositionReg = %d;\n", stpTeachContactResult->iContactPosnReg);
	fprintf(fpData, "fInitForceCommandReadBack = %f;\n", stpTeachContactResult->dInitForceCommandReadBack);

	fprintf(fpData, "SearchHeight_um = %d; \n", (stpSearchContactAndForceControl->stpTeachContactPara->iSearchHeightPosition - stpTeachContactResult->iContactPosnReg));
	fprintf(fpData, "SearchSpd = %d; \n", stpSearchContactAndForceControl->stpTeachContactPara->iSearchVel);
	fprintf(fpData, "DetectionThPE = %d; \n", stpSearchContactAndForceControl->stpTeachContactPara->iSearchTolPE);
	fprintf(fpData, "DetectionFlag = %d; %% %s \n", 
		stpSearchContactAndForceControl->stpTeachContactPara->cFlagDetectionSP, 
		pstrDetectionSwitch[stpSearchContactAndForceControl->stpTeachContactPara->cFlagDetectionSP]);
	fprintf(fpData, "%% Force Command Block Information\n");
	fprintf(fpData, "nNumSeg = %d;\n", stpForceControlBlk->uiNumSegment);
	fprintf(fpData, "aRampCountSeg = [");
	for(ii = 0; ii <stpForceControlBlk->uiNumSegment; ii++)
	{
		if(ii < stpForceControlBlk->uiNumSegment - 1)
		{
			fprintf(fpData, "%d, ", stpForceControlBlk->aiForceBlk_Rampcount[ii]);
		}
		else
		{
			fprintf(fpData, "%d];\n", stpForceControlBlk->aiForceBlk_Rampcount[ii]);
		}
	}
	fprintf(fpData, "aLevelCountSeg = [");
	for(ii = 0; ii <stpForceControlBlk->uiNumSegment; ii++)
	{
		if(ii < stpForceControlBlk->uiNumSegment - 1)
		{
			fprintf(fpData, "%d, ", stpForceControlBlk->aiForceBlk_LevelCount[ii]);
		}
		else
		{
			fprintf(fpData, "%d];\n", stpForceControlBlk->aiForceBlk_LevelCount[ii]);
		}
	}
	fprintf(fpData, "afLevelAmplitude = [");
	for(ii = 0; ii <stpForceControlBlk->uiNumSegment; ii++)
	{
		if(ii < stpForceControlBlk->uiNumSegment - 1)
		{
			fprintf(fpData, "%8.4f, ", stpForceControlBlk->adForceBlk_LevelAmplitude[ii]);
		}
		else
		{
			fprintf(fpData, "%8.4f];\n", stpForceControlBlk->adForceBlk_LevelAmplitude[ii]);
		}
	}
	fprintf(fpData, "%% ACSC Controller, Axis- %d: FPOS, RPOS, PE, iDebCounter, ForceCmd, ForceFb, \n aContactForceControlData = [    ", 
		stpSearchContactAndForceControl->stpTeachContactPara->iAxis);
	for(ii = 0; ii< gstSystemScope.uiDataLen; ii++)
	{

		for(jj = 0; jj< gstSystemScope.uiNumData; jj++)
		{
			if(jj == gstSystemScope.uiNumData - 1)
			{	
				fprintf(fpData, "%8.4f", gdScopeCollectData[jj* gstSystemScope.uiDataLen + ii]);
			}
			else
			{
				fprintf(fpData, "%8.4f,  ", gdScopeCollectData[jj* gstSystemScope.uiDataLen + ii]);
			}
		}
		fprintf(fpData, "\n");
	}
	fprintf(fpData, "];\n");

}
