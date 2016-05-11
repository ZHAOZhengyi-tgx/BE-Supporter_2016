// ServoController.cpp : implementation file
//

#include "stdafx.h"
#include "DataStruct.h"
#include "ServoController.h"
#include "acsc.h"
#include "macdef.h"
//#include "WireBond.h"
#include "ForceCali.h"

// CServoController

CServoController::CServoController()
: dJogSpeed(2000.0)
, dTblXPosLimit(SYS_TBL_X_POSITIVE_LIMIT)
, dTblXNegLimit(SYS_TBL_X_NEGTIVE_LIMIT)
, dTblYPosLimit(SYS_TBL_Y_POSITIVE_LIMIT)
, dTblYNegLimit(SYS_TBL_Y_NEGTIVE_LIMIT)
, m_bDpramLock(false)
{
    m_stServoSpeedProfile.nTblXMaxVel     = round(TBL_MAX_VEL * GetTableResolution() / 1000000.0);
    m_stServoSpeedProfile.nTblXGeneralAcc = round(1.4E7 * GetTableResolution() / 1000000.0);
    m_stServoSpeedProfile.nTblXBondAcc    = round(1.4E7 * GetTableResolution() / 1000000.0);
    m_stServoSpeedProfile.nTblYMaxVel     = round(TBL_MAX_VEL * GetTableResolution() / 1000000.0);
    m_stServoSpeedProfile.nTblYGeneralAcc = round(1.4E7 * GetTableResolution() / 1000000.0);
    m_stServoSpeedProfile.nTblYBondAcc    = round(1.4E7 * GetTableResolution() / 1000000.0);
    m_stServoSpeedProfile.nBhMaxVel       = round(BH_MAX_VEL * GetBondHeadResolution() / 1000000.0);
    m_stServoSpeedProfile.nBhGeneralAcc   = round(1.0E8 * GetBondHeadResolution() / 1000000.0);
    m_stServoSpeedProfile.nBhStage1Acc    = round(3.0E8 * GetBondHeadResolution() / 1000000.0);
    m_stServoSpeedProfile.nBhStage2Acc    = round(1.0E8 * GetBondHeadResolution() / 1000000.0);
    m_stServoSpeedProfile.nBhStage3Acc    = round(2.0E8 * GetBondHeadResolution() / 1000000.0);
    m_stServoSpeedProfile.nBhStage4Acc    = round(2.0E8 * GetBondHeadResolution() / 1000000.0);
    m_stServoSpeedProfile.nBhStage5Acc    = round(3.0E8 * GetBondHeadResolution() / 1000000.0);
    m_stServoSpeedProfile.nBhStage6Acc    = round(1.0E8 * GetBondHeadResolution() / 1000000.0);

    InitWireClampProfile();
}

CServoController::~CServoController()
{
}


// CServoController member functions

int CServoController::Initialization(bool bOperationMode)
{
	m_bOperationMode = bOperationMode;

    if (bOperationMode == SYS_OFFLINE)
		m_hCommHandle = acsc_OpenCommDirect();
	else
		m_hCommHandle = acsc_OpenCommPCI(ACSC_NONE);

	if (m_hCommHandle == ACSC_INVALID)
		return 1;

	if (bOperationMode == SYS_ONLINE)
	{
		double flTimeClock = 1.0; //0.5
        ACSC_WAITBLOCK stWaitBlk;
		if (!acsc_WriteReal( m_hCommHandle, ACSC_NONE, "CTIME", ACSC_NONE, ACSC_NONE, ACSC_NONE, ACSC_NONE, &flTimeClock, &stWaitBlk))
			return 1;
	}

	int anAxes[] = { ACSC_AXIS_X, ACSC_AXIS_Y, ACSC_AXIS_A, ACSC_AXIS_B, -1 };
	if (!acsc_EnableM(m_hCommHandle, anAxes, NULL))
		return 1;

    //if (!acsc_LoadBuffersFromFile(m_hCommHandle, "C:\\working\\spiiplus\\simulate.prg", NULL))
	//	return 1;

	return 0;
}

// Move distance for Axis X
int CServoController::MoveAxisX(long lDistance, short nMotionMode, short nWait)
{
	int iFlag = 0;

	if (nMotionMode == MOTION_RELATIVE)
		iFlag = ACSC_AMF_RELATIVE;

	if (nWait == MOTION_WITHOUT_WAIT)
	{
		if (!acsc_ToPoint(m_hCommHandle, iFlag, ACSC_AXIS_X, (double)lDistance, NULL))
			return 1;
	}
	else
	{
		int iReceived = 0;
		ACSC_WAITBLOCK stWaitBlk;
		if (!acsc_ToPoint(m_hCommHandle, iFlag, ACSC_AXIS_X, (double)lDistance, &stWaitBlk))
			return 1;
		
		if (acsc_WaitForAsyncCall(m_hCommHandle, NULL, &iReceived, &stWaitBlk, 500))
			return stWaitBlk.Ret;
		else
			return 1;
	}

	return 0;
}

// Move distance for Axis Y
int CServoController::MoveAxisY(long lDistance, short nMotionMode, short nWait)
{
	int iFlag = 0;

	if (nMotionMode == MOTION_RELATIVE)
		iFlag = ACSC_AMF_RELATIVE;

	if (nWait == MOTION_WITHOUT_WAIT)
	{
		if (!acsc_ToPoint(m_hCommHandle, iFlag, ACSC_AXIS_Y, (double)lDistance, NULL))
			return 1;
	}
	else
	{
		int iReceived = 0;
		ACSC_WAITBLOCK stWaitBlk;
		if (!acsc_ToPoint(m_hCommHandle, iFlag, ACSC_AXIS_Y, (double)lDistance, &stWaitBlk))
			return 1;
		
		if (acsc_WaitForAsyncCall(m_hCommHandle, NULL, &iReceived, &stWaitBlk, 500))
			return stWaitBlk.Ret;
		else
			return 1;
	}

	return 0;
}

// Move distance for Axis Z
int CServoController::MoveAxisZ(long lDistance, short nMotionMode, short nWait)
{
	int iFlag = 0;

	if (nMotionMode == MOTION_RELATIVE)
		iFlag = ACSC_AMF_RELATIVE;

	if (nWait == MOTION_WITHOUT_WAIT)
	{
		if (!acsc_ToPoint(m_hCommHandle, iFlag, ACSC_AXIS_A, (double)lDistance, NULL))
			return 1;
	}
	else
	{
		int iReceived = 0;
		ACSC_WAITBLOCK stWaitBlk;
		if (!acsc_ToPoint(m_hCommHandle, iFlag, ACSC_AXIS_A, (double)lDistance, &stWaitBlk))
			return 1;
		
		if (acsc_WaitForAsyncCall(m_hCommHandle, NULL, &iReceived, &stWaitBlk, 500))
			return stWaitBlk.Ret;
		else
			return 1;
	}

	return 0;
}

// Disable all servo controllers for power off
int CServoController::DisableServoController(void)
{
	StopServoProgram(SERVO_BUFFER_2);

    Sleep(50);

    if (!acsc_DisableAll(m_hCommHandle, NULL))
		return 1;

	if (!acsc_CloseComm(m_hCommHandle))
		return 1;
	
	return 0;
}

int CServoController::StartJogX(short nJogDirection)
{	
	if (!acsc_Jog(m_hCommHandle, ACSC_AMF_VELOCITY, ACSC_AXIS_X, dJogSpeed * nJogDirection, NULL))
		return 1;
	
	return 0;
}

int CServoController::StartJogY(short nJogDirection)
{
	if (!acsc_Jog(m_hCommHandle, ACSC_AMF_VELOCITY, ACSC_AXIS_Y, dJogSpeed * nJogDirection, NULL))
		return 1;

	return 0;
}

int CServoController::StartJogZ(short nJogDirection)
{
	if (!acsc_Jog(m_hCommHandle, ACSC_AMF_VELOCITY, ACSC_AXIS_A, dJogSpeed * nJogDirection, NULL))
		return 1;

	return 0;
}

int CServoController::EndJogX(void)
{
	if (!acsc_Kill(m_hCommHandle, ACSC_AXIS_X, NULL))
		return 1;

	return 0;
}

int CServoController::EndJogY(void)
{
	if (!acsc_Kill(m_hCommHandle, ACSC_AXIS_Y, NULL))
		return 1;

	return 0;
}

int CServoController::EndJogZ(void)
{
	if (!acsc_Kill(m_hCommHandle, ACSC_AXIS_A, NULL))
		return 1;

	return 0;
}

// Adjust jog speed to 2 times
int CServoController::JogSpeedUp(void)
{
	dJogSpeed *= 2.0;

	return 0;
}

// Adjust jog speed to half
int CServoController::JogSlowDown(void)
{
	dJogSpeed /= 2.0;

	return 0;
}

// Run program buffer of servo controller
int CServoController::RunServoProgram(short nBuffer)
{
    /*
    int lTrigSignal[1];

    if (m_bOperationMode == SYS_ONLINE)
        lTrigSignal[0] = 0;
    else
        lTrigSignal[0] = 1;

    acsc_WriteInteger(m_hCommHandle, ACSC_NONE, "lOfflineSys ", 0, 1, 
                ACSC_NONE, ACSC_NONE, lTrigSignal, NULL );
    */

    if (!acsc_RunBuffer( m_hCommHandle, nBuffer, NULL, NULL))
		return 1;

    //if (nBuffer > 1)
	//{
	//    if (!acsc_RunBuffer( m_hCommHandle, 1, NULL, NULL))
	//		return 1;
	//}

    return 0;
}

// Read table command position
int CServoController::GetTablePosition(TBL_POS* pstTblPos)
{
    if (!acsc_GetRPosition( m_hCommHandle, ACSC_AXIS_X, &(pstTblPos->dx), NULL))
        return 1;

    if (!acsc_GetRPosition( m_hCommHandle, ACSC_AXIS_Y, &(pstTblPos->dy), NULL))
        return 1;
                                     
    return 0;
}

// Get resolution of XY table
double CServoController::GetTableResolution(void)
{
    return SYS_TBL_RESOL;
}

// Get resolution of bond head
double CServoController::GetBondHeadResolution(void)
{
    return SYS_BH_RESOL;
}

// Get XY table mechanical limit
int CServoController::GetTableMechLimit(double* pdXPosLmt, double* pdXNegLmt, double* pdYPosLmt, double* pdYNegLmt)
{
    *pdXPosLmt = dTblXPosLimit;
    *pdXNegLmt = dTblXNegLimit;
    *pdYPosLmt = dTblYPosLimit;
    *pdYNegLmt = dTblYNegLimit;

    return 0;
}

// Stop program buffer of servo controller
int CServoController::StopServoProgram(short nBuffer)
{
	if (!acsc_StopBuffer( m_hCommHandle, nBuffer, NULL))
		return 1;

	//if (!acsc_StopBuffer( m_hCommHandle, 1, NULL))
	//	return 1;

    return 0;
}

// Trigger bond buffer via DPRAM
int CServoController::TriggerBondBuffer(short nBuffer)
{
    short nDpramAddr;

    if (m_bOperationMode == SYS_ONLINE)
    {
        if (nBuffer == 0)
            nDpramAddr = DPRAM_ADDR_START_BOND0;
        else
            nDpramAddr = DPRAM_ADDR_START_BOND1;

        WriteDpramInteger(nDpramAddr, 1 );
    }

    /*
    else
    {
        int lTrigSignal[] = {1};
        if (nBuffer == 0)
        {
            acsc_WriteInteger(m_hCommHandle, nBuffer, "lStartBond0Offline", 0, 1, 
                ACSC_NONE, ACSC_NONE, lTrigSignal, NULL );
        }
        else
        {
            acsc_WriteInteger(m_hCommHandle, nBuffer, "lStartBond1Offline", 0, 1, 
                ACSC_NONE, ACSC_NONE, lTrigSignal, NULL );
        }
    }
    */

    return 0;
}

// Read integer data from Dpram
int CServoController::ReadDpramInteger(short nDpramAddr, int *plReadResult)
{
    if (m_bDpramLock == true)
        return 0;

    if (m_bOperationMode == SYS_ONLINE)
    {
        m_bDpramLock = true;
        acsc_ReadDPRAMInteger(m_hCommHandle, nDpramAddr, plReadResult);
        m_bDpramLock = false;
    }

    return 0;
}

// Write integer data to Dpram
int CServoController::WriteDpramInteger(short nDpramAddr, int lReadResult)
{
    while (m_bDpramLock == true)
    {
        ;
    }

    if (m_bOperationMode == SYS_ONLINE)
    {
        m_bDpramLock = true;
        acsc_WriteDPRAMInteger(m_hCommHandle, nDpramAddr, lReadResult);
        m_bDpramLock = false;
    }

    return 0;
}

// Get bond progress
int CServoController::GetBondProgress(int *plBondProgress)
{
    short nDpramAddr = DPRAM_ADDR_BOND_PROGRESS;

    if (m_bOperationMode == SYS_ONLINE)
    {
        ReadDpramInteger(nDpramAddr, plBondProgress);
    }
    else
        Sleep(100);

    return 0;
}

// Reset bond progress
int CServoController::ResetBondProgress(void)
{
    short nDpramAddr = DPRAM_ADDR_BOND_PROGRESS;

    if (m_bOperationMode == SYS_ONLINE)
    {
        WriteDpramInteger(nDpramAddr, 0 );
    }

    return 0;
}

// Set trigger signal for restart wire to servo.
int CServoController::TriggerRestartWire(bool bEnable)
{
    short nDpramAddr = DPRAM_ADDR_RESTART_BOND;
    long  lRestartFlag;

    if (bEnable == true)
        lRestartFlag = 1;
    else
        lRestartFlag = 0;

    if (m_bOperationMode == SYS_ONLINE)
    {
        WriteDpramInteger(nDpramAddr, lRestartFlag );
    }

    /*
    int lTrigSignal[] = {1};
    if (acsc_WriteInteger(m_hCommHandle, ACSC_NONE, "lTrigNext1stPt", 0, 1, 
        ACSC_NONE, ACSC_NONE, lTrigSignal, NULL))
    {
        return 1;
    }

    if (acsc_WriteInteger(m_hCommHandle, ACSC_NONE, "lTrigZSrchHt", 0, 1, 
        ACSC_NONE, ACSC_NONE, lTrigSignal, NULL))
    {
        return 1;
    }
    */

    return 0;
}

// Send integer data to servo
int CServoController::SendIntDataToServo(int *palIntData, short nDataLength, short nBuffer)
{
    if (!acsc_WriteInteger(m_hCommHandle, nBuffer, "IntData", 0, nDataLength - 1, 
        ACSC_NONE, ACSC_NONE, palIntData, NULL ))
    {
        CheckServoError();
        return 1;
    }

    return 0;
}

// Send real data to servo
int CServoController::SendRealDataToServo(double *padRealData, short nDataLength, short nBuffer, short nServoVar)
{
    char  chVariable[4][128] = {"TrajPoints", "TablePoints", "SrchHtPoints", "ResetLvlPoints"};

    if (nServoVar == SERVO_VAR_TRAJ)
    {
        if (!acsc_WriteReal(m_hCommHandle, nBuffer, chVariable[nServoVar], 0, 2, 0, nDataLength - 1, padRealData, NULL ))
        {
            CheckServoError();
            return 1;
        }
    }
    else if (nServoVar == SERVO_VAR_FIRST_MOTION)
    {
        if (!acsc_WriteReal(m_hCommHandle, nBuffer, chVariable[nServoVar], 0, 1, 0, nDataLength - 1, padRealData, NULL ))
        {
            CheckServoError();
            return 1;
        }
    }
    else
    {
        if (!acsc_WriteReal(m_hCommHandle, nBuffer, chVariable[nServoVar], 0, 0, 0, nDataLength - 1, padRealData, NULL ))
        {
            CheckServoError();
            return 1;
        }
    }

    return 0;
}

// Get speed profile including Acc, Vel in enc and sample
int CServoController::GetSpeedProfile(short nMotor, short nSpeedSet, double *pdMaxAcc, double *pdMaxVel)
{
    if (nMotor == MOTOR_X)
    {
        *pdMaxVel = (double)m_stServoSpeedProfile.nTblXMaxVel * 1.0E6 / GetTableResolution() / SERVO_SAMPLE_RATE;
        if (nSpeedSet == SPEED_GENERAL_SET)
            *pdMaxAcc = (double)m_stServoSpeedProfile.nTblXGeneralAcc * 1.0E6 / GetTableResolution() / SERVO_SAMPLE_RATE / SERVO_SAMPLE_RATE;
        else
            *pdMaxAcc = (double)m_stServoSpeedProfile.nTblXBondAcc * 1.0E6 / GetTableResolution() / SERVO_SAMPLE_RATE / SERVO_SAMPLE_RATE;
    }
    else if (nMotor == MOTOR_Y)
    {
        *pdMaxVel = (double)m_stServoSpeedProfile.nTblYMaxVel * 1.0E6 / GetTableResolution() / SERVO_SAMPLE_RATE;
        if (nSpeedSet == SPEED_GENERAL_SET)
            *pdMaxAcc = (double)m_stServoSpeedProfile.nTblYGeneralAcc * 1.0E6 / GetTableResolution() / SERVO_SAMPLE_RATE / SERVO_SAMPLE_RATE;
        else
            *pdMaxAcc = (double)m_stServoSpeedProfile.nTblYBondAcc * 1.0E6 / GetTableResolution() / SERVO_SAMPLE_RATE / SERVO_SAMPLE_RATE;
    }
    else if (nMotor == MOTOR_Z)
    {
        *pdMaxVel = (double)m_stServoSpeedProfile.nBhMaxVel * 1.0E6 / GetBondHeadResolution() / SERVO_SAMPLE_RATE;
        if (nSpeedSet == SPEED_GENERAL_SET)
            *pdMaxAcc = (double)m_stServoSpeedProfile.nBhGeneralAcc * 1.0E6 / GetBondHeadResolution() / SERVO_SAMPLE_RATE / SERVO_SAMPLE_RATE;
        else if (nSpeedSet == SPEED_BOND_SET1)
            *pdMaxAcc = (double)m_stServoSpeedProfile.nBhStage1Acc * 1.0E6 / GetBondHeadResolution() / SERVO_SAMPLE_RATE / SERVO_SAMPLE_RATE;
        else if (nSpeedSet == SPEED_BOND_SET2)
            *pdMaxAcc = (double)m_stServoSpeedProfile.nBhStage2Acc * 1.0E6 / GetBondHeadResolution() / SERVO_SAMPLE_RATE / SERVO_SAMPLE_RATE;
        else if (nSpeedSet == SPEED_BOND_SET3)
            *pdMaxAcc = (double)m_stServoSpeedProfile.nBhStage3Acc * 1.0E6 / GetBondHeadResolution() / SERVO_SAMPLE_RATE / SERVO_SAMPLE_RATE;
        else if (nSpeedSet == SPEED_BOND_SET4)
            *pdMaxAcc = (double)m_stServoSpeedProfile.nBhStage4Acc * 1.0E6 / GetBondHeadResolution() / SERVO_SAMPLE_RATE / SERVO_SAMPLE_RATE;
        else if (nSpeedSet == SPEED_BOND_SET5)
            *pdMaxAcc = (double)m_stServoSpeedProfile.nBhStage5Acc * 1.0E6 / GetBondHeadResolution() / SERVO_SAMPLE_RATE / SERVO_SAMPLE_RATE;
        else
            *pdMaxAcc = (double)m_stServoSpeedProfile.nBhStage6Acc * 1.0E6 / GetBondHeadResolution() / SERVO_SAMPLE_RATE / SERVO_SAMPLE_RATE;
    }

    return 0;
}

// Get the position error
int CServoController::GetPositionError(short nMotor, double* pdPosError)
{
    if (m_bOperationMode == SYS_ONLINE)
    {
        if (!acsc_ReadReal(m_hCommHandle, ACSC_NONE, "PE", 4, 4, ACSC_NONE, ACSC_NONE, pdPosError, NULL)) //&stWaitBlk ))
        {
            CheckServoError();
            return 1;
        }
    }
    else
    {
        *pdPosError = -11.0;
    }

    return 0;
}

// Read bond head command position
int CServoController::GetBondHeadPosition(double* pdPosition)
{
    if (m_bOperationMode == SYS_ONLINE)
    {
        if (!acsc_GetRPosition( m_hCommHandle, ACSC_AXIS_A, pdPosition, NULL)) //&stWaitBlk))
            return 1;
    }
    else
        *pdPosition = -4100;

    return 0;
}


// Move table synchronized
int CServoController::SynchroMoveTable(TBL_POS* pstTblPos, short nMotionMode, short nWait)
{
    MoveAxisX(round(pstTblPos->dx), nMotionMode, nWait);
    MoveAxisY(round(pstTblPos->dy), nMotionMode, nWait);

    return 0;
}

// Synchronize motion finish
int CServoController::SyncMotion(short nMotor)
{
    return 0;
}

// Get servo speed profile 
int CServoController::GetServoSpeedProfile(SERVO_SPEED_PROFILE* pstServoSpeedProfile)
{
    *pstServoSpeedProfile = m_stServoSpeedProfile;

    return FOK;
}

// Store servo speed profile 
int CServoController::StoreServoSpeedProfile(SERVO_SPEED_PROFILE* pstServoSpeedProfile)
{
    m_stServoSpeedProfile = *pstServoSpeedProfile;

    SetServoSpeedProfile();

    return FOK;
}

// Save servo speed profile to saving buffer
int CServoController::SaveServoSpeedProfile(void)
{
    theDataMgr.CopyToBuffer((char *)&m_stServoSpeedProfile, sizeof(SERVO_SPEED_PROFILE));

    return FOK;
}

// Load servo speed profile from saving buffer
int CServoController::LoadServoSpeedProfile(void)
{
    theDataMgr.CopyFromBuffer((char *)&m_stServoSpeedProfile, sizeof(SERVO_SPEED_PROFILE));

    m_stServoSpeedProfile.nTblXMaxVel = round(TBL_MAX_VEL * GetTableResolution() / 1000000.0);
    m_stServoSpeedProfile.nTblYMaxVel = round(TBL_MAX_VEL * GetTableResolution() / 1000000.0);
    m_stServoSpeedProfile.nBhMaxVel   = round(BH_MAX_VEL * GetBondHeadResolution() / 1000000.0);

    SetServoSpeedProfile();
    
    return FOK;
}

// Set servo speed profile to servo controller
int CServoController::SetServoSpeedProfile(void)
{
    double dVel, dAcc;
    ACSC_WAITBLOCK stWaitBlk;

    dVel = (double)m_stServoSpeedProfile.nTblXMaxVel / GetTableResolution() * 1.0E6;
    dAcc = (double)m_stServoSpeedProfile.nTblXGeneralAcc / GetTableResolution() * 1.0E6;
    
    if (!acsc_WriteReal( m_hCommHandle, ACSC_NONE, "VEL.0", ACSC_NONE, ACSC_NONE, ACSC_NONE, ACSC_NONE, &dVel, &stWaitBlk))
    {
		CheckServoError();
    }

    if (!acsc_WriteReal( m_hCommHandle, ACSC_NONE, "ACC.0", ACSC_NONE, ACSC_NONE, ACSC_NONE, ACSC_NONE, &dAcc, &stWaitBlk))
    {
		CheckServoError();
    }

    dVel = (double)m_stServoSpeedProfile.nTblYMaxVel / GetTableResolution() * 1.0E6;
    dAcc = (double)m_stServoSpeedProfile.nTblYGeneralAcc / GetTableResolution() * 1.0E6;

    if (!acsc_WriteReal( m_hCommHandle, ACSC_NONE, "VEL.1", ACSC_NONE, ACSC_NONE, ACSC_NONE, ACSC_NONE, &dVel, &stWaitBlk))
    {
		CheckServoError();
    }

    if (!acsc_WriteReal( m_hCommHandle, ACSC_NONE, "ACC.1", ACSC_NONE, ACSC_NONE, ACSC_NONE, ACSC_NONE, &dAcc, &stWaitBlk))
    {
		CheckServoError();
    }

    dVel = (double)m_stServoSpeedProfile.nBhMaxVel / GetBondHeadResolution() * 1.0E6;
    dAcc = (double)m_stServoSpeedProfile.nBhGeneralAcc / GetBondHeadResolution() * 1.0E6;
    
    if (!acsc_WriteReal( m_hCommHandle, ACSC_NONE, "VEL.4", ACSC_NONE, ACSC_NONE, ACSC_NONE, ACSC_NONE, &dVel, &stWaitBlk))
    {
		CheckServoError();
    }

    if (!acsc_WriteReal( m_hCommHandle, ACSC_NONE, "ACC.4", ACSC_NONE, ACSC_NONE, ACSC_NONE, ACSC_NONE, &dAcc, &stWaitBlk))
    {
		CheckServoError();
    }

    return FOK;
}

// Check servo error status
int CServoController::CheckServoError(void)
{
    int  lError, lStringLength;
    char chErrorMsg[255];
    CString szString;
    CStringList szStringList;

    szStringList.RemoveAll();

    if (acsc_GetMotorError(m_hCommHandle, ACSC_AXIS_X, &lError, NULL))
    {
        if (lError > 0)
        {
            if (acsc_GetErrorString(m_hCommHandle, lError, chErrorMsg, 255, &lStringLength))
            {
                chErrorMsg[lStringLength] = '\0';
                szString.Format(_T("Motor X Error %d (%s)\n"), lError, chErrorMsg);
                szStringList.AddTail(szString);
            }
        }
    }
    else
    {
        szString.Format(_T("Motor X Transaction Error %d\n"), acsc_GetLastError());
        szStringList.AddTail(szString);
    }

    if (acsc_GetMotorError(m_hCommHandle, ACSC_AXIS_Y, &lError, NULL))
    {
        if (lError > 0)
        {
            if (acsc_GetErrorString(m_hCommHandle, lError, chErrorMsg, 255, &lStringLength))
            {
                chErrorMsg[lStringLength] = '\0';
                szString.Format(_T("Motor Y Error %d (%s)\n"), lError, chErrorMsg);
                szStringList.AddTail(szString);
            }
        }
    }
    else
    {
        szString.Format(_T("Motor Y Transaction Error %d\n"), acsc_GetLastError());
        szStringList.AddTail(szString);
    }

    if (acsc_GetMotorError(m_hCommHandle, ACSC_AXIS_A, &lError, NULL))
    {
        if (lError > 0)
        {
            if (acsc_GetErrorString(m_hCommHandle, lError, chErrorMsg, 255, &lStringLength))
            {
                chErrorMsg[lStringLength] = '\0';
                szString.Format(_T("Motor Z Error %d (%s)\n"), lError, chErrorMsg);
                szStringList.AddTail(szString);
            }
        }
    }
    else
    {
        szString.Format(_T("Motor Z Transaction Error %d\n"), acsc_GetLastError());
        szStringList.AddTail(szString);
    }

    if (acsc_GetMotorError(m_hCommHandle, ACSC_AXIS_B, &lError, NULL))
    {
        if (lError > 0)
        {
            if (acsc_GetErrorString(m_hCommHandle, lError, chErrorMsg, 255, &lStringLength))
            {
                chErrorMsg[lStringLength] = '\0';
                szString.Format(_T("Motor WCL Error %d (%s)\n"), lError, chErrorMsg);
                szStringList.AddTail(szString);
            }
        }
    }
    else
    {
        szString.Format(_T("Motor WCL Transaction Error %d\n"), acsc_GetLastError());
        szStringList.AddTail(szString);
    }

    if (!szStringList.IsEmpty())
	{
        theMsgMgr.ShowMessage(HELP_MSG, szStringList);
		return FNOK;
	}

    return FOK;
}

// Perform force calibration
int CServoController::PerformForceCalibration(long* plCtactLvl, double* pdForceFactorKf, double* pdForceFactorKa, double* pdForceFactorIo)
{
    MACHINE_DATA stMcData;
    FORCE_CALIBRATION_INPUT stForceCalibInput;
    FORCE_CALIBRATION_OUTPUT stForceCalibrationOutput;

    theMachineData.GetMachineDataRcd(&stMcData);

    acsc_upload_search_contact_parameter(m_hCommHandle, &stForceCalibInput.stTeachContactPara);
    stForceCalibInput.stTeachContactPara.iFlagSwitchToForceControl = 1;

    stForceCalibInput.stForceCaliPara.iInitForceHold_cnt   = 10;
    stForceCalibInput.stForceCaliPara.dInitForceCommand    = DEF_FORCE_OFFSET_I0;
    stForceCalibInput.stForceCaliPara.uiNumPointsSmoothing = 10;
    stForceCalibInput.stForceCaliPara.uiNumSegment         =  8;
    stForceCalibInput.stForceCaliPara.dFirstForce_gram     = 10;
    stForceCalibInput.stForceCaliPara.dLastForce_gram      = 80;
    stForceCalibInput.stForceCaliPara.dDefForceFactor_Kf   = stMcData.dForceFactorKf;
    stForceCalibInput.stForceCaliPara.dDefPosnFactor_Ka    = stMcData.dForceFactorKa;
    stForceCalibInput.stForceCaliPara.dDefForceOffset_I0   = stMcData.dForceFactorIo;

    stForceCalibInput.stPosnCompensate.uiTotalPoints   = 10;
    stForceCalibInput.stPosnCompensate.dUpperPosnLimit = -1000.0;
    stForceCalibInput.stPosnCompensate.dLowerPosnLimit = -4000.0;
    stForceCalibInput.stPosnCompensate.dPositionFactor = 0.0;
    stForceCalibInput.stPosnCompensate.dCtrlOutOffset  = 0.0;
    stForceCalibInput.stPosnCompensate.uiGetStartingDateLen  = 200;
    stForceCalibInput.stPosnCompensate.uiSleepInterAction_ms = 250;
    sprintf_s(stForceCalibInput.stPosnCompensate.strDataFileName, "DataPosnCompn.m");

    stForceCalibInput.iFlagDebug = 0;

    mtn_force_calibration(m_hCommHandle, &stForceCalibInput, &stForceCalibrationOutput);
    if (stForceCalibrationOutput.iStatus == FOK)
    {
        *plCtactLvl = stForceCalibrationOutput.iContactPosnReg;
        *pdForceFactorKf = stForceCalibrationOutput.fForceFactor_Kf;
        *pdForceFactorKa = stForceCalibrationOutput.fPosnFactor_Ka;
        *pdForceFactorIo = stForceCalibrationOutput.fForceOffset_I0;
    }

    return (stForceCalibrationOutput.iStatus);
}

// Perform force verification
int CServoController::PerformForceVerification(short nVerifyForce, double* pdFeedBackForce)
{
    MACHINE_DATA        stMcData;
    FORCE_VERIFY_INPUT  stForceVerifyInput;
    FORCE_VERIFY_OUTPUT stForceVerifyOutput;

    if (m_bOperationMode == SYS_OFFLINE)
    {
        *pdFeedBackForce = nVerifyForce;
        return FOK;
    }

    theMachineData.GetMachineDataRcd(&stMcData);

    acsc_upload_search_contact_parameter(m_hCommHandle, &stForceVerifyInput.stTeachContactPara);
    stForceVerifyInput.stTeachContactPara.iFlagSwitchToForceControl = 1;

    stForceVerifyInput.iFlagTeachContact = 1;
    stForceVerifyInput.dForceVerifyDesiredGram = nVerifyForce;

    stForceVerifyInput.fForceFactor_Kf = stMcData.dForceFactorKf;
    stForceVerifyInput.fPosnFactor_Ka  = stMcData.dForceFactorKa;
    stForceVerifyInput.fForceOffset_I0 = stMcData.dForceFactorIo;
    stForceVerifyInput.iContactPosn    = stMcData.lForceCalibLvl;

    stForceVerifyInput.iFlagDebug      = 0;

    mtn_force_verify(m_hCommHandle, &stForceVerifyInput, &stForceVerifyOutput);
    if (stForceVerifyOutput.iStatus == FOK)
        *pdFeedBackForce = stForceVerifyOutput.dInitForceCommandReadBack;

    return (stForceVerifyOutput.iStatus);
}

// Initialize wire clamp default profile
int CServoController::InitWireClampProfile(void)
{
    m_astWireClampProfile[WCL_BALL_SIT_OPEN_CLOSE].nProfileType     = WCL_BALL_SIT_OPEN_CLOSE;
    m_astWireClampProfile[WCL_BALL_SIT_OPEN_CLOSE].nInterimForce    = 60;
    m_astWireClampProfile[WCL_BALL_SIT_OPEN_CLOSE].nInterimRampTime = 1;
    m_astWireClampProfile[WCL_BALL_SIT_OPEN_CLOSE].nInterimHoldTime = 1;
    m_astWireClampProfile[WCL_BALL_SIT_OPEN_CLOSE].nHoldForce       = -80;
    m_astWireClampProfile[WCL_BALL_SIT_OPEN_CLOSE].nHoldRamptime    = 1;

    m_astWireClampProfile[WCL_SEARCH_HEIGHT_OPEN].nProfileType     = WCL_SEARCH_HEIGHT_OPEN;
    m_astWireClampProfile[WCL_SEARCH_HEIGHT_OPEN].nInterimForce    = 10;
    m_astWireClampProfile[WCL_SEARCH_HEIGHT_OPEN].nInterimRampTime = 1;
    m_astWireClampProfile[WCL_SEARCH_HEIGHT_OPEN].nInterimHoldTime = 1;
    m_astWireClampProfile[WCL_SEARCH_HEIGHT_OPEN].nHoldForce       = 60;
    m_astWireClampProfile[WCL_SEARCH_HEIGHT_OPEN].nHoldRamptime    = 1;

    m_astWireClampProfile[WCL_PEAK_HEIGHT_CLOSE].nProfileType     = WCL_PEAK_HEIGHT_CLOSE;
    m_astWireClampProfile[WCL_PEAK_HEIGHT_CLOSE].nInterimForce    = -30;
    m_astWireClampProfile[WCL_PEAK_HEIGHT_CLOSE].nInterimRampTime = 1;
    m_astWireClampProfile[WCL_PEAK_HEIGHT_CLOSE].nInterimHoldTime = 1;
    m_astWireClampProfile[WCL_PEAK_HEIGHT_CLOSE].nHoldForce       = -80;
    m_astWireClampProfile[WCL_PEAK_HEIGHT_CLOSE].nHoldRamptime    = 1;

    m_astWireClampProfile[WCL_TAIL_HEIGHT_CLOSE].nProfileType     = WCL_TAIL_HEIGHT_CLOSE;
    m_astWireClampProfile[WCL_TAIL_HEIGHT_CLOSE].nInterimForce    = -30;
    m_astWireClampProfile[WCL_TAIL_HEIGHT_CLOSE].nInterimRampTime = 1;
    m_astWireClampProfile[WCL_TAIL_HEIGHT_CLOSE].nInterimHoldTime = 1;
    m_astWireClampProfile[WCL_TAIL_HEIGHT_CLOSE].nHoldForce       = -90;
    m_astWireClampProfile[WCL_TAIL_HEIGHT_CLOSE].nHoldRamptime    = 1;

    m_astWireClampProfile[WCL_THREAD_WIRE_OPEN].nProfileType     = WCL_THREAD_WIRE_OPEN;
    m_astWireClampProfile[WCL_THREAD_WIRE_OPEN].nInterimForce    = 10;
    m_astWireClampProfile[WCL_THREAD_WIRE_OPEN].nInterimRampTime = 1;
    m_astWireClampProfile[WCL_THREAD_WIRE_OPEN].nInterimHoldTime = 1;
    m_astWireClampProfile[WCL_THREAD_WIRE_OPEN].nHoldForce       = 80;
    m_astWireClampProfile[WCL_THREAD_WIRE_OPEN].nHoldRamptime    = 1;

    m_astWireClampProfile[WCL_THREAD_WIRE_CLOSE].nProfileType     = WCL_THREAD_WIRE_CLOSE;
    m_astWireClampProfile[WCL_THREAD_WIRE_CLOSE].nInterimForce    = -10;
    m_astWireClampProfile[WCL_THREAD_WIRE_CLOSE].nInterimRampTime = 1;
    m_astWireClampProfile[WCL_THREAD_WIRE_CLOSE].nInterimHoldTime = 1;
    m_astWireClampProfile[WCL_THREAD_WIRE_CLOSE].nHoldForce       = -80;
    m_astWireClampProfile[WCL_THREAD_WIRE_CLOSE].nHoldRamptime    = 1;

    m_astWireClampProfile[WCL_HOLD_WIRE_CLOSE].nProfileType     = WCL_HOLD_WIRE_CLOSE;
    m_astWireClampProfile[WCL_HOLD_WIRE_CLOSE].nInterimForce    = -50;
    m_astWireClampProfile[WCL_HOLD_WIRE_CLOSE].nInterimRampTime = 1;
    m_astWireClampProfile[WCL_HOLD_WIRE_CLOSE].nInterimHoldTime = 1;
    m_astWireClampProfile[WCL_HOLD_WIRE_CLOSE].nHoldForce       = -50;
    m_astWireClampProfile[WCL_HOLD_WIRE_CLOSE].nHoldRamptime    = 1;

    return FOK;
}

// Get wire clamp profile
int CServoController::GetWireClampProfile(short nProfileIndex, WIRE_CLAMP_PROFILE* pstWireClampProfile)
{
    if (nProfileIndex < WCL_MAX_PROFILE)
    {
        *pstWireClampProfile = m_astWireClampProfile[nProfileIndex];
        return FOK;
    }
    else
        return FNOK;
}

// Store wire clamp profile
int CServoController::StoreWireClampProfile(short nProfileIndex, WIRE_CLAMP_PROFILE* pstWireClampProfile)
{
    if (nProfileIndex < WCL_MAX_PROFILE)
    {
        m_astWireClampProfile[nProfileIndex] = *pstWireClampProfile;
        return FOK;
    }
    else
        return FNOK;
}

// Save wire clamp profile to saving buffer
int CServoController::SaveWireClampProfile(void)
{
    short i;
    
    for (i = WCL_BALL_SIT_OPEN_CLOSE; i < WCL_MAX_PROFILE; i++)
        theDataMgr.CopyToBuffer((char *)&m_astWireClampProfile[i], sizeof(WIRE_CLAMP_PROFILE));

    return FOK;
}

// Load wire clamp profile from saving buffer
int CServoController::LoadWireClampProfile(void)
{
    short i;
    
    for (i = WCL_BALL_SIT_OPEN_CLOSE; i < WCL_MAX_PROFILE; i++)
        theDataMgr.CopyFromBuffer((char *)&m_astWireClampProfile[i], sizeof(WIRE_CLAMP_PROFILE));

    return FOK;
}

// Initialize wire clamp
int CServoController::InitializeWireClamp(void)
{
    RunServoProgram(SERVO_BUFFER_2);
    
    Sleep(50);

    UpdateWireClampProfile();

    Sleep(50);

    TriggerWireClampProfile(WCL_HOLD_WIRE_CLOSE);

    return FOK;
}

// Initialize wire clamp
int CServoController::UpdateWireClampProfile(void)
{
    short i;
    int  alInterimRampTime[7], alInterimTime[7], alHoldRampTime[7];
    REAL arInterimForce[7], arHoldForce[7];
    MACHINE_DATA stMcData;

    theMachineData.GetMachineDataRcd(&stMcData);
   
    for (i = WCL_BALL_SIT_OPEN_CLOSE; i < WCL_MAX_PROFILE; i++)
    {
        alInterimRampTime[i] = m_astWireClampProfile[i].nInterimRampTime; 
        alInterimTime[i]     = m_astWireClampProfile[i].nInterimHoldTime;
        alHoldRampTime[i]    = m_astWireClampProfile[i].nHoldRamptime;
        arInterimForce[i]    = (double)m_astWireClampProfile[i].nInterimForce * stMcData.dWclForceFactorKf + stMcData.dWclForceFactorIo;
        arHoldForce[i]       = (double)m_astWireClampProfile[i].nHoldForce * stMcData.dWclForceFactorKf + stMcData.dWclForceFactorIo;
    }

    if (!acsc_WriteInteger(m_hCommHandle, SERVO_BUFFER_2, "alInterimRampTime", 0, WCL_MAX_PROFILE - 1, 
        ACSC_NONE, ACSC_NONE, alInterimRampTime, NULL ))
    {
        CheckServoError();
        return FNOK;
    }

    if (!acsc_WriteInteger(m_hCommHandle, SERVO_BUFFER_2, "alInterimTime", 0, WCL_MAX_PROFILE - 1, 
        ACSC_NONE, ACSC_NONE, alInterimTime, NULL ))
    {
        CheckServoError();
        return FNOK;
    }

    if (!acsc_WriteInteger(m_hCommHandle, SERVO_BUFFER_2, "alHoldRampTime", 0, WCL_MAX_PROFILE - 1, 
        ACSC_NONE, ACSC_NONE, alHoldRampTime, NULL ))
    {
        CheckServoError();
        return FNOK;
    }

    if (!acsc_WriteReal(m_hCommHandle, SERVO_BUFFER_2, "arInterimForce", 0, WCL_MAX_PROFILE - 1, 
        ACSC_NONE, ACSC_NONE, arInterimForce, NULL ))
    {
        CheckServoError();
        return FNOK;
    }

    if (!acsc_WriteReal(m_hCommHandle, SERVO_BUFFER_2, "arHoldForce", 0, WCL_MAX_PROFILE - 1, 
        ACSC_NONE, ACSC_NONE, arHoldForce, NULL ))
    {
        CheckServoError();
        return FNOK;
    }

    return FOK;
}

// Trigger wire clamp profile
int CServoController::TriggerWireClampProfile(short nWclProfile)
{
    int  lWclCmdCode = nWclProfile;
    
    if (!acsc_WriteInteger(m_hCommHandle, SERVO_BUFFER_2, "lWclCmdCode", 0, 0, 
        ACSC_NONE, ACSC_NONE, &lWclCmdCode, NULL ))
    {
        CheckServoError();
        return FNOK;
    }

    return FOK;
}

// Apply force control
int CServoController::ApplyForceControl(short nForce, short nTime)
{
    int    lCurDac, lBkupDac, lCtactLvl;
    double dForceDac;
    REF_RECORD   stRefRcd;
    MACHINE_DATA stMcData;

    theMachineData.GetMachineDataRcd(&stMcData);

    if (theProgram.IsProgramValid() == true)
    {
        theRefMgr.GetRefRcd(&stRefRcd, 0);
        lCtactLvl = stRefRcd.lCtactLvl;
    }
    else
        lCtactLvl = stMcData.lForceCalibLvl;

    dForceDac = (double)nForce * stMcData.dForceFactorKf + (double)lCtactLvl * stMcData.dForceFactorKa + stMcData.dForceFactorIo;
    
    if (!acsc_ReadInteger(m_hCommHandle, ACSC_NONE, "A_MFLAGS", ACSC_NONE, ACSC_NONE, 
        ACSC_NONE, ACSC_NONE, &lBkupDac, NULL ))
    {
        CheckServoError();
        return FNOK;
    }

    lCurDac = lBkupDac | 0x0002;
    lCurDac = lCurDac & 0xFEFF;

    if (!acsc_WriteInteger(m_hCommHandle, ACSC_NONE, "A_MFLAGS", ACSC_NONE, ACSC_NONE, 
        ACSC_NONE, ACSC_NONE, &lCurDac, NULL ))
    {
        CheckServoError();
        return FNOK;
    }

    if (!acsc_WriteReal(m_hCommHandle, ACSC_NONE, "A_DCOM", ACSC_NONE, ACSC_NONE, 
        ACSC_NONE, ACSC_NONE, &dForceDac, NULL ))
    {
        CheckServoError();
        return FNOK;
    }

    Sleep(nTime);

    if (!acsc_WriteInteger(m_hCommHandle, ACSC_NONE, "A_MFLAGS", ACSC_NONE, ACSC_NONE, 
        ACSC_NONE, ACSC_NONE, &lBkupDac, NULL ))
    {
        CheckServoError();
        return FNOK;
    }

    return FOK;
}

// Get operation mode
bool CServoController::GetOperationMode(void)
{
    return m_bOperationMode;
}
