
#include "stdafx.h"
#include "MtnDialog_FbMonitor.h"
#include "DigitalIO.h"  // 20081030
#include "efo_monitor.h"

/// To be deleted
#ifndef NO_IO_PCI_CARD 
#include "DigitalIO.h"  // 20081030
#include "efo_monitor.h"
EFO_Monitor theEFO_Monitor;
#endif

// extern EFO_Monitor theEFO_Monitor;

//// EFO trigger and monitoring only by 1739U
#include "EfoMonitor1739U.h"
extern EFO_MONITOR_1739U mEfoMonitor1739U;
extern unsigned short usDigitalIO_PortTrigEFO_Byte;   // 20091121

//// General variables for 1739U
extern unsigned short usDigitalIO_PortInputBytePort, usDigitalIO_PortOutputBytePort;
extern unsigned short usDigitalIO1739_ReadByteValue, usDigitalIO1739_ReadByteOutputValue;
extern unsigned short usDigitalIO1739_WriteByteReg;

unsigned int nMaxWB_WorkHolder_13V_IO_BondTrack = 12;

char *astrWB_WorkHolder_13V_BondTrackNameLabel_en[] =  {
"OffloaderPusherValve",
"OffloaderClawValve",
"OnloadPickerLF_DetectSensor",
"OnloadTrackSensor",
"PreBondTrackSensor",
"OffloadTrackSensor",
"OffloadPusherFrontSensor",
"OffloadPusherRearSensor",
"BondCupDetectSensor",
"DoubleLFCheckSensor",
"OnloadForkExistSensor",
"OffloadForkExistSensor"
};

char *astrWB_WorkHolder_13V_BondTrackNameLabel_cn[] =  {
"下料推片气阀",
"下料抓片气阀",
"上料取片支架检测感应器",
"上料轨道感应器",
"焊前轨道感应器",
"下料轨道感应器",
"下料推片气缸前部感应器",
"下料推片气缸后部感应器",
"焊线位置有杯检测感应器",
"双片检测感应器",
"上料叉存在感应器",
"下料叉存在感应器"
};

char *astrWB_WorkHolder_13V_BondTrackCardAddress[] = {
"OutByte3-bit4_Bd(1),(ID=0)",
"OutByte3-bit5_Bd(1),(ID=0)",
"InByte4-bit0_Bd(1),(ID=0)",
"InByte4-bit1_Bd(1),(ID=0)",
"InByte4-bit2_Bd(1),(ID=0)",
"InByte4-bit3_Bd(1),(ID=0)",
"InByte4-bit6_Bd(1),(ID=0)",
"InByte4-bit7_Bd(1),(ID=0)",
"InByte5-bit1_Bd(1),(ID=0)",
"InByte5-bit4_Bd(1),(ID=0)",
"InByte5-bit5_Bd(1),(ID=0)",
"InByte5-bit6_Bd(1),(ID=0)"
};

unsigned int nMaxWB_WorkHolder_13V_IO_Others = 22;

char *astrWB_WorkHolder_13V_OthersNameLabel_en[] =  {
"HEATER_RELAY_STATE",
"EFO_SPARK_BUTTON",
"WIRE_CLAMP_BUTTON",
"JOYSTICK_X_POSITIVE",
"JOYSTICK_X_NEGATIVE",
"JOYSTICK_Y_POSITIVE",
"JOYSTICK_Y_NEGATIVE",
"EFO_GoodNGFeedback",
"TOWER_LIGHT_GREEN",
"TOWER_LIGHT_RED",
"TOWER_LIGHT_YELLOW",
"Beep (Red Alarm)",
"WireEndSensor",
"UPS_PowerSupplyState",
"USG-CmdByte-b0",
"USG-CmdByte-b1",
"USG-CmdByte-b2",
"USG-CmdByte-b3",
"USG-CmdByte-b4",
"USG-CmdByte-b5",
"USG-CmdByte-b6",
"USG-CmdByte-b7"
};

char *astrWB_WorkHolder_13V_OthersNameLabel_cn[] =  {
"温度状态",
"电打火按钮",
"夹线按钮",
"摇杆",
"摇杆",
"摇杆",
"摇杆",
"电打火烧球反馈",
"三色灯之绿色",
"三色灯之红色",
"三色灯之黄色",
"蜂鸣器长鸣（保留，红灯同步）",
"焊线终结传感器",
"不间断电源供电状态",
"超声命令字节 (0-255)-b0",
"超声命令字节 (0-255)-b1",
"超声命令字节 (0-255)-b2",
"超声命令字节 (0-255)-b3",
"超声命令字节 (0-255)-b4",
"超声命令字节 (0-255)-b5",
"超声命令字节 (0-255)-b6",
"超声命令字节 (0-255)-b7"
};
char *astrWB_WorkHolder_13V_OthersCardAddress[] = {
"InByte2-bit-0_Bd(1),(ID=0)",
"InByte2-bit-1_Bd(1),(ID=0)",
"InByte2-bit-2_Bd(1),(ID=0)",
"InByte2-bit-3_Bd(1),(ID=0)",
"InByte2-bit-4_Bd(1),(ID=0)",
"InByte2-bit-5_Bd(1),(ID=0)",
"InByte2-bit-6_Bd(1),(ID=0)",
"InByte2-bit-7_Bd(1),(ID=0)",
"OutByte3-bit-0_Bd(1),(ID=0)",
"OutByte3-bit-1_Bd(1),(ID=0)",
"OutByte3-bit-2_Bd(1),(ID=0)",
"OutByte3-bit-6_Bd(1),(ID=0)",
"InByte5-bit-0_Bd(1),(ID=0)",
"InByte5-bit-7_Bd(1),(ID=0)",
"OutByt0-bit-0_Bd(1),(ID=0)",
"OutByt0-bit-1_Bd(1),(ID=0)",
"OutByt0-bit-2_Bd(1),(ID=0)",
"OutByt0-bit-3_Bd(1),(ID=0)",
"OutByt0-bit-4_Bd(1),(ID=0)",
"OutByt0-bit-5_Bd(1),(ID=0)",
"OutByt0-bit-6_Bd(1),(ID=0)",
"OutByt0-bit-7_Bd(1),(ID=0)"
};

////// 20120823
int iFlagUsingIO1739U = 0;

#include "_Digital_IO_Ad7248.h"
extern I16 sCardId_7248[__MAX_CARD_NUMBER_7248];
extern U16 uiCard_number_7248;
extern U32 inputA[2], inputB[2], inputC[2] , inputD[2];
extern U32 outputA[2],outputB[2],outputC[2] ,outputD[2];

extern U16 uiComboSelectionInputCard7248, uiComboSelectionOutputCard7248;
extern U32 ulDigitalIO7248_ReadByteInPortValue, ulDigitalIO7248_ReadByteOutPortValue;

////// Vertical LED, ForkWH
#include "_IO_VertiLED_DualClamperFork.c"

// Horizontal LED application, 
#include "_IO_HoriLED_01.c"

// Horizontal LED application, 
#include "_IO_HoriLED_02.c"

extern int iFlagMachineType;

static int iFlagHasInitialize1739IO = 0;
static int iFlagHasInitialize7248 = 0;
void MtnDialog_FbMonitor::DialogInitDigitalIO_1739U()
{
	if(iFlagHasInitialize1739IO == TRUE)
	{
		return;
	}
	iFlagHasInitialize1739IO = TRUE;
	// Digital IO-1739, card selection
	if(theDigitalIO.m_IoCardInfo.sNumOfDevices >= 1)
	{
		iFlagUsingIO1739U = 1;
	}
	CComboBox *pDigitalIO1739UCombo = (CComboBox*) GetDlgItem(IDC_COMBO_DIGITAL_INPUT_CARD_LIST);
	for(int ii=0; ii< theDigitalIO.m_IoCardInfo.sNumOfDevices; ii++)
	{
		pDigitalIO1739UCombo->InsertString(ii, theDigitalIO.m_IoCardInfo.stDeviceList[ii].szDeviceName);
	}
	//// 20110726
	int idxSelDevCombo = 0xFFFF;
	if(theDigitalIO.m_IoCardInfo.sNumOfDevices >= 1)
	{
		idxSelDevCombo = 0;
		pDigitalIO1739UCombo->SetCurSel(idxSelDevCombo);
		if(lDrvHandleDigitalInput_IO1739U == NULL)
		{
		}
		else
		{
			DRV_DeviceClose((LONG far *)&lDrvHandleDigitalInput_IO1739U);
		}
		DRV_DeviceOpen(theDigitalIO.m_IoCardInfo.stDeviceList[idxSelDevCombo].dwDeviceNum,
									(LONG far *)&lDrvHandleDigitalInput_IO1739U);
	}  //// 20110726

	// Card for Digital Output, IDC_COMBO_DIGITAL_OUTPUT_CARD_LIST
	CComboBox *pDigitalIO1739UComboOutputCard = (CComboBox*) GetDlgItem(IDC_COMBO_DIGITAL_OUTPUT_CARD_LIST);
	for(int ii=0; ii< theDigitalIO.m_IoCardInfo.sNumOfDevices; ii++)
	{
		pDigitalIO1739UComboOutputCard->InsertString(ii, theDigitalIO.m_IoCardInfo.stDeviceList[ii].szDeviceName);
	}
	//// 20110726
	idxSelDevCombo = 0xFFFF;
	if(theDigitalIO.m_IoCardInfo.sNumOfDevices >= 1)
	{
		idxSelDevCombo = 0;
		pDigitalIO1739UComboOutputCard->SetCurSel(idxSelDevCombo);
		if(lDrvHandleDigitalOutput_IO1739U == NULL)
		{
		}
		else
		{
			DRV_DeviceClose((LONG far *)&lDrvHandleDigitalOutput_IO1739U);
		}
		DRV_DeviceOpen(theDigitalIO.m_IoCardInfo.stDeviceList[idxSelDevCombo].dwDeviceNum,
									(LONG far *)&lDrvHandleDigitalOutput_IO1739U);
	}  //// 20110726

	// Card for EFO_Monitor, // 20090824
	CComboBox *pDigitalIO1739UComboMonitorEFOCard = (CComboBox*) GetDlgItem(IDC_COMBO_EFO_MONITOR_USE_1739U_CARD);
	for(int ii=0; ii< theDigitalIO.m_IoCardInfo.sNumOfDevices; ii++)
	{
		pDigitalIO1739UComboMonitorEFOCard->InsertString(ii, theDigitalIO.m_IoCardInfo.stDeviceList[ii].szDeviceName);
	}

	//// 20110726
	idxSelDevCombo = 0xFFFF;
	if(theDigitalIO.m_IoCardInfo.sNumOfDevices >= 1)
	{
		idxSelDevCombo = 0;
		pDigitalIO1739UComboMonitorEFOCard->SetCurSel(idxSelDevCombo);
		if(lDrvHandleEFO_Monitor_1739U == NULL)
		{
		}
		else
		{
			DRV_DeviceClose((LONG far *)&lDrvHandleEFO_Monitor_1739U);
		}
		DRV_DeviceOpen(theDigitalIO.m_IoCardInfo.stDeviceList[idxSelDevCombo].dwDeviceNum,
									(LONG far *)&lDrvHandleEFO_Monitor_1739U);
	}  //// 20110726

	// Byte for EFO_Trigger, 	IDC_COMBO_EFO_TRIG_USE_1739U_BYTE, 20091121
	CComboBox *pDigitalIO1739UComboTrigEFO_Byte = (CComboBox*) GetDlgItem(IDC_COMBO_EFO_TRIG_USE_1739U_BYTE);
	char strTemp[32];
	for(int ii=0; ii<6; ii++)
	{
		// cstrTemp.format(
		sprintf_s(strTemp, 32, "TrigByte-%d", ii);
		pDigitalIO1739UComboTrigEFO_Byte->InsertString(ii, strTemp);
	}
	usDigitalIO_PortTrigEFO_Byte = EFO_MONITOR_1739U_OUT_BIT_CHANNEL;  // 20110726
	pDigitalIO1739UComboTrigEFO_Byte->SetCurSel((int)usDigitalIO_PortTrigEFO_Byte);

	// input byte
	CComboBox *pDigitalIO1739UComboInput = (CComboBox*) GetDlgItem(IDC_COMBO_DIGITAL_IO_SELECT_INPUT_BYTE);
	for(int ii=0; ii<6; ii++)
	{
		// cstrTemp.format(
		sprintf_s(strTemp, 32, "InByte-%d", ii);
		pDigitalIO1739UComboInput->InsertString(ii, strTemp);
	}
	
	pDigitalIO1739UComboInput->SetCurSel((int)usDigitalIO_PortInputBytePort);

	CComboBox *pDigitalIO1739UComboOutput = (CComboBox*) GetDlgItem(IDC_COMBO_DIGITAL_IO_SELECT_OUTPUT_BYTE);
	for(int ii=0; ii<6; ii++)
	{
		// cstrTemp.format(
		sprintf_s(strTemp, 32, "OutByte-%d", ii);
		pDigitalIO1739UComboOutput->InsertString(ii, strTemp);
	}
//	usDigitalIO_PortOutputBytePort = 0; 
	pDigitalIO1739UComboOutput->SetCurSel((int)usDigitalIO_PortOutputBytePort);

}

void MtnDialog_FbMonitor::DialogInitDigitalIO_7248()
{
	if(iFlagHasInitialize7248 == TRUE)
	{
		return;
	}
	iFlagHasInitialize7248 = TRUE;

	// 
	DigitalIO_7248_Registration();

	CString cstrTemp;
	unsigned int ii;
	unsigned int uiTotalNumCards7248 = DIO7248_sys_get_total_num_cards();
	// Digital IO-1739, card selection
	CComboBox *pComboDigitalIO = (CComboBox*) GetDlgItem(IDC_COMBO_DIGITAL_INPUT_CARD_LIST);
	for(ii=0; ii< uiTotalNumCards7248; ii++)
	{
		cstrTemp.Format("Input from 7248-%d", ii);
		pComboDigitalIO->InsertString(ii, cstrTemp);
	}
	int idxSelDevCombo = 0xFFFF;
	if(uiTotalNumCards7248 >= 1)
	{
		idxSelDevCombo = 0;
		pComboDigitalIO->SetCurSel(idxSelDevCombo);
		uiComboSelectionInputCard7248 = sCardId_7248[idxSelDevCombo];
	}  

	// Card for Digital Output, IDC_COMBO_DIGITAL_OUTPUT_CARD_LIST
	pComboDigitalIO = (CComboBox*) GetDlgItem(IDC_COMBO_DIGITAL_OUTPUT_CARD_LIST);
	for(unsigned int ii=0; ii< uiTotalNumCards7248; ii++)
	{
		cstrTemp.Format("Output to 7248-%d", ii);
		pComboDigitalIO->InsertString(ii, cstrTemp);
	}
	idxSelDevCombo = 0xFFFF;
	if(uiTotalNumCards7248 >= 1)
	{
		idxSelDevCombo = 0;
		pComboDigitalIO->SetCurSel(idxSelDevCombo);
		uiComboSelectionOutputCard7248 = sCardId_7248[idxSelDevCombo];
	} 
}
void MtnDialog_FbMonitor::MonitorUpdateDigitalInputStatusFilling(unsigned int uiStaticId, int iFlagLighting)
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

void MtnDialog_FbMonitor::UpdateInputStaticFrom1739IO()
{
	// IDC_STATIC_DIGITAL_IO_INPUT_BIT_7
	if(usDigitalIO1739_ReadByteValue & 0x80)
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_7)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_7)->SetWindowTextA(_T("0"));
	}
	MonitorUpdateDigitalInputStatusFilling(IDC_STATIC_DIGITAL_IO_INPUT_BIT_7, usDigitalIO1739_ReadByteValue & 0x80);
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_6
	if(usDigitalIO1739_ReadByteValue & 0x40)
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_6)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_6)->SetWindowTextA(_T("0"));
	}
	MonitorUpdateDigitalInputStatusFilling(IDC_STATIC_DIGITAL_IO_INPUT_BIT_6, usDigitalIO1739_ReadByteValue & 0x40);
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_5
	if(usDigitalIO1739_ReadByteValue & 0x20)
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_5)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_5)->SetWindowTextA(_T("0"));
	}
	MonitorUpdateDigitalInputStatusFilling(IDC_STATIC_DIGITAL_IO_INPUT_BIT_5, usDigitalIO1739_ReadByteValue & 0x20);
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_4
	if(usDigitalIO1739_ReadByteValue & 0x10)
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_4)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_4)->SetWindowTextA(_T("0"));
	}
	MonitorUpdateDigitalInputStatusFilling(IDC_STATIC_DIGITAL_IO_INPUT_BIT_4, usDigitalIO1739_ReadByteValue & 0x10);
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_3
	if(usDigitalIO1739_ReadByteValue & 0x8)
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_3)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_3)->SetWindowTextA(_T("0"));
	}
	MonitorUpdateDigitalInputStatusFilling(IDC_STATIC_DIGITAL_IO_INPUT_BIT_3, usDigitalIO1739_ReadByteValue & 0x8);
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_2
	if(usDigitalIO1739_ReadByteValue & 0x4)
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_2)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_2)->SetWindowTextA(_T("0"));
	}
	MonitorUpdateDigitalInputStatusFilling(IDC_STATIC_DIGITAL_IO_INPUT_BIT_2, usDigitalIO1739_ReadByteValue & 0x4);
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_1
	if(usDigitalIO1739_ReadByteValue & 0x2)
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_1)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_1)->SetWindowTextA(_T("0"));
	}
	MonitorUpdateDigitalInputStatusFilling(IDC_STATIC_DIGITAL_IO_INPUT_BIT_1, usDigitalIO1739_ReadByteValue & 0x2);
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_0
	if(usDigitalIO1739_ReadByteValue & 0x1)
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_0)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_0)->SetWindowTextA(_T("0"));
	}
	MonitorUpdateDigitalInputStatusFilling(IDC_STATIC_DIGITAL_IO_INPUT_BIT_0, usDigitalIO1739_ReadByteValue & 0x1);
	//// Digital Output Register Status
	// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_7
	if(usDigitalIO1739_ReadByteOutputValue & 0x80)
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_7)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_7)->SetWindowTextA(_T("0"));
	}
	// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_6
	if(usDigitalIO1739_ReadByteOutputValue & 0x40)
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_6)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_6)->SetWindowTextA(_T("0"));
	}
	// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_5
	if(usDigitalIO1739_ReadByteOutputValue & 0x20)
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_5)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_5)->SetWindowTextA(_T("0"));
	}
	// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_4
	if(usDigitalIO1739_ReadByteOutputValue & 0x10)
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_4)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_4)->SetWindowTextA(_T("0"));
	}
	// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_3
	if(usDigitalIO1739_ReadByteOutputValue & 0x8)
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_3)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_3)->SetWindowTextA(_T("0"));
	}
	// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_2
	if(usDigitalIO1739_ReadByteOutputValue & 0x4)
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_2)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_2)->SetWindowTextA(_T("0"));
	}
	// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_1
	if(usDigitalIO1739_ReadByteOutputValue & 0x2)
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_1)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_1)->SetWindowTextA(_T("0"));
	}
	// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_0
	if(usDigitalIO1739_ReadByteOutputValue & 0x1)
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_0)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_0)->SetWindowTextA(_T("0"));
	}
}

/////////////////// Adding Digital IO 7248
void MtnDialog_FbMonitor::UpdateInputStaticFromAD7248IO()
{
	// IDC_STATIC_DIGITAL_IO_INPUT_BIT_7
	if(ulDigitalIO7248_ReadByteInPortValue & 0x80)
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_7)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_7)->SetWindowTextA(_T("0"));
	}
	MonitorUpdateDigitalInputStatusFilling(IDC_STATIC_DIGITAL_IO_INPUT_BIT_7, ulDigitalIO7248_ReadByteInPortValue & 0x80);
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_6
	if(ulDigitalIO7248_ReadByteInPortValue & 0x40)
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_6)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_6)->SetWindowTextA(_T("0"));
	}
	MonitorUpdateDigitalInputStatusFilling(IDC_STATIC_DIGITAL_IO_INPUT_BIT_6, ulDigitalIO7248_ReadByteInPortValue & 0x40);
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_5
	if(ulDigitalIO7248_ReadByteInPortValue & 0x20)
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_5)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_5)->SetWindowTextA(_T("0"));
	}
	MonitorUpdateDigitalInputStatusFilling(IDC_STATIC_DIGITAL_IO_INPUT_BIT_5, ulDigitalIO7248_ReadByteInPortValue & 0x20);
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_4
	if(ulDigitalIO7248_ReadByteInPortValue & 0x10)
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_4)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_4)->SetWindowTextA(_T("0"));
	}
	MonitorUpdateDigitalInputStatusFilling(IDC_STATIC_DIGITAL_IO_INPUT_BIT_4, ulDigitalIO7248_ReadByteInPortValue & 0x10);
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_3
	if(ulDigitalIO7248_ReadByteInPortValue & 0x8)
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_3)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_3)->SetWindowTextA(_T("0"));
	}
	MonitorUpdateDigitalInputStatusFilling(IDC_STATIC_DIGITAL_IO_INPUT_BIT_3, ulDigitalIO7248_ReadByteInPortValue & 0x8);
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_2
	if(ulDigitalIO7248_ReadByteInPortValue & 0x4)
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_2)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_2)->SetWindowTextA(_T("0"));
	}
	MonitorUpdateDigitalInputStatusFilling(IDC_STATIC_DIGITAL_IO_INPUT_BIT_2, ulDigitalIO7248_ReadByteInPortValue & 0x4);
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_1
	if(ulDigitalIO7248_ReadByteInPortValue & 0x2)
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_1)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_1)->SetWindowTextA(_T("0"));
	}
	MonitorUpdateDigitalInputStatusFilling(IDC_STATIC_DIGITAL_IO_INPUT_BIT_1, ulDigitalIO7248_ReadByteInPortValue & 0x2);
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_0
	if(ulDigitalIO7248_ReadByteInPortValue & 0x1)
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_0)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_STATIC_DIGITAL_IO_INPUT_BIT_DIG_0)->SetWindowTextA(_T("0"));
	}
	MonitorUpdateDigitalInputStatusFilling(IDC_STATIC_DIGITAL_IO_INPUT_BIT_0, ulDigitalIO7248_ReadByteInPortValue & 0x1);
	//// Digital Output Register Status
	// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_7
	if(ulDigitalIO7248_ReadByteOutPortValue & 0x80)
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_7)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_7)->SetWindowTextA(_T("0"));
	}
	// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_6
	if(ulDigitalIO7248_ReadByteOutPortValue & 0x40)
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_6)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_6)->SetWindowTextA(_T("0"));
	}
	// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_5
	if(ulDigitalIO7248_ReadByteOutPortValue & 0x20)
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_5)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_5)->SetWindowTextA(_T("0"));
	}
	// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_4
	if(ulDigitalIO7248_ReadByteOutPortValue & 0x10)
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_4)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_4)->SetWindowTextA(_T("0"));
	}
	// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_3
	if(ulDigitalIO7248_ReadByteOutPortValue & 0x8)
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_3)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_3)->SetWindowTextA(_T("0"));
	}
	// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_2
	if(ulDigitalIO7248_ReadByteOutPortValue & 0x4)
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_2)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_2)->SetWindowTextA(_T("0"));
	}
	// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_1
	if(ulDigitalIO7248_ReadByteOutPortValue & 0x2)
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_1)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_1)->SetWindowTextA(_T("0"));
	}
	// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_0
	if(ulDigitalIO7248_ReadByteOutPortValue & 0x1)
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_0)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_0)->SetWindowTextA(_T("0"));
	}
}
void MtnDialog_FbMonitor::MonitorUpdateDigitalIO7248CurrByte()
{
// Read from InputPort
	if(uiComboSelectionInputCard7248 != -1)
	{
		if(usDigitalIO_PortInputBytePort <= 2)
		{
			DO_ReadPort(uiComboSelectionInputCard7248, usDigitalIO_PortInputBytePort, &ulDigitalIO7248_ReadByteInPortValue); 
		}
		else if(usDigitalIO_PortInputBytePort == 3)
		{
			DO_ReadPort(uiComboSelectionInputCard7248, Channel_P2A, &ulDigitalIO7248_ReadByteInPortValue); 			
		}
		else if(usDigitalIO_PortInputBytePort == 4)
		{
			DO_ReadPort(uiComboSelectionInputCard7248, Channel_P2B, &ulDigitalIO7248_ReadByteInPortValue); 
		}
		else if(usDigitalIO_PortInputBytePort == 5)
		{
			DO_ReadPort(uiComboSelectionInputCard7248, Channel_P2C, &ulDigitalIO7248_ReadByteInPortValue); 
		}
	}
// Read from OutputPort
	if(uiComboSelectionOutputCard7248 != -1)
	{
		if(usDigitalIO_PortOutputBytePort <= 2)
		{
			DO_ReadPort(uiComboSelectionOutputCard7248, usDigitalIO_PortOutputBytePort, &ulDigitalIO7248_ReadByteOutPortValue); 
		}
		else if(usDigitalIO_PortOutputBytePort == 3)
		{
			DO_ReadPort(uiComboSelectionOutputCard7248, Channel_P2A, &ulDigitalIO7248_ReadByteOutPortValue); 
		}
		else if(usDigitalIO_PortOutputBytePort == 4)
		{
			DO_ReadPort(uiComboSelectionOutputCard7248, Channel_P2B, &ulDigitalIO7248_ReadByteOutPortValue); 
		}
		else if(usDigitalIO_PortOutputBytePort == 5)
		{
			DO_ReadPort(uiComboSelectionOutputCard7248, Channel_P2C, &ulDigitalIO7248_ReadByteOutPortValue); 
		}
	}
}

void MtnDialog_FbMonitor::MonitorUpdateDigitalInputStatic()
{
	if(iFlagUsingIO1739U)
	{
		MonitorUpdateDigitalIO1739Status();
		UpdateInputStaticFrom1739IO();
	}
	else if(DIO7248_sys_is_using_cards())
	{
		MonitorUpdateDigitalIO7248CurrByte();
		UpdateInputStaticFromAD7248IO();
	}

}

void MtnDialog_FbMonitor::MonitorUpdateDigitalIO1739Status()
{
	static      PT_DioReadPortByte ptDioReadPortByte;
	ptDioReadPortByte.port = usDigitalIO_PortInputBytePort;
	ptDioReadPortByte.value = (USHORT far *)&usDigitalIO1739_ReadByteValue;

	if(lDrvHandleDigitalInput_IO1739U == NULL)
	{
	}
	else
	{
		DRV_DioReadPortByte(lDrvHandleDigitalInput_IO1739U,
                (LPT_DioReadPortByte)&ptDioReadPortByte);
	}

	static      PT_DioReadPortByte ptDioReadPortByteOutput;
	ptDioReadPortByteOutput.port = usDigitalIO_PortOutputBytePort;
	ptDioReadPortByteOutput.value = (USHORT far *)&usDigitalIO1739_ReadByteOutputValue;
	if(lDrvHandleDigitalOutput_IO1739U == NULL)
	{
	}
	else
	{
		DRV_DioReadPortByte(lDrvHandleDigitalOutput_IO1739U,
                (LPT_DioReadPortByte)&ptDioReadPortByteOutput);
	}
//	MonitorUpdateDigitalInputStatic(); 20120823
}

void MtnDialog_FbMonitor::DigitalIO7248_ToggleOutputBit(unsigned short usMask)
{
	if(uiComboSelectionOutputCard7248 != -1)
	{
		if(usDigitalIO_PortOutputBytePort <= 2)
		{
			DO_ReadPort(uiComboSelectionOutputCard7248, usDigitalIO_PortOutputBytePort, &ulDigitalIO7248_ReadByteOutPortValue); 
		}
		else if(usDigitalIO_PortOutputBytePort == 3)
		{
			DO_ReadPort(uiComboSelectionOutputCard7248, Channel_P2A, &ulDigitalIO7248_ReadByteOutPortValue); 
		}
		else if(usDigitalIO_PortOutputBytePort == 4)
		{
			DO_ReadPort(uiComboSelectionOutputCard7248, Channel_P2B, &ulDigitalIO7248_ReadByteOutPortValue); 
		}
		else if(usDigitalIO_PortOutputBytePort == 5)
		{
			DO_ReadPort(uiComboSelectionOutputCard7248, Channel_P2C, &ulDigitalIO7248_ReadByteOutPortValue); 
		}

		U32 ulDigitalIO7248_WriteByte; 
		ulDigitalIO7248_WriteByte = 
			((UINT_MAX - ulDigitalIO7248_ReadByteOutPortValue) & usMask);
		if(ulDigitalIO7248_WriteByte)
		{
			ulDigitalIO7248_WriteByte = ulDigitalIO7248_WriteByte | ulDigitalIO7248_ReadByteOutPortValue;
		}
		else
		{
			ulDigitalIO7248_WriteByte = ulDigitalIO7248_ReadByteOutPortValue - usMask;
		}
			
		if(usDigitalIO_PortOutputBytePort <=2)
		{
			DO_WritePort(uiComboSelectionOutputCard7248, usDigitalIO_PortOutputBytePort, ulDigitalIO7248_WriteByte);
		}
		else if(usDigitalIO_PortOutputBytePort == 3)
		{
			DO_WritePort(uiComboSelectionOutputCard7248, Channel_P2A, ulDigitalIO7248_WriteByte);
		}
		else if(usDigitalIO_PortOutputBytePort == 4)
		{
			DO_WritePort(uiComboSelectionOutputCard7248, Channel_P2B, ulDigitalIO7248_WriteByte);
		}
		else if(usDigitalIO_PortOutputBytePort == 5)
		{
			DO_WritePort(uiComboSelectionOutputCard7248, Channel_P2B, ulDigitalIO7248_WriteByte);
		}
	}
}

void MtnDialog_FbMonitor::DigitalIO_ToggleOutputBit(unsigned short usMask)
{
	if(iFlagUsingIO1739U)
	{
		DigitalIO1739_ToggleOutputBit(usMask);
	}
	else if(DIO7248_sys_is_using_cards())
	{
		DigitalIO7248_ToggleOutputBit(usMask);
	}
}
////// 20120823
///////////////////////////
void MtnDialog_FbMonitor::DigitalIO1739_TrigEFO_Byte()
{
	if(lDrvHandleEFO_Monitor_1739U == NULL)
	{
	}
	else
	{
		static PT_DioWritePortByte ptDioWritePortByteOutTrigEFO;
		ptDioWritePortByteOutTrigEFO.port = usDigitalIO_PortTrigEFO_Byte;
		ptDioWritePortByteOutTrigEFO.state = 0;
		ptDioWritePortByteOutTrigEFO.mask = 0xFF;
		DRV_DioWritePortByte(lDrvHandleEFO_Monitor_1739U, &ptDioWritePortByteOutTrigEFO);
	}

}

void MtnDialog_FbMonitor::DigitalIO1739_ResetTrigEFO_Byte()
{
	if(lDrvHandleEFO_Monitor_1739U == NULL)
	{
	}
	else
	{
		static PT_DioWritePortByte ptDioWritePortByteOutTrigEFO;
		ptDioWritePortByteOutTrigEFO.port = usDigitalIO_PortTrigEFO_Byte;
		ptDioWritePortByteOutTrigEFO.state = 0xFF;
		ptDioWritePortByteOutTrigEFO.mask = 0xFF;
		DRV_DioWritePortByte(lDrvHandleEFO_Monitor_1739U, &ptDioWritePortByteOutTrigEFO);
	}
}

void MtnDialog_FbMonitor::DigitalIO1739_ToggleOutputBit(unsigned short usMask)
{
	static      PT_DioReadPortByte ptDioWritePortByteOutput;
	ptDioWritePortByteOutput.port = usDigitalIO_PortOutputBytePort;
	ptDioWritePortByteOutput.value = (USHORT far *)&usDigitalIO1739_WriteByteReg;
	if(lDrvHandleDigitalOutput_IO1739U == NULL)
	{
	}
	else
	{
		DRV_DioReadPortByte(lDrvHandleDigitalOutput_IO1739U,
                (LPT_DioReadPortByte)&ptDioWritePortByteOutput);
		static PT_DioWritePortByte ptDioWritePortByte;
		ptDioWritePortByte.port = usDigitalIO_PortOutputBytePort;
		ptDioWritePortByte.state = USHRT_MAX - usDigitalIO1739_WriteByteReg;
		ptDioWritePortByte.mask = usMask;
		DRV_DioWritePortByte(lDrvHandleDigitalOutput_IO1739U, &ptDioWritePortByte);
	}

}

// If lDrvHandleDigitalInput_IO1739U is not NULL (valid device)
// close current device
// open new device, update lDrvHandleDigitalInput_IO1739U
void MtnDialog_FbMonitor::OnCbnSelchangeComboDigitalIo1739List()
{
// IDC_COMBO_DIGITAL_INPUT_CARD_LIST
	CComboBox *pDigitalIO1739UCombo = (CComboBox*) GetDlgItem(IDC_COMBO_DIGITAL_INPUT_CARD_LIST);
	int idxDev = pDigitalIO1739UCombo->GetCurSel();

	if(lDrvHandleDigitalInput_IO1739U == NULL)
	{
	}
	else
	{
		DRV_DeviceClose((LONG far *)&lDrvHandleDigitalInput_IO1739U);
	}
	DRV_DeviceOpen(theDigitalIO.m_IoCardInfo.stDeviceList[idxDev].dwDeviceNum,
                                (LONG far *)&lDrvHandleDigitalInput_IO1739U);
}


void MtnDialog_FbMonitor::OnCbnSelchangeComboDigitalIoSelectInputByte()
{
// IDC_COMBO_DIGITAL_IO_SELECT_INPUT_BYTE
	CComboBox *pDigitalIO1739UComboInput = (CComboBox*) GetDlgItem(IDC_COMBO_DIGITAL_IO_SELECT_INPUT_BYTE);
	usDigitalIO_PortInputBytePort = (unsigned short)pDigitalIO1739UComboInput->GetCurSel();
}

void MtnDialog_FbMonitor::OnCbnSelchangeComboDigitalIoSelectOutputByte()
{
// IDC_COMBO_DIGITAL_IO_SELECT_OUTPUT_BYTE
	CComboBox *pDigitalIO1739UComboOutput = (CComboBox*) GetDlgItem(IDC_COMBO_DIGITAL_IO_SELECT_OUTPUT_BYTE);
	usDigitalIO_PortOutputBytePort = (unsigned short)pDigitalIO1739UComboOutput->GetCurSel();
	
}
// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_7
void MtnDialog_FbMonitor::OnBnClickedButtonDigitalIoOutputBit7()
{
	DigitalIO_ToggleOutputBit(0x80);
}
// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_6
void MtnDialog_FbMonitor::OnBnClickedButtonDigitalIoOutputBit6()
{
	DigitalIO_ToggleOutputBit(0x40);
}
// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_5
void MtnDialog_FbMonitor::OnBnClickedButtonDigitalIoOutputBit5()
{
	DigitalIO_ToggleOutputBit(0x20);
}
// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_4
void MtnDialog_FbMonitor::OnBnClickedButtonDigitalIoOutputBit4()
{
	DigitalIO_ToggleOutputBit(0x10);
}
//IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_3
void MtnDialog_FbMonitor::OnBnClickedButtonDigitalIoOutputBit3()
{
	DigitalIO_ToggleOutputBit(0x8);
}
// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_2
void MtnDialog_FbMonitor::OnBnClickedButtonDigitalIoOutputBit2()
{
	DigitalIO_ToggleOutputBit(0x4);
}
// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_1
void MtnDialog_FbMonitor::OnBnClickedButtonDigitalIoOutputBit1()
{
	DigitalIO_ToggleOutputBit(0x2);
}
// IDC_BUTTON_DIGITAL_IO_OUTPUT_BIT_0
void MtnDialog_FbMonitor::OnBnClickedButtonDigitalIoOutputBit0()
{
	DigitalIO_ToggleOutputBit(0x1);
}


// IDC_STATIC_DIGITAL_IO_INPUT_BIT_7
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_6
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_5
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_4
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_3
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_2
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_1
// IDC_STATIC_DIGITAL_IO_INPUT_BIT_0

// IDC_COMBO_DIGITAL_OUTPUT_CARD_LIST
void MtnDialog_FbMonitor::OnCbnSelchangeComboDigitalOutput1739List()
{
	
	CComboBox *pDigitalIO1739UCombo = (CComboBox*) GetDlgItem(IDC_COMBO_DIGITAL_OUTPUT_CARD_LIST);
	int idxDev = pDigitalIO1739UCombo->GetCurSel();

	if(lDrvHandleDigitalOutput_IO1739U == NULL)
	{
	}
	else
	{
		DRV_DeviceClose((LONG far *)&lDrvHandleDigitalOutput_IO1739U);
	}
	DRV_DeviceOpen(theDigitalIO.m_IoCardInfo.stDeviceList[idxDev].dwDeviceNum,
                                (LONG far *)&lDrvHandleDigitalOutput_IO1739U);

}

void MtnDialog_FbMonitor::UI_OpenBoard1739CheckEFO_GoodNG()
{
	int iBoardIdTemp;
	for(int ii = 0; ii<theDigitalIO.m_IoCardInfo.sNumOfDevices; ii++)
	{
		sscanf_s(theDigitalIO.m_IoCardInfo.stDeviceList[ii].szDeviceName, "PCI-1739U BoardID=%d", &iBoardIdTemp);
		if(iBoardIdTemp == 1)
		{
			if(lDrvHandleEFO_Monitor_1739U == NULL)
			{
			}
			else
			{
				DRV_DeviceClose((LONG far *)&lDrvHandleEFO_Monitor_1739U);
			}
			DRV_DeviceOpen(theDigitalIO.m_IoCardInfo.stDeviceList[ii].dwDeviceNum,
										(LONG far *)&lDrvHandleEFO_Monitor_1739U);
		}
	}
}

// IDC_COMBO_EFO_MONITOR_USE_1739U_CARD, 20090824
void MtnDialog_FbMonitor::OnCbnSelchangeComboEfoMonitorUse1739uCard()
{
	CComboBox *pEfoMonitor1739UCombo = (CComboBox*) GetDlgItem(IDC_COMBO_EFO_MONITOR_USE_1739U_CARD);
	int idxDev = pEfoMonitor1739UCombo->GetCurSel();

	if(lDrvHandleEFO_Monitor_1739U == NULL)
	{
	}
	else
	{
		DRV_DeviceClose((LONG far *)&lDrvHandleEFO_Monitor_1739U);
	}
	DRV_DeviceOpen(theDigitalIO.m_IoCardInfo.stDeviceList[idxDev].dwDeviceNum,
                                (LONG far *)&lDrvHandleEFO_Monitor_1739U);
}

// IDC_BUTTON_FB_MONITOR_EFO_GOODNG_BY_1739U
void MtnDialog_FbMonitor::OnBnClickedButtonFbMonitorEfoGoodngBy1739u()
{
	// TODO: Add your control notification handler code here
	if(lDrvHandleEFO_Monitor_1739U != NULL)
	{
		if(mEfoMonitor1739U.m_bRunEFO_MonitorThread1739U)
		{
			mEfoMonitor1739U.EFO_StopMonitor_1739U();
			GetDlgItem(IDC_BUTTON_FB_MONITOR_EFO_GOODNG_BY_1739U)->SetWindowTextA(_T("StartEFO_Mon"));
		}
		else
		{
			mEfoMonitor1739U.EFO_Monitor_1739U_Init(lDrvHandleEFO_Monitor_1739U);
			mEfoMonitor1739U.EFO_StartMonitor_1739U();
			GetDlgItem(IDC_BUTTON_FB_MONITOR_EFO_GOODNG_BY_1739U)->SetWindowTextA(_T("StopEFO_Mon"));
		}
	}
}

// IDC_CHECK_EFO_MONITOR_1739U_DEBUG_FLAG, 20090824
void MtnDialog_FbMonitor::OnBnClickedCheckEfoMonitor1739uDebugFlag()
{
	if(((CButton*)GetDlgItem(IDC_CHECK_EFO_MONITOR_1739U_DEBUG_FLAG))->GetCheck())
	{
		mEfoMonitor1739U.EFO_Monitor_1739U_SetDebugFlag(1);
		GetDlgItem(IDC_EDIT_FB_MONITOR_EFO_BSD_BY_1739U_TOTAL_CNT)->EnableWindow(1);
	}
	else
	{
		mEfoMonitor1739U.EFO_Monitor_1739U_SetDebugFlag(0);
		GetDlgItem(IDC_EDIT_FB_MONITOR_EFO_BSD_BY_1739U_TOTAL_CNT)->EnableWindow(0);
	}
}

void MtnDialog_FbMonitor::OnCbnSelchangeComboEfoTrigUse1739uByte()
{
	usDigitalIO_PortTrigEFO_Byte  = ((CComboBox*) GetDlgItem(IDC_COMBO_EFO_TRIG_USE_1739U_BYTE))->GetCurSel();
}
