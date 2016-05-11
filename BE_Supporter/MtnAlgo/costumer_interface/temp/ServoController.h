#pragma once
#include "DataStruct.h"

typedef enum {MOTION_ABSOLUTE, MOTION_RELATIVE} enMotionMode;
typedef enum {MOTION_WITH_WAIT, MOTION_WITHOUT_WAIT} enSyncMode;
typedef enum {MOTION_NEGATIVE = -1, MOTION_POSITIVE = 1} enMotionDir;
typedef enum {
    SPEED_GENERAL_SET, 
    SPEED_BOND_SET1, //Search height 
    SPEED_BOND_SET2, //Reverse height
    SPEED_BOND_SET3, //Loop top
    SPEED_BOND_SET4, //Trajectory
    SPEED_BOND_SET5, //Tail
    SPEED_BOND_SET6  //Reset level
} enSpeedSetting;

typedef enum {SERVO_VAR_TRAJ, SERVO_VAR_FIRST_MOTION, SERVO_VAR_SRCH_HT, SERVO_VAR_RESET};
typedef enum {
    WCL_BALL_SIT_OPEN_CLOSE, 
    WCL_SEARCH_HEIGHT_OPEN, 
    WCL_PEAK_HEIGHT_CLOSE, 
    WCL_TAIL_HEIGHT_CLOSE,
    WCL_THREAD_WIRE_OPEN,
    WCL_THREAD_WIRE_CLOSE,
    WCL_HOLD_WIRE_CLOSE,
    WCL_MAX_PROFILE
};

#define WCL_OPEN_TIME            4000   //4000ms

typedef enum {SERVO_BUFFER_0, SERVO_BUFFER_1, SERVO_BUFFER_2};

#define SYS_TBL_RESOL 0.5
#define SYS_BH_RESOL  1.0
#define SYS_TBL_X_POSITIVE_LIMIT  200000
#define SYS_TBL_X_NEGTIVE_LIMIT  -200000
#define SYS_TBL_Y_POSITIVE_LIMIT   35000
#define SYS_TBL_Y_NEGTIVE_LIMIT   -35000

#define TBL_MAX_ACC      5.0E7     //enc/s2 2.5g
#define TBL_MAX_VEL      2.0E6     //enc/s  1m/s
#define BH_MAX_ACC       9.0E8     //enc/s2 90g
#define BH_MAX_VEL       1.0E6     //enc/s  1m/s

#define SERVO_SAMPLE_RATE 4000.0   //4K
#define TABLE_LONG_DIST   10000.0  //10mm

#define SERVO_TIMEOUT     50000      //500ms

typedef enum
{
    DPRAM_ADDR_START_BOND0   = 0x84, //Trigger buffer 0 start bonding
    DPRAM_ADDR_BOND_PROGRESS = 0x88, //Report bond progress to PC
    DPRAM_ADDR_VISION_STATUS = 0x8C, //Inform controller vision status
    DPRAM_ADDR_USG_PATTERN   = 0x90, //USG trigger pattern from controller
    DPRAM_ADDR_START_BOND1   = 0x94, //Trigger buffer 1 start bonding
    DPRAM_ADDR_BOND_STATUS   = 0x98, //Inform controller bond error status
    DPRAM_ADDR_RESTART_BOND  = 0x9C, //Trigger restart bonding
};

// CServoController command target

class CServoController : public CObject
{
private:
	HANDLE m_hCommHandle;
    bool   m_bOperationMode;
public:
	CServoController();
	virtual ~CServoController();
	int Initialization(bool bOperationMode);
	// Move distance for Axis X
	int MoveAxisX(long lDistance, short nMotionMode, short nWait);
	// Move distance for Axis Y
	int MoveAxisY(long lDistance, short nMotionMode, short nWait);
	// Move distance for Axis Z
	int MoveAxisZ(long lDistance, short nMotionMode, short nWait);
	// Disable all servo controllers for power off
	int DisableServoController(void);
	int StartJogX(short nJogDirection);
	int StartJogY(short nJogDirection);
	int StartJogZ(short nJogDirection);
	int EndJogX(void);
	int EndJogY(void);
	int EndJogZ(void);
private:
	// Table Jog speed encoder/second
	double dJogSpeed;
    double dTblXPosLimit;
    double dTblXNegLimit;
    double dTblYPosLimit;
    double dTblYNegLimit;
    SERVO_SPEED_PROFILE m_stServoSpeedProfile;
    WIRE_CLAMP_PROFILE  m_astWireClampProfile[WCL_MAX_PROFILE];
public:
	// Adjust jog speed to 2 times
	int JogSpeedUp(void);
	// Adjust jog speed to half
	int JogSlowDown(void);
	// Run program buffer of servo controller
	int RunServoProgram(short nBuffer);
    // Read table command position
    int GetTablePosition(TBL_POS* pstTblPos);
    // Get resolution of XY table
    double GetTableResolution(void);
    // Get resolution of bond head
    double GetBondHeadResolution(void);
    // Get XY table mechanical limit
    int GetTableMechLimit(double* pdXPosLmt, double* pdXNegLmt, double* pdYPosLmt, double* pdYNegLmt);
    // Stop program buffer of servo controller
    int StopServoProgram(short nBuffer);
    // Trigger bond buffer via DPRAM
    int TriggerBondBuffer(short nBuffer);
    // Send integer data to servo
    int SendIntDataToServo(int *palIntData, short nDataLength, short nBuffer);
    // Set trigger signal for restart wire to servo.
    int TriggerRestartWire(bool bEnable);
    // Send real data to servo
    int SendRealDataToServo(double *padRealData, short nDataLength, short nBuffer, short nServoVar);
    // Get speed profile including Acc, Vel in enc and sample
    int GetSpeedProfile(short nMotor, short nSpeedSet, double *pdMaxAcc, double *pdMaxVel);
    // Get bond progress
    int GetBondProgress(int *plBondProgress);
    // Get the position error
    int GetPositionError(short nMotor, double* pdPosError);
    // Read bond head command position
    int GetBondHeadPosition(double* pdPosition);
    // Reset bond progress
    int ResetBondProgress(void);
    // Read integer data from Dpram
    int ReadDpramInteger(short nDpramAddr, int *plReadResult);
    // Write integer data to Dpram
    int WriteDpramInteger(short nDpramAddr, int lReadResult);
    // Move table synchronized
    int SynchroMoveTable(TBL_POS* pstTblPos, short nMotionMode, short nWait);
    // Synchronize motion finish
    int SyncMotion(short nMotor);
    // Get servo speed profile 
    int GetServoSpeedProfile(SERVO_SPEED_PROFILE* pstServoSpeedProfile);
    // Store servo speed profile 
    int StoreServoSpeedProfile(SERVO_SPEED_PROFILE* pstServoSpeedProfile);
    // Save servo speed profile to saving buffer
    int SaveServoSpeedProfile(void);
    // Load servo speed profile from saving buffer
    int LoadServoSpeedProfile(void);
    // Set servo speed profile to servo controller
    int SetServoSpeedProfile(void);
    // Check servo error status
    int CheckServoError(void);
    // Perform force calibration
    int PerformForceCalibration(long* plCtactLvl, double* pdForceFactorKf, double* pdForceFactorKa, double* pdForceFactorIo);
    // Perform force verification
    int PerformForceVerification(short nVerifyForce, double* pdFeedBackForce);
    // Initialize wire clamp default profile
    int InitWireClampProfile(void);
    // Get wire clamp profile
    int GetWireClampProfile(short nProfileIndex, WIRE_CLAMP_PROFILE* pstWireClampProfile);
    // Store wire clamp profile
    int StoreWireClampProfile(short nProfileIndex, WIRE_CLAMP_PROFILE* pstWireClampProfile);
    // Save wire clamp profile to saving buffer
    int SaveWireClampProfile(void);
    // Load wire clamp profile from saving buffer
    int LoadWireClampProfile(void);
    // Initialize wire clamp
    int InitializeWireClamp(void);
    // Update wire clamp profile
    int UpdateWireClampProfile(void);
    // Trigger wire clamp profile
    int TriggerWireClampProfile(short nWclProfile);
    // Apply force control
    int ApplyForceControl(short nForce, short nTime);
private:
    // Dpram lock
    bool m_bDpramLock;
public:
    // Get operation mode
    bool GetOperationMode(void);
    // Get acsc communication handle
    HANDLE GetCommHandle(void);
};

extern CServoController theServoController;

