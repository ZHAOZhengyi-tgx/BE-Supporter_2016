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

#pragma once

// History
// YYYYMMDD  Author			Notes
// 20090129  Zhengyi		Add relaxed position
// 20090321  Zhengyi		Add notes
// 20090508  Zhengyi        Add multiple blocks for parameter
#include "mtnapi.h"

#define SYS_MAX_ACS_CARDS_PCI  16


//class CMotionServo: public CObject
//{
//private:

void mtnapi_static_lib_init_all();

//public:
void mtnapi_init_def_servo_acs();
int mtnapi_init_servo_control_para_acs(char *strFilename, HANDLE Handle) ;
	// Init default parameter to recover
int mtnapi_init_wb_def_servo_control_para_acs(HANDLE Handle);

int mtnapi_download_bonder_z_servo_para_search_home();
// 20100609
int mtn_tune_upload_wb_servo_ctrl(HANDLE hCommunicationHandle);
int mtn_tune_upload_wb_speed_profile(HANDLE hCommunicationHandle);
int mtnapi_upload_wb_servo_speed_parameter_acs(HANDLE hCommunicationHandle);

	// controller Ts
	void sys_init_controller_ts(double dTs_ms);
	double sys_get_controller_ts();

	void sys_set_comm_default();
	int sys_init_acs_communication();
	void sys_set_ctrl_communication_type(int iCommuniType);
	int sys_get_ctrl_communication_type();


	void mtnapi_init_master_struct_ptr();
	int mtnapi_init_master_config_acs(char *strFilename, HANDLE Handle);



	// Save all parameters{Servo, MotionDefault, ...}, for all axes
	int mtnapi_save_servo_parameter_acs();
	int mtnapi_save_master_cfg_file();
	// Save all parameters for one axis,
	int mtnapi_save_servo_parameter_acs_per_axis(unsigned int uiAxis);
	int mtnapi_save_servo_parameter_acs_per_axis_no_label(unsigned int uiAxis);
	int mtnapi_rename_bakup_servo_parameter_acs_per_axis(unsigned int uiAxis);

	int mtnapi_save_acs_para_into_def_xls_file();
	int mtnapi_write_acs_para_into_xls_file_ptr();

	// initialize all axis, initialize the Host PC's static variable
	// load servo parameter only, 
	int mtnapi_init_servo_parameter_from_file();
	// load motion parameter only,
	int mtnapi_init_speed_parameter_from_file();
	// load position registration parameters only
	int mtnapi_init_position_reg_from_file(); // 20090129
	int mtnapi_init_all_axis_parameter_from_file_no_label();

	// Safety and Off servo
	void mtn_api_servo_go_to_safety_position(HANDLE stCommHandle);
	void mtn_api_servo_off_all(HANDLE stCommHandle);

	// initialize only one axis
	int mtnapi_init_servo_axis_para_acs(SERVO_AXIS_BLK *stpServoAxis_ACS);
	// initialize only one axis
	int mtnapi_init_speed_para_axis_acs(SERVO_AXIS_BLK *stpServoAxis_ACS);

	// SP Variables
	int mtnapi_download_acs_sp_parameter_jerk_ff(HANDLE Handle, int iAxis, double dJerkFf);
	int mtnapi_download_acs_sp_parameter_k_x_from_a(HANDLE Handle, double dDecoupleK_x_From_a);
	int mtnapi_download_acs_sp_parameter_k_a_from_x(HANDLE Handle, double dDecoupleK_a_From_x);

	// Upload SP
	int mtnapi_upload_acs_sp_parameter_decouple_x_from_a(HANDLE Handle, double *pdDecoupleX_from_A); //
	int mtnapi_upload_acs_sp_parameter_decouple_a_from_x(HANDLE Handle, double *pdDecoupleA_from_X); // 

	// upload the current parameter from controller card volatile memory, overwrite HostPC's static variable
	int mtnapi_upload_servo_parameter_acs(HANDLE strHandle);
	int mtnapi_upload_servo_parameter_acs_v60_per_axis(HANDLE Handle, int iAxis, CTRL_PARA_ACS *stpServoPara);
	int mtnapi_upload_servo_parameter_acs_per_axis(HANDLE Handle, int iAxis, CTRL_PARA_ACS *stpServoPara);
	int mtnapi_upload_safety_parameter_acs_per_axis(HANDLE Handle, int iAxis, SAFETY_PARA_ACS *stpSafetyPara);
	int mtnapi_upload_basic_parameter_acs_per_axis(HANDLE Handle, int iAxis, BASIC_PARA_ACS_AXIS *stpBasicParaAxisACS);
	int mtnapi_upload_global_parameter_acs_per_axis(HANDLE Handle, BASIC_PARA_ACS_GLOBAL *stpProgramParaAxisACS);

	// 20090129
	int mtnapi_upload_position_reg_acs_per_axis(HANDLE Handle, int iAxis, MTN_POSITION_REGISTRATION *stpRegPosition);
	int mtn_api_get_relax_position_by_acs_axis(int iAxis, double *RelaxPosn);

	int mtnapi_download_acs_sp_parameter_damp_switch(HANDLE Handle, int iDampGA);
	int mtnapi_upload_acs_sp_parameter_damp_switch(HANDLE Handle, int *iDampGA);

	// download to controller card volatile memory, overwrite current one, not yet saved to flash
	// download single axis parameters, 
	int mtnapi_download_servo_parameter_acs_per_axis(HANDLE Handle, int iAxis, CTRL_PARA_ACS *stpServoPara);
	int mtnapi_download_servo_parameter_acs_v60_per_axis(HANDLE Handle, int iAxis, CTRL_PARA_ACS *stpServoPara);
	int mtnapi_download_safety_parameter_acs_per_axis(HANDLE Handle, int iAxis, SAFETY_PARA_ACS *stpSafetyPara);
	int mtnapi_download_position_reg_acs_per_axis(HANDLE Handle, int iAxis, MTN_POSITION_REGISTRATION *stpRegPosition);
	int mtnapi_download_parameter_acs_axis(HANDLE stHandle, int iAxis, SERVO_AXIS_BLK *stpServoAxisBlk);

	// download all axis's control parameters
	int mtnapi_download_servo_control_para_all_axis_acs(); // only download servo control parameters for all axis

	// download all parameters for all axis
	int mtnapi_download_servo_all_axis_acs(); // download all parameters [1] servo control, [2] speed, [3] posn_reg
//}

// WB interface
#include "MtnTune.h"
extern void mtn_b1w_write_para_bh_servo(HANDLE stCommHandle, MTN_TUNE_PARAMETER_SET *stpPara, int iObjSectionFlagB1W);
extern void mtn_b1w_read_para_bh_servo(HANDLE stCommHandle, MTN_TUNE_PARAMETER_SET *stpPara, int iObjSectionFlagB1W);
extern int mtn_b1w_write_para_bh_servo_readve(HANDLE stCommHandle, MTN_TUNE_PARAMETER_SET *stpPara, int iObjSectionFlagB1W);

extern SERVO_AXIS_BLK stServoAxis_ACS[];
extern SERVO_ACS stServoACS;
extern SAFETY_PARA_ACS stSafetyParaAxis_ACS[];
extern BASIC_PARA_ACS_AXIS stBasicParaAxis_ACS[];

