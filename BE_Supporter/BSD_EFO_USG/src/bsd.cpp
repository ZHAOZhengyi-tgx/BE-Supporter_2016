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

// 20091221 BSD2ndBond_FirstRisingEdge default is MAXIM
// 20100217 automatically update efo-counter, return communication error-code,
// 20110822 
#include "stdafx.h"
#include "mtndefin.h"
#include "RTC_Roya_bd_6412.h"
#include "RoyaBd6412.h"
//#include "WinTiming.h"

#include "BSD_DEF.h"
#include "BSD_API.h"

extern char code[];

unsigned int uiDataAdcBSD[_BSD_MAX_BLK_PARAMETER_][_BSD_MAX_DATA_LEN_];

#include "math.h"

//#define __BSD_DEBUG__

// Function to get host name, and upto 10 IP addresses
// maximum of ten IP addresses
short get_local_network_info(char strHostName[_MAX_HOST_NAME_GET_STRING_], char astrIPAddresses[10][_MAX_STRLEN_IP_ADDRESS_])
{
	short sRet = MTN_API_OK_ZERO;
	WSADATA WSAData;

	// Initialize winsock dll
	if(::WSAStartup(MAKEWORD(1, 0), &WSAData))
	{
		// Error handling
		sRet = MTN_API_ERROR_START_WIN_SOCKET_DLL;
		goto label_get_local_network_info;
	}

	// Get local host name
	if(::gethostname(strHostName, _MAX_HOST_NAME_GET_STRING_))
	{
		// Error handling -> call 'WSAGetLastError()'
		sRet = MTN_API_ERROR_GET_HOSTNAME;
		goto label_get_local_network_info;
	}

	// Get local IP addresses
	struct sockaddr_in SocketAddress;
	struct hostent *pHost = 0;

	pHost = ::gethostbyname(strHostName);
	if(!pHost)
	{
		// Error handling -> call 'WSAGetLastError()'
		sRet = MTN_API_ERROR_GET_HOST;
		goto label_get_local_network_info;
	}

	for(int iCnt = 0; ((pHost->h_addr_list[iCnt]) && (iCnt < 10)); ++iCnt)
	{
		memcpy(&SocketAddress.sin_addr, pHost->h_addr_list[iCnt], pHost->h_length);
		strcpy_s(astrIPAddresses[iCnt], _MAX_STRLEN_IP_ADDRESS_, inet_ntoa(SocketAddress.sin_addr));
	}

	// Cleanup
	WSACleanup();

label_get_local_network_info:
	return sRet;
}

// Firmware upload and download Roya-
extern unsigned int roya_firmware_get_size();

short roya_firmware_download_eth()
{
	unsigned int uiSizeFirmware = roya_firmware_get_size();

	unsigned int uiDownloadLoop = (int)(uiSizeFirmware/__MAX_FLASH_READ_LEN);

	unsigned int uiDownloadLastLoopSize = uiSizeFirmware %__MAX_FLASH_READ_LEN;

	unsigned int uiStartAddressRelativeFlash;

	short sRet= MTN_API_OK_ZERO;

	uiStartAddressRelativeFlash = 0;
	for(unsigned int ii = 0; ii<uiDownloadLoop; ii++)
	{
		sRet = roya_6412_write_flash(uiStartAddressRelativeFlash, __MAX_FLASH_READ_LEN, (unsigned char *)&(code[uiStartAddressRelativeFlash]));
		uiStartAddressRelativeFlash = uiStartAddressRelativeFlash + __MAX_FLASH_READ_LEN;

		if(sRet != MTN_API_OK_ZERO)
		{
			return sRet;
		}
		else
		{
			Sleep(100);
		}
	}

	sRet = roya_6412_write_flash(uiStartAddressRelativeFlash, uiDownloadLastLoopSize, (unsigned char *)&(code[uiStartAddressRelativeFlash]));
	return sRet;

}

// EFO related
static unsigned int uiEFO_GoodCounter;
//	unsigned int uiPrevCount = uiEFO_GoodCounter;
unsigned int uiPrevCount;

short efo_api_get_good_counter(unsigned int *uiEFOCounter)
{
	short sRet = MTN_API_OK_ZERO;
	
	sRet = roya_6412_get_efo_good_flag(&uiEFO_GoodCounter);

	*uiEFOCounter = uiEFO_GoodCounter;

	return sRet;
}

short efo_api_init()
{
	short sRet = MTN_API_OK_ZERO;
	sRet = roya_6412_get_efo_good_flag(&uiPrevCount);
	uiEFO_GoodCounter = uiPrevCount;

	return sRet;

}

#define __EFO_CHECK_DEBOUNCE_BY_PC__
short efo_api_check_good_once(BOOL *bEfoIsGood)
{
	short sRet = MTN_API_OK_ZERO;
	sRet = roya_6412_get_efo_good_flag(&uiEFO_GoodCounter);
#ifdef   __EFO_CHECK_DEBOUNCE_BY_PC__
	if(uiEFO_GoodCounter > uiPrevCount || 
	(uiEFO_GoodCounter == 0 && uiPrevCount == UINT_MAX)
		) // Once a good signal is detected, the BSD start_flag is set automatically
#else
	if(uiEFO_GoodCounter == (uiPrevCount + 1) || 
	(uiEFO_GoodCounter == 0 && uiPrevCount == UINT_MAX)
		) // Once a good signal is detected, the BSD start_flag is set automatically
#endif // __EFO_CHECK_DEBOUNCE__
	{
		*bEfoIsGood = TRUE;
	}
	else
	{
		*bEfoIsGood = FALSE;
	}

	uiPrevCount = uiEFO_GoodCounter; // 20100217, automatically update
	return sRet;
}

// EFO related
static unsigned int uiWireSpoolMotorMoveCounter;
static unsigned int uiWireSpoolMotorMovePreCounter;
static KEY_INTERRUPT_COUNTER stRoyaKeyIntCounter;

short bsd_api_get_wire_spool_motor_move_counter(unsigned int *uiWireSpoolMotorCounter)
{
	short sRet = MTN_API_OK_ZERO;
	sRet = roya_6412_get_key_interrupt_counter(&stRoyaKeyIntCounter);
	*uiWireSpoolMotorCounter = stRoyaKeyIntCounter.uiWireFeedSensorTriggerCounter;

	return sRet;
}

short bsd_api_check_wire_spool_motor_init()
{
	short sRet = MTN_API_OK_ZERO;
	bsd_api_get_wire_spool_motor_move_counter(&uiWireSpoolMotorMoveCounter);
	uiWireSpoolMotorMovePreCounter = uiWireSpoolMotorMoveCounter;

	return sRet;//efo_api_init();
}

short bsd_api_check_wire_spool_motor_once(BOOL *bWireSpoolMotorIsMoved)
{
	short sRet = MTN_API_OK_ZERO;
	sRet = bsd_api_get_wire_spool_motor_move_counter(&uiWireSpoolMotorMoveCounter);

	if(uiWireSpoolMotorMoveCounter != uiWireSpoolMotorMovePreCounter)
	{
		*bWireSpoolMotorIsMoved = TRUE;
	}
	else
	{
		*bWireSpoolMotorIsMoved = FALSE;
	}

	uiWireSpoolMotorMovePreCounter = uiWireSpoolMotorMoveCounter; // 20100217, automatically update
	return sRet;
}

// Calling sequence must be in pair
// efo_api_init()
// ... Trigger EFO once, wait around 10ms
// efo_api_check_good_once(BOOL *bEfoIsGood);
// possible reasons for error
// (1) successfully trigger more than once, but called efo_api_check_good_once only once;
// (2) triggered but not successful
// (3) triggered but too short (i.e. <= 5 ms)

short bsd_api_get_efo_good_counter(unsigned int *pEfoGoodFlag)
{
	return efo_api_get_good_counter(pEfoGoodFlag);
}

#include "RTC_Roya_bd_6412.h"
#define ROYA_TIMER_IDX_NSD_ISR     1

static unsigned short usFreqFactor_Base10KHz = 5;
// Function related with timer-freqency 
short bsd_api_set_isr_freqency_factor_10KHz(unsigned short usFreqFactor_10KHz)
{
	short sRet = MTN_API_OK_ZERO;

	unsigned int uiTimerPeriod;

	if(usFreqFactor_10KHz > 0) // CANNOT be 0, at least 1
	{
		uiTimerPeriod = (unsigned int) ((double) PERIOD_2KHZ_ON_ROYA_BOARD_6412/5.0 * usFreqFactor_10KHz);
			
		sRet = roya_6412_comm_test_set_timer_period(ROYA_TIMER_IDX_NSD_ISR, uiTimerPeriod);
		if(sRet == MTN_API_OK_ZERO)
		{
			usFreqFactor_Base10KHz = usFreqFactor_10KHz;
		}
	}
	else
	{
		sRet = MTN_API_ERR_BSD_ZERO_FREQ_FACTOR;
	}

	return sRet;
}

unsigned short bsd_api_get_isr_freq_factor_base_10KHz()
{
	return usFreqFactor_Base10KHz;
}

short roya_api_get_timer_period_clock_cnt(char cTimerId, unsigned int *uiTimerPeriod)
{
	short sRet = MTN_API_OK_ZERO;

	if(cTimerId <= 1 && cTimerId >= 0)
	{
		sRet = roya_6412_comm_test_get_timer_period_clock_cnt(cTimerId, uiTimerPeriod);
	}
	else
	{
		sRet = MTN_API_ERR_ROYA_EXCEED_MAX_TIMER;
	}

	return sRet;
}

// Function related with real-time clock
short roya_api_get_rt_clock(RTC_TM *stpRealTimeClock)
{
	short sRet = roya_6412_get_rt_clock(stpRealTimeClock);
	if(stpRealTimeClock->mon >= 12)
	{
		stpRealTimeClock->mon = 11;
	}
	if(stpRealTimeClock->wday >= 7)
	{
		stpRealTimeClock->wday = 6;
	}
	return sRet;
}

short roya_api_set_rt_clock(RTC_TM *stpRealTimeClock)
{
	if(roya_6412_get_flag_real_time_clock_year_byte() == ROYA_BSD_YEAR_2_BYTE)
	{
		return roya_6412_set_rt_clock_year2B(stpRealTimeClock);
	}
	else
	{
		return roya_6412_set_rt_clock(stpRealTimeClock);
	}
}
// 20111008
void bsd_api_init_dac_10v_10bit()
{
	roya_6412_comm_test_set_dac(0, 1023);
	roya_6412_comm_test_set_dac(1, 1023);
	roya_6412_comm_test_set_dac(2, 1023);
	roya_6412_comm_test_set_dac(3, 1023);
}

// 20110916
void bsd_api_init_dac_0v_10bit()
{
	roya_6412_comm_test_set_dac(0, 512);
	roya_6412_comm_test_set_dac(1, 512);
	roya_6412_comm_test_set_dac(2, 512);
	roya_6412_comm_test_set_dac(3, 512);
}

void bsd_api_init_dac_0v_12bit()
{
	roya_6412_comm_test_set_dac(0, 2048);
	roya_6412_comm_test_set_dac(1, 2048);
	roya_6412_comm_test_set_dac(2, 2048);
	roya_6412_comm_test_set_dac(3, 2048);
} // 20110916

void bsd_api_version_control(VERSION_INFO *stpVersionRoya6412)
{
	if(stpVersionRoya6412->usVerMajor == 1 && stpVersionRoya6412->usVerMinor == 5)
	{
		roya_6412_set_flag_adc_byte(ROYA_BSD_ADC_4_BYTE);
		roya_6412_set_flag_real_time_clock_year_byte(ROYA_BSD_YEAR_2_BYTE);
		roya_6412_set_flag_dac_bit(ROYA_BSD_DAC_10b);    // 20110822
		roya_6412_set_flag_adc_polarity(ROYA_BSD_ADC_POLARITY_SINGLE);  // 20110822

		//20111008
//		bsd_api_init_dac_10v_10bit();
//		Sleep(2000);

		//
		bsd_api_init_dac_0v_10bit();  // 20110916

	}
	else if(stpVersionRoya6412->usVerMajor == 1 && stpVersionRoya6412->usVerMinor == 6)
	{
		roya_6412_set_flag_adc_byte(ROYA_BSD_ADC_2_BYTE);
		roya_6412_set_flag_real_time_clock_year_byte(ROYA_BSD_YEAR_4_BYTE);
		roya_6412_set_flag_dac_bit(ROYA_BSD_DAC_10b);    // 20110822
		roya_6412_set_flag_adc_polarity(ROYA_BSD_ADC_POLARITY_SINGLE);  // 20110822

		//20111008
//		bsd_api_init_dac_10v_10bit();
//		Sleep(2000);
		//
		bsd_api_init_dac_0v_10bit();  // 20110916
	}
	else if(stpVersionRoya6412->usVerMajor == 1 && stpVersionRoya6412->usVerMinor == 7)  // 20110822
	{
		roya_6412_set_flag_adc_byte(ROYA_BSD_ADC_2_BYTE);
		roya_6412_set_flag_real_time_clock_year_byte(ROYA_BSD_YEAR_4_BYTE);
		roya_6412_set_flag_dac_bit(ROYA_BSD_DAC_12b);
		roya_6412_set_flag_adc_polarity(ROYA_BSD_ADC_BI_POLAR);  // 20110822

		bsd_api_init_dac_0v_12bit(); // 20110916
	}
}

static int iFlagDoneAtInitialization = 0;
// Function related with version control
short bsd_api_get_version_roya_6412(VERSION_INFO *stpVersionRoya6412)
{
	short sRet = roya_6412_get_version(stpVersionRoya6412);

	if(iFlagDoneAtInitialization == 0)
	{
		iFlagDoneAtInitialization = 1;
		// Version control
		if(sRet == MTN_API_OK_ZERO)
		{
			bsd_api_version_control(stpVersionRoya6412);
		}
		else
		{
			stpVersionRoya6412->usVerDate = 1;
			stpVersionRoya6412->usVerMonth = 1;
			stpVersionRoya6412->usVerYear = 1900;
			stpVersionRoya6412->usVerMajor = 0;
			stpVersionRoya6412->usVerMinor = 0;
		}
	}

	return sRet;
}


short bsd_api_set_parameter(BSD_API_PARAMETER *stpParaBlkApiBSD, unsigned char ucBlkBSD)
{
short sRet = MTN_API_OK_ZERO;
BSD_PARAMETER stParaBlkBSD;

	stParaBlkBSD.cDetectFactorIsr = stpParaBlkApiBSD->cDetectFactorIsr;
	stParaBlkBSD.cAdcIdx = stpParaBlkApiBSD->cAdcIdx;
	stParaBlkBSD.cDacIdx = stpParaBlkApiBSD->cDacIdx;
	if(roya_6412_get_flag_dac_bit() == ROYA_BSD_DAC_10b)
	{
		stParaBlkBSD.uiDacValue = (unsigned int)((stpParaBlkApiBSD->fDacValue_v - _OUT_VOLT_VIA_DAC_OFFSET_)/_OUT_VOLT_VIA_DAC_FACTOR_);
	}
	else  // 12bit DAC, 20120213
	{
		stParaBlkBSD.uiDacValue = (unsigned int)((stpParaBlkApiBSD->fDacValue_v - _OUT_VOLT_VIA_DAC_OFFSET_12b)/_OUT_VOLT_VIA_DAC_FACTOR_12b);
	}

	stParaBlkBSD.usDetectSampleLength = stpParaBlkApiBSD->usDetectSampleLength;

	// Default parameter
	stParaBlkBSD.cDataBufferIdx = ucBlkBSD;
	if(ucBlkBSD == 0 || ucBlkBSD == 2)
	{
		stParaBlkBSD.cNextParaBlkIdx = ucBlkBSD + 1;
	}
	else if(ucBlkBSD == 1 || ucBlkBSD == 3)
	{
		stParaBlkBSD.cNextParaBlkIdx = ucBlkBSD;
	}
	else
	{
		sRet |= BSD_PARA_ERR_EXCEED_MAX_NUM_BLK; // ucBlkIdx
	}

	// Verify the validity of parameters
	if(stParaBlkBSD.cAdcIdx >= ROYA_MAX_NUM_ADC || stParaBlkBSD.cAdcIdx < 0)
	{
		sRet |= BSD_PARA_ERR_EXCEED_MAX_NUM_ADC;
	}

	if(stParaBlkBSD.cDacIdx >= ROYA_MAX_NUM_DAC || stParaBlkBSD.cDacIdx < 0)
	{
		sRet |= BSD_PARA_ERR_EXCEED_MAX_NUM_DAC;
	}

	if(stParaBlkBSD.uiDacValue > ROYA_MAX_DAC_VALUE)
	{
		sRet |= BSD_PARA_ERR_DAC_OUT_RANGE;
	}

	if(stParaBlkBSD.usDetectSampleLength > _BSD_MAX_DATA_LEN_)
	{
		sRet |= BSD_PARA_ERR_EXCEED_BUFFER_LEN;
	}

	if(ucBlkBSD > _BSD_MAX_BLK_PARAMETER_)
	{
		sRet |= BSD_PARA_ERR_EXCEED_MAX_NUM_BLK;
	}

	// download if OKEY
	if(sRet == BSD_OKEY)
	{
		sRet = roya_6412_bsd_set_parameters(&stParaBlkBSD, ucBlkBSD);
	}

	return sRet;
}

short bsd_api_get_parameter(BSD_API_PARAMETER *stpParaBlkApiBSD, unsigned char ucBlkBSD)
{
short sRet = BSD_OKEY;
BSD_PARAMETER stParaBlkBSD;

	if(ucBlkBSD > _BSD_MAX_BLK_PARAMETER_)
	{
		sRet |= BSD_PARA_ERR_EXCEED_MAX_NUM_BLK;
	}

	if(sRet == BSD_OKEY)
	{
		sRet = roya_6412_bsd_get_parameters(&stParaBlkBSD, ucBlkBSD);
		if(sRet == BSD_OKEY)
		{
			stpParaBlkApiBSD->cAdcIdx = stParaBlkBSD.cAdcIdx;
			stpParaBlkApiBSD->cDacIdx = stParaBlkBSD.cDacIdx;
			stpParaBlkApiBSD->cDetectFactorIsr = stParaBlkBSD.cDetectFactorIsr;
			stpParaBlkApiBSD->usDetectSampleLength = stParaBlkBSD.usDetectSampleLength;
			if(roya_6412_get_flag_dac_bit() == ROYA_BSD_DAC_10b)
			{
				stpParaBlkApiBSD->fDacValue_v = (float)(stParaBlkBSD.uiDacValue * _OUT_VOLT_VIA_DAC_FACTOR_ + _OUT_VOLT_VIA_DAC_OFFSET_);
			}
			else  // 12bit DAC, 20120213
			{
				stpParaBlkApiBSD->fDacValue_v = (float)(stParaBlkBSD.uiDacValue * _OUT_VOLT_VIA_DAC_FACTOR_12b + _OUT_VOLT_VIA_DAC_OFFSET_12b);
			}
		}
	}
//#define _OUT_VOLT_VIA_DAC_FACTOR_12b     (20.0/4096)
//#define _OUT_VOLT_VIA_DAC_OFFSET_12b     (-10.0)

return sRet;
}

short bsd_api_get_adc_volt(unsigned int idxAdc, double *dAdcVolt)
{
	unsigned int iAdcValue;
	short sRet = roya_6412_comm_test_get_adc(idxAdc, &iAdcValue);

	if(roya_6412_get_flag_adc_polarity() == ROYA_BSD_ADC_BI_POLAR)   // 20110822
	{
		(*dAdcVolt) = __CONVERT_16b_BI_ADC_TO_VOLT_FlOAT(iAdcValue);
	}
	else  // ROYA_BSD_ADC_POLARITY_SINGLE
	{
		(*dAdcVolt) = __CONVERT_ADC_TO_VOLT_FlOAT(iAdcValue);
	}   // 20110822

	return sRet;
}

short bsd_api_start_detect(unsigned char ucBlkIdx)
{
    short sRet = BSD_OKEY;

	if(ucBlkIdx >= 4)
	{
		sRet |= BSD_PARA_ERR_EXCEED_MAX_NUM_BLK; // ucBlkIdx
	}
	else
	{
		sRet = roya_6412_bsd_start_detect(ucBlkIdx);
	}

	return sRet;

}

short bsd_api_warm_reset()
{
	return roya_6412_bsd_warm_reset();
}

short bsd_api_get_status(BSD_STATUS *pstStatusBSD)
{
	return roya_6412_bsd_get_status(pstStatusBSD);
}

short bsd_api_analyze_data(unsigned int uiBlkIdx, unsigned int uiSampleCnt, BSD_DATA_ANALYSIS *stpAnalyDataBSD)
{
unsigned int ii;
double dSumData = 0, dMeanData, dSumSq = 0;
short sRet = BSD_OKEY;

	for(ii = 0; ii<uiSampleCnt; ii++)
	{
		if(ii == 0)
		{
			stpAnalyDataBSD->dMin = uiDataAdcBSD[uiBlkIdx][ii];
			stpAnalyDataBSD->dMax = uiDataAdcBSD[uiBlkIdx][ii];
		}
		else
		{
			if(stpAnalyDataBSD->dMin > uiDataAdcBSD[uiBlkIdx][ii])
			{
				stpAnalyDataBSD->dMin = uiDataAdcBSD[uiBlkIdx][ii];
			}
			if(stpAnalyDataBSD->dMax < uiDataAdcBSD[uiBlkIdx][ii])
			{
				stpAnalyDataBSD->dMax = uiDataAdcBSD[uiBlkIdx][ii];
			}
		}
		dSumData += uiDataAdcBSD[uiBlkIdx][ii];
	}
	dMeanData = dSumData/uiSampleCnt;
	stpAnalyDataBSD->dMean = dMeanData;

	for(ii = 0; ii<uiSampleCnt; ii++)
	{
		dSumSq += (uiDataAdcBSD[uiBlkIdx][ii] - dMeanData)* (uiDataAdcBSD[uiBlkIdx][ii] - dMeanData);
	}
	stpAnalyDataBSD->dStandDevia = sqrt(dSumSq/uiSampleCnt);

	BSD_PARAMETER  stParameterBlkBSD;
	sRet = roya_6412_bsd_get_parameters(&stParameterBlkBSD, uiBlkIdx);
	if(roya_6412_get_flag_dac_bit() == ROYA_BSD_DAC_10b)
	{
		stpAnalyDataBSD->dVoltOut = stParameterBlkBSD.uiDacValue * _OUT_VOLT_VIA_DAC_FACTOR_ + _OUT_VOLT_VIA_DAC_OFFSET_;
	}
	else  // 12bit DAC, 20120213
	{
		stpAnalyDataBSD->dVoltOut = stParameterBlkBSD.uiDacValue * _OUT_VOLT_VIA_DAC_FACTOR_12b + _OUT_VOLT_VIA_DAC_OFFSET_12b;
	}
	if(roya_6412_get_flag_adc_polarity() == ROYA_BSD_ADC_POLARITY_SINGLE)  // ROYA_BSD_ADC_POLARITY_SINGLE
	{
		stpAnalyDataBSD->dVoltReadMean = dMeanData * _IN_VOLT_VIA_ADC_FACTOR_ + _IN_VOLT_VIA_ADC_OFFSET_;
	}
	else // Bipolar ADC, 20120213
	{
		stpAnalyDataBSD->dVoltReadMean = dMeanData * _IN_VOLT_VIA_ADC_FACTOR_BI_16b + _IN_VOLT_VIA_ADC_OFFSET_BI_16b;
	}
	return sRet;
}
//#define _OUT_VOLT_VIA_DAC_FACTOR_12b     (20.0/4096)
//#define _OUT_VOLT_VIA_DAC_OFFSET_12b     (-10.0)
//#define _IN_VOLT_VIA_ADC_FACTOR_BI_16b      (12.0/32767.0)
//#define _IN_VOLT_VIA_ADC_OFFSET_BI_16b       -12.0

short bsd_write_file_data(FILE *fpData, unsigned char ucPrevBlkBSD, unsigned char ucCurrBlkBSD)
{
short sRet = BSD_OKEY;
	BSD_PARAMETER stBSD_Para, stBSD_PrevBlkPara;

		sRet = roya_6412_bsd_get_parameters(&stBSD_PrevBlkPara, ucPrevBlkBSD);
		fprintf(fpData, "%%BSD_Blk %d - Prev Parameter: \n %%[AdcIdx, DacIdx, FactorIsr, NextBlkIdx, DacVal, SampleLen] = [%d, %d, %d, %d, %d, %d]\n\n ", ucPrevBlkBSD, 
			stBSD_PrevBlkPara.cAdcIdx, stBSD_PrevBlkPara.cDacIdx,
			stBSD_PrevBlkPara.cDetectFactorIsr, stBSD_PrevBlkPara.cNextParaBlkIdx, stBSD_PrevBlkPara.uiDacValue,
			stBSD_PrevBlkPara.usDetectSampleLength);

		// Convert to volt: _IN_VOLT_VIA_ADC_FACTOR_, _IN_VOLT_VIA_ADC_OFFSET_
		fprintf(fpData, "%%FbVoltage - 1st Bond(in volt): \n Adc1stBond_v = [");
		if(roya_6412_get_flag_adc_polarity() == ROYA_BSD_ADC_POLARITY_SINGLE)  // ROYA_BSD_ADC_POLARITY_SINGLE
		{
			for(unsigned int ii = 0; ii<stBSD_PrevBlkPara.usDetectSampleLength; ii++)
			{
				fprintf(fpData, "%4.1f\n", __CONVERT_ADC_TO_VOLT_FlOAT(uiDataAdcBSD[ucPrevBlkBSD][ii]));
			}
		}
		else // Bipolar ADC, 20120213
		{
			for(unsigned int ii = 0; ii<stBSD_PrevBlkPara.usDetectSampleLength; ii++)
			{
				fprintf(fpData, "%4.1f\n", __CONVERT_16b_BI_ADC_TO_VOLT_FlOAT(uiDataAdcBSD[ucPrevBlkBSD][ii]));
			}
		}
		fprintf(fpData, "];\n\n");

		sRet = roya_6412_bsd_get_parameters(&stBSD_Para, ucCurrBlkBSD);
		fprintf(fpData, "%%BSD_Blk %d - Curr Parameter: \n %%[AdcIdx, DacIdx, FactorIsr, NextBlkIdx, DacVal, SampleLen] = [%d, %d, %d, %d, %d, %d]\n\n",  ucCurrBlkBSD,
			stBSD_Para.cAdcIdx, stBSD_Para.cDacIdx,
			stBSD_Para.cDetectFactorIsr, stBSD_Para.cNextParaBlkIdx, stBSD_Para.uiDacValue,
			stBSD_Para.usDetectSampleLength);


		fprintf(fpData, "%%FbVoltage - 2nd Bond (in volt):\n Adc2ndBond_v = [");
		if(roya_6412_get_flag_adc_polarity() == ROYA_BSD_ADC_POLARITY_SINGLE)  // ROYA_BSD_ADC_POLARITY_SINGLE
		{
			for(unsigned int ii = 0; ii<stBSD_Para.usDetectSampleLength; ii++)
			{
				fprintf(fpData, "%4.1f\n", __CONVERT_ADC_TO_VOLT_FlOAT(uiDataAdcBSD[ucCurrBlkBSD][ii])); // stBSD_Para.cNextParaBlkIdx
			}
		}
		else  // Bipolar ADC, 20120213
		{
			for(unsigned int ii = 0; ii<stBSD_Para.usDetectSampleLength; ii++)
			{
				fprintf(fpData, "%4.1f\n", __CONVERT_16b_BI_ADC_TO_VOLT_FlOAT(uiDataAdcBSD[ucCurrBlkBSD][ii])); // stBSD_Para.cNextParaBlkIdx
			}
		}
		fprintf(fpData, "];\n\n");

		///////////////////////////////////////////////////////////////////////////////////////
		///////// Original ADC
		fprintf(fpData, "%%Adc Data Read- 1st Bond\n Adc1stBond = [");
		for(unsigned int ii = 0; ii<stBSD_PrevBlkPara.usDetectSampleLength; ii++)
		{
			fprintf(fpData, "%d\n", uiDataAdcBSD[ucPrevBlkBSD][ii]);    // 20100129
		}
		fprintf(fpData, "];\n\n");

		sRet = roya_6412_bsd_get_parameters(&stBSD_Para, ucCurrBlkBSD);
		fprintf(fpData, "%%BSD_Blk - Curr Parameter: \n %%[AdcIdx, DacIdx, FactorIsr, NextBlkIdx, DacVal, SampleLen] = [%d, %d, %d, %d, %d, %d]\n\n", 
			stBSD_Para.cAdcIdx, stBSD_Para.cDacIdx,
			stBSD_Para.cDetectFactorIsr, stBSD_Para.cNextParaBlkIdx, stBSD_Para.uiDacValue,
			stBSD_Para.usDetectSampleLength);


		fprintf(fpData, "%%Adc Data Read- 2nd Bond\n Adc2ndBond = [");
		for(unsigned int ii = 0; ii<stBSD_Para.usDetectSampleLength; ii++)
		{
			fprintf(fpData, "%d\n", uiDataAdcBSD[ucCurrBlkBSD][ii]);  // stBSD_Para.cNextParaBlkIdx
		}
		fprintf(fpData, "];");

		return sRet;
}


short bsd_api_analyze_stickness(BSD_ANALYZE_STICKNESS_INPUT *stpInputAnalyzeBSD, 
									 BSD_ANALYZE_STICKNESS_OUTPUT *stpOutputAnalyzeBSD)
{
    short sRet = BSD_OKEY;

	BSD_PARAMETER stBSD_Para_1stBond, stBSD_Para_2ndBond;
	//BSD_STATUS stStatusBSD;
	unsigned char ucCurrBlkBSD, ucPrevBlkBSD;

	stpOutputAnalyzeBSD->us1stBondStickStatus = BSD_OKEY;
	stpOutputAnalyzeBSD->us2ndBondStickStatus = BSD_OKEY;

	// to save timing, this Blk Idx is from input
	ucPrevBlkBSD = stpInputAnalyzeBSD->ucParaBlk1stBSD;
	ucCurrBlkBSD = stpInputAnalyzeBSD->ucParaBlk2ndBSD;

	sRet = roya_6412_bsd_get_parameters(&stBSD_Para_1stBond, ucPrevBlkBSD);
	if(sRet != MTN_API_OK_ZERO)		return sRet;  // 20100217

	unsigned int uiActualSampleLen;
// not used function, for small amount of data // 	roya_6412_bsd_get_data_from_len(ucPrevBlkBSD, 0, stBSD_Para_1stBond.usDetectSampleLength,  &uiActualSampleLen);
	sRet = roya_6412_bsd_get_data(ucPrevBlkBSD, &uiActualSampleLen);
	if(sRet != MTN_API_OK_ZERO)		return sRet;  // 20100217

	unsigned short us1stBondSkipSample = stpInputAnalyzeBSD->us1stBondSkipSamples;
	float fBSD1stBondStickDieThAdc_v = stpInputAnalyzeBSD->fBSD1stBondStickDieThAdc_v;
	unsigned short us1stBondNumSamplesConsecGreaterDieThAdc = stpInputAnalyzeBSD->us1stBondNumSamplesConsecGreaterDieThAdc;
	unsigned short us1stBondCounterNonStick = 0;
	unsigned short us1stBondStartStickCount = 0;

	if(roya_6412_get_flag_adc_polarity() == ROYA_BSD_ADC_POLARITY_SINGLE)  // ROYA_BSD_ADC_POLARITY_SINGLE
	{
		for(int ii=us1stBondSkipSample; ii<stBSD_Para_1stBond.usDetectSampleLength; ii++)
		{
			if(uiDataAdcBSD[ucPrevBlkBSD][ii] * _IN_VOLT_VIA_ADC_FACTOR_ > fBSD1stBondStickDieThAdc_v)
			{
				us1stBondCounterNonStick ++;
			}
			else
			{
				us1stBondCounterNonStick = 0;
			}

			if(us1stBondCounterNonStick >= us1stBondNumSamplesConsecGreaterDieThAdc)
			{
				us1stBondStartStickCount = ii;
				break;
			}
		}
	}
	else  // Bipolar ADC, 20120213
	{
		for(int ii=us1stBondSkipSample; ii<stBSD_Para_1stBond.usDetectSampleLength; ii++)
		{
			if(__CONVERT_16b_BI_ADC_TO_VOLT_FlOAT(uiDataAdcBSD[ucPrevBlkBSD][ii])  > fBSD1stBondStickDieThAdc_v)
			{
				us1stBondCounterNonStick ++;
			}
			else
			{
				us1stBondCounterNonStick = 0;
			}

			if(us1stBondCounterNonStick >= us1stBondNumSamplesConsecGreaterDieThAdc)
			{
				us1stBondStartStickCount = ii;
				break;
			}
		}
	}
	// Judge whether 1st Bond Stick OK,
	if(us1stBondCounterNonStick >= us1stBondNumSamplesConsecGreaterDieThAdc)
	{
		stpOutputAnalyzeBSD->us1stBondStickStatus = BSD_ERR_1ST_BOND_NON_STICK_DIE;
		stpOutputAnalyzeBSD->us1stBondEndCheckStickness = us1stBondStartStickCount; 
	}
	else
	{
		stpOutputAnalyzeBSD->us1stBondStickStatus = BSD_OKEY;
		stpOutputAnalyzeBSD->us1stBondEndCheckStickness = stBSD_Para_1stBond.usDetectSampleLength; 
	}


	// check the 2nd bond stickness
	if(stpOutputAnalyzeBSD->us1stBondStickStatus != BSD_OKEY)
	{
		stpOutputAnalyzeBSD->us2ndBondStickStatus = BSD_ERR_2ND_BOND_SKIP_1ST_BOND_ERR;
	}

	// 2nd Bond Stickness Detection
	sRet = roya_6412_bsd_get_parameters(&stBSD_Para_2ndBond, ucCurrBlkBSD);
	if(sRet != MTN_API_OK_ZERO)		return sRet; // 20100217

// not used function, for small amount of data//	roya_6412_bsd_get_data_from_len(ucCurrBlkBSD, 0, stBSD_Para_2ndBond.usDetectSampleLength,  &uiActualSampleLen);
	sRet = roya_6412_bsd_get_data(ucCurrBlkBSD, &uiActualSampleLen);
	if(sRet != MTN_API_OK_ZERO)		return sRet; // 20100217

	unsigned short us2ndBondSkipSample = stpInputAnalyzeBSD->us2ndBondSkipSamples;
	float fBSD2ndBondStickLeadThAdc_v = stpInputAnalyzeBSD->fBSD2ndBondStickLeadThAdc_v;
	float fBSD2ndBondNonStickThAdc_v = stpInputAnalyzeBSD->fBSD2ndBondNonStickThAdc_v;
	unsigned short us2ndBondCntrContinuLower = 0;
	unsigned short us2ndBondCntrContinuUpper = 0;
	unsigned short us2ndBondStartStickCount = 0;
	unsigned short us2ndBondNumSamplesConsecLowerLeadThAdc = stpInputAnalyzeBSD->us2ndBondNumSamplesConsecLowerLeadThAdc;
	unsigned short us2ndBondOpenStickCount = 0;

	// (2.2) initialize us2ndBondOpenSticknessFirstRiseCount = 0;
	stpOutputAnalyzeBSD->us2ndBondOpenSticknessFirstRiseCount = 0;
	//if(uiDataAdcBSD[ucCurrBlkBSD][us2ndBondSkipSample] * _IN_VOLT_VIA_ADC_FACTOR_ >= fBSD2ndBondNonStickThAdc_v)
	//{ // if AdcValue >= us2ndBondNonStickThAdc from the very beginning, it is Non-stick
	//	stpOutputAnalyzeBSD->us2ndBondStickStatus = BSD_ERR_2ND_BOND_NON_STICK_LEAD;
	//}
	//else
	//{   // Donot check ADC threshold initially, 20091119
	if(roya_6412_get_flag_adc_polarity() == ROYA_BSD_ADC_POLARITY_SINGLE)  // ROYA_BSD_ADC_POLARITY_SINGLE
	{
		for(int ii=us2ndBondSkipSample; ii<stBSD_Para_2ndBond.usDetectSampleLength; ii++)
		{
			if(uiDataAdcBSD[ucCurrBlkBSD][ii] * _IN_VOLT_VIA_ADC_FACTOR_ <= fBSD2ndBondStickLeadThAdc_v)
			{
				us2ndBondCntrContinuLower ++;
			}
			else
			{
				us2ndBondCntrContinuLower = 0;
			}

			if(us2ndBondCntrContinuLower >= stpInputAnalyzeBSD->us2ndBondNumSamplesConsecLowerLeadThAdc)
			{
				us2ndBondStartStickCount = ii;
				ii = stBSD_Para_2ndBond.usDetectSampleLength; // break the for loop
			}

			// check debouncing for larger counter
			if(uiDataAdcBSD[ucCurrBlkBSD][us2ndBondSkipSample] * _IN_VOLT_VIA_ADC_FACTOR_ >= fBSD2ndBondNonStickThAdc_v)
			{
				us2ndBondCntrContinuUpper ++;
			}
			else
			{
				us2ndBondCntrContinuUpper = 0;
			}
			if(us2ndBondCntrContinuUpper >= stpInputAnalyzeBSD->us2ndBondNumSampleConsecHigher)
			{
				stpOutputAnalyzeBSD->us2ndBondStickStatus = BSD_ERR_2ND_BOND_NON_STICK_LEAD;
				ii = stBSD_Para_2ndBond.usDetectSampleLength; // break the for loop
			}
		}
	}
	else  // Bipolar ADC, 20120213  __CONVERT_16b_BI_ADC_TO_VOLT_FlOAT
	{
		for(int ii=us2ndBondSkipSample; ii<stBSD_Para_2ndBond.usDetectSampleLength; ii++)
		{
			if(__CONVERT_16b_BI_ADC_TO_VOLT_FlOAT(uiDataAdcBSD[ucCurrBlkBSD][ii]) <= fBSD2ndBondStickLeadThAdc_v)
			{
				us2ndBondCntrContinuLower ++;
			}
			else
			{
				us2ndBondCntrContinuLower = 0;
			}

			if(us2ndBondCntrContinuLower >= stpInputAnalyzeBSD->us2ndBondNumSamplesConsecLowerLeadThAdc)
			{
				us2ndBondStartStickCount = ii;
				ii = stBSD_Para_2ndBond.usDetectSampleLength; // break the for loop
			}

			// check debouncing for larger counter
			if(__CONVERT_16b_BI_ADC_TO_VOLT_FlOAT(uiDataAdcBSD[ucCurrBlkBSD][us2ndBondSkipSample]) >= fBSD2ndBondNonStickThAdc_v)
			{
				us2ndBondCntrContinuUpper ++;
			}
			else
			{
				us2ndBondCntrContinuUpper = 0;
			}
			if(us2ndBondCntrContinuUpper >= stpInputAnalyzeBSD->us2ndBondNumSampleConsecHigher)
			{
				stpOutputAnalyzeBSD->us2ndBondStickStatus = BSD_ERR_2ND_BOND_NON_STICK_LEAD;
				ii = stBSD_Para_2ndBond.usDetectSampleLength; // break the for loop
			}
		}
	}
		if(us2ndBondStartStickCount == 0)
		{
			stpOutputAnalyzeBSD->us2ndBondStickStatus = BSD_ERR_2ND_BOND_NON_STICK_LEAD;
			stpOutputAnalyzeBSD->us2ndBondStartSampleStickness = stBSD_Para_2ndBond.usDetectSampleLength;
			stpOutputAnalyzeBSD->us2ndBondOpenSticknessFirstRiseCount = 0;
		}
		else
		{
			stpOutputAnalyzeBSD->us2ndBondStartSampleStickness = us2ndBondStartStickCount;
		}

		// check open stick count
//		if( stpOutputAnalyzeBSD->us2ndBondStickStatus == BSD_OKEY)
//		{
		if(us2ndBondCntrContinuUpper < stpInputAnalyzeBSD->us2ndBondNumSampleConsecHigher)
		{
			stpOutputAnalyzeBSD->us2ndBondOpenSticknessFirstRiseCount = stBSD_Para_2ndBond.usDetectSampleLength; // 20091221
		}
		else
		{
			stpOutputAnalyzeBSD->us2ndBondOpenSticknessFirstRiseCount = 0; // 20091221
		}

		if(roya_6412_get_flag_adc_polarity() == ROYA_BSD_ADC_POLARITY_SINGLE)  // ROYA_BSD_ADC_POLARITY_SINGLE
		{
			us2ndBondOpenStickCount = 0;
			for(int ii=us2ndBondStartStickCount; ii<stBSD_Para_2ndBond.usDetectSampleLength - 1; ii++)
			{
				if(uiDataAdcBSD[ucCurrBlkBSD][ii] * _IN_VOLT_VIA_ADC_FACTOR_< fBSD2ndBondNonStickThAdc_v && 
					uiDataAdcBSD[ucCurrBlkBSD][ii + 1] * _IN_VOLT_VIA_ADC_FACTOR_ > fBSD2ndBondNonStickThAdc_v)
				{
					us2ndBondOpenStickCount ++;
					if( us2ndBondOpenStickCount == 1)
					{  // Only record the first rising edge
						stpOutputAnalyzeBSD->us2ndBondOpenSticknessFirstRiseCount = ii;
					}
				}
			}
			stpOutputAnalyzeBSD->us2ndBondOpenStickCount = us2ndBondOpenStickCount;
		}
		else  // Bipolar ADC, 20120213 __CONVERT_16b_BI_ADC_TO_VOLT_FlOAT
		{
			us2ndBondOpenStickCount = 0;
			for(int ii=us2ndBondStartStickCount; ii<stBSD_Para_2ndBond.usDetectSampleLength - 1; ii++)
			{
				if( __CONVERT_16b_BI_ADC_TO_VOLT_FlOAT(uiDataAdcBSD[ucCurrBlkBSD][ii]) < fBSD2ndBondNonStickThAdc_v && 
					__CONVERT_16b_BI_ADC_TO_VOLT_FlOAT(uiDataAdcBSD[ucCurrBlkBSD][ii + 1] ) >= fBSD2ndBondNonStickThAdc_v)
				{
					us2ndBondOpenStickCount ++;
					if( us2ndBondOpenStickCount == 1)
					{  // Only record the first rising edge
						stpOutputAnalyzeBSD->us2ndBondOpenSticknessFirstRiseCount = ii;
					}
				}
			}
			stpOutputAnalyzeBSD->us2ndBondOpenStickCount = us2ndBondOpenStickCount;
		}

#ifdef  __BSD_DEBUG__
	FILE *fpData;

	fopen_s(&fpData, "BSD_Debug_data.m", "w");

	if(fpData != NULL)
	{
		bsd_write_file_data(fpData, ucPrevBlkBSD, ucCurrBlkBSD);
	}
#endif

	return sRet; // 20100217
//	} // Donot check ADC threshold initially, 20091119
}

short bsd_api_set_parameter_firmware_analyze_stickness(BSD_ANALYZE_STICKNESS_INPUT *stpInputAnalyzeBSD)
{
BSD_FIRMWARE_ANALYZE_STICKNESS_PARAMETER stBSD_InputFirmwareAnalyzeStickness;

//	stBSD_InputFirmwareAnalyzeStickness.ucParaBlk1stBSD = stpInputAnalyzeBSD->ucParaBlk1stBSD;
//	stBSD_InputFirmwareAnalyzeStickness.ucParaBlk2ndBSD = stpInputAnalyzeBSD->ucParaBlk2ndBSD;
	stBSD_InputFirmwareAnalyzeStickness.us1stBondNumSamplesConsecGreaterDieThAdc = stpInputAnalyzeBSD->us1stBondNumSamplesConsecGreaterDieThAdc;
	stBSD_InputFirmwareAnalyzeStickness.us1stBondNumSamplesConsecLowerLeadThAdc = stpInputAnalyzeBSD->us1stBondNumSamplesConsecLowerLeadThAdc;
	stBSD_InputFirmwareAnalyzeStickness.us1stBondSkipSamples = stpInputAnalyzeBSD->us1stBondSkipSamples;
	stBSD_InputFirmwareAnalyzeStickness.us2ndBondNumSampleConsecHigher = stpInputAnalyzeBSD->us2ndBondNumSampleConsecHigher;
	stBSD_InputFirmwareAnalyzeStickness.us2ndBondNumSamplesConsecLowerLeadThAdc = stpInputAnalyzeBSD->us2ndBondNumSamplesConsecLowerLeadThAdc;
	stBSD_InputFirmwareAnalyzeStickness.us2ndBondSkipSamples = stpInputAnalyzeBSD->us2ndBondSkipSamples;
	if(roya_6412_get_flag_dac_bit() == ROYA_BSD_DAC_10b)  // 20120415
	{
		stBSD_InputFirmwareAnalyzeStickness.usBSD1stBondStickDieThAdc = (unsigned short)(stpInputAnalyzeBSD->fBSD1stBondStickDieThAdc_v / _IN_VOLT_VIA_ADC_FACTOR_);
		stBSD_InputFirmwareAnalyzeStickness.usBSD2ndBondNonStickThAdc = (unsigned short)(stpInputAnalyzeBSD->fBSD2ndBondNonStickThAdc_v / _IN_VOLT_VIA_ADC_FACTOR_);
		stBSD_InputFirmwareAnalyzeStickness.usBSD2ndBondStickLeadThAdc = (unsigned short)(stpInputAnalyzeBSD->fBSD2ndBondStickLeadThAdc_v / _IN_VOLT_VIA_ADC_FACTOR_);
	}
	else
	{
		stBSD_InputFirmwareAnalyzeStickness.usBSD1stBondStickDieThAdc = (unsigned short)__CONVERT_VOLT_FlOAT_ADC_TO_16b_BI(stpInputAnalyzeBSD->fBSD1stBondStickDieThAdc_v);
		stBSD_InputFirmwareAnalyzeStickness.usBSD2ndBondNonStickThAdc = (unsigned short)__CONVERT_VOLT_FlOAT_ADC_TO_16b_BI(stpInputAnalyzeBSD->fBSD2ndBondNonStickThAdc_v);
		stBSD_InputFirmwareAnalyzeStickness.usBSD2ndBondStickLeadThAdc = (unsigned short)__CONVERT_VOLT_FlOAT_ADC_TO_16b_BI(stpInputAnalyzeBSD->fBSD2ndBondStickLeadThAdc_v);		
	}

	return roya_6412_bsd_set_firmware_analyze_parameter(&stBSD_InputFirmwareAnalyzeStickness);
}

short bsd_api_get_parameter_firmware_analyze_stickness(BSD_ANALYZE_STICKNESS_INPUT *stpInputAnalyzeBSD)
{
BSD_FIRMWARE_ANALYZE_STICKNESS_PARAMETER stBSD_InputFirmwareAnalyzeStickness;

short sRet = roya_6412_bsd_get_firmware_analyze_parameter(&stBSD_InputFirmwareAnalyzeStickness);

	stpInputAnalyzeBSD->us1stBondNumSamplesConsecGreaterDieThAdc = stBSD_InputFirmwareAnalyzeStickness.us1stBondNumSamplesConsecGreaterDieThAdc;
	stpInputAnalyzeBSD->us1stBondNumSamplesConsecLowerLeadThAdc = stBSD_InputFirmwareAnalyzeStickness.us1stBondNumSamplesConsecLowerLeadThAdc;
	stpInputAnalyzeBSD->us1stBondSkipSamples = stBSD_InputFirmwareAnalyzeStickness.us1stBondSkipSamples;
	stpInputAnalyzeBSD->us2ndBondNumSampleConsecHigher = stBSD_InputFirmwareAnalyzeStickness.us2ndBondNumSampleConsecHigher;
	stpInputAnalyzeBSD->us2ndBondNumSamplesConsecLowerLeadThAdc = stBSD_InputFirmwareAnalyzeStickness.us2ndBondNumSamplesConsecLowerLeadThAdc;
	stpInputAnalyzeBSD->us2ndBondSkipSamples = stBSD_InputFirmwareAnalyzeStickness.us2ndBondSkipSamples;
	if(roya_6412_get_flag_dac_bit() == ROYA_BSD_DAC_10b)  // 20120415
	{
		stpInputAnalyzeBSD->fBSD1stBondStickDieThAdc_v = (float)(stBSD_InputFirmwareAnalyzeStickness.usBSD1stBondStickDieThAdc * _IN_VOLT_VIA_ADC_FACTOR_);
		stpInputAnalyzeBSD->fBSD2ndBondNonStickThAdc_v = (float )(stBSD_InputFirmwareAnalyzeStickness.usBSD2ndBondNonStickThAdc * _IN_VOLT_VIA_ADC_FACTOR_);
		stpInputAnalyzeBSD->fBSD2ndBondStickLeadThAdc_v = (float )(stBSD_InputFirmwareAnalyzeStickness.usBSD2ndBondStickLeadThAdc * _IN_VOLT_VIA_ADC_FACTOR_);
	}
	else
	{
		stpInputAnalyzeBSD->fBSD1stBondStickDieThAdc_v = (float)__CONVERT_16b_BI_ADC_TO_VOLT_FlOAT(stBSD_InputFirmwareAnalyzeStickness.usBSD1stBondStickDieThAdc);
		stpInputAnalyzeBSD->fBSD2ndBondNonStickThAdc_v = (float )__CONVERT_16b_BI_ADC_TO_VOLT_FlOAT(stBSD_InputFirmwareAnalyzeStickness.usBSD2ndBondNonStickThAdc);
		stpInputAnalyzeBSD->fBSD2ndBondStickLeadThAdc_v = (float )__CONVERT_16b_BI_ADC_TO_VOLT_FlOAT(stBSD_InputFirmwareAnalyzeStickness.usBSD2ndBondStickLeadThAdc);
//		* _IN_VOLT_VIA_ADC_FACTOR_BI_16b + _IN_VOLT_VIA_ADC_OFFSET_BI_16b;
	}
	return sRet;
}

short bsd_api_firmware_analyze_stickness(unsigned char ucBSD1stBlk, unsigned char ucBSD2ndBlk, 
									 BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT *stpOutputAnalyzeBSD)
{

	short sRet = roya_6412_bsd_firmware_analyze_stickness(ucBSD1stBlk, ucBSD2ndBlk, stpOutputAnalyzeBSD);

	return sRet;
}

short bsd_api_get_firmware_analyze_stickness_output(BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT *stpOutput)
{
	short sRet = roya_6412_bsd_get_firmware_analyze_output(stpOutput);

	return sRet;
}

