// CommunicationDialog.cpp : implementation file
//
// Port from ACS CPP by VC60, to DotNet
// History
// YYYYMMDD Author  Notes
// 20080820 Zhengyi 
// 20090219  Zhengyi replace ServoController.h to AcsServo.h, follow ZhangDong's new release                  
// 20091110			 Add lighting control via SerPort
// 20100729  
#include "stdafx.h"
#include "MtnInitAcs.h"  // For communication-type setup, 20100729
#include "MtnTesterResDef.h"
//#include "ACSCTerm.h"
#include "CommunicationDialog.h"
#include "AcsServo.h" //"ServoController.h" // 20081009
#include "MtnTesterEntry.h"

static 	BOOL	m_fConnect;

// #include "MACDEF.h"

// 20121203
//extern char sys_acs_communication_get_flag_sc_udi();
#include "MotAlgo_DLL.h"

//extern char cFlag_ConnectACS_SC_UDI;

#define MAX_NUM_COMM_PORT_RATE      8
char *pstrCommPortRate[] = { 
"300",
"1200",
"2400",
"9600",
"19200",
"38400",
"57600",
"115200"};
// Default Comm Port rate 9600
#define DEF_COMM_PORT_RATE_IDX  		3
#define DEF_COMM_PORT_RATE_IDX_LIGHT    3
#define DEF_COMM_PORT_RATE_IDX_ACS		7
#define DEF_COMM_PORT_RATE_IDX_ELMO	    4
#define DEF_COMM_PORT_RATE_IDX_XENUS    3
#define DEF_COMM_PORT_RATE_IDX_SRVTRNX  7

// IDC_COMBO_SEL_SERPORT_FOR_APP
static int idxComboSelSerportForApp;
#define SERPORT_APP_IDX_LIGHT       0
#define SERPORT_APP_IDX_ELMO		1
#define SERPORT_APP_IDX_XENUS		2
#define SERPORT_APP_IDX_ACS			3
#define SERPORT_APP_IDX_SERVOTRONIX 4
#define DEF_APP_IDX_USING_SERPORT   SERPORT_APP_IDX_LIGHT

#define MAX_NUM_APP_USING_SER_PORT  5
char *pstrAppNameUsingSerPort[] = {
	"Lighting",
	"ELMO",
	"COPLEY",
	"ACS",
	"SrvTrnx"
};

static CString cstrScriptLoadForRunning;  // 20120924
static CString cstrFilePathLoadScript;
static char cFlagPauseThread = 0;  // 20121030

// Lighting via SerPort
//#include "CSerialPort2.h"
//CSerialPort2 mLightingViaSerPort;
//CSerialPort2 mSerPortCfgELMO;
//CSerialPort2 mSerPortCfgXenus;

#define MAX_NUM_LIGHTING_CHANNEL    4
static unsigned char ucLightnessVal[MAX_NUM_LIGHTING_CHANNEL] = {50, 50, 50, 50};

static int m_IdxSerPort;
static unsigned char ucIdxLightCh;
static CComboBox *pSelLightChannelCombo;
static char cFlagConnectionSerialPort;
char cLightCommParity = 0, cLightCommByteSize = 8;
BOOL bLightCommUseModemEvt;
static int iCommPortBaudRate = 9600;
static char cFlagIsDebugMode;

static int iFlagRunningTimer = TRUE;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern double dSampleTime_ms[];
//CServoController theAcsServo;
extern COMM_SETTINGS stServoControllerCommSet;

#include "Serial.h"
CSerial cSerialPort;
LONG lLastError = ERROR_SUCCESS;

#include "_CommuDlg_LightingPR.cpp"

#define  __PR_MIL__
#ifdef __PR_MIL__
#include "_PR_ImageMil.cpp"
#endif  // __PR_MIL__

#include <conio.h> 
static int iFlagIsGrabbingImage = 0;

/////////////////////////////////////////////////////////////////////////////
// CCommunicationDialog dialog

CCommunicationDialog::CCommunicationDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCommunicationDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCommunicationDialog)
	m_CommunicationType = stServoControllerCommSet.CommType;
	m_IdxSerPort = stServoControllerCommSet.SerialPort;
//	m_IdxSerPortRate = stServoControllerCommSet.SerialRate;
	m_IdxSerPortRate = DEF_COMM_PORT_RATE_IDX_LIGHT;

	m_IdxComboConnectionEthPort = stServoControllerCommSet.EthernetConnection;
 	m_strAddressEthACS = stServoControllerCommSet.EthernetAddress;
	m_PCICardNumber = -1; // Simulation only ? 20080820
	//}}AFX_DATA_INIT
	Handle = stServoControllerCommSet.Handle;
	m_ConnectionString = stServoControllerCommSet.ConnectionString; 

	if(Handle != ACSC_INVALID)
	{
		m_fConnect = FALSE;
	}
	else
	{
		m_fConnect = TRUE;
	}
}

//CCommunicationDialog::OnClose
void CCommunicationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCommunicationDialog)
	DDX_Control(pDX, IDC_PCICOMBO, m_comboPCI);
	DDX_Radio(pDX, IDC_SERIALRADIO, m_CommunicationType);
	DDX_CBIndex(pDX, IDC_PORTCOMBO, m_IdxSerPort);
	DDX_CBIndex(pDX, IDC_RATECOMBO, m_IdxSerPortRate);
	DDX_CBIndex(pDX, IDC_ETHERNETCOMBO, m_IdxComboConnectionEthPort);
	DDX_Text(pDX, IDC_ETHERNETEDIT, m_strAddressEthACS);
	DDX_CBIndex(pDX, IDC_PCICOMBO, m_PCICardNumber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCommunicationDialog, CDialog)
	//{{AFX_MSG_MAP(CCommunicationDialog)
	ON_BN_CLICKED(IDC_SERIALRADIO, OnSerialRadioButton)
	ON_BN_CLICKED(IDC_ETHERNETRADIO, OnEthernetRadioButton)
	ON_BN_CLICKED(IDC_PCIRADIO, OnPCIRadioButton)
	ON_BN_CLICKED(IDC_SIMULRADIO, OnSimulRadioButton)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTUN_COMM_CONNECT_SERPORT, &CCommunicationDialog::OnBnClickedButtunCommConnectSerport)
	ON_EN_KILLFOCUS(IDC_EDIT_LIGHTING_VALUE_CH, &CCommunicationDialog::OnEnKillfocusEditLightingValueCh)
	ON_CBN_SELCHANGE(IDC_COMB_SEL_LIGHT_CHANNEL, &CCommunicationDialog::OnCbnSelchangeCombSelLightChannel)
	ON_BN_CLICKED(IDC_CHECK_TEST_SERPORT_COMM, &CCommunicationDialog::OnBnClickedCheckTestSerportComm)
	ON_CBN_SELCHANGE(IDC_RATECOMBO, &CCommunicationDialog::OnCbnSelchangeRatecombo)
	ON_CBN_SELCHANGE(IDC_PORTCOMBO, &CCommunicationDialog::OnCbnSelchangePortcombo)
	ON_BN_CLICKED(IDCANCEL, &CCommunicationDialog::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO_SEL_SERPORT_FOR_APP, &CCommunicationDialog::OnCbnSelchangeComboSelSerportForApp)
	ON_BN_CLICKED(IDC_BUTTON_COMMU_CONNECT_ACS, &CCommunicationDialog::OnBnClickedButtonCommuConnectAcs)
	ON_BN_CLICKED(IDC_BUTTON_COMM_DLG_START_END_GRAB_IMAGE, &CCommunicationDialog::OnBnClickedButtonCommDlgStartEndGrabImage)
	ON_BN_CLICKED(IDC_BUTTON_COMM_DLG_SAVE_IMAGE, &CCommunicationDialog::OnBnClickedButtonCommDlgSaveImage)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_LIGHTING_CH7_CoRED, &CCommunicationDialog::OnNMCustomdrawSliderLightingCh7Cored)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_LIGHTING_CH10_Co_BLUE, &CCommunicationDialog::OnNMCustomdrawSliderLightingCh7CoBlue)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_LIGHTING_CH9_SI_RED, &CCommunicationDialog::OnNMCustomdrawSliderLightingCh7SiRed)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_LIGHTING_CH8_SI_BLUE, &CCommunicationDialog::OnNMCustomdrawSliderLightingCh7SiBlue)
	ON_BN_CLICKED(IDC_CHECK_COMMUNICATION_DLG_CONNECT_ACS, &CCommunicationDialog::OnBnClickedCheckCommunicationDlgConnectAcs)
	ON_EN_KILLFOCUS(IDC_EDIT_COPLEY_I_SCALE, &CCommunicationDialog::OnEnKillfocusEditCopleyIScale)
	ON_EN_KILLFOCUS(IDC_EDIT_COPLEY_LIMIT_I_PEAK, &CCommunicationDialog::OnEnKillfocusEditCopleyLimitIPeak)
	ON_EN_KILLFOCUS(IDC_EDIT_COPLEY_LIMIT_I_RMS, &CCommunicationDialog::OnEnKillfocusEditCopleyLimitIRms)
	ON_EN_KILLFOCUS(IDC_EDIT_COPLEY_LIMIT_I2T, &CCommunicationDialog::OnEnKillfocusEditCopleyLimitI2t)
	ON_EN_KILLFOCUS(IDC_EDIT_COPLEY_GAIN_C_P, &CCommunicationDialog::OnEnKillfocusEditCopleyGainCP)
	ON_EN_KILLFOCUS(IDC_EDIT_COPLEY_GAIN_C_I, &CCommunicationDialog::OnEnKillfocusEditCopleyGainCI)
	ON_BN_CLICKED(IDC_BUTTON_COPLEY_SAVE_FLASH, &CCommunicationDialog::OnBnClickedButtonCopleySaveFlash)
	ON_BN_CLICKED(IDC_BUTTON_COPLY_READ_FLASH, &CCommunicationDialog::OnBnClickedButtonCoplyReadFlash)
	ON_EN_KILLFOCUS(IDC_EDIT_ELMO_I_SCALE, &CCommunicationDialog::OnEnKillfocusEditElmoIScale)
	ON_EN_KILLFOCUS(IDC_EDIT_ELMO_LIMIT_I_PEAK, &CCommunicationDialog::OnEnKillfocusEditElmoLimitIPeak)
	ON_EN_KILLFOCUS(IDC_EDIT_ELMO_LIMIT_I_RMS, &CCommunicationDialog::OnEnKillfocusEditElmoLimitIRms)
	ON_EN_KILLFOCUS(IDC_EDIT_ELMO_CURRENT_GAIN_P, &CCommunicationDialog::OnEnKillfocusEditElmoCurrentGainP)
	ON_EN_KILLFOCUS(IDC_EDIT_ELMO_CURRENT_GAIN_I, &CCommunicationDialog::OnEnKillfocusEditElmoCurrentGainI)
	ON_EN_KILLFOCUS(IDC_EDIT_ELMO_LIMIT_PEAK_TIME, &CCommunicationDialog::OnEnKillfocusEditElmoLimitPeakTime)
	ON_BN_CLICKED(IDC_BUTTON_SERIAL_PORT_SEND_COMM_DLG, &CCommunicationDialog::OnBnClickedButtonSerialPortSendCommDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_SERVOTRONIX_I_SCALE, &CCommunicationDialog::OnEnKillfocusEditServotronixIScale)
	ON_EN_KILLFOCUS(IDC_EDIT_SERVOTRONIX_DRIVER_LIMIT_CURRENT_PEAK, &CCommunicationDialog::OnEnKillfocusEditServotronixDriverLimitCurrentPeak)
	ON_EN_KILLFOCUS(IDC_EDIT_SERVOTRONIX_DRIVER_LIMIT_CURRENT_CONTI, &CCommunicationDialog::OnEnKillfocusEditServotronixDriverLimitCurrentConti)
	ON_EN_KILLFOCUS(IDC_EDIT_SERVOTRONIX_CURRENT_GAIN_P, &CCommunicationDialog::OnEnKillfocusEditServotronixCurrentGainP)
	ON_EN_KILLFOCUS(IDC_EDIT_SERVOTRONIX_CURRENT_GAIN_I, &CCommunicationDialog::OnEnKillfocusEditServotronixCurrentGainI)
	ON_EN_KILLFOCUS(IDC_EDIT_SERVOTRONIX_CURRENT_FF, &CCommunicationDialog::OnEnKillfocusEditServotronixCurrentFf)
	ON_EN_KILLFOCUS(IDC_EDIT_SERVOTRONIX_MOTOR_LIMIT_CURRENT_CONTI, &CCommunicationDialog::OnEnKillfocusEditServotronixMotorLimitCurrentConti)
	ON_EN_KILLFOCUS(IDC_EDIT_SERVOTRONIX_MOTOR_LIMIT_CURRENT_PEAK, &CCommunicationDialog::OnEnKillfocusEditServotronixMotorLimitCurrentPeak)
	ON_EN_KILLFOCUS(IDC_EDIT_SERVOTRONIX_CURRENT_K_BCAK_EMF, &CCommunicationDialog::OnEnKillfocusEditServotronixCurrentKBcakEmf)
	ON_BN_CLICKED(IDC_BUTTON_SERVOTRONIX_ENABLE_DISABLE_DRIVER, &CCommunicationDialog::OnBnClickedButtonServotronixEnableDisableDriver)
	ON_CBN_KILLFOCUS(IDC_COMMU_DLG_SEND_COMBO_, &CCommunicationDialog::OnCbnKillfocusCommuDlgSendCombo)
	ON_BN_CLICKED(IDC_BUTTON_COMMUDLG_LOAD_SCRIPT, &CCommunicationDialog::OnBnClickedButtonCommudlgLoadScript)
	ON_BN_CLICKED(IDC_BUTTON_COMMUDLG_RUN_SCRIPT, &CCommunicationDialog::OnBnClickedButtonCommudlgRunScript)
//	ON_BN_CLICKED(IDC_EDIT_COMMUNICATION_DLG_SRVTRONICS_MFBDIR, &CCommunicationDialog::OnBnClickedCheckCommunicationDlgSrvtronicsMfbdir)
	ON_EN_KILLFOCUS(IDC_EDIT_COMMUNICATION_DLG_SRVTRONICS_MPHASE, &CCommunicationDialog::OnEnKillfocusEditCommunicationDlgSrvtronicsMphase)
	ON_BN_CLICKED(IDC_CHECK_COMMUNICATION_DLG_SRVTRONICS_POSITION_DIR, &CCommunicationDialog::OnBnClickedCheckCommunicationDlgSrvtronicsPositionDir)
	ON_EN_KILLFOCUS(IDC_EDIT_COMMUNICATION_DLG_SRVTRONICS_MFBDIR, &CCommunicationDialog::OnEnKillfocusEditCommunicationDlgSrvtronicsMfbdir)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCommunicationDialog message handlers
void CCommunicationDialog::OnSerialRadioButton() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_PORTCOMBO)->EnableWindow(TRUE);
	GetDlgItem(IDC_RATECOMBO)->EnableWindow(TRUE);
	GetDlgItem(IDC_ETHERNETEDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_ETHERNETCOMBO)->EnableWindow(FALSE);
	GetDlgItem(IDC_PCICOMBO)->EnableWindow(FALSE);
}

void CCommunicationDialog::OnEthernetRadioButton() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_PORTCOMBO)->EnableWindow(FALSE);
	GetDlgItem(IDC_RATECOMBO)->EnableWindow(FALSE);
	GetDlgItem(IDC_ETHERNETEDIT)->EnableWindow(TRUE);
	GetDlgItem(IDC_ETHERNETCOMBO)->EnableWindow(TRUE);
	GetDlgItem(IDC_PCICOMBO)->EnableWindow(FALSE);
}

void CCommunicationDialog::OnPCIRadioButton() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_PORTCOMBO)->EnableWindow(FALSE);
	GetDlgItem(IDC_RATECOMBO)->EnableWindow(FALSE);
	GetDlgItem(IDC_ETHERNETEDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_ETHERNETCOMBO)->EnableWindow(FALSE);
	GetDlgItem(IDC_PCICOMBO)->EnableWindow(TRUE);
}

void CCommunicationDialog::OnSimulRadioButton() 
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_PORTCOMBO)->EnableWindow(FALSE);
	GetDlgItem(IDC_RATECOMBO)->EnableWindow(FALSE);
	GetDlgItem(IDC_ETHERNETEDIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_ETHERNETCOMBO)->EnableWindow(FALSE);
	GetDlgItem(IDC_PCICOMBO)->EnableWindow(FALSE);
}

void CCommunicationDialog::InitVarMachineType(int iFlagMachType)
{
	if(iFlagMachType == WB_STATION_USG_LIGHTING_PR 
		|| iFlagMachType == WB_STATION_EFO_BSD)
	{
		UI_EnableGroup_ACS_CommunicationDlg(0);  // 20121203
	}
	else
	{
	}
}

//#include "MotAlgo_DLL.h"
BOOL CCommunicationDialog::OnInitDialog() 
{
	int ii;
	
	int iFlagMachineType = get_sys_machine_type_flag();
	InitVarMachineType(iFlagMachineType);

	// UI Functions
	((CButton*) GetDlgItem(IDC_CHECK_COMMUNICATION_DLG_CONNECT_ACS))->SetCheck(sys_acs_communication_get_flag_sc_udi());

	switch (m_CommunicationType)
	{
		case 0:
			OnSerialRadioButton();
			break;
		case 1:
			OnEthernetRadioButton();
			break;
		case 2:
			OnPCIRadioButton();
			break;
		case 3:
			OnSimulRadioButton();
			break;
	}

	// get all installed SPiiPlus PCI cards
	if (acsc_GetPCICards(PCICards, 16, &ObtainedCards))
	{
		if (ObtainedCards > 0)
		{
			CString str;
			for(int ii = 0; ii < ObtainedCards; ii++)
			{
				str.Format("Bus: %d  Slot: %d", PCICards[ii].BusNumber, PCICards[ii].SlotNumber);
				m_comboPCI.AddString(str);
			}
			m_comboPCI.SetCurSel(0);
		}
		else
			GetDlgItem(IDC_PCIRADIO)->EnableWindow(FALSE);
	}
	else
		GetDlgItem(IDC_PCIRADIO)->EnableWindow(FALSE);

	// Set Comm Port for app
	idxComboSelSerportForApp  = SERPORT_APP_IDX_LIGHT;

	for(ii=0; ii< MAX_NUM_APP_USING_SER_PORT; ii++)
	{
		((CComboBox *)GetDlgItem(IDC_COMBO_SEL_SERPORT_FOR_APP))->InsertString(ii, pstrAppNameUsingSerPort[ii]);
	} // 
	((CComboBox *)GetDlgItem(IDC_COMBO_SEL_SERPORT_FOR_APP))->SetCurSel(idxComboSelSerportForApp);

	// Comm Port Rate
	for( ii = 0; ii< MAX_NUM_COMM_PORT_RATE; ii++)
	{
		((CComboBox *)GetDlgItem(IDC_RATECOMBO))->InsertString(ii, pstrCommPortRate[ii]);
	}
	((CComboBox *)GetDlgItem(IDC_RATECOMBO))->SetCurSel(m_IdxSerPortRate);

	// Check available COM ports
	// COM1;COM2;COM3;COM4;COM5;COM6;COM7;COM8;COM9;COM10;COM11;COM12;COM13;COM14;COM15;COM16;
	((CComboBox *)GetDlgItem(IDC_PORTCOMBO))->ResetContent();
	char sPortName[32];
	HANDLE  hCom;
	int nNumSerialPort;
	nNumSerialPort = 0;
	for(ii=1; ii<=16; ii++)
	{
		sprintf_s(sPortName, 32, "COM%d", ii);
		hCom = CreateFile(sPortName, GENERIC_READ | GENERIC_WRITE, 
              0, NULL, OPEN_EXISTING, 0, NULL);
		if (hCom != INVALID_HANDLE_VALUE) 
		{
			((CComboBox *)GetDlgItem(IDC_PORTCOMBO))->DeleteString(ii - 1);
			sprintf_s(sPortName, 32, "COM%d...", ii);
			((CComboBox *)GetDlgItem(IDC_PORTCOMBO))->InsertString(ii - 1, sPortName);  // nNumSerialPort
			nNumSerialPort ++;
			CloseHandle(hCom);
		}
	}
//    lLogList.AddString("Error opening port");
//	::getsystemh

	// Initialize the lighting control
	CString cstrTemp;
	pSelLightChannelCombo = (CComboBox*) GetDlgItem(IDC_COMB_SEL_LIGHT_CHANNEL);
	for(int ii = 0; ii< MAX_NUM_LIGHTING_CHANNEL; ii++)
	{
		cstrTemp.Format("J%d", ii + 7);
		pSelLightChannelCombo->InsertString(ii, cstrTemp);
	}
	// Lighting 124
	UI_InitSlider_Lighting124();
	cFlagConnectionSerialPort = cSerialPort.IsOpen(); // mLightingViaSerPort.isConnected();
	UpdateCommunicationDlgUI_byConnectFlag();

	ucIdxLightCh = 0;
	pSelLightChannelCombo->SetCurSel(ucIdxLightCh);

	//Initialize the baudrate
	SetUserInterfaceLanguage();  //	OnCbnSelchangeRatecombo(); 	UpdateConnectButtonACS();

	// 
	GetDlgItem(IDC_BUTTON_COMMUDLG_RUN_SCRIPT)->EnableWindow(FALSE);
	cstrFilePathLoadScript.Format("...");
	GetDlgItem(IDC_STATIC_COMMU_DLG_SCRIPT_FILEPATH)->SetWindowTextA(cstrFilePathLoadScript);

	//
	GetDlgItem(IDC_BUTTON_COMM_DLG_START_END_GRAB_IMAGE)->SetWindowTextA(_T("StartGrabImage"));

	return CDialog::OnInitDialog();  // TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CCommunicationDialog::UpdateConnectButtonACS()
{
	if(m_fConnect == TRUE)
	{
		if(get_sys_language_option() == LANGUAGE_UI_EN)
		{
			GetDlgItem(IDC_BUTTON_COMMU_CONNECT_ACS)->SetWindowText(_T("ConnectACS"));
		}
		else
		{
			GetDlgItem(IDC_BUTTON_COMMU_CONNECT_ACS)->SetWindowText(_T("连接ACS"));
		}
	}
	else
	{
		if(get_sys_language_option() == LANGUAGE_UI_EN)
		{
			GetDlgItem(IDC_BUTTON_COMMU_CONNECT_ACS)->SetWindowText(_T("ByeACS"));
		}
		else
		{
			GetDlgItem(IDC_BUTTON_COMMU_CONNECT_ACS)->SetWindowText(_T("断开ACS"));
		}
	}

}

void CCommunicationDialog::OnOK() 
{
	iFlagRunningTimer = FALSE;
}

int CCommunicationDialog::DlgGetFlagTimerRunning()
{
	return iFlagRunningTimer;
}

BOOL CCommunicationDialog::ShowWindow(int nCmdShow)
{
	iFlagRunningTimer = TRUE;
	return CDialog::ShowWindow(nCmdShow);
}

void CCommunicationDialog::GetCommSettings(COMM_SETTINGS* CommSet)
{
	// retrieve communication settings
	CommSet->CommType = m_CommunicationType;
	CommSet->SerialPort = m_IdxSerPort;
	CommSet->SerialRate = m_IdxSerPortRate;
	CommSet->EthernetConnection = m_IdxComboConnectionEthPort;
 	CommSet->EthernetAddress = m_strAddressEthACS;
	CommSet->Handle = Handle;
	CommSet->ConnectionString = m_ConnectionString; 
}

void CCommunicationDialog::ReadByteFromEdit(int nResId, char *pcValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", pcValue);
}

    // Attempt to open the serial port (COM1)
#include "Serial.h"
// IDC_BUTTUN_COMM_CONNECT_SERPORT
void CCommunicationDialog::OnBnClickedButtunCommConnectSerport()
{
//	CSerial cSerialPort;
//	int iRetErrorConnectSerPort;
	char strComPortId[64];

	//((CComboBox *)GetDlgItem(IDC_RATECOMBO))->GetCurSel();
	//((CComboBox *)GetDlgItem(IDC_RATECOMBO))->GetWindowTextA(
	sprintf_s(strComPortId, 64, "COM%d", m_IdxSerPort + 1);

	switch(idxComboSelSerportForApp)
	{
	case SERPORT_APP_IDX_LIGHT:

		if(cFlagConnectionSerialPort == 0)
		{
			lLastError = cSerialPort.Open(_T(strComPortId),0,0,false);  // "COM1"
			lLastError = cSerialPort.Setup(CSerial::EBaud9600,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);

			if(lLastError != ERROR_SUCCESS)
			{
				AfxMessageBox(_T("ErrorConnection!"));   // strTemp));  // 
			}
			else
			{
				ltOutputAllChannelDefaultValue();
			}
		}
		else
		{
			ltOutputZeroAllChannel();  
			cFlagDlgDriverCommunicationThreadRunning= FALSE; Sleep(500);  pDlgDriverCommunicationWinThread = NULL;// 20121011
			cSerialPort.Close();
		}

		cFlagConnectionSerialPort = cSerialPort.IsOpen();
	break;
	case SERPORT_APP_IDX_ELMO: // UpdateELMO_UI_byConnectFlag
		if(cFlagConnectionSerialPort == 0)
		{
			
			lLastError = cSerialPort.Open(_T(strComPortId),0,0,false);  // "COM1"
			lLastError = cSerialPort.Setup(CSerial::EBaud19200,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);

			if(lLastError == ERROR_SUCCESS)
			{
				cSerialPort.SetMask(CSerial::EEventBreak |
										CSerial::EEventCTS   |
										CSerial::EEventDSR   |
										CSerial::EEventError |
										CSerial::EEventRing  |
										CSerial::EEventRLSD  |
										CSerial::EEventRecv);
				cSerialPort.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);

				GetDlgItem(IDC_BUTTUN_COMM_CONNECT_SERPORT)->EnableWindow(FALSE);
				UpdateELMO_UI_byConnectFlag();
			}
			else
			{
				AfxMessageBox(_T("ErrorConnection!"));   // strTemp));  // 
			}
		}
		else
		{
			cFlagDlgDriverCommunicationThreadRunning= FALSE; Sleep(500); pDlgDriverCommunicationWinThread = NULL; // 20121011
			cSerialPort.Close();
		}
		cFlagConnectionSerialPort = cSerialPort.IsOpen();
		break;
	case SERPORT_APP_IDX_XENUS:
		if(cFlagConnectionSerialPort == 0)
		{
			lLastError = cSerialPort.Open(_T(strComPortId),0,0,false); // "COM1"
			if(iCommPortBaudRate == 115200) // 20120719
			{
				lLastError = cSerialPort.Setup(CSerial::EBaud115200,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
			}
			else
			{
				lLastError = cSerialPort.Setup(CSerial::EBaud9600,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
			}
			//cSerialPort.Setup(CSerial::EBaud115200,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
			ResetCopleyDriverBaudRate9600();

			Sleep(1500);

			if(lLastError == ERROR_SUCCESS)
			{
				cSerialPort.SetMask(CSerial::EEventBreak |
										CSerial::EEventCTS   |
										CSerial::EEventDSR   |
										CSerial::EEventError |
										CSerial::EEventRing  |
										CSerial::EEventRLSD  |
										CSerial::EEventRecv);
				cSerialPort.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);

				GetDlgItem(IDC_BUTTUN_COMM_CONNECT_SERPORT)->EnableWindow(FALSE);
				Sleep(1000);
				UpdateCopleyUI_byConnectFlag();
//				cFlagConnectionSerialPort = 1;
			}
			else
			{
				AfxMessageBox(_T("ErrorConnection!"));   // strTemp));  // 
//				cFlagConnectionSerialPort = 0;
			}
		}
		else
		{
			cFlagDlgDriverCommunicationThreadRunning= FALSE; Sleep(500); pDlgDriverCommunicationWinThread = NULL; // 20121011
			cSerialPort.Close();
//			cFlagConnectionSerialPort = 0;
		}
		cFlagConnectionSerialPort = cSerialPort.IsOpen();
		
		break;
	case SERPORT_APP_IDX_ACS:
		break;
	case SERPORT_APP_IDX_SERVOTRONIX: // 
		if(cFlagConnectionSerialPort == 0)
		{
			
			lLastError = cSerialPort.Open(_T(strComPortId),0,0,false);  // "COM1"
//			EBaudrate eBaudrate = CSerial::EBaud115200;
			lLastError = cSerialPort.Setup( CSerial::EBaud115200, CSerial::EData8,CSerial::EParNone,CSerial::EStop1);

			if(lLastError == ERROR_SUCCESS)
			{
				cSerialPort.SetMask(CSerial::EEventBreak |
										CSerial::EEventCTS   |
										CSerial::EEventDSR   |
										CSerial::EEventError |
										CSerial::EEventRing  |
										CSerial::EEventRLSD  |
										CSerial::EEventRecv);
				cSerialPort.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);

				GetDlgItem(IDC_BUTTUN_COMM_CONNECT_SERPORT)->EnableWindow(FALSE);
				UpdateServoTronix_UI_byConnectFlag();
			}
			else
			{
				AfxMessageBox(_T("ErrorConnection!"));   // strTemp));  // 
			}
		}
		else
		{
			cFlagDlgDriverCommunicationThreadRunning= FALSE; Sleep(500); pDlgDriverCommunicationWinThread = NULL; // 20121011
			cSerialPort.Close();
		}
		cFlagConnectionSerialPort = cSerialPort.IsOpen();
		
		break;
	}

	if(cFlagConnectionSerialPort == TRUE)
	{
		StartDriverCommunicationThread();
	}

	Sleep(1000);
	UpdateCommunicationDlgUI_byConnectFlag();
	GetDlgItem(IDC_BUTTUN_COMM_CONNECT_SERPORT)->EnableWindow(TRUE);
}


// IDC_CHECK_TEST_SERPORT_COMM
void CCommunicationDialog::OnBnClickedCheckTestSerportComm()
{
	cFlagIsDebugMode = ((CButton *)GetDlgItem(IDC_CHECK_TEST_SERPORT_COMM))->GetCheck();
	if(cFlagIsDebugMode)
	{
		GetDlgItem(IDC_COMB_SEL_LIGHT_CHANNEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_LIGHTING_VALUE_CH)->EnableWindow(TRUE);
	}
	else
	{
		if(cSerialPort.IsOpen())  // mLightingViaSerPort.isConnected() == 0
		{
			GetDlgItem(IDC_COMB_SEL_LIGHT_CHANNEL)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_LIGHTING_VALUE_CH)->EnableWindow(FALSE);
		}
	}
}
// IDC_RATECOMBO
void CCommunicationDialog::OnCbnSelchangeRatecombo()
{
	char strTemp[32];
//	((CComboBox *)GetDlgItem(IDC_RATECOMBO))->GetLBText(m_IdxSerPortRate, strTemp);
	((CComboBox *)GetDlgItem(IDC_RATECOMBO))->GetWindowTextA(strTemp, 32);
	sscanf_s(strTemp, "%d", &iCommPortBaudRate);
}
// IDC_PORTCOMBO
void CCommunicationDialog::OnCbnSelchangePortcombo()
{
	m_IdxSerPort = ((CComboBox *)GetDlgItem(IDC_PORTCOMBO))->GetCurSel();
}

extern void mtntestdlg_entry_reset_flag_trigger_acs_comm();
void CCommunicationDialog::OnBnClickedCancel()
{
	ltOutputZeroAllChannel();
	if(cSerialPort.IsOpen()) //cFlagConnectionSerialPort = mLightingViaSerPort.isConnected())
	{
		cFlagDlgDriverCommunicationThreadRunning= FALSE; Sleep(500); pDlgDriverCommunicationWinThread = NULL; // 20121011
		cSerialPort.Close(); // mLightingViaSerPort.Disconnect();
	}

	mtntestdlg_entry_reset_flag_trigger_acs_comm();

	iFlagRunningTimer = FALSE;
	OnCancel();
}

// IDC_STATIC_COMMDLG_DRIVER_HALL_U
// IDC_STATIC_COMMDLG_DRIVER_HALL_V
// IDC_STATIC_COMMDLG_DRIVER_HALL_W

// IDC_COMBO_SEL_SERPORT_FOR_APP
void CCommunicationDialog::OnCbnSelchangeComboSelSerportForApp()
{
	idxComboSelSerportForApp = ((CComboBox *)GetDlgItem(IDC_COMBO_SEL_SERPORT_FOR_APP))->GetCurSel();
	switch(idxComboSelSerportForApp)
	{
	case SERPORT_APP_IDX_LIGHT:
		m_IdxSerPortRate = DEF_COMM_PORT_RATE_IDX_LIGHT;
		break;
	case SERPORT_APP_IDX_ELMO:
		m_IdxSerPortRate = DEF_COMM_PORT_RATE_IDX_ELMO;
		break;
	case SERPORT_APP_IDX_XENUS:
		m_IdxSerPortRate = DEF_COMM_PORT_RATE_IDX_XENUS;
		break;
	case SERPORT_APP_IDX_ACS:
		m_IdxSerPortRate = DEF_COMM_PORT_RATE_IDX_ACS;
		break;
	case SERPORT_APP_IDX_SERVOTRONIX:   // 20120622
		m_IdxSerPortRate = DEF_COMM_PORT_RATE_IDX_SRVTRNX;
		break;
	}
	((CComboBox *)GetDlgItem(IDC_RATECOMBO))->SetCurSel(m_IdxSerPortRate);
}
extern void mtn_api_prompt_acs_error_code(HANDLE stCommHandleAcs);  // 20110803

#include "MtnTesterEntry.h"
#include "aftprbs.h"
// extern MtnTesterEntry cMainDialog;

// IDC_BUTTON_COMMU_CONNECT_ACS
void CCommunicationDialog::OnBnClickedButtonCommuConnectAcs()
{
	UpdateData(TRUE);

	if (m_fConnect) //connect
	{
		Handle = ACSC_INVALID;
		int iConnectionEthPort;
		switch(m_CommunicationType)
		{
			case 0:	//cSerialPort
				char str[256];
				((CComboBox *)GetDlgItem(IDC_RATECOMBO))->GetLBText(m_IdxSerPortRate, str);
				Handle = acsc_OpenCommSerial(m_IdxSerPort + 1, atoi(str));
				m_ConnectionString.Format("Communication: Serial, COM%d, %s", m_IdxSerPort + 1, str);
				theAcsServo.Initialization(ONLINE_MODE);
				break;
			case 1:	//ethernet
				iConnectionEthPort = (m_IdxComboConnectionEthPort == 0) ? ACSC_SOCKET_DGRAM_PORT : ACSC_SOCKET_STREAM_PORT;
				Handle = acsc_OpenCommEthernet((char *)LPCTSTR(m_strAddressEthACS), iConnectionEthPort);
				Sleep(100);
				m_ConnectionString.Format("Communication: Ethernet, %s", (char *)LPCTSTR(m_strAddressEthACS));
//				theAcsServo.Initialization(ONLINE_MODE);
				Sleep(500);
				// cFlag_ConnectACS_SC_UDI
				if(sys_acs_communication_get_flag_sc_udi() == TRUE)
				{
					InitWireBondNameServo_ACS_SC_UDI_Axis(); // 20121203
				}
				else
				{
					InitWireBondServoAxisName();
				}

				break;
			case 2:	//PCI bus
				Handle = acsc_OpenCommPCI(PCICards[m_PCICardNumber].SlotNumber);
				m_ConnectionString.Format("Communication: PCI bus, Bus %d, Slot %d", PCICards[m_PCICardNumber].BusNumber, PCICards[m_PCICardNumber].SlotNumber);

//				mtn_api_prompt_acs_error_code(Handle);  // 20110803

				theAcsServo.Initialization(ONLINE_MODE);
				break;
			case 3:	//simulator
				//Handle = acsc_OpenCommDirect();
				m_ConnectionString.Format("Communication: Simulator");
				theAcsServo.Initialization(OFFLINE_MODE);
				if(theAcsServo.GetServoHandle() != ACSC_INVALID)
				{
					Handle = theAcsServo.GetServoHandle();
				}
				break;
		}
		if (Handle == ACSC_INVALID)
		{
			MessageBox("Communication link has not been established", "ACSC Terminal", MB_ICONWARNING);
			m_ConnectionString.Format("Communication: OFF");
		}
		else
		{
			stServoControllerCommSet.Handle = Handle;
			mtn_api_prompt_acs_error_code(Handle);  // 20110803
			mtn_api_init_acs_buffer_prog();

			// Set the interrupt period
			double dSampleTime_ms = 0.5;
			char cErrorCode[64];
			switch(m_CommunicationType)
			{
				case 0:	//serial
				case 1:	//ethernet
					dSampleTime_ms = 0.5;
					if(!acsc_ReadReal(Handle, ACSC_NONE, "CTIME", 0, 0, ACSC_NONE, ACSC_NONE, &dSampleTime_ms, NULL))
					{
						sprintf_s(cErrorCode, "transaction error: %d\n", acsc_GetLastError());
					}
					sys_set_ctrl_communication_type(FLAG_CONNECTED_WITH_ACT_CARD);
					sys_init_controller_ts(dSampleTime_ms);
//					(*MtnTesterEntry)m_pMainWnd->OnCbnSelchangeComboSysMachineType(); // 20121203
					sys_config_machine_type(get_sys_machine_type_flag());

					break;
				case 2:	//PCI bus
					dSampleTime_ms = 0.5;
					if(!acsc_WriteReal(Handle, ACSC_NONE, "CTIME", 0, 0, ACSC_NONE, ACSC_NONE, &dSampleTime_ms, NULL))
					{
						sprintf_s(cErrorCode, "transaction error: %d\n", acsc_GetLastError());
					}
					sys_set_ctrl_communication_type(FLAG_CONNECTED_WITH_ACT_CARD);
					sys_init_controller_ts(dSampleTime_ms);

					break;
				case 3:	//simulator
					sys_set_ctrl_communication_type(FLAG_CONNECTED_WITH_SIMULATOR);
					break;
			}

			m_fConnect = FALSE;
		}
	}
	else // disconnect
	{
		acsc_CloseHistoryBuffer(Handle);
		mtn_api_clear_acs_buffer_prof(); // 20121203

		acsc_CloseComm(Handle);
		m_fConnect = TRUE;
		Handle = ACSC_INVALID;
	}

	UpdateConnectButtonACS();
	GetCommSettings(&stServoControllerCommSet);
}

// 
void CCommunicationDialog::SetUserInterfaceLanguage()
{
	if(get_sys_language_option() == LANGUAGE_UI_EN)
	{
		GetDlgItem(IDC_SERIALRADIO					 )->SetWindowTextA(_T("Serial"));
		GetDlgItem(IDC_ETHERNETRADIO                 )->SetWindowTextA(_T("Ethernet"));
		GetDlgItem(IDC_PCIRADIO                      )->SetWindowTextA(_T("PCI Bus"));
		GetDlgItem(IDC_SIMULRADIO                    )->SetWindowTextA(_T("Simulator"));
//		GetDlgItem(IDC_BUTTON_COMMU_CONNECT_ACS      )->SetWindowTextA(_T("Connect_ACS"));
		GetDlgItem(IDC_LABEL_SER_PORT_USEFOR_COMM_DLG)->SetWindowTextA(_T("SerPort For:"));
		GetDlgItem(IDC_PORTSTATIC                    )->SetWindowTextA(_T("Port"));
		GetDlgItem(IDC_ADDRESSSTATIC                 )->SetWindowTextA(_T("Remote address"));
		GetDlgItem(IDC_PCISTATIC                     )->SetWindowTextA(_T("Bus/Slot"));
		GetDlgItem(IDC_STATIC_LIGHTING_BL00124       )->SetWindowTextA(_T("Lighting"));
		GetDlgItem(IDC_LABEL_STATIC_LIGHTING_CHANNEL )->SetWindowTextA(_T("ConnectSerPort"));
		GetDlgItem(IDC_LABEL_LIGHTING_VALUE_CH )->SetWindowTextA(_T("Value[<255]"));
//
		GetDlgItem(IDC_STATIC_LIGHTING_CH7_CoRED     )->SetWindowTextA(_T("Co RedJ7"));
		GetDlgItem(IDC_STATIC_LIGHTING_CH10_Co_BLUE   )->SetWindowTextA(_T("Co.BlueJ10"));
		GetDlgItem(IDC_STATIC_LIGHTING_CH9_SI_RED	 )->SetWindowTextA(_T("Si.BlueJ9"));
		GetDlgItem(IDC_STATIC_LIGHTING_CH8_SI_BLUE   )->SetWindowTextA(_T("Si.RedJ8"));
		GetDlgItem(IDC_BUTTON_COPLEY_SAVE_FLASH)->SetWindowTextA(_T("Save F"));
		GetDlgItem(IDC_BUTTON_COPLY_READ_FLASH)->SetWindowTextA(_T("Read F"));

	}
	else
	{
		GetDlgItem(IDC_SERIALRADIO					 )->SetWindowTextA(_T("串行口"));
		GetDlgItem(IDC_ETHERNETRADIO                 )->SetWindowTextA(_T("以太网"));
		GetDlgItem(IDC_PCIRADIO                      )->SetWindowTextA(_T("PCI总线"));
		GetDlgItem(IDC_SIMULRADIO                    )->SetWindowTextA(_T("软件仿真"));
//		GetDlgItem(IDC_BUTTON_COMMU_CONNECT_ACS      )->SetWindowTextA(_T(""));
		GetDlgItem(IDC_LABEL_SER_PORT_USEFOR_COMM_DLG)->SetWindowTextA(_T("串口用途"));
		GetDlgItem(IDC_PORTSTATIC                    )->SetWindowTextA(_T("串口选择"));
		GetDlgItem(IDC_ADDRESSSTATIC                 )->SetWindowTextA(_T("IP地址"));
		GetDlgItem(IDC_PCISTATIC                     )->SetWindowTextA(_T("PCI地址"));
		GetDlgItem(IDC_STATIC_LIGHTING_BL00124       )->SetWindowTextA(_T("灯板"));
		GetDlgItem(IDC_LABEL_STATIC_LIGHTING_CHANNEL )->SetWindowTextA(_T("灯板通道"));
		GetDlgItem(IDC_LABEL_LIGHTING_VALUE_CH )->SetWindowTextA(_T("数值[<255]"));

//
		GetDlgItem(IDC_STATIC_LIGHTING_CH7_CoRED     )->SetWindowTextA(_T("同轴红J7"));
		GetDlgItem(IDC_STATIC_LIGHTING_CH10_Co_BLUE   )->SetWindowTextA(_T("同轴蓝J10"));
		GetDlgItem(IDC_STATIC_LIGHTING_CH9_SI_RED	 )->SetWindowTextA(_T("侧兰J9"));
		GetDlgItem(IDC_STATIC_LIGHTING_CH8_SI_BLUE   )->SetWindowTextA(_T("侧红J8"));
		GetDlgItem(IDC_BUTTON_COPLEY_SAVE_FLASH)->SetWindowTextA(_T("写存"));
		GetDlgItem(IDC_BUTTON_COPLY_READ_FLASH)->SetWindowTextA(_T("读存"));
	}
	
	UpdateCommunicationDlgUI_byConnectFlag();
	UpdateConnectButtonACS();
}

// IDC_BUTTON_COMM_DLG_START_END_GRAB_IMAGE
void CCommunicationDialog::OnBnClickedButtonCommDlgStartEndGrabImage()
{
	if(iFlagIsGrabbingImage == 0)
	{
#ifdef  __PR_MIL__
		mil_init_alloc_default();
		Sleep(100);
		mil_start_grab_continuous();
#endif  // __PR_MIL__
		iFlagIsGrabbingImage = 1;
		GetDlgItem(IDC_BUTTON_COMM_DLG_START_END_GRAB_IMAGE)->SetWindowTextA(_T("StopGrabImage"));
		GetDlgItem(IDC_BUTTON_COMM_DLG_SAVE_IMAGE)->EnableWindow(TRUE);
	}
	else
	{
#ifdef  __PR_MIL__
		mil_stop_grab_image();
		Sleep(100);
		mil_release_resource();
#endif  // __PR_MIL__
		iFlagIsGrabbingImage = 0;
		GetDlgItem(IDC_BUTTON_COMM_DLG_START_END_GRAB_IMAGE)->SetWindowTextA(_T("StartGrabImage"));
		GetDlgItem(IDC_BUTTON_COMM_DLG_SAVE_IMAGE)->EnableWindow(FALSE);
	}
}

void CCommunicationDialog::OnBnClickedButtonCommDlgSaveImage()
{
	BOOL	 SaveStatus;
	//LPCTSTR lpszPathName;
   CString TempPath, cszPathName;
   long FileFormat ;
   char strFilenameDefault[128]; 
struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;

	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);

   sprintf_s(strFilenameDefault, 128, "Image_%d-%d-%d_%d-%d_J7-%d_J8-%d_J9-%d_J10-%d.bmp", stpTime->tm_year +1900, stpTime->tm_mon +1, 
			stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, ucLightnessVal[0],ucLightnessVal[1],ucLightnessVal[2],ucLightnessVal[3]);
   //CString cszPathName
   // Get extension for file format determination
   CFileDialog cFileDlg(TRUE, NULL, strFilenameDefault, OFN_HIDEREADONLY,"BMP   Files(*.*)|*.*||",NULL);
   // Get extension for file format determination
   if(cFileDlg.DoModal() == IDOK)
   {
	   cszPathName = cFileDlg.GetFileName();
   }//cszPathName;
   else
   {
	   return ;
   }

   TempPath = cszPathName;  // cszPathName;
   TempPath.MakeUpper();

#ifdef  __PR_MIL__
   //Set the file format to M_MIL when the file path extension is ".MIM"
   if (TempPath.Find(".MIM") != -1)
      FileFormat = M_MIL;
   //Set the file format to M_TIFF when the file path extension is ".TIF"
   if (TempPath.Find(".TIF") != -1)
      FileFormat = M_TIFF;
   //Set the file format to M_BMP when the file path extension is ".BMP"
   if (TempPath.Find(".BMP") != -1)
      FileFormat = M_BMP;
   //Set the file format to M_JPEG_LOSSY when the file path extension is ".JPG"
   if (TempPath.Find(".JPG") != -1)
      FileFormat = M_JPEG_LOSSY;
   //Set the file format to M_JPEG2000_LOSSLESS when the file path extension is ".JP2"
   if (TempPath.Find(".JP2") != -1)
      FileFormat = M_JPEG2000_LOSSLESS;
   //Set the file format to M_RAW when the file path extension is ".RAW"
   if (TempPath.Find(".RAW") != -1)
      FileFormat = M_RAW;
		
   MdigHalt(MilDigitizer);
	// Save the current buffer [CALL TO MIL]
   MbufExport((LPSTR)cszPathName.GetString(), FileFormat, MilImage);
	
	// Verify if save operation was successful [CALL TO MIL]
	SaveStatus = (MappGetError(M_CURRENT,M_NULL) == M_NULL_ERROR);

	//// Document has been saved
	//if (!((((CMDispMFCApp*)AfxGetApp())->m_pGrabView == pFirstView) &&
	//      (((CMDispMFCApp*)AfxGetApp())->m_isGrabStarted == TRUE)))
	//   SetModifiedFlag(FALSE);
	MdigGrabContinuous(MilDigitizer, MilImage);
#endif  // __PR_MIL__
}

static char *strStopString;
void CCommunicationDialog::ReadDoubleFromEdit(int nResId, double *dValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	*dValue = strtod(tempChar, &strStopString);
}
void CCommunicationDialog::ReadUnsignedIntegerFromEdit(int nResId, unsigned int *uiValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", uiValue);
}

// IDC_COMMUNICATION_DLG_ACS_GROUP
// IDC_SERIALRADIO
// IDC_ETHERNETRADIO
// IDC_PCIRADIO
// IDC_SIMULRADIO

// IDC_CHECK_COMMUNICATION_DLG_CONNECT_ACS
void CCommunicationDialog::OnBnClickedCheckCommunicationDlgConnectAcs()
{
	char cTempFlag_ConnectACS_SC_UDI;
	cTempFlag_ConnectACS_SC_UDI = ((CButton*) GetDlgItem(IDC_CHECK_COMMUNICATION_DLG_CONNECT_ACS))->GetCheck();
	if(cTempFlag_ConnectACS_SC_UDI == 0)  // 20121203
	{
		m_CommunicationType = 2;
		m_strAddressEthACS.Format("10.0.0.100");
		m_IdxComboConnectionEthPort = 1;
	}
	else
	{
		m_IdxComboConnectionEthPort = 1; // Use Network Connection for ACS-SC-UDI controller
		m_CommunicationType = 1;
		m_strAddressEthACS.Format("192.168.157.20");
	}
	sys_acs_communication_set_flag_sc_udi(cTempFlag_ConnectACS_SC_UDI);
	UpdateData(FALSE); // 20121203
//	UI_EnableGroup_ACS_CommunicationDlg(cFlag_ConnectACS_SC_UDI);
}

void CCommunicationDialog::UI_EnableGroup_ACS_CommunicationDlg(BOOL bEnableFlag)
{
	GetDlgItem(IDC_COMMUNICATION_DLG_ACS_GROUP)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_SERIALRADIO)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_ETHERNETRADIO)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_PCIRADIO)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_SIMULRADIO)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_COMMU_CONNECT_ACS)->ShowWindow(bEnableFlag);	
}

static DWORD dwBytesRead = 0;
static char szBuffer[1024];
unsigned int iCoplyCurrentScale, iCopleyCurrenntPeakLimit, iCopleyCurrentContiLimit, iCopleyI2T_Limit;
double dCoplyCurrentScale, dCopleyCurrenntPeakLimit, dCopleyCurrentContiLimit, dCopleyI2T_Limit;
static char szBufferSendCommand[512];

unsigned int iCopleyGainCp, iCopleyGainCi;

LONG CCommunicationDialog :: SerialPortReadReply(DWORD dwTimeout)
{
	memset(szBuffer, 0, 1024); // 20121030

		lLastError = cSerialPort.WaitEvent(0, dwTimeout);
		if (lLastError != ERROR_SUCCESS)
		{ AfxMessageBox( _T("Unable to wait for a COM-port event.")); return MTN_API_ERROR; }

		// Save event
		const CSerial::EEvent eEvent = cSerialPort.GetEventType();

		// Handle break event
		if (eEvent & CSerial::EEventBreak)
		{
			printf("\n### BREAK received ###\n");
		}

		// Handle CTS event
		if (eEvent & CSerial::EEventCTS)
		{
			printf("\n### Clear to send %s ###\n", cSerialPort.GetCTS()?"on":"off");
		}

		// Handle DSR event
		if (eEvent & CSerial::EEventDSR)
		{
			printf("\n### Data set ready %s ###\n", cSerialPort.GetDSR()?"on":"off");
		}

		// Handle error event
		if (eEvent & CSerial::EEventError)
		{
			printf("\n### ERROR: ");
			switch (cSerialPort.GetError())
			{
			case CSerial::EErrorBreak:		printf("Break condition");			break;
			case CSerial::EErrorFrame:		printf("Framing error");			break;
			case CSerial::EErrorIOE:		printf("IO device error");			break;
			case CSerial::EErrorMode:		printf("Unsupported mode");			break;
			case CSerial::EErrorOverrun:	printf("Buffer overrun");			break;
			case CSerial::EErrorRxOver:		printf("Input buffer overflow");	break;
			case CSerial::EErrorParity:		printf("Input parity error");		break;
			case CSerial::EErrorTxFull:		printf("Output buffer full");		break;
			default:						printf("Unknown");					break;
			}
			printf(" ###\n");
		}

		// Handle ring event
		if (eEvent & CSerial::EEventRing)
		{
			printf("\n### RING ###\n");
		}

		// Handle RLSD/CD event
		if (eEvent & CSerial::EEventRLSD)
		{
			printf("\n### RLSD/CD %s ###\n", cSerialPort.GetRLSD()?"on":"off");
		}

		// Handle data receive event
		if (eEvent & CSerial::EEventRecv)
		{
			// Read data, until there is nothing left
			do
			{
				// Read data from the COM-port
				lLastError = cSerialPort.Read(szBuffer,sizeof(szBuffer)-1,&dwBytesRead);
				if (lLastError != ERROR_SUCCESS)
				{ AfxMessageBox(_T("Unable to read from COM-port.")); 	return UNABLE_TO_READ_FROM_COM_PORT;}

				if (dwBytesRead > 0)
				{
					// Finalize the data, so it is a valid string
					szBuffer[dwBytesRead] = '\0';

					// Display the data
					printf("%s", szBuffer);

					// Check if EOF (CTRL+'[') has been specified
//					if (strchr(szBuffer,EOF_Char))
//						fContinue = false;
				}
			}
		    while (dwBytesRead == sizeof(szBuffer)-1);
		}

		return MTN_API_OK_ZERO;
}

#include "_CommuDlg_Copley.cpp"
#include "_CommuDlg_ELMO.cpp"
#include "_CommuDlg_SrvTronics.cpp"
CComboBox	*pComboBoxCommunicationDlgSendCmd;
CString strCommuDlgComboSentCmd;
DWORD dwTimeOutSerialPort = 15000; // in milliSec
static char m_HistBuf[HISTBUF_SIZE];
//static char m_EditBuf[EDITBUF_SIZE];
CString cstrEditBuffer;
static int m_nEditPos = 0;

void CCommunicationDialog::FormatBufferAddSendAndReplyFromSerialPort()
{
	// each line of the controller replies are concluded by '\r'
	// function adds '\n' in the end of the lines after '\r'
	static char tmpbuf[2*HISTBUF_SIZE];
	memset(tmpbuf, 0, 2*HISTBUF_SIZE); // 20121030

	
	sprintf_s(m_HistBuf, HISTBUF_SIZE, "%s", szBuffer);
	//sprintf_s(tmpbuf, 2 * HISTBUF_SIZE, "%s\r\n", );  // 20110709
	// strCommuDlgComboSentCmd.GetString()
	int hbuf_pos = strCommuDlgComboSentCmd.GetLength();
	int tmpbuf_pos =  + 2;  // 20110709， strCommuDlgComboSentCmd.GetLength()
	while (m_HistBuf[hbuf_pos] != '\0')
	{
		if (m_HistBuf[hbuf_pos] == '\n')
		{	
			tmpbuf[tmpbuf_pos++] = '\r'; 
			tmpbuf[tmpbuf_pos++] = '\n';
		}
		else
		{
			tmpbuf[tmpbuf_pos++] = m_HistBuf[hbuf_pos];
		}
		hbuf_pos++;
	}
	tmpbuf[tmpbuf_pos++] = '\0';    // 20100723, only keep the last single '/0'
	if (EDITBUF_SIZE - m_nEditPos < tmpbuf_pos)
	{
		int shortage = tmpbuf_pos - (HISTBUF_SIZE - m_nEditPos);
		m_nEditPos -= shortage;
//		memcpy(m_EditBuf, &m_EditBuf[shortage], m_nEditPos);
	}
//	memcpy(&m_EditBuf[m_nEditPos], tmpbuf, tmpbuf_pos); // strcpy_s, run-time error, 20100723
	m_nEditPos += (tmpbuf_pos - 1);  // 20100723, only keep the last single '/0'
}

// IDC_BUTTON_SERIAL_PORT_SEND_COMM_DLG
// IDC_EDIT_REPLY_SERIAL_PORT_SEND_COMM_DLG

void CCommunicationDialog::OnBnClickedButtonSerialPortSendCommDlg()
{	
CString cstrSendWithCR;
char strSendTextWithCR[256];

	OnCbnKillfocusCommuDlgSendCombo();

	cFlagPauseThread = 1; Sleep(100);

	sprintf_s(strSendTextWithCR, 256, "%s\r", strCommuDlgComboSentCmd.GetString());
//	cstrSendWithCR = strCommuDlgComboSentCmd + '\r';
	long lRetWrite = cSerialPort.Write(strSendTextWithCR); // cstrSendWithCR.GetString());
	Sleep(100);

	if(SerialPortReadReply() == MTN_API_OK_ZERO)  // dwTimeOutSerialPort
	{
		pComboBoxCommunicationDlgSendCmd = ((CComboBox *) GetDlgItem(IDC_COMMU_DLG_SEND_COMBO_));
		int ind = pComboBoxCommunicationDlgSendCmd->FindStringExact(-1, strCommuDlgComboSentCmd);
		if (ind != CB_ERR) pComboBoxCommunicationDlgSendCmd->DeleteString(ind); // delete redundant history
		pComboBoxCommunicationDlgSendCmd->InsertString(0, strCommuDlgComboSentCmd);

		//pEditReplyTerminal = GetDlgItem(IDC_EDIT_REPLY_SERIAL_PORT_SEND_COMM_DLG);
		cstrEditBuffer.AppendFormat("%s\r\n", szBuffer);

		GetDlgItem(IDC_EDIT_REPLY_SERIAL_PORT_SEND_COMM_DLG)->SetWindowTextA(cstrEditBuffer);  // m_HistBuf, SetWindowText
		int nLines = ((CEdit*)GetDlgItem(IDC_EDIT_REPLY_SERIAL_PORT_SEND_COMM_DLG))->GetLineCount();
		((CEdit*)GetDlgItem(IDC_EDIT_REPLY_SERIAL_PORT_SEND_COMM_DLG))->LineScroll(nLines); 

	}
	cFlagPauseThread = 0;

}

// IDC_COMMU_DLG_SEND_COMBO_
void CCommunicationDialog::OnCbnKillfocusCommuDlgSendCombo()
{
	char strTemp[128];

	pComboBoxCommunicationDlgSendCmd = ((CComboBox *) GetDlgItem(IDC_COMMU_DLG_SEND_COMBO_));
	pComboBoxCommunicationDlgSendCmd->GetWindowTextA(strTemp, 128);
	strCommuDlgComboSentCmd.Format("%s", strTemp);

//	m_comboSend.GetWindowTextA(strTemp, 128); // m_comboSend.GetWindowTextLengthA()); //  m_comboSend.GetWindowText(m_strSentCommand);
	// every sent command should be concluded by '\r' character, 

	// Read reply from serial port, if has
}

static int nTotalNumLines = 0;
void CCommunicationDialog::OnBnClickedButtonCommudlgLoadScript()
{
    char buffer[BUFSIZ+1];

	CFileDialog cFileloadScript(TRUE, NULL, "*.txt",OFN_HIDEREADONLY,"Text   Files(*.*)|*.*||",NULL);   ;
	int   iResult   = (int)cFileloadScript.DoModal();
	if( iResult   !=   IDOK   )
	{
		return;   
	}   

	cstrFilePathLoadScript = cFileloadScript.GetPathName();

	FILE *fpScript = NULL;

	fopen_s(&fpScript, cstrFilePathLoadScript.GetString(), "r");
	nTotalNumLines = 0;
	if(fpScript == NULL)
	{
		AfxMessageBox(_T("打开读数据文件失败!"));
		GetDlgItem(IDC_BUTTON_COMMUDLG_RUN_SCRIPT)->EnableWindow(FALSE);
		return;
	}
	else
	{
		GetDlgItem(IDC_STATIC_COMMU_DLG_SCRIPT_FILEPATH)->SetWindowTextA(cstrFilePathLoadScript);
		cstrScriptLoadForRunning.Empty();
		while(feof(fpScript) == 0)
		{
			fgets(buffer, BUFSIZ, fpScript);
			cstrScriptLoadForRunning.AppendFormat("%s\r\n", buffer);
			nTotalNumLines ++;
		}

		GetDlgItem(IDC_EDIT_REPLY_SERIAL_PORT_SEND_COMM_DLG)->SetWindowTextA(cstrScriptLoadForRunning);
		fclose(fpScript);

		if(cSerialPort.IsOpen() || Handle != ACSC_INVALID) // 20121106 
		{
			GetDlgItem(IDC_BUTTON_COMMUDLG_RUN_SCRIPT)->EnableWindow(TRUE);
		}
	}
}

void CCommunicationDialog::RunScript_DriverRS232()
{
	char strCmdCode[BUFSIZ+2];
    char buffer[BUFSIZ+1];
	int nCmdLen, iCurrDownload;
	FILE *fpScript = NULL;

	CString cstrDownload;

	cFlagPauseThread = 1; Sleep(100); // 20121030

	fopen_s(&fpScript, cstrFilePathLoadScript.GetString(), "r");
	if(fpScript != NULL)
	{
		iCurrDownload = 0;
		while(feof(fpScript) == 0)
		{
			fgets(buffer, BUFSIZ, fpScript);
			sprintf_s(strCmdCode, BUFSIZ+1, "%s\r", buffer);	nCmdLen = (int)strlen(strCmdCode) - 1;
			cSerialPort.Write(strCmdCode); 	Sleep(10);	SerialPortReadReply();
			iCurrDownload ++; cstrDownload.Format("%s Script: %d/%d", pstrAppNameUsingSerPort[idxComboSelSerportForApp], iCurrDownload, nTotalNumLines); // 20121106

			GetDlgItem(IDC_STATIC_COMMU_DLG_SCRIPT_FILEPATH)->SetWindowTextA(cstrDownload);
		}

		fclose(fpScript);

	}
	cFlagPauseThread = 0; // 20121030
}

void CCommunicationDialog::RunScript_ACS_PCI()  // 20121106
{
	char strCmdCode[BUFSIZ+2];
    char buffer[BUFSIZ+1];
	int nCmdLen, iCurrDownload;
	FILE *fpScript = NULL;

	CString cstrDownload;

	cFlagPauseThread = 1; Sleep(100); // 20121030

	fopen_s(&fpScript, cstrFilePathLoadScript.GetString(), "r");
	if(fpScript != NULL)
	{
		iCurrDownload = 0;
		while(feof(fpScript) == 0)
		{
			fgets(buffer, BUFSIZ, fpScript);
			sprintf_s(strCmdCode, BUFSIZ+1, "%s\r", buffer);	nCmdLen = (int)strlen(strCmdCode);
//			cSerialPort.Write(strCmdCode); 	Sleep(10);	SerialPortReadReply();

			acsc_Send(Handle, strCmdCode, nCmdLen, NULL);
			Sleep(100);

			iCurrDownload ++; cstrDownload.Format("ACS Script: %d/%d", iCurrDownload, nTotalNumLines);
			GetDlgItem(IDC_STATIC_COMMU_DLG_SCRIPT_FILEPATH)->SetWindowTextA(cstrDownload);
		}

		fclose(fpScript);

	}
	cFlagPauseThread = 0; // 20121030
}

// IDC_BUTTON_COMMUDLG_RUN_SCRIPT
void CCommunicationDialog::OnBnClickedButtonCommudlgRunScript()
{
	if(cFlagConnectionSerialPort == TRUE) // 20121106
	{
		switch(idxComboSelSerportForApp)
		{
		case SERPORT_APP_IDX_LIGHT:
			// cSerialPort
			break;
		case SERPORT_APP_IDX_ELMO: // UpdateELMO_UI_byConnectFlag
		case SERPORT_APP_IDX_XENUS:
		case SERPORT_APP_IDX_SERVOTRONIX: // 
			RunScript_DriverRS232();
			break;
		case SERPORT_APP_IDX_ACS:
			break;
		}
	}
	else // Other channel's by ACS or etc. // 20121106
	{
		if(Handle != ACSC_INVALID)
		{
			RunScript_ACS_PCI();  // 20121106
		}
	}

}

void CCommunicationDialog::DlgUpdateDigitalInputStatusFilling(unsigned int uiStaticId, int iFlagLighting)
{
	CWnd *pWnd; // , *pWnd1;
	CDC *pDC; // , *pDC1;
	CRect Rect;//, Rect1;
	COLORREF liColourRGB;

	pWnd = GetDlgItem(uiStaticId);
	
	pDC = pWnd->GetDC();
	pWnd->GetClientRect(&Rect);
	if(iFlagLighting == 0)
	{
		liColourRGB = RGB(0, 127, 0);
	}
	else
	{
		liColourRGB = RGB(127, 0, 0);
	}
	pDC->FillSolidRect(Rect, liColourRGB);
	ReleaseDC(pDC);

}

int aiHallsReading[3];
// IDC_STATIC_COMMDLG_DRIVER_HALL_U
// IDC_STATIC_COMMDLG_DRIVER_HALL_V
// IDC_STATIC_COMMDLG_DRIVER_HALL_W
void CCommunicationDialog::UpdateHallSensorUI()
{
	CString cstrTemp;

	cstrTemp.Format("%d", aiHallsReading[0]);
	GetDlgItem(IDC_STATIC_COMMDLG_DRIVER_HALL_U)->SetWindowTextA(cstrTemp);
	DlgUpdateDigitalInputStatusFilling(IDC_STATIC_COMMDLG_DRIVER_HALL_U, aiHallsReading[0]);
	cstrTemp.Format("%d", aiHallsReading[1]);
	GetDlgItem(IDC_STATIC_COMMDLG_DRIVER_HALL_V)->SetWindowTextA(cstrTemp);
	DlgUpdateDigitalInputStatusFilling(IDC_STATIC_COMMDLG_DRIVER_HALL_V, aiHallsReading[1]);
	cstrTemp.Format("%d", aiHallsReading[2]);
	GetDlgItem(IDC_STATIC_COMMDLG_DRIVER_HALL_W)->SetWindowTextA(cstrTemp);
	DlgUpdateDigitalInputStatusFilling(IDC_STATIC_COMMDLG_DRIVER_HALL_W, aiHallsReading[2]);
}

void CCommunicationDialog::DlgTimerEvent()
{
	//////
	if(iFlagRunningTimer)
	{
	}
}

UINT DriverCommunicationThreadInDialog( LPVOID pParam )
{
    CCommunicationDialog* pObject = (CCommunicationDialog *)pParam;
	return pObject->DlgDriverCommunicationThread(); 	
}

void CCommunicationDialog::StartDriverCommunicationThread()
{
	cFlagDlgDriverCommunicationThreadRunning = TRUE;
	pDlgDriverCommunicationWinThread = AfxBeginThread(DriverCommunicationThreadInDialog, this, THREAD_PRIORITY_TIME_CRITICAL); 
	pDlgDriverCommunicationWinThread->m_bAutoDelete = FALSE;
}
UINT CCommunicationDialog::DlgDriverCommunicationThread()
{
	CString cstrTemp;
	char strCmdCode[256];
	char strScanFormat[512];
	int nCmdLen;
	int iCopleyHallRamInt;

	while(cFlagDlgDriverCommunicationThreadRunning)
	{
		if(cFlagPauseThread == 0)
		{
			// Update hall sensor
			if(cFlagConnectionSerialPort	) // cSerialPort.IsOpen()
			{
				switch(idxComboSelSerportForApp)
				{
					case SERPORT_APP_IDX_ELMO: // UpdateELMO_UI_byConnectFlag
						break;
					case SERPORT_APP_IDX_XENUS:
						sprintf_s(strCmdCode, 256, "g r0xA2\r");
						nCmdLen = (int)strlen(strCmdCode) - 1;  cSerialPort.Write(strCmdCode);
						Sleep(10);	SerialPortReadReply();
						sscanf_s(szBuffer, "v %d", &iCopleyHallRamInt);
						aiHallsReading[0] = iCopleyHallRamInt & 0x1;
						aiHallsReading[1] = iCopleyHallRamInt & 0x2;
						aiHallsReading[2] = iCopleyHallRamInt & 0x4;

						break;
					case SERPORT_APP_IDX_SERVOTRONIX: // 20121212
						sprintf_s(strCmdCode, 256, "halls\r");
						nCmdLen = (int)strlen(strCmdCode) - 1;
						cSerialPort.Write(strCmdCode);	Sleep(10);	SerialPortReadReply();
						//dServoTronxCurrentScale = strtod(&szBuffer[nCmdLen], &strStopString); strCmdCode
						sprintf_s(strScanFormat, 512, "halls\rHu Hv Hw<7A> %%d %%d %%d");
						//strstr(szBuffer, strScanFormat
						sscanf_s(&szBuffer[0], strScanFormat, &aiHallsReading[0], &aiHallsReading[1], &aiHallsReading[2]); // "%d %d %d"
	//					cstrTemp.Format("%4.2f", dServoTronxCurrentScale);
						break;
				}
			}
			UpdateHallSensorUI();
			Sleep(50);
		}
	}

	return 0;
}

// IDC_EDIT_COMMUNICATION_DLG_SRVTRONICS_MFBDIR
//void CCommunicationDialog::OnBnClickedCheckCommunicationDlgSrvtronicsMfbdir()
//{
//}

// IDC_CHECK_COMMUNICATION_DLG_SRVTRONICS_POSITION_DIR
void CCommunicationDialog::OnBnClickedCheckCommunicationDlgSrvtronicsPositionDir()
{
	iPositionDir = ((CButton *)GetDlgItem(IDC_CHECK_COMMUNICATION_DLG_SRVTRONICS_POSITION_DIR))->GetCheck();
	sprintf_s(szBufferSendCommand, 512, "DIR %d\r", iPositionDir);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100); 
	sprintf_s(szBufferSendCommand, 512, "CONFIG\r"); cSerialPort.Write(szBufferSendCommand);
}

// IDC_EDIT_COMMUNICATION_DLG_SRVTRONICS_MPHASE
void CCommunicationDialog::OnEnKillfocusEditCommunicationDlgSrvtronicsMphase()
{
	ReadDoubleFromEdit(IDC_EDIT_COMMUNICATION_DLG_SRVTRONICS_MPHASE, &dServoTronxCommutatePhase);
	sprintf_s(szBufferSendCommand, 512, "MPHASE %d\r", (int)dServoTronxCommutatePhase);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100); 
	sprintf_s(szBufferSendCommand, 512, "CONFIG\r"); cSerialPort.Write(szBufferSendCommand);
}

//////////////////////////////// Class functions
void CCommunicationDialog :: UI_InitSlider_Lighting124()
{
// IDC_SLIDER_LIGHTING_CH7_CoRED
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH7_CoRED))->SetRangeMax(255);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH7_CoRED))->SetRange(0, 255);
// IDC_SLIDER_LIGHTING_CH10_Co_BLUE
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH10_Co_BLUE))->SetRangeMax(255);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH10_Co_BLUE))->SetRange(0, 255);
// IDC_SLIDER_LIGHTING_CH9_SI_RED
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH9_SI_RED))->SetRangeMax(255);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH9_SI_RED))->SetRange(0, 255);
// IDC_SLIDER_LIGHTING_CH8_SI_BLUE
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH8_SI_BLUE))->SetRangeMax(255);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH8_SI_BLUE))->SetRange(0, 255);
}
void CCommunicationDialog :: UpdateCommunicationDlgUI_byConnectFlag()
{
	if(cFlagConnectionSerialPort == 0)
	{
		GetDlgItem(IDC_EDIT_LIGHTING_VALUE_CH)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMB_SEL_LIGHT_CHANNEL)->EnableWindow(FALSE);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH7_CoRED))->EnableWindow(FALSE);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH10_Co_BLUE))->EnableWindow(FALSE);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH9_SI_RED))->EnableWindow(FALSE);
		((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH8_SI_BLUE))->EnableWindow(FALSE);
		// Copley
		GetDlgItem(IDC_EDIT_COPLEY_I_SCALE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_COPLEY_LIMIT_I_PEAK)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_COPLEY_LIMIT_I_RMS)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_COPLEY_LIMIT_I2T)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_COPLEY_GAIN_C_P)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_COPLEY_GAIN_C_I)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_COPLEY_SAVE_FLASH)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_COPLY_READ_FLASH)->EnableWindow(FALSE);
		// Elmo
		GetDlgItem(IDC_EDIT_ELMO_I_SCALE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_ELMO_LIMIT_I_PEAK)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_ELMO_LIMIT_I_RMS)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_ELMO_CURRENT_GAIN_P)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_ELMO_CURRENT_GAIN_I)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_ELMO_LIMIT_PEAK_TIME)->EnableWindow(FALSE);
		// ServoTronix
		GetDlgItem(IDC_EDIT_SERVOTRONIX_I_SCALE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SERVOTRONIX_DRIVER_LIMIT_CURRENT_PEAK)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SERVOTRONIX_DRIVER_LIMIT_CURRENT_CONTI)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SERVOTRONIX_CURRENT_GAIN_P)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SERVOTRONIX_CURRENT_GAIN_I)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SERVOTRONIX_CURRENT_FF)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SERVOTRONIX_MOTOR_LIMIT_CURRENT_CONTI)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SERVOTRONIX_MOTOR_LIMIT_CURRENT_PEAK)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_SERVOTRONIX_CURRENT_K_BCAK_EMF)->EnableWindow(FALSE);
// 
		GetDlgItem(IDC_EDIT_COMMUNICATION_DLG_SRVTRONICS_MFBDIR)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_COMMUNICATION_DLG_SRVTRONICS_MPHASE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_COMMUNICATION_DLG_SRVTRONICS_POSITION_DIR)->EnableWindow(FALSE);  // 20121012

		// to be connected, IDC_BUTTUN_COMM_CONNECT_SERPORT
		if(get_sys_language_option() == LANGUAGE_UI_EN)
		{
			GetDlgItem(IDC_BUTTUN_COMM_CONNECT_SERPORT)->SetWindowTextA(_T("Connect"));
		}
		else
		{
			GetDlgItem(IDC_BUTTUN_COMM_CONNECT_SERPORT)->SetWindowTextA(_T("连接串口"));
		}			
	}
	else
	{
		switch(idxComboSelSerportForApp)
		{
		case SERPORT_APP_IDX_LIGHT:
			GetDlgItem(IDC_EDIT_LIGHTING_VALUE_CH)->EnableWindow(TRUE);
			GetDlgItem(IDC_COMB_SEL_LIGHT_CHANNEL)->EnableWindow(TRUE);
			((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH7_CoRED))->EnableWindow(TRUE);
			((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH10_Co_BLUE))->EnableWindow(TRUE);
			((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH9_SI_RED))->EnableWindow(TRUE);
			((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH8_SI_BLUE))->EnableWindow(TRUE);
			break;
		case SERPORT_APP_IDX_ELMO:
			GetDlgItem(IDC_EDIT_ELMO_I_SCALE)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_ELMO_LIMIT_I_PEAK)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_ELMO_LIMIT_I_RMS)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_ELMO_LIMIT_PEAK_TIME)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_ELMO_CURRENT_GAIN_P)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_ELMO_CURRENT_GAIN_I)->EnableWindow(TRUE);

			break;
		case SERPORT_APP_IDX_XENUS:
			GetDlgItem(IDC_EDIT_COPLEY_I_SCALE)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_COPLEY_LIMIT_I_PEAK)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_COPLEY_LIMIT_I_RMS)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_COPLEY_LIMIT_I2T)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_COPLEY_GAIN_C_P)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_COPLEY_GAIN_C_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_COPLEY_SAVE_FLASH)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_COPLY_READ_FLASH)->EnableWindow(TRUE);
			break;
		case SERPORT_APP_IDX_ACS:
			break;
		case SERPORT_APP_IDX_SERVOTRONIX:
			GetDlgItem(IDC_EDIT_SERVOTRONIX_I_SCALE)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_SERVOTRONIX_DRIVER_LIMIT_CURRENT_PEAK)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_SERVOTRONIX_DRIVER_LIMIT_CURRENT_CONTI)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_SERVOTRONIX_CURRENT_GAIN_P)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_SERVOTRONIX_CURRENT_GAIN_I)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_SERVOTRONIX_CURRENT_FF)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_SERVOTRONIX_MOTOR_LIMIT_CURRENT_CONTI)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_SERVOTRONIX_MOTOR_LIMIT_CURRENT_PEAK)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_SERVOTRONIX_CURRENT_K_BCAK_EMF)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_COMMUNICATION_DLG_SRVTRONICS_MFBDIR)->EnableWindow(TRUE);
			GetDlgItem(IDC_EDIT_COMMUNICATION_DLG_SRVTRONICS_MPHASE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_COMMUNICATION_DLG_SRVTRONICS_POSITION_DIR)->EnableWindow(TRUE);  // 20121012
			break;
		}

		// to be Dis-connected, IDC_BUTTUN_COMM_CONNECT_SERPORT
		if(get_sys_language_option() == LANGUAGE_UI_EN)
		{
			GetDlgItem(IDC_BUTTUN_COMM_CONNECT_SERPORT)->SetWindowTextA(_T("Disconnect"));
		}
		else
		{
			GetDlgItem(IDC_BUTTUN_COMM_CONNECT_SERPORT)->SetWindowTextA(_T("断开串口"));
		}
	}
	CString cstrTemp;
	cstrTemp.Format("%d", ucLightnessVal[ucIdxLightCh]);
	GetDlgItem(IDC_EDIT_LIGHTING_VALUE_CH)->SetWindowTextA(cstrTemp);
	// 
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH7_CoRED))->SetPos(ucLightnessVal[0]);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH10_Co_BLUE))->SetPos(ucLightnessVal[2]);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH9_SI_RED))->SetPos(ucLightnessVal[3]);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH8_SI_BLUE))->SetPos(ucLightnessVal[1]);

}
// IDC_EDIT_COMMUNICATION_DLG_SRVTRONICS_MFBDIR
void CCommunicationDialog::OnEnKillfocusEditCommunicationDlgSrvtronicsMfbdir()
{
	ReadUnsignedIntegerFromEdit(IDC_EDIT_COMMUNICATION_DLG_SRVTRONICS_MFBDIR, &iServoTronxMfbDir);
	sprintf_s(szBufferSendCommand, 512, "MFBDIR %d\r", iServoTronxMfbDir);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100); 
	sprintf_s(szBufferSendCommand, 512, "CONFIG\r"); cSerialPort.Write(szBufferSendCommand);
}
