
#include "stdafx.h"
#include "MtnDialog_FbMonitor.h"
#include "DigitalIO.h"  // 20081030

#include "MtnApi.h"

static char strDebugText[1024];
extern HANDLE stCommHandle_DlgFbMonitor_ACS;	// communication handle

void mtn_acs_set_clear_out_io(unsigned int uiStatus,  int nBit)
{
	int iOut0[1];

	if (!acsc_ReadInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, &iOut0[0], NULL ))  // Handle
	{
		sprintf_s(strDebugText, 128, "Error Read OUT0, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
	}
	switch(nBit)
	{
		case 0:
			if (uiStatus == 1)
			{
				iOut0[0] = iOut0[0] | 0x1;
			}
			else
			{
				iOut0[0] = iOut0[0] & 0xFFFFFFFE;
			}
			break;
		case 1:
			if (uiStatus == 1)
			{
				iOut0[0] = iOut0[0] | ACS_OUT0_OR_SET_BIT1;
			}
			else
			{
				iOut0[0] = iOut0[0] & ACS_OUT0_AND_CLEAR_BIT1;

			}
			break;
		case 2:
			if (uiStatus == 1)
			{
				iOut0[0] = iOut0[0] | ACS_OUT0_OR_SET_BIT2;
			}
			else
			{
				iOut0[0] = iOut0[0] & ACS_OUT0_AND_CLEAR_BIT2;
			}
			break;
		case 3:
			if (uiStatus == 1)
			{
				iOut0[0] = iOut0[0] | ACS_OUT0_OR_SET_BIT3;
			}
			else
			{
				iOut0[0] = iOut0[0] & ACS_OUT0_AND_CLEAR_BIT3;
			}
			break;
		case 4:
			if (uiStatus == 1)
			{
				iOut0[0] = iOut0[0] | ACS_OUT0_OR_SET_BIT4;
			}
			else
			{
				iOut0[0] = iOut0[0] & ACS_OUT0_AND_CLEAR_BIT4;
			}
			break;
		case 5:
			if (uiStatus == 1)
			{
				iOut0[0] = iOut0[0] | ACS_OUT0_OR_SET_BIT5;
			}
			else
			{
				iOut0[0] = iOut0[0] & ACS_OUT0_AND_CLEAR_BIT5;
			}
			break;
		case 6:
			if (uiStatus == 1)
			{
				iOut0[0] = iOut0[0] | ACS_OUT0_OR_SET_BIT6;
			}
			else
			{
				iOut0[0] = iOut0[0] & ACS_OUT0_AND_CLEAR_BIT6;
			}
			break;
		case 7:
			if (uiStatus == 1)
			{
				iOut0[0] = iOut0[0] | ACS_OUT0_OR_SET_BIT7;
			}
			else
			{
				iOut0[0] = iOut0[0] & ACS_OUT0_AND_CLEAR_BIT7;
			}
			break;

	}

	if (!acsc_WriteInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, iOut0, NULL ))  // Handle
	{
		sprintf_s(strDebugText, 128, "Error write OUT0, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
		return;
	}

}

// IDC_CHECK_FB_ACS_OUT0, 20081030, trigger by OUT0.0
void MtnDialog_FbMonitor::OnBnClickedCheckOut0()
{
	UINT uiRadioState = ((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT0))->GetCheck();

	mtn_acs_set_clear_out_io(uiRadioState, 0);
	//if (!acsc_ReadInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, &iOut0[0], NULL ))  // Handle
	//{
	//	sprintf_s(strDebugText, 128, "Error Read OUT0, Error Code: %d", acsc_GetLastError());
	//	AfxMessageBox(_T(strDebugText));
	//}

	//if (uiRadioState == 1)
	//{
	//	iOut0[0] = iOut0[0] | 0x1;
	//}
	//else
	//{
	//	iOut0[0] = iOut0[0] & 0xFFFFFFFE;

	//}
	//if (!acsc_WriteInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, iOut0, NULL ))  // Handle
	//{
	//	sprintf_s(strDebugText, 128, "Error write OUT0, Error Code: %d", acsc_GetLastError());
	//	AfxMessageBox(_T(strDebugText));
	//	return;
	//}
}

void MtnDialog_FbMonitor::OnBnClickedCheckAcsOut1()
{
	UINT uiRadioState = ((CButton *)GetDlgItem(IDC_CHECK_ACS_OUT1))->GetCheck();

	mtn_acs_set_clear_out_io(uiRadioState, 1);
	//if (!acsc_ReadInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, &iOut0[0], NULL ))
	//{
	//	sprintf_s(strDebugText, 128, "Error Read OUT1, Error Code: %d", acsc_GetLastError());
	//	AfxMessageBox(_T(strDebugText));
	//}

	//if (uiRadioState == 1)
	//{
	//	iOut0[0] = iOut0[0] | ACS_OUT0_OR_SET_BIT1;
	//}
	//else
	//{
	//	iOut0[0] = iOut0[0] & ACS_OUT0_AND_CLEAR_BIT1;

	//}
	//if (!acsc_WriteInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, iOut0, NULL ))
	//{
	//	sprintf_s(strDebugText, 128, "Error write OUT1, Error Code: %d", acsc_GetLastError());
	//	AfxMessageBox(_T(strDebugText));
	//}
	Sleep(20);

}
// IDC_CHECK_ACS_OUT2
void MtnDialog_FbMonitor::OnBnClickedCheckAcsOut2()
{
	UINT uiRadioState = ((CButton *)GetDlgItem(IDC_CHECK_ACS_OUT2))->GetCheck();

	mtn_acs_set_clear_out_io(uiRadioState, 2);
	//if (!acsc_ReadInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, &iOut0[0], NULL ))
	//{
	//	sprintf_s(strDebugText, 128, "Error Read OUT1, Error Code: %d", acsc_GetLastError());
	//	AfxMessageBox(_T(strDebugText));
	//}

	//if (uiRadioState == 1)
	//{
	//	iOut0[0] = iOut0[0] | ACS_OUT0_OR_SET_BIT2;
	//}
	//else
	//{
	//	iOut0[0] = iOut0[0] & ACS_OUT0_AND_CLEAR_BIT2;

	//}
	//if (!acsc_WriteInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, iOut0, NULL ))
	//{
	//	sprintf_s(strDebugText, 128, "Error write OUT1, Error Code: %d", acsc_GetLastError());
	//	AfxMessageBox(_T(strDebugText));
	//}
	Sleep(20);
}
// IDC_CHECK_ACS_OUT3
void MtnDialog_FbMonitor::OnBnClickedCheckAcsOut3()
{
	UINT uiRadioState = ((CButton *)GetDlgItem(IDC_CHECK_ACS_OUT3))->GetCheck();

	mtn_acs_set_clear_out_io(uiRadioState, 3);
	//if (!acsc_ReadInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, &iOut0[0], NULL ))
	//{
	//	sprintf_s(strDebugText, 128, "Error Read OUT0, Error Code: %d", acsc_GetLastError());
	//	AfxMessageBox(_T(strDebugText));
	//}

	//if (uiRadioState == 1)
	//{
	//	iOut0[0] = iOut0[0] | ACS_OUT0_OR_SET_BIT3;
	//}
	//else
	//{
	//	iOut0[0] = iOut0[0] & ACS_OUT0_AND_CLEAR_BIT3;

	//}
	//if (!acsc_WriteInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, iOut0, NULL ))
	//{
	//	sprintf_s(strDebugText, 128, "Error write OUT1, Error Code: %d", acsc_GetLastError());
	//	AfxMessageBox(_T(strDebugText));
	//}
	Sleep(20);
}

// IDC_CHECK_FB_ACS_OUT8
void MtnDialog_FbMonitor::OnBnClickedCheckFbAcsOut8()
{
	UINT uiRadioState = ((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT8))->GetCheck();

	if (!acsc_ReadInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, &iOut0[0], NULL ))
	{
		sprintf_s(strDebugText, 128, "Error Read OUT8, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
	}

	if (uiRadioState == 1)
	{
		iOut0[0] = iOut0[0] | ACS_OUT0_OR_SET_BIT8;
	}
	else
	{
		iOut0[0] = iOut0[0] & ACS_OUT0_AND_CLEAR_BIT8;

	}
	if (!acsc_WriteInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, iOut0, NULL ))
	{
		sprintf_s(strDebugText, 128, "Error write OUT8, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
	}
	Sleep(20);
}
// IDC_CHECK_FB_ACS_OUT9
void MtnDialog_FbMonitor::OnBnClickedCheckFbAcsOut9()
{
	UINT uiRadioState = ((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT9))->GetCheck();

	if (!acsc_ReadInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, &iOut0[0], NULL ))
	{
		sprintf_s(strDebugText, 128, "Error Read OUT9, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
	}

	if (uiRadioState == 1)
	{
		iOut0[0] = iOut0[0] | ACS_OUT0_OR_SET_BIT9;
	}
	else
	{
		iOut0[0] = iOut0[0] & ACS_OUT0_AND_CLEAR_BIT9;

	}
	if (!acsc_WriteInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, iOut0, NULL ))
	{
		sprintf_s(strDebugText, 128, "Error write OUT9, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
	}
	Sleep(20);
}
// IDC_CHECK_FB_ACS_OUT10
void MtnDialog_FbMonitor::OnBnClickedCheckFbAcsOut10()
{
	UINT uiRadioState = ((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT10))->GetCheck();

	if (!acsc_ReadInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, &iOut0[0], NULL ))
	{
		sprintf_s(strDebugText, 128, "Error Read OUT10, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
	}

	if (uiRadioState == 1)
	{
		iOut0[0] = iOut0[0] | ACS_OUT0_OR_SET_BIT10;
	}
	else
	{
		iOut0[0] = iOut0[0] & ACS_OUT0_AND_CLEAR_BIT10;

	}
	if (!acsc_WriteInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, iOut0, NULL ))
	{
		sprintf_s(strDebugText, 128, "Error write OUT10, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
	}
	Sleep(20);
}
// IDC_CHECK_FB_ACS_OUT11
void MtnDialog_FbMonitor::OnBnClickedCheckFbAcsOut11()
{
	UINT uiRadioState = ((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT11))->GetCheck();

	if (!acsc_ReadInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, &iOut0[0], NULL ))
	{
		sprintf_s(strDebugText, 128, "Error Read OUT11, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
	}

	if (uiRadioState == 1)
	{
		iOut0[0] = iOut0[0] | ACS_OUT0_OR_SET_BIT11;
	}
	else
	{
		iOut0[0] = iOut0[0] & ACS_OUT0_AND_CLEAR_BIT11;

	}
	if (!acsc_WriteInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, iOut0, NULL ))
	{
		sprintf_s(strDebugText, 128, "Error write OUT11, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
	}
	Sleep(20);
}

// IDC_CHECK_FB_ACS_OUT4
void MtnDialog_FbMonitor::OnBnClickedCheckFbAcsOut4()
{
	UINT uiRadioState = ((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT4))->GetCheck();

	mtn_acs_set_clear_out_io(uiRadioState, 4);
	//if (!acsc_ReadInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, &iOut0[0], NULL ))
	//{
	//	sprintf_s(strDebugText, 128, "Error Read OUT4, Error Code: %d", acsc_GetLastError());
	//	AfxMessageBox(_T(strDebugText));
	//}

	//if (uiRadioState == 1)
	//{
	//	iOut0[0] = iOut0[0] | ACS_OUT0_OR_SET_BIT4;
	//}
	//else
	//{
	//	iOut0[0] = iOut0[0] & ACS_OUT0_AND_CLEAR_BIT4;

	//}
	//if (!acsc_WriteInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, iOut0, NULL ))
	//{
	//	sprintf_s(strDebugText, 128, "Error write OUT4, Error Code: %d", acsc_GetLastError());
	//	AfxMessageBox(_T(strDebugText));
	//}
	Sleep(20);
}
// IDC_CHECK_FB_ACS_OUT5
void MtnDialog_FbMonitor::OnBnClickedCheckFbAcsOut5()
{
	UINT uiRadioState = ((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT5))->GetCheck();

	mtn_acs_set_clear_out_io(uiRadioState, 5);
	//if (!acsc_ReadInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, &iOut0[0], NULL ))
	//{
	//	sprintf_s(strDebugText, 128, "Error Read OUT5, Error Code: %d", acsc_GetLastError());
	//	AfxMessageBox(_T(strDebugText));
	//}

	//if (uiRadioState == 1)
	//{
	//	iOut0[0] = iOut0[0] | ACS_OUT0_OR_SET_BIT5;
	//}
	//else
	//{
	//	iOut0[0] = iOut0[0] & ACS_OUT0_AND_CLEAR_BIT5;

	//}
	//if (!acsc_WriteInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, iOut0, NULL ))
	//{
	//	sprintf_s(strDebugText, 128, "Error write OUT5, Error Code: %d", acsc_GetLastError());
	//	AfxMessageBox(_T(strDebugText));
	//}
	Sleep(20);
}
// IDC_CHECK_FB_ACS_OUT6
void MtnDialog_FbMonitor::OnBnClickedCheckFbAcsOut6()
{
	UINT uiRadioState = ((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT6))->GetCheck();

	mtn_acs_set_clear_out_io(uiRadioState, 6);
	//if (!acsc_ReadInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, &iOut0[0], NULL ))
	//{
	//	sprintf_s(strDebugText, 128, "Error Read OUT6, Error Code: %d", acsc_GetLastError());
	//	AfxMessageBox(_T(strDebugText));
	//}

	//if (uiRadioState == 1)
	//{
	//	iOut0[0] = iOut0[0] | ACS_OUT0_OR_SET_BIT6;
	//}
	//else
	//{
	//	iOut0[0] = iOut0[0] & ACS_OUT0_AND_CLEAR_BIT6;

	//}
	//if (!acsc_WriteInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, iOut0, NULL ))
	//{
	//	sprintf_s(strDebugText, 128, "Error write OUT6, Error Code: %d", acsc_GetLastError());
	//	AfxMessageBox(_T(strDebugText));
	//}
	Sleep(20);
}
// IDC_CHECK_FB_ACS_OUT7
void MtnDialog_FbMonitor::OnBnClickedCheckFbAcsOut7()
{
	UINT uiRadioState = ((CButton *)GetDlgItem(IDC_CHECK_FB_ACS_OUT7))->GetCheck();

	mtn_acs_set_clear_out_io(uiRadioState, 7);
	//if (!acsc_ReadInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, &iOut0[0], NULL ))
	//{
	//	sprintf_s(strDebugText, 128, "Error Read OUT7, Error Code: %d", acsc_GetLastError());
	//	AfxMessageBox(_T(strDebugText));
	//}

	//if (uiRadioState == 1)
	//{
	//	iOut0[0] = iOut0[0] | ACS_OUT0_OR_SET_BIT7;
	//}
	//else
	//{
	//	iOut0[0] = iOut0[0] & ACS_OUT0_AND_CLEAR_BIT7;

	//}
	//if (!acsc_WriteInteger(stCommHandle_DlgFbMonitor_ACS, ACSC_NONE, "OUT0", 0, 0, ACSC_NONE, ACSC_NONE, iOut0, NULL ))
	//{
	//	sprintf_s(strDebugText, 128, "Error write OUT7, Error Code: %d", acsc_GetLastError());
	//	AfxMessageBox(_T(strDebugText));
	//}
	Sleep(20);
}

void MtnDialog_FbMonitor::UI_Enable_Group_Servo_ACS_FbMonitorDlg(BOOL bEnableFlag)
{
	GetDlgItem(IDC_GROUP_LABEL_SERVO_CHANNEL)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHANNEL_SEL_COMBO)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHANNEL_NUM)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_ENABLE_MOTOR)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_LABEL_FB_POSITION)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_FB_POSITION)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_LABEL_FB_VELOCITY)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_FB_VELOCITY)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_LABEL_FB_FORCE)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_FB_FORCE)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_LABEL_FB_ACCELERATION)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_FB_ACCELERATION)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_LABEL_FB_LIMIT_SENSOR)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_FB_LIMIT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_LABEL_FB_INDEX)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_FB_INDEX)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_LABEL_FB_LIMIT_SENSOR2)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_FB_MOTOR_STATUS)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_FB_MONITOR_INDEX_POSITION)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_FB_INDEX_POSITION)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_FB_MONITOR_CHANNEL)->ShowWindow(bEnableFlag);
}

void MtnDialog_FbMonitor::UI_Enable_Group_Normal_ACS_ServoHome_MtnDialog_FbMonitor(BOOL bEnableFlag)
{
	GetDlgItem(IDC_CHECK_DEBUG_SEARCH_INDEX)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_AXIS_API_GO_LIMIT_AND_HOME)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_AXIS_API_GO_LIMIT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_AXIS_API_GO_HOME_FROM_LIMIT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_OUT_LIMIT_DIST_INDEX_1)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SEARCH_HOME_OUT_LIMIT_DIST_INDEX_1)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_OUT_INDEX_1)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SEARCH_HOME_OUT_INDEX_1)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_OUT_INDEX_2)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SEARCH_HOME_OUT_INDEX_2)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_OUT_DIST_INDEX_RELAX)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SEARCH_HOME_OUT_DIST_INDEX_RELAX)->ShowWindow(bEnableFlag);
//	GetDlgItem(IDC_BUTTON_FB_MONITOR_HOME_ALL_API)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_BONDER_HOME_ALL)->ShowWindow(bEnableFlag);		
}

void MtnDialog_FbMonitor::UI_Enable_Group_Setting_ACS_ServoHome_MtnDialog_FbMonitor(BOOL bEnableFlag)
{
	GetDlgItem(IDC_GROUP_LABEL_SEARCH_HOME)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_VEL)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SEARCH_HOME_MOVE_VEL)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_Acc)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SEARCH_HOME_MOVE_Acc)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_JERK)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SEARCH_HOME_MOVE_JERK)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_DEC)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SEARCH_HOME_MOVE_DEC)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SEARCH_HOME_DETECT_FREQ_FACTOR_10KHZ)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_DETECT_MAX_DIST_PROT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SEARCH_HOME_DETECT_MAX_DIST_PROT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SEARCH_HOME_MOVE_DIST_SEARCH_LIMIT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SEARCH_HOME_JOG_VEL_SEARCH_LIMIT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_PE_TH_SEARCH_LIMIT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SEARCH_HOME_PE_TH_SEARCH_LIMIT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_1)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_1)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SEARCH_HOME_MOVE_DIST_BF_SEARCH_INDEX_2)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_2)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SEARCH_HOME_JOG_VEL_SEARCH_INDEX_2)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_SEARCH_HOME_POSN_ERR_SETTLE)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SEARCH_HOME_POSN_ERR_SETTLE)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_AXIS_GO_HOME)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_AXIS_GO_LIMIT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_AXIS_GO_HOME_FROM_LIMIT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_SEARCH_INDEX_VERIFY_BY_REPEATING)->ShowWindow(bEnableFlag);	
	GetDlgItem(IDC_BUTTON_FB_MONITOR_HOME_ALL_API)->ShowWindow(bEnableFlag);	

	UI_InitWH_IOTextGuide(!bEnableFlag);
}

void MtnDialog_FbMonitor::UI_Enable_Group_ACS_OUT_MtnDialog_FbMonitor(BOOL bEnableFlag)
{
	GetDlgItem(IDC_GROUP_LABEL_ACS_IO)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_FB_ACS_OUT0)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_ACS_OUT1)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_ACS_OUT2)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_ACS_OUT3)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_FB_ACS_OUT4)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_FB_ACS_OUT5)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_FB_ACS_OUT6)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_FB_ACS_OUT7)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_FB_ACS_OUT8)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_FB_ACS_OUT9)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_FB_ACS_OUT10)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_FB_ACS_OUT11)->ShowWindow(bEnableFlag);

	GetDlgItem(IDC_FB_MONITOR_START)->ShowWindow(bEnableFlag);

	GetDlgItem(IDC_SLIDER_FB_DLG_TRIG_EFO_RATIO_PERIOD)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_FB_DLG_TRIG_EFO_RATIO_PERIOD)->ShowWindow(bEnableFlag);
	
}

void MtnDialog_FbMonitor::UpdateServoCtrlUI()
{
	if(stCommHandle_DlgFbMonitor_ACS == ACSC_INVALID)  // Handle
	{
		GetDlgItem(IDC_ENABLE_MOTOR)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_AXIS_GO_HOME)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_AXIS_GO_LIMIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_AXIS_GO_HOME_FROM_LIMIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_AXIS_API_GO_LIMIT_AND_HOME)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_AXIS_API_GO_LIMIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_AXIS_API_GO_HOME_FROM_LIMIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FB_ACS_OUT0)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_ACS_OUT1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_ACS_OUT2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_ACS_OUT3)->EnableWindow(FALSE);

		GetDlgItem(IDC_CHECK_FB_ACS_OUT4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FB_ACS_OUT5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FB_ACS_OUT6)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FB_ACS_OUT7)->EnableWindow(FALSE);

		GetDlgItem(IDC_CHECK_FB_ACS_OUT8)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FB_ACS_OUT9)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FB_ACS_OUT10)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FB_ACS_OUT11)->EnableWindow(FALSE);
		GetDlgItem(IDC_FB_MONITOR_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_FB_MONITOR_HOME_ALL_API)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_BONDER_HOME_ALL)->EnableWindow(FALSE);

		UI_Enable_Group_ACS_OUT_MtnDialog_FbMonitor(FALSE);

		cFlagEnable_Group_Servo_ACS_ChannelFbMonitor = 0;
		((CButton*) GetDlgItem(IDC_CHECK_FLAG_ENA_ACS_CH_MONITOR))->SetCheck(cFlagEnable_Group_Servo_ACS_ChannelFbMonitor); 
		UI_Enable_Group_Servo_ACS_FbMonitorDlg(cFlagEnable_Group_Servo_ACS_ChannelFbMonitor);

	}
	else
	{
		GetDlgItem(IDC_ENABLE_MOTOR)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_AXIS_GO_HOME)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_AXIS_GO_LIMIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_AXIS_GO_HOME_FROM_LIMIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_AXIS_API_GO_LIMIT_AND_HOME)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_AXIS_API_GO_LIMIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_AXIS_API_GO_HOME_FROM_LIMIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_FB_ACS_OUT0)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_ACS_OUT1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_ACS_OUT2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_ACS_OUT3)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_FB_ACS_OUT4)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_FB_ACS_OUT5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_FB_ACS_OUT6)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_FB_ACS_OUT7)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_FB_ACS_OUT8)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_FB_ACS_OUT9)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_FB_ACS_OUT10)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_FB_ACS_OUT11)->EnableWindow(TRUE);
		GetDlgItem(IDC_FB_MONITOR_START)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FB_MONITOR_HOME_ALL_API)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_BONDER_HOME_ALL)->EnableWindow(TRUE);

		UI_Enable_Group_ACS_OUT_MtnDialog_FbMonitor(cFlagEnable_Group_ACS_OUT_MtnDialog_FbMonitor);
		UI_Enable_Group_Servo_ACS_FbMonitorDlg(cFlagEnable_Group_Servo_ACS_ChannelFbMonitor);

	}
}
