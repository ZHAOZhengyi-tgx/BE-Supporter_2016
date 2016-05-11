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
#ifndef __MTN_DEFINE__
#define __MTN_DEFINE__

//#ifdef __cplusplus      /* For the mentally challenged */
//extern "C" {
//#endif


typedef unsigned char 	MOT_ALGO_BYTE;
typedef unsigned char   MOT_ALGO_UCHAR;
typedef unsigned short 	MOT_ALGO_WORD;
typedef unsigned long 	MOT_ALGO_DWORD;
typedef unsigned int    MOT_ALGO_UINT;
typedef unsigned long   MOT_ALGO_ULONG;
typedef unsigned short  MOT_ALGO_USHORT;

typedef signed short    MOT_ALGO_BOOL;
typedef signed char     MOT_ALGO_SBYTE;
typedef signed short    MOT_ALGO_SHORT;
typedef signed int      MOT_ALGO_INT;
typedef signed long     MOT_ALGO_LONG;
typedef char            MOT_ALGO_CHAR;
typedef float           MOT_ALGO_FLOAT;
typedef double          MOT_ALGO_DBL;

typedef signed short    MTN_SHORT;

// For module mtncalc
#define MTN_CALC_SUCCESS        0
#define MTN_CALC_ERROR          1
#define MTN_CALC_FORCE_SENSOR_ERROR      2
#define MTN_CALC_FORCE_SENSOR_SATURATION 3
#define MTN_EROR_FORCE_RATIO_OUT_RANGE   4

// for mtn aft and mtn_api
#define MTN_API_OK_ZERO				0
#define MTN_API_ERROR_UPLOAD_DATA   -1
#define MTN_API_ERROR_OPEN_FILE		-2
#define MTN_API_ERROR_READ_FILE     -3
#define MTN_API_ERROR_COUNT_NULL_STRING -4
#define MTN_API_ERROR_MEMORY		-5
#define MTN_API_ERR_FILE_PTR            -6
#define MTN_API_ERR_RENAME_FILE		-7
#define MTN_API_ERR_MODIFY_STRING   -8
#define MTN_API_ERR_INSERT_STRING   -9
#define MTN_API_ERR_INVALID_AXIS    -10
#define MTN_API_ERROR_DOWNLOAD_DATA -11
#define MTN_API_ERROR_SEARCH_TIMEOUT -12
#define MTN_API_ERR_BSD_ZERO_FREQ_FACTOR   -13
#define MTN_API_ERR_ROYA_EXCEED_MAX_TIMER  -14
#define MTN_API_ERROR_COMMUNICATION_WITH_CONTROLLER    -15
#define MTN_API_ERROR_EXCEED_MAX_BLK    -16
#define MTN_API_ERROR_ACS_BUFF_PROG     -17
#define MTN_API_ERROR_ACS_ERROR_SETTLING -18
#define MTN_API_ERROR_WRONG_POLARITY   -19
#define MTN_API_ERROR_DRIVER_NO_FORCE  -20
#define MTN_API_ERROR_SETTLING         -21

// for network errors
#define MTN_API_ERROR_START_WIN_SOCKET_DLL  101
#define MTN_API_ERROR_GET_HOSTNAME			102
#define MTN_API_ERROR_GET_HOST				103

// For initialization WB-Motion Default parameter
#define MTN_API_ERROR_INIT_WB_MOTION        200
#define MTN_API_ERROR_TUNING_NOT_ALL_PASS   201
// for BSD 300 , 400

#define MTN_API_ERROR				1   // General error, check corresponding strText for error message
#define MTN_API_ERROR_NOT_SUPPORT_VENDOR 2
// Serial Port Communication
#define UNABLE_TO_READ_FROM_COM_PORT   1001

// For all the feature name, maximum string length
// SERVO_AXIS_BLK: <strApplicationNameAxis, strAxisNameAcs>
// SERVO_ACS: strAxisNameAcs
// All debug strings, in mtnconfig.cpp, 
//                    in mtnconfig.h, 
#define MTN_API_MAX_STRLEN_FEATURENAME   128
// locally used in MtnConfig.cpp
#define OPENERR         1
#define OPENOK          0

#define READERR 		2
#define READOK          0

#define CFG_MAX_NUM_BLOCKS 128

#define MTN_API_MAX_STRLEN_FILENAME      512
#define __MAX_FILE_NAME__  (MTN_API_MAX_STRLEN_FILENAME)

///// ACS Bits
#define _FULL_BITS_INTEGER_32b_             0xFFFFFFFF
#define  AND_CLEAR_BIT_1    0xFFFFFFFD
#define  OR_SET_BIT_1    0x2
#define  AND_CLEAR_BIT_17    0xFFFDFFFF
#define  OR_SET_BIT_17    0x20000

//// Servo Assembly Functional Test
#define MAX_DRIVE_OUT_RANGE_CASES    7
typedef struct
{
	int iRangeId;
	double dRangeNum;
	char *strRangeText;
}RANGE_DRIVE_OUT;

// Open Loop Tuning Driver
typedef struct
{
	int iMoveCurrLoopTestUppLevel;
	int iMoveCurrLoopTestLowLevel;
	unsigned int uiTotalIter;
	int aiControllerAxisCurrentCommand[8];
}MOVE_CURR_LOOP_STEP_TEST;

#define MAX_NUM_TRACE_IN_SCOPE_DATA_BUFFER  128
typedef struct {

	unsigned int uiDataLen;
	unsigned int uiNumData;
	double dMaxDataAtTrace[MAX_NUM_TRACE_IN_SCOPE_DATA_BUFFER];
	double dMinDataAtTrace[MAX_NUM_TRACE_IN_SCOPE_DATA_BUFFER];
	double dSamplePeriod_ms;
}MTN_SCOPE;

// 2. Language Option
#define LANGUAGE_UI_EN			0
#define LANGUAGE_UI_CN			1
extern int get_sys_language_option();		// MtnTesterEntry.cpp
extern int set_sys_language_option(int iFlag);

// 3. definition for servo position, speed 
//// Structer for speed profile, Servo and Stepper
typedef struct {
	unsigned int uiProfileType;
	double dMaxVelocity;
	double dMaxAcceleration;
	double dMaxDeceleration;
	double dMaxJerk;
	double dMaxKillDeceleration;
	double dStepperStartVelocity;
	double dStepperDriveVelocity;
} MTN_SPEED_PROFILE;

typedef struct {
	double dRelaxPosition;  // 20090129
	double dPositiveLimit;
	double dNegativeLimit;
	double dHomeMode;
	double dHomeType;
	double dHomeP0_Dir;
	double dHomeP0_Vel;
	double dHomeP1_Dir;
	double dHomeP1_Vel;
	double dHomeP2_Dir;
	double dHomeP2_Vel;
	double dHomeOffset;
} MTN_POSITION_REGISTRATION;

// 4. System password protection level
#define __SYSTEM_PROTECTING_OPERATOR__  0
#define __SYSTEM_PROTECTING_PROCESS__  1
#define __SYSTEM_PROTECTING_SERVICE__  2
#define __SYSTEM_PROTECTING_ENGINEER__  3
char get_sys_pass_protect_level();
void set_sys_pass_protect_level(char cSysPassLevel);


// 5. Force Calibration and Teach Contact  
typedef struct {
	unsigned int uiTotalPoints;
	double dUpperPosnLimit;
	double dLowerPosnLimit;
	double dPositionFactor;
	double dCtrlOutOffset;
	unsigned int uiSleepInterAction_ms;
	unsigned int uiGetStartingDateLen;
	int iDebug;
	char strDataFileName[MTN_API_MAX_STRLEN_FILENAME];
}MTN_TUNE_POSN_COMPENSATION;

typedef struct {
	double dPositionFactor;
	double dCtrlOutOffset;
	int iFactor_SP_ACS;
	int iGain_SP_ACS;
	int iOffset_SP_ACS;
	int iFlagEnable;
	int iEncoderOffsetSP;
}MTUNE_OUT_POSN_COMPENSATION;

/// Teach contact
typedef struct
{
	int iAxis; // AXIS=4  !"A" axis 
	int iSearchHeightPosition;		// DISTANCE=-3800   ! Will be used for distance till starting the search for contact
	int iStartVel;		//START_VEL= -20000   ! Velocity till starting the search for contact
	int iSearchVel;	    //SEARCH_VEL=-5000 ! Search velocity 
	int iMaxAccMoveSrchHt;
	int iMaxJerkMoveSrchHt;
	int iResetPosition;  // TARGET_POINT= 0, absolute position after receiving the contact
	int iAntiBounce ;	//ANTIBOUNCE=10 ! Antibouncing in msec for the contact contact
	int iMaxDist;		//MAX_DIST= -7000 ! Maximum distance in case that no contact was received
	int iSearchTolPE;  // SEARCH_TOL_PE = 20
	int iTimeOut;
	int iFlagSwitchToForceControl; // FLAG whether to switch to force control after searching contact
	MTN_SPEED_PROFILE stSpeedProfileTeachContact;
	int cFlagDetectionSP;  // 20121030
	int iFlagDebug;
}TEACH_CONTACT_INPUT;

typedef struct
{
	int iContactPosnReg;
	int iStatus;	// 0: OK
					// 1: TIME_OUT
					// 2: DISTANCE_OUT
					// 3: MOTOR_ERROR
	double dInitForceCommandReadBack;
}TEACH_CONTACT_OUTPUT;

//// Search Contact then switch to Force control
#define MAX_FORCE_BLK  32
#define MAX_FORCE_SEG  16

typedef struct
{
	unsigned int uiNumSegment;
	int aiForceBlk_Rampcount[MAX_FORCE_SEG];
	int aiForceBlk_LevelCount[MAX_FORCE_SEG];
	double adForceBlk_LevelAmplitude[MAX_FORCE_SEG];
	double dPreImpForce_DCOM;
	int iInitForceHold_cnt;
}FORCE_BLOCK_ACS;

typedef struct
{
	TEACH_CONTACT_INPUT *stpTeachContactPara;
	FORCE_BLOCK_ACS *stpForceBlkPara;
}SEARCH_CONTACT_AND_FORCE_CONTROL_INPUT;

typedef struct
{
	unsigned int uiNumSegment;
	double dStepDCOM;
	double dFirstForce_gram;
	double dLastForce_gram;
	double dPreImpForce_DCOM;
	double dPreImpactForce_gram;
	int iInitForceHold_cnt;
	double dDefForceFactor_Kf;
	double dDefForceOffset_I0;
	double dDefPosnFactor_Ka;
	unsigned int uiNumPointsSmoothing;
	double dPreImpForce_gram;
}FORCE_CALI_PARAMETER;

typedef struct
{
	TEACH_CONTACT_INPUT  stTeachContactPara;
	FORCE_CALI_PARAMETER stForceCaliPara;
	MTN_TUNE_POSN_COMPENSATION stPosnCompensate;
	int iFlagDebug;
}FORCE_CALIBRATION_INPUT;


//#ifdef __cplusplus
//}
//#endif

 #endif //__MTN_DEFINE__