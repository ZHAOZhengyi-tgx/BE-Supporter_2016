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

#include <direct.h>


#include "MtnWbDef.h"
#include "MotAlgo_DLL.h"
#include "mtndefin.h"
#include "MtnApi.h"
#include "MtnTune.h"
#include "MtnTesterResDef.h"
#include "MtnInitAcs.h"

//////////////////////////////////////////////////////////////////////
// External variables declaration
extern AXIS_INFO_WIRE_BOND astAxisInfoWireBond[MAX_SERVO_AXIS_WIREBOND];
extern char *astrMachineTypeNameLabel_en[];
extern MTN_TUNE_GENETIC_INPUT stMtnTuneInputTableX_DRA;
extern MTN_TUNE_GENETIC_OUTPUT stMtnTuneOutputTableX_DRA;
extern MTN_TUNE_GENETIC_INPUT stMtnTuneInputTableY_DRA;
extern MTN_TUNE_GENETIC_OUTPUT stMtnTuneOutputTableY_DRA;

extern MTN_TUNE_GENETIC_INPUT astMtnTuneInput[MAX_SERVO_AXIS_WIREBOND][MAX_BLK_PARAMETER]; 
extern MTN_TUNE_GENETIC_OUTPUT astMtnTuneOutput[MAX_SERVO_AXIS_WIREBOND][MAX_BLK_PARAMETER];

extern MTN_TUNE_GENETIC_INPUT astMtnTuneSectorInputTableX[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];
extern MTN_TUNE_GENETIC_OUTPUT astMtnTuneSectorOutputTableX[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];
extern MTN_TUNE_GENETIC_INPUT astMtnTuneSectorInputTableY[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];
extern MTN_TUNE_GENETIC_OUTPUT astMtnTuneSectorOutputTableY[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];

extern COMM_SETTINGS stServoControllerCommSet;

//////////////////////////////////////////////////
// Global Variable Definition
MTN_TUNE_CASE astMotionTuneFeasibleParameterB1W[MAX_CASE_FEASIBLE_B1W_PARAMETER];
int nFeasibleParameter = 0;

char strWbTuningNewFolderName[128] = "D:\\MT\\TuneTemp";

void mtn_tune_make_output_folder(int iWbAxis)
{
struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;
	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);
	// Mechanical configuration
	int iMechCfg = get_sys_machine_type_flag();

	if(iWbAxis == WB_AXIS_BOND_Z) 
	{
		sprintf_s(strWbTuningNewFolderName, 128, "D:\\MT\\TuneB1W_%s-%d.%d.%d.H%d.M%d_%s-%d",
			astAxisInfoWireBond[iWbAxis].strAxisNameEn,
			stpTime->tm_year +1900, stpTime->tm_mon +1, stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, 
			astrMachineTypeNameLabel_en[iMechCfg], get_sys_machine_serial_num());
	}
	else
	{
		sprintf_s(strWbTuningNewFolderName, 128, "D:\\MT\\Tune_%s-%d.%d.%d.H%d.M%d_%s-%d",
			astAxisInfoWireBond[iWbAxis].strAxisNameEn,
			stpTime->tm_year +1900, stpTime->tm_mon +1, stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, 
			astrMachineTypeNameLabel_en[iMechCfg], get_sys_machine_serial_num());
	}

	if(_chdir("D:\\MT"))
	{
		_mkdir("D:\\MT");
	}
	_mkdir(strWbTuningNewFolderName);

}

int _mtn_wb_save_waveform_write_file_ptr(FILE *fptr)
{
int iRet = MTN_API_OK_ZERO;

	if(fptr != NULL)
	{

double *pdRefPosnX, *pdFeedPosnX, *pdRefPosnY, *pdFeedPosnY;
double *pdRefPosnZ, *pdFeedPosnZ, *pdWireClampCmd, *pdMotorFlagZ;

		mtn_tune_get_waveform_data_ptr(&pdRefPosnX, &pdFeedPosnX, &pdRefPosnY, &pdFeedPosnY, 
									&pdRefPosnZ, &pdFeedPosnZ, &pdWireClampCmd, &pdMotorFlagZ);

		fprintf_s(fptr, "[RPX, FPX, RPY, FPY, RPZ, FPZ, MFZ, DW] = [\n");
		for(int ii=0; ii<LEN_UPLOAD_ARRAY; ii++)
		{
			fprintf_s(fptr, "%f, %f, %f, %f, %f, %f, %f, %f;\n", pdRefPosnX[ii], pdFeedPosnX[ii], pdRefPosnY[ii], pdFeedPosnY[ii],
				pdRefPosnZ[ii], pdFeedPosnZ[ii], pdWireClampCmd[ii], pdMotorFlagZ[ii]);
		}
		fprintf_s(fptr, "];\n");
		fclose(fptr);
	}
	else
	{
		iRet = MTN_API_ERR_FILE_PTR;
	}

	return iRet;
}
int mtn_wb_save_waveform_with_path(char *strNamePrefWithFolderPath)
{
struct tm stTime;
__time64_t stLongTime;
char strFilenameWbWaveform[512];
FILE *fptr;
int iRet = MTN_API_OK_ZERO;

	_time64(&stLongTime);
	_localtime64_s(&stTime, &stLongTime);
	sprintf_s(strFilenameWbWaveform, 512, "%s_%d-%d-%d_%d-%d.m", 
			strNamePrefWithFolderPath, stTime.tm_year +1900, stTime.tm_mon +1, 
			stTime.tm_mday, stTime.tm_hour, stTime.tm_min);
	fopen_s(&fptr, strFilenameWbWaveform, "w");

	iRet = _mtn_wb_save_waveform_write_file_ptr(fptr);

	return iRet;
}

int mtn_tune_save_waveform(char *strNamePrefix)
{
struct tm stTime;
__time64_t stLongTime;
char strFilenameWbWaveform[512];
FILE *fptr;
int iRet = MTN_API_OK_ZERO;

	_time64(&stLongTime);
	_localtime64_s(&stTime, &stLongTime);
	sprintf_s(strFilenameWbWaveform, 512, "%s\\%s_%d-%d-%d_%d-%d.m", 
			strWbTuningNewFolderName, // 20120522
			strNamePrefix, stTime.tm_year +1900, stTime.tm_mon +1, 
			stTime.tm_mday, stTime.tm_hour, stTime.tm_min);
	fopen_s(&fptr, strFilenameWbWaveform, "w");

	iRet = _mtn_wb_save_waveform_write_file_ptr(fptr);

	return iRet;
}

void mtn_wb_tune_b1w_init_feasible_set()
{
	nFeasibleParameter = 0;
	memset(&astMotionTuneFeasibleParameterB1W[0], 0, MAX_CASE_FEASIBLE_B1W_PARAMETER * sizeof(MTN_TUNE_CASE));

	return;
}

void mtn_wb_tune_b1w_record_feasible_parameter(WB_TUNE_B1W_BH_OBJ *stpWbTuneB1wObj, BOND_HEAD_PERFORMANCE  *stpBondHeadPerformance, 
											   int iObjSectionFlagB1W, MTN_TUNE_PARAMETER_SET *stpCurrTuneBondHeadParaB1W)
{
	astMotionTuneFeasibleParameterB1W[nFeasibleParameter].dTuningObj = stpWbTuneB1wObj->dObj[iObjSectionFlagB1W];
	astMotionTuneFeasibleParameterB1W[nFeasibleParameter].stTuneBondHeadPerformanceB1W = *stpBondHeadPerformance;

	MTN_TUNE_PARAMETER_SET stpParaB1W;
	//if(nFeasibleParameter == 0)
	//{
	//	astMotionTuneFeasibleParameterB1W[nFeasibleParameter].stServoParaB1W[iObjSectionFlagB1W].stMtnPara = *stpCurrTuneBondHeadParaB1W;
		for(int ii = 0; ii<= WB_BH_2ND_CONTACT; ii++)
		{
			//if(ii != iObjSectionFlagB1W)
			//{
				mtn_b1w_read_para_bh_servo(stServoControllerCommSet.Handle, &stpParaB1W, ii);
				astMotionTuneFeasibleParameterB1W[nFeasibleParameter].stServoParaB1W[ii].stMtnPara = stpParaB1W;
			//}
		}
	//}
	astMotionTuneFeasibleParameterB1W[nFeasibleParameter].stTuneB1wObjValues = *stpWbTuneB1wObj;  // 20120514
	nFeasibleParameter ++;
	nFeasibleParameter = nFeasibleParameter % MAX_CASE_FEASIBLE_B1W_PARAMETER;
}

int mtn_wb_tune_b1w_check_is_feasible_parameter(double adObj[], WB_TUNE_B1W_BH_PASS_FLAG *stpWbTuneB1wPassFlag)
{
	int iRet = TRUE;

	if(adObj[WB_BH_SRCH_HT] > _B1W_TUNE_OBJ_TH_MOVE_1ST_SRCH)
	{
		stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_SRCH_HT] = FALSE;
		iRet = FALSE;
	}
	else
	{
		stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_SRCH_HT] = TRUE;
	}

	if(adObj[WB_BH_1ST_CONTACT] > _B1W_TUNE_OBJ_TH_1ST_SRCH)
	{
		stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_1ST_CONTACT] = FALSE;
		iRet = FALSE;
	}
	else
	{
		stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_1ST_CONTACT] = TRUE;
	}

	stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_IDLE] = TRUE;
	stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_LOOPING] = TRUE;
	stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_LOOP_TOP] = TRUE;
#ifdef __CHECK_FEASIBLE_LOOPING__
	if(adObj[WB_BH_LOOPING] > _B1W_TUNE_OBJ_TH_LOOPING)
	{
		stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_LOOPING] = FALSE;
		iRet = FALSE;
	}
	else
	{
		stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_LOOPING] = TRUE;
	}
	if(adObj[WB_BH_LOOP_TOP] > _B1W_TUNE_OBJ_TH_LOOPTOP)
	{
		stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_LOOP_TOP] = FALSE;
		iRet = FALSE;
	}
	else
	{
		stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_LOOP_TOP] = TRUE;
	}
#endif  // __CHECK_FEASIBLE_LOOPING__
	if(adObj[WB_BH_TRAJECTORY] > _B1W_TUNE_OBJ_TH_TRAJECTORY)
	{
		stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_TRAJECTORY] = FALSE;
		iRet = FALSE;
	}
	else
	{
		stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_TRAJECTORY] = TRUE;
	}
	if(adObj[WB_BH_TAIL] > _B1W_TUNE_OBJ_TH_TAIL)
	{
		stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_TAIL] = FALSE;
		iRet = FALSE;
	}
	else
	{
		stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_TAIL] = TRUE;
	}
	if(adObj[WB_BH_RESET] > _B1W_TUNE_OBJ_TH_RESET)
	{
		stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_RESET] = FALSE;
		iRet = FALSE;
	}
	else
	{
		stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_RESET] = TRUE;
	}

	if(adObj[WB_BH_2ND_CONTACT] > _B1W_TUNE_OBJ_TH_2ND_SRCH)
	{
		stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_2ND_CONTACT] = FALSE;
		iRet = FALSE;
	}
	else
	{
		stpWbTuneB1wPassFlag->aiTuneB1W_PassFlag[WB_BH_2ND_CONTACT] = TRUE;
	}

	return iRet;
}

void mtn_wb_tune_b1w_save_feasible_set_file()
{
char strFilenameWbPerformIndex[512];
FILE *fptr = NULL;
struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;

	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);
	int iMechCfg = get_sys_machine_type_flag();
	sprintf_s(strFilenameWbPerformIndex, 512, "%s\\WB%s-%d_B1W_FsblParaSet_%d-%d-%d_%d-%d-%d.m", 
				strWbTuningNewFolderName,   // 20120522
				astrMachineTypeNameLabel_en[iMechCfg],		get_sys_machine_serial_num(),
				stpTime->tm_year +1900, stpTime->tm_mon +1, 
				stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, stpTime->tm_sec);

	if(nFeasibleParameter > 0 )
	{
		fopen_s(&fptr, strFilenameWbPerformIndex, "w");
	}
	if(fptr != NULL)
	{
			// Tuning History
		for(int ii=0; ii<nFeasibleParameter; ii++)
		{
			fprintf(fptr, "astTuneCaseB1W(%d).ObjValues = [%4.0f, %4.0f, %4.0f, %4.0f, %4.0f, %4.0f, %4.0f, %4.0f, %4.0f] \n",
					ii, 	astMotionTuneFeasibleParameterB1W[ii].stTuneB1wObjValues.dObj[0], astMotionTuneFeasibleParameterB1W[ii].stTuneB1wObjValues.dObj[1],
					astMotionTuneFeasibleParameterB1W[ii].stTuneB1wObjValues.dObj[2], astMotionTuneFeasibleParameterB1W[ii].stTuneB1wObjValues.dObj[3],
					astMotionTuneFeasibleParameterB1W[ii].stTuneB1wObjValues.dObj[4], astMotionTuneFeasibleParameterB1W[ii].stTuneB1wObjValues.dObj[5],
					astMotionTuneFeasibleParameterB1W[ii].stTuneB1wObjValues.dObj[6], astMotionTuneFeasibleParameterB1W[ii].stTuneB1wObjValues.dObj[7],
					astMotionTuneFeasibleParameterB1W[ii].stTuneB1wObjValues.dObj[8]);

			fprintf(fptr, "astTuneCaseB1W(%d).astTunePara(1) = [%4.1f, %4.0f, %4.0f, %4.0f]; astTuneCaseB1W(%d).astTunePara(2) = [%4.1f, %4.0f, %4.0f, %4.0f]; astTuneCaseB1W(%d).astTunePara(3) = [%4.1f, %4.0f, %4.0f, %4.0f]; astTuneCaseB1W(%d).astTunePara(4) = [%4.1f, %4.0f, %4.0f, %4.0f]; astTuneCaseB1W(%d).astTunePara(5) = [%4.1f, %4.0f, %4.0f, %4.0f]; astTuneCaseB1W(%d).astTunePara(6) = [%4.1f, %4.0f, %4.0f, %4.0f]; astTuneCaseB1W(%d).astTunePara(7) = [%4.1f, %4.0f, %4.0f, %4.0f]; astTuneCaseB1W(%d).astTunePara(8) = [%4.1f, %4.0f, %4.0f, %4.0f]; astTuneCaseB1W(%d).astTunePara(9) = [%4.1f, %4.0f, %4.0f, %4.0f];\n ", 
					ii,		astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[0].stMtnPara.dVelLoopKP, astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[0].stMtnPara.dVelLoopKI, 
					astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[0].stMtnPara.dPosnLoopKP,	astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[0].stMtnPara.dAccFFC,
					ii,		astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[1].stMtnPara.dVelLoopKP, astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[1].stMtnPara.dVelLoopKI, 
					astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[1].stMtnPara.dPosnLoopKP,	astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[1].stMtnPara.dAccFFC,	
					ii,		astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[2].stMtnPara.dVelLoopKP, astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[2].stMtnPara.dVelLoopKI, 
					astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[2].stMtnPara.dPosnLoopKP,	astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[2].stMtnPara.dAccFFC,
					ii,		astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[3].stMtnPara.dVelLoopKP, astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[3].stMtnPara.dVelLoopKI, 
					astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[3].stMtnPara.dPosnLoopKP,	astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[3].stMtnPara.dAccFFC,
					ii,		astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[4].stMtnPara.dVelLoopKP, astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[4].stMtnPara.dVelLoopKI, 
					astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[4].stMtnPara.dPosnLoopKP,	astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[4].stMtnPara.dAccFFC,
					ii,		astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[5].stMtnPara.dVelLoopKP, astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[5].stMtnPara.dVelLoopKI, 
					astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[5].stMtnPara.dPosnLoopKP,	astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[5].stMtnPara.dAccFFC,
					ii,		astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[6].stMtnPara.dVelLoopKP, astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[6].stMtnPara.dVelLoopKI, 
					astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[6].stMtnPara.dPosnLoopKP,	astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[6].stMtnPara.dAccFFC,
					ii,		astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[7].stMtnPara.dVelLoopKP, astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[7].stMtnPara.dVelLoopKI, 
					astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[7].stMtnPara.dPosnLoopKP,	astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[7].stMtnPara.dAccFFC,
					ii,		astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[8].stMtnPara.dVelLoopKP, astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[8].stMtnPara.dVelLoopKI, 
					astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[8].stMtnPara.dPosnLoopKP,	astMotionTuneFeasibleParameterB1W[ii].stServoParaB1W[8].stMtnPara.dAccFFC
					);

		}
		fclose(fptr);
	}

}

#include "acs_buff_prog.h"
void mtn_tune_contact_save_data(HANDLE m_hHandle, MTN_SCOPE *pstSystemScope, int uiAxisAcs, char *strFilename, char *strCommentTitleInFirstLine)
{
	char strTuneOutFileFullPath[256];
	sprintf_s(strTuneOutFileFullPath, 256, "%s_%s", strWbTuningNewFolderName, strFilename);
	// Upload data
	FILE *fpData;
	if(mtnscope_upload_acsc_data_varname(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "rAFT_Scope") 
		== MTN_API_OK_ZERO)
	{	
		fopen_s(&fpData, strTuneOutFileFullPath, "w");  // strFilename, 20120522

		unsigned int ii, jj;

		if(fpData != NULL)
		{
			fprintf(fpData, "aTuneB1W_StopSrch = [");
			for(ii = 0; ii< pstSystemScope->uiDataLen; ii++)
			{
				for(jj = 0; jj< pstSystemScope->uiNumData; jj++)
				{
					if(jj == pstSystemScope->uiNumData - 1)
					{	
						fprintf(fpData, "%8.2f", gdScopeCollectData[jj* pstSystemScope->uiDataLen + ii]);
					}
					else
					{
						fprintf(fpData, "%8.2f,  ", gdScopeCollectData[jj* pstSystemScope->uiDataLen + ii]);
					}
				}
				fprintf(fpData, "\n");
			}
			fprintf(fpData, "];\n");
			fprintf(fpData, strCommentTitleInFirstLine, uiAxisAcs);
			fclose(fpData);
		}
	}

}

#define MTN_WB_TUNE_GROUP_XYZ_OUT_FILE    "WbSrvoTuneOut"
void mtn_wb_tune_save_group_tuning_output(char *strFilename, int iLenChar)
{
	mtn_wb_tune_save_group_tuning_parameter(MTN_WB_TUNE_GROUP_XYZ_OUT_FILE, strFilename, iLenChar);
}

#define MTN_WB_GROUP_XYZ_BEF_TUNE_FILE    "WbSrvoBefTune"
void mtn_wb_tune_save_group_para_bef_tuning()
{
	char strParaFilenameTuningBefTuning[128];
	mtn_wb_tune_save_group_tuning_parameter(MTN_WB_GROUP_XYZ_BEF_TUNE_FILE, strParaFilenameTuningBefTuning, 128);
}

void mtn_wb_tune_save_one_tune_condition_output(FILE *fptr, MTN_TUNE_GENETIC_INPUT *stpMtnTuneInput, MTN_TUNE_GENETIC_OUTPUT *stpMtnTuneOutput)
{
	fprintf(fptr, "aPosnTuning = [%8.1f, %8.1f]\n", stpMtnTuneInput->stTuningCondition.dMovePosition_1, stpMtnTuneInput->stTuningCondition.dMovePointion_2);
	fprintf(fptr, "aParameterTuneOut = [%8.1f, %8.1f, %8.1f, %8.1f]\n\n", 
		stpMtnTuneOutput->stBestParameterSet.dVelLoopKP, stpMtnTuneOutput->stBestParameterSet.dVelLoopKI,
		stpMtnTuneOutput->stBestParameterSet.dPosnLoopKP, stpMtnTuneOutput->stBestParameterSet.dAccFFC);
}

char *astrPassOrFail[2] = {
	"P",
	"F"
};

int mtn_wb_tune_save_group_tuning_parameter(char *strFilenamePrefix, char *strActualFilename, int iLenChar)
{
struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;
char strFilenameWbPerformIndex[512];
FILE *fptr;
int iRet = MTN_API_OK_ZERO;

	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);
	sprintf_s(strFilenameWbPerformIndex, 512, "%s\\%s_%d-%d-%d_%d-%d-%d.txt", strWbTuningNewFolderName,
				strFilenamePrefix, stpTime->tm_year +1900, stpTime->tm_mon +1, 
				stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, stpTime->tm_sec);
	sprintf_s(strActualFilename, iLenChar, "%s", strFilenameWbPerformIndex);
	fopen_s(&fptr, strFilenameWbPerformIndex, "w");
	if(fptr != NULL)
	{
		// Bond Head Parameters
		fprintf(fptr, "%%[BOND_HEAD_Z_0] %% IDLE, %s\n", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_IDLE].iFlagTuningIsFail]);
		mtn_wb_tune_save_one_tune_condition_output(fptr, &astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_IDLE], 	&astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_IDLE]);

		fprintf(fptr, "%%[BOND_HEAD_Z_1] %% 1ST_CONTACT, %s\n", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_1ST_CONTACT].iFlagTuningIsFail]);
		mtn_wb_tune_save_one_tune_condition_output(fptr, &astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_1ST_CONTACT], &astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_1ST_CONTACT]);

		fprintf(fptr, "%%[BOND_HEAD_Z_2] %% SRCH_HT, %s\n", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_SRCH_HT].iFlagTuningIsFail]);
		mtn_wb_tune_save_one_tune_condition_output(fptr, &astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_SRCH_HT], &astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_SRCH_HT]);

		fprintf(fptr, "%%[BOND_HEAD_Z_3] %% LOOPING, %s\n", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_LOOPING].iFlagTuningIsFail]);
		mtn_wb_tune_save_one_tune_condition_output(fptr, &astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_LOOPING], &astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_LOOPING]);

		fprintf(fptr, "%%[BOND_HEAD_Z_4] %% LOOP_TOP, %s\n", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_LOOP_TOP].iFlagTuningIsFail]);
		mtn_wb_tune_save_one_tune_condition_output(fptr, &astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_LOOP_TOP], &astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_LOOP_TOP]);

		fprintf(fptr, "%%[BOND_HEAD_Z_5] %% TRAJECTORY, %s\n", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_TRAJECTORY].iFlagTuningIsFail]);
		mtn_wb_tune_save_one_tune_condition_output(fptr, &astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_TRAJECTORY], &astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_TRAJECTORY]);

		fprintf(fptr, "%%[BOND_HEAD_Z_6] %% TAIL, %s\n", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_TAIL].iFlagTuningIsFail]);
		mtn_wb_tune_save_one_tune_condition_output(fptr, &astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_TAIL], &astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_TAIL]);

		fprintf(fptr, "%%[BOND_HEAD_Z_7] %% RESET, %s\n", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_RESET].iFlagTuningIsFail]);
		mtn_wb_tune_save_one_tune_condition_output(fptr, &astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_RESET], &astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_RESET]);

		fprintf(fptr, "%%[BOND_HEAD_Z_8] %% 2ND_CONTACT, %s\n", astrPassOrFail[astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_2ND_CONTACT].iFlagTuningIsFail]);
		mtn_wb_tune_save_one_tune_condition_output(fptr, &astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_2ND_CONTACT], &astMtnTuneOutput[WB_AXIS_BOND_Z][WB_BH_2ND_CONTACT]);

		// Table-XY Single Sector
		fprintf(fptr, "%%[TABLE_X] Single Sector\n");
			mtn_wb_tune_save_one_tune_condition_output(fptr, &astMtnTuneInput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_BTO], 
				&astMtnTuneOutput[WB_AXIS_TABLE_X][WB_TBL_MOTION_IDX_MOVE_BTO]);

		fprintf(fptr, "%%[TABLE_Y] Single Sector\n");
			mtn_wb_tune_save_one_tune_condition_output(fptr, &astMtnTuneInput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_BTO], 
				&astMtnTuneOutput[WB_AXIS_TABLE_Y][WB_TBL_MOTION_IDX_MOVE_BTO]);

		// Table-XY Multiple Sector
		unsigned int ii, uiMaxSector = DEF_NUM_SECTOR_POSN_RANGE_TBL_X; // mtn_wb_tune_get_max_sector_table_x();
		for(ii =0; ii< uiMaxSector; ii++)
		{
			fprintf(fptr, "%%[TABLE_X_U%d]\n", ii );
			mtn_wb_tune_save_one_tune_condition_output(fptr, &astMtnTuneSectorInputTableX[ii][WB_TBL_MOTION_IDX_MOVE_BTO], 
				&astMtnTuneSectorOutputTableX[ii][WB_TBL_MOTION_IDX_MOVE_BTO]);
		}
		 uiMaxSector = mtn_wb_tune_get_max_sector_table_y();
		for(ii =0; ii< uiMaxSector; ii++)
		{
			fprintf(fptr, "%%[TABLE_Y_U%d]\n", ii );
			mtn_wb_tune_save_one_tune_condition_output(fptr, &astMtnTuneSectorInputTableY[ii][WB_TBL_MOTION_IDX_MOVE_BTO], 
				&astMtnTuneSectorOutputTableY[ii][WB_TBL_MOTION_IDX_MOVE_BTO]);
		}

		fclose(fptr);
	}
	else
	{
		iRet = MTN_API_ERR_FILE_PTR;
	}

	return iRet;
}

extern MTN_TUNE_CASE astMotionTuneHistory[MAX_CASE_TUNING_HISTORY_BUFFER];

void mtn_tune_save_tuning_history(MTN_TUNE_GENETIC_INPUT *stpMtnTuneInput, MTN_TUNE_GENETIC_OUTPUT *stpMtnTuneOutput, int nTotalCases)
{
struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;
char strFilenameWbPerformIndex[256];
FILE *fptr;
char strAppName[32];
TUNE_ALGO_SETTING       *stpTuningAlgoSetting = &stpMtnTuneInput->stTuningAlgoSetting;
MTN_TUNE_PARA_UNION		*stpTuningParameterIni = &stpMtnTuneInput->stTuningParameterIni;
MTN_TUNE_PARA_UNION		*stpTuningParameterUppBound = &stpMtnTuneInput->stTuningParameterUppBound;
MTN_TUNE_PARA_UNION		*stpTuningParameterLowBound = &stpMtnTuneInput->stTuningParameterLowBound;
MTN_TUNE_THEME			*stpTuningTheme = &stpMtnTuneInput->stTuningTheme;
MTN_TUNE_CONDITION		*stpTuningCondition = &stpMtnTuneInput->stTuningCondition;
MTN_TUNE_ENC_TIME_CFG	*stpTuningEncTimeConfig = &stpMtnTuneInput->stTuningEncTimeConfig;
MTN_TUNE_SPECIFICATION  *stpTuningPassSpecfication = &stpMtnTuneInput->stTuningPassSpecfication;
int iAxisTuning = stpMtnTuneInput->iAxisTuning;

	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);
	if(iAxisTuning == APP_X_TABLE_ACS_ID)
	{
		sprintf_s(strAppName, 32, "TblX");
	}
	else if(iAxisTuning == APP_Y_TABLE_ACS_ID)
	{
		sprintf_s(strAppName, 32, "TblY");
	}
	else if(iAxisTuning == sys_get_acs_axis_id_bnd_z())
	{
		sprintf_s(strAppName, 32, "BHZ");
	}
	else
	{
		sprintf_s(strAppName, 32, "Ax%d", iAxisTuning);
	}

	sprintf_s(strFilenameWbPerformIndex, 256, "%s\\Tune_%s_%d-%d-%d_%d-%d.m", 
		strWbTuningNewFolderName, strAppName, // 20120522
		stpTime->tm_year +1900, stpTime->tm_mon +1, 
				stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min);
	fopen_s(&fptr, strFilenameWbPerformIndex, "w");
	if(fptr != NULL)
	{
		// Tuning Condition
		fprintf(fptr, "iTuningAxis = %d;\n", iAxisTuning);
		fprintf(fptr, "stSpeedProfile_VAJ = [%6.1f, %6.1f, %6.1f, %6.1f]; %% Vel, Acc, Dec, Jerk\n", stpTuningCondition->stMtnSpeedProfile.dMaxVelocity, 
			stpTuningCondition->stMtnSpeedProfile.dMaxAcceleration, stpTuningCondition->stMtnSpeedProfile.dMaxDeceleration, stpTuningCondition->stMtnSpeedProfile.dMaxJerk);
		fprintf(fptr, "aMovePosn = [%6.1f, %6.1f];\n", stpTuningCondition->dMovePosition_1, stpTuningCondition->dMovePointion_2);
		fprintf(fptr, "aEnc_um_Sample_ms_SettTh_um = [%6.1f, %6.1f, %6.1f];\n", stpTuningEncTimeConfig->dEncCntUnit_um, stpTuningEncTimeConfig->dSampleTime_ms, stpTuningEncTimeConfig->dSettleThreshold_um);
		fprintf(fptr, "aLowerBoundPara = [%6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f];\n", stpTuningParameterLowBound->stMtnPara.dVelLoopKP, stpTuningParameterLowBound->stMtnPara.dVelLoopKI,
			stpTuningParameterLowBound->stMtnPara.dPosnLoopKP, stpTuningParameterLowBound->stMtnPara.dAccFFC, stpTuningParameterLowBound->stMtnPara.dSecOrdFilterFreq_Hz, stpTuningParameterLowBound->stMtnPara.dVelLoopLimitI);
		fprintf(fptr, "aUpperBoundPara = [%6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f]\n", stpTuningParameterUppBound->stMtnPara.dVelLoopKP, stpTuningParameterUppBound->stMtnPara.dVelLoopKI,
			stpTuningParameterUppBound->stMtnPara.dPosnLoopKP, stpTuningParameterUppBound->stMtnPara.dAccFFC, stpTuningParameterUppBound->stMtnPara.dSecOrdFilterFreq_Hz, stpTuningParameterUppBound->stMtnPara.dVelLoopLimitI);
//		fprintf(fptr, "aInitialPara = [%6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f]\n", stpTuningParameterIni->stMtnPara.dVelLoopKP, stpTuningParameterIni->stMtnPara.dVelLoopKI,
//			stpTuningParameterIni->stMtnPara.dPosnLoopKP, stpTuningParameterIni->stMtnPara.dAccFFC, stpTuningParameterIni->stMtnPara.dSecOrdFilterFreq_Hz, stpTuningParameterIni->stMtnPara.dVelLoopLimitI);
		fprintf(fptr, "%%%% VKP, VKI, PKP, AccFF, SOF, VLI\n");

		fprintf(fptr, "stTuneTheme = [%d, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f]\n", stpTuningTheme->iThemeType, 
			stpTuningTheme->fWeightCmdEndOverUnderShoot, stpTuningTheme->fWeightNegativeDPE, stpTuningTheme->fWeightPositiveDPE, stpTuningTheme->fWeightDiffDPE,
			stpTuningTheme->fWeightPeakDAC, stpTuningTheme->fWeightRMS_DAC, stpTuningTheme->fWeightSettlingTime);

		// Initial Response
		fprintf(fptr, "%%%% BackOUS, FrontOUS, BackDPE, FrontDPE, BackOS, BackUS, FrontOS, FrontUS, BackSettleTime, FrontSettleTime, PeakPcDAC, RMSPcDAC, RMS_StaticPE\n");
		fprintf(fptr, "aInitialPerformance = [%6.1f, %6.1f,  %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f];\n",
			stpMtnTuneOutput->stInitialResponse.fCmdEndBackOUS, stpMtnTuneOutput->stInitialResponse.fCmdEndFrontOUS,
			stpMtnTuneOutput->stInitialResponse.fMaxBackMoveDPE, stpMtnTuneOutput->stInitialResponse.fMaxFrontMoveDPE,
			stpMtnTuneOutput->stInitialResponse.fMaxBackOS, stpMtnTuneOutput->stInitialResponse.fMaxBackUS,
			stpMtnTuneOutput->stInitialResponse.fMaxFrontOS, stpMtnTuneOutput->stInitialResponse.fMaxFrontUS,
			stpMtnTuneOutput->stInitialResponse.fSettleTimeBackDir, stpMtnTuneOutput->stInitialResponse.fSettleTimeFrontDir,
			stpMtnTuneOutput->stInitialResponse.fPeakPercentDAC, stpMtnTuneOutput->stInitialResponse.fRMS_Over32767_DAC,
			stpMtnTuneOutput->stInitialResponse.fRMS_StaticPE);
		fprintf(fptr, "aInitPara = [%6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f]; \n", 
			stpMtnTuneInput->stTuningParameterIni.stMtnPara.dVelLoopKP,		stpMtnTuneInput->stTuningParameterIni.stMtnPara.dVelLoopKI,
			stpMtnTuneInput->stTuningParameterIni.stMtnPara.dPosnLoopKP,		stpMtnTuneInput->stTuningParameterIni.stMtnPara.dAccFFC,
			stpMtnTuneInput->stTuningParameterIni.stMtnPara.dSecOrdFilterFreq_Hz,		stpMtnTuneInput->stTuningParameterIni.stMtnPara.dVelLoopLimitI);
		fprintf(fptr, "[SLDRA, SLDRX] = [%6.1f, %6.1f]; \n", 
			stpMtnTuneInput->stTuningParameterIni.stMtnPara.dServoLoopDRA, stpMtnTuneInput->stTuningParameterIni.stMtnPara.dServoLoopDRX);
		fprintf(fptr, "fInitialObj = %6.1f\n\n", stpMtnTuneOutput->dInitialObj);

		// Tuning Output
		fprintf(fptr, "aOutputBestPerformance = [%6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f];\n",
			stpMtnTuneOutput->stResultResponse.fCmdEndBackOUS, stpMtnTuneOutput->stResultResponse.fCmdEndFrontOUS,
			stpMtnTuneOutput->stResultResponse.fMaxBackMoveDPE, stpMtnTuneOutput->stResultResponse.fMaxFrontMoveDPE,
			stpMtnTuneOutput->stResultResponse.fMaxBackOS, stpMtnTuneOutput->stResultResponse.fMaxBackUS,
			stpMtnTuneOutput->stResultResponse.fMaxFrontOS, stpMtnTuneOutput->stResultResponse.fMaxFrontUS,
			stpMtnTuneOutput->stResultResponse.fSettleTimeBackDir, stpMtnTuneOutput->stResultResponse.fSettleTimeFrontDir,
			stpMtnTuneOutput->stResultResponse.fPeakPercentDAC, stpMtnTuneOutput->stResultResponse.fRMS_Over32767_DAC,
			stpMtnTuneOutput->stResultResponse.fRMS_StaticPE);
		fprintf(fptr, "aTuneOutputPara = [%6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f]; \n", 
			stpMtnTuneOutput->stBestParameterSet.dVelLoopKP,		stpMtnTuneOutput->stBestParameterSet.dVelLoopKI,
			stpMtnTuneOutput->stBestParameterSet.dPosnLoopKP,		stpMtnTuneOutput->stBestParameterSet.dAccFFC,
			stpMtnTuneOutput->stBestParameterSet.dSecOrdFilterFreq_Hz,		stpMtnTuneOutput->stBestParameterSet.dVelLoopLimitI);
		fprintf(fptr, "[SLDRA, SLDRX] = [%6.1f, %6.1f]; \n", 
			stpMtnTuneOutput->stBestParameterSet.dServoLoopDRA, stpMtnTuneOutput->stBestParameterSet.dServoLoopDRX);
		fprintf(fptr, "fOutputBestObj = %6.1f\n", stpMtnTuneOutput->dTuningObj);
		
		if(stpMtnTuneInput->iDebugFlag >= 3)
		{
			// Tuning History
			for(int ii=0; ii<nTotalCases; ii++)
			{
				fprintf(fptr, "astTuneCase(%d).stTunePara = [%6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f]; astTuneCase(%d).stTuneResponseIndex = [%6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f, %6.1f]; astTuneCase(%d).fObj = %6.1f; \n",
					ii,		astMotionTuneHistory[ii].stServoPara.stMtnPara.dVelLoopKP, astMotionTuneHistory[ii].stServoPara.stMtnPara.dVelLoopKI, astMotionTuneHistory[ii].stServoPara.stMtnPara.dPosnLoopKP,
					astMotionTuneHistory[ii].stServoPara.stMtnPara.dAccFFC, astMotionTuneHistory[ii].stServoPara.stMtnPara.dSecOrdFilterFreq_Hz, astMotionTuneHistory[ii].stServoPara.stMtnPara.dVelLoopLimitI,
					ii,		astMotionTuneHistory[ii].stServoTuneIndex.fCmdEndBackOUS, astMotionTuneHistory[ii].stServoTuneIndex.fCmdEndFrontOUS,
					astMotionTuneHistory[ii].stServoTuneIndex.fMaxBackMoveDPE, astMotionTuneHistory[ii].stServoTuneIndex.fMaxFrontMoveDPE,
					astMotionTuneHistory[ii].stServoTuneIndex.fMaxBackOS, astMotionTuneHistory[ii].stServoTuneIndex.fMaxBackUS,
					astMotionTuneHistory[ii].stServoTuneIndex.fMaxFrontOS, astMotionTuneHistory[ii].stServoTuneIndex.fMaxFrontUS,
					astMotionTuneHistory[ii].stServoTuneIndex.fSettleTimeBackDir, astMotionTuneHistory[ii].stServoTuneIndex.fSettleTimeFrontDir,
					astMotionTuneHistory[ii].stServoTuneIndex.fPeakPercentDAC, astMotionTuneHistory[ii].stServoTuneIndex.fRMS_Over32767_DAC,
					ii,		astMotionTuneHistory[ii].dTuningObj);
			}
		}
		fclose(fptr);
	}
}

