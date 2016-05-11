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
#include "math.h"

#include "mtndefin.h"
#include "RTC_Roya_bd_6412.h"
#include "RoyaBd6412.h"
//#include "WinTiming.h"

#include "BSD_DEF.h"
#include "BSD_API.h"

#include "BSD_DLL.h"

extern short bsd_write_file_data(FILE *fpData, unsigned char ucPrevBlkBSD, unsigned char ucCurrBlkBSD);
extern short roya_6412_bsd_get_data(unsigned int uiCurrBlkIdx, unsigned int *uiSampleCnt);

extern unsigned int uiDataAdcBSD[_BSD_MAX_BLK_PARAMETER_][_BSD_MAX_DATA_LEN_];
unsigned int uiDataDebug1stBondAdc[30] = {8128, 7997, 8005, 8006, 7954, 8205, 7978, 7765, 7776, 8917, 7830, 9463, 9007, 10536, 7742, 4247, 4447, 8492, 11267, 7566, 8133, 8060, 7820, 7554, 7502, 7415, 3210, 2777, 2451, 2906};
unsigned int uiDataDebug2ndBondAdc[60] = {2880, 2822, 2840, 3182, 2994, 2997, 2875, 2883, 3282, 2953, 3008, 3055, 2765, 3058, 2918, 2965, 2928, 2961, 3124, 2735, 2729, 2262, 1901, 2481, 6429, 11338, 16254, 20544, 21385, 21455, 21426, 21337, 21256, 21247, 21290, 21377, 21459, 21480, 21490, 21447, 21394, 21362, 21317, 21298, 21314, 21321, 21339, 21360, 21376, 21378, 21393, 21389, 21394, 21422, 21413, 21431, 21421, 21396, 21397, 21385};
void bsd_debug_init_adc_value()
{
	unsigned int ii;
	for(ii = 0; ii<30; ii++)
	{
		uiDataAdcBSD[0][ii] = uiDataDebug1stBondAdc[ii];
		uiDataAdcBSD[2][ii] = uiDataDebug1stBondAdc[ii];
	}
	for(ii = 0; ii<60; ii++)
	{
		uiDataAdcBSD[1][ii] = uiDataDebug2ndBondAdc[ii];
		uiDataAdcBSD[3][ii] = uiDataDebug2ndBondAdc[ii];
	}
}

static int iFlagInitOnceEthernet = 0;
static char strHostName[128];
static char astrIPAddresses[10][16];
#define DEF_ROYA_BOARD_IP_ADDRESS   "192.168.2.5"
static char strLocalIP[16], strRemoteIP[16]; 

short bsd_api_init_once_ethernet()
{
short sRet = BSD_OKEY;

	if( iFlagInitOnceEthernet == 0)
	{
		if(get_local_network_info(strHostName, astrIPAddresses) != BSD_OKEY)
		{
	//		AfxMessageBox("Error Init Local IP information! Roya board communication needs ethernet.");
	//		exit(0);
			sRet = MTN_ROYA_GET_SOCKET_ERR;
		}

		sprintf_s(strLocalIP, 16, "%s", astrIPAddresses[0]);
		sprintf_s(strRemoteIP,16, "%s", DEF_ROYA_BOARD_IP_ADDRESS);

		if(roya_6412_init_eth_socket(strLocalIP, strRemoteIP) != BSD_OKEY)
		{
			sRet = MTN_ROYA_WSA_STARTUP_ERR;
			//AfxMessageBox("Initialization Failure");
		}

#ifdef _DEBUG
	bsd_debug_init_adc_value();
#else
		// Get version
		VERSION_INFO stRoyaVer;
		sRet = bsd_api_get_version_roya_6412(&stRoyaVer); // roya_6412_get_version(&stRoyaVer); 20100403
#endif

		if(sRet == BSD_OKEY)
		{
			iFlagInitOnceEthernet = 1;
		}
	}

	return sRet;
}

short bsd_debug_save_data_file(char *strFilename)
{
short sRet = BSD_OKEY;
	FILE *fpData;
	BSD_STATUS stStatusBSD;
	unsigned int uiCurrBlkBSD, uiPrevBlkBSD;
	unsigned int uiSampleCnt;

	if(strFilename != NULL)
	{
		fopen_s(&fpData, strFilename, "w");
	}

	if(fpData != NULL)
	{
		sRet = roya_6412_bsd_get_status(&stStatusBSD);
		uiCurrBlkBSD = stStatusBSD.cBlkIdxBSD;
		if((char)uiCurrBlkBSD - 1 >= 0)
		{
			uiPrevBlkBSD = uiCurrBlkBSD- 1;
		}
		else
		{
			uiPrevBlkBSD = _BSD_MAX_BLK_PARAMETER_ - 1;
		}
		// Data in Previous Block 
		sRet = roya_6412_bsd_get_data(uiPrevBlkBSD, &uiSampleCnt);
		// Data in Current Block 
		sRet = roya_6412_bsd_get_data(uiCurrBlkBSD, &uiSampleCnt);

		sRet = bsd_write_file_data(fpData, uiPrevBlkBSD, uiCurrBlkBSD);

		fprintf(fpData, "\n%% BSD_TEACH_FLAG_OK    %d\n", BSD_TEACH_FLAG_OK);
		fprintf(fpData, "%% BSD_TEACH_FLAG_NSOP  %d\n", BSD_TEACH_FLAG_NSOP);
		fprintf(fpData, "%% BSD_TEACH_FLAG_NSOL  %d\n", BSD_TEACH_FLAG_NSOL);
		fprintf(fpData, "%% BSD_TEACH_FLAG_TAIL_SHORT   %d\n", BSD_TEACH_FLAG_TAIL_SHORT);

		fprintf(fpData, "ActualSticknessFlag = 0\n");

		fclose(fpData);
	}
	else
	{
		sRet = BSD_FILE_NAME_NULL;
	}

	return sRet;
}

short bsd_api_gen_debug_file(char *strFilenamePrefixBSD, char strDebugBsdData[], int iFilenameLen)
{
	short sRet = BSD_OKEY;

	sRet = bsd_api_init_once_ethernet();
	if(	sRet != BSD_OKEY)
	{
		goto label_bsd_api_gen_debug_file_return;
	}
	if(iFilenameLen < BSD_FILE_NAME_MIN_LEN)
	{
		sRet = BSD_FILE_NAME_TOO_SHORT;
		goto label_bsd_api_gen_debug_file_return;
	}

struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;
	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);
	sprintf_s(strDebugBsdData, iFilenameLen, "%s_bsd_raw_data_%d.%d.%d_%d.%d.m", 
		strFilenamePrefixBSD, stpTime->tm_year + 1900, stpTime->tm_mon + 1, stpTime->tm_mday, 
		stpTime->tm_hour, stpTime->tm_min);

	// Save to file
	bsd_debug_save_data_file(strDebugBsdData);

label_bsd_api_gen_debug_file_return:
	return sRet;
}

double bsd_calc_get_max(double *pfBsdVoltIn, unsigned int nLen)
{
	double dRet = pfBsdVoltIn[0];

	for(unsigned int ii=1; ii<nLen; ii++)
	{
		if(dRet < pfBsdVoltIn[ii])
		{
			dRet = pfBsdVoltIn[ii];
		}
	}

	return dRet;
}

double bsd_calc_get_abs_max(double *pfBsdVoltIn, unsigned int nLen)
{
	double dRet = fabs(pfBsdVoltIn[0]), dTemp;

	for(unsigned int ii=1; ii<nLen; ii++)
	{
		dTemp = fabs(pfBsdVoltIn[ii]);
		if(dRet < dTemp)
		{
			dRet = dTemp;
		}
	}

	return dRet;
}

double bsd_calc_get_min(double *pfBsdVoltIn, unsigned int nLen)
{
	double dRet = pfBsdVoltIn[0];

	for(unsigned int ii=1; ii<nLen; ii++)
	{
		if(dRet > pfBsdVoltIn[ii])
		{
			dRet = pfBsdVoltIn[ii];
		}
	}
	return dRet;
}

double bsd_calc_get_mean(double *pfBsdVoltIn, unsigned int nLen)
{
	double dRet = pfBsdVoltIn[0];
	for(unsigned int ii=1; ii<nLen; ii++)
	{
		dRet = dRet + pfBsdVoltIn[ii];
	}
	dRet = dRet/nLen;
	return dRet;
}

double bsd_calc_get_std(double *pfBsdVoltIn, double fMean, unsigned int nLen)
{
	double dRet = (pfBsdVoltIn[0] - fMean);
	dRet = dRet * dRet;
	for(unsigned int ii=1; ii<nLen; ii++)
	{
		dRet = dRet + (pfBsdVoltIn[ii] - fMean) * (pfBsdVoltIn[ii] - fMean);
	}
	dRet = sqrt(dRet/nLen);

	return dRet;
}

static double afDetectVoltage1stBond_v[_BSD_MAX_DATA_LEN_];  
static double afDetectVoltage2ndBond_v[_BSD_MAX_DATA_LEN_];

void bsd_calc_convert_adc_to_voltage(unsigned int uiBlkIdx1stB, unsigned int uiDataLen1stB, 
									 unsigned int uiBlkIdx2ndB, unsigned int uiDataLen2ndB)
{
unsigned int ii;
	for(ii= 0; ii<uiDataLen1stB; ii++)
	{
		afDetectVoltage1stBond_v[ii] = __CONVERT_ADC_TO_VOLT_FlOAT(uiDataAdcBSD[uiBlkIdx1stB][ii]);
	}
	for(ii= 0; ii<uiDataLen2ndB; ii++)
	{
		afDetectVoltage2ndBond_v[ii] = __CONVERT_ADC_TO_VOLT_FlOAT(uiDataAdcBSD[uiBlkIdx2ndB][ii]);
	}
}

static BSD_CALC_STATISTICS stBsdCalculateStatistic;
short bsd_calc_stickness_parameter(STICKNESS_PARAMETER * stpParameterBSD, 
								   unsigned int uiBlkIdx1stB, unsigned int uiDataLen1stB, 
								   unsigned int uiBlkIdx2ndB, unsigned int uiDataLen2ndB)
{
    double fMean1stBondDieVoltage;
    double fMax1stBondDieVoltage;
    double fMin1stBondDieVoltage;
    double fStd1stBondDieVoltage;

    double fAve2ndBondLeadVoltage;
    double fMin2ndBondLeadVoltage;
    double fMax2ndBondLeadVoltage;
    double fStd2ndBondLeadVoltage;

    double fAve2ndBondOpenVoltage;
    double fMin2ndBondOpenVoltage;
    double fMax2ndBondOpenVoltage;
    double fStd2ndBondOpenVoltage;
	short sRet = BSD_OKEY;

	fMean1stBondDieVoltage = bsd_calc_get_mean(&afDetectVoltage1stBond_v[0], uiDataLen1stB);
	fMax1stBondDieVoltage = bsd_calc_get_max(&afDetectVoltage1stBond_v[0], uiDataLen1stB);
	fMin1stBondDieVoltage = bsd_calc_get_min(&afDetectVoltage1stBond_v[0], uiDataLen1stB);
	fStd1stBondDieVoltage = bsd_calc_get_std(&afDetectVoltage1stBond_v[0], fMean1stBondDieVoltage, uiDataLen1stB);

unsigned int ii, nMaxInit2ndBondLen = 10;
unsigned int nCount = 0;
double fSumInit2ndBondVoltage = 0;
	for(ii = 0; ii< nMaxInit2ndBondLen; ii++)
	{
		if(afDetectVoltage2ndBond_v[ii] < fMax1stBondDieVoltage)
		{
			nCount = nCount + 1;
			fSumInit2ndBondVoltage = fSumInit2ndBondVoltage + afDetectVoltage2ndBond_v[ii];
		}
	}

double fAveInit2ndBondVoltage = fSumInit2ndBondVoltage/(double)nCount;
double fMaxInit2ndBondVoltage = bsd_calc_get_max(&afDetectVoltage2ndBond_v[0], nMaxInit2ndBondLen);
double fStdInit2ndBondVoltage = bsd_calc_get_std(&afDetectVoltage2ndBond_v[0], fAveInit2ndBondVoltage, nMaxInit2ndBondLen);

unsigned int iRisingTime = 0;
	for( ii = 0; ii< uiDataLen2ndB; ii++)
	{
		if(( afDetectVoltage2ndBond_v[ii] > fMaxInit2ndBondVoltage + fStdInit2ndBondVoltage) 
			&&  ( afDetectVoltage2ndBond_v[ii + 1] > fMaxInit2ndBondVoltage + fStdInit2ndBondVoltage) 
			&&  ( afDetectVoltage2ndBond_v[ii + 2] > fMaxInit2ndBondVoltage + fStdInit2ndBondVoltage) 
			&&  ( afDetectVoltage2ndBond_v[ii + 3] > fMaxInit2ndBondVoltage + fStdInit2ndBondVoltage)
			)
		{
		   iRisingTime = ii + 1;  break;
		}
	}

	fAve2ndBondLeadVoltage = bsd_calc_get_mean(&afDetectVoltage2ndBond_v[0], iRisingTime);
	fMin2ndBondLeadVoltage = bsd_calc_get_min(&afDetectVoltage2ndBond_v[0], iRisingTime);
	fMax2ndBondLeadVoltage = bsd_calc_get_max(&afDetectVoltage2ndBond_v[0], iRisingTime);
	fStd2ndBondLeadVoltage = bsd_calc_get_std(&afDetectVoltage2ndBond_v[0], fAve2ndBondLeadVoltage, iRisingTime);

double fAve2ndBondInitOpenVoltage = bsd_calc_get_mean(&afDetectVoltage2ndBond_v[iRisingTime + 2],  8);
double fStd2ndBondInitOpenVoltage = bsd_calc_get_std(&afDetectVoltage2ndBond_v[iRisingTime + 2], fAve2ndBondInitOpenVoltage, 8);

unsigned idxValid2ndBondOpen = uiDataLen2ndB;
	for(ii = (iRisingTime+10); ii< uiDataLen2ndB; ii++)
	{
		if((afDetectVoltage2ndBond_v[ii - 2] < (fAve2ndBondInitOpenVoltage - fStd2ndBondInitOpenVoltage)) && 
			(afDetectVoltage2ndBond_v[ii - 1] < (fAve2ndBondInitOpenVoltage - fStd2ndBondInitOpenVoltage))  && 
			(afDetectVoltage2ndBond_v[ii] < (fAve2ndBondInitOpenVoltage - fStd2ndBondInitOpenVoltage))
				)
		{
			idxValid2ndBondOpen = ii;
			break;
		}
	}
unsigned int uiLen2ndBondOpenVoltage = idxValid2ndBondOpen - (iRisingTime + 2);
	fAve2ndBondOpenVoltage = bsd_calc_get_mean(&afDetectVoltage2ndBond_v[iRisingTime + 2],  uiLen2ndBondOpenVoltage);
	fMin2ndBondOpenVoltage = bsd_calc_get_min(&afDetectVoltage2ndBond_v[iRisingTime + 2],  uiLen2ndBondOpenVoltage);
	fMax2ndBondOpenVoltage = bsd_calc_get_max(&afDetectVoltage2ndBond_v[iRisingTime + 2],  uiLen2ndBondOpenVoltage);
	fStd2ndBondOpenVoltage = bsd_calc_get_std(&afDetectVoltage2ndBond_v[iRisingTime + 2], fAve2ndBondOpenVoltage,  uiLen2ndBondOpenVoltage);

	stBsdCalculateStatistic.fMean1stBondDieVoltage = fMean1stBondDieVoltage;
	stBsdCalculateStatistic.fMax1stBondDieVoltage = fMax1stBondDieVoltage;
	stBsdCalculateStatistic.fMin1stBondDieVoltage = fMin1stBondDieVoltage;
	stBsdCalculateStatistic.fStd1stBondDieVoltage = fStd1stBondDieVoltage;

	stBsdCalculateStatistic.fAve2ndBondLeadVoltage = fAve2ndBondLeadVoltage;
	stBsdCalculateStatistic.fMin2ndBondLeadVoltage = fMin2ndBondLeadVoltage;
	stBsdCalculateStatistic.fMax2ndBondLeadVoltage = fMax2ndBondLeadVoltage;
	stBsdCalculateStatistic.fStd2ndBondLeadVoltage = fStd2ndBondLeadVoltage;

	stBsdCalculateStatistic.fAve2ndBondOpenVoltage = fAve2ndBondOpenVoltage;
	stBsdCalculateStatistic.fMin2ndBondOpenVoltage = fMin2ndBondOpenVoltage;
	stBsdCalculateStatistic.fMax2ndBondOpenVoltage = fMax2ndBondOpenVoltage;
	stBsdCalculateStatistic.fStd2ndBondOpenVoltage = fStd2ndBondOpenVoltage;

	stpParameterBSD->dThresholdDie_v = ((fMax1stBondDieVoltage + fStd1stBondDieVoltage)*1.0 + (fAve2ndBondOpenVoltage - fStd2ndBondOpenVoltage)*9.0)/10.0;
	stpParameterBSD->dThresholdLead_v = ((fMax2ndBondLeadVoltage + fStd2ndBondLeadVoltage)*1.0 + (fAve2ndBondOpenVoltage - fStd2ndBondOpenVoltage)*9.0)/10.0;
	stpParameterBSD->iThreshold_NSOL = iRisingTime;
	stpParameterBSD->iThreshold_TailShort = 0;

	// Add protection, 20111007
	if(stpParameterBSD->dThresholdDie_v > 9.0 || stpParameterBSD->dThresholdDie_v < 5)
	{
		stpParameterBSD->dThresholdDie_v = 7.2;
	}
	if(stpParameterBSD->dThresholdLead_v > 9.0 || stpParameterBSD->dThresholdLead_v < 5)
	{
		stpParameterBSD->dThresholdLead_v = 7.2;
	}
	return sRet;
}
short bsd_api_calc_stickness_parameter(STICKNESS_PARAMETER * stpParameterBSD, int iFlagTeachCurrStickness)
{
	short sRet = BSD_OKEY;

 	sRet = bsd_api_init_once_ethernet();
	if(	sRet != BSD_OKEY)
	{
		goto label_bsd_api_calc_stickness_parameter;
	}


	unsigned int uiSampleCnt1stB, uiSampleCnt2ndB;
	BSD_STATUS stStatusBSD;
	unsigned int uiCurrBlkBSD, uiPrevBlkBSD;

#ifndef _DEBUG

	sRet = roya_6412_bsd_get_status(&stStatusBSD);
	uiCurrBlkBSD = stStatusBSD.cBlkIdxBSD;
	if((char)uiCurrBlkBSD - 1 >= 0)
	{
		uiPrevBlkBSD = uiCurrBlkBSD- 1;
	}
	else
	{
		uiPrevBlkBSD = _BSD_MAX_BLK_PARAMETER_ - 1;
	}
	// Data in Previous Block 
	sRet = roya_6412_bsd_get_data(uiPrevBlkBSD, &uiSampleCnt1stB);
	if(	sRet != BSD_OKEY)
	{
		goto label_bsd_api_calc_stickness_parameter;
	}
	// Data in Current Block 
	sRet = roya_6412_bsd_get_data(uiCurrBlkBSD, &uiSampleCnt2ndB);
	if(	sRet != BSD_OKEY)
	{
		goto label_bsd_api_calc_stickness_parameter;
	}

#else

	uiPrevBlkBSD = 0;	uiSampleCnt1stB = 30; uiCurrBlkBSD = 1; uiSampleCnt2ndB = 60;
#endif

	bsd_calc_convert_adc_to_voltage(uiPrevBlkBSD, uiSampleCnt1stB, uiCurrBlkBSD, uiSampleCnt2ndB);

	sRet = bsd_calc_stickness_parameter(stpParameterBSD, uiPrevBlkBSD, uiSampleCnt1stB, uiCurrBlkBSD, uiSampleCnt2ndB);

label_bsd_api_calc_stickness_parameter:
	return sRet;
}