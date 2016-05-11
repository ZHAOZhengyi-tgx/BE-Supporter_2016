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



#ifndef __AFT_PRBS__
#define __AFT_PRBS__

// libary local parameters initialization
#define DAC_MAX_16BIT_SIGNED    32768
#define MAX_PRBS_LEN    10000
// 20110315 Modify for 7 x 4200, sinesweep
#define PRBS_FB_MAX_ARRAY_ROW    7
#define PRBS_FB_MAX_ARRAY_COL    4200
#define PRBS_NUM_POINT_ARRAY_SPDC  10000

// Spectrum test return codes
#define AFT_PRBS_ERR_EXCEED_MAX_LEN    1
#define AFT_PRBS_ERR_DOWNLOAD_ERROR    2
#define AFT_PRBS_ERR_UPLOAD_DATA       3
#define AFT_PRBS_ERR_NOT_VALID_PARAMETER   5
#define AFT_SPEC_TEST_ERR_TIME_OUT    6
#define AFT_SPEC_TEST_ERR_MOTOR_DISABLED  7

// Structure related with SineSweep
typedef struct
{
	double fAmplitude;
	double fTs_ms;
	double fFreq_Hz;
	double dNumCycle;
}SINE_SWEEP_GEN_EXCITE_ONE_FREQ_IN;
// #define  __CMD_LEN_AT_TS__          30000
typedef struct
{
	double afSineOut[PRBS_FB_MAX_ARRAY_COL];
	double afTimePointTs[PRBS_FB_MAX_ARRAY_COL];
	unsigned int uiLen;
}SINE_SWEEP_GEN_EXCITE_ONE_FREQ_OUT;

typedef struct
{
	unsigned short usNumCaseDec;
	double dLowFreq_Hz;
	double dUppFreq_Hz;
}SINE_SWEEP_SYS_CONFOG;

typedef struct
{
	SINE_SWEEP_SYS_CONFOG  stSineSweepSysCfg;
	double dAmpSineSweepRatio;
	unsigned int uiTotalCaseFreq;
	double *pdFreqArray;
}SINE_SWEEP_AXIS_CONFIG;

typedef struct
{
//	unsigned short usCountLenSineSweep;
	SINE_SWEEP_GEN_EXCITE_ONE_FREQ_IN stSineSweepOnceExciteFreq;
	unsigned int uiExcitingAxis;
}SINE_SWEEP_ONE_EXCITE;

typedef struct
{
	unsigned short usCountLenPRBS;
	unsigned short usAmplitudePRBS;
	unsigned short usFreqFactorPRBS;
	unsigned int uiExcitingAxis;
	unsigned int uiRandSeed;
}PRBS_TEST_CONDITION;

#define MAX_NUM_AXIS_GROUP_SPECTRUM    4

typedef struct
{
	unsigned short usActualNumAxis;
	unsigned int uiAxis_CtrlCardId[MAX_NUM_AXIS_GROUP_SPECTRUM];
	int iLowerLimitPosition[MAX_NUM_AXIS_GROUP_SPECTRUM];
	int iUpperLimitPosition[MAX_NUM_AXIS_GROUP_SPECTRUM];
	unsigned int uiNumPoints[MAX_NUM_AXIS_GROUP_SPECTRUM];
	unsigned char ucFlagIsExciteAxis[MAX_NUM_AXIS_GROUP_SPECTRUM];
	unsigned short usAxisAmpSineSweep[MAX_NUM_AXIS_GROUP_SPECTRUM];
	unsigned short usAxisTimeLenSineSweep[MAX_NUM_AXIS_GROUP_SPECTRUM];
}SINE_SWEEP_GROUP_CONFIG;

typedef struct
{
	unsigned short usActualNumAxis;
	unsigned int uiAxis_CtrlCardId[MAX_NUM_AXIS_GROUP_SPECTRUM];
	int iAxisPosn[MAX_NUM_AXIS_GROUP_SPECTRUM];
	SINE_SWEEP_AXIS_CONFIG stSinSweepAxisCfg;
	SINE_SWEEP_ONE_EXCITE stSineSweepExciteOnce;
}SINESWEEP_GROUP_TEST_INPUT;

typedef struct
{
	unsigned short usActualNumAxis;
	unsigned int uiAxis_CtrlCardId[MAX_NUM_AXIS_GROUP_SPECTRUM];
	int iAxisPosn[MAX_NUM_AXIS_GROUP_SPECTRUM];
	PRBS_TEST_CONDITION stPrbsTestOnceInput;
	int iFlagHasBondHeadApp;   // 20110719
}PRBS_GROUP_TEST_INPUT;

typedef struct
{
	double dFreqControl_Hz;
	unsigned short usActualNumAxis;
	unsigned int uiAxis_CtrlCardId[MAX_NUM_AXIS_GROUP_SPECTRUM];
	int iLowerLimitPosition[MAX_NUM_AXIS_GROUP_SPECTRUM];
	int iUpperLimitPosition[MAX_NUM_AXIS_GROUP_SPECTRUM];
	unsigned int uiNumPoints[MAX_NUM_AXIS_GROUP_SPECTRUM];
	unsigned char ucFlagIsExciteAxis[MAX_NUM_AXIS_GROUP_SPECTRUM];
	unsigned short usAxisAmpPrbs[MAX_NUM_AXIS_GROUP_SPECTRUM];
	unsigned short usAxisPrbsLen[MAX_NUM_AXIS_GROUP_SPECTRUM];
	unsigned short usAxisPrbsFreqFactor[MAX_NUM_AXIS_GROUP_SPECTRUM];
	unsigned short usAmplitudeSineSweep16bDAC[MAX_NUM_AXIS_GROUP_SPECTRUM];
}SPECTRUM_SYS_ANA_GROUP_CONFIG;


short aft_spectrum_initialize();
void aft_spectrum_init_sine_sweep_cfg();

double aft_get_basic_time_unit();

int aft_get_spectrum_test_bakup_flag();
void aft_backup_speedprofile_for_spec_axis(PRBS_GROUP_TEST_INPUT *stpGroupPrbsInput);
void aft_restore_speedprofile_for_spec_axis(PRBS_GROUP_TEST_INPUT *stpGroupPrbsInput);
short aft_set_smaller_speed(double dAccRatio, double dJerkRatio, PRBS_GROUP_TEST_INPUT *stpGroupPrbsInput);
short aft_group_prbs_move_and_excite_once(PRBS_GROUP_TEST_INPUT *stpGroupPrbsInput);

void aft_backup_servo_control_for_spec_axis(PRBS_GROUP_TEST_INPUT *stpGroupPrbsInput);
void aft_restore_servo_control_for_spec_axis(PRBS_GROUP_TEST_INPUT *stpGroupPrbsInput);

short aft_gen_rand_array(unsigned int uiRandSeed, unsigned short usPrbsLen, short sPrbsAmp);
short aft_get_prbs_array(short *psCmdPrbs, unsigned short sLen);
short aft_download_prbs_array(unsigned short usCountLenPRBS);
short aft_download_prbs_parameter(PRBS_TEST_CONDITION *stpPrbsTestInput);
short aft_start_prbs_excitation();
short aft_get_prbs_response(unsigned short usCountLenPRBS);
short aft_prbs_excite_once(PRBS_TEST_CONDITION *stpPrbsTestInput);
void aft_get_prbs_response_array(double *adResponseArrayPrbs, unsigned short *usRow, unsigned short *usCol);
void aft_get_prbs_sp_vel_response(double adResponseArraySP_VelPrbs[], unsigned short *usLen);
int aft_get_hold_drive_output(int iAxisInCtrl);

void aft_convert_group_prbs_cfg_to_test_input(SPECTRUM_SYS_ANA_GROUP_CONFIG stPRBS_GroupTestConfig, 
											  PRBS_GROUP_TEST_INPUT* stpPrbsGroupTestCase,
											  unsigned int uiRandSeed);
void aft_spec_test_dlg_save_response_memory_to_file(char *strFilename, PRBS_TEST_CONDITION stPrbsTestInput);
void aft_spectrum_sinesweep_save_response_memory_to_file(char *strFilename, SINE_SWEEP_ONE_EXCITE *stpSineSweepOneExcite);
void aft_spectrum_sinesweep_save_master_cfg(char strFilename[], SINESWEEP_GROUP_TEST_INPUT *stpGroupSineSweepInput);
void aft_spectrum_make_new_folder(unsigned char ucFlagExciteAxisZ);  // for white noise
void aft_spectrum_make_new_folder_tuning_vk(unsigned char ucFlagExciteAxisZ, unsigned int uiCurrCase, unsigned int uiTotalCase);

void aft_white_noise_spectrum_make_master_file(SPECTRUM_SYS_ANA_GROUP_CONFIG stPRBS_GroupTestConfig, 
											  PRBS_GROUP_TEST_INPUT* stpPrbsGroupTestCase);

void aft_spectrum_make_new_folder_axis(unsigned char ucFlagExciteAxisZ, int iSysAnaAxis); // for Sinesweep and 
int aft_spectrum_save_servo_parameter_into_spectrum_folder(HANDLE hAcsHandle);

short aft_get_sample_period_ms(	double *pdSamplePeriod_ms);
int aft_spectrum_get_debug_flag();
void aft_spectrum_set_debug_flag(int iFlag);
int aft_spectrum_get_sine_sweep_current_case_freq();

void aft_spectrum_enable_debug();
void aft_spectrum_disable_debug();
short aft_safe_exit_spectrum_test();

#define OPEN_LOOP_SPECTRUM_TEST     0
#define VEL_LOOP_SPECTRUM_TEST		1

char aft_spectrum_get_flag_velloop();
void aft_spectrum_set_flag_velocity_loop_excite(char cFlagExciteVelLoop);
char aft_spectrum_get_flag_dsp_data();
void aft_spectrum_set_flag_dsp_data(char cFlag);

int aft_spectrum_get_thread_flag_stop();
void aft_spectrum_set_thread_flag_stop(int bFlagStop);

/////////
#define SYSTEM_ANALYSIS_PSEUDO_WHITE_NOISE  0
#define SYSTEM_ANALYSIS_PSEUDO_SINE_SWEEP   1
#define SYSTEM_ANALYSIS_PSEUDO_ONE_PULSE_RESP  2
int aft_get_spectrum_sys_analysis_flag();
void aft_set_spectrum_sys_analysis_flag(int iFlagSysAnalysis);

// SineSweep Related
short aft_group_sine_sweep_move_and_excite_multi_freq(SINESWEEP_GROUP_TEST_INPUT *stpGroupSineSweepInput);
unsigned short aft_spectrum_build_sinesweep_from_sys_ana_cfg(SPECTRUM_SYS_ANA_GROUP_CONFIG *stpSysAnaSpectrumCfg, SINE_SWEEP_AXIS_CONFIG *astSineSweepAxisCfg);

#endif // 