

int Lighting124_COM_Out(unsigned char ucIdxCh, unsigned char ucValLight)
{
int iRet = MTN_API_OK_ZERO;

	ucLightnessVal[ucIdxCh] = ucValLight;
	char strTemp[32];
	if(ucLightnessVal[ucIdxCh] <10)
	{
		sprintf_s(strTemp, 32, "A%d00%d",ucIdxCh + 1, ucLightnessVal[ucIdxCh]);
	}
	else if(ucLightnessVal[ucIdxCh] <100)
	{
		sprintf_s(strTemp, 32, "A%d0%d",ucIdxCh + 1, ucLightnessVal[ucIdxCh]);
	}
	else
	{
		sprintf_s(strTemp, 32, "A%d%d",ucIdxCh + 1, ucLightnessVal[ucIdxCh]);
	}

	if(cFlagIsDebugMode)
	{
		AfxMessageBox(_T(strTemp));
	}

	if(cSerialPort.IsOpen()) // mLightingViaSerPort.isConnected() == TRUE)
	{
//		mLightingViaSerPort.Send((BYTE*)(&strTemp[0]), 5);
		cSerialPort.Write(&strTemp[0], 5);
	}
	else
	{
		iRet = MTN_API_ERROR_COMMUNICATION_WITH_CONTROLLER;
	}

	if(cFlagIsDebugMode)
	{
		AfxMessageBox(_T(strTemp));
	}

	return iRet;
}

static void ltOutputZeroAllChannel()
{
	int ii;
	char strTemp[32];
	if(cSerialPort.IsOpen()) // mLightingViaSerPort.isConnected())
	{
		for(ii = 0; ii<MAX_NUM_LIGHTING_CHANNEL; ii++)
		{
			sprintf_s(strTemp, 32, "A%d000", ii+1);
//			mLightingViaSerPort.Send((BYTE*)(&strTemp[0]), 5);
			cSerialPort.Write(&strTemp[0], 5);

			Sleep(100);
		}
	}
}
void ltConvertChannelValueToChar(int iChFrom0, unsigned char ucValue, char *strCmdComm)
{
	if(ucValue <10)
	{
		sprintf_s(strCmdComm, 32, "A%d00%d",iChFrom0 + 1, ucValue);
	}
	else if(ucLightnessVal[ucIdxLightCh] <100)
	{
		sprintf_s(strCmdComm, 32, "A%d0%d",iChFrom0 + 1, ucValue);
	}
	else
	{
		sprintf_s(strCmdComm, 32, "A%d%d",iChFrom0 + 1, ucValue);
	}
}

static void ltOutputAllChannelDefaultValue()
{
	int ii;
	char strTemp[32];
	if(cSerialPort.IsOpen()) // mLightingViaSerPort.isConnected())
	{
		for(ii = 0; ii<MAX_NUM_LIGHTING_CHANNEL; ii++)
		{
			sprintf_s(strTemp, 32, "A%d050", ii+1);
//			ltConvertChannelValueToChar(ii, ucLightnessVal[ii], strTemp);

			//mLightingViaSerPort.Send((BYTE*)(&strTemp[0]), 5);
			cSerialPort.Write(strTemp, 5);
			Sleep(500);
		}
	}
}

void CCommunicationDialog::OutputCurrChannelLightingByCom(unsigned char ucLightCmdCh)
{
	Lighting124_COM_Out(ucIdxLightCh, ucLightCmdCh);
}
// IDC_EDIT_LIGHTING_VALUE_CH
void CCommunicationDialog::OnEnKillfocusEditLightingValueCh()
{
	// TODO: Add your control notification handler code here
	static char cTempRead;
	ReadByteFromEdit(IDC_EDIT_LIGHTING_VALUE_CH, &cTempRead);
	ucLightnessVal[ucIdxLightCh] = (unsigned char)cTempRead;
	OutputCurrChannelLightingByCom(ucLightnessVal[ucIdxLightCh]);
}

// IDC_COMB_SEL_LIGHT_CHANNEL
void CCommunicationDialog::OnCbnSelchangeCombSelLightChannel()
{
	ucIdxLightCh = pSelLightChannelCombo->GetCurSel();
	CString cstrTemp;
	cstrTemp.Format("%d", ucLightnessVal[ucIdxLightCh]);
	GetDlgItem(IDC_EDIT_LIGHTING_VALUE_CH)->SetWindowTextA(cstrTemp);
	OutputCurrChannelLightingByCom(ucLightnessVal[ucIdxLightCh]);
}

// IDC_SLIDER_LIGHTING_CH7_CoRED
void CCommunicationDialog::OnNMCustomdrawSliderLightingCh7Cored(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	//*pResult = 0;
	unsigned char ucTempReadSlide = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH7_CoRED))->GetPos();
	ucLightnessVal[0] = ucTempReadSlide;
	if(ucIdxLightCh == 0)
	{
		CString cstrTemp;
		cstrTemp.Format("%d", ucLightnessVal[ucIdxLightCh]);
		GetDlgItem(IDC_EDIT_LIGHTING_VALUE_CH)->SetWindowTextA(cstrTemp);
	}
	Lighting124_COM_Out(0, ucLightnessVal[0]);
}
// IDC_SLIDER_LIGHTING_CH10_Co_BLUE
void CCommunicationDialog::OnNMCustomdrawSliderLightingCh7CoBlue(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	//*pResult = 0;
	unsigned char ucTempReadSlide = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH10_Co_BLUE))->GetPos();
	ucLightnessVal[3] = ucTempReadSlide;
	if(ucIdxLightCh == 3)
	{
		CString cstrTemp;
		cstrTemp.Format("%d", ucLightnessVal[ucIdxLightCh]);
		GetDlgItem(IDC_EDIT_LIGHTING_VALUE_CH)->SetWindowTextA(cstrTemp);
	}
	Lighting124_COM_Out(3, ucLightnessVal[3]);
}
// IDC_SLIDER_LIGHTING_CH9_SI_RED
void CCommunicationDialog::OnNMCustomdrawSliderLightingCh7SiRed(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	*pResult = 0;
	unsigned char ucTempReadSlide = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH9_SI_RED))->GetPos();
	ucLightnessVal[2] = ucTempReadSlide;
	if(ucIdxLightCh == 2)
	{
		CString cstrTemp;
		cstrTemp.Format("%d", ucLightnessVal[ucIdxLightCh]);
		GetDlgItem(IDC_EDIT_LIGHTING_VALUE_CH)->SetWindowTextA(cstrTemp);
	}
	Lighting124_COM_Out(2, ucLightnessVal[2]);
}
// IDC_SLIDER_LIGHTING_CH8_SI_BLUE
void CCommunicationDialog::OnNMCustomdrawSliderLightingCh7SiBlue(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	*pResult = 0;
	unsigned char ucTempReadSlide = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_LIGHTING_CH8_SI_BLUE))->GetPos();
	ucLightnessVal[1] = ucTempReadSlide;
	if(ucIdxLightCh == 1)
	{
		CString cstrTemp;
		cstrTemp.Format("%d", ucLightnessVal[ucIdxLightCh]);
		GetDlgItem(IDC_EDIT_LIGHTING_VALUE_CH)->SetWindowTextA(cstrTemp);
	}
	Lighting124_COM_Out(1, ucLightnessVal[1]);
}
