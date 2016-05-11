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


// 20090908 Replace Sleep to be high_precision_sleep_ms

#include "stdafx.h"
#include "acsc.h"
#include "ForceCali.h"
#include "math.h"
#include "WinTiming.h"
#include "acs_buff_prog.h"

double dForceRatio = DEF_FORCE_GRAM_PER_ADC;  // 20121218
double mtn_get_force_ratio()
{
	return dForceRatio;
}
int mtn_set_force_ratio(double dNewRatio)
{
int iRet = MTN_API_OK_ZERO;

	if(dNewRatio < DEF_FORCE_GRAM_PER_ADC_UPP
		|| dNewRatio > DEF_FORCE_GRAM_PER_ADC_LOW)
	{
		dForceRatio = dNewRatio;
	}
	else
	{
		iRet = MTN_EROR_FORCE_RATIO_OUT_RANGE;
	}

	return iRet;

}

static char strDebugText[128];

// Download parameter
int acsc_download_search_contact_parameter(HANDLE m_hHandle, TEACH_CONTACT_INPUT *stpTeachContact)
{

	int iRet = MTN_API_OK_ZERO;
	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "AXIS", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iAxis, NULL ))
	{
		sprintf_s(strDebugText, 128, "download AXIS error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_download_search_contact_para;
	}
	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "SRCH_HEIGHT_POSN", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iSearchHeightPosition, NULL ))
	{
		sprintf_s(strDebugText, 128, "download SrchHt Position error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_download_search_contact_para;
	}
	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iMaxAcc", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iMaxAccMoveSrchHt, NULL ))
	{
		sprintf_s(strDebugText, 128, "download max acc error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_download_search_contact_para;
	}
	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iMaxJerk", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iMaxJerkMoveSrchHt, NULL ))
	{
		sprintf_s(strDebugText, 128, "download max acc error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_download_search_contact_para;
	}
	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "SEARCH_VEL", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iSearchVel, NULL ))
	{
		sprintf_s(strDebugText, 128, "download trajectory error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_download_search_contact_para;
	}
	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "TARGET_POINT", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iResetPosition, NULL ))
	{
		sprintf_s(strDebugText, 128, "download trajectory error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_download_search_contact_para;
	}
	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "ANTIBOUNCE", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iAntiBounce, NULL ))
	{
		sprintf_s(strDebugText, 128, "download trajectory error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_download_search_contact_para;
	}
	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "MAX_DIST", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iMaxDist, NULL ))
	{
		sprintf_s(strDebugText, 128, "download trajectory error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_download_search_contact_para;
	}
	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "SEARCH_TOL_PE", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iSearchTolPE, NULL ))
	{
		sprintf_s(strDebugText, 128, "download trajectory error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_download_search_contact_para;
	}
	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "TIME_OUT", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iTimeOut, NULL ))
	{
		sprintf_s(strDebugText, 128, "download trajectory error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_download_search_contact_para;
	}
	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iFlagSwitchToForceControl", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iFlagSwitchToForceControl, NULL ))
	{
		sprintf_s(strDebugText, 128, "download FlagSwitchToForceControl error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_download_search_contact_para;
	}

	// 20121030
	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "gnDetect", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->cFlagDetectionSP, NULL ))
	{
		sprintf_s(strDebugText, 128, "download detection flag, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_download_search_contact_para;
	}

label_return_download_search_contact_para:

	return iRet;
}

// Upload parameter
int acsc_upload_search_contact_parameter(HANDLE m_hHandle, TEACH_CONTACT_INPUT *stpTeachContact)
{
	int iRet = MTN_API_OK_ZERO;

	//int iTemp[1];
	//if (!acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "AXIS", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iAxis, NULL ))
	//{
	//	sprintf_s(strDebugText, 128, "upload AXIS error, Error Code: %d", acsc_GetLastError());
	//	AfxMessageBox(_T(strDebugText));
	//	return;
	//}
	stpTeachContact->iAxis = 4; // Temperary ?? iTemp[0];

	if (!acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "SRCH_HEIGHT_POSN", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iSearchHeightPosition, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload SRCH_HEIGHT_POSN error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_search_contact_para;
	}
	if (!acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iMaxAcc", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iMaxAccMoveSrchHt, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload MAX_ACC error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_search_contact_para;
	}
	if (!acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iMaxJerk", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iMaxJerkMoveSrchHt, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload MAX_JERK error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_search_contact_para;
	}
	if (!acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "SEARCH_VEL", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iSearchVel, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload SEARCH_VEL error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_search_contact_para;
	}
	if (!acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "TARGET_POINT", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iResetPosition, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload TARGET_POINT error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_search_contact_para;
	}
	if (!acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "ANTIBOUNCE", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iAntiBounce, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload ANTIBOUNCE error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_search_contact_para;
	}
	if (!acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "MAX_DIST", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iMaxDist, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload MAX_DIST error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_search_contact_para;
	}
	if (!acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "SEARCH_TOL_PE", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iSearchTolPE, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload SEARCH_TOL_PE error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_search_contact_para;
	}
	if (!acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "TIME_OUT", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iTimeOut, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload TIME_OUT error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_search_contact_para;
	}

	if (!acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iFlagSwitchToForceControl", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->iFlagSwitchToForceControl, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload FlagSwitchToForceControl error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_search_contact_para;
	}

	// 20121030
	if (!acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "gnDetect", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContact->cFlagDetectionSP, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload detection flag, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_search_contact_para;
	}

label_return_upload_search_contact_para:
	return iRet;
}

int acsc_upload_search_contact_result(HANDLE m_hHandle, TEACH_CONTACT_OUTPUT *stpTeachContactResult)
{
int iRet;

	if (!acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "CONTACT_POSN_REG", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContactResult->iContactPosnReg, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload CONTACT_POSN_REG error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_search_contact_result;
	}
	if (!acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "SEARCH_CONTACT_STATUS", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContactResult->iStatus, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload SEARCH_CONTACT_STATUS error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_search_contact_result;
	}
	// dInitForceCommandReadBack
	if (!acsc_ReadReal(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "dPreImpForce_DCOM", 0, 0, ACSC_NONE, ACSC_NONE, &stpTeachContactResult->dInitForceCommandReadBack, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload dPreImpForce_DCOM error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_search_contact_result;
	}
label_return_upload_search_contact_result:
	return iRet;
}

int mtn_teach_contact_acs(HANDLE m_hHandle, TEACH_CONTACT_INPUT *stpTeachContactParameter, TEACH_CONTACT_OUTPUT *stpTeachContactResult)
{
	int iRet;
	MTN_SPEED_PROFILE stSpeedProfileBak_Z;

	// Upload and backup speed profile
	mtnapi_get_speed_profile(m_hHandle, stpTeachContactParameter->iAxis, &stSpeedProfileBak_Z, 0);

//	mtnapi_set_speed_profile(m_hHandle, stpTeachContactParameter->iAxis, &stpTeachContactParameter->stSpeedProfileTeachContact, 0);

	acsc_download_search_contact_parameter(m_hHandle, stpTeachContactParameter);

	//acsc_WriteDPRAMInteger(m_hHandle, DPRAM_ADDR_START_BUF_7, 1);
	acs_write_srch_contact_buffer_auto_tune_flag(m_hHandle, 0); // 20120203
	int iTempTrigger;
	iTempTrigger = 1;
	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iAFT_StartSearchContact_buffer7", 0, 0, ACSC_NONE, ACSC_NONE, &iTempTrigger, NULL ))
	{
		sprintf_s(strDebugText, 128, "download start-flag error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_teach_contact;
	}

	Sleep(500);
	int iFlagFinishedSearch = 0; // iAFT_DoneSearchContact_buffer7 @ DPRAM_ADDR_DONE_SEARCH_CONTACT
//	acsc_ReadDPRAMInteger(m_hHandle, DPRAM_ADDR_DONE_SEARCH_CONTACT, &iFlagFinishedSearch);
	acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iAFT_DoneSearchContact_buffer7", 0, 0, ACSC_NONE, ACSC_NONE, &iFlagFinishedSearch, NULL );
	if(iFlagFinishedSearch == 0)
	{
		Sleep(100);
		acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iAFT_DoneSearchContact_buffer7", 0, 0, ACSC_NONE, ACSC_NONE, &iFlagFinishedSearch, NULL );
	}

	acsc_upload_search_contact_result(m_hHandle, stpTeachContactResult);
	mtnapi_set_speed_profile(m_hHandle, stpTeachContactParameter->iAxis, &stSpeedProfileBak_Z, 0);

	if(stpTeachContactResult->iStatus == 0)
	{
		iRet = MTN_API_OK_ZERO;
	}
	else
	{
		iRet = MTN_API_ERROR;
	}

label_return_teach_contact:
	return iRet;
}

int acsc_download_force_control_blk(HANDLE m_hHandle, FORCE_BLOCK_ACS *stpForceBlock)
{
	int iRet = MTN_API_OK_ZERO;

	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iHoldCountInitForce", 
		0, 0, ACSC_NONE, ACSC_NONE, &stpForceBlock->iInitForceHold_cnt, NULL ))
	{
		sprintf_s(strDebugText, 128, "download iHoldCountInitForce, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_download_force_control_blk;
	}
	if (!acsc_WriteReal(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "dPreImpForce_DCOM", 
		0, 0, ACSC_NONE, ACSC_NONE, &stpForceBlock->dPreImpForce_DCOM, NULL ))
	{
		sprintf_s(strDebugText, 128, "download InitForceCommand, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_download_force_control_blk;
	}

	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "nMaxForceSeg", 
		0, 0, ACSC_NONE, ACSC_NONE, (int*)&stpForceBlock->uiNumSegment, NULL ))
	{
		sprintf_s(strDebugText, 128, "download number of seg, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_download_force_control_blk;
	}

	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iForceBlk_RampCount", 
		0, stpForceBlock->uiNumSegment - 1, ACSC_NONE, ACSC_NONE, stpForceBlock->aiForceBlk_Rampcount, NULL ))
	{
		sprintf_s(strDebugText, 128, "download ramp count, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_download_force_control_blk;
	}

	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iForceBlk_LevelCount", 
		0, stpForceBlock->uiNumSegment - 1, ACSC_NONE, ACSC_NONE, stpForceBlock->aiForceBlk_LevelCount, NULL ))
	{
		sprintf_s(strDebugText, 128, "download level count error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_download_force_control_blk;
	}

	if (!acsc_WriteReal(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "fForceBlk_LevelAmplitude", 
		0, stpForceBlock->uiNumSegment - 1, ACSC_NONE, ACSC_NONE, stpForceBlock->adForceBlk_LevelAmplitude, NULL ))
	{
		sprintf_s(strDebugText, 128, "download level amplitude error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_download_force_control_blk;
	}

label_return_download_force_control_blk:
	return iRet;

}

int acsc_upload_force_control_blk(HANDLE m_hHandle, FORCE_BLOCK_ACS *stpForceBlock)
{
	int iRet = MTN_API_OK_ZERO;

	if (!acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iHoldCountInitForce", 
		0, 0, ACSC_NONE, ACSC_NONE, &stpForceBlock->iInitForceHold_cnt, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload iHoldCountInitForce, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_force_control_blk;
	}
	if (!acsc_ReadReal(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "dPreImpForce_DCOM", 
		0, 0, ACSC_NONE, ACSC_NONE, &stpForceBlock->dPreImpForce_DCOM, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload InitForceCommand, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_force_control_blk;
	}
	if (!acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "nMaxForceSeg", 
		0, 0, ACSC_NONE, ACSC_NONE, (int*)&stpForceBlock->uiNumSegment, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload number of seg, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_force_control_blk;
	}

	if (!acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iForceBlk_RampCount", 
		0, stpForceBlock->uiNumSegment - 1, ACSC_NONE, ACSC_NONE, stpForceBlock->aiForceBlk_Rampcount, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload ramp count, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_force_control_blk;
	}

	if (!acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iForceBlk_LevelCount", 
		0, stpForceBlock->uiNumSegment - 1, ACSC_NONE, ACSC_NONE, stpForceBlock->aiForceBlk_LevelCount, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload level count error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_force_control_blk;
	}

	if (!acsc_ReadReal(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "fForceBlk_LevelAmplitude", 
		0, stpForceBlock->uiNumSegment - 1, ACSC_NONE, ACSC_NONE, stpForceBlock->adForceBlk_LevelAmplitude, NULL ))
	{
		sprintf_s(strDebugText, 128, "upload level amplitude error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_upload_force_control_blk;
	}

label_return_upload_force_control_blk:
	return iRet;

}

int mtn_search_contract_and_force_control(HANDLE m_hHandle, 
										  SEARCH_CONTACT_AND_FORCE_CONTROL_INPUT *stpSearchContactAndForceControl, 
										  TEACH_CONTACT_OUTPUT *stpTeachContactResult)
{
	int iRet;
	MTN_SPEED_PROFILE stSpeedProfileBak_Z;
	FORCE_BLOCK_ACS stForceBlkVerify;

	// Upload and backup speed profile
	mtnapi_get_speed_profile(m_hHandle, stpSearchContactAndForceControl->stpTeachContactPara->iAxis, &stSpeedProfileBak_Z, 0);

	mtnapi_set_speed_profile(m_hHandle, 
		stpSearchContactAndForceControl->stpTeachContactPara->iAxis, 
		&stpSearchContactAndForceControl->stpTeachContactPara->stSpeedProfileTeachContact, 0);

	acsc_download_search_contact_parameter(m_hHandle, 
		stpSearchContactAndForceControl->stpTeachContactPara);

//	stForceBlock = stBakForceBlock;
	acsc_download_force_control_blk(m_hHandle, stpSearchContactAndForceControl->stpForceBlkPara);
	acsc_upload_force_control_blk(m_hHandle, &stForceBlkVerify);

	Sleep(20);
//	acsc_WriteDPRAMInteger(m_hHandle, DPRAM_ADDR_START_BUF_7, 1);

	acs_write_srch_contact_buffer_auto_tune_flag(m_hHandle, 0); // 20120203
	int iTempTrigger;
	iTempTrigger = 1;
	if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iAFT_StartSearchContact_buffer7", 0, 0, ACSC_NONE, ACSC_NONE, &iTempTrigger, NULL ))
	{
		sprintf_s(strDebugText, 128, "download start-flag error, Error Code: %d", acsc_GetLastError());
		iRet = MTN_API_ERROR; goto label_return_search_contract_and_force_control;
	}

	Sleep(20);
	int iFlagFinishedSearch; // iAFT_DoneSearchContact_buffer7 @ DPRAM_ADDR_DONE_SEARCH_CONTACT
//	acsc_ReadDPRAMInteger(m_hHandle, DPRAM_ADDR_DONE_SEARCH_CONTACT, &iFlagFinishedSearch);
//	if(iFlagFinishedSearch == 0)
//	{
//		Sleep(20);
//		acsc_ReadDPRAMInteger(m_hHandle, DPRAM_ADDR_DONE_SEARCH_CONTACT, &iFlagFinishedSearch);
//	}
	acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iAFT_DoneSearchContact_buffer7", 0, 0, ACSC_NONE, ACSC_NONE, &iFlagFinishedSearch, NULL );
	if(iFlagFinishedSearch == 0)
	{
		Sleep(20);
		acsc_ReadInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "iAFT_DoneSearchContact_buffer7", 0, 0, ACSC_NONE, ACSC_NONE, &iFlagFinishedSearch, NULL );
	}

	acsc_upload_search_contact_result(m_hHandle, stpTeachContactResult);
	mtnapi_set_speed_profile(m_hHandle, stpSearchContactAndForceControl->stpTeachContactPara->iAxis, &stSpeedProfileBak_Z, 0);

	if(stpTeachContactResult->iStatus = 0)
	{
		iRet = MTN_API_OK_ZERO;
	}
	else
	{
		iRet = MTN_API_ERROR;
	}

label_return_search_contract_and_force_control:
	return iRet;

}



double calc_mean(double *dInputData, int nLen)
{
	double dSum = 0;
	for(int ii = 0; ii<nLen; ii++)
	{
		dSum = dSum + dInputData[ii];
	}

	if(nLen > 0)
		return dSum/nLen;
	else
		return dInputData[0];
}

#include "MotAlgo_DLL.h"
#include "MtnInitAcs.h"
/////////////////
static unsigned int aStartIndexPerSeg[MAX_FORCE_SEG];
static unsigned int aEndIndexPerSeg[MAX_FORCE_SEG];
static double aForceCmdMean_dcom[MAX_FORCE_SEG];
static double aForceFbMean_adc[MAX_FORCE_SEG];
static double aForceFbMean_gram[MAX_FORCE_SEG];
//double aMatrix_A [ MAX_FORCE_SEG * 2];
//double aMatrix_A_T [ MAX_FORCE_SEG * 2];
//double aRHS_A [MAX_FORCE_SEG];

#define MATRIX_COL_2       2
#define MATRIX_ROW_2       2
static double aMatrix_Sq[ MATRIX_COL_2 * MATRIX_ROW_2];
static double aRHS_Sq [MATRIX_ROW_2];
static double aResult[MATRIX_ROW_2];

int mtn_force_calibration(HANDLE m_hHandle, FORCE_CALIBRATION_INPUT *stpForceCalibrationInput, FORCE_CALIBRATION_OUTPUT *stpForceCaliOutput)
{
	int iRet = MTN_API_OK_ZERO;


	// Step-1: Get contact position
	// Step-: First teach contact
	TEACH_CONTACT_OUTPUT stTeachContactResult;
	stpForceCalibrationInput->stTeachContactPara.iFlagSwitchToForceControl = 0;
	stpForceCalibrationInput->stTeachContactPara.iFlagDebug = stpForceCalibrationInput->iFlagDebug;
	if(mtn_teach_contact_acs(m_hHandle, &stpForceCalibrationInput->stTeachContactPara, &stTeachContactResult) != MTN_API_OK_ZERO)
	{
		iRet = MTN_API_ERROR;
		goto label_return_mtn_force_calibration;
	}
	int iReadContactPosition = stTeachContactResult.iContactPosnReg;
	stpForceCalibrationInput->stTeachContactPara.iFlagSwitchToForceControl = 1;

	// Step-2: First perform position compensation, from lower position to upper position then stay at upper position
	double dLowerPosition, dUpperPosition;
	MTUNE_OUT_POSN_COMPENSATION stOutputPosnCompensationTune;
	dLowerPosition = iReadContactPosition + 500; // stpForceCalibrationInput->stPosnCompensate.dLowerPosnLimit;
	dUpperPosition =  mtn_wb_init_bh_relax_position_from_sp(m_hHandle);  // mtn_wb_get_bh_upper_limit_position();  // 20110711 //stpForceCalibrationInput->stPosnCompensate.dUpperPosnLimit;
	if( dLowerPosition > dUpperPosition)
	{
		stpForceCalibrationInput->stPosnCompensate.dLowerPosnLimit = dUpperPosition;
		stpForceCalibrationInput->stPosnCompensate.dUpperPosnLimit = dLowerPosition;
	}
	else // 20130108
	{
		stpForceCalibrationInput->stPosnCompensate.dLowerPosnLimit = dLowerPosition;
		stpForceCalibrationInput->stPosnCompensate.dUpperPosnLimit = dUpperPosition;
	}
	stpForceCalibrationInput->stPosnCompensate.iDebug = stpForceCalibrationInput->iFlagDebug;
	if(mtune_position_compensation(m_hHandle, stpForceCalibrationInput->stTeachContactPara.iAxis, 
		&(stpForceCalibrationInput->stPosnCompensate), &stOutputPosnCompensationTune) != MTN_API_OK_ZERO)
	{
		iRet = MTN_API_ERROR;
		goto label_return_mtn_force_calibration;
	}
	double dPosnFactor_Ka = fabs(stOutputPosnCompensationTune.dPositionFactor)/327.67;  // 20130102

	double dDriveCommand_byContactPosn = dPosnFactor_Ka * iReadContactPosition;  // 20130102

	mtn_api_disable_spring_compensation_sp_para(m_hHandle); // 20130101

	// Step-3: then apply force control and get Kf
	FORCE_BLOCK_ACS stForceControlBlk;
	unsigned int ii, jj, nTotalSeg;
	nTotalSeg = stpForceCalibrationInput->stForceCaliPara.uiNumSegment;
	if( nTotalSeg >= MAX_FORCE_SEG)
	{
		nTotalSeg = MAX_FORCE_SEG - 1;
	}

	// Protection of input range
	stForceControlBlk.uiNumSegment = nTotalSeg + 1;
	stForceControlBlk.dPreImpForce_DCOM = -1.2; // stpForceCalibrationInput->stForceCaliPara.dPreImpForce_DCOM;
	stForceControlBlk.iInitForceHold_cnt = 0; // stpForceCalibrationInput->stForceCaliPara.iInitForceHold_cnt;
	if(stpForceCalibrationInput->stForceCaliPara.dFirstForce_gram > DEF_MAX_FORCE_GRAM)
	{
		stpForceCalibrationInput->stForceCaliPara.dFirstForce_gram = DEF_MAX_FORCE_GRAM;
	}
	if(stpForceCalibrationInput->stForceCaliPara.dFirstForce_gram < DEF_MIN_FORCE_GRAM)
	{
		stpForceCalibrationInput->stForceCaliPara.dFirstForce_gram = DEF_MIN_FORCE_GRAM;
	}
	if(stpForceCalibrationInput->stForceCaliPara.dLastForce_gram > DEF_MAX_FORCE_GRAM)
	{
		stpForceCalibrationInput->stForceCaliPara.dLastForce_gram = DEF_MAX_FORCE_GRAM;
	}
	if(stpForceCalibrationInput->stForceCaliPara.dLastForce_gram < DEF_MIN_FORCE_GRAM)
	{
		stpForceCalibrationInput->stForceCaliPara.dLastForce_gram = DEF_MIN_FORCE_GRAM;
	}

	// Setup stForceControlBlk
	double dFirstForce_gram, dDeltaForce_gram, dDefForceFactor_Kf, dDefForceOffset_i0, dDefPosnFactor_Ka;
	dFirstForce_gram = stpForceCalibrationInput->stForceCaliPara.dFirstForce_gram;
	dDeltaForce_gram = (stpForceCalibrationInput->stForceCaliPara.dLastForce_gram - dFirstForce_gram)/(nTotalSeg  - 1);
	dDefForceFactor_Kf = stpForceCalibrationInput->stForceCaliPara.dDefForceFactor_Kf;
	dDefForceOffset_i0 = stpForceCalibrationInput->stForceCaliPara.dDefForceOffset_I0;
	dDefPosnFactor_Ka = stpForceCalibrationInput->stForceCaliPara.dDefPosnFactor_Ka;
	for(ii = 0; ii< nTotalSeg; ii ++)
	{	
		stForceControlBlk.adForceBlk_LevelAmplitude[ii] = 
			-fabs((dFirstForce_gram + ii* dDeltaForce_gram) * dDefForceFactor_Kf)   // 20130101
			+ dDefForceOffset_i0 + iReadContactPosition * dDefPosnFactor_Ka;
		stForceControlBlk.aiForceBlk_LevelCount[ii] = 20;
		stForceControlBlk.aiForceBlk_Rampcount[ii] = 5;
	}
#ifndef __USE_GRAM__
	double dStepDOCM = stpForceCalibrationInput->stForceCaliPara.dStepDCOM;
	for(ii = 0; ii< nTotalSeg; ii ++)
	{	
		stForceControlBlk.adForceBlk_LevelAmplitude[ii] = stForceControlBlk.dPreImpForce_DCOM - ii * dStepDOCM;
		stForceControlBlk.aiForceBlk_LevelCount[ii] = 30;
		stForceControlBlk.aiForceBlk_Rampcount[ii] = 5;
	}
#endif
	// Add one last segment to recover to 0 gram
	stForceControlBlk.adForceBlk_LevelAmplitude[nTotalSeg] = 0 * dDefForceFactor_Kf + dDefForceOffset_i0 + iReadContactPosition * dDefPosnFactor_Ka;
	stForceControlBlk.aiForceBlk_LevelCount[nTotalSeg] = 50;// 20150124
	stForceControlBlk.aiForceBlk_Rampcount[nTotalSeg] = 5;

	// Setup stSearchContactAndForceControlForceCali, stTeachContactResultForceCali
	SEARCH_CONTACT_AND_FORCE_CONTROL_INPUT stSearchContactAndForceControlForceCali;
	TEACH_CONTACT_OUTPUT stTeachContactResultForceCali;
	
	stSearchContactAndForceControlForceCali.stpTeachContactPara = &stpForceCalibrationInput->stTeachContactPara;
	stSearchContactAndForceControlForceCali.stpForceBlkPara = &stForceControlBlk;

	int iDampSP = 60; // 
	mtnapi_download_acs_sp_parameter_damp_switch(m_hHandle, iDampSP); // 20130101

	// Do once search-contact-force-control
	mtn_search_contract_and_force_control(m_hHandle, &stSearchContactAndForceControlForceCali, &stTeachContactResultForceCali);
	if(stTeachContactResultForceCali.iStatus != 0)
	{
		iRet = MTN_API_ERROR;
		goto label_return_mtn_force_calibration;
	}

	//// Do once search-contact-force-control
	//mtn_search_contract_and_force_control(m_hHandle, &stSearchContactAndForceControlForceCali, &stTeachContactResultForceCali);
	//if(stTeachContactResultForceCali.iStatus != 0)
	//{
	//	iRet = MTN_API_ERROR;
	//	goto label_return_mtn_force_calibration;
	//}

	// Upload data
	gstSystemScope.uiNumData = 6;
	gstSystemScope.uiDataLen = 5000;
	gstSystemScope.dSamplePeriod_ms = 0.5;

#define EPS_POSN_ERROR  (1E-6)
#define EPS_FORCE_CMD   (1E-2)
#define DEF_PE_ID_IN_TRACE              2
#define DEF_SEARCH_COUNT_DEBOUNCE       3
#define DEF_FORCE_CMD_ID_IN_TRACE       4
#define DEF_FORCE_FB_ID_IN_TRACE        5
#define DEF_REF_VEL_IN_TRACE        6

extern 	double sys_get_controller_ts();
	gstSystemScope.uiNumData = 7;
	gstSystemScope.uiDataLen = 4200;
	gstSystemScope.dSamplePeriod_ms = sys_get_controller_ts();

static	unsigned int iStartIdx = 0;
static	unsigned int nDataLen = gstSystemScope.uiDataLen;
static	unsigned int iEndIdx = nDataLen;
	double *aPosnErr = &gdScopeCollectData[ DEF_PE_ID_IN_TRACE * nDataLen];
	double *aForceCmd = &gdScopeCollectData[ DEF_FORCE_CMD_ID_IN_TRACE * nDataLen];
	double *aForceFb = &gdScopeCollectData[ DEF_FORCE_FB_ID_IN_TRACE * nDataLen];
	double *aDebounceCounter = &gdScopeCollectData[DEF_SEARCH_COUNT_DEBOUNCE * nDataLen];
	Sleep((int)(nDataLen * 0.5));

	if(mtnscope_upload_acsc_data_varname(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "rAFT_Scope") 
		!= MTN_API_OK_ZERO)
	{
		iRet = MTN_API_ERROR;
		goto label_return_mtn_force_calibration;
	}

	/// Save Data // input fpData stTeachContactResult, stForceControlBlk
#define FILE_NAME_CONTACT_FORCE_CALI  "ForceCalibration"
FILE *fpData = NULL;
char strFilename[512];
struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;
_time64(&stLongTime);
_localtime64_s(stpTime, &stLongTime);

	iStartIdx = 0;
	nDataLen = gstSystemScope.uiDataLen;

	if(stpForceCalibrationInput ->iFlagDebug == 1)
	{
		sprintf(strFilename, "%s_Y%dM%dD%d_H%dM%dS%d__Srv%2.1f_Ht%d_PETh%d.m", FILE_NAME_CONTACT_FORCE_CALI,
				stpTime->tm_year +1900, stpTime->tm_mon +1, stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, stpTime->tm_sec,
				fabs(stpForceCalibrationInput->stTeachContactPara.iSearchVel/-10000.0),
				(stpForceCalibrationInput->stTeachContactPara.iSearchHeightPosition - stTeachContactResult.iContactPosnReg),
				stpForceCalibrationInput->stTeachContactPara.iSearchTolPE); // 20121211

		fopen_s(&fpData, strFilename, "w");  // FILE_NAME_CONTACT_FORCE_CALI
	}
int iDampGA;  // 20130102
	if( fpData != NULL)
	{
		fprintf(fpData, "%% Position Calibration Output\n");
		fprintf(fpData, "dPositionFactor = %8.4f;", stOutputPosnCompensationTune.dPositionFactor);
		fprintf(fpData, "dCtrlOutOffset = %8.4f;\n", stOutputPosnCompensationTune.dCtrlOutOffset);
		mtnapi_upload_acs_sp_parameter_damp_switch(m_hHandle, &iDampGA);
		fprintf(fpData, "iDampACS = %d; \n", iDampGA);

		fprintf(fpData, "ForceRatio = %10.7f \n", mtn_get_force_ratio());

		mtn_debug_print_to_file(fpData, &stSearchContactAndForceControlForceCali, &stTeachContactResultForceCali);
	}
#define  SKIP_STARTING_MOVING   50
	/// Find the section of force control by condition s.t. PositionErr == 0
	for(ii = SKIP_STARTING_MOVING; ii<nDataLen - 1 ; ii ++)
	{
		if( (fabs(gdScopeCollectData[ DEF_PE_ID_IN_TRACE * nDataLen + ii]) < EPS_POSN_ERROR) 
			&& (fabs(gdScopeCollectData[ DEF_PE_ID_IN_TRACE * nDataLen + ii+1]) < EPS_POSN_ERROR) 
			&& (fabs(gdScopeCollectData[ DEF_PE_ID_IN_TRACE * nDataLen + ii+2]) < EPS_POSN_ERROR) 
			&& (fabs(gdScopeCollectData[ DEF_PE_ID_IN_TRACE * nDataLen + ii+3]) < EPS_POSN_ERROR) 
			&& (fabs(gdScopeCollectData[ DEF_PE_ID_IN_TRACE * nDataLen + ii+4]) < EPS_POSN_ERROR) 
			&& (fabs(gdScopeCollectData[ DEF_PE_ID_IN_TRACE * nDataLen + ii+5]) < EPS_POSN_ERROR) 
			&& (fabs(gdScopeCollectData[ DEF_PE_ID_IN_TRACE * nDataLen + ii+6]) < EPS_POSN_ERROR) 
			&& (fabs(gdScopeCollectData[ DEF_PE_ID_IN_TRACE * nDataLen + ii+7]) < EPS_POSN_ERROR) 
			&& (fabs(gdScopeCollectData[ DEF_PE_ID_IN_TRACE * nDataLen + ii+8]) < EPS_POSN_ERROR) 
			&& (fabs(gdScopeCollectData[ DEF_PE_ID_IN_TRACE * nDataLen + ii - 1]) > (stpForceCalibrationInput->stTeachContactPara.iSearchTolPE/2.0))  
//			&& (aDebounceCounter[ii] >= stSearchContactAndForceControlForceCali.stpTeachContactPara->iAntiBounce )
			&& (iStartIdx == 0)
		   )
		{
			iStartIdx = ii;
		}
		if( (fabs(gdScopeCollectData[ DEF_REF_VEL_IN_TRACE * nDataLen + ii-1]) < EPS_POSN_ERROR) 
			&& (fabs(gdScopeCollectData[ DEF_REF_VEL_IN_TRACE * nDataLen + ii]) < EPS_POSN_ERROR) 
			&&  (fabs(gdScopeCollectData[ DEF_REF_VEL_IN_TRACE * nDataLen + ii + 1]) > 1.0) 
			&&  (fabs(gdScopeCollectData[ DEF_REF_VEL_IN_TRACE * nDataLen + ii + 2]) > 1.0) 
			&&  (fabs(gdScopeCollectData[ DEF_REF_VEL_IN_TRACE * nDataLen + ii + 3]) > 1.0) 
			&&  (fabs(gdScopeCollectData[ DEF_REF_VEL_IN_TRACE * nDataLen + ii + 4]) > 1.0) 
//			&& (aDebounceCounter[ii] >= stSearchContactAndForceControlForceCali.stpTeachContactPara->iAntiBounce )
			&& (iStartIdx != 0)
			&&  (iEndIdx == nDataLen) 
		   )
		{
			iEndIdx = ii;
		}
		if( iStartIdx != 0 && iEndIdx != nDataLen)
		{
			break;
		}
	}

	/// Find different force segment by condition s.t. afLevelAmplitude
	// nTotalSeg
	for(jj = 0; jj < nTotalSeg; jj++)
	{
		aStartIndexPerSeg[jj] = iStartIdx - 1;
		aEndIndexPerSeg[jj] = iEndIdx;
	}
	jj = 0;
	for(ii = iStartIdx; ii < iEndIdx; ii ++)
	{
		if( (fabs(aForceCmd[ii] - stForceControlBlk.adForceBlk_LevelAmplitude[jj] ) < EPS_FORCE_CMD )
			&& aStartIndexPerSeg[jj] == (iStartIdx - 1)
			)
		{
			aStartIndexPerSeg[jj] = ii; 
		}
		if( (fabs(aForceCmd[ii] - stForceControlBlk.adForceBlk_LevelAmplitude[jj] ) > EPS_FORCE_CMD ) 
			&& aStartIndexPerSeg[jj] >= iStartIdx)
		{
			aEndIndexPerSeg[jj] = ii; 
			jj = jj + 1;

		}
		if( jj >= nTotalSeg)
		{
			break;
		}
	}
	//// For each force segment, find the mean value of command and feedback.
	unsigned int nCutLenIgnoreNoise, iAverageStart, nLenForAve;
	double dBaseForce_adc = 0;

	dBaseForce_adc = calc_mean( &aForceFb[0], iStartIdx/2);
	for(jj = 0; jj <  nTotalSeg; jj++)
	{
		nCutLenIgnoreNoise = (int)floor((aEndIndexPerSeg[jj] - aStartIndexPerSeg[jj])/10.0);
		iAverageStart = aStartIndexPerSeg[jj] + nCutLenIgnoreNoise;
		nLenForAve = aEndIndexPerSeg[jj] - iAverageStart;
		aForceCmdMean_dcom[jj] = calc_mean(&aForceCmd[iAverageStart], nLenForAve) - dDriveCommand_byContactPosn; // 20130102
		aForceFbMean_adc[jj] = calc_mean( &aForceFb[iAverageStart], nLenForAve);
		aForceFbMean_gram[jj] = (aForceFbMean_adc[jj] - dBaseForce_adc) * DEF_FORCE_GRAM_PER_ADC; // 20130102
	}

	if( fpData != NULL)
	{
		fprintf(fpData, "%% SegNo.   StartIndex,  EndIndex,   MeanForceCmd_dcom, MeanForceFb_adc, MeanForce_gram\n aForceFbMean_gram = [");
		for(jj = 0; jj <  nTotalSeg; jj++)
		{
			fprintf(fpData, "%d,  %d,  %d,  %8.4f,  %8.4f,  %8.4f\n",
				jj, aStartIndexPerSeg[jj], aEndIndexPerSeg[jj], aForceCmdMean_dcom[jj], aForceFbMean_adc[jj], aForceFbMean_gram[jj]);
		}
		fprintf(fpData, "];\n\n");
	}
	/// Least square fitting
	aRHS_Sq[0] = aRHS_Sq[1] = 0;
	aMatrix_Sq[0] = aMatrix_Sq[1] = aMatrix_Sq[2] = aMatrix_Sq[3] = 0;

	for(jj = 0; jj< nTotalSeg; jj ++)
	{
		aMatrix_Sq[0 * MATRIX_COL_2 + 0] += aForceFbMean_gram[jj] * aForceFbMean_gram[jj];
		aMatrix_Sq[0 * MATRIX_COL_2 + 1] += aForceFbMean_gram[jj];
		aRHS_Sq[0] += aForceCmdMean_dcom[jj] * aForceFbMean_gram[jj];
		aRHS_Sq[1] += aForceCmdMean_dcom[jj];
	}
	aMatrix_Sq[1 * MATRIX_COL_2 + 0] = aMatrix_Sq[0 * MATRIX_COL_2 + 1];
	aMatrix_Sq[1 * MATRIX_COL_2 + 1] = nTotalSeg;
	if( fpData != NULL)
	{
		fprintf(fpData, "MatrixSq_ByC = [%8.5f, %8.5f; %8.5f, %8.5f];\n",
			aMatrix_Sq[0], aMatrix_Sq[1], aMatrix_Sq[2], aMatrix_Sq[3]);
	}

	if(brinv(aMatrix_Sq, MATRIX_COL_2) == 0)
	{
		aResult[0] = 0;
		aResult[1] = 0;
		sprintf_s(strDebugText, "Matrix singular, %s, %d", __FILE__, __LINE__);
		if( fpData != NULL)
		{
			fprintf(fpData, "%s\n", strDebugText);
			fclose(fpData);
		}
		iRet = MTN_API_ERROR;
		goto label_return_mtn_force_calibration;
	}
	
	brmul(aMatrix_Sq, aRHS_Sq, MATRIX_COL_2, MATRIX_ROW_2, 1, aResult);
	if( fpData != NULL)
	{
		fprintf(fpData, "MatrixSqInv_ByC = [%8.5f, %8.5f; %8.5f, %8.5f];\n",
			aMatrix_Sq[0], aMatrix_Sq[1], aMatrix_Sq[2], aMatrix_Sq[3]);
		fprintf(fpData, "RHS_Sq_ByC = [%8.5f; %8.5f];\n", aRHS_Sq[0], aRHS_Sq[0]);
		fprintf(fpData, "%% Calculation By C Library\n");
		fprintf(fpData, "ForceFactor_Kf_ByC = %10.6f\n", aResult[0]);
		fprintf(fpData, "ForceOffset_I0_IncludeKaCp_ByC = %10.6f\n", aResult[1]);
	}

	//// Step-4:  Further verify for a given point for a number of times and get the average of I0
double fForceFactorKf = aResult[0];
double fForceOffset_temp = aResult[1];
static FORCE_VERIFY_INPUT stForceVerifySmoothI0Input;
static FORCE_VERIFY_OUTPUT stForceVerifySmoothI0Output;
double dSumForceReadBack_gram = 0, dSumI0 = 0;

	if(stpForceCalibrationInput ->iFlagDebug >= 2)
	{

		stForceVerifySmoothI0Input.stTeachContactPara = stpForceCalibrationInput->stTeachContactPara;
		stForceVerifySmoothI0Input.stTeachContactPara.iFlagSwitchToForceControl = 1;
		stForceVerifySmoothI0Input.iFlagTeachContact = 0;
		stForceVerifySmoothI0Input.iContactPosn = stTeachContactResultForceCali.iContactPosnReg;
		stForceVerifySmoothI0Input.fForceFactor_Kf = fForceFactorKf;
		stForceVerifySmoothI0Input.fPosnFactor_Ka = dPosnFactor_Ka;
		stForceVerifySmoothI0Input.fForceOffset_I0 = fForceOffset_temp
			- dPosnFactor_Ka * stTeachContactResultForceCali.iContactPosnReg;
		if(stpForceCalibrationInput->stForceCaliPara.dLastForce_gram < stpForceCalibrationInput->stForceCaliPara.dFirstForce_gram)
		{
			stForceVerifySmoothI0Input.dForceVerifyDesiredGram = stpForceCalibrationInput->stForceCaliPara.dLastForce_gram;
		}
		else
		{
			stForceVerifySmoothI0Input.dForceVerifyDesiredGram = stpForceCalibrationInput->stForceCaliPara.dFirstForce_gram;
		}
		stForceVerifySmoothI0Input.iFlagDebug = 0;

		if( fpData != NULL)
		{
			fprintf(fpData, "%% PointId, DesiredForce_gram, ReadBackForce_gram, Kf, I0\n");
			fprintf(fpData, "matForceVerifySmoothI0 = [ \n");
		}

		for(unsigned int ii = 0; ii<stpForceCalibrationInput->stForceCaliPara.uiNumPointsSmoothing; ii++)
		{
			mtn_force_verify(m_hHandle, &stForceVerifySmoothI0Input, &stForceVerifySmoothI0Output);

			if( fpData != NULL)
			{
				fprintf(fpData, "%d,  %8.5f,  %8.5f,  %8.5f,  %8.5f \n", ii, 
					stForceVerifySmoothI0Input.dForceVerifyDesiredGram,
					stForceVerifySmoothI0Output.dForceVerifyReadBack,
					stForceVerifySmoothI0Input.fForceFactor_Kf,
					stForceVerifySmoothI0Input.fForceOffset_I0);

			}

			stForceVerifySmoothI0Input.fForceOffset_I0 = stForceVerifySmoothI0Input.fForceOffset_I0 + 
				(stForceVerifySmoothI0Input.dForceVerifyDesiredGram - stForceVerifySmoothI0Output.dForceVerifyReadBack) * stForceVerifySmoothI0Input.fForceFactor_Kf;

			dSumForceReadBack_gram = dSumForceReadBack_gram + stForceVerifySmoothI0Output.dForceVerifyReadBack;
			dSumI0 = dSumI0 + stForceVerifySmoothI0Input.fForceOffset_I0;
			Sleep(100);
		}
		if( fpData != NULL)
		{
			fprintf(fpData, "];\n");
		}
	} // 20120102
	double dOffset_I0 = dSumI0/(stpForceCalibrationInput->stForceCaliPara.uiNumPointsSmoothing);

	stpForceCaliOutput->fPosnFactor_Ka = dPosnFactor_Ka; // from Step-1
	stpForceCaliOutput->fForceFactor_Kf = fForceFactorKf;
	stpForceCaliOutput->fForceOffset_I0 = fForceOffset_temp;
	stpForceCaliOutput->iStatus = stTeachContactResultForceCali.iStatus;
	stpForceCaliOutput->dInitForceCommandReadBack = stTeachContactResultForceCali.dInitForceCommandReadBack;
	stpForceCaliOutput->iContactPosnReg = stTeachContactResultForceCali.iContactPosnReg;
	if( fpData != NULL)
	{
		fprintf(fpData, "fFactorKa = %9.7f\n", stpForceCaliOutput->fPosnFactor_Ka);
		fprintf(fpData, "fOffset_I0 = %9.7f \n", stpForceCaliOutput->fForceOffset_I0);
		fclose(fpData);
	}

	if(aForceFbMean_adc[0] <= -8100 || aForceFbMean_adc[nTotalSeg - 1] >= 8100)
	{
		stpForceCaliOutput->iStatus = MTN_CALC_FORCE_SENSOR_SATURATION;
	}

	stpForceCaliOutput->fForceCalibratePa = fabs(fForceFactorKf);
	stpForceCaliOutput->fForceCalibratePb = fabs(dPosnFactor_Ka);
	stpForceCaliOutput->fForceCalibratePc = stpForceCaliOutput->fForceOffset_I0; // fForceOffset_temp;

label_return_mtn_force_calibration:
	return iRet;

}
double calc_round(double dIn)
{
	if(dIn >= 0)
		return floor(dIn + 0.5);
	else
		return floor(dIn - 0.5);
}

int mtn_force_verify(HANDLE hHandle, FORCE_VERIFY_INPUT *stpForceVerifyInput, FORCE_VERIFY_OUTPUT *stpForceVerifyOutput)
{
	int iRet;
	int iReadContactPosition;
	// Step-1: First teach contact
	if(stpForceVerifyInput->iFlagTeachContact >= 1)
	{
		TEACH_CONTACT_OUTPUT stTeachContactResult;
		stpForceVerifyInput->stTeachContactPara.iFlagSwitchToForceControl = 0;
		stpForceVerifyInput->stTeachContactPara.iFlagDebug = stpForceVerifyInput->iFlagDebug;
		if(mtn_teach_contact_acs(hHandle, &stpForceVerifyInput->stTeachContactPara, &stTeachContactResult) != MTN_API_OK_ZERO)
		{
			iRet = MTN_API_ERROR;
			goto label_return_mtn_force_verify;
		}
		iReadContactPosition = stTeachContactResult.iContactPosnReg;
		stpForceVerifyInput->stTeachContactPara.iFlagSwitchToForceControl = 1;
	}
	else
	{
		iReadContactPosition = stpForceVerifyInput->iContactPosn;
	}

	// Step-2: Verify force
	FORCE_BLOCK_ACS stForceControlBlk;
	unsigned int ii, jj;
	stForceControlBlk.uiNumSegment = 2;
	stForceControlBlk.aiForceBlk_Rampcount[0] = 50;
	stForceControlBlk.aiForceBlk_LevelCount[0] = 600;
	stForceControlBlk.adForceBlk_LevelAmplitude[0] = 
		-fabs(stpForceVerifyInput->dForceVerifyDesiredGram * stpForceVerifyInput->fForceFactor_Kf) 
						+ iReadContactPosition * fabs(stpForceVerifyInput->fPosnFactor_Ka) 
						+ stpForceVerifyInput->fForceOffset_I0 ;

	stForceControlBlk.aiForceBlk_Rampcount[1] = 50;
	stForceControlBlk.aiForceBlk_LevelCount[1] = 10;
	stForceControlBlk.adForceBlk_LevelAmplitude[1] = 0 
		+ iReadContactPosition * fabs(stpForceVerifyInput->fPosnFactor_Ka) 
		+ stpForceVerifyInput->fForceOffset_I0;
	
	stForceControlBlk.iInitForceHold_cnt = 5;
	stForceControlBlk.dPreImpForce_DCOM = stpForceVerifyInput->fForceOffset_I0;

	SEARCH_CONTACT_AND_FORCE_CONTROL_INPUT stSearchContactAndForceControlForceVerify;
	TEACH_CONTACT_OUTPUT stTeachContactResultForceVerify;
	
	stSearchContactAndForceControlForceVerify.stpTeachContactPara = &stpForceVerifyInput->stTeachContactPara;
	stSearchContactAndForceControlForceVerify.stpForceBlkPara = &stForceControlBlk;

	stSearchContactAndForceControlForceVerify.stpTeachContactPara->iSearchVel = -10000;
	stSearchContactAndForceControlForceVerify.stpTeachContactPara->iSearchTolPE = 25;
	stSearchContactAndForceControlForceVerify.stpTeachContactPara->iMaxAccMoveSrchHt = 100;
	stSearchContactAndForceControlForceVerify.stpTeachContactPara->iMaxJerkMoveSrchHt = 1;

	// Do once search-contact-force-control
	mtn_search_contract_and_force_control(hHandle, &stSearchContactAndForceControlForceVerify, &stTeachContactResultForceVerify);
	if(stTeachContactResultForceVerify.iStatus != 0)
	{
		iRet = MTN_API_ERROR;
		goto label_return_mtn_force_verify;
	}

	// Upload data
extern 	double sys_get_controller_ts();
	gstSystemScope.uiNumData = 7;
	gstSystemScope.uiDataLen = 4200;
	gstSystemScope.dSamplePeriod_ms = sys_get_controller_ts();

	if(mtnscope_upload_acsc_data_varname(hHandle, BUFFER_ID_SEARCH_CONTACT, "rAFT_Scope") 
		!= MTN_API_OK_ZERO)
	{
		iRet = MTN_API_ERROR;
		goto label_return_mtn_force_verify;
	}

	/// Save Data // input fpData stTeachContactResult, stForceControlBlk
#define FILE_NAME_CONTACT_FORCE_VERIFY  "ForceVerify.m"
	FILE *fpData = NULL;
	if(stpForceVerifyInput->iFlagDebug)
	{
		fopen_s(&fpData, FILE_NAME_CONTACT_FORCE_VERIFY, "w");
	}
	int iDampGA;
	if( fpData != NULL)
	{
		fprintf(fpData, "%% Position Calibration Output\n");
		fprintf(fpData, "dPositionFactor = %8.4f;", stpForceVerifyInput->fPosnFactor_Ka);
		fprintf(fpData, "dCtrlOutOffset = %8.4f;\n", stpForceVerifyInput->fForceOffset_I0);
		mtnapi_upload_acs_sp_parameter_damp_switch(hHandle, &iDampGA);
		fprintf(fpData, "iDampACS = %d; \n", iDampGA);
		mtn_debug_print_to_file(fpData, &stSearchContactAndForceControlForceVerify, &stTeachContactResultForceVerify);
	}

	unsigned int iStartIdx = 0;
	unsigned int nDataLen = gstSystemScope.uiDataLen;
	unsigned int iEndIdx = nDataLen;
	double *aPosnErr = &gdScopeCollectData[ DEF_PE_ID_IN_TRACE * nDataLen];
	double *aForceCmd = &gdScopeCollectData[ DEF_FORCE_CMD_ID_IN_TRACE * nDataLen];
	double *aForceFb = &gdScopeCollectData[ DEF_FORCE_FB_ID_IN_TRACE * nDataLen];
	double *aDebounceCounter = &gdScopeCollectData[DEF_SEARCH_COUNT_DEBOUNCE * nDataLen];

	/// Find the section of force control by condition s.t. PositionErr == 0
	for(ii = 1; ii<nDataLen - 1 ; ii ++)
	{
		if( (fabs(aPosnErr[ii]) < EPS_POSN_ERROR) 
			&& (fabs(aPosnErr[ii+1]) < EPS_POSN_ERROR) 
			&& (fabs(aPosnErr[ii+2]) < EPS_POSN_ERROR) 
			&& (fabs(aPosnErr[ii+3]) < EPS_POSN_ERROR) 
			&& (fabs(aPosnErr[ii+4]) < EPS_POSN_ERROR) 
			&& (fabs(aPosnErr[ii+5]) < EPS_POSN_ERROR) 
			&& (fabs(aPosnErr[ii+6]) < EPS_POSN_ERROR) 
			&& (fabs(aPosnErr[ii+7]) < EPS_POSN_ERROR) 
			&& (fabs(aPosnErr[ii+8]) < EPS_POSN_ERROR) 
			&& (fabs(aPosnErr[ii+9]) < EPS_POSN_ERROR) 
			&& (fabs(aPosnErr[ii - 1]) > stSearchContactAndForceControlForceVerify.stpTeachContactPara->iSearchTolPE/2.0)  
	//		&& (aDebounceCounter[ii] >= stSearchContactAndForceControlForceVerify.stpTeachContactPara->iAntiBounce )
			&& (iStartIdx == 0)
		   )
		{
			iStartIdx = ii;
		}
		if( (fabs(aPosnErr[ii-1]) < EPS_POSN_ERROR) 
			&& (fabs(aPosnErr[ii]) < EPS_POSN_ERROR) 
			&&  (fabs(aPosnErr[ii + 1]) > EPS_POSN_ERROR) 
			&& (iStartIdx != 0)
//			&& (aDebounceCounter[ii] >= stSearchContactAndForceControlForceVerify.stpTeachContactPara->iAntiBounce )
			&&  (iEndIdx == nDataLen) 
		   )
		{
			iEndIdx = ii;
		}
		if( iStartIdx != 0 && iEndIdx != nDataLen)
		{
			break;
		}
	}
unsigned int uiStartIndexVerifyForce = iStartIdx - 1;
unsigned int uiEndIndexVerifyForce = iEndIdx;

	jj = 0;
	for(ii = iStartIdx; ii < iEndIdx; ii ++)
	{
		if( (fabs(aForceCmd[ii] - stForceControlBlk.adForceBlk_LevelAmplitude[jj] ) < EPS_FORCE_CMD )
			&& uiStartIndexVerifyForce == (iStartIdx - 1)
			)
		{
			uiStartIndexVerifyForce = ii; 
		}
		if( (fabs(aForceCmd[ii] - stForceControlBlk.adForceBlk_LevelAmplitude[jj] ) > EPS_FORCE_CMD ) 
			&& uiStartIndexVerifyForce >= iStartIdx)
		{
			uiEndIndexVerifyForce = ii; 
			jj = jj + 1;
		}
		if( jj >= 1) // Totally there is only 1 valid segment
		{
			break;
		}
	}

	//// For the valid segment, find the mean value of feedback.
	unsigned int nCutLenIgnoreNoise, iAverageStart, nLenForAve;
	nCutLenIgnoreNoise = (int)floor((uiEndIndexVerifyForce - uiStartIndexVerifyForce)/5.0); // skip first 20% data, 20150124
	iAverageStart = uiStartIndexVerifyForce + nCutLenIgnoreNoise;
	nLenForAve = uiEndIndexVerifyForce - iAverageStart;

	double dBaseForceMean_adc = calc_mean( &aForceFb[0], iStartIdx/2);
	double dForceFbMean_adc = calc_mean( &aForceFb[iAverageStart], nLenForAve);
	double dForceFbMean_gram = fabs(dForceFbMean_adc - dBaseForceMean_adc)* DEF_FORCE_GRAM_PER_ADC;

	if( fpData != NULL)
	{
		fprintf(fpData, "CmdForce_DCOM = %8.5f; \n", stForceControlBlk.adForceBlk_LevelAmplitude[0] );
		fprintf(fpData, "%% Readback force\n");
		fprintf(fpData, "dForceFbMean_adc = %10.6f;\n", dForceFbMean_adc);
		fprintf(fpData, "dForceFbMean_gram = %10.6f;\n", dForceFbMean_gram);
		fprintf(fpData, "DesiredForce_gram = %10.6f;\n", stpForceVerifyInput->dForceVerifyDesiredGram);
		fprintf(fpData, "Kf = %10.6f;\n", stpForceVerifyInput->fForceFactor_Kf);
		fprintf(fpData, "I0 = %10.6f;\n", stpForceVerifyInput->fForceOffset_I0);
		fclose(fpData);
	}

	stpForceVerifyOutput->dForceVerifyReadBack = dForceFbMean_gram;
	stpForceVerifyOutput->dInitForceCommandReadBack = stTeachContactResultForceVerify.dInitForceCommandReadBack;
	stpForceVerifyOutput->iContactPosnReg = stTeachContactResultForceVerify.iContactPosnReg;
	stpForceVerifyOutput->iStatus = stTeachContactResultForceVerify.iStatus;

label_return_mtn_force_verify:
	return iRet;
}

