
#ifndef _BSD_DEF_
#define _BSD_DEF_

#ifndef Uint8
typedef unsigned char Uint8;  // BYTE, unsigned __int8
#endif // Uint8

#ifndef Uint16
typedef unsigned short Uint16;  // __int16
#endif // Uint16

typedef unsigned short BSD_ADC_TYPE;

#define _BQM_MAX_DATA_LEN_       8192
#define _BQM_MAX_BLK_   2

#define __CVT_BIPOLAR_2_SINGLE_POLAR_OFST__   32767
typedef struct
{
	Uint8 cIdxCurrBlkBQM;
    int iFlagStartBQM;
	int iCounterBQM;

	int iPreCountBQM_TH[_BQM_MAX_BLK_];
	int iMaxCountBQM_Adc[_BQM_MAX_BLK_];

}BQM_STATUS;

#define _BSD_MAX_DATA_LEN_       512
#define _BSD_MAX_BLK_PARAMETER_   4

typedef struct
{
Uint8 cDetectFactorIsr;
Uint16 usDetectSampleLength;
Uint8 cAdcIdx;
Uint8 cDacIdx;
unsigned int uiDacValue;
Uint8 cDataBufferIdx;
Uint8 cNextParaBlkIdx;
}BSD_PARAMETER;

typedef struct
{
unsigned int uiTrigCnt;
unsigned short usBsdSampleCnt;
char cFlagStartBSD;
char cBlkIdxBSD;	// Uint8 
char cFlagTriggerBSD;  // Uint8
}BSD_STATUS;

typedef struct
{
//	unsigned char  ucParaBlk1stBSD;
//	unsigned char  ucParaBlk2ndBSD;

	unsigned short us1stBondSkipSamples;
	unsigned short usBSD1stBondStickDieThAdc;  // first bond, when AdcFeedback is lower than this value, it means successful sticking to die
	unsigned short us1stBondNumSamplesConsecGreaterDieThAdc;
	unsigned short us1stBondNumSamplesConsecLowerLeadThAdc; // Not used for checking stickness, only for monitoring

	unsigned short usBSD2ndBondStickLeadThAdc;  // 2nd bond, when AdcFeedback is lower than this value, it means successful sticking to lead
	unsigned short us2ndBondSkipSamples;
	unsigned short us2ndBondNumSamplesConsecLowerLeadThAdc;
	unsigned short usBSD2ndBondNonStickThAdc;  // 2nd bond, when AdcFeedback is higher than this value, it means successful non-stickness
	unsigned short us2ndBondNumSampleConsecHigher;
}BSD_FIRMWARE_ANALYZE_STICKNESS_PARAMETER;

typedef struct
{
	unsigned short us1stBondStickStatus;         // 0: OK; Non-zero: Not OK, above error code, BSD_ERR_*
//	unsigned short us1stBondEndCheckStickness;

	unsigned short us2ndBondStickStatus;         // 0: OK; Non-zero: Not OK, above error code, BSD_ERR_*
	unsigned short us2ndBondOpenSticknessFirstRiseCount;
	unsigned short us2ndBondStartSampleStickness;
//	unsigned short us2ndBondOpenStickCount;      // number of times, when open stickness condition satisfied 
}BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT;

#define BSD_OKEY   0

#define BSD_ERR_1ST_BOND_NON_STICK_DIE     1
#define BSD_ERR_2ND_BOND_SKIP_1ST_BOND_ERR 2
#define BSD_ERR_2ND_BOND_NON_STICK_LEAD    3
#define BSD_ERR_2ND_BOND_TAIL_TOO_SHORT    4
#define BSD_FILE_NAME_TOO_SHORT			   5
#define BSD_FILE_NAME_NULL				   6

typedef struct
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
}BSD_CALC_STATISTICS;

#endif // _BSD_DEF_

