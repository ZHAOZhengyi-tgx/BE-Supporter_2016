

////////////////////////////////////////////////////
// ServoTronix Driver
// Variables
double dServoTronxCurrentScale, dServoTronixCurrentLoopGainP, dServoTronixCurrentLoopGainI, dServoTronixCurrentLoopGainFF, dServoTronixCurrentLoopGainBemf;
double dServoTronxCurrentLimitMotorPeak, dServoTronxCurrentLimitMotorConti;
double dServoTronxCurrentLimitDriverPeak, dServoTronxCurrentLimitDriverConti;
unsigned int iServoTronxMfbDir, iPositionDir;
double dServoTronxCommutatePhase;
// IDC_EDIT_SERVOTRONIX_I_SCALE
void CCommunicationDialog::OnEnKillfocusEditServotronixIScale()
{
	ReadDoubleFromEdit(IDC_EDIT_SERVOTRONIX_I_SCALE, &dServoTronxCurrentScale);
	sprintf_s(szBufferSendCommand, 512, "ANIN1ISCALE %6.4f\r", dServoTronxCurrentScale);
    cSerialPort.Write(szBufferSendCommand);
}
// IDC_EDIT_SERVOTRONIX_DRIVER_LIMIT_CURRENT_PEAK
void CCommunicationDialog::OnEnKillfocusEditServotronixDriverLimitCurrentPeak()
{
	ReadDoubleFromEdit(IDC_EDIT_SERVOTRONIX_DRIVER_LIMIT_CURRENT_PEAK, &dServoTronxCurrentLimitDriverPeak);
	sprintf_s(szBufferSendCommand, 512, "ILIM %6.4f\r", dServoTronxCurrentLimitDriverPeak);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100); 
	sprintf_s(szBufferSendCommand, 512, "CONFIG\r"); cSerialPort.Write(szBufferSendCommand);
}
// IDC_EDIT_SERVOTRONIX_DRIVER_LIMIT_CURRENT_CONTI
void CCommunicationDialog::OnEnKillfocusEditServotronixDriverLimitCurrentConti()
{
}
// IDC_EDIT_SERVOTRONIX_CURRENT_GAIN_P
void CCommunicationDialog::OnEnKillfocusEditServotronixCurrentGainP()
{
	ReadDoubleFromEdit(IDC_EDIT_SERVOTRONIX_CURRENT_GAIN_P, &dServoTronixCurrentLoopGainP);
	sprintf_s(szBufferSendCommand, 512, "KCP %6.4f\r", dServoTronixCurrentLoopGainP);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100); 
	sprintf_s(szBufferSendCommand, 512, "CONFIG\r"); cSerialPort.Write(szBufferSendCommand);
}
// IDC_EDIT_SERVOTRONIX_CURRENT_GAIN_I
void CCommunicationDialog::OnEnKillfocusEditServotronixCurrentGainI()
{
	ReadDoubleFromEdit(IDC_EDIT_SERVOTRONIX_CURRENT_GAIN_I, &dServoTronixCurrentLoopGainI);
	sprintf_s(szBufferSendCommand, 512, "KCI %6.4f\r", dServoTronixCurrentLoopGainI);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100); 
	sprintf_s(szBufferSendCommand, 512, "CONFIG\r"); cSerialPort.Write(szBufferSendCommand);
}
// IDC_EDIT_SERVOTRONIX_CURRENT_FF
void CCommunicationDialog::OnEnKillfocusEditServotronixCurrentFf()
{
	ReadDoubleFromEdit(IDC_EDIT_SERVOTRONIX_CURRENT_FF, &dServoTronixCurrentLoopGainFF);
	sprintf_s(szBufferSendCommand, 512, "KCFF %6.4f\r", dServoTronixCurrentLoopGainFF);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100); 
	sprintf_s(szBufferSendCommand, 512, "CONFIG\r"); cSerialPort.Write(szBufferSendCommand);
}
// IDC_EDIT_SERVOTRONIX_CURRENT_K_BCAK_EMF
void CCommunicationDialog::OnEnKillfocusEditServotronixCurrentKBcakEmf()
{
	ReadDoubleFromEdit(IDC_EDIT_SERVOTRONIX_CURRENT_K_BCAK_EMF, &dServoTronixCurrentLoopGainBemf);
	sprintf_s(szBufferSendCommand, 512, "KcBemf %6.4f\r", dServoTronixCurrentLoopGainBemf);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100); 
	sprintf_s(szBufferSendCommand, 512, "CONFIG\r"); cSerialPort.Write(szBufferSendCommand);
}
// IDC_EDIT_SERVOTRONIX_MOTOR_LIMIT_CURRENT_CONTI
void CCommunicationDialog::OnEnKillfocusEditServotronixMotorLimitCurrentConti()
{
	ReadDoubleFromEdit(IDC_EDIT_SERVOTRONIX_MOTOR_LIMIT_CURRENT_CONTI, &dServoTronxCurrentLimitMotorConti);
	sprintf_s(szBufferSendCommand, 512, "MICONT %6.4f\r", dServoTronxCurrentLimitMotorConti);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100); 
	sprintf_s(szBufferSendCommand, 512, "CONFIG\r"); cSerialPort.Write(szBufferSendCommand);
}
// IDC_EDIT_SERVOTRONIX_MOTOR_LIMIT_CURRENT_PEAK
void CCommunicationDialog::OnEnKillfocusEditServotronixMotorLimitCurrentPeak()
{	
	ReadDoubleFromEdit(IDC_EDIT_SERVOTRONIX_MOTOR_LIMIT_CURRENT_PEAK, &dServoTronxCurrentLimitMotorPeak);
	sprintf_s(szBufferSendCommand, 512, "MIPEAK %6.4f\r", dServoTronxCurrentLimitMotorPeak);
    cSerialPort.Write(szBufferSendCommand);
	Sleep(100); 
	sprintf_s(szBufferSendCommand, 512, "CONFIG\r"); cSerialPort.Write(szBufferSendCommand);
}

void CCommunicationDialog :: UpdateServoTronix_UI_byConnectFlag()
{
	CString cstrTemp;
	char strCmdCode[256];
	int nCmdLen;
	long lSendRet;
	//////
	sprintf_s(strCmdCode, 256, "ANIN1ISCALE\r");
	nCmdLen = (int)strlen(strCmdCode) - 1;
    lSendRet = cSerialPort.Write(strCmdCode);	Sleep(100);	SerialPortReadReply();
	dServoTronxCurrentScale = strtod(&szBuffer[nCmdLen], &strStopString);
	cstrTemp.Format("%4.2f", dServoTronxCurrentScale);
	GetDlgItem(IDC_EDIT_SERVOTRONIX_I_SCALE)->SetWindowTextA(cstrTemp);

	//// MIPEAK
	sprintf_s(strCmdCode, 256, "MIPEAK\r");
	nCmdLen = (int)strlen(strCmdCode) - 1;
    cSerialPort.Write(strCmdCode);	Sleep(100);	SerialPortReadReply();
	dServoTronxCurrentLimitMotorPeak = strtod(&szBuffer[nCmdLen], &strStopString);
	cstrTemp.Format("%4.2f", dServoTronxCurrentLimitMotorPeak);
	GetDlgItem(IDC_EDIT_SERVOTRONIX_MOTOR_LIMIT_CURRENT_PEAK)->SetWindowTextA(cstrTemp);

	sprintf_s(strCmdCode, 256, "MICONT\r");
	nCmdLen = (int)strlen(strCmdCode) - 1;
    cSerialPort.Write(strCmdCode); 	Sleep(100);	SerialPortReadReply();
	dServoTronxCurrentLimitMotorConti = strtod(&szBuffer[nCmdLen], &strStopString);
	cstrTemp.Format("%4.2f", dServoTronxCurrentLimitMotorConti);
	GetDlgItem(IDC_EDIT_SERVOTRONIX_MOTOR_LIMIT_CURRENT_CONTI)->SetWindowTextA(cstrTemp);

	// ILIM
	sprintf_s(strCmdCode, 256, "ILIM\r");
	nCmdLen = (int)strlen(strCmdCode) - 1;
    cSerialPort.Write(strCmdCode); 	Sleep(100);	SerialPortReadReply();
	dServoTronxCurrentLimitDriverPeak = strtod(&szBuffer[nCmdLen], &strStopString);
	cstrTemp.Format("%4.2f", dServoTronxCurrentLimitDriverPeak);
	GetDlgItem(IDC_EDIT_SERVOTRONIX_DRIVER_LIMIT_CURRENT_PEAK)->SetWindowTextA(cstrTemp);

	// DICONT
	sprintf_s(strCmdCode, 256, "DICONT\r");
	nCmdLen = (int)strlen(strCmdCode) - 1;
    cSerialPort.Write(strCmdCode); 	Sleep(100);	SerialPortReadReply();
	dServoTronxCurrentLimitDriverConti = strtod(&szBuffer[nCmdLen], &strStopString);
	cstrTemp.Format("%4.2f", dServoTronxCurrentLimitDriverConti);
	GetDlgItem(IDC_EDIT_SERVOTRONIX_DRIVER_LIMIT_CURRENT_CONTI)->SetWindowTextA(cstrTemp);

	//KCP
	sprintf_s(strCmdCode, 256, "KCP\r");
	nCmdLen = (int)strlen(strCmdCode) - 1;
    cSerialPort.Write(strCmdCode); 	Sleep(100);	SerialPortReadReply();
	dServoTronixCurrentLoopGainP = strtod(&szBuffer[nCmdLen], &strStopString);
	cstrTemp.Format("%4.2f", dServoTronixCurrentLoopGainP);
	GetDlgItem(IDC_EDIT_SERVOTRONIX_CURRENT_GAIN_P)->SetWindowTextA(cstrTemp);

	//KCI
	sprintf_s(strCmdCode, 256, "KCI\r");
	nCmdLen = (int)strlen(strCmdCode) - 1;
    cSerialPort.Write(strCmdCode); 	Sleep(100);	SerialPortReadReply();
	dServoTronixCurrentLoopGainI = strtod(&szBuffer[nCmdLen], &strStopString);
	cstrTemp.Format("%4.2f", dServoTronixCurrentLoopGainI);
	GetDlgItem(IDC_EDIT_SERVOTRONIX_CURRENT_GAIN_I)->SetWindowTextA(cstrTemp);

	//KcBemf
	sprintf_s(strCmdCode, 256, "KcBemf\r");
	nCmdLen = (int)strlen(strCmdCode) - 1;
    cSerialPort.Write(strCmdCode); 	Sleep(100);	SerialPortReadReply();
	dServoTronixCurrentLoopGainBemf = strtod(&szBuffer[nCmdLen], &strStopString);
	cstrTemp.Format("%4.2f", dServoTronixCurrentLoopGainBemf);
	GetDlgItem(IDC_EDIT_SERVOTRONIX_CURRENT_K_BCAK_EMF)->SetWindowTextA(cstrTemp);

	//KCFF
	sprintf_s(strCmdCode, 256, "KCFF\r");
	nCmdLen = (int)strlen(strCmdCode) - 1;
    cSerialPort.Write(strCmdCode); 	Sleep(100);	SerialPortReadReply();
	dServoTronixCurrentLoopGainFF = strtod(&szBuffer[nCmdLen], &strStopString);
	cstrTemp.Format("%4.2f", dServoTronixCurrentLoopGainFF);
	GetDlgItem(IDC_EDIT_SERVOTRONIX_CURRENT_FF)->SetWindowTextA(cstrTemp);

	// iServoTronxMfbDir;
	sprintf_s(strCmdCode, 256, "mfbdir\r");
	nCmdLen = (int)strlen(strCmdCode) - 1;
    cSerialPort.Write(strCmdCode); 	Sleep(100);	SerialPortReadReply();
	sscanf(&szBuffer[nCmdLen], "%d", &iServoTronxMfbDir); //strtod(&szBuffer[nCmdLen], &strStopString);
	cstrTemp.Format("%d", iServoTronxMfbDir);
	GetDlgItem(IDC_EDIT_COMMUNICATION_DLG_SRVTRONICS_MFBDIR)->SetWindowTextA(cstrTemp);

	// mphase // double dServoTronxCommutatePhase;
	sprintf_s(strCmdCode, 256, "mphase\r");
	nCmdLen = (int)strlen(strCmdCode) - 1;
    cSerialPort.Write(strCmdCode); 	Sleep(100);	SerialPortReadReply();
	dServoTronxCommutatePhase = strtod(&szBuffer[nCmdLen], &strStopString);
	cstrTemp.Format("%4.2f", dServoTronxCommutatePhase);
	GetDlgItem(IDC_EDIT_COMMUNICATION_DLG_SRVTRONICS_MPHASE)->SetWindowTextA(cstrTemp);

	// iPositionDir, IDC_CHECK_COMMUNICATION_DLG_SRVTRONICS_POSITION_DIR
	sprintf_s(strCmdCode, 256, "dir\r");
	nCmdLen = (int)strlen(strCmdCode) - 1;
    cSerialPort.Write(strCmdCode); 	Sleep(100);	SerialPortReadReply();
	sscanf(&szBuffer[nCmdLen], "%d", &iPositionDir); //strtod(&szBuffer[nCmdLen], &strStopString);
	//cstrTemp.Format("%4.2f", dServoTronixCurrentLoopGainFF);
	((CButton *)GetDlgItem(IDC_CHECK_COMMUNICATION_DLG_SRVTRONICS_POSITION_DIR))->SetCheck(iPositionDir);

	return ;
}
// Filters
// 
// IDC_BUTTON_SERVOTRONIX_ENABLE_DISABLE_DRIVER
void CCommunicationDialog::OnBnClickedButtonServotronixEnableDisableDriver()
{
}
// Display driver status: disable or enable
// driver mode:opmode

// Commutation Mode: [PhaseFindMode] = 2, [MENCTYPE] = 4
// ----- Current Loop tuning
// CLTUNE
//------ Anti Vibration
// KNLANTIRESSHARP
// KNLANTIVIB
// KNLANTIRESHZ
// KNLUSERGAIN: adaptive global gain.
// -------------- Low Pass filter
// NLFILTDAMPING
// NLFILTT1
// -------------- Notch filter
// NLNOTCHBW
// NLNOTCHCENTER

// config is required after modifying values of any following parameters
// DICONT, DIPEAK
// DIR
// KCBEMF, KCP, KCI, KCFF, KCFFSAT, KCPISAT, KCSAT
// MICONT, MIPEAK
// MKF, MKT, ML
// MLGAINC, MLGAINP, 
// MPITCH, MPOLES, MRESPOLES, MSPEED, VBUS
// KCDQCOMP, MFBDIR, MENCTYPE, MFBMODE
///// Even if the values have not been changed, to set following variables
// needs sending a "config" command
// FEEDBACKTYPE, MVANGLF, MVANGLH, PWMFRQ, VLIM

// CAN-open bus to synchronize motion in multiple axis

// IDC_BUTTON_COMMUDLG_LOAD_SCRIPT