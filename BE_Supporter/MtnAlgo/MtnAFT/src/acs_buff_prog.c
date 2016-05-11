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
// 20110123 
#include "WinTiming.h"

//#include "stdafx.h"
// Module acs_buff_prog.c
#include "stdlib.h"

#include "mtndefin.h"
#include "MtnApi.h"

#include "acs_buff_prog.h"
#include "MotAlgo_DLL.h"

// Local debug text
static char strDebugText[128];
static char cFlagEnableDebug;

static HANDLE stCommHandleACS; // communication handle

// 	"        wait 1; \n",  // Suggested by ACS, 20110123
	//"        wait 1; \n",  // Suggested by ACS, 20110123
	//"        if (iAFT_Axis = 0) spdc aScopeSP, 30000, 0.2, 0, iParAddress_X, 0 \n", // SP0:X_VEL
	//"        elseif (iAFT_Axis = 1) SPDC aScopeSP, 30000, 0.2, 1,iParAddress_Y,0 \n", // SP1:X_VEL
	//"        elseif (iAFT_Axis = 4) SPDC aScopeSP, 30000, 0.2, 0,iParAddress_A,0 \n", // SP0:A_VEL

static int nNumLinesInBuffer_Move1OpenLoopPWN; // Pseudo-White Noise
char *strFlagEndBuff = "!EndBufferProg";
// Controller Buffer Program#8
char *strBufferProgram_Move1_OpenLoopPWN[] = 
{
	"global REAL arMotorXProfile(4)(3); global REAL arMotorAProfile(5)(3); global REAL arBHCtrl(20)(4); global REAL arXCtrl(10)(4); global REAL arYCtrl(4)(4);\n",
	"global REAL rWaveForm(8)(2000);\n", 
	"global REAL rAFT_TrajCmdGenPeriod_ms; \n",				//  ! trajectory command period unit in mili-second, default 0.1, minimum 0.05
	"REAL rAFT_TrajectoryPoints_buffer8(3)(2000); \n",		// ! trajectory path point, command velocity
//	"global int iAFT_TrajLen_buffer8;\n",		// ! Actual trajectory length, dpr 0x300 
//	"global int dpr 0x304 iAFT_DPR_StartPathMotion_buffer8;\n",	// ! Flag to indicate starting a motion
	"GLOBAL REAL rAFT_Scope(7)(4200);\n",						// ! buffer for scope, data collection
	"global int iAFT_FlagMotion; ! 1: motion in close loop, 0: testing in open loop\n",  // ! Flags for different motion testing
	"global int iAFT_PrbsLen;\n",
	"global REAL rAFT_PrbsSampleTime_ms;\n",
	"global int iAFT_Axis;\n",
	"GLOBAL REAL  rAFT_PrbsArray(10000);\n",
	"GLOBAL int iFlagRunningBufferProg;\n",
	"global int aiBufferDataWb(100);\n",
	"global int i1stBondContactDoutReg, i2ndBondContactDoutReg;\n",
	"GLOBAL int nNumLoops;\n",                       // 20110727
	"int iCurrLoop;\n",
	"int iFlagPositiveVel;\n"
	"GLOBAL real rUppPosnLimit, rLowPosnLimit;\n",
	"GLOBAL real rActualJogVel, rAbsJogVel, rBakSLPKP;\n",      // 20110727
	"global int iBndZ_Axis;\n", // 20130328
	"int ii;\n",
	"global int iAFT_TrajLen_buffer8;\n",		// ! Actual trajectory length, not using DPR
	"global int iAFT_StartPathMotion_buffer8;\n",	// ! Flag to indicate starting a motion, not using DPR
    "int iVelStepPreMoveDist;\n",  // 20120807
	"rAFT_PrbsSampleTime_ms = CTIME;\n",  //   1.0;\n
	"rAFT_TrajCmdGenPeriod_ms = CTIME;\n", // 20110426
	"GLOBAL INT aScopeSP(30000);\n",
	"GLOBAL INT iParAddress_X, iParAddress_Y, iParAddress_A; \n",
	"GLOBAL INT iFlagDcSP; \n",
	"rUppPosnLimit = 5000; rLowPosnLimit = -5000;\n",  // 20110727
	"rAbsJogVel = 50000;\n",   // 20110727
	"iFlagDcSP = 0; \n"
//	"iParAddress_X = GETSPA(0,\"X_CP_L\");\n", // GETSPV(SP0:A_CP_L),   CP_L, VEL
//	"iParAddress_A = GETSPA(0,\"A_CP_L\");\n", //
//	"iParAddress_Y = GETSPA(1,\"X_CP_L\");\n", //
	"while(1)\n",								//! wait until host trigger
	"TILL iAFT_StartPathMotion_buffer8 = 1; iAFT_StartPathMotion_buffer8 = 0; iFlagRunningBufferProg = 0; ii = 0;\n",
	"    if  iAFT_FlagMotion = 0\n",
//	"		     DC/l rAFT_Scope, iAFT_PrbsLen, rAFT_PrbsSampleTime_ms, X_FVEL, Y_FVEL, A_FVEL, X_DOUT, Y_DOUT, A_DOUT\n",   // ! start data collection
	"        if iAFT_Axis = 0 \n",   // 20130328
	"            DC/l rAFT_Scope, iAFT_PrbsLen, rAFT_PrbsSampleTime_ms, FVEL(0), FVEL(1), FVEL(iBndZ_Axis), DOUT(0), DOUT(1), DOUT(iBndZ_Axis), FPOS(0)\n",   // ! start data collection
	"        elseif iAFT_Axis = 1 \n",
	"            DC/l rAFT_Scope, iAFT_PrbsLen, rAFT_PrbsSampleTime_ms, FVEL(0), FVEL(1), FVEL(iBndZ_Axis), DOUT(0), DOUT(1), DOUT(iBndZ_Axis), FPOS(1)\n",   // ! start data collection
	"        elseif iAFT_Axis = iBndZ_Axis \n",
	"            DC/l rAFT_Scope, iAFT_PrbsLen, rAFT_PrbsSampleTime_ms, FVEL(0), FVEL(1), FVEL(iBndZ_Axis), DOUT(0), DOUT(1), DOUT(iBndZ_Axis), FPOS(iBndZ_Axis)\n",   // ! start data collection
	"        end \n",
	"        if iFlagDcSP = 1 \n", 
	"            if (iAFT_Axis = 0) spdc aScopeSP, 30000, 0.2, 0, iParAddress_X, 0 \n", // SP0:X_VEL
	"            elseif (iAFT_Axis = 1) SPDC aScopeSP, 30000, 0.2, 1,iParAddress_Y,0 \n", // SP1:X_VEL
	"            elseif (iAFT_Axis = iBndZ_Axis) SPDC aScopeSP, 30000, 0.2, 0,iParAddress_A,0 \n", // SP0:A_VEL  // 20130328
    "            end \n",
	"        end \n",
//	"        else \n",
//	"        MFLAGS(iAFT_Axis).1=1; MFLAGS(iAFT_Axis).17=0;\n",
	"        loop iAFT_PrbsLen;             SLIOFFS(iAFT_Axis) =  rAFT_PrbsArray(ii); ii = ii + 1;         end\n",  // DCOM
//	"        MFLAGS(iAFT_Axis).1=0; MFLAGS(iAFT_Axis).17=1; \n",
	"		 SLIOFFS(iAFT_Axis)=0; \n",  // DCOM
	"    elseif iAFT_FlagMotion = 1\n",
	"        DC rAFT_Scope, 4200, rAFT_PrbsSampleTime_ms, FVEL(0), RVEL(0), FVEL(1), RVEL(1), FVEL(iBndZ_Axis), RVEL(iBndZ_Axis)\n",  // 20110425
	"!EndBufferProg"
};

char *strBufferProgram_Move1_OpenLoopPWN_Sec2_PATH_012[] = 
{
	"        iFlagRunningBufferProg = 1; PATH/r 012, rAFT_TrajCmdGenPeriod_ms; MPOINT 012, rAFT_TrajectoryPoints_buffer8, iAFT_TrajLen_buffer8; ends 012\n"
};

char *strBufferProgram_Move1_OpenLoopPWN_Sec2_PATH_XYA[] = 
{
	"        iFlagRunningBufferProg = 1; PATH/r XYA, rAFT_TrajCmdGenPeriod_ms; MPOINT XYA, rAFT_TrajectoryPoints_buffer8, iAFT_TrajLen_buffer8; ends XYA\n"
};

char *strBufferProgram_Move1_OpenLoopPWN_Sec2[] = 
{
	"		 wait(iAFT_TrajLen_buffer8 * rAFT_TrajCmdGenPeriod_ms); iFlagRunningBufferProg = 0; \n",
	"    elseif iAFT_FlagMotion = 3\n",                       // 20110727
	"       iVelStepPreMoveDist = abs(rUppPosnLimit - rLowPosnLimit)/4; \n",  // 20120807
	"		rActualJogVel = abs(rAbsJogVel); iFlagPositiveVel = 1;\n",
	"		if (FPOS(iAFT_Axis) > rUppPosnLimit) rActualJogVel = -abs(rAbsJogVel); end;\n",
	"		if (FPOS(iAFT_Axis) < rLowPosnLimit) rActualJogVel = abs(rAbsJogVel); end;\n",
	"		iCurrLoop = 0; \n",
	"		JOG/V (iAFT_Axis), rActualJogVel;\n",
	"           block\n",
	"             while(iCurrLoop < nNumLoops)\n",
	"		      if  (FPOS(iAFT_Axis) > rUppPosnLimit)  iFlagPositiveVel = 1; rActualJogVel = -abs(rAbsJogVel); JOG/V (iAFT_Axis), rActualJogVel; end;\n",
	"		      if  (FPOS(iAFT_Axis) < rLowPosnLimit)  if(FVEL(iAFT_Axis) < 0 & iFlagPositiveVel = 1) iCurrLoop = iCurrLoop + 1; iFlagPositiveVel = 0; end rActualJogVel = abs(rAbsJogVel); JOG/V (iAFT_Axis), rActualJogVel;  end;\n",
	"		      if ((iCurrLoop = 6) | (iCurrLoop = 12))  DC rAFT_Scope, 3000, CTIME, RPOS(iAFT_Axis), FVEL(iAFT_Axis), RVEL(iAFT_Axis), FACC(iAFT_Axis),  DOUT(iAFT_Axis); end;\n",
	"             end\n",
	"           end\n",
//	"		end\n",20130128
	"		kill iAFT_Axis;  rBakSLPKP = SLPKP(iAFT_Axis); SLPKP(iAFT_Axis) = 0; wait 100; disable iAFT_Axis; SLPKP(iAFT_Axis) = rBakSLPKP; wait 100; enable iAFT_Axis; \n",                        // 20110727        PTP iAFT_Axis, EOFFS(iAFT_Axis);
	"    else\n",
	"        iFlagRunningBufferProg = 1; ACC(0)  = arMotorXProfile(aiBufferDataWb(0))(1); ACC(1) = arMotorXProfile(aiBufferDataWb(0))(1); X_DEC  = arMotorXProfile(aiBufferDataWb(0))(1); Y_DEC = arMotorXProfile(aiBufferDataWb(0))(1); X_JERK = arMotorXProfile(aiBufferDataWb(0))(2); Y_JERK = arMotorXProfile(aiBufferDataWb(0))(2);\n",
	"        DC rWaveForm, 1000, 1, RPOS(0), FPOS(0), RPOS(1), FPOS(1), RPOS(4), FPOS(4), DCOM(5), MFLAGS(4);\n",
	"        SLVKP(0) = arXCtrl(aiBufferDataWb(1))(0); SLVKI(0) = arXCtrl(aiBufferDataWb(1))(1); SLPKP(0) = arXCtrl(aiBufferDataWb(1))(2); SLAFF(0) = arXCtrl(aiBufferDataWb(1))(3); SLVKP(1) = arYCtrl(aiBufferDataWb(2))(0); SLVKI(1) = arYCtrl(aiBufferDataWb(2))(1); SLPKP(1) = arYCtrl(aiBufferDataWb(2))(2); SLAFF(1) = arYCtrl(aiBufferDataWb(2))(3); PTP/m XY, aiBufferDataWb(3), aiBufferDataWb(4);\n",
	"        TILL X_GRTIME <= aiBufferDataWb(5); SLVKP(iAFT_Axis) = arBHCtrl(2)(0); SLVKI(iAFT_Axis) = arBHCtrl(2)(1); SLPKP(iAFT_Axis) = arBHCtrl(2)(2); SLAFF(iAFT_Axis) = arBHCtrl(2)(3); ACC(iAFT_Axis) = arMotorAProfile(0)(1); DEC(iAFT_Axis) = arMotorAProfile(0)(1); JERK(iAFT_Axis) = arMotorAProfile(0)(2); \n",
	"        PTP/f A, aiBufferDataWb(6), abs(aiBufferDataWb(7));\n",
	"        TILL ^AST(4).#MOVE; SLVKP(iAFT_Axis) = arBHCtrl(1)(0); SLVKI(iAFT_Axis) = arBHCtrl(1)(1); SLPKP(iAFT_Axis) = arBHCtrl(1)(2); SLAFF(iAFT_Axis) = arBHCtrl(1)(3); ACC(iAFT_Axis) = 1E9; DEC(iAFT_Axis) = 1E9; JERK(iAFT_Axis) = 1E11; JOG/v iAFT_Axis, aiBufferDataWb(7);\n",
	"        wait aiBufferDataWb(37); if (aiBufferDataWb(25) > 0.5) TILL ((abs(PE(iAFT_Axis)) > aiBufferDataWb(8)) & (abs(FVEL(iAFT_Axis)) < 10000)) | (FPOS(iAFT_Axis) < aiBufferDataWb(9)); else wait 2; end;  KILL iAFT_Axis; i1stBondContactDoutReg = DOUT(iAFT_Axis); DCOM(iAFT_Axis) = aiBufferDataWb(10) / 1000; MFLAGS(iAFT_Axis).1=1;\n", //  A_MFLAGS.17=0;
	"        if (aiBufferDataWb(25) <= 0.5) wait 5; else if (aiBufferDataWb(11) > 0) DCOM(iAFT_Axis)=aiBufferDataWb(12) / 1000; wait aiBufferDataWb(11); end; if (aiBufferDataWb(13) > 0) DCOM(iAFT_Axis) = aiBufferDataWb(14) / 1000; wait aiBufferDataWb(13); end; if (aiBufferDataWb(15) > 0) DCOM(iAFT_Axis)=aiBufferDataWb(16) / 1000; wait aiBufferDataWb(15); end; end; KILL iAFT_Axis;\n",
	"        MFLAGS(iAFT_Axis).1=0; \n",  // A_MFLAGS.17=1; A_DCOM = i2ndBondContactDoutReg/32767;
	"        wait 1; ACC(0) = arMotorXProfile(0)(1); ACC(1) = arMotorXProfile(0)(1); DEC(0) = arMotorXProfile(0)(1); DEC(1) = arMotorXProfile(0)(1); JERK(0) = arMotorXProfile(0)(2); JERK(1) = arMotorXProfile(0)(2); SLVKP(iAFT_Axis) = arBHCtrl(3)(0); SLVKI(iAFT_Axis) = arBHCtrl(3)(1); SLPKP(iAFT_Axis) = arBHCtrl(3)(2); SLAFF(iAFT_Axis) = arBHCtrl(3)(3); ACC(iAFT_Axis) = arMotorAProfile(1)(1); DEC(iAFT_Axis) = arMotorAProfile(1)(1); JERK(iAFT_Axis) = arMotorAProfile(1)(2); \n",
    "        PTP/er iAFT_Axis, aiBufferDataWb(17); TILL abs(PE(iAFT_Axis)) < 1000;\n",
	"!EndBufferProg"
};

char *strBufferProgram_Move1_OpenLoopPWN_Sec3_PathXYA[] =
{
	"        PTP/mr XYA, aiBufferDataWb(18), aiBufferDataWb(19), aiBufferDataWb(20);\n"
};

char *strBufferProgram_Move1_OpenLoopPWN_Sec3_Path012[] =
{
	"        PTP/mr 012, aiBufferDataWb(18), aiBufferDataWb(19), aiBufferDataWb(20);\n"
};

char *strBufferProgram_Move1_OpenLoopPWN_Sec3[] =
{
	"        TILL ^AST(4).#MOVE; TILL abs(PE(0)) < 1000; TILL abs(PE(1)) < 1000; TILL abs(PE(iAFT_Axis)) < 1000;\n",
	"        SLVKP(iAFT_Axis) = arBHCtrl(4)(0); SLVKI(iAFT_Axis) = arBHCtrl(4)(1); SLPKP(iAFT_Axis) = arBHCtrl(4)(2); SLAFF(iAFT_Axis) = arBHCtrl(4)(3); ACC(iAFT_Axis) = arMotorAProfile(2)(1); DEC(iAFT_Axis) = arMotorAProfile(2)(1); JERK(iAFT_Axis) = arMotorAProfile(2)(2); \n",
	"        PTP/r iAFT_Axis, aiBufferDataWb(21); \n",  //  if (IntData(70) = 2) PTP/mr XYA, IntData(71), IntData(72), IntData(73); else PTP/r A, IntData(21); end
	"        TILL ^AST(4).#MOVE; SLVKP(iAFT_Axis) = arBHCtrl(5)(0); SLVKI(iAFT_Axis) = arBHCtrl(5)(1); SLPKP(iAFT_Axis) = arBHCtrl(5)(2); SLAFF(iAFT_Axis) = arBHCtrl(5)(3);\n",
	"!EndBufferProg"
};

char *strBufferProgram_Move1_OpenLoopPWN_Sec4PathXYA[] = 
{
	"        PATH/r XYA, 1; mpoint XYA, rAFT_TrajectoryPoints_buffer8, aiBufferDataWb(22); ends XYA;\n"
};// NOT 0.25

char *strBufferProgram_Move1_OpenLoopPWN_Sec4Path012[] = 
{
	"        PATH/r 012, 1; mpoint 012, rAFT_TrajectoryPoints_buffer8, aiBufferDataWb(22); ends 012;\n"
};// NOT 0.25

char *strBufferProgram_Move1_OpenLoopPWN_Sec4[] = 
{
	"        SLVKP(iAFT_Axis) = arBHCtrl(8)(0); SLVKI(iAFT_Axis) = arBHCtrl(8)(1); SLPKP(iAFT_Axis) = arBHCtrl(8)(2); SLAFF(iAFT_Axis) = arBHCtrl(8)(3); ACC(iAFT_Axis) = 1E9; DEC(iAFT_Axis) = 1E9; JERK(iAFT_Axis) = 1E11; JOG/v iAFT_Axis, aiBufferDataWb(23); \n", // PTP/m XY, ?, ?;
    "        wait aiBufferDataWb(24); if (aiBufferDataWb(25) >= 0.5) TILL ((abs(PE(iAFT_Axis)) > aiBufferDataWb(26)) & (abs(FVEL(iAFT_Axis)) < 10000)) | (FPOS(iAFT_Axis) < aiBufferDataWb(27)); else wait 2; end; KILL (iAFT_Axis); i2ndBondContactDoutReg = DOUT(iAFT_Axis); DCOM(iAFT_Axis) = aiBufferDataWb(28) / 1000; MFLAGS(iAFT_Axis).1=1; \n",    // "		 if (IntData(55) = 1) rSpeed = IntData(56) / 100.0; else rSpeed = IntData(52) / 100.0; end; X_ACC = arMotorXProfile(0)(1) * rSpeed; Y_ACC = arMotorXProfile(0)(1) * rSpeed; X_DEC = arMotorXProfile(0)(1) * rSpeed; Y_DEC = arMotorXProfile(0)(1) * rSpeed; X_JERK = arMotorXProfile(0)(2) * rSpeed; Y_JERK = arMotorXProfile(0)(2) * rSpeed; lBondProgress = 5; A_MFLAGS.17=0; 
    "        if (aiBufferDataWb(25) <= 0.5) wait 2; else if (aiBufferDataWb(29) > 0) DCOM(iAFT_Axis) = aiBufferDataWb(30) / 1000; wait aiBufferDataWb(29); end; end; \n", // if (IntData(55) = 1) A_DCOM=IntData(58) / 1000; PTP/m XY, IntData(60), IntData(61); PTP/mr XY, IntData(64), IntData(65); PTP/mr XY, -IntData(64) * 2.0, -IntData(65) * 2.0; PTP/mr XY, IntData(64), IntData(65); PTP/m XY, IntData(62), IntData(63); wait IntData(59); rSpeed = IntData(52) / 100.0; X_ACC = arMotorXProfile(0)(1) * rSpeed; Y_ACC = arMotorXProfile(0)(1) * rSpeed; X_DEC = arMotorXProfile(0)(1) * rSpeed; Y_DEC = arMotorXProfile(0)(1) * rSpeed; X_JERK = arMotorXProfile(0)(2) * rSpeed; Y_JERK = arMotorXProfile(0)(2) * rSpeed; end;
    "        if (aiBufferDataWb(25) <= 0.5) wait 2; else if (aiBufferDataWb(31) > 0) DCOM(iAFT_Axis) = aiBufferDataWb(32) / 1000; wait aiBufferDataWb(31); end; if (aiBufferDataWb(33) > 0) DCOM(iAFT_Axis) = aiBufferDataWb(34) / 1000; wait aiBufferDataWb(33); end; end; \n", // PTP/m XY, IntData(39), IntData(40); // PTP/m XY, IntData(43), IntData(44);
    "        KILL (iAFT_Axis); MFLAGS(iAFT_Axis).1=0;  \n",  // A_MFLAGS.17=1; A_DCOM = i1stBondContactDoutReg /32767;
    "        wait 1; SLVKP(iAFT_Axis) = arBHCtrl(6)(0); SLVKI(iAFT_Axis) = arBHCtrl(6)(1); SLPKP(iAFT_Axis) = arBHCtrl(6)(2); SLAFF(iAFT_Axis) = arBHCtrl(6)(3); ACC(iAFT_Axis) = arMotorAProfile(3)(1); DEC(iAFT_Axis) = arMotorAProfile(3)(1); JERK(iAFT_Axis) = arMotorAProfile(3)(2); \n",
	"        PTP/r (iAFT_Axis), aiBufferDataWb(35); \n", //    "        if (IntData(36) > 0) TILL A_GRTIME <= IntData(36); lWclBondCmdCode = 3; TILL ^AST(4).#MOVE; else TILL ^AST(4).#MOVE; wait abs(IntData(36)); lWclBondCmdCode = 3; end; if (IntData(35) = 1) wait 1000; else wait 2; end; TILL abs(A_PE) < 1000; if (IntData(35) < 2) if (IntData(37) = 1) lTrigNext1stPt1 = 1; else lTrigNext1stPt1 = 0; end; end; OUT0.2 = 0;
    "        TILL ^AST(iAFT_Axis).#MOVE; OUT0.2 = 1; SLVKP(iAFT_Axis) = arBHCtrl(7)(0); SLVKI(iAFT_Axis) = arBHCtrl(7)(1); SLPKP(iAFT_Axis) = arBHCtrl(7)(2); SLAFF(iAFT_Axis) = arBHCtrl(7)(3); ACC(iAFT_Axis) = arMotorAProfile(4)(1); DEC(iAFT_Axis) = arMotorAProfile(4)(1); JERK(iAFT_Axis) = arMotorAProfile(4)(2); \n",
	"        PTP (iAFT_Axis), aiBufferDataWb(36); \n",
	"        TILL ^AST(iAFT_Axis).#MOVE; SLVKP(iAFT_Axis) = arBHCtrl(0)(0); SLVKI(iAFT_Axis) = arBHCtrl(0)(1); SLPKP(iAFT_Axis) = arBHCtrl(0)(2); SLAFF(iAFT_Axis) = arBHCtrl(0)(3); ACC(iAFT_Axis) = arMotorAProfile(0)(1); DEC(iAFT_Axis) = arMotorAProfile(0)(1); JERK(iAFT_Axis) = arMotorAProfile(0)(2); iFlagRunningBufferProg = 0;\n",
	"    end\n",
	"end\n",
	"STOP\n",
	"!EndBufferProg"
};

// Controller Buffer Program#9
static int nNumLinesInBuffer_VelLoop_Move2;
char *strBufferProgram_VelLoop_Move2[] = 
{
	"global REAL arMotorXProfile(4)(3); global REAL arMotorAProfile(5)(3); global REAL arBHCtrl(20)(4);\n",
	"global REAL rWaveForm(8)(2000);\n", 
	"global REAL rAFT_TrajCmdGenPeriod_ms; \n", // ! declared in buffer 8
	"GLOBAL REAL rAFT_Scope(7)(4200);\n", // ! buffer for scope, data collection // // already defined, must be the same as in buffer-nNumLinesInBuffer_Move1OpenLoopPWN
	"global int iAFT_FlagMotion; ! 1: motion in close loop, 0: testing in open loop\n",  // ! Flags for different motion testing
	"REAL rAFT_TrajectoryPoints_buffer9(3)(2000); \n",   // ! trajectory path point, command velocity
	"global int iAFT_TrajLen_buffer9; \n", // ! Actual trajectory length, not using DPR, dpr 0x308 
	"global int iAFT_StartPathMotion_buffer9;\n", // ! Flag to indicate starting a motion, not using DPR, dpr 0x30C 
	"real rBakPosnGain; \n",
	"GLOBAL int iFlagRunningBufferProg;\n",
	"global int iAFT_PrbsLen;\n",
	"global REAL rAFT_PrbsSampleTime_ms;\n",
	"global int iAFT_Axis;\n",
	"global int iBndZ_Axis; \n",  // 20130328
	"GLOBAL REAL  rAFT_PrbsArray(10000);\n",
	"GLOBAL INT aScopeSP(30000);\n",
	"GLOBAL INT iFlagDcSP; \n",
	"GLOBAL INT iParAddress_X, iParAddress_Y, iParAddress_A; \n",
	"rAFT_PrbsSampleTime_ms = CTIME;\n",  //  
	"iBndZ_Axis = 4; \n",  // 20130328
//	"iParAddress_X = GETSPA(0,\"X_CP_L\");\n", // GETSPV(SP0:A_CP_L),   CP_L, VEL
//	"iParAddress_A = GETSPA(0,\"A_CP_L\");\n", //
//	"iParAddress_Y = GETSPA(1,\"X_CP_L\");\n", //
	"while(1)\n", 
    "   TILL iAFT_StartPathMotion_buffer9 = 1; iAFT_StartPathMotion_buffer9 = 0; iFlagRunningBufferProg = 0; \n", // ! wait until host trigger
	"    if  iAFT_FlagMotion = 0\n",
	"        iFlagRunningBufferProg = 1;rBakPosnGain = SLPKP(iAFT_Axis); SLPKP(iAFT_Axis) = 0; \n",
	"        if iAFT_Axis = 0 \n",
	"            DC/l rAFT_Scope, iAFT_PrbsLen, rAFT_PrbsSampleTime_ms, FVEL(0), FVEL(1), FVEL(iAFT_Axis), DOUT(0), DOUT(1), DOUT(iAFT_Axis), FPOS(0)\n",   // ! start data collection
	"        elseif iAFT_Axis = 1 \n",
	"            DC/l rAFT_Scope, iAFT_PrbsLen, rAFT_PrbsSampleTime_ms, FVEL(0), FVEL(1), FVEL(iAFT_Axis), DOUT(0), DOUT(1), DOUT(iAFT_Axis), FPOS(1)\n",   // ! start data collection
	"        elseif iAFT_Axis = iBndZ_Axis \n", // 20130328
	"            DC/l rAFT_Scope, iAFT_PrbsLen, rAFT_PrbsSampleTime_ms, FVEL(0), FVEL(1), FVEL(iAFT_Axis), DOUT(0), DOUT(1), DOUT(iAFT_Axis), FPOS(iAFT_Axis)\n",   // ! start data collection
	"        end \n",
	"        if iFlagDcSP = 1 \n", 
	"            if (iAFT_Axis = 0) spdc aScopeSP, 30000, 0.2, 0, iParAddress_X, 0 \n", // SP0:X_VEL
	"            elseif (iAFT_Axis = 1) SPDC aScopeSP, 30000, 0.2, 1,iParAddress_Y,0 \n", // SP1:X_VEL
	"            elseif (iAFT_Axis = iBndZ_Axis) SPDC aScopeSP, 30000, 0.2, 0,iParAddress_A,0 \n", // SP0:A_VEL, 20130328
    "            end \n",
    "        end \n",
//	"        else \n",
	"        PATH/r (iAFT_Axis), rAFT_PrbsSampleTime_ms; MPOINT (iAFT_Axis), rAFT_PrbsArray, iAFT_PrbsLen; ends (iAFT_Axis)\n",
	"        SLPKP(iAFT_Axis) = rBakPosnGain; DCOM(iAFT_Axis)=0; iFlagRunningBufferProg = 0;\n",
	"    else\n",
	"!EndBufferProg"
};
//    "    DC rAFT_Scope, (iAFT_TrajLen_buffer9 * 2), rAFT_TrajCmdGenPeriod_ms, X_FVEL, X_RVEL, Y_FVEL, Y_RVEL, A_FVEL, A_RVEL\n", // ! start data collection

char *strBufferProgram_VelLoop_Move2_Sec2_PathXYA[] = 
{
	"    iFlagRunningBufferProg = 1; PATH/r XYA, rAFT_TrajCmdGenPeriod_ms; MPOINT XYA, rAFT_TrajectoryPoints_buffer9, iAFT_TrajLen_buffer9; ends XYA\n"
};
char *strBufferProgram_VelLoop_Move2_Sec2_Path012[] = 
{
	"    iFlagRunningBufferProg = 1; PATH/r 012, rAFT_TrajCmdGenPeriod_ms; MPOINT 012, rAFT_TrajectoryPoints_buffer9, iAFT_TrajLen_buffer9; ends 012\n"
};

char *strBufferProgram_VelLoop_Move2_Sec2[] = 
{
	"    wait(iAFT_TrajLen_buffer9 * rAFT_TrajCmdGenPeriod_ms); iFlagRunningBufferProg = 0; \n",
	"    end\n",
	"end\n",
	"STOP\n",
	"!EndBufferProg"
};

static int nNumLinesInBuffer_SearchContactForceControl;
char *strBufferProgram_SearchContactForceControl[] = 
{
	"! YYYYDDMM Author        Notes",
	"! 20081113      JohnZHAO  Port from ACS",
	"!Search for conntact",
	"! Pay attention that this program can run on X,Y,Z,T axes ONLY.",
	"! The input number in lines 36 and 37 should be changed per the axis in use:",
	"! 0 for X axis, 2 for Y axis, 4 for Z axis, 6 for T axis ",
	"GLOBAL INT AXIS ; AXIS=4 ", // !"A" axis 
	"INT SRCH_HEIGHT_POSN ;SRCH_HEIGHT_POSN= EOFFS(AXIS) - 1000", //   ! Will be used for distance till starting the search for input
	"INT START_VEL;START_VEL= -500000", //   ! Velocity till starting the search for input
	"INT iMaxAcc; iMaxAcc = 100;", // 20121105
	"INT iMaxJerk; iMaxJerk = 5;", // 20121105
	"INT SEARCH_VEL;SEARCH_VEL= -20000", // ! Search velocity 
	"INT TARGET_POINT;TARGET_POINT= EOFFS(AXIS)", // ! Relative position after receiving the input
	"INT ANTIBOUNCE ; ANTIBOUNCE= 5",   // ! Antibouncing in msec for the contact input
	"INT MAX_DIST; MAX_DIST= 8000",     // ! Maximum distance in case that no input was received
	"INT SEARCH_TOL_PE;  SEARCH_TOL_PE = 20",
	"INT FLAG_FIRST_DETECT;  FLAG_FIRST_DETECT = 0;",
	"real dPreImpForce_DCOM; int iHoldCountInitForce; iHoldCountInitForce = 5; dPreImpForce_DCOM = -1.0",
	"INT TIME1",
	"INT TIME2",
	"INT CONTACT_POSN_REG;",
	"INT SEARCH_CONTACT_STATUS;",
	"INT TIME_OUT;  TIME_OUT = 20000; ! 10 second at 2 KHz",
	"global int iAFT_StartSearchContact_buffer7; iAFT_StartSearchContact_buffer7 = 0",  // dpr 0x330 
	"global int iAFT_DoneSearchContact_buffer7; iAFT_DoneSearchContact_buffer7 = 1;",   // dpr 0x334 
	"INT iDeBounceCount; iDeBounceCount= 0;",
	"GLOBAL REAL rAFT_Scope(7)(4200);",
	"INT iFlagSwitchToForceControl; iFlagSwitchToForceControl = 0;",
	"INT nMaxForceSeg; nMaxForceSeg = 8;",
	"global int iCount_ForceSeg; iCount_ForceSeg = 0;",
	"real fForceBlk_LevelAmplitude(16);",
	"int iForceBlk_LevelCount(16);",
	"int iForceBlk_RampCount(16);",
	"int iRampCount; iRampCount = 0;",
	"int iLevelCount; iLevelCount = 0;",
	"int iRevHeight; iRevHeight = 400;",
	"int iKinkHeight; iKinkHeight = 100;",
	"int iLoopTop; iLoopTop = 1000;",
	"real fDeltaDriverCommand;",
	"real fCurrDriverCommand;",
	"global REAL arMotorXProfile(4)(3); global REAL arMotorYProfile(4)(3); global REAL arMotorAProfile(5)(3); global REAL arBHCtrl(20)(4); global REAL arXCtrl(10)(4); global REAL arYCtrl(4)(4); REAL rSpeed; global int lWclBondCmdCode; ", // 20120201
	"global int IntData(100);",
	"int iAutoTuneFlagBnd_A;",
	"real fStopSearchPosn, fContactPosn, rJerkBak, rAccBak;",// 20120212
//	"int lAddrDetectSw; int lAddrForceOffSw; int lAddrPreDrvCmd; int lAddrCtactDrvCmd; int lAddrLevelPE; int gnDetect;
	"int lAddrDetectSw; int lAddrForceOffSw; int lAddrPreDrvCmd; int lAddrCtactDrvCmd; int lAddrLevelPE;", // 20121030
	"int iSP_PreDrvCmd; int iSP_CtactDrvCmd; int gnDetect; gnDetect = 2;", // 20121030
	"lAddrDetectSw = GETSPA(0, \"DetectSW\"); lAddrForceOffSw = GETSPA(0, \"ForceOffSw\"); lAddrPreDrvCmd = GETSPA(0, \"PreDrvCmd\"); lAddrCtactDrvCmd = GETSPA(0, \"ContactDrvCmd\"); lAddrLevelPE = GETSPA(0, \"LevelPE\"); ", // 20130227, 20121030 PAR_ADDRESS = GETSPA(0,\"A_PE\");
	"!------------ Search for contact FSM---------------------------",
	"while(1)",
	"! wait until host trigger",
	"    TILL iAFT_StartSearchContact_buffer7 = 1;  iAFT_StartSearchContact_buffer7 = 0; iAFT_DoneSearchContact_buffer7 = 0; MFLAGS(AXIS).1=0; MFLAGS(AXIS).17=1; ",
	"    if iAutoTuneFlagBnd_A = 0 ",
	"		if(iFlagSwitchToForceControl = 0) DC rAFT_Scope, 3000, CTIME, FPOS(AXIS), RPOS(AXIS), PE(AXIS), RVEL(AXIS), SEARCH_CONTACT_STATUS, iDeBounceCount; else DC rAFT_Scope, 4200, CTIME, FPOS(AXIS), RPOS(AXIS), PE(AXIS), iDeBounceCount, DCOM(AXIS), AIN(0), RVEL(AXIS);  end ", //! MFLAGS(AXIS), FLAG_FIRST_DETECT, AIN(0)
	"		FLAG_FIRST_DETECT = 0; iDeBounceCount = 0; SEARCH_CONTACT_STATUS = 8888; ENABLE (AXIS); ",
	"       BLOCK",
	"           SETSP(0, lAddrDetectSw, 0); ", // 20130227
	"           rJerkBak = JERK(AXIS); rAccBak = ACC(AXIS); JERK(AXIS)=iMaxJerk * 1E9; ACC(AXIS)=iMaxAcc * 1E6; DEC(AXIS)=iMaxAcc* 1E6; VEL(AXIS) = 1.95E6; SLVKP(AXIS) = arBHCtrl(2)(0); SLVKI(AXIS) = arBHCtrl(2)(1); SLPKP(AXIS) = arBHCtrl(2)(2); SLAFF(AXIS) = arBHCtrl(2)(3);",
	"           PTP/f (AXIS), SRCH_HEIGHT_POSN,   abs(SEARCH_VEL); TIME2 = TIME + TIME_OUT; ",
	"       END",
	"       JOG/V (AXIS), SEARCH_VEL; wait 8; SLVKP(AXIS) = arBHCtrl(1)(0); SLVKI(AXIS) = arBHCtrl(1)(1); SLPKP(AXIS) = arBHCtrl(1)(2); SLAFF(AXIS) = arBHCtrl(1)(3); ! IST(AXIS).#MARK=0;",  // /VE, START_VEL,
	"       BLOCK",
	"           TILL RPOS(AXIS) <= SRCH_HEIGHT_POSN; ", // 20121030  rBuf1Ctact1 = FPOS(4);
	"           SETSP(0, lAddrLevelPE, SEARCH_TOL_PE);  SETSP(0, lAddrDetectSw, gnDetect);", // 20130227
	"       END",
	"		WHILE(TIME < TIME2 )           ",
	"           BLOCK",  
	"                 SETSP(0, lAddrPreDrvCmd, dPreImpForce_DCOM);  SETSP(0, lAddrCtactDrvCmd,dPreImpForce_DCOM);", // 20130227
	"                 IF (abs(FPOS(AXIS)) > MAX_DIST) GOTO ERROR; END; ",
	"                 if gnDetect = 0 IF ((abs(PE(AXIS)) > SEARCH_TOL_PE) & (FLAG_FIRST_DETECT = 0) )  iDeBounceCount = 0; TIME1 = 0; FLAG_FIRST_DETECT = 1; END;           IF (FLAG_FIRST_DETECT=1 & abs(PE(AXIS)) > SEARCH_TOL_PE ) iDeBounceCount = iDeBounceCount + 1; END;           IF (iDeBounceCount > ANTIBOUNCE) DCOM(AXIS) = DOUT(AXIS)/327.67; dPreImpForce_DCOM = DCOM(AXIS); GOTO SEARCH_CONTACT_GOT_IT; END; end;",
	//"                 if gnDetect = 0 IF (FLAG_FIRST_DETECT=1 & abs(PE(AXIS)) < SEARCH_TOL_PE ) iDeBounceCount = 0; FLAG_FIRST_DETECT=0 END; end;",
	"                 if gnDetect = 2 TILL MST4.#OPEN; end kill AXIS;  GOTO SEARCH_CONTACT_GOT_IT;", // DCOM(AXIS) = DOUT(AXIS)/327.67; dPreImpForce_DCOM = DCOM(AXIS); 
	"           END",
	"       END",
	"       BLOCK",
	"		    SETSP(0, lAddrForceOffSw, 1); ", // 20130227
	"           SEARCH_CONTACT_STATUS = 1;  kill(AXIS);     ENABLE (AXIS); MFLAGS(AXIS).1=0; MFLAGS(AXIS).17=1;  wait(8) PTP (AXIS), TARGET_POINT; GOTO FSM_SEARCH_CONTACT_ONCE; ! TIME_OUT ",
	"       END",
	"		SEARCH_CONTACT_GOT_IT:  SEARCH_CONTACT_STATUS = 0; CONTACT_POSN_REG = FPOS(AXIS);      if(iFlagSwitchToForceControl = 1) MFLAGS(AXIS).1=1; MFLAGS(AXIS).17=0; goto FSM_FORCE_CONTROL; end ", // DCOM(AXIS) = DOUT(AXIS)/327.67; dPreImpForce_DCOM = DCOM(AXIS);
	"       BLOCK",
	"		    SETSP(0, lAddrForceOffSw, 1); ", // 20130227
	"           KILL AXIS;ENABLE (AXIS); MFLAGS(AXIS).1=0; MFLAGS(AXIS).17=1; PTP/v (AXIS), TARGET_POINT, abs(START_VEL) ;   GOTO FSM_SEARCH_CONTACT_ONCE;",
	"       END",
	"       BLOCK", // 20121030
	"		FSM_FORCE_CONTROL: iCount_ForceSeg=0; iRampCount = 0; iLevelCount = 0; fCurrDriverCommand = DCOM(AXIS); wait(iHoldCountInitForce);",
	"		while(iCount_ForceSeg < nMaxForceSeg)         ",
	"			 if iForceBlk_RampCount(iCount_ForceSeg) > 0             fDeltaDriverCommand = (fForceBlk_LevelAmplitude(iCount_ForceSeg) - fCurrDriverCommand)/iForceBlk_RampCount(iCount_ForceSeg);             while(iRampCount < iForceBlk_RampCount(iCount_ForceSeg))                 fCurrDriverCommand = fCurrDriverCommand + fDeltaDriverCommand; DCOM(AXIS) = fCurrDriverCommand;                 iRampCount = iRampCount + 1;             end         end",
	"			iLevelCount = 0; DCOM(AXIS) = fForceBlk_LevelAmplitude(iCount_ForceSeg);         while( iLevelCount < iForceBlk_LevelCount(iCount_ForceSeg) )            iLevelCount = iLevelCount + 1;        end         iCount_ForceSeg = iCount_ForceSeg + 1; fCurrDriverCommand = DCOM(AXIS); iRampCount = 0; iLevelCount = 0;",
	"		end ", //! loop for nMaxForceSeg
	"       END;", // 20121030
	"		SETSP(0, lAddrForceOffSw, 1); ", // 20130227
	"       kill(AXIS);     ENABLE (AXIS); MFLAGS(AXIS).1=0; MFLAGS(AXIS).17=1; ",
	"		SLVKP(AXIS) = arBHCtrl(7)(0); SLVKI(AXIS) = arBHCtrl(7)(1); SLPKP(AXIS) = arBHCtrl(7)(2); SLAFF(AXIS) = arBHCtrl(7)(3); ACC(AXIS) = arMotorAProfile(4)(1); DEC(AXIS) = arMotorAProfile(4)(1); JERK(AXIS) = arMotorAProfile(4)(2); wait(1);",
	"		PTP (AXIS), TARGET_POINT;     GOTO FSM_SEARCH_CONTACT_ONCE;",
	"		ERROR:    SEARCH_CONTACT_STATUS = 2; KILL (AXIS);    ENABLE (AXIS);  MFLAGS(AXIS).1=0; MFLAGS(AXIS).17=1; PTP/v (AXIS), TARGET_POINT, abs(START_VEL)",
	"		FSM_SEARCH_CONTACT_ONCE:       JERK(AXIS) = rJerkBak; ACC(AXIS) = rAccBak; DEC(AXIS) = rAccBak; iAFT_DoneSearchContact_buffer7 = 1;",
	"! END of SearchContact With Force Control, for Force Calibration  ",
	"	else",
	"       DC rAFT_Scope, 3000, CTIME, RPOS(AXIS), PE(AXIS), RVEL(AXIS), FVEL(AXIS), DCOM(AXIS), DOUT(AXIS), SLAFF(AXIS); ",
	"		BLOCK",  // 20120201
	"		SLVKP(AXIS) = arBHCtrl(2)(0); SLVKI(AXIS) = arBHCtrl(2)(1); SLPKP(AXIS) = arBHCtrl(2)(2); SLAFF(AXIS) = arBHCtrl(2)(3); ACC(AXIS) = arMotorAProfile(0)(1); DEC(AXIS) = arMotorAProfile(0)(1); JERK(AXIS) = arMotorAProfile(0)(2); ",
	"		PTP/f (AXIS), IntData(3), abs(IntData(4));",
	"		END",
	"		fStopSearchPosn = IntData(3) - abs(IntData(4)/1000) * 8; JOG/v (AXIS), -abs(IntData(4)); TILL RPOS(AXIS) <= IntData(3); SLVKP(AXIS) = arBHCtrl(1)(0); SLVKI(AXIS) = arBHCtrl(1)(1); SLPKP(AXIS) = arBHCtrl(1)(2); SLAFF(AXIS) = arBHCtrl(1)(3); ",
	"		TILL FPOS(AXIS) <= fStopSearchPosn | RPOS(AXIS) <= CONTACT_POSN_REG; KILL (AXIS); ENABLE (AXIS); ",
	"		BLOCK",
	"		SLVKP(AXIS) = arBHCtrl(3)(0); SLVKI(AXIS) = arBHCtrl(3)(1); SLPKP(AXIS) = arBHCtrl(3)(2); SLAFF(AXIS) = arBHCtrl(3)(3); ACC(AXIS) = arMotorAProfile(1)(1); DEC(AXIS) = arMotorAProfile(1)(1); JERK(AXIS) = arMotorAProfile(1)(2);",
	"		PTP/re (AXIS), iRevHeight;",
	"		PTP/re (AXIS), iKinkHeight;",
	"		END",
	"		BLOCK",
	"		SLVKP(AXIS) = arBHCtrl(4)(0); SLVKI(AXIS) = arBHCtrl(4)(1); SLPKP(AXIS) = arBHCtrl(4)(2); SLAFF(AXIS) = arBHCtrl(4)(3); ACC(AXIS) = arMotorAProfile(2)(1); DEC(AXIS) = arMotorAProfile(2)(1); JERK(AXIS) = arMotorAProfile(2)(2);",
	"		PTP/re (AXIS), iLoopTop;",
	"		END",
	"		SLVKP(AXIS) = arBHCtrl(5)(0); SLVKI(AXIS) = arBHCtrl(5)(1); SLPKP(AXIS) = arBHCtrl(5)(2); SLAFF(AXIS) = arBHCtrl(5)(3); ACC(AXIS) = arMotorAProfile(0)(1); DEC(AXIS) = arMotorAProfile(0)(1); JERK(AXIS) = arMotorAProfile(0)(2);", // 20120802, use speed profile the same as move to 1st SrchHt
	"		PTP/f (AXIS), IntData(3), abs(IntData(4));",
	"		fStopSearchPosn = IntData(3) - abs(IntData(4)/1000) * 8; JOG/v (AXIS), -abs(IntData(4)); TILL RPOS(AXIS) <= IntData(3); SLVKP(AXIS) = arBHCtrl(1)(0); SLVKI(AXIS) = arBHCtrl(1)(1); SLPKP(AXIS) = arBHCtrl(1)(2); SLAFF(AXIS) = arBHCtrl(1)(3); ",
	"		TILL FPOS(AXIS) <= fStopSearchPosn | RPOS(AXIS) <= CONTACT_POSN_REG; KILL (AXIS); ENABLE AXIS; ",  // 2nd Bond Search&Stop
	"		BLOCK",
	"		SLVKP(AXIS) = arBHCtrl(6)(0); SLVKI(AXIS) = arBHCtrl(6)(1); SLPKP(AXIS) = arBHCtrl(6)(2); SLAFF(AXIS) = arBHCtrl(6)(3); ACC(AXIS) = arMotorAProfile(3)(1); DEC(AXIS) = arMotorAProfile(3)(1); JERK(AXIS) = arMotorAProfile(3)(2);",
	"       PTP/re (AXIS), IntData(31);"
	"		END",
	"       TILL ^AST(AXIS).#MOVE; SLVKP(AXIS) = arBHCtrl(7)(0); SLVKI(AXIS) = arBHCtrl(7)(1); SLPKP(AXIS) = arBHCtrl(7)(2); SLAFF(AXIS) = arBHCtrl(7)(3); ACC(AXIS) = arMotorAProfile(4)(1); DEC(AXIS) = arMotorAProfile(4)(1); JERK(AXIS) = arMotorAProfile(4)(2);", 
	"		BLOCK",
	"		PTP (AXIS), IntData(32);",
	"		END",  // 20120201
	"		TILL ^AST(AXIS).#MOVE; SLVKP(AXIS) = arBHCtrl(0)(0); SLVKI(AXIS) = arBHCtrl(0)(1); SLPKP(AXIS) = arBHCtrl(0)(2); SLAFF(AXIS) = arBHCtrl(0)(3);",
	"	end",
	"end",
	"STOP\n",
	"ON MST4.#OPEN; kill AXIS; RET",  // 20121030
	"!EndBufferProg"
};


//#include "acs_buff_prog.h"
short mtn_api_init_acs_buffer_prog()
{
	short sRet = MTN_API_OK_ZERO;
	// init buffer prog in buffer# BUFFER_ID_AFT_SPECTRUM_TEST
	sRet = acs_init_buffer_prog_prbs_prof_cfg_move();
	sRet = acs_init_buffer_prog_vel_loop_prof_cfg_move2();
	sRet = acs_init_buffer_prog_srch_contact_f_cali_auto_tune_z();
//	Sleep(50);
	return sRet;
}

short mtn_api_clear_acs_buffer_prof()
{
	short sRet = MTN_API_OK_ZERO;
	// clean the buffer program
	sRet = acs_clear_buffer_prog_prbs_prof_cfg_move();
	sRet = acs_clear_buffer_prog_vel_loop_prof_cfg_move2();
	sRet = acs_clear_buffer_prog_search_contact_force_control();

	return sRet;
}

short acs_buff_prog_init_comm(HANDLE stCommHandle)
{
	if(stCommHandle != ACSC_INVALID)  // NULL, 20120113, bug
	{
		stCommHandleACS = stCommHandle;
		return MTN_API_OK_ZERO;
	}
	else
	{
		return MTN_API_ERROR_COMMUNICATION_WITH_CONTROLLER;
	}
}
// 
short acs_compile_buffer_prog_srch_contact_f_cali_auto_tune_z()
{
	short sRet = MTN_API_OK_ZERO;

	if (!acsc_CompileBuffer( stCommHandleACS, // communication handle
		BUFFER_ID_SEARCH_CONTACT, // ACSPL+ program buffer number
		NULL // waiting call
		))
		{
			sprintf_s(strDebugText, 128, "Error: error compile buffer program- %d, Error Code: %d", BUFFER_ID_SEARCH_CONTACT,
				acsc_GetLastError());
#ifndef __MOTALGO_DLL
			if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
			sRet = MTN_API_ERR_START_FIRMWARE_PROG;
		}
	Sleep(100);
	return sRet;
}


short acs_init_buffer_prog_srch_contact_f_cali_auto_tune_z()
{
	short sRet = MTN_API_OK_ZERO;

	// download buffer program
	int ii = 0;
	while(strcmp(strBufferProgram_SearchContactForceControl[ii], strFlagEndBuff) != 0)
	{
		if((strstr(strBufferProgram_SearchContactForceControl[ii], "GETSP") == NULL
			&& strstr(strBufferProgram_SearchContactForceControl[ii], "SETSP") == NULL)
			|| sys_acs_communication_get_flag_sc_udi() == 0
			)// 20130227
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SEARCH_CONTACT, // ACSPL+ program buffer number
				strBufferProgram_SearchContactForceControl[ii], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_SearchContactForceControl[ii]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SEARCH_CONTACT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_init_buffer_prog_srch_contact_f_cali_auto_tune_z;
			}
			nNumLinesInBuffer_Move1OpenLoopPWN ++;  // 20130228
		}

		ii ++;
		Sleep(10);
	}

	Sleep(100);
	sRet = acs_compile_buffer_prog_srch_contact_f_cali_auto_tune_z();

label_acs_init_buffer_prog_srch_contact_f_cali_auto_tune_z:

	return sRet;

}


short acs_run_buffer_prog_srch_contact_f_cali_auto_tune_z()
{
	short sRet = MTN_API_OK_ZERO;
	if (!acsc_RunBuffer(stCommHandleACS, BUFFER_ID_SEARCH_CONTACT, NULL, NULL))
	{
		sprintf_s(strDebugText, 128, "Error: error start buffer program- %d, Error Code: %d", 
			BUFFER_ID_SEARCH_CONTACT,
			acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = MTN_API_ERR_START_FIRMWARE_PROG;
	}
	Sleep(10);
	return sRet;
}

short acs_stop_buffer_prog_srch_contact_f_cali_auto_tune_z()
{
	short sRet = MTN_API_OK_ZERO;
	if (!acsc_StopBuffer(stCommHandleACS, BUFFER_ID_SEARCH_CONTACT,NULL))
	{
		sprintf_s(strDebugText, 128, "Error: error stop buffer program- %d, Error Code: %d", BUFFER_ID_SEARCH_CONTACT,
			acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = MTN_API_ERR_START_FIRMWARE_PROG;
	}
	Sleep(10);
	return sRet;
}

short acs_clear_buffer_prog_srch_contact_f_cali_auto_tune_z()
{
	short sRet = MTN_API_OK_ZERO;

	if (!acsc_StopBuffer(stCommHandleACS, BUFFER_ID_SEARCH_CONTACT, NULL))
	{
		sRet = MTN_API_ERR_STOP_FIRMWARE_PROG;
		goto label_acs_clear_buffer_prog_srch_contact_f_cali_auto_tune_z;
	}

	if (!acsc_ClearBuffer(stCommHandleACS, BUFFER_ID_SEARCH_CONTACT, 1, ACSC_MAX_LINE, NULL)) // nNumLinesInBuffer_Move1OpenLoopPWN
	{
		sRet = MTN_API_ERR_CLEAR_FIRMWARE_PROG;
		goto label_acs_clear_buffer_prog_srch_contact_f_cali_auto_tune_z;
	}

label_acs_clear_buffer_prog_srch_contact_f_cali_auto_tune_z:
	return sRet;
}

//// Buffer PRBS and CFG move
short acs_init_buffer_prog_prbs_prof_cfg_move()
{
	short sRet = MTN_API_OK_ZERO;

	// download buffer program
	int ii = 0;
	while(strcmp(strBufferProgram_Move1_OpenLoopPWN[ii], strFlagEndBuff) != 0)
	{
		if((strstr(strBufferProgram_SearchContactForceControl[ii], "GETSP") == NULL
			&& strstr(strBufferProgram_SearchContactForceControl[ii], "SETSP") == NULL)
			|| sys_acs_communication_get_flag_sc_udi() == 0
			)// 20130227
		{

			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_AFT_SPECTRUM_TEST, // ACSPL+ program buffer number
				strBufferProgram_Move1_OpenLoopPWN[ii], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_Move1_OpenLoopPWN[ii]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_AFT_SPECTRUM_TEST,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_init_buffer_prog_prbs_prof_cfg_move;
			}
			nNumLinesInBuffer_Move1OpenLoopPWN ++;
		}
		ii ++;
		Sleep(10);
	}
	// = ii;
	// strBufferProgram_Move1_OpenLoopPWN_Sec2_PATH_012
	if(sys_acs_communication_get_flag_sc_udi() == TRUE)
	{
		acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_AFT_SPECTRUM_TEST, // ACSPL+ program buffer number
				strBufferProgram_Move1_OpenLoopPWN_Sec2_PATH_012[0], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_Move1_OpenLoopPWN_Sec2_PATH_012[0]), // size of this buffer
				NULL // waiting call
				);
	}
	else 
	{
		acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_AFT_SPECTRUM_TEST, // ACSPL+ program buffer number
				strBufferProgram_Move1_OpenLoopPWN_Sec2_PATH_XYA[0], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_Move1_OpenLoopPWN_Sec2_PATH_XYA[0]), // size of this buffer
				NULL // waiting call
				);
	}

	ii = 0;
	while(strcmp(strBufferProgram_Move1_OpenLoopPWN_Sec2[ii], strFlagEndBuff) != 0)
	{
		if((strstr(strBufferProgram_Move1_OpenLoopPWN_Sec2[ii], "GETSP") == NULL
			&& strstr(strBufferProgram_Move1_OpenLoopPWN_Sec2[ii], "SETSP") == NULL)
			|| sys_acs_communication_get_flag_sc_udi() == 0
			)// 20130227
		{

			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_AFT_SPECTRUM_TEST, // ACSPL+ program buffer number
				strBufferProgram_Move1_OpenLoopPWN_Sec2[ii], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_Move1_OpenLoopPWN_Sec2[ii]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_AFT_SPECTRUM_TEST,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_init_buffer_prog_prbs_prof_cfg_move;
			}
			nNumLinesInBuffer_Move1OpenLoopPWN ++;
		}
		ii ++;
		Sleep(10);
	}
	///////////////////// Section 3
	if(sys_acs_communication_get_flag_sc_udi() == TRUE)
	{
		acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_AFT_SPECTRUM_TEST, // ACSPL+ program buffer number
				strBufferProgram_Move1_OpenLoopPWN_Sec3_Path012[0], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_Move1_OpenLoopPWN_Sec3_Path012[0]), // size of this buffer
				NULL // waiting call
				);
	}
	else 
	{
		acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_AFT_SPECTRUM_TEST, // ACSPL+ program buffer number
				strBufferProgram_Move1_OpenLoopPWN_Sec3_PathXYA[0], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_Move1_OpenLoopPWN_Sec3_PathXYA[0]), // size of this buffer
				NULL // waiting call
				);
	}

	ii = 0;
	while(strcmp(strBufferProgram_Move1_OpenLoopPWN_Sec3[ii], strFlagEndBuff) != 0)
	{
		if((strstr(strBufferProgram_Move1_OpenLoopPWN_Sec3[ii], "GETSP") == NULL
			&& strstr(strBufferProgram_Move1_OpenLoopPWN_Sec3[ii], "SETSP") == NULL)
			|| sys_acs_communication_get_flag_sc_udi() == 0
			)// 20130227
		{

			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_AFT_SPECTRUM_TEST, // ACSPL+ program buffer number
				strBufferProgram_Move1_OpenLoopPWN_Sec3[ii], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_Move1_OpenLoopPWN_Sec3[ii]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_AFT_SPECTRUM_TEST,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_init_buffer_prog_prbs_prof_cfg_move;
			}
			nNumLinesInBuffer_Move1OpenLoopPWN ++;
		}
		ii ++;
		Sleep(10);
	}



    //////////////////// Section 4
	if(sys_acs_communication_get_flag_sc_udi() == TRUE)
	{
		acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_AFT_SPECTRUM_TEST, // ACSPL+ program buffer number
				strBufferProgram_Move1_OpenLoopPWN_Sec4Path012[0], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_Move1_OpenLoopPWN_Sec4Path012[0]), // size of this buffer
				NULL // waiting call
				);
	}
	else 
	{
		acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_AFT_SPECTRUM_TEST, // ACSPL+ program buffer number
				strBufferProgram_Move1_OpenLoopPWN_Sec4PathXYA[0], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_Move1_OpenLoopPWN_Sec4PathXYA[0]), // size of this buffer
				NULL // waiting call
				);
	}

	ii = 0;
	while(strcmp(strBufferProgram_Move1_OpenLoopPWN_Sec4[ii], strFlagEndBuff) != 0)
	{
		if((strstr(strBufferProgram_Move1_OpenLoopPWN_Sec4[ii], "GETSP") == NULL
			&& strstr(strBufferProgram_Move1_OpenLoopPWN_Sec4[ii], "SETSP") == NULL)
			|| sys_acs_communication_get_flag_sc_udi() == 0
			)// 20130227
		{

			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_AFT_SPECTRUM_TEST, // ACSPL+ program buffer number
				strBufferProgram_Move1_OpenLoopPWN_Sec4[ii], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_Move1_OpenLoopPWN_Sec4[ii]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_AFT_SPECTRUM_TEST,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_init_buffer_prog_prbs_prof_cfg_move;
			}
			nNumLinesInBuffer_Move1OpenLoopPWN ++;
		}
		ii ++;
		Sleep(10);
	}

	Sleep(50);
	sRet = acs_compile_buffer_prog_prbs_prof_cfg_move();

label_acs_init_buffer_prog_prbs_prof_cfg_move:

	return sRet;

}

short acs_compile_buffer_prog_prbs_prof_cfg_move()
{
	short sRet = MTN_API_OK_ZERO;
	// Compile buffer
	if (!acsc_CompileBuffer( stCommHandleACS, // communication handle
		BUFFER_ID_AFT_SPECTRUM_TEST, // ACSPL+ program buffer number
		NULL // waiting call
		))
		{
			sprintf_s(strDebugText, 128, "Error: error compile buffer program- %d, Error Code: %d", BUFFER_ID_AFT_SPECTRUM_TEST,
				acsc_GetLastError());
#ifndef __MOTALGO_DLL
			if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
			sRet = MTN_API_ERR_START_FIRMWARE_PROG;
		}
	Sleep(10);
	return sRet;

}

short acs_run_buffer_prog_prbs_prof_cfg_move()
{
	short sRet = MTN_API_OK_ZERO;
	if (!acsc_RunBuffer(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, NULL, NULL))
	{
		sprintf_s(strDebugText, 128, "Error: error start buffer program- %d, Error Code: %d", BUFFER_ID_AFT_SPECTRUM_TEST,
			acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = MTN_API_ERR_START_FIRMWARE_PROG;
	}
	Sleep(10);
	return sRet;
}

short acs_stop_buffer_prog_prbs_prof_cfg_move()
{
	short sRet = MTN_API_OK_ZERO;
	if (!acsc_StopBuffer(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST,NULL))
	{
		sprintf_s(strDebugText, 128, "Error: error stop buffer program- %d, Error Code: %d", BUFFER_ID_AFT_SPECTRUM_TEST,
			acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = MTN_API_ERR_START_FIRMWARE_PROG;
	}
	Sleep(10);
	return sRet;
}

short acs_clear_buffer_prog_prbs_prof_cfg_move()
{
	short sRet = MTN_API_OK_ZERO;

	if (!acsc_StopBuffer(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, NULL))
	{
		sRet = MTN_API_ERR_STOP_FIRMWARE_PROG;
		goto label_acs_clear_buffer_prog_prbs_prof_cfg_move;
	}

	if (!acsc_ClearBuffer(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, 1, ACSC_MAX_LINE, NULL)) // nNumLinesInBuffer_Move1OpenLoopPWN
	{
		sRet = MTN_API_ERR_CLEAR_FIRMWARE_PROG;
		goto label_acs_clear_buffer_prog_prbs_prof_cfg_move;
	}

label_acs_clear_buffer_prog_prbs_prof_cfg_move:
	return sRet;

}

// BUFFER_ID_VEL_TEST_PROF_MOVE_2
short acs_init_buffer_prog_vel_loop_prof_cfg_move2()
{
	short sRet = MTN_API_OK_ZERO;

	// download buffer program
	int ii = 0;
	while(strcmp(strBufferProgram_VelLoop_Move2[ii], strFlagEndBuff) != 0)
	{
		if((strstr(strBufferProgram_SearchContactForceControl[ii], "GETSP") == NULL
			&& strstr(strBufferProgram_SearchContactForceControl[ii], "SETSP") == NULL)
			|| sys_acs_communication_get_flag_sc_udi() == 0
			)// 20130227
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_VEL_TEST_PROF_MOVE_2, // ACSPL+ program buffer number
				strBufferProgram_VelLoop_Move2[ii], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_VelLoop_Move2[ii]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program - %d, Error Code: %d", BUFFER_ID_VEL_TEST_PROF_MOVE_2,
					acsc_GetLastError());
	#ifndef __MOTALGO_DLL
				if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
	#endif // __MOTALGO_DLL
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_init_buffer_prog_vel_loop_prof_cfg_move2;
			}
			nNumLinesInBuffer_VelLoop_Move2 ++;
		}
		ii ++;
		Sleep(10);
	}
	//  = ii;

	if(sys_acs_communication_get_flag_sc_udi() == TRUE)
	{
		acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_VEL_TEST_PROF_MOVE_2, // ACSPL+ program buffer number
				strBufferProgram_VelLoop_Move2_Sec2_Path012[0], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_VelLoop_Move2_Sec2_Path012[0]), // size of this buffer
				NULL // waiting call
				);
	}
	else 
	{
		acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_VEL_TEST_PROF_MOVE_2, // ACSPL+ program buffer number
				strBufferProgram_VelLoop_Move2_Sec2_PathXYA[0], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_VelLoop_Move2_Sec2_PathXYA[0]), // size of this buffer
				NULL // waiting call
				);
	}

	ii = 0;
	while(strcmp(strBufferProgram_VelLoop_Move2_Sec2[ii], strFlagEndBuff) != 0)
	{
		if((strstr(strBufferProgram_VelLoop_Move2_Sec2[ii], "GETSP") == NULL
			&& strstr(strBufferProgram_VelLoop_Move2_Sec2[ii], "SETSP") == NULL)
			|| sys_acs_communication_get_flag_sc_udi() == 0
			)// 20130227
		{

			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_VEL_TEST_PROF_MOVE_2, // ACSPL+ program buffer number
				strBufferProgram_VelLoop_Move2_Sec2[ii], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_VelLoop_Move2_Sec2[ii]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_AFT_SPECTRUM_TEST,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_init_buffer_prog_vel_loop_prof_cfg_move2;
			}
			nNumLinesInBuffer_VelLoop_Move2 ++;
		}
		ii ++;
		Sleep(10);
	}

	Sleep(50);
	sRet = acs_compile_buffer_prog_vel_loop_prof_cfg_move2();

label_acs_init_buffer_prog_vel_loop_prof_cfg_move2:

	return sRet;

}
short acs_compile_buffer_prog_vel_loop_prof_cfg_move2()
{
	short sRet = MTN_API_OK_ZERO;
	// Compile buffer
	if (!acsc_CompileBuffer( stCommHandleACS, // communication handle
		BUFFER_ID_VEL_TEST_PROF_MOVE_2, // ACSPL+ program buffer number
		NULL // waiting call
		))
		{
			sprintf_s(strDebugText, 128, "Error: error compile buffer program - %d, Error Code: %d", BUFFER_ID_VEL_TEST_PROF_MOVE_2, 
				acsc_GetLastError());
#ifndef __MOTALGO_DLL
			if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
			sRet = MTN_API_ERR_START_FIRMWARE_PROG;
		}
	Sleep(10);
	return sRet;

}
short acs_run_buffer_prog_vel_loop_prof_cfg_move2()
{
	short sRet = MTN_API_OK_ZERO;
	if (!acsc_RunBuffer(stCommHandleACS, BUFFER_ID_VEL_TEST_PROF_MOVE_2, NULL, NULL))
	{
		sprintf_s(strDebugText, 128, "Error: error start buffer program - %d, Error Code: %d", BUFFER_ID_VEL_TEST_PROF_MOVE_2,
			acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = MTN_API_ERR_START_FIRMWARE_PROG;
	}
	Sleep(10);
	return sRet;

}
short acs_stop_buffer_prog_vel_loop_prof_cfg_move2()
{
	short sRet = MTN_API_OK_ZERO;
	if (!acsc_StopBuffer(stCommHandleACS, BUFFER_ID_VEL_TEST_PROF_MOVE_2, NULL))
	{
		sprintf_s(strDebugText, 128, "Error: error start buffer program - %d, Error Code: %d", BUFFER_ID_VEL_TEST_PROF_MOVE_2,
			acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = MTN_API_ERR_START_FIRMWARE_PROG;
	}
	Sleep(10);
	return sRet;
}

short acs_clear_buffer_prog_vel_loop_prof_cfg_move2()
{
	short sRet = MTN_API_OK_ZERO;

	if (!acsc_StopBuffer(stCommHandleACS, BUFFER_ID_VEL_TEST_PROF_MOVE_2, NULL))
	{
		sRet = MTN_API_ERR_STOP_FIRMWARE_PROG;
		goto label_acs_clear_buffer_prog_vel_loop_prof_cfg_move2;
	}

	if (!acsc_ClearBuffer(stCommHandleACS, BUFFER_ID_VEL_TEST_PROF_MOVE_2, 1, ACSC_MAX_LINE, NULL))  // nNumLinesInBuffer_Move1OpenLoopPWN
	{
		sRet = MTN_API_ERR_CLEAR_FIRMWARE_PROG;
		goto label_acs_clear_buffer_prog_vel_loop_prof_cfg_move2;
	}

label_acs_clear_buffer_prog_vel_loop_prof_cfg_move2:
	return sRet;
}

// BUFFER_ID_SEARCH_CONTACT
short acs_init_buff_prog_search_contact_force_control()
{
	short sRet = MTN_API_OK_ZERO;

	// download buffer program, search contact force control
	int ii = 0;
	while(strcmp(strBufferProgram_SearchContactForceControl[ii], strFlagEndBuff) != 0)
	{
		if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
			BUFFER_ID_SEARCH_CONTACT, // ACSPL+ program buffer number
			strBufferProgram_SearchContactForceControl[ii], // buffer contained ACSPL+ program(s)
			(int)strlen(strBufferProgram_SearchContactForceControl[ii]), // size of this buffer
			NULL // waiting call
			))
		{
			sprintf_s(strDebugText, 128, "Error: error download buffer program, Error Code: %d", acsc_GetLastError());
#ifndef __MOTALGO_DLL
			if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
			sRet = MTN_API_ERR_START_FIRMWARE_PROG;
			goto label_acs_init_buffer_prog_search_contact_force_control;
		}
		ii ++;
	}
	nNumLinesInBuffer_SearchContactForceControl = ii;

	// Compile buffer
	if (!acsc_CompileBuffer( stCommHandleACS, // communication handle
		BUFFER_ID_SEARCH_CONTACT, // ACSPL+ program buffer number
		NULL // waiting call
		))
		{
			sprintf_s(strDebugText, 128, "Error: error compile buffer program, Error Code: %d", acsc_GetLastError());
#ifndef __MOTALGO_DLL
			if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
			sRet = MTN_API_ERR_START_FIRMWARE_PROG;
			goto label_acs_init_buffer_prog_search_contact_force_control;
		}
	// 
	if (!acsc_RunBuffer(stCommHandleACS, BUFFER_ID_SEARCH_CONTACT, NULL, NULL))
	{
		sprintf_s(strDebugText, 128, "Error: error start buffer program, Error Code: %d", acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = MTN_API_ERR_START_FIRMWARE_PROG;
		goto label_acs_init_buffer_prog_search_contact_force_control;
	}

label_acs_init_buffer_prog_search_contact_force_control:

	return sRet;
}

short acs_clear_buffer_prog_search_contact_force_control()
{
	short sRet = MTN_API_OK_ZERO;

	if (!acsc_StopBuffer(stCommHandleACS, BUFFER_ID_SEARCH_CONTACT, NULL))
	{
		sRet = MTN_API_ERR_STOP_FIRMWARE_PROG;
		goto label_acs_clear_buffer_prog_search_contact_force_control;
	}

	if (!acsc_ClearBuffer(stCommHandleACS, BUFFER_ID_SEARCH_CONTACT, 1, ACSC_MAX_LINE, NULL))  // nNumLinesInBuffer_SearchContactForceControl
	{
		sRet = MTN_API_ERR_CLEAR_FIRMWARE_PROG;
		goto label_acs_clear_buffer_prog_search_contact_force_control;
	}

label_acs_clear_buffer_prog_search_contact_force_control:
	return sRet;

}


int acs_bufprog_write_motion_flag_buff_8(int iFlag)
{
	short iRet = MTN_API_OK_ZERO;
	int iFlagMotion[1];  iFlagMotion[0] = iFlag;

	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "iAFT_FlagMotion", 0, 0, 0, 0, iFlagMotion, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	return iRet;
}

int acs_bufprog_start_buff_8()
{
	short iRet = MTN_API_OK_ZERO;
	int iFlagStartMotion[1];  iFlagStartMotion[0] = 1;
	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "iAFT_StartPathMotion_buffer8", 0, 0, 0, 0, iFlagStartMotion, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	return iRet;
}

static WB_ONE_WIRE_PERFORMANCE_CALC astWbEmuB1WPerform[NUM_TOTAL_WIRE_IN_ONE_SCOPE];

int acs_bufprog_start_buff_b1w_(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbEmuB1WPerformance)
{
	short iRet = MTN_API_OK_ZERO;
	int iFlagStartMotion[1];  iFlagStartMotion[0] = 1;
	int nTotalNumWire, iCountDataFailure = 0;

#ifdef __USE_BUFFER_8__
	int iFlagDoingTest;
	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_EMU_B1W, "iAFT_StartPathMotion_buffer8", 0, 0, 0, 0, iFlagStartMotion, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	acsc_ReadInteger(stCommHandleACS, BUFFER_ID_EMU_B1W, "iFlagRunningBufferProg", 0, 0, 0, 0, &iFlagDoingTest, NULL);
	while(iFlagDoingTest)
	{
		Sleep(50);
		acsc_ReadInteger(stCommHandleACS, BUFFER_ID_EMU_B1W, "iFlagRunningBufferProg", 0, 0, 0, 0, &iFlagDoingTest, NULL);
	}
#else
	acs_bufprog_0_restart_b1w();
	while(acs_buffprog_0_get_bond_progress() != 7)
	{
		Sleep(20);
	}

#endif

	// Upload waveform
	if(mtn_tune_upload_latest_wb_waveform(stCommHandleACS) != MTN_API_OK_ZERO)
	{
		iRet = MTN_API_ERROR_UPLOAD_DATA;
	}

	// Calculate performance index
	mtn_tune_ana_wb_waveform(astWbEmuB1WPerform, &nTotalNumWire);
	while(astWbEmuB1WPerform[0].iFlagHasWireInfo == 0)
	{
		iCountDataFailure ++;
		if(iCountDataFailure >= 3)
		{
			iRet = MTN_API_ERROR_UPLOAD_DATA;
		}
#ifdef __USE_BUFFER_8__
		if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_EMU_B1W, "iAFT_StartPathMotion_buffer8", 0, 0, 0, 0, iFlagStartMotion, NULL ))
		{
			iRet = MTN_API_ERROR_ACS_BUFF_PROG;
		}
		acsc_ReadInteger(stCommHandleACS, BUFFER_ID_EMU_B1W, "iFlagRunningBufferProg", 0, 0, 0, 0, &iFlagDoingTest, NULL);
		while(iFlagDoingTest)
		{
			Sleep(50);
			acsc_ReadInteger(stCommHandleACS, BUFFER_ID_EMU_B1W, "iFlagRunningBufferProg", 0, 0, 0, 0, &iFlagDoingTest, NULL);
		}
#else
		acs_bufprog_0_restart_b1w();
		while(acs_buffprog_0_get_bond_progress() != 7)
		{
			Sleep(20);
		}

#endif
		// Upload waveform
		if(mtn_tune_upload_latest_wb_waveform(stCommHandleACS) != MTN_API_OK_ZERO)
		{
			iRet = MTN_API_ERROR_UPLOAD_DATA;
		}
		// Calculate performance index
		mtn_tune_ana_wb_waveform(astWbEmuB1WPerform, &nTotalNumWire);
	}
	if(iRet == MTN_API_OK_ZERO)
	{
//		stpWbEmuB1WPerformance->astBondHeadMotionAft1stBond = astWbEmuB1WPerform[0].astBondHeadMotionAft1stBond;
		stpWbEmuB1WPerformance->idxEndOneWireXY = astWbEmuB1WPerform[0].idxEndOneWireXY;
		stpWbEmuB1WPerformance->idxEndOneWireZ = astWbEmuB1WPerform[0].idxEndOneWireZ;
		stpWbEmuB1WPerformance->iFlagHasWireInfo = astWbEmuB1WPerform[0].iFlagHasWireInfo;
		stpWbEmuB1WPerformance->iFlagIsDryRun = astWbEmuB1WPerform[0].iFlagIsDryRun;
		stpWbEmuB1WPerformance->nTimeB1W_cnt = astWbEmuB1WPerform[0].nTimeB1W_cnt;
		stpWbEmuB1WPerformance->nTimeB1W_S2E_cnt = astWbEmuB1WPerform[0].nTimeB1W_S2E_cnt;
		stpWbEmuB1WPerformance->nTotalBondHeadZ_MotionAft1stBond = astWbEmuB1WPerform[0].nTotalBondHeadZ_MotionAft1stBond;
		stpWbEmuB1WPerformance->stBondHeadPerformance = astWbEmuB1WPerform[0].stBondHeadPerformance;
		stpWbEmuB1WPerformance->stPosnsOfBondHeadZ = astWbEmuB1WPerform[0].stPosnsOfBondHeadZ;
		stpWbEmuB1WPerformance->stTimePointsOfBondHeadZ = astWbEmuB1WPerform[0].stTimePointsOfBondHeadZ;
	}
	return iRet;
}

int acs_bufprog_get_start_flag_buff_8(int *piFlagStartMotion)
{
	short iRet = MTN_API_OK_ZERO;
	if(!acsc_ReadInteger(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "iAFT_StartPathMotion_buffer8", 0, 0, 0, 0, piFlagStartMotion, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	return iRet;
	
}

int acs_bufprog_write_traj_len_buff_8(int iTrajLen)
{
	short iRet = MTN_API_OK_ZERO;
	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "iAFT_TrajLen_buffer8", 0, 0, 0, 0, &iTrajLen, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	return iRet;
}

int acs_bufprog_start_buff_9()
{
	short iRet = MTN_API_OK_ZERO;
	int iFlagStartMotion[1];  iFlagStartMotion[0] = 1;
	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_VEL_TEST_PROF_MOVE_2, "iAFT_StartPathMotion_buffer9", 0, 0, 0, 0, iFlagStartMotion, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	return iRet;
	
}

int acs_bufprog_get_start_flag_buff_9(int *piFlagStartMotion)
{
	short iRet = MTN_API_OK_ZERO;
	if(!acsc_ReadInteger(stCommHandleACS, BUFFER_ID_VEL_TEST_PROF_MOVE_2, "iAFT_StartPathMotion_buffer9", 0, 0, 0, 0, piFlagStartMotion, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	return iRet;
	
}

int acs_bufprog_write_traj_len_buff_9(int iTrajLen)
{
	short iRet = MTN_API_OK_ZERO;
	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_VEL_TEST_PROF_MOVE_2, "iAFT_TrajLen_buffer9", 0, 0, 0, 0, &iTrajLen, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	return iRet;
}

char lstrACS_CmdGenPeriod_ms[] = "rAFT_TrajCmdGenPeriod_ms";
static char gstrAcsVarName[128];
//#define PROFILE_TYPE_REL_MOVE    
// IDC_BUTTON_MOVE_RELATIVE

void mtnmove_declare_acsc_var_cmdgen_period_ms(HANDLE Handle)
{
	sprintf_s(gstrAcsVarName, 128, "%s(1)(1)", lstrACS_CmdGenPeriod_ms);

	acsc_DeclareVariable(Handle, ACSC_REAL_TYPE, gstrAcsVarName, NULL);
}
int acs_bufprog_write_traj_move_time_unit_ms(double dCmdGenPeriod_ms)
{
	short iRet = MTN_API_OK_ZERO;
//	double dCmdGenPeriod_ms[] = {(dCmdGenTs_ms/1000.0) * 1000};
	if (!acsc_WriteReal(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, lstrACS_CmdGenPeriod_ms, 0, 0, ACSC_NONE, ACSC_NONE, &dCmdGenPeriod_ms, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
		//return;
	}
	if (!acsc_WriteReal(stCommHandleACS, BUFFER_ID_VEL_TEST_PROF_MOVE_2, lstrACS_CmdGenPeriod_ms, 0, 0, ACSC_NONE, ACSC_NONE, &dCmdGenPeriod_ms, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
		//return;
	}
	return iRet;
}

int acs_bufprog_download_vel_prof_buff_8(int idxMatCol, int nTrajSize, double *pdVelProf)
{
	short iRet = MTN_API_OK_ZERO;
	if (!acsc_WriteReal(stCommHandleACS, BUFFER_ID_AFT_SPECTRUM_TEST, "rAFT_TrajectoryPoints_buffer8", idxMatCol, idxMatCol, 0, nTrajSize - 1, pdVelProf, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	return iRet;
}

int acs_bufprog_download_vel_prof_buff_9(int idxMatCol, int nTrajSize, double *pdVelProf)
{
	short iRet = MTN_API_OK_ZERO;
	if (!acsc_WriteReal(stCommHandleACS, BUFFER_ID_VEL_TEST_PROF_MOVE_2, "rAFT_TrajectoryPoints_buffer9", idxMatCol, idxMatCol, 0, nTrajSize - 1, pdVelProf, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	return iRet;
}

char astrBuffProgramACS[MAX_NUM_ACS_BUFFERS][MAX_NUM_BYTES_PER_BUFFER];
int iTotalNumLinesPerBuffer[MAX_NUM_ACS_BUFFERS];
int iTotalBytesPerBuffer[MAX_NUM_ACS_BUFFERS];

int acs_buffprog_upload_prog_get_line(unsigned int idxBuffer)
{
	short iRet = MTN_API_OK_ZERO;

	if(!acsc_UploadBuffer(stCommHandleACS, idxBuffer, 0, &astrBuffProgramACS[idxBuffer][0], 
		MAX_NUM_BYTES_PER_BUFFER, &iTotalBytesPerBuffer[idxBuffer], NULL))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
		goto label_return_acs_buffprog_upload_prog_get_line;
	}
	astrBuffProgramACS[idxBuffer][iTotalBytesPerBuffer[idxBuffer]] = '\0'; // append '\0' for later %s print
	iTotalNumLinesPerBuffer[idxBuffer] = 0;
	for(int ii = 0; ii<iTotalBytesPerBuffer[idxBuffer]; ii++)
	{
		if(astrBuffProgramACS[idxBuffer][ii] == '\n')
		{
			iTotalNumLinesPerBuffer[idxBuffer] ++;
		}
	}

label_return_acs_buffprog_upload_prog_get_line:
	return iRet;
}

int acs_buffprog_get_num_lines_at_buff(unsigned int idxBuffer)
{
	if(idxBuffer < MAX_NUM_ACS_BUFFERS)
	{
		return iTotalNumLinesPerBuffer[idxBuffer];
	}
	else
	{
		return 0;
	}
}

int acs_buffprog_get_num_bytes_at_buff(unsigned int idxBuffer)
{
	if(idxBuffer < MAX_NUM_ACS_BUFFERS)
	{
		return iTotalBytesPerBuffer[idxBuffer];
	}
	else
	{
		return 0;
	}
}

static int aiDataBuffProg_0[MAX_LEN_INT_DATA_BUFF_PROG_0];
int acs_bufprog_read_int_data_wb_buff_0()
{
	short iRet = MTN_API_OK_ZERO;
	if(!acsc_ReadInteger(stCommHandleACS, 0, "IntData", 0, WB_BUFF_PROG_0_Y_Ctrl, 0, 0, aiDataBuffProg_0, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	return iRet;
}

//int mtn_tune_emu_b1w_download_traj(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbOneWirePerformance)
//{
//	int iTrajLen = stpWbOneWirePerformance->stTimePointsOfBondHeadZ.idxEndTrajectory2ndBondSearchHeight
//		- stpWbOneWirePerformance->stTimePointsOfBondHeadZ.idxStartTrajectory;
//
//	
//	acs_bufprog_download_vel_prof_buff_8(0, iTrajLen, fRefVelX[
//}
#define WB_DCOM_FACTOR_SYS_VS_TUNING    (32767.0/32767.0)
static BOND_DCOM_TIME_SEG st1stBondContactSeg;
void acs_bufProg_get_1st_bond_dcom_time_seg()
{
	st1stBondContactSeg.iTimeSeg1 = aiDataBuffProg_0[WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_1_TIME_1ST_B] ; //   11
	st1stBondContactSeg.iDcomSeg1 = (int)(aiDataBuffProg_0[WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_1_DCOM_1ST_B] * WB_DCOM_FACTOR_SYS_VS_TUNING); //   12
	st1stBondContactSeg.iTimeSeg2 = aiDataBuffProg_0[WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_2_TIME_1ST_B] ; //   13
	st1stBondContactSeg.iDcomSeg2 = (int)(aiDataBuffProg_0[WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_2_DCOM_1ST_B] * WB_DCOM_FACTOR_SYS_VS_TUNING); //   14
	st1stBondContactSeg.iTimeSeg3 = 0; // aiDataBuffProg_0[WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_3_TIME_1ST_B] ; //   15
	st1stBondContactSeg.iDcomSeg3 = 0; // (int)(aiDataBuffProg_0[WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_3_DCOM_1ST_B] * WB_DCOM_FACTOR_SYS_VS_TUNING); //   16
}

static BOND_DCOM_TIME_SEG st2ndBondContactSeg;
void acs_bufProg_get_2nd_bond_dcom_time_seg()
{
	st2ndBondContactSeg.iTimeSeg1 = aiDataBuffProg_0[WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_1_TIME_2ND_B] ; //   29
	st2ndBondContactSeg.iDcomSeg1 = (int)(aiDataBuffProg_0[WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_1_DCOM_2ND_B] * WB_DCOM_FACTOR_SYS_VS_TUNING); //   30
	st2ndBondContactSeg.iTimeSeg2 = aiDataBuffProg_0[WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_2_TIME_2ND_B] ; //   31
	st2ndBondContactSeg.iDcomSeg2 = (int)(aiDataBuffProg_0[WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_2_DCOM_2ND_B] * WB_DCOM_FACTOR_SYS_VS_TUNING); //   32
	st2ndBondContactSeg.iTimeSeg3 = 0; // aiDataBuffProg_0[WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_3_TIME_2ND_B] ; //   33
	st2ndBondContactSeg.iDcomSeg3 = 0; // (int)(aiDataBuffProg_0[WB_BUFF_PROG_0_Z_CONTACT_FORCE_SEG_3_DCOM_2ND_B] * WB_DCOM_FACTOR_SYS_VS_TUNING); //   34
}

int acs_bufprog_get_wb_srch_delay_1st_bond()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_1stBond_SearchDelay];
}
int acs_bufprog_get_wb_srch_delay_2nd_bond()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_2ndBond_SearchDelay];
}
int acs_bufprog_get_wb_remain_time_move_z_before_xy_stop()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_Remaining_Time_MovingZ_BeforeEndingXY];
}
int acs_bufprog_get_wb_xy_speed_prof_idx()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_XY_SpeedProfileMoveTo1stBond];
}
int acs_bufprog_get_wb_x_ctrl_idx()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_X_Ctrl];
}
int acs_bufprog_get_wb_y_ctrl_idx()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_Y_Ctrl];
}
int acs_bufprog_get_wb_traj_len()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_TRAJ_LEN];
}
int acs_bufprog_get_wb_x_1st_bond_posn()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_X_1ST_BOND_POSN];
}
int acs_bufprog_get_wb_y_1st_bond_posn()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_Y_1ST_BOND_POSN];
}
int acs_bufprog_get_wb_z_1st_bond_search_height_posn()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_Z_SRCH_HEIGHT_1ST_BOND];
}
int acs_bufprog_get_wb_z_1st_bond_search_vel()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_Z_SRCH_VEL_1ST_BOND];
}
int acs_bufprog_get_wb_z_reverse_height_dist()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_Z_REVERSE_HEIGHT];
}
int acs_bufprog_get_wb_x_reverse_dist()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_X_REVERSE_DIST];
}
int acs_bufprog_get_wb_y_reverse_dist()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_Y_REVERSE_DIST];
}
int acs_bufprog_get_wb_z_reverse_dist()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_Z_REVERSE_DIST];
}
int acs_bufprog_get_wb_z_loop_top_dist()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_Z_LOOP_TOP_DIST];
}
int acs_bufprog_get_wb_z_2nd_bond_search_vel()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_Z_SRCH_VEL_2ND_BOND];
}
int acs_bufprog_get_wb_z_tail_dist()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_Z_TAIL_DIST];
}
int acs_bufprog_get_wb_z_2nd_reset_level_posn()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_Z_RESET_LEVEL_POSN];
}
int acs_bufprog_get_wb_z_1st_bond_search_threshold_pe()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_Z_SRCH_THRESHOLD_PE_1ST_B];
}
int acs_bufprog_get_wb_z_1st_bond_search_prot_fb_posn()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_Z_SRCH_PROT_FD_POSN_1ST_B];
}
int acs_bufprog_get_wb_z_1st_bond_pre_contact_force_dcom_1000()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_Z_SRCH_PRE_CONTACT_FORCE_DCOM_1ST_B];
}
int acs_bufprog_get_wb_z_2nd_bond_search_threshold_pe()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_Z_SRCH_THRESHOLD_PE_2ND_B];
}
int acs_bufprog_get_wb_z_2nd_bond_search_prot_fb_posn()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_Z_SRCH_PROT_FD_POSN_2ND_B];
}
int acs_bufprog_get_wb_z_2nd_bond_pre_contact_force_dcom_1000()
{
	return aiDataBuffProg_0[WB_BUFF_PROG_0_Z_SRCH_PRE_CONTACT_FORCE_DCOM_2ND_B];
}

static double adTrajPoints_BuffProg0[3][500];
static int iTrajLen_BuffProg0;

#include "MtnTune.h"

void mtn_tune_read_traj_points_from_waveform(WB_ONE_WIRE_PERFORMANCE_CALC *stpAnalyzeOneWire, int *pnTrajLen)
{
	mtn_tune_get_traj_points(stpAnalyzeOneWire, 
							  &adTrajPoints_BuffProg0[2][0], &adTrajPoints_BuffProg0[0][0], &adTrajPoints_BuffProg0[1][0], 
							  pnTrajLen);
}

int acs_bufprog_read_traj_points_wb_buff_0(int *piTrajLen)
{
	short iRet = MTN_API_OK_ZERO;
	if(!acsc_ReadReal(stCommHandleACS, 0, "TrajPoints", 0, 2, 0, 499, &adTrajPoints_BuffProg0[0][0], NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}
	else
	{
		*piTrajLen = acs_bufprog_get_wb_traj_len();
	}

	return iRet;
}

// Calculate some values from waveform
void mtn_tune_calc_b1w_setting_from_waveform(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbOneWireInfo)
{
	aiDataBuffProg_0[WB_BUFF_PROG_0_Z_SRCH_VEL_1ST_BOND] = (int)(stpWbOneWireInfo->stBondHeadSpeedSetting.dSrchVel1stBond * 1000);
	aiDataBuffProg_0[WB_BUFF_PROG_0_Z_SRCH_VEL_2ND_BOND] = (int)(stpWbOneWireInfo->stBondHeadSpeedSetting.dSrchVel2ndBond * 1000);
// XY Table position
	aiDataBuffProg_0[WB_BUFF_PROG_0_X_1ST_BOND_POSN] = (int)(stpWbOneWireInfo->stPosnsOfTable.f1stBondPosition_X);  // acs_bufprog_get_wb_x_1st_bond_posn();
	aiDataBuffProg_0[WB_BUFF_PROG_0_Y_1ST_BOND_POSN] = (int)(stpWbOneWireInfo->stPosnsOfTable.f1stBondPosition_Y); // acs_bufprog_get_wb_y_1st_bond_posn(); 

//	aiDataBuffProg_0[] = (int)(stpWbOneWireInfo->);
//	aiDataBuffProg_0[] = (int)(stpWbOneWireInfo->);
// 
	aiDataBuffProg_0[WB_BUFF_PROG_0_X_REVERSE_DIST]	= (int)(stpWbOneWireInfo->stPosnsOfTable.f1stBondReversePosition_X - stpWbOneWireInfo->stPosnsOfTable.f1stBondPosition_X); //  acs_bufprog_get_wb_x_reverse_dist();
	aiDataBuffProg_0[WB_BUFF_PROG_0_Y_REVERSE_DIST]	= (int)(stpWbOneWireInfo->stPosnsOfTable.f1stBondReversePosition_Y - stpWbOneWireInfo->stPosnsOfTable.f1stBondPosition_Y); //  acs_bufprog_get_wb_y_reverse_dist();

// Position: BondHead Z 
	aiDataBuffProg_0[WB_BUFF_PROG_0_Z_SRCH_HEIGHT_1ST_BOND] = (int)stpWbOneWireInfo->stPosnsOfBondHeadZ.f1stBondSearchHeight_Z; // acs_bufprog_get_wb_z_1st_bond_search_height_posn();
	aiDataBuffProg_0[WB_BUFF_PROG_0_Z_RESET_LEVEL_POSN]  = (int)(stpWbOneWireInfo->stPosnsOfBondHeadZ.fEndFireLevel_Z); // acs_bufprog_get_wb_z_2nd_reset_level_posn();

// Relative distance 
	aiDataBuffProg_0[WB_BUFF_PROG_0_Z_REVERSE_HEIGHT] = (int)(stpWbOneWireInfo->stPosnsOfBondHeadZ.fReverseHeightPosn_Z - stpWbOneWireInfo->stPosnsOfBondHeadZ.f1stBondContactPosn_Z); // acs_bufprog_get_wb_z_reverse_height_dist();
	aiDataBuffProg_0[WB_BUFF_PROG_0_Z_REVERSE_DIST] = (int)(stpWbOneWireInfo->stPosnsOfBondHeadZ.fKinkHeightPosn_Z - stpWbOneWireInfo->stPosnsOfBondHeadZ.fReverseHeightPosn_Z); // acs_bufprog_get_wb_z_reverse_dist();
	aiDataBuffProg_0[WB_BUFF_PROG_0_Z_LOOP_TOP_DIST] = (int)(stpWbOneWireInfo->stPosnsOfBondHeadZ.fLoopTopPosn_Z - stpWbOneWireInfo->stPosnsOfBondHeadZ.fKinkHeightPosn_Z); //  acs_bufprog_get_wb_z_loop_top_dist();
	aiDataBuffProg_0[WB_BUFF_PROG_0_Z_TAIL_DIST] = (int)(stpWbOneWireInfo->stPosnsOfBondHeadZ.fTailPosn_Z - stpWbOneWireInfo->stPosnsOfBondHeadZ.f2ndBondContactPosn_Z); // acs_bufprog_get_wb_z_tail_dist();

}

#define   MAX_LEN_B1W_MTN_PROG_BUFF_DATA    100
static int aiBondOneWireMtnProgData[MAX_LEN_B1W_MTN_PROG_BUFF_DATA];

void mtn_tune_calc_bond_prog_by_one_wire_info(WB_ONE_WIRE_PERFORMANCE_CALC *stpWbOneWireInfo)
{
	if(acs_bufprog_read_int_data_wb_buff_0() == MTN_API_ERROR_ACS_BUFF_PROG)
	{
		mtn_tune_calc_b1w_setting_from_waveform(stpWbOneWireInfo);
	}

	if(	acs_bufprog_read_traj_points_wb_buff_0(&iTrajLen_BuffProg0) == MTN_API_ERROR_ACS_BUFF_PROG)
	{
		mtn_tune_read_traj_points_from_waveform(stpWbOneWireInfo, &iTrajLen_BuffProg0);
	}

	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_XY_SPEED_PROFILE_MOVE_TO_1ST_B]= acs_bufprog_get_wb_xy_speed_prof_idx();
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_X_CTRL_MOVE_TO_1ST_B]	=	acs_bufprog_get_wb_x_ctrl_idx();
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Y_CTRL_MOVE_TO_1ST_B]	=	acs_bufprog_get_wb_y_ctrl_idx();
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_XY_REMAIN_TIME_START_Z_1ST_B]	=	acs_bufprog_get_wb_remain_time_move_z_before_xy_stop();

	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_X_POSN_1ST_B]	=	(int)stpWbOneWireInfo->stPosnsOfTable.f1stBondPosition_X; // acs_bufprog_get_wb_x_1st_bond_posn();
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Y_POSN_1ST_B]	=	(int)stpWbOneWireInfo->stPosnsOfTable.f1stBondPosition_Y; // acs_bufprog_get_wb_y_1st_bond_posn(); 
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_SRCH_HEIGHT_1ST_B]	=	(int)stpWbOneWireInfo->stPosnsOfBondHeadZ.f1stBondSearchHeight_Z; // acs_bufprog_get_wb_z_1st_bond_search_height_posn();
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_SRCH_VEL_1ST_B]	=	acs_bufprog_get_wb_z_1st_bond_search_vel(); // (int)stpWbOneWireInfo->stBondHeadSpeedSetting.dSrchVel1stBond;
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_SRCH_THRESHOLD_PE_1ST_B]	=	acs_bufprog_get_wb_z_1st_bond_search_threshold_pe();
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_SRCH_PROT_FD_POSN_1ST_B]	=	acs_bufprog_get_wb_z_1st_bond_search_prot_fb_posn();
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_SRCH_PRE_CONTACT_FORCE_DCOM_1ST_B]	=	abs((int)(acs_bufprog_get_wb_z_1st_bond_pre_contact_force_dcom_1000() * WB_DCOM_FACTOR_SYS_VS_TUNING));

	acs_bufProg_get_1st_bond_dcom_time_seg();
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_1_TIME_1ST_B]	= st1stBondContactSeg.iTimeSeg1;
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_1_DCOM_1ST_B]	= -abs(st1stBondContactSeg.iDcomSeg1);
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_2_TIME_1ST_B]	= st1stBondContactSeg.iTimeSeg2;
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_2_DCOM_1ST_B]	= -abs(st1stBondContactSeg.iDcomSeg2);
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_3_TIME_1ST_B]	= st1stBondContactSeg.iTimeSeg3;
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_3_DCOM_1ST_B]	= -abs(st1stBondContactSeg.iDcomSeg3);
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_REVERSE_HEIGHT]	=	(int)(stpWbOneWireInfo->stPosnsOfBondHeadZ.fReverseHeightPosn_Z - stpWbOneWireInfo->stPosnsOfBondHeadZ.f1stBondContactPosn_Z); // acs_bufprog_get_wb_z_reverse_height_dist();
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_X_REVERSE_DIST]	=	(int)(stpWbOneWireInfo->stPosnsOfTable.f1stBondReversePosition_X - stpWbOneWireInfo->stPosnsOfTable.f1stBondPosition_X); //  acs_bufprog_get_wb_x_reverse_dist();
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Y_REVERSE_DIST]	=	(int)(stpWbOneWireInfo->stPosnsOfTable.f1stBondReversePosition_Y - stpWbOneWireInfo->stPosnsOfTable.f1stBondPosition_Y); //  acs_bufprog_get_wb_y_reverse_dist();
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_REVERSE_DIST]	=	(int)(stpWbOneWireInfo->stPosnsOfBondHeadZ.fKinkHeightPosn_Z - stpWbOneWireInfo->stPosnsOfBondHeadZ.fReverseHeightPosn_Z); // acs_bufprog_get_wb_z_reverse_dist();
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_LOOP_TOP_DIST]	=	(int)(stpWbOneWireInfo->stPosnsOfBondHeadZ.fLoopTopPosn_Z - stpWbOneWireInfo->stPosnsOfBondHeadZ.fKinkHeightPosn_Z); //  acs_bufprog_get_wb_z_loop_top_dist();
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_XYZ_TRAJ_LENGTH]	=	iTrajLen_BuffProg0; // stpWbOneWireInfo->stTimePointsOfBondHeadZ.idxEndTrajectory2ndBondSearchHeight - stpWbOneWireInfo->stTimePointsOfBondHeadZ.idxStartTrajectory;
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_SRCH_VEL_2ND_B]	= acs_bufprog_get_wb_z_2nd_bond_search_vel(); // (int)stpWbOneWireInfo->stBondHeadSpeedSetting.dSrchVel2ndBond;
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_DRY_RUN_FLAG]	=	stpWbOneWireInfo->iFlagIsDryRun;
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_SRCH_THRESHOLD_PE_2ND_B]	=	acs_bufprog_get_wb_z_2nd_bond_search_threshold_pe();
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_SRCH_PROT_FD_POSN_2ND_B]	=	acs_bufprog_get_wb_z_2nd_bond_search_prot_fb_posn();
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_SRCH_PRE_CONTACT_FORCE_DCOM_2ND_B]	=	abs((int)(acs_bufprog_get_wb_z_2nd_bond_pre_contact_force_dcom_1000() * WB_DCOM_FACTOR_SYS_VS_TUNING));

	acs_bufProg_get_2nd_bond_dcom_time_seg();
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_1_TIME_2ND_B]	= st2ndBondContactSeg.iTimeSeg1;
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_1_DCOM_2ND_B]	= -abs(st2ndBondContactSeg.iDcomSeg1);
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_2_TIME_2ND_B]	= st2ndBondContactSeg.iTimeSeg2;
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_2_DCOM_2ND_B]	= -abs(st2ndBondContactSeg.iDcomSeg2);
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_3_TIME_2ND_B]	= st2ndBondContactSeg.iTimeSeg3;
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_3_DCOM_2ND_B]	= -abs(st2ndBondContactSeg.iDcomSeg3);
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_TAIL_DIST]	=	(int)(stpWbOneWireInfo->stPosnsOfBondHeadZ.fTailPosn_Z - stpWbOneWireInfo->stPosnsOfBondHeadZ.f2ndBondContactPosn_Z); // acs_bufprog_get_wb_z_tail_dist();
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_RESET_LEVEL]	=	(int)(stpWbOneWireInfo->stPosnsOfBondHeadZ.fEndFireLevel_Z); // acs_bufprog_get_wb_z_2nd_reset_level_posn();

	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_SRCH_DELAY_1ST_B] = acs_bufprog_get_wb_srch_delay_1st_bond();
	aiBondOneWireMtnProgData[BUFFER_DATA_WB_OFST_Z_SRCH_DELAY_2ND_B] = acs_bufprog_get_wb_srch_delay_2nd_bond();
}

void mtn_tune_set_buff_data_wb_tune_st(BUFFER_DATA_WB_TUNE *stpBuffDataWbTune)
{
	stpBuffDataWbTune->	iBufferDataWbTune_XY_SPEED_PROFILE_MOVE_TO_1ST_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_XY_SPEED_PROFILE_MOVE_TO_1ST_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_X_CTRL_MOVE_TO_1ST_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_X_CTRL_MOVE_TO_1ST_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Y_CTRL_MOVE_TO_1ST_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Y_CTRL_MOVE_TO_1ST_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_X_POSN_1ST_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_X_POSN_1ST_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Y_POSN_1ST_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Y_POSN_1ST_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_XY_REMAIN_TIME_START_Z_1ST_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_XY_REMAIN_TIME_START_Z_1ST_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_SRCH_HEIGHT_1ST_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_SRCH_HEIGHT_1ST_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_SRCH_VEL_1ST_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_SRCH_VEL_1ST_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_SRCH_THRESHOLD_PE_1ST_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_SRCH_THRESHOLD_PE_1ST_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_SRCH_PROT_FD_POSN_1ST_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_SRCH_PROT_FD_POSN_1ST_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_SRCH_PRE_CONTACT_FORCE_DCOM_1ST_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_SRCH_PRE_CONTACT_FORCE_DCOM_1ST_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_1_TIME_1ST_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_1_TIME_1ST_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_1_DCOM_1ST_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_1_DCOM_1ST_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_2_TIME_1ST_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_2_TIME_1ST_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_2_DCOM_1ST_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_2_DCOM_1ST_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_3_TIME_1ST_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_3_TIME_1ST_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_3_DCOM_1ST_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_3_DCOM_1ST_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_REVERSE_HEIGHT	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_REVERSE_HEIGHT	];
	stpBuffDataWbTune->	iBufferDataWbTune_X_REVERSE_DIST	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_X_REVERSE_DIST	];
	stpBuffDataWbTune->	iBufferDataWbTune_Y_REVERSE_DIST	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Y_REVERSE_DIST	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_REVERSE_DIST	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_REVERSE_DIST	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_LOOP_TOP_DIST	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_LOOP_TOP_DIST	];
	stpBuffDataWbTune->	iBufferDataWbTune_XYZ_TRAJ_LENGTH	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_XYZ_TRAJ_LENGTH	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_SRCH_VEL_2ND_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_SRCH_VEL_2ND_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_SRCH_DELAY_2ND_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_SRCH_DELAY_2ND_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_DRY_RUN_FLAG	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_DRY_RUN_FLAG	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_SRCH_THRESHOLD_PE_2ND_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_SRCH_THRESHOLD_PE_2ND_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_SRCH_PROT_FD_POSN_2ND_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_SRCH_PROT_FD_POSN_2ND_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_SRCH_PRE_CONTACT_FORCE_DCOM_2ND_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_SRCH_PRE_CONTACT_FORCE_DCOM_2ND_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_1_TIME_2ND_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_1_TIME_2ND_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_1_DCOM_2ND_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_1_DCOM_2ND_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_2_TIME_2ND_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_2_TIME_2ND_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_2_DCOM_2ND_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_2_DCOM_2ND_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_3_TIME_2ND_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_3_TIME_2ND_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_CONTACT_FORCE_SEG_3_DCOM_2ND_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_CONTACT_FORCE_SEG_3_DCOM_2ND_B	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_TAIL_DIST	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_TAIL_DIST	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_RESET_LEVEL	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_RESET_LEVEL	];
	stpBuffDataWbTune->	iBufferDataWbTune_Z_SRCH_DELAY_1ST_B	=	aiBondOneWireMtnProgData[	BUFFER_DATA_WB_OFST_Z_SRCH_DELAY_1ST_B	];
}

int acs_bufprog_write_data_emu_b1w_buff_8()
{
	short iRet = MTN_API_OK_ZERO;
	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_POSITIVE_MOVE, "aiBufferDataWb", 0, MAX_LEN_B1W_MTN_PROG_BUFF_DATA - 1, 0, 0, aiBondOneWireMtnProgData, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}
	else
	{
		iRet = acs_bufprog_download_vel_prof_buff_8(0, iTrajLen_BuffProg0, adTrajPoints_BuffProg0[0]);
		iRet |= acs_bufprog_download_vel_prof_buff_8(1, iTrajLen_BuffProg0, adTrajPoints_BuffProg0[1]);
		iRet |= acs_bufprog_download_vel_prof_buff_8(2, iTrajLen_BuffProg0, adTrajPoints_BuffProg0[2]);
		iRet |= acs_bufprog_write_motion_flag_buff_8(ACS_BUFPROG_MTN_FLAG_EMU_B1W);
	}

	return iRet;
}



static int nNumLinesInBuffer_OpenCtrlACS_Para; // ACS NewDSP Default Parameter at Power Up
// Controller Buffer Program#6
char *strBufferProgram_OpenCtrlACS_Para_13V[] = 
{
	"int iJerkFF_Ga_AxisA, iJerkFF_Gf_AxisA;\n",  // 0
	"int iJerkFF_Ga_AxisX, iJerkFF_Gf_AxisX;\n",  // 1
	"int iJerkFF_Ga_AxisY, iJerkFF_Gf_AxisY;\n",  // 2
	"int iFlagSpringCompOn, iSpringCompGa, iSpringCompGf, iSpringOffset; int iKxaGa, iKxaGf, iKaxGa, iKaxGf; \n", // 3
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
	"iJerkFF_Gf_AxisA = 12;\n",                      // 16,  LINE_ID_SP_DEF_PARA_DECLARE_
	"iJerkFF_Ga_AxisA = 8.192E+006;\n",              // 17
	"iJerkFF_Gf_AxisX = 11;\n",                     // 18
	"iJerkFF_Ga_AxisX =8.192E+006;\n",              // 19
	"iJerkFF_Gf_AxisY = 11;\n",			            // 20
	"iJerkFF_Ga_AxisY = 8.192E+006;\n",             // 21
	"iSpringCompGa = 0.000000000000000;\n",			// 22
	"iSpringCompGf = 0.000000000000000; \n",		// 14, 23
	"iSpringOffset = 0.000000000000000;\n",			// 24
	"iKxaGa = -6.5536E+006;\n",				// 25
	"iKxaGf = 17; \n",				        // 26
	"iKaxGa= -6.5536E+006; \n",				// 27
	"iKaxGf= 15;\n",					    // 28
	"\n",  // 29
	// ACC ->: /2E6, Jerk -> /2E9
	//  Acc:  ,             Jerk                   Jerk for Production 
	//  8E7 -> 40 m/s^2      22E9 -> 11K m/s^3     9E9
	//  5E7 -> 25           10E9 -> 5K m/s^3       4E9
	//  3E7 -> 15           3E9 -> 1.5K m/s^3      1E9
	//  
	"arMotorXProfile(0)(0) = 1.9E6; arMotorXProfile(0)(1) = 8E7; arMotorXProfile(0)(2) = 22E9;\n",  // 30, 13V config different from 13T
	"arMotorXProfile(1)(0) = 1.9E6; arMotorXProfile(1)(1) = 5E7; arMotorXProfile(1)(2) = 10E9;\n",   //31 
	"arMotorXProfile(2)(0) = 1.9E6; arMotorXProfile(2)(1) = 3E7; arMotorXProfile(2)(2) = 3E9;\n",	// 32
	"arMotorXProfile(3)(0) = 1.9E6; arMotorXProfile(3)(1) = 2E7; arMotorXProfile(3)(2) = 2E9;\n",	// 33
	"\n",	// 34
	// ACC ->: / 2E6, Jerk -> / 2E9                
	// Acc                   Jerk                  Jerk for Production 
	// 5E7 -> 25  ,          8E9 -> 4000           4.4E9 -> 2200
	// 3E7 -> 15  ,          6E9 -> 3000           4E9  -> 2000            
	// 3E7 -> 15  ,          4E9 -> 2000           3E9  -> 1500 ,          
	// 2E7 -> 10  ,          2E9 -> 1000           1E9  -> 500 
	"arMotorYProfile(0)(0) = 1.9E6; arMotorYProfile(0)(1) = 5E7; arMotorYProfile(0)(2) = 8E9;\n", // 35
	"arMotorYProfile(1)(0) = 1.9E6; arMotorYProfile(1)(1) = 3E7; arMotorYProfile(1)(2) = 6E9;\n",	// 36
	"arMotorYProfile(2)(0) = 1.9E6; arMotorYProfile(2)(1) = 3E7; arMotorYProfile(2)(2) = 4E9;\n",	// 37
	"arMotorYProfile(3)(0) = 1.9E6; arMotorYProfile(3)(1) = 2E7; arMotorYProfile(3)(2) = 2E9;\n",	// 38
	"\n",	// 39
	"arMotorAProfile(0)(0) = 1.95E6; arMotorAProfile(0)(1) = 3.5E8; arMotorAProfile(0)(2) = 8E10;\n",	// 40
	"arMotorAProfile(1)(0) = 1.95E6; arMotorAProfile(1)(1) = 4.5E8; arMotorAProfile(1)(2) = 3E10;\n",	// 41
	"arMotorAProfile(2)(0) = 1.95E6; arMotorAProfile(2)(1) = 4.5E8; arMotorAProfile(2)(2) = 3E10;\n",	// 42
	"arMotorAProfile(3)(0) = 1.95E6; arMotorAProfile(3)(1) = 4.5E8; arMotorAProfile(3)(2) = 3E10;\n",	// 43
	"arMotorAProfile(4)(0) = 1.95E6; arMotorAProfile(4)(1) = 4.5E8; arMotorAProfile(4)(2) = 3E10;\n",	// 44
	"\n",	// 45
	"nLenRMS = 6000;\n",// 46
	"idxCurrentDcom = 0; idxOldestDcom = nLenRMS - 1; idx2ndOldestDcom = nLenRMS - 2;\n", // 47
	"rMeanSOS_Axis4_curr = 0;rMeanSOS_Axis4_prev = 0; rSOS_Axis4 = 0; \n",   // 48
	"rMeanSOS_Axis0_curr = 0;rMeanSOS_Axis0_prev = 0; rSOS_Axis0 = 0; \n",   // 49
	"rMeanSOS_Axis1_curr = 0;rMeanSOS_Axis1_prev = 0; rSOS_Axis1 = 0; \n",   // 50
	"ii = 0; \n", // 51
	"loop nLenRMS;\n", // 52
	"rDelayLineRMS_Axis4(ii) = abs(DOUT(4) * DOUT(4)); rSOS_Axis4 =  rSOS_Axis4+ rDelayLineRMS_Axis4(ii); \n",  // 53
	"rDelayLineRMS_Axis0(ii) = abs(DOUT(0) * DOUT(0)); rSOS_Axis0 =  rSOS_Axis0+ rDelayLineRMS_Axis0(ii); \n",  // 54
	"rDelayLineRMS_Axis1(ii) = abs(DOUT(1) * DOUT(1)); rSOS_Axis1 =  rSOS_Axis1+ rDelayLineRMS_Axis1(ii); ii = ii +1;        end \n", // 55
	"rMeanSOS_Axis4_prev = rSOS_Axis4/nLenRMS; \n", //56
	"rMeanSOS_Axis0_prev = rSOS_Axis0/nLenRMS; \n", //57
	"rMeanSOS_Axis1_prev = rSOS_Axis1/nLenRMS; \n", //58
	"rRMS_Axis4 = sqrt(rSOS_Axis4/nLenRMS)/327.67; rRMS_Axis0 = sqrt(rSOS_Axis0/nLenRMS)/327.67; rRMS_Axis1 = sqrt(rSOS_Axis1/nLenRMS)/327.67; \n",  // 59
	"while(1);\n", // 60 // 20120902
	"TILL (MST4.#ENABLED = 1 | MST0.#ENABLED = 1 | MST1.#ENABLED = 1) & iFlagEnableCalcRMS = 1;\n",  //61 20120906
	"	BLOCK;\n", //62	
	"	!0;\n", //63
	"		rSqDout_Axis0_curr =  abs(DOUT(0));  rSqDout_Axis0_curr = rSqDout_Axis0_curr * rSqDout_Axis0_curr;\n", //64
	"		rMeanSOS_Axis0_curr = rMeanSOS_Axis0_prev + ((rSqDout_Axis0_curr - rDelayLineRMS_Axis0(idxOldestDcom)))/nLenRMS;\n", //65
	"		if(rMeanSOS_Axis0_curr < 0) rMeanSOS_Axis0_curr = 0; end;\n", //66
	"		rMeanSOS_Axis0_prev = rMeanSOS_Axis0_curr;\n", //67
	"	!1  !/3 +   rDelayLineRMS_Axis1(idxCurrentDcom) -  rDelayLineRMS_Axis1(idx2ndOldestDcom);\n", //68
	"		rSqDout_Axis1_curr =  abs(DOUT(1));  rSqDout_Axis1_curr = rSqDout_Axis1_curr * rSqDout_Axis1_curr;\n", // 69
	"		rMeanSOS_Axis1_curr = rMeanSOS_Axis1_prev + ((rSqDout_Axis1_curr - rDelayLineRMS_Axis1(idxOldestDcom)))/nLenRMS;\n", //70
	"		if(rMeanSOS_Axis1_curr < 0) rMeanSOS_Axis1_curr = 0; end;\n", // 71
	"		rMeanSOS_Axis1_prev = rMeanSOS_Axis1_curr;\n", //72
	"	!4 !/3 +   rDelayLineRMS_Axis4(idxCurrentDcom) -  rDelayLineRMS_Axis4(idx2ndOldestDcom);\n", //73
	"		rSqDout_Axis4_curr =  abs(DOUT(4));  rSqDout_Axis4_curr = rSqDout_Axis4_curr * rSqDout_Axis4_curr; \n", //74
	"		rMeanSOS_Axis4_curr = rMeanSOS_Axis4_prev + ((rSqDout_Axis4_curr - rDelayLineRMS_Axis4(idxOldestDcom)))/nLenRMS; \n", //75
	"		if(rMeanSOS_Axis4_curr < 0) rMeanSOS_Axis4_curr = 0; end \n", //76
	"		rMeanSOS_Axis4_prev = rMeanSOS_Axis4_curr; \n", //77
	"	! Update index \n", //78
	"		idxCurrentDcom = idxCurrentDcom -1; if idxCurrentDcom = -1 idxCurrentDcom = 5999 end; \n", //79
	"		idxOldestDcom = idx2ndOldestDcom;  \n", //80
	"		idx2ndOldestDcom = idx2ndOldestDcom - 1; if idx2ndOldestDcom = -1 idx2ndOldestDcom = 5999 end; \n", //81
	"	!0 \n", //82
	"		if MST0.#ENABLED = 1 \n",//83
	"		rDelayLineRMS_Axis0(idxCurrentDcom) = rSqDout_Axis0_curr; \n", //84
	"		rRMS_Axis0 = sqrt(rMeanSOS_Axis0_curr)/327.67; \n", //85
	"		end \n", //86
	"	!1 \n",//87
	"		if MST1.#ENABLED = 1 \n", //88
	"		rDelayLineRMS_Axis1(idxCurrentDcom) = rSqDout_Axis1_curr; \n", //89
	"		rRMS_Axis1 = sqrt(rMeanSOS_Axis1_curr)/327.67; \n", //90
	"		end \n", //91
	"	!4 \n", //92
	"		if MST4.#ENABLED = 1 \n", //93
	"		rDelayLineRMS_Axis4(idxCurrentDcom) = rSqDout_Axis4_curr; \n", //94
	"		rRMS_Axis4 = sqrt(rMeanSOS_Axis4_curr)/327.67; \n", //95
	"		end \n", //96
	" \n", //97
	"		arRMS(0) = rRMS_Axis0; arRMS(1) = rRMS_Axis1; arRMS(2) = rRMS_Axis4; \n", //98
	"	END \n", //99
	"     aiFlagIO(idxChekIO) = IN0.3; idxChekIO = idxChekIO + 1; if idxChekIO = 5 idxChekIO = 0 end; \n", //100
	"     BLOCK\n", //101
	"         !if iFlagEnableEmergencyCheck = 1 \n", //102
	"            iFilteredIO = aiFlagIO(0) & aiFlagIO(1); iFilteredIO = iFilteredIO & aiFlagIO(2); iFilteredIO = iFilteredIO & aiFlagIO(3); iFilteredIO = iFilteredIO & aiFlagIO(4);\n", //103
	"         !   iFilteredIO = iFilteredIO & aiFlagIO(5);iFilteredIO = iFilteredIO & aiFlagIO(6);iFilteredIO = iFilteredIO & aiFlagIO(7);iFilteredIO = iFilteredIO & aiFlagIO(8); iFilteredIO = iFilteredIO & aiFlagIO(9);\n", //104
	"            iRelaxPosnBH = EOFFS(4);\n", //105
	"         !end\n", //106
	"     END\n", //107
	"end \n", //108
	"STOP\n",	// 109
	"ON MST4.#ENABLED; SetSPV(SP0:A_JerkFFGf, iJerkFF_Gf_AxisA); SetSPV(SP0:A_JerkFFGa, iJerkFF_Ga_AxisA); SetSPV(SP0:KaxGa, iKaxGa); SetSPV(SP0:KaxGf, iKaxGf); RET\n",	// 110
	"ON MST0.#ENABLED; SetSPV(SP0:X_JerkFFGf, iJerkFF_Gf_AxisX); SetSPV(SP0:X_JerkFFGa, iJerkFF_Ga_AxisX); SetSPV(SP0:KxaGa, iKxaGa); SetSPV(SP0:KxaGf, iKxaGf); RET\n",	// 111
	"ON MST1.#ENABLED; SetSPV(SP1:X_JerkFFGf, iJerkFF_Gf_AxisY); SetSPV(SP1:X_JerkFFGa, iJerkFF_Ga_AxisY); RET\n",	// 112
	"!ON iFlagEnableEmergencyCheck = 0 & MST4.#ENABLED;  wait 10; SLPKP(4) = rBakPKPA ; RET;\n",	// 113
	"ON iFlagEnableEmergencyCheck = 1 & iFilteredIO = 1 & MST4.#ENABLED;  rBakPKPA = SLPKP(4); KILL X; KILL Y; KILL A; STOP 0; STOP 1; wait 1; enable A; ptp/e A, iRelaxPosnBH; SLPKP(4) = 0; iFlagEnableEmergencyCheck = 0; iCountFlagIO = iCountFlagIO+1; wait 100; disable A; SLPKP(4) = rBakPKPA;  RET\n",	// 114
	"ON (PST0.#RUN | PST1.#RUN) & (MST0.#ENABLED = 0 | MST1.#ENABLED = 0);   rBakPKPA = SLPKP(4); KILL A; STOP 0; STOP 1; wait(1); enable A; ptp/e A, iRelaxPosnBH; wait 10; KILL A; iCountCutOffXY = iCountCutOffXY +1; SLPKP(4) = 0; wait 100; disable A; SLPKP(4) = rBakPKPA;  RET\n",	// 115
	"!ON (PST0.#RUN | PST1.#RUN) & (MST4.#ENABLED = 0 );   rBakPKPA = SLPKP(4); STOP 0; STOP 1; wait(1); enable A; ptp/e A, iRelaxPosnBH; wait 10; KILL A; iCountCutOffBH = iCountCutOffBH +1; SLPKP(4) = 0; wait 100; disable A; SLPKP(4) = rBakPKPA;  RET\n",	// 116
	"!EndBufferProg"		// 117, LINE_END_SCRIPT_ACS_BUFFER_7_
};

// Controller Buffer Program#6
char *strBufferProgram_OpenCtrlACS_Para_WB13T[] = 
{
	"int iJerkFF_Ga_AxisA, iJerkFF_Gf_AxisA;\n",  // 0
	"int iJerkFF_Ga_AxisX, iJerkFF_Gf_AxisX;\n",  // 1
	"int iJerkFF_Ga_AxisY, iJerkFF_Gf_AxisY;\n",  // 2
	"int iFlagSpringCompOn, iSpringCompGa, iSpringCompGf, iSpringOffset; int iKxaGa, iKxaGf, iKaxGa, iKaxGf; \n", // 3
	"global REAL arMotorXProfile(4)(3); global REAL arMotorYProfile(4)(3); global REAL arMotorAProfile(5)(3); global REAL arBHCtrl(20)(4); global REAL arXCtrl(10)(4); global REAL arYCtrl(4)(4); \n",     // 4 global REAL rWaveForm(9)(2000);
	"int nLenRMS, iFlagEnableCalcRMS;\n", // 5, 20120116, 20120902
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
	"iJerkFF_Gf_AxisA = 12;\n",                      // 16,  LINE_ID_SP_DEF_PARA_DECLARE_
	"iJerkFF_Ga_AxisA = 8.192E+006;\n",              // 17
	"iJerkFF_Gf_AxisX = 11;\n",                     // 18
	"iJerkFF_Ga_AxisX =8.192E+006;\n",              // 19
	"iJerkFF_Gf_AxisY = 11;\n",			            // 20
	"iJerkFF_Ga_AxisY = 8.192E+006;\n",             // 21
	"iSpringCompGa = 0.000000000000000;\n",			// 22
	"iSpringCompGf = 0.000000000000000; \n",		// 14, 23
	"iSpringOffset = 0.000000000000000;\n",			// 24
	"iKxaGa = -6.5536E+006;\n",				// 25
	"iKxaGf = 17; \n",				        // 26
	"iKaxGa= -6.5536E+006; \n",				// 27
	"iKaxGf= 15;\n",					    // 28
	"\n",  // 29
	// ACC ->: /2E6, Jerk -> /2E9
	//  Acc:  ,             Jerk
	// 9E7 -> 45           24E9 -> 12000
	// 6E7 -> 30           9E9 -> 4000
	// 4.6E7  -> 23        5E9  -> 2500
	// 3.0E7  -> 15        4E9  -> 2000
	"arMotorXProfile(0)(0) = 1.9E6; arMotorXProfile(0)(1) = 9E7; arMotorXProfile(0)(2) = 24E9;\n",  // 30, LINE_ID_13V_13T_SPEED_PROFILE_DEF
	"arMotorXProfile(1)(0) = 1.9E6; arMotorXProfile(1)(1) = 6E7; arMotorXProfile(1)(2) = 9E9;\n",   // 31
	"arMotorXProfile(2)(0) = 1.9E6; arMotorXProfile(2)(1) = 4.6E7; arMotorXProfile(2)(2) = 5E9;\n",	// 32
	"arMotorXProfile(3)(0) = 1.9E6; arMotorXProfile(3)(1) = 3.0E7; arMotorXProfile(3)(2) = 4E9;\n",	// 33
	"\n",	// 34
	// ACC ->: /2E6, Jerk -> /2E9
	//  Acc:  ,             Jerk
	// 11E7 -> 55           24E9 -> 12000
	// 11E7 -> 55           24E9 -> 12000
	// 5E7  -> 25           10E9  -> 5000
	// 3.4E7  -> 17         6E9  -> 3000
	"arMotorYProfile(0)(0) = 1.9E6; arMotorYProfile(0)(1) = 11E7; arMotorYProfile(0)(2) = 24E9;\n", // 35
	"arMotorYProfile(1)(0) = 1.9E6; arMotorYProfile(1)(1) = 11E7; arMotorYProfile(1)(2) = 24E9;\n",	// 36
	"arMotorYProfile(2)(0) = 1.9E6; arMotorYProfile(2)(1) = 5E7; arMotorYProfile(2)(2) =  10E9;\n",	// 37
	"arMotorYProfile(3)(0) = 1.9E6; arMotorYProfile(3)(1) = 3.4E7; arMotorYProfile(3)(2) = 6E9;\n",	// 38
	"\n",	// 39
	"arMotorAProfile(0)(0) = 1.95E6; arMotorAProfile(0)(1) = 3.5E8; arMotorAProfile(0)(2) = 8E10;\n",	// 40
	"arMotorAProfile(1)(0) = 1.95E6; arMotorAProfile(1)(1) = 4.5E8; arMotorAProfile(1)(2) = 3E10;\n",	// 41
	"arMotorAProfile(2)(0) = 1.95E6; arMotorAProfile(2)(1) = 4.5E8; arMotorAProfile(2)(2) = 3E10;\n",	// 42
	"arMotorAProfile(3)(0) = 1.95E6; arMotorAProfile(3)(1) = 4.5E8; arMotorAProfile(3)(2) = 3E10;\n",	// 43
	"arMotorAProfile(4)(0) = 1.95E6; arMotorAProfile(4)(1) = 4.5E8; arMotorAProfile(4)(2) = 3E10;\n",	// 44, LINE_END_SPEED_CFG_DIFF_WB13V_13T_
	"\n",	// 45
};
////////////////////// Speed Profile for 20T
char *strBufferProgram_OpenCtrlACS_Para_WB20T[] = 
{
	"int iJerkFF_Ga_AxisA, iJerkFF_Gf_AxisA;\n",  // 0
	"int iJerkFF_Ga_AxisX, iJerkFF_Gf_AxisX;\n",  // 1
	"int iJerkFF_Ga_AxisY, iJerkFF_Gf_AxisY;\n",  // 2
	"int iFlagSpringCompOn, iSpringCompGa, iSpringCompGf, iSpringOffset; int iKxaGa, iKxaGf, iKaxGa, iKaxGf; \n", // 3
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
	"iJerkFF_Gf_AxisA = 12;\n",                      // 16,  LINE_ID_SP_DEF_PARA_DECLARE_
	"iJerkFF_Ga_AxisA = 8.192E+006;\n",              // 17
	"iJerkFF_Gf_AxisX = 11;\n",                     // 18
	"iJerkFF_Ga_AxisX =8.192E+006;\n",              // 19
	"iJerkFF_Gf_AxisY = 11;\n",			            // 20
	"iJerkFF_Ga_AxisY = 8.192E+006;\n",             // 21
	"iSpringCompGa = 0.000000000000000;\n",			// 22
	"iSpringCompGf = 0.000000000000000; \n",		// 14, 23
	"iSpringOffset = 0.000000000000000;\n",			// 24
	"iKxaGa = -6.5536E+006;\n",				// 25
	"iKxaGf = 17; \n",				        // 26
	"iKaxGa= -6.5536E+006; \n",				// 27
	"iKaxGf= 15;\n",					    // 28
	"\n",  // 29
	// ACC ->: /2E6, Jerk -> /2E9
	//  Acc:  ,             Jerk
	// 20E7 -> 100           80E9 -> 40000
	// 16E7 ->  80           40E9 -> 20000
	// 16E7  -> 80           40E9 -> 20000
	// 16E7  -> 80           40E9 -> 20000
	"arMotorXProfile(0)(0) = 1.9E6; arMotorXProfile(0)(1) = 20E7; arMotorXProfile(0)(2) = 80E9;\n",  // 30, LINE_ID_13V_13T_SPEED_PROFILE_DEF
	"arMotorXProfile(1)(0) = 1.9E6; arMotorXProfile(1)(1) = 16E7; arMotorXProfile(1)(2) = 40E9;\n",   // 31
	"arMotorXProfile(2)(0) = 1.9E6; arMotorXProfile(2)(1) = 16E7; arMotorXProfile(2)(2) = 40E9;\n",	// 32
	"arMotorXProfile(3)(0) = 1.9E6; arMotorXProfile(3)(1) = 16E7; arMotorXProfile(3)(2) = 40E9;\n",	// 33
	"\n",	// 34
	// ACC ->: /2E6, Jerk -> /2E9
	//  Acc:  ,             Jerk
	// 24E7 -> 120          100E9 -> 50000
	// 16E7 -> 80            80E9 -> 40000
	// 16E7 -> 80            40E9 -> 20000
	// 16E7 -> 80            40E9 -> 20000
	"arMotorYProfile(0)(0) = 1.9E6; arMotorYProfile(0)(1) = 24E7; arMotorYProfile(0)(2) = 100E9;\n", // 35
	"arMotorYProfile(1)(0) = 1.9E6; arMotorYProfile(1)(1) = 16E7; arMotorYProfile(1)(2) = 80E9;\n",	// 36
	"arMotorYProfile(2)(0) = 1.9E6; arMotorYProfile(2)(1) = 16E7; arMotorYProfile(2)(2) = 40E9;\n",	// 37
	"arMotorYProfile(3)(0) = 1.9E6; arMotorYProfile(3)(1) = 16E7; arMotorYProfile(3)(2) = 40E9;\n",	// 38
	"\n",	// 39
	"arMotorAProfile(0)(0) = 1.95E6; arMotorAProfile(0)(1) = 3.5E8; arMotorAProfile(0)(2) = 8E10;\n",	// 40
	"arMotorAProfile(1)(0) = 1.95E6; arMotorAProfile(1)(1) = 4.5E8; arMotorAProfile(1)(2) = 3E10;\n",	// 41
	"arMotorAProfile(2)(0) = 1.95E6; arMotorAProfile(2)(1) = 4.5E8; arMotorAProfile(2)(2) = 3E10;\n",	// 42
	"arMotorAProfile(3)(0) = 1.95E6; arMotorAProfile(3)(1) = 4.5E8; arMotorAProfile(3)(2) = 3E10;\n",	// 43
	"arMotorAProfile(4)(0) = 1.95E6; arMotorAProfile(4)(1) = 4.5E8; arMotorAProfile(4)(2) = 3E10;\n",	// 44, LINE_END_SPEED_CFG_DIFF_WB13V_13T_
	"\n",	// 45
};

// Controller Buffer Program#6 for 18V machine- SprintPro-18
char *strBufferProgram_OpenCtrlACS_Para_WB18V[] = 
{
	"int iJerkFF_Ga_AxisA, iJerkFF_Gf_AxisA;\n",  // 0
	"int iJerkFF_Ga_AxisX, iJerkFF_Gf_AxisX;\n",  // 1
	"int iJerkFF_Ga_AxisY, iJerkFF_Gf_AxisY;\n",  // 2
	"int iFlagSpringCompOn, iSpringCompGa, iSpringCompGf, iSpringOffset; int iKxaGa, iKxaGf, iKaxGa, iKaxGf; \n", // 3
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
	"iJerkFF_Gf_AxisA = 12;\n",                      // 16,  LINE_ID_SP_DEF_PARA_DECLARE_
	"iJerkFF_Ga_AxisA = 8.192E+006;\n",              // 17
	"iJerkFF_Gf_AxisX = 11;\n",                     // 18
	"iJerkFF_Ga_AxisX =8.192E+006;\n",              // 19
	"iJerkFF_Gf_AxisY = 11;\n",			            // 20
	"iJerkFF_Ga_AxisY = 8.192E+006;\n",             // 21
	"iSpringCompGa = 0.000000000000000;\n",			// 22
	"iSpringCompGf = 0.000000000000000; \n",		// 14, 23
	"iSpringOffset = 0.000000000000000;\n",			// 24
	"iKxaGa = -6.5536E+006;\n",				// 25
	"iKxaGf = 17; \n",				        // 26
	"iKaxGa= -6.5536E+006; \n",				// 27
	"iKaxGf= 15;\n",					    // 28
	"\n",  // 29
	// ACC ->: /2E6, Jerk -> /2E9
	//  Acc:  ,             Jerk
	// 20E7 -> 100           100E9 -> 50000
	// 16E7 -> 80            40E9 -> 20000
	// 16E7 -> 80            40E9 -> 20000
	// 16E7 -> 80            40E9 -> 20000
	"arMotorXProfile(0)(0) = 1.9E6; arMotorXProfile(0)(1) = 20E7; arMotorXProfile(0)(2) = 100E9;\n",  // 30, LINE_ID_13V_13T_SPEED_PROFILE_DEF
	"arMotorXProfile(1)(0) = 1.9E6; arMotorXProfile(1)(1) = 16E7; arMotorXProfile(1)(2) = 40E9;\n",   // 31
	"arMotorXProfile(2)(0) = 1.9E6; arMotorXProfile(2)(1) = 16E7; arMotorXProfile(2)(2) = 40E9;\n",	// 32
	"arMotorXProfile(3)(0) = 1.9E6; arMotorXProfile(3)(1) = 16E7; arMotorXProfile(3)(2) = 40E9;\n",	// 33
	"\n",	// 34
	// ACC ->: / 2E6, Jerk -> / 2E9                
	// Acc                   Jerk                  Jerk for Production 
	// 5E7 -> 25  ,          24E9 -> 12000           4.4E9 -> 2200
	// 3E7 -> 15  ,          16E9 -> 8000           4E9  -> 2000            
	// 3E7 -> 15  ,          16E9 -> 8000           3E9  -> 1500 ,          
	// 3E7 -> 15  ,          8E9 -> 4000           1E9  -> 500 

	"arMotorYProfile(0)(0) = 1.9E6; arMotorYProfile(0)(1) = 5E7; arMotorYProfile(0)(2) = 24E9;\n", // 35
	"arMotorYProfile(1)(0) = 1.9E6; arMotorYProfile(1)(1) = 3E7; arMotorYProfile(1)(2) = 16E9;\n",	// 36
	"arMotorYProfile(2)(0) = 1.9E6; arMotorYProfile(2)(1) = 3E7; arMotorYProfile(2)(2) = 16E9;\n",	// 37
	"arMotorYProfile(3)(0) = 1.9E6; arMotorYProfile(3)(1) = 3E7; arMotorYProfile(3)(2) = 8E9;\n",	// 38
	"\n",	// 39
	"arMotorAProfile(0)(0) = 1.95E6; arMotorAProfile(0)(1) = 3.5E8; arMotorAProfile(0)(2) = 8E10;\n",	// 40
	"arMotorAProfile(1)(0) = 1.95E6; arMotorAProfile(1)(1) = 4.5E8; arMotorAProfile(1)(2) = 3E11;\n",	// 41
	"arMotorAProfile(2)(0) = 1.95E6; arMotorAProfile(2)(1) = 4.5E8; arMotorAProfile(2)(2) = 3E11;\n",	// 42
	"arMotorAProfile(3)(0) = 1.95E6; arMotorAProfile(3)(1) = 4.5E8; arMotorAProfile(3)(2) = 3E11;\n",	// 43
	"arMotorAProfile(4)(0) = 1.95E6; arMotorAProfile(4)(1) = 8.0E8; arMotorAProfile(4)(2) = 6E11;\n",	// 44, LINE_END_SPEED_CFG_DIFF_WB13V_13T_
	"\n",	// 45
};
// SAB: ScriptAcsBuffprogram

#define BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT  7
#define LINE_ID_SP_DEF_PARA_DECLARE_              16
#define LINE_ID_13V_13T_SPEED_PROFILE_DEF         30
#define LINE_END_SPEED_CFG_DIFF_WB13V_13T_        44
#define LINE_END_SCRIPT_ACS_BUFFER_7_             117

static char strAppendACS_Buffer7[512];
static char strDownloadCmd[256];
static char strUploadMessage[256];
static char *strStopString;

#include "MtnWbDef.h"

//ifdef __ACS_BUFF_7
short acs_update_buffer_7_from_ram_para_(int iMachType)
{
short sRet = MTN_API_OK_ZERO;

	// download buffer program
	int ii = 0;

	for(ii =0; ii< LINE_ID_SP_DEF_PARA_DECLARE_; ii++)  //  20120906
	{
		if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
			BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
			strBufferProgram_OpenCtrlACS_Para_13V[ii], // buffer contained ACSPL+ program(s)
			(int)strlen(strBufferProgram_OpenCtrlACS_Para_13V[ii]), // size of this buffer
			NULL // waiting call
			))
		{
			sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
				acsc_GetLastError());
#ifndef __MOTALGO_DLL
			if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
			sRet = MTN_API_ERR_START_FIRMWARE_PROG;
			goto label_acs_update_buffer_7_from_ram_para_;
		}
	}

int iCountReceived;
int iTimeout = 50;

	//JerkFF_Gf_AxisA
int iJerkFF_Gf_AxisA;
double dJerkFF_Gf_AxisA;
	sprintf_s(strDownloadCmd, 256, "?GetSPV(SP0:A_JerkFFGf)\n\r"); //,  %d
//		stOutput24bFloat.iGain);
	if(acsc_Transaction(stCommHandleACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dJerkFF_Gf_AxisA = strtod(strUploadMessage, &strStopString);
			iJerkFF_Gf_AxisA = (int)dJerkFF_Gf_AxisA;
			sprintf_s(strAppendACS_Buffer7, 512, "iJerkFF_Gf_AxisA = %d;\n", iJerkFF_Gf_AxisA);                      // 13
			
			acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "iJerkFF_Gf_AxisA", 0, 0, 0, 0, &iJerkFF_Gf_AxisA, NULL );
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
	#ifndef __MOTALGO_DLL
				if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
	#endif // __MOTALGO_DLL
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_Para_13V[7], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_Para_13V[7]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
	#ifndef __MOTALGO_DLL
				if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
	#endif // __MOTALGO_DLL
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}

//	"iJerkFF_Ga_AxisA = 7.68E+006;\n",              // 14
int iJerkFF_Ga_AxisA;
double dJerkFF_Ga_AxisA;
	sprintf_s(strDownloadCmd, 256, "?GetSPV(SP0:A_JerkFFGa)\n\r"); //,  %d
	if(acsc_Transaction(stCommHandleACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dJerkFF_Ga_AxisA = strtod(strUploadMessage, &strStopString);
			iJerkFF_Ga_AxisA = (int)dJerkFF_Ga_AxisA;
			acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "iJerkFF_Ga_AxisA", 0, 0, 0, 0, &iJerkFF_Ga_AxisA, NULL );

			sprintf_s(strAppendACS_Buffer7, 512, "iJerkFF_Ga_AxisA = %d;\n", iJerkFF_Ga_AxisA);                     
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
	#ifndef __MOTALGO_DLL
				if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
	#endif // __MOTALGO_DLL
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_Para_13V[8], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_Para_13V[8]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
	#ifndef __MOTALGO_DLL
				if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
	#endif // __MOTALGO_DLL
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}

	//"iJerkFF_Gf_AxisX = 11;\n",                     // 15
int iJerkFF_Gf_AxisX;
double dJerkFF_Gf_AxisX;
	sprintf_s(strDownloadCmd, 256, "?GetSPV(SP0:X_JerkFFGf)\n\r"); //,  %d
//		stOutput24bFloat.iGain);
	if(acsc_Transaction(stCommHandleACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dJerkFF_Gf_AxisX = strtod(strUploadMessage, &strStopString);
			iJerkFF_Gf_AxisX = (int)dJerkFF_Gf_AxisX;
			sprintf_s(strAppendACS_Buffer7, 512, "iJerkFF_Gf_AxisX = %d;\n", iJerkFF_Gf_AxisX);                      
			
			acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "iJerkFF_Gf_AxisX", 0, 0, 0, 0, &iJerkFF_Gf_AxisX, NULL );
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
	#ifndef __MOTALGO_DLL
				if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
	#endif // __MOTALGO_DLL
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_Para_13V[9], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_Para_13V[9]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
	#ifndef __MOTALGO_DLL
				if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
	#endif // __MOTALGO_DLL
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}
	//"iJerkFF_Ga_AxisX =8.192E+006;\n",              // 16
int iJerkFF_Ga_AxisX;
double dJerkFF_Ga_AxisX;
	sprintf_s(strDownloadCmd, 256, "?GetSPV(SP0:X_JerkFFGa)\n\r"); //,  %d
	if(acsc_Transaction(stCommHandleACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dJerkFF_Ga_AxisX = strtod(strUploadMessage, &strStopString);
			iJerkFF_Ga_AxisX = (int)dJerkFF_Ga_AxisX;
			acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "iJerkFF_Ga_AxisX", 0, 0, 0, 0, &iJerkFF_Ga_AxisX, NULL );

			sprintf_s(strAppendACS_Buffer7, 512, "iJerkFF_Ga_AxisX = %d;\n", iJerkFF_Ga_AxisX);                      
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
	#ifndef __MOTALGO_DLL
				if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
	#endif // __MOTALGO_DLL
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_Para_13V[10], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_Para_13V[10]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
	#ifndef __MOTALGO_DLL
				if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
	#endif // __MOTALGO_DLL
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}

	//"iJerkFF_Gf_AxisY = 11;\n",			            // 17
int iJerkFF_Gf_AxisY;
double dJerkFF_Gf_AxisY;
	sprintf_s(strDownloadCmd, 256, "?GetSPV(SP1:X_JerkFFGf)\n\r"); //,  %d
//		stOutput24bFloat.iGain);
	if(acsc_Transaction(stCommHandleACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dJerkFF_Gf_AxisY = strtod(strUploadMessage, &strStopString);
			iJerkFF_Gf_AxisY = (int)dJerkFF_Gf_AxisY;
			sprintf_s(strAppendACS_Buffer7, 512, "iJerkFF_Gf_AxisY = %d;\n", iJerkFF_Gf_AxisY);                      
			
			acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "iJerkFF_Gf_AxisY", 0, 0, 0, 0, &iJerkFF_Gf_AxisY, NULL );
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
	#ifndef __MOTALGO_DLL
				if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
	#endif // __MOTALGO_DLL
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_Para_13V[11], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_Para_13V[11]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
	#ifndef __MOTALGO_DLL
				if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
	#endif // __MOTALGO_DLL
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}

	//"iJerkFF_Ga_AxisY = 8.192E+006;\n",             // 18
int iJerkFF_Ga_AxisY;
double dJerkFF_Ga_AxisY;
	sprintf_s(strDownloadCmd, 256, "?GetSPV(SP1:X_JerkFFGa)\n\r"); //,  %d
	if(acsc_Transaction(stCommHandleACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dJerkFF_Ga_AxisY = strtod(strUploadMessage, &strStopString);
			iJerkFF_Ga_AxisY = (int)dJerkFF_Ga_AxisY;
			acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "iJerkFF_Ga_AxisY", 0, 0, 0, 0, &iJerkFF_Ga_AxisY, NULL );

			sprintf_s(strAppendACS_Buffer7, 512, "iJerkFF_Ga_AxisY = %d;\n", iJerkFF_Ga_AxisY);                      
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
	#ifndef __MOTALGO_DLL
				if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
	#endif // __MOTALGO_DLL
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_Para_13V[12], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_Para_13V[12]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
	#ifndef __MOTALGO_DLL
				if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
	#endif // __MOTALGO_DLL
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}

//"iSpringCompGa = 0.000000000000000;\n",			// 13 // 20120810
int iSpringCompGa;
double dSpringCompGa;
	sprintf_s(strDownloadCmd, 256, "?GetSPV(SP0:SpringCompGa)\n\r"); //,  %d
	if(acsc_Transaction(stCommHandleACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dSpringCompGa = strtod(strUploadMessage, &strStopString);
			iSpringCompGa = (int)dSpringCompGa;
			acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "iSpringCompGa", 0, 0, 0, 0, &iSpringCompGa, NULL );

			sprintf_s(strAppendACS_Buffer7, 512, "iSpringCompGa = %d;\n", iSpringCompGa);                      
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_Para_13V[13], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_Para_13V[13]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}
//"iSpringCompGf = 0.000000000000000; \n",		// 14 // 20120810
int iSpringCompGf;
double dSpringCompGf;
	sprintf_s(strDownloadCmd, 256, "?GetSPV(SP0:SpringCompGf)\n\r"); //,  %d
	if(acsc_Transaction(stCommHandleACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dSpringCompGf = strtod(strUploadMessage, &strStopString);
			iSpringCompGf = (int)dSpringCompGf;
			acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "iSpringCompGf", 0, 0, 0, 0, &iSpringCompGf, NULL );

			sprintf_s(strAppendACS_Buffer7, 512, "iSpringCompGf = %d;\n", iSpringCompGf);                      
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_Para_13V[14], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_Para_13V[14]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}

//"iSpringOffset = 0.000000000000000;\n",			// 15 // 20120810
int iSpringOffset;
double dSpringOffset;
	sprintf_s(strDownloadCmd, 256, "?GetSPV(SP0:SpringOffset)\n\r"); //,  %d
	if(acsc_Transaction(stCommHandleACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dSpringOffset = strtod(strUploadMessage, &strStopString);
			iSpringOffset = (int)dSpringOffset;
			acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "iSpringOffset", 0, 0, 0, 0, &iSpringOffset, NULL );

			sprintf_s(strAppendACS_Buffer7, 512, "iSpringOffset = %d;\n", iSpringOffset);                      
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_Para_13V[15], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_Para_13V[15]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}

//"iKxaGa = 0.000000000000000;\n",				// 16 // 20120810
int iKxaGa;
double dKxaGa;
	sprintf_s(strDownloadCmd, 256, "?GetSPV(SP0:KxaGa)\n\r"); //,  %d
	if(acsc_Transaction(stCommHandleACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dKxaGa = strtod(strUploadMessage, &strStopString);
			iKxaGa = (int)dKxaGa;
			acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "iKxaGa", 0, 0, 0, 0, &iKxaGa, NULL );

			sprintf_s(strAppendACS_Buffer7, 512, "iKxaGa = %d;\n", iKxaGa);                      
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_Para_13V[16], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_Para_13V[16]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}

//"iKxaGf = 0.000000000000000; \n",				// 17 // 20120810
int iKxaGf;
double dKxaGf;
	sprintf_s(strDownloadCmd, 256, "?GetSPV(SP0:KxaGf)\n\r"); //,  %d
	if(acsc_Transaction(stCommHandleACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dKxaGf = strtod(strUploadMessage, &strStopString);
			iKxaGf = (int)dKxaGf;
			acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "iKxaGf", 0, 0, 0, 0, &iKxaGf, NULL );

			sprintf_s(strAppendACS_Buffer7, 512, "iKxaGf = %d;\n", iKxaGf);                      
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_Para_13V[17], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_Para_13V[17]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}

//"iKaxGa= 0.000000000000000; \n",				// 18 // 20120810
int iKaxGa;
double dKaxGa;
	sprintf_s(strDownloadCmd, 256, "?GetSPV(SP0:KaxGa)\n\r"); //,  %d
	if(acsc_Transaction(stCommHandleACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dKaxGa = strtod(strUploadMessage, &strStopString);
			iKaxGa = (int)dKaxGa;
			acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "iKaxGa", 0, 0, 0, 0, &iKaxGa, NULL );

			sprintf_s(strAppendACS_Buffer7, 512, "iKaxGa = %d;\n", iKaxGa);                      
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_Para_13V[18], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_Para_13V[18]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}

//"iKaxGf= 0.000000000000000;\n",					// 19 // 20120810
int iKaxGf;
double dKaxGf;
	sprintf_s(strDownloadCmd, 256, "?GetSPV(SP0:KaxGf)\n\r"); //,  %d
	if(acsc_Transaction(stCommHandleACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL))
	{
		if(iCountReceived >= 2)
		{
            strUploadMessage[iCountReceived] = '\0';
			dKaxGf = strtod(strUploadMessage, &strStopString);
			iKaxGf = (int)dKaxGf;
			acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "iKaxGf", 0, 0, 0, 0, &iKaxGf, NULL );

			sprintf_s(strAppendACS_Buffer7, 512, "iKaxGf = %d;\n", iKaxGf);                      
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strAppendACS_Buffer7, // buffer contained ACSPL+ program(s)
				(int)strlen(strAppendACS_Buffer7), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
		}
		else
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_Para_13V[19], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_Para_13V[19]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
			sRet = MTN_API_ERROR;
		}
	}

	if(iMachType == WB_MACH_TYPE_HORI_LED ||
		iMachType == WB_STATION_XY_TOP) // 20111213 // 20120810
	{
		for(ii =LINE_ID_13V_13T_SPEED_PROFILE_DEF; ii<= LINE_END_SPEED_CFG_DIFF_WB13V_13T_; ii++)  // First 6  // 2012
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_Para_WB13T[ii], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_Para_WB13T[ii]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
	#ifndef __MOTALGO_DLL
				if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
	#endif // __MOTALGO_DLL
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
			Sleep(20);
		}
	}
	// 18V
	else if(iMachType == BE_WB_ONE_TRACK_18V_LED)
	{ // strBufferProgram_OpenCtrlACS_Para_WB18V
		for(ii =LINE_ID_13V_13T_SPEED_PROFILE_DEF; ii<= LINE_END_SPEED_CFG_DIFF_WB13V_13T_; ii++)  // First 6 Line {1, ..., 6} done // 2012
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_Para_WB18V[ii], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_Para_WB18V[ii]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
			Sleep(20);
		}
	}
	else if(iMachType == BE_WB_HORI_20T_LED)
	{ // strBufferProgram_OpenCtrlACS_Para_WB20T
		for(ii =LINE_ID_13V_13T_SPEED_PROFILE_DEF; ii<= LINE_END_SPEED_CFG_DIFF_WB13V_13T_; ii++)  // First 6 Line {1, ..., 6} done // 2012
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_Para_WB20T[ii], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_Para_WB20T[ii]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
			Sleep(20);
		}
	}
	else   // 20120810
	{
		for(ii =LINE_ID_13V_13T_SPEED_PROFILE_DEF; ii<= LINE_END_SPEED_CFG_DIFF_WB13V_13T_; ii++)  // First 6 Line {1, ..., 6} done // 2012
		{
			if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
				BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
				strBufferProgram_OpenCtrlACS_Para_13V[ii], // buffer contained ACSPL+ program(s)
				(int)strlen(strBufferProgram_OpenCtrlACS_Para_13V[ii]), // size of this buffer
				NULL // waiting call
				))
			{
				sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
				sRet = MTN_API_ERR_START_FIRMWARE_PROG;
				goto label_acs_update_buffer_7_from_ram_para_;
			}
			Sleep(20);
		}
	}

	for(ii = LINE_END_SPEED_CFG_DIFF_WB13V_13T_ + 1; ii <= LINE_END_SCRIPT_ACS_BUFFER_7_; ii++)
	{
		if (!acsc_AppendBuffer( stCommHandleACS, // communication handle
			BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
			strBufferProgram_OpenCtrlACS_Para_13V[ii], // buffer contained ACSPL+ program(s)
			(int)strlen(strBufferProgram_OpenCtrlACS_Para_13V[ii]), // size of this buffer
			NULL // waiting call
			))
		{
			sprintf_s(strDebugText, 128, "Error: error download buffer program- %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
					acsc_GetLastError());
	#ifndef __MOTALGO_DLL
			if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
	#endif // __MOTALGO_DLL
			sRet = MTN_API_ERR_START_FIRMWARE_PROG;
			goto label_acs_update_buffer_7_from_ram_para_;
		}
		Sleep(20);
	}

label_acs_update_buffer_7_from_ram_para_:
	return sRet;
}

short acs_compile_start_buffer_7()
{
	short sRet = MTN_API_OK_ZERO;
	// Compile buffer
	if (!acsc_CompileBuffer( stCommHandleACS, // communication handle
		BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, // ACSPL+ program buffer number
		NULL // waiting call
		))
		{
			sprintf_s(strDebugText, 128, "Error: error compile buffer program - %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, 
				acsc_GetLastError());
#ifndef __MOTALGO_DLL
			if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
			sRet = MTN_API_ERR_START_FIRMWARE_PROG;
		}

	Sleep(100);
	if (!acsc_RunBuffer(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, NULL, NULL))
	{
		sprintf_s(strDebugText, 128, "Error: error start buffer program - %d, Error Code: %d", BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
			acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		sRet = MTN_API_ERR_START_FIRMWARE_PROG;
	}
	Sleep(1000);
	return sRet;
}

//#endif // __ACS_BUFF_7
int acs_write_buffer_7_to_flash_()
{
	short sRet = MTN_API_OK_ZERO;

	return sRet;
}

short acs_clear_buffer_7_from_ram_para_()
{
	short sRet = MTN_API_OK_ZERO;

	if (!acsc_StopBuffer(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, NULL))
	{
		sRet = MTN_API_ERR_STOP_FIRMWARE_PROG;
		goto label_acs_clear_buffer_7_from_ram_para_;
	}

	if (!acsc_ClearBuffer(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, 1, ACSC_MAX_LINE, NULL))  // nNumLinesInBuffer_SearchContactForceControl
	{
		sRet = MTN_API_ERR_CLEAR_FIRMWARE_PROG;
		goto label_acs_clear_buffer_7_from_ram_para_;
	}

label_acs_clear_buffer_7_from_ram_para_:
	return sRet;
}

// Trigger BuffProg-0: 1st Bond
// lRestartBond: int dpr 0x9C, lRestartBond
// lTrigNext1stPt0 
// lTrigNext1stPt1 
////////////////////////////////////////////////// Bond One Wire from System Buffer
#define BUFFER_ID_SYS_B1W_0    0
#define __BE_SYS_ACS_BUFF_DPR_BOND_PROGESS__    (0x88)
#define __BE_SYS_ACS_BUFF_DPR_START_BOND__      (0x9C)
#define __BE_SYS_ACS_BUFF_DPR_LAST_WIRE_OK_ERR__  (0x84)
int acs_bufprog_0_restart_b1w()
{
	int iRet = MTN_API_OK_ZERO;
	int iBondOneWireTriggerStart = 1;
	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SYS_B1W_0, "lTrigNext1stPt0", 0, 0, 0, 0, &iBondOneWireTriggerStart, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}
	else
	{
		if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SYS_B1W_0, "IntData", 0, 0, 33, 33, &iBondOneWireTriggerStart, NULL ))
		{
			iRet = MTN_API_ERROR_ACS_BUFF_PROG;
		}
		if(!acsc_WriteDPRAMInteger(stCommHandleACS, __BE_SYS_ACS_BUFF_DPR_LAST_WIRE_OK_ERR__, iBondOneWireTriggerStart)) // DPR-flag to judge EFO-GoodNG
		//if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SYS_B1W_0, "lRestartBond", 0, 0, 0, 0, &iBondOneWireTriggerStart, NULL ))
		{
			iRet = MTN_API_ERROR_ACS_BUFF_PROG;
		}
		if(!acsc_WriteDPRAMInteger(stCommHandleACS, __BE_SYS_ACS_BUFF_DPR_START_BOND__, iBondOneWireTriggerStart))
		//if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SYS_B1W_0, "lRestartBond", 0, 0, 0, 0, &iBondOneWireTriggerStart, NULL ))
		{
			iRet = MTN_API_ERROR_ACS_BUFF_PROG;
		}
	}

	return iRet;
}

int acs_buffprog_0_get_bond_progress()
{
	int iBondProgress = 0;
	if(!acsc_ReadDPRAMInteger(stCommHandleACS, __BE_SYS_ACS_BUFF_DPR_BOND_PROGESS__, &iBondProgress))
	//if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SYS_B1W_0, "lRestartBond", 0, 0, 0, 0, &iBondOneWireTriggerStart, NULL ))
	{
		iBondProgress = 0;
	}

	return iBondProgress;
}

int acs_bufprog_0_run_()
{
	int iRet = MTN_API_OK_ZERO;
	if (!acsc_RunBuffer(stCommHandleACS, BUFFER_ID_SYS_B1W_0, NULL, NULL))
	{
		sprintf_s(strDebugText, 128, "Error: error start buffer program- %d, Error Code: %d", BUFFER_ID_SYS_B1W_0,
			acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		iRet = MTN_API_ERR_START_FIRMWARE_PROG;
	}

	return iRet;
}

int acs_bufprog_0_stop_()
{
	int iRet = MTN_API_OK_ZERO;
	if (!acsc_StopBuffer(stCommHandleACS, BUFFER_ID_SYS_B1W_0, NULL))
	{
		sprintf_s(strDebugText, 128, "Error: error start buffer program- %d, Error Code: %d", BUFFER_ID_SYS_B1W_0,
			acsc_GetLastError());
#ifndef __MOTALGO_DLL
		if(cFlagEnableDebug) AfxMessageBox(_T(strDebugText));
#endif // __MOTALGO_DLL
		iRet = MTN_API_ERR_START_FIRMWARE_PROG;
	}

	return iRet;
}

int acs_command_save_parameter()
{
int iRet = MTN_API_OK_ZERO;
char strDownloadCmd[256];
char strUploadMessage[256];
int iCountReceived = 10;
int iTimeout = 10;
int iFailCount = 0;

	sprintf_s(strDownloadCmd, 256, "#SAVEPAR\n\r"); //,  %d

	acsc_Transaction(stCommHandleACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL);
	while(iCountReceived >= 2)
	{
		iFailCount ++;
		if(iFailCount >= iTimeout)
		{
			iRet = MTN_API_ERR_SAVE_ACS_FLASH;
			break;
		}
		Sleep(1000);
		acsc_Transaction(stCommHandleACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL);

	}

	return iRet;
}

int acs_command_save_program()
{
int iRet = MTN_API_OK_ZERO;

char strDownloadCmd[256];
char strUploadMessage[256];
int iCountReceived = 10;
int iTimeout = 10;
int iFailCount = 0;

	Sleep(500);

	sprintf_s(strDownloadCmd, 256, "#SAVEPROG 7\n\r"); //,  %d

	acsc_Transaction(stCommHandleACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL);
	while(iCountReceived >= 2)
	{
		iFailCount ++;
		if(iFailCount >= iTimeout)
		{
			iRet = MTN_API_ERR_SAVE_ACS_FLASH;
			break;
		}
		Sleep(1000);
		acsc_Transaction(stCommHandleACS, strDownloadCmd, (int)strlen(strDownloadCmd)+2, strUploadMessage, 256, &iCountReceived, NULL);
	}

	return iRet;
}

int acs_read_current_rms_array_wb13v_cfg(double *adRMS)
{
int iRet = MTN_API_OK_ZERO;
double adCurrentRMS[4];

	// ACSC_NONE: global variable
	if(!acsc_ReadReal(stCommHandleACS, ACSC_NONE, "arRMS", 0, 3, 0, 0, &adCurrentRMS[0], NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}
	else
	{
		adRMS[0] = adCurrentRMS[0];
		adRMS[1] = adCurrentRMS[1];
		adRMS[2] = adCurrentRMS[2];
		adRMS[3] = adCurrentRMS[3];
	}

	return iRet;
}

int acs_read_current_rms_prot_lmt_array_wb13v_cfg(double *adRMS_ProtLmt)
{
int iRet = MTN_API_OK_ZERO;
double adXRMS[8];

	// ACSC_NONE: global variable
	if(!acsc_ReadReal(stCommHandleACS, ACSC_NONE, "XRMS", 0, 7, 0, 0, &adXRMS[0], NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}
	else
	{
		adRMS_ProtLmt[0] = adXRMS[0];
		adRMS_ProtLmt[1] = adXRMS[1];
		adRMS_ProtLmt[2] = adXRMS[4];
		adRMS_ProtLmt[3] = adXRMS[5];
	}

	return iRet;
}

// CERRI: Critical Position Error in Idle mode
int acs_read_current_pe_prot_lmt_array_wb13v_cfg(double *adPosnErr_ProtLmt)
{
int iRet = MTN_API_OK_ZERO;
double adCriticalLmtPE[8];

	// ACSC_NONE: global variable
	if(!acsc_ReadReal(stCommHandleACS, ACSC_NONE, "CERRI", 0, 7, 0, 0, &adCriticalLmtPE[0], NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}
	else
	{
		adPosnErr_ProtLmt[0] = adCriticalLmtPE[0];
		adPosnErr_ProtLmt[1] = adCriticalLmtPE[1];
		adPosnErr_ProtLmt[2] = adCriticalLmtPE[4];
		adPosnErr_ProtLmt[3] = adCriticalLmtPE[5];
	}

	return iRet;
}

int acs_read_current_posn_err_array_wb13v_cfg(double *adPosnErr)
{
int iRet = MTN_API_OK_ZERO;
//double adServoPositionErr[4];

    if(stCommHandleACS != ACSC_INVALID)
	{
		mtn_api_get_position_err(stCommHandleACS, 0, &adPosnErr[0]);
		mtn_api_get_position_err(stCommHandleACS, 1, &adPosnErr[1]);
		mtn_api_get_position_err(stCommHandleACS, 4, &adPosnErr[2]);
		mtn_api_get_position_err(stCommHandleACS, 5, &adPosnErr[3]);
	}
	else
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	return iRet;
}

// Data defined in BUFFER_ID_SEARCH_CONTACT
int acs_set_search_height_posn(int iSrchHt)
{
//	IntData(3), 
int iRet = MTN_API_OK_ZERO;
	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SEARCH_CONTACT, "IntData", 0, 0, 3, 3, &iSrchHt, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	return iRet;
}
int acs_set_search_velocity(int iSrchVel)
{ // abs(IntData(4))
int iRet = MTN_API_OK_ZERO;
	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SEARCH_CONTACT, "IntData", 0, 0, 4, 4, &iSrchVel, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	return iRet;
}
int acs_set_search_stop_position(double dSearchStopPosn)
{
	
int iRet = MTN_API_OK_ZERO;
	if(!acsc_WriteReal(stCommHandleACS, BUFFER_ID_SEARCH_CONTACT, "fStopSearchPosn", 0, 0, 0, 0, &dSearchStopPosn, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}
	return iRet;
}

int acs_set_search_tune_bnd_z_tail_dist(int iTailDist)
{
//IntData(31);",
int iRet = MTN_API_OK_ZERO;
	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SEARCH_CONTACT, "IntData", 0, 0, 31, 31, &iTailDist, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	return iRet;
}

int acs_set_search_tune_bnd_z_reset_posn(int iResetPosn)
{
// 	"		PTP A, IntData(32);",
int iRet = MTN_API_OK_ZERO;
	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SEARCH_CONTACT, "IntData", 0, 0, 32, 32, &iResetPosn, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}

	return iRet;
}

// 20120512
int acs_set_search_tune_bnd_z_reverse_height(int iRevHt)
{
int iRet = MTN_API_OK_ZERO;
	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SEARCH_CONTACT, "iRevHeight", 0, 0, 0, 0, &iRevHt, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}
	return iRet;
}

int acs_set_search_tune_bnd_z_kink_height(int iKinkHt)
{
int iRet = MTN_API_OK_ZERO;
	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SEARCH_CONTACT, "iKinkHeight", 0, 0, 0, 0, &iKinkHt, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}
	return iRet;
}

int acs_set_search_tune_bnd_z_loop_top(int iLoopTop)
{
int iRet = MTN_API_OK_ZERO;
	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SEARCH_CONTACT, "iLoopTop", 0, 0, 0, 0, &iLoopTop, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}
	return iRet;
}

// iFlagEnableEmergencyCheck
int acs_enable_check_in3_flag()
{
int iRet = MTN_API_OK_ZERO;
int iFlagEnableEmergencyCheck = 1;
	if(!acsc_WriteInteger(stCommHandleACS, ACSC_NONE, "iFlagEnableEmergencyCheck", 0, 0, 0, 0, &iFlagEnableEmergencyCheck, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}
	return iRet;
}

int acs_disable_check_in3_flag()
{
int iRet = MTN_API_OK_ZERO;
int iFlagEnableEmergencyCheck = 0;
	if(!acsc_WriteInteger(stCommHandleACS, ACSC_NONE, "iFlagEnableEmergencyCheck", 0, 0, 0, 0, &iFlagEnableEmergencyCheck, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}
	return iRet;
}
// iFlagEnableCalcRMS
int acs_enable_check_rms_flag()
{
int iRet = MTN_API_OK_ZERO;
int iFlagEnableCalcRMS = 1;
	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "iFlagEnableCalcRMS", 0, 0, 0, 0, &iFlagEnableCalcRMS, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}
	return iRet;
}

int acs_disable_check_rms_flag()
{
int iRet = MTN_API_OK_ZERO;
int iFlagEnableCalcRMS = 0;
	if(!acsc_WriteInteger(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, "iFlagEnableCalcRMS", 0, 0, 0, 0, &iFlagEnableCalcRMS, NULL ))
	{
		iRet = MTN_API_ERROR_ACS_BUFF_PROG;
	}
	return iRet;
}

short acs_run_buffer_prog_rms_emerg_stop()
{
	short sRet = MTN_API_OK_ZERO;
	if (!acsc_RunBuffer(stCommHandleACS, BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT, NULL, NULL))
	{
		sprintf_s(strDebugText, 128, "Error: error start buffer program - %d, Error Code: %d", 
			BUFFER_ID_SP_DEF_PARA_RMS_EMERGENT,
			acsc_GetLastError());
		sRet = MTN_API_ERR_START_FIRMWARE_PROG;
	}
	Sleep(10);
	return sRet;

}
// Velocity Loop test,  iAFT_FlagMotion == 2