// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MOTALGO_DLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MOTALGO_DLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MOTALGO_DLL_EXPORTS
#define MOTALGO_DLL_API __declspec(dllexport)
#else
#define MOTALGO_DLL_API __declspec(dllimport)
#endif

// This class is exported from the MotAlgo_DLL.dll
class MOTALGO_DLL_API CMotAlgo_DLL {
public:
	CMotAlgo_DLL(void);
	// TODO: add your methods here.
};

// Interface btw MotAlgo_DLL and WireBonder
#include "MTN_WB_INTERFACE.h"

#include "Version.h"


//////////// Version Control
extern MOTALGO_DLL_API int mtnapi_dll_init_get_version(VERSION_INFO *stpVersion);
extern MOTALGO_DLL_API char sys_acs_communication_get_flag_sc_udi();
extern MOTALGO_DLL_API void sys_acs_communication_set_flag_sc_udi(char cTempFlag);
extern MOTALGO_DLL_API char sys_get_acs_communication_flag_udi();
extern MOTALGO_DLL_API char sys_get_acs_axis_id_bnd_z();
extern MOTALGO_DLL_API char sys_get_acs_axis_id_wire_clamp();

extern MOTALGO_DLL_API int mtnapi_dll_init_servo_control_para_acs(HANDLE hAcsHandle, SERVO_MOTION_PARAMETER *pSpeedPara, SERVO_CONTROL_PARAMETER *pControlProfile);
extern MOTALGO_DLL_API int mtnapi_dll_init_wb_speed_parameter_acs(HANDLE hAcsHandle, SERVO_MOTION_PARAMETER *pSpeedPara);
extern MOTALGO_DLL_API int mtnapi_dll_init_wb_servo_parameter_acs(HANDLE hAcsHandle, SERVO_CONTROL_PARAMETER *pControlProfile);
extern MOTALGO_DLL_API int mtnapi_dll_save_wb_speed_servo_parameter_acs(HANDLE hAcsHandle);

extern MOTALGO_DLL_API int _mtnapi_dll_init_wb_servo_parameter_acs(HANDLE hAcsHandle);
extern MOTALGO_DLL_API int _mtnapi_dll_init_servo_speed_para_acs(HANDLE hAcsHandle);
extern MOTALGO_DLL_API void mtnapi_dll_init_master_struct_ptr();

extern MOTALGO_DLL_API int mtn_wb_dll_download_acs_servo_speed_parameter_acs(HANDLE hCommunicationHandle);

// Servo Tuning
#define WB_SERVO_TUNING_BIT_TUNE_2POINTS_X    1
#define WB_SERVO_TUNING_BIT_TUNE_2POINTS_Y    2
#define WB_SERVO_TUNING_BIT_TUNE_2POINTS_Z    4
#define WB_SERVO_TUNING_BIT_TUNE_B1W_Z        0x10
#define WB_SERVO_TUNING_BIT_TUNE_SECTOR_XY    0x20

extern MOTALGO_DLL_API void mtn_dll_wb_tune_servo_trigger_move_2points_thread(UINT uiBitFlagComboTuningThread);
extern MOTALGO_DLL_API int mtn_dll_wb_tune_servo_get_flag_stopping_thread();
extern MOTALGO_DLL_API void mtn_dll_wb_tune_servo_stop_thread();
extern MOTALGO_DLL_API int mtn_dll_wb_servo_tune_get_stop_flag();
extern MOTALGO_DLL_API void mtn_dll_wb_servo_tune_set_stop_flag(int iFlag);

extern MOTALGO_DLL_API char mtn_wb_tune_thread_get_flag_running();
extern MOTALGO_DLL_API void mtn_wb_tune_thread_set_flag_running(char cFlag);

// Search Home, safety position
extern MOTALGO_DLL_API void mtn_dll_init_def_para_search_index_vled_bonder_xyz(int iAxisX, int iAxisY, int iAxisZ);
extern MOTALGO_DLL_API void mtn_dll_init_def_para_search_index_hori_bonder_xyz(int iAxisX, int iAxisY, int iAxisZ);
extern MOTALGO_DLL_API void mtn_dll_init_def_para_search_index_13v_vled_bonder_xyz(int iAxisX, int iAxisY, int iAxisZ);
extern MOTALGO_DLL_API void mtn_dll_init_def_para_search_index_18v_bonder_xyz(int iAxisX, int iAxisY, int iAxisZ);

extern MOTALGO_DLL_API int mtn_dll_bonder_xyz_start_search_limit_go_home(HANDLE Handle, int iAxisX, int iAxisY, int iAxisZ);
extern MOTALGO_DLL_API int mtn_wb_table_servo_go_to_safety_position(HANDLE stCommHandle, int iAxisACS_TableX, int iAxisACS_TableY);

// machine type 
extern MOTALGO_DLL_API int get_sys_machine_type_flag();
// machine serial number, 20111013
extern MOTALGO_DLL_API unsigned int get_sys_machine_serial_num();
extern MOTALGO_DLL_API int set_sys_machine_serial_num(unsigned int uiMachineSn);

extern MOTALGO_DLL_API int mtn_api_load_machine_config(int *piMachCfg);
extern MOTALGO_DLL_API void mtn_wb_dll_set_sys_machine_type(int iMachCfg);
extern MOTALGO_DLL_API void mtnapi_dll_init_all();
extern MOTALGO_DLL_API double mtn_wb_get_bh_upper_limit_position();
extern MOTALGO_DLL_API double mtn_wb_get_bh_relax_position();
extern MOTALGO_DLL_API double mtn_wb_init_bh_relax_position_from_sp(HANDLE Handle);

// Music LOGO
extern MOTALGO_DLL_API void mtn_dll_music_logo_normal_start();
extern MOTALGO_DLL_API void mtn_dll_music_logo_normal_stop();

// From ACS axis to app-axis 
extern MOTALGO_DLL_API int aft_get_mechatr_axis_from_ctrl_axis(int iSysAnaAxis_CtrlCardAx, int iMechTronicsType);

// #include "MtnApi.h"
// position compensation
//extern MOTALGO_DLL_API int mtune_position_compensation(HANDLE Handle, int iAxis, MTN_TUNE_POSN_COMPENSATION *stpPosnCompensationTune, MTUNE_OUT_POSN_COMPENSATION *stpOutputPosnCompensationTune);
//extern MOTALGO_DLL_API int mtn_api_set_spring_ompensation_sp_para(HANDLE Handle, MTUNE_OUT_POSN_COMPENSATION *stpOutputPosnCompensationTune, FILE *fpData);


//////// more 
extern MOTALGO_DLL_API int nMotAlgo_DLL;

extern MOTALGO_DLL_API void mtn_dll_init_home_set_x_table_offset_startbond(double dOffsetTableX);
extern MOTALGO_DLL_API double mtn_dll_init_home_get_x_table_offset_startbond();
extern MOTALGO_DLL_API void mtn_dll_init_home_set_y_table_offset_startbond(double dOffsetTableY);
extern MOTALGO_DLL_API double mtn_dll_init_home_get_y_table_offset_startbond();
extern MOTALGO_DLL_API void mtn_dll_init_home_set_z_bondhead_firelevel(double dBondHeadFireLevel);
extern MOTALGO_DLL_API double mtn_dll_init_home_get_z_bondhead_firelevel();

extern MOTALGO_DLL_API int mtn_dll_axis_search_limit(HANDLE Handle, int iAxis);
extern MOTALGO_DLL_API int mtn_dll_axis_go_home_from_limit(HANDLE Handle, int iAxis);
extern MOTALGO_DLL_API int mtn_dll_axis_search_limit_go_home(HANDLE Handle, int iAxis);

extern MOTALGO_DLL_API void mtn_wb_bakup_software_limit(HANDLE Handle, int iAxisX, int iAxisY, int iAxisZ);
extern MOTALGO_DLL_API void mtn_wb_restore_software_limit(HANDLE Handle, int iAxisX, int iAxisY, int iAxisZ);
extern MOTALGO_DLL_API void mtn_wb_enlarge_software_limit_by_(HANDLE Handle, int iAxisX, int iAxisY, int iAxisZ, double dFactor);

extern MOTALGO_DLL_API void mtn_wb_bakup_servo_protect_limit(HANDLE Handle, int iIdAcsAxisX, int iIdAcsAxisY, int iIdAcsAxisZ);
extern MOTALGO_DLL_API void mtn_wb_enlarge_servo_protect_limit(HANDLE Handle, int iIdAcsAxisX, int iIdAcsAxisY, int iIdAcsAxisZ, double dEnlargeFactor);
extern MOTALGO_DLL_API void mtn_wb_restore_servo_protect_limit(HANDLE Handle, int iIdAcsAxisX, int iIdAcsAxisY, int iIdAcsAxisZ);

extern MOTALGO_DLL_API void mtn_dll_trigger_axis_home_thread();
extern MOTALGO_DLL_API int mtn_dll_home_get_flag_stopping_thread();
extern MOTALGO_DLL_API void mtn_dll_home_stop_thread();
extern MOTALGO_DLL_API void mtn_dll_home_set_current_axis(int iAxis);
extern MOTALGO_DLL_API void mtn_dll_home_set_acs_ctrl_handle(HANDLE hHandle);
extern MOTALGO_DLL_API int mtn_dll_home_get_flag_stopping_thread();
extern MOTALGO_DLL_API void mtn_dll_trigger_wb_xyz_home_thread();

MOTALGO_DLL_API int fnMotAlgo_DLL(void);
