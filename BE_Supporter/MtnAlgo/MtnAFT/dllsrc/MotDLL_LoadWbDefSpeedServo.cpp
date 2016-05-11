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

#include "MtnApi.h"
#include "MtnInitAcs.h"
#include "MotAlgo_DLL.h"
#include "MtnAlgo_Private.h"

#include "mtndefin.h"
#include "MtnWbDef.h"  // 20130305

extern COMM_SETTINGS stServoControllerCommSet;

// Defined local
extern void mtnapi_wb_LoadDefaultMotionPara(SERVO_MOTION_PARAMETER *pSpeedPara);
extern void mtnapi_wb_LoadDefaultControlProfile(SERVO_CONTROL_PARAMETER *pControlProfile);
extern void mtnapi_dll_InitWireBondServoAxisName();

extern void mtnapi_init_static_vaj_unit_factor();

#ifdef __INIT_ACS_BUFF__
extern short acs_buff_prog_init_comm(HANDLE stCommHandle);
#endif  // __INIT_ACS_BUFF__


char cFlag_ConnectACS_SC_UDI;
char sys_acs_communication_get_flag_sc_udi()
{
	return cFlag_ConnectACS_SC_UDI;
}

char aAxisList_BndZ_ACS[256] = {APP_Z_BOND_ACS_PCI_ID, APP_Z_BOND_ACS_SC_UDI_ID};
char aAxisList_BndWireClamp_ACS[256] = {APP_WIRE_CLAMP_ACS_PCI_ID, APP_WIRE_CLAMP_ACS_SC_UDI_ID};

void sys_acs_communication_set_flag_sc_udi(char cTempFlag)
{
	cFlag_ConnectACS_SC_UDI = cTempFlag;
}
char sys_get_acs_communication_flag_udi()
{
	return cFlag_ConnectACS_SC_UDI;
}
char sys_get_acs_axis_id_bnd_z()
{
	return aAxisList_BndZ_ACS[cFlag_ConnectACS_SC_UDI];
}
char sys_get_acs_axis_id_wire_clamp()
{
	return aAxisList_BndWireClamp_ACS[cFlag_ConnectACS_SC_UDI];
}

///// static variables
static HANDLE hLocalDllInitAcsHandle; static int iFlagInitByLocalAcsComm = 0;
static double dTableX_EncRes_mm;
static double dTableY_EncRes_mm;
static double dBndHeadZ_EncRes_mm;
static double dFactorBH_Z_Acc_Fr_cnt_sec_to_si;
static double dFactorTbl_X_Acc_Fr_cnt_sec_to_si;
static double dFactorTbl_Y_Acc_Fr_cnt_sec_to_si;
static double dFactorBH_Z_Jerk_Fr_cnt_sec_to_si;
static double dFactorTbl_X_Jerk_Fr_cnt_sec_to_si;
static double dFactorTbl_Y_Jerk_Fr_cnt_sec_to_si;
static double dFactorBH_Z_Vel_Fr_cnt_sec_to_mmps;
static double dFactorTbl_X_Vel_Fr_cnt_sec_to_mmps;
static double dFactorTbl_Y_Vel_Fr_cnt_sec_to_mmps;

char *strDeBugServoWb = "C:\\WbData\\ParaBase\\Test1.000";
FILE *fpDebugServoIni = NULL;


extern SERVO_AXIS_BLK stServoAxis_ACS[MAX_CTRL_AXIS_PER_SERVO_BOARD];


#define MOT_ALGO_DLL_VERSION_MINOR   2
#define MOT_ALGO_DLL_VERSION_MAJOR   1
#define MOT_ALGO_DLL_VERSION_DATE    27
#define MOT_ALGO_DLL_VERSION_MONTH   2
#define MOT_ALGO_DLL_VERSION_YEAR    2012

int mtnapi_dll_init_get_version(VERSION_INFO *stpVersion)
{
	stpVersion->usVerMajor = MOT_ALGO_DLL_VERSION_MAJOR;
	stpVersion->usVerMinor = MOT_ALGO_DLL_VERSION_MINOR;
	//// 
	stpVersion->usVerDate = MOT_ALGO_DLL_VERSION_DATE;
	stpVersion->usVerMonth = MOT_ALGO_DLL_VERSION_MONTH;
	stpVersion->usVerYear = MOT_ALGO_DLL_VERSION_YEAR;

	return MTN_API_OK_ZERO;
}

static int iFlagDonePointerInitialization;
// Save speed and servo parameter for wb application
char *strDefaultSaveFilename_SpeedServoWb = "C:\\WbData\\ParaBase\\ServoMaster.ini";
char *strDefaultReadDownloadFilename_SpeedServoWb = "C:\\WbData\\DefParaBase\\ServoMaster.ini";
// Init default parameter
// it is important to initialize the memory in the dll
void mtnapi_dll_init_master_struct_ptr()
{
	if( iFlagDonePointerInitialization == 0)
	{
		unsigned int ii;
		stServoACS.uiTotalNumAxis = MAX_CTRL_AXIS_PER_SERVO_BOARD;
		for( ii = 0; ii<stServoACS.uiTotalNumAxis; ii++)
		{
			stServoACS.stpServoAxis_ACS[ii] = &stServoAxis_ACS[ii];
			stServoACS.stpSafetyAxis_ACS[ii] = &stSafetyParaAxis_ACS[ii];
			stServoACS.stpBasicAxis_ACS[ii] = &stBasicParaAxis_ACS[ii];
		}
		iFlagDonePointerInitialization = 1;
	}
	sprintf_s(&stServoACS.strFilename[0], MTN_API_MAX_STRLEN_FILENAME, "%s", strDefaultSaveFilename_SpeedServoWb);

}

#include "MtnWbDef.h"
extern char *astrWireBondServoAxisNameEn[];
extern char *astrWireBondServoAxisNameCn[];
extern double afRatioRMS_DrvCmd[MAX_SERVO_AXIS_WIREBOND];
extern AXIS_INFO_WIRE_BOND astAxisInfoWireBond[MAX_SERVO_AXIS_WIREBOND];

void mtnapi_dll_InitWireBondServoAxisName_VerLED_ACS()
{
	astAxisInfoWireBond[WB_AXIS_TABLE_X].iAxisInCtrlCardACS = ACS_CARD_AXIS_X;
	astAxisInfoWireBond[WB_AXIS_TABLE_Y].iAxisInCtrlCardACS = ACS_CARD_AXIS_Y;
	astAxisInfoWireBond[WB_AXIS_BOND_Z].iAxisInCtrlCardACS = ACS_CARD_AXIS_A;
	astAxisInfoWireBond[WB_AXIS_WIRE_CLAMP].iAxisInCtrlCardACS = ACS_CARD_AXIS_B;

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

void mtnapi_dll_InitWireBondServoAxisName_YZ_1SP_ACS()
{
	astAxisInfoWireBond[WB_AXIS_TABLE_X].iAxisInCtrlCardACS = ACS_CARD_AXIS_Y;
	astAxisInfoWireBond[WB_AXIS_TABLE_Y].iAxisInCtrlCardACS = ACS_CARD_AXIS_X;
	astAxisInfoWireBond[WB_AXIS_BOND_Z].iAxisInCtrlCardACS = ACS_CARD_AXIS_A;
	astAxisInfoWireBond[WB_AXIS_WIRE_CLAMP].iAxisInCtrlCardACS = ACS_CARD_AXIS_B;

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

void mtnapi_dll_InitWireBondServoAxisName()
{
	int iTempMachTypeFlag = get_sys_machine_type_flag();

	if(iTempMachTypeFlag == WB_MACH_TYPE_VLED_FORK ||
		iTempMachTypeFlag == WB_MACH_TYPE_ONE_TRACK_13V_LED ||
		iTempMachTypeFlag == WB_MACH_TYPE_VLED_MAGAZINE )  // machine type dependency Item-2.
	{
		mtnapi_dll_InitWireBondServoAxisName_VerLED_ACS();
	}
	else if(iTempMachTypeFlag == WB_MACH_TYPE_HORI_LED || iTempMachTypeFlag == WB_STATION_XY_TOP|| iTempMachTypeFlag == BE_WB_HORI_20T_LED
		|| iTempMachTypeFlag == BE_WB_ONE_TRACK_18V_LED)
	{
		mtnapi_dll_InitWireBondServoAxisName_YZ_1SP_ACS();
	}
	else
	{
		mtnapi_dll_InitWireBondServoAxisName_VerLED_ACS();
	}
}

void mtnapi_dll_init_all()
{
	// 1. Machine config
	int iTempMachCfg;
	if(mtn_api_load_machine_config(&iTempMachCfg) == MTN_API_OK_ZERO)
	{
		mtn_wb_dll_set_sys_machine_type(iTempMachCfg); //iFlagSysMachineType = iTempMachCfg;
	}
	// 2. initialize with default parameter
	mtnapi_dll_init_master_struct_ptr(); // mtnapi_init_master_struct_ptr();
	mtnapi_init_def_servo_acs();

    // 3. WireBonder Application related
//	mtnapi_dll_InitWireBondServoAxisName();  // InitWireBondServoAxisName(); 	//
	if(iTempMachCfg == WB_MACH_TYPE_VLED_FORK || iTempMachCfg == WB_MACH_TYPE_ONE_TRACK_13V_LED ||
		iTempMachCfg == WB_MACH_TYPE_VLED_MAGAZINE ) // machine type dependency Item-2.
	{
		mtnapi_dll_InitWireBondServoAxisName_VerLED_ACS();
	}
	else if(iTempMachCfg == WB_MACH_TYPE_HORI_LED || iTempMachCfg == WB_STATION_XY_TOP || iTempMachCfg == BE_WB_HORI_20T_LED
		|| 	iTempMachCfg == BE_WB_ONE_TRACK_18V_LED)
	{
		mtnapi_dll_InitWireBondServoAxisName_YZ_1SP_ACS();
	}
	else
	{
		mtnapi_dll_InitWireBondServoAxisName_VerLED_ACS();
	}

	mtnapi_init_static_vaj_unit_factor();

	// 4  Tuning Related
	mtn_tune_init_wb_bondhead_tuning_position_set();
	mtn_tune_init_wb_table_x_tuning_position_set();

	mtn_dll_wb_tune_initialization(); // 20120117

	// 5. Home Related
	iTempMachCfg = get_sys_machine_type_flag();
	if(iTempMachCfg == WB_MACH_TYPE_VLED_FORK || iTempMachCfg == WB_STATION_XY_VERTICAL  ||
		iTempMachCfg == WB_MACH_TYPE_VLED_MAGAZINE )   // machine type dependency Item-3.
	{ 
		mtn_dll_init_def_para_search_index_vled_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
		mtn_tune_init_wb_table_y_tuning_vled_position_set();
	}
	else if(iTempMachCfg == WB_MACH_TYPE_HORI_LED || iTempMachCfg == WB_STATION_XY_TOP|| iTempMachCfg == BE_WB_HORI_20T_LED
		|| iTempMachCfg == BE_WB_ONE_TRACK_18V_LED)
	{
		mtn_dll_init_def_para_search_index_hori_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
		if(iTempMachCfg == BE_WB_ONE_TRACK_18V_LED)
		{
			mtn_tune_init_wb_table_y_tuning_vled_position_set();
		}
		else
		{
			mtn_tune_init_wb_table_y_tuning_hori_led_position_set();
		}
	}
	else // if (get_sys_machine_type_flag() == WB_MACH_TYPE_ONE_TRACK_13V_LED)  // default
	{
		mtn_dll_init_def_para_search_index_13v_vled_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
		mtn_tune_init_wb_table_y_tuning_vled_position_set();
	}
}

extern short acs_init_buffer_prog_prbs_prof_cfg_move();
short acs_clear_buffer_prog_prbs_prof_cfg_move();

// Update Last saved parameter for next time of loading
// Check path
// modify the READONLY mode to R/W
// delete
// Copy
// Set mode to READONLY


// 20110404, to write to para, with protection if no such folder
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <direct.h>
int mtnapi_create_para_base_master_template()
{
	int iRet = MTN_API_OK_ZERO;
//	unsigned int ii;
	FILE *fptr;
	char strPathBuffer[_MAX_PATH];
	char strPathCurrBakBuffer[_MAX_PATH];


	_getcwd(strPathCurrBakBuffer, _MAX_PATH);
	if(_chdir("C:\\WbData\\ParaBase\\"))
	{
		_mkdir("C:\\WbData");  // 20120522
		_mkdir("C:\\WbData\\ParaBase\\");
	}
	_chdir(strPathCurrBakBuffer);

	_makepath(strPathBuffer, "C", "\\WbData\\ParaBase\\", "ServoMaster", "ini");

	fopen_s(&fptr, strDefaultSaveFilename_SpeedServoWb, "w");

	if(fptr != NULL)
	{
		fprintf(fptr, "## BE-WB Servo Control Parameter dataBase\n");
		fprintf(fptr, "## Under GNU license, (c) All right reserved\n");
		fprintf(fptr, "## efsika@gmail.com\n");
		fprintf(fptr, "\n\n[SERVO_MASTER_CONFIG]\n");
		fprintf(fptr, "TOTAL_AXIS = %d\n", 4);
		fprintf(fptr, "CONTROLLER_BOARD = A3S\n\n"); // just for security

		// Table-X 0
			fprintf(fptr, "[SERVO_SETTING_AXIS_1]\n");
			fprintf(fptr, "APPLICATION_NAME = X_TBL\n");
			fprintf(fptr, "PARA_FILE_PATH = C:\\WbData\\ParaBase\\ctrl_acsc_x.ini\n");
			fprintf(fptr, "MTN_TUNE_AXIS = C:\\WbData\\ParaBase\\MtnTune_acs_x.ini\n");
			fprintf(fptr, "AXIS_ON_SERVO_BOARD = 1\n\n");

		// Table-Y 1
			fprintf(fptr, "[SERVO_SETTING_AXIS_2]\n");
			fprintf(fptr, "APPLICATION_NAME = Y_TBL\n");
			fprintf(fptr, "PARA_FILE_PATH = C:\\WbData\\ParaBase\\ctrl_acsc_y.ini\n");
			fprintf(fptr, "MTN_TUNE_AXIS = C:\\WbData\\ParaBase\\MtnTune_acs_y.ini\n");
			fprintf(fptr, "AXIS_ON_SERVO_BOARD = 0\n\n");
		// Bnd-Z, 4-ACS
			fprintf(fptr, "[SERVO_SETTING_AXIS_3]\n");
			fprintf(fptr, "APPLICATION_NAME = Z_BONDHEAD\n");
			fprintf(fptr, "PARA_FILE_PATH = C:\\WbData\\ParaBase\\ctrl_acsc_z.ini\n");
			fprintf(fptr, "MTN_TUNE_AXIS = C:\\WbData\\ParaBase\\MtnTune_acs_z.ini\n");
			fprintf(fptr, "AXIS_ON_SERVO_BOARD = 4\n\n");
		// WireClamp, 5
			fprintf(fptr, "[SERVO_SETTING_AXIS_4]\n");
			fprintf(fptr, "APPLICATION_NAME = WireClamp\n");
			fprintf(fptr, "PARA_FILE_PATH = C:\\WbData\\ParaBase\\ctrl_acsc_w.ini\n");
			fprintf(fptr, "MTN_TUNE_AXIS = C:\\WbData\\ParaBase\\MtnTune_acs_z.ini\n");
			fprintf(fptr, "AXIS_ON_SERVO_BOARD = 5\n\n");

		//for(int ii = 0; ii <NUM_TOTAL_PROGRAM_BUFFER; ii ++)               // // 20090508
		//{
		//	fprintf(fptr, "\n\n[ACS_PROG_CONFIG-%d]\n", ii);
		//	fprintf(fptr, "0x%x\n", stServoACS.stGlobalParaACS.aiProgramFlags[ii]);	// ProgramFlags
		//	fprintf(fptr, "%d\n", stServoACS.stGlobalParaACS.aiProgramRate[ii]);	// ProgramRate
		//	fprintf(fptr, "%d\n", stServoACS.stGlobalParaACS.aiProgramAutoRoutineRate[ii]);	// ProgramAutoRoutineRate
		//}

		fclose(fptr);
	}
	else
	{
		iRet = MTN_API_ERR_FILE_PTR;
	}
	return iRet;

}
// Save to c:\\WbData\\ParaBase
int mtnapi_dll_save_wb_speed_servo_parameter_acs(HANDLE hAcsHandle)
{
	int iRet = MTN_API_OK_ZERO;
	mtnapi_dll_init_master_struct_ptr();
//	mtnapi_test_write_file_1();
	if((iRet =sys_init_acs_communication()) == MTN_API_OK_ZERO)
	{
		hLocalDllInitAcsHandle = stServoControllerCommSet.Handle;
		// Buffer Program
#ifdef  __INIT_ACS_BUFF__
		acs_buff_prog_init_comm(stServoControllerCommSet.Handle);
		mtn_api_clear_acs_buffer_prof();  // Each time stop and clean the buffer program, before downloading
		Sleep(200);
		mtn_api_init_acs_buffer_prog();
#endif  // __INIT_ACS_BUFF__
		iFlagInitByLocalAcsComm = 1;
	}
	else
	{
		hLocalDllInitAcsHandle = hAcsHandle;
	}

	// Initialize buffer program
#ifdef __INIT_ACS_BUFF__
	acs_buff_prog_init_comm(hLocalDllInitAcsHandle);
	acs_init_buffer_prog_prbs_prof_cfg_move(); // must initialize buffer program
#endif  // __INIT_ACS_BUFF__

	mtnapi_confirm_para_base_path_exist();  // 20110404, Protection to handle
	iRet = mtnapi_init_master_config_acs(strDefaultSaveFilename_SpeedServoWb, hLocalDllInitAcsHandle); // NOT, mtnapi_init_servo_control_para_acs
	iRet = mtnapi_upload_wb_servo_speed_parameter_acs(hLocalDllInitAcsHandle);
//	mtnapi_test_write_file_2(hAcsHandle);

	if(iRet == MTN_API_OK_ZERO)
	{
		iRet = mtnapi_save_servo_parameter_acs();
	}

//	acs_clear_buffer_prog_prbs_prof_cfg_move();
	if(iFlagInitByLocalAcsComm == 1)
	{
#ifdef __INIT_ACS_BUFF__
		mtn_api_clear_acs_buffer_prof();
#endif  // __INIT_ACS_BUFF__

		acsc_CloseComm(stServoControllerCommSet.Handle);
		iFlagInitByLocalAcsComm = 0;  // Protection, 20110121
	}
//	mtnapi_test_close_file();

	return iRet;
}
//#define __DEBUG__
int mtnapi_dll_init_wb_speed_parameter_acs(HANDLE hAcsHandle, SERVO_MOTION_PARAMETER *pSpeedPara)
{
	mtnapi_dll_init_all(); // mtnapi_dll_init_master_struct_ptr();
	int iRet = MTN_API_OK_ZERO;

#ifdef __DEBUG__
	mtnapi_test_write_file_1();
#endif

	mtnapi_init_wb_def_servo_control_para_acs(hAcsHandle);
	if( iRet == MTN_API_OK_ZERO)
	{
		mtnapi_wb_LoadDefaultMotionPara(pSpeedPara);
	}
	else
	{
		mtnapi_wb_LoadDefaultMotionPara(pSpeedPara);
		iRet = MTN_API_ERROR_INIT_WB_MOTION;
	}
#ifdef __DEBUG__
	mtnapi_test_write_file_3(pSpeedPara);
	mtnapi_test_close_file();
#endif
	return iRet;
}

int mtnapi_dll_init_wb_servo_parameter_acs(HANDLE hAcsHandle, SERVO_CONTROL_PARAMETER *pControlProfile)
{
	mtnapi_dll_init_master_struct_ptr();
	int iRet = MTN_API_OK_ZERO;

	mtnapi_init_wb_def_servo_control_para_acs(hAcsHandle); // call mtnapi_init_servo_control_para_acs

	if( iRet == MTN_API_OK_ZERO)
	{
		mtnapi_wb_LoadDefaultControlProfile(pControlProfile);
	}
	else
	{
		mtnapi_wb_LoadDefaultControlProfile(pControlProfile);
		iRet = MTN_API_ERROR_INIT_WB_MOTION;
	}
	return iRet;
}

extern AXIS_INFO_WIRE_BOND astAxisInfoWireBond[MAX_SERVO_AXIS_WIREBOND];


void mtnapi_init_static_vaj_unit_factor()
{
	dTableX_EncRes_mm = astAxisInfoWireBond[WB_AXIS_TABLE_X].afEncoderResolution_cnt_p_mm;
	dTableY_EncRes_mm = astAxisInfoWireBond[WB_AXIS_TABLE_Y].afEncoderResolution_cnt_p_mm;
	dBndHeadZ_EncRes_mm = astAxisInfoWireBond[WB_AXIS_BOND_Z].afEncoderResolution_cnt_p_mm;

	dFactorBH_Z_Acc_Fr_cnt_sec_to_si = 1.0/dBndHeadZ_EncRes_mm/1000;
	dFactorTbl_X_Acc_Fr_cnt_sec_to_si = 1.0/dTableX_EncRes_mm/1000;
	dFactorTbl_Y_Acc_Fr_cnt_sec_to_si = 1.0/dTableY_EncRes_mm/1000;
	dFactorBH_Z_Jerk_Fr_cnt_sec_to_si = 1.0/dBndHeadZ_EncRes_mm/1000;
	dFactorTbl_X_Jerk_Fr_cnt_sec_to_si = 1.0/dTableX_EncRes_mm/1000;
	dFactorTbl_Y_Jerk_Fr_cnt_sec_to_si = 1.0/dTableY_EncRes_mm/1000;

	dFactorBH_Z_Vel_Fr_cnt_sec_to_mmps = 1.0/dBndHeadZ_EncRes_mm;
	dFactorTbl_X_Vel_Fr_cnt_sec_to_mmps = 1.0/dTableX_EncRes_mm;
	dFactorTbl_Y_Vel_Fr_cnt_sec_to_mmps = 1.0/dTableY_EncRes_mm;
}

void mtnapi_wb_LoadDefaultMotionPara(SERVO_MOTION_PARAMETER *pSpeedPara)
{

	int ii;
  // X
    // pSpeedPara->stSpeedProfileX[0].nMaxVel = 950;   //mm/s
    // pSpeedPara->stSpeedProfileX[0].nMaxAcc = 25;    //m/s^2 //@3
    // pSpeedPara->stSpeedProfileX[0].lJerk   = 6000;  //m/s^3

    //pSpeedPara->stSpeedProfileX[1].nMaxVel = 950;   //mm/s
    //pSpeedPara->stSpeedProfileX[1].nMaxAcc = 22;    //m/s^2
    //pSpeedPara->stSpeedProfileX[1].lJerk   = 3000;  //m/s^3

    // pSpeedPara->stSpeedProfileX[2].nMaxVel = 950;   //mm/s
    // pSpeedPara->stSpeedProfileX[2].nMaxAcc = 15;    //m/s^2
    // pSpeedPara->stSpeedProfileX[2].lJerk   = 1500;  //m/s^3

    //pSpeedPara->stSpeedProfileX[3].nMaxVel = 950;   //mm/s
    //pSpeedPara->stSpeedProfileX[3].nMaxAcc = 10;    //m/s^2
    //pSpeedPara->stSpeedProfileX[3].lJerk   = 500;   //m/s^3
	for(ii = 0; ii<=3; ii++)  // hard-code from WireBonder.exe
	{
		pSpeedPara->stSpeedProfileX[ii].nMaxVel = (short)(stServoACS.stpServoAxis_ACS[WB_AXIS_TABLE_X]->stSpeedProfile[ii].dMaxVelocity * dFactorTbl_X_Vel_Fr_cnt_sec_to_mmps);
		pSpeedPara->stSpeedProfileX[ii].nMaxAcc = (short)(stServoACS.stpServoAxis_ACS[WB_AXIS_TABLE_X]->stSpeedProfile[ii].dMaxAcceleration * dFactorTbl_X_Acc_Fr_cnt_sec_to_si);
		pSpeedPara->stSpeedProfileX[ii].lJerk   = (long)(stServoACS.stpServoAxis_ACS[WB_AXIS_TABLE_X]->stSpeedProfile[ii].dMaxJerk * dFactorTbl_X_Jerk_Fr_cnt_sec_to_si);
	}

//	stServoACS.stpServoAxis_ACS[ii]->stSpeedProfile[0].dMaxAcceleration

	for(ii = 0; ii<=3; ii++)  // hard-code from WireBonder.exe
	{
		pSpeedPara->stSpeedProfileY[ii].nMaxVel = (short)(stServoACS.stpServoAxis_ACS[WB_AXIS_TABLE_Y]->stSpeedProfile[ii].dMaxVelocity * dFactorTbl_Y_Vel_Fr_cnt_sec_to_mmps);
		pSpeedPara->stSpeedProfileY[ii].nMaxAcc = (short)(stServoACS.stpServoAxis_ACS[WB_AXIS_TABLE_Y]->stSpeedProfile[ii].dMaxAcceleration * dFactorTbl_Y_Acc_Fr_cnt_sec_to_si);
		pSpeedPara->stSpeedProfileY[ii].lJerk   = (long)(stServoACS.stpServoAxis_ACS[WB_AXIS_TABLE_Y]->stSpeedProfile[ii].dMaxJerk * dFactorTbl_Y_Jerk_Fr_cnt_sec_to_si);
	}

	for(ii = 0; ii<=4; ii++)  // hard-code from WireBonder.exe
	{   // WB_AXIS_BOND_Z
		pSpeedPara->stSpeedProfileZ[ii].nMaxVel = (short)(stServoACS.stpServoAxis_ACS[WB_AXIS_BOND_Z]->stSpeedProfile[ii+1].dMaxVelocity * dFactorBH_Z_Vel_Fr_cnt_sec_to_mmps); //mm/s
		pSpeedPara->stSpeedProfileZ[ii].nMaxAcc = (short)(stServoACS.stpServoAxis_ACS[WB_AXIS_BOND_Z]->stSpeedProfile[ii+1].dMaxAcceleration * dFactorBH_Z_Acc_Fr_cnt_sec_to_si);   //m/s^2
		pSpeedPara->stSpeedProfileZ[ii].lJerk   = (long)(stServoACS.stpServoAxis_ACS[WB_AXIS_BOND_Z]->stSpeedProfile[ii+1].dMaxJerk * dFactorBH_Z_Jerk_Fr_cnt_sec_to_si);      //m/s^3  
	}
	ii = 5;
	pSpeedPara->stSpeedProfileZ[ii].nMaxVel = (short)(stServoACS.stpServoAxis_ACS[WB_AXIS_BOND_Z]->stSpeedProfile[0].dMaxVelocity * dFactorBH_Z_Vel_Fr_cnt_sec_to_mmps); //mm/s
	pSpeedPara->stSpeedProfileZ[ii].nMaxAcc = (short)(stServoACS.stpServoAxis_ACS[WB_AXIS_BOND_Z]->stSpeedProfile[0].dMaxAcceleration * dFactorBH_Z_Acc_Fr_cnt_sec_to_si);   //m/s^2
	pSpeedPara->stSpeedProfileZ[ii].lJerk   = (long)(stServoACS.stpServoAxis_ACS[WB_AXIS_BOND_Z]->stSpeedProfile[0].dMaxJerk * dFactorBH_Z_Jerk_Fr_cnt_sec_to_si);      //m/s^3  

}


void mtnapi_wb_LoadDefaultControlProfile(SERVO_CONTROL_PARAMETER *pControlProfile)
{
int ii;
// Table - X
	for(ii = 0; ii < X_MOTOR_CONTROL_PROFILE_NUM; ii++)
	{
		pControlProfile->stControlProfileX[ii].lVelGain = (long)(stServoACS.stpServoAxis_ACS[0]->stServoParaACS[ii].dVelocityLoopProportionalGain); // 350;
        pControlProfile->stControlProfileX[ii].lVelIntegrator = (long)(stServoACS.stpServoAxis_ACS[0]->stServoParaACS[ii].dVelocityLoopIntegratorGain);
        pControlProfile->stControlProfileX[ii].lPosGain = (long)(stServoACS.stpServoAxis_ACS[0]->stServoParaACS[ii].dPositionLoopProportionalGain); //  150;
		pControlProfile->stControlProfileX[ii].lAccFeedFwd = (long)(stServoACS.stpServoAxis_ACS[0]->stServoParaACS[ii].dAccelerationFeedforward);   // 7250;
	}

	//Y
	for(ii = 0; ii < Y_MOTOR_CONTROL_PROFILE_NUM; ii++)
	{
		pControlProfile->stControlProfileY[ii].lVelGain = (long)(stServoACS.stpServoAxis_ACS[1]->stServoParaACS[ii].dVelocityLoopProportionalGain); // 400;
		pControlProfile->stControlProfileY[ii].lVelIntegrator = (long)(stServoACS.stpServoAxis_ACS[1]->stServoParaACS[ii].dVelocityLoopIntegratorGain); // 320;
        pControlProfile->stControlProfileY[ii].lPosGain = (long)(stServoACS.stpServoAxis_ACS[1]->stServoParaACS[ii].dPositionLoopProportionalGain); // 50;
		pControlProfile->stControlProfileY[ii].lAccFeedFwd = (long)(stServoACS.stpServoAxis_ACS[1]->stServoParaACS[ii].dAccelerationFeedforward);  // 11250;
	}

   //Z=9
	for(ii = 0; ii < Z_MOTOR_CONTROL_PROFILE_NUM; ii++)
	{
		pControlProfile->stControlProfileZ[ii].lVelGain = (long)(stServoACS.stpServoAxis_ACS[2]->stServoParaACS[ii].dVelocityLoopProportionalGain);  //  30;
		pControlProfile->stControlProfileZ[ii].lVelIntegrator = (long)(stServoACS.stpServoAxis_ACS[2]->stServoParaACS[ii].dVelocityLoopIntegratorGain); //  200;
		pControlProfile->stControlProfileZ[ii].lPosGain = (long)(stServoACS.stpServoAxis_ACS[2]->stServoParaACS[ii].dPositionLoopProportionalGain);  //  100;
		pControlProfile->stControlProfileZ[ii].lAccFeedFwd = (long)(stServoACS.stpServoAxis_ACS[2]->stServoParaACS[ii].dAccelerationFeedforward);  //  650;
	}


}


int mtnapi_dll_init_servo_control_para_acs(HANDLE hAcsHandle, SERVO_MOTION_PARAMETER *pSpeedPara, SERVO_CONTROL_PARAMETER *pControlProfile) 
{
	mtnapi_dll_init_master_struct_ptr();
	int iRet = mtnapi_init_wb_def_servo_control_para_acs(hAcsHandle);

	if( iRet == MTN_API_OK_ZERO)
	{
		mtnapi_wb_LoadDefaultMotionPara(pSpeedPara);
		mtnapi_wb_LoadDefaultControlProfile(pControlProfile);
	}
	else
	{
		mtnapi_wb_LoadDefaultMotionPara(pSpeedPara);
		mtnapi_wb_LoadDefaultControlProfile(pControlProfile);
		iRet = MTN_API_ERROR_INIT_WB_MOTION;
	}
	return iRet;
}

#include "MTN_WB_INTERFACE.h"
static SERVO_MOTION_PARAMETER stSpeedPara;
static SERVO_CONTROL_PARAMETER stControlProfile;

int _mtnapi_dll_init_wb_servo_parameter_acs(HANDLE hAcsHandle)
{
	return mtnapi_dll_init_wb_servo_parameter_acs(hAcsHandle, &stControlProfile);
}

int _mtnapi_dll_init_servo_speed_para_acs(HANDLE hAcsHandle) 
{
	return mtnapi_dll_init_servo_control_para_acs(stServoControllerCommSet.Handle, 
				&stSpeedPara, &stControlProfile);
}

///////////////////////////////////// Download Parameter to WB structure Ctrl, Speed, in ACS
extern int mtn_wb_download_acs_servo_speed_parameter_acs(HANDLE hCommunicationHandle);

int mtn_wb_dll_download_acs_servo_speed_parameter_acs(HANDLE hCommunicationHandle)
{
	int iRet = MTN_API_OK_ZERO;
	mtnapi_dll_init_master_struct_ptr();
//	mtnapi_test_write_file_1();
	if(hCommunicationHandle != ACSC_INVALID)
	{
		hLocalDllInitAcsHandle = hCommunicationHandle;
	}
	else 
	{
		if((iRet =sys_init_acs_communication()) == MTN_API_OK_ZERO)
		{
			hLocalDllInitAcsHandle = stServoControllerCommSet.Handle;
			iFlagInitByLocalAcsComm = 1;
		}
	}

	// Initialize buffer program
	mtnapi_confirm_para_base_path_exist();  // 20110404, Protection to handle
	iRet = mtnapi_init_master_config_acs(strDefaultReadDownloadFilename_SpeedServoWb, hLocalDllInitAcsHandle);

	iRet = _mtnapi_dll_init_servo_speed_para_acs(hLocalDllInitAcsHandle);

//	Sleep(100);
	iRet = mtn_wb_download_acs_servo_speed_parameter_acs(hLocalDllInitAcsHandle);
	Sleep(100);
	if(iFlagInitByLocalAcsComm == 1)
	{
		acsc_CloseComm(stServoControllerCommSet.Handle);
		iFlagInitByLocalAcsComm = 0;  // Protection, 20110121
	}
	return iRet;
}

int mtnapi_confirm_para_base_path_exist()
{
	int iRet = MTN_API_OK_ZERO;
	mtnapi_dll_init_master_struct_ptr();
	iRet = _access(strDefaultSaveFilename_SpeedServoWb, 0);
	if(iRet)
	{
		iRet = mtnapi_create_para_base_master_template(); // create default
//		MTN_API_ERR_FILE_PTR;
	}

	return iRet;
}

// Servo Parameter Tuning, Motion between 2 points
static CWinThread* m_pWinThread_ServoTuning;
//extern int iFlagStopThread_ServoTuning;
int iFlagStopThread_ServoTuning = TRUE;
int mtn_dll_wb_servo_tune_get_stop_flag()
{
	return iFlagStopThread_ServoTuning;
}

void mtn_dll_wb_servo_tune_set_stop_flag(int iFlag)
{
	iFlagStopThread_ServoTuning = iFlag;
}

static 	char cFlagDlgParaServoTuningThreadRunning;
char mtn_wb_tune_thread_get_flag_running()
{
	return cFlagDlgParaServoTuningThreadRunning;
}
void mtn_wb_tune_thread_set_flag_running(char cFlag)
{
	cFlagDlgParaServoTuningThreadRunning = cFlag;
}

extern UINT uiBitComboFlag_ServoTuning;
extern UINT mtn_dll_wb_tune_servo_start_move_2points_thread( LPVOID pParam );

void mtn_dll_wb_tune_servo_trigger_move_2points_thread(UINT uiBitFlagComboTuningThread)
{
	if(iFlagStopThread_ServoTuning == TRUE)
	{
		// Set global variable flag
		iFlagStopThread_ServoTuning = FALSE;

		uiBitComboFlag_ServoTuning = uiBitFlagComboTuningThread;

		//
		m_pWinThread_ServoTuning = AfxBeginThread(mtn_dll_wb_tune_servo_start_move_2points_thread, 0);  // mtn_dll_wb_tune_servo_move_2points_thread
		SetPriorityClass(m_pWinThread_ServoTuning->m_hThread, REALTIME_PRIORITY_CLASS);

		m_pWinThread_ServoTuning->m_bAutoDelete = FALSE;
	}
}


int mtn_dll_wb_tune_servo_get_flag_stopping_thread()
{
	return iFlagStopThread_ServoTuning;
}
void mtn_dll_wb_tune_servo_stop_thread()
{
	if (m_pWinThread_ServoTuning)
	{
		iFlagStopThread_ServoTuning = TRUE;
		WaitForSingleObject(m_pWinThread_ServoTuning->m_hThread, 5000);
		// delete m_pWinThreadSpecTest;
		m_pWinThread_ServoTuning = NULL;
	}
}



#ifdef __DEBUG__
void mtnapi_test_write_file_1()
{
	fopen_s(&fpDebugServoIni, strDeBugServoWb, "w");

//	fprintf(fpDebugServoIni, "%% ACSC Controller, %s\n\n", strACSC_VarName);
	if(fpDebugServoIni != NULL)
	{
		fprintf(fpDebugServoIni, "%% Debug-1\n");
	} 
}
void mtnapi_test_write_file_2(HANDLE hAcsHandle)
{
	int iAxisTuningACS = 0;
	CTRL_PARA_ACS stAxisServoCtrlParaBak;
	mtnapi_upload_servo_parameter_acs_per_axis(hAcsHandle, iAxisTuningACS, &stAxisServoCtrlParaBak);
	if(fpDebugServoIni != NULL)
	{
		fprintf(fpDebugServoIni, "%%Z Servo-BLK0: %6.1f, %6.1f, %6.1f\n", 
			stServoAxis_ACS[2].stServoParaACS[0].dAccelerationFeedforward,
			stServoAxis_ACS[2].stServoParaACS[0].dVelocityLoopProportionalGain,
			stServoAxis_ACS[2].stServoParaACS[0].dVelocityLoopIntegratorGain);
		fprintf(fpDebugServoIni, "%%Z Servo-ACS: %6.1f, %6.1f, %6.1f\n", 
			stAxisServoCtrlParaBak.dAccelerationFeedforward,
			stAxisServoCtrlParaBak.dVelocityLoopProportionalGain,
			stAxisServoCtrlParaBak.dVelocityLoopIntegratorGain
			);

	}
}

void mtnapi_test_write_file_3(SERVO_MOTION_PARAMETER *pSpeedPara)
{
	if(fpDebugServoIni != NULL)
	{
		fprintf(fpDebugServoIni, "%%Z Motion-BLK0: %d, %d, %d\n", 
			pSpeedPara->stSpeedProfileZ[0].nMaxVel, pSpeedPara->stSpeedProfileZ[0].nMaxAcc, pSpeedPara->stSpeedProfileZ[0].lJerk);

		fprintf(fpDebugServoIni, "%%X Motion-BLK0: %d, %d, %d\n", 
			pSpeedPara->stSpeedProfileX[0].nMaxVel, pSpeedPara->stSpeedProfileX[0].nMaxAcc, pSpeedPara->stSpeedProfileX[0].lJerk);

		fprintf(fpDebugServoIni, "%%Y Motion-BLK0: %d, %d, %d\n", 
			pSpeedPara->stSpeedProfileY[0].nMaxVel, pSpeedPara->stSpeedProfileY[0].nMaxAcc, pSpeedPara->stSpeedProfileY[0].lJerk);

		fprintf(fpDebugServoIni, "%%Factors-Table-X-[V, A, J]: %6.8f, %6.8f, %6.8f\n", 
			dFactorTbl_X_Vel_Fr_cnt_sec_to_mmps, dFactorTbl_X_Acc_Fr_cnt_sec_to_si, dFactorTbl_X_Jerk_Fr_cnt_sec_to_si);

		fprintf(fpDebugServoIni, "%%Factors-Table-Y-[V, A, J]: %6.8f, %6.8f, %6.8f\n", 
			dFactorTbl_Y_Vel_Fr_cnt_sec_to_mmps, dFactorTbl_Y_Acc_Fr_cnt_sec_to_si, dFactorTbl_Y_Jerk_Fr_cnt_sec_to_si);

		fprintf(fpDebugServoIni, "%%Factors-Bond-Z-[V, A, J]: %6.8f, %6.8f, %6.8f\n", 
			dFactorBH_Z_Vel_Fr_cnt_sec_to_mmps, dFactorBH_Z_Acc_Fr_cnt_sec_to_si, dFactorBH_Z_Jerk_Fr_cnt_sec_to_si);

		fprintf(fpDebugServoIni, "%%EncRes_mm-[X, Y, Z]: %6.8f, %6.8f, %6.8f\n", 
			dTableX_EncRes_mm, dTableY_EncRes_mm, dBndHeadZ_EncRes_mm);

		fprintf(fpDebugServoIni, "%%Blk0-Speed-Bond-Z-[V, A, J]: %6.8f, %6.8f, %6.8f\n", 
			stServoACS.stpServoAxis_ACS[WB_AXIS_BOND_Z]->stSpeedProfile[0].dMaxVelocity, 
			stServoACS.stpServoAxis_ACS[WB_AXIS_BOND_Z]->stSpeedProfile[0].dMaxAcceleration, 
			stServoACS.stpServoAxis_ACS[WB_AXIS_BOND_Z]->stSpeedProfile[0].dMaxJerk);
		

	}
}
void mtnapi_test_close_file()
{
	if(fpDebugServoIni != NULL)
	{
		fclose(fpDebugServoIni);
	}
}
#endif // __DEBUG__