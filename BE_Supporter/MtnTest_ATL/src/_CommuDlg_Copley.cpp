
void CCommunicationDialog :: ResetCopleyDriverBaudRate9600()
{
	cSerialPort.Write("\r");
}

void CCommunicationDialog :: UpdateCopleyUI_byConnectFlag()
{
	CString cstrTemp;
	//////
    cSerialPort.Write("g r0x19\r");
	Sleep(100);	SerialPortReadReply();
	sscanf_s(szBuffer, "v %d", &iCoplyCurrentScale);
	dCoplyCurrentScale = iCoplyCurrentScale/100.0;
	cstrTemp.Format("%4.2f", dCoplyCurrentScale);
	GetDlgItem(IDC_EDIT_COPLEY_I_SCALE)->SetWindowTextA(cstrTemp);

	//////
    cSerialPort.Write("g r0x21\r"); //  Peak current
	Sleep(100);	SerialPortReadReply();
	sscanf_s(szBuffer, "v %d", &iCopleyCurrenntPeakLimit);
	dCopleyCurrenntPeakLimit = iCopleyCurrenntPeakLimit/100.0;
	cstrTemp.Format("%4.2f", dCopleyCurrenntPeakLimit);
	GetDlgItem(IDC_EDIT_COPLEY_LIMIT_I_PEAK)->SetWindowTextA(cstrTemp);

	//////
    cSerialPort.Write("g r0x22\r"); //  Continuous current
	Sleep(100);	SerialPortReadReply();
	sscanf_s(szBuffer, "v %d", &iCopleyCurrentContiLimit);
	dCopleyCurrentContiLimit = iCopleyCurrentContiLimit/100.0;
	cstrTemp.Format("%4.2f", dCopleyCurrentContiLimit);
	GetDlgItem(IDC_EDIT_COPLEY_LIMIT_I_RMS)->SetWindowTextA(cstrTemp);

	//////
    cSerialPort.Write("g r0x23\r"); //  I2T Time
	Sleep(100);	SerialPortReadReply();
	sscanf_s(szBuffer, "v %d", &iCopleyI2T_Limit);
	cstrTemp.Format("%d", iCopleyI2T_Limit);	
	GetDlgItem(IDC_EDIT_COPLEY_LIMIT_I2T)->SetWindowTextA(cstrTemp);

	//////
    cSerialPort.Write("g r0x00\r"); //  Cp
	Sleep(100);	SerialPortReadReply();
	sscanf_s(szBuffer, "v %d", &iCopleyGainCp);
	cstrTemp.Format("%d", iCopleyGainCp);
	GetDlgItem(IDC_EDIT_COPLEY_GAIN_C_P)->SetWindowTextA(cstrTemp);

	//////
    cSerialPort.Write("g r0x01\r"); //  Ci
	Sleep(100);	SerialPortReadReply();
	sscanf_s(szBuffer, "v %d", &iCopleyGainCi);
	cstrTemp.Format("%d", iCopleyGainCi);
	GetDlgItem(IDC_EDIT_COPLEY_GAIN_C_I)->SetWindowTextA(cstrTemp);

	return ;
}

// IDC_EDIT_COPLEY_I_SCALE
void CCommunicationDialog::OnEnKillfocusEditCopleyIScale()
{
	ReadDoubleFromEdit(IDC_EDIT_COPLEY_I_SCALE, &dCoplyCurrentScale);
	iCoplyCurrentScale = (int)(dCoplyCurrentScale * 100.0);
	sprintf_s(szBufferSendCommand, 512, "s r0x19 %d\r", iCoplyCurrentScale);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100);
	UpdateCopleyUI_byConnectFlag();
}

// IDC_EDIT_COPLEY_LIMIT_I_PEAK
void CCommunicationDialog::OnEnKillfocusEditCopleyLimitIPeak()
{
	ReadDoubleFromEdit(IDC_EDIT_COPLEY_LIMIT_I_PEAK, &dCopleyCurrenntPeakLimit);
	iCopleyCurrenntPeakLimit = (int)(dCopleyCurrenntPeakLimit * 100.0);
	sprintf_s(szBufferSendCommand, 512, "s r0x21 %d\r", iCopleyCurrenntPeakLimit);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100);
	UpdateCopleyUI_byConnectFlag();
}
// IDC_EDIT_COPLEY_LIMIT_I_RMS
void CCommunicationDialog::OnEnKillfocusEditCopleyLimitIRms()
{
	ReadDoubleFromEdit(IDC_EDIT_COPLEY_LIMIT_I_RMS, &dCopleyCurrentContiLimit);
	iCopleyCurrentContiLimit = (int)(dCopleyCurrentContiLimit * 100.0);
	sprintf_s(szBufferSendCommand, 512, "s r0x22 %d\r", iCopleyCurrentContiLimit);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100);
	UpdateCopleyUI_byConnectFlag();
}
// IDC_EDIT_COPLEY_LIMIT_I2T
void CCommunicationDialog::OnEnKillfocusEditCopleyLimitI2t()
{
	ReadUnsignedIntegerFromEdit(IDC_EDIT_COPLEY_LIMIT_I2T, &iCopleyI2T_Limit);
	sprintf_s(szBufferSendCommand, 512, "s r0x23 %d\r", iCopleyI2T_Limit);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100);
	UpdateCopleyUI_byConnectFlag();
}

// IDC_EDIT_COPLEY_GAIN_C_P
void CCommunicationDialog::OnEnKillfocusEditCopleyGainCP()
{
	ReadUnsignedIntegerFromEdit(IDC_EDIT_COPLEY_GAIN_C_P, &iCopleyGainCp);
	sprintf_s(szBufferSendCommand, 512, "s r0x00 %d\r", iCopleyGainCp);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100);
	UpdateCopleyUI_byConnectFlag();
}

// IDC_EDIT_COPLEY_GAIN_C_I
void CCommunicationDialog::OnEnKillfocusEditCopleyGainCI()
{
	ReadUnsignedIntegerFromEdit(IDC_EDIT_COPLEY_GAIN_C_I, &iCopleyGainCi);
	sprintf_s(szBufferSendCommand, 512, "s r0x01 %d\r", iCopleyGainCi);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100);
	UpdateCopleyUI_byConnectFlag();
}

// IDC_BUTTON_COPLEY_SAVE_FLASH
void CCommunicationDialog::OnBnClickedButtonCopleySaveFlash()
{
	GetDlgItem(IDC_BUTTON_COPLEY_SAVE_FLASH)->EnableWindow(FALSE);
	sprintf_s(szBufferSendCommand, 512, "s f0x19 %d\r", iCoplyCurrentScale);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100);

	sprintf_s(szBufferSendCommand, 512, "s f0x21 %d\r", iCopleyCurrenntPeakLimit);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100);

	sprintf_s(szBufferSendCommand, 512, "s f0x22 %d\r", iCopleyCurrentContiLimit);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100);

	sprintf_s(szBufferSendCommand, 512, "s f0x23 %d\r", iCopleyI2T_Limit);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100);

	sprintf_s(szBufferSendCommand, 512, "s f0x00 %d\r", iCopleyGainCp);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100);

	sprintf_s(szBufferSendCommand, 512, "s f0x01 %d\r", iCopleyGainCi);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100);

	GetDlgItem(IDC_BUTTON_COPLEY_SAVE_FLASH)->EnableWindow(TRUE);
}

// IDC_BUTTON_COPLY_READ_FLASH
void CCommunicationDialog::OnBnClickedButtonCoplyReadFlash()
{
	CString cstrTemp;

	GetDlgItem(IDC_BUTTON_COPLY_READ_FLASH)->EnableWindow(FALSE);
	//SerialPortReadReply();
	///// Read from Flash and update the UI-edit
	sprintf_s(szBuffer, 256, "");
    cSerialPort.Write("g f0x19\r");
	Sleep(100);	SerialPortReadReply();
	char *pStartRead;
	pStartRead = strstr(szBuffer, "v ");
	sscanf_s(pStartRead, "v %d", &iCoplyCurrentScale);
	//int nLenBuff = strlen(szBuffer);
	//if(nLenBuff > 8)
	//{
	//	sscanf_s(&szBuffer[nLenBuff-8], "v %d", &iCoplyCurrentScale);
	//}
	//else
	//{
		sscanf_s(szBuffer, "v %d", &iCoplyCurrentScale);  // 
	//}
	dCoplyCurrentScale = iCoplyCurrentScale/100.0;
	cstrTemp.Format("%4.2f", dCoplyCurrentScale);
	GetDlgItem(IDC_EDIT_COPLEY_I_SCALE)->SetWindowTextA(cstrTemp);

	//////
    cSerialPort.Write("g f0x21\r"); //  Peak current
	Sleep(100);	SerialPortReadReply();
	sscanf_s(szBuffer, "v %d", &iCopleyCurrenntPeakLimit);
	dCopleyCurrenntPeakLimit = iCopleyCurrenntPeakLimit/100.0;
	cstrTemp.Format("%4.2f", dCopleyCurrenntPeakLimit);
	GetDlgItem(IDC_EDIT_COPLEY_LIMIT_I_PEAK)->SetWindowTextA(cstrTemp);

	//////
    cSerialPort.Write("g f0x22\r"); //  Continuous current
	Sleep(100);	SerialPortReadReply();
	sscanf_s(szBuffer, "v %d", &iCopleyCurrentContiLimit);
	dCopleyCurrentContiLimit = iCopleyCurrentContiLimit/100.0;
	cstrTemp.Format("%4.2f", dCopleyCurrentContiLimit);
	GetDlgItem(IDC_EDIT_COPLEY_LIMIT_I_RMS)->SetWindowTextA(cstrTemp);

	//////
    cSerialPort.Write("g f0x23\r"); //  I2T Time
	Sleep(100);	SerialPortReadReply();
	sscanf_s(szBuffer, "v %d", &iCopleyI2T_Limit);
	cstrTemp.Format("%d", iCopleyI2T_Limit);	
	GetDlgItem(IDC_EDIT_COPLEY_LIMIT_I2T)->SetWindowTextA(cstrTemp);

	//////
    cSerialPort.Write("g f0x00\r"); //  Cp
	Sleep(100);	SerialPortReadReply();
	sscanf_s(szBuffer, "v %d", &iCopleyGainCp);
	cstrTemp.Format("%d", iCopleyGainCp);
	GetDlgItem(IDC_EDIT_COPLEY_GAIN_C_P)->SetWindowTextA(cstrTemp);

	//////
    cSerialPort.Write("g f0x01\r"); //  Ci
	Sleep(100);	SerialPortReadReply();
	sscanf_s(szBuffer, "v %d", &iCopleyGainCi);
	cstrTemp.Format("%d", iCopleyGainCi);
	GetDlgItem(IDC_EDIT_COPLEY_GAIN_C_I)->SetWindowTextA(cstrTemp);

	///////////////// update the RAM memeory in the Copley
	sprintf_s(szBufferSendCommand, 512, "s r0x19 %d\r", iCoplyCurrentScale);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100);

	sprintf_s(szBufferSendCommand, 512, "s r0x21 %d\r", iCopleyCurrenntPeakLimit);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100);

	sprintf_s(szBufferSendCommand, 512, "s r0x22 %d\r", iCopleyCurrentContiLimit);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100);

	sprintf_s(szBufferSendCommand, 512, "s r0x23 %d\r", iCopleyI2T_Limit);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100);

	sprintf_s(szBufferSendCommand, 512, "s r0x00 %d\r", iCopleyGainCp);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100);

	sprintf_s(szBufferSendCommand, 512, "s r0x01 %d\r", iCopleyGainCi);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100);

	GetDlgItem(IDC_BUTTON_COPLY_READ_FLASH)->EnableWindow(TRUE);
}