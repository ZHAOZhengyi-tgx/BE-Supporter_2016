

//// ELMO variables
// KI[1], KP[1]
// AG[1],
// MO
// PL[1] PL[2]
// CA[1...40]: Commutation Array
// IDC_EDIT_ELMO_I_SCALE
double dElmoCurrentScale, dElmoCurrentPeakLimit, dElmoCurrentContiLimit, dElmoPeakCurrentTime;
double dElmoCurrentGainP, dElmoCurrentGainI;

void CCommunicationDialog::OnEnKillfocusEditElmoIScale()
{
	ReadDoubleFromEdit(IDC_EDIT_ELMO_I_SCALE, &dElmoCurrentScale);
	sprintf_s(szBufferSendCommand, 512, "AG[1]=%f\r", dElmoCurrentScale);
    cSerialPort.Write(szBufferSendCommand);
}
// IDC_EDIT_ELMO_LIMIT_I_PEAK
void CCommunicationDialog::OnEnKillfocusEditElmoLimitIPeak()
{
	ReadDoubleFromEdit(IDC_EDIT_ELMO_LIMIT_I_PEAK, &dElmoCurrentPeakLimit);
	sprintf_s(szBufferSendCommand, 512, "PL[1]=%f\r", dElmoCurrentPeakLimit);
    cSerialPort.Write(szBufferSendCommand);
}
// IDC_EDIT_ELMO_LIMIT_I_RMS
void CCommunicationDialog::OnEnKillfocusEditElmoLimitIRms()
{
	ReadDoubleFromEdit(IDC_EDIT_ELMO_LIMIT_I_RMS, &dElmoCurrentContiLimit);
	sprintf_s(szBufferSendCommand, 512, "CL[1]=%f\r", dElmoCurrentContiLimit);
    cSerialPort.Write(szBufferSendCommand);
}
// IDC_EDIT_ELMO_CURRENT_GAIN_P
void CCommunicationDialog::OnEnKillfocusEditElmoCurrentGainP()
{
	ReadDoubleFromEdit(IDC_EDIT_ELMO_CURRENT_GAIN_P, &dElmoCurrentGainP);
	sprintf_s(szBufferSendCommand, 512, "KP[1]=%f\r", dElmoCurrentGainP);
    cSerialPort.Write(szBufferSendCommand);
}
// IDC_EDIT_ELMO_CURRENT_GAIN_I
void CCommunicationDialog::OnEnKillfocusEditElmoCurrentGainI()
{
	ReadDoubleFromEdit(IDC_EDIT_ELMO_CURRENT_GAIN_I, &dElmoCurrentGainI);
	sprintf_s(szBufferSendCommand, 512, "KI[1]=%f\r", dElmoCurrentGainI);
    cSerialPort.Write(szBufferSendCommand);
}

void CCommunicationDialog :: UpdateELMO_UI_byConnectFlag()
{
	CString cstrTemp;
	//////
    cSerialPort.Write("AG[1]\r");
	Sleep(100);	SerialPortReadReply();
	//sscanf_s(szBuffer, "AG[1]%f;", &dElmoCurrentScale);
	dElmoCurrentScale = strtod(&szBuffer[5], &strStopString);
	cstrTemp.Format("%4.2f", dElmoCurrentScale);
	GetDlgItem(IDC_EDIT_ELMO_I_SCALE)->SetWindowTextA(cstrTemp);

	//////
    cSerialPort.Write("PL[1]\r"); //  Peak Current
	Sleep(100);	SerialPortReadReply();
	dElmoCurrentPeakLimit = strtod(&szBuffer[5], &strStopString);
	cstrTemp.Format("%6.1f", dElmoCurrentPeakLimit);
	GetDlgItem(IDC_EDIT_ELMO_LIMIT_I_PEAK)->SetWindowTextA(cstrTemp);

	//////
    cSerialPort.Write("CL[1]\r"); //  Continuous Current Limit
	Sleep(100);	SerialPortReadReply();
	dElmoCurrentContiLimit = strtod(&szBuffer[5], &strStopString);
	cstrTemp.Format("%6.1f", dElmoCurrentContiLimit);
	GetDlgItem(IDC_EDIT_ELMO_LIMIT_I_RMS)->SetWindowTextA(cstrTemp);

	//////
    cSerialPort.Write("PL[2]\r"); //  Peak Current
	Sleep(100);	SerialPortReadReply();
	dElmoPeakCurrentTime = strtod(&szBuffer[5], &strStopString);
	cstrTemp.Format("%6.1f", dElmoPeakCurrentTime);
	GetDlgItem(IDC_EDIT_ELMO_LIMIT_PEAK_TIME)->SetWindowTextA(cstrTemp);

	//////
    cSerialPort.Write("KP[1]\r"); //  Cp
	Sleep(100);	SerialPortReadReply();
	//sscanf_s(szBuffer, "KP[1]%f;", &dElmoCurrentGainP);
	dElmoCurrentGainP = strtod(&szBuffer[5], &strStopString);
	cstrTemp.Format("%6.4f", dElmoCurrentGainP);
	GetDlgItem(IDC_EDIT_ELMO_CURRENT_GAIN_P)->SetWindowTextA(cstrTemp);

	//////
    cSerialPort.Write("KI[1]\r"); //  Ci
	Sleep(100);	SerialPortReadReply();
	// sscanf_s(szBuffer, "KI[1]%f;", &dElmoCurrentGainI);
	dElmoCurrentGainI = strtod(&szBuffer[5], &strStopString);
	cstrTemp.Format("%6.1f", dElmoCurrentGainI);
	GetDlgItem(IDC_EDIT_ELMO_CURRENT_GAIN_I)->SetWindowTextA(cstrTemp);

	return ;
}


// IDC_EDIT_ELMO_LIMIT_PEAK_TIME
void CCommunicationDialog::OnEnKillfocusEditElmoLimitPeakTime()
{
	// TODO: Add your control notification handler code here
}

// DL: Download program
// CP - Clear Program
// CC - Compiled Program Ready
// DV[N] - Reference Desired Value
// EC - Error Code
// EF[N] - Encoder Filter Frequency