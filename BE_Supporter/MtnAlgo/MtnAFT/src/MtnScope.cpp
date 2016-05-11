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



#include "stdafx.h"
#include "MtnApi.h"
#include "math.h"
// CMtnScope dialog
MTN_SCOPE gstSystemScope = {2000, 5, 1.0};
char gstrScopeDataVarName[] = "ScopeDataVar";
char gstrScopeArrayName[128];
double gdScopeCollectData[30000];
char strACSC_VarName[128] = "RPOS(0)\rRVEL(0)";

void mtnscope_declare_var_on_controller(HANDLE Handle)
{
	sprintf_s(gstrScopeArrayName, 128, "%s(%d)(%d)", gstrScopeDataVarName, 
		gstSystemScope.uiNumData, 
		gstSystemScope.uiDataLen);

	acsc_DeclareVariable(Handle, ACSC_REAL_TYPE, gstrScopeArrayName, NULL);
}

void mtnscope_set_acsc_var_collecting_posncompn(int iAxis)
{
	sprintf_s(strACSC_VarName, "FPOS(%d)\rDOUT(%d)", iAxis, iAxis);
}

void mtnscope_set_acsc_var_collecting_searchindex(int iAxis)
{
	sprintf_s(strACSC_VarName, "RPOS(%d)\rPE(%d)\rRVEL(%d)\rIND(%d)\rDOUT(%d)", iAxis, iAxis, iAxis, iAxis, iAxis);
}

void mtnscope_set_acsc_var_collecting_move(int iAxis)
{
	sprintf_s(strACSC_VarName, "PE(%d),RPOS(%d),FVEL(%d),RVEL(%d),DOUT(%d)", iAxis, iAxis, iAxis, iAxis, iAxis);
}

void mtnscope_set_acsc_var_collecting_velloop_test(int iAxis)
{
	sprintf_s(strACSC_VarName, "RPOS(%d)\rFVEL(%d)\rRVEL(%d)\rFACC(%d)\rDOUT(%d)", iAxis, iAxis, iAxis, iAxis, iAxis);
}
void mtnscope_set_acsc_var_collecting_settling(int iAxis)
{
	sprintf_s(strACSC_VarName, "PE(%d)\rDOUT(%d)", iAxis, iAxis);
}

#define  NUM_COLLECT_DATA_SEGMENT  100
#define  MAX_WAIT_COUNT_SEGMENT_SETTLING  30
int mtnapi_wait_axis_settling(HANDLE Handle, int iAxis, double dPosnErrSettleTH)
{
	short sRet = MTN_API_OK_ZERO;
	int iCount_AbsPositionError_GE_TH = 0;

	MTN_SCOPE stScopeCfg;

	stScopeCfg.uiDataLen = NUM_COLLECT_DATA_SEGMENT;
	stScopeCfg.uiNumData = 2;
//	mtnscope_declare_var_on_controller(Handle);
	sprintf_s(gstrScopeArrayName, 128, "%s(%d)(%d)", gstrScopeDataVarName, 
		stScopeCfg.uiNumData, 
		stScopeCfg.uiDataLen);

	acsc_DeclareVariable(Handle, ACSC_REAL_TYPE, gstrScopeArrayName, NULL);
	mtnscope_set_acsc_var_collecting_settling(iAxis);

	if (!acsc_CollectB(Handle, 0, // system data collection
			gstrScopeArrayName, // name of data collection array
			stScopeCfg.uiDataLen, // number of samples to be collected
			1, // sampling period 1 millisecond
			strACSC_VarName, // variables to be collected
			NULL)
			)
	{
		printf("transaction error: %d\n", acsc_GetLastError());
		sRet = MTN_API_ERROR_ACS_ERROR_SETTLING;
	}
	Sleep(NUM_COLLECT_DATA_SEGMENT);
	mtnscope_upload_acsc_data_userdef(Handle, &stScopeCfg);
	while((fabs(stScopeCfg.dMaxDataAtTrace[0]) > dPosnErrSettleTH)
		|| (fabs(stScopeCfg.dMinDataAtTrace[0]) > dPosnErrSettleTH))
	{
		iCount_AbsPositionError_GE_TH ++;
		if(iCount_AbsPositionError_GE_TH > MAX_WAIT_COUNT_SEGMENT_SETTLING)
		{
			sRet = MTN_API_ERROR_ACS_ERROR_SETTLING;
			break;
		}
		if (!acsc_CollectB(Handle, 0, // system data collection
				gstrScopeArrayName, // name of data collection array
				stScopeCfg.uiDataLen, // number of samples to be collected
				1, // sampling period 1 millisecond
				strACSC_VarName, // variables to be collected
				NULL)
				)
		{
			printf("transaction error: %d\n", acsc_GetLastError());
			sRet = MTN_API_ERROR_ACS_ERROR_SETTLING;
		}
		Sleep(NUM_COLLECT_DATA_SEGMENT);
		mtnscope_upload_acsc_data_userdef(Handle, &stScopeCfg);
	}

	return sRet;
}
#include "MtnInitAcs.h"
short mtn_save_curr_axis_traj_data_to_filename(char *strFilename, unsigned int uiAxisCtrlCard)
{
	FILE *fpData;
	short sRet = MTN_API_OK_ZERO;

	double dEncRes_mm;
	switch(uiAxisCtrlCard)
	{
	case ACSC_AXIS_X:
	case ACSC_AXIS_Y:
		dEncRes_mm = 0.0005;
		break;

	case ACSC_AXIS_A:
		dEncRes_mm = 0.001;
		break;
	}
	unsigned int ii, jj;
	fopen_s(&fpData, strFilename, "w");

//	fprintf(fpData, "%% ACSC Controller, %s\n\n", strACSC_VarName);
	if(fpData != NULL)
	{
		fprintf(fpData, "%% ACS_Axis - %d \n", uiAxisCtrlCard );
		fprintf(fpData, "fSampleTime_ms = %5.1f \n", sys_get_controller_ts());
		fprintf(fpData, "fEncRes_mm = %8.5f \n",  dEncRes_mm);
		fprintf(fpData, "%% PE RPOS FVEL RVEL DOUT\n");
		fprintf(fpData, "TrajData = [");
		for(ii = 0; ii<gstSystemScope.uiDataLen; ii++)
		{
			for(jj = 0; jj<gstSystemScope.uiNumData; jj++)
			{
				if(jj == gstSystemScope.uiNumData - 1)
				{	
					fprintf(fpData, "%8.2f", gdScopeCollectData[jj* gstSystemScope.uiDataLen + ii]);
				}
				else
				{
					fprintf(fpData, "%8.2f,  ", gdScopeCollectData[jj* gstSystemScope.uiDataLen + ii]);
				}
			}
			if(ii == (gstSystemScope.uiDataLen - 1))
			{
				fprintf(fpData, "];\n");
			}
			else
			{
				fprintf(fpData, "\n");
			}
		}
		fclose(fpData);
	}
	else
	{
		sRet = MTN_API_ERROR_OPEN_FILE;
	}
	return sRet;
}


double mtnscope_FindMaxInSpecScopeData(int idxData, MTN_SCOPE *stpScope)
{
	double dMaxData;
	unsigned int ii;
	for(ii=0; ii<stpScope->uiDataLen; ii ++)
	{
		if(ii == 0)
		{
			dMaxData = gdScopeCollectData[idxData * stpScope->uiDataLen + ii];
		}
		else
		{
			if(dMaxData < gdScopeCollectData[idxData * stpScope->uiDataLen + ii])
			{
				dMaxData = gdScopeCollectData[idxData * stpScope->uiDataLen + ii];
			}
		}
	}
	return dMaxData;
}

double mtnscope_FindMinInSpecScopeData(int idxData, MTN_SCOPE *stpScope)
{
	double dMinData;
	unsigned int ii;
	for(ii=0; ii<stpScope->uiDataLen; ii ++)
	{
		if(ii == 0)
		{
			dMinData = gdScopeCollectData[idxData * stpScope->uiDataLen + ii];
		}
		else
		{
			if(dMinData > gdScopeCollectData[idxData * stpScope->uiDataLen + ii])
			{
				dMinData = gdScopeCollectData[idxData * stpScope->uiDataLen + ii];
			}
		}
	}
	return dMinData;
}


double mtnscope_FindMaxInScopeData(int idxData)
{
	double dMaxData;
	unsigned int ii;
	for(ii=0; ii<gstSystemScope.uiDataLen; ii ++)
	{
		if(ii == 0)
		{
			dMaxData = gdScopeCollectData[idxData * gstSystemScope.uiDataLen + ii];
		}
		else
		{
			if(dMaxData < gdScopeCollectData[idxData * gstSystemScope.uiDataLen + ii])
			{
				dMaxData = gdScopeCollectData[idxData * gstSystemScope.uiDataLen + ii];
			}
		}
	}
	return dMaxData;
}

double mtnscope_FindMinInScopeData(int idxData)
{
	double dMinData;
	unsigned int ii;
	for(ii=0; ii<gstSystemScope.uiDataLen; ii ++)
	{
		if(ii == 0)
		{
			dMinData = gdScopeCollectData[idxData * gstSystemScope.uiDataLen + ii];
		}
		else
		{
			if(dMinData > gdScopeCollectData[idxData * gstSystemScope.uiDataLen + ii])
			{
				dMinData = gdScopeCollectData[idxData * gstSystemScope.uiDataLen + ii];
			}
		}
	}
	return dMinData;
}

int mtnscope_upload_acsc_data(HANDLE Handle)
{
	int iRet;
	char cErrorCode[256];

	if (!acsc_ReadReal( Handle, ACSC_NONE, gstrScopeDataVarName, 0, gstSystemScope.uiNumData-1, 0, gstSystemScope.uiDataLen - 1, gdScopeCollectData, NULL))
	{
		sprintf_s(cErrorCode, "transaction error: %d\n", acsc_GetLastError());
		iRet = MTN_API_ERROR_UPLOAD_DATA;
	}
	else
	{	
		for(unsigned int ii=0; ii< gstSystemScope.uiNumData; ii++)
		{
			gstSystemScope.dMaxDataAtTrace[ii] = mtnscope_FindMaxInScopeData(ii);
			gstSystemScope.dMinDataAtTrace[ii] = mtnscope_FindMinInScopeData(ii);
		}
		iRet = MTN_API_OK_ZERO;
	}
	return iRet;
}

// 20120522
int mtnscope_upload_acsc_data_userdef(HANDLE Handle, MTN_SCOPE *stpScope)
{
	int iRet;
	char cErrorCode[256];

	if (!acsc_ReadReal( Handle, ACSC_NONE, gstrScopeDataVarName, 0, stpScope->uiNumData-1, 0, stpScope->uiDataLen - 1, gdScopeCollectData, NULL))
	{
		sprintf_s(cErrorCode, "transaction error: %d\n", acsc_GetLastError());
		iRet = MTN_API_ERROR_UPLOAD_DATA;
	}
	else
	{	
		for(unsigned int ii=0; ii< stpScope->uiNumData; ii++)
		{
			stpScope->dMaxDataAtTrace[ii] = mtnscope_FindMaxInSpecScopeData(ii, stpScope);
			stpScope->dMinDataAtTrace[ii] = mtnscope_FindMinInSpecScopeData(ii, stpScope);
		}
		iRet = MTN_API_OK_ZERO;
	}
	return iRet;
}

static char gstrTuningScopeArrayName[128];
void mtnscope_tuning_declare_var_on_controller(HANDLE Handle)
{
	sprintf_s(gstrTuningScopeArrayName, 128, "TuningScopeData(5)(2000)");
int iRet;
	iRet = acsc_DeclareVariable(Handle, ACSC_REAL_TYPE, gstrTuningScopeArrayName, NULL);
}

int mtnscope_tuning_upload_acsc_data(HANDLE Handle)
{
	int iRet;
	char cErrorCode[256];

	if (!acsc_ReadReal( Handle, ACSC_NONE, "TuningScopeData", 0, 4, 0, 1999, gdScopeCollectData, NULL))
	{
		sprintf_s(cErrorCode, "transaction error: %d\n", acsc_GetLastError());
		iRet = MTN_API_ERROR_UPLOAD_DATA;
	}
	//else
	//{	
	//	for(unsigned int ii=0; ii< gstSystemScope.uiNumData; ii++)
	//	{
	//		gstSystemScope.dMaxDataAtTrace[ii] = mtnscope_FindMaxInScopeData(ii);
	//		gstSystemScope.dMinDataAtTrace[ii] = mtnscope_FindMinInScopeData(ii);
	//	}
	//	iRet = MTN_API_OK_ZERO;
	//}
	return iRet;
}

int mtnscope_upload_acsc_data_varname(HANDLE Handle, int idxBuff, char * strVarName)
{
	int iRet;
	char cErrorCode[256];

	if (!acsc_ReadReal( Handle, idxBuff, strVarName, 0, gstSystemScope.uiNumData-1, 0, gstSystemScope.uiDataLen - 1, gdScopeCollectData, NULL))
	{
		sprintf_s(cErrorCode, "transaction error: %d\n", acsc_GetLastError());
		iRet = MTN_API_ERROR_UPLOAD_DATA;
	}
	else
	{	
		for(unsigned int ii=0; ii< gstSystemScope.uiNumData; ii++)
		{
			gstSystemScope.dMaxDataAtTrace[ii] = mtnscope_FindMaxInScopeData(ii);
			gstSystemScope.dMinDataAtTrace[ii] = mtnscope_FindMinInScopeData(ii);
		}
		iRet = MTN_API_OK_ZERO;
	}
	return iRet;
}