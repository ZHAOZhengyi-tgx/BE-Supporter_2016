//The MIT License (MIT)
//
//Copyright (c) 2016 ZHAOZhengyi-tgx
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
// (c)All right reserved, sg.LongRenE@gmail.com

// Module: Motion Stepper
// Template structure is based on ADS-PCI-1240
// History
// YYYYMMDD  Author    Notes
// 
#include "stdafx.h"
#include "MotionStepper.h"
#include "ADS1240.h"

#define MAX_DWORD      4294967296.0

STEPPER_SPEED_PROFILE_PER_BOARD astStepperSpeedProfilePerBoard[3];

void mtnstp_wb_set_home_para(unsigned int uiCurrBoardId, unsigned long *ulErrCode)
{
	// Home Type: 1
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[WB_WH_INNER_CLAMPER_STEPPER].dHomeType = 1;
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[WB_WH_OUTER_CLAMPER_STEPPER].dHomeType = 1;
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[WB_WH_ONLOADER_STEPPER].dHomeType = 1;

	// Home Vel
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[WB_WH_INNER_CLAMPER_STEPPER].dHomeP0_Vel = 2000;
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[WB_WH_OUTER_CLAMPER_STEPPER].dHomeP0_Vel = 2000;
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[WB_WH_ONLOADER_STEPPER].dHomeP0_Vel = 2000;

	mtnstp_download_parameters_ads1240(uiCurrBoardId, ulErrCode);
}

#include "Mpc2810.h"
int mtnstp_upload_parameters_leetro_2812(BYTE uiCurrBoardId, DWORD *ulErrCode)
{
	int iRet = MTN_API_OK_ZERO;

	int ii, iMinAxisOnBoard, iMaxAxisOnBoard, idxProfileOnBoard;
//	iAxisOnBoard = get_axe(uiCurrBoardId);

	*ulErrCode = 0;

	iMinAxisOnBoard = uiCurrBoardId * 4 + 1;
	iMaxAxisOnBoard = iMinAxisOnBoard + 3;

	for(ii = iMinAxisOnBoard; ii<= iMaxAxisOnBoard; ii++)
	{
		idxProfileOnBoard = ii - iMinAxisOnBoard;
		get_profile(ii,
			&astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[idxProfileOnBoard].dStepperStartVelocity,
			&astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[idxProfileOnBoard].dMaxVelocity,
			&astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[idxProfileOnBoard].dMaxAcceleration);

		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[idxProfileOnBoard].dStepperDriveVelocity = get_conspeed(ii);
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[idxProfileOnBoard].dMaxJerk = 0;
	}
	return iRet;
}

int mtnstp_download_parameters_leetro_2812(BYTE uiCurrBoardId, DWORD *ulErrCode)
{
	int iRet = MTN_API_OK_ZERO;

	int ii, iMinAxisOnBoard, iMaxAxisOnBoard, idxProfileOnBoard;
//	iAxisOnBoard = get_axe(uiCurrBoardId);

	*ulErrCode = 0;
	iMinAxisOnBoard = uiCurrBoardId * 4 + 1;
	iMaxAxisOnBoard = iMinAxisOnBoard + 3;

	for(ii = iMinAxisOnBoard; ii<= iMaxAxisOnBoard; ii++)
	{
		idxProfileOnBoard = ii - iMinAxisOnBoard;
		set_profile(ii,
			astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[idxProfileOnBoard].dStepperStartVelocity,
			astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[idxProfileOnBoard].dMaxVelocity,
			astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[idxProfileOnBoard].dMaxAcceleration);

		set_conspeed(ii, astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[idxProfileOnBoard].dStepperDriveVelocity);
		set_maxspeed(ii, astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[idxProfileOnBoard].dMaxVelocity);
	}
	return iRet;
}

int mtnstp_upload_parameters_ads1240(BYTE uiCurrBoardId, DWORD *ulErrCode)
{
	int iRet = MTN_API_OK_ZERO;
	unsigned long ulXSV, ulYSV, ulZSV, ulUSV;
	unsigned long ulXDV, ulYDV, ulZDV, ulUDV;
	unsigned long ulXMDV, ulYMDV, ulZMDV, ulUMDV;
	unsigned long ulXAC, ulYAC, ulZAC, ulUAC;
	unsigned long ulXJerk, ulYJerk, ulZJerk, ulUJerk;

	*ulErrCode = 0;

    // read the initial setup ---Start_Velocity
    *ulErrCode = (ULONG)P1240MotRdMutiReg(uiCurrBoardId, XYZU_Axis, SV, &ulXSV, &ulYSV, &ulZSV, &ulUSV);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_upload_parameters_ads1240;
    }
	else
	{
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dStepperStartVelocity = (double)ulXSV;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dStepperStartVelocity = (double)ulYSV;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dStepperStartVelocity = (double)ulZSV;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dStepperStartVelocity = (double)ulUSV;
	}

    // read the initial setup ---Drive_Velocity        
    *ulErrCode = (ULONG)P1240MotRdMutiReg(uiCurrBoardId, XYZU_Axis, DV, &ulXDV, &ulYDV, &ulZDV, &ulUDV);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_upload_parameters_ads1240;
    }
	else
	{
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dStepperDriveVelocity = (double)ulXDV;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dStepperDriveVelocity = (double)ulYDV;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dStepperDriveVelocity = (double)ulZDV;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dStepperDriveVelocity = (double)ulUDV;
	}

    // read the initial setup ---Max Drive_Velocity        
    *ulErrCode = (ULONG)P1240MotRdMutiReg(uiCurrBoardId, XYZU_Axis, MDV, &ulXMDV, &ulYMDV, &ulZMDV, &ulUMDV);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_upload_parameters_ads1240;
    }
	else
	{
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dMaxVelocity = (double)ulXMDV;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dMaxVelocity = (double)ulYMDV;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dMaxVelocity = (double)ulZMDV;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dMaxVelocity = (double)ulUMDV;
	}

    // read the initial setup ---Max Acc        
    *ulErrCode = (ULONG)P1240MotRdMutiReg(uiCurrBoardId, XYZU_Axis, AC, &ulXAC, &ulYAC, &ulZAC, &ulUAC);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_upload_parameters_ads1240;
    }
	else
	{
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dMaxAcceleration = (double)ulXAC;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dMaxAcceleration = (double)ulYAC;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dMaxAcceleration = (double)ulZAC;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dMaxAcceleration = (double)ulUAC;
	}

    // read the initial setup ---Max Jerk        
    *ulErrCode = (ULONG)P1240MotRdMutiReg(uiCurrBoardId, XYZU_Axis, AK, &ulXJerk, &ulYJerk, &ulZJerk, &ulUJerk);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_upload_parameters_ads1240;
    }
	else
	{
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dMaxJerk = (double)ulXJerk;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dMaxJerk = (double)ulYJerk;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dMaxJerk = (double)ulZJerk;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dMaxJerk = (double)ulUJerk;
	}

    // read the initial setup --- Positive Limit      
	DWORD lXPosLmt, lYPosLmt, lZPosLmt, lUPosLmt;
    *ulErrCode = (ULONG)P1240MotRdMutiReg(uiCurrBoardId, XYZU_Axis, PLmt, &lXPosLmt, &lYPosLmt, &lZPosLmt, &lUPosLmt);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_upload_parameters_ads1240;
    }
	else
	{
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dPositiveLimit = (double)lXPosLmt;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dPositiveLimit = (double)lYPosLmt;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dPositiveLimit = (double)lZPosLmt;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dPositiveLimit = (double)lUPosLmt;
	}

	    // read the initial setup --- Negative Limit      
	DWORD lXNegLmt, lYNegLmt, lZNegLmt, lUNegLmt;
    *ulErrCode = (ULONG)P1240MotRdMutiReg(uiCurrBoardId, XYZU_Axis, NLmt, &lXNegLmt, &lYNegLmt, &lZNegLmt, &lUNegLmt);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_upload_parameters_ads1240;
    }
	else
	{
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dNegativeLimit = (double)(lXNegLmt) - MAX_DWORD;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dNegativeLimit = (double)lYNegLmt - MAX_DWORD;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dNegativeLimit = (double)lZNegLmt - MAX_DWORD;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dNegativeLimit = (double)lUNegLmt - MAX_DWORD;
	}

	    // read the initial setup --- HomeOffset
	DWORD lXHomeOffset, lYHomeOffset, lZHomeOffset, lUHomeOffset;
    *ulErrCode = (ULONG)P1240MotRdMutiReg(uiCurrBoardId, XYZU_Axis, HomeOffset, &lXHomeOffset, &lYHomeOffset, &lZHomeOffset, &lUHomeOffset);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_upload_parameters_ads1240;
    }
	else
	{
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeOffset = (double)lXHomeOffset;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeOffset = (double)lYHomeOffset;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeOffset = (double)lZHomeOffset;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeOffset = (double)lUHomeOffset;
	}

	    // read the initial setup --- HomeMode
	DWORD lXHomeMode, lYHomeMode, lZHomeMode, lUHomeMode;
    *ulErrCode = (ULONG)P1240MotRdMutiReg(uiCurrBoardId, XYZU_Axis, HomeMode, &lXHomeMode, &lYHomeMode, &lZHomeMode, &lUHomeMode);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_upload_parameters_ads1240;
    }
	else
	{
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeMode = (double)lXHomeMode;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeMode = (double)lYHomeMode;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeMode = (double)lZHomeMode;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeMode = (double)lUHomeMode;
	}

	    // read the initial setup --- HomeType
	DWORD lXHomeType, lYHomeType, lZHomeType, lUHomeType;
    *ulErrCode = (ULONG)P1240MotRdMutiReg(uiCurrBoardId, XYZU_Axis, HomeType, &lXHomeType, &lYHomeType, &lZHomeType, &lUHomeType);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_upload_parameters_ads1240;
    }
	else
	{
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeType = (double)lXHomeType;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeType = (double)lYHomeType;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeType = (double)lZHomeType;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeType = (double)lUHomeType;
	}

	    // read the initial setup --- HomeP0_Dir
	DWORD lXHomeP0_Dir, lYHomeP0_Dir, lZHomeP0_Dir, lUHomeP0_Dir;
    *ulErrCode = (ULONG)P1240MotRdMutiReg(uiCurrBoardId, XYZU_Axis, HomeP0_Dir, &lXHomeP0_Dir, &lYHomeP0_Dir, &lZHomeP0_Dir, &lUHomeP0_Dir);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_upload_parameters_ads1240;
    }
	else
	{
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeP0_Dir = (double)lXHomeP0_Dir;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeP0_Dir = (double)lYHomeP0_Dir;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeP0_Dir = (double)lZHomeP0_Dir;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeP0_Dir = (double)lUHomeP0_Dir;
	}

	    // read the initial setup --- HomeP0_Speed
	DWORD lXHomeP0_Speed, lYHomeP0_Speed, lZHomeP0_Speed, lUHomeP0_Speed;
    *ulErrCode = (ULONG)P1240MotRdMutiReg(uiCurrBoardId, XYZU_Axis, HomeP0_Speed, &lXHomeP0_Speed, &lYHomeP0_Speed, &lZHomeP0_Speed, &lUHomeP0_Speed);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR;goto label_return_mtnstp_upload_parameters_ads1240;
    }
	else
	{
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeP0_Vel = (double)lXHomeP0_Speed;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeP0_Vel = (double)lYHomeP0_Speed;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeP0_Vel = (double)lZHomeP0_Speed;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeP0_Vel = (double)lUHomeP0_Speed;
	}

	    // read the initial setup --- HomeP1_Dir
	DWORD lXHomeP1_Dir, lYHomeP1_Dir, lZHomeP1_Dir, lUHomeP1_Dir;
    *ulErrCode = (ULONG)P1240MotRdMutiReg(uiCurrBoardId, XYZU_Axis, HomeP1_Dir, &lXHomeP1_Dir, &lYHomeP1_Dir, &lZHomeP1_Dir, &lUHomeP1_Dir);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR;goto label_return_mtnstp_upload_parameters_ads1240;
    }
	else
	{
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeP1_Dir = (double)lXHomeP1_Dir;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeP1_Dir = (double)lYHomeP1_Dir;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeP1_Dir = (double)lZHomeP1_Dir;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeP1_Dir = (double)lUHomeP1_Dir;
	}

	    // read the initial setup --- HomeP1_Speed
	DWORD lXHomeP1_Speed, lYHomeP1_Speed, lZHomeP1_Speed, lUHomeP1_Speed;
    *ulErrCode = (ULONG)P1240MotRdMutiReg(uiCurrBoardId, XYZU_Axis, HomeP1_Speed, &lXHomeP1_Speed, &lYHomeP1_Speed, &lZHomeP1_Speed, &lUHomeP1_Speed);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_upload_parameters_ads1240;
    }
	else
	{
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeP1_Vel = (double)lXHomeP1_Speed;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeP1_Vel = (double)lYHomeP1_Speed;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeP1_Vel = (double)lZHomeP1_Speed;
		astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeP1_Vel = (double)lUHomeP1_Speed;
	}

label_return_mtnstp_upload_parameters_ads1240:

	return iRet;

}

// download parameter to ads1240 PCI-Stepper
int mtnstp_download_parameters_ads1240(BYTE uiCurrBoardId, DWORD *ulErrCode)
{
	int iRet = MTN_API_OK_ZERO;
	unsigned long ulXSV, ulYSV, ulZSV, ulUSV;
	unsigned long ulXDV, ulYDV, ulZDV, ulUDV;
	unsigned long ulXMDV, ulYMDV, ulZMDV, ulUMDV;
	unsigned long ulXAC, ulYAC, ulZAC, ulUAC;
	unsigned long ulXJerk, ulYJerk, ulZJerk, ulUJerk;

	*ulErrCode = 0;

    // write the initial setup ---Start_Velocity
	ulXSV =	(DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dStepperStartVelocity;
	ulYSV = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dStepperStartVelocity;
	ulZSV = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dStepperStartVelocity;
	ulUSV = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dStepperStartVelocity;
    *ulErrCode = (ULONG)P1240MotWrMutiReg(uiCurrBoardId, XYZU_Axis, SV, ulXSV, ulYSV, ulZSV, ulUSV);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_download_parameters_ads1240;
    }

    // write the initial setup ---Drive_Velocity        
	ulXDV = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dStepperDriveVelocity;
	ulYDV = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dStepperDriveVelocity;
	ulZDV = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dStepperDriveVelocity;
	ulUDV = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dStepperDriveVelocity;
    *ulErrCode = (ULONG)P1240MotWrMutiReg(uiCurrBoardId, XYZU_Axis, DV, ulXDV, ulYDV, ulZDV, ulUDV);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_download_parameters_ads1240;
    }

    // write the initial setup ---Max Drive_Velocity        
	ulXMDV = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dMaxVelocity;
	ulYMDV = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dMaxVelocity;
	ulZMDV = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dMaxVelocity;
	ulUMDV = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dMaxVelocity;
    *ulErrCode = (ULONG)P1240MotWrMutiReg(uiCurrBoardId, XYZU_Axis, MDV, ulXMDV, ulYMDV, ulZMDV, ulUMDV);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_download_parameters_ads1240;
    }

    // write the initial setup ---Max Acc        
	ulXAC = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dMaxAcceleration;
	ulYAC = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dMaxAcceleration;
	ulZAC = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dMaxAcceleration;
	ulUAC = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dMaxAcceleration;
    *ulErrCode = (ULONG)P1240MotWrMutiReg(uiCurrBoardId, XYZU_Axis, AC, ulXAC, ulYAC, ulZAC, ulUAC);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_download_parameters_ads1240;
    }

    // write the initial setup ---Max Jerk        
	ulXJerk = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dMaxJerk;
	ulYJerk = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dMaxJerk;
	ulZJerk = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dMaxJerk;
	ulUJerk = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dMaxJerk;
    *ulErrCode = (ULONG)P1240MotWrMutiReg(uiCurrBoardId, XYZU_Axis, AK, ulXJerk, ulYJerk, ulZJerk, ulUJerk);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_download_parameters_ads1240;
    }

	DWORD lXPosLmt, lYPosLmt, lZPosLmt, lUPosLmt;
    // write the initial setup --- Positive Limit      
	lXPosLmt = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dPositiveLimit;
	lYPosLmt = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dPositiveLimit;
	lZPosLmt = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dPositiveLimit;
	lUPosLmt = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dPositiveLimit;
    *ulErrCode = (ULONG)P1240MotWrMutiReg(uiCurrBoardId, XYZU_Axis, PLmt, lXPosLmt, lYPosLmt, lZPosLmt, lUPosLmt);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_download_parameters_ads1240;
    }

	// read the initial setup --- Negative Limit      
	DWORD lXNegLmt, lYNegLmt, lZNegLmt, lUNegLmt;
	lXNegLmt = (DWORD)(astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dNegativeLimit + MAX_DWORD);
	lYNegLmt = (DWORD)(astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dNegativeLimit + MAX_DWORD);
	lZNegLmt = (DWORD)(astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dNegativeLimit + MAX_DWORD);
	lUNegLmt = (DWORD)(astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dNegativeLimit + MAX_DWORD);
    *ulErrCode = (ULONG)P1240MotWrMutiReg(uiCurrBoardId, XYZU_Axis, NLmt, lXNegLmt, lYNegLmt, lZNegLmt, lUNegLmt);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_download_parameters_ads1240;
    }

	DWORD lXHomeOffset, lYHomeOffset, lZHomeOffset, lUHomeOffset;
	// write the initial setup --- HomeOffset
	lXHomeOffset = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeOffset;
	lYHomeOffset = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeOffset;
	lZHomeOffset = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeOffset;
	lUHomeOffset = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeOffset;
    *ulErrCode = (ULONG)P1240MotWrMutiReg(uiCurrBoardId, XYZU_Axis, HomeOffset, lXHomeOffset, lYHomeOffset, lZHomeOffset, lUHomeOffset);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_download_parameters_ads1240;
    }

	DWORD lXHomeMode, lYHomeMode, lZHomeMode, lUHomeMode;
	    // write the initial setup --- HomeMode
	lXHomeMode = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeMode;
	lYHomeMode = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeMode;
	lZHomeMode = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeMode;
	lUHomeMode = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeMode;
    *ulErrCode = (ULONG)P1240MotWrMutiReg(uiCurrBoardId, XYZU_Axis, HomeMode, lXHomeMode, lYHomeMode, lZHomeMode, lUHomeMode);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_download_parameters_ads1240;
    }

	DWORD lXHomeType, lYHomeType, lZHomeType, lUHomeType;
	    // write the initial setup --- HomeType
	lXHomeType = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeType;
	lYHomeType = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeType;
	lZHomeType = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeType;
	lUHomeType = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeType;
    *ulErrCode = (ULONG)P1240MotWrMutiReg(uiCurrBoardId, XYZU_Axis, HomeType, lXHomeType, lYHomeType, lZHomeType, lUHomeType);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_download_parameters_ads1240;
    }

	DWORD lXHomeP0_Dir, lYHomeP0_Dir, lZHomeP0_Dir, lUHomeP0_Dir;
	    // write the initial setup --- HomeP0_Dir
	lXHomeP0_Dir = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeP0_Dir;
	lYHomeP0_Dir = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeP0_Dir;
	lZHomeP0_Dir = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeP0_Dir;
	lUHomeP0_Dir = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeP0_Dir;
    *ulErrCode = (ULONG)P1240MotWrMutiReg(uiCurrBoardId, XYZU_Axis, HomeP0_Dir, lXHomeP0_Dir, lYHomeP0_Dir, lZHomeP0_Dir, lUHomeP0_Dir);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_download_parameters_ads1240;
    }

	DWORD lXHomeP0_Speed, lYHomeP0_Speed, lZHomeP0_Speed, lUHomeP0_Speed;
	    // write the initial setup --- HomeP0_Speed
	lXHomeP0_Speed = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeP0_Vel;
	lYHomeP0_Speed = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeP0_Vel;
	lZHomeP0_Speed = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeP0_Vel;
	lUHomeP0_Speed = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeP0_Vel;
    *ulErrCode = (ULONG)P1240MotWrMutiReg(uiCurrBoardId, XYZU_Axis, HomeP0_Speed, lXHomeP0_Speed, lYHomeP0_Speed, lZHomeP0_Speed, lUHomeP0_Speed);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR;goto label_return_mtnstp_download_parameters_ads1240;
    }

	DWORD lXHomeP1_Dir, lYHomeP1_Dir, lZHomeP1_Dir, lUHomeP1_Dir;
	    // write the initial setup --- HomeP1_Dir
	lXHomeP1_Dir = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeP1_Dir;
	lYHomeP1_Dir = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeP1_Dir;
	lZHomeP1_Dir = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeP1_Dir;
	lUHomeP1_Dir = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeP1_Dir;
    *ulErrCode = (ULONG)P1240MotWrMutiReg(uiCurrBoardId, XYZU_Axis, HomeP1_Dir, lXHomeP1_Dir, lYHomeP1_Dir, lZHomeP1_Dir, lUHomeP1_Dir);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR;goto label_return_mtnstp_download_parameters_ads1240;
    }

	DWORD lXHomeP1_Speed, lYHomeP1_Speed, lZHomeP1_Speed, lUHomeP1_Speed;
	    // write the initial setup --- HomeP1_Speed
	lXHomeP1_Speed = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeP1_Vel;
	lYHomeP1_Speed = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeP1_Vel;
	lZHomeP1_Speed = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeP1_Vel;
	lUHomeP1_Speed = (DWORD)astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeP1_Vel;
    *ulErrCode = (ULONG)P1240MotWrMutiReg(uiCurrBoardId, XYZU_Axis, HomeP1_Speed, lXHomeP1_Speed, lYHomeP1_Speed, lZHomeP1_Speed, lUHomeP1_Speed);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_download_parameters_ads1240;
    }

	*ulErrCode = (ULONG)P1240MotSavePara(uiCurrBoardId, XYZU_Axis);
    if( *ulErrCode != 0)
    {
		iRet = MTN_API_ERROR; goto label_return_mtnstp_download_parameters_ads1240;
    }

label_return_mtnstp_download_parameters_ads1240:

	return iRet;

}