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

#ifndef _ROYA_BD_6412_
#define _ROYA_BD_6412_

#include "Roya_Version.h"
#include "usg_blk.h"
#include "rt_clock_def.h"

#define __CONVERT_ADC_TO_VOLT_FlOAT(x)		(x * _IN_VOLT_VIA_ADC_FACTOR_ + _IN_VOLT_VIA_ADC_OFFSET_)
#define __CONVERT_16b_BI_ADC_TO_VOLT_FlOAT(x)		(x * _IN_VOLT_VIA_ADC_FACTOR_BI_16b + _IN_VOLT_VIA_ADC_OFFSET_BI_16b)

#define __CONVERT_VOLT_FlOAT_ADC_TO_16b_BI(x)		((x - _IN_VOLT_VIA_ADC_OFFSET_BI_16b) / _IN_VOLT_VIA_ADC_FACTOR_BI_16b)

#define __CONVERT_DAC_TO_VOLT(x)    ((x - 512) * (- _OUT_VOLT_VIA_DAC_OFFSET_)/ 512)
#define __CONVERT_DAC_TO_VOLT_12b(x)    ((x - 2047) * 12/ 2047)

#define __CONVERT_DAC_FROM_VOLT_TO_10b_(fDacValue_v)   ((fDacValue_v - _OUT_VOLT_VIA_DAC_OFFSET_)/_OUT_VOLT_VIA_DAC_FACTOR_)
#define __CONVERT_DAC_FROM_VOLT_TO_12b_(fDacValue_v)   ((fDacValue_v - _OUT_VOLT_VIA_DAC_OFFSET_12b)/_OUT_VOLT_VIA_DAC_FACTOR_12b)

typedef struct
{
	char strLocalIP[16];
	char strRemoteIP[16]; 
	SOCKET stSocket;
	//struct ip_mreq mCast;
	SOCKADDR_IN Remote,Local;
	WSADATA wsd;
	unsigned int  s_Port;
	int iRoyaEthSocketBind;
}ROYA_ETH_INFO;

short roya_6412_check_eth_socket();

////////////////////////////////////////////////////////
// EFO Related Functions and Macro-Define
////////////////////////////////////////////////////////
short roya_6412_get_efo_good_flag(unsigned int *pEfoGoodFlag);

////////////////////////////////////////////////////////
// Non-Stick Related Functions and Macro-Define
////////////////////////////////////////////////////////
#include "BSD_DEF.h"
#include "Roya_def.h"

void roya_6412_set_flag_adc_byte(int iFlag);
void roya_6412_set_flag_real_time_clock_year_byte(int iFlag);
int roya_6412_get_flag_adc_byte();
int roya_6412_get_flag_real_time_clock_year_byte();
short roya_6412_set_rt_clock_year2B(RTC_TM *stpRealTimeClock);

void roya_6412_set_flag_dac_bit(int iFlag);  // 20110822
int roya_6412_get_flag_dac_bit();

void roya_6412_set_flag_adc_polarity(int iFlag);
int roya_6412_get_flag_adc_polarity();

short roya_6412_bsd_get_parameters(BSD_PARAMETER *stpBSD_Para, unsigned char ucBlkBSD);
short roya_6412_bsd_set_parameters(BSD_PARAMETER *stpBSD_Para, unsigned char ucBlkBSD);
short roya_6412_bsd_start_detect(unsigned char ucBlkIdx);
short roya_6412_bsd_warm_reset();
short roya_6412_bsd_get_status(BSD_STATUS *stpStatusBSD);
short roya_6412_bsd_get_data(unsigned int uiBlkIdx, unsigned int *uiSampleCnt);
short roya_6412_bsd_get_data_from_len(unsigned int uiCurrBlkIdx, unsigned short usFromCnt, unsigned short usLenData, unsigned int *uiSampleCnt);
short roya_6412_bsd_firmware_analyze_stickness(unsigned char ucBSD1stBlk, unsigned char ucBSD2ndBlk, BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT *stpOutputAnalyzeBSD);
short roya_6412_bsd_set_firmware_analyze_parameter(BSD_FIRMWARE_ANALYZE_STICKNESS_PARAMETER *stpInput);
short roya_6412_bsd_get_firmware_analyze_parameter(BSD_FIRMWARE_ANALYZE_STICKNESS_PARAMETER *stpInput);
short roya_6412_bsd_get_firmware_analyze_output(BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT *stpOutput);

// Functions to test communication
short roya_6412_test_communication(char *strSendChar, char cLen);
short roya_6412_get_version(VERSION_INFO *stpVersionRoya6412);
short roya_6412_comm_test_set_lcd(char cByteLCD);
short roya_6412_comm_test_set_dac(unsigned int uiDacIdx, unsigned int uiDacValue);
short roya_6412_comm_test_get_adc(unsigned int uiAdcIdx, unsigned int *uiAdcValue);
short roya_6412_comm_test_set_timer_period(char cTimerId, unsigned int uiTimerPeriod);
short roya_6412_get_rt_clock(RTC_TM *stpRealTimeClock);
short roya_6412_set_rt_clock(RTC_TM *stpRealTimeClock);
short roya_6412_comm_test_get_timer_period_clock_cnt(char cTimerId, unsigned int *uiTimerPeriod);
short roya_6412_get_key_interrupt_counter(KEY_INTERRUPT_COUNTER *stpRoyaKeyIntCounter);
short roya_6412_read_flash(unsigned int uiAddressFlashRelative, unsigned char ucReadLen, unsigned char *strReadBuffer);
short roya_6412_write_flash(unsigned int uiAddressFlashRelative, unsigned char ucWriteLen, unsigned char *strWriteBuffer);

// USG application related: Ultra-Sonic Generation
short roya_6412_usg_prof_set_blk_seg(unsigned int uiBlk, unsigned int uiSeg, USG_UNIT_SEG *stpUSG_Seg);
short roya_6412_usg_prof_get_blk_seg(unsigned int uiBlk, unsigned int uiSeg, USG_UNIT_SEG *stpUSG_Seg);

short roya_6412_usg_profblk_set_trigger_address(unsigned int uiBlk, int iAddr);
short roya_6412_usg_profblk_get_trigger_address(unsigned int uiBlk, int *iTrigAddress);
short roya_6412_usg_profblk_set_nextblk_index(unsigned int uiBlk, unsigned int uiNextBlk);
short roya_6412_usg_profblk_get_nextblk_index(unsigned int uiBlk, unsigned int *uiNextBlk);
short roya_6412_usg_profblk_set_max_count_wait_trig_prot(unsigned int uiBlk, unsigned int uiWaitCnt);
short roya_6412_usg_profblk_get_max_count_wait_trig_prot(unsigned int uiBlk, unsigned int *uiMaxCountTrigProt);
short roya_6412_usg_profblk_set_start_flag_with_blk(unsigned int uiBlk, unsigned int uiStartFlag);
short roya_6412_usg_profblk_get_start_with_status(unsigned int *uiBlk, unsigned int *uiSegIdx, 
												  unsigned int *uiStartFlag, int *iErrorFlag);
short roya_6412_usg_profblk_warm_reset();

// Function application with SingalGen
short roya_6412_siggen_set_new_cycle(unsigned int uiCycle);
short roya_6412_siggen_get_curr_cycle(unsigned int *uiCurrCycle);
short roya_6412_siggen_set_new_amplitude(unsigned int uiAmp);
short roya_6412_siggen_get_curr_amplitude(unsigned int *uiCurrAmp);
short roya_6412_siggen_set_start_flag_calc_sine(unsigned int uiFlag);
short roya_6412_siggen_get_calc_sine_flag(unsigned int *uiFlag);
short roya_6412_siggen_get_curr_sine_gen_cfg(SIG_GEN_SINE_OUT_CFG *stpSigGenSineOutCfg);
short roya_6412_siggen_set_new_frac_cycle(unsigned int uiCycle, unsigned int uiFracCycle);

/// BQM related
short roya_6412_bqm_set_pre_counter_th(int *iPreCounterTH);
short roya_6412_bqm_set_max_counter_adc(int *iMaxCounter);
short roya_6412_bqm_get_curr_status(BQM_STATUS *stpBQM_Status);
short roya_6412_bqm_get_adc_bh_c_from_len(unsigned int uiCurrBlkIdx, unsigned short usFromCnt, unsigned short usLenData, unsigned int *uiSampleCnt);

#endif // _ROYA_BD_6412_