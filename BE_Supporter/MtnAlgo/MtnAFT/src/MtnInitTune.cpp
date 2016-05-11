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
#include "MtnInitAcs.h"
#include "mtnconfg.h"
#include "MtnTune.h"

// from MtnWbTune.cpp
extern MTN_TUNE_GENETIC_INPUT stMtnTuneInputTableX_DRA;
extern MTN_TUNE_GENETIC_OUTPUT stMtnTuneOutputTableX_DRA;
extern MTN_TUNE_GENETIC_INPUT stMtnTuneInputTableY_DRA;
extern MTN_TUNE_GENETIC_OUTPUT stMtnTuneOutputTableY_DRA;
extern MTN_TUNE_GENETIC_INPUT astMtnTuneInput[MAX_SERVO_AXIS_WIREBOND][MAX_BLK_PARAMETER]; 
extern MTN_TUNE_GENETIC_INPUT astMtnTuneSectorInputTableX[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];
extern MTN_TUNE_GENETIC_INPUT astMtnTuneSectorInputTableY[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];

extern MTN_TUNE_GENETIC_OUTPUT astMtnTuneOutput[MAX_SERVO_AXIS_WIREBOND][MAX_BLK_PARAMETER];
extern MTN_TUNE_GENETIC_OUTPUT astMtnTuneSectorOutputTableX[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];
extern MTN_TUNE_GENETIC_OUTPUT astMtnTuneSectorOutputTableY[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];
extern unsigned int nNumSectorPosnRangeTableX;
extern unsigned int nNumSectorPosnRangeTableY;

// from MtnInitAcs.cpp
extern COMM_SETTINGS stServoControllerCommSet;
extern AXIS_INFO_WIRE_BOND astAxisInfoWireBond[MAX_SERVO_AXIS_WIREBOND];
extern SERVO_ACS stServoACS;

static char strDebugInitTuneMessage[512];

int mtnapi_init_tune_para_upp_low_bound_(char *strFilename, unsigned int *uiTotalBlk,
													  MTN_TUNE_PARA_UNION *stpServoParaUppBound_ACS, MTN_TUNE_PARA_UNION *stpServoParaLowBound_ACS);
int mtnapi_init_axis_tune_para_upp_low_bound(int iAxis, char *strFilename);

int mtnapi_init_tune_from_cfg_file(char *strFilename)
{
	int iRet = MTN_API_OK_ZERO;

	goto label_return_mtnapi_init_tune_from_cfg_file;


label_return_mtnapi_init_tune_from_cfg_file:
	return iRet;
}

int mtnapi_init_axis_tune_algo_spec_theme(int iAxis, char *strFilename)
{
	int iRet = MTN_API_OK_ZERO;
	goto label_return_mtnapi_init_axis_tune_algo_spec_theme;


label_return_mtnapi_init_axis_tune_algo_spec_theme:
	return iRet;
}

int mtnapi_init_tune_para_bound()
{
//	unsigned int ii;
	int iRet = MTN_API_OK_ZERO;

	iRet = mtnapi_init_axis_tune_para_upp_low_bound(WB_AXIS_TABLE_X, stServoACS.stpServoAxis_ACS[WB_AXIS_TABLE_X]->strAxisMtnServoTune);
//	if(iRet == MTN_API_OK_ZERO)
//	{
		iRet |= mtnapi_init_axis_tune_para_upp_low_bound(WB_AXIS_TABLE_Y, stServoACS.stpServoAxis_ACS[WB_AXIS_TABLE_Y]->strAxisMtnServoTune);
//	}
//	else
//	{
//		return iRet;
//	}
//	if(iRet == MTN_API_OK_ZERO)
//	{
		iRet |= mtnapi_init_axis_tune_para_upp_low_bound(WB_AXIS_BOND_Z, stServoACS.stpServoAxis_ACS[WB_AXIS_BOND_Z]->strAxisMtnServoTune);
//	}

	return iRet;
}

int mtnapi_init_axis_tune_para_upp_low_bound(int iAxis, char *strFilename)
{
	int iRet = MTN_API_OK_ZERO;

	MTN_TUNE_PARA_UNION stServoParaUppBound_ACS[MAX_BLK_PARAMETER];
	MTN_TUNE_PARA_UNION stServoParaLowBound_ACS[MAX_BLK_PARAMETER];
	unsigned int jj, ii, uiTotalBlk;

	iRet = mtnapi_init_tune_para_upp_low_bound_(strFilename, &uiTotalBlk, &stServoParaUppBound_ACS[0], &stServoParaLowBound_ACS[0]); // &stServoParaUppBound_ACS, &stServoParaLowBound_ACS);
	if(iRet == MTN_API_OK_ZERO)
	{
		for(ii = 0; ii<uiTotalBlk; ii++)
		{
			astMtnTuneInput[iAxis][ii].stTuningParameterUppBound = stServoParaUppBound_ACS[ii];
			astMtnTuneInput[iAxis][ii].stTuningParameterLowBound = stServoParaLowBound_ACS[ii];

			if(iAxis == WB_AXIS_TABLE_X)
			{
				for(jj = 0; jj<MAX_NUM_SECTOR_POSN_RANGE_TBL; jj++)
				{
					astMtnTuneSectorInputTableX[jj][ii].stTuningParameterUppBound = stServoParaUppBound_ACS[ii];
					astMtnTuneSectorInputTableX[jj][ii].stTuningParameterLowBound = stServoParaLowBound_ACS[ii];
				}
			}
			else if(iAxis == WB_AXIS_TABLE_Y)
			{
				for(jj = 0; jj<MAX_NUM_SECTOR_POSN_RANGE_TBL; jj++)
				{
					astMtnTuneSectorInputTableY[jj][ii].stTuningParameterUppBound = stServoParaUppBound_ACS[ii];
					astMtnTuneSectorInputTableY[jj][ii].stTuningParameterLowBound = stServoParaLowBound_ACS[ii];
				}
			}
			else if(iAxis == WB_AXIS_BOND_Z)
			{   // 20120802, AutoTuning B1W
				astMtnTuneInput[iAxis][ii].stTuningParameterUppBound.stMtnPara.dJerkFf = 1200;
				astMtnTuneInput[iAxis][ii].stTuningParameterLowBound.stMtnPara.dJerkFf = 300;
			}
		}
	}
	else
	{
		goto label_return_mtnapi_init_axis_tune_para_upp_low_bound;
	}


label_return_mtnapi_init_axis_tune_para_upp_low_bound:
	
	return iRet;
}

int mtnapi_init_tune_para_upp_low_bound_(char *strFilename, 
										 unsigned int *uiActualTotalBlk,
													  MTN_TUNE_PARA_UNION *stpServoParaUppBound_ACS, 
													  MTN_TUNE_PARA_UNION *stpServoParaLowBound_ACS)
{
	int iRet = MTN_API_OK_ZERO;
	char    buffer[BUFSIZ];
	unsigned int uTemp;
	int retval;
	unsigned int ii, uiTotalBlk;

	// 1. Initialize communication handler

	// 2. initialize from a master config file
    if(mtn_cfg_OpenConfigFile(strFilename) != OPENOK)
    {
        sprintf_s(strDebugInitTuneMessage, 512, "FATAL ERROR! [%s %d]: Unable to open Master Config file %s\n",
                        __FILE__, __LINE__, strFilename);
        iRet = MTN_API_ERROR_OPEN_FILE;
		return iRet;
    }

    if(mtn_cfg_ReadConfigString("TUNE_MASTER_CONFIG", "TOTAL_BLKS", &buffer[0]) == READOK)
    {
        retval = sscanf_s(buffer,"%d", &uTemp);
        if (retval != EOF && retval == 1)
        {
			uiTotalBlk = uTemp;
        }
    }
    else
    {
        sprintf_s(strDebugInitTuneMessage, 512, "FATAL ERROR! [%s %d]: Total parameter blocks not specified in %s\n",
			__FILE__, __LINE__, strFilename);
        iRet = MTN_API_ERROR_READ_FILE;
		goto label_mtnapi_init_tune_para_upp_low_bound_;

    }

	if(uiTotalBlk > MAX_BLK_PARAMETER)
	{
        iRet = MTN_API_ERROR_EXCEED_MAX_BLK;
		goto label_mtnapi_init_tune_para_upp_low_bound_;
	}
	else
	{
		*uiActualTotalBlk = uiTotalBlk;
	}

//	sprintf_s(&stServoACS.strFilename[0], MTN_API_MAX_STRLEN_FILENAME, "%s", strFilename);
//
	char strTempParameterBlkConfig[128];
	for(ii = 0; ii<uiTotalBlk; ii++)
	{
		sprintf_s(strTempParameterBlkConfig, 128, "PARAMETER_BOUND_BLK_%d", ii);
		// LOW_BOUND_VKP
		if(mtn_cfg_ReadConfigString(strTempParameterBlkConfig, "LOW_BOUND_VKP", &buffer[0]) == READOK)
		{
			retval = sscanf_s(buffer,"%d", &uTemp);
			if (retval != EOF && retval == 1)
			{
				stpServoParaLowBound_ACS[ii].stMtnPara.dVelLoopKP = (double)uTemp;
			}
		}
		else
		{
			sprintf_s(strDebugInitTuneMessage, 512, "FATAL ERROR! [%s %d]:  LOW_BOUND_VKP in %s\n",
				__FILE__, __LINE__, strFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto label_mtnapi_init_tune_para_upp_low_bound_;
		}

		// LOW_BOUND_VKI
		if(mtn_cfg_ReadConfigString(strTempParameterBlkConfig, "LOW_BOUND_VKI", &buffer[0]) == READOK)
		{
			retval = sscanf_s(buffer,"%d", &uTemp);
			if (retval != EOF && retval == 1)
			{
				stpServoParaLowBound_ACS[ii].stMtnPara.dVelLoopKI = (double)uTemp;
			}
		}
		else
		{
			sprintf_s(strDebugInitTuneMessage, 512, "FATAL ERROR! [%s %d]:  LOW_BOUND_VKI in %s\n",
				__FILE__, __LINE__, strFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto label_mtnapi_init_tune_para_upp_low_bound_;
		}

		// LOW_BOUND_PKP
		if(mtn_cfg_ReadConfigString(strTempParameterBlkConfig, "LOW_BOUND_PKP", &buffer[0]) == READOK)
		{
			retval = sscanf_s(buffer,"%d", &uTemp);
			if (retval != EOF && retval == 1)
			{
				stpServoParaLowBound_ACS[ii].stMtnPara.dPosnLoopKP = (double)uTemp;
			}
		}
		else
		{
			sprintf_s(strDebugInitTuneMessage, 512, "FATAL ERROR! [%s %d]:  LOW_BOUND_PKP in %s\n",
				__FILE__, __LINE__, strFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto label_mtnapi_init_tune_para_upp_low_bound_;
		}

		// LOW_BOUND_AFF
		if(mtn_cfg_ReadConfigString(strTempParameterBlkConfig, "LOW_BOUND_AFF", &buffer[0]) == READOK)
		{
			retval = sscanf_s(buffer,"%d", &uTemp);
			if (retval != EOF && retval == 1)
			{
				stpServoParaLowBound_ACS[ii].stMtnPara.dAccFFC = (double)uTemp;
			}
		}
		else
		{
			sprintf_s(strDebugInitTuneMessage, 512, "FATAL ERROR! [%s %d]:  LOW_BOUND_AFF in %s\n",
				__FILE__, __LINE__, strFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto label_mtnapi_init_tune_para_upp_low_bound_;
		}

		// LOW_BOUND_SOF
		if(mtn_cfg_ReadConfigString(strTempParameterBlkConfig, "LOW_BOUND_SOF", &buffer[0]) == READOK)
		{
			retval = sscanf_s(buffer,"%d", &uTemp);
			if (retval != EOF && retval == 1)
			{
				stpServoParaLowBound_ACS[ii].stMtnPara.dSecOrdFilterFreq_Hz = (double)uTemp;
			}
		}
		else
		{
			sprintf_s(strDebugInitTuneMessage, 512, "FATAL ERROR! [%s %d]:  LOW_BOUND_SOF in %s\n",
				__FILE__, __LINE__, strFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto label_mtnapi_init_tune_para_upp_low_bound_;
		}

		// LOW_BOUND_VLI
		if(mtn_cfg_ReadConfigString(strTempParameterBlkConfig, "LOW_BOUND_VLI", &buffer[0]) == READOK)
		{
			retval = sscanf_s(buffer,"%d", &uTemp);
			if (retval != EOF && retval == 1)
			{
				stpServoParaLowBound_ACS[ii].stMtnPara.dVelLoopLimitI = (double)uTemp;
			}
		}
		else
		{
			sprintf_s(strDebugInitTuneMessage, 512, "FATAL ERROR! [%s %d]:  LOW_BOUND_VLI in %s\n",
				__FILE__, __LINE__, strFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto label_mtnapi_init_tune_para_upp_low_bound_;
		}

		///////
		// UPP_BOUND_VKP
		if(mtn_cfg_ReadConfigString(strTempParameterBlkConfig, "UPP_BOUND_VKP", &buffer[0]) == READOK)
		{
			retval = sscanf_s(buffer,"%d", &uTemp);
			if (retval != EOF && retval == 1)
			{
				stpServoParaUppBound_ACS[ii].stMtnPara.dVelLoopKP = (double)uTemp;
			}
		}
		else
		{
			sprintf_s(strDebugInitTuneMessage, 512, "FATAL ERROR! [%s %d]:  UPP_BOUND_VKP in %s\n",
				__FILE__, __LINE__, strFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto label_mtnapi_init_tune_para_upp_low_bound_;
		}

		// UPP_BOUND_VKI
		if(mtn_cfg_ReadConfigString(strTempParameterBlkConfig, "UPP_BOUND_VKI", &buffer[0]) == READOK)
		{
			retval = sscanf_s(buffer,"%d", &uTemp);
			if (retval != EOF && retval == 1)
			{
				stpServoParaUppBound_ACS[ii].stMtnPara.dVelLoopKI = (double)uTemp;
			}
		}
		else
		{
			sprintf_s(strDebugInitTuneMessage, 512, "FATAL ERROR! [%s %d]:  UPP_BOUND_VKI in %s\n",
				__FILE__, __LINE__, strFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto label_mtnapi_init_tune_para_upp_low_bound_;
		}

		// UPP_BOUND_PKP
		if(mtn_cfg_ReadConfigString(strTempParameterBlkConfig, "UPP_BOUND_PKP", &buffer[0]) == READOK)
		{
			retval = sscanf_s(buffer,"%d", &uTemp);
			if (retval != EOF && retval == 1)
			{
				stpServoParaUppBound_ACS[ii].stMtnPara.dPosnLoopKP = (double)uTemp;
			}
		}
		else
		{
			sprintf_s(strDebugInitTuneMessage, 512, "FATAL ERROR! [%s %d]:  UPP_BOUND_PKP in %s\n",
				__FILE__, __LINE__, strFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto label_mtnapi_init_tune_para_upp_low_bound_;
		}

		// UPP_BOUND_AFF
		if(mtn_cfg_ReadConfigString(strTempParameterBlkConfig, "UPP_BOUND_AFF", &buffer[0]) == READOK)
		{
			retval = sscanf_s(buffer,"%d", &uTemp);
			if (retval != EOF && retval == 1)
			{
				stpServoParaUppBound_ACS[ii].stMtnPara.dAccFFC = (double)uTemp;
			}
		}
		else
		{
			sprintf_s(strDebugInitTuneMessage, 512, "FATAL ERROR! [%s %d]:  UPP_BOUND_AFF in %s\n",
				__FILE__, __LINE__, strFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto label_mtnapi_init_tune_para_upp_low_bound_;
		}

		// UPP_BOUND_SOF
		if(mtn_cfg_ReadConfigString(strTempParameterBlkConfig, "UPP_BOUND_SOF", &buffer[0]) == READOK)
		{
			retval = sscanf_s(buffer,"%d", &uTemp);
			if (retval != EOF && retval == 1)
			{
				stpServoParaUppBound_ACS[ii].stMtnPara.dSecOrdFilterFreq_Hz = (double)uTemp;
			}
		}
		else
		{
			sprintf_s(strDebugInitTuneMessage, 512, "FATAL ERROR! [%s %d]:  UPP_BOUND_SOF in %s\n",
				__FILE__, __LINE__, strFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto label_mtnapi_init_tune_para_upp_low_bound_;
		}

		// UPP_BOUND_VLI
		if(mtn_cfg_ReadConfigString(strTempParameterBlkConfig, "UPP_BOUND_VLI", &buffer[0]) == READOK)
		{
			retval = sscanf_s(buffer,"%d", &uTemp);
			if (retval != EOF && retval == 1)
			{
				stpServoParaUppBound_ACS[ii].stMtnPara.dVelLoopLimitI = (double)uTemp;
			}
		}
		else
		{
			sprintf_s(strDebugInitTuneMessage, 512, "FATAL ERROR! [%s %d]:  UPP_BOUND_VLI in %s\n",
				__FILE__, __LINE__, strFilename);
			iRet = MTN_API_ERROR_READ_FILE;
			goto label_mtnapi_init_tune_para_upp_low_bound_;
		}
	}

label_mtnapi_init_tune_para_upp_low_bound_:
	mtn_cfg_CloseConfigFile();
	return iRet;
}
//#undef MOTALGO_DLL_EXPORTS
#include "MotAlgo_DLL.h"
#define DEF_SECTOR_TBL_X 2
static unsigned int uiCurrWireNo=0; //, uiBlkMovePosnSet=0;
void mtn_tune_init_servo_para_tuning_config()
{
	unsigned int ii, jj;
	for(ii = 0; ii< MAX_BLK_PARAMETER; ii++)
	{
		// 20101002, BH makes difference in idle and motion
		mtn_wb_tune_init_def_servo_para_tune_input(&astMtnTuneInput[WB_AXIS_BOND_Z][ii], WB_AXIS_BOND_Z, uiCurrWireNo, ii); 

		if(get_sys_machine_type_flag() == WB_MACH_TYPE_VLED_FORK)
		{
			mtn_wb_tune_init_def_servo_para_tune_input(&astMtnTuneInput[WB_AXIS_TABLE_X][ii], WB_AXIS_TABLE_X, DEF_SECTOR_TBL_X, ii);  // 20110515
		}
		else
		{
			mtn_wb_tune_init_def_servo_para_tune_input(&astMtnTuneInput[WB_AXIS_TABLE_X][ii], WB_AXIS_TABLE_X, uiCurrWireNo, ii);  // 20110515
		}

		mtn_wb_tune_init_def_servo_para_tune_input(&astMtnTuneInput[WB_AXIS_TABLE_Y][ii], WB_AXIS_TABLE_Y, uiCurrWireNo, ii); 
		mtn_wb_tune_init_def_servo_para_tune_input(&astMtnTuneInput[WB_AXIS_WIRE_CLAMP][ii], WB_AXIS_WIRE_CLAMP, uiCurrWireNo, ii); 
	}
	mtn_wb_tune_init_def_servo_para_tune_input(&stMtnTuneInputTableX_DRA, WB_AXIS_TABLE_X, uiCurrWireNo, ii); 
	mtn_wb_tune_init_def_servo_para_tune_input(&stMtnTuneInputTableY_DRA, WB_AXIS_TABLE_Y, uiCurrWireNo, ii); 

	// Initialize SectorBased Table Tuning Input
	for(ii = 0; ii< MAX_BLK_PARAMETER; ii++)
	{
		for(jj =0; jj< nNumSectorPosnRangeTableX; jj++)
		{
			mtn_wb_tune_init_def_servo_para_tune_input_no_condi(&astMtnTuneSectorInputTableX[jj][ii], WB_AXIS_TABLE_X);
			mtn_wb_tune_update_tuning_condition(&(astMtnTuneSectorInputTableX[jj][ii].stTuningCondition), 
				&(astMtnTuneSectorInputTableX[jj][ii].stTuningEncTimeConfig), 
				WB_AXIS_TABLE_X, jj, ii);
		}
		for(jj =0; jj< nNumSectorPosnRangeTableY; jj++)
		{
			mtn_wb_tune_init_def_servo_para_tune_input_no_condi(&astMtnTuneSectorInputTableY[jj][ii], WB_AXIS_TABLE_Y);
			mtn_wb_tune_update_tuning_condition(&(astMtnTuneSectorInputTableY[jj][ii].stTuningCondition), 
				&(astMtnTuneSectorInputTableY[jj][ii].stTuningEncTimeConfig), 
				WB_AXIS_TABLE_Y, jj, ii);
		}
	}
	mtn_wb_tune_init_def_servo_para_tune_input_no_condi(&stMtnTuneInputTableX_DRA, WB_AXIS_TABLE_X);
	mtn_wb_tune_update_tuning_condition(&stMtnTuneInputTableX_DRA.stTuningCondition, 
				&(stMtnTuneInputTableX_DRA.stTuningEncTimeConfig), 
				WB_AXIS_TABLE_X, DEF_SECTOR_TBL_X, 0);  // 20110515

	mtn_wb_tune_init_def_servo_para_tune_input_no_condi(&stMtnTuneInputTableY_DRA, WB_AXIS_TABLE_Y);
	mtn_wb_tune_update_tuning_condition(&stMtnTuneInputTableY_DRA.stTuningCondition, 
				&(stMtnTuneInputTableY_DRA.stTuningEncTimeConfig), 
				WB_AXIS_TABLE_Y, 0, 0);

}

void mtn_tune_set_initial_tuning_para()
{
	unsigned int ii, jj;
	for(ii = 0; ii< MAX_BLK_PARAMETER; ii++)
	{
		mtn_wb_tune_load_servo_para_to_tune_output(&astMtnTuneOutput[WB_AXIS_TABLE_X][ii], WB_AXIS_TABLE_X, uiCurrWireNo, ii); 
		mtn_wb_tune_load_servo_para_to_tune_output(&astMtnTuneOutput[WB_AXIS_TABLE_Y][ii], WB_AXIS_TABLE_Y, uiCurrWireNo, ii); 
		mtn_wb_tune_load_servo_para_to_tune_output(&astMtnTuneOutput[WB_AXIS_BOND_Z][ii], WB_AXIS_BOND_Z, uiCurrWireNo, ii); 

		mtn_wb_tune_load_servo_para_to_tune_ini(&astMtnTuneInput[WB_AXIS_TABLE_X][ii], WB_AXIS_TABLE_X, uiCurrWireNo, ii); 
		mtn_wb_tune_load_servo_para_to_tune_ini(&astMtnTuneInput[WB_AXIS_TABLE_Y][ii], WB_AXIS_TABLE_Y, uiCurrWireNo, ii); 
		mtn_wb_tune_load_servo_para_to_tune_ini(&astMtnTuneInput[WB_AXIS_BOND_Z][ii], WB_AXIS_BOND_Z, uiCurrWireNo, ii); 
	}

	// Initialize SectorBased Table Tuning Input
	for(ii = 0; ii< MAX_BLK_PARAMETER; ii++)
	{
		for(jj =0; jj< nNumSectorPosnRangeTableX; jj++)
		{
			mtn_wb_tune_load_servo_para_to_tune_output(&astMtnTuneSectorOutputTableX[jj][ii], WB_AXIS_TABLE_X, jj, ii); 
			mtn_wb_tune_load_servo_para_to_tune_ini(&astMtnTuneSectorInputTableX[jj][ii], WB_AXIS_TABLE_X, jj, ii); 

			mtn_wb_tune_load_servo_para_to_tune_output(&astMtnTuneSectorOutputTableY[jj][ii], WB_AXIS_TABLE_Y, jj, ii); 
			mtn_wb_tune_load_servo_para_to_tune_ini(&astMtnTuneSectorInputTableY[jj][ii], WB_AXIS_TABLE_Y, jj, ii); 
		}
	}
	//
	mtn_wb_tune_load_servo_para_to_tune_output(&stMtnTuneOutputTableX_DRA, WB_AXIS_TABLE_X, uiCurrWireNo, ii); 
	mtn_wb_tune_load_servo_para_to_tune_ini(&stMtnTuneInputTableX_DRA, WB_AXIS_TABLE_X, uiCurrWireNo, ii); 

	mtn_wb_tune_load_servo_para_to_tune_output(&stMtnTuneOutputTableY_DRA, WB_AXIS_TABLE_Y, uiCurrWireNo, ii); 
	mtn_wb_tune_load_servo_para_to_tune_ini(&stMtnTuneInputTableY_DRA, WB_AXIS_TABLE_Y, uiCurrWireNo, ii); 

}

void mtn_wb_tune_init_def_servo_para_tune_input_no_condi(MTN_TUNE_GENETIC_INPUT *stpMtnServoParaTuneInput, int iWbAxisOpt)
{
	stpMtnServoParaTuneInput->stTuningTheme.fPenaltyCmdEndOverUnderShoot = 5.0;   // 7.0;   Thomas  //  30;
	stpMtnServoParaTuneInput->stTuningTheme.fWeightCmdEndOverUnderShoot = 5.0;    // 7.0;   Thomas,  // 3;
	stpMtnServoParaTuneInput->stTuningTheme.fWeightDiffDPE = 1.0;
	stpMtnServoParaTuneInput->stTuningTheme.fWeightNegativeDPE = 1.0;
	stpMtnServoParaTuneInput->stTuningTheme.fWeightPositiveDPE = 1.0;
	stpMtnServoParaTuneInput->stTuningTheme.fWeightPeakDAC = 5.0;
	stpMtnServoParaTuneInput->stTuningTheme.fWeightRMS_DAC = 10;
	stpMtnServoParaTuneInput->stTuningTheme.fWeightSettlingTime = 20.0;
	stpMtnServoParaTuneInput->stTuningTheme.fWeightRMS_StaticPE = 50.0; // 20121017

	stpMtnServoParaTuneInput->stTuningPassSpecfication.dCmdEndOverUnderShoot_um = 5;
	stpMtnServoParaTuneInput->stTuningPassSpecfication.dMaxDPE_um = 50;
	stpMtnServoParaTuneInput->stTuningPassSpecfication.dOverUnderShoot_um = 50;
	stpMtnServoParaTuneInput->stTuningPassSpecfication.dPeakDAC_pc = 90;
	stpMtnServoParaTuneInput->stTuningPassSpecfication.dRMS_DAC_pc = 25;
	stpMtnServoParaTuneInput->stTuningPassSpecfication.dRptError = 3;
	stpMtnServoParaTuneInput->stTuningPassSpecfication.dSettleTime_ms = 10;

	stpMtnServoParaTuneInput->stTuningAlgoSetting.dCrossOverPercent = 0.2;
	stpMtnServoParaTuneInput->stTuningAlgoSetting.dMutionPercent = 0.2;
	stpMtnServoParaTuneInput->stTuningAlgoSetting.dStopingStd = 0.1;
	stpMtnServoParaTuneInput->stTuningAlgoSetting.uiMaxGeneration = 20;
	stpMtnServoParaTuneInput->stTuningAlgoSetting.uiPopulationSize = 20;
//	stpMtnServoParaTuneInput->stTuningCondition.iIterMotionDelay_ms = 10;

	stpMtnServoParaTuneInput->stCommHandle = stServoControllerCommSet.Handle;
	stpMtnServoParaTuneInput->stTuningEncTimeConfig.dEncCntUnit_um = 1000.0 / astAxisInfoWireBond[iWbAxisOpt].afEncoderResolution_cnt_p_mm; //[iWbAxisOpt];
	stpMtnServoParaTuneInput->stTuningEncTimeConfig.dSampleTime_ms = 1.0;

	stpMtnServoParaTuneInput->stTuningCondition.iIterMotionDelay_ms = 200;

	stpMtnServoParaTuneInput->iAxisTuning = astAxisInfoWireBond[iWbAxisOpt].iAxisInCtrlCardACS;

//	mtnapi_get_speed_profile(stpMtnServoParaTuneInput->stCommHandle, stpMtnServoParaTuneInput->iAxisTuning, 
//			&stpMtnServoParaTuneInput->stTuningCondition.stMtnSpeedProfile, NULL);

	stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dServoLoopDRA = 0;
	stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dServoLoopDRX = 0;
	stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dJerkFf = 0;

	stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dServoLoopDRA = 200;
	stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dServoLoopDRX = 100000;  // 10% of XVEL
	stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dJerkFf = 10000;

	switch (iWbAxisOpt)
	{
	case WB_AXIS_TABLE_X:
		stpMtnServoParaTuneInput->stTuningEncTimeConfig.dSettleThreshold_um = 3;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dSecOrdFilterFreq_Hz = 800;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dSecOrdFilterFreq_Hz = 1600;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKP		=	TBL_X_TUNINGPARAMETER_LOWBOUND_VELLOOP_KP;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKI	=	TBL_X_TUNINGPARAMETER_LOWBOUND_VELLOOPKI;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dPosnLoopKP	=	TBL_X_TUNINGPARAMETER_LOWBOUND_POSNLOOPKP;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dAccFFC		=	TBL_X_TUNINGPARAMETER_LOWBOUND_ACCFFC;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopLimitI	=	TBL_X_TUNINGPARAMETER_LOWBOUND_VELLOOPLIMITI;

		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKP		=	TBL_X_TUNINGPARAMETER_UPPBOUND_VELLOOP_KP;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKI	=	TBL_X_TUNINGPARAMETER_UPPBOUND_VELLOOPKI;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dPosnLoopKP	=	TBL_X_TUNINGPARAMETER_UPPBOUND_POSNLOOPKP;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dAccFFC		=	TBL_X_TUNINGPARAMETER_UPPBOUND_ACCFFC;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopLimitI	=	TBL_X_TUNINGPARAMETER_UPPBOUND_VELLOOPLIMITI;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dJerkFf = TBL_X_TUNINGPARAMETER_LOWBOUND_JERKFF_;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dJerkFf = TBL_X_TUNINGPARAMETER_UPPBOUND_JERKFF_;

		stpMtnServoParaTuneInput->stTuningTheme.iThemeType = THEME_MIN_CEOUS_DPE_W_DAC;

		break;
	case WB_AXIS_TABLE_Y:
		stpMtnServoParaTuneInput->stTuningEncTimeConfig.dSettleThreshold_um = 3;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dSecOrdFilterFreq_Hz = 300;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dSecOrdFilterFreq_Hz = 1200;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKP			=	TBL_Y_TUNINGPARAMETER_LOWBOUND_VELLOOP_KP	;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKI		=	TBL_Y_TUNINGPARAMETER_LOWBOUND_VELLOOPKI	;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dPosnLoopKP		=	TBL_Y_TUNINGPARAMETER_LOWBOUND_POSNLOOPKP	;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dAccFFC			=	TBL_Y_TUNINGPARAMETER_LOWBOUND_ACCFFC	;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopLimitI	=	TBL_Y_TUNINGPARAMETER_LOWBOUND_VELLOOPLIMITI	;

		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKP			=	TBL_Y_TUNINGPARAMETER_UPPBOUND_VELLOOP_KP	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKI		=	TBL_Y_TUNINGPARAMETER_UPPBOUND_VELLOOPKI	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dPosnLoopKP		=	TBL_Y_TUNINGPARAMETER_UPPBOUND_POSNLOOPKP	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dAccFFC			=	TBL_Y_TUNINGPARAMETER_UPPBOUND_ACCFFC	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopLimitI	=	TBL_Y_TUNINGPARAMETER_UPPBOUND_VELLOOPLIMITI	;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dJerkFf	=	TBL_Y_TUNINGPARAMETER_LOWBOUND_JERKFF_;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dJerkFf	=	TBL_Y_TUNINGPARAMETER_UPPBOUND_JERKFF_;

		stpMtnServoParaTuneInput->stTuningTheme.iThemeType = THEME_MIN_CEOUS_DPE_W_DAC;
		break;
	case WB_AXIS_BOND_Z:
		stpMtnServoParaTuneInput->stTuningEncTimeConfig.dSettleThreshold_um = 5;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_LOWBOUND_ACCFFC	;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_LOWBOUND_POSNLOOPKP	;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPKI	;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPLIMITI	;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOP_KP	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_UPPBOUND_ACCFFC	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_UPPBOUND_POSNLOOPKP	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPKI	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOP_KP	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPLIMITI;

		stpMtnServoParaTuneInput->stTuningTheme.iThemeType = THEME_MIN_CEOUS_BH_W_DAC; // THEME_MIN_CEOUS_W_DAC;
		break;
	default :
		break;
	}
}

void mtn_tune_init_def_condi_algo(MTN_TUNE_GENETIC_INPUT *stpMtnServoParaTuneInput)
{
	stpMtnServoParaTuneInput->stTuningTheme.fPenaltyCmdEndOverUnderShoot = 30;
	stpMtnServoParaTuneInput->stTuningTheme.fWeightCmdEndOverUnderShoot = 3.0;
	stpMtnServoParaTuneInput->stTuningTheme.fWeightDiffDPE = 1.0;
	stpMtnServoParaTuneInput->stTuningTheme.fWeightNegativeDPE = 1.0;
	stpMtnServoParaTuneInput->stTuningTheme.fWeightPositiveDPE = 1.0;
	stpMtnServoParaTuneInput->stTuningTheme.fWeightPeakDAC = 1.0;
	stpMtnServoParaTuneInput->stTuningTheme.fWeightRMS_DAC = 10;
	stpMtnServoParaTuneInput->stTuningTheme.fWeightSettlingTime = 1.0;
	stpMtnServoParaTuneInput->stTuningTheme.fWeightRMS_StaticPE = 10.0;

	stpMtnServoParaTuneInput->stTuningPassSpecfication.dCmdEndOverUnderShoot_um = 5;
	stpMtnServoParaTuneInput->stTuningPassSpecfication.dMaxDPE_um = 50;
	stpMtnServoParaTuneInput->stTuningPassSpecfication.dOverUnderShoot_um = 50;
	stpMtnServoParaTuneInput->stTuningPassSpecfication.dPeakDAC_pc = 90;
	stpMtnServoParaTuneInput->stTuningPassSpecfication.dRMS_DAC_pc = 25;
	stpMtnServoParaTuneInput->stTuningPassSpecfication.dRptError = 3;
	stpMtnServoParaTuneInput->stTuningPassSpecfication.dSettleTime_ms = 10;

	stpMtnServoParaTuneInput->stTuningAlgoSetting.dCrossOverPercent = 0.2;
	stpMtnServoParaTuneInput->stTuningAlgoSetting.dMutionPercent = 0.2;
	stpMtnServoParaTuneInput->stTuningAlgoSetting.dStopingStd = 0.1;
	stpMtnServoParaTuneInput->stTuningAlgoSetting.uiMaxGeneration = 20;
	stpMtnServoParaTuneInput->stTuningAlgoSetting.uiPopulationSize = 20;
//	stpMtnServoParaTuneInput->stTuningCondition.iIterMotionDelay_ms = 10;

	stpMtnServoParaTuneInput->stCommHandle = stServoControllerCommSet.Handle;
	stpMtnServoParaTuneInput->stTuningEncTimeConfig.dSampleTime_ms = 1.0;

	stpMtnServoParaTuneInput->stTuningCondition.iIterMotionDelay_ms = 200;

}

void mtn_wb_tune_init_def_servo_para_tune_input_bh_idle(MTN_TUNE_GENETIC_INPUT *stpMtnServoParaTuneInput, int iWbAxisOpt)
{

	mtn_tune_init_def_condi_algo(stpMtnServoParaTuneInput);
	stpMtnServoParaTuneInput->iAxisTuning = astAxisInfoWireBond[iWbAxisOpt].iAxisInCtrlCardACS;
	stpMtnServoParaTuneInput->stTuningEncTimeConfig.dEncCntUnit_um = 1000.0 / astAxisInfoWireBond[iWbAxisOpt].afEncoderResolution_cnt_p_mm; //[iWbAxisOpt];

	switch (iWbAxisOpt)
	{
	case WB_AXIS_BOND_Z:
		stpMtnServoParaTuneInput->stTuningEncTimeConfig.dSettleThreshold_um = 3;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOP_KP	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPLIMITI;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOP_KP_BH_IDLE	;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPKI_BH_IDLE	;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_LOWBOUND_POSNLOOPKP_BH_IDLE	;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_LOWBOUND_ACCFFC_BH_IDLE	;

		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOP_KP_BH_IDLE	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPKI_BH_IDLE	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_UPPBOUND_POSNLOOPKP_BH_IDLE	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_UPPBOUND_ACCFFC_BH_IDLE	;

		stpMtnServoParaTuneInput->stTuningTheme.iThemeType = THEME_MIN_CEOUS_W_DAC; // THEME_WB_B1W; // 
		break;
	default :
		break;
	}
}
void mtn_wb_tune_init_def_servo_para_tune_input_bh_1st_contact(MTN_TUNE_GENETIC_INPUT *stpMtnServoParaTuneInput, int iWbAxisOpt)
{

	mtn_tune_init_def_condi_algo(stpMtnServoParaTuneInput);
	stpMtnServoParaTuneInput->iAxisTuning = astAxisInfoWireBond[iWbAxisOpt].iAxisInCtrlCardACS;

	switch (iWbAxisOpt)
	{
	case WB_AXIS_BOND_Z:
		stpMtnServoParaTuneInput->stTuningEncTimeConfig.dSettleThreshold_um = 3;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPLIMITI	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPLIMITI;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOP_KP_BH_1ST_CONTACT;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPKI_BH_1ST_CONTACT;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_LOWBOUND_POSNLOOPKP_BH_1ST_CONTACT;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_LOWBOUND_ACCFFC_BH_1ST_CONTACT;

		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOP_KP_BH_1ST_CONTACT	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPKI_BH_1ST_CONTACT	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_UPPBOUND_POSNLOOPKP_BH_1ST_CONTACT	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_UPPBOUND_ACCFFC_BH_1ST_CONTACT	;

		stpMtnServoParaTuneInput->stTuningTheme.iThemeType = THEME_WB_B1W; // THEME_MIN_CEOUS_W_DAC;
		break;
	default :
		break;
	}
}

void mtn_wb_tune_init_def_servo_para_tune_input_bh_srch_ht(MTN_TUNE_GENETIC_INPUT *stpMtnServoParaTuneInput, int iWbAxisOpt)
{

	mtn_tune_init_def_condi_algo(stpMtnServoParaTuneInput);
	stpMtnServoParaTuneInput->iAxisTuning = astAxisInfoWireBond[iWbAxisOpt].iAxisInCtrlCardACS;

	switch (iWbAxisOpt)
	{
	case WB_AXIS_BOND_Z:
		stpMtnServoParaTuneInput->stTuningEncTimeConfig.dSettleThreshold_um = 3;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPLIMITI	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPLIMITI;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOP_KP_BH_SRCH_HT;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPKI_BH_SRCH_HT;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_LOWBOUND_POSNLOOPKP_BH_SRCH_HT;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_LOWBOUND_ACCFFC_BH_SRCH_HT;

		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOP_KP_BH_SRCH_HT	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPKI_BH_SRCH_HT	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_UPPBOUND_POSNLOOPKP_BH_SRCH_HT	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_UPPBOUND_ACCFFC_BH_SRCH_HT	;

		stpMtnServoParaTuneInput->stTuningTheme.iThemeType = THEME_WB_B1W; // THEME_MIN_CEOUS_W_DAC;
		break;
	default :
		break;
	}
}
void mtn_wb_tune_init_def_servo_para_tune_input_bh_looping(MTN_TUNE_GENETIC_INPUT *stpMtnServoParaTuneInput, int iWbAxisOpt)
{

	mtn_tune_init_def_condi_algo(stpMtnServoParaTuneInput);
	stpMtnServoParaTuneInput->iAxisTuning = astAxisInfoWireBond[iWbAxisOpt].iAxisInCtrlCardACS;

	switch (iWbAxisOpt)
	{
	case WB_AXIS_BOND_Z:
		stpMtnServoParaTuneInput->stTuningEncTimeConfig.dSettleThreshold_um = 3;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPLIMITI	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPLIMITI;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOP_KP_BH_LOOPING;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPKI_BH_LOOPING;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_LOWBOUND_POSNLOOPKP_BH_LOOPING;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_LOWBOUND_ACCFFC_BH_LOOPING;

		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOP_KP_BH_LOOPING	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPKI_BH_LOOPING	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_UPPBOUND_POSNLOOPKP_BH_LOOPING	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_UPPBOUND_ACCFFC_BH_LOOPING	;

		stpMtnServoParaTuneInput->stTuningTheme.iThemeType = THEME_WB_B1W; // THEME_MIN_CEOUS_W_DAC;
		break;
	default :
		break;
	}
}

void mtn_wb_tune_init_def_servo_para_tune_input_bh_loop_top(MTN_TUNE_GENETIC_INPUT *stpMtnServoParaTuneInput, int iWbAxisOpt)
{

	mtn_tune_init_def_condi_algo(stpMtnServoParaTuneInput);
	stpMtnServoParaTuneInput->iAxisTuning = astAxisInfoWireBond[iWbAxisOpt].iAxisInCtrlCardACS;

	switch (iWbAxisOpt)
	{
	case WB_AXIS_BOND_Z:
		stpMtnServoParaTuneInput->stTuningEncTimeConfig.dSettleThreshold_um = 3;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPLIMITI	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPLIMITI;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOP_KP_BH_LOOP_TOP;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPKI_BH_LOOP_TOP;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_LOWBOUND_POSNLOOPKP_BH_LOOP_TOP;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_LOWBOUND_ACCFFC_BH_LOOP_TOP;

		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOP_KP_BH_LOOP_TOP	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPKI_BH_LOOP_TOP	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_UPPBOUND_POSNLOOPKP_BH_LOOP_TOP	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_UPPBOUND_ACCFFC_BH_LOOP_TOP	;

		stpMtnServoParaTuneInput->stTuningTheme.iThemeType = THEME_WB_B1W; // THEME_MIN_CEOUS_W_DAC;
		break;
	default :
		break;
	}
}
void mtn_wb_tune_init_def_servo_para_tune_input_bh_trajectory(MTN_TUNE_GENETIC_INPUT *stpMtnServoParaTuneInput, int iWbAxisOpt)
{

	mtn_tune_init_def_condi_algo(stpMtnServoParaTuneInput);
	stpMtnServoParaTuneInput->iAxisTuning = astAxisInfoWireBond[iWbAxisOpt].iAxisInCtrlCardACS;

	switch (iWbAxisOpt)
	{
	case WB_AXIS_BOND_Z:
		stpMtnServoParaTuneInput->stTuningEncTimeConfig.dSettleThreshold_um = 3;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPLIMITI	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPLIMITI;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOP_KP_BH_TRAJECTORY;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPKI_BH_TRAJECTORY;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_LOWBOUND_POSNLOOPKP_BH_TRAJECTORY;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_LOWBOUND_ACCFFC_BH_TRAJECTORY;

		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOP_KP_BH_TRAJECTORY	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPKI_BH_TRAJECTORY	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_UPPBOUND_POSNLOOPKP_BH_TRAJECTORY	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_UPPBOUND_ACCFFC_BH_TRAJECTORY	;

		stpMtnServoParaTuneInput->stTuningTheme.iThemeType = THEME_WB_B1W; // THEME_MIN_CEOUS_W_DAC;
		break;
	default :
		break;
	}
}
void mtn_wb_tune_init_def_servo_para_tune_input_bh_tail(MTN_TUNE_GENETIC_INPUT *stpMtnServoParaTuneInput, int iWbAxisOpt)
{

	mtn_tune_init_def_condi_algo(stpMtnServoParaTuneInput);
	stpMtnServoParaTuneInput->iAxisTuning = astAxisInfoWireBond[iWbAxisOpt].iAxisInCtrlCardACS;

	switch (iWbAxisOpt)
	{
	case WB_AXIS_BOND_Z:
		stpMtnServoParaTuneInput->stTuningEncTimeConfig.dSettleThreshold_um = 3;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPLIMITI	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPLIMITI;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOP_KP_BH_TAIL;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPKI_BH_TAIL;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_LOWBOUND_POSNLOOPKP_BH_TAIL;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_LOWBOUND_ACCFFC_BH_TAIL;

		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOP_KP_BH_TAIL	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPKI_BH_TAIL	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_UPPBOUND_POSNLOOPKP_BH_TAIL	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_UPPBOUND_ACCFFC_BH_TAIL	;

		stpMtnServoParaTuneInput->stTuningTheme.iThemeType = THEME_WB_B1W; // THEME_MIN_CEOUS_W_DAC;
		break;
	default :
		break;
	}
}
void mtn_wb_tune_init_def_servo_para_tune_input_bh_reset(MTN_TUNE_GENETIC_INPUT *stpMtnServoParaTuneInput, int iWbAxisOpt)
{

	mtn_tune_init_def_condi_algo(stpMtnServoParaTuneInput);
	stpMtnServoParaTuneInput->iAxisTuning = astAxisInfoWireBond[iWbAxisOpt].iAxisInCtrlCardACS;

	switch (iWbAxisOpt)
	{
	case WB_AXIS_BOND_Z:
		stpMtnServoParaTuneInput->stTuningEncTimeConfig.dSettleThreshold_um = 3;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPLIMITI	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPLIMITI;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOP_KP_BH_RESET;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPKI_BH_RESET;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_LOWBOUND_POSNLOOPKP_BH_RESET;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_LOWBOUND_ACCFFC_BH_RESET;

		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOP_KP_BH_RESET	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPKI_BH_RESET	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_UPPBOUND_POSNLOOPKP_BH_RESET	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_UPPBOUND_ACCFFC_BH_RESET	;

		stpMtnServoParaTuneInput->stTuningTheme.iThemeType = THEME_WB_B1W; // THEME_MIN_CEOUS_W_DAC;
		break;
	default :
		break;
	}
}

void mtn_wb_tune_init_def_servo_para_tune_input_bh_2nd_contact(MTN_TUNE_GENETIC_INPUT *stpMtnServoParaTuneInput, int iWbAxisOpt)
{

	mtn_tune_init_def_condi_algo(stpMtnServoParaTuneInput);
	stpMtnServoParaTuneInput->iAxisTuning = astAxisInfoWireBond[iWbAxisOpt].iAxisInCtrlCardACS;

	switch (iWbAxisOpt)
	{
	case WB_AXIS_BOND_Z:
		stpMtnServoParaTuneInput->stTuningEncTimeConfig.dSettleThreshold_um = 3;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dSecOrdFilterFreq_Hz = 0;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPLIMITI	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopLimitI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPLIMITI;

		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOP_KP_BH_2ND_CONTACT;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_LOWBOUND_VELLOOPKI_BH_2ND_CONTACT;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_LOWBOUND_POSNLOOPKP_BH_2ND_CONTACT;
		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_LOWBOUND_ACCFFC_BH_2ND_CONTACT;

		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKP		=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOP_KP_BH_2ND_CONTACT	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dVelLoopKI	=	BH_TUNINGPARAMETER_UPPBOUND_VELLOOPKI_BH_2ND_CONTACT	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dPosnLoopKP	=	BH_TUNINGPARAMETER_UPPBOUND_POSNLOOPKP_BH_2ND_CONTACT	;
		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dAccFFC		=	BH_TUNINGPARAMETER_UPPBOUND_ACCFFC_BH_2ND_CONTACT	;

		stpMtnServoParaTuneInput->stTuningTheme.iThemeType = THEME_WB_B1W; // THEME_MIN_CEOUS_W_DAC;
		break;
	default :
		break;
	}
}

void mtn_wb_tune_init_def_servo_para_tune_input(MTN_TUNE_GENETIC_INPUT *stpMtnServoParaTuneInput, int iWbAxisOpt, unsigned int uiCurrWireNo, unsigned int uiBlkMovePosnSet)
{
	// 20101002, BH makes difference in idle and motion
	if( iWbAxisOpt == WB_AXIS_BOND_Z)
	{
		switch (uiBlkMovePosnSet)
		{
		case WB_BH_IDLE:
			mtn_wb_tune_init_def_servo_para_tune_input_bh_idle(stpMtnServoParaTuneInput, iWbAxisOpt);
			break;
		case WB_BH_1ST_CONTACT:
			mtn_wb_tune_init_def_servo_para_tune_input_bh_1st_contact(stpMtnServoParaTuneInput, iWbAxisOpt);
			break;
		case WB_BH_SRCH_HT:
			mtn_wb_tune_init_def_servo_para_tune_input_bh_srch_ht(stpMtnServoParaTuneInput, iWbAxisOpt);
			break;
		case WB_BH_LOOPING:
			mtn_wb_tune_init_def_servo_para_tune_input_bh_looping(stpMtnServoParaTuneInput, iWbAxisOpt);
			break;
		case WB_BH_LOOP_TOP:
			mtn_wb_tune_init_def_servo_para_tune_input_bh_loop_top(stpMtnServoParaTuneInput, iWbAxisOpt);
			break;
		case WB_BH_TRAJECTORY:
			mtn_wb_tune_init_def_servo_para_tune_input_bh_trajectory(stpMtnServoParaTuneInput, iWbAxisOpt);
			break;
		case WB_BH_TAIL:
			mtn_wb_tune_init_def_servo_para_tune_input_bh_tail(stpMtnServoParaTuneInput, iWbAxisOpt);
			break;
		case WB_BH_RESET:
			mtn_wb_tune_init_def_servo_para_tune_input_bh_reset(stpMtnServoParaTuneInput, iWbAxisOpt);
			break;
		case WB_BH_2ND_CONTACT:
			mtn_wb_tune_init_def_servo_para_tune_input_bh_2nd_contact(stpMtnServoParaTuneInput, iWbAxisOpt);
			break;
		}
//		stpMtnServoParaTuneInput->stTuningParameterLowBound.stMtnPara.dJerkFf = 100;    // 20110524
//		stpMtnServoParaTuneInput->stTuningParameterUppBound.stMtnPara.dJerkFf = 20000;
	}
	else
	{
		mtn_wb_tune_init_def_servo_para_tune_input_no_condi(stpMtnServoParaTuneInput, iWbAxisOpt);// Init EncTime Config, must done before initializing tune condition
	}
	
	mtn_wb_tune_update_tuning_condition(&stpMtnServoParaTuneInput->stTuningCondition, &stpMtnServoParaTuneInput->stTuningEncTimeConfig, 
			iWbAxisOpt, uiCurrWireNo, uiBlkMovePosnSet);
}

#define SPEED_PROFILE_XY_SEGMENT_BL0_UPP_MM       3.0
#define SPEED_PROFILE_XY_SEGMENT_BL1_UPP_MM       10.0
#define SPEED_PROFILE_XY_SEGMENT_BL2_UPP_MM       20.0

void mtn_wb_tune_update_tuning_condition(MTN_TUNE_CONDITION *stpTuningCondition, MTN_TUNE_ENC_TIME_CFG *stpTuningEncTimeConfig, 
										 int iWbAxisOpt, unsigned int uiCurrWireNo, unsigned int uiBlkMovePosnSet)
{
double dDistTable_mm;
double dPosn1, dPosn2, dPosn3, dPosn4;
	if(iWbAxisOpt == WB_AXIS_BOND_Z)
	{
		switch(uiBlkMovePosnSet)
		{
		case WB_BH_IDLE:
		case WB_BH_1ST_CONTACT:
		case WB_BH_2ND_CONTACT: // = 8,
			break;
		case WB_BH_SRCH_HT: // = 2,
			stpTuningCondition->stMtnSpeedProfile = stServoAxis_ACS[WB_AXIS_BOND_Z].stSpeedProfile[0]; // WB-Blk-1
			mtn_tune_get_bond_head_tuning_position(uiCurrWireNo, IDX_BH_MOTION_1ST_BOND_SEARCH_HEIGHT, 
				&stpTuningCondition->dMovePosition_1, 
				&stpTuningCondition->dMovePointion_2);
			break;
		case WB_BH_LOOPING: // = 3,
			stpTuningCondition->stMtnSpeedProfile = stServoAxis_ACS[WB_AXIS_BOND_Z].stSpeedProfile[1]; // WB-Blk-2
			mtn_tune_get_bond_head_tuning_position(uiCurrWireNo, IDX_BH_MOTION_REVERSE_HEIGHT, &dPosn1, &dPosn2);
			mtn_tune_get_bond_head_tuning_position(uiCurrWireNo, IDX_BH_MOTION_KINK_HEIGHT, &dPosn3, &dPosn4);
			stpTuningCondition->dMovePosition_1 = dPosn1; 
			stpTuningCondition->dMovePointion_2 = dPosn4;
			break;
		case WB_BH_LOOP_TOP: // = 4,
			stpTuningCondition->stMtnSpeedProfile = stServoAxis_ACS[WB_AXIS_BOND_Z].stSpeedProfile[1]; // WB-Blk-2
			mtn_tune_get_bond_head_tuning_position(uiCurrWireNo, IDX_BH_MOTION_LOOP_TOP, 
				&stpTuningCondition->dMovePosition_1, 
				&stpTuningCondition->dMovePointion_2);
			break;
		case WB_BH_TRAJECTORY: // = 5,
			stpTuningCondition->stMtnSpeedProfile = stServoAxis_ACS[WB_AXIS_BOND_Z].stSpeedProfile[2]; // WB-Blk-3
			mtn_tune_get_bond_head_tuning_position(uiCurrWireNo, IDX_BH_MOTION_TRAJ_2ND_BOND_SEARCH_H, 
				&stpTuningCondition->dMovePosition_1, 
				&stpTuningCondition->dMovePointion_2);
			break;
		case WB_BH_TAIL: // = 6,
			stpTuningCondition->stMtnSpeedProfile = stServoAxis_ACS[WB_AXIS_BOND_Z].stSpeedProfile[3]; // WB-Blk-4
			mtn_tune_get_bond_head_tuning_position(uiCurrWireNo, IDX_BH_MOTION_TAIL, 
				&stpTuningCondition->dMovePosition_1, 
				&stpTuningCondition->dMovePointion_2);
			break;
		case WB_BH_RESET: // = 7,
			stpTuningCondition->stMtnSpeedProfile = stServoAxis_ACS[WB_AXIS_BOND_Z].stSpeedProfile[4]; // WB-Blk-5
			mtn_tune_get_bond_head_tuning_position(uiCurrWireNo, IDX_BH_MOTION_RESET_FIRE_LEVEL, 
				&stpTuningCondition->dMovePosition_1, 
				&stpTuningCondition->dMovePointion_2);
			break;
		default :
			break;
		}
	}

	else if(iWbAxisOpt == WB_AXIS_TABLE_X)
	{
		mtn_tune_get_table_x_tuning_position(uiCurrWireNo, uiBlkMovePosnSet,  
			&stpTuningCondition->dMovePosition_1, 
			&stpTuningCondition->dMovePointion_2);
		dDistTable_mm = stpTuningEncTimeConfig->dEncCntUnit_um / 1000 * fabs(stpTuningCondition->dMovePointion_2 - stpTuningCondition->dMovePosition_1);
		if(dDistTable_mm < SPEED_PROFILE_XY_SEGMENT_BL0_UPP_MM)
		{
			stpTuningCondition->stMtnSpeedProfile = stServoAxis_ACS[WB_AXIS_TABLE_X].stSpeedProfile[0];
		}
		else if(dDistTable_mm >= SPEED_PROFILE_XY_SEGMENT_BL0_UPP_MM 
			&& dDistTable_mm < SPEED_PROFILE_XY_SEGMENT_BL1_UPP_MM)
		{
			stpTuningCondition->stMtnSpeedProfile = stServoAxis_ACS[WB_AXIS_TABLE_X].stSpeedProfile[1];
		}
		else if(dDistTable_mm >= SPEED_PROFILE_XY_SEGMENT_BL1_UPP_MM
			&& dDistTable_mm < SPEED_PROFILE_XY_SEGMENT_BL2_UPP_MM)
		{
			stpTuningCondition->stMtnSpeedProfile = stServoAxis_ACS[WB_AXIS_TABLE_X].stSpeedProfile[2];
		}
		else // dDistTable_mm >= SPEED_PROFILE_XY_SEGMENT_BL2_UPP_MM
		{
			stpTuningCondition->stMtnSpeedProfile = stServoAxis_ACS[WB_AXIS_TABLE_X].stSpeedProfile[3];
		}

	}

	else if(iWbAxisOpt == WB_AXIS_TABLE_Y)
	{
		mtn_tune_get_table_y_tuning_position(uiCurrWireNo, uiBlkMovePosnSet, 
			&stpTuningCondition->dMovePosition_1, 
			&stpTuningCondition->dMovePointion_2);
		dDistTable_mm = stpTuningEncTimeConfig->dEncCntUnit_um / 1000 * fabs(stpTuningCondition->dMovePointion_2 - stpTuningCondition->dMovePosition_1);
		if(dDistTable_mm < SPEED_PROFILE_XY_SEGMENT_BL0_UPP_MM)
		{
			stpTuningCondition->stMtnSpeedProfile = stServoAxis_ACS[WB_AXIS_TABLE_Y].stSpeedProfile[0];
		}
		else if(dDistTable_mm >= SPEED_PROFILE_XY_SEGMENT_BL0_UPP_MM 
			&& dDistTable_mm < SPEED_PROFILE_XY_SEGMENT_BL1_UPP_MM)
		{
			stpTuningCondition->stMtnSpeedProfile = stServoAxis_ACS[WB_AXIS_TABLE_Y].stSpeedProfile[1];
		}
		else if(dDistTable_mm >= SPEED_PROFILE_XY_SEGMENT_BL1_UPP_MM 
			&& dDistTable_mm < SPEED_PROFILE_XY_SEGMENT_BL2_UPP_MM)
		{
			stpTuningCondition->stMtnSpeedProfile = stServoAxis_ACS[WB_AXIS_TABLE_Y].stSpeedProfile[2];
		}
		else // dDistTable_mm >= SPEED_PROFILE_XY_SEGMENT_BL2_UPP_MM
		{
			stpTuningCondition->stMtnSpeedProfile = stServoAxis_ACS[WB_AXIS_TABLE_Y].stSpeedProfile[3];
		}
	}
}


MTN_TUNE_2POINTS_POSNS astWbTuningBondHeadPositionSet[NUM_TOTAL_WIRE_IN_ONE_SCOPE][MAX_NUM_SET_POSITION_TUNE_WB_BH];
void mtn_tune_get_wb_bondhead_tuning_position_set(int idxWireNo, POSNS_OF_BONDHEAD_Z *stpPosnsOfBondHeadZ)
{
		astWbTuningBondHeadPositionSet[idxWireNo][IDX_BH_MOTION_1ST_BOND_SEARCH_HEIGHT].dPosition1 = stpPosnsOfBondHeadZ->fStartFireLevel_Z;
		astWbTuningBondHeadPositionSet[idxWireNo][IDX_BH_MOTION_1ST_BOND_SEARCH_HEIGHT].dPosition2 = stpPosnsOfBondHeadZ->f1stBondSearchHeight_Z ;

		astWbTuningBondHeadPositionSet[idxWireNo][IDX_BH_MOTION_REVERSE_HEIGHT].dPosition1 = stpPosnsOfBondHeadZ->f1stBondContactPosn_Z + 100 ;
		astWbTuningBondHeadPositionSet[idxWireNo][IDX_BH_MOTION_REVERSE_HEIGHT].dPosition2 = stpPosnsOfBondHeadZ->fReverseHeightPosn_Z ;

		astWbTuningBondHeadPositionSet[idxWireNo][IDX_BH_MOTION_KINK_HEIGHT].dPosition1 = stpPosnsOfBondHeadZ->fReverseHeightPosn_Z ;
		astWbTuningBondHeadPositionSet[idxWireNo][IDX_BH_MOTION_KINK_HEIGHT].dPosition2 = stpPosnsOfBondHeadZ->fKinkHeightPosn_Z ;

		astWbTuningBondHeadPositionSet[idxWireNo][IDX_BH_MOTION_LOOP_TOP].dPosition1 = stpPosnsOfBondHeadZ->fKinkHeightPosn_Z ;
		astWbTuningBondHeadPositionSet[idxWireNo][IDX_BH_MOTION_LOOP_TOP].dPosition2 = stpPosnsOfBondHeadZ->fLoopTopPosn_Z ;

		astWbTuningBondHeadPositionSet[idxWireNo][IDX_BH_MOTION_TRAJ_2ND_BOND_SEARCH_H].dPosition1 = stpPosnsOfBondHeadZ->fLoopTopPosn_Z ;
		astWbTuningBondHeadPositionSet[idxWireNo][IDX_BH_MOTION_TRAJ_2ND_BOND_SEARCH_H].dPosition2 = stpPosnsOfBondHeadZ->f2ndBondSearchHeightPosn_Z ;

		astWbTuningBondHeadPositionSet[idxWireNo][IDX_BH_MOTION_TAIL].dPosition1 = stpPosnsOfBondHeadZ->f2ndBondContactPosn_Z + 100 ;
		astWbTuningBondHeadPositionSet[idxWireNo][IDX_BH_MOTION_TAIL].dPosition2 = stpPosnsOfBondHeadZ->fTailPosn_Z ;

		astWbTuningBondHeadPositionSet[idxWireNo][IDX_BH_MOTION_RESET_FIRE_LEVEL].dPosition1 = stpPosnsOfBondHeadZ->fTailPosn_Z ;
		astWbTuningBondHeadPositionSet[idxWireNo][IDX_BH_MOTION_RESET_FIRE_LEVEL].dPosition2 = stpPosnsOfBondHeadZ->fEndFireLevel_Z ;
}

void mtn_tune_init_wb_bondhead_tuning_position_set_vled()
{
	for(int ii=0; ii<NUM_TOTAL_WIRE_IN_ONE_SCOPE; ii++)
	{
		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_1ST_BOND_SEARCH_HEIGHT].dPosition1 = 106;
		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_1ST_BOND_SEARCH_HEIGHT].dPosition2 = 5000;

		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_REVERSE_HEIGHT].dPosition1 =  -94;
		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_REVERSE_HEIGHT].dPosition2 = 436.7;

		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_KINK_HEIGHT].dPosition1 = 436.7;
		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_KINK_HEIGHT].dPosition2 = 528.7;

		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_LOOP_TOP].dPosition1 = 528.7;
		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_LOOP_TOP].dPosition2 = 1504.7;

		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_TRAJ_2ND_BOND_SEARCH_H].dPosition1 = 722.6;
		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_TRAJ_2ND_BOND_SEARCH_H].dPosition2 = 1504.7;

		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_TAIL].dPosition1 = 412.6;
		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_TAIL].dPosition2 = 859.7;

		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_RESET_FIRE_LEVEL].dPosition1 = 859.7;
		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_RESET_FIRE_LEVEL].dPosition2 = 5000.0;
	}
}

void mtn_tune_init_wb_bondhead_tuning_position_set_hori_led()
{
	for(int ii=0; ii<NUM_TOTAL_WIRE_IN_ONE_SCOPE; ii++)
	{
		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_1ST_BOND_SEARCH_HEIGHT].dPosition1 = 106 - 5000;
		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_1ST_BOND_SEARCH_HEIGHT].dPosition2 = 0;

		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_REVERSE_HEIGHT].dPosition1 =  -94 - 5000;
		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_REVERSE_HEIGHT].dPosition2 = 436  - 5000;

		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_KINK_HEIGHT].dPosition1 = 436  - 5000;
		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_KINK_HEIGHT].dPosition2 = 528  - 5000;

		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_LOOP_TOP].dPosition1 = 528   - 5000;
		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_LOOP_TOP].dPosition2 = 1504  - 5000;

		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_TRAJ_2ND_BOND_SEARCH_H].dPosition1 = 722  - 5000;
		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_TRAJ_2ND_BOND_SEARCH_H].dPosition2 = 1504 - 5000;

		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_TAIL].dPosition1 = 412  - 5000;
		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_TAIL].dPosition2 = 859  - 5000;

		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_RESET_FIRE_LEVEL].dPosition1 = 859  - 5000;
		astWbTuningBondHeadPositionSet[ii][IDX_BH_MOTION_RESET_FIRE_LEVEL].dPosition2 = 5000 - 5000;
	}
}
// Set the initial position of tuning points, make difference between Vertical-LED and Horizontal-LED, 20110630
void mtn_tune_init_wb_bondhead_tuning_position_set()
{
int	iFlagMachineType = get_sys_machine_type_flag();

	if(iFlagMachineType == WB_MACH_TYPE_HORI_LED)
	{
		mtn_tune_init_wb_bondhead_tuning_position_set_hori_led();
	}
	else
	{
		mtn_tune_init_wb_bondhead_tuning_position_set_vled();
	}
}

MTN_TUNE_2POINTS_POSNS astWbTuningTableXPositionSet[DEF_NUM_SECTOR_POSN_RANGE_TBL_X][MAX_NUM_SET_POSITION_TUNE_WB_TABLE]; // 20111006, Bug fix
MTN_TUNE_2POINTS_POSNS astWbTuningTableYPositionSet[DEF_NUM_SECTOR_POSN_RANGE_TBL_X][MAX_NUM_SET_POSITION_TUNE_WB_TABLE];
void mtn_tune_get_wb_table_tuning_position_set(int idxSectorTable, POSNS_OF_TABLE *stpPosnsOfTable)
{
	astWbTuningTableXPositionSet[idxSectorTable][WB_TBL_MOTION_IDX_MOVE_BTO	].dPosition1	=		stpPosnsOfTable->fStartPosition_X;
	astWbTuningTableXPositionSet[idxSectorTable][WB_TBL_MOTION_IDX_MOVE_BTO	].dPosition2	=		stpPosnsOfTable->f1stBondPosition_X;
	astWbTuningTableXPositionSet[idxSectorTable][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	].dPosition1	=		stpPosnsOfTable->f1stBondPosition_X;
	astWbTuningTableXPositionSet[idxSectorTable][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	].dPosition2	=		stpPosnsOfTable->f1stBondReversePosition_X;
	astWbTuningTableXPositionSet[idxSectorTable][WB_TBL_MOTION_IDX_MOVE_KINK_1	].dPosition1	=		stpPosnsOfTable->f1stBondReversePosition_X;
	astWbTuningTableXPositionSet[idxSectorTable][WB_TBL_MOTION_IDX_MOVE_KINK_1	].dPosition2	=		stpPosnsOfTable->f1stBondReversePosition_X;
	astWbTuningTableXPositionSet[idxSectorTable][WB_TBL_MOTION_IDX_MOVE_TRAJ	].dPosition1	=		stpPosnsOfTable->f1stBondReversePosition_X;
	astWbTuningTableXPositionSet[idxSectorTable][WB_TBL_MOTION_IDX_MOVE_TRAJ	].dPosition2	=		stpPosnsOfTable->fTrajEnd2ndBondPosn_X;

	astWbTuningTableYPositionSet[idxSectorTable][WB_TBL_MOTION_IDX_MOVE_BTO	].dPosition1	=		stpPosnsOfTable->fStartPosition_Y;
	astWbTuningTableYPositionSet[idxSectorTable][WB_TBL_MOTION_IDX_MOVE_BTO	].dPosition2	=		stpPosnsOfTable->f1stBondPosition_Y;
	astWbTuningTableYPositionSet[idxSectorTable][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	].dPosition1	=		stpPosnsOfTable->f1stBondPosition_Y;
	astWbTuningTableYPositionSet[idxSectorTable][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	].dPosition2	=		stpPosnsOfTable->f1stBondReversePosition_Y;
	astWbTuningTableYPositionSet[idxSectorTable][WB_TBL_MOTION_IDX_MOVE_KINK_1	].dPosition1	=		stpPosnsOfTable->f1stBondReversePosition_Y;
	astWbTuningTableYPositionSet[idxSectorTable][WB_TBL_MOTION_IDX_MOVE_KINK_1	].dPosition2	=		stpPosnsOfTable->f1stBondReversePosition_Y;
	astWbTuningTableYPositionSet[idxSectorTable][WB_TBL_MOTION_IDX_MOVE_TRAJ	].dPosition1	=		stpPosnsOfTable->f1stBondReversePosition_Y;
	astWbTuningTableYPositionSet[idxSectorTable][WB_TBL_MOTION_IDX_MOVE_TRAJ	].dPosition2	=		stpPosnsOfTable->fTrajEnd2ndBondPosn_Y;

}

// There may be difference between NUM_TOTAL_WIRE_IN_ONE_SCOPE and MAX_NUM_SECTOR_POSN_RANGE_TBL
// The initial tuning position is set by sector
 // 20121017, Special for KoreaPCB-SunLED
void mtn_tune_init_wb_table_x_tuning_position_set()
{
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_0U_1U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	0	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_0U_N1U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	0	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_1U_2U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	30000	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_N1U_N2U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	-40000	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_2U_3U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	80000	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_N2U_N3U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	-80000	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_3U_4U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	120000	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_N3U_N4U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	-120000	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_4U_5U]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	150000	;
	astWbTuningTableXPositionSet	[	WB_TBL_POSITION_RANGE_N4U_N5U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	-150000	;

	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_0U_1U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	15000;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_0U_N1U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	-15000;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_1U_2U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	45000;  // 20121017
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_N1U_N2U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	-55000;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_2U_3U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	95000;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_N2U_N3U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	-95000;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_3U_4U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	135000;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_N3U_N4U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	-135000;
	astWbTuningTableXPositionSet	[	WB_TBL_POSITION_RANGE_4U_5U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	165000	;
	astWbTuningTableXPositionSet	[	WB_TBL_POSITION_RANGE_N4U_N5U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	-165000	;

	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_0U_1U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition1	=	15000	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_0U_N1U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition1	=	-15000	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_1U_2U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition1	=	45000	; // 20121017
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_N1U_N2U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition1	=	-55000	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_2U_3U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition1	=	95000	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_N2U_N3U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition1	=	-95000	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_3U_4U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition1	=	135000	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_N3U_N4U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition1	=	-135000	;
	astWbTuningTableXPositionSet	[	WB_TBL_POSITION_RANGE_4U_5U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition1	=	165000	;
	astWbTuningTableXPositionSet	[	WB_TBL_POSITION_RANGE_N4U_N5U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition1	=	-165000	;
											
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_0U_1U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition2	=	13800	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_0U_N1U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition2	=	-13800	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_1U_2U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition2	=	43800	; // 20121017
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_N1U_N2U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition2	=	-53800	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_2U_3U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition2	=	93800	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_N2U_N3U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition2	=	-93800	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_3U_4U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition2	=	133800	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_N3U_N4U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition2	=	-133800	;
	astWbTuningTableXPositionSet	[	WB_TBL_POSITION_RANGE_4U_5U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	163800	;
	astWbTuningTableXPositionSet	[	WB_TBL_POSITION_RANGE_N4U_N5U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	-163800	;

	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_0U_1U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition1	=	13800	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_0U_N1U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition1	=	-13800	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_1U_2U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition1	=	43800	; // 20121017
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_N1U_N2U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition1	=	-53800	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_2U_3U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition1	=	93800	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_N2U_N3U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition1	=	-93800	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_3U_4U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition1	=	133800	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_N3U_N4U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition1	=	-133800	;
	astWbTuningTableXPositionSet	[	WB_TBL_POSITION_RANGE_4U_5U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition1	=	163800	;
	astWbTuningTableXPositionSet	[	WB_TBL_POSITION_RANGE_N4U_N5U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition1	=	-163800	;
											
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_0U_1U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition2	=	17200	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_0U_N1U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition2	=	-17200	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_1U_2U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition2	=	47200	; // 20121017
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_N1U_N2U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition2	=	-57200	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_2U_3U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition2	=	97200	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_N2U_N3U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition2	=	-97200	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_3U_4U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition2	=	137200	;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_N3U_N4U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition2	=	-137200	;
	astWbTuningTableXPositionSet	[	WB_TBL_POSITION_RANGE_4U_5U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition2	=	167200	;
	astWbTuningTableXPositionSet	[	WB_TBL_POSITION_RANGE_N4U_N5U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition2	=	-167200	;

}

// Change default tuning condition to larger distance
// Version 1: 85um, 60um, 100um
// Version 2: 600um, 575um, 1125um
void mtn_tune_init_wb_table_y_tuning_vled_position_set()
{
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_0U_1U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	20000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_0U_N1U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	-20000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_1U_2U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	40000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N1U_N2U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	-40000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_2U_3U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	80000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N2U_N3U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	-80000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_3U_4U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	120000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N3U_N4U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	-120000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_4U_5U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	160000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N4U_N5U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	-160000	;
											
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_0U_1U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	18800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_0U_N1U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	-18800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_1U_2U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	38800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N1U_N2U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	-38800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_2U_3U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	78800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N2U_N3U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	-78800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_3U_4U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	118800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N3U_N4U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	-118800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_4U_5U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	158800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N4U_N5U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	-158800	;
											
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_0U_1U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition1	=	18800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_0U_N1U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition1	=	-18800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_1U_2U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition1	=	38800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N1U_N2U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition1	=	-38800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_2U_3U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition1	=	78800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N2U_N3U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition1	=	-78800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_3U_4U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition1	=	118800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N3U_N4U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition1	=	-118800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_4U_5U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition1	=	158800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N4U_N5U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition1	=	-158800	;
											
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_0U_1U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	19950	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_0U_N1U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	-19950	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_1U_2U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	39950	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N1U_N2U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	-39950	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_2U_3U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	79950	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N2U_N3U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	-79950	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_3U_4U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	119950	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N3U_N4U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	-119950	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_4U_5U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	159950	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N4U_N5U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	-159950	;
											
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_0U_1U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition1	=	19950	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_0U_N1U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition1	=	-19950	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_1U_2U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition1	=	39950	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N1U_N2U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition1	=	-39950	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_2U_3U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition1	=	79950	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N2U_N3U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition1	=	-79950	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_3U_4U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition1	=	119950	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N3U_N4U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition1	=	-119950	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_4U_5U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition1	=	159950	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N4U_N5U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition1	=	-159950	;
											
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_0U_1U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition2	=	22200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_0U_N1U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition2	=	-22200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_1U_2U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition2	=	42200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N1U_N2U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition2	=	-42200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_2U_3U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition2	=	82200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N2U_N3U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition2	=	-82200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_3U_4U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition2	=	122200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N3U_N4U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition2	=	-122200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_4U_5U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition2	=	162200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N4U_N5U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition2	=	-162200	;

}

//
//void mtn_tune_init_wb_val_y_tuning_position_set_hori_bonder()
//{
//
////	WB_TBL_MOTION_IDX_MOVE_TRAJ
//}

void mtn_tune_init_wb_table_y_tuning_hori_led_position_set()
{
	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_0U_1U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	0	;
	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_0U_N1U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	0	;
	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_1U_2U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	40000	;
	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_N1U_N2U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	-40000	;

	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_0U_1U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	15000;
	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_0U_N1U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	-15000;
	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_1U_2U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	55000;
	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_N1U_N2U	][WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	-55000;

	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_0U_1U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition1	=	15000	;
	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_0U_N1U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition1	=	-15000	;
	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_1U_2U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition1	=	55000	;
	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_N1U_N2U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST].dPosition1	=	-55000	;

	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_0U_1U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	10000;
	astWbTuningTableXPositionSet[WB_TBL_POSITION_RANGE_0U_N1U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	-10000;
	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_1U_2U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	50000;
	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_N1U_N2U	][WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	-50000;

	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_0U_1U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition1	=	15000	;
	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_0U_N1U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition1	=	-15000	;
	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_1U_2U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition1	=	55000	;
	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_N1U_N2U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition1	=	-55000	;


	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_0U_1U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition2	=	17200	;
	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_0U_N1U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition2	=	-17200	;
	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_1U_2U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition2	=	57200	;
	astWbTuningTableYPositionSet[WB_TBL_POSITION_RANGE_N1U_N2U	][WB_TBL_MOTION_IDX_MOVE_TRAJ].dPosition2	=	-57200	;

	/// Later Blocks
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_2U_3U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	80000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N2U_N3U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	-80000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_3U_4U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	120000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N3U_N4U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	-120000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_4U_5U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	160000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N4U_N5U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition1	=	-160000	;
											
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_2U_3U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	65000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N2U_N3U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	-65000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_3U_4U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	105000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N3U_N4U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	-105000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_4U_5U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	145000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N4U_N5U	]	[	WB_TBL_MOTION_IDX_MOVE_BTO	]	.dPosition2	=	-145000	;
											
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_2U_3U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition1	=	65000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N2U_N3U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition1	=	-65000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_3U_4U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition1	=	105000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N3U_N4U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition1	=	-105000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_4U_5U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition1	=	145000	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N4U_N5U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition1	=	-145000	;
											
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_2U_3U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	66200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N2U_N3U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	-66200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_3U_4U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	106200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N3U_N4U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	-106200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_4U_5U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	146200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N4U_N5U	]	[	WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST	]	.dPosition2	=	-146200	;
											
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_2U_3U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition1	=	66200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N2U_N3U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition1	=	-66200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_3U_4U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition1	=	106200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N3U_N4U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition1	=	-106200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_4U_5U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition1	=	146200	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N4U_N5U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition1	=	-146200	;
											
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_2U_3U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition2	=	63800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N2U_N3U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition2	=	-63800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_3U_4U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition2	=	103800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N3U_N4U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition2	=	-103800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_4U_5U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition2	=	143800	;
	astWbTuningTableYPositionSet	[	WB_TBL_POSITION_RANGE_N4U_N5U	]	[	WB_TBL_MOTION_IDX_MOVE_TRAJ	]	.dPosition2	=	-143800	;

}

void mtn_tune_get_bond_head_tuning_position(int idxWireNo, unsigned int uiBlk,  double *dPosn1, double *dPosn2)
{
	if(uiBlk < MAX_NUM_SET_POSITION_TUNE_WB_BH)
	{
		*dPosn1 = astWbTuningBondHeadPositionSet[idxWireNo][uiBlk].dPosition1;
		*dPosn2 = astWbTuningBondHeadPositionSet[idxWireNo][uiBlk].dPosition2;
	}
	else
	{
		*dPosn1 = 0;
		*dPosn2 = 0;
	}
}

void mtn_tune_get_table_x_tuning_position(int idxWireNo, unsigned int uiBlk,  double *dPosn1, double *dPosn2)
{
	if(uiBlk < MAX_NUM_SET_POSITION_TUNE_WB_TABLE)
	{
		*dPosn1 = astWbTuningTableXPositionSet[idxWireNo][uiBlk].dPosition1;
		*dPosn2 = astWbTuningTableXPositionSet[idxWireNo][uiBlk].dPosition2;
	}
	else
	{
		*dPosn1 = 0;
		*dPosn2 = 0;
	}
}
void mtn_tune_get_table_y_tuning_position(int idxWireNo, unsigned int uiBlk,  double *dPosn1, double *dPosn2)
{
	if(uiBlk < MAX_NUM_SET_POSITION_TUNE_WB_TABLE)
	{
		*dPosn1 = astWbTuningTableYPositionSet[idxWireNo][uiBlk].dPosition1;
		*dPosn2 = astWbTuningTableYPositionSet[idxWireNo][uiBlk].dPosition2;
	}
	else
	{
		*dPosn1 = 0;
		*dPosn2 = 0;
	}
}
