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
#include "aftprbs.h"

#include "MtnWbDef.h"
extern AXIS_INFO_WIRE_BOND astAxisInfoWireBond[MAX_SERVO_AXIS_WIREBOND];

extern char *astrMachineTypeNameLabel_en[];


char cFlagEnableDebug;
void aft_spectrum_enable_debug()
{
	cFlagEnableDebug = 1;
}

void aft_spectrum_disable_debug()
{
	cFlagEnableDebug = 0;
}

int iFlagSpectrumSysAnaFlag = SYSTEM_ANALYSIS_PSEUDO_WHITE_NOISE;  // Default is WhiteNoiseSpectrum
int aft_get_spectrum_sys_analysis_flag()
{
	return iFlagSpectrumSysAnaFlag;
}
void aft_set_spectrum_sys_analysis_flag(int iFlagSysAnalysis)
{
	iFlagSpectrumSysAnaFlag = iFlagSysAnalysis;
}

#include <direct.h>
char strSpectrumNewFolderName[128];

#include "MotAlgo_DLL.h"
#include "MtnWbDef.h"

#include "acs_buff_prog.h"

void mtnapi_set_bond_z_acs_axis(HANDLE stCommHandleACS)
{
	int iACS_AxisZ = sys_get_acs_axis_id_bnd_z();

	acsc_WriteInteger(stCommHandleACS, BUFFER_ID_VEL_TEST_PROF_MOVE_2, "iBndZ_Axis", 0, 0, 0, 0, &iACS_AxisZ, NULL );

}

int aft_spectrum_save_servo_parameter_into_spectrum_folder(HANDLE hAcsHandle)
{
	int iRet = MTN_API_ERROR;

	int nNumLineBuff0 = 0;
	if(acs_buffprog_upload_prog_get_line(0) == MTN_API_OK_ZERO)
	{
		nNumLineBuff0 = acs_buffprog_get_num_lines_at_buff(0);
	}

	if(hAcsHandle != ACSC_INVALID)
	{
//		mtnapi_dll_init_master_struct_ptr();
//		if(nNumLineBuff0 >= 1)
//		{ // upload WB - servo & Speed parameter
			mtnapi_dll_save_wb_speed_servo_parameter_acs(hAcsHandle);
//		}
//		else  // load from file, may be wrong, // 20120725
//		{
//			_mtnapi_dll_init_servo_speed_para_acs(hAcsHandle);
//		}
char strCommandCopyFileToFolder[1024];

		if(_chdir("C:\\WbData\\ParaBase\\") == FALSE) // 0: OK, folder exist
		{			
			sprintf_s(strCommandCopyFileToFolder, "%s\\ctrl_acsc_x.ini", strSpectrumNewFolderName);
			CopyFile("C:\\WbData\\ParaBase\\ctrl_acsc_x.ini", strCommandCopyFileToFolder, 0);
			sprintf_s(strCommandCopyFileToFolder, "%s\\ctrl_acsc_y.ini", strSpectrumNewFolderName);
			CopyFile("C:\\WbData\\ParaBase\\ctrl_acsc_y.ini", strCommandCopyFileToFolder, 0);
			sprintf_s(strCommandCopyFileToFolder, "%s\\ctrl_acsc_z.ini", strSpectrumNewFolderName);
			CopyFile("C:\\WbData\\ParaBase\\ctrl_acsc_z.ini", strCommandCopyFileToFolder, 0);
			//sprintf_s(strCommandCopyFileToFolder, "%s\\ctrl_acsc_w.ini", strSpectrumNewFolderName);
			//CopyFile("C:\\WbData\\ParaBase\\ctrl_acsc_w.ini", strCommandCopyFileToFolder, 0);
			//
			//sprintf_s(strCommandCopyFileToFolder, "%s\\MtnTune_acs_x.ini", strSpectrumNewFolderName);
			//CopyFile("C:\\WbData\\ParaBase\\MtnTune_acs_x.ini", strCommandCopyFileToFolder, 0);
			//sprintf_s(strCommandCopyFileToFolder, "%s\\MtnTune_acs_y.ini", strSpectrumNewFolderName);
			//CopyFile("C:\\WbData\\ParaBase\\MtnTune_acs_y.ini", strCommandCopyFileToFolder, 0);
			//sprintf_s(strCommandCopyFileToFolder, "%s\\MtnTune_acs_z.ini", strSpectrumNewFolderName);
			//CopyFile("C:\\WbData\\ParaBase\\MtnTune_acs_z.ini", strCommandCopyFileToFolder, 0);
			// 20110801
			sprintf_s(strCommandCopyFileToFolder, "%s\\McData.dat", strSpectrumNewFolderName);
			CopyFile("D:\\WbData\\McData.dat", strCommandCopyFileToFolder, 0);
			sprintf_s(strCommandCopyFileToFolder, "%s\\PowerOn.dat", strSpectrumNewFolderName);
			CopyFile("D:\\WbData\\PowerOn.dat", strCommandCopyFileToFolder, 0);
			sprintf_s(strCommandCopyFileToFolder, "%s\\CtrlData.dat", strSpectrumNewFolderName);
			CopyFile("D:\\WbData\\CtrlData.dat", strCommandCopyFileToFolder, 0);
			sprintf_s(strCommandCopyFileToFolder, "%s\\WhData.dat", strSpectrumNewFolderName);
			CopyFile("D:\\WbData\\WhData.dat", strCommandCopyFileToFolder, 0);
			
			sprintf_s(strCommandCopyFileToFolder, "%s\\McConfig.ini", strSpectrumNewFolderName);
			CopyFile("D:\\WbData\\McConfig.ini", strCommandCopyFileToFolder, 0);
		}
	}
	else
	{
		iRet = MTN_API_ERROR;
	}
	return iRet;
}
extern char strWbTuningNewFolderName[];
int mtn_tune_save_servo_parameter_xy_folder(HANDLE hAcsHandle)
{
	int iRet = MTN_API_ERROR;

	int nNumLineBuff0 = 0;
	if(acs_buffprog_upload_prog_get_line(0) == MTN_API_OK_ZERO)
	{
		nNumLineBuff0 = acs_buffprog_get_num_lines_at_buff(0);
	}

	if(hAcsHandle != ACSC_INVALID)
	{
//		mtnapi_dll_init_master_struct_ptr();
		if(nNumLineBuff0 >= 1)
		{ // upload WB - servo & Speed parameter
			mtnapi_dll_save_wb_speed_servo_parameter_acs(hAcsHandle);
		}
char strCommandCopyFileToFolder[1024];

//		if(_chdir("C:\\WbData\\ParaBase\\") == FALSE) // 0: OK, folder exist
//		{			
			sprintf_s(strCommandCopyFileToFolder, "%s\\ctrl_acsc_x.ini", strWbTuningNewFolderName);
			CopyFile("C:\\WbData\\ParaBase\\ctrl_acsc_x.ini", strCommandCopyFileToFolder, 0);
			sprintf_s(strCommandCopyFileToFolder, "%s\\ctrl_acsc_y.ini", strWbTuningNewFolderName);
			CopyFile("C:\\WbData\\ParaBase\\ctrl_acsc_y.ini", strCommandCopyFileToFolder, 0);
			//sprintf_s(strCommandCopyFileToFolder, "%s\\ctrl_acsc_z.ini", strWbTuningNewFolderName);
			//CopyFile("C:\\WbData\\ParaBase\\ctrl_acsc_z.ini", strCommandCopyFileToFolder, 0);
			//sprintf_s(strCommandCopyFileToFolder, "%s\\ctrl_acsc_w.ini", strWbTuningNewFolderName);
			//CopyFile("C:\\WbData\\ParaBase\\ctrl_acsc_w.ini", strCommandCopyFileToFolder, 0);
			//
			sprintf_s(strCommandCopyFileToFolder, "%s\\MtnTune_acs_x.ini", strWbTuningNewFolderName);
			CopyFile("C:\\WbData\\ParaBase\\MtnTune_acs_x.ini", strCommandCopyFileToFolder, 0);
			sprintf_s(strCommandCopyFileToFolder, "%s\\MtnTune_acs_y.ini", strWbTuningNewFolderName);
			CopyFile("C:\\WbData\\ParaBase\\MtnTune_acs_y.ini", strCommandCopyFileToFolder, 0);
			// 20110801
			
			sprintf_s(strCommandCopyFileToFolder, "%s\\McConfig.ini", strWbTuningNewFolderName);
			CopyFile("D:\\WbData\\McConfig.ini", strCommandCopyFileToFolder, 0);
//		}
	}
	else
	{
		iRet = MTN_API_ERROR;
	}
	return iRet;
}

extern char strWbTuningNewFolderName[128];
int mtn_tune_save_servo_parameter_b1w_folder(HANDLE hAcsHandle)
{
	int iRet = MTN_API_ERROR;

	int nNumLineBuff0 = 0;
	if(acs_buffprog_upload_prog_get_line(0) == MTN_API_OK_ZERO)
	{
		nNumLineBuff0 = acs_buffprog_get_num_lines_at_buff(0);
	}

	if(hAcsHandle != ACSC_INVALID)
	{
//		mtnapi_dll_init_master_struct_ptr();
		if(nNumLineBuff0 >= 1)
		{ // upload WB - servo & Speed parameter
			iRet = mtnapi_dll_save_wb_speed_servo_parameter_acs(hAcsHandle);
			if(iRet != MTN_API_OK_ZERO)
			{
				//Sleep(500);
			}
		}
		else  // load from file, may be wrong, // 20120725
		{
//			_mtnapi_dll_init_servo_speed_para_acs(hAcsHandle);
		}

//		Sleep(500);
char strCommandCopyFileToFolder[1024];

		if(_chdir("C:\\WbData\\ParaBase\\"))
		{
			_mkdir("C:\\WbData\\ParaBase\\");
		}

		sprintf_s(strCommandCopyFileToFolder, "%s\\ctrl_acsc_z.ini", strWbTuningNewFolderName);
		CopyFile("C:\\WbData\\ParaBase\\ctrl_acsc_z.ini", strCommandCopyFileToFolder, 0);
		sprintf_s(strCommandCopyFileToFolder, "%s\\MtnTune_acs_z.ini", strWbTuningNewFolderName);
		CopyFile("C:\\WbData\\ParaBase\\MtnTune_acs_z.ini", strCommandCopyFileToFolder, 0);			
		sprintf_s(strCommandCopyFileToFolder, "%s\\McConfig.ini", strWbTuningNewFolderName);
		CopyFile("D:\\WbData\\McConfig.ini", strCommandCopyFileToFolder, 0);
		sprintf_s(strCommandCopyFileToFolder, "%s\\McConfig.dat", strWbTuningNewFolderName);
		CopyFile("D:\\WbData\\McConfig.dat", strCommandCopyFileToFolder, 0);
	}
	else
	{
		iRet = MTN_API_ERROR;
	}
	return iRet;
}


void aft_spectrum_make_new_folder_axis(unsigned char ucFlagExciteAxisZ, int iSysAnaAxis)
{
// Make New Folder to store data
struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;
	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);
static char strMechaTronicsConfig[32];
	// Mechanical configuration
	int iMechCfg = get_sys_machine_type_flag();   // machine type dependency Item-9

		switch(iMechCfg)
		{
		case WB_MACH_TYPE_VLED_MAGAZINE:
			sprintf_s(strMechaTronicsConfig, 32, "CL_LinX480_Y180_Z-BEI_WC-1");
			break;
		case WB_MACH_TYPE_VLED_FORK:
		case WB_MACH_TYPE_HORI_LED:
		case WB_MACH_TYPE_ONE_TRACK_13V_LED:
		case BE_WB_ONE_TRACK_18V_LED:   // 20120715
		case BE_WB_HORI_20T_LED:   // 20120827
		case WB_STATION_XY_VERTICAL:    // 20111202
		case WB_STATION_XY_TOP:         // 20111202
		case WB_STATION_BH:				// 20111202
			sprintf_s(strMechaTronicsConfig, 32, "%s-%d_",
				astrMachineTypeNameLabel_en[iMechCfg],
				get_sys_machine_serial_num());
//			break;
//			sprintf_s(strMechaTronicsConfig, 32, "CL_Lin-X360-Y240_Z-TAMA_WC-2");
			break;
		//case WB_STATION_XY_VERTICAL:
		//	sprintf_s(strMechaTronicsConfig, 32, "CL_Stn_XY_Vert");
		//	break;
		//case WB_STATION_XY_TOP:
		//	sprintf_s(strMechaTronicsConfig, 32, "CL_Stn_XY_Top");
		//	break;
		//case WB_STATION_BH:
		//	sprintf_s(strMechaTronicsConfig, 32, "CL_Stn_BH");
		//	break;
		default:
			sprintf_s(strMechaTronicsConfig, 32, "CL_OneTrack");
			break;
		}
static char strSystemAnalysisFlag[32];
	// SYSTEM_ANALYSIS_PSEUDO_WHITE_NOISE
	switch(iFlagSpectrumSysAnaFlag)
	{
	case SYSTEM_ANALYSIS_PSEUDO_WHITE_NOISE:
		sprintf_s(strSystemAnalysisFlag, "PseuWhiN");
		break;
	case SYSTEM_ANALYSIS_PSEUDO_SINE_SWEEP:
		sprintf_s(strSystemAnalysisFlag, "SineSwp");
		break;
	case SYSTEM_ANALYSIS_PSEUDO_ONE_PULSE_RESP:
		sprintf_s(strSystemAnalysisFlag, "OnePuls");
		break;
	default : // SYSTEM_ANALYSIS_PSEUDO_WHITE_NOISE
		break;
	}

	if(ucFlagExciteAxisZ) // Including Z
	{
		sprintf_s(strSpectrumNewFolderName, 128, "D:\\MT\\Spec_XYZ_%d.%d.%d.H%d.M%d_%s_%s_Ax%s",
			stpTime->tm_year +1900, stpTime->tm_mon +1, stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, 
			strSystemAnalysisFlag, strMechaTronicsConfig, astAxisInfoWireBond[iSysAnaAxis].strAxisNameEn);
	}
	else
	{
		sprintf_s(strSpectrumNewFolderName, 128, "D:\\MT\\Spec_XY_%d.%d.%d.H%d.M%d_%s_%s_Ax%s",
			stpTime->tm_year +1900, stpTime->tm_mon +1, stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, 
			strSystemAnalysisFlag, strMechaTronicsConfig, astAxisInfoWireBond[iSysAnaAxis].strAxisNameEn);
	}

	if(_chdir("D:\\MT"))
	{
		_mkdir("D:\\MT");
	}
	_mkdir(strSpectrumNewFolderName);
}

void aft_spectrum_make_new_folder(unsigned char ucFlagExciteAxisZ)
{
// Make New Folder to store data
struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;
	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);
static char strMechaTronicsConfig[32];
	// Mechanical configuration
	int iMechCfg = get_sys_machine_type_flag();  // machine type dependency Item-8
	if(aft_spectrum_get_flag_velloop() == TRUE)
	{
		switch(iMechCfg)
		{
		case WB_MACH_TYPE_VLED_MAGAZINE:
			sprintf_s(strMechaTronicsConfig, 32, "VL_LinX480_Y180_Z-BEI_WC-1");
			break;
		case WB_MACH_TYPE_VLED_FORK:
//			sprintf_s(strMechaTronicsConfig, 32, "WB13V_%d_");
//			break;
		case WB_MACH_TYPE_HORI_LED:
		case WB_MACH_TYPE_ONE_TRACK_13V_LED:
		case BE_WB_ONE_TRACK_18V_LED:   // 20120715
		case BE_WB_HORI_20T_LED:   // 20120827
		case WB_STATION_XY_VERTICAL:			// 20111202
		case WB_STATION_XY_TOP:					// 20111202
		case WB_STATION_BH:						// 20111202
			sprintf_s(strMechaTronicsConfig, 32, "VL_%s-%d_",
				astrMachineTypeNameLabel_en[iMechCfg],
				get_sys_machine_serial_num());
			break;
		default:
			sprintf_s(strMechaTronicsConfig, 32, "VL_OneTrack");  // Lin-X360-Y240_Z-TAMA_WC-3
			break;
		}
	}
	else
	{
		switch(iMechCfg)
		{
		case WB_MACH_TYPE_VLED_MAGAZINE:
			sprintf_s(strMechaTronicsConfig, 32, "CL_LinX480_Y180_Z-BEI_WC-1");
			break;
		case WB_MACH_TYPE_VLED_FORK:
//			sprintf_s(strMechaTronicsConfig, 32, "CL_LinX480_Y180_Z-BEI_WC-2");
//			break;
		case WB_MACH_TYPE_ONE_TRACK_13V_LED:
		case BE_WB_ONE_TRACK_18V_LED:   // 20120715
		case BE_WB_HORI_20T_LED:   // 20120827
		case WB_MACH_TYPE_HORI_LED:
		case WB_STATION_XY_VERTICAL:		// 20111202
		case WB_STATION_XY_TOP:				// 20111202
		case WB_STATION_BH:					// 20111202
//			sprintf_s(strMechaTronicsConfig, 32, "CL_Lin-X360-Y240_Z-TAMA_WC-2");
			sprintf_s(strMechaTronicsConfig, 32, "%s-%d_",
				astrMachineTypeNameLabel_en[iMechCfg],
				get_sys_machine_serial_num());
			break;
		default:
			sprintf_s(strMechaTronicsConfig, 32, "CL_OneTrack");  // Lin-X360-Y240_Z-TAMA_WC-3
			break;
		}
	}
static char strSystemAnalysisFlag[32];
	// SYSTEM_ANALYSIS_PSEUDO_WHITE_NOISE
	switch(iFlagSpectrumSysAnaFlag)
	{
	case SYSTEM_ANALYSIS_PSEUDO_WHITE_NOISE:
		sprintf_s(strSystemAnalysisFlag, "PseuWhiN");
		break;
	case SYSTEM_ANALYSIS_PSEUDO_SINE_SWEEP:
		sprintf_s(strSystemAnalysisFlag, "SineSwp");
		break;
	case SYSTEM_ANALYSIS_PSEUDO_ONE_PULSE_RESP:
		sprintf_s(strSystemAnalysisFlag, "OnePuls");
		break;
	default : // SYSTEM_ANALYSIS_PSEUDO_WHITE_NOISE
		break;
	}

	if(ucFlagExciteAxisZ) // Including Z
	{
		sprintf_s(strSpectrumNewFolderName, 128, "D:\\MT\\Spec_%s_XYZ_%s_%d.%d.%d.H%d.M%d", 
			strSystemAnalysisFlag, strMechaTronicsConfig,
			stpTime->tm_year +1900, stpTime->tm_mon +1, stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min);
	}
	else
	{
		sprintf_s(strSpectrumNewFolderName, 128, "D:\\MT\\Spec_%s_XY_%s_%d.%d.%d.H%d.M%d", 
			strSystemAnalysisFlag, strMechaTronicsConfig,
			stpTime->tm_year +1900, stpTime->tm_mon +1, stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min);
	}
	if(_chdir("D:\\MT"))
	{
		_mkdir("D:\\MT");
	}
	_mkdir(strSpectrumNewFolderName);
}

void aft_spectrum_make_new_folder_tuning_vk(unsigned char ucFlagExciteAxisZ, unsigned int uiCurrCase, unsigned int uiTotalCase)
{
// Make New Folder to store data
struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;
	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);
static char strMechaTronicsConfig[32];
	// Mechanical configuration
	int iMechCfg = get_sys_machine_type_flag();  // machine type dependency Item-8
	if(aft_spectrum_get_flag_velloop() == TRUE)
	{
		switch(iMechCfg)
		{
		case WB_MACH_TYPE_VLED_MAGAZINE:
			sprintf_s(strMechaTronicsConfig, 32, "VL_LinX480_Y180_Z-BEI_WC-1");
			break;
		case WB_MACH_TYPE_VLED_FORK:
//			sprintf_s(strMechaTronicsConfig, 32, "WB13V_%d_");
//			break;
		case WB_MACH_TYPE_HORI_LED:
		case WB_MACH_TYPE_ONE_TRACK_13V_LED:
		case BE_WB_ONE_TRACK_18V_LED:   // 20120715
		case BE_WB_HORI_20T_LED:   // 20120827
		case WB_STATION_XY_VERTICAL:			// 20111202
		case WB_STATION_XY_TOP:					// 20111202
		case WB_STATION_BH:						// 20111202
			sprintf_s(strMechaTronicsConfig, 32, "VL_%s-%d_",
				astrMachineTypeNameLabel_en[iMechCfg],
				get_sys_machine_serial_num());
			break;
		default:
			sprintf_s(strMechaTronicsConfig, 32, "VL_OneTrack");  // Lin-X360-Y240_Z-TAMA_WC-3
			break;
		}
	}
	else
	{
		switch(iMechCfg)
		{
		case WB_MACH_TYPE_VLED_MAGAZINE:
			sprintf_s(strMechaTronicsConfig, 32, "CL_LinX480_Y180_Z-BEI_WC-1");
			break;
		case WB_MACH_TYPE_VLED_FORK:
//			sprintf_s(strMechaTronicsConfig, 32, "CL_LinX480_Y180_Z-BEI_WC-2");
//			break;
		case WB_MACH_TYPE_ONE_TRACK_13V_LED:
		case BE_WB_ONE_TRACK_18V_LED:   // 20120715
		case BE_WB_HORI_20T_LED:   // 20120827
		case WB_MACH_TYPE_HORI_LED:
		case WB_STATION_XY_VERTICAL:		// 20111202
		case WB_STATION_XY_TOP:				// 20111202
		case WB_STATION_BH:					// 20111202
//			sprintf_s(strMechaTronicsConfig, 32, "CL_Lin-X360-Y240_Z-TAMA_WC-2");
			sprintf_s(strMechaTronicsConfig, 32, "%s-%d_",
				astrMachineTypeNameLabel_en[iMechCfg],
				get_sys_machine_serial_num());
			break;
		default:
			sprintf_s(strMechaTronicsConfig, 32, "CL_OneTrack");  // Lin-X360-Y240_Z-TAMA_WC-3
			break;
		}
	}
static char strSystemAnalysisFlag[32];
	// SYSTEM_ANALYSIS_PSEUDO_WHITE_NOISE
	switch(iFlagSpectrumSysAnaFlag)
	{
	case SYSTEM_ANALYSIS_PSEUDO_WHITE_NOISE:
		sprintf_s(strSystemAnalysisFlag, "PseuWhiN");
		break;
	case SYSTEM_ANALYSIS_PSEUDO_SINE_SWEEP:
		sprintf_s(strSystemAnalysisFlag, "SineSwp");
		break;
	case SYSTEM_ANALYSIS_PSEUDO_ONE_PULSE_RESP:
		sprintf_s(strSystemAnalysisFlag, "OnePuls");
		break;
	default : // SYSTEM_ANALYSIS_PSEUDO_WHITE_NOISE
		break;
	}

	if(ucFlagExciteAxisZ) // Including Z
	{
		sprintf_s(strSpectrumNewFolderName, 128, "D:\\MT\\Spec_%s_XYZ_%s_%d.%d.%d.H%d.M%d_%dof%d", 
			strSystemAnalysisFlag, strMechaTronicsConfig,
			stpTime->tm_year +1900, stpTime->tm_mon +1, stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min,
			uiCurrCase, uiTotalCase);
	}
	else
	{
		sprintf_s(strSpectrumNewFolderName, 128, "D:\\MT\\Spec_%s_XY_%s_%d.%d.%d.H%d.M%d_%dof%d", 
			strSystemAnalysisFlag, strMechaTronicsConfig,
			stpTime->tm_year +1900, stpTime->tm_mon +1, stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min,
			uiCurrCase, uiTotalCase);
	}
	if(_chdir("D:\\MT"))
	{
		_mkdir("D:\\MT");
	}
	_mkdir(strSpectrumNewFolderName);
}

