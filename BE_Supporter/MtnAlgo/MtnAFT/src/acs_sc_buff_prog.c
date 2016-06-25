
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


#include "stdlib.h"

#include "mtndefin.h"
// #include "MtnApi.h"

#include "acs_buff_prog.h"
//#include "MotAlgo_DLL.h"

// Controller Buffer Program#7 for ACS-SC, 2016Q2
char *strBufferProgram_OpenCtrlACS_SC_Para[] = 
{
	"real rJerkFF_AxisZ;\n",  // 0
	"real  rSLDFF_AxisX, rSLDFF_AxisY, rSLDFF_AxisZ;\n",  // 1
	"real rJerkFF_AxisX, rJerkFF_AxisY, rSpringCompGa, rKfromYtoZ, rKxaGf, rKfromZtoY, rKaxGf, rOffsetFromYtoZ, rOffsetFromZtoY;\n",  // 2 2016Q2
	"int iFlagSpringCompOn,  iSpringOffset, iSpringCompGf; \n", // 3
	"global REAL arMotorXProfile(4)(3); global REAL arMotorYProfile(4)(3); global REAL arMotorAProfile(5)(3); global REAL arBHCtrl(20)(4); global REAL arXCtrl(10)(4); global REAL arYCtrl(4)(4); \n",     // 4 global REAL rWaveForm(9)(2000);
	"int nLenRMS, iFlagEnableCalcRMS;\n", // 5, 20120116
	"real rDelayLineRMS_Axis4(6000),rMeanSOS_Axis4_curr, rMeanSOS_Axis4_prev, rSqDout_Axis4_curr; \n",  // 6
	"real rDelayLineRMS_Axis0(6000),rMeanSOS_Axis0_curr, rMeanSOS_Axis0_prev, rSqDout_Axis0_curr; \n",  // 7
	"real rDelayLineRMS_Axis1(6000),rMeanSOS_Axis1_curr, rMeanSOS_Axis1_prev, rSqDout_Axis1_curr; \n",  // 8
	"int idxCurrentDcom, idxOldestDcom, idx2ndOldestDcom, ii; \n",  // 9
	"global real rSOS_Axis4, rRMS_Axis4, rSOS_Axis0, rRMS_Axis0, rSOS_Axis1, rRMS_Axis1, arRMS(4); global REAL TuningScopeData(5)(2000);\n",  // 10
	"global int iFlagEnableEmergencyCheck, aiFlagIO(10), idxChekIO, iFilteredIO, iRelaxPosnBH, iCountFlagIO, iCountCutOffXY, iCountCutOffBH;\n",  // 11
	"global real rBakPKPA;\n",  // 12
	"\n",  // 5 13
	"AUTOEXEC:",                                    // 14   
	"iFlagEnableCalcRMS = 0; idxChekIO = 0; iFlagEnableEmergencyCheck=0; iRelaxPosnBH = EOFFS(4); iCountFlagIO= 0; iCountCutOffXY = 0; iCountCutOffBH = 0;\n",  // 15
	"rJerkFF_AxisZ = 1.2E10;\n",                      // 16,  LINE_ID_SP_DEF_PARA_DECLARE_
	"rJerkFF_AxisX = 0.75;\n",              // 17
	"rJerkFF_AxisY = 0.8;\n",                     // 18
	"rSLDFF_AxisX = 1;\n",              // 19
	"rSLDFF_AxisY = 1;\n",			            // 20
	"rSLDFF_AxisZ = 1;\n",             // 21
	"rKfromYtoZ = 0;\n",				// 22  2016Q2
	"rKfromZtoY= 0; \n",				// 23   2016Q2
	"rKxaGf = 17; \n",				        // 24
	"rKaxGf= 15;\n",					    // 25
	"\n",  // 26
	// ACC ->: /2E6, Jerk -> /2E9
	//  Acc:  ,             Jerk
	// 20E7 -> 100           100E9 -> 50000
	// 16E7 -> 80            40E9 -> 20000
	// 16E7 -> 80            40E9 -> 20000
	// 16E7 -> 80            40E9 -> 20000
	"arMotorXProfile(0)(0) = 1.9E6; arMotorXProfile(0)(1) = 20E7; arMotorXProfile(0)(2) = 100E9;\n",  // 27, LINE_ID_13V_13T_SPEED_PROFILE_DEF
	"arMotorXProfile(1)(0) = 1.9E6; arMotorXProfile(1)(1) = 16E7; arMotorXProfile(1)(2) = 40E9;\n",   // 28
	"arMotorXProfile(2)(0) = 1.9E6; arMotorXProfile(2)(1) = 16E7; arMotorXProfile(2)(2) = 40E9;\n",	// 29
	"arMotorXProfile(3)(0) = 1.9E6; arMotorXProfile(3)(1) = 16E7; arMotorXProfile(3)(2) = 40E9;\n",	// 30
	"\n",	// 31
	// ACC ->: / 2E6, Jerk -> / 2E9                
	// Acc                   Jerk                  Jerk for Production 
	// 5E7 -> 25  ,          24E9 -> 12000           4.4E9 -> 2200
	// 3E7 -> 15  ,          16E9 -> 8000           4E9  -> 2000            
	// 3E7 -> 15  ,          16E9 -> 8000           3E9  -> 1500 ,          
	// 3E7 -> 15  ,          8E9 -> 4000           1E9  -> 500 

	"arMotorYProfile(0)(0) = 1.9E6; arMotorYProfile(0)(1) = 5E7; arMotorYProfile(0)(2) = 24E9;\n", // 32
	"arMotorYProfile(1)(0) = 1.9E6; arMotorYProfile(1)(1) = 3E7; arMotorYProfile(1)(2) = 16E9;\n",	// 33
	"arMotorYProfile(2)(0) = 1.9E6; arMotorYProfile(2)(1) = 3E7; arMotorYProfile(2)(2) = 16E9;\n",	// 34
	"arMotorYProfile(3)(0) = 1.9E6; arMotorYProfile(3)(1) = 3E7; arMotorYProfile(3)(2) = 8E9;\n",	// 35
	"\n",	// 36
	"arMotorAProfile(0)(0) = 1.95E6; arMotorAProfile(0)(1) = 3.5E8; arMotorAProfile(0)(2) = 8E10;\n",	// 37
	"arMotorAProfile(1)(0) = 1.95E6; arMotorAProfile(1)(1) = 4.5E8; arMotorAProfile(1)(2) = 3E11;\n",	// 38
	"arMotorAProfile(2)(0) = 1.95E6; arMotorAProfile(2)(1) = 4.5E8; arMotorAProfile(2)(2) = 3E11;\n",	// 39
	"arMotorAProfile(3)(0) = 1.95E6; arMotorAProfile(3)(1) = 4.5E8; arMotorAProfile(3)(2) = 3E11;\n",	// 40
	"arMotorAProfile(4)(0) = 1.95E6; arMotorAProfile(4)(1) = 8.0E8; arMotorAProfile(4)(2) = 6E11;\n",	// 41, LINE_END_SPEED_CFG_DIFF_WB13V_13T_
	"\n",	// 42
	"nLenRMS = 6000;\n",// 43
	"idxCurrentDcom = 0; idxOldestDcom = nLenRMS - 1; idx2ndOldestDcom = nLenRMS - 2;\n", // 44
	"rMeanSOS_Axis4_curr = 0;rMeanSOS_Axis4_prev = 0; rSOS_Axis4 = 0; \n",   // 45
	"rMeanSOS_Axis0_curr = 0;rMeanSOS_Axis0_prev = 0; rSOS_Axis0 = 0; \n",   // 46
	"rMeanSOS_Axis1_curr = 0;rMeanSOS_Axis1_prev = 0; rSOS_Axis1 = 0; \n",   // 47
	"ii = 0; \n", // 48
	"loop nLenRMS;\n", // 49
	"rDelayLineRMS_Axis4(ii) = abs(DOUT(2) * DOUT(2)); rSOS_Axis4 =  rSOS_Axis4+ rDelayLineRMS_Axis4(ii); \n",  // 50
	"rDelayLineRMS_Axis0(ii) = abs(DOUT(0) * DOUT(0)); rSOS_Axis0 =  rSOS_Axis0+ rDelayLineRMS_Axis0(ii); \n",  // 51
	"rDelayLineRMS_Axis1(ii) = abs(DOUT(1) * DOUT(1)); rSOS_Axis1 =  rSOS_Axis1+ rDelayLineRMS_Axis1(ii); ii = ii +1;        end \n", // 52
	"rMeanSOS_Axis4_prev = rSOS_Axis4/nLenRMS; \n", //53
	"rMeanSOS_Axis0_prev = rSOS_Axis0/nLenRMS; \n", //54
	"rMeanSOS_Axis1_prev = rSOS_Axis1/nLenRMS; \n", //55
	"rRMS_Axis4 = sqrt(rSOS_Axis4/nLenRMS)/327.67; rRMS_Axis0 = sqrt(rSOS_Axis0/nLenRMS)/327.67; rRMS_Axis1 = sqrt(rSOS_Axis1/nLenRMS)/327.67; \n",  // 56
	"while(1);\n", // 57 // 20120902
	"TILL (MST2.#ENABLED = 1 | MST0.#ENABLED = 1 | MST1.#ENABLED = 1) & iFlagEnableCalcRMS = 1;\n",  //58 20120906
	"	BLOCK;\n", //59
	"	!0;\n", //60
	"		rSqDout_Axis0_curr =  abs(DOUT(0));  rSqDout_Axis0_curr = rSqDout_Axis0_curr * rSqDout_Axis0_curr;\n", //61
	"		rMeanSOS_Axis0_curr = rMeanSOS_Axis0_prev + ((rSqDout_Axis0_curr - rDelayLineRMS_Axis0(idxOldestDcom)))/nLenRMS;\n", //62
	"		if(rMeanSOS_Axis0_curr < 0) rMeanSOS_Axis0_curr = 0; end;\n", //63
	"		rMeanSOS_Axis0_prev = rMeanSOS_Axis0_curr;\n", //64
	"	!1  !/3 +   rDelayLineRMS_Axis1(idxCurrentDcom) -  rDelayLineRMS_Axis1(idx2ndOldestDcom);\n", //65
	"		rSqDout_Axis1_curr =  abs(DOUT(1));  rSqDout_Axis1_curr = rSqDout_Axis1_curr * rSqDout_Axis1_curr;\n", // 66
	"		rMeanSOS_Axis1_curr = rMeanSOS_Axis1_prev + ((rSqDout_Axis1_curr - rDelayLineRMS_Axis1(idxOldestDcom)))/nLenRMS;\n", //67
	"		if(rMeanSOS_Axis1_curr < 0) rMeanSOS_Axis1_curr = 0; end;\n", // 68
	"		rMeanSOS_Axis1_prev = rMeanSOS_Axis1_curr;\n", //69
	"	!4 !/3 +   rDelayLineRMS_Axis4(idxCurrentDcom) -  rDelayLineRMS_Axis4(idx2ndOldestDcom);\n", //70
	"		rSqDout_Axis4_curr =  abs(DOUT(2));  rSqDout_Axis4_curr = rSqDout_Axis4_curr * rSqDout_Axis4_curr; \n", //71
	"		rMeanSOS_Axis4_curr = rMeanSOS_Axis4_prev + ((rSqDout_Axis4_curr - rDelayLineRMS_Axis4(idxOldestDcom)))/nLenRMS; \n", //72
	"		if(rMeanSOS_Axis4_curr < 0) rMeanSOS_Axis4_curr = 0; end \n", //73
	"		rMeanSOS_Axis4_prev = rMeanSOS_Axis4_curr; \n", //74
	"	! Update index \n", //75
	"		idxCurrentDcom = idxCurrentDcom -1; if idxCurrentDcom = -1 idxCurrentDcom = 5999 end; \n", //76
	"		idxOldestDcom = idx2ndOldestDcom;  \n", //77
	"		idx2ndOldestDcom = idx2ndOldestDcom - 1; if idx2ndOldestDcom = -1 idx2ndOldestDcom = 5999 end; \n", //78
	"	!0 \n", //79
	"		if MST0.#ENABLED = 1 \n",//80
	"		rDelayLineRMS_Axis0(idxCurrentDcom) = rSqDout_Axis0_curr; \n", //81
	"		rRMS_Axis0 = sqrt(rMeanSOS_Axis0_curr)/327.67; \n", //82
	"		end \n", //83
	"	!1 \n",//84
	"		if MST1.#ENABLED = 1 \n", //85
	"		rDelayLineRMS_Axis1(idxCurrentDcom) = rSqDout_Axis1_curr; \n", //86
	"		rRMS_Axis1 = sqrt(rMeanSOS_Axis1_curr)/327.67; \n", //87
	"		end \n", //88
	"	!4 \n", //89
	"		if MST2.#ENABLED = 1 \n", //90
	"		rDelayLineRMS_Axis4(idxCurrentDcom) = rSqDout_Axis4_curr; \n", //91
	"		rRMS_Axis4 = sqrt(rMeanSOS_Axis4_curr)/327.67; \n", //92
	"		end \n", //93
	" \n", //94
	"		arRMS(0) = rRMS_Axis0; arRMS(1) = rRMS_Axis1; arRMS(2) = rRMS_Axis4; \n", //95
	"	END \n", //96
	"     aiFlagIO(idxChekIO) = IN0.3; idxChekIO = idxChekIO + 1; if idxChekIO = 5 idxChekIO = 0 end; \n", //97
	"     BLOCK\n", //98
	"         !if iFlagEnableEmergencyCheck = 1 \n", //99
	"            iFilteredIO = aiFlagIO(0) & aiFlagIO(1); iFilteredIO = iFilteredIO & aiFlagIO(2); iFilteredIO = iFilteredIO & aiFlagIO(3); iFilteredIO = iFilteredIO & aiFlagIO(4);\n", //100
	"         !   iFilteredIO = iFilteredIO & aiFlagIO(5);iFilteredIO = iFilteredIO & aiFlagIO(6);iFilteredIO = iFilteredIO & aiFlagIO(7);iFilteredIO = iFilteredIO & aiFlagIO(8); iFilteredIO = iFilteredIO & aiFlagIO(9);\n", //101
	"            iRelaxPosnBH = EOFFS(2);\n", //102
	"         !end\n", //103
	"     END\n", //104
	"end \n", //105
	"STOP\n",	// 106
	"ON MST2.#ENABLED; SetSP(1, getspa(1, \"axes[0].SLJFF\"), rJerkFF_AxisZ); SetSP(1, getspa(1, \"axes[0].SLDFF\"), rSLDFF_AxisZ); RET\n",	// 107
	"ON MST0.#ENABLED; SetSP(0, getspa(0, \"axes[0].SLJFF\"), rJerkFF_AxisY); SetSP(0, getspa(0, \"axes[0].SLDFF\"), rSLDFF_AxisY); RET\n",	// 108
	"ON MST1.#ENABLED; SetSP(0, getspa(0, \"axes[1].SLJFF\"), rJerkFF_AxisX); SetSP(0, getspa(0, \"axes[1].SLDFF\"), rSLDFF_AxisX); RET\n",	// 109 
	"ON MST0.#MOVE; WHILE (MST0.#MOVE & MST2.#ENABLED); rOffsetFromYtoZ = rKfromYtoZ * RACC(0)/XACC(0);  if MST2.#OPEN; DCOM(2) = rOffsetFromYtoZ; else SLIOFFS(2) = rOffsetFromYtoZ; end; END; RET\n",	// 110
	"ON iFlagEnableEmergencyCheck = 1 & iFilteredIO = 1 & MST2.#ENABLED;  rBakPKPA = SLPKP(2); KILL X; KILL Y; KILL A; STOP 0; STOP 1; wait 1; enable A; ptp/e A, iRelaxPosnBH; SLPKP(2) = 0; iFlagEnableEmergencyCheck = 0; iCountFlagIO = iCountFlagIO+1; wait 100; disable A; SLPKP(2) = rBakPKPA;  RET\n",	// 111
	"ON (PST0.#RUN | PST1.#RUN) & (MST0.#ENABLED = 0 | MST1.#ENABLED = 0);   rBakPKPA = SLPKP(2); KILL A; STOP 0; STOP 1; wait(1); enable A; ptp/e A, iRelaxPosnBH; wait 10; KILL A; iCountCutOffXY = iCountCutOffXY +1; SLPKP(2) = 0; wait 100; disable A; SLPKP(2) = rBakPKPA;  RET\n",	// 112
	"!ON (PST0.#RUN | PST1.#RUN) & (MST2.#ENABLED = 0 );   rBakPKPA = SLPKP(2); STOP 0; STOP 1; wait(1); enable A; ptp/e A, iRelaxPosnBH; wait 10; KILL A; iCountCutOffBH = iCountCutOffBH +1; SLPKP(2) = 0; wait 100; disable A; SLPKP(2) = rBakPKPA;  RET\n",	// 113
	"!EndBufferProg"		// 114, LINE_END_SCRIPT_ACS_SC_BUFFER_7_
};
// !ON iFlagEnableEmergencyCheck = 0 & MST2.#ENABLED;  wait 10; SLPKP(2) = rBakPKPA ; RET;
// Local debug text
static char strDebugText[128];
static char cFlagEnableDebug;
static char strAppendACS_Buffer7[512];
static char strDownloadCmd[256];
static char strUploadMessage[256];
static char *strStopString;

extern HANDLE stCommHandleBuffProgACS; // communication handle

#include "MtnInitAcs.h"
// 2016Q2
//ifdef __ACS_BUFF_7
short acs_sc_update_buffer_7_from_ram_para_(UPDATE_ACS_SC_BUFFER_RMS_CALC *stpUpdate_buffer_acs_sc)
{
short sRet = MTN_API_OK_ZERO;

	// download buffer program
	int ii = 0;

	for(ii =0; ii< LINE_ID_SP_DEF_PARA_DECLARE_; ii++)  //  20120906
	{
		if (!acsc_AppendBuffer( stCommHandleBuffProgACS, // communication handle
			BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
			strBufferProgram_OpenCtrlACS_SC_Para[ii], // buffer contained ACSPL+ program(s)
			(int)strlen(strBufferProgram_OpenCtrlACS_SC_Para[ii]), // size of this buffer
			NULL // waiting call
			))
		{
			sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
				acsc_GetLastError());

			sRet = MTN_API_ERR_START_FIRMWARE_PROG;
			goto label_acs_sc_update_buffer_7_from_ram_para_;
		}
	}

int iCountReceived;
int iTimeout = 50;

	//JerkFF_AxisZ  // 16
double dJerkFF_AxisZ;
	sprintf_s(strDownloadCmd, 256, "?GetSP(1, getspa(1, \"axes[0].SLJFF\"))\n\r"); //
	if(acsc_Transaction(stCommHandleBuffProgACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dJerkFF_AxisZ = strtod(strUploadMessage, &strStopString);
			sprintf_s(strAppendACS_Buffer7, 512, "rJerkFF_AxisZ = %16.10f;\n", dJerkFF_AxisZ);                      
			
			acsc_WriteReal(stCommHandleBuffProgACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "rJerkFF_AxisZ", 0, 0, 0, 0, &dJerkFF_AxisZ, NULL );
			if (!acsc_AppendBuffer( stCommHandleBuffProgACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_sc_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleBuffProgACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_SC_Para[16], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_SC_Para[16]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_sc_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}

	//JerkFF_AxisX  // 17
double dJerkFF_AxisX;
	sprintf_s(strDownloadCmd, 256, "?GetSP(0, getspa(0, \"axes[1].SLJFF\"))\n\r"); //
	if(acsc_Transaction(stCommHandleBuffProgACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dJerkFF_AxisX = strtod(strUploadMessage, &strStopString);
			sprintf_s(strAppendACS_Buffer7, 512, "rJerkFF_AxisX = %16.10f;\n", dJerkFF_AxisX);                      
			
			acsc_WriteReal(stCommHandleBuffProgACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "rJerkFF_AxisX", 0, 0, 0, 0, &dJerkFF_AxisX, NULL );
			if (!acsc_AppendBuffer( stCommHandleBuffProgACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_sc_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleBuffProgACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_SC_Para[17], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_SC_Para[17]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_sc_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}
	//JerkFF_AxisY  // 18
double dJerkFF_AxisY;
	sprintf_s(strDownloadCmd, 256, "?GetSP(0, getspa(0, \"axes[0].SLJFF\"))\n\r"); //
	if(acsc_Transaction(stCommHandleBuffProgACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dJerkFF_AxisY = strtod(strUploadMessage, &strStopString);
			sprintf_s(strAppendACS_Buffer7, 512, "rJerkFF_AxisY = %16.10f;\n", dJerkFF_AxisY);                      
			
			acsc_WriteReal(stCommHandleBuffProgACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "rJerkFF_AxisY", 0, 0, 0, 0, &dJerkFF_AxisY, NULL );
			if (!acsc_AppendBuffer( stCommHandleBuffProgACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_sc_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleBuffProgACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_SC_Para[18], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_SC_Para[18]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_sc_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}

	//SLDFF_AxisX  // 19
double dSLDFF_AxisX;
	sprintf_s(strDownloadCmd, 256, "?GetSP(0, getspa(0, \"axes[1].SLDFF\"))\n\r"); //
	if(acsc_Transaction(stCommHandleBuffProgACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dSLDFF_AxisX = strtod(strUploadMessage, &strStopString);
			sprintf_s(strAppendACS_Buffer7, 512, "rSLDFF_AxisX = %f;\n", dSLDFF_AxisX);                      
			
			acsc_WriteReal(stCommHandleBuffProgACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "rSLDFF_AxisX", 0, 0, 0, 0, &dSLDFF_AxisX, NULL );
			if (!acsc_AppendBuffer( stCommHandleBuffProgACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_sc_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleBuffProgACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_SC_Para[19], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_SC_Para[19]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_sc_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}

	//SLDFF_AxisY  // 20
double dSLDFF_AxisY;
	sprintf_s(strDownloadCmd, 256, "?GetSP(0, getspa(0, \"axes[0].SLDFF\"))\n\r"); //
	if(acsc_Transaction(stCommHandleBuffProgACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dSLDFF_AxisY = strtod(strUploadMessage, &strStopString);
			sprintf_s(strAppendACS_Buffer7, 512, "rSLDFF_AxisY = %f;\n", dSLDFF_AxisY);                      
			
			acsc_WriteReal(stCommHandleBuffProgACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "rSLDFF_AxisY", 0, 0, 0, 0, &dSLDFF_AxisY, NULL );
			if (!acsc_AppendBuffer( stCommHandleBuffProgACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_sc_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleBuffProgACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_SC_Para[20], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_SC_Para[20]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_sc_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}

	//SLDFF_AxisZ  // 21
double dSLDFF_AxisZ;
	sprintf_s(strDownloadCmd, 256, "?GetSP(1, getspa(1, \"axes[0].SLDFF\"))\n\r"); //
	if(acsc_Transaction(stCommHandleBuffProgACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dSLDFF_AxisZ = strtod(strUploadMessage, &strStopString);
			sprintf_s(strAppendACS_Buffer7, 512, "rSLDFF_AxisZ = %f;\n", dSLDFF_AxisZ);                      
			
			acsc_WriteReal(stCommHandleBuffProgACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "rSLDFF_AxisZ", 0, 0, 0, 0, &dSLDFF_AxisZ, NULL );
			if (!acsc_AppendBuffer( stCommHandleBuffProgACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_sc_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleBuffProgACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_SC_Para[21], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_SC_Para[21]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_sc_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}
//	"rKfromYtoZ = 1;\n",				// 22  2016Q2
double dAxisDecouple_from_y_to_z;
//    if(mtnapi_upload_acs_sc_sp_parameter_from_y_to_z(stCommHandleBuffProgACS, &dAxisDecouple_from_y_to_z) == MTN_API_OK_ZERO)
	{
		sprintf_s(strAppendACS_Buffer7, 512, "rKfromYtoZ = %16.10f;\n", stpUpdate_buffer_acs_sc->dDecouple_from_Y_to_Z);                      
		if (!acsc_AppendBuffer( stCommHandleBuffProgACS, // communication handle
			BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
			strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
			(int)strlen(strAppendACS_Buffer7), // size of this buffer
			NULL // waiting call
			))
		{
			sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
				acsc_GetLastError());
			sRet = MTN_API_ERR_START_FIRMWARE_PROG;
			goto label_acs_sc_update_buffer_7_from_ram_para_;
		}

	}
	//else
	//{
	//	if (!acsc_AppendBuffer( stCommHandleBuffProgACS, // communication handle
	//		BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
	//		strBufferProgram_OpenCtrlACS_SC_Para[22], // buffer contained ACSPL+ program(s)
	//		(int)strlen(strBufferProgram_OpenCtrlACS_SC_Para[22]), // size of this buffer
	//		NULL // waiting call
	//		))
	//	{
	//		sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
	//			acsc_GetLastError());
	//		sRet = MTN_API_ERR_START_FIRMWARE_PROG;
	//		goto label_acs_sc_update_buffer_7_from_ram_para_;
	//	}
	//	sRet = MTN_API_ERROR;
	//}
	sprintf_s(strAppendACS_Buffer7, 512, "rKfromZtoY = %16.10f;\n", stpUpdate_buffer_acs_sc->dDecouple_from_Z_to_Y);                      
		if (!acsc_AppendBuffer( stCommHandleBuffProgACS, // communication handle
			BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
			strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
			(int)strlen(strAppendACS_Buffer7), // size of this buffer
			NULL // waiting call
			))
		{
			sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
				acsc_GetLastError());
			sRet = MTN_API_ERR_START_FIRMWARE_PROG;
			goto label_acs_sc_update_buffer_7_from_ram_para_;
		}

	for(ii = 24; ii <= LINE_END_SCRIPT_ACS_SC_BUFFER_7_; ii++)
	{
		if (!acsc_AppendBuffer( stCommHandleBuffProgACS, // communication handle
			BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
			strBufferProgram_OpenCtrlACS_SC_Para[ii], // buffer contained ACSPL+ program(s)
			(int)strlen(strBufferProgram_OpenCtrlACS_SC_Para[ii]), // size of this buffer
			NULL // waiting call
			))
		{
			sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
			sRet = MTN_API_ERR_START_FIRMWARE_PROG;
			goto label_acs_sc_update_buffer_7_from_ram_para_;
		}
		Sleep(20);
	}
label_acs_sc_update_buffer_7_from_ram_para_:
	return sRet;
}

