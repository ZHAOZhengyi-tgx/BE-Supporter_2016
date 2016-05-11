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


// 20100706 John 
// 20100726 Add protection of analyzing waveform, has wire or NOT
// 20101002, BH makes difference in idle and motion, Tuning ripple section

#include "stdafx.h"

#include "MtnTune.h"

#include "math.h"
#include "MtnInitAcs.h"

#include "MtnWbDef.h"
extern COMM_SETTINGS stServoControllerCommSet;
extern AXIS_INFO_WIRE_BOND astAxisInfoWireBond[MAX_SERVO_AXIS_WIREBOND];


WB_ONE_WIRE_PERFORMANCE_CALC astWbOneWirePerformance[NUM_TOTAL_WIRE_IN_ONE_SCOPE]; // 20110529

char *astrWireBondServoAxisNameEn[] =
{
	"Table.X",
	"Table.Y",
	"Bond.Z",
	"WireClamp"
};
char *astrWireBondServoAxisNameCn[] =
{
	"X-平台",
	"Y-平台",
	"焊头-Z",
	"线夹-W"
};

char *astrMachineTypeNameLabel_en[] = {
	"VLED-Magazine",
	"DualTrack-V",
	"WB13T",
	"WB13V",
	"Stn_XY_vert",
	"Stn_XY_top",
	"Stn_BH",
	"Stn_WireClamp",
	"Stn_EFO-BSD",
	"Stn_USG-LT",
	"18V",
	"20T",
	"UserDefine-1"
};

char *astrMachineTypeNameLabel_cn[] = {
	"垂直料盒",
	"双轨叉子",
	"平面",
	"13V",
	"测台_XY_垂直",
	"测台_XY_平面",
	"测台_BH",
	"测台_WireClamp",
	"测台打火检测",
	"测台超声照明",
	"18V",
	"20T",
	"UserDefine-1"
};

double afRatioRMS_DrvCmd[MAX_SERVO_AXIS_WIREBOND];
AXIS_INFO_WIRE_BOND astAxisInfoWireBond[MAX_SERVO_AXIS_WIREBOND];
void InitWireBond_XYZW_EncDrvMotorSpec()  // 20121203
{
	for(int ii=0; ii<MAX_SERVO_AXIS_WIREBOND; ii++)
	{
		astAxisInfoWireBond[ii].strAxisNameCn = astrWireBondServoAxisNameCn[ii];
		astAxisInfoWireBond[ii].strAxisNameEn = astrWireBondServoAxisNameEn[ii];
	}

	astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm = 2000;
	astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm = 2000;
	astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm = 1000;
	astAxisInfoWireBond[WB_AXIS_WIRE_CLAMP].afEncoderResolution_cnt_p_mm = 0;
	afRatioRMS_DrvCmd[WB_AXIS_TABLE_X] = 0.27;
	afRatioRMS_DrvCmd[WB_AXIS_TABLE_Y] = 0.25;
	afRatioRMS_DrvCmd[WB_AXIS_BOND_Z] = 0.25;
	afRatioRMS_DrvCmd[WB_AXIS_WIRE_CLAMP] = 0.3;
}

void InitWireBondServoAxisName_VerLED_ACS()
{
	astAxisInfoWireBond[WB_AXIS_TABLE_X].iAxisInCtrlCardACS = ACS_CARD_AXIS_X;
	astAxisInfoWireBond[WB_AXIS_TABLE_Y].iAxisInCtrlCardACS = ACS_CARD_AXIS_Y;
	astAxisInfoWireBond[WB_AXIS_BOND_Z].iAxisInCtrlCardACS = ACS_CARD_AXIS_A;
	astAxisInfoWireBond[WB_AXIS_WIRE_CLAMP].iAxisInCtrlCardACS = ACS_CARD_AXIS_B;

	InitWireBond_XYZW_EncDrvMotorSpec(); // 20121203
}

void InitWireBondServoAxisName_HoriLED_ACS()
{
	astAxisInfoWireBond[WB_AXIS_TABLE_X].iAxisInCtrlCardACS = ACS_CARD_AXIS_Y;
	astAxisInfoWireBond[WB_AXIS_TABLE_Y].iAxisInCtrlCardACS = ACS_CARD_AXIS_X;
	astAxisInfoWireBond[WB_AXIS_BOND_Z].iAxisInCtrlCardACS = ACS_CARD_AXIS_A;
	astAxisInfoWireBond[WB_AXIS_WIRE_CLAMP].iAxisInCtrlCardACS = ACS_CARD_AXIS_B;

	InitWireBond_XYZW_EncDrvMotorSpec(); // 20121203
}


void InitWireBondServoAxisName_VerLED_ACS_SC_UDI()
{
	astAxisInfoWireBond[WB_AXIS_TABLE_X].iAxisInCtrlCardACS = ACS_CARD_AXIS_X;
	astAxisInfoWireBond[WB_AXIS_TABLE_Y].iAxisInCtrlCardACS = ACS_CARD_AXIS_Y;
	astAxisInfoWireBond[WB_AXIS_BOND_Z].iAxisInCtrlCardACS = APP_Z_BOND_ACS_SC_UDI_ID;
	astAxisInfoWireBond[WB_AXIS_WIRE_CLAMP].iAxisInCtrlCardACS = APP_WIRE_CLAMP_ACS_SC_UDI_ID;

	InitWireBond_XYZW_EncDrvMotorSpec(); // 20121203
}

void InitWireBondServoAxisName_HoriLED_ACS_SC_UDI()
{
	astAxisInfoWireBond[WB_AXIS_TABLE_X].iAxisInCtrlCardACS = ACS_CARD_AXIS_Y;
	astAxisInfoWireBond[WB_AXIS_TABLE_Y].iAxisInCtrlCardACS = ACS_CARD_AXIS_X;
	astAxisInfoWireBond[WB_AXIS_BOND_Z].iAxisInCtrlCardACS = APP_Z_BOND_ACS_SC_UDI_ID;
	astAxisInfoWireBond[WB_AXIS_WIRE_CLAMP].iAxisInCtrlCardACS = APP_WIRE_CLAMP_ACS_SC_UDI_ID;

	InitWireBond_XYZW_EncDrvMotorSpec(); // 20121203
}

#include "MotAlgo_DLL.h"
void InitWireBondServoAxisName()
{
	int iTempMachCfg = get_sys_machine_type_flag();
	if(iTempMachCfg == WB_MACH_TYPE_VLED_FORK || iTempMachCfg == WB_MACH_TYPE_ONE_TRACK_13V_LED)  // machine type dependency Item-4.
	{
		InitWireBondServoAxisName_VerLED_ACS();
	}
	else if(iTempMachCfg == WB_MACH_TYPE_HORI_LED || iTempMachCfg == WB_STATION_XY_TOP || iTempMachCfg == BE_WB_HORI_20T_LED
		 || iTempMachCfg == BE_WB_ONE_TRACK_18V_LED)
	{
		InitWireBondServoAxisName_HoriLED_ACS();
	}
	else
	{
		InitWireBondServoAxisName_VerLED_ACS();
	}

	// Init Wb Performance Structure, 20110529
	for(int ii=0; ii<NUM_TOTAL_WIRE_IN_ONE_SCOPE; ii++)
	{
		astWbOneWirePerformance[ii].iFlagHasWireInfo = 0;
	}
}

void InitWireBondNameServo_ACS_SC_UDI_Axis()
{
	int iTempMachCfg = get_sys_machine_type_flag();
	if(iTempMachCfg == WB_MACH_TYPE_VLED_FORK || iTempMachCfg == WB_MACH_TYPE_ONE_TRACK_13V_LED)  // machine type dependency Item-4.
	{
		InitWireBondServoAxisName_VerLED_ACS_SC_UDI(); // SC_UDI
	}
	else if(iTempMachCfg == WB_MACH_TYPE_HORI_LED || iTempMachCfg == WB_STATION_XY_TOP || iTempMachCfg == BE_WB_HORI_20T_LED
		 || iTempMachCfg == BE_WB_ONE_TRACK_18V_LED)
	{
		InitWireBondServoAxisName_HoriLED_ACS_SC_UDI();
	}
	else
	{
		InitWireBondServoAxisName_VerLED_ACS_SC_UDI();
	}

	// Init Wb Performance Structure, 20110529
	for(int ii=0; ii<NUM_TOTAL_WIRE_IN_ONE_SCOPE; ii++)
	{
		astWbOneWirePerformance[ii].iFlagHasWireInfo = 0;
	}
}

char *astrTablePosnRange[] = {
"0U_1U   ",
"0U_N1U  ",
"1U_2U   ",
"N1U_N2U ",
"2U_3U   ",
"N2U_N3U ",
"3U_4U   ",
"N3U_N4U ",
"4U_5U   ",
"N4U_N5U ",
"5U_6U   ",
"N5U_N6U ",
"6U_7U   ",
"N6U_N7U ",
"7U_8U   ",
"N7U_N8U ",
"8U_9U   ",
"N8U_N9U ",
"9U_10U  ",
"N9U_N10U"};

unsigned int nNumSectorPosnRangeTableX = DEF_NUM_SECTOR_POSN_RANGE_TBL_X;
unsigned int nNumSectorPosnRangeTableY = DEF_NUM_SECTOR_POSN_RANGE_TBL_Y;
unsigned int nCurrentTuneSector;

unsigned int mtn_wb_tune_get_curr_tune_sector()
{
	return nCurrentTuneSector;
}

unsigned int mtn_wb_tune_get_max_sector_table_x()
{
	return nNumSectorPosnRangeTableX;
}
unsigned int mtn_wb_tune_get_max_sector_table_y()
{
	return nNumSectorPosnRangeTableY;
}
void mtn_wb_tune_set_max_sector_table_x(unsigned int uiMaxSector)
{
	if(uiMaxSector <= DEF_NUM_SECTOR_POSN_RANGE_TBL_X)
	{
		nNumSectorPosnRangeTableX = uiMaxSector;
	}
	else
	{
		nNumSectorPosnRangeTableX = DEF_NUM_SECTOR_POSN_RANGE_TBL_X;
	}
}

MTN_TUNE_GENETIC_INPUT stMtnTuneInputTableX_DRA;
MTN_TUNE_GENETIC_OUTPUT stMtnTuneOutputTableX_DRA;
MTN_TUNE_GENETIC_INPUT stMtnTuneInputTableY_DRA;
MTN_TUNE_GENETIC_OUTPUT stMtnTuneOutputTableY_DRA;

MTN_TUNE_GENETIC_INPUT astMtnTuneInput[MAX_SERVO_AXIS_WIREBOND][MAX_BLK_PARAMETER]; 
MTN_TUNE_GENETIC_OUTPUT astMtnTuneOutput[MAX_SERVO_AXIS_WIREBOND][MAX_BLK_PARAMETER];

MTN_TUNE_GENETIC_INPUT astMtnTuneSectorInputTableX[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];
MTN_TUNE_GENETIC_OUTPUT astMtnTuneSectorOutputTableX[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];
MTN_TUNE_GENETIC_INPUT astMtnTuneSectorInputTableY[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];
MTN_TUNE_GENETIC_OUTPUT astMtnTuneSectorOutputTableY[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];

//// Verification by check sum and IsFailFlag
double mtn_wb_checksum_para_b1w_out(MTN_TUNE_PARAMETER_SET *stpParsSetTuneOut)
{
	double dCheckSumWbB1W = 0;

	dCheckSumWbB1W += stpParsSetTuneOut->dAccFFC;
	dCheckSumWbB1W += stpParsSetTuneOut->dPosnLoopKP;
	dCheckSumWbB1W += stpParsSetTuneOut->dVelLoopKI;
	dCheckSumWbB1W += stpParsSetTuneOut->dVelLoopKP;

	return dCheckSumWbB1W;
}

#define EPSLON_VERIFY_TUNE_B1W_CHECKSUM_BY_FAILFLAG  (0.5)
int mtn_wb_verify_tune_out_b1w()
{
int iRet = MTN_API_OK_ZERO;
double dCheckSum = 0;

	//Move to Search Height 1stB
	dCheckSum = (1.0 - astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_SRCH_HT].iFlagTuningIsFail) 
		* mtn_wb_checksum_para_b1w_out(&astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_SRCH_HT].stBestParameterSet);	
	if(fabs(dCheckSum) < EPSLON_VERIFY_TUNE_B1W_CHECKSUM_BY_FAILFLAG)
	{
		iRet = MTN_CALC_ERROR;
	}

	//Jogging 1stB
	dCheckSum = (1.0 - astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_1ST_CONTACT].iFlagTuningIsFail) 
		* mtn_wb_checksum_para_b1w_out(&astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_1ST_CONTACT].stBestParameterSet);	
	if(fabs(dCheckSum) < EPSLON_VERIFY_TUNE_B1W_CHECKSUM_BY_FAILFLAG)
	{
		iRet = MTN_CALC_ERROR;
	}

	//Traj
	dCheckSum = (1.0 - astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_TRAJECTORY].iFlagTuningIsFail) 
		* mtn_wb_checksum_para_b1w_out(&astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_TRAJECTORY].stBestParameterSet);	
	if(fabs(dCheckSum) < EPSLON_VERIFY_TUNE_B1W_CHECKSUM_BY_FAILFLAG)
	{
		iRet = MTN_CALC_ERROR;
	}

	//Jogging 2ndB
	dCheckSum = (1.0 - astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_2ND_CONTACT].iFlagTuningIsFail) 
		* mtn_wb_checksum_para_b1w_out(&astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_2ND_CONTACT].stBestParameterSet);	
	if(fabs(dCheckSum) < EPSLON_VERIFY_TUNE_B1W_CHECKSUM_BY_FAILFLAG)
	{
		iRet = MTN_CALC_ERROR;
	}

	//Tail 
	dCheckSum = (1.0 - astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_TAIL].iFlagTuningIsFail) 
		* mtn_wb_checksum_para_b1w_out(&astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_TAIL].stBestParameterSet);	
	if(fabs(dCheckSum) < EPSLON_VERIFY_TUNE_B1W_CHECKSUM_BY_FAILFLAG)
	{
		iRet = MTN_CALC_ERROR;
	}

	//idle
	dCheckSum = (1.0 - astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_IDLE].iFlagTuningIsFail) 
		* mtn_wb_checksum_para_b1w_out(&astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_IDLE].stBestParameterSet);	
	if(fabs(dCheckSum) < EPSLON_VERIFY_TUNE_B1W_CHECKSUM_BY_FAILFLAG)
	{
		iRet = MTN_CALC_ERROR;
	}

	//reset
	dCheckSum = (1.0 - astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_RESET].iFlagTuningIsFail) 
		* mtn_wb_checksum_para_b1w_out(&astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_RESET].stBestParameterSet);	
	if(fabs(dCheckSum) < EPSLON_VERIFY_TUNE_B1W_CHECKSUM_BY_FAILFLAG)
	{
		iRet = MTN_CALC_ERROR;
	}
	return iRet;
}

double f_get_sign(double fIn)
{
	if(fIn >= 0)
	{
		return 1.0;
	}
	else // if(fIn < 0)
	{
		return -1.0;
	}
}

double f_get_max(double *afIn, int nLen)
{
	double dRet = afIn[0];

	for(int ii=1; ii<nLen; ii++)
	{
		if(dRet < afIn[ii])
		{
			dRet = afIn[ii];
		}
	}

	return dRet;
}

double f_get_abs_max(double *afIn, int nLen)
{
	double dRet = fabs(afIn[0]), dTemp;

	for(int ii=1; ii<nLen; ii++)
	{
		dTemp = fabs(afIn[ii]);
		if(dRet < dTemp)
		{
			dRet = dTemp;
		}
	}

	return dRet;
}

double f_get_rms_max_err_w_offset(double *afIn, int nLen, double dOffset)
{
	double dRet = 0, dTemp;
	double dSumSq = 0;

	for(int ii=0; ii<nLen; ii++)
	{
		dSumSq = dSumSq + (afIn[ii] - dOffset)*(afIn[ii] - dOffset);
	}
	dTemp = dSumSq/nLen;
	dRet = sqrt(dTemp);

	return dRet;
}

double f_get_min(double *afIn, int nLen)
{
	double dRet = afIn[0];

	for(int ii=1; ii<nLen; ii++)
	{
		if(dRet > afIn[ii])
		{
			dRet = afIn[ii];
		}
	}
	return dRet;
}

double f_get_mean(double *afIn, int nLen)
{
	double dRet = afIn[0];
	for(int ii=1; ii<nLen; ii++)
	{
		dRet = dRet + afIn[ii];
	}
	dRet = dRet/nLen;
	return dRet;
}

double f_get_std(double *afIn, double fMean, int nLen)
{
	double dRet = (afIn[0] - fMean);
	dRet = dRet * dRet;
	for(int ii=1; ii<nLen; ii++)
	{
		dRet = dRet + (afIn[ii] - fMean) * (afIn[ii] - fMean);
	}
	dRet = sqrt(dRet/nLen);

	return dRet;
}
static double dControlSampleTimeMPU = 0.5;
void mtn_tune_calc_vaj_from_posn_waveform();
extern int mtn_tune_get_wb_waveform_ena_flag();
static double fRefPosnX[LEN_UPLOAD_ARRAY];
static double fRefPosnY[LEN_UPLOAD_ARRAY];
static double fFeedPosnX[LEN_UPLOAD_ARRAY];
static double fFeedPosnY[LEN_UPLOAD_ARRAY];
static double fFeedPosnZ[LEN_UPLOAD_ARRAY];

double fRefPosnZ[LEN_UPLOAD_ARRAY];
double fRefVelZ[LEN_UPLOAD_ARRAY];
double fFeedVelZ[LEN_UPLOAD_ARRAY];
double fPosnErrZ[LEN_UPLOAD_ARRAY];

static double fRefVelX[LEN_UPLOAD_ARRAY];
static double fRefVelY[LEN_UPLOAD_ARRAY];
static double fFeedVelX[LEN_UPLOAD_ARRAY];
static double fFeedVelY[LEN_UPLOAD_ARRAY];
static double fPosnErrX[LEN_UPLOAD_ARRAY];
static double fPosnErrY[LEN_UPLOAD_ARRAY];

static double fFeedAccZ[LEN_UPLOAD_ARRAY];  // 20110520

static double fRefAccX[LEN_UPLOAD_ARRAY];
static double fRefAccY[LEN_UPLOAD_ARRAY];
static double fRefAccZ[LEN_UPLOAD_ARRAY];

static double fRefJerkX[LEN_UPLOAD_ARRAY];
static double fRefJerkY[LEN_UPLOAD_ARRAY];
static double fRefJerkZ[LEN_UPLOAD_ARRAY];

static double fMotorFlagZ[LEN_UPLOAD_ARRAY];
static int iMotorFlagZ[LEN_UPLOAD_ARRAY];
static double fWireClampCmd[LEN_UPLOAD_ARRAY];

void mtn_tune_get_waveform_data_ptr(double **pdRefPosnX, double **pdFeedPosnX, double **pdRefPosnY, double **pdFeedPosnY, 
									double **pdRefPosnZ, double **pdFeedPosnZ, double **pdWireClampCmd, double **pdMotorFlagZ)
{
	*pdRefPosnX = &fRefPosnX[0];
	*pdFeedPosnX = &fFeedPosnX[0];
	*pdRefPosnY = &fRefPosnY[0]; 
	*pdFeedPosnY = &fFeedPosnY[0]; 
	*pdRefPosnZ = &fRefPosnZ[0]; 
	*pdFeedPosnZ = &fFeedPosnZ[0];
	*pdWireClampCmd= &fWireClampCmd[0];
	*pdMotorFlagZ= &fMotorFlagZ[0];
}

// rWaveForm
int mtn_tune_upload_latest_wb_waveform(HANDLE hCommunicationHandle)
{
	int iRet = MTN_API_OK_ZERO;
	int iFlagEnableDebug = 1; // mtn_tune_get_wb_waveform_ena_flag();
	if( iFlagEnableDebug == 1)
	{
		// Need to update with ZD
		if(!acsc_ReadReal(hCommunicationHandle, 0, "rWaveForm", 0, 0, 0, LEN_UPLOAD_ARRAY-1,  fRefPosnX, NULL))
		{
			//// rWaveFormData
			if(!acsc_ReadReal(hCommunicationHandle, 0, "rWaveFormData", 0, 0, 0, LEN_UPLOAD_ARRAY-1,  fRefPosnX, NULL))
			{
				iRet = MTN_API_ERROR_UPLOAD_DATA;
			}
		}
		if(!acsc_ReadReal(hCommunicationHandle, 0, "rWaveForm", 1, 1, 0, LEN_UPLOAD_ARRAY-1,  fFeedPosnX, NULL))
		{
			if(!acsc_ReadReal(hCommunicationHandle, 0, "rWaveFormData", 1, 1, 0, LEN_UPLOAD_ARRAY-1,  fFeedPosnX, NULL))
			{
				iRet = MTN_API_ERROR_UPLOAD_DATA;
			}
		}
		if(!acsc_ReadReal(hCommunicationHandle, 0, "rWaveForm", 2, 2, 0, LEN_UPLOAD_ARRAY-1,  fRefPosnY, NULL))
		{
			if(!acsc_ReadReal(hCommunicationHandle, 0, "rWaveFormData", 2, 2, 0, LEN_UPLOAD_ARRAY-1,  fRefPosnY, NULL))
			{
				iRet = MTN_API_ERROR_UPLOAD_DATA;
			}
		}
		if(!acsc_ReadReal(hCommunicationHandle, 0, "rWaveForm", 3, 3, 0, LEN_UPLOAD_ARRAY-1,  fFeedPosnY, NULL))
		{
			if(!acsc_ReadReal(hCommunicationHandle, 0, "rWaveFormData", 3, 3, 0, LEN_UPLOAD_ARRAY-1,  fFeedPosnY, NULL))
			{
				iRet = MTN_API_ERROR_UPLOAD_DATA;
			}
		}
		if(!acsc_ReadReal(hCommunicationHandle, 0, "rWaveForm", 4, 4, 0, LEN_UPLOAD_ARRAY-1,  fRefPosnZ, NULL))
		{
			if(!acsc_ReadReal(hCommunicationHandle, 0, "rWaveFormData", 4, 4, 0, LEN_UPLOAD_ARRAY-1,  fRefPosnZ, NULL))
			{
				iRet = MTN_API_ERROR_UPLOAD_DATA;
			}
		}
		if(!acsc_ReadReal(hCommunicationHandle, 0, "rWaveForm", 5, 5, 0, LEN_UPLOAD_ARRAY-1,  fFeedPosnZ, NULL))
		{
			if(!acsc_ReadReal(hCommunicationHandle, 0, "rWaveFormData", 5, 5, 0, LEN_UPLOAD_ARRAY-1,  fFeedPosnZ, NULL))
			{
				iRet = MTN_API_ERROR_UPLOAD_DATA;
			}
		}
		// W_Cmd
		if(!acsc_ReadReal(hCommunicationHandle, 0, "rWaveForm", 7, 7, 0, LEN_UPLOAD_ARRAY-1,  fWireClampCmd, NULL))
		{
			if(!acsc_ReadReal(hCommunicationHandle, 0, "rWaveFormData", 7, 7, 0, LEN_UPLOAD_ARRAY-1,  fWireClampCmd, NULL))
			{
				iRet = MTN_API_ERROR_UPLOAD_DATA;
			}
		}
		if(!acsc_ReadReal(hCommunicationHandle, 0, "rWaveForm", 6, 6, 0, LEN_UPLOAD_ARRAY-1,  fMotorFlagZ, NULL))
		{
			if(!acsc_ReadReal(hCommunicationHandle, 0, "rWaveFormData", 6, 6, 0, LEN_UPLOAD_ARRAY-1,  fMotorFlagZ, NULL))
			{
				iRet = MTN_API_ERROR_UPLOAD_DATA;
			}
		}

		// 20110802
		double dTempCTime[1]; 
		if(!acsc_ReadReal(hCommunicationHandle, ACSC_NONE, "CTIME", 0, 0, 0, 0, dTempCTime, NULL))
		{
			dControlSampleTimeMPU = 0.5;
		}
		else
		{
			dControlSampleTimeMPU = dTempCTime[0];
		}
		
		if(iRet != MTN_API_ERROR_UPLOAD_DATA)
		{
			mtn_tune_calc_vaj_from_posn_waveform();
		}
	}

	return iRet;
}
void mtn_tune_calc_vaj_from_posn_waveform()
{
	int ii;
		for(ii=0; ii<LEN_UPLOAD_ARRAY - 1; ii++)
		{
			fRefVelX[ii] = fRefPosnX[ii+1] - fRefPosnX[ii];
			fRefVelY[ii] = fRefPosnY[ii+1] - fRefPosnY[ii];
			fRefVelZ[ii] = fRefPosnZ[ii+1] - fRefPosnZ[ii];
			fFeedVelX[ii] = fFeedPosnX[ii+1] - fFeedPosnX[ii];
			fFeedVelY[ii] = fFeedPosnY[ii+1] - fFeedPosnY[ii];
			fFeedVelZ[ii] = fFeedPosnZ[ii+1] - fFeedPosnZ[ii];
			// 20110802
			fRefVelX[ii] = fRefVelX[ii]/dControlSampleTimeMPU;
			fRefVelY[ii] = fRefVelY[ii]/dControlSampleTimeMPU;
			fRefVelZ[ii] = fRefVelZ[ii]/dControlSampleTimeMPU;
			fFeedVelX[ii] = fFeedVelX[ii]/dControlSampleTimeMPU;
			fFeedVelY[ii] = fFeedVelY[ii]/dControlSampleTimeMPU;
			fFeedVelZ[ii] = fFeedVelZ[ii]/dControlSampleTimeMPU;
			//
			fPosnErrX[ii] = fRefPosnX[ii] - fFeedPosnX[ii];
			fPosnErrY[ii] = fRefPosnY[ii] - fFeedPosnY[ii];
			fPosnErrZ[ii] = fRefPosnZ[ii] - fFeedPosnZ[ii];
		}
		fRefVelX[LEN_UPLOAD_ARRAY - 1] = 0;
		fRefVelY[LEN_UPLOAD_ARRAY - 1] = 0;
		fRefVelZ[LEN_UPLOAD_ARRAY - 1] = 0;
		fFeedVelX[LEN_UPLOAD_ARRAY - 1] = 0;
		fFeedVelY[LEN_UPLOAD_ARRAY - 1] = 0;
		fFeedVelZ[LEN_UPLOAD_ARRAY - 1] = 0;

		fRefAccX[0] = fRefAccY[0] = fRefAccZ[0] = fFeedAccZ[0] = 0;  // 20110520
		for(int ii=1; ii<LEN_UPLOAD_ARRAY; ii++)
		{
			fRefAccX[ii] = (fRefVelX[ii] - fRefVelX[ii - 1]) * dControlSampleTimeMPU;   // 20110810
			fRefAccY[ii] = (fRefVelY[ii] - fRefVelY[ii - 1]) * dControlSampleTimeMPU;
			fRefAccZ[ii] = (fRefVelZ[ii] - fRefVelZ[ii - 1]) * dControlSampleTimeMPU;   // 20110810

			fFeedAccZ[ii] = fFeedVelZ[ii] - fFeedVelZ[ii - 1]; // 20110520
			iMotorFlagZ[ii] = (int)fMotorFlagZ[ii];
		}
		iMotorFlagZ[0] = (int)fMotorFlagZ[0];
		for(int ii=0; ii<LEN_UPLOAD_ARRAY - 1; ii++)
		{
			fRefJerkX[ii] = fRefAccX[ii+1] - fRefAccX[ii];
			fRefJerkY[ii] = fRefAccY[ii+1] - fRefAccY[ii];
			fRefJerkZ[ii] = fRefAccZ[ii+1] - fRefAccZ[ii];
		}
		fRefJerkX[LEN_UPLOAD_ARRAY - 1] = fRefJerkY[LEN_UPLOAD_ARRAY - 1] = fRefJerkZ[LEN_UPLOAD_ARRAY - 1] = 0;
}
char strTokenEndWaveform[] = "];";
static char *strStopString;

//#include"iostream.h"
int Strfind(char *s,char *t)
{
	int index,lengths=0,lengtht=0,i; //,j;
//	char * Position;
	while(s[lengths]!=0)//获取字符串s的长度
	{
		lengths++;
	}
	while(t[lengtht]!=0)
	{
		lengtht++;
	}
	for(i=0;i<lengths-lengtht;i++)//循环到源串的长度减去目标串的长度即可
	{
		index=0;
		while(s[i+index]==t[index] && index<lengtht)
		{
			index++;
		}
		if(index==lengtht)
		{
	  // Position=new char;
	  // *Position=i;
	  // return Position;
			return i;//返回i的位置既是找到的位置
		}
	}
	//Position=NULL;
	return -1;//找不到
}

extern double gdScopeCollectData[30000];

int mtn_debug_load_b1w_data_from_file(char *strFilenameWavefrom, unsigned int *uiTotalLen)
{
	int iRet = MTN_API_OK_ZERO;
	FILE *fptr;
	unsigned int ii = 0, nLines = 0;
	char strReadLine[256], strReadNum[128];
	double dRP_Z, dPE_Z, dRVel_Z, dFdVel_Z, dCmdDrv_Z, dDrvOut_Z, dSLAFF_Z;
	unsigned int iNextCharStart = 0, nRelLocationNextChar;

	fopen_s(&fptr, strFilenameWavefrom, "r");
	if(fptr != NULL)
	{
		while(!feof(fptr))
		{
			fgets(strReadLine, 256, fptr);
			// fscanf(fptr, "%s = [", strReadLine);
			int iCompareTokenFlag = strcmp(strReadLine, strTokenEndWaveform );
			if(iCompareTokenFlag == 0)
			{
				*uiTotalLen = nLines;
				break;
			}
			else 
			{
				int iRetStrCompare = strcmp("aTuneB1W_StopSrch = [", strReadLine);
				if(iRetStrCompare == -1)
				{
					continue;
				}
				else
				{   // RPOS(AXIS), PE(AXIS), RVEL(AXIS), FVEL(AXIS), DCOM(AXIS), DOUT(AXIS), SLAFF(AXIS) 
//					sscanf(strReadLine, "%f, %f, %f, %f, %f, %f, %f, %f;", &dRPX, &dFPX, &dRPY, &dFPY, &dRPZ, &dFPZ, &dMFZ, &dDW);

					sscanf(strReadLine, "%s,", strReadNum);
					dRP_Z = strtod(strReadNum, &strStopString);

					iNextCharStart = 0;
					nRelLocationNextChar = Strfind(&strReadLine[iNextCharStart], ",");
					iNextCharStart = iNextCharStart + nRelLocationNextChar + 1;
					sscanf(&strReadLine[iNextCharStart+1], "%s,", strReadNum);
					dPE_Z = strtod(strReadNum, &strStopString);

					nRelLocationNextChar = Strfind(&strReadLine[iNextCharStart], ",");
					iNextCharStart = iNextCharStart + nRelLocationNextChar + 1;
					sscanf(&strReadLine[iNextCharStart+1], "%s,", strReadNum);
					dRVel_Z = strtod(strReadNum, &strStopString);

					nRelLocationNextChar = Strfind(&strReadLine[iNextCharStart], ",");
					iNextCharStart = iNextCharStart + nRelLocationNextChar + 1;
					sscanf(&strReadLine[iNextCharStart+1], "%s,", strReadNum);
					dFdVel_Z = strtod(strReadNum, &strStopString);

					nRelLocationNextChar = Strfind(&strReadLine[iNextCharStart], ",");
					iNextCharStart = iNextCharStart + nRelLocationNextChar + 1;
					sscanf(&strReadLine[iNextCharStart+1], "%s,", strReadNum);
					dCmdDrv_Z = strtod(strReadNum, &strStopString);

					nRelLocationNextChar = Strfind(&strReadLine[iNextCharStart], ",");
					iNextCharStart = iNextCharStart + nRelLocationNextChar + 1;
					sscanf(&strReadLine[iNextCharStart+1], "%s,", strReadNum);
					dDrvOut_Z = strtod(strReadNum, &strStopString);

					nRelLocationNextChar = Strfind(&strReadLine[iNextCharStart], ",");
					iNextCharStart = iNextCharStart + nRelLocationNextChar + 1;
					sscanf(&strReadLine[iNextCharStart+1], "%s,", strReadNum);
					dSLAFF_Z = strtod(strReadNum, &strStopString);

					gdScopeCollectData[nLines] = dRP_Z;
					gdScopeCollectData[nLines + gstSystemScope.uiDataLen] = dPE_Z; 
					gdScopeCollectData[nLines + gstSystemScope.uiDataLen * 2] = dRVel_Z;
					gdScopeCollectData[nLines + gstSystemScope.uiDataLen * 3] = dFdVel_Z;
					gdScopeCollectData[nLines + gstSystemScope.uiDataLen * 4] = dCmdDrv_Z;
					gdScopeCollectData[nLines + gstSystemScope.uiDataLen * 5] = dDrvOut_Z;
					gdScopeCollectData[nLines + gstSystemScope.uiDataLen * 6] = dSLAFF_Z; 
					nLines ++;
					if(nLines >= 3000) 
					{
						*uiTotalLen = nLines; break;
					}
				}
			}
		}
		if(*uiTotalLen > 3000)
		{
			*uiTotalLen = 3000;
		}
		fclose(fptr);

		memcpy(&fRefPosnZ[0], &gdScopeCollectData[0], LEN_UPLOAD_ARRAY * sizeof(double));
		memcpy(&fPosnErrZ[0],     &gdScopeCollectData[gstSystemScope.uiDataLen], LEN_UPLOAD_ARRAY * sizeof(double));
		memcpy(&fRefVelZ[0],  &gdScopeCollectData[gstSystemScope.uiDataLen * 2], LEN_UPLOAD_ARRAY * sizeof(double));
		memcpy(&fFeedVelZ[0], &gdScopeCollectData[gstSystemScope.uiDataLen * 3], LEN_UPLOAD_ARRAY * sizeof(double));
		double dCountPerSec = 1000.0; // / sys_get_controller_ts();
		int ii;
		for(ii=0; ii<LEN_UPLOAD_ARRAY; ii++)
		{
			fRefVelZ[ii] /= dCountPerSec;
			fFeedVelZ[ii] /= dCountPerSec;
		}

	}
	else
	{
		iRet = MTN_API_ERROR_OPEN_FILE;
	}

	return iRet;
}


int mtn_debug_load_waveform_from_file(char *strFilenameWavefrom, unsigned int *uiTotalLen)
{
	int iRet = MTN_API_OK_ZERO;
	FILE *fptr;
	unsigned int ii = 0;
	char strReadLine[256], strReadNum[128];
	double dRPX, dFPX, dRPY, dFPY, dRPZ, dFPZ, dMFZ, dDW;
	unsigned int iNextCharStart = 0, nRelLocationNextChar;

	fopen_s(&fptr, strFilenameWavefrom, "r");
	if(fptr != NULL)
	{
		while(!feof(fptr))
		{
			fgets(strReadLine, 256, fptr);
			// fscanf(fptr, "%s = [", strReadLine);
			if(strcmp(strTokenEndWaveform, strReadLine) <= 0)
			{
				*uiTotalLen = ii;
				break;
			}
			else 
			{
				int iRetStrCompare = strcmp("[RPX, FPX, RPY, FPY, RPZ, FPZ, MFZ, DW] = [", strReadLine);
				if(iRetStrCompare <= 0)
				{
					continue;
				}
				else
				{
//					sscanf(strReadLine, "%f, %f, %f, %f, %f, %f, %f, %f;", &dRPX, &dFPX, &dRPY, &dFPY, &dRPZ, &dFPZ, &dMFZ, &dDW);

					sscanf(strReadLine, "%s,", strReadNum);
					dRPX = strtod(strReadNum, &strStopString);

					iNextCharStart = 0;
					nRelLocationNextChar = Strfind(&strReadLine[iNextCharStart], ",");
					iNextCharStart = iNextCharStart + nRelLocationNextChar + 1;
					sscanf(&strReadLine[iNextCharStart+1], "%s,", strReadNum);
					dFPX = strtod(strReadNum, &strStopString);

					nRelLocationNextChar = Strfind(&strReadLine[iNextCharStart], ",");
					iNextCharStart = iNextCharStart + nRelLocationNextChar + 1;
					sscanf(&strReadLine[iNextCharStart+1], "%s,", strReadNum);
					dRPY = strtod(strReadNum, &strStopString);

					nRelLocationNextChar = Strfind(&strReadLine[iNextCharStart], ",");
					iNextCharStart = iNextCharStart + nRelLocationNextChar + 1;
					sscanf(&strReadLine[iNextCharStart+1], "%s,", strReadNum);
					dFPY = strtod(strReadNum, &strStopString);

					nRelLocationNextChar = Strfind(&strReadLine[iNextCharStart], ",");
					iNextCharStart = iNextCharStart + nRelLocationNextChar + 1;
					sscanf(&strReadLine[iNextCharStart+1], "%s,", strReadNum);
					dRPZ = strtod(strReadNum, &strStopString);

					nRelLocationNextChar = Strfind(&strReadLine[iNextCharStart], ",");
					iNextCharStart = iNextCharStart + nRelLocationNextChar + 1;
					sscanf(&strReadLine[iNextCharStart+1], "%s,", strReadNum);
					dFPZ = strtod(strReadNum, &strStopString);

					nRelLocationNextChar = Strfind(&strReadLine[iNextCharStart], ",");
					iNextCharStart = iNextCharStart + nRelLocationNextChar + 1;
					sscanf(&strReadLine[iNextCharStart+1], "%s,", strReadNum);
					dMFZ = strtod(strReadNum, &strStopString);

					nRelLocationNextChar = Strfind(&strReadLine[iNextCharStart], ",");
					iNextCharStart = iNextCharStart + nRelLocationNextChar + 1;
					sscanf(&strReadLine[iNextCharStart+1], "%s;", strReadNum);
					dDW = strtod(strReadNum, &strStopString);

					fRefPosnX[ii] = dRPX;
					fFeedPosnX[ii] = dFPX;
					fRefPosnY[ii] = dRPY;
					fFeedPosnY[ii] = dFPY;
					fRefPosnZ[ii] = dRPZ;
					fFeedPosnZ[ii] = dFPZ;
					fWireClampCmd[ii] = dMFZ;
					fMotorFlagZ[ii] = dDW;
					ii ++;
				}
			}
		}

		if(ii >= 2)
		{
			mtn_tune_calc_vaj_from_posn_waveform();
		}
		fclose(fptr);
	}
	else
	{
		iRet = MTN_API_ERROR_OPEN_FILE;
	}

	return iRet;
}

//#define WB_SHOW_WAVEFORM_ALL   0
//#define WB_SHOW_WAVEFORM_BH    1
//#define WB_SHOW_WAVEFORM_XY    2
//static int iWbShowWaveform_DebugLevel;


#define EPSLN   1E-6
int mtn_tune_is_posn_z_cmd_eq_fb(int idx)
{
	if(fabs(fPosnErrZ[idx])< EPSLN)  // fRefPosnZ[idx] - fFeedPosnZ[idx]
		return TRUE;
	else
		return FALSE;
}

int mtn_tune_is_z_posn_err_less_than_(int idx, double dThPosnErr)  // 20110519
{
	if(fabs(fPosnErrZ[idx])< EPSLN)  // fRefPosnZ[idx] - fFeedPosnZ[idx]
		return TRUE;
	else
		return FALSE;
}

int mtn_tune_is_posn_x_cmd_eq_fb(int idx)
{
	if(fabs(fFeedPosnX[idx] - fRefPosnX[idx])< EPSLN)
		return TRUE;
	else
		return FALSE;
}
int mtn_tune_is_posn_y_cmd_eq_fb(int idx)
{
	if(fabs(fFeedPosnY[idx] - fRefPosnY[idx])< EPSLN)
		return TRUE;
	else
		return FALSE;
}
int mtn_tune_is_ref_vel_x_eq_0(int idx)
{
	if(fabs(fRefVelX[idx])< 0.5)  // EPSLN
		return TRUE;
	else
		return FALSE;
}

int mtn_tune_is_ref_vel_y_eq_0(int idx)
{
	if(fabs(fRefVelY[idx])< 0.2)  // EPSLN
		return TRUE;
	else
		return FALSE;
}
int mtn_tune_is_ref_vel_z_eq_0(int idx)
{
	if(fabs(fRefVelZ[idx])<= EPSLN)
		return TRUE;
	else
		return FALSE;
}

int mtn_tune_is_ref_vel_z_less_1(int idx)
{
	if(fabs(fRefVelZ[idx])<= 1.0)
		return TRUE;
	else
		return FALSE;
}

int mtn_tune_is_ref_vel_z_abs_min_local(int idx)
{
	if(fabs(fRefVelZ[idx])< fabs(fRefVelZ[idx - 1])
		&& fabs(fRefVelZ[idx]) < fabs(fRefVelZ[idx + 1])
		&& fabs(fRefVelZ[idx]) > 0.01
		&& fabs(fRefVelZ[idx]) <= 1.0
		)
		return TRUE;
	else
		return FALSE;
}

static int iCurrSelAxisServoParaTuning = WB_AXIS_TABLE_X;  // X
static unsigned int uiCurrWireNo=0; //, uiBlkMovePosnSet=0;
static int idxTuningBlockCurrAxis;
static int idxTuningSectorTblCurrAxis;

unsigned int mtn_wb_tune_get_curr_sector_table()
{
	return idxTuningSectorTblCurrAxis;
}
void mtn_wb_tune_set_curr_sector_table(unsigned int uiCurrSector)
{
	idxTuningSectorTblCurrAxis = uiCurrSector;
}
unsigned int mtn_wb_tune_get_curr_wire_number()
{
	return uiCurrWireNo;
}
void mtn_wb_tune_set_curr_wire_number(unsigned int uiWireNo)
{
	uiCurrWireNo = uiWireNo;
}

unsigned int mtn_wb_tune_get_blk_move_posn_set()
{
	return idxTuningBlockCurrAxis;
}

void mtn_wb_tune_set_blk_move_posn_set(unsigned int uiBlkPosnSet)
{
	idxTuningBlockCurrAxis = uiBlkPosnSet;
}
int mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()
{
	return iCurrSelAxisServoParaTuning;
}
void mtn_wb_tune_set_curr_wb_axis_servo_para_tuning(int iWbAxis)
{
	iCurrSelAxisServoParaTuning = iWbAxis;
}


void mtn_cvt_servo_para_set_from_acs_to_tune(CTRL_PARA_ACS *stpServoParaACS, MTN_TUNE_PARAMETER_SET *stpParaTuningSet)
{
	stpParaTuningSet->dSecOrdFilterFreq_Hz = stpServoParaACS->dSecondOrderLowPassFilterBandwidth;
	stpParaTuningSet->dVelLoopKI = stpServoParaACS->dVelocityLoopIntegratorGain;
	stpParaTuningSet->dVelLoopKP = stpServoParaACS->dVelocityLoopProportionalGain;
	stpParaTuningSet->dVelLoopLimitI = stpServoParaACS->dVelocityLoopIntegratorLimit;
	stpParaTuningSet->dAccFFC = stpServoParaACS->dAccelerationFeedforward;
	stpParaTuningSet->dPosnLoopKP = stpServoParaACS->dPositionLoopProportionalGain;
	stpParaTuningSet->dJerkFf = stpServoParaACS->dJerkFf;  // 20110524
}

void mtn_cvt_servo_para_set_from_tune_to_acs(MTN_TUNE_PARAMETER_SET *stpParaTuningSet, CTRL_PARA_ACS *stpServoParaACS)
{
	stpServoParaACS->dSecondOrderLowPassFilterBandwidth = stpParaTuningSet->dSecOrdFilterFreq_Hz;
	stpServoParaACS->dVelocityLoopIntegratorGain = stpParaTuningSet->dVelLoopKI;
	stpServoParaACS->dVelocityLoopProportionalGain = stpParaTuningSet->dVelLoopKP;
	stpServoParaACS->dVelocityLoopIntegratorLimit = stpParaTuningSet->dVelLoopLimitI;
	stpServoParaACS->dAccelerationFeedforward = stpParaTuningSet->dAccFFC;
	stpServoParaACS->dPositionLoopProportionalGain = stpParaTuningSet->dPosnLoopKP;
	stpServoParaACS->dJerkFf = stpParaTuningSet->dJerkFf;  // 20120228
}

void mtn_wb_tune_load_servo_para_to_tune_output(MTN_TUNE_GENETIC_OUTPUT *stpMtnServoParaTuneOutput, int iWbAxisOpt, unsigned int uiCurrTableSector, unsigned int uiBlkMovePosnSet)
{
	if(iWbAxisOpt == WB_AXIS_BOND_Z)
	{
		switch(uiBlkMovePosnSet)
		{
		case WB_BH_IDLE:
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[WB_BH_IDLE], &stpMtnServoParaTuneOutput->stBestParameterSet);
			break;
		case WB_BH_1ST_CONTACT:
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[WB_BH_1ST_CONTACT], &stpMtnServoParaTuneOutput->stBestParameterSet);
			break;
		case WB_BH_2ND_CONTACT: // = 8,
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[WB_BH_2ND_CONTACT], &stpMtnServoParaTuneOutput->stBestParameterSet);
			break;
		case WB_BH_SRCH_HT: // = 2,
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[WB_BH_SRCH_HT], &stpMtnServoParaTuneOutput->stBestParameterSet);
			break;
		case WB_BH_LOOPING: // = 3,
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[WB_BH_LOOPING], &stpMtnServoParaTuneOutput->stBestParameterSet);
			break;
		case WB_BH_LOOP_TOP: // = 4,
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[WB_BH_LOOP_TOP], &stpMtnServoParaTuneOutput->stBestParameterSet);
			break;
		case WB_BH_TRAJECTORY: // = 5,
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[WB_BH_TRAJECTORY], &stpMtnServoParaTuneOutput->stBestParameterSet);
			break;
		case WB_BH_TAIL: // = 6,
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[WB_BH_TAIL], &stpMtnServoParaTuneOutput->stBestParameterSet);
			break;
		case WB_BH_RESET: // = 7,
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[WB_BH_RESET], &stpMtnServoParaTuneOutput->stBestParameterSet);
			break;
		default :
			break;
		}
	}
	unsigned int uiMaxSector = mtn_wb_tune_get_max_sector_table_x();
	if(iWbAxisOpt == WB_AXIS_TABLE_X)
	{ // all use the same parameters
		mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_TABLE_X].stServoParaACS[0], &stpMtnServoParaTuneOutput->stBestParameterSet);
		uiMaxSector = mtn_wb_tune_get_max_sector_table_x();
		if(uiCurrTableSector < uiMaxSector)
		{
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_TABLE_X].stServoParaACS[uiCurrTableSector], &stpMtnServoParaTuneOutput->stBestParameterSet);
		}
	}
	else if(iWbAxisOpt == WB_AXIS_TABLE_Y)
	{
		mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_TABLE_Y].stServoParaACS[0], &stpMtnServoParaTuneOutput->stBestParameterSet);
		uiMaxSector = mtn_wb_tune_get_max_sector_table_y();
		if(uiCurrTableSector < uiMaxSector)
		{
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_TABLE_Y].stServoParaACS[uiCurrTableSector], &stpMtnServoParaTuneOutput->stBestParameterSet);
		}
	}
}

void mtn_wb_tune_load_servo_para_to_tune_ini(MTN_TUNE_GENETIC_INPUT *stpMtnServoParaTuneInput, int iWbAxisOpt, unsigned int uiCurrTableSector, unsigned int uiBlkMovePosnSet)
{
	if(iWbAxisOpt == WB_AXIS_BOND_Z)
	{
		switch(uiBlkMovePosnSet)
		{
		case WB_BH_IDLE:
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[WB_BH_IDLE], &stpMtnServoParaTuneInput->stTuningParameterIni.stMtnPara);
			break;
		case WB_BH_1ST_CONTACT:
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[WB_BH_1ST_CONTACT], &stpMtnServoParaTuneInput->stTuningParameterIni.stMtnPara);
			break;
		case WB_BH_2ND_CONTACT: // = 8,
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[WB_BH_2ND_CONTACT], &stpMtnServoParaTuneInput->stTuningParameterIni.stMtnPara);
			break;
		case WB_BH_SRCH_HT: // = 2,
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[WB_BH_SRCH_HT], &stpMtnServoParaTuneInput->stTuningParameterIni.stMtnPara);
			break;
		case WB_BH_LOOPING: // = 3,
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[WB_BH_LOOPING], &stpMtnServoParaTuneInput->stTuningParameterIni.stMtnPara);
			break;
		case WB_BH_LOOP_TOP: // = 4,
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[WB_BH_LOOP_TOP], &stpMtnServoParaTuneInput->stTuningParameterIni.stMtnPara);
			break;
		case WB_BH_TRAJECTORY: // = 5,
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[WB_BH_TRAJECTORY], &stpMtnServoParaTuneInput->stTuningParameterIni.stMtnPara);
			break;
		case WB_BH_TAIL: // = 6,
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[WB_BH_TAIL], &stpMtnServoParaTuneInput->stTuningParameterIni.stMtnPara);
			break;
		case WB_BH_RESET: // = 7,
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_BOND_Z].stServoParaACS[WB_BH_RESET], &stpMtnServoParaTuneInput->stTuningParameterIni.stMtnPara);
			break;
		default :
			break;
		}
	}
	unsigned int uiMaxSector = mtn_wb_tune_get_max_sector_table_x();

	if(iWbAxisOpt == WB_AXIS_TABLE_X)
	{ // all use the same parameters
		mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_TABLE_X].stServoParaACS[0], &stpMtnServoParaTuneInput->stTuningParameterIni.stMtnPara);
		uiMaxSector = mtn_wb_tune_get_max_sector_table_x();
		if(uiCurrTableSector < uiMaxSector)
		{
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_TABLE_X].stServoParaACS[uiCurrTableSector], &stpMtnServoParaTuneInput->stTuningParameterIni.stMtnPara);
		}
	}
	else if(iWbAxisOpt == WB_AXIS_TABLE_Y)
	{
		mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_TABLE_Y].stServoParaACS[0], &stpMtnServoParaTuneInput->stTuningParameterIni.stMtnPara);
		uiMaxSector = mtn_wb_tune_get_max_sector_table_y();
		if(uiCurrTableSector < uiMaxSector)
		{
			mtn_cvt_servo_para_set_from_acs_to_tune(&stServoAxis_ACS[WB_AXIS_TABLE_Y].stServoParaACS[uiCurrTableSector], &stpMtnServoParaTuneInput->stTuningParameterIni.stMtnPara);
		}
	}
}


extern double f_sign(double dInput);
extern double fFeedVelZ[LEN_UPLOAD_ARRAY];

double mtn_wb_tune_b1w_calc_bh_obj(TIME_POINTS_OF_BONDHEAD_Z *stpTimePointsOfBondHeadZ, int iObjSectionFlagB1W)
{
double dObj, dMoveDir, dObjPosn;
int idxStart = 0, idxEnd = 0;

///////////// Object Part -- 1
// Object of Over-Under Shoot
	switch(iObjSectionFlagB1W)
	{
	case WB_BH_SRCH_HT:
		idxStart = stpTimePointsOfBondHeadZ->idxEndMove1stBondSearchHeight;
		idxEnd = (stpTimePointsOfBondHeadZ->idxEndMove1stBondSearchHeight); // Search delay // 20110506
		dMoveDir = f_sign(fRefVelZ[((int)(stpTimePointsOfBondHeadZ->idxStartMove1stBondSearchHeight + idxEnd)/2)]);
		break;
	case WB_BH_LOOPING:
		idxStart = (stpTimePointsOfBondHeadZ->idxEndReverseHeight + stpTimePointsOfBondHeadZ->idxStartReverseHeight)/2;  // 20110708
		idxEnd = stpTimePointsOfBondHeadZ->idxStartMoveLoopTop;   // 20110505
		dMoveDir = f_sign(fRefVelZ[((int)(stpTimePointsOfBondHeadZ->idxStartReverseHeight + idxEnd)/2)]);
		break;
	case WB_BH_LOOP_TOP:
		idxStart = stpTimePointsOfBondHeadZ->idxEndMoveLoopTop- 2;
		idxEnd = stpTimePointsOfBondHeadZ->idxStartTrajectory;  // 20110505  // idxEndMoveLoopTop + 2
		dMoveDir = f_sign(fRefVelZ[((int)(stpTimePointsOfBondHeadZ->idxStartMoveLoopTop + idxEnd)/2)]);
		break;
	case WB_BH_TRAJECTORY:
		idxStart = stpTimePointsOfBondHeadZ->idxEndTrajectory2ndBondSearchHeight- 5 ;
		idxEnd = stpTimePointsOfBondHeadZ->idxEndTrajectory2ndBondSearchHeight + 6;  // 20110421
		dMoveDir = f_sign(fRefVelZ[((int)(stpTimePointsOfBondHeadZ->idxStartTrajectory + idxEnd)/2)]);
		break;
	case WB_BH_TAIL:
		idxStart = stpTimePointsOfBondHeadZ->idxEndTail- 1;
		idxEnd = stpTimePointsOfBondHeadZ->idxEndTail+1;
		dMoveDir = f_sign(fRefVelZ[((int)(stpTimePointsOfBondHeadZ->idxStartTail + idxEnd)/2)]);
		break;
	case WB_BH_RESET:
		idxStart = stpTimePointsOfBondHeadZ->idxEndFireLevel- 1;
		idxEnd = stpTimePointsOfBondHeadZ->idxEndFireLevel+2;  // Default inter-wire delay 15 ms, from "H:\Documents and Settings\All Users\Documents\BE_WB_DATA\Tuning\AutoTune\Wb_thomas_process\17_10 XY Default FFC.dat" // 20101017
		dMoveDir = f_sign(fRefVelZ[((int)(stpTimePointsOfBondHeadZ->idxStartFireLevel + stpTimePointsOfBondHeadZ->idxEndFireLevel)/2)]); // 20101017
		break;
	case WB_BH_IDLE:
		idxStart = stpTimePointsOfBondHeadZ->idxEndFireLevel + 8;
		idxEnd = stpTimePointsOfBondHeadZ->idxEndFireLevel + 16;  // Default inter-wire delay 15 ms, from "H:\Documents and Settings\All Users\Documents\BE_WB_DATA\Tuning\AutoTune\Wb_thomas_process\17_10 XY Default FFC.dat" // 20101017
		break;
	}
	
// stpAnalyzeOneWire->iFlagHasWireInfo = 0;
	dObj = 0; dObjPosn = 0;
int ii;
double dTempOverUnderShoot;
	for(ii = idxStart; ii<=idxEnd; ii++)
	{
		dTempOverUnderShoot = dMoveDir * fPosnErrZ[ii];
		switch(iObjSectionFlagB1W)
		{
		case WB_BH_SRCH_HT:
		case WB_BH_TRAJECTORY:

			dObj = dObj + fabs(fPosnErrZ[ii]);
			if((fRefVelZ[ii] - fFeedVelZ[ii]) * dMoveDir > 0)
			{
				dObj = dObj + fabs((fRefVelZ[ii] - fFeedVelZ[ii])); // /fRefVelZ[ii]
			}
//			dObj = dObj + fabs((fRefVelZ[ii] - fFeedVelZ[ii]) *(fRefVelZ[ii] - fFeedVelZ[ii]) );  // 20110524
			break;
		case WB_BH_LOOP_TOP:
		case WB_BH_LOOPING:
			//dObj = dObj + fPosnErrZ[ii] * fPosnErrZ[ii] * stpTuneTheme->fWeightCmdEndOverUnderShoot; // 20110505 // dTempOverUnderShoot * dTempOverUnderShoot
			//break;
			dObj = dObj + fabs(fPosnErrZ[ii]);
			if(fFeedVelZ[ii] * dMoveDir< 0) // penalty on overshoot, 
			{
				dObj = dObj + fFeedVelZ[ii] * fFeedVelZ[ii];
			}				
			break;
		case WB_BH_RESET:
		case WB_BH_TAIL:
		case WB_BH_IDLE:
			dObj = dObj + fabs(fPosnErrZ[ii]);
			break;

		}
	}
///////////// Object Part -- 2
// Add penalty of ripple
#define __TUNE_B1W_LEN_POSN_ERR_RIPPLE    200
double  adDiffPosnErr[__TUNE_B1W_LEN_POSN_ERR_RIPPLE], dTemp2ndDiff;
double  adDiffFbPosn[__TUNE_B1W_LEN_POSN_ERR_RIPPLE];
int iTemp;
int iForceCtrlStart = 0, iForceCtrlTime = 0;
double dMeanFCtrlFbPosn, dStdFCtrlFbPosn;
	switch(iObjSectionFlagB1W)
	{
	case WB_BH_SRCH_HT:
		idxStart = (stpTimePointsOfBondHeadZ->idxEndMove1stBondSearchHeight -1); // + stpTimePointsOfBondHeadZ->idxEndMove1stBondSearchHeight * 7)/8; // 20101002
		idxEnd = stpTimePointsOfBondHeadZ->idxEndMove1stBondSearchHeight +2; // mininum search delay // 20110506
		break;
	case WB_BH_LOOPING:
		idxStart = stpTimePointsOfBondHeadZ->idxEndReverseHeight- 2; // 20110509 (stpTimePointsOfBondHeadZ->idxStartReverseHeight + stpTimePointsOfBondHeadZ->idxEndReverseHeight * 7)/8; // 20101002
		idxEnd = stpTimePointsOfBondHeadZ->idxStartMoveLoopTop; // 20110509 idxEndReverseHeight;
		break;
	case WB_BH_LOOP_TOP:
		idxStart = stpTimePointsOfBondHeadZ->idxEndMoveLoopTop - 2; // (stpTimePointsOfBondHeadZ->idxStartMoveLoopTop + stpTimePointsOfBondHeadZ->idxEndMoveLoopTop * 7)/8; // 20101002
		idxEnd = stpTimePointsOfBondHeadZ->idxStartTrajectory;  // 20110509 idxEndMoveLoopTop;
		break;
	case WB_BH_TRAJECTORY:
		idxStart = stpTimePointsOfBondHeadZ->idxEndTrajectory2ndBondSearchHeight - 1; // (stpTimePointsOfBondHeadZ->idxStartTrajectory +  * 7)/8 ; // 20101002, // 20110421
		idxEnd = stpTimePointsOfBondHeadZ->idxEndTrajectory2ndBondSearchHeight + 6; // 20110506
		break;
	case WB_BH_TAIL:
		idxStart = stpTimePointsOfBondHeadZ->idxEndTail - 3; // (stpTimePointsOfBondHeadZ->idxStartTail + stpTimePointsOfBondHeadZ->idxEndTail * 7)/8;  // 20101002
		idxEnd = stpTimePointsOfBondHeadZ->idxStartFireLevel; //stpTimePointsOfBondHeadZ->idxEndTail + ;
		break;
	case WB_BH_RESET:
		idxStart = stpTimePointsOfBondHeadZ->idxEndFireLevel - 1; //(stpTimePointsOfBondHeadZ->idxStartFireLevel + stpTimePointsOfBondHeadZ->idxEndFireLevel* 7)/8;  // 20101002
		idxEnd = stpTimePointsOfBondHeadZ->idxEndFireLevel + 2;
		break;
	case WB_BH_1ST_CONTACT:
		idxStart = stpTimePointsOfBondHeadZ->idxEndMove1stBondSearchHeight + 6; // Ignore First 3ms, for settling
		if(stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl > fabs(stpTimePointsOfBondHeadZ->idxEndMove1stBondSearchHeight + 10.0))
		{
			idxEnd = stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl - 4; // idxStart1stBondForceCtrl - 4;
		}
		else
		{
			idxEnd = stpTimePointsOfBondHeadZ->idxEndMove1stBondSearchHeight + 16;
		}
		iForceCtrlStart = stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl;
		iForceCtrlTime = stpTimePointsOfBondHeadZ->idxEnd1stBondForceCtrl - iForceCtrlStart;
		break;
	case WB_BH_2ND_CONTACT:
		idxStart = stpTimePointsOfBondHeadZ->idxEndTrajectory2ndBondSearchHeight + 6; // Ignore First 3ms, for settling
		if(stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl > fabs(stpTimePointsOfBondHeadZ->idxEndTrajectory2ndBondSearchHeight + 10.0))
		{
			idxEnd = stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl - 4;
		}
		else
		{
			idxEnd = stpTimePointsOfBondHeadZ->idxEndTrajectory2ndBondSearchHeight + 16;
		}
		iForceCtrlStart = stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl;
		iForceCtrlTime = stpTimePointsOfBondHeadZ->idxEnd2ndBondForceCtrl - iForceCtrlStart;
		break;
	case WB_BH_IDLE:
		idxStart = stpTimePointsOfBondHeadZ->idxEndFireLevel + 1;
		idxEnd = stpTimePointsOfBondHeadZ->idxEndFireLevel + 12;  // 20101002, Tuning ripple section
		break;
	}

	for(ii = idxStart; ii< idxEnd; ii++)
	{
		adDiffPosnErr[ii - idxStart] = fPosnErrZ[ii + 1] - fPosnErrZ[ii];	
		adDiffFbPosn[ii - idxStart] = fFeedPosnZ[ii +1] - fFeedPosnZ[ii];  // 20101002
		switch(iObjSectionFlagB1W)
		{
		case WB_BH_1ST_CONTACT:
		case WB_BH_2ND_CONTACT:
			dObj = dObj + (fRefVelZ[ii] - fFeedVelZ[ii])/2.0 * (fRefVelZ[ii] - fFeedVelZ[ii])/2.0;
			dObjPosn = dObjPosn + (fPosnErrZ[ii]/EXPECT_POSN_ERR_FB_LEAD_CMD_SRCH_CONTACT) * (fPosnErrZ[ii]/EXPECT_POSN_ERR_FB_LEAD_CMD_SRCH_CONTACT);
//			dObj = dObj + fFeedAccZ[ii] * fFeedAccZ[ii] * (ii - idxStart) * (ii - idxStart);
			break;
		case WB_BH_IDLE:
			dObj = dObj + fabs(fPosnErrZ[ii]); // * fPosnErrZ[ii];
			break;
		case WB_BH_TAIL:
		case WB_BH_TRAJECTORY: // 20110421
		case WB_BH_SRCH_HT:  // 20110506
		case WB_BH_LOOPING:   // 20110509
		case WB_BH_LOOP_TOP:  // 20110509
//			if (dMoveDir * fFeedVelZ[ii] < 0)  // adDiffFbPosn[ii - idxStart] < 0
//			{
//				dObj = dObj + fFeedVelZ[ii] * fFeedVelZ[ii];
//			}
			break;
		}
	}	
	switch(iObjSectionFlagB1W)
	{
		case WB_BH_1ST_CONTACT:
		case WB_BH_2ND_CONTACT:
			dObj = sqrt(dObj/(idxEnd - idxStart));
			dObjPosn = sqrt(dObjPosn/(idxEnd - idxStart));

			dObj = dObj + dObjPosn;
			break;
		default:
			break;
	}

	for(ii = idxStart; ii< idxEnd - 2; ii++)
	{
		switch(iObjSectionFlagB1W)
		{
		case WB_BH_TAIL:  // Only consider 
		case WB_BH_TRAJECTORY: // 20110421
		case WB_BH_SRCH_HT:  // 20110506
			iTemp = ii - idxStart;
			dTemp2ndDiff = adDiffFbPosn[iTemp + 1] - adDiffFbPosn[iTemp];
			dObj = dObj + dTemp2ndDiff * dTemp2ndDiff;    // 20101002
			break;
		case WB_BH_IDLE:
			iTemp = ii - idxStart;
			dTemp2ndDiff = adDiffPosnErr[iTemp + 1] - adDiffPosnErr[iTemp];
			dObj = dObj + dTemp2ndDiff * dTemp2ndDiff; 
			break;
		default: break;
		}
	}
	dMeanFCtrlFbPosn =  f_get_mean(&fFeedPosnZ[iForceCtrlStart], iForceCtrlTime); // 20101002, Tuning ripple section
	dStdFCtrlFbPosn = f_get_std(&fFeedPosnZ[iForceCtrlStart], dMeanFCtrlFbPosn, iForceCtrlTime); // 

	return dObj;

}

double mtn_tune_calc_b1w_bh_performance(WB_ONE_WIRE_PERFORMANCE_CALC *stpAnalyzeOneWire, MTN_TUNE_THEME *stpTuneTheme,
									  int iObjSectionFlagB1W)
{
//BOND_HEAD_PERFORMANCE *stpBondHeadPerformance = &(stpAnalyzeOneWire->stBondHeadPerformance);
TIME_POINTS_OF_BONDHEAD_Z *stpTimePointsOfBondHeadZ = &(stpAnalyzeOneWire->stTimePointsOfBondHeadZ);

double dObjOUS_Settling;
double dObjOverall;
	dObjOUS_Settling = mtn_wb_tune_b1w_calc_bh_obj(stpTimePointsOfBondHeadZ, iObjSectionFlagB1W);

	dObjOverall = 0;
	//////////////////////// Obj --- 3: Penalty of false contact
#define MTN_WB_TUNE_BIG_NUMBER_PENALTY  200000
	// Add detection of False Contact
	int nLenSearch_cnt;
	if(stpAnalyzeOneWire->iFlagIsDryRun == 0)
	{
		switch(iObjSectionFlagB1W)
		{
		case WB_BH_LOOP_TOP:
		case WB_BH_TRAJECTORY:
		case WB_BH_2ND_CONTACT:
			nLenSearch_cnt = stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl - stpTimePointsOfBondHeadZ->idxStart2ndBondSearchContact;
			if(nLenSearch_cnt < 10)
			{
				dObjOverall = dObjOverall + MTN_WB_TUNE_BIG_NUMBER_PENALTY;
			}
			break;
		case WB_BH_RESET:
		case WB_BH_IDLE:
		case WB_BH_1ST_CONTACT:
		case WB_BH_SRCH_HT:
			nLenSearch_cnt = stpTimePointsOfBondHeadZ->idxStart1stBondForceCtrl - stpTimePointsOfBondHeadZ->idxEndMove1stBondSearchHeight;
			if(nLenSearch_cnt < 10)
			{
				dObjOverall = dObjOverall + MTN_WB_TUNE_BIG_NUMBER_PENALTY;
			}
			break;
		}
	}

	dObjOverall = dObjOverall + dObjOUS_Settling;

	return dObjOverall;

}

#define  WEIGHT_PARA_1ST_BOND_OVER_LOOPING   (0.25)
#define  WEIGHT_PARA_2ND_BOND_OVER_LOOPING   (2.0)

void mtn_tune_round_parameter_to_nearest_1(MTN_TUNE_PARAMETER_SET *stpMtnParaRoundIn, MTN_TUNE_PARAMETER_SET *stpMtnParaRoundOut); // from MtnTune_2.cpp
extern SERVO_ACS stServoACS;

void mtn_wb_copy_acs_para_from_TuningOutParameterSet_to_ServoParaACS(CTRL_PARA_ACS		*stpCopyToServoParaACS,
																	 MTN_TUNE_PARAMETER_SET *stpCopyFromTuneOutParameterSet)
{
		stpCopyToServoParaACS->dVelocityLoopIntegratorGain = 			stpCopyFromTuneOutParameterSet->dVelLoopKI;
		stpCopyToServoParaACS->dVelocityLoopProportionalGain = 		stpCopyFromTuneOutParameterSet->dVelLoopKP;
		stpCopyToServoParaACS->dPositionLoopProportionalGain = 		stpCopyFromTuneOutParameterSet->dPosnLoopKP;
		stpCopyToServoParaACS->dAccelerationFeedforward =			stpCopyFromTuneOutParameterSet->dAccFFC;

}

///////////////////////////////////////////////////// Default Stable VKP for XY Table
#define _DEF_XY_VKP_STABLE__      180
#define _DEF_XY_VKP_STABLE_18V_   300

void mtn_wb_update_acs_parameter_servo_struct_common_blk(int nTotalBlkInAcs, int iWbAxis, MTN_TUNE_PARAMETER_SET *stpTuneOutParameterSet)
{
	int ii;
	int nActualMaxBLk = MAX_BLK_PARAMETER;
	if(nActualMaxBLk > nTotalBlkInAcs)
	{
		nActualMaxBLk = nTotalBlkInAcs;
	}
	for(ii = 0; ii<nTotalBlkInAcs; ii++)
	{
		mtn_wb_copy_acs_para_from_TuningOutParameterSet_to_ServoParaACS(&stServoACS.stpServoAxis_ACS[iWbAxis]->stServoParaACS[ii],
			stpTuneOutParameterSet);

//		if(ii == 0 && iWbAxis == WB_AXIS_TABLE_X)  // 20120628, totally 2 places need this default value
//		{ // 20121017
		double dUppVKP; //, dLowVKP;
		double dUppVKI; //, dLowVKI;
		dUppVKP = astMtnTuneInput[iWbAxis][ii].stTuningParameterUppBound.stMtnPara.dVelLoopKP;
		dUppVKI = astMtnTuneInput[iWbAxis][ii].stTuningParameterUppBound.stMtnPara.dVelLoopKI;

			if(stServoACS.stpServoAxis_ACS[iWbAxis]->stServoParaACS[ii].dVelocityLoopProportionalGain 
				> dUppVKP) // //	> _DEF_XY_VKP_STABLE__)
			{
				stServoACS.stpServoAxis_ACS[iWbAxis]->stServoParaACS[ii].dVelocityLoopProportionalGain = 
					dUppVKP;
			}
			if(stServoACS.stpServoAxis_ACS[iWbAxis]->stServoParaACS[ii].dVelocityLoopIntegratorGain > dUppVKI)
			{
				stServoACS.stpServoAxis_ACS[iWbAxis]->stServoParaACS[ii].dVelocityLoopIntegratorGain = dUppVKI;
			}
//		}

	}
}

void mtn_wb_bond_head_z_update_acs_parameter_servo_struct_multi_blk(int nTotalBlkInAcs)
{
	int ii;
	int nActualMaxBLk = WB_BH_2ND_CONTACT + 1;   // MtnWbDef.h
	if(nActualMaxBLk > nTotalBlkInAcs)
	{
		nActualMaxBLk = nTotalBlkInAcs;
	}
	for(ii = 0; ii<nActualMaxBLk; ii++)
	{
		if(mtn_wb_checksum_para_b1w_out(&astMtnTuneOutput[WB_AXIS_BOND_Z][ii].stBestParameterSet) > 10)
		{
			mtn_wb_copy_acs_para_from_TuningOutParameterSet_to_ServoParaACS(&stServoACS.stpServoAxis_ACS[WB_AXIS_BOND_Z]->stServoParaACS[ii],
				&astMtnTuneOutput[WB_AXIS_BOND_Z][ii].stBestParameterSet);
		}
	}
}
// Sum of PKP, AFF, VKP, VKI
#define MIN_CHECK_SUM_FEASIBLE_PARA  100

void mtn_wb_bond_head_z_update_acs_parameter_servo_struct_multi_blk_w_chksum(int nTotalBlkInAcs)
{
	int ii;
	int nActualMaxBLk = WB_BH_2ND_CONTACT + 1;   // MtnWbDef.h
	if(nActualMaxBLk > nTotalBlkInAcs)
	{
		nActualMaxBLk = nTotalBlkInAcs;
	}
	for(ii = 0; ii<nActualMaxBLk; ii++)
	{
		if(mtn_wb_checksum_para_b1w_out(&astMtnTuneOutput[WB_AXIS_BOND_Z][ii].stBestParameterSet) > MIN_CHECK_SUM_FEASIBLE_PARA)
		{
			mtn_wb_copy_acs_para_from_TuningOutParameterSet_to_ServoParaACS(&stServoACS.stpServoAxis_ACS[WB_AXIS_BOND_Z]->stServoParaACS[ii],
				&astMtnTuneOutput[WB_AXIS_BOND_Z][ii].stBestParameterSet);
		}

	}
}

#include "MtnAlgo_Private.h"
// stCommHandle
int mtn_wb_bh_verify_consolidate_save_para_tune_out()
{
int iRet = MTN_API_OK_ZERO, iRetCheckSum = MTN_API_OK_ZERO;

	iRetCheckSum = mtn_wb_verify_tune_out_b1w();
	if(iRetCheckSum == MTN_API_OK_ZERO)
	{
		iRet = mtnapi_confirm_para_base_path_exist();
		if(iRet == MTN_API_OK_ZERO)
		{
			iRet = mtnapi_init_master_config_acs("C:\\WbData\\ParaBase\\ServoMaster.ini", stServoControllerCommSet.Handle); // must be the same as in MotDLL_LoadW*.cpp
			if(iRet  == MTN_API_OK_ZERO) // NOT, mtnapi_init_servo_control_para_acs
			{
				mtn_wb_bond_head_z_update_acs_parameter_servo_struct_multi_blk(WB_BH_2ND_CONTACT + 1);
				//	NUM_BONDHEAD_SPEED_PROF_BLK time of mtn_tune_upload_wb_bh_z_speed_profile_blk(stcommhandle, ii);
				mtn_tune_upload_wb_speed_profile(stServoControllerCommSet.Handle); // 20110202, XYZ stCommHandle
				mtnapi_rename_bakup_servo_parameter_acs_per_axis(WB_AXIS_BOND_Z);  // 20110509
				mtnapi_save_servo_parameter_acs_per_axis_no_label(WB_AXIS_BOND_Z);
			//}
			}
		}
	}
	else
	{
		iRet = MTN_API_ERROR_TUNING_NOT_ALL_PASS;  // 20110601
		if(mtnapi_confirm_para_base_path_exist() == MTN_API_OK_ZERO)
		{
			int iTempRet = mtnapi_init_master_config_acs("C:\\WbData\\ParaBase\\ServoMaster.ini", stServoControllerCommSet.Handle); // must be the same as in MotDLL_LoadW*.cpp
			if(iTempRet  == MTN_API_OK_ZERO) // NOT, mtnapi_init_servo_control_para_acs
			{
				mtn_wb_bond_head_z_update_acs_parameter_servo_struct_multi_blk_w_chksum(WB_BH_2ND_CONTACT + 1); // 20110427
				//	NUM_BONDHEAD_SPEED_PROF_BLK time of mtn_tune_upload_wb_bh_z_speed_profile_blk(stcommhandle, ii);
				mtn_tune_upload_wb_speed_profile(stServoControllerCommSet.Handle); // 20110202, XYZ stCommHandle

				// Backup file name
				char tmpStringPath[256];
				sprintf(tmpStringPath, "%s", stServoACS.stpServoAxis_ACS[WB_AXIS_BOND_Z]->strAxisServoParaFilename);
				sprintf(stServoACS.stpServoAxis_ACS[WB_AXIS_BOND_Z]->strAxisServoParaFilename, "D:\\MT\\PutPassParametersTo_C_WbData_DefParaBase_ctrl_acsc_z.ini");
				mtnapi_save_servo_parameter_acs_per_axis_no_label(WB_AXIS_BOND_Z);
				sprintf(stServoACS.stpServoAxis_ACS[WB_AXIS_BOND_Z]->strAxisServoParaFilename, "%s", tmpStringPath);
			//}
			}
			else
			{
				iRet = iTempRet;
			}
		}
	}

	return iRet;
}

void mtn_wb_table_x_update_acs_parameter_servo_struct_multi_blk(int nTotalBlkInAcs)
{
	int ii;
	int nActualMaxBLk = MAX_BLK_PARAMETER;   // MtnApi.h
	MTN_TUNE_PARAMETER_SET stBestParameterSet;

	if(nActualMaxBLk > nTotalBlkInAcs)
	{
		nActualMaxBLk = nTotalBlkInAcs;
	}
	int nMaxBlkByTuning = mtn_wb_tune_get_max_sector_table_x();
	for(ii = 0; ii<nTotalBlkInAcs; ii++)
	{
		if(ii < nMaxBlkByTuning)
		{
			mtn_wb_copy_acs_para_from_TuningOutParameterSet_to_ServoParaACS(&stServoACS.stpServoAxis_ACS[WB_AXIS_TABLE_X]->stServoParaACS[ii],
				&astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet);
		}
		else
		{
			stBestParameterSet = astMtnTuneSectorOutputTableX[nMaxBlkByTuning - 1][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet;
			stBestParameterSet.dVelLoopKP = _DEF_XY_VKP_STABLE__;
			mtn_wb_copy_acs_para_from_TuningOutParameterSet_to_ServoParaACS(&stServoACS.stpServoAxis_ACS[WB_AXIS_TABLE_X]->stServoParaACS[ii],
				&astMtnTuneSectorOutputTableX[nMaxBlkByTuning - 1][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet);
		}
	}
}

void mtn_wb_table_y_update_acs_parameter_servo_struct_multi_blk(int nTotalBlkInAcs)
{
	int ii;
	int nActualMaxBLk = MAX_BLK_PARAMETER;   // MtnApi.h
	if(nActualMaxBLk > nTotalBlkInAcs) // Protection memeory overuse
	{
		nActualMaxBLk = nTotalBlkInAcs;
	}
	int nMaxBlkByTuning = mtn_wb_tune_get_max_sector_table_y();
	MTN_TUNE_PARAMETER_SET stBestParameterSet;
	for(ii = 0; ii<nTotalBlkInAcs; ii++)
	{
		if(ii < nMaxBlkByTuning)
		{
			mtn_wb_copy_acs_para_from_TuningOutParameterSet_to_ServoParaACS(&stServoACS.stpServoAxis_ACS[WB_AXIS_TABLE_Y]->stServoParaACS[ii],
				&astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet);
		}
		else
		{
			stBestParameterSet = astMtnTuneSectorOutputTableY[nMaxBlkByTuning - 1][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet;
			stBestParameterSet.dVelLoopKP = 180;
			mtn_wb_copy_acs_para_from_TuningOutParameterSet_to_ServoParaACS(&stServoACS.stpServoAxis_ACS[WB_AXIS_TABLE_Y]->stServoParaACS[ii],
				&astMtnTuneSectorOutputTableY[nMaxBlkByTuning - 1][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet);

		}
	}
}

void mtn_wb_tune_consolidate_table_x_servo_para()
{
	MTN_TUNE_PARAMETER_SET stBestParameterSetOneBlkTable_X, stBestParameterSetInt;

	stBestParameterSetOneBlkTable_X.dVelLoopKP = ( astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dVelLoopKP +
		WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dVelLoopKP +
		WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dVelLoopKP) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

	stBestParameterSetOneBlkTable_X.dVelLoopKI = //astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dVelLoopKI;
		( astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dVelLoopKI +
		WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dVelLoopKI +
		WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dVelLoopKI) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

	double adTempPara[3];
	adTempPara[0] = astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dPosnLoopKP;
	adTempPara[1] = astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dPosnLoopKP;
	adTempPara[2] = astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dPosnLoopKP;

	stBestParameterSetOneBlkTable_X.dPosnLoopKP = f_get_max(adTempPara, 3);
		//( astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dPosnLoopKP +
		//WEIGHT_PARA_1ST_BOND_2ND_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dPosnLoopKP +
		//WEIGHT_PARA_1ST_BOND_2ND_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dPosnLoopKP) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

	stBestParameterSetOneBlkTable_X.dAccFFC = ( astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dAccFFC +
		WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dAccFFC +
		WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dAccFFC) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

	stBestParameterSetOneBlkTable_X.dVelLoopLimitI = ( astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dVelLoopLimitI +
		WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dVelLoopLimitI +
		WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dVelLoopLimitI) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

	stBestParameterSetOneBlkTable_X.dSecOrdFilterFreq_Hz = ( astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dSecOrdFilterFreq_Hz +
		WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dSecOrdFilterFreq_Hz +
		WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dSecOrdFilterFreq_Hz) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

	mtn_tune_round_parameter_to_nearest_1(&stBestParameterSetOneBlkTable_X, &stBestParameterSetInt);

	astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet = stBestParameterSetInt;
	astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet = stBestParameterSetInt;
	astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet = stBestParameterSetInt;

	//astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet = astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet;
	//astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet = astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet;
	unsigned int ii, uiMaxSector = MAX_NUM_SECTOR_TBL_XY; // 0120801 mtn_wb_tune_get_max_sector_table_x();
	for(ii =0; ii< uiMaxSector; ii++)
	{
		//astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet = astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet;
		//astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet = astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet;
		stBestParameterSetOneBlkTable_X.dVelLoopKP = ( astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dVelLoopKP +
			WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dVelLoopKP +
			WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dVelLoopKP) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

		// Protection for homing, 20110505, 20120628 totally 2 places need this default value		//
		stBestParameterSetOneBlkTable_X.dVelLoopKI = //astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dVelLoopKI;
			( astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dVelLoopKI +
			WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dVelLoopKI +
			WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dVelLoopKI) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

		adTempPara[0] = astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dPosnLoopKP;
		adTempPara[1] = astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dPosnLoopKP;
		adTempPara[2] = astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dPosnLoopKP;

		stBestParameterSetOneBlkTable_X.dPosnLoopKP = f_get_max(adTempPara, 3);
			//( astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dPosnLoopKP +
			//WEIGHT_PARA_1ST_BOND_2ND_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dPosnLoopKP +
			//WEIGHT_PARA_1ST_BOND_2ND_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dPosnLoopKP) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

		stBestParameterSetOneBlkTable_X.dAccFFC = ( astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dAccFFC +
			WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dAccFFC +
			WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dAccFFC) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

		stBestParameterSetOneBlkTable_X.dVelLoopLimitI = ( astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dVelLoopLimitI +
			WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dVelLoopLimitI +
			WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dVelLoopLimitI) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

		stBestParameterSetOneBlkTable_X.dSecOrdFilterFreq_Hz = ( astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dSecOrdFilterFreq_Hz +
			WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dSecOrdFilterFreq_Hz +
			WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dSecOrdFilterFreq_Hz) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

		mtn_tune_round_parameter_to_nearest_1(&stBestParameterSetOneBlkTable_X, &stBestParameterSetInt);

		astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet = stBestParameterSetInt;
		astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet = stBestParameterSetInt;
		astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet = stBestParameterSetInt;

	}
}

void mtn_wb_tune_consolidate_table_y_servo_para()
{
	MTN_TUNE_PARAMETER_SET stBestParameterSetOneBlkTable_Y, stBestParameterSetInt;

	stBestParameterSetOneBlkTable_Y.dVelLoopKP = ( astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dVelLoopKP +
		WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dVelLoopKP +
		WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dVelLoopKP) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);
// Select the KI, s.t. for 1st Bond
	stBestParameterSetOneBlkTable_Y.dVelLoopKI = //astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dVelLoopKI;
		( astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dVelLoopKI +
		WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dVelLoopKI +
		WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dVelLoopKI) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

	stBestParameterSetOneBlkTable_Y.dPosnLoopKP = ( astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dPosnLoopKP +
		WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dPosnLoopKP +
		WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dPosnLoopKP) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

	stBestParameterSetOneBlkTable_Y.dAccFFC = ( astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dAccFFC +
		WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dAccFFC +
		WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dAccFFC) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

	stBestParameterSetOneBlkTable_Y.dVelLoopLimitI = ( astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dVelLoopLimitI +
		WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dVelLoopLimitI +
		WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dVelLoopLimitI) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

	stBestParameterSetOneBlkTable_Y.dSecOrdFilterFreq_Hz = ( astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dSecOrdFilterFreq_Hz +
		WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dSecOrdFilterFreq_Hz +
		WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dSecOrdFilterFreq_Hz) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

	mtn_tune_round_parameter_to_nearest_1(&stBestParameterSetOneBlkTable_Y, &stBestParameterSetInt);

	astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet = stBestParameterSetInt;
	astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet = stBestParameterSetInt;
	astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet = stBestParameterSetInt;

	unsigned int ii, uiMaxSector = MAX_NUM_SECTOR_TBL_XY; // 20120801 mtn_wb_tune_get_max_sector_table_y();
	for(ii =0; ii< uiMaxSector; ii++)
	{
		stBestParameterSetOneBlkTable_Y.dVelLoopKP = ( astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dVelLoopKP +
			WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dVelLoopKP +
			WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dVelLoopKP) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

// Select the KI, s.t. for 1st Bond
		stBestParameterSetOneBlkTable_Y.dVelLoopKI = astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dVelLoopKI;
		//( astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dVelLoopKI +
		//	WEIGHT_PARA_1ST_BOND_2ND_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dVelLoopKI +
		//	WEIGHT_PARA_1ST_BOND_2ND_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dVelLoopKI) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

		stBestParameterSetOneBlkTable_Y.dPosnLoopKP = ( astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dPosnLoopKP +
			WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dPosnLoopKP +
			WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dPosnLoopKP) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

		stBestParameterSetOneBlkTable_Y.dAccFFC = ( astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dAccFFC +
			WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dAccFFC +
			WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dAccFFC) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

		stBestParameterSetOneBlkTable_Y.dVelLoopLimitI = ( astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dVelLoopLimitI +
			WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dVelLoopLimitI +
			WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dVelLoopLimitI) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

		stBestParameterSetOneBlkTable_Y.dSecOrdFilterFreq_Hz = ( astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet.dSecOrdFilterFreq_Hz +
			WEIGHT_PARA_1ST_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet.dSecOrdFilterFreq_Hz +
			WEIGHT_PARA_2ND_BOND_OVER_LOOPING * astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet.dSecOrdFilterFreq_Hz) / (WEIGHT_PARA_1ST_BOND_OVER_LOOPING + WEIGHT_PARA_2ND_BOND_OVER_LOOPING + 1.0);

		mtn_tune_round_parameter_to_nearest_1(&stBestParameterSetOneBlkTable_Y, &stBestParameterSetInt);

		astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].stBestParameterSet = stBestParameterSetInt;
		astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_TRAJ].stBestParameterSet = stBestParameterSetInt;
		astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_BTO].stBestParameterSet = stBestParameterSetInt;
	}
}


void mtn_tune_get_traj_points(WB_ONE_WIRE_PERFORMANCE_CALC *stpAnalyzeOneWire, 
							  double *adRefVelocityBondHeadZ_Traj, double *adRefVelocityTableX_Traj, double *adRefVelocityTableY_Traj, 
							  int *nTrajLen)
{
	int idxStartTraj, idxEndTraj;

	idxStartTraj = stpAnalyzeOneWire->stTimePointsOfTable.idxStartTrajTo2ndBondX;  // Start from XY table motion
	idxEndTraj = stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxEndTrajectory2ndBondSearchHeight; // End together with Z

	*nTrajLen = idxEndTraj - idxStartTraj + 1;
	for(int ii=  idxStartTraj; ii <= idxEndTraj; ii++)
	{
		adRefVelocityBondHeadZ_Traj[ii - idxStartTraj] = fRefVelZ[ii];
		adRefVelocityTableX_Traj[ii - idxStartTraj] = fRefVelX[ii];
		adRefVelocityTableY_Traj[ii - idxStartTraj] = fRefVelY[ii];
	}
}

void mtn_tune_calc_one_wire_performance(START_ONE_WIRE_TIME_INFO *stpStartOneWireTimeInfo, WB_ONE_WIRE_PERFORMANCE_CALC *stpAnalyzeOneWire)
{
int idxStartOneWireZ = stpStartOneWireTimeInfo->idxStartOneWireZ;
int idxStartOneWireXY = stpStartOneWireTimeInfo->idxStartOneWireXY;

int tLen = LEN_UPLOAD_ARRAY; //stWaveformData.tLen;

stpAnalyzeOneWire->iFlagHasWireInfo = 0;

BOND_HEAD_PERFORMANCE *stpBondHeadPerformance = &(stpAnalyzeOneWire->stBondHeadPerformance);
TIME_POINTS_OF_BONDHEAD_Z *stpTimePointsOfBondHeadZ = &(stpAnalyzeOneWire->stTimePointsOfBondHeadZ);
POSNS_OF_BONDHEAD_Z *stpPosnsOfBondHeadZ = &(stpAnalyzeOneWire->stPosnsOfBondHeadZ);
POSNS_OF_TABLE *stpPosnsOfTable = &(stpAnalyzeOneWire->stPosnsOfTable);
TIME_POINTS_OF_TABLE *stpTimePointsOfTable = &(stpAnalyzeOneWire->stTimePointsOfTable);
TABLE_WB_PERFORMANCE *stpTableXPerformance = &(stpAnalyzeOneWire->stTableXPerformance);
TABLE_WB_PERFORMANCE *stpTableYPerformance = &(stpAnalyzeOneWire->stTableYPerformance);

////FireLevel_Z
////1stSrchHeight_Z
////1stBondReverseHeight
////1stBondKinkHeight
////1stBondLoopTop
////TrajectoryEndHeight_Z = 2ndBond_Search_Height
////2ndBondEndTailHeight
////2ndBondEndFireLevel
int idxStartMove1stBondSearchHeight = 0;
int idxEndMove1stBondSearchHeight = 0;
int idxStart1stBondForceCtrl = 0;
int idxEnd1stBondForceCtrl = 0;
int idxStartReverseHeight = 0;
int idxEndReverseHeight = 0;
int idxStartKinkHeight = 0;
int idxEndKinkHeight = 0;
int idxStartMoveLoopTop = 0;
int idxEndMoveLoopTop = 0;
int idxStartTrajectory = 0;
int idxEndTrajectory2ndBondSearchHeight = 0;
int idxStart2ndBondForceCtrl = 0;
int idxEnd2ndBondForceCtrl = 0;
int idxStartTail = 0;
int idxEndTail = 0;
int idxStartFireLevel = 0;
int idxEndFireLevel = 0;
int idxStart2ndBondSearchContact = 0;

int ii;
//int nCountNumMotionAfter1stBond;
int idxStartCalcDeform1stB;
int idxStartCalcDeform2ndB;

int iFlagBndZGoingDownTrajectory = 0;

	for( ii = idxStartOneWireZ; ii<tLen-2; ii++)
	{
		if( fabs(fRefVelZ[ii]) < EPSLN && fabs(fRefVelZ[ii+1]) >EPSLN && (fRefVelZ[ii+1] < -EPSLN )&& idxStartMove1stBondSearchHeight == 0)  // 20101017
		{
			idxStartMove1stBondSearchHeight = ii+1;  // CANNOT be 0, at least start from 1
			stpPosnsOfBondHeadZ->fStartFireLevel_Z = fRefPosnZ[ii];
		}
		if( fabs(fRefVelZ[ii]) > EPSLN 
			&& ( (fabs(fRefVelZ[ii+1]) < EPSLN) || fabs(fRefVelZ[ii+1] - fRefVelZ[ii]) < EPSLN || fabs(fRefVelZ[ii+2] - fRefVelZ[ii+1]) < EPSLN)   // 20110519
			&& idxStartMove1stBondSearchHeight != 0 && idxEndMove1stBondSearchHeight == 0)   
		{
			idxEndMove1stBondSearchHeight = ii;
			stpPosnsOfBondHeadZ->f1stBondSearchHeight_Z = fRefPosnZ[ii];
		}
		if( (((mtn_tune_is_posn_z_cmd_eq_fb(ii) == FALSE) && (mtn_tune_is_posn_z_cmd_eq_fb(ii+1) == TRUE) )
//			||( fabs(fPosnErrZ[ii] - fPosnErrZ[ii+1]) > EPSLN && fabs(fPosnErrZ[ii+2] - fPosnErrZ[ii+3]) < EPSLN && fabs(fPosnErrZ[ii+1] - fPosnErrZ[ii+2]) < EPSLN ) 
			||( fabs(fRefVelZ[ii]) > EPSLN) && fabs(fRefVelZ[ii+1]) < EPSLN && fabs(fRefVelZ[ii+2]) < EPSLN && fabs(fRefVelZ[ii+3]) < EPSLN && fabs(fRefVelZ[ii+4]) < EPSLN && fabs(fRefVelZ[ii+5]) < EPSLN && fabs(fRefVelZ[ii+6]) < EPSLN && fabs(fRefVelZ[ii+7]) < EPSLN && fabs(fRefVelZ[ii+8]) < EPSLN && fabs(fRefVelZ[ii+9]) < EPSLN && fabs(fRefVelZ[ii+10]) < EPSLN && fabs(fRefVelZ[ii+11]) < EPSLN )  // 20110519
			&& idxEndMove1stBondSearchHeight != 0 && idxStart1stBondForceCtrl == 0)  
		{
			idxStart1stBondForceCtrl = ii;
			stpBondHeadPerformance->dRefDeformPosition1stB = fRefPosnZ[ii+1];  // 20110526
			if((mtn_tune_is_posn_z_cmd_eq_fb(ii) == FALSE) && (mtn_tune_is_posn_z_cmd_eq_fb(ii+1) == TRUE) )
			{
				stpAnalyzeOneWire->fBondHeadSetting_1stB_SrchSafeTH = fabs(fPosnErrZ[ii]);  
				if(stpAnalyzeOneWire->fBondHeadSetting_1stB_SrchSafeTH < 1.0) stpAnalyzeOneWire->fBondHeadSetting_1stB_SrchSafeTH = fabs(fPosnErrZ[ii - 1]);  // 20111011
			}
			else
			{
				if(idxEndMove1stBondSearchHeight == 0 || (idxStart1stBondForceCtrl - idxEndMove1stBondSearchHeight) < 10)
				{
					stpAnalyzeOneWire->iFlagIsDryRun = TRUE;
					stpAnalyzeOneWire->fBondHeadSetting_1stB_SrchSafeTH = 100; // fPosnErrZ[ii];
				}
			}
		}
		if( (((mtn_tune_is_posn_z_cmd_eq_fb(ii) == TRUE) && (mtn_tune_is_posn_z_cmd_eq_fb(ii + 1) == FALSE))   // fFeedPosnZ[ii] == fRefPosnZ[ii], fFeedPosnZ[ii+1] != fRefPosnZ[ii+1]
			  || ( fabs(fPosnErrZ[ii+2] - fPosnErrZ[ii+3]) > EPSLN && fabs(fPosnErrZ[ii+1] - fPosnErrZ[ii+2]) > EPSLN ))   // 20110519
		    && fRefVelZ[ii + 1] >EPSLN && fRefVelZ[ii + 2] >EPSLN && fRefVelZ[ii + 3] >EPSLN
			&& idxStart1stBondForceCtrl != 0 && idxEnd1stBondForceCtrl == 0)
		{
			idxEnd1stBondForceCtrl = ii-1; // NewDSP, better switch from force to position, 20110221
			stpPosnsOfBondHeadZ->f1stBondContactPosn_Z = f_get_mean(&fRefPosnZ[idxStart1stBondForceCtrl], idxEnd1stBondForceCtrl - idxStart1stBondForceCtrl);
		}

		/////////////// 20120214
#define DEF_BND_Z_GOING_DOWN_VEL_CNT_2000Hz              -50
		if( fRefVelZ[ii] <  DEF_BND_Z_GOING_DOWN_VEL_CNT_2000Hz 
			&& iFlagBndZGoingDownTrajectory == 0
			&& idxEnd1stBondForceCtrl != 0)
		{
			iFlagBndZGoingDownTrajectory = 1;
		}
		// mtn_tune_is_ref_vel_z_eq_0(ii) == FALSE && 
		// fRefVelZ[ii+1] == fRefVelZ[ii]  && fRefVelZ[ii+1] == fRefVelZ[ii+2]  mtn_tune_is_ref_vel_z_eq_0(ii + 1) == FALSE

		if( fRefVelZ[ii] <0 && fRefVelZ[ii+1] <0 
			&& ( fabs(fRefVelZ[ii+1] - fRefVelZ[ii]) <0.01  && fabs(fRefVelZ[ii+1] - fRefVelZ[ii+2]) < 0.01 && fabs(fRefVelZ[ii+2] - fRefVelZ[ii+3]) < 0.01 ) // Change from || to &&, 20110221
			&& idxEnd1stBondForceCtrl != 0 && idxStart2ndBondSearchContact == 0 ) // && iFlagBndZGoingDownTrajectory == 1， 20120915
		{
			idxStart2ndBondSearchContact = ii + 1;
			stpPosnsOfBondHeadZ->f2ndBondStartSearchContact_Z = fRefPosnZ[ii];
//			idxEndTrajectory2ndBondSearchHeight = idxStart2ndBondSearchContact - 5;   // 20110602
//			stpPosnsOfBondHeadZ->f2ndBondSearchHeightPosn_Z = fRefPosnZ[idxEndTrajectory2ndBondSearchHeight];  // 20110602
		}
		 ////idxEnd1stBondForceCtrl
		if( (((mtn_tune_is_posn_z_cmd_eq_fb(ii) == FALSE) // (iMotorFlagZ[ii] & 2) == 0) && ((iMotorFlagZ[ii+1] & 2) != 0)//
			 && (mtn_tune_is_posn_z_cmd_eq_fb(ii+1) == TRUE ) 
			 && (mtn_tune_is_posn_z_cmd_eq_fb(ii+2) == TRUE ) 
//			 && (mtn_tune_is_posn_z_cmd_eq_fb(ii+3) == TRUE ) 
//			 && (mtn_tune_is_posn_z_cmd_eq_fb(ii+4) == TRUE ) 
			 ) // && iFlagBndZGoingDownTrajectory == 1
			         /////////////// 20120214
			 ||  (( fabs(fRefVelZ[ii]) > EPSLN) 
					&& fabs(fRefVelZ[ii+1]) < EPSLN && fabs(fRefVelZ[ii+2]) < EPSLN && fabs(fRefVelZ[ii+3]) < EPSLN && fabs(fRefVelZ[ii+4]) < EPSLN 
					&& fabs(fRefVelZ[ii+5]) < EPSLN && fabs(fRefVelZ[ii+6]) < EPSLN && fabs(fRefVelZ[ii+7]) < EPSLN && fabs(fRefVelZ[ii+8]) < EPSLN 
					&& fabs(fRefVelZ[ii+9]) < EPSLN && fabs(fRefVelZ[ii+10]) < EPSLN && fabs(fRefVelZ[ii+11]) < EPSLN && fabs(fRefVelZ[ii+12]) < EPSLN
					&& fabs(fRefVelZ[ii+13]) < EPSLN && fabs(fRefVelZ[ii+14]) < EPSLN && fabs(fRefVelZ[ii+15]) < EPSLN && fabs(fRefVelZ[ii+16]) < EPSLN)  // 20110519
					&& idxStart2ndBondSearchContact == 0 )  // 20110519  // Enlarge to 16 samples for dry-run BUT NOT loop-top delay  && iFlagBndZGoingDownTrajectory == 1
			&& idxEnd1stBondForceCtrl != 0 && idxStart2ndBondForceCtrl == 0  )
		{
			idxStart2ndBondForceCtrl = ii+1;
			stpBondHeadPerformance->dRefDeformPosition2ndB = fRefPosnZ[ii+1];  // 20110526
			// 20110519
			if((mtn_tune_is_posn_z_cmd_eq_fb(ii) == FALSE) // (iMotorFlagZ[ii] & 2) == 0) && ((iMotorFlagZ[ii+1] & 2) != 0)//
			 && (mtn_tune_is_posn_z_cmd_eq_fb(ii+1) == TRUE ) 
			 && (mtn_tune_is_posn_z_cmd_eq_fb(ii+2) == TRUE ) 
//			 && (mtn_tune_is_posn_z_cmd_eq_fb(ii+3) == TRUE ) 
//			 && (mtn_tune_is_posn_z_cmd_eq_fb(ii+4) == TRUE ) 
			)
			{
				stpAnalyzeOneWire->fBondHeadSetting_2ndB_SrchSafeTH = fabs(fPosnErrZ[ii]);  // 20111011
			}
			else
			{
				idxStart2ndBondSearchContact = idxStart2ndBondForceCtrl;
				idxEndTrajectory2ndBondSearchHeight = idxStart2ndBondSearchContact;  // 20110602
				stpAnalyzeOneWire->iFlagIsDryRun = TRUE;
			} // 20110519
		}
		if( (((fabs(fFeedPosnZ[ii] - fRefPosnZ[ii])< EPSLN) && (fabs(fFeedPosnZ[ii+1] - fRefPosnZ[ii+1]) >= EPSLN))  //((iMotorFlagZ[ii] & 2) != 0) && ((iMotorFlagZ[ii+1] & 2) == 0)
			     || ( fabs(fPosnErrZ[ii+2] - fPosnErrZ[ii+3]) > EPSLN && fabs(fPosnErrZ[ii+1] - fPosnErrZ[ii+2]) > EPSLN )
		     )   // 20110519
		    && fRefVelZ[ii + 1] >EPSLN && fRefVelZ[ii + 2] >EPSLN && fRefVelZ[ii + 3] >EPSLN
			&& idxStart2ndBondForceCtrl != 0 && idxEnd2ndBondForceCtrl == 0)
		{
			idxEnd2ndBondForceCtrl = ii - 1;  // NewDSP, better switch from force to position, 20110221			
			break;
		}
		if(idxEndMove1stBondSearchHeight != 0 && fabs(fRefPosnZ[ii] - stpPosnsOfBondHeadZ->fStartFireLevel_Z) < EPSLN)
		{
			idxEndFireLevel = ii;
			break;
		}
	}

	if(idxEnd2ndBondForceCtrl != 0)
	{

		ii = idxEnd2ndBondForceCtrl; 
		while(ii<tLen-2 && idxEndFireLevel == 0)
		{
			if( fabs(fRefVelZ[ii]) < EPSLN && (fRefVelZ[ii+1] >EPSLN) && (fRefVelZ[ii+2] > EPSLN )&& idxStartTail == 0)  // 20101017
			{
				idxStartTail = ii;
			}
			if( fabs(fRefVelZ[ii]) > EPSLN && ( (fabs(fRefVelZ[ii+1]) < EPSLN) || (fRefVelZ[ii+1] <  fRefVelZ[ii] && fRefVelZ[ii+1] <  fRefVelZ[ii + 2])) 
				&& idxStartTail != 0 && idxEndTail == 0)
			{
				idxEndTail = ii + 1;
				stpPosnsOfBondHeadZ->fTailPosn_Z = fRefPosnZ[ii];
				// 20111007
				if(fabs(stpPosnsOfBondHeadZ->fTailPosn_Z - stpPosnsOfBondHeadZ->fStartFireLevel_Z) < 0.5)
				{ // No Tail motion
					idxStartFireLevel = idxStartTail;
					idxEndFireLevel = idxEndTail;
					idxEndTail = idxStartTail;
					stpPosnsOfBondHeadZ->fEndFireLevel_Z = stpPosnsOfBondHeadZ->fTailPosn_Z;
					stpPosnsOfBondHeadZ->fTailPosn_Z = fRefPosnZ[idxStartTail];

					break;
				}
				// 20111007
			}
			if( (fabs(fRefVelZ[ii]) < EPSLN || (fRefVelZ[ii+1] <  fRefVelZ[ii] && fRefVelZ[ii+1] <  fRefVelZ[ii + 2]) )
				&& fabs(fRefVelZ[ii+1]) >EPSLN && (fRefVelZ[ii+1] > EPSLN )
				&& idxEndTail != 0 && idxStartFireLevel == 0)  // 20101017
			{
				idxStartFireLevel = ii;
			}
			if( fabs(fRefVelZ[ii]) > EPSLN && ( (fabs(fRefVelZ[ii+1]) < EPSLN) || fabs(fRefVelZ[ii+1] - fRefVelZ[ii]) < EPSLN) 
				&& idxStartFireLevel != 0 && idxEndFireLevel == 0)
			{
				idxEndFireLevel = ii+1;
				stpPosnsOfBondHeadZ->fEndFireLevel_Z = fRefPosnZ[ii+1];
			}
			ii++;
		}

		// Calculate Deformation Rate for actual bonding // 20110526
		idxStartCalcDeform1stB = idxEndMove1stBondSearchHeight + 2;
		for(ii= idxStart1stBondForceCtrl - 2; ii> idxEndMove1stBondSearchHeight + 2; ii--)
		{
			if(((fRefVelZ[ii] - fFeedVelZ[ii])/ fabs(fRefVelZ[ii])) > -0.2 ) // || fabs
//				fFeedVelZ[ii - 1] > fFeedVelZ[ii]      // 20111025 
//				)
			{
				idxStartCalcDeform1stB = ii - 1;
				break;
			}
		}
		for(ii = idxStartCalcDeform1stB; ii< idxStartCalcDeform1stB + MAX_LEN_FORCE_CTRL_MS - 1; ii++)
		{
			if(ii < idxEnd1stBondForceCtrl)
			{
				stpBondHeadPerformance->adDeformPosn1stB[ii - idxStartCalcDeform1stB] = fFeedPosnZ[ii] - stpBondHeadPerformance->dRefDeformPosition1stB;
			}
			else
			{
				stpBondHeadPerformance->adDeformPosn1stB[ii - idxStartCalcDeform1stB] = 0;
			}
		}
		idxStartCalcDeform2ndB = idxStart2ndBondSearchContact + 2;
		for(ii= idxStart2ndBondForceCtrl - 3; ii> idxStart2ndBondSearchContact + 2; ii--)
		{
			if(((fRefVelZ[ii] - fFeedVelZ[ii]) / fabs(fRefVelZ[ii])) > - 0.2 ) //|| fabs
//				(fFeedVelZ[ii - 1] > fFeedVelZ[ii])      // 20111025 
//				)
			{
				idxStartCalcDeform2ndB = ii - 1;
				break;
			}
		}
		for(ii = idxStartCalcDeform2ndB; ii< idxStartCalcDeform2ndB + MAX_LEN_FORCE_CTRL_MS - 1; ii++)
		{
			if(ii < idxEnd2ndBondForceCtrl)
			{
				stpBondHeadPerformance->adDeformPosn2ndB[ii - idxStartCalcDeform2ndB] = fFeedPosnZ[ii] - stpBondHeadPerformance->dRefDeformPosition2ndB;
			}
			else
			{
				stpBondHeadPerformance->adDeformPosn2ndB[ii - idxStartCalcDeform2ndB] = 0 ;
			}
		}
		stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxStartCalcDeform1stB =	idxStartCalcDeform1stB;
		stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxStartCalcDeform2ndB = idxStartCalcDeform2ndB;
		// 20110526
	}

	// 20110808
	if(idxStart1stBondForceCtrl == 0 || idxStart2ndBondForceCtrl ==0 || idxStart2ndBondSearchContact == 0)
	{
		stpAnalyzeOneWire->iFlagHasWireInfo = 0;
		return ;
	}

	if(idxStart2ndBondSearchContact >= idxStart2ndBondForceCtrl || idxStart2ndBondSearchContact == 0)
	{
		stpAnalyzeOneWire->iFlagIsDryRun = TRUE;
		if(idxEnd1stBondForceCtrl == 0)
		{
			idxEnd1stBondForceCtrl = idxEndMove1stBondSearchHeight;
		}
		if(idxStart2ndBondSearchContact == 0)
		{
			idxStart2ndBondSearchContact = idxStart2ndBondForceCtrl;
		}
	}
	else
		stpAnalyzeOneWire->iFlagIsDryRun = FALSE;

	if(iFlagBndZGoingDownTrajectory == 0)
	{
		stpAnalyzeOneWire->iFlagBallBond = TRUE;
	}
	else
	{
		stpAnalyzeOneWire->iFlagBallBond = FALSE;
	}

	if(stpAnalyzeOneWire->iFlagIsDryRun == FALSE)
	{
		// Judge False Contact
		for(ii = idxStart1stBondForceCtrl - 2; ii< idxStart1stBondForceCtrl + 5; ii ++)
		{
			if(fabs(fRefVelZ[ii + 1] - fRefVelZ[ii]) > 0.1)
			{
				if(fRefVelZ[ii + 1] > fRefVelZ[ii])
				{
					stpAnalyzeOneWire->stBondHeadPerformance.iFlagIsFalseContact1stB = FALSE;
				}
				else
				{
					stpAnalyzeOneWire->stBondHeadPerformance.iFlagIsFalseContact1stB = TRUE;
				}
				break;
			}
		}
		for(ii = idxStart2ndBondForceCtrl - 2; ii< idxStart2ndBondForceCtrl + 5; ii ++)
		{  // iFlagIsFalseContact2ndB
			if(fabs(fRefVelZ[ii + 1] - fRefVelZ[ii]) > 0.1)
			{
				if(fRefVelZ[ii + 1] > fRefVelZ[ii])
				{
					stpAnalyzeOneWire->stBondHeadPerformance.iFlagIsFalseContact2ndB = FALSE;
				}
				else
				{
					stpAnalyzeOneWire->stBondHeadPerformance.iFlagIsFalseContact2ndB = TRUE;
				}
				break;
			}
		}
	}
 ////// Motion After 1st Bond and Before going to reset level
ONE_MOTION_INFO *astBondHeadMotionAft1stBond[NUM_MOTION_AFTER_1ST_BOND_FORCE_WITHIN_ONE_WIRE];

	for(ii = 0; ii<NUM_MOTION_AFTER_1ST_BOND_FORCE_WITHIN_ONE_WIRE; ii++)
	{
		astBondHeadMotionAft1stBond[ii] = &stpAnalyzeOneWire->astBondHeadMotionAft1stBond[ii];

		// initialize
		astBondHeadMotionAft1stBond[ii]->idxStartMotion = 0;
		astBondHeadMotionAft1stBond[ii]->idxEndMotion = 0;
		astBondHeadMotionAft1stBond[ii]->fMotionTargetPosn = stpPosnsOfBondHeadZ->f1stBondSearchHeight_Z;
		astBondHeadMotionAft1stBond[ii]->fMotionStartPosn = stpPosnsOfBondHeadZ->f1stBondSearchHeight_Z;

	}
int idxMotionAft1stBond = 0;
	astBondHeadMotionAft1stBond[idxMotionAft1stBond]->idxStartMotion = 0;
	astBondHeadMotionAft1stBond[idxMotionAft1stBond]->idxEndMotion = 0;
	astBondHeadMotionAft1stBond[idxMotionAft1stBond]->fMotionTargetPosn = stpPosnsOfBondHeadZ->f1stBondSearchHeight_Z;
	astBondHeadMotionAft1stBond[idxMotionAft1stBond]->fMotionStartPosn = stpPosnsOfBondHeadZ->f1stBondSearchHeight_Z;
int idxTrajMotion = -1, idxLoopTopMotion = -1, idxRevDistMotion = -1, idxRevHightMotion=-1;

	for( ii = idxEnd1stBondForceCtrl - 1; ii<tLen-2; ii++)  // 20110220, need some buffer
	{
			int iTemp1, iTemp2, iTemp3; 
			double fChangeSignVelocity;
			iTemp1 = mtn_tune_is_ref_vel_z_eq_0(ii);
			iTemp2 = mtn_tune_is_ref_vel_z_eq_0(ii+1);
			iTemp3 =  (int)(fRefVelZ[ii+2] - fRefVelZ[ii+1]);
			fChangeSignVelocity = fRefVelZ[ii] * fRefVelZ[ii + 1];  // 20110221
 //|| ((fRefVelZ[ii-1] * fRefVelZ[ii]) <= 0.0)
		if( (mtn_tune_is_ref_vel_z_less_1(ii) == TRUE )&&    // mtn_tune_is_ref_vel_z_eq_0
			mtn_tune_is_ref_vel_z_eq_0(ii+1) == FALSE && 
			fRefVelZ[ii+2] != fRefVelZ[ii+1] && fRefVelZ[ii+4] != fRefVelZ[ii+3] &&
			astBondHeadMotionAft1stBond[idxMotionAft1stBond]->idxStartMotion == 0 
				&& 
				(((ii <= idxStart2ndBondSearchContact - 5 || ii >= idxEnd2ndBondForceCtrl ) && stpAnalyzeOneWire->iFlagIsDryRun == FALSE)  // Prevent pattern, [-44, -26, -10, 0, -12idxStart2ndBondSearchContact - 5
				|| ((ii <= idxEndFireLevel || ii >= idxEnd2ndBondForceCtrl) && stpAnalyzeOneWire->iFlagIsDryRun == TRUE )
				)   
          )
		{
			astBondHeadMotionAft1stBond[idxMotionAft1stBond]->idxStartMotion = ii;
			astBondHeadMotionAft1stBond[idxMotionAft1stBond]->fMotionStartPosn = fRefPosnZ[ii];
		}
		//// either stop motion or jogging, or invert direction  // (fRefVelZ[ii+1] * fRefVelZ[ii]) <= 0.0), mtn_tune_is_ref_vel_z_less_1(ii+1) == TRUE 
		if( mtn_tune_is_ref_vel_z_eq_0(ii) == FALSE &&   // mtn_tune_is_ref_vel_z_abs_min_local(ii + 1) == TRUE
				(mtn_tune_is_ref_vel_z_eq_0(ii + 1) == TRUE ||   ( fChangeSignVelocity < 0) || 
						 (fRefVelZ[ii+2] == fRefVelZ[ii+1] && fRefVelZ[ii+3] == fRefVelZ[ii+2]) )  // mtn_tune_is_ref_vel_z_abs_min_local(ii) == TRUE || 
						&&  // mtn_tune_is_ref_vel_z_eq_0
				(astBondHeadMotionAft1stBond[idxMotionAft1stBond]->idxStartMotion != 0) && (astBondHeadMotionAft1stBond[idxMotionAft1stBond]->idxEndMotion == 0 )
				   && (ii <= idxStart2ndBondSearchContact || ii >= idxEnd2ndBondForceCtrl))
		{
			astBondHeadMotionAft1stBond[idxMotionAft1stBond]->idxEndMotion = ii + 1;
			astBondHeadMotionAft1stBond[idxMotionAft1stBond]->fMotionTargetPosn = fRefPosnZ[ii+1];
			//////
			if( (fabs(astBondHeadMotionAft1stBond[idxMotionAft1stBond]->fMotionTargetPosn - stpPosnsOfBondHeadZ->fStartFireLevel_Z)< 1.0)
				|| fabs(astBondHeadMotionAft1stBond[idxMotionAft1stBond]->fMotionTargetPosn - stpPosnsOfBondHeadZ->fEndFireLevel_Z) < 1.0
				) // EPSLN
			{
				if(fabs(astBondHeadMotionAft1stBond[idxMotionAft1stBond]->fMotionTargetPosn - stpPosnsOfBondHeadZ->fEndFireLevel_Z) < 1.0 )
				{
					idxTrajMotion = idxMotionAft1stBond -2, idxLoopTopMotion = idxMotionAft1stBond-3, idxRevDistMotion = idxMotionAft1stBond-4, idxRevHightMotion= idxMotionAft1stBond-5;
				}
				break;
			}
			else
			{
				idxMotionAft1stBond = idxMotionAft1stBond + 1;
				astBondHeadMotionAft1stBond[idxMotionAft1stBond]->idxEndMotion = 0;
				astBondHeadMotionAft1stBond[idxMotionAft1stBond]->fMotionStartPosn = astBondHeadMotionAft1stBond[idxMotionAft1stBond-1]->fMotionTargetPosn;
				astBondHeadMotionAft1stBond[idxMotionAft1stBond]->fMotionTargetPosn = astBondHeadMotionAft1stBond[idxMotionAft1stBond]->fMotionStartPosn;
				if( fChangeSignVelocity < 0 ) // (fRefVelZ[ii+1] * fRefVelZ[ii]) <= 0.0) // 20110221  // mtn_tune_is_ref_vel_z_less_1(ii+1) == TRUE || 
				{   // some kinds of special motion ending, Next motion already start next sample
					// 1. end velocity < 1.0， ||mtn_tune_is_ref_vel_z_abs_min_local(ii + 1) == TRUE
					// 2. velocity change sign  
					astBondHeadMotionAft1stBond[idxMotionAft1stBond]->idxStartMotion = ii + 1;
				}
				else
				{
					astBondHeadMotionAft1stBond[idxMotionAft1stBond]->idxStartMotion = 0;  // 20110221
				}
			}
			if(idxMotionAft1stBond == 7) // Maximum 6 motions after 1st Bond ForceControl
			{
				idxMotionAft1stBond = 6;
				break;
			}

		}
		if(idxMotionAft1stBond >= (NUM_MOTION_AFTER_1ST_BOND_FORCE_WITHIN_ONE_WIRE -1)) // 20110301
		{
			break;
		}
	}
	int nBondHeadMotionAfter1stBond = idxMotionAft1stBond;
	int idxEndOneWireZ = astBondHeadMotionAft1stBond[nBondHeadMotionAfter1stBond]->idxEndMotion;

	if(idxMotionAft1stBond <= 1)
	{
		stpAnalyzeOneWire->iFlagHasWireInfo = 0;
		return ;
	}
////// 
	if(idxStartTail == 0)
	{
		idxStartTail = astBondHeadMotionAft1stBond[nBondHeadMotionAfter1stBond-1]->idxStartMotion;
		idxEndTail = astBondHeadMotionAft1stBond[nBondHeadMotionAfter1stBond-1]->idxEndMotion;
		stpPosnsOfBondHeadZ->fTailPosn_Z = astBondHeadMotionAft1stBond[nBondHeadMotionAfter1stBond-1]->fMotionTargetPosn;
	}
	if(idxStartFireLevel == 0)
	{
		idxStartFireLevel = astBondHeadMotionAft1stBond[nBondHeadMotionAfter1stBond]->idxStartMotion;
		idxEndFireLevel = astBondHeadMotionAft1stBond[nBondHeadMotionAfter1stBond]->idxEndMotion;
		stpPosnsOfBondHeadZ->fEndFireLevel_Z = astBondHeadMotionAft1stBond[nBondHeadMotionAfter1stBond]->fMotionTargetPosn;
	}


	// Back track from idxStart2ndBondSearchContact to idxEnd1stBondForceCtrl
	for(ii = idxStart2ndBondSearchContact - 1; ii> idxEnd1stBondForceCtrl; ii--)
	{
		if(idxEndTrajectory2ndBondSearchHeight == 0 
			&& (fabs(fRefVelZ[ii]) < EPSLN || (fRefVelZ[ii] > fRefVelZ[ii-1] && fRefVelZ[ii] > fRefVelZ[ii+1]))
			&& fRefVelZ[ii-1] < - EPSLN
//			&& (fRefVelZ[ii+1] == 0 && 
//				(fabs(fRefVelZ[ii+1] - fRefVelZ[ii+2]) < EPSLN || fabs(fRefVelZ[ii+2] - fRefVelZ[ii+3]) < EPSLN || fabs(fRefVelZ[ii+3] - fRefVelZ[ii+4]) < EPSLN || fabs(fRefVelZ[ii+4] - fRefVelZ[ii+5]) < EPSLN)
//				)
		   )
		{
			idxEndTrajectory2ndBondSearchHeight = ii;
			stpPosnsOfBondHeadZ->f2ndBondSearchHeightPosn_Z = fRefPosnZ[ii];
		}
		if(idxEndTrajectory2ndBondSearchHeight != 0 && idxStartTrajectory ==0 
			&& (fabs(fRefVelZ[ii]) < EPSLN || fRefVelZ[ii] >EPSLN ) 
			&& fRefVelZ[ii+1] < -EPSLN 
			&& (fabs(fRefVelZ[ii-1]) < EPSLN  || fRefVelZ[ii-1] > EPSLN )
			)
		{
			idxStartTrajectory = ii;
		}

		if(idxStartTrajectory != 0 && idxEndMoveLoopTop == 0 && fRefVelZ[ii] > EPSLN && (fRefVelZ[ii+1] <= 0 ))
		{
			idxEndMoveLoopTop = ii;
			stpPosnsOfBondHeadZ->fLoopTopPosn_Z = fRefPosnZ[ii];
		}
		if(idxEndMoveLoopTop != 0 && idxStartMoveLoopTop ==0 && fRefVelZ[ii+1] > EPSLN  
			&& ( (fabs(fRefVelZ[ii]) < EPSLN) || (fRefVelZ[ii] * fRefVelZ[ii+1] < 0) || (fRefVelZ[ii] < fRefVelZ[ii-1] && fRefVelZ[ii] < fRefVelZ[ii+1]) 
			   ) 
		  )
		{
			idxStartMoveLoopTop = ii;
			idxStartKinkHeight = ii;
			idxEndKinkHeight = ii;
			stpPosnsOfBondHeadZ->fKinkHeightPosn_Z = fRefPosnZ[ii];

			idxStartReverseHeight = idxEnd1stBondForceCtrl + 1;
			idxEndReverseHeight = ii;
			stpPosnsOfBondHeadZ->fReverseHeightPosn_Z = fRefPosnZ[ii];

		}
	}

	stpAnalyzeOneWire->nTotalBondHeadZ_MotionAft1stBond = nBondHeadMotionAfter1stBond + 1;

	////// (stpAnalyzeOneWire->iFlagIsDryRun == FALSE) && (idxEnd1stBondForceCtrl == 0 || idxStart2ndBondForceCtrl == 0 || idxEnd2ndBondForceCtrl == 0)
	if( idxStartMove1stBondSearchHeight == 0 || idxStartTail == 0 || idxEndTail == 0 || idxStartFireLevel == 0 || idxEndFireLevel == 0)  // 20100726
	{
		return; // stAnalyzeOneWire;
	}
	else
	{
		stpAnalyzeOneWire->iFlagHasWireInfo = 1;
		if(stpAnalyzeOneWire->iFlagIsDryRun == TRUE)
		{
			stpPosnsOfBondHeadZ->f1stBondContactPosn_Z = stpPosnsOfBondHeadZ->f1stBondSearchHeight_Z;
			stpPosnsOfBondHeadZ->f2ndBondContactPosn_Z = stpPosnsOfBondHeadZ->f2ndBondSearchHeightPosn_Z;
		}
		else
		{
			stpPosnsOfBondHeadZ->f1stBondContactPosn_Z = f_get_mean(&fRefPosnZ[idxStart1stBondForceCtrl], (idxEnd1stBondForceCtrl - idxStart1stBondForceCtrl));
			stpPosnsOfBondHeadZ->f2ndBondContactPosn_Z = f_get_mean(&fRefPosnZ[idxStart2ndBondForceCtrl], (idxEnd2ndBondForceCtrl - idxStart2ndBondForceCtrl));
		}
	}
#ifdef __TBD__
#endif // __TBD__
//// -1 : moving down
//// 1: Moving up
//// +: Undershoot
//// -: Overshoot
stpBondHeadPerformance->f1stBondMoveToSearchHeightOverUnderShoot = (fRefPosnZ[idxEndMove1stBondSearchHeight] - fFeedPosnZ[idxEndMove1stBondSearchHeight]) * (-1);
//af1stBondForceCtrlFbPosn = fFeedPosnZ(idxStart1stBondForceCtrl:idxEnd1stBondForceCtrl);
int nLen1stBondForceCtrl = idxEnd1stBondForceCtrl - idxStart1stBondForceCtrl;
stpBondHeadPerformance->f1stBondForceCtrlPosnRipplePtoP = f_get_max(&fFeedPosnZ[idxStart1stBondForceCtrl], nLen1stBondForceCtrl) - f_get_min(&fFeedPosnZ[idxStart1stBondForceCtrl], nLen1stBondForceCtrl);
double fMeanPosn1stBond = f_get_mean(&fFeedPosnZ[idxStart1stBondForceCtrl], nLen1stBondForceCtrl);
stpBondHeadPerformance->f1stBondForceCtrlPosnRippleStd = f_get_std(&fFeedPosnZ[idxStart1stBondForceCtrl], fMeanPosn1stBond, nLen1stBondForceCtrl);
//stpBondHeadPerformance->af1stBondForceCtrlFbPosn = af1stBondForceCtrlFbPosn;

stpBondHeadPerformance->fReverseHeightOverUnderShoot = fRefPosnZ[idxEndReverseHeight] - fFeedPosnZ[idxEndReverseHeight];
stpBondHeadPerformance->fKinkHeightOverUnderShoot = (fRefPosnZ[idxEndKinkHeight] - fFeedPosnZ[idxEndKinkHeight]) * f_get_sign(fRefVelZ[idxEndKinkHeight - 2]);
stpBondHeadPerformance->fLoopTopOverUnderShoot = fRefPosnZ[idxEndMoveLoopTop] - fFeedPosnZ[idxEndMoveLoopTop];
stpBondHeadPerformance->f2ndBondMoveToSearchHeightOverUnderShoot = (fRefPosnZ[idxEndTrajectory2ndBondSearchHeight] - fFeedPosnZ[idxEndTrajectory2ndBondSearchHeight])*(-1);
stpBondHeadPerformance->fTailOverUnderShoot = fRefPosnZ[idxEndTail] - fFeedPosnZ[idxEndTail];
stpBondHeadPerformance->fEndFireLevelOverUnderShoot = fRefPosnZ[idxEndFireLevel] - fFeedPosnZ[idxEndFireLevel];

//af2ndBondForceCtrlFbPosn = fFeedPosnZ(idxStart2ndBondForceCtrl:idxEnd2ndBondForceCtrl);
int nLen2ndBondForceCtrl = idxEnd2ndBondForceCtrl - idxStart2ndBondForceCtrl;

stpBondHeadPerformance->f2ndBondForceCtrlPosnRipplePtoP = f_get_max(&fRefPosnZ[idxStart2ndBondForceCtrl], nLen2ndBondForceCtrl) - f_get_min(&fRefPosnZ[idxStart2ndBondForceCtrl], nLen2ndBondForceCtrl);
double fMeanPosn2ndBond = f_get_mean(&fRefPosnZ[idxStart2ndBondForceCtrl], nLen2ndBondForceCtrl);
stpBondHeadPerformance->f2ndBondForceCtrlPosnRippleStd = f_get_std(&fRefPosnZ[idxStart2ndBondForceCtrl], fMeanPosn2ndBond, nLen2ndBondForceCtrl);
//stpBondHeadPerformance->af2ndBondForceCtrlFbPosn = af2ndBondForceCtrlFbPosn;

double dFeedAccSOS;  dFeedAccSOS = 0;

	// Search Ripple of FbVelocity, 1st Bond
	for(ii = idxEndMove1stBondSearchHeight; ii< idxStart1stBondForceCtrl - 2; ii++)
	{
		stpBondHeadPerformance->af1stBondSearchFbVel[ii - idxEndMove1stBondSearchHeight] = fFeedVelZ[ii];
		stpBondHeadPerformance->af1stBondSearchVelErr[ii - idxEndMove1stBondSearchHeight] = fRefVelZ[ii] - fFeedVelZ[ii];

		dFeedAccSOS = dFeedAccSOS + fFeedAccZ[ii] * fFeedAccZ[ii];
	}
	int nLenSearch = idxStartCalcDeform1stB - 1 - idxEndMove1stBondSearchHeight;  // idxStart1stBondForceCtrl, 20120214
	stpBondHeadPerformance->f1stBondSearchVelErrP2PAfterSrchDelay = f_get_max( &stpBondHeadPerformance->af1stBondSearchVelErr[10], nLenSearch - 10) 
		- f_get_min(&stpBondHeadPerformance->af1stBondSearchVelErr[10], nLenSearch - 10);

	stpBondHeadPerformance->f1stBondSearchFeedAccRMS = sqrt(dFeedAccSOS / (double) nLenSearch);

	stpBondHeadPerformance->f1stBondVelErrAtMoveSrchHeight = fRefVelZ[idxEndMove1stBondSearchHeight] - fFeedVelZ[idxEndMove1stBondSearchHeight];//20110524
    // 2nd Bond
	dFeedAccSOS = 0;
	int idxStartRecord2ndSrch;
	if(idxStart2ndBondSearchContact != 0)
	{
		idxStartRecord2ndSrch = idxStart2ndBondSearchContact;
	}
	else
	{
		idxStartRecord2ndSrch = idxEndTrajectory2ndBondSearchHeight + 4;
	}
	for(ii = idxStartRecord2ndSrch; ii< idxStart2ndBondForceCtrl - 2; ii++)
	{
		stpBondHeadPerformance->af2ndBondSearchFbVel[ii - idxStartRecord2ndSrch] = fFeedVelZ[ii];
		stpBondHeadPerformance->af2ndBondSearchVelErr[ii - idxStartRecord2ndSrch] = fRefVelZ[ii] - fFeedVelZ[ii];
		dFeedAccSOS = dFeedAccSOS + fFeedAccZ[ii] * fFeedAccZ[ii];
	}

	nLenSearch = idxStartCalcDeform2ndB - 1 - idxStartRecord2ndSrch;  // idxStart2ndBondForceCtrl, 20120214,  idxStart2ndBondSearchContact
	stpBondHeadPerformance->f2ndBondSearchVelErrP2PAfterSrchDelay = f_get_max( &stpBondHeadPerformance->af2ndBondSearchVelErr[10], nLenSearch - 10) 
		- f_get_min(&stpBondHeadPerformance->af2ndBondSearchVelErr[10], nLenSearch - 10);
	stpBondHeadPerformance->f2ndBondSearchFeedAccRMS = sqrt(dFeedAccSOS / (double) nLenSearch);

	stpBondHeadPerformance->f2ndBondVelErrAtTrajEnd = fRefVelZ[idxEndTrajectory2ndBondSearchHeight] - fFeedVelZ[idxEndTrajectory2ndBondSearchHeight]; //20110524

//////// Table
//// StartPosition: Table_X, Table_Y
//// FireLevel_Z
//// 1stBondPosition, Table_X, Table_Y
//// 1stSrchHeight_Z
//// 1stBondReverseHeight
//// 1stBondReverseDist, 1stBondReversePosition, Table_X, Table_Y
//// 1stBondKinkHeight
//// 1stBondLoopTop
//// 1stBondMoveTo2ndBondPosition, Trajectory Table_X, Table_Y
//// TrajectoryEndHeight_Z = 2ndBond_Search_Height
//// 2ndBondEndTailHeight
//// 2ndBondEndFireLevel

int	idxStartMoveX1stBond = 0;
int	idxStartMoveY1stBond = 0;
int	idxEndMoveX_1stBondPosn = 0;
int	idxEndMoveY_1stBondPosn = 0;
int	idxStartReverseMoveX = 0;
int	idxStartReverseMoveY = 0;
int	idxEndReverseMoveX = 0;
int	idxEndReverseMoveY = 0;
int	idxStartTrajTo2ndBondX = 0;
int	idxStartTrajTo2ndBondY = 0;
int	idxEndTraj2ndBondPosnX = 0;
int	idxEndTraj2ndBondPosnY = 0;
int	idxStartMoveNextWirePR_X = 0;
int	idxStartMoveNextWirePR_Y = 0;
int	idxEndMoveNextWirePR_X = 0;
int	idxEndMoveNextWirePR_Y = 0;
int	idxStartMoveNextPR_X = 0;
int	idxEndMoveNextPR_X = 0;
int	idxStartMoveNextPR_Y = 0;
int	idxEndMoveNextPR_Y = 0;

int  nNumPullMotionX = 0;
int  nNumPullMotionY = 0;

	stpPosnsOfTable->fStartPosition_X	=	0;
	stpPosnsOfTable->fStartPosition_Y	=	0;
	stpPosnsOfTable->f1stBondPosition_X	=	fRefPosnX[idxStart1stBondForceCtrl];
	stpPosnsOfTable->f1stBondPosition_Y	=	fRefPosnY[idxStart1stBondForceCtrl];
	stpPosnsOfTable->f1stBondReversePosition_X	=	0;
	stpPosnsOfTable->f1stBondReversePosition_Y	=	0;
	stpPosnsOfTable->fTrajEnd2ndBondPosn_X	=	fRefPosnX[idxStart2ndBondForceCtrl];
	stpPosnsOfTable->fTrajEnd2ndBondPosn_Y	=	fRefPosnY[idxStart2ndBondForceCtrl];
	stpPosnsOfTable->fMoveToNextPR_Posn_X	=	fRefPosnX[idxStart2ndBondForceCtrl];
	stpPosnsOfTable->fMoveToNextPR_Posn_Y	=	fRefPosnY[idxStart2ndBondForceCtrl];

	ii = idxStartOneWireXY; //// :1:idxEndOneWireZ-1
	while( ii < tLen && (idxEndMoveNextPR_Y == 0 || idxEndMoveNextPR_X == 0) && (ii < idxEndOneWireZ)) //// idxEndOneWireZ-1
	{
		if( mtn_tune_is_ref_vel_x_eq_0(ii) == TRUE && mtn_tune_is_ref_vel_x_eq_0(ii+1) == FALSE && idxStartMoveX1stBond == 0 
			&& fabs(stpPosnsOfTable->fStartPosition_X) < 1.0)
		{
			idxStartMoveX1stBond = ii;
			stpPosnsOfTable->fStartPosition_X = fRefPosnX[ii];
		}
		if( mtn_tune_is_ref_vel_y_eq_0(ii) == TRUE && mtn_tune_is_ref_vel_y_eq_0(ii+1) == FALSE && idxStartMoveY1stBond == 0
			&& fabs(stpPosnsOfTable->fStartPosition_Y) < 1.0)
		{
			idxStartMoveY1stBond = ii;
			stpPosnsOfTable->fStartPosition_Y = fRefPosnY[ii];
		}
		if( mtn_tune_is_ref_vel_x_eq_0(ii) == FALSE && mtn_tune_is_ref_vel_x_eq_0(ii+1) == TRUE &&
			(fabs(fRefPosnX[ii+1] - stpPosnsOfTable->f1stBondPosition_X) < 2) && 
			(idxStartMoveX1stBond !=0 || fabs(stpPosnsOfTable->fStartPosition_X) > 1.0) && idxEndMoveX_1stBondPosn == 0)
		{
			idxEndMoveX_1stBondPosn = ii + 1;
			stpPosnsOfTable->f1stBondPosition_X = fRefPosnX[ii+1];

			if(idxStartMoveX1stBond == 1 && idxEndMoveX_1stBondPosn >= 200) // For 2nd Wire, Table may start motion before BH start motion from last wire
			{
				int jj;
				for(jj = idxEndMoveX_1stBondPosn - 1; jj>0 ; jj --)
				{
					if(mtn_tune_is_ref_vel_x_eq_0(jj - 1) == TRUE && mtn_tune_is_ref_vel_x_eq_0(jj) == FALSE)
					{
						idxStartMoveX1stBond = jj;
						jj = 0;
					}
				}
			}
			if(fabs(stpPosnsOfTable->f1stBondPosition_X - stpPosnsOfTable->fStartPosition_X) < 1.5)
			{
				stpAnalyzeOneWire->stTableX_ErrPullMotion[nNumPullMotionX].fMotionStartPosn = stpPosnsOfTable->fStartPosition_X;
				stpAnalyzeOneWire->stTableX_ErrPullMotion[nNumPullMotionX].fMotionTargetPosn = stpPosnsOfTable->f1stBondPosition_X;
				stpAnalyzeOneWire->stTableX_ErrPullMotion[nNumPullMotionX].idxStartMotion = idxStartMoveX1stBond;
				stpAnalyzeOneWire->stTableX_ErrPullMotion[nNumPullMotionX].idxEndMotion = idxEndMoveX_1stBondPosn;
#ifdef __DEBUG_ROUND_PULL_MOTION__
				idxStartMoveX1stBond = idxEndMoveX_1stBondPosn = 0;
#endif // __DEBUG_ROUND_PULL_MOTION__
				nNumPullMotionX ++;
			}
		}
		if( mtn_tune_is_ref_vel_y_eq_0(ii) == FALSE && mtn_tune_is_ref_vel_y_eq_0(ii+1) == TRUE && 
			(fabs(fRefPosnY[ii+1] - stpPosnsOfTable->f1stBondPosition_Y) < 2) &&
			(idxStartMoveY1stBond !=0 || fabs(stpPosnsOfTable->fStartPosition_Y) > 1.0)&& idxEndMoveY_1stBondPosn == 0)
		{
			idxEndMoveY_1stBondPosn = ii + 1;
			stpPosnsOfTable->f1stBondPosition_Y = fRefPosnY[ii+1];
			if(fabs(stpPosnsOfTable->f1stBondPosition_Y - stpPosnsOfTable->fStartPosition_Y) < 1.5)
			{
				stpAnalyzeOneWire->stTableY_ErrPullMotion[nNumPullMotionY].fMotionStartPosn = stpPosnsOfTable->fStartPosition_Y;
				stpAnalyzeOneWire->stTableY_ErrPullMotion[nNumPullMotionY].fMotionTargetPosn = stpPosnsOfTable->f1stBondPosition_Y;
				stpAnalyzeOneWire->stTableY_ErrPullMotion[nNumPullMotionY].idxStartMotion = idxStartMoveY1stBond;
				stpAnalyzeOneWire->stTableY_ErrPullMotion[nNumPullMotionY].idxEndMotion = idxEndMoveY_1stBondPosn;
#ifdef __DEBUG_ROUND_PULL_MOTION__
				idxStartMoveY1stBond = idxEndMoveY_1stBondPosn = 0;
#endif // __DEBUG_ROUND_PULL_MOTION__
				nNumPullMotionY ++;
			}
		}
		if( mtn_tune_is_ref_vel_x_eq_0(ii) == TRUE && mtn_tune_is_ref_vel_x_eq_0(ii+1) == FALSE 
			&& idxEndMoveX_1stBondPosn != 0 && idxStartReverseMoveX == 0)
		{
			idxStartReverseMoveX = ii;
		}
		if( mtn_tune_is_ref_vel_y_eq_0(ii) == TRUE && mtn_tune_is_ref_vel_y_eq_0(ii+1) == FALSE 
			&& idxEndMoveY_1stBondPosn != 0 && idxStartReverseMoveY == 0)
		{
			idxStartReverseMoveY = ii;
		}
		if( mtn_tune_is_ref_vel_x_eq_0(ii) == FALSE && (mtn_tune_is_ref_vel_x_eq_0(ii+1) == TRUE || fRefVelX[ii+1] * fRefVelX[ii] < 0 )
			&& idxStartReverseMoveX != 0 && idxEndReverseMoveX == 0)
		{
			idxEndReverseMoveX = ii + 1;
			stpPosnsOfTable->f1stBondReversePosition_X = fRefPosnX[ii+1];
			if(fabs(stpPosnsOfTable->f1stBondReversePosition_X - stpPosnsOfTable->f1stBondPosition_X) < 1.5)
			{
				stpAnalyzeOneWire->stTableX_ErrPullMotion[nNumPullMotionX].fMotionStartPosn = stpPosnsOfTable->f1stBondPosition_X;
				stpAnalyzeOneWire->stTableX_ErrPullMotion[nNumPullMotionX].fMotionTargetPosn = stpPosnsOfTable->f1stBondReversePosition_X;
				stpAnalyzeOneWire->stTableX_ErrPullMotion[nNumPullMotionX].idxStartMotion = idxStartReverseMoveX;
				stpAnalyzeOneWire->stTableX_ErrPullMotion[nNumPullMotionX].idxEndMotion = idxEndReverseMoveX;
#ifdef __DEBUG_ROUND_PULL_MOTION__
				idxStartReverseMoveX = idxEndReverseMoveX = 0;
#endif // __DEBUG_ROUND_PULL_MOTION__
				nNumPullMotionX ++;
			}
		}
		if( mtn_tune_is_ref_vel_y_eq_0(ii) == FALSE && (mtn_tune_is_ref_vel_y_eq_0(ii+1) == TRUE  || fRefVelY[ii+1] * fRefVelY[ii] < 0 )
			&& idxStartReverseMoveY != 0 && idxEndReverseMoveY == 0)
		{
			idxEndReverseMoveY = ii + 1;
			stpPosnsOfTable->f1stBondReversePosition_Y = fRefPosnY[ii+1];

			if(fabs(stpPosnsOfTable->f1stBondReversePosition_Y - stpPosnsOfTable->f1stBondPosition_Y) < 1.5)
			{
				stpAnalyzeOneWire->stTableY_ErrPullMotion[nNumPullMotionY].fMotionStartPosn = stpPosnsOfTable->f1stBondPosition_Y;
				stpAnalyzeOneWire->stTableY_ErrPullMotion[nNumPullMotionY].fMotionTargetPosn = stpPosnsOfTable->f1stBondReversePosition_Y;
				stpAnalyzeOneWire->stTableY_ErrPullMotion[nNumPullMotionY].idxStartMotion = idxStartReverseMoveY;
				stpAnalyzeOneWire->stTableY_ErrPullMotion[nNumPullMotionY].idxEndMotion = idxEndReverseMoveY;
#ifdef __DEBUG_ROUND_PULL_MOTION__
				idxStartReverseMoveY = idxEndReverseMoveY = 0;
#endif // __DEBUG_ROUND_PULL_MOTION__
				nNumPullMotionY ++;
			}
		}
		if( (mtn_tune_is_ref_vel_x_eq_0(ii) == TRUE || fRefVelX[ii+1] * fRefVelX[ii] < 0 )&& mtn_tune_is_ref_vel_x_eq_0(ii+1) == FALSE 
			&& idxEndReverseMoveX != 0 && idxStartTrajTo2ndBondX == 0)
		{
			idxStartTrajTo2ndBondX = ii+1;
			idxStartTrajTo2ndBondY = ii + 1;
		}
		if( (mtn_tune_is_ref_vel_y_eq_0(ii) == TRUE || fRefVelY[ii+1] * fRefVelY[ii] < 0 )&& mtn_tune_is_ref_vel_y_eq_0(ii+1) == FALSE 
			&& idxEndReverseMoveY != 0 && idxStartTrajTo2ndBondY == 0)
		{
			idxStartTrajTo2ndBondY = ii + 1;
			idxStartTrajTo2ndBondX = ii+1;
		}
		if( mtn_tune_is_ref_vel_x_eq_0(ii) == FALSE && mtn_tune_is_ref_vel_x_eq_0(ii+1) == TRUE 
			&& idxStartTrajTo2ndBondX != 0 && idxEndTraj2ndBondPosnX == 0)
		{
			idxEndTraj2ndBondPosnX = ii + 1;
			idxEndTraj2ndBondPosnY = ii + 1; // 20130127
			stpPosnsOfTable->fTrajEnd2ndBondPosn_X = fRefPosnX[ii+1];

			if(fabs(stpPosnsOfTable->fTrajEnd2ndBondPosn_X - stpPosnsOfTable->f1stBondReversePosition_X) < 1.5)
			{
				stpAnalyzeOneWire->stTableX_ErrPullMotion[nNumPullMotionX].fMotionStartPosn = stpPosnsOfTable->f1stBondReversePosition_X;
				stpAnalyzeOneWire->stTableX_ErrPullMotion[nNumPullMotionX].fMotionTargetPosn = stpPosnsOfTable->fTrajEnd2ndBondPosn_X;
				stpAnalyzeOneWire->stTableX_ErrPullMotion[nNumPullMotionX].idxStartMotion = idxStartTrajTo2ndBondX;
				stpAnalyzeOneWire->stTableX_ErrPullMotion[nNumPullMotionX].idxEndMotion = idxEndTraj2ndBondPosnX;
#ifdef __DEBUG_ROUND_PULL_MOTION__
				idxEndTraj2ndBondPosnX = idxStartTrajTo2ndBondX = 0;
#endif //  __DEBUG_ROUND_PULL_MOTION__
				nNumPullMotionX ++;
			}

		}
		if( mtn_tune_is_ref_vel_y_eq_0(ii) == FALSE && mtn_tune_is_ref_vel_y_eq_0(ii+1) == TRUE 
			&& idxStartTrajTo2ndBondY != 0 && idxEndTraj2ndBondPosnY == 0)
		{
			idxEndTraj2ndBondPosnY = ii + 1;
			idxEndTraj2ndBondPosnX = ii + 1;
			stpPosnsOfTable->fTrajEnd2ndBondPosn_Y = fRefPosnY[ii+1];

			if(fabs(stpPosnsOfTable->fTrajEnd2ndBondPosn_Y - stpPosnsOfTable->f1stBondReversePosition_Y) < 1.5)
			{
				stpAnalyzeOneWire->stTableY_ErrPullMotion[nNumPullMotionY].fMotionStartPosn = stpPosnsOfTable->f1stBondReversePosition_Y;
				stpAnalyzeOneWire->stTableY_ErrPullMotion[nNumPullMotionY].fMotionTargetPosn = stpPosnsOfTable->fTrajEnd2ndBondPosn_Y;
				stpAnalyzeOneWire->stTableY_ErrPullMotion[nNumPullMotionY].idxStartMotion = idxStartTrajTo2ndBondY;
				stpAnalyzeOneWire->stTableY_ErrPullMotion[nNumPullMotionY].idxEndMotion = idxEndTraj2ndBondPosnY;
#ifdef __DEBUG_ROUND_PULL_MOTION__
				idxEndTraj2ndBondPosnY = idxStartTrajTo2ndBondY = 0;
#endif //  __DEBUG_ROUND_PULL_MOTION__
				nNumPullMotionY ++;
			}

		}
		if( mtn_tune_is_ref_vel_x_eq_0(ii) == TRUE && mtn_tune_is_ref_vel_x_eq_0(ii+1) == FALSE 
			&& idxEndTraj2ndBondPosnX != 0 && idxStartMoveNextPR_X == 0)
		{
			idxStartMoveNextPR_X = ii;
		}
		if( mtn_tune_is_ref_vel_y_eq_0(ii) == TRUE && mtn_tune_is_ref_vel_y_eq_0(ii+1) == FALSE 
			&& idxEndTraj2ndBondPosnY != 0 && idxStartMoveNextPR_Y == 0)
		{
			idxStartMoveNextPR_Y = ii;
		}
		if( mtn_tune_is_ref_vel_x_eq_0(ii) == FALSE && mtn_tune_is_ref_vel_x_eq_0(ii+1) == TRUE 
			&& idxStartMoveNextPR_X != 0 && idxEndMoveNextPR_X == 0)
		{
			idxEndMoveNextPR_X = ii + 1;
			stpPosnsOfTable->fMoveToNextPR_Posn_X = fRefPosnX[ii+1];

#ifdef __DEBUG_ROUND_PULL_MOTION__
			if(fabs(stpPosnsOfTable->fMoveToNextPR_Posn_X - stpPosnsOfTable->fTrajEnd2ndBondPosn_X) < 1.5)
			{
				stpAnalyzeOneWire->stTableX_ErrPullMotion[nNumPullMotionX].fMotionStartPosn = stpPosnsOfTable->fTrajEnd2ndBondPosn_X;
				stpAnalyzeOneWire->stTableX_ErrPullMotion[nNumPullMotionX].fMotionTargetPosn = stpPosnsOfTable->fMoveToNextPR_Posn_X;
				stpAnalyzeOneWire->stTableX_ErrPullMotion[nNumPullMotionX].idxStartMotion = idxStartMoveNextPR_X;
				stpAnalyzeOneWire->stTableX_ErrPullMotion[nNumPullMotionX].idxEndMotion = idxEndMoveNextPR_X;
				idxStartMoveNextPR_X = idxEndMoveNextPR_X = 0;
				nNumPullMotionX ++;
			}
#endif //  __DEBUG_ROUND_PULL_MOTION__
		}
		if( mtn_tune_is_ref_vel_y_eq_0(ii) == FALSE && mtn_tune_is_ref_vel_y_eq_0(ii+1) == TRUE && idxStartMoveNextPR_Y != 0 && idxEndMoveNextPR_Y == 0)
		{
			idxEndMoveNextPR_Y = ii + 1;
			stpPosnsOfTable->fMoveToNextPR_Posn_Y = fRefPosnY[ii+1];

#ifdef __DEBUG_ROUND_PULL_MOTION__
			if(fabs(stpPosnsOfTable->fMoveToNextPR_Posn_Y - stpPosnsOfTable->fTrajEnd2ndBondPosn_Y) < 1.5)
			{
				stpAnalyzeOneWire->stTableY_ErrPullMotion[nNumPullMotionY].fMotionStartPosn = stpPosnsOfTable->fTrajEnd2ndBondPosn_Y;
				stpAnalyzeOneWire->stTableY_ErrPullMotion[nNumPullMotionY].fMotionTargetPosn = stpPosnsOfTable->fMoveToNextPR_Posn_Y;
				stpAnalyzeOneWire->stTableY_ErrPullMotion[nNumPullMotionY].idxStartMotion = idxStartMoveNextPR_Y;
				stpAnalyzeOneWire->stTableY_ErrPullMotion[nNumPullMotionY].idxEndMotion = idxEndMoveNextPR_Y;
				idxStartMoveNextPR_Y = idxEndMoveNextPR_Y = 0;
				nNumPullMotionY ++;
			}
#endif //  __DEBUG_ROUND_PULL_MOTION__
		}
		ii = ii + 1;
	}

	// Output to st
	stpAnalyzeOneWire->nNumPullMotionX = nNumPullMotionX;
	stpAnalyzeOneWire->nNumPullMotionY = nNumPullMotionY;

//////// Motion Performance, CEOUS: CommandEndOverUnderShoot, 
//////// MaxDPE: Maximum Dynamic Position Error
	stpTableXPerformance->afMaxDPE_MoveTo1stBondPosn_BTO[0] = f_get_max(&fPosnErrX[idxStartMoveX1stBond], idxEndMoveX_1stBondPosn - idxStartMoveX1stBond);
	stpTableXPerformance->afMaxDPE_MoveTo1stBondPosn_BTO[1] = f_get_min(&fPosnErrX[idxStartMoveX1stBond], idxEndMoveX_1stBondPosn - idxStartMoveX1stBond);
	stpTableXPerformance->fCEOUS_MoveTo1stBondPosn_BTO = fPosnErrX[idxEndMoveX_1stBondPosn + 1] * f_get_sign(stpPosnsOfTable->f1stBondPosition_X - stpPosnsOfTable->fStartPosition_X);
	stpTableXPerformance->afMaxDPE_MoveToKink1[0] = f_get_max(&fPosnErrX[idxStartReverseMoveX], idxEndReverseMoveX - idxStartReverseMoveX);
	stpTableXPerformance->afMaxDPE_MoveToKink1[1] = f_get_min(&fPosnErrX[idxStartReverseMoveX], idxEndReverseMoveX - idxStartReverseMoveX);
	stpTableXPerformance->fCEOUS_MoveToKink1 = fPosnErrX[idxEndReverseMoveX + 1] * f_get_sign(stpPosnsOfTable->f1stBondReversePosition_X - stpPosnsOfTable->f1stBondPosition_X);
	stpTableXPerformance->afMaxDPE_TrajMoveTo2ndBond[0] = f_get_max(&fPosnErrX[idxStartTrajTo2ndBondX], idxEndTraj2ndBondPosnX - idxStartTrajTo2ndBondX);
	stpTableXPerformance->afMaxDPE_TrajMoveTo2ndBond[1] = f_get_min(&fPosnErrX[idxStartTrajTo2ndBondX], idxEndTraj2ndBondPosnX - idxStartTrajTo2ndBondX);
	stpTableXPerformance->fCEOUS_TrajMoveTo2ndBond = fPosnErrX[idxEndTraj2ndBondPosnX + 1] * f_get_sign(stpPosnsOfTable->fTrajEnd2ndBondPosn_X - stpPosnsOfTable->f1stBondReversePosition_X);
	if( idxEndMoveNextPR_X != 0)
	{
		stpTableXPerformance->afMaxDPE_MoveToNextPR[0] = f_get_max(&fPosnErrX[idxStartMoveNextPR_X], idxEndMoveNextPR_X - idxStartMoveNextPR_X);
		stpTableXPerformance->afMaxDPE_MoveToNextPR[1] = f_get_min(&fPosnErrX[idxStartMoveNextPR_X], idxEndMoveNextPR_X - idxStartMoveNextPR_X);
		stpTableXPerformance->fCEOUS_MoveToNextPR = fPosnErrX[idxEndMoveNextPR_X + 1] * f_get_sign(stpPosnsOfTable->fMoveToNextPR_Posn_X - stpPosnsOfTable->fTrajEnd2ndBondPosn_X);
	}
	else
	{
		stpTableXPerformance->afMaxDPE_MoveToNextPR[0] = 0;
		stpTableXPerformance->afMaxDPE_MoveToNextPR[1] = 0;
		stpTableXPerformance->fCEOUS_MoveToNextPR = 0;
		stpPosnsOfTable->fMoveToNextPR_Posn_X = stpPosnsOfTable->fTrajEnd2ndBondPosn_X;
	}
	for(int ii=0; ii<MAX_LENGTH_TABLE_SETTLE; ii++)
	{
		stpTableXPerformance->afTablePosnErrSettle_MoveTo1stBond[ii] = fPosnErrX[idxEndMoveX_1stBondPosn + ii];
		stpTableXPerformance->afTablePosnErrSettle_Kink1[ii] = fPosnErrX[idxEndReverseMoveX + ii];
		stpTableXPerformance->afTablePosnErrSettle_MoveTo2ndBond[ii] = fPosnErrX[idxEndTraj2ndBondPosnX + ii];
	}
/// idxEndMove1stBondSearchHeight; idxStart1stBondForceCtrl; idxEnd1stBondForceCtrl; 
/// idxEndTrajectory2ndBondSearchHeight; idxStart2ndBondForceCtrl; idxEnd2ndBondForceCtrl;

	// 20111130, Table.X
	double dMaxPosnErr = fPosnErrX[idxEndMove1stBondSearchHeight], dMinPosnErr = fPosnErrX[idxEndMove1stBondSearchHeight];
	for(int ii=idxEndMove1stBondSearchHeight; ii<idxStart1stBondForceCtrl; ii++)
	{
		stpTableXPerformance->afTblPosnErrFromSrch1B[ii - idxEndMove1stBondSearchHeight] = fPosnErrX[ii];
	}

	int idxStartCalcTableSettling1stB;
	if(stpAnalyzeOneWire->iFlagIsDryRun == 1)  // 20120518
	{
		idxStartCalcTableSettling1stB = idxStart1stBondForceCtrl + 12;
	}
	else
	{
		idxStartCalcTableSettling1stB = idxStart1stBondForceCtrl;
	}
	dMaxPosnErr = fPosnErrX[idxStartCalcTableSettling1stB], dMinPosnErr = fPosnErrX[idxStartCalcTableSettling1stB];
	for(int ii=idxStartCalcTableSettling1stB; ii<idxEnd1stBondForceCtrl; ii++) // 20120518
	{
		stpTableXPerformance->afTblPosnErrContact1B[ii - idxStartCalcTableSettling1stB] = fPosnErrX[ii];  // 20120716
		if( dMaxPosnErr < fPosnErrX[ii])
		{
			dMaxPosnErr = fPosnErrX[ii];
		}
		if( dMinPosnErr > fPosnErrX[ii])
		{
			dMinPosnErr = fPosnErrX[ii];
		}
	}
	stpTableXPerformance->fTblPosnErrP2PContact1B = dMaxPosnErr - dMinPosnErr;
	if(fabs(dMaxPosnErr) > fabs(dMinPosnErr))
	{
		stpTableXPerformance->fTblMaxAbsPosnErrContact1B = fabs(dMaxPosnErr);
	}
	else
	{
		stpTableXPerformance->fTblMaxAbsPosnErrContact1B = fabs(dMinPosnErr);
	}

	// 20111130
	for(int ii=idxEndTrajectory2ndBondSearchHeight; ii<idxStart2ndBondForceCtrl; ii++)
	{
		stpTableXPerformance->afTblPosnErrFromSrch2B[ii - idxEndTrajectory2ndBondSearchHeight] = fPosnErrX[ii];
	}

	int idxStartCalcTableSettling2ndB;
	if(stpAnalyzeOneWire->iFlagIsDryRun == 1)  // 20120518
	{
		idxStartCalcTableSettling2ndB = idxStart2ndBondForceCtrl + 12;
	}
	else
	{
		idxStartCalcTableSettling2ndB = idxStart2ndBondForceCtrl;
	}
	dMaxPosnErr = fPosnErrX[idxStartCalcTableSettling2ndB], dMinPosnErr = fPosnErrX[idxStartCalcTableSettling2ndB];
	for(int ii=idxStartCalcTableSettling2ndB; ii<idxEnd2ndBondForceCtrl; ii++)
	{
		stpTableXPerformance->afTblPosnErrContact2B[ii - idxStartCalcTableSettling2ndB] = fPosnErrX[ii]; //20120716
		if( dMaxPosnErr < fPosnErrX[ii])
		{
			dMaxPosnErr = fPosnErrX[ii];
		}
		if( dMinPosnErr > fPosnErrX[ii])
		{
			dMinPosnErr = fPosnErrX[ii];
		}
	}

	stpTableXPerformance->fTblPosnErrP2PContact2B = dMaxPosnErr - dMinPosnErr;
	if(fabs(dMaxPosnErr) > fabs(dMinPosnErr))
	{
		stpTableXPerformance->fTblMaxAbsPosnErrContact2B = fabs(dMaxPosnErr);
	}
	else
	{
		stpTableXPerformance->fTblMaxAbsPosnErrContact2B = fabs(dMinPosnErr);
	}

	// Table-Y Performance
	//for( ii = 0; ii < LEN_UPLOAD_ARRAY-1; ii++)
	//{
	//	fPosnErrY[ii] = fRefPosnY[ii] - fFeedPosnY[ii];
	//}
	stpTableYPerformance->afMaxDPE_MoveTo1stBondPosn_BTO[0] = f_get_max(&fPosnErrY[idxStartMoveY1stBond], idxEndMoveY_1stBondPosn - idxStartMoveY1stBond);
	stpTableYPerformance->afMaxDPE_MoveTo1stBondPosn_BTO[1] = f_get_min(&fPosnErrY[idxStartMoveY1stBond], idxEndMoveY_1stBondPosn - idxStartMoveY1stBond);
	stpTableYPerformance->fCEOUS_MoveTo1stBondPosn_BTO = fPosnErrY[idxEndMoveY_1stBondPosn + 1] * f_get_sign(stpPosnsOfTable->f1stBondPosition_Y - stpPosnsOfTable->fStartPosition_Y);
	stpTableYPerformance->afMaxDPE_MoveToKink1[0] = f_get_max(&fPosnErrY[idxStartReverseMoveY], idxEndReverseMoveY - idxStartReverseMoveY);
	stpTableYPerformance->afMaxDPE_MoveToKink1[1] = f_get_min(&fPosnErrY[idxStartReverseMoveY], idxEndReverseMoveY - idxStartReverseMoveY);
	stpTableYPerformance->fCEOUS_MoveToKink1 = fPosnErrY[idxEndReverseMoveY + 1] * f_get_sign(stpPosnsOfTable->f1stBondReversePosition_Y - stpPosnsOfTable->f1stBondPosition_Y);
	stpTableYPerformance->afMaxDPE_TrajMoveTo2ndBond[0] = f_get_max(&fPosnErrY[idxStartTrajTo2ndBondY], idxEndTraj2ndBondPosnY - idxStartTrajTo2ndBondY);
	stpTableYPerformance->afMaxDPE_TrajMoveTo2ndBond[1] = f_get_min(&fPosnErrY[idxStartTrajTo2ndBondY], idxEndTraj2ndBondPosnY - idxStartTrajTo2ndBondY);
	stpTableYPerformance->fCEOUS_TrajMoveTo2ndBond = fPosnErrY[idxEndTraj2ndBondPosnY + 1] * f_get_sign(stpPosnsOfTable->fTrajEnd2ndBondPosn_Y - stpPosnsOfTable->f1stBondReversePosition_Y);
	if( idxEndMoveNextPR_Y != 0)
	{
		stpTableYPerformance->afMaxDPE_MoveToNextPR[0] = f_get_max(&fPosnErrY[idxStartMoveNextPR_Y], idxEndMoveNextPR_Y - idxStartMoveNextPR_Y);
		stpTableYPerformance->afMaxDPE_MoveToNextPR[1] = f_get_min(&fPosnErrY[idxStartMoveNextPR_Y], idxEndMoveNextPR_Y - idxStartMoveNextPR_Y);
		stpTableYPerformance->fCEOUS_MoveToNextPR = fPosnErrY[idxEndMoveNextPR_Y + 1] * f_get_sign(stpPosnsOfTable->fMoveToNextPR_Posn_Y - stpPosnsOfTable->fTrajEnd2ndBondPosn_Y);
	}
	else
	{
		stpTableYPerformance->afMaxDPE_MoveToNextPR[0] = 0;
		stpTableYPerformance->afMaxDPE_MoveToNextPR[1] = 0;
		stpTableYPerformance->fCEOUS_MoveToNextPR = 0;
		stpPosnsOfTable->fMoveToNextPR_Posn_Y = stpPosnsOfTable->fTrajEnd2ndBondPosn_Y;
	}
	for(int ii=0; ii<MAX_LENGTH_TABLE_SETTLE; ii++)
	{
		stpTableYPerformance->afTablePosnErrSettle_MoveTo1stBond[ii] = fPosnErrY[idxEndMoveY_1stBondPosn + ii];
		stpTableYPerformance->afTablePosnErrSettle_Kink1[ii] = fPosnErrY[idxEndReverseMoveY + ii];
		stpTableYPerformance->afTablePosnErrSettle_MoveTo2ndBond[ii] = fPosnErrY[idxEndTraj2ndBondPosnY + ii];
	}

	// 20111130, Table.Y
	for(int ii=idxEndMove1stBondSearchHeight; ii<idxStart1stBondForceCtrl; ii++)
	{
		stpTableYPerformance->afTblPosnErrFromSrch1B[ii - idxEndMove1stBondSearchHeight] = fPosnErrY[ii];
	}

	dMaxPosnErr = fPosnErrY[idxStartCalcTableSettling1stB ], dMinPosnErr = fPosnErrY[idxStartCalcTableSettling1stB ];
	for(int ii=idxStartCalcTableSettling1stB ; ii<idxEnd1stBondForceCtrl; ii++)  // 20120518  idxStart1stBondForceCtrl
	{
		stpTableYPerformance->afTblPosnErrContact1B[ii - idxStartCalcTableSettling1stB] = fPosnErrY[ii];  // 20120716
		if( dMaxPosnErr < fPosnErrY[ii])
		{
			dMaxPosnErr = fPosnErrY[ii];
		}
		if( dMinPosnErr > fPosnErrY[ii])
		{
			dMinPosnErr = fPosnErrY[ii];
		}
	}
	stpTableYPerformance->fTblPosnErrP2PContact1B = dMaxPosnErr - dMinPosnErr;
	if(fabs(dMaxPosnErr) > fabs(dMinPosnErr))
	{
		stpTableYPerformance->fTblMaxAbsPosnErrContact1B = fabs(dMaxPosnErr);
	}
	else
	{
		stpTableYPerformance->fTblMaxAbsPosnErrContact1B = fabs(dMinPosnErr);
	}

	// 20111130
	for(int ii=idxEndTrajectory2ndBondSearchHeight; ii<idxStart2ndBondForceCtrl; ii++)
	{
		stpTableYPerformance->afTblPosnErrFromSrch2B[ii - idxEndTrajectory2ndBondSearchHeight] = fPosnErrY[ii];
	}

	dMaxPosnErr = fPosnErrY[idxStartCalcTableSettling2ndB], dMinPosnErr = fPosnErrY[idxStartCalcTableSettling2ndB];
	for(int ii=idxStartCalcTableSettling2ndB; ii<idxEnd2ndBondForceCtrl; ii++)
	{
		stpTableYPerformance->afTblPosnErrContact2B[ii - idxStartCalcTableSettling2ndB] = fPosnErrY[ii]; // 20120716
		if( dMaxPosnErr < fPosnErrY[ii])
		{
			dMaxPosnErr = fPosnErrY[ii];
		}
		if( dMinPosnErr > fPosnErrY[ii])
		{
			dMinPosnErr = fPosnErrY[ii];
		}
	}

	stpTableYPerformance->fTblPosnErrP2PContact2B = dMaxPosnErr - dMinPosnErr;
	if(fabs(dMaxPosnErr) > fabs(dMinPosnErr))
	{
		stpTableYPerformance->fTblMaxAbsPosnErrContact2B = fabs(dMaxPosnErr);
	}
	else
	{
		stpTableYPerformance->fTblMaxAbsPosnErrContact2B = fabs(dMinPosnErr);
	}


//// Bond Head SpeedSetting
	/// Actual Acc and Jerk, fRefAccZ, 
	stpAnalyzeOneWire->stBondHeadSpeedSetting.dMaxRefAccMoveToSrchHeight = f_get_abs_max(&fRefAccZ[idxStartMove1stBondSearchHeight], idxEndMove1stBondSearchHeight-idxStartMove1stBondSearchHeight);
	stpAnalyzeOneWire->stBondHeadSpeedSetting.dMaxRefAccMoveReverseHeight = f_get_abs_max(&fRefAccZ[idxStartReverseHeight], idxEndReverseHeight - idxStartReverseHeight);
	stpAnalyzeOneWire->stBondHeadSpeedSetting.dMaxRefAccMoveReverseDist = f_get_abs_max(&fRefAccZ[idxStartKinkHeight], idxEndKinkHeight - idxStartKinkHeight);
	stpAnalyzeOneWire->stBondHeadSpeedSetting.dMaxRefAccMoveLoopTop = f_get_abs_max(&fRefAccZ[idxStartMoveLoopTop], idxEndMoveLoopTop - idxStartMoveLoopTop);
	stpAnalyzeOneWire->stBondHeadSpeedSetting.dMaxRefAccMoveTrajectory = f_get_abs_max(&fRefAccZ[idxStartTrajectory], idxEndTrajectory2ndBondSearchHeight - idxStartTrajectory);
	stpAnalyzeOneWire->stBondHeadSpeedSetting.dMaxRefAccMoveTail = f_get_abs_max(&fRefAccZ[idxStartTail], idxEndTail - idxStartTail);
	stpAnalyzeOneWire->stBondHeadSpeedSetting.dMaxRefAccMoveResetLevel = f_get_abs_max(&fRefAccZ[idxStartFireLevel], idxEndFireLevel - idxStartFireLevel);
	// fRefJerkZ
	stpAnalyzeOneWire->stBondHeadSpeedSetting.dMaxRefJerkMoveToSrchHeight = f_get_abs_max(&fRefJerkZ[idxStartMove1stBondSearchHeight], idxEndMove1stBondSearchHeight-idxStartMove1stBondSearchHeight );
	stpAnalyzeOneWire->stBondHeadSpeedSetting.dMaxRefJerkMoveReverseHeight = f_get_abs_max(&fRefJerkZ[idxStartReverseHeight], idxEndReverseHeight - idxStartReverseHeight );
	stpAnalyzeOneWire->stBondHeadSpeedSetting.dMaxRefJerkMoveReverseDist = f_get_abs_max(&fRefJerkZ[idxStartKinkHeight], idxEndKinkHeight - idxStartKinkHeight);
	stpAnalyzeOneWire->stBondHeadSpeedSetting.dMaxRefJerkMoveLoopTop = f_get_abs_max(&fRefJerkZ[idxStartMoveLoopTop], idxEndMoveLoopTop - idxStartMoveLoopTop);
	stpAnalyzeOneWire->stBondHeadSpeedSetting.dMaxRefJerkMoveTrajectory = f_get_abs_max(&fRefJerkZ[idxStartTrajectory], idxEndTrajectory2ndBondSearchHeight - idxStartTrajectory );
	stpAnalyzeOneWire->stBondHeadSpeedSetting.dMaxRefJerkMoveTail = f_get_abs_max(&fRefJerkZ[idxStartTail], idxEndTail - idxStartTail );
	stpAnalyzeOneWire->stBondHeadSpeedSetting.dMaxRefJerkMoveResetLevel = f_get_abs_max(&fRefJerkZ[idxStartFireLevel], idxEndFireLevel - idxStartFireLevel);

////////// Output
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxStartMove1stBondSearchHeight = idxStartMove1stBondSearchHeight;
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxEndMove1stBondSearchHeight = idxEndMove1stBondSearchHeight;
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxStart1stBondForceCtrl = idxStart1stBondForceCtrl;
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxEnd1stBondForceCtrl = idxEnd1stBondForceCtrl;
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxStartReverseHeight = idxStartReverseHeight;
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxEndReverseHeight = idxEndReverseHeight;
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxStartKinkHeight = idxStartKinkHeight;
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxEndKinkHeight = idxEndKinkHeight;
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxStartMoveLoopTop = idxStartMoveLoopTop;
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxEndMoveLoopTop = idxEndMoveLoopTop;
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxStartTrajectory = idxStartTrajectory;
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxEndTrajectory2ndBondSearchHeight = idxEndTrajectory2ndBondSearchHeight;
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxStart2ndBondForceCtrl = idxStart2ndBondForceCtrl;
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxEnd2ndBondForceCtrl = idxEnd2ndBondForceCtrl;
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxStartTail = idxStartTail;
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxEndTail = idxEndTail;
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxStartFireLevel = idxStartFireLevel;
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxEndFireLevel = idxEndFireLevel;
stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxStart2ndBondSearchContact = idxStart2ndBondSearchContact;

int idxStartSearch = stpTimePointsOfBondHeadZ->idxStart2ndBondSearchContact;
int idxEndSearch = stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl;
stpAnalyzeOneWire->stBondHeadSpeedSetting.dSrchVel1stBond = fRefVelZ[(idxStartSearch + idxEndSearch )/2];
idxStartSearch = stpTimePointsOfBondHeadZ->idxStart2ndBondSearchContact, idxEndSearch = stpTimePointsOfBondHeadZ->idxStart2ndBondForceCtrl;
stpAnalyzeOneWire->stBondHeadSpeedSetting.dSrchVel2ndBond = fRefVelZ[(idxStartSearch + idxEndSearch )/2];


	// X, Actual Acc and Jerk, fRefAccX, - Move1stB_BTO
	stpAnalyzeOneWire->stActSpeedTableX.dMaxRefAccMove1stB_BTO = f_get_abs_max(&fRefAccX[idxStartMoveX1stBond], idxEndMoveX_1stBondPosn - idxStartMoveX1stBond);
	stpAnalyzeOneWire->stActSpeedTableX.dMaxRefJerkMove1stB_BTO = f_get_abs_max(&fRefJerkX[idxStartMoveX1stBond], idxEndMoveX_1stBondPosn - idxStartMoveX1stBond);

	// - ReverseDist
	stpAnalyzeOneWire->stActSpeedTableX.dMaxRefAccReverseDist = f_get_abs_max(&fRefAccX[idxStartReverseMoveX], idxEndReverseMoveX - idxStartReverseMoveX);
	stpAnalyzeOneWire->stActSpeedTableX.dMaxRefJerkReverseDist = f_get_abs_max(&fRefJerkX[idxStartReverseMoveX], idxEndReverseMoveX - idxStartReverseMoveX);
	// - Traj
	stpAnalyzeOneWire->stActSpeedTableX.dMaxRefAccTraj = f_get_abs_max(&fRefAccX[idxStartTrajTo2ndBondX], idxEndTraj2ndBondPosnX - idxStartTrajTo2ndBondX);
	stpAnalyzeOneWire->stActSpeedTableX.dMaxRefJerkTraj = f_get_abs_max(&fRefJerkX[idxStartTrajTo2ndBondX], idxEndTraj2ndBondPosnX - idxStartTrajTo2ndBondX);

	// Y, Actual Acc and Jerk, fRefAccY, - Move1stB_BTO
	stpAnalyzeOneWire->stActSpeedTableY.dMaxRefAccMove1stB_BTO = f_get_abs_max(&fRefAccY[idxStartMoveY1stBond], idxEndMoveY_1stBondPosn - idxStartMoveY1stBond);
	stpAnalyzeOneWire->stActSpeedTableY.dMaxRefJerkMove1stB_BTO = f_get_abs_max(&fRefJerkY[idxStartMoveY1stBond], idxEndMoveY_1stBondPosn - idxStartMoveY1stBond);

	// - ReverseDist
	stpAnalyzeOneWire->stActSpeedTableY.dMaxRefAccReverseDist = f_get_abs_max(&fRefAccY[idxStartReverseMoveY], idxEndReverseMoveY - idxStartReverseMoveY);
	stpAnalyzeOneWire->stActSpeedTableY.dMaxRefJerkReverseDist = f_get_abs_max(&fRefJerkY[idxStartReverseMoveY], idxEndReverseMoveY - idxStartReverseMoveY);
	// - Traj
	stpAnalyzeOneWire->stActSpeedTableY.dMaxRefAccTraj = f_get_abs_max(&fRefAccY[idxStartTrajTo2ndBondY], idxEndTraj2ndBondPosnY - idxStartTrajTo2ndBondY);
	stpAnalyzeOneWire->stActSpeedTableY.dMaxRefJerkTraj = f_get_abs_max(&fRefJerkY[idxStartTrajTo2ndBondY], idxEndTraj2ndBondPosnY - idxStartTrajTo2ndBondY);

stpAnalyzeOneWire->stTimePointsOfTable.idxStartMoveX1stBond = idxStartMoveX1stBond;
stpAnalyzeOneWire->stTimePointsOfTable.idxStartMoveY1stBond = idxStartMoveY1stBond;
stpAnalyzeOneWire->stTimePointsOfTable.idxEndMoveX_1stBondPosn = idxEndMoveX_1stBondPosn;
stpAnalyzeOneWire->stTimePointsOfTable.idxEndMoveY_1stBondPosn = idxEndMoveY_1stBondPosn;
stpAnalyzeOneWire->stTimePointsOfTable.idxStartReverseMoveX = idxStartReverseMoveX;
stpAnalyzeOneWire->stTimePointsOfTable.idxStartReverseMoveY = idxStartReverseMoveY;
stpAnalyzeOneWire->stTimePointsOfTable.idxEndReverseMoveX = idxEndReverseMoveX;
stpAnalyzeOneWire->stTimePointsOfTable.idxEndReverseMoveY = idxEndReverseMoveY;
stpAnalyzeOneWire->stTimePointsOfTable.idxStartTrajTo2ndBondX = idxStartTrajTo2ndBondX;
stpAnalyzeOneWire->stTimePointsOfTable.idxStartTrajTo2ndBondY = idxStartTrajTo2ndBondY;
stpAnalyzeOneWire->stTimePointsOfTable.idxEndTraj2ndBondPosnX = idxEndTraj2ndBondPosnX;
stpAnalyzeOneWire->stTimePointsOfTable.idxEndTraj2ndBondPosnY = idxEndTraj2ndBondPosnY;

stpAnalyzeOneWire->stTimePointsOfTable.idxStartMoveNextPR_X = idxStartMoveNextPR_X;
stpAnalyzeOneWire->stTimePointsOfTable.idxEndMoveNextPR_X = idxEndMoveNextPR_X;
stpAnalyzeOneWire->stTimePointsOfTable.idxStartMoveNextPR_Y = idxStartMoveNextPR_Y;
stpAnalyzeOneWire->stTimePointsOfTable.idxEndMoveNextPR_Y = idxEndMoveNextPR_Y;


stpAnalyzeOneWire->idxEndOneWireZ = idxEndOneWireZ;
stpAnalyzeOneWire->idxEndOneWireXY = idxEndTraj2ndBondPosnX;

stpAnalyzeOneWire->nTimeB1W_cnt = stpAnalyzeOneWire->idxEndOneWireZ - stpStartOneWireTimeInfo->idxStartOneWireZ;
stpAnalyzeOneWire->nTimeB1W_S2E_cnt = stpAnalyzeOneWire->idxEndOneWireZ - stpAnalyzeOneWire->stTimePointsOfBondHeadZ.idxStartMove1stBondSearchHeight;

}

void mtn_tune_calc_stat_min_max_std(MULTI_WIRE_PERFORM_STAT *stpPerformStat, double *adInputOneWireData, int nTotalNumWires)
{
static	double dMax, dMin, dSum, dMean;
static	int ii;

	dMax = dMin = dSum = adInputOneWireData[0]; dMean = 0;
	for(ii = 1; ii<nTotalNumWires; ii++)
	{
		dSum = dSum + adInputOneWireData[ii];
		if(dMax < adInputOneWireData[ii])
		{
			dMax = adInputOneWireData[ii];
		}
		if(dMin > adInputOneWireData[ii])
		{
			dMin = adInputOneWireData[ii];
		}
	}
	stpPerformStat->dMax = dMax;
	stpPerformStat->dMin = dMin;

	if(nTotalNumWires < 1)
	{
		dMean = 0;
	}
	else
	{
		dMean = dSum/nTotalNumWires;
	}

	dSum = (adInputOneWireData[0] - dMean) * (adInputOneWireData[0] - dMean);
	for(ii=1; ii<nTotalNumWires; ii++)
	{
		dSum = dSum + (adInputOneWireData[ii] - dMean) * (adInputOneWireData[ii] - dMean);
	}

	if(nTotalNumWires < 1)
	{
		stpPerformStat->dStd = 0;
	}
	else
	{
		stpPerformStat->dStd = sqrt(dSum/nTotalNumWires);
	}
}

// #include "acs_buff_prog.h"

void mtn_tune_ana_wb_multi_wire_stat(MULTI_WIRE_WB_PERFORMANCE *stpWbMultiWirePerformStat, 
									 WB_ONE_WIRE_PERFORMANCE_CALC astTempWbOneWirePerformance[], int nTotalNumWires)
{
	double adCalcStatisticWb[NUM_TOTAL_WIRE_IN_ONE_SCOPE];

	stpWbMultiWirePerformStat->nTotalWires = nTotalNumWires;

	int ii;
	for(ii = 0; ii<nTotalNumWires; ii++)
	{
		stpWbMultiWirePerformStat->dTableX1stBondPosnList[ii] = astTempWbOneWirePerformance[ii].stPosnsOfTable.f1stBondPosition_X;
		stpWbMultiWirePerformStat->dTableX2ndBondPosnList[ii] = astTempWbOneWirePerformance[ii].stPosnsOfTable.fTrajEnd2ndBondPosn_X;
		stpWbMultiWirePerformStat->dTableY1stBondPosnList[ii] = astTempWbOneWirePerformance[ii].stPosnsOfTable.f1stBondPosition_Y;
		stpWbMultiWirePerformStat->dTableY2ndBondPosnList[ii] = astTempWbOneWirePerformance[ii].stPosnsOfTable.fTrajEnd2ndBondPosn_Y;

		stpWbMultiWirePerformStat->dBondHead1stContactPosnList[ii] = astTempWbOneWirePerformance[ii].stPosnsOfBondHeadZ.f1stBondContactPosn_Z;
		stpWbMultiWirePerformStat->dBondHead2ndContactPosnList[ii] = astTempWbOneWirePerformance[ii].stPosnsOfBondHeadZ.f2ndBondContactPosn_Z;
	}

	// 1st Search Height
	for(ii = 0; ii<nTotalNumWires; ii++)
	{
		adCalcStatisticWb[ii] = astTempWbOneWirePerformance[ii].stBondHeadPerformance.f1stBondMoveToSearchHeightOverUnderShoot;
	}
	mtn_tune_calc_stat_min_max_std(&stpWbMultiWirePerformStat->stBondHeadMove1stSearchHeight_CEOUS,
		adCalcStatisticWb, nTotalNumWires);
	// Reverse Height
	for(ii = 0; ii<nTotalNumWires; ii++)
	{
		adCalcStatisticWb[ii] = astTempWbOneWirePerformance[ii].stBondHeadPerformance.fReverseHeightOverUnderShoot;
	}
	mtn_tune_calc_stat_min_max_std(&stpWbMultiWirePerformStat->stBondHeadReverseHeight_CEOUS,
		adCalcStatisticWb, nTotalNumWires);

	// Kink1
	for(ii = 0; ii<nTotalNumWires; ii++)
	{
		adCalcStatisticWb[ii] = astTempWbOneWirePerformance[ii].stBondHeadPerformance.fKinkHeightOverUnderShoot;
	}
	mtn_tune_calc_stat_min_max_std(&stpWbMultiWirePerformStat->stBondHeadKink1_CEOUS,
		adCalcStatisticWb, nTotalNumWires);

	// LoopTop
	for(ii = 0; ii<nTotalNumWires; ii++)
	{
		adCalcStatisticWb[ii] = astTempWbOneWirePerformance[ii].stBondHeadPerformance.fLoopTopOverUnderShoot;
	}
	mtn_tune_calc_stat_min_max_std(&stpWbMultiWirePerformStat->stBondHeadLoopTop_CEOUS,
		adCalcStatisticWb, nTotalNumWires);

	// Traj
	for(ii = 0; ii<nTotalNumWires; ii++)
	{
		adCalcStatisticWb[ii] = astTempWbOneWirePerformance[ii].stBondHeadPerformance.f2ndBondMoveToSearchHeightOverUnderShoot;
	}
	mtn_tune_calc_stat_min_max_std(&stpWbMultiWirePerformStat->stBondHeadTrajectory_CEOUS,
		adCalcStatisticWb, nTotalNumWires);

	// Tail
	for(ii = 0; ii<nTotalNumWires; ii++)
	{
		adCalcStatisticWb[ii] = astTempWbOneWirePerformance[ii].stBondHeadPerformance.fTailOverUnderShoot;
	}
	mtn_tune_calc_stat_min_max_std(&stpWbMultiWirePerformStat->stBondHeadTail_CEOUS,
		adCalcStatisticWb, nTotalNumWires);

	// Reset
	for(ii = 0; ii<nTotalNumWires; ii++)
	{
		adCalcStatisticWb[ii] = astTempWbOneWirePerformance[ii].stBondHeadPerformance.fEndFireLevelOverUnderShoot;
	}
	mtn_tune_calc_stat_min_max_std(&stpWbMultiWirePerformStat->stBondHeadResetFireLevel_CEOUS,
		adCalcStatisticWb, nTotalNumWires);

	// 1st Contact Posn
	for(ii = 0; ii<nTotalNumWires; ii++)
	{
		adCalcStatisticWb[ii] = astTempWbOneWirePerformance[ii].stPosnsOfBondHeadZ.f1stBondContactPosn_Z;
	}
	mtn_tune_calc_stat_min_max_std(&stpWbMultiWirePerformStat->stBondHead1stContactPosn,
		adCalcStatisticWb, nTotalNumWires);

	// 2nd Contact Posn
	for(ii = 0; ii<nTotalNumWires; ii++)
	{
		adCalcStatisticWb[ii] = astTempWbOneWirePerformance[ii].stPosnsOfBondHeadZ.f2ndBondContactPosn_Z;
	}
	mtn_tune_calc_stat_min_max_std(&stpWbMultiWirePerformStat->stBondHead2ndContactPosn,
		adCalcStatisticWb, nTotalNumWires);

	// 1st F Control Posn Ripple
	for(ii = 0; ii<nTotalNumWires; ii++)
	{
		adCalcStatisticWb[ii] = astTempWbOneWirePerformance[ii].stBondHeadPerformance.f1stBondForceCtrlPosnRippleStd;
	}
	mtn_tune_calc_stat_min_max_std(&stpWbMultiWirePerformStat->stBondHead1stBondPosnRippleStd,
		adCalcStatisticWb, nTotalNumWires);
	// 2nd F Control Posn Ripple
	for(ii = 0; ii<nTotalNumWires; ii++)
	{
		adCalcStatisticWb[ii] = astTempWbOneWirePerformance[ii].stBondHeadPerformance.f2ndBondForceCtrlPosnRippleStd;
	}
	mtn_tune_calc_stat_min_max_std(&stpWbMultiWirePerformStat->stBondHead2ndBondPosnRippleStd,
		adCalcStatisticWb, nTotalNumWires);

	//Table-X Performance statistics
	// Move to 1stBond Position
	for(ii = 0; ii<nTotalNumWires; ii++)
	{
		adCalcStatisticWb[ii] = astTempWbOneWirePerformance[ii].stTableXPerformance.fCEOUS_MoveTo1stBondPosn_BTO;
	}
	mtn_tune_calc_stat_min_max_std(&stpWbMultiWirePerformStat->stTableXMove1stBond_CEOUS,
		adCalcStatisticWb, nTotalNumWires);

	// Kink
	for(ii = 0; ii<nTotalNumWires; ii++)
	{
		adCalcStatisticWb[ii] = astTempWbOneWirePerformance[ii].stTableXPerformance.fCEOUS_MoveToKink1;
	}
	mtn_tune_calc_stat_min_max_std(&stpWbMultiWirePerformStat->stTableXKink1_CEOUS,
		adCalcStatisticWb, nTotalNumWires);

	// Move to 2ndBond Position
	for(ii = 0; ii<nTotalNumWires; ii++)
	{
		adCalcStatisticWb[ii] = astTempWbOneWirePerformance[ii].stTableXPerformance.fCEOUS_TrajMoveTo2ndBond;
	}
	mtn_tune_calc_stat_min_max_std(&stpWbMultiWirePerformStat->stTableXTrajMove2ndBond_CEOUS,
		adCalcStatisticWb, nTotalNumWires);

	//Table-Y Performance statistics
	// Move to 1stBond Position
	for(ii = 0; ii<nTotalNumWires; ii++)
	{
		adCalcStatisticWb[ii] = astTempWbOneWirePerformance[ii].stTableYPerformance.fCEOUS_MoveTo1stBondPosn_BTO;
	}
	mtn_tune_calc_stat_min_max_std(&stpWbMultiWirePerformStat->stTableYMove1stBond_CEOUS,
		adCalcStatisticWb, nTotalNumWires);

	// Kink
	for(ii = 0; ii<nTotalNumWires; ii++)
	{
		adCalcStatisticWb[ii] = astTempWbOneWirePerformance[ii].stTableYPerformance.fCEOUS_MoveToKink1;
	}
	mtn_tune_calc_stat_min_max_std(&stpWbMultiWirePerformStat->stTableYKink1_CEOUS,
		adCalcStatisticWb, nTotalNumWires);

	// Move to 2ndBond Position
	for(ii = 0; ii<nTotalNumWires; ii++)
	{
		adCalcStatisticWb[ii] = astTempWbOneWirePerformance[ii].stTableYPerformance.fCEOUS_TrajMoveTo2ndBond;
	}
	mtn_tune_calc_stat_min_max_std(&stpWbMultiWirePerformStat->stTableYTrajMove2ndBond_CEOUS,
		adCalcStatisticWb, nTotalNumWires);

	stpWbMultiWirePerformStat->dMeanTimeB1W_cnt = (astTempWbOneWirePerformance[nTotalNumWires -1].stTimePointsOfBondHeadZ.idxEndFireLevel
		- astTempWbOneWirePerformance[0].stTimePointsOfBondHeadZ.idxStartMove1stBondSearchHeight)/ (double)nTotalNumWires;
}

void mtn_tune_ana_wb_waveform(WB_ONE_WIRE_PERFORMANCE_CALC astTempWbOneWirePerformStruct[], int *nTotalNumWires)
{
unsigned int nNumOfWires = 0;
int idxStartOneWireZ = 0;
int idxStartOneWireXY = 0;
int idxExpectEndNextWireXY = 0;
int idxExpectEndNextWireZ = 0;
int tLen = LEN_UPLOAD_ARRAY;
START_ONE_WIRE_TIME_INFO stStartOneWireTimeInfo;
WB_ONE_WIRE_PERFORMANCE_CALC *stpAnalyzeOutputOneWire;
int idxEndOneWireZ, idxEndOneWireXY;

	while( idxExpectEndNextWireXY < tLen && idxExpectEndNextWireZ < tLen  && nNumOfWires<NUM_TOTAL_WIRE_IN_ONE_SCOPE)
	{
		stStartOneWireTimeInfo.idxStartOneWireZ = idxStartOneWireZ;
		stStartOneWireTimeInfo.idxStartOneWireXY = idxStartOneWireXY;
		stpAnalyzeOutputOneWire = &astTempWbOneWirePerformStruct[nNumOfWires];
		mtn_tune_calc_one_wire_performance(&stStartOneWireTimeInfo, stpAnalyzeOutputOneWire);
		if( stpAnalyzeOutputOneWire->iFlagHasWireInfo == 0 )
		{
			// %% no wire info
			break;
		}
		idxEndOneWireZ = stpAnalyzeOutputOneWire->idxEndOneWireZ;
		if(stpAnalyzeOutputOneWire->stTimePointsOfBondHeadZ.idxEnd2ndBondForceCtrl > stpAnalyzeOutputOneWire->idxEndOneWireXY)  // 20120105
		{
			idxEndOneWireXY = stpAnalyzeOutputOneWire->stTimePointsOfBondHeadZ.idxEnd2ndBondForceCtrl;
		}
		else
		{
			idxEndOneWireXY = stpAnalyzeOutputOneWire->idxEndOneWireXY;
		}
		idxExpectEndNextWireXY = (idxEndOneWireXY - idxStartOneWireXY) + idxStartOneWireXY;
		idxExpectEndNextWireZ = (idxEndOneWireZ - idxStartOneWireZ) + idxEndOneWireZ;
	    
		idxStartOneWireZ = idxEndOneWireZ;
		idxStartOneWireXY = idxEndOneWireXY;
		nNumOfWires = nNumOfWires + 1;
//		astAnalyzeOutputOneWire(nNumOfWires) = stAnalyzeOutputOneWire;
	}
	*nTotalNumWires = nNumOfWires;
	mtn_wb_tune_set_num_total_wire(nNumOfWires);

	// Analyse total wire's position, CEOUS min, max, std
}

static unsigned int nTotalWireInfo = 0;
unsigned int mtn_wb_tune_get_num_total_wire()
{
	return nTotalWireInfo;
}
void mtn_wb_tune_set_num_total_wire(unsigned int nNumWire)
{
	if(nNumWire <= NUM_TOTAL_WIRE_IN_ONE_SCOPE)
	{
		nTotalWireInfo = nNumWire;
	}
}

extern CMtnTune mMotionTuning;
// extern char mtn_wb_tune_thread_get_flag_running();
int idxTuningBlockBondHead;
int idxTuningBlockTable;
int iFlagDebug_SaveFile_UIProtPass;
int iFlagTableSectorTuning;
unsigned int idxCurrSectorTable;
char cFlagTuningRoutine;
char cTuneBlocks_FlagTableX, cTuneBlocks_FlagTableY, cTuneBlocks_FlagBondHeadZ;

///////////////////
int iFlagB1W_TuneIdle = 1;
int iFlagB1W_TuneSrch1B = 1;
int iFlagB1W_TuneMoveTo1B = 1;
int iFlagB1W_TuneLooping = 1;
int iFlagB1W_TuneLoopTop = 1;
int iFlagB1W_TuneTraj2B = 1;
int iFlagB1W_TuneTail = 1;
int iFlagB1W_TuneReset = 1;
int iFlagB1W_TuneSrch2B = 1;
///
int iFlagIsActualContact;
int iFlagTuningMotionZ_B1W = 1;
void mtn_wb_tune_b1w_set_flag_contact(int iFlag)
{
	iFlagIsActualContact = iFlag;

	//
	iFlagB1W_TuneSrch1B = iFlagIsActualContact;
	iFlagB1W_TuneSrch2B = iFlagIsActualContact;
}
int mtn_wb_tune_b1w_get_flag_contact()
{
	return iFlagIsActualContact;
}

void mtn_wb_tune_b1w_set_flag_motion(int iFlag) 
{
	iFlagTuningMotionZ_B1W = iFlag;
	//
	iFlagB1W_TuneIdle = iFlagTuningMotionZ_B1W;
	iFlagB1W_TuneMoveTo1B = iFlagTuningMotionZ_B1W;
	iFlagB1W_TuneLooping = iFlagTuningMotionZ_B1W;
	iFlagB1W_TuneLoopTop = iFlagTuningMotionZ_B1W;
	iFlagB1W_TuneTraj2B = iFlagTuningMotionZ_B1W;
	iFlagB1W_TuneTail = iFlagTuningMotionZ_B1W;
	iFlagB1W_TuneReset = iFlagTuningMotionZ_B1W;
}

void mtn_wb_tune_b1w_set_enable_all_section() 
{
	iFlagB1W_TuneIdle = 1;
	iFlagB1W_TuneMoveTo1B = 1;
	iFlagB1W_TuneLooping = 1;
	iFlagB1W_TuneLoopTop = 1;
	iFlagB1W_TuneTraj2B = 1;
	iFlagB1W_TuneTail = 1;
	iFlagB1W_TuneReset = 1;
}

void mtn_wb_tune_b1w_set_clear_all_section() 
{
	iFlagB1W_TuneIdle = 0;
	iFlagB1W_TuneMoveTo1B = 0;
	iFlagB1W_TuneLooping = 0;
	iFlagB1W_TuneLoopTop = 0;
	iFlagB1W_TuneTraj2B = 0;
	iFlagB1W_TuneTail = 0;
	iFlagB1W_TuneReset = 0;
}

int mtn_wb_tune_b1w_get_flag_motion()
{
	return iFlagTuningMotionZ_B1W;
}

///////////////////////////////
int AutoTuneBondHeadBlk()
{
	int iRet = MTN_API_OK_ZERO;

//			mMotionTuning.mtn_tune_set_tune_axis(APP_Z_BOND_ACS_ID); mtn_wb_tune_set_curr_wb_axis_servo_para_tuning(WB_AXIS_BOND_Z);
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iRet == MTN_API_OK_ZERO)
			{
				idxTuningBlockBondHead = WB_BH_SRCH_HT; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				iRet = mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead]);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iRet == MTN_API_OK_ZERO)
			{
				idxTuningBlockBondHead = WB_BH_LOOPING; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead);astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				iRet = mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead]);
			}
	//		mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_LOOPING], &astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_LOOPING]);
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iRet == MTN_API_OK_ZERO)
			{
				idxTuningBlockBondHead = WB_BH_LOOP_TOP; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				iRet = mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead]);
			}
	//		mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_LOOP_TOP], &astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_LOOP_TOP]);
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iRet == MTN_API_OK_ZERO)
			{
				idxTuningBlockBondHead = WB_BH_TRAJECTORY; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				iRet = mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead]);
			}
	//		mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_TRAJECTORY], &astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_TRAJECTORY]);
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iRet == MTN_API_OK_ZERO)
			{
				idxTuningBlockBondHead = WB_BH_TAIL; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				iRet = mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead]);
			}
	//		mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_TAIL], &astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_TAIL]);
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iRet == MTN_API_OK_ZERO)
			{
				idxTuningBlockBondHead = WB_BH_RESET; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead); astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				iRet = mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneInput[WB_AXIS_BOND_Z][idxTuningBlockBondHead], &astMtnTuneOutput[WB_AXIS_BOND_Z][idxTuningBlockBondHead]);
			}
	//		mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_RESET], &astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_RESET]);

	return iRet;
}

int mtn_wb_acs_download_tbl_x_servo_ctrl_struct(HANDLE hCommunicationHandle, unsigned int uiBlk, CTRL_PARA_ACS *stpServoParaACS);
int mtn_wb_acs_download_tbl_y_servo_ctrl_struct(HANDLE hCommunicationHandle, unsigned int uiBlk, CTRL_PARA_ACS *stpServoParaACS);
void mtn_cvt_servo_para_set_from_tune_to_acs(MTN_TUNE_PARAMETER_SET *stpParaTuningSet, CTRL_PARA_ACS *stpServoParaACS);

int AutoTuneTableX_CurrentSectorBlk()
{
	int iRet = MTN_API_OK_ZERO;
	static CTRL_PARA_ACS stServoParaACSTemp;
			mMotionTuning.mtn_tune_set_tune_axis(astAxisInfoWireBond[WB_AXIS_TABLE_X].iAxisInCtrlCardACS); 
			mtn_wb_tune_set_curr_wb_axis_servo_para_tuning(WB_AXIS_TABLE_X); // astAxisInfoWireBond[WB_AXIS_TABLE_X].iAxisInCtrlCardACS);
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iRet == MTN_API_OK_ZERO)
			{
				idxTuningBlockTable = WB_TBL_MOTION_IDX_MOVE_BTO; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockTable); astMtnTuneInput[WB_AXIS_TABLE_X][idxTuningBlockTable].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				iRet = mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneInput[WB_AXIS_TABLE_X][idxTuningBlockTable], &astMtnTuneOutput[WB_AXIS_TABLE_X][idxTuningBlockTable]);
				//.
				mtn_cvt_servo_para_set_from_tune_to_acs(&astMtnTuneOutput[WB_AXIS_TABLE_X][idxTuningBlockTable].stBestParameterSet, &stServoParaACSTemp);
				mtn_wb_acs_download_tbl_x_servo_ctrl_struct(&astMtnTuneInput[WB_AXIS_TABLE_X][idxTuningBlockTable].stCommHandle, idxTuningBlockTable,&stServoParaACSTemp);

			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iRet == MTN_API_OK_ZERO)
			{
				idxTuningBlockTable = WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockTable); astMtnTuneInput[WB_AXIS_TABLE_X][idxTuningBlockTable].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				iRet = mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneInput[WB_AXIS_TABLE_X][idxTuningBlockTable], &astMtnTuneOutput[WB_AXIS_TABLE_X][idxTuningBlockTable]);
				//.
				mtn_cvt_servo_para_set_from_tune_to_acs(&astMtnTuneOutput[WB_AXIS_TABLE_X][idxTuningBlockTable].stBestParameterSet, &stServoParaACSTemp);
				mtn_wb_acs_download_tbl_x_servo_ctrl_struct(&astMtnTuneInput[WB_AXIS_TABLE_X][idxTuningBlockTable].stCommHandle, idxTuningBlockTable,&stServoParaACSTemp);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iRet == MTN_API_OK_ZERO)
			{
				idxTuningBlockTable = WB_TBL_MOTION_IDX_MOVE_TRAJ; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockTable); astMtnTuneInput[WB_AXIS_TABLE_X][idxTuningBlockTable].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				iRet = mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneInput[WB_AXIS_TABLE_X][idxTuningBlockTable], &astMtnTuneOutput[WB_AXIS_TABLE_X][idxTuningBlockTable]);
				//.
				mtn_cvt_servo_para_set_from_tune_to_acs(&astMtnTuneOutput[WB_AXIS_TABLE_X][idxTuningBlockTable].stBestParameterSet, &stServoParaACSTemp);
				mtn_wb_acs_download_tbl_x_servo_ctrl_struct(&astMtnTuneInput[WB_AXIS_TABLE_X][idxTuningBlockTable].stCommHandle, idxTuningBlockTable,&stServoParaACSTemp);
			}
	return iRet;

}


int AutoTuneTableY_CurrentSectorBlk()
{
	int iRet = MTN_API_OK_ZERO;  // APP_Y_TABLE_ACS_ID
	static CTRL_PARA_ACS stServoParaACSTemp;

	mMotionTuning.mtn_tune_set_tune_axis(astAxisInfoWireBond[WB_AXIS_TABLE_Y].iAxisInCtrlCardACS); 
	mtn_wb_tune_set_curr_wb_axis_servo_para_tuning(WB_AXIS_TABLE_Y); // astAxisInfoWireBond[WB_AXIS_TABLE_Y].iAxisInCtrlCardACS);

			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iRet == MTN_API_OK_ZERO)
			{
				idxTuningBlockTable = WB_TBL_MOTION_IDX_MOVE_BTO; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockTable); astMtnTuneInput[WB_AXIS_TABLE_Y][idxTuningBlockTable].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				iRet = mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneInput[WB_AXIS_TABLE_Y][idxTuningBlockTable], &astMtnTuneOutput[WB_AXIS_TABLE_Y][idxTuningBlockTable]);
				// update astMtnTuneOutput[WB_AXIS_TABLE_Y][idxTuningBlockTable]
				//.
				mtn_cvt_servo_para_set_from_tune_to_acs(&astMtnTuneOutput[WB_AXIS_TABLE_Y][idxTuningBlockTable].stBestParameterSet, &stServoParaACSTemp);
				mtn_wb_acs_download_tbl_y_servo_ctrl_struct(&astMtnTuneInput[WB_AXIS_TABLE_Y][idxTuningBlockTable].stCommHandle, idxTuningBlockTable,&stServoParaACSTemp);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iRet == MTN_API_OK_ZERO)
			{
				idxTuningBlockTable = WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockTable); astMtnTuneInput[WB_AXIS_TABLE_Y][idxTuningBlockTable].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				iRet = mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneInput[WB_AXIS_TABLE_Y][idxTuningBlockTable], &astMtnTuneOutput[WB_AXIS_TABLE_Y][idxTuningBlockTable]);
				//.
				mtn_cvt_servo_para_set_from_tune_to_acs(&astMtnTuneOutput[WB_AXIS_TABLE_Y][idxTuningBlockTable].stBestParameterSet, &stServoParaACSTemp);
				mtn_wb_acs_download_tbl_y_servo_ctrl_struct(&astMtnTuneInput[WB_AXIS_TABLE_Y][idxTuningBlockTable].stCommHandle, idxTuningBlockTable,&stServoParaACSTemp);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iRet == MTN_API_OK_ZERO)
			{
				idxTuningBlockTable = WB_TBL_MOTION_IDX_MOVE_TRAJ; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockTable); astMtnTuneInput[WB_AXIS_TABLE_Y][idxTuningBlockTable].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				iRet = mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneInput[WB_AXIS_TABLE_Y][idxTuningBlockTable], &astMtnTuneOutput[WB_AXIS_TABLE_Y][idxTuningBlockTable]);
				//.
				mtn_cvt_servo_para_set_from_tune_to_acs(&astMtnTuneOutput[WB_AXIS_TABLE_Y][idxTuningBlockTable].stBestParameterSet, &stServoParaACSTemp);
				mtn_wb_acs_download_tbl_y_servo_ctrl_struct(&astMtnTuneInput[WB_AXIS_TABLE_Y][idxTuningBlockTable].stCommHandle, idxTuningBlockTable,&stServoParaACSTemp);
			}
	return iRet;
}

int AutoTuneTableX_AllSectorBlk()
{
	int iRet = MTN_API_OK_ZERO;
	unsigned int ii, uiMaxSector = mtn_wb_tune_get_max_sector_table_x();
	for(ii =0; ii< uiMaxSector; ii++) // 20100730, 9 sectors
	{
		nCurrentTuneSector = ii;  // 20110217
		mMotionTuning.mtn_tune_set_tune_axis(astAxisInfoWireBond[WB_AXIS_TABLE_X].iAxisInCtrlCardACS); 
		mtn_wb_tune_set_curr_wb_axis_servo_para_tuning(WB_AXIS_TABLE_X); // astAxisInfoWireBond[WB_AXIS_TABLE_X].iAxisInCtrlCardACS);  // APP_X_TABLE_ACS_ID
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iRet == MTN_API_OK_ZERO)
			{
				idxTuningBlockTable = WB_TBL_MOTION_IDX_MOVE_BTO; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockTable); 
				astMtnTuneSectorInputTableX[ii][idxTuningBlockTable].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				iRet = mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneSectorInputTableX[ii][idxTuningBlockTable], &astMtnTuneSectorOutputTableX[ii][idxTuningBlockTable]);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iRet == MTN_API_OK_ZERO)
			{
				idxTuningBlockTable = WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockTable); 
				astMtnTuneSectorInputTableX[ii][idxTuningBlockTable].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				iRet = mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneSectorInputTableX[ii][idxTuningBlockTable], &astMtnTuneSectorOutputTableX[ii][idxTuningBlockTable]);
			}
			if(mtn_wb_tune_thread_get_flag_running() == TRUE && iRet == MTN_API_OK_ZERO)
			{
				idxTuningBlockTable = WB_TBL_MOTION_IDX_MOVE_TRAJ; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockTable); 
				astMtnTuneSectorInputTableX[ii][idxTuningBlockTable].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
				iRet = mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneSectorInputTableX[ii][idxTuningBlockTable], &astMtnTuneSectorOutputTableX[ii][idxTuningBlockTable]);
			}
	}
	nCurrentTuneSector = 0;  // 20110217
	return iRet;
}

int AutoTuneTableY_AllSectorBlk()
{
	int iRet = MTN_API_OK_ZERO;
	unsigned int ii, uiMaxSector = mtn_wb_tune_get_max_sector_table_y();
	for(ii =0; ii< uiMaxSector; ii++) // 20100730, 2 sectors
	{
		nCurrentTuneSector = ii;  // 20110217 APP_Y_TABLE_ACS_ID
		mMotionTuning.mtn_tune_set_tune_axis(astAxisInfoWireBond[WB_AXIS_TABLE_Y].iAxisInCtrlCardACS); 
		mtn_wb_tune_set_curr_wb_axis_servo_para_tuning(WB_AXIS_TABLE_Y); // astAxisInfoWireBond[WB_AXIS_TABLE_Y].iAxisInCtrlCardACS);
		if(mtn_wb_tune_thread_get_flag_running() == TRUE && iRet == MTN_API_OK_ZERO)
		{
			idxTuningBlockTable = WB_TBL_MOTION_IDX_MOVE_BTO; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockTable); 
			astMtnTuneSectorInputTableY[ii][idxTuningBlockTable].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
			iRet = mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneSectorInputTableY[ii][idxTuningBlockTable], &astMtnTuneSectorOutputTableY[ii][idxTuningBlockTable]);
		}
		if(mtn_wb_tune_thread_get_flag_running() == TRUE && iRet == MTN_API_OK_ZERO)
		{
			idxTuningBlockTable = WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockTable); 
			astMtnTuneSectorInputTableY[ii][idxTuningBlockTable].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
			iRet = mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneSectorInputTableY[ii][idxTuningBlockTable], &astMtnTuneSectorOutputTableY[ii][idxTuningBlockTable]);
		}
		if(mtn_wb_tune_thread_get_flag_running() == TRUE && iRet == MTN_API_OK_ZERO)
		{
			idxTuningBlockTable = WB_TBL_MOTION_IDX_MOVE_TRAJ; mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockTable); 
			astMtnTuneSectorInputTableY[ii][idxTuningBlockTable].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
			iRet = mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneSectorInputTableY[ii][idxTuningBlockTable], &astMtnTuneSectorOutputTableY[ii][idxTuningBlockTable]);
		}
	}
	nCurrentTuneSector = 0;  // 20110217

	return iRet;
}

//#define __DEBUG__

//extern int mtn_wb_table_servo_go_to_safety_position(HANDLE stCommHandle, int iAxisACS_TableX, int iAxisACS_TableY);
int mtn_wb_tune_xyz_multi_cases(HANDLE stCommHandle)
{
int iRet = MTN_API_OK_ZERO;
double dBondHeadSafeUpperPosition; 

static int iFlagMachType = get_sys_machine_type_flag();

//#ifdef __PREV__
	mtnscope_tuning_declare_var_on_controller(stCommHandle);  // 20120117
//#endif
	dBondHeadSafeUpperPosition = mtn_wb_init_bh_relax_position_from_sp(stCommHandle);  // 20110806

	//if(cFlagTuningRoutine == SERVO_PARA_TUNING_ROUTINE_AUTO_FULL)
	//{
	// Initialize all servo parameters, other NOT-tuned parameters, from control-card, // 20110202
	mtn_tune_upload_wb_servo_ctrl(stCommHandle);
		if(cTuneBlocks_FlagBondHeadZ == TRUE)
		{
			// MOVE XY-Table to SafePosition
			iRet = mtn_wb_table_servo_go_to_safety_position(stCommHandle, 
				astAxisInfoWireBond[WB_AXIS_TABLE_X].iAxisInCtrlCardACS, 
				astAxisInfoWireBond[WB_AXIS_TABLE_Y].iAxisInCtrlCardACS);

			// Tuning BondHead
			unsigned int ii;
			for(ii = 0; ii< MAX_BLK_PARAMETER; ii++)
			{
				astMtnTuneInput[WB_AXIS_BOND_Z][ii].stTuningTheme.iThemeType = THEME_MIN_CEOUS_BH_DPE_W_DAC;
			}

			mMotionTuning.mtn_tune_set_tune_axis(sys_get_acs_axis_id_bnd_z()); mtn_wb_tune_set_curr_wb_axis_servo_para_tuning(WB_AXIS_BOND_Z);
			iRet = AutoTuneBondHeadBlk();
		}

		// Move BondHead to FireLevel
		if(iFlagMachType != WB_STATION_XY_VERTICAL
			&& iFlagMachType != WB_STATION_XY_TOP
			&& iFlagMachType != WB_STATION_WIRE_CLAMP)
			//&& theAcsServo.GetServoOperationMode() != OFFLINE_MODE) // 20110914, stCommHandle != 
		{
			acsc_ToPoint(stCommHandle, 0, // start up the motion immediately
				sys_get_acs_axis_id_bnd_z(), dBondHeadSafeUpperPosition, NULL);  // astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_RESET].stTuningCondition.dMovePointion_2, 20110711
			while(mtn_qc_is_axis_still_moving(stCommHandle, sys_get_acs_axis_id_bnd_z()) == TRUE){		Sleep(1);}
			if(mtn_qc_is_axis_locked_safe(stCommHandle, sys_get_acs_axis_id_bnd_z()) == FALSE)
			{
				iRet = MTN_API_ERROR;
				goto label_mtn_wb_tune_xyz_multi_cases;
			}
		}
		if(cTuneBlocks_FlagTableX)
		{
			// Tuning Table X
#ifdef __DEBUG__
	FILE * fptr;
    fopen_s(&fptr, "TuneDbg1.txt", "w");
	fprintf(fptr, "iFlagTableSectorTuning = %d", iFlagTableSectorTuning);
	fclose(fptr);
#endif  // __DEBUG__
			if(iFlagTableSectorTuning == TRUE)
			{
				iRet = AutoTuneTableX_AllSectorBlk();
			}
			else
			{
				iRet = AutoTuneTableX_CurrentSectorBlk();
			}
			mtn_wb_tune_consolidate_table_x_servo_para();

			if(iFlagTableSectorTuning == TRUE)
			{
				mtn_wb_table_x_update_acs_parameter_servo_struct_multi_blk(	MAX_BLK_PARAMETER);
			}
			else
			{
				mtn_wb_update_acs_parameter_servo_struct_common_blk(
					mtn_wb_tune_get_max_sector_table_x(), WB_AXIS_TABLE_X, &astMtnTuneOutput[WB_AXIS_TABLE_X][0].stBestParameterSet);
				//MTN_TUNE_PARAMETER_SET *stpTuneOutParameterSet);
			}
			if( mtnapi_init_master_config_acs("C:\\WbData\\ParaBase\\ServoMaster.ini", stCommHandle) == MTN_API_OK_ZERO) // NOT, mtnapi_init_servo_control_para_acs
			{
				mtn_tune_upload_wb_speed_profile(stCommHandle); // 20110202
				mtnapi_rename_bakup_servo_parameter_acs_per_axis(WB_AXIS_TABLE_X);  // 20110509
				mtnapi_save_servo_parameter_acs_per_axis_no_label(WB_AXIS_TABLE_X);
			}
		}
#ifdef __DEBUG__
	FILE * fptr;
    fopen_s(&fptr, "TuneDbg2.txt", "w");
	fprintf(fptr, "cTuneBlocks_FlagTableY = %d; iFlagTableSectorTuning = %d\n", cTuneBlocks_FlagTableY, iFlagTableSectorTuning);
	fprintf(fptr, "Axis:%d, Speed: %5.1f, %5.1f, %5.1f; Position: [%6.1f, %6.1f], [%6.1f, %6.1f]\n", 
		astMtnTuneInput[WB_AXIS_TABLE_Y][0].iAxisTuning,
		astMtnTuneInput[WB_AXIS_TABLE_Y][0].stTuningCondition.stMtnSpeedProfile.dMaxVelocity,
		astMtnTuneInput[WB_AXIS_TABLE_Y][0].stTuningCondition.stMtnSpeedProfile.dMaxAcceleration,
		astMtnTuneInput[WB_AXIS_TABLE_Y][0].stTuningCondition.stMtnSpeedProfile.dMaxJerk,
		astMtnTuneInput[WB_AXIS_TABLE_Y][0].stTuningCondition.dMovePosition_1,
		astMtnTuneInput[WB_AXIS_TABLE_Y][0].stTuningCondition.dMovePointion_2,
		astMtnTuneInput[WB_AXIS_TABLE_Y][0].stTuningCondition.d2ndMovePosition_1,
		astMtnTuneInput[WB_AXIS_TABLE_Y][0].stTuningCondition.d2ndMovePosition_2);
	fclose(fptr);
#endif  // __DEBUG__

		if(cTuneBlocks_FlagTableY)
		{
			// Tuning Table Y
			if(iFlagTableSectorTuning == TRUE)
			{
				iRet = AutoTuneTableY_AllSectorBlk();
			}
			else
			{
				iRet = AutoTuneTableY_CurrentSectorBlk();
			}
			mtn_wb_tune_consolidate_table_y_servo_para();

			if(iFlagTableSectorTuning == TRUE)
			{
				mtn_wb_table_y_update_acs_parameter_servo_struct_multi_blk(	mtn_wb_tune_get_max_sector_table_y());
			}
			else
			{
				mtn_wb_update_acs_parameter_servo_struct_common_blk(
					mtn_wb_tune_get_max_sector_table_y(), WB_AXIS_TABLE_Y, &astMtnTuneOutput[WB_AXIS_TABLE_Y][0].stBestParameterSet);
			}
			if( mtnapi_init_master_config_acs("C:\\WbData\\ParaBase\\ServoMaster.ini", stCommHandle) == MTN_API_OK_ZERO) // NOT, mtnapi_init_servo_control_para_acs
			{
				//for(int ii =0; ii<NUM_TABLE_SPEED_PROF_BLK; ii++)
				//{
				//	mtn_tune_upload_wb_table_y_speed_profile_blk(stCommHandle, ii);
				//}
				mtn_tune_upload_wb_speed_profile(stCommHandle); // 20110202
				mtnapi_rename_bakup_servo_parameter_acs_per_axis(WB_AXIS_TABLE_Y);  // 20110509
				mtnapi_save_servo_parameter_acs_per_axis_no_label(WB_AXIS_TABLE_Y);
			}
		}
//	}

label_mtn_wb_tune_xyz_multi_cases:
	return iRet;
}

void mtn_dll_wb_tune_initialization()
{
	mtn_tune_init_wb_bondhead_tuning_position_set(); // 20120404
	mtn_tune_init_wb_table_x_tuning_position_set();  // 20120404
	int iFlagMachType = get_sys_machine_type_flag();
	if(iFlagMachType == WB_MACH_TYPE_VLED_FORK || iFlagMachType == WB_STATION_XY_VERTICAL)  // 20120815
	{
		mtn_tune_init_wb_table_y_tuning_vled_position_set();
	}
	else if(iFlagMachType == WB_MACH_TYPE_HORI_LED || iFlagMachType == WB_STATION_XY_TOP || iFlagMachType == BE_WB_HORI_20T_LED ||
		iFlagMachType == BE_WB_ONE_TRACK_18V_LED)
	{
		if(iFlagMachType == BE_WB_ONE_TRACK_18V_LED)
		{
			mtn_tune_init_wb_table_y_tuning_vled_position_set();
		}
		else
		{
			mtn_tune_init_wb_table_y_tuning_hori_led_position_set();
		}
	}
	else
	{
		mtn_tune_init_wb_table_y_tuning_vled_position_set();
	}

	InitWireBondServoAxisName();// 20120815

	mtn_tune_init_servo_para_tuning_config();
	mtn_tune_set_initial_tuning_para();
	mtnapi_init_tune_para_bound();
}

// To be used in DLL
UINT uiBitComboFlag_ServoTuning;

UINT mtn_dll_wb_tune_servo_move_2points_thread()
{
	UINT iRet;
	int iFlagStopThread_ServoTuningTemp;
	iFlagStopThread_ServoTuningTemp = mtn_dll_wb_servo_tune_get_stop_flag();
	while(iFlagStopThread_ServoTuningTemp == FALSE)
	{
		mtnapi_dll_save_wb_speed_servo_parameter_acs(stServoControllerCommSet.Handle); // SaveCurrent Parameters to "C:\\WbData\\ParaBase\\" , 20110425
		iRet = mtn_wb_tune_xyz_multi_cases(stServoControllerCommSet.Handle);
		mtn_dll_wb_servo_tune_set_stop_flag(TRUE); // iFlagStopThread_ServoTuning = TRUE;
	}
	return iRet;
}

static HANDLE hLocalDllAcsHandle; static int iFlagByLocalAcsComm = 0;
UINT mtn_dll_wb_tune_servo_start_move_2points_thread( LPVOID pParam )
{
			cTuneBlocks_FlagTableX = uiBitComboFlag_ServoTuning & WB_SERVO_TUNING_BIT_TUNE_2POINTS_X; 
			cTuneBlocks_FlagTableY = uiBitComboFlag_ServoTuning & WB_SERVO_TUNING_BIT_TUNE_2POINTS_Y;
			cTuneBlocks_FlagBondHeadZ = uiBitComboFlag_ServoTuning & WB_SERVO_TUNING_BIT_TUNE_2POINTS_Z;
	iFlagTableSectorTuning = uiBitComboFlag_ServoTuning & WB_SERVO_TUNING_BIT_TUNE_SECTOR_XY;

#ifdef __DEBUG__
	FILE * fptr;
    fopen_s(&fptr, "TuneDbg.txt", "w");
	fprintf(fptr, "uiBitComboFlag_ServoTuning = %d", uiBitComboFlag_ServoTuning);
	fclose(fptr);
#endif  // __DEBUG__
	UINT iRet;
	int iFlagStopThread_ServoTuningTemp;
	static char *strSaveFilename_SpeedServoWb = "C:\\WbData\\ParaBase\\ServoMaster.ini";

	iFlagStopThread_ServoTuningTemp = mtn_dll_wb_servo_tune_get_stop_flag();
	mtn_wb_tune_thread_set_flag_running(TRUE); // 20120404

	if((iRet =sys_init_acs_communication()) == MTN_API_OK_ZERO)
	{
		hLocalDllAcsHandle = stServoControllerCommSet.Handle;
		mtnscope_tuning_declare_var_on_controller(hLocalDllAcsHandle);  // 20120117
		// Buffer Program
#ifdef  __INIT_ACS_BUFF__
		acs_buff_prog_init_comm(stServoControllerCommSet.Handle);
		mtn_api_clear_acs_buffer_prof();  // Each time stop and clean the buffer program, before downloading
		Sleep(200);
		mtn_api_init_acs_buffer_prog();
#endif  // __INIT_ACS_BUFF__
		iFlagByLocalAcsComm = 1;
	}
	// Initialization
	mtnapi_confirm_para_base_path_exist();  // 20110404, Protection to handle
	iRet = mtnapi_init_master_config_acs(strSaveFilename_SpeedServoWb, hLocalDllAcsHandle); // NOT, mtnapi_init_servo_control_para_acs
	iRet = mtnapi_upload_wb_servo_speed_parameter_acs(hLocalDllAcsHandle);
	mtn_dll_wb_tune_initialization();
	Sleep(1000);


		if(iRet == MTN_API_OK_ZERO)
		{
			iRet = mtnapi_save_servo_parameter_acs();
		}
		////////////////////////
		////////////   Actual routine to start XY tuning
		////////////////////////
		iRet = mtn_wb_tune_xyz_multi_cases(hLocalDllAcsHandle); // stServoControllerCommSet.Handle
		mtn_dll_wb_servo_tune_set_stop_flag(TRUE); //iFlagStopThread_ServoTuning = TRUE;
		iFlagStopThread_ServoTuningTemp = mtn_dll_wb_servo_tune_get_stop_flag();

	CopyFile(_T("C:\\WbData\\ParaBase\\ctrl_acsc_x.ini"), _T("C:\\WbData\\DefParaBase\\ctrl_acsc_x.ini"), 0);
	CopyFile(_T("C:\\WbData\\ParaBase\\ctrl_acsc_y.ini"), _T("C:\\WbData\\DefParaBase\\ctrl_acsc_y.ini"), 0);
	CopyFile(_T("C:\\WbData\\ParaBase\\ctrl_acsc_z.ini"), _T("C:\\WbData\\DefParaBase\\ctrl_acsc_z.ini"), 0);

	if(iFlagByLocalAcsComm == 1)
	{
#ifdef __INIT_ACS_BUFF__
		mtn_api_clear_acs_buffer_prof();
#endif  // __INIT_ACS_BUFF__

		acsc_CloseComm(stServoControllerCommSet.Handle);
		iFlagByLocalAcsComm = 0;  // Protection, 20110121
	}

	return iRet;//mtn_dll_wb_tune_servo_move_2points_thread();
}

int __cdecl _chdir(_In_z_ const char * _Path);

//#include <direct.h>
#include "acs_buff_prog.h"


void api_copy_file_from_to(char *strResource, char *strTarget)
{
	FILE *fptrSource, *fptrTarget;
	fptrSource = NULL;
	fptrTarget = NULL;
	char buffer[BUFSIZ+1];

	fopen_s(&fptrSource, strResource, "r");
	fopen_s(&fptrTarget, strTarget, "w");

	if(fptrSource != NULL
		&& fptrTarget != NULL)
	{
		while(!feof(fptrSource))
		{
			fgets(buffer, BUFSIZ, fptrSource);
			fputs(buffer, fptrTarget);
		}

		fclose(fptrSource); fclose(fptrTarget);
	}
}


#ifndef MOTALGO_DLL_EXPORTS


#endif // ndef MOTALGO_DLL_EXPORTS