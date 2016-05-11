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
// Module: Motion Application Programming Interface
// 
// History
// YYYYMMDD  Author			Notes
// 20081001  Zhengyi		Created, with Position compensation & Scope function
// 20081101  ZhengyiJohn	Add control parameter data-file
// 20090105  ZhengyiJohn	Add stepper parameter
// 20090129  ZhengyiJohn	Add Relax position
// 20091115					Motion Tuning
#ifndef __MTN_API__
#define __MTN_API__

//#if _MSC_VER >= 1000
//#pragma once
//#endif // _MSC_VER >= 1000
//
//#ifndef __AFXWIN_H__
//	#error include 'stdafx.h' before including this file for PCH
//#endif

#include "ACSC.h"

#include "mtndefin.h"

#define HISTBUF_SIZE	10000
#define EDITBUF_SIZE	10000

#define MAX_CTRL_AXIS_PER_SERVO_BOARD  8

#define MAX_STEPPER_AXIX_ON_BOARD 4 // 20090105
#define MAX_STEPPER_CTRL_BOARD    8 // 20090105

typedef struct {
	int CommType;				// 0 - serial, 1 - ethernet, 2 - PCI bus, 3 -simulator
	HANDLE Handle;				// communication handle
	CString ConnectionString;	
	int SerialPort;	
	int SerialRate;
	CString EthernetAddress;
	int EthernetConnection;
	ACSC_PCI_SLOT PCISlot;	
} COMM_SETTINGS;

extern unsigned int uiMtnApi_Channel;

short acs_get_error_code_string(char * strErrorText, int iErrorCode);

#define ACS_OUT0_OR_SET_BIT0    0x1
#define ACS_OUT0_OR_SET_BIT1    0x2
#define ACS_OUT0_OR_SET_BIT2    0x4
#define ACS_OUT0_OR_SET_BIT3    0x8
#define ACS_OUT0_OR_SET_BIT4    0x10
#define ACS_OUT0_OR_SET_BIT5    0x20
#define ACS_OUT0_OR_SET_BIT6    0x40
#define ACS_OUT0_OR_SET_BIT7    0x80
#define ACS_OUT0_OR_SET_BIT8    0x100
#define ACS_OUT0_OR_SET_BIT9    0x200
#define ACS_OUT0_OR_SET_BIT10    0x400
#define ACS_OUT0_OR_SET_BIT11    0x800

#define ACS_OUT0_AND_CLEAR_BIT0    0xFFFFFFFE
#define ACS_OUT0_AND_CLEAR_BIT1    0xFFFFFFFD
#define ACS_OUT0_AND_CLEAR_BIT2    0xFFFFFFFB
#define ACS_OUT0_AND_CLEAR_BIT3    0xFFFFFFF7
#define ACS_OUT0_AND_CLEAR_BIT4    0xFFFFFFEF
#define ACS_OUT0_AND_CLEAR_BIT5    0xFFFFFFDF
#define ACS_OUT0_AND_CLEAR_BIT6    0xFFFFFFBF
#define ACS_OUT0_AND_CLEAR_BIT7    0xFFFFFF7F
#define ACS_OUT0_AND_CLEAR_BIT8    0xFFFFFEFF
#define ACS_OUT0_AND_CLEAR_BIT9    0xFFFFFDFF
#define ACS_OUT0_AND_CLEAR_BIT10    0xFFFFFBFF
#define ACS_OUT0_AND_CLEAR_BIT11    0xFFFFF7FF

////// Structure for config file, read and write
//#ifndef MTN_CFG_PRF_BLK
//typedef struct __config_profile_block
//{
//	char prf_blk_name[MTN_API_MAX_STRLEN_FEATURENAME];	/* expects string less than MTN_API_MAX_STRLEN_FEATURENAME characters */
//    long start_file_pos;
//    long end_file_pos;
//    short num_of_lines;
//}MTN_CFG_PRF_BLK;
//#endif // MTN_CFG_PRF_BLK



// BLOCK definition for ACS controller
typedef struct
{
	double dPositionLoopProportionalGain;
	double dVelocityLoopProportionalGain;
	double dVelocityLoopIntegratorGain;
	double dVelocityLoopIntegratorLimit; //	[percentage]
	double dSecondOrderLowPassFilterBandwidth;
	double dSecondOrderLowPassFilterDamping;
	double dNotchFilterFrequency;
	double dNotchFilterWidth;
	double dNotchFilterAttenuation;
	double dAccelerationFeedforward;
	double dDynamicFrictionFeedforward;
	//
	double dCurrentLoopOffset;   // 20110802, SLIOFFS
	double dVelocityFrictionPc;
	double dVelocityFrictionMax;
	double dPositionIntegralGain;
	double dPositionIntegralLimit;
	//int iConstantDriveCommand;
	//double dTorqueLimitIdle;				//[percentage]
	//double dTorqueLimitMoving;				//[percentage]
	//double dTorqueLimitRMS;				//[percentage]
	double dDRA;
	double dDRX;
	int iFlagEnableNotchFilter;
	int iFlagEnableSOF;

	double dJerkFf;

	double dBiquadNumFreq;
	double dBiquadDenFreq;
	double dBiquadNumDamp;
	double dBiquadDenDamp;
}CTRL_PARA_ACS;

typedef struct
{
	double dPosnErrIdle;
	double dPosnErrVel;
	double dPosnErrAcc;
	double dCriticalPosnErrIdle;
	double dCriticalPosnErrVel;
	double dCriticalPosnErrAcc;
	double dSoftwarePosnLimitLow;   // LLIMIT
	double dSoftwarePosnLimitUpp;	// RLIMIT
	double dMaxVelX;
	double dMaxAccX;
	double dDynamicBrakeThresholdVel;
	double dRMS_DrvCmdX;
	double dRMS_TimeConst;
	double dRMS_DrvCmdIdle;
	double dRMS_DrvCmdMtn;
}SAFETY_PARA_ACS;

typedef struct
{
	int iMotorFlags;
	int iAxisFlags;
	int iEncoderType;
	int iEncoderFreq;
	double dEncoderFactor;
	double dEnableTime;
	double dBrakeOffTime;
	double dBrakeOnTime;
	double dSettlingTime;
	double dTargetRadix;
}BASIC_PARA_ACS_AXIS;

#define NUM_TOTAL_PROGRAM_BUFFER  10
typedef struct
{
	int aiProgramFlags[NUM_TOTAL_PROGRAM_BUFFER];
	int aiProgramRate[NUM_TOTAL_PROGRAM_BUFFER];
	int aiProgramAutoRoutineRate[NUM_TOTAL_PROGRAM_BUFFER];
}BASIC_PARA_ACS_GLOBAL;

// maximum number of parameter blocks for either servo parameter or motion parameter

#define MAX_BLK_PARAMETER    30
// (X_MOTOR_CONTROL_PROFILE_NUM), #include "MTN_WB_INTERFACE.h"

// Default block-id
// For such controller that there is a single block of parameter
#define DEF_BLK_UPLOAD_PARA_FROM_CTRL   0
#define DEF_BLK_DOWNLOAD_PARA_TO_CTRL   0
// for special applications
#define DEF_BLK_ID_SERVO_PARA_Z_SEARCH_HOME 7
#define DEF_BLK_ID_SERVO_PARA_Z_BONDING     0


typedef struct
{
	CTRL_PARA_ACS		stServoParaACS[MAX_BLK_PARAMETER];            // 20090508
	MTN_SPEED_PROFILE	stSpeedProfile[MAX_BLK_PARAMETER];
	MTN_POSITION_REGISTRATION  stServoPositionReg; // 20090129
	MTUNE_OUT_POSN_COMPENSATION stPosnCompensationWbBH;			// 20110721
	char strApplicationNameAxis[MTN_API_MAX_STRLEN_FEATURENAME];
	char strAxisNameAcs[MTN_API_MAX_STRLEN_FEATURENAME];
	char strAxisServoParaFilename[MTN_API_MAX_STRLEN_FILENAME];
	char strAxisMtnServoTune[MTN_API_MAX_STRLEN_FILENAME];		// 20091115
	unsigned int uiAxisOnACS;

}SERVO_AXIS_BLK;

// Add for stepper axis, 20090105
typedef struct
{
	MTN_POSITION_REGISTRATION  *stpStepperPositionReg;
	MTN_SPEED_PROFILE	*stpStepperSpeedProfile;
	char strApplicationNameAxis[MTN_API_MAX_STRLEN_FEATURENAME];
	char strAxisParameterFilename[MTN_API_MAX_STRLEN_FILENAME];
	unsigned int uiAxisOnBoard;
	unsigned int uiBoardId;

}STEPPER_AXIS_BLK;

typedef struct
{
	HANDLE Handle;
	unsigned int uiTotalNumAxis;
	char strFilename[MTN_API_MAX_STRLEN_FILENAME];
	char strCtrlBoardName[MTN_API_MAX_STRLEN_FEATURENAME];
	SERVO_AXIS_BLK *stpServoAxis_ACS[MAX_CTRL_AXIS_PER_SERVO_BOARD];
	SAFETY_PARA_ACS *stpSafetyAxis_ACS[MAX_CTRL_AXIS_PER_SERVO_BOARD];
	BASIC_PARA_ACS_AXIS *stpBasicAxis_ACS[MAX_CTRL_AXIS_PER_SERVO_BOARD];
	STEPPER_AXIS_BLK *stpStepperAxis_ADS[MAX_STEPPER_CTRL_BOARD];  // 20090105 , MAX_STEPPER_AXIX_ON_BOARD
	BASIC_PARA_ACS_GLOBAL stGlobalParaACS;

}SERVO_ACS;


typedef struct {
	int iFactor;
	int iGain;
}ACS_SP_VAR_FL;
#define ACS_SP_BIG_INT   8388607
// 2^23 - 1

#define GRAPH_DEF_800_PIXEL  1280
#define GRAPH_DEF_600_PIXEL  768
// Global variable, declared in MtnScope.cpp
extern MTN_SCOPE gstSystemScope;
extern char gstrScopeDataVarName[];
extern char gstrScopeArrayName[];
extern double gdScopeCollectData[];
extern char strACSC_VarName[];
// Scope
void mtnscope_declare_var_on_controller(HANDLE Handle);
void mtnscope_set_acsc_var_collecting_posncompn(int iAxis);
void mtnscope_set_acsc_var_collecting_move(int iAxis);
void mtnscope_set_acsc_var_collecting_velloop_test(int iAxis); // 20100328
void mtnscope_set_acsc_var_collecting_searchindex(int iAxis); // 20090119
double mtnscope_FindMaxInScopeData(int idxData);
double mtnscope_FindMinInScopeData(int idxData);
double mtnscope_FindMaxInSpecScopeData(int idxData, MTN_SCOPE *stpScope); // 20120522
double mtnscope_FindMinInSpecScopeData(int idxData, MTN_SCOPE *stpScope); 
int mtnscope_upload_acsc_data_userdef(HANDLE Handle, MTN_SCOPE *stpScope); // 20120522

int mtnscope_upload_acsc_data(HANDLE Handle);
int mtnscope_tuning_upload_acsc_data(HANDLE Handle); // Dedicated for tuning
void mtnscope_tuning_declare_var_on_controller(HANDLE Handle);

int mtnscope_upload_acsc_data_varname(HANDLE Handle, int iAxis, char * strVarName);
short mtn_save_curr_axis_traj_data_to_file(unsigned int uiAxisCtrlCard);
short mtn_save_curr_axis_traj_data_to_filename(char *strFilename, unsigned int uiAxisCtrlCard);

int mtnapi_wait_axis_settling(HANDLE Handle, int iAxis, double dPosnErrSettleTH);
//
int mtn_acs_wait_axis_settle(HANDLE m_hHandle, int iAcsAxis);

// from Algo Library
// Square matrix inverse, input a: 
int brinv(double a[],int n); // return -  1: OK, 0: error
// Matrix multiplication a * b = c, a: m by n, b: n by k, c: m by k
void brmul(double *a, double *b, int m, int n, int k, double *c);

// Buffer program
short mtn_api_init_acs_buffer_prog();
short mtn_api_clear_acs_buffer_prof();

// mtn_api_* has faster interface
void mtn_api_get_position_err(HANDLE mHandle, int iAxis, double *pdPositionErrorFb);
void mtn_api_get_drv_cmd(HANDLE mHandle, int iAxis, double *pdDrvCmdPc); // 20120927

// mtnapi_* has slower but more standard interface defined in local 
int mtnapi_get_fb_position(HANDLE Handle, int Axis, double* FPosition, int iDebug);
int mtnapi_get_fb_velocity(HANDLE Handle, int Axis, double* pdlbFbVelocity, int iDebug);
int mtnapi_get_ref_velocity(HANDLE Handle, int Axis, double* pdlbRefVelocity, int iDebug);
int mtnapi_get_ref_position(HANDLE Handle, int Axis, double* pdlbRefPosition, int iDebug);

int mtnapi_get_motor_state(HANDLE Handle, int Axis, int* iState, int iDebug);
int mtnapi_enable_motor(HANDLE Handle, int Axis, int iDebug);
int mtnapi_disable_motor(HANDLE Handle, int Axis, int iDebug);

int mtnapi_enable_motor_current_mode(HANDLE Handle, int iAxisACS);
int mtnapi_disable_motor_current_mode(HANDLE Handle, int iAxisACS);

int mtnapi_get_speed_profile(HANDLE Handle, int Axis, MTN_SPEED_PROFILE* stpSpeedProfile, int iDebug);
int mtnapi_set_speed_profile(HANDLE Handle, int Axis, MTN_SPEED_PROFILE* stpSpeedProfile, int iDebug);

int mtnapi_axis_acs_move_to(HANDLE Handle, int iAxis, int iPosnTo);
int mtnapi_axis_profile_move_to(HANDLE Handle, int iAxis, MTN_SPEED_PROFILE* stpSpeedProfile, int iPosnTo);

void mtn_api_get_position_upp_lmt(HANDLE mHandle, int iAxis, double *pdPositionUppLmt);
void mtn_api_get_position_low_lmt(HANDLE mHandle, int iAxis, double *pdPositionLowLmt);

void mtn_api_set_position_upp_lmt(HANDLE mHandle, int iAxis, double dPositionUppLmt);
void mtn_api_set_position_low_lmt(HANDLE mHandle, int iAxis, double dPositionLowLmt);

void mtn_api_get_motor_flag(HANDLE mHandle, int iAxis, int *piMotorFlag);
int mtn_api_get_flag_inv_drv_polarity(HANDLE mHandle, int iAxis);

BOOL mtn_qc_is_axis_still_moving(HANDLE hCommunicationHandle, int iAxis);
BOOL mtn_qc_is_axis_locked_safe(HANDLE hCommunicationHandle, int iAxis);

void acs_cvt_float_to_24b_int_gf(double fInput, ACS_SP_VAR_FL *stpOutput);
void acs_cvt_24b_int_gf_to_float(ACS_SP_VAR_FL *stpInput, double *dOutput);

//#ifdef MOTALGO_DLL_EXPORTS
//// Tuning
int mtune_position_compensation(HANDLE Handle, int iAxis, MTN_TUNE_POSN_COMPENSATION *stpPosnCompensationTune, MTUNE_OUT_POSN_COMPENSATION *stpOutputPosnCompensationTune);
int mtn_api_set_spring_compensation_sp_para(HANDLE Handle, MTUNE_OUT_POSN_COMPENSATION *stpOutputPosnCompensationTune, FILE *fpData);
int mtn_api_get_spring_compensation_sp_para(HANDLE Handle, MTUNE_OUT_POSN_COMPENSATION *stpOutputPosnCompensationTune);
int mtn_api_disable_spring_compensation_sp_para(HANDLE Handle);
int mtn_api_enable_spring_compensation_sp_para(HANDLE Handle);

void mtn_tune_init_position_compensation_condition();
void mtn_tune_contact_save_data(HANDLE m_hHandle, MTN_SCOPE *pstSystemScope, int uiAxisAcs, char *strFilename, char *strCommentTitleInFirstLine);
void mtn_tune_make_output_folder(int iWbAxis);
int mtn_tune_save_servo_parameter_b1w_folder(HANDLE hAcsHandle);
int mtn_tune_save_servo_parameter_xy_folder(HANDLE hAcsHandle);

/// Position Calibration by thread
void mtn_thread_trigger_start_position_compensation();
void mtn_tune_stop_position_compensation_thread();
int mtn_tune_get_flag_stopping_position_compensation_thread();

void mtn_api_prompt_acs_error_code(HANDLE stCommHandleAcs);


int acs_read_srch_contact_buffer_auto_tune_flag(HANDLE stCommHandleAcs, int *piFlagAutoTune);
int acs_write_srch_contact_buffer_auto_tune_flag(HANDLE stCommHandleAcs, int iFlagAutoTune);
int mtn_run_srch_contact_b1w(HANDLE m_hHandle);

int aft_acs_routine_check_force_feedback_direction(int iAcsAxis);

//#endif
/// Teach Limit by 
short aft_teach_upper_limit(HANDLE hHandle, int iAxisOnACS, int iDebug, double *dRetUppLimitPosn);
short aft_teach_lower_limit(HANDLE hHandle, int iAxisOnACS, int iDebug, double *dRetLowLimitPosn);

void mtnapi_set_bond_z_acs_axis(HANDLE stCommHandleACS);

// #include "ACS_Efsika_app.h"

#endif // __MTN_API__