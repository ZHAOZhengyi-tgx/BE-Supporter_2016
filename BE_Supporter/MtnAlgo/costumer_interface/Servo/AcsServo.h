#pragma once

typedef enum OPERATE_MODE {ONLINE_MODE, OFFLINE_MODE};

class CAcsServo
{
public:
    CAcsServo(void);
    ~CAcsServo(void);
    // Initialization
    int Initialization(bool bOperationMode);
private:
    // Operation mode: Offline, Online
    bool m_bOperationMode;
    // Servo communication handle
    HANDLE m_hCommHandle;
public:
    // Get servo operation mode
    bool GetServoOperationMode(void);
    // Enable motors
    int EnableMotors(int iMotor1 = -1, int iMotor2 = -1, int iMotor3 = -1, int iMotor4 = -1,
        int iMotor5 = -1, int iMotor6 = -1, int iMotor7 = -1, int iMotor8 = -1);
    // Get acs servo handle
    HANDLE GetServoHandle(void);
    // Read integer data from Dpram
    int ReadDpramInteger(short nDpramAddr, int *plReadResult);
    // Write integer data to Dpram
    int WriteDpramInteger(short nDpramAddr, int lReadResult);
};

extern CAcsServo theAcsServo;