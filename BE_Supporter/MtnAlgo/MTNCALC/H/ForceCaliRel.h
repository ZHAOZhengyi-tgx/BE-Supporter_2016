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


#pragma once

/// force calibration
#define DEF_MAX_FORCE_GRAM    (220.0)
#define DEF_MIN_FORCE_GRAM    (5.0)
#define DEF_FORCE_POSN_FACTOR_KA    (0.00012162)
#define DEF_FORCE_FACTOR_KF   (-0.008581)
#define DEF_FORCE_OFFSET_I0   (-0.19)
#define DEF_FORCE_SENSOR_PCB_GRAM_PER_ADC   (0.1087 * 5.0/ 9 * 2.6)
#define DEF_FORCE_GRAM_PER_ADC   (500.0/8192.0)

typedef struct
{
	int iContactPosnReg;
	int iStatus;	// 0: OK
					// 1: TIME_OUT
					// 2: DISTANCE_OUT
					// 3: MOTOR_ERROR
	double dInitForceCommandReadBack;
	double fForceFactor_Kf;
	double fForceOffset_I0;
	double fPosnFactor_Ka;

	double fForceCalibratePa;
	double fForceCalibratePb;
	double fForceCalibratePc;
}FORCE_CALIBRATION_OUTPUT;
int mtn_force_calibration(HANDLE m_hHandle, FORCE_CALIBRATION_INPUT *stpForceCalibrationInput, FORCE_CALIBRATION_OUTPUT *stpForceCaliOutput);

/// Force verification
typedef struct
{
	double dForceVerifyDesiredGram;
	double fPosnFactor_Ka;
	double fForceFactor_Kf;
	double fForceOffset_I0;
	int iContactPosn;
	TEACH_CONTACT_INPUT  stTeachContactPara;
	int iFlagTeachContact;
	int iFlagDebug;
}FORCE_VERIFY_INPUT;

typedef struct
{
	int iContactPosnReg;
	int iStatus;	// 0: OK
					// 1: TIME_OUT
					// 2: DISTANCE_OUT
					// 3: MOTOR_ERROR
	double dInitForceCommandReadBack;
	double dForceVerifyReadBack;
}FORCE_VERIFY_OUTPUT;
int mtn_force_verify(HANDLE hHandle, FORCE_VERIFY_INPUT *stpForceVerifyInput, FORCE_VERIFY_OUTPUT *stpForceVerifyOutput);

