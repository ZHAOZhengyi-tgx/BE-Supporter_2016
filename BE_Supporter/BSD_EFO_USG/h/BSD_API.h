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

#ifndef __BSD_API__
#define __BSD_API__

#include "rt_clock_def.h"
#include "Roya_Version.h"
#define _MAX_HOST_NAME_GET_STRING_  128
#define _MAX_STRLEN_IP_ADDRESS_      16
// Function to get host name, and upto 10 IP addresses
// maximum of ten IP addresses
short get_local_network_info(char strHostName[_MAX_HOST_NAME_GET_STRING_], char astrIPAddresses[10][_MAX_STRLEN_IP_ADDRESS_]);


// Initialize communication
short roya_6412_init_eth_socket(char *strLocalIP, char *strRemoteIP);

#define BSD_OKEY   0
///////////////   Error Code for BSD and Communication
#define BSD_PARA_ERR_EXCEED_BUFFER_LEN  1
#define BSD_PARA_ERR_DAC_OUT_RANGE      2
#define BSD_PARA_ERR_EXCEED_MAX_NUM_DAC 4
#define BSD_PARA_ERR_EXCEED_MAX_NUM_ADC 8
#define BSD_PARA_ERR_EXCEED_MAX_NUM_BLK 16

#define MTN_ROYA_WSA_STARTUP_ERR          64
#define MTN_ROYA_GET_SOCKET_ERR           65
#define MTN_ROYA_COMM_PRE_CHECK_ERROR     66
#define MTN_ROYA_COMM_REPLY_LEN_ERROR     67
#define MTN_ROYA_GET_WRONG_DATA           68
#define MTN_ROYA_COMM_SENT_LEN_ERROR      69
#define MTN_ROYA_ETH_UDP_BIND_ERROR       70
#define MTN_ROYA_ETH_WRITE_FLASH_ERR      71

// 20111010 Threshold in ms, For cheching the communication error
#define BSD_COMMUNICATION_ERROR_TH_MS     300

// Communication related, time-out around 20 seconds
short bsd_api_get_version_roya_6412(VERSION_INFO *stpVersionRoya6412);
short roya_api_get_rt_clock(RTC_TM *stpRealTimeClock);
short roya_api_set_rt_clock(RTC_TM *stpRealTimeClock);

unsigned int roya_firmware_get_size();
short roya_firmware_download_eth();

/////////
/// EFO related
/////////
short efo_api_init();
short efo_api_get_good_counter(unsigned int *uiEFOCounter);
short efo_api_check_good_once(BOOL *bEfoIsGood);
short bsd_api_get_efo_good_counter(unsigned int *pEfoGoodFlag); // The same as efo_api_get_good_counter

//////////
/// Wire Spool Rotation Detection
//////////
short bsd_api_check_wire_spool_motor_init();
short bsd_api_check_wire_spool_motor_once(BOOL *bWireSpoolMotorIsMoved);

// Calling sequence must be in pair
// efo_api_init()
// ... Trigger EFO once, wait around 10ms
// efo_api_check_good_once(BOOL *bEfoIsGood);
// possible reasons for error
// (1) successfully trigger more than once, but called efo_api_check_good_once only once;
// (2) triggered but not successful
// (3) triggered but too short (i.e. <= 5 ms)

////////////////////////////////////////////////////////
// Non-Stick Related Functions and Macro-Define
////////////////////////////////////////////////////////
#include "BSD_DEF.h"

// Basic IO Unit parameters
#define _OUT_VOLT_VIA_DAC_FACTOR_     (20.0/1024)
#define _OUT_VOLT_VIA_DAC_OFFSET_     (-10.0)

#define _OUT_VOLT_VIA_DAC_FACTOR_12b     (20.0/4096)
#define _OUT_VOLT_VIA_DAC_OFFSET_12b     (-10.0)

#define _IN_VOLT_VIA_ADC_FACTOR_      (25.0/65536)
#define _IN_VOLT_VIA_ADC_OFFSET_       0.0

#define _IN_VOLT_VIA_ADC_FACTOR_BI_16b      (12.0/32767.0)
#define _IN_VOLT_VIA_ADC_OFFSET_BI_16b       -12.0

#define ROYA_MAX_NUM_ADC    4
#define ROYA_MAX_NUM_DAC    4
#define ROYA_MAX_DAC_VALUE  1023
#define ROYA_MAX_ADC_VALUE  65535

// 1. BSD Parameter related, Non-Stick Detection
typedef struct
{
Uint8 cDetectFactorIsr;
Uint16 usDetectSampleLength;
Uint8 cAdcIdx;
Uint8 cDacIdx;
float fDacValue_v;
}BSD_API_PARAMETER;

short bsd_api_set_parameter(BSD_API_PARAMETER *stpParaBlkBSD, unsigned char ucBlkBSD);
short bsd_api_get_parameter(BSD_API_PARAMETER *stpParaBlkBSD, unsigned char ucBlkBSD);
short bsd_api_start_detect(unsigned char ucBlkIdx);
short bsd_api_warm_reset();
short bsd_api_get_status(BSD_STATUS *pstStatusBSD);
short bsd_api_get_adc_volt(unsigned int idxAdc, double *dAdcVolt);

short bsd_debug_save_data_file(char *strFilename);

// set and get the BSD-ISR Freq Factor based on 10KHz
short bsd_api_set_isr_freqency_factor_10KHz(unsigned short usFreqFactor_10KHz);
unsigned short bsd_api_get_isr_freq_factor_base_10KHz();
short roya_api_get_timer_period_clock_cnt(char cTimerId, unsigned int *uiTimerPeriod);

typedef struct
{
	double dMax;
	double dMin;
	double dMean;
	double dStandDevia;
	double dVoltOut;
	double dVoltReadMean;

}BSD_DATA_ANALYSIS;
short bsd_api_analyze_data(unsigned int uiBlkIdx, unsigned int uiSampleCnt, BSD_DATA_ANALYSIS *stpAnalyDataBSD);

// 2. Non-Stick Analyze Stickness after Detection

typedef struct
{
	unsigned char  ucParaBlk1stBSD;
	unsigned char  ucParaBlk2ndBSD;


	unsigned short us1stBondSkipSamples;
	float fBSD1stBondStickDieThAdc_v;  // first bond, when AdcFeedback is lower than this value, it means successful sticking to die
	unsigned short us1stBondNumSamplesConsecGreaterDieThAdc;
	unsigned short us1stBondNumSamplesConsecLowerLeadThAdc; // Not used for checking stickness, only for monitoring

	float fBSD2ndBondStickLeadThAdc_v;  // 2nd bond, when AdcFeedback is lower than this value, it means successful sticking to lead
	unsigned short us2ndBondSkipSamples;
	unsigned short us2ndBondNumSamplesConsecLowerLeadThAdc;
	float fBSD2ndBondNonStickThAdc_v;  // 2nd bond, when AdcFeedback is higher than this value, it means successful non-stickness
	unsigned short us2ndBondNumSampleConsecHigher;
}BSD_ANALYZE_STICKNESS_INPUT;

// Bond Stickness Detection Logic:
// (1) First bond, detection nons, tickness xor stickness,
//       NonStickness: error
//			whenever AdcFeedback is consecutively >= fBSD1stBondStickDieThAdc_v for us1stBondNumSamplesConsecGreaterDieThAdc samples
//       Stickness: OK
// (2) Second bond, only if first-bond is OK, detection of following cases
//      (2.1) StickingLead: once AdcFeedback is consecutively <= fBSD2ndBondStickLeadThAdc_v for us2ndBondNumSamplesConsecLowerLeadThAdc samples, 
//		(2.2) Above condition never satisfied:error BSD_ERR_2ND_BOND_NON_STICK_LEAD
//		(2.3) detection of first rising edge, 
//				us2ndBondOpenSticknessFirstRiseCount = ii, 
//					s.t. {AdcFeedback[ii] >= fBSD2ndBondStickLeadThAdc_v and  AdcFeedback[ii-1] < fBSD2ndBondStickLeadThAdc_v}
//		(2.4) monitoring total number of above rising edges
//				us2ndBondOpenStickCount

// 20090728, initialize for maximum 10KHz sampling rate.
// Only return the Counter_SampleNo of the first rising edge, 
//  (2.1) after us2ndBondSkipSamples
//  (2.2) initialize us2ndBondOpenSticknessFirstRiseCount = 0;
typedef struct
{
	unsigned short us1stBondStickStatus;         // 0: OK; Non-zero: Not OK, above error code, BSD_ERR_*
	unsigned short us1stBondEndCheckStickness;

	unsigned short us2ndBondStickStatus;         // 0: OK; Non-zero: Not OK, above error code, BSD_ERR_*
	unsigned short us2ndBondOpenSticknessFirstRiseCount;
	unsigned short us2ndBondStartSampleStickness;
	unsigned short us2ndBondOpenStickCount;      // number of times, when open stickness condition satisfied 
}BSD_ANALYZE_STICKNESS_OUTPUT;
short bsd_api_analyze_stickness(BSD_ANALYZE_STICKNESS_INPUT *stpInputAnalyzeBSD, 
									 BSD_ANALYZE_STICKNESS_OUTPUT *stpOutputAnalyzeBSD);

short bsd_api_firmware_analyze_stickness(unsigned char ucBSD1stBlk, unsigned char ucBSD2ndBlk, 
									 BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT *stpOutputAnalyzeBSD);
short bsd_api_set_parameter_firmware_analyze_stickness(BSD_ANALYZE_STICKNESS_INPUT *stpInputAnalyzeBSD);
short bsd_api_get_parameter_firmware_analyze_stickness(BSD_ANALYZE_STICKNESS_INPUT *stpInputAnalyzeBSD);
short bsd_api_get_firmware_analyze_stickness_output(BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT *stpOutput);


#endif  // __BSD_API__