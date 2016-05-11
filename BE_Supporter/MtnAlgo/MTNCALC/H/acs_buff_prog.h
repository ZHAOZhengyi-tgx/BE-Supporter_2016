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



// acs_buff_prog.h

#ifndef __ACS_BUFF_PROG__
#define __ACS_BUFF_PROG__

typedef enum
{
	DPRAM_ADDR_PATH_LEN_BUF_8          = 0x300, // Trajectory length buffer 8
    DPRAM_ADDR_START_MOVE_BUF_8        = 0x304, //Trigger buffer 8 start moving
	DPRAM_ADDR_PATH_LEN_BUF_9          = 0x308, // Trajectory length buffer 9
    DPRAM_ADDR_START_MOVE_BUF_9        = 0x30C, //Trigger buffer 9 start moving
	DPRAM_ADDR_DONE_MOTION_BUF_8       = 0x310, // 1: Done motion, 0: doing
    DPRAM_ADDR_DONE_MOTION_BUF_9       = 0x314, // 1: Done motion, 0: doing
};

#define BUFFER_ID_SEARCH_CONTACT	6     // 20120201
#define BUFFER_ID_AFT_SPECTRUM_TEST    8
#define BUFFER_ID_VEL_TEST_PROF_MOVE_2 9
#define BUFFER_ID_POSITIVE_MOVE 8
#define BUFFER_ID_NEGATIVE_MOVE 9
#define BUFFER_ID_EMU_B1W		8

typedef enum
{
	DPRAM_ADDR_START_BUF_7          = 0x330, // Trigger buffer 7 start searching 
	DPRAM_ADDR_DONE_SEARCH_CONTACT  = 0x334, // 
};
short acs_buff_prog_init_comm(HANDLE stCommHandle);
/// Search Contact and AutoTune-Z
short acs_compile_buffer_prog_srch_contact_f_cali_auto_tune_z();
short acs_init_buffer_prog_srch_contact_f_cali_auto_tune_z();
short acs_run_buffer_prog_srch_contact_f_cali_auto_tune_z();
short acs_stop_buffer_prog_srch_contact_f_cali_auto_tune_z();
short acs_clear_buffer_prog_srch_contact_f_cali_auto_tune_z();

int acs_set_search_height_posn(int iSrchHt);
int acs_set_search_velocity(int iSrchVel);
int acs_set_search_stop_position(double dSearchStopPosn);
int acs_set_search_tune_bnd_z_tail_dist(int iTailDist);
int acs_set_search_tune_bnd_z_reset_posn(int iResetPosn);
int acs_set_search_tune_bnd_z_reverse_height(int iRevHt);
int acs_set_search_tune_bnd_z_kink_height(int iKinkHt);
int acs_set_search_tune_bnd_z_loop_top(int iLoopTop);
//// Emergent PowerSupply Lost, check IN0.3
int acs_enable_check_in3_flag();
int acs_disable_check_in3_flag();
//// Check RMS
int acs_enable_check_rms_flag();
int acs_disable_check_rms_flag();
short acs_run_buffer_prog_rms_emerg_stop();

/// PRBS in current loop
short acs_init_buffer_prog_prbs_prof_cfg_move();
short acs_clear_buffer_prog_prbs_prof_cfg_move();
short acs_compile_buffer_prog_prbs_prof_cfg_move();
short acs_run_buffer_prog_prbs_prof_cfg_move();
short acs_stop_buffer_prog_prbs_prof_cfg_move();

/// PRBS in velocity loop
short acs_init_buffer_prog_vel_loop_prof_cfg_move2();
short acs_clear_buffer_prog_vel_loop_prof_cfg_move2();
short acs_compile_buffer_prog_vel_loop_prof_cfg_move2();
short acs_run_buffer_prog_vel_loop_prof_cfg_move2();
short acs_stop_buffer_prog_vel_loop_prof_cfg_move2();

short acs_init_buff_prog_search_contact_force_control();
short acs_clear_buffer_prog_search_contact_force_control();

int acs_bufprog_write_motion_flag_buff_8(int iFlag);
int acs_bufprog_start_buff_8();
int acs_bufprog_get_start_flag_buff_8(int *piFlagStartMotion);
int acs_bufprog_write_traj_len_buff_8(int iTrajLen);
int acs_bufprog_download_vel_prof_buff_8(int idxMatCol, int nTrajSize, double *pdVelProf);

int acs_bufprog_start_buff_9();
int acs_bufprog_get_start_flag_buff_9(int *piFlagStartMotion);
int acs_bufprog_write_traj_len_buff_9(int iTrajLen);
int acs_bufprog_download_vel_prof_buff_9(int idxMatCol, int nTrajSize, double *pdVelProf);


int acs_bufprog_write_traj_move_time_unit_ms(double dCmdGenPeriod_ms);

#define ACS_BUFPROG_MTN_FLAG_OPEN_LOOP    0
#define ACS_BUFPROG_MTN_FLAG_CLOSE_LOOP   1
#define ACS_BUFPROG_MTN_FLAG_EMU_B1W      2
#define ACS_BUFPROG_MTN_FLAG_VELOOP_STEP  3

// Buffer Program Manager
#define MAX_NUM_ACS_BUFFERS  11
#define MAX_CHAR_PER_LINE_ACS_BUFF   512
#define MAX_NUM_BYTES_PER_BUFFER     (ACSC_MAX_LINE * MAX_CHAR_PER_LINE_ACS_BUFF)

int acs_buffprog_upload_prog_get_line(unsigned int idxBuffer);
int acs_buffprog_get_num_lines_at_buff(unsigned int idxBuffer);
int acs_buffprog_get_num_bytes_at_buff(unsigned int idxBuffer);


#define MTN_API_ERR_START_FIRMWARE_PROG   4
#define MTN_API_ERR_STOP_FIRMWARE_PROG     8
#define MTN_API_ERR_CLEAR_FIRMWARE_PROG   9
#define MTN_API_ERR_SAVE_ACS_FLASH        10

#define BUFFER_DATA_WB_OFST_XY_SPEED_PROFILE_MOVE_TO_1ST_B   0
#define BUFFER_DATA_WB_OFST_X_CTRL_MOVE_TO_1ST_B   1
#define BUFFER_DATA_WB_OFST_Y_CTRL_MOVE_TO_1ST_B   2
#define BUFFER_DATA_WB_OFST_X_POSN_1ST_B   3
#define BUFFER_DATA_WB_OFST_Y_POSN_1ST_B   4
#define BUFFER_DATA_WB_OFST_XY_REMAIN_TIME_START_Z_1ST_B   5
#define BUFFER_DATA_WB_OFST_Z_SRCH_HEIGHT_1ST_B    6
#define BUFFER_DATA_WB_OFST_Z_SRCH_VEL_1ST_B       7
#define BUFFER_DATA_WB_OFST_Z_SRCH_THRESHOLD_PE_1ST_B    8
#define BUFFER_DATA_WB_OFST_Z_SRCH_PROT_FD_POSN_1ST_B    9
#define BUFFER_DATA_WB_OFST_Z_SRCH_PRE_CONTACT_FORCE_DCOM_1ST_B       10
#define BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_1_TIME_1ST_B   11
#define BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_1_DCOM_1ST_B   12
#define BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_2_TIME_1ST_B   13
#define BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_2_DCOM_1ST_B   14
#define BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_3_TIME_1ST_B   15
#define BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_3_DCOM_1ST_B   16
#define BUFFER_DATA_WB_OFST_Z_REVERSE_HEIGHT   17
#define BUFFER_DATA_WB_OFST_X_REVERSE_DIST     18
#define BUFFER_DATA_WB_OFST_Y_REVERSE_DIST     19
#define BUFFER_DATA_WB_OFST_Z_REVERSE_DIST     20
#define BUFFER_DATA_WB_OFST_Z_LOOP_TOP_DIST    21
#define BUFFER_DATA_WB_OFST_XYZ_TRAJ_LENGTH    22
#define BUFFER_DATA_WB_OFST_Z_SRCH_VEL_2ND_B   23
#define BUFFER_DATA_WB_OFST_Z_SRCH_DELAY_2ND_B   24
#define BUFFER_DATA_WB_OFST_DRY_RUN_FLAG       25
#define BUFFER_DATA_WB_OFST_Z_SRCH_THRESHOLD_PE_2ND_B   26
#define BUFFER_DATA_WB_OFST_Z_SRCH_PROT_FD_POSN_2ND_B   27
#define BUFFER_DATA_WB_OFST_Z_SRCH_PRE_CONTACT_FORCE_DCOM_2ND_B   28
#define BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_1_TIME_2ND_B   29
#define BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_1_DCOM_2ND_B   30
#define BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_2_TIME_2ND_B   31
#define BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_2_DCOM_2ND_B   32
#define BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_3_TIME_2ND_B   33
#define BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_3_DCOM_2ND_B   34
#define BUFFER_DATA_WB_OFST_Z_TAIL_DIST   35
#define BUFFER_DATA_WB_OFST_Z_RESET_LEVEL   36
#define BUFFER_DATA_WB_OFST_Z_SRCH_DELAY_1ST_B  37

// IntData mapping
#define   MAX_LEN_INT_DATA_BUFF_PROG_0    100
#define WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_1_TIME_1ST_B   7
#define WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_1_DCOM_1ST_B   6
#define WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_2_TIME_1ST_B   9
#define WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_2_DCOM_1ST_B   8

#define WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_1_TIME_2ND_B   25
#define WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_1_DCOM_2ND_B   24
#define WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_2_TIME_2ND_B   27
#define WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_2_DCOM_2ND_B   26

#define WB_BUFF_PROG_0_TRAJ_LEN						30
#define WB_BUFF_PROG_0_1stBond_SearchDelay			45 
#define WB_BUFF_PROG_0_2ndBond_SearchDelay			46
#define WB_BUFF_PROG_0_Remaining_Time_MovingZ_BeforeEndingXY 2
#define WB_BUFF_PROG_0_XY_SpeedProfileMoveTo1stBond 34
#define WB_BUFF_PROG_0_X_Ctrl						74
#define WB_BUFF_PROG_0_Y_Ctrl						75
#define WB_BUFF_PROG_0_X_1ST_BOND_POSN			    53
#define WB_BUFF_PROG_0_Y_1ST_BOND_POSN			    54
#define WB_BUFF_PROG_0_Z_SRCH_HEIGHT_1ST_BOND       3
#define WB_BUFF_PROG_0_Z_SRCH_VEL_1ST_BOND          4
#define WB_BUFF_PROG_0_Z_REVERSE_HEIGHT				12
#define WB_BUFF_PROG_0_X_REVERSE_DIST				13
#define WB_BUFF_PROG_0_Y_REVERSE_DIST				14
#define WB_BUFF_PROG_0_Z_REVERSE_DIST				15
#define WB_BUFF_PROG_0_Z_LOOP_TOP_DIST				21
#define WB_BUFF_PROG_0_Z_SRCH_VEL_2ND_BOND          22
#define WB_BUFF_PROG_0_Z_TAIL_DIST					31
#define WB_BUFF_PROG_0_Z_RESET_LEVEL_POSN			32
#define WB_BUFF_PROG_0_Z_SRCH_THRESHOLD_PE_1ST_B    5
#define WB_BUFF_PROG_0_Z_SRCH_PROT_FD_POSN_1ST_B    67
#define WB_BUFF_PROG_0_Z_SRCH_PRE_CONTACT_FORCE_DCOM_1ST_B       48
#define WB_BUFF_PROG_0_Z_SRCH_THRESHOLD_PE_2ND_B   23
#define WB_BUFF_PROG_0_Z_SRCH_PROT_FD_POSN_2ND_B   68
#define WB_BUFF_PROG_0_Z_SRCH_PRE_CONTACT_FORCE_DCOM_2ND_B       49

typedef struct
{
	int iTimeSeg1;
	int iDcomSeg1;
	int iTimeSeg2;
	int iDcomSeg2;
	int iTimeSeg3;
	int iDcomSeg3;
	int iTimeSeg4;
	int iDcomSeg4;
	int iTimeSeg5;
	int iDcomSeg5;
}BOND_DCOM_TIME_SEG;

extern int acs_bufprog_read_int_data_wb_buff_0();
extern int acs_bufprog_get_wb_srch_delay_1st_bond();
extern int acs_bufprog_get_wb_srch_delay_2nd_bond();
extern int acs_bufprog_get_wb_remain_time_move_z_before_xy_stop();
extern int acs_bufprog_get_wb_xy_speed_prof_idx();
extern int acs_bufprog_get_wb_x_ctrl_idx();
extern int acs_bufprog_get_wb_y_ctrl_idx();
extern int acs_bufprog_get_wb_traj_len();
int acs_bufprog_get_wb_x_1st_bond_posn();
int acs_bufprog_get_wb_y_1st_bond_posn();
int acs_bufprog_get_wb_z_1st_bond_search_height_posn();
int acs_bufprog_get_wb_z_1st_bond_search_vel();
int acs_bufprog_get_wb_z_reverse_height_dist();
int acs_bufprog_get_wb_x_reverse_dist();
int acs_bufprog_get_wb_y_reverse_dist();
int acs_bufprog_get_wb_z_reverse_dist();
int acs_bufprog_get_wb_z_loop_top_dist();
int acs_bufprog_get_wb_z_2nd_bond_search_vel();
int acs_bufprog_get_wb_z_tail_dist();
int acs_bufprog_get_wb_z_2nd_reset_level_posn();
int acs_bufprog_get_wb_z_1st_bond_search_threshold_pe();
int acs_bufprog_get_wb_z_1st_bond_search_prot_fb_posn();
int acs_bufprog_get_wb_z_1st_bond_pre_contact_force_dcom_1000();
int acs_bufprog_get_wb_z_2nd_bond_search_threshold_pe();
int acs_bufprog_get_wb_z_2nd_bond_search_prot_fb_posn();
int acs_bufprog_get_wb_z_2nd_bond_pre_contact_force_dcom_1000();
extern int acs_bufprog_write_data_emu_b1w_buff_8();

short acs_update_buffer_7_from_ram_para_(int iMachType);
short acs_clear_buffer_7_from_ram_para_();
short acs_compile_start_buffer_7();

// Buffer program from system software, B1W
int acs_bufprog_0_restart_b1w();
int acs_bufprog_0_run_();
int acs_bufprog_0_stop_();
int acs_buffprog_0_get_bond_progress();
int acs_command_save_parameter();  // 20110530
int acs_command_save_program();

int acs_read_current_rms_array_wb13v_cfg(double *adRMS);  // 20120113
int acs_read_current_rms_prot_lmt_array_wb13v_cfg(double *adRMS_ProtLmt);
int acs_read_current_posn_err_array_wb13v_cfg(double *adPosnErr);
int acs_read_current_pe_prot_lmt_array_wb13v_cfg(double *adPosnErr_ProtLmt);

#include "MtnTune.h"
extern void mtn_tune_calc_bond_prog_by_one_wire_info(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbOneWireInfo);
extern void mtn_tune_set_buff_data_wb_tune_st(BUFFER_DATA_WB_TUNE *stpBuffDataWbTune);
int acs_bufprog_start_buff_b1w_(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbEmuB1WPerformance);

#endif // __ACS_BUFF_PROG__
