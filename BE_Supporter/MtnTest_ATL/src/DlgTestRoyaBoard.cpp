// DlgTestRoyaBoard.cpp : implementation file
// 20100403		Zhengyi	Add waveform, 

#include "stdafx.h"
#include "DlgTestRoyaBoard.h"
#include "RoyaBd6412.h"
#include "mtndefin.h"
#include "Roya_Version.h"
#include "usg_blk.h"
#include "MtnTesterResDef.h"

#define TEST_BSD_API
// Real time clock related
#include "rt_clock_def.h"
RTC_TM stRoyaRealTimeClock;

// Local variable 
static BOOL bRoyaLCD_Bit7, bRoyaLCD_Bit6, bRoyaLCD_Bit5, bRoyaLCD_Bit4, bRoyaLCD_Bit3, bRoyaLCD_Bit2, bRoyaLCD_Bit1, bRoyaLCD_Bit0;
static unsigned char ucNumLCD;
static int iFlagDryRunBSD; // 20110805
static BOOL bFlagBSD_CheckByWaveForm = FALSE;
static int iFlagEnableDebugRoyaBoard = 0;
// XLS class 20100403
//#include "XLEzAutomation.h"
//class CXLEzAutomation;

//static	CXLEzAutomation * m_pBSD_WaveformByXls;

// CDlgTestRoyaBoard dialog
static unsigned int uiCurrBlkNo, uiCurrSegNo, uiNextBlkIdx, uiMaxCountTrigProt;
static int iTrigAddress;
static char tempChar[256];
USG_UNIT_SEG stUSG_Seg;
static unsigned int uiStartFlag;
void init_roya_board_usg_blk();

static int iAutoCheckUSG;
static int iAutoCheckADC = TRUE;
static unsigned int uiAdcIdx, uiDacIdx;
static char cTrigPatternViaAcs;
#define TEST_TRIGGER_USG_PATTERN  0x3

#define _USG_MAX_NUM_ADC  4
#define _USG_MAX_NUM_DAC  4

static int aiDacValue[_USG_MAX_NUM_DAC];
static double adDacVolt[_USG_MAX_NUM_DAC];
static unsigned int iCurrAdcValue;

// timer related
static UINT_PTR iTimerIdRoyaTestDlg;

// BSD related
#include "BSD_DEF.h"
#include "BSD_API.h"
static BSD_PARAMETER  stParameterBlkBSD;
static Uint8 ucBlkIdxBSD = 0;
static BSD_STATUS stStatusBSD;
static int iAutoCheckBSD = TRUE;  // 20110805
static unsigned int uiPrevEfoGoodFlag;	

static 	BSD_ANALYZE_STICKNESS_INPUT stInputAnalyzeBSD;
static	BSD_ANALYZE_STICKNESS_OUTPUT stOutputAnalyzeBSD;

// related with download firmware
extern char code[];
static unsigned char *pcDownloadBuffer;
static unsigned int uiDownloadSize;
#define __MAX_FLASH_READ_LEN  96
static int iFlagVerifyDownloadRoyaFirmware;

#define __DEBUG__
// Debug timing related
#ifdef __DEBUG__
static int iFlagCheckEnableTimerAll = 0;
#else
static int iFlagCheckEnableTimerAll = TRUE;  // 20110805
#endif

static unsigned long ulBSD_CommuStartTime, ulBSD_CommuElapseTime, ulBSD_CommuElapseTime_2;
static struct tm stDebugTimeRoyaDlg;
static struct tm *stpDebugTimeRoyaDlg = &stDebugTimeRoyaDlg;
static __time64_t stDebugLongTimeRoyaDlg;

static unsigned char cFlagBSDAnalyzeByFirmware = 0;

// #define BSD_AUTO_START_DETECTION_AFTER_EFO_GOOD
// Function to get local IP address
#include "winsock2.h" // Add 'ws2_32.lib' to your linker options

IMPLEMENT_DYNAMIC(CDlgTestRoyaBoard, CDialog)
CDlgTestRoyaBoard::CDlgTestRoyaBoard(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTestRoyaBoard::IDD, pParent)
{

}

#define DEF_ROYA_BOARD_IP_ADDRESS   "192.168.2.5"

#define  _PERIOD_TIMER_UPDATE_BSD_		100
static unsigned int uiTimerPeriod_ms = _PERIOD_TIMER_UPDATE_BSD_;

char strHostName[128];
char astrIPAddresses[10][16];
#define _USG_MAX_NUM_TIMER 2

extern unsigned int uiDataAdcBSD[_BSD_MAX_BLK_PARAMETER_][_BSD_MAX_DATA_LEN_];

#include "BSD_DLL.h"
char cIdxMaxPercentOutDAC = 3;  	// 20130613
double adMaxPercentOutDAC[5] = {1.0, 0.5, 0.4, 0.2, 0.1};

BOOL CDlgTestRoyaBoard::OnInitDialog()
{
	static char strLocalIP[16], strRemoteIP[16]; 
	CString  cstrSystemPath;
	short sRet;
	CString cstrTemp;

#ifdef _DEBUG
	bsd_debug_init_adc_value();
#endif

	// Language
	SetUserInterfaceLanguage(get_sys_language_option());
	// Init UI
	for(int ii = 0; ii<_USG_MAX_NUM_BLK; ii ++)
	{
		sprintf_s(tempChar, 256, "%d", ii);
		((CComboBox *)GetDlgItem(IDC_ROYA_USG_COMBO_BLK_NO))->InsertString(ii, _T(tempChar));
	}
    // IDC_ROYA_COMBO_TIMER_ID
	for(int ii = 0; ii<_USG_MAX_NUM_TIMER; ii ++)
	{
		sprintf_s(tempChar, 256, "%d", ii);
		((CComboBox *)GetDlgItem(IDC_ROYA_COMBO_TIMER_ID))->InsertString(ii, _T(tempChar));
	}
	// IDC_COMBO_ROYA_ADC_INDEX
	for(int ii = 0; ii<_USG_MAX_NUM_ADC; ii ++)
	{
		sprintf_s(tempChar, 256, "%d", ii);
		((CComboBox *)GetDlgItem(IDC_COMBO_ROYA_ADC_INDEX))->InsertString(ii, _T(tempChar));
	}
	// IDC_COMBO_ROYA_SELECT_DAC_IDX
	for(int ii = 0; ii<_USG_MAX_NUM_DAC; ii ++)
	{
		sprintf_s(tempChar, 256, "%d", ii);
		((CComboBox *)GetDlgItem(IDC_COMBO_ROYA_SELECT_DAC_IDX))->InsertString(ii, _T(tempChar));
		aiDacValue[ii] = 512;
		adDacVolt[ii] = __CONVERT_DAC_TO_VOLT(aiDacValue[ii]);
	}

	sprintf_s(tempChar, 256, "100%%"); 	// 20130613
	((CComboBox *)GetDlgItem(IDC_ROYA_COMBO_DAC_MAX_PERCENT))->InsertString(0, _T(tempChar));
	sprintf_s(tempChar, 256, "50%%");
	((CComboBox *)GetDlgItem(IDC_ROYA_COMBO_DAC_MAX_PERCENT))->InsertString(1, _T(tempChar));
	sprintf_s(tempChar, 256, "40%%");
	((CComboBox *)GetDlgItem(IDC_ROYA_COMBO_DAC_MAX_PERCENT))->InsertString(2, _T(tempChar));
	sprintf_s(tempChar, 256, "20%%");
	((CComboBox *)GetDlgItem(IDC_ROYA_COMBO_DAC_MAX_PERCENT))->InsertString(3, _T(tempChar));
	sprintf_s(tempChar, 256, "10%%");
	((CComboBox *)GetDlgItem(IDC_ROYA_COMBO_DAC_MAX_PERCENT))->InsertString(4, _T(tempChar));
	((CComboBox *)GetDlgItem(IDC_ROYA_COMBO_DAC_MAX_PERCENT))->SetCurSel(cIdxMaxPercentOutDAC);

	uiAdcIdx = uiDacIdx = 0; 
	((CComboBox *)GetDlgItem(IDC_COMBO_ROYA_SELECT_DAC_IDX))->SetCurSel(uiDacIdx);
	((CComboBox *)GetDlgItem(IDC_COMBO_ROYA_ADC_INDEX))->SetCurSel(uiAdcIdx);

	GetDlgItem(IDC_EDIT_ROYA_TEST_DLG_RPT)->SetWindowPos(NULL, 500, 50, 600, 220, SWP_NOZORDER); // 10, 15, 720, 400,  SWP_NOMOVE

	//
// Init communication with Roya-BSD box
	if(get_local_network_info(strHostName, astrIPAddresses) != MTN_API_OK_ZERO)
	{
		AfxMessageBox("Error Init Local IP information! Roya board communication needs ethernet.");
	}

	sprintf_s(strLocalIP, 16, "%s", astrIPAddresses[0]);
	sprintf_s(strRemoteIP,16, "%s", DEF_ROYA_BOARD_IP_ADDRESS);

	if(roya_6412_init_eth_socket(strLocalIP, strRemoteIP) != MTN_API_OK_ZERO)
	{
		AfxMessageBox("Initialization Failure");
	}

	// Get version
	VERSION_INFO stRoyaVer;
	sRet = bsd_api_get_version_roya_6412(&stRoyaVer); // roya_6412_get_version(&stRoyaVer); 20100403
	if(sRet != MTN_API_OK_ZERO) // 20100403
	{
		iFlagEnableDebugRoyaBoard = FALSE;
		UI_ShowWindow_SignalGen_RoyaDlg(FALSE);
		goto label_test_roya_on_init_dlg_;
	}
	InitDacSliderRangePosn(); // 20110822, must be after getting version

	UpdateLCD_BtnText();

	if(roya_6412_get_flag_adc_byte() == ROYA_BSD_ADC_2_BYTE)
	{
		GetDlgItem(IDC_ROYA_STATIC_ADC_NUM_BYTE)->SetWindowTextA(_T("2B ADC"));
	}
	else
	{
		GetDlgItem(IDC_ROYA_STATIC_ADC_NUM_BYTE)->SetWindowTextA(_T("4B ADC"));
	}

	// USG related
	cTrigPatternViaAcs = TEST_TRIGGER_USG_PATTERN;
	UpdateRoyaBoardUsgStartButton();  // ETH_init-1
	init_roya_board_usg_blk();
	iAutoCheckUSG = 0;
//	iAutoCheckADC = 0;
	((CButton *)GetDlgItem(IDC_ROYA_USG_CHECK_AUTO))->SetCheck(iAutoCheckUSG);
	((CButton *)GetDlgItem(IDC_ROYA_ADDA_CHECK_AUTO))->SetCheck(iAutoCheckADC);

	// Signal Generator Related
	if(stRoyaVer.usVerYear >= 2012)
	{
		UI_InitSignalGenerator();
		UI_UpdateBQM_RoyaDlg();
	}
	else
	{
		UI_ShowWindow_SignalGen_RoyaDlg(FALSE);
	}
	// BSD application related
	InitDlgBSD();			// ETH_init-{2-5, 6, 7}
//	iAutoCheckBSD = 0;
	((CButton *)GetDlgItem(IDC_ROYA_BSD_CHECK_AUTO))->SetCheck(iAutoCheckBSD);
	// Debug
	((CButton*)GetDlgItem(IDC_CHECK_CHECK_ROYA_COMMUNICATION_BY_BSD))->SetCheck((int)cFlagBSDAnalyzeByFirmware);
	((CButton*)GetDlgItem(IDC_ROYA_TEST_CHECK_ENABLE_TIMER_ALL))->SetCheck(iFlagCheckEnableTimerAll);

	cstrTemp.Format("%d", bsd_api_get_isr_freq_factor_base_10KHz());
	GetDlgItem(IDC_EDIT_BSD_ISR_FREQ_FACTOR_10KHZ)->SetWindowTextA(cstrTemp);

	InitParameterBSD();

	// related with firmware download
	uiDownloadSize = roya_firmware_get_size();
	cstrTemp.Format("Size: %d Byte", uiDownloadSize);
	GetDlgItem(IDC_ROYA_TEXT_FIRMWARE_SIZE)->SetWindowTextA(cstrTemp);
	GetDlgItem(IDC_ROYA_STATIC_CURR_DOWNLOAD_FILENAME)->SetWindowTextA(_T("a6610.h"));
	pcDownloadBuffer = (unsigned char *)code;
	((CProgressCtrl*)GetDlgItem(IDC_ROYA_DOWNLOAD_PROGRESS))->ShowWindow(FALSE);
	((CButton*)GetDlgItem(IDC_ROYA_CHECK_DOWNLOAD_VERIFY))->SetCheck(iFlagVerifyDownloadRoyaFirmware);

#ifdef TEST_BSD_API
	sRet = roya_api_get_rt_clock(&stRoyaRealTimeClock);
#else
	sRet = roya_6412_get_rt_clock(&stRoyaRealTimeClock);
#endif
	RoyaDlgCheckAndShowDebugMessage(sRet);


	UpdateRoyaDlgRT_Clock();

	// set the check, a flag whether to enable uploading and plot
	((CButton*)GetDlgItem(IDC_ROYA_BSD_FLAG_CHECK_WAVEFORM))->SetCheck(bFlagBSD_CheckByWaveForm);
	((CButton*)GetDlgItem(IDC_ROYA_BSD_FLAG_CHECK_DRY_RUN))->SetCheck(iFlagDryRunBSD);

//	StartTimer(uiTimerPeriod_ms);  20120202

label_test_roya_on_init_dlg_:
	UI_EnableGroupUSG_Items(iFlagEnableDebugRoyaBoard);

	return CDialog::OnInitDialog();
}
static int iDlgRoyaBdShowFlag = 0;
int CDlgTestRoyaBoard::UI_GetShowWindowFlag()
{
	return iDlgRoyaBdShowFlag;
}

void CDlgTestRoyaBoard::ShowWindow(int nCmdShow)
{
//	StartTimer(uiTimerPeriod_ms);
	iDlgRoyaBdShowFlag = 1;
	CDialog::ShowWindow(nCmdShow);
}

CDlgTestRoyaBoard::~CDlgTestRoyaBoard()
{
//	StopTimer();
	iDlgRoyaBdShowFlag = 0;
}

static unsigned int uiCommErrNumber = 1;
CString cstrRoyaDlgDebugText;

void CDlgTestRoyaBoard::RoyaDlgCheckAndShowDebugMessage(short sRetCode)
{
	if(sRetCode == MTN_ROYA_COMM_REPLY_LEN_ERROR
		|| sRetCode == MTN_ROYA_COMM_PRE_CHECK_ERROR) // ONly detect some specifie error, communication delay
	{
		
		_time64(&stDebugLongTimeRoyaDlg);
		_localtime64_s(stpDebugTimeRoyaDlg, &stDebugLongTimeRoyaDlg);

		if(uiCommErrNumber == 1)
		{
			if(get_sys_language_option() == LANGUAGE_UI_EN)
			{
				cstrRoyaDlgDebugText.Format("Commu Error: #%d,  ErrorCode:%d, Hour:%d, Min:%d \r\n", uiCommErrNumber, sRetCode, stpDebugTimeRoyaDlg->tm_hour, stpDebugTimeRoyaDlg->tm_min);
			}
			else
			{
				cstrRoyaDlgDebugText.Format("通讯错误: #%d,  ErrorCode:%d, Hour:%d, Min:%d \r\n", uiCommErrNumber, sRetCode, stpDebugTimeRoyaDlg->tm_hour, stpDebugTimeRoyaDlg->tm_min);
			}
		}
		else
		{
			if(get_sys_language_option() == LANGUAGE_UI_EN)
			{
				cstrRoyaDlgDebugText.AppendFormat("Commu Error: #%d:  CommuErrorCode:%d, Hour:%d, Min:%d \r\n", uiCommErrNumber, sRetCode, stpDebugTimeRoyaDlg->tm_hour, stpDebugTimeRoyaDlg->tm_min);
			}
			else
			{
				cstrRoyaDlgDebugText.AppendFormat("通讯错误 #%d:  CommuErrorCode:%d, Hour:%d, Min:%d \r\n", uiCommErrNumber, sRetCode, stpDebugTimeRoyaDlg->tm_hour, stpDebugTimeRoyaDlg->tm_min);
			}
		}
		uiCommErrNumber ++;
		//

//		AfxMessageBox(cstrRoyaDlgDebugText);
		GetDlgItem(IDC_EDIT_ROYA_TEST_DLG_RPT)->SetWindowTextA(cstrRoyaDlgDebugText);

	}
}

void CDlgTestRoyaBoard::DlgTimerEvent()
{
	if(iFlagCheckEnableTimerAll)
	{
#ifndef _DEBUG_NO_HARDWARE_
		// USG related
		if(iAutoCheckUSG)
		{
			UpdateRoyaBoardUsgStartButton();
		}

		// ADC related
		if(iAutoCheckADC)  // iAutoCheckBSD, iAutoCheckADC
		{
			short sRet = roya_6412_comm_test_get_adc(uiAdcIdx, &iCurrAdcValue);

			RoyaDlgCheckAndShowDebugMessage(sRet);

	//		sprintf_s(tempChar, 256, "%d", iCurrAdcValue);
			if(roya_6412_get_flag_adc_polarity() == ROYA_BSD_ADC_BI_POLAR) // 20110822
			{
				sprintf_s(tempChar, 256, "%6.1f", (double)__CONVERT_16b_BI_ADC_TO_VOLT_FlOAT(iCurrAdcValue)); // __CONVERT_ADC_TO_VOLT_FlOAT(iCurrAdcValue));
			}
			else
			{
				sprintf_s(tempChar, 256, "%6.1f", (double)__CONVERT_ADC_TO_VOLT_FlOAT(iCurrAdcValue)); // __CONVERT_ADC_TO_VOLT_FlOAT(iCurrAdcValue));
			}
			GetDlgItem(IDC_STATIC_SHOW_ROYA_ADC_VOLT_VALUE)->SetWindowTextA(_T(tempChar));

			// IDC_STATIC_SHOW_ROYA_ADC_VALUE_16b
			sprintf_s(tempChar, 256, "%d", iCurrAdcValue); // __CONVERT_ADC_TO_VOLT_FlOAT(iCurrAdcValue));
			GetDlgItem(IDC_STATIC_SHOW_ROYA_ADC_VALUE_16b)->SetWindowTextA(_T(tempChar));
			static unsigned int uiCounterGetRealTimeClock = 0;  // Get real time clock every 3 second
			uiCounterGetRealTimeClock ++;
			if((uiCounterGetRealTimeClock % 36000) == 0)
			{
	#ifdef TEST_BSD_API
				sRet = roya_api_get_rt_clock(&stRoyaRealTimeClock);
	#else
				sRet = roya_6412_get_rt_clock(&stRoyaRealTimeClock);
	#endif
			}

			RoyaDlgCheckAndShowDebugMessage(sRet);

			UpdateRoyaDlgRT_Clock();
	}

	// BSD related
	if(iAutoCheckBSD)
	{
		UpdateStatusDlgBSD();
	}
#endif
	}

	if(iFlagDryRunBSD == 1)
	{
		GetDlgItem(IDC_EDIT_ROYA_TEST_DLG_RPT)->ShowWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_ROYA_TEST_DLG_RPT)->ShowWindow(FALSE);
	}

}
void CDlgTestRoyaBoard::OnTimer(UINT nIDEvent)
{
	// void CDlgTestRoyaBoard::DlgTimerEvent()
	CDialog::OnTimer(nIDEvent);
}

void CDlgTestRoyaBoard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgTestRoyaBoard, CDialog)
	ON_BN_CLICKED(IDC_ROYA_TEST_BUTTON_SEND_COMMTEST, &CDlgTestRoyaBoard::OnBnClickedRoyaTestButtonSendCommtest)
	ON_BN_CLICKED(IDC_BUTTON_ROYA_GET_VERSION, &CDlgTestRoyaBoard::OnBnClickedButtonRoyaGetVersion)
	ON_BN_CLICKED(IDC_ROYA_LCD_BIT7, &CDlgTestRoyaBoard::OnBnClickedRoyaLcdBit7)
	ON_BN_CLICKED(IDC_ROYA_LCD_BIT6, &CDlgTestRoyaBoard::OnBnClickedRoyaLcdBit6)
	ON_BN_CLICKED(IDC_ROYA_LCD_BIT5, &CDlgTestRoyaBoard::OnBnClickedRoyaLcdBit5)
	ON_BN_CLICKED(IDC_ROYA_LCD_BIT4, &CDlgTestRoyaBoard::OnBnClickedRoyaLcdBit4)
	ON_BN_CLICKED(IDC_ROYA_LCD_BIT3, &CDlgTestRoyaBoard::OnBnClickedRoyaLcdBit3)
	ON_BN_CLICKED(IDC_ROYA_LCD_BIT2, &CDlgTestRoyaBoard::OnBnClickedRoyaLcdBit2)
	ON_BN_CLICKED(IDC_ROYA_LCD_BIT1, &CDlgTestRoyaBoard::OnBnClickedRoyaLcdBit1)
	ON_BN_CLICKED(IDC_ROYA_LCD_BIT0, &CDlgTestRoyaBoard::OnBnClickedRoyaLcdBit0)
	ON_BN_CLICKED(IDC_BTN_ROYA_TEST_LCD, &CDlgTestRoyaBoard::OnBnClickedBtnRoyaTestLcd)
	ON_CBN_SELCHANGE(IDC_ROYA_USG_COMBO_BLK_NO, &CDlgTestRoyaBoard::OnCbnSelchangeRoyaUsgComboBlkNo)
	ON_EN_CHANGE(IDC_ROYA_USG_EDIT_SEG_NO, &CDlgTestRoyaBoard::OnEnChangeRoyaUsgEditSegNo)
	ON_BN_CLICKED(IDC_ROYA_BUTTON_SET_USG_SEG, &CDlgTestRoyaBoard::OnBnClickedRoyaButtonSetUsgSeg)
	ON_EN_CHANGE(IDC_ROYA_USG_EDIT_NEXT_BLK_IDX, &CDlgTestRoyaBoard::OnEnChangeRoyaUsgEditNextBlkIdx)
	ON_EN_CHANGE(IDC_ROYA_USG_EDIT_TRIG_ADDR, &CDlgTestRoyaBoard::OnEnChangeRoyaUsgEditTrigAddr)
	ON_EN_CHANGE(IDC_ROYA_USG_EDIT_MAX_COUNT_TRIG_PROT, &CDlgTestRoyaBoard::OnEnChangeRoyaUsgEditMaxCountTrigProt)
	ON_BN_CLICKED(IDC_ROYA_BOARD_SET_TIMER_PERIOD, &CDlgTestRoyaBoard::OnBnClickedRoyaBoardSetTimerPeriod)
	ON_BN_CLICKED(IDC_START_ROYA_USG_PROF_FSM, &CDlgTestRoyaBoard::OnBnClickedStartRoyaUsgProfFsm)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_WARM_RESET_ROYA_USG, &CDlgTestRoyaBoard::OnBnClickedButtonWarmResetRoyaUsg)
	ON_EN_CHANGE(IDC_EDIT_TRIG_PATTERN_ROYA_USG_BY_ACS, &CDlgTestRoyaBoard::OnEnChangeEditTrigPatternRoyaUsgByAcs)
	ON_BN_CLICKED(IDC_BUTTON_TRIG_ROYA_USG_VIA_ACS, &CDlgTestRoyaBoard::OnBnClickedButtonTrigRoyaUsgViaAcs)
	ON_CBN_SELCHANGE(IDC_COMBO_ROYA_SELECT_DAC_IDX, &CDlgTestRoyaBoard::OnCbnSelchangeComboRoyaSelectDacIdx)
	ON_EN_CHANGE(IDC_EDIT_ROYA_DAC_VALUE, &CDlgTestRoyaBoard::OnEnChangeEditRoyaDacValue)
	ON_BN_CLICKED(IDC_BUTTON_ROYA_SET_DAC, &CDlgTestRoyaBoard::OnBnClickedButtonRoyaSetDac)
	ON_CBN_SELCHANGE(IDC_COMBO_ROYA_ADC_INDEX, &CDlgTestRoyaBoard::OnCbnSelchangeComboRoyaAdcIndex)
	ON_CBN_SELCHANGE(IDC_ROYA_BSD_BLK_IDX, &CDlgTestRoyaBoard::OnCbnSelchangeRoyaNsdBlkIdx)
	ON_EN_CHANGE(IDC_ROYA_BSD_EDIT_FACTOR_ISR, &CDlgTestRoyaBoard::OnEnChangeRoyaNsdEditFactorIsr)
	ON_EN_CHANGE(IDC_ROYA_BSD_EDIT_SAMPLE_LEN, &CDlgTestRoyaBoard::OnEnChangeRoyaNsdEditSampleLen)
	ON_EN_CHANGE(IDC_ROYA_BSD_EDIT_ADC_IDX, &CDlgTestRoyaBoard::OnEnChangeRoyaNsdEditAdcIdx)
	ON_EN_CHANGE(IDC_ROYA_BSD_EDIT_DAC_IDX, &CDlgTestRoyaBoard::OnEnChangeRoyaNsdEditDacIdx)
	ON_EN_CHANGE(IDC_ROYA_BSD_EDIT_DAC_VALUE, &CDlgTestRoyaBoard::OnEnChangeRoyaNsdEditDacValue)
	ON_BN_CLICKED(IDC_ROYA_BSD_BUTTON_SET_BLK_PARA, &CDlgTestRoyaBoard::OnBnClickedRoyaNsdButtonSetBlkPara)
	ON_BN_CLICKED(IDC_ROYA_BSD_BUTTON_START_W_BLK, &CDlgTestRoyaBoard::OnBnClickedRoyaNsdButtonStartWBlk)
	ON_BN_CLICKED(IDC_ROYA_BSD_BUTTON_WARM_RESET, &CDlgTestRoyaBoard::OnBnClickedRoyaNsdButtonWarmReset)
	ON_BN_CLICKED(IDC_ROYA_BSD_BUTTON_GET_DATA, &CDlgTestRoyaBoard::OnBnClickedRoyaNsdButtonGetData)
	ON_BN_CLICKED(IDC_ROYA_USG_CHECK_AUTO, &CDlgTestRoyaBoard::OnBnClickedRoyaUsgCheckAuto)
	ON_BN_CLICKED(IDC_ROYA_ADDA_CHECK_AUTO, &CDlgTestRoyaBoard::OnBnClickedRoyaAddaCheckAuto)
	ON_BN_CLICKED(IDC_ROYA_BSD_CHECK_AUTO, &CDlgTestRoyaBoard::OnBnClickedRoyaNsdCheckAuto)
	ON_BN_CLICKED(IDC_ROYA_BSD_BUTTON_API_SET_BLK_PARA, &CDlgTestRoyaBoard::OnBnClickedRoyaNsdButtonApiSetBlkPara)
	ON_BN_CLICKED(IDC_ROYA_BSD_BUTTON_API_GET_BLK_PARA, &CDlgTestRoyaBoard::OnBnClickedRoyaNsdButtonApiGetBlkPara)
	ON_EN_CHANGE(IDC_EDIT_ROYA_RTC_YEAR, &CDlgTestRoyaBoard::OnEnChangeEditRoyaRtcYear)
	ON_EN_CHANGE(IDC_EDIT_ROYA_RTC_MONTH, &CDlgTestRoyaBoard::OnEnChangeEditRoyaRtcMonth)
	ON_EN_CHANGE(IDC_EDIT_ROYA_RTC_MDATE, &CDlgTestRoyaBoard::OnEnChangeEditRoyaRtcMdate)
	ON_EN_CHANGE(IDC_EDIT_ROYA_RTC_WDAY, &CDlgTestRoyaBoard::OnEnChangeEditRoyaRtcWday)
	ON_EN_CHANGE(IDC_EDIT_ROYA_RTC_HOUR, &CDlgTestRoyaBoard::OnEnChangeEditRoyaRtcHour)
	ON_EN_CHANGE(IDC_EDIT_ROYA_RTC_MIN, &CDlgTestRoyaBoard::OnEnChangeEditRoyaRtcMin)
	ON_EN_CHANGE(IDC_EDIT_ROYA_RTC_SECOND, &CDlgTestRoyaBoard::OnEnChangeEditRoyaRtcSecond)
	ON_BN_CLICKED(IDC_BUTTON_SET_ROYA_RT_CLOCK, &CDlgTestRoyaBoard::OnBnClickedButtonSetRoyaRtClock)
	ON_BN_CLICKED(IDOK, &CDlgTestRoyaBoard::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgTestRoyaBoard::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_ROYA_COMBO_TIMER_ID, &CDlgTestRoyaBoard::OnCbnSelchangeRoyaComboTimerId)
	ON_EN_CHANGE(IDC_EDIT_ROYA_BSD_DEBOUNCE_1ST_BOND, &CDlgTestRoyaBoard::OnEnChangeEditRoyaBsdDebounce1stBond)
	ON_EN_CHANGE(IDC_EDIT_ROYA_BSD_DEBOUNCE_2ND_BOND, &CDlgTestRoyaBoard::OnEnChangeEditRoyaBsdDebounce2ndBond)
	ON_EN_CHANGE(IDC_EDIT_ROYA_BSD_THRE_HOLD_V_1ST_BOND, &CDlgTestRoyaBoard::OnEnChangeEditRoyaBsdThreHoldV1stBond)
	ON_EN_CHANGE(IDC_EDIT_ROYA_BSD_THRE_HOLD_V_2ND_BOND, &CDlgTestRoyaBoard::OnEnChangeEditRoyaBsdThreHoldV2ndBond)
	ON_EN_CHANGE(IDC_EDIT_ROYA_BSD_DEBOUNCE_1ST_BOND_CONSE_LOW, &CDlgTestRoyaBoard::OnEnChangeEditRoyaBsdDebounce1stBondConseLow)
	ON_EN_CHANGE(IDC_EDIT_ROYA_BSD_DEBOUNCE_2ND_BOND_CONSE_LOW, &CDlgTestRoyaBoard::OnEnChangeEditRoyaBsdDebounce2ndBondConseLow)
	ON_EN_CHANGE(IDC_EDIT_ROYA_BSD_SKIP_SAMPLE_1ST_BOND, &CDlgTestRoyaBoard::OnEnChangeEditRoyaBsdSkipSample1stBond)
	ON_EN_CHANGE(IDC_EDIT_ROYA_BSD_SKIP_SAMPLE_2ND_BOND, &CDlgTestRoyaBoard::OnEnChangeEditRoyaBsdSkipSample2ndBond)
	ON_EN_CHANGE(IDC_EDIT_ROYA_BSD_THRE_HOLD_V_LOWER_LEAD_2ND_BOND, &CDlgTestRoyaBoard::OnEnChangeEditRoyaBsdThreHoldVLowerLead2ndBond)
	ON_BN_CLICKED(IDC_ROYA_BSD_BUTTON_API_SET_ANALYZE_STICKNESS_PARA, &CDlgTestRoyaBoard::OnBnClickedRoyaBsdButtonApiSetAnalyzeSticknessPara)
	ON_BN_CLICKED(IDC_ROYA_BSD_BUTTON_API_GET_ANALYZE_STICKNESS_PARA, &CDlgTestRoyaBoard::OnBnClickedRoyaBsdButtonApiGetAnalyzeSticknessPara)
	ON_BN_CLICKED(IDC_ROYA_BSD_BUTTON_GET_FIRMWARE_ANALYZE, &CDlgTestRoyaBoard::OnBnClickedRoyaBsdButtonGetFirmwareAnalyze)
	ON_BN_CLICKED(IDC_ROYA_BSD_BUTTON_FIRMWARE_ANALYZE_STICKNESS, &CDlgTestRoyaBoard::OnBnClickedRoyaBsdButtonFirmwareAnalyzeStickness)
	ON_BN_CLICKED(IDC_CHECK_CHECK_ROYA_COMMUNICATION_BY_BSD, &CDlgTestRoyaBoard::OnBnClickedCheckCheckRoyaCommunicationByBsd)
	ON_BN_CLICKED(IDC_ROYA_TEST_CHECK_ENABLE_TIMER_ALL, &CDlgTestRoyaBoard::OnBnClickedRoyaTestCheckEnableTimerAll)
	ON_BN_CLICKED(IDC_ROYA_BUTTON_DOWNLOAD_FIRMWARE, &CDlgTestRoyaBoard::OnBnClickedRoyaButtonDownloadFirmware)
	ON_BN_CLICKED(IDC_ROYA_BUTTON_LOAD_FIRMWARE_BIN, &CDlgTestRoyaBoard::OnBnClickedRoyaButtonLoadFirmwareBin)
	ON_BN_CLICKED(IDC_ROYA_CHECK_DOWNLOAD_VERIFY, &CDlgTestRoyaBoard::OnBnClickedRoyaCheckDownloadVerify)
	ON_BN_CLICKED(IDC_ROYA_BSD_FLAG_CHECK_WAVEFORM, &CDlgTestRoyaBoard::OnBnClickedRoyaBsdFlagCheckWaveform)
//	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_SET_DAC_VOLT, &CDlgTestRoyaBoard::OnNMCustomdrawSliderSetDacVolt)
//ON_NOTIFY(NM_THEMECHANGED, IDC_SLIDER_ROYA_SET_DAC_VOLT, &CDlgTestRoyaBoard::OnNMThemeChangedSliderRoyaSetDacVolt)
//ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_ROYA_SET_DAC_VOLT, &CDlgTestRoyaBoard::OnTRBNThumbPosChangingSliderRoyaSetDacVolt)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_ROYA_SET_DAC_VOLT, &CDlgTestRoyaBoard::OnNMCustomdrawSliderRoyaSetDacVolt)
ON_BN_CLICKED(IDC_ROYA_BSD_FLAG_CHECK_DRY_RUN, &CDlgTestRoyaBoard::OnBnClickedRoyaBsdFlagCheckDryRun)
ON_BN_CLICKED(IDC_ROYA_BSD_BUTTON_E_TEST_WIRING, &CDlgTestRoyaBoard::OnBnClickedRoyaBsdButtonETestWiring)
ON_BN_CLICKED(IDC_CHECK_ROYA_BSD_DEBUG, &CDlgTestRoyaBoard::OnBnClickedCheckRoyaBsdDebug)
//ON_BN_CLICKED(IDC_BUTTON_ROYA_BSD_DLG_TEST_PING_START_DEAD, &CDlgTestRoyaBoard::OnBnClickedButtonRoyaBsdDlgTestPingStartDead)
ON_STN_CLICKED(IDC_STATIC_ROYA_USG_PROF_DURATION, &CDlgTestRoyaBoard::OnStnClickedStaticRoyaUsgProfDuration)
//ON_CBN_SELCHANGE(IDC_COMBO_ROYA_SIG_GEN_SINE_OUT_FREQ, &CDlgTestRoyaBoard::OnCbnSelchangeComboRoyaSigGenSineOutFreq)
ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_ROYA_SIG_GEN_SINE_OUT_AMP_DAC, &CDlgTestRoyaBoard::OnNMReleasedcaptureSliderRoyaSigGenSineOutAmpDac)
ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_ROYA_SET_DAC_VOLT, &CDlgTestRoyaBoard::OnNMReleasedcaptureSliderRoyaSetDacVolt)
ON_EN_KILLFOCUS(IDC_EDIT_ROYA_SIG_SINE_OUT_GEN_CYCLE, &CDlgTestRoyaBoard::OnEnKillfocusEditRoyaSigSineOutGenCycle)
ON_EN_KILLFOCUS(IDC_EDIT_ROYA_SIG_SINE_OUT_GEN_FRAC_CYCLE, &CDlgTestRoyaBoard::OnEnKillfocusEditRoyaSigSineOutGenFracCycle)
ON_EN_KILLFOCUS(IDC_EDIT_ROYA_BQM_PRE_COUNTER_TH_1, &CDlgTestRoyaBoard::OnEnKillfocusEditRoyaBqmPreCounterTh1)
ON_EN_KILLFOCUS(IDC_EDIT_ROYA_BQM_PRE_COUNTER_TH_2, &CDlgTestRoyaBoard::OnEnKillfocusEditRoyaBqmPreCounterTh2)
ON_EN_KILLFOCUS(IDC_EDIT_ROYA_BQM_MAX_COUNTER_ADC_1, &CDlgTestRoyaBoard::OnEnKillfocusEditRoyaBqmMaxCounterAdc1)
ON_EN_KILLFOCUS(IDC_EDIT_ROYA_BQM_MAX_COUNTER_ADC_2, &CDlgTestRoyaBoard::OnEnKillfocusEditRoyaBqmMaxCounterAdc2)
ON_CBN_SELCHANGE(IDC_ROYA_COMBO_DAC_MAX_PERCENT, &CDlgTestRoyaBoard::OnCbnSelchangeRoyaComboDacMaxPercent)
ON_EN_KILLFOCUS(IDC_EDIT_ROYA_STEP_CMD_DAC2_V1, &CDlgTestRoyaBoard::OnEnKillfocusEditRoyaStepCmdDac2V1)
ON_EN_KILLFOCUS(IDC_EDIT_ROYA_STEP_CMD_DAC2_V2, &CDlgTestRoyaBoard::OnEnKillfocusEditRoyaStepCmdDac2V2)
ON_EN_KILLFOCUS(IDC_EDIT_ROYA_STEP_CMD_DAC2_PROT_V, &CDlgTestRoyaBoard::OnEnKillfocusEditRoyaStepCmdDac2ProtV)
ON_CBN_SELCHANGE(IDC_COMBO_ROYA_STEP_CMD_DAC2_T, &CDlgTestRoyaBoard::OnCbnSelchangeComboRoyaStepCmdDac2T)
ON_BN_CLICKED(IDC_CHECK_ROYA_STEP_CMD_DAC2_RUN, &CDlgTestRoyaBoard::OnBnClickedCheckRoyaStepCmdDac2Run)
END_MESSAGE_MAP()


// CDlgTestRoyaBoard message handlers


// IDC_ROYA_STATIC_VERSION
void CDlgTestRoyaBoard::OnBnClickedButtonRoyaGetVersion() // 20100403
{
	VERSION_INFO stRoyaVer;
	short sRet = 0;
	
	sRet = bsd_api_get_version_roya_6412(&stRoyaVer); // roya_6412_get_version(&stRoyaVer); 20100403

	if(sRet == MTN_API_OK_ZERO)
	{
		sprintf_s(tempChar, 256, "Ver: %d.%d, Y-%d, M-%d, D-%d", 
			stRoyaVer.usVerMajor, stRoyaVer.usVerMinor,
			stRoyaVer.usVerYear, stRoyaVer.usVerMonth, stRoyaVer.usVerDate);
		GetDlgItem(IDC_ROYA_STATIC_VERSION)->SetWindowTextA(_T(tempChar));
	}
	else
	{
		RoyaDlgCheckAndShowDebugMessage(sRet);
	}
//	return sRet;
}

void CDlgTestRoyaBoard::OnBnClickedRoyaLcdBit7()
{ // IDC_ROYA_LCD_BIT7
	bRoyaLCD_Bit7 = !bRoyaLCD_Bit7; 
	UpdateLCD_BtnText();
}
void CDlgTestRoyaBoard::OnBnClickedRoyaLcdBit6()
{ // IDC_ROYA_LCD_BIT6
	bRoyaLCD_Bit6 = !bRoyaLCD_Bit6; 
	UpdateLCD_BtnText();
}
void CDlgTestRoyaBoard::OnBnClickedRoyaLcdBit5()
{ // IDC_ROYA_LCD_BIT5
	bRoyaLCD_Bit5 = !bRoyaLCD_Bit5; 
	UpdateLCD_BtnText();
}

void CDlgTestRoyaBoard::OnBnClickedRoyaLcdBit4()
{ // IDC_ROYA_LCD_BIT4
	bRoyaLCD_Bit4 = !bRoyaLCD_Bit4; 
	UpdateLCD_BtnText();
}

void CDlgTestRoyaBoard::OnBnClickedRoyaLcdBit3()
{ // IDC_ROYA_LCD_BIT3
	bRoyaLCD_Bit3 = !bRoyaLCD_Bit3; 
	UpdateLCD_BtnText();
}

void CDlgTestRoyaBoard::OnBnClickedRoyaLcdBit2()
{ // IDC_ROYA_LCD_BIT2
	bRoyaLCD_Bit2 = !bRoyaLCD_Bit2; 
	UpdateLCD_BtnText();
}

void CDlgTestRoyaBoard::OnBnClickedRoyaLcdBit1()
{ // IDC_ROYA_LCD_BIT1
	bRoyaLCD_Bit1 = !bRoyaLCD_Bit1; 
	UpdateLCD_BtnText();
}

void CDlgTestRoyaBoard::OnBnClickedRoyaLcdBit0()
{ // IDC_ROYA_LCD_BIT0
	bRoyaLCD_Bit0 = !bRoyaLCD_Bit0; 
	UpdateLCD_BtnText();
}

void CDlgTestRoyaBoard::UpdateLCD_BtnText()
{
	if(bRoyaLCD_Bit7)
	{
		GetDlgItem(IDC_ROYA_LCD_BIT7)->SetWindowTextA(_T("7"));
	}
	else
	{
		GetDlgItem(IDC_ROYA_LCD_BIT7)->SetWindowTextA(_T("0"));
	}

	if(bRoyaLCD_Bit6)
	{
		GetDlgItem(IDC_ROYA_LCD_BIT6)->SetWindowTextA(_T("6"));
	}
	else
	{
		GetDlgItem(IDC_ROYA_LCD_BIT6)->SetWindowTextA(_T("0"));
	}
	if(bRoyaLCD_Bit5)
	{
		GetDlgItem(IDC_ROYA_LCD_BIT5)->SetWindowTextA(_T("5"));
	}
	else
	{
		GetDlgItem(IDC_ROYA_LCD_BIT5)->SetWindowTextA(_T("0"));
	}
	if(bRoyaLCD_Bit4)
	{
		GetDlgItem(IDC_ROYA_LCD_BIT4)->SetWindowTextA(_T("4"));
	}
	else
	{
		GetDlgItem(IDC_ROYA_LCD_BIT4)->SetWindowTextA(_T("0"));
	}
	if(bRoyaLCD_Bit3)
	{
		GetDlgItem(IDC_ROYA_LCD_BIT3)->SetWindowTextA(_T("3"));
	}
	else
	{
		GetDlgItem(IDC_ROYA_LCD_BIT3)->SetWindowTextA(_T("0"));
	}
	if(bRoyaLCD_Bit2)
	{
		GetDlgItem(IDC_ROYA_LCD_BIT2)->SetWindowTextA(_T("2"));
	}
	else
	{
		GetDlgItem(IDC_ROYA_LCD_BIT2)->SetWindowTextA(_T("0"));
	}
	if(bRoyaLCD_Bit1)
	{
		GetDlgItem(IDC_ROYA_LCD_BIT1)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_ROYA_LCD_BIT1)->SetWindowTextA(_T("0"));
	}
	if(bRoyaLCD_Bit0)
	{
		GetDlgItem(IDC_ROYA_LCD_BIT0)->SetWindowTextA(_T("0"));
	}
	else
	{
		GetDlgItem(IDC_ROYA_LCD_BIT0)->SetWindowTextA(_T(""));
	}

	ucNumLCD = 128 * bRoyaLCD_Bit7 + 64 * bRoyaLCD_Bit6 + 32 * bRoyaLCD_Bit5 + 16 * bRoyaLCD_Bit4 + 8 * bRoyaLCD_Bit3 + 4 * bRoyaLCD_Bit2 + 2 * bRoyaLCD_Bit1 + bRoyaLCD_Bit0;

	sprintf_s(tempChar, 256, "%d", ucNumLCD);
	GetDlgItem(IDC_ROYA_LCD_NUM_STATIC)->SetWindowTextA(_T(tempChar));

}

void CDlgTestRoyaBoard::OnBnClickedBtnRoyaTestLcd()
{
	// IDC_BTN_ROYA_TEST_LCD
	short sRet = roya_6412_comm_test_set_lcd(ucNumLCD);
	RoyaDlgCheckAndShowDebugMessage(sRet);
}

void CDlgTestRoyaBoard::OnCbnSelchangeRoyaUsgComboBlkNo()
{
	short sRet;
	// IDC_ROYA_USG_COMBO_BLK_NO
	uiCurrBlkNo = ((CComboBox *)GetDlgItem(IDC_ROYA_USG_COMBO_BLK_NO))->GetCurSel();
	if(uiCurrSegNo < _USG_MAX_NUM_SEGMENT)
	{
		sRet = roya_6412_usg_prof_get_blk_seg(uiCurrBlkNo, uiCurrSegNo, &stUSG_Seg);
		RoyaDlgCheckAndShowDebugMessage(sRet);
	}
	// uiNextBlkIdx, iTrigAddress, uiMaxCountTrigProt
	sRet = roya_6412_usg_profblk_get_trigger_address(uiCurrBlkNo, &iTrigAddress);
	sRet = roya_6412_usg_profblk_get_nextblk_index(uiCurrBlkNo, &uiNextBlkIdx);
	sRet = roya_6412_usg_profblk_get_max_count_wait_trig_prot(uiCurrBlkNo, &uiMaxCountTrigProt);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	UpdateUsgSeg();

}

void CDlgTestRoyaBoard::OnEnChangeRoyaUsgEditSegNo()
{
	CString cstrSegNo;
	// IDC_ROYA_USG_EDIT_SEG_NO
	GetDlgItem(IDC_ROYA_USG_EDIT_SEG_NO)->GetWindowTextA(&tempChar[0], 10);
	sscanf_s(tempChar, "%d", &uiCurrSegNo);
	if(uiCurrSegNo < _USG_MAX_NUM_SEGMENT)
	{
		short sRet = roya_6412_usg_prof_get_blk_seg(uiCurrBlkNo, uiCurrSegNo, &stUSG_Seg);
		RoyaDlgCheckAndShowDebugMessage(sRet);

		UpdateUsgSeg();
	}
}

void CDlgTestRoyaBoard::UpdateUsgSeg()
{
	sprintf_s(tempChar, 256, "%d", stUSG_Seg.iUSG_Duration_ms);
	GetDlgItem(IDC_ROYA_USG_EDIT_DURATION)->SetWindowTextA(_T(tempChar));
	sprintf_s(tempChar, 256, "%d", stUSG_Seg.iUSG_RampTime);
	GetDlgItem(IDC_ROYA_USG_EDIT_RAMP_TIME)->SetWindowTextA(_T(tempChar));
	sprintf_s(tempChar, 256, "%d", stUSG_Seg.iUSG_TriggerPattern);
	GetDlgItem(IDC_ROYA_USG_EDIT_TRIG_PATTERN)->SetWindowTextA(_T(tempChar));
	sprintf_s(tempChar, 256, "%d", stUSG_Seg.iUSG_Amplitude);
	GetDlgItem(IDC_ROYA_USG_EDIT_AMP)->SetWindowTextA(_T(tempChar));
	sprintf_s(tempChar, 256, "%d", stUSG_Seg.uiCmdType);
	GetDlgItem(IDC_ROYA_USG_EDIT_CMD_TYPE)->SetWindowTextA(_T(tempChar));
		// uiNextBlkIdx, iTrigAddress, uiMaxCountTrigProt
	sprintf_s(tempChar, 256, "%d", uiNextBlkIdx);
	GetDlgItem(IDC_ROYA_USG_EDIT_NEXT_BLK_IDX)->SetWindowTextA(_T(tempChar));
	sprintf_s(tempChar, 256, "%d", iTrigAddress);
	GetDlgItem(IDC_ROYA_USG_EDIT_TRIG_ADDR)->SetWindowTextA(_T(tempChar));
	sprintf_s(tempChar, 256, "%d", uiMaxCountTrigProt);
	GetDlgItem(IDC_ROYA_USG_EDIT_MAX_COUNT_TRIG_PROT)->SetWindowTextA(_T(tempChar));

}
// IDC_ROYA_USG_EDIT_DURATION
// IDC_ROYA_USG_EDIT_RAMP_TIME
// IDC_ROYA_USG_EDIT_TRIG_PATTERN
// IDC_ROYA_USG_EDIT_AMP
// IDC_ROYA_USG_EDIT_CMD_TYPE
// IDC_ROYA_BUTTON_SET_USG_SEG
void CDlgTestRoyaBoard::OnBnClickedRoyaButtonSetUsgSeg()
{
	GetDlgItem(IDC_ROYA_USG_EDIT_DURATION)->GetWindowTextA(&tempChar[0], 10);
	sscanf_s(tempChar, "%d", &(stUSG_Seg.iUSG_Duration_ms));
	GetDlgItem(IDC_ROYA_USG_EDIT_RAMP_TIME)->GetWindowTextA(&tempChar[0], 10);
	sscanf_s(tempChar, "%d", &(stUSG_Seg.iUSG_RampTime));
	GetDlgItem(IDC_ROYA_USG_EDIT_TRIG_PATTERN)->GetWindowTextA(&tempChar[0], 10);
	sscanf_s(tempChar, "%d", &(stUSG_Seg.iUSG_TriggerPattern));
	GetDlgItem(IDC_ROYA_USG_EDIT_AMP)->GetWindowTextA(&tempChar[0], 10);
	sscanf_s(tempChar, "%d", &(stUSG_Seg.iUSG_Amplitude));
	GetDlgItem(IDC_ROYA_USG_EDIT_CMD_TYPE)->GetWindowTextA(&tempChar[0], 10);
	sscanf_s(tempChar, "%d", &(stUSG_Seg.uiCmdType));

	//
	short sRet;
	if(uiCurrSegNo < _USG_MAX_NUM_SEGMENT)
	{
		sRet = roya_6412_usg_prof_set_blk_seg(uiCurrBlkNo, uiCurrSegNo, &stUSG_Seg);
		RoyaDlgCheckAndShowDebugMessage(sRet);

	}
	sRet = roya_6412_usg_profblk_set_trigger_address(uiCurrBlkNo, iTrigAddress);
	sRet = roya_6412_usg_profblk_set_nextblk_index(uiCurrBlkNo, uiNextBlkIdx);
	sRet = roya_6412_usg_profblk_set_max_count_wait_trig_prot(uiCurrBlkNo, uiMaxCountTrigProt);
	RoyaDlgCheckAndShowDebugMessage(sRet);

}

// IDC_ROYA_USG_EDIT_NEXT_BLK_IDX
// IDC_ROYA_USG_EDIT_TRIG_ADDR
// IDC_ROYA_USG_EDIT_MAX_COUNT_TRIG_PROT


void CDlgTestRoyaBoard::OnEnChangeRoyaUsgEditNextBlkIdx()
{
	GetDlgItem(IDC_ROYA_USG_EDIT_NEXT_BLK_IDX)->GetWindowTextA(&tempChar[0], 10);
	sscanf_s(tempChar, "%d", &(uiNextBlkIdx));
}

void CDlgTestRoyaBoard::OnEnChangeRoyaUsgEditTrigAddr()
{
	GetDlgItem(IDC_ROYA_USG_EDIT_TRIG_ADDR)->GetWindowTextA(&tempChar[0], 10);
	sscanf_s(tempChar, "%d", &(iTrigAddress));
}

void CDlgTestRoyaBoard::OnEnChangeRoyaUsgEditMaxCountTrigProt()
{
	GetDlgItem(IDC_ROYA_USG_EDIT_MAX_COUNT_TRIG_PROT)->GetWindowTextA(&tempChar[0], 10);
	sscanf_s(tempChar, "%d", &(uiMaxCountTrigProt));
}

// IDC_ROYA_COMBO_TIMER_ID
// IDC_ROYA_BOARD_SET_TIMER_PERIOD
// IDC_ROYA_BOARD_TIMER_PERIOD
void CDlgTestRoyaBoard::OnBnClickedRoyaBoardSetTimerPeriod()
{
char cTimerId = ((CComboBox *)GetDlgItem(IDC_ROYA_COMBO_TIMER_ID))->GetCurSel();
unsigned int uiTimerPeriod;    

	GetDlgItem(IDC_ROYA_BOARD_TIMER_PERIOD)->GetWindowTextA(&tempChar[0], 10);
	sscanf_s(tempChar, "%d", &(uiTimerPeriod));
	short sRet = roya_6412_comm_test_set_timer_period(cTimerId, uiTimerPeriod);
	RoyaDlgCheckAndShowDebugMessage(sRet);
}
// IDC_START_ROYA_USG_PROF_FSM
void CDlgTestRoyaBoard::OnBnClickedStartRoyaUsgProfFsm()
{
	uiStartFlag = 1 - uiStartFlag;
	short sRet = roya_6412_usg_profblk_set_start_flag_with_blk(uiCurrBlkNo, uiStartFlag);
	RoyaDlgCheckAndShowDebugMessage(sRet);
}

void CDlgTestRoyaBoard::UpdateRoyaBoardUsgStartButton()
{
	unsigned int uiBlk, uiSegIdx;
	int iErrorFlag;
	short sRet = roya_6412_usg_profblk_get_start_with_status(&uiBlk, &uiSegIdx,	&uiStartFlag, &iErrorFlag);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	if(uiStartFlag)
	{
		GetDlgItem(IDC_START_ROYA_USG_PROF_FSM)->SetWindowTextA(_T("Stop"));
	}
	else
	{
		GetDlgItem(IDC_START_ROYA_USG_PROF_FSM)->SetWindowTextA(_T("Start"));
	}
	sprintf_s(tempChar, 256, "StartFlag: %d, BlkIdx: %d, SegIdx: %d, Error: %d", uiStartFlag, uiBlk, uiSegIdx, iErrorFlag);
	GetDlgItem(IDC_ROYA_USG_PROF_STATUS)->SetWindowTextA(_T(tempChar));

}

void CDlgTestRoyaBoard::init_roya_board_usg_blk()
{
	short sRet;
	USG_UNIT_SEG stStaticUSGSeg;
	stStaticUSGSeg.iUSG_Amplitude = 100;
	stStaticUSGSeg.iUSG_Duration_ms = 10; 
	stStaticUSGSeg.iUSG_RampTime = 4;       // to avoid divided by zero
	stStaticUSGSeg.iUSG_TriggerPattern = TEST_TRIGGER_USG_PATTERN;
	stStaticUSGSeg.uiCmdType = _USG_CMD_RAMP_STEP;
	sRet = roya_6412_usg_prof_set_blk_seg(1, 0, &stStaticUSGSeg);
	RoyaDlgCheckAndShowDebugMessage(sRet);

//	roya_6412_usg_profblk_set_trigger_address(uiCurrBlkNo, iTrigAddress);
//	roya_6412_usg_profblk_set_nextblk_index(uiCurrBlkNo, uiNextBlkIdx);
//	roya_6412_usg_profblk_set_max_count_wait_trig_prot(uiCurrBlkNo, uiMaxCountTrigProt);

	//// From BLk 1 
	stStaticUSGSeg.iUSG_Amplitude = 200;
	stStaticUSGSeg.iUSG_Duration_ms = 10; 
	stStaticUSGSeg.iUSG_RampTime = 4;       // to avoid divided by zero
	stStaticUSGSeg.iUSG_TriggerPattern = TEST_TRIGGER_USG_PATTERN;
	stStaticUSGSeg.uiCmdType = _USG_CMD_WAIT_TRIGGER;
	sRet = roya_6412_usg_prof_set_blk_seg(1, 1, &stStaticUSGSeg);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	stStaticUSGSeg.iUSG_Amplitude = 50;
	stStaticUSGSeg.iUSG_Duration_ms = 10; 
	stStaticUSGSeg.iUSG_RampTime = 4;       // to avoid divided by zero
	stStaticUSGSeg.iUSG_TriggerPattern = TEST_TRIGGER_USG_PATTERN;
	stStaticUSGSeg.uiCmdType = _USG_CMD_RAMP_STEP;
	sRet = roya_6412_usg_prof_set_blk_seg(1, 2, &stStaticUSGSeg);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	stStaticUSGSeg.iUSG_Amplitude = 50;
	stStaticUSGSeg.iUSG_Duration_ms = 10; 
	stStaticUSGSeg.iUSG_RampTime = 4;       // to avoid divided by zero
	stStaticUSGSeg.iUSG_TriggerPattern = TEST_TRIGGER_USG_PATTERN;
	stStaticUSGSeg.uiCmdType = _USG_CMD_END_GOTO_NEXT_BLK;
	sRet = roya_6412_usg_prof_set_blk_seg(1, 3, &stStaticUSGSeg);
	RoyaDlgCheckAndShowDebugMessage(sRet);
	//stUltraSonicPowerBlk[1].uiTotalNumActiveSegment = 128;  

	//// From BLk 2 
	stStaticUSGSeg.iUSG_Amplitude = 255;
	stStaticUSGSeg.iUSG_Duration_ms = 10; 
	stStaticUSGSeg.iUSG_RampTime = 6;       // to avoid divided by zero
	stStaticUSGSeg.iUSG_TriggerPattern = TEST_TRIGGER_USG_PATTERN;
	stStaticUSGSeg.uiCmdType = _USG_CMD_RAMP_STEP;
	sRet = roya_6412_usg_prof_set_blk_seg(2, 0, &stStaticUSGSeg);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	stStaticUSGSeg.iUSG_Amplitude = 100;
	stStaticUSGSeg.iUSG_Duration_ms = 10; 
	stStaticUSGSeg.iUSG_RampTime = 7;       // to avoid divided by zero
	stStaticUSGSeg.iUSG_TriggerPattern = TEST_TRIGGER_USG_PATTERN;
	stStaticUSGSeg.uiCmdType = _USG_CMD_WAIT_TRIGGER;
	sRet = roya_6412_usg_prof_set_blk_seg(2, 1, &stStaticUSGSeg);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	// Set for testing protection
	// usg_blk_set_max_count_wait_trig_prot(2, 1000);

	stStaticUSGSeg.iUSG_Amplitude = 50;
	stStaticUSGSeg.iUSG_Duration_ms = 10; 
	stStaticUSGSeg.iUSG_RampTime = 9;       // to avoid divided by zero
	stStaticUSGSeg.iUSG_TriggerPattern = TEST_TRIGGER_USG_PATTERN;
	stStaticUSGSeg.uiCmdType = _USG_CMD_RAMP_STEP;
	sRet = roya_6412_usg_prof_set_blk_seg(2, 2, &stStaticUSGSeg);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	stStaticUSGSeg.iUSG_Amplitude = 50;
	stStaticUSGSeg.iUSG_Duration_ms = 10; 
	stStaticUSGSeg.iUSG_RampTime = 4;       // to avoid divided by zero
	stStaticUSGSeg.iUSG_TriggerPattern = TEST_TRIGGER_USG_PATTERN;
	stStaticUSGSeg.uiCmdType = _USG_CMD_END_GOTO_NEXT_BLK;
	sRet = roya_6412_usg_prof_set_blk_seg(2, 3, &stStaticUSGSeg);
	RoyaDlgCheckAndShowDebugMessage(sRet);

}
// IDC_BUTTON_WARM_RESET_ROYA_USG
void CDlgTestRoyaBoard::OnBnClickedButtonWarmResetRoyaUsg()
{
	short sRet = roya_6412_usg_profblk_warm_reset();
	RoyaDlgCheckAndShowDebugMessage(sRet);
}

// IDC_EDIT_TRIG_PATTERN_ROYA_USG_BY_ACS
void CDlgTestRoyaBoard::OnEnChangeEditTrigPatternRoyaUsgByAcs()
{
	GetDlgItem(IDC_EDIT_TRIG_PATTERN_ROYA_USG_BY_ACS)->GetWindowTextA(&tempChar[0], 10);
	sscanf_s(tempChar, "%d", &(cTrigPatternViaAcs));
}
// IDC_BUTTON_TRIG_ROYA_USG_VIA_ACS
void CDlgTestRoyaBoard::OnBnClickedButtonTrigRoyaUsgViaAcs()
{
}

// IDC_COMBO_ROYA_SELECT_DAC_IDX
void CDlgTestRoyaBoard::OnCbnSelchangeComboRoyaSelectDacIdx()
{
	uiDacIdx = ((CComboBox *)GetDlgItem(IDC_COMBO_ROYA_SELECT_DAC_IDX))->GetCurSel();

	InitDacSliderRangePosn();
	UpdateDacVoltageValueBySlider();
//	sprintf_s(tempChar, 256, "%d", aiDacValue[uiDacIdx]);
//	GetDlgItem(IDC_EDIT_ROYA_DAC_VALUE)->SetWindowTextA(_T(tempChar));
	
}
// IDC_COMBO_ROYA_ADC_INDEX
void CDlgTestRoyaBoard::OnCbnSelchangeComboRoyaAdcIndex()
{
	uiAdcIdx = ((CComboBox *)GetDlgItem(IDC_COMBO_ROYA_ADC_INDEX))->GetCurSel();
}

// IDC_EDIT_ROYA_DAC_VALUE
void CDlgTestRoyaBoard::OnEnChangeEditRoyaDacValue()
{
//	GetDlgItem(IDC_EDIT_ROYA_DAC_VALUE)->GetWindowTextA(&tempChar[0], 10);
//	sscanf_s(tempChar, "%d", &(aiDacValue[uiDacIdx]));

}

// IDC_BUTTON_ROYA_SET_DAC
void CDlgTestRoyaBoard::OnBnClickedButtonRoyaSetDac()
{
	short sRet = roya_6412_comm_test_set_dac(uiDacIdx, aiDacValue[uiDacIdx]);
	RoyaDlgCheckAndShowDebugMessage(sRet);
}
// IDC_STATIC_SHOW_ROYA_ADC_VOLT_VALUE


///////////////// BSD application related functions
void CDlgTestRoyaBoard::InitDlgBSD()
{

	for(int ii = 0; ii<_BSD_MAX_BLK_PARAMETER_; ii ++)
	{
		sprintf_s(tempChar, 256, "%d", ii);
		((CComboBox *)GetDlgItem(IDC_ROYA_BSD_BLK_IDX))->InsertString(ii, _T(tempChar));
	}
	((CComboBox *)GetDlgItem(IDC_ROYA_BSD_BLK_IDX))->SetCurSel(ucBlkIdxBSD);

	short sRet;
	for(int ii = 0; ii< _BSD_MAX_BLK_PARAMETER_; ii++)
	{
//#else
		sRet = roya_6412_bsd_get_parameters(&stParameterBlkBSD, ii);
		RoyaDlgCheckAndShowDebugMessage(sRet);

	}
	BSD_STATUS stStatusBSD;
	sRet = roya_6412_bsd_get_status(&stStatusBSD);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	ucBlkIdxBSD = stStatusBSD.cBlkIdxBSD;
	sRet = roya_6412_bsd_get_parameters(&stParameterBlkBSD, ucBlkIdxBSD);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	GetDlgItem(IDC_ROYA_BSD_DATA_PREV_BLK_MAX)->SetWindowTextA(_T("-"));
	GetDlgItem(IDC_ROYA_BSD_DATA_PREV_BLK_MIN)->SetWindowTextA(_T("-"));
	GetDlgItem(IDC_ROYA_BSD_DATA_PREV_BLK_MEAN)->SetWindowTextA(_T("-"));
	GetDlgItem(IDC_ROYA_BSD_DATA_PREV_BLK_STD)->SetWindowTextA(_T("-"));
	GetDlgItem(IDC_ROYA_BSD_PREV_BLK_V_OUT)->SetWindowTextA(_T("-"));
	GetDlgItem(IDC_ROYA_BSD_PREV_BLK_V_READ)->SetWindowTextA(_T("-"));

	GetDlgItem(IDC_ROYA_BSD_DATA_CURR_BLK_MAX)->SetWindowTextA(_T("-"));
	GetDlgItem(IDC_ROYA_BSD_DATA_CURR_BLK_MIN)->SetWindowTextA(_T("-"));
	GetDlgItem(IDC_ROYA_BSD_DATA_CURR_BLK_MEAN)->SetWindowTextA(_T("-"));
	GetDlgItem(IDC_ROYA_BSD_DATA_CURR_BLK_STD)->SetWindowTextA(_T("-"));
	GetDlgItem(IDC_ROYA_BSD_CURR_BLK_V_OUT)->SetWindowTextA(_T("-"));
	GetDlgItem(IDC_ROYA_BSD_CURR_BLK_V_READ)->SetWindowTextA(_T("-"));

	UpdateParameterDlgBSD();

	GetDlgItem(IDC_EDIT_ROYA_TEST_DLG_RPT)->ShowWindow(FALSE);
}

// IDC_ROYA_BSD_BUTTON_API_SET_BLK_PARA
// IDC_ROYA_BSD_BUTTON_API_GET_BLK_PARA
void CDlgTestRoyaBoard::OnBnClickedRoyaNsdButtonApiSetBlkPara()
{
BSD_API_PARAMETER stParaBlkApiBSD;

	stParaBlkApiBSD.cAdcIdx = stParameterBlkBSD.cAdcIdx;
	stParaBlkApiBSD.cDacIdx = stParameterBlkBSD.cDacIdx;
	stParaBlkApiBSD.cDetectFactorIsr = stParameterBlkBSD.cDetectFactorIsr;
	stParaBlkApiBSD.fDacValue_v = (float)(stParameterBlkBSD.uiDacValue * _OUT_VOLT_VIA_DAC_FACTOR_ + _OUT_VOLT_VIA_DAC_OFFSET_);
	stParaBlkApiBSD.usDetectSampleLength = stParameterBlkBSD.usDetectSampleLength;

	bsd_api_set_parameter(&stParaBlkApiBSD, ucBlkIdxBSD);
}

void CDlgTestRoyaBoard::OnBnClickedRoyaNsdButtonApiGetBlkPara()
{
BSD_API_PARAMETER stParaBlkApiBSD;

	if(bsd_api_get_parameter(&stParaBlkApiBSD, ucBlkIdxBSD) == BSD_OKEY)
	{
		stParameterBlkBSD.cAdcIdx = stParaBlkApiBSD.cAdcIdx;
		stParameterBlkBSD.cDacIdx = stParaBlkApiBSD.cDacIdx;
		stParameterBlkBSD.cDetectFactorIsr = stParaBlkApiBSD.cDetectFactorIsr;
		stParameterBlkBSD.usDetectSampleLength = stParaBlkApiBSD.usDetectSampleLength;
		stParameterBlkBSD.uiDacValue = (unsigned int)((stParaBlkApiBSD.fDacValue_v - _OUT_VOLT_VIA_DAC_OFFSET_)/ _OUT_VOLT_VIA_DAC_FACTOR_);
	}
	UpdateParameterDlgBSD();
}


void CDlgTestRoyaBoard::UpdateParameterDlgBSD()
{
	// char strTemp[64];
	CString strTemp;
	strTemp.Format("%d", (int)(stParameterBlkBSD.cDetectFactorIsr));
	GetDlgItem(IDC_ROYA_BSD_EDIT_FACTOR_ISR)->SetWindowTextA(strTemp);

	strTemp.Format("%d", (int)(stParameterBlkBSD.usDetectSampleLength));
	GetDlgItem(IDC_ROYA_BSD_EDIT_SAMPLE_LEN)->SetWindowTextA(strTemp);

	strTemp.Format("%d", (int)(stParameterBlkBSD.cAdcIdx));
	GetDlgItem(IDC_ROYA_BSD_EDIT_ADC_IDX)->SetWindowTextA(strTemp);

	strTemp.Format("%d", (int)(stParameterBlkBSD.cDacIdx));
	GetDlgItem(IDC_ROYA_BSD_EDIT_DAC_IDX)->SetWindowTextA(strTemp);

	strTemp.Format("%d", (int)(stParameterBlkBSD.uiDacValue));
	GetDlgItem(IDC_ROYA_BSD_EDIT_DAC_VALUE)->SetWindowTextA(strTemp);

	// IDC_ROYA_BSD_DATA_BUFFER_IDX
	strTemp.Format("%d", (int)(stParameterBlkBSD.cDataBufferIdx));
	GetDlgItem(IDC_ROYA_BSD_DATA_BUFFER_IDX)->SetWindowTextA(strTemp);

	// IDC_ROYA_BSD_NEXT_BLK
	strTemp.Format("%d", (int)(stParameterBlkBSD.cNextParaBlkIdx));
	GetDlgItem(IDC_ROYA_BSD_NEXT_BLK)->SetWindowTextA(strTemp);
}

extern short bsd_api_get_wire_spool_motor_move_counter(unsigned int *uiWireSpoolMotorCounter);
static unsigned int uiCounterWireSpoolMotor_MoveFlag;
static BOOL bWireSpoolMotorMoveFlag;  // bsd_api_check_wire_spool_motor_once
void CDlgTestRoyaBoard::UpdateStatusDlgBSD()
{
//	unsigned char ucSampleCnt, ucBlkIdx, ucFlagStart, ucFlagTriggered;
	CString strTemp;
	short sRet;

#ifdef TEST_BSD_API
	sRet = bsd_api_get_status(&stStatusBSD);
#else
	sRet = roya_6412_bsd_get_status(&stStatusBSD);
#endif
	RoyaDlgCheckAndShowDebugMessage(sRet);

	// IDC_ROYA_BSD_STATUS_SAMPLE_CNT
	strTemp.Format("%d", stStatusBSD.usBsdSampleCnt);
	GetDlgItem(IDC_ROYA_BSD_STATUS_SAMPLE_CNT)->SetWindowTextA(strTemp);
	// IDC_ROYA_BSD_STATUS_START_FLAG
	strTemp.Format("%d", stStatusBSD.cFlagStartBSD);
	GetDlgItem(IDC_ROYA_BSD_STATUS_START_FLAG)->SetWindowTextA(strTemp);
	// IDC_ROYA_BSD_STATUS_PARA_BLK
	strTemp.Format("%d", stStatusBSD.cBlkIdxBSD);
	GetDlgItem(IDC_ROYA_BSD_STATUS_PARA_BLK)->SetWindowTextA(strTemp);
	// IDC_ROYA_BSD_STATUS_TRIGGER_FLAG
	strTemp.Format("%d", stStatusBSD.cFlagTriggerBSD);
	GetDlgItem(IDC_ROYA_BSD_STATUS_TRIGGER_FLAG)->SetWindowTextA(strTemp);
	// IDC_ROYA_BSD_STATUS_TRIGGER_CNT
	strTemp.Format("%d", stStatusBSD.uiTrigCnt);
	GetDlgItem(IDC_ROYA_BSD_STATUS_TRIGGER_CNT)->SetWindowTextA(strTemp);

	// EFO Good NG
unsigned int uiEfoGoodFlag;

	ulBSD_CommuStartTime = GetCurrentTime();
	bsd_api_get_efo_good_counter(&uiEfoGoodFlag);
	ulBSD_CommuElapseTime = GetCurrentTime() - ulBSD_CommuStartTime;

	strTemp.Format("%d", uiEfoGoodFlag);
	GetDlgItem(IDC_ROYA_EFO_FLAG)->SetWindowTextA(strTemp);
	if(uiEfoGoodFlag > uiPrevEfoGoodFlag || 
		(uiEfoGoodFlag == 0 && uiPrevEfoGoodFlag == UINT_MAX)
		) // Once a good signal is detected, the BSD start_flag is set automatically
	{
		uiPrevEfoGoodFlag = uiEfoGoodFlag;
	}

	if(iFlagDryRunBSD == TRUE)
	{
		BSDAutoCheckCommuError();
	}

	////// Other Interrupt Counter
	// IDC_ROYA_DSP_WFS_COUNTER_
	unsigned int uiWireFeedSensorTriggerCounter;
	bsd_api_get_wire_spool_motor_move_counter(&uiWireFeedSensorTriggerCounter);
	strTemp.Format("%d", uiWireFeedSensorTriggerCounter);
	GetDlgItem(IDC_ROYA_DSP_WFS_COUNTER_)->SetWindowTextA(strTemp);

	uiCounterWireSpoolMotor_MoveFlag ++;  // IDC_ROYA_DSP_WFS_MOVED_FLAG
	if(uiCounterWireSpoolMotor_MoveFlag >= 10)
	{
		uiCounterWireSpoolMotor_MoveFlag = 0;
		bsd_api_check_wire_spool_motor_once(&bWireSpoolMotorMoveFlag);

		if(bWireSpoolMotorMoveFlag == TRUE)
		{
			if(get_sys_language_option() == LANGUAGE_UI_EN)
			{
				strTemp.Format("M");
			}
			else
			{
				strTemp.Format("动");
				
			}			
		}
		else
		{
			if(get_sys_language_option() == LANGUAGE_UI_EN)
			{
				strTemp.Format("S");
			}
			else
			{
				strTemp.Format("静");
				
			}			
		}
		GetDlgItem(IDC_ROYA_DSP_WFS_MOVED_FLAG)->SetWindowTextA(strTemp);
	}
}

void CDlgTestRoyaBoard::BSDAutoCheckCommuError()
{
	BSD_PARAMETER  stParameterBlkBSDCommuErr;

	ulBSD_CommuStartTime = GetCurrentTime();
	if(!cFlagBSDAnalyzeByFirmware)
	{
		bsd_api_analyze_stickness(&stInputAnalyzeBSD, 
										  &stOutputAnalyzeBSD);
	}
	else
	{
		OnBnClickedRoyaBsdButtonFirmwareAnalyzeStickness();
	}

	ulBSD_CommuElapseTime_2 = GetCurrentTime() - ulBSD_CommuStartTime;

	CString cstrTemp;
	cstrTemp.Format("S: %d,  Dur_2: %d", ulBSD_CommuStartTime,  ulBSD_CommuElapseTime_2); //Dur_1: %d, ulBSD_CommuElapseTime,
	GetDlgItem(IDC_STATIC_BSD_COMMU_MONITOR)->SetWindowTextA(cstrTemp);

	if( // !cFlagBSDAnalyzeByFirmware &&
		ulBSD_CommuElapseTime_2 > BSD_COMMUNICATION_ERROR_TH_MS) // 20111010 // ulBSD_CommuElapseTime > 300 ||
	{
#ifdef  __STOP_ONCE_ERROR_HAPPENS
		iFlagCheckEnableTimerAll = 0;
		((CButton*)GetDlgItem(IDC_ROYA_TEST_CHECK_ENABLE_TIMER_ALL))->SetCheck(iFlagCheckEnableTimerAll);
#endif  // __STOP_ONCE_ERROR_HAPPENS
		Beep(400, 1000);
		_time64(&stDebugLongTimeRoyaDlg);
		_localtime64_s(stpDebugTimeRoyaDlg, &stDebugLongTimeRoyaDlg);

		if(ulBSD_CommuElapseTime > BSD_COMMUNICATION_ERROR_TH_MS) // && add protection over overflow
		{
			roya_6412_bsd_get_parameters(&stParameterBlkBSDCommuErr, 0);
			cstrTemp.AppendFormat("BSD Comm-1 >= 300, %d", stParameterBlkBSDCommuErr.usDetectSampleLength);
			roya_6412_bsd_get_parameters(&stParameterBlkBSDCommuErr, 2);
			cstrTemp.AppendFormat(",%d, Hour:%d, Min:%d", stParameterBlkBSDCommuErr.usDetectSampleLength, stpDebugTimeRoyaDlg->tm_hour, stpDebugTimeRoyaDlg->tm_min);

			uiCommErrNumber ++;
			cstrRoyaDlgDebugText.AppendFormat("\r\n CommError: %d,  %s", uiCommErrNumber, cstrTemp.GetString());
			GetDlgItem(IDC_EDIT_ROYA_TEST_DLG_RPT)->SetWindowTextA(cstrRoyaDlgDebugText);

//			AfxMessageBox(cstrTemp);
		}
		else if(ulBSD_CommuElapseTime_2 > BSD_COMMUNICATION_ERROR_TH_MS)
		{
			roya_6412_bsd_get_parameters(&stParameterBlkBSDCommuErr, 0);
			cstrTemp.AppendFormat("BSD Comm-2 >= 300, %d", stParameterBlkBSDCommuErr.usDetectSampleLength);
			roya_6412_bsd_get_parameters(&stParameterBlkBSDCommuErr, 2);
			cstrTemp.AppendFormat(",%d, Hour:%d, Min:%d", stParameterBlkBSDCommuErr.usDetectSampleLength, stpDebugTimeRoyaDlg->tm_hour, stpDebugTimeRoyaDlg->tm_min);

			uiCommErrNumber ++;
			cstrRoyaDlgDebugText.AppendFormat("\r\n CommError: %d,  %s", uiCommErrNumber, cstrTemp.GetString());
			GetDlgItem(IDC_EDIT_ROYA_TEST_DLG_RPT)->SetWindowTextA(cstrRoyaDlgDebugText);
//			AfxMessageBox(cstrTemp);
		}
	}
static unsigned char ucCurrStartBlkIdx;

	if(ucCurrStartBlkIdx == 0)
	{
		ucCurrStartBlkIdx = 2;
	}
	else if( ucCurrStartBlkIdx == 2)
	{
		ucCurrStartBlkIdx = 0;
	}
	bsd_api_start_detect(ucCurrStartBlkIdx);
}

// IDC_ROYA_BSD_BLK_IDX
void CDlgTestRoyaBoard::OnCbnSelchangeRoyaNsdBlkIdx()
{	
	ucBlkIdxBSD = ((CComboBox *)GetDlgItem(IDC_ROYA_BSD_BLK_IDX))->GetCurSel();
	short sRet = roya_6412_bsd_get_parameters(&stParameterBlkBSD, ucBlkIdxBSD);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	UpdateParameterDlgBSD();

}

// IDC_ROYA_BSD_EDIT_FACTOR_ISR
void CDlgTestRoyaBoard::OnEnChangeRoyaNsdEditFactorIsr()
{
	int iTemp;
	GetDlgItem(IDC_ROYA_BSD_EDIT_FACTOR_ISR)->GetWindowTextA(&tempChar[0], 10);
	sscanf_s(tempChar, "%d", &(iTemp));
	stParameterBlkBSD.cDetectFactorIsr = (char)iTemp;
}

// IDC_ROYA_BSD_EDIT_SAMPLE_LEN
void CDlgTestRoyaBoard::OnEnChangeRoyaNsdEditSampleLen()
{
	int iTemp;
	GetDlgItem(IDC_ROYA_BSD_EDIT_SAMPLE_LEN)->GetWindowTextA(&tempChar[0], 10);
	sscanf_s(tempChar, "%d", &(iTemp));
	stParameterBlkBSD.usDetectSampleLength = (unsigned short)iTemp;
}

// IDC_ROYA_BSD_EDIT_ADC_IDX
void CDlgTestRoyaBoard::OnEnChangeRoyaNsdEditAdcIdx()
{
	int iTemp;
	GetDlgItem(IDC_ROYA_BSD_EDIT_ADC_IDX)->GetWindowTextA(&tempChar[0], 10);
	sscanf_s(tempChar, "%d", &(iTemp));
	stParameterBlkBSD.cAdcIdx = (char)iTemp;
}
// IDC_ROYA_BSD_EDIT_DAC_IDX
void CDlgTestRoyaBoard::OnEnChangeRoyaNsdEditDacIdx()
{
	int iTemp;
	GetDlgItem(IDC_ROYA_BSD_EDIT_DAC_IDX)->GetWindowTextA(&tempChar[0], 10);
	sscanf_s(tempChar, "%d", &(iTemp));
	stParameterBlkBSD.cDacIdx = (char)iTemp;
}
// IDC_ROYA_BSD_EDIT_DAC_VALUE
void CDlgTestRoyaBoard::OnEnChangeRoyaNsdEditDacValue()
{
	GetDlgItem(IDC_ROYA_BSD_EDIT_DAC_VALUE)->GetWindowTextA(&tempChar[0], 10);
	sscanf_s(tempChar, "%d", &(stParameterBlkBSD.uiDacValue));
}
// IDC_ROYA_BSD_BUTTON_SET_BLK_PARA
void CDlgTestRoyaBoard::OnBnClickedRoyaNsdButtonSetBlkPara()
{
	short sRet = roya_6412_bsd_set_parameters(&stParameterBlkBSD, ucBlkIdxBSD);
	RoyaDlgCheckAndShowDebugMessage(sRet);
}

// IDC_ROYA_BSD_BUTTON_START_W_BLK
void CDlgTestRoyaBoard::OnBnClickedRoyaNsdButtonStartWBlk()
{
	short sRet;
#ifdef TEST_BSD_API
	sRet = bsd_api_start_detect(ucBlkIdxBSD);
#else
	sRet = roya_6412_bsd_start_detect(ucBlkIdxBSD);
#endif
	RoyaDlgCheckAndShowDebugMessage(sRet);

}
// IDC_ROYA_BSD_BUTTON_WARM_RESET
void CDlgTestRoyaBoard::OnBnClickedRoyaNsdButtonWarmReset()
{
	short sRet;
#ifdef TEST_BSD_API
	sRet = bsd_api_warm_reset();
#else
	sRet = roya_6412_bsd_warm_reset();
#endif
	RoyaDlgCheckAndShowDebugMessage(sRet);

}

static char *pstrBSD_AnalyseStatus[] = {
	"BSD_OK",
	"1B_NSDie",
	"2B_ERR_1B",
	"2B_NSLead",
	"2B_TailShort"
};

#define BSD_DEF_1ST_BOND_DIE_TH_RATIO			0.6
#define BSD_DEF_2ND_BOND_NON_STICK_TH_RATIO		0.7
#define BSD_DEF_2ND_BOND_STICK_LEAD_V_RATIO		0.6

void CDlgTestRoyaBoard::UpdateUShortToEdit(int nResId, unsigned short usValue)
{
	CString cstrTemp;
	cstrTemp.Format("%d", usValue);
//	sprintf_s(tempChar, 32, "%d", sValue);
	GetDlgItem(nResId)->SetWindowTextA(cstrTemp); // _T(tempChar));
}

void CDlgTestRoyaBoard::UpdateFloatToEdit(int nResId, float fValue)
{
	CString cstrTemp;
	cstrTemp.Format("%3.1f", fValue);
	GetDlgItem(nResId)->SetWindowTextA(cstrTemp); // _T(tempChar));
}

void CDlgTestRoyaBoard::ReadUShortFromEdit(int nResId, unsigned short *pusValue)
{
	static char tempChar[32];
	int iTemp;
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", &iTemp);
	*pusValue = (unsigned short)iTemp;
}

void CDlgTestRoyaBoard::ReadFloatFromEdit(int nResId, float *pfValue)
{
	static char tempChar[32];
	float fTemp;
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%f", &fTemp);
	*pfValue = fTemp;
}

#define DEF_BSD_VOLTAGE_DAC_V		8.0

void CDlgTestRoyaBoard::InitParameterBSD()
{
	stInputAnalyzeBSD.us1stBondNumSamplesConsecGreaterDieThAdc = 5;
	stInputAnalyzeBSD.us2ndBondNumSampleConsecHigher = 5;
	stInputAnalyzeBSD.fBSD1stBondStickDieThAdc_v = (float)(DEF_BSD_VOLTAGE_DAC_V * BSD_DEF_1ST_BOND_DIE_TH_RATIO);
	stInputAnalyzeBSD.fBSD2ndBondNonStickThAdc_v = (float)(DEF_BSD_VOLTAGE_DAC_V * BSD_DEF_2ND_BOND_NON_STICK_TH_RATIO);


	stInputAnalyzeBSD.us1stBondSkipSamples = 1;
	stInputAnalyzeBSD.us1stBondNumSamplesConsecLowerLeadThAdc = 0; // Not used
	stInputAnalyzeBSD.us2ndBondNumSamplesConsecLowerLeadThAdc = 5;
	stInputAnalyzeBSD.us2ndBondSkipSamples = 1;
	stInputAnalyzeBSD.fBSD2ndBondStickLeadThAdc_v = (float)(DEF_BSD_VOLTAGE_DAC_V * BSD_DEF_2ND_BOND_STICK_LEAD_V_RATIO);

	UpdateUShortToEdit(IDC_EDIT_ROYA_BSD_DEBOUNCE_1ST_BOND, stInputAnalyzeBSD.us1stBondNumSamplesConsecGreaterDieThAdc);
	UpdateUShortToEdit(IDC_EDIT_ROYA_BSD_DEBOUNCE_2ND_BOND, stInputAnalyzeBSD.us2ndBondNumSampleConsecHigher);
	UpdateUShortToEdit(IDC_EDIT_ROYA_BSD_SKIP_SAMPLE_1ST_BOND, stInputAnalyzeBSD.us1stBondSkipSamples);
	UpdateUShortToEdit(IDC_EDIT_ROYA_BSD_SKIP_SAMPLE_2ND_BOND, stInputAnalyzeBSD.us2ndBondSkipSamples);
	UpdateUShortToEdit(IDC_EDIT_ROYA_BSD_DEBOUNCE_1ST_BOND_CONSE_LOW, stInputAnalyzeBSD.us1stBondNumSamplesConsecLowerLeadThAdc);
	UpdateUShortToEdit(IDC_EDIT_ROYA_BSD_DEBOUNCE_2ND_BOND_CONSE_LOW, stInputAnalyzeBSD.us2ndBondNumSamplesConsecLowerLeadThAdc);

	// Voltage, 1st Bond
	UpdateFloatToEdit(IDC_EDIT_ROYA_BSD_THRE_HOLD_V_1ST_BOND, stInputAnalyzeBSD.fBSD1stBondStickDieThAdc_v);
	// Voltage, 2nd Bond
	UpdateFloatToEdit(IDC_EDIT_ROYA_BSD_THRE_HOLD_V_2ND_BOND, stInputAnalyzeBSD.fBSD2ndBondNonStickThAdc_v);
	UpdateFloatToEdit(IDC_EDIT_ROYA_BSD_THRE_HOLD_V_LOWER_LEAD_2ND_BOND, stInputAnalyzeBSD.fBSD2ndBondStickLeadThAdc_v);

}

// 20110014 // 
#include "MotAlgo_DLL.h"
extern char *astrMachineTypeNameLabel_en[];

// IDC_ROYA_BSD_BUTTON_GET_DATA
void CDlgTestRoyaBoard::OnBnClickedRoyaNsdButtonGetData()
{

	// #define __CONVERT_ADC_TO_VOLT_FlOAT(x)		(x * _IN_VOLT_VIA_ADC_FACTOR_ + _IN_VOLT_VIA_ADC_OFFSET_)
	short sRet = BSD_OKEY;

static char strPrefixDebugBsd[128];  // 20111014
static	char strDebugBsdFile[128];

#ifndef _DEBUG

	int iMechCfg = get_sys_machine_type_flag();

	sprintf_s(strPrefixDebugBsd, 128, "%s-%d_", 
		astrMachineTypeNameLabel_en[iMechCfg],
				get_sys_machine_serial_num() );
	sRet = bsd_api_gen_debug_file(strPrefixDebugBsd, strDebugBsdFile, 128);

	if(bFlagBSD_CheckByWaveForm && (sRet == BSD_OKEY))
	{
		static char strSystemCommand[256];
		sprintf_s(strSystemCommand, 256, "notepad %s", strDebugBsdFile);
		system(strSystemCommand);
	}
#endif  // NOT _DEBUG
static STICKNESS_PARAMETER stSuggestParameterBSD;
	CString cstrTemp;

	if(sRet == BSD_OKEY)
	{
		bsd_api_calc_stickness_parameter(&stSuggestParameterBSD, BSD_TEACH_FLAG_OK);
		cstrTemp.Format("%4.2f", stSuggestParameterBSD.dThresholdDie_v);
		GetDlgItem(IDC_ROYA_BSD_SUGGEST_DIE_TH)->SetWindowTextA(cstrTemp);
		cstrTemp.Format("%4.2f", stSuggestParameterBSD.dThresholdLead_v);
		GetDlgItem(IDC_ROYA_BSD_SUGGEST_LEAD_TH)->SetWindowTextA(cstrTemp);
	}

#ifndef _DEBUG 
	unsigned int uiSampleCnt;
	BSD_DATA_ANALYSIS stAnalyDataBSD;
//  Get current status
	sRet = roya_6412_bsd_get_status(&stStatusBSD);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	unsigned int uiCurrBlkIdx = stStatusBSD.cBlkIdxBSD;
	unsigned int uiPrevBlkIdx = stStatusBSD.cBlkIdxBSD - 1;
	if(uiPrevBlkIdx >= _BSD_MAX_BLK_PARAMETER_)
	{
		uiPrevBlkIdx = _BSD_MAX_BLK_PARAMETER_ - 1;
	}

	// Analyze the stickness 
	BSD_API_PARAMETER stParaPrevBlkBSD, stParaCurrBlkBSD;
	bsd_api_get_parameter(&stParaPrevBlkBSD, uiPrevBlkIdx);
	bsd_api_get_parameter(&stParaCurrBlkBSD, uiCurrBlkIdx);

	// 1st Bond BSD Parameter
	stInputAnalyzeBSD.ucParaBlk1stBSD = uiPrevBlkIdx;
	// 2nd Bond BSD Parameter
	stInputAnalyzeBSD.ucParaBlk2ndBSD = uiCurrBlkIdx;

	sRet = bsd_api_analyze_stickness(&stInputAnalyzeBSD, 
									  &stOutputAnalyzeBSD);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	// 1st bond
	cstrTemp.Format("%s", pstrBSD_AnalyseStatus[stOutputAnalyzeBSD.us1stBondStickStatus]);
	GetDlgItem(IDC_ROYA_BSD_DATA_STICKNESS_1ST_BOND)->SetWindowTextA(cstrTemp);
	// 2nd bond
	cstrTemp.Format("%s", pstrBSD_AnalyseStatus[stOutputAnalyzeBSD.us2ndBondStickStatus]);
	GetDlgItem(IDC_ROYA_BSD_DATA_STICKNESS_2ND_BOND)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("L-%d,R-%d", stOutputAnalyzeBSD.us2ndBondStartSampleStickness, stOutputAnalyzeBSD.us2ndBondOpenSticknessFirstRiseCount);
	GetDlgItem(IDC_SHOW_ROYA_BSD_DATA_2ND_BOND_RISING_EDGE_CNT)->SetWindowTextA(cstrTemp);

// Data in Previous Block 
	sRet = roya_6412_bsd_get_data(uiPrevBlkIdx, &uiSampleCnt);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	sRet = bsd_api_analyze_data(uiPrevBlkIdx, uiSampleCnt, &stAnalyDataBSD);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	CString strTemp;
	// IDC_ROYA_BSD_DATA_PREV_BLK_MAX
	strTemp.Format("%5.1f", __CONVERT_ADC_TO_VOLT_FlOAT(stAnalyDataBSD.dMax));
	GetDlgItem(IDC_ROYA_BSD_DATA_PREV_BLK_MAX)->SetWindowTextA(strTemp);
	// IDC_ROYA_BSD_DATA_PREV_BLK_MIN
	strTemp.Format("%5.1f", __CONVERT_ADC_TO_VOLT_FlOAT(stAnalyDataBSD.dMin));
	GetDlgItem(IDC_ROYA_BSD_DATA_PREV_BLK_MIN)->SetWindowTextA(strTemp);
	// IDC_ROYA_BSD_DATA_PREV_BLK_MEAN
	strTemp.Format("%5.1f", __CONVERT_ADC_TO_VOLT_FlOAT(stAnalyDataBSD.dMean));
	GetDlgItem(IDC_ROYA_BSD_DATA_PREV_BLK_MEAN)->SetWindowTextA(strTemp);
	// IDC_ROYA_BSD_DATA_PREV_BLK_STD
	strTemp.Format("%5.1f", __CONVERT_ADC_TO_VOLT_FlOAT(stAnalyDataBSD.dStandDevia));
	GetDlgItem(IDC_ROYA_BSD_DATA_PREV_BLK_STD)->SetWindowTextA(strTemp);
	// IDC_ROYA_BSD_PREV_BLK_V_OUT
	strTemp.Format("%5.1f", stAnalyDataBSD.dVoltOut);
	GetDlgItem(IDC_ROYA_BSD_PREV_BLK_V_OUT)->SetWindowTextA(strTemp);
	// IDC_ROYA_BSD_PREV_BLK_V_READ
	strTemp.Format("%5.1f", stAnalyDataBSD.dVoltReadMean);
	GetDlgItem(IDC_ROYA_BSD_PREV_BLK_V_READ)->SetWindowTextA(strTemp);

	// Data in Current Block 
	sRet = roya_6412_bsd_get_data(uiCurrBlkIdx, &uiSampleCnt);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	sRet = bsd_api_analyze_data(uiCurrBlkIdx, uiSampleCnt, &stAnalyDataBSD);
	RoyaDlgCheckAndShowDebugMessage(sRet);
	// IDC_ROYA_BSD_DATA_MAX
	strTemp.Format("%5.1f", __CONVERT_ADC_TO_VOLT_FlOAT(stAnalyDataBSD.dMax));
	GetDlgItem(IDC_ROYA_BSD_DATA_CURR_BLK_MAX)->SetWindowTextA(strTemp);
	// IDC_ROYA_BSD_DATA_CURR_BLK_MIN
	strTemp.Format("%5.1f", __CONVERT_ADC_TO_VOLT_FlOAT(stAnalyDataBSD.dMin));
	GetDlgItem(IDC_ROYA_BSD_DATA_CURR_BLK_MIN)->SetWindowTextA(strTemp);
	// IDC_ROYA_BSD_DATA_CURR_BLK_MEAN
	strTemp.Format("%5.1f", __CONVERT_ADC_TO_VOLT_FlOAT(stAnalyDataBSD.dMean));
	GetDlgItem(IDC_ROYA_BSD_DATA_CURR_BLK_MEAN)->SetWindowTextA(strTemp);
	// IDC_ROYA_BSD_DATA_CURR_BLK_STD
	strTemp.Format("%5.1f", __CONVERT_ADC_TO_VOLT_FlOAT(stAnalyDataBSD.dStandDevia));
	GetDlgItem(IDC_ROYA_BSD_DATA_CURR_BLK_STD)->SetWindowTextA(strTemp);
	// IDC_ROYA_BSD_CURR_BLK_V_OUT
	strTemp.Format("%5.1f", stAnalyDataBSD.dVoltOut);
	GetDlgItem(IDC_ROYA_BSD_CURR_BLK_V_OUT)->SetWindowTextA(strTemp);
	// IDC_ROYA_BSD_CURR_BLK_V_READ
	strTemp.Format("%5.1f", stAnalyDataBSD.dVoltReadMean);
	GetDlgItem(IDC_ROYA_BSD_CURR_BLK_V_READ)->SetWindowTextA(strTemp);
#endif  // NOT _DEBUG

}
// IDC_ROYA_USG_CHECK_AUTO
void CDlgTestRoyaBoard::OnBnClickedRoyaUsgCheckAuto()
{
	iAutoCheckUSG = ((CButton *)GetDlgItem(IDC_ROYA_USG_CHECK_AUTO))->GetCheck();
}
// IDC_ROYA_ADDA_CHECK_AUTO
void CDlgTestRoyaBoard::OnBnClickedRoyaAddaCheckAuto()
{
	iAutoCheckADC = ((CButton *)GetDlgItem(IDC_ROYA_ADDA_CHECK_AUTO))->GetCheck();
}
// IDC_ROYA_BSD_CHECK_AUTO
void CDlgTestRoyaBoard::OnBnClickedRoyaNsdCheckAuto()
{
	iAutoCheckBSD = ((CButton *)GetDlgItem(IDC_ROYA_BSD_CHECK_AUTO))->GetCheck();
}

char *strMonth[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
char *strWeekDay[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

void CDlgTestRoyaBoard::UpdateRoyaDlgRT_Clock()
{
	CString cstrTemp;

	cstrTemp.Format("%d", stRoyaRealTimeClock.year);
	GetDlgItem(IDC_EDIT_ROYA_RTC_YEAR)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("%s", strMonth[stRoyaRealTimeClock.mon]);
	GetDlgItem(IDC_EDIT_ROYA_RTC_MONTH)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("%d", stRoyaRealTimeClock.mday);
	GetDlgItem(IDC_EDIT_ROYA_RTC_MDATE)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("%s", strWeekDay[stRoyaRealTimeClock.wday]);
	GetDlgItem(IDC_EDIT_ROYA_RTC_WDAY)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("%d", stRoyaRealTimeClock.hours);
	GetDlgItem(IDC_EDIT_ROYA_RTC_HOUR)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("%d", stRoyaRealTimeClock.mins);
	GetDlgItem(IDC_EDIT_ROYA_RTC_MIN)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("%d", stRoyaRealTimeClock.secs);
	GetDlgItem(IDC_EDIT_ROYA_RTC_SECOND)->SetWindowTextA(cstrTemp);

}
/// Real-time Clock related
// IDC_EDIT_ROYA_RTC_YEAR
void CDlgTestRoyaBoard::OnEnChangeEditRoyaRtcYear()
{
}
// IDC_EDIT_ROYA_RTC_MONTH
void CDlgTestRoyaBoard::OnEnChangeEditRoyaRtcMonth()
{
}
// IDC_EDIT_ROYA_RTC_MDATE
void CDlgTestRoyaBoard::OnEnChangeEditRoyaRtcMdate()
{
}
// IDC_EDIT_ROYA_RTC_WDAY
void CDlgTestRoyaBoard::OnEnChangeEditRoyaRtcWday()
{
}
// IDC_EDIT_ROYA_RTC_HOUR
void CDlgTestRoyaBoard::OnEnChangeEditRoyaRtcHour()
{
}
// IDC_EDIT_ROYA_RTC_MIN
void CDlgTestRoyaBoard::OnEnChangeEditRoyaRtcMin()
{
}
// IDC_EDIT_ROYA_RTC_SECOND
void CDlgTestRoyaBoard::OnEnChangeEditRoyaRtcSecond()
{
} 

#include <time.h>
// IDC_BUTTON_SET_ROYA_RT_CLOCK
void CDlgTestRoyaBoard::OnBnClickedButtonSetRoyaRtClock()
{
static struct tm stTime;
static struct tm *stpTime = &stTime;
static __time64_t stLongTime;

	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);

	stRoyaRealTimeClock.year = stpTime->tm_year + 1900; 
	stRoyaRealTimeClock.mon = stpTime->tm_mon; 
	stRoyaRealTimeClock.mday = stpTime->tm_mday; 
	stRoyaRealTimeClock.wday = stpTime->tm_wday; 
	stRoyaRealTimeClock.hours = stpTime->tm_hour; 
	stRoyaRealTimeClock.mins = stpTime->tm_min; 
	stRoyaRealTimeClock.secs = stpTime->tm_sec; 

short sRet;
#ifdef TEST_BSD_API
	sRet = roya_api_set_rt_clock(&stRoyaRealTimeClock);
#else
	sRet = roya_6412_set_rt_clock(&stRoyaRealTimeClock);
#endif
	RoyaDlgCheckAndShowDebugMessage(sRet);
}

// IDC_STATIC_BSD_ISR_FREQ_FACTOR_10KHZ
// IDC_EDIT_BSD_ISR_FREQ_FACTOR_10KHZ

void CDlgTestRoyaBoard::SetUserInterfaceLanguage(int iLanguageOption)
{
	if(iLanguageOption == LANGUAGE_UI_EN)
	{
		GetDlgItem(IDC_STATIC_LABEL_ROYA_COMMUNICATION_TEST)->SetWindowTextA(_T("Hardware Communication"));
		GetDlgItem(IDC_ROYA_TEST_BUTTON_SEND_COMMTEST)->SetWindowTextA(_T("Send&Bounce"));
		GetDlgItem(IDC_BUTTON_ROYA_GET_VERSION)->SetWindowTextA(_T("Version"));
		GetDlgItem(IDC_BTN_ROYA_TEST_LCD)->SetWindowTextA(_T("ToLCD"));
		GetDlgItem(IDC_ROYA_BOARD_SET_TIMER_PERIOD)->SetWindowTextA(_T("SetTimerPer"));
		GetDlgItem(IDC_BUTTON_ROYA_SET_DAC)->SetWindowTextA(_T("SetDAC(V)"));
		GetDlgItem(IDC_STATIC_ROYA_READ_ADC)->SetWindowTextA(_T("ReadADC(V)"));
		GetDlgItem(IDC_ROYA_ADDA_CHECK_AUTO)->SetWindowTextA(_T("PeriodicalCheck"));
		GetDlgItem(IDC_STATIC_LABEL_ROYA_REAL_TIME_CLOCK)->SetWindowTextA(_T("RealTimeClock"));
		GetDlgItem(IDC_BUTTON_SET_ROYA_RT_CLOCK)->SetWindowTextA(_T("UpdateBySystem"));
		GetDlgItem(IDC_STATIC_ROYA_RTC_YEAR)->SetWindowTextA(_T("Year"));
		GetDlgItem(IDC_STATIC_ROYA_RTC_MONTH)->SetWindowTextA(_T("Month"));
		GetDlgItem(IDC_STATIC_ROYA_RTC_MDATE)->SetWindowTextA(_T("MDate"));
		GetDlgItem(IDC_STATIC_ROYA_RTC_WDAY)->SetWindowTextA(_T("WDay"));
		GetDlgItem(IDC_STATIC_ROYA_RTC_HOUR)->SetWindowTextA(_T("Hour"));
		GetDlgItem(IDC_STATIC_ROYA_RTC_MINUTE)->SetWindowTextA(_T("Min"));
		GetDlgItem(IDC_STATIC_ROYA_RTC_SECOND)->SetWindowTextA(_T("Sec"));
		// USG
		GetDlgItem(IDC_STATIC_LABEL_ROYA_USG_PROF_SETTING)->SetWindowTextA(_T("USGProfByRoya"));
		GetDlgItem(IDC_STATIC_USG_PARA_BLK_IDX)->SetWindowTextA(_T("Blk"));
		GetDlgItem(IDC_STATIC_ROYA_USG_PROF_DURATION)->SetWindowTextA(_T("Duration"));
		GetDlgItem(IDC_STATIC_ROYA_USG_PROF_RAMP_TIME)->SetWindowTextA(_T("RampTime"));
		GetDlgItem(IDC_STATIC_ROYA_USG_PROF_TRIG_PATTERN)->SetWindowTextA(_T("TrigPattern"));
		GetDlgItem(IDC_STATIC_ROYA_USG_PROF_AMPLITUDE)->SetWindowTextA(_T("Amplitude"));
		GetDlgItem(IDC_STATIC_ROYA_USG_PROF_CMD_TYPE)->SetWindowTextA(_T("CmdType"));
		GetDlgItem(IDC_STATIC_ROYA_USG_PROF_NEXT_BLK_IDX)->SetWindowTextA(_T("NextBlk"));
		GetDlgItem(IDC_STATIC_ROYA_USG_PROF_TRIG_ADDRESS)->SetWindowTextA(_T("TrigAddress"));
		GetDlgItem(IDC_STATIC_ROYA_USG_PROF_MAX_CNT_PROTECT)->SetWindowTextA(_T("MaxCntProt"));
		GetDlgItem(IDC_ROYA_BUTTON_SET_USG_SEG)->SetWindowTextA(_T("SetParaSeg"));
		GetDlgItem(IDC_START_ROYA_USG_PROF_FSM)->SetWindowTextA(_T("StartUSG"));
		GetDlgItem(IDC_BUTTON_WARM_RESET_ROYA_USG)->SetWindowTextA(_T("WarmReset"));
		GetDlgItem(IDC_BUTTON_TRIG_ROYA_USG_VIA_ACS)->SetWindowTextA(_T("TrigByAcs"));
		GetDlgItem(IDC_STATIC_ROYA_USG_PROF_TRIG_PATTERN_BY_ACS)->SetWindowTextA(_T("TrigPattern"));
		GetDlgItem(IDC_ROYA_USG_CHECK_AUTO)->SetWindowTextA(_T("Periodically Check"));
		
		// BSD related
		GetDlgItem(IDC_STATIC_LABEL_ROYA_BSD_MONITOR)->SetWindowTextA(_T("Bond Stick Detection"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_PARAMETER_BLOCK_NO)->SetWindowTextA(_T("ParaBlk"));
		GetDlgItem(IDC_STATIC_USG_PARA_SEG_IDX)->SetWindowTextA(_T("SegIdx"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_PARA_FREQ_FACTOR)->SetWindowTextA(_T("FactorISR"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_PARA_SAMPLE_LEN)->SetWindowTextA(_T("SampleLen"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_PARA_ADC_IDX)->SetWindowTextA(_T("AdcIdx"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_PARA_DAC_IDX)->SetWindowTextA(_T("DacIdx"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_PARA_DAC_VALUE)->SetWindowTextA(_T("DacVal"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_PARA_DAC_BUFF_IDX)->SetWindowTextA(_T("DatBufIdx"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_PARA_NEXT_BLK_IDX)->SetWindowTextA(_T("NextBlk"));
		GetDlgItem(IDC_ROYA_BSD_CHECK_AUTO)->SetWindowTextA(_T("AutoCheck"));
		GetDlgItem(IDC_ROYA_BSD_BUTTON_SET_BLK_PARA)->SetWindowTextA(_T("SetParaBlk"));
		GetDlgItem(IDC_ROYA_BSD_BUTTON_START_W_BLK)->SetWindowTextA(_T("StartBlk"));
		GetDlgItem(IDC_ROYA_BSD_BUTTON_WARM_RESET)->SetWindowTextA(_T("WarmReset"));
		GetDlgItem(IDC_ROYA_BSD_BUTTON_GET_DATA)->SetWindowTextA(_T("GetDataAna"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_STATUS)->SetWindowTextA(_T("BSD-Status"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_STATUS_SAMPLE_CNT)->SetWindowTextA(_T("SampleCnt"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_STATUS_START_FLAG)->SetWindowTextA(_T("StartFlag"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_STATUS_BLK_IDX)->SetWindowTextA(_T("BlkIdx"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_STATUS_TRIG_FLAG)->SetWindowTextA(_T("TrigFlag"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_STATUS_TRIG_CNT)->SetWindowTextA(_T("TrigCnt"));
		GetDlgItem(IDC_STATIC_LABEL_ROYA_EFO_FEEDBACK)->SetWindowTextA(_T("EFO_Fb"));
		GetDlgItem(IDC_ROYA_BSD_BUTTON_API_SET_BLK_PARA)->SetWindowTextA(_T("ApiSetBlk"));
		GetDlgItem(IDC_ROYA_BSD_BUTTON_API_GET_BLK_PARA)->SetWindowTextA(_T("ApiGetBlk"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_DATA_ANA_MAX)->SetWindowTextA(_T("Max"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_DATA_ANA_MIN)->SetWindowTextA(_T("Min"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_DATA_ANA_MEAN)->SetWindowTextA(_T("Mean"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_DATA_ANA_STDEV)->SetWindowTextA(_T("StDev"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_DATA_ANA_VOUTPUT)->SetWindowTextA(_T("Vout"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_DATA_ANA_VREAD)->SetWindowTextA(_T("Vread"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_DATA_2ND_BOND_RISING_EDGE_CNT)->SetWindowTextA(_T("RisingEdgeCnt"));
		GetDlgItem(IDC_STATIC_BSD_ISR_FREQ_FACTOR_10KHZ)->SetWindowTextA(_T("ISR_b_10KHz"));
		GetDlgItem(IDC_ROYA_BSD_FLAG_CHECK_DRY_RUN)->SetWindowTextA(_T("DryRunBSD"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_SUGGEST_STICKNESS_PARA)->SetWindowTextA(_T("SugPara"));

		GetDlgItem(IDC_ROYA_BSD_BUTTON_E_TEST_WIRING)->SetWindowTextA(_T("RoyaE-Test"));
		GetDlgItem(IDC_ROYA_BSD_FLAG_CHECK_WAVEFORM)->SetWindowTextA(_T("ShowData"));

//		GetDlgItem(IDC_BUTTON_ROYA_BSD_DLG_TEST_PING_START_DEAD)->SetWindowTextA(_T("Test-Comm_Start-Dead"));
		
			
	}
	else
	{
//		GetDlgItem(IDC_BUTTON_ROYA_BSD_DLG_TEST_PING_START_DEAD)->SetWindowTextA(_T("通讯测试-启动死机"));

		GetDlgItem(IDC_STATIC_LABEL_ROYA_COMMUNICATION_TEST)->SetWindowTextA(_T("通讯"));
		GetDlgItem(IDC_ROYA_TEST_BUTTON_SEND_COMMTEST)->SetWindowTextA(_T("回音，启动死机"));
		GetDlgItem(IDC_BUTTON_ROYA_GET_VERSION)->SetWindowTextA(_T("版本"));
		GetDlgItem(IDC_BTN_ROYA_TEST_LCD)->SetWindowTextA(_T("数字IO"));
		GetDlgItem(IDC_ROYA_BOARD_SET_TIMER_PERIOD)->SetWindowTextA(_T("定时器周期"));
		GetDlgItem(IDC_BUTTON_ROYA_SET_DAC)->SetWindowTextA(_T("设定电压"));  // [0~1023]:[-12 ~12]
		GetDlgItem(IDC_STATIC_ROYA_READ_ADC)->SetWindowTextA(_T("反馈电压(V)"));
		GetDlgItem(IDC_ROYA_ADDA_CHECK_AUTO)->SetWindowTextA(_T("循环检测"));
		GetDlgItem(IDC_STATIC_LABEL_ROYA_REAL_TIME_CLOCK)->SetWindowTextA(_T("实时时钟"));
		GetDlgItem(IDC_BUTTON_SET_ROYA_RT_CLOCK)->SetWindowTextA(_T("更新时间"));
		GetDlgItem(IDC_STATIC_ROYA_RTC_YEAR)->SetWindowTextA(_T("年"));
		GetDlgItem(IDC_STATIC_ROYA_RTC_MONTH)->SetWindowTextA(_T("月"));
		GetDlgItem(IDC_STATIC_ROYA_RTC_MDATE)->SetWindowTextA(_T("日"));
		GetDlgItem(IDC_STATIC_ROYA_RTC_WDAY)->SetWindowTextA(_T("周"));
		GetDlgItem(IDC_STATIC_ROYA_RTC_HOUR)->SetWindowTextA(_T("时"));
		GetDlgItem(IDC_STATIC_ROYA_RTC_MINUTE)->SetWindowTextA(_T("分"));
		GetDlgItem(IDC_STATIC_ROYA_RTC_SECOND)->SetWindowTextA(_T("秒"));

		// USG related
		GetDlgItem(IDC_STATIC_LABEL_ROYA_USG_PROF_SETTING)->SetWindowTextA(_T("超声包络"));
		GetDlgItem(IDC_STATIC_USG_PARA_BLK_IDX)->SetWindowTextA(_T("参数块"));
		GetDlgItem(IDC_STATIC_USG_PARA_SEG_IDX)->SetWindowTextA(_T("参数单元"));
		GetDlgItem(IDC_STATIC_ROYA_USG_PROF_DURATION)->SetWindowTextA(_T("电平时段"));
		GetDlgItem(IDC_STATIC_ROYA_USG_PROF_RAMP_TIME)->SetWindowTextA(_T("1阶坡时段"));
		GetDlgItem(IDC_STATIC_ROYA_USG_PROF_TRIG_PATTERN)->SetWindowTextA(_T("触发口令"));
		GetDlgItem(IDC_STATIC_ROYA_USG_PROF_AMPLITUDE)->SetWindowTextA(_T("超声幅值"));
		GetDlgItem(IDC_STATIC_ROYA_USG_PROF_CMD_TYPE)->SetWindowTextA(_T("命令类型"));
		GetDlgItem(IDC_STATIC_ROYA_USG_PROF_NEXT_BLK_IDX)->SetWindowTextA(_T("下一参数块"));
		GetDlgItem(IDC_STATIC_ROYA_USG_PROF_TRIG_ADDRESS)->SetWindowTextA(_T("触发地址"));
		GetDlgItem(IDC_STATIC_ROYA_USG_PROF_MAX_CNT_PROTECT)->SetWindowTextA(_T("保护超时"));
		GetDlgItem(IDC_ROYA_BUTTON_SET_USG_SEG)->SetWindowTextA(_T("设定参数单元"));
		GetDlgItem(IDC_START_ROYA_USG_PROF_FSM)->SetWindowTextA(_T("开始超声"));
		GetDlgItem(IDC_BUTTON_WARM_RESET_ROYA_USG)->SetWindowTextA(_T("重置"));
		GetDlgItem(IDC_BUTTON_TRIG_ROYA_USG_VIA_ACS)->SetWindowTextA(_T("从ACS触发"));
		GetDlgItem(IDC_STATIC_ROYA_USG_PROF_TRIG_PATTERN_BY_ACS)->SetWindowTextA(_T("触发口令"));
		GetDlgItem(IDC_ROYA_USG_CHECK_AUTO)->SetWindowTextA(_T("循环检测"));

		// BSD related
		GetDlgItem(IDC_STATIC_LABEL_ROYA_BSD_MONITOR)->SetWindowTextA(_T("焊线检测"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_PARAMETER_BLOCK_NO)->SetWindowTextA(_T("参数单元"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_PARA_FREQ_FACTOR)->SetWindowTextA(_T("分频"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_PARA_SAMPLE_LEN)->SetWindowTextA(_T("采样点长"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_PARA_ADC_IDX)->SetWindowTextA(_T("模数编号"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_PARA_DAC_IDX)->SetWindowTextA(_T("数模编号"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_PARA_DAC_VALUE)->SetWindowTextA(_T("模拟量"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_PARA_DAC_BUFF_IDX)->SetWindowTextA(_T("缓存编号"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_PARA_NEXT_BLK_IDX)->SetWindowTextA(_T("下个单元"));
		GetDlgItem(IDC_ROYA_BSD_CHECK_AUTO)->SetWindowTextA(_T("循环检测"));
		GetDlgItem(IDC_ROYA_BSD_BUTTON_SET_BLK_PARA)->SetWindowTextA(_T("设定参数单元"));
		GetDlgItem(IDC_ROYA_BSD_BUTTON_START_W_BLK)->SetWindowTextA(_T("开始当前单元"));
		GetDlgItem(IDC_ROYA_BSD_BUTTON_WARM_RESET)->SetWindowTextA(_T("重置"));
		GetDlgItem(IDC_ROYA_BSD_BUTTON_GET_DATA)->SetWindowTextA(_T("读数分析"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_STATUS)->SetWindowTextA(_T("检测状态"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_STATUS_SAMPLE_CNT)->SetWindowTextA(_T("采集长度"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_STATUS_START_FLAG)->SetWindowTextA(_T("开始标志"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_STATUS_BLK_IDX)->SetWindowTextA(_T("单元编号"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_STATUS_TRIG_FLAG)->SetWindowTextA(_T("触发标志"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_STATUS_TRIG_CNT)->SetWindowTextA(_T("触发次数"));
		GetDlgItem(IDC_STATIC_LABEL_ROYA_EFO_FEEDBACK)->SetWindowTextA(_T("电火花反馈"));
		GetDlgItem(IDC_ROYA_BSD_BUTTON_API_SET_BLK_PARA)->SetWindowTextA(_T("接口设参"));
		GetDlgItem(IDC_ROYA_BSD_BUTTON_API_GET_BLK_PARA)->SetWindowTextA(_T("接口读参"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_DATA_ANA_MAX)->SetWindowTextA(_T("最大"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_DATA_ANA_MIN)->SetWindowTextA(_T("最小"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_DATA_ANA_MEAN)->SetWindowTextA(_T("平均"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_DATA_ANA_STDEV)->SetWindowTextA(_T("标准差"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_DATA_ANA_VOUTPUT)->SetWindowTextA(_T("设定电压"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_DATA_ANA_VREAD)->SetWindowTextA(_T("平均电压"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_DATA_2ND_BOND_RISING_EDGE_CNT)->SetWindowTextA(_T("开路时点"));
		GetDlgItem(IDC_STATIC_BSD_ISR_FREQ_FACTOR_10KHZ)->SetWindowTextA(_T("ISR分频10K"));
		GetDlgItem(IDC_ROYA_BSD_FLAG_CHECK_DRY_RUN)->SetWindowTextA(_T("检测烤机"));
		GetDlgItem(IDC_STATIC_ROYA_BSD_SUGGEST_STICKNESS_PARA)->SetWindowTextA(_T("推荐参数"));		
		GetDlgItem(IDC_ROYA_BSD_BUTTON_E_TEST_WIRING)->SetWindowTextA(_T("自动测试"));

		GetDlgItem(IDC_ROYA_BSD_FLAG_CHECK_WAVEFORM)->SetWindowTextA(_T("显示数据"));
	}
	
}

// IDC_SLIDER_SET_DAC_VOLT
void CDlgTestRoyaBoard::OnBnClickedOk()
{
//	StopTimer();
	iDlgRoyaBdShowFlag = 0;  // 20120202

	OnOK();
}

void CDlgTestRoyaBoard::OnBnClickedCancel()
{
//	StopTimer();
	iDlgRoyaBdShowFlag = 0;  // 20120202

	OnCancel();
}

void CDlgTestRoyaBoard::OnCbnSelchangeRoyaComboTimerId()
{
	// TODO: Add your control notification handler code here
char cTimerId = ((CComboBox *)GetDlgItem(IDC_ROYA_COMBO_TIMER_ID))->GetCurSel();
static unsigned int uiTimerPeriod;    

	roya_api_get_timer_period_clock_cnt(cTimerId, &uiTimerPeriod);

CString cstrTemp;
	cstrTemp.Format("%d", uiTimerPeriod);
	GetDlgItem(IDC_ROYA_BOARD_TIMER_PERIOD)->SetWindowTextA(cstrTemp);

}

// IDC_EDIT_ROYA_BSD_DEBOUNCE_1ST_BOND // stInputAnalyzeBSD.us1stBondNumSamplesConsecGreaterDieThAdc
void CDlgTestRoyaBoard::OnEnChangeEditRoyaBsdDebounce1stBond()
{
	ReadUShortFromEdit(IDC_EDIT_ROYA_BSD_DEBOUNCE_1ST_BOND, &(stInputAnalyzeBSD.us1stBondNumSamplesConsecGreaterDieThAdc));
}
// IDC_EDIT_ROYA_BSD_DEBOUNCE_2ND_BOND
void CDlgTestRoyaBoard::OnEnChangeEditRoyaBsdDebounce2ndBond()
{
	ReadUShortFromEdit(IDC_EDIT_ROYA_BSD_DEBOUNCE_2ND_BOND, &(stInputAnalyzeBSD.us2ndBondNumSampleConsecHigher));
}
// IDC_EDIT_ROYA_BSD_THRE_HOLD_V_1ST_BOND, stInputAnalyzeBSD.fBSD1stBondStickDieThAdc_v
void CDlgTestRoyaBoard::OnEnChangeEditRoyaBsdThreHoldV1stBond()
{
	ReadFloatFromEdit(IDC_EDIT_ROYA_BSD_THRE_HOLD_V_1ST_BOND, &(stInputAnalyzeBSD.fBSD1stBondStickDieThAdc_v));
}
// IDC_EDIT_ROYA_BSD_THRE_HOLD_V_2ND_BOND
void CDlgTestRoyaBoard::OnEnChangeEditRoyaBsdThreHoldV2ndBond()
{
	ReadFloatFromEdit(IDC_EDIT_ROYA_BSD_THRE_HOLD_V_2ND_BOND, &(stInputAnalyzeBSD.fBSD2ndBondNonStickThAdc_v));
}
// IDC_CHECK_CHECK_ROYA_COMMUNICATION_BY_BSD

// IDC_EDIT_ROYA_BSD_DEBOUNCE_1ST_BOND_CONSE_LOW
void CDlgTestRoyaBoard::OnEnChangeEditRoyaBsdDebounce1stBondConseLow()
{
	// TODO:  Add your control notification handler code here
	ReadUShortFromEdit(IDC_EDIT_ROYA_BSD_DEBOUNCE_1ST_BOND_CONSE_LOW, &(stInputAnalyzeBSD.us1stBondNumSamplesConsecLowerLeadThAdc));
}
// IDC_EDIT_ROYA_BSD_DEBOUNCE_2ND_BOND_CONSE_LOW
void CDlgTestRoyaBoard::OnEnChangeEditRoyaBsdDebounce2ndBondConseLow()
{
	// TODO:  Add your control notification handler code here
	ReadUShortFromEdit(IDC_EDIT_ROYA_BSD_DEBOUNCE_2ND_BOND_CONSE_LOW, &(stInputAnalyzeBSD.us2ndBondNumSamplesConsecLowerLeadThAdc));
}
// IDC_EDIT_ROYA_BSD_SKIP_SAMPLE_1ST_BOND
void CDlgTestRoyaBoard::OnEnChangeEditRoyaBsdSkipSample1stBond()
{
	// TODO:  Add your control notification handler code here
	ReadUShortFromEdit(IDC_EDIT_ROYA_BSD_SKIP_SAMPLE_1ST_BOND, &(stInputAnalyzeBSD.us1stBondSkipSamples));
}
// IDC_EDIT_ROYA_BSD_SKIP_SAMPLE_2ND_BOND
void CDlgTestRoyaBoard::OnEnChangeEditRoyaBsdSkipSample2ndBond()
{
	// TODO:  Add your control notification handler code here
	ReadUShortFromEdit(IDC_EDIT_ROYA_BSD_SKIP_SAMPLE_2ND_BOND, &(stInputAnalyzeBSD.us2ndBondSkipSamples));
}
// IDC_EDIT_ROYA_BSD_THRE_HOLD_V_LOWER_LEAD_2ND_BOND
void CDlgTestRoyaBoard::OnEnChangeEditRoyaBsdThreHoldVLowerLead2ndBond()
{
	// TODO:  Add your control notification handler code here
	ReadFloatFromEdit(IDC_EDIT_ROYA_BSD_THRE_HOLD_V_LOWER_LEAD_2ND_BOND, &(stInputAnalyzeBSD.fBSD2ndBondStickLeadThAdc_v));
}

// IDC_ROYA_BSD_BUTTON_API_SET_ANALYZE_STICKNESS_PARA
void CDlgTestRoyaBoard::OnBnClickedRoyaBsdButtonApiSetAnalyzeSticknessPara()
{
	// TODO: Add your control notification handler code here
	bsd_api_set_parameter_firmware_analyze_stickness(&stInputAnalyzeBSD);
}
// IDC_ROYA_BSD_BUTTON_API_GET_ANALYZE_STICKNESS_PARA
void CDlgTestRoyaBoard::OnBnClickedRoyaBsdButtonApiGetAnalyzeSticknessPara()
{
	// TODO: Add your control notification handler code here
	bsd_api_get_parameter_firmware_analyze_stickness(&stInputAnalyzeBSD);
	//
	UpdateUShortToEdit(IDC_EDIT_ROYA_BSD_DEBOUNCE_1ST_BOND, stInputAnalyzeBSD.us1stBondNumSamplesConsecGreaterDieThAdc);
	UpdateUShortToEdit(IDC_EDIT_ROYA_BSD_DEBOUNCE_2ND_BOND, stInputAnalyzeBSD.us2ndBondNumSampleConsecHigher);
	UpdateUShortToEdit(IDC_EDIT_ROYA_BSD_SKIP_SAMPLE_1ST_BOND, stInputAnalyzeBSD.us1stBondSkipSamples);
	UpdateUShortToEdit(IDC_EDIT_ROYA_BSD_SKIP_SAMPLE_2ND_BOND, stInputAnalyzeBSD.us2ndBondSkipSamples);
	UpdateUShortToEdit(IDC_EDIT_ROYA_BSD_DEBOUNCE_1ST_BOND_CONSE_LOW, stInputAnalyzeBSD.us1stBondNumSamplesConsecLowerLeadThAdc);
	UpdateUShortToEdit(IDC_EDIT_ROYA_BSD_DEBOUNCE_2ND_BOND_CONSE_LOW, stInputAnalyzeBSD.us2ndBondNumSamplesConsecLowerLeadThAdc);

	// Voltage, 1st Bond
	UpdateFloatToEdit(IDC_EDIT_ROYA_BSD_THRE_HOLD_V_1ST_BOND, stInputAnalyzeBSD.fBSD1stBondStickDieThAdc_v);
	// Voltage, 2nd Bond
	UpdateFloatToEdit(IDC_EDIT_ROYA_BSD_THRE_HOLD_V_2ND_BOND, stInputAnalyzeBSD.fBSD2ndBondNonStickThAdc_v);
	UpdateFloatToEdit(IDC_EDIT_ROYA_BSD_THRE_HOLD_V_LOWER_LEAD_2ND_BOND, stInputAnalyzeBSD.fBSD2ndBondStickLeadThAdc_v);

}
BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT stBsdFirmwareAnalyzeSticknessOutput;
// IDC_ROYA_BSD_BUTTON_GET_FIRMWARE_ANALYZE
void CDlgTestRoyaBoard::OnBnClickedRoyaBsdButtonGetFirmwareAnalyze()
{
	// TODO: Add your control notification handler code here
	bsd_api_get_firmware_analyze_stickness_output(&stBsdFirmwareAnalyzeSticknessOutput);

	UpdateSticknessStatusUI();
}
// IDC_ROYA_BSD_BUTTON_FIRMWARE_ANALYZE_STICKNESS
void CDlgTestRoyaBoard::OnBnClickedRoyaBsdButtonFirmwareAnalyzeStickness()
{
	short sRet = roya_6412_bsd_get_status(&stStatusBSD);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	unsigned int uiCurrBlkIdx = stStatusBSD.cBlkIdxBSD;
	unsigned int uiPrevBlkIdx = stStatusBSD.cBlkIdxBSD - 1;
	if(uiPrevBlkIdx >= _BSD_MAX_BLK_PARAMETER_)
	{
		uiPrevBlkIdx = _BSD_MAX_BLK_PARAMETER_ - 1;
	}
	// Analyze by firmware
	sRet = bsd_api_set_parameter_firmware_analyze_stickness(&stInputAnalyzeBSD);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	sRet = bsd_api_firmware_analyze_stickness(uiPrevBlkIdx, uiCurrBlkIdx, &stBsdFirmwareAnalyzeSticknessOutput);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	UpdateSticknessStatusUI();
}

void CDlgTestRoyaBoard::UpdateSticknessStatusUI()
{
	// 1st bond
	CString cstrTemp;
	cstrTemp.Format("%s", pstrBSD_AnalyseStatus[stBsdFirmwareAnalyzeSticknessOutput.us1stBondStickStatus]);
	GetDlgItem(IDC_ROYA_BSD_DATA_STICKNESS_1ST_BOND)->SetWindowTextA(cstrTemp);
	// 2nd bond
	cstrTemp.Format("%s", pstrBSD_AnalyseStatus[stBsdFirmwareAnalyzeSticknessOutput.us2ndBondStickStatus]);
	GetDlgItem(IDC_ROYA_BSD_DATA_STICKNESS_2ND_BOND)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("L-%d,R-%d", stBsdFirmwareAnalyzeSticknessOutput.us2ndBondStartSampleStickness, 
		stBsdFirmwareAnalyzeSticknessOutput.us2ndBondOpenSticknessFirstRiseCount);
	GetDlgItem(IDC_SHOW_ROYA_BSD_DATA_2ND_BOND_RISING_EDGE_CNT)->SetWindowTextA(cstrTemp);

}
// IDC_CHECK_CHECK_ROYA_COMMUNICATION_BY_BSD
void CDlgTestRoyaBoard::OnBnClickedCheckCheckRoyaCommunicationByBsd()
{
	// TODO: Add your control notification handler code here
	cFlagBSDAnalyzeByFirmware = ((CButton*)GetDlgItem(IDC_CHECK_CHECK_ROYA_COMMUNICATION_BY_BSD))->GetCheck();
}

// IDC_ROYA_TEST_CHECK_ENABLE_TIMER_ALL
void CDlgTestRoyaBoard::OnBnClickedRoyaTestCheckEnableTimerAll()
{
	iFlagCheckEnableTimerAll = ((CButton*)GetDlgItem(IDC_ROYA_TEST_CHECK_ENABLE_TIMER_ALL))->GetCheck();
}

static unsigned char pcCheckFirmwareUploadBuffer[__MAX_FLASH_READ_LEN];
#define ROYA_ERROR_UPLOAD			111
#define ROYA_ERROR_VERIFY_FIRMWARE	112
int RoyaVerifyDownloadFirmware(int iStartAddress, int iLen, unsigned char * pcDownloadCopy)
{
	int iCheckRet = MTN_API_OK_ZERO;

		memset(pcCheckFirmwareUploadBuffer, 0, sizeof(pcCheckFirmwareUploadBuffer));
		
		if(MTN_API_OK_ZERO != roya_6412_read_flash(iStartAddress, iLen, pcCheckFirmwareUploadBuffer))
		{
			iCheckRet = ROYA_ERROR_UPLOAD;
			return iCheckRet;
		}
		else
		{
			if(strcmp((char*)pcDownloadCopy, (char*)pcCheckFirmwareUploadBuffer))
			{ // NOT equal 0, <=> There is difference between upload and download
				iCheckRet = ROYA_ERROR_VERIFY_FIRMWARE;
				CString cstrTemp;
				cstrTemp.Format("Error Verify Download: StartAddr:%d, First 5 Downl: %x %x %x %x %x, Upl: %x %x %x %x %x", 
					iStartAddress, 
					pcDownloadCopy[0], pcDownloadCopy[1], pcDownloadCopy[2], pcDownloadCopy[3], pcDownloadCopy[4],
					pcCheckFirmwareUploadBuffer[0], pcCheckFirmwareUploadBuffer[1], pcCheckFirmwareUploadBuffer[2], pcCheckFirmwareUploadBuffer[3], pcCheckFirmwareUploadBuffer[4]);
				AfxMessageBox(cstrTemp);

				return iCheckRet;

			}
		}
	return iCheckRet;

}

void RoyaErrorReport(int iErrorCode)
{
	CString cstrTemp;
	switch(iErrorCode)
	{
		case ROYA_ERROR_UPLOAD:
			AfxMessageBox(_T("读取失误"));
			break;
		case ROYA_ERROR_VERIFY_FIRMWARE:
			break;

		default:
			break;
	}
}

//
static unsigned char pcCheckFirmwareDownloadCopy[__MAX_FLASH_READ_LEN];
// IDC_ROYA_BUTTON_DOWNLOAD_FIRMWARE
void CDlgTestRoyaBoard::OnBnClickedRoyaButtonDownloadFirmware()
{
	int iVerifyRet;

	// TODO: Add your control notification handler code here
//	roya_firmware_download_eth();

//	unsigned int uiSizeFirmware = roya_firmware_get_size();

	unsigned int uiDownloadLoop = (int)(uiDownloadSize/__MAX_FLASH_READ_LEN);

	unsigned int uiDownloadLastLoopSize = uiDownloadSize %__MAX_FLASH_READ_LEN;

	unsigned int uiStartAddressRelativeFlash;
	((CProgressCtrl*)GetDlgItem(IDC_ROYA_DOWNLOAD_PROGRESS))->ShowWindow(TRUE);
	((CProgressCtrl*)GetDlgItem(IDC_ROYA_DOWNLOAD_PROGRESS))->SetRange32(0, uiDownloadLoop);

	short sRet= MTN_API_OK_ZERO;
	int iTryOnError=0;

	uiStartAddressRelativeFlash = 0;
	for(unsigned int ii = 0; ii<uiDownloadLoop; ii++)
	{
		sRet = roya_6412_write_flash(uiStartAddressRelativeFlash, __MAX_FLASH_READ_LEN, (unsigned char *)&(pcDownloadBuffer[uiStartAddressRelativeFlash]));

		if(sRet != MTN_API_OK_ZERO)
		{
			return ; //sRet;
		}
		else
		{
			((CProgressCtrl*)GetDlgItem(IDC_ROYA_DOWNLOAD_PROGRESS))->SetPos(ii);
			if(iFlagVerifyDownloadRoyaFirmware) // need to check firmware
			{
				iTryOnError = 0;
				memcpy_s(pcCheckFirmwareDownloadCopy, __MAX_FLASH_READ_LEN, &(pcDownloadBuffer[uiStartAddressRelativeFlash]), __MAX_FLASH_READ_LEN);
				if( iVerifyRet = RoyaVerifyDownloadFirmware(uiStartAddressRelativeFlash, __MAX_FLASH_READ_LEN, pcCheckFirmwareDownloadCopy))
				{
					while(iTryOnError <=3)
					{
						roya_6412_write_flash(uiStartAddressRelativeFlash, __MAX_FLASH_READ_LEN, (unsigned char *)&(pcDownloadBuffer[uiStartAddressRelativeFlash]));

						iVerifyRet = RoyaVerifyDownloadFirmware(uiStartAddressRelativeFlash, __MAX_FLASH_READ_LEN, pcCheckFirmwareDownloadCopy);
						if(iVerifyRet == MTN_API_OK_ZERO)
						{
							iTryOnError = 4;
						}
						else
						{
							iTryOnError ++;
						}
					}
				}
				if(iVerifyRet)
				{
					RoyaErrorReport(iVerifyRet);
				}
			}
		}

		uiStartAddressRelativeFlash = uiStartAddressRelativeFlash + __MAX_FLASH_READ_LEN;

	}

	sRet = roya_6412_write_flash(uiStartAddressRelativeFlash, uiDownloadLastLoopSize, (unsigned char *)&(pcDownloadBuffer[uiStartAddressRelativeFlash]));
	((CProgressCtrl*)GetDlgItem(IDC_ROYA_DOWNLOAD_PROGRESS))->ShowWindow(FALSE);

	if(iFlagVerifyDownloadRoyaFirmware) // need to check firmware
	{
		memcpy_s(pcCheckFirmwareDownloadCopy, uiDownloadLastLoopSize, &(pcDownloadBuffer[uiStartAddressRelativeFlash]), uiDownloadLastLoopSize);
		if( iVerifyRet = RoyaVerifyDownloadFirmware(uiStartAddressRelativeFlash, uiDownloadLastLoopSize, pcCheckFirmwareDownloadCopy))
			RoyaErrorReport(iVerifyRet);
	}

//	return sRet;

}


// IDC_ROYA_BUTTON_LOAD_FIRMWARE_BIN
void CDlgTestRoyaBoard::OnBnClickedRoyaButtonLoadFirmwareBin()
{
	// TODO: Add your control notification handler code here
	CFileDialog cFileDownloadBin(TRUE, NULL, "*.bin",OFN_HIDEREADONLY,"Bin   Files(*.*)|*.*||",NULL);   ;
	CString cstrTemp;
	int   iResult   = (int)cFileDownloadBin.DoModal();
	if( iResult   !=   IDOK   )
	{
		return;   
	}   

	CFile cfDownloadFirmwareBin; // ,datfile2;

	cstrTemp = cFileDownloadBin.GetPathName();

//	cstrTemp.Append(cFileDownloadBin.GetFileName());
	if( !cfDownloadFirmwareBin.Open(cstrTemp, CFile::modeRead + CFile::typeBinary ) ) // c:\\a66101.bin
	{			//打开数据文件失败
//			TRACE_ERR() ;
		AfxMessageBox(_T("打开读数据文件失败!"));
		return;
	}
	uiDownloadSize = (unsigned int)cfDownloadFirmwareBin.GetLength( );  // ULONGLONG
	if(pcDownloadBuffer)
	{
		if(pcDownloadBuffer == (unsigned char *)code)
		{
			pcDownloadBuffer = NULL;
		}
		else
		{
			delete []pcDownloadBuffer ;
		}
	}
	pcDownloadBuffer = new unsigned char[ uiDownloadSize ];
	cfDownloadFirmwareBin.Read(pcDownloadBuffer , uiDownloadSize );
	cfDownloadFirmwareBin.Close();

	GetDlgItem(IDC_ROYA_STATIC_CURR_DOWNLOAD_FILENAME)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("Size: %d Byte", uiDownloadSize);
	GetDlgItem(IDC_ROYA_TEXT_FIRMWARE_SIZE)->SetWindowTextA(cstrTemp);

}

// IDC_ROYA_CHECK_DOWNLOAD_VERIFY
void CDlgTestRoyaBoard::OnBnClickedRoyaCheckDownloadVerify()
{
	iFlagVerifyDownloadRoyaFirmware = ((CButton*)GetDlgItem(IDC_ROYA_CHECK_DOWNLOAD_VERIFY))->GetCheck();
}

// IDC_ROYA_BSD_FLAG_CHECK_WAVEFORM
void CDlgTestRoyaBoard::OnBnClickedRoyaBsdFlagCheckWaveform()
{
	bFlagBSD_CheckByWaveForm = ((CButton*)GetDlgItem(IDC_ROYA_BSD_FLAG_CHECK_WAVEFORM))->GetCheck();  // (CBUTTON*)
	
	if(bFlagBSD_CheckByWaveForm)
	{
//		if(NULL == m_pBSD_WaveformByXls)
//		{
	//		MessageBox("Press 'Stop' to end automatic updating of the chart","Guide", MB_OK);
	//		m_pBSD_WaveformByXls = new CXLEzAutomation();  // (FALSE); // FALSE not visible
//		}
	}
	else
	{
//		if(NULL != m_pBSD_WaveformByXls)
//		{
//			m_pBSD_WaveformByXls->ReleaseExcel();
//			delete m_pBSD_WaveformByXls;
//			m_pBSD_WaveformByXls = NULL;
//		}
	}

}

// IDC_SLIDER_ROYA_SET_DAC_VOLT
void CDlgTestRoyaBoard::OnNMCustomdrawSliderRoyaSetDacVolt(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
//	UpdateDacVoltageValueBySlider();
	*pResult = 0;
}

void CDlgTestRoyaBoard::OnNMReleasedcaptureSliderRoyaSetDacVolt(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateDacVoltageValueBySlider();
	*pResult = 0;
}

void CDlgTestRoyaBoard::InitDacSliderRangePosn()
{
	VERSION_INFO stVersionRoya6412;
	bsd_api_get_version_roya_6412(&stVersionRoya6412); // 20110822
	if( roya_6412_get_flag_dac_bit() == ROYA_BSD_DAC_10b) // 20110822
	{
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER_ROYA_SET_DAC_VOLT))->SetRangeMax(1023);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER_ROYA_SET_DAC_VOLT))->SetRange(0, 1023);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER_ROYA_SET_DAC_VOLT))->SetPos(aiDacValue[uiDacIdx]);
	}
	else if(roya_6412_get_flag_dac_bit() == ROYA_BSD_DAC_12b)  // 20110822
	{
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER_ROYA_SET_DAC_VOLT))->SetRangeMax(4095);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER_ROYA_SET_DAC_VOLT))->SetRange(0, 4095);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER_ROYA_SET_DAC_VOLT))->SetPos(aiDacValue[uiDacIdx]);
	}
}
void CDlgTestRoyaBoard::UpdateDacVoltageValueBySlider()
{
	// 20130613
	aiDacValue[uiDacIdx] = (((CSliderCtrl*)GetDlgItem(IDC_SLIDER_ROYA_SET_DAC_VOLT))->GetPos() - 512) * adMaxPercentOutDAC[cIdxMaxPercentOutDAC] + 512;

	if( roya_6412_get_flag_dac_bit() == ROYA_BSD_DAC_10b) // 20110822
	{
		adDacVolt[uiDacIdx] = __CONVERT_DAC_TO_VOLT(aiDacValue[uiDacIdx]);
	}
	else if(roya_6412_get_flag_dac_bit() == ROYA_BSD_DAC_12b)  // 20110822
	{
		adDacVolt[uiDacIdx] = __CONVERT_DAC_TO_VOLT_12b(aiDacValue[uiDacIdx]); // 20110822
	}
	CString cstrTemp;
	cstrTemp.Format("%5.1f V", adDacVolt[uiDacIdx]);
	GetDlgItem(IDC_EDIT_ROYA_DAC_VALUE)->SetWindowTextA(cstrTemp);
}

#include "DlgKeyInputPad.h"
// IDC_ROYA_BSD_FLAG_CHECK_DRY_RUN
void CDlgTestRoyaBoard::OnBnClickedRoyaBsdFlagCheckDryRun()
{
	int iTemp;
	static int iFlagFailurePass = FALSE;
	iTemp = ((CButton*)GetDlgItem(IDC_ROYA_BSD_FLAG_CHECK_DRY_RUN))->GetCheck();
	if(iTemp == 1)
	{
		static CDlgKeyInputPad cDlgInputPad;
		cDlgInputPad.SetFlagShowNumberOnKeyPad(0);
		cDlgInputPad.SetInputNumber(0);
		if(cDlgInputPad.DoModal() == IDOK)
		{
			if(cDlgInputPad.GetReturnNumber()== wb_mtn_tester_get_password_brightlux_6d_zzy()) // wb_mtn_tester_get_password_sg_8d_zzy()
			{
				iFlagFailurePass = FALSE;
			}
			else
			{
				iFlagFailurePass = TRUE;
			}
		}
		else
		{
			iFlagFailurePass = TRUE;
		}
		if(iFlagFailurePass == TRUE)
		{
			iTemp = FALSE;
			((CButton *)GetDlgItem(IDC_ROYA_BSD_FLAG_CHECK_DRY_RUN))->SetCheck(FALSE);
		}

	}
	else
	{
		((CButton *)GetDlgItem(IDC_ROYA_BSD_FLAG_CHECK_DRY_RUN))->SetCheck(FALSE);
	}
	iFlagDryRunBSD = iTemp;

	if(iFlagDryRunBSD == 1)
	{
		if(bsd_api_get_efo_good_counter(&uiPrevEfoGoodFlag) == MTN_API_OK_ZERO)  // ETH_init-8
		{   // in case of communication error, only one time-out
			bsd_api_set_isr_freqency_factor_10KHz(2); // 5KHz ,  // ETH_init-9
		}
	}
}

static int iFlagStopThreadRoyaTestCmd = 1;
CWinThread* m_pWinThreadRoyaTestCmd;

UINT dlg_roya_test_thread()
{
		static char strSystemCommand[256];
		sprintf_s(strSystemCommand, 256, "ping 192.168.2.5 -t");
		system(strSystemCommand);
	return 0;
}

// Home by Thread
UINT dlg_roya_test_start_thread( LPVOID pParam )
{
	return dlg_roya_test_thread();
}

void dlg_roya_test_trigger_thread()
{
	if(iFlagStopThreadRoyaTestCmd == TRUE)
	{
		iFlagStopThreadRoyaTestCmd = FALSE;
		//
		m_pWinThreadRoyaTestCmd = AfxBeginThread(dlg_roya_test_start_thread, 0);  // mtn_dll_home_axis_thread
		SetPriorityClass(m_pWinThreadRoyaTestCmd->m_hThread, REALTIME_PRIORITY_CLASS);

		m_pWinThreadRoyaTestCmd->m_bAutoDelete = FALSE;
	}
}

// #include "MotAlgo_DLL.h"
#define _ROYA_E_TEST_DAC_  800
#define _ROYA_E_TEST_DAC_VOLT  (9.0)

#include "MtnDialog_FbMonitor.h"
//extern MtnDialog_FbMonitor *cpDlgMonitorFeedback;
MtnDialog_FbMonitor *cpRefFromBsdDlgMonitorFeedback;
#include "process.h"

extern void mtn_acs_set_clear_out_io(unsigned int uiStatus,  int nBit);

// IDC_ROYA_BSD_BUTTON_E_TEST_WIRING
void CDlgTestRoyaBoard::OnBnClickedRoyaBsdButtonETestWiring()
{
	GetDlgItem(IDC_ROYA_BSD_BUTTON_E_TEST_WIRING)->EnableWindow(FALSE);
	// Check Version
	OnBnClickedButtonRoyaGetVersion();
	Sleep(100);
	// Test ADC-DAC

	double dOutDacVolt, dInAdcVolt;
	dOutDacVolt = _ROYA_E_TEST_DAC_VOLT;  // __CONVERT_DAC_TO_VOLT(_ROYA_E_TEST_DAC_);
	int iSetDacValue;
	if(roya_6412_get_flag_dac_bit() == ROYA_BSD_DAC_10b)
	{
		iSetDacValue = (int)__CONVERT_DAC_FROM_VOLT_TO_10b_(dOutDacVolt);
	}
	else
	{
		iSetDacValue = (int)__CONVERT_DAC_FROM_VOLT_TO_12b_(dOutDacVolt);
	}
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_ROYA_SET_DAC_VOLT))->SetPos(iSetDacValue);  // 20110822, _ROYA_E_TEST_DAC_
	UpdateDacVoltageValueBySlider();
	OnBnClickedButtonRoyaSetDac();
	Sleep(100);

	short sRet = roya_6412_comm_test_get_adc(uiAdcIdx, &iCurrAdcValue);
	RoyaDlgCheckAndShowDebugMessage(sRet);

	CString cstrAutoTestResult;

	if(roya_6412_get_flag_adc_polarity() == ROYA_BSD_ADC_BI_POLAR)   // 20110822
	{
		dInAdcVolt = __CONVERT_16b_BI_ADC_TO_VOLT_FlOAT(iCurrAdcValue);
	}
	else  // ROYA_BSD_ADC_POLARITY_SINGLE
	{
		dInAdcVolt = __CONVERT_ADC_TO_VOLT_FlOAT(iCurrAdcValue);
	}   // 20110822

#define __BSD_APPLY_VOLTAGE_FB_12VDC__    12.5
	////// According to the language, 20110916
	int iLanguageOption = get_sys_language_option();
	if(iLanguageOption == LANGUAGE_UI_EN)
	{
		cstrAutoTestResult.Format("OutputVolt: %4.1f(v), Feedback Volt: %4.1f(v) [%4.1f, %4.1f]  -- ", 
			dOutDacVolt, dInAdcVolt, dOutDacVolt - 3, __BSD_APPLY_VOLTAGE_FB_12VDC__);
		if(dInAdcVolt < dOutDacVolt - 3 || dInAdcVolt > __BSD_APPLY_VOLTAGE_FB_12VDC__)  // 20110929
		{
			cstrAutoTestResult.AppendFormat("Failure, Check (1) cable to BE00010; \n\r (2) Relay on BE00010; (3) Replace Roya-DSP\n\r");
		}
		else
		{
			cstrAutoTestResult.AppendFormat("PASS\n\r");
		}
	}
	else
	{
		cstrAutoTestResult.Format("检测电压: %4.1f(v), 反馈电压: %4.1f(v) [%4.1f, %4.1f]  -- ", dOutDacVolt, dInAdcVolt,
			dOutDacVolt - 3, __BSD_APPLY_VOLTAGE_FB_12VDC__);
		if(dInAdcVolt < 0.5)  // volt
		{
			cstrAutoTestResult.AppendFormat("质检失败, 检查列表 \n\r (1) 电线连接 to BE00010; \n\r");
			cstrAutoTestResult.AppendFormat("(2) 继电器和IC-272 \n\r (3) 断开连接, 用万用表量测BE00010(2510端子-4脚-NSD_Roya), DAC脚, 如果 DAC持续为零， 更换 Roya-DSP\n\r");
			cstrAutoTestResult.AppendFormat("(4) 量测Q3，Q4后面，电阻（R14_JP，R15_JP）上电压，两个电压必须一高一低\n\r");
		}
		else if(dInAdcVolt < dOutDacVolt - 3 || dInAdcVolt > __BSD_APPLY_VOLTAGE_FB_12VDC__)  // 20110929  // dOutDacVolt + 1.5
		{
			cstrAutoTestResult.AppendFormat("质检失败, 检查列表 \n\r (1) 电线连接 to BE00010; \n\r (2) 电阻 1K/1K on BE00010; \n\r (3) 驱动继电器的功率三极管 \n\r");
		}
		else
		{
			cstrAutoTestResult.AppendFormat("PASS\n\r");
		}
	}

//		sprintf_s(tempChar, 256, "%d", iCurrAdcValue);
	sprintf_s(tempChar, 256, "%6.1f", (double)__CONVERT_ADC_TO_VOLT_FlOAT(iCurrAdcValue));
	GetDlgItem(IDC_STATIC_SHOW_ROYA_ADC_VOLT_VALUE)->SetWindowTextA(_T(tempChar));

	Sleep(100);
	// Test BSD-Trig
	unsigned int uiInitCount, uiCounterAfterTrigger; //  = 
	BSD_STATUS stAutoCheckStatusBSD;
	//ReadUShortFromEdit(IDC_ROYA_BSD_STATUS_TRIGGER_CNT, &usInitCount); // ->GetWindowTextA
	bsd_api_get_status(&stAutoCheckStatusBSD); uiInitCount = stAutoCheckStatusBSD.uiTrigCnt; // uiCounterAfterTrigger
	OnBnClickedRoyaNsdButtonStartWBlk();
	Sleep(500);

	int iFlagMachType = get_sys_machine_type_flag();
	if(iFlagMachType == WB_STATION_EFO_BSD)  // machine type dependency Item-7
	{
		if(cpRefFromBsdDlgMonitorFeedback != NULL)
		{
			cpRefFromBsdDlgMonitorFeedback->OnBnClickedButtonDigitalIoOutputBit7();
			Sleep(500);
			cpRefFromBsdDlgMonitorFeedback->OnBnClickedButtonDigitalIoOutputBit7();
			Sleep(500);
			cpRefFromBsdDlgMonitorFeedback->OnBnClickedButtonDigitalIoOutputBit7();
			Sleep(500);
			cpRefFromBsdDlgMonitorFeedback->OnBnClickedButtonDigitalIoOutputBit7();
			Sleep(500);
		}
	}
	else
	{
			//cpRefFromBsdDlgMonitorFeedback->OnBnClickedCheckAcsOut2();
		mtn_acs_set_clear_out_io(0,  2);
		Sleep(500);
		mtn_acs_set_clear_out_io(1,  2);
		Sleep(500);
		mtn_acs_set_clear_out_io(0,  2);
		Sleep(500);
		mtn_acs_set_clear_out_io(1,  2);
		Sleep(500);
	}

	OnBnClickedRoyaNsdButtonStartWBlk();
	Sleep(500);
	// ReadUShortFromEdit(IDC_ROYA_BSD_STATUS_TRIGGER_CNT, &usCounterAfterTrigger); // ->GetWindowTextA
	bsd_api_get_status(&stAutoCheckStatusBSD); uiCounterAfterTrigger = stAutoCheckStatusBSD.uiTrigCnt; // 

	cstrAutoTestResult.AppendFormat("%d:%d,  ", uiInitCount, stAutoCheckStatusBSD);
	if(iLanguageOption == LANGUAGE_UI_EN)
	{
		if(uiCounterAfterTrigger >= (uiInitCount+1))
		{
			cstrAutoTestResult.AppendFormat("TriggerBSD: OK\n\r  \n\r下一步测试继电器，请保证环境安静，准备好点击确定");
		}
		else
		{
			cstrAutoTestResult.AppendFormat("TriggerBSD: Failure\n\r Checklist: (1) BE0000(2), \n\r(2)Cable to BE00011,\n\r(3) RoyaDSP\n\r  \n\r下一步测试继电器，请保证环境安静，准备好点击确定");
		}
	}
	else  // LANGUAGE_UI_CN
	{
		if(uiCounterAfterTrigger >= (uiInitCount+1))
		{
			cstrAutoTestResult.AppendFormat("触发检测: 正常\n\r \n\r下一步测试继电器，请保证环境安静，准备好点击确定");
		}
		else
		{
			cstrAutoTestResult.AppendFormat("触发检测有误: \n\r 检查列表: (1) BE0006(2)-J31接法, \n\r(2)电缆线到 BE00011,\n\r (3) 信号板 \n\r \n\r  下一步测试继电器，请保证环境安静，准备好点击确定");
		}

	}
	AfxMessageBox(cstrAutoTestResult);

//	iFlagMachType = get_sys_machine_type_flag();
	if(iFlagMachType == WB_STATION_EFO_BSD)
	{
		if(cpRefFromBsdDlgMonitorFeedback != NULL)
		{
			Sleep(500);

			if(iFlagMachType == WB_STATION_EFO_BSD)    // machine type dependency Item-13
			{
				cpRefFromBsdDlgMonitorFeedback->OnBnClickedButtonDigitalIoOutputBit4();
				Sleep(1000);
				cpRefFromBsdDlgMonitorFeedback->OnBnClickedButtonDigitalIoOutputBit4();
				Sleep(1000);
			}
			else
			{
				mtn_acs_set_clear_out_io(1,  1); // cpRefFromBsdDlgMonitorFeedback->OnBnClickedCheckAcsOut1();
				Sleep(1000);
				mtn_acs_set_clear_out_io(0,  1); // cpRefFromBsdDlgMonitorFeedback->OnBnClickedCheckAcsOut1();
				Sleep(1000);
			}
		}
		AfxMessageBox(_T("如果没有听到继电器声音(2声，间隔约1秒)，请依次更换BE00010上的IC。\n\r如果声音太小（或持续不断），请检查继电器电压，（三极管输出侧电阻后）"));
	}
	// Update time
	OnBnClickedButtonSetRoyaRtClock();
		//// 20120413
#define IDX_ADC_CHECK_WIRE_END_CONNECT    1
		double dAdcWireRod_Volt;
		//// 20120413

	if(iFlagMachType == WB_MACH_TYPE_HORI_LED || iFlagMachType == WB_MACH_TYPE_ONE_TRACK_13V_LED)
	{
		mtn_acs_set_clear_out_io(1,  1);
		Sleep(1000);

		sRet = roya_6412_comm_test_get_adc(uiAdcIdx, &iCurrAdcValue);
		RoyaDlgCheckAndShowDebugMessage(sRet);

		if(roya_6412_get_flag_adc_polarity() == ROYA_BSD_ADC_BI_POLAR)   // 20110822
		{
			dInAdcVolt = __CONVERT_16b_BI_ADC_TO_VOLT_FlOAT(iCurrAdcValue);
		}
		else  // ROYA_BSD_ADC_POLARITY_SINGLE
		{
			dInAdcVolt = __CONVERT_ADC_TO_VOLT_FlOAT(iCurrAdcValue);
		}   // 20110822

		bsd_api_get_adc_volt(IDX_ADC_CHECK_WIRE_END_CONNECT, &dAdcWireRod_Volt);		//// 20120413

double dVoltageBSD_Mode_LowBound = dOutDacVolt - 4;
		if(iLanguageOption == LANGUAGE_UI_EN)
		{
			cstrAutoTestResult.Format("BSD Mode FeedbackVoltage：%3.1f V, [%4.1f, %4.1f]", dInAdcVolt
				, dVoltageBSD_Mode_LowBound, __BSD_APPLY_VOLTAGE_FB_12VDC__);
		}
		else
		{
			cstrAutoTestResult.Format("焊线检测模式反馈电压：%3.1f V, [%4.1f, %4.1f]", dInAdcVolt
				, dVoltageBSD_Mode_LowBound, __BSD_APPLY_VOLTAGE_FB_12VDC__);
		}

		if(iLanguageOption == LANGUAGE_UI_EN)
		{
			if(dInAdcVolt < dVoltageBSD_Mode_LowBound)
			{
				cstrAutoTestResult.AppendFormat("\r\n Please check Wire Clamp Isolation");
			}
			else
			{
				cstrAutoTestResult.AppendFormat("\r\n PASS");
			}
		}
		else
		{
			if(dInAdcVolt < dVoltageBSD_Mode_LowBound)
			{
				cstrAutoTestResult.AppendFormat("\r\n 请检查线夹绝缘");
			}
			else
			{
				cstrAutoTestResult.AppendFormat("\r\n PASS");
			}
		}
		AfxMessageBox(cstrAutoTestResult);
		mtn_acs_set_clear_out_io(0,  1);
	}
	// Test EFO-Fb-Counter

	// Communication Test // 20111016
	CString cstrTemp;
	if(iLanguageOption == LANGUAGE_UI_EN)
	{
		cstrTemp.Format("Yes: Continue to test WireEnd Connection \r\n No: NOT to test wire end connection, continue to next ");
	}
	else
	{
		cstrTemp.Format("Yes: 测试线尾连接 \r\n No:不测试");
	}
	if(AfxMessageBox(cstrTemp, MB_YESNO) == IDYES)
	{
		if(dAdcWireRod_Volt > 5.0)
		{
			if(iLanguageOption == LANGUAGE_UI_EN)
			{
				cstrAutoTestResult.Format(" %3.1f V, Connection OK", dAdcWireRod_Volt);
			}
			else
			{
				cstrAutoTestResult.Format(" %3.1f V, 线尾接触OK", dAdcWireRod_Volt);
			}
		}
		else
		{
			if(iLanguageOption == LANGUAGE_UI_EN)
			{
				cstrAutoTestResult.Format(" %3.1f V, Please check WireEnd connection, air-tension flow meter, or other connection", dAdcWireRod_Volt);
			}
			else
			{
				cstrAutoTestResult.Format(" %3.1f V, 请确保线尾接触，并检查气流", dAdcWireRod_Volt);
			}
		}
		AfxMessageBox(cstrAutoTestResult);
	}

	// Communication Test // 20111016
	if(iLanguageOption == LANGUAGE_UI_EN)
	{
		cstrTemp.Format("Yes: Begin Communication Test \r\n PurchaseIncomingQC: 15min，AssemblyTest 30min。\r\n Normal: Reply from 192.168.2.5: bytes=32 time < 1ms TTL=128  \r\n ERROR: Hardware Error, (Please check connection，HostPC IP-Address:192.168.2.101) \r\n No: Exit");
	}
	else
	{
		cstrTemp.Format("Yes: 开始通讯测试\r\n 电路板入库测试15分钟，组装检测盒测试30分钟。\r\n 正常为Reply from 192.168.2.5: bytes=32 time < 1ms TTL=128  \r\n 一旦出现Hardware Error或其他, 即为信号板死机（保证接线没有问题，上位机IP地址没有设错），请填返修单。\r\n No:退出");
	}
	if(AfxMessageBox(cstrTemp, MB_YESNO) == IDYES)
	{
#define __INIT_BSD_PARA_SETTING__
#ifdef  __INIT_BSD_PARA_SETTING__
		int ii;
		for(ii = 0; ii<3; ii++)
		{
			roya_6412_bsd_get_parameters(&stParameterBlkBSD, ii);
			if(ii == 0 || ii == 2)
			{
				stParameterBlkBSD.usDetectSampleLength = 20; //  60?
			}
			else
			{
				stParameterBlkBSD.usDetectSampleLength = 60;  //  100?
			}
			roya_6412_bsd_set_parameters(&stParameterBlkBSD, ii);
			Sleep(100);
		}
#endif  // __INIT_BSD_PARA_SETTING__

		GetDlgItem(IDC_EDIT_ROYA_TEST_DLG_RPT)->ShowWindow(TRUE);

		iAutoCheckBSD = 1;
		((CButton *)GetDlgItem(IDC_ROYA_BSD_CHECK_AUTO))->SetCheck(iAutoCheckBSD);

		iAutoCheckADC = 0; // Donot check ADC when testing communication
		((CButton *)GetDlgItem(IDC_ROYA_ADDA_CHECK_AUTO))->SetCheck(iAutoCheckADC);

//		iFlagEnableDebugRoyaBoard = 1;

		cFlagBSDAnalyzeByFirmware = 1;
		((CButton*)GetDlgItem(IDC_CHECK_CHECK_ROYA_COMMUNICATION_BY_BSD))->SetCheck((int)cFlagBSDAnalyzeByFirmware);

		iFlagDryRunBSD = 1;
		((CButton*)GetDlgItem(IDC_ROYA_BSD_FLAG_CHECK_DRY_RUN))->SetCheck(iFlagDryRunBSD);

		iFlagCheckEnableTimerAll = 1;
		((CButton*)GetDlgItem(IDC_ROYA_TEST_CHECK_ENABLE_TIMER_ALL))->SetCheck(iFlagCheckEnableTimerAll);

		//static char strSystemCommand[256];
		//sprintf_s(strSystemCommand, 256, "ping 192.168.2.5 -t");
		//system(strSystemCommand);
//		
		dlg_roya_test_trigger_thread();
	}

	Sleep(1000);
	// Enable the button
	GetDlgItem(IDC_ROYA_BSD_BUTTON_E_TEST_WIRING)->EnableWindow(TRUE);

}

// IDC_CHECK_ROYA_BSD_DEBUG
void CDlgTestRoyaBoard::OnBnClickedCheckRoyaBsdDebug()
{
	int iTemp;
	static int iFlagFailurePass = FALSE;
	iTemp = ((CButton *)GetDlgItem(IDC_CHECK_ROYA_BSD_DEBUG))->GetCheck();
	if(iTemp == TRUE)
	{
		static CDlgKeyInputPad cDlgInputPad;
		cDlgInputPad.SetFlagShowNumberOnKeyPad(0);
		cDlgInputPad.SetInputNumber(0);
		if(cDlgInputPad.DoModal() == IDOK)
		{
			if(cDlgInputPad.GetReturnNumber()== wb_mtn_tester_get_password_brightlux_6d_zzy()) // wb_mtn_tester_get_password_sg_8d_zzy()
			{
				iFlagFailurePass = FALSE;
			}
			else
			{
				iFlagFailurePass = TRUE;
			}
		}
		else
		{
			iFlagFailurePass = TRUE;
		}
		if(iFlagFailurePass == TRUE)
		{
			iTemp = FALSE;
			((CButton *)GetDlgItem(IDC_CHECK_ROYA_BSD_DEBUG))->SetCheck(FALSE);
		}
	}
	else
	{
		UI_EnableGroupUSG_Items(iTemp);
	}
	iFlagEnableDebugRoyaBoard = iTemp;
	UI_EnableGroupUSG_Items(iFlagEnableDebugRoyaBoard);
}

void CDlgTestRoyaBoard::UI_EnableGroupUSG_Items(int iEnableFlag)
{

	//_spawnl(_P_NOWAIT, "ping", "192.168.1.1", "-t"); // NULL , P_NOWAITO, _P_DETACH

	//execl("ping", "192.168.1.1 -t");


	GetDlgItem(IDC_ROYA_COMBO_TIMER_ID                 )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_BOARD_TIMER_PERIOD             )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_BOARD_SET_TIMER_PERIOD         )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_BUTTON_DOWNLOAD_FIRMWARE       )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_CHECK_DOWNLOAD_VERIFY          )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_BUTTON_LOAD_FIRMWARE_BIN       )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_TEXT_FIRMWARE_SIZE             )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_DOWNLOAD_PROGRESS              )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_STATIC_LABEL_ROYA_USG_PROF_SETTING  )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_STATIC_USG_PARA_BLK_IDX             )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_USG_COMBO_BLK_NO               )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_STATIC_USG_PARA_SEG_IDX             )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_USG_EDIT_SEG_NO                )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_BUTTON_SET_USG_SEG             )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_START_ROYA_USG_PROF_FSM             )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_STATIC_ROYA_USG_PROF_DURATION       )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_USG_EDIT_DURATION              )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_STATIC_ROYA_USG_PROF_NEXT_BLK_IDX   )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_USG_EDIT_NEXT_BLK_IDX          )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_STATIC_ROYA_USG_PROF_RAMP_TIME      )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_USG_EDIT_RAMP_TIME             )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_STATIC_ROYA_USG_PROF_TRIG_ADDRESS   )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_USG_EDIT_TRIG_ADDR             )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_BUTTON_WARM_RESET_ROYA_USG          )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_STATIC_ROYA_USG_PROF_TRIG_PATTERN   )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_USG_EDIT_TRIG_PATTERN          )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_STATIC_ROYA_USG_PROF_MAX_CNT_PROTECT)->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_USG_EDIT_MAX_COUNT_TRIG_PROT   )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_STATIC_ROYA_USG_PROF_AMPLITUDE      )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_USG_EDIT_AMP                   )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_USG_PROF_STATUS                )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_STATIC_ROYA_USG_PROF_CMD_TYPE       )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_USG_EDIT_CMD_TYPE              )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_ROYA_USG_CHECK_AUTO                 )->ShowWindow(iEnableFlag);

	GetDlgItem(IDC_EDIT_TRIG_PATTERN_ROYA_USG_BY_ACS   )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_STATIC_ROYA_USG_PROF_TRIG_PATTERN_BY_ACS   )->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_BUTTON_TRIG_ROYA_USG_VIA_ACS   )->ShowWindow(iEnableFlag);

	GetDlgItem(IDC_EDIT_ROYA_TEST_DLG_RPT)->ShowWindow( 1 - iEnableFlag);

}

// IDC_ROYA_EDIT_TEST_COMM
// IDC_ROYA_STATIC_TEST_COMM_RCV
// IDC_ROYA_TEST_BUTTON_SEND_COMMTEST
extern char strSendBuffer[];
extern char strReceiveBuffer[];

void CDlgTestRoyaBoard::OnBnClickedRoyaTestButtonSendCommtest()
{
	CString strSended;
	GetDlgItem(IDC_ROYA_EDIT_TEST_COMM)->GetWindowText(strSended);
	sprintf_s(tempChar, 256, "%s", strSended);
	short sRet = roya_6412_test_communication(tempChar, strSended.GetLength());
	if(sRet == MTN_API_OK_ZERO)
	{
		GetDlgItem(IDC_ROYA_STATIC_TEST_COMM_RCV)->SetWindowTextA(strSended);
	}
	else
	{
		RoyaDlgCheckAndShowDebugMessage(sRet);
	}

	dlg_roya_test_trigger_thread();
}

BOOL CDlgTestRoyaBoard::StopTimer ()
{
	if (!KillTimer (iTimerIdRoyaTestDlg))
	{
		return FALSE;
	}
	return TRUE;
} // end StopTimer

UINT CDlgTestRoyaBoard::StartTimer (UINT TimerDuration)
{
	iTimerIdRoyaTestDlg = SetTimer(IDT_TIMER_ROYA_TEST_DLG, TimerDuration, 0);
	
	if (iTimerIdRoyaTestDlg == 0)
	{
		AfxMessageBox("Unable to obtain timer");
	}

    return (UINT)iTimerIdRoyaTestDlg;
}// end StartTimer

void CDlgTestRoyaBoard::OnStnClickedStaticRoyaUsgProfDuration()
{
	// TODO: Add your control notification handler code here
}


static unsigned int anCycleForSineOut[] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155, 160, 165, 170, 175, 180, 185, 190, 195, 200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 300, 320, 340, 360, 380, 400, 420, 440, 460, 480, 500, 520, 540, 560, 580, 600, 620, 640, 660, 680, 700, 720, 740, 760, 780, 800, 820, 840, 860, 880, 900, 920, 940, 960, 980, 1000, 1020, 1040, 1060, 1080, 1100, 1120, 1140, 1160, 1180, 1200, 1220, 1240, 1260, 1280, 1300, 1320, 1340, 1360, 1380, 1400, 1420, 1440, 1460, 1480, 1500, 1520, 1540, 1560, 1580, 1600, 1620, 1640, 1660, 1680, 1700, 1720, 1740, 1760, 1780, 1800, 1820, 1840, 1860, 1880, 1900, 1920, 1940, 1960, 1980, 2000, 2020, 2040, 2060, 2080, 2100, 2120, 2140, 2160, 2180, 2200, 2220, 2240, 2260, 2280, 2300, 2320, 2340, 2360, 2380, 2400, 2420, 2440, 2460, 2480, 2500, 2520, 2540, 2560, 2580, 2600, 2620, 2640, 2660, 2680, 2700, 2720, 2740, 2760, 2780, 2800, 2820, 2840, 2860, 2880, 2900, 2920, 2940, 2960, 2980, 3000, 3020, 3040, 3060, 3080, 3100, 3120, 3140, 3160, 3180, 3200, 3220, 3240, 3260, 3280, 3300, 3320, 3340, 3360, 3380, 3400, 3420, 3440, 3460, 3480, 3500, 3520, 3540, 3560, 3580, 3600, 3620, 3640, 3660, 3680, 3700, 3720, 3740, 3760, 3780, 3800, 3820, 3840, 3860, 3880, 3900, 3920, 3940, 3960, 3980, 4000, 4020, 4040, 4060, 4080, 4100, 4120, 4140, 4160, 4180, 4200, 4220, 4240, 4260, 4280, 4300, 4320, 4340, 4360, 4380, 4400, 4420, 4440, 4460, 4480, 4500, 4520, 4540, 4560, 4580, 4600, 4620, 4640, 4660, 4680, 4700, 4720, 4740, 4760, 4780, 4800, 4820, 4840, 4860, 4880, 4900, 4920, 4940, 4960, 4980, 5000};
static unsigned int uiSignalGenAmpSineOut = 128;
static unsigned int nCycleNumISR = 10;
static double dSignalGenSineOutAmp_V;
static unsigned int nAmplitudeDacHalf;
static SIG_GEN_SINE_OUT_CFG stSigGenSineOutCfg;

void CDlgTestRoyaBoard::UI_InitSignalGenerator()
{
	int nLen = sizeof(anCycleForSineOut)/sizeof(int);

	roya_6412_siggen_get_curr_amplitude(&uiSignalGenAmpSineOut);
	roya_6412_siggen_get_curr_cycle(&nCycleNumISR);

	roya_6412_siggen_get_curr_sine_gen_cfg(&stSigGenSineOutCfg);

	if( roya_6412_get_flag_dac_bit() == ROYA_BSD_DAC_10b) // 20110822
	{
		nAmplitudeDacHalf = DAC_SIGNED_MAX_10BIT;
	}
	else if(roya_6412_get_flag_dac_bit() == ROYA_BSD_DAC_12b)  // 20110822
	{
		nAmplitudeDacHalf = DAC_SIGNED_MAX_12BIT;
	}

	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_ROYA_SIG_GEN_SINE_OUT_AMP_DAC))->SetRangeMax(nAmplitudeDacHalf);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_ROYA_SIG_GEN_SINE_OUT_AMP_DAC))->SetRange(0, nAmplitudeDacHalf);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_ROYA_SIG_GEN_SINE_OUT_AMP_DAC))->SetPos(uiSignalGenAmpSineOut);

	dSignalGenSineOutAmp_V = (double)uiSignalGenAmpSineOut/ nAmplitudeDacHalf * 12;

	UI_UpdateSignalGen_SineOutCfg();

	CString cstrTemp;
	cstrTemp.Format("%d", nCycleNumISR);
	GetDlgItem(IDC_EDIT_ROYA_SIG_SINE_OUT_GEN_CYCLE)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("%d", stSigGenSineOutCfg.nRefSignalCycle);
	GetDlgItem(IDC_EDIT_ROYA_SIG_SINE_OUT_GEN_FRAC_CYCLE)->SetWindowTextA(cstrTemp);
}
// IDC_STATIC_ROYA_SIG_GEN_GROUP
// IDC_STATIC_ROYA_SIG_SINE_OUT_GEN_CYCLE
// IDC_COMBO_ROYA_SIG_GEN_SINE_OUT_FREQ
// IDC_SLIDER_ROYA_SIG_GEN_SINE_OUT_AMP_DAC
// IDC_EDIT_ROYA_SIG_SINE_OUT_GEN_CYCLE
// IDC_EDIT_ROYA_SIG_SINE_OUT_GEN_FRAC_CYCLE
void CDlgTestRoyaBoard::UpdateSignalGenSineOutampVoltValueBySlider()
{
	uiSignalGenAmpSineOut = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_ROYA_SIG_GEN_SINE_OUT_AMP_DAC))->GetPos();
	roya_6412_siggen_set_new_amplitude(uiSignalGenAmpSineOut);

	dSignalGenSineOutAmp_V = (double)uiSignalGenAmpSineOut/ nAmplitudeDacHalf * 12;
	UI_UpdateSignalGen_SineOutCfg();

}
void CDlgTestRoyaBoard::OnNMReleasedcaptureSliderRoyaSigGenSineOutAmpDac(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateSignalGenSineOutampVoltValueBySlider();
	*pResult = 0;
}

// IDC_EDIT_ROYA_SIG_SINE_OUT_GEN_CYCLE
void CDlgTestRoyaBoard::OnEnKillfocusEditRoyaSigSineOutGenCycle()
{
	unsigned short usCycle;
	ReadUShortFromEdit(IDC_EDIT_ROYA_SIG_SINE_OUT_GEN_CYCLE, &usCycle);
	if(usCycle > stSigGenSineOutCfg.nRefSignalCycle)
	{
		nCycleNumISR = usCycle;
		stSigGenSineOutCfg.nCycleISR = nCycleNumISR;		
		roya_6412_siggen_set_new_cycle(nCycleNumISR);
	}
	UI_UpdateSignalGen_SineOutCfg();
}
// IDC_EDIT_ROYA_SIG_SINE_OUT_GEN_FRAC_CYCLE
void CDlgTestRoyaBoard::OnEnKillfocusEditRoyaSigSineOutGenFracCycle()
{
	unsigned short usRefCycle;
	ReadUShortFromEdit(IDC_EDIT_ROYA_SIG_SINE_OUT_GEN_FRAC_CYCLE, &usRefCycle);
	if(usRefCycle < nCycleNumISR)
	{
		stSigGenSineOutCfg.nRefSignalCycle = usRefCycle;
		roya_6412_siggen_set_new_frac_cycle(stSigGenSineOutCfg.nCycleISR, usRefCycle);
	}
	UI_UpdateSignalGen_SineOutCfg();
}

//
void CDlgTestRoyaBoard::UI_UpdateSignalGen_SineOutCfg()
{
	CString cstrTemp;
	cstrTemp.Format("Sin:C-%d Amp-%3.1fV F: %5.1fKHz", nCycleNumISR, dSignalGenSineOutAmp_V, 
		stSigGenSineOutCfg.dFreqISR_Hz / nCycleNumISR * stSigGenSineOutCfg.nRefSignalCycle/1000.0);
	GetDlgItem(IDC_STATIC_ROYA_SIG_SINE_OUT_GEN_CYCLE)->SetWindowTextA(cstrTemp);

}

void CDlgTestRoyaBoard::UI_ShowWindow_SignalGen_RoyaDlg(int iFlag)
{
	GetDlgItem(IDC_STATIC_ROYA_SIG_GEN_GROUP)->ShowWindow(iFlag);
	GetDlgItem(IDC_STATIC_ROYA_SIG_SINE_OUT_GEN_CYCLE)->ShowWindow(iFlag);
	GetDlgItem(IDC_SLIDER_ROYA_SIG_GEN_SINE_OUT_AMP_DAC)->ShowWindow(iFlag);
	GetDlgItem(IDC_EDIT_ROYA_SIG_SINE_OUT_GEN_CYCLE)->ShowWindow(iFlag);
	GetDlgItem(IDC_EDIT_ROYA_SIG_SINE_OUT_GEN_FRAC_CYCLE)->ShowWindow(iFlag);

}

//// IDC_STATIC_ROYA_BQM_PRE_COUNTER
//// IDC_STATIC_ROYA_BQM_MAX_COUNTER_ADC

void CDlgTestRoyaBoard::UI_ShowWindow_QBM_RoyaDlg(int iFlag)
{
	GetDlgItem(IDC_STATIC_ROYA_BQM_PRE_COUNTER)->ShowWindow(iFlag);
	GetDlgItem(IDC_STATIC_ROYA_BQM_MAX_COUNTER_ADC)->ShowWindow(iFlag);
	GetDlgItem(IDC_EDIT_ROYA_BQM_PRE_COUNTER_TH_1)->ShowWindow(iFlag);
	GetDlgItem(IDC_EDIT_ROYA_BQM_PRE_COUNTER_TH_2)->ShowWindow(iFlag);
	GetDlgItem(IDC_EDIT_ROYA_BQM_MAX_COUNTER_ADC_1)->ShowWindow(iFlag);
	GetDlgItem(IDC_EDIT_ROYA_BQM_MAX_COUNTER_ADC_2)->ShowWindow(iFlag);
}

static BQM_STATUS  stStatusBQM;

void CDlgTestRoyaBoard::UI_UpdateBQM_RoyaDlg()
{
	GetDlgItem(IDC_STATIC_ROYA_BQM_PRE_COUNTER)->SetWindowTextA(_T("PreCntrTH"));
	GetDlgItem(IDC_STATIC_ROYA_BQM_MAX_COUNTER_ADC)->SetWindowTextA(_T("MaxCntrAdc"));
	roya_6412_bqm_get_curr_status(&stStatusBQM);

	CString cstrTemp;
	cstrTemp.Format("%d", stStatusBQM.iPreCountBQM_TH[0]);
	GetDlgItem(IDC_EDIT_ROYA_BQM_PRE_COUNTER_TH_1)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("%d", stStatusBQM.iPreCountBQM_TH[1]);
	GetDlgItem(IDC_EDIT_ROYA_BQM_PRE_COUNTER_TH_2)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("%d", stStatusBQM.iMaxCountBQM_Adc[0]);
	GetDlgItem(IDC_EDIT_ROYA_BQM_MAX_COUNTER_ADC_1)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("%d", stStatusBQM.iMaxCountBQM_Adc[1]);
	GetDlgItem(IDC_EDIT_ROYA_BQM_MAX_COUNTER_ADC_2)->SetWindowTextA(cstrTemp);
}

// IDC_EDIT_ROYA_BQM_PRE_COUNTER_TH_1
void CDlgTestRoyaBoard::OnEnKillfocusEditRoyaBqmPreCounterTh1()
{
	unsigned short usPreCnter;
	ReadUShortFromEdit(IDC_EDIT_ROYA_BQM_PRE_COUNTER_TH_1, &usPreCnter);
	stStatusBQM.iPreCountBQM_TH[0] = usPreCnter;
	roya_6412_bqm_set_pre_counter_th(stStatusBQM.iPreCountBQM_TH);
}
// IDC_EDIT_ROYA_BQM_PRE_COUNTER_TH_2
void CDlgTestRoyaBoard::OnEnKillfocusEditRoyaBqmPreCounterTh2()
{
	unsigned short usPreCnter;
	ReadUShortFromEdit(IDC_EDIT_ROYA_BQM_PRE_COUNTER_TH_2, &usPreCnter);
	stStatusBQM.iPreCountBQM_TH[1] = usPreCnter;
	roya_6412_bqm_set_pre_counter_th(stStatusBQM.iPreCountBQM_TH);
}
// IDC_EDIT_ROYA_BQM_MAX_COUNTER_ADC_1
void CDlgTestRoyaBoard::OnEnKillfocusEditRoyaBqmMaxCounterAdc1()
{
	unsigned short usMaxCnter;
	ReadUShortFromEdit(IDC_EDIT_ROYA_BQM_MAX_COUNTER_ADC_1, &usMaxCnter);
	stStatusBQM.iMaxCountBQM_Adc[0] = usMaxCnter;
	roya_6412_bqm_set_max_counter_adc(stStatusBQM.iMaxCountBQM_Adc);
}
// IDC_EDIT_ROYA_BQM_MAX_COUNTER_ADC_2
void CDlgTestRoyaBoard::OnEnKillfocusEditRoyaBqmMaxCounterAdc2()
{
	unsigned short usMaxCnter;
	ReadUShortFromEdit(IDC_EDIT_ROYA_BQM_MAX_COUNTER_ADC_2, &usMaxCnter);
	stStatusBQM.iMaxCountBQM_Adc[1] = usMaxCnter;
	roya_6412_bqm_set_max_counter_adc(stStatusBQM.iMaxCountBQM_Adc);
}

void CDlgTestRoyaBoard::OnCbnSelchangeRoyaComboDacMaxPercent()
{
	cIdxMaxPercentOutDAC = ((CComboBox *)GetDlgItem(IDC_ROYA_COMBO_DAC_MAX_PERCENT))->GetCurSel();
}

// IDC_EDIT_ROYA_STEP_CMD_DAC2_V1
void CDlgTestRoyaBoard::OnEnKillfocusEditRoyaStepCmdDac2V1()
{
}
// IDC_EDIT_ROYA_STEP_CMD_DAC2_V2
void CDlgTestRoyaBoard::OnEnKillfocusEditRoyaStepCmdDac2V2()
{
}
// IDC_EDIT_ROYA_STEP_CMD_DAC2_PROT_V
void CDlgTestRoyaBoard::OnEnKillfocusEditRoyaStepCmdDac2ProtV()
{
}
// IDC_COMBO_ROYA_STEP_CMD_DAC2_T
void CDlgTestRoyaBoard::OnCbnSelchangeComboRoyaStepCmdDac2T()
{
}
// IDC_CHECK_ROYA_STEP_CMD_DAC2_RUN
void CDlgTestRoyaBoard::OnBnClickedCheckRoyaStepCmdDac2Run()
{
}
