/*
History 
07 Oct, 2008 			--- Zhang Dong  @1
- Port from MotAlgo Library
08 Oct, 2008			--- Zhengyi @2
- Add functions to get private variables
31 Oct, 2008			--- Zhengyi @3
- Specify Interrupt Event Type
10 Aug, 2009			--- Zhengyi @4
- Change USG channel and card address
*/

#pragma once
#include "driver.h"
#include "Event.h"

// CDigitalIO command target
#define     MAX_DEVICES                             8
#define     DIGITAL_IO_DEVICE                       0
#define     USG_SUBDEVICE                           0

// Application dependant address allocation
#define		USG_10H_P2_DO_DEVICE_ID					0   // @4 
#define     USG_OUTPUT_BYTE_CHANNEL                 3   // 4  // @4
#define     USG_OUTPUT_BIT_CHANNEL                  0	// 3  // @4
#define     USG_OUTPUT_BIT_ADDRESS                  0	// 7  // @4

#define     CYLINDER_VALVE_BIT_CHANNEL              3
#define     LF_PICKER_FORWARD_VALVE_BIT_ADDRESS     5
#define     LF_PICKER_BACKWARD_VALVE_BIT_ADDRESS    6

#define     CYLINDER_SENSOR_BIT_CHANNEL             2
#define     LF_PICKER_FORWARD_SENSOR_BIT_ADDRESS    7
#define     LF_PICKER_BACKWARD_SENSOR_BIT_ADDRESS   6
#define     LF_PICKER_BUFFER_SENSOR_BIT_ADDRESS     5

#define     KEYPAD_BIT_CHANNEL                      5
#define     WIRE_CLAMP_BUTTON_BIT_ADDRESS           7
#define     AIR_TENSION_BUTTON_BIT_ADDRESS          6

typedef enum {
    LF_PICKER_FORWARD_VALVE, 
    LF_PICKER_BACKWARD_VALVE
};

typedef enum {
    LF_PICKER_FORWARD_SENSOR, 
    LF_PICKER_BACKWARD_SENSOR, 
    LF_PICKER_BUFFER_SENSOR,
    WIRE_CLAMP_BUTTON,
    AIR_TENSION_BUTTON
};

typedef struct
{
    DEVLIST     stDeviceList[MAX_DEVICES];
    DEVLIST     stSubDeviceList[MAX_DEVICES];
    SHORT       sNumOfDevices;
    SHORT       sNumOfSubdevices;  // number of installed devices

}PCI_ADS_CARDS;

typedef struct
{
    LONG        DriverHandle;          // driver handle   = (LONG)NULL
    USHORT      gwDevice;
    USHORT      gwSubDevice;				// device index
    PT_DioWritePortByte stPCIADS_WritePortByte; // DioWritePortByte table

}PCI_ADS_IO_WRITE_BYTE;

typedef struct
{
    LONG        DriverHandle;          // driver handle   = (LONG)NULL
    USHORT      gwDevice;
    USHORT      gwSubDevice;				// device index
    PT_DioWriteBit      stPCIADS_WriteBit;      // DioWriteBit table
}PCI_ADS_IO_WRITE_BIT;

typedef struct
{
    LONG            DriverHandle;          // driver handle   = (LONG)NULL
    USHORT          gwDevice;
    USHORT          gwSubDevice;		   // device index
    USHORT          unState;
    PT_DioReadBit   stPCIADS_ReadBit;      // DioWriteBit table
}PCI_ADS_IO_READ_BIT;

class CDigitalIO : public CObject
{
public:
	CDigitalIO();
	virtual ~CDigitalIO();
private:
    PCI_ADS_IO_WRITE_BYTE  m_UsgOutputByte;
    PCI_ADS_IO_WRITE_BIT   m_UsgOutPutBit;
public:
    PCI_ADS_CARDS    m_IoCardInfo;
    // Initialize IO PCI card
    int InitializeIoCard(void);
    // Enable interrupt event of digital IO card
    int EnableInterruptEvent(bool bEnable);
    // Check interrupt event status
    int CheckInterruptEvent(unsigned short*  pusEventType);
    // Write bit to USG output
    int UsgWriteOutputBit(unsigned short usState);
    // Write byte data to USG output
    int UsgWriteOutputByte(unsigned short usState);

	// EFO¡¡Monitor related

	// @2
	USHORT usGetUsgCardsChannel();
	void GetUsgOutPutBit_port_bit(USHORT *port, USHORT *bit);
	void GetCardsName(USHORT usIdxCard, char *strText, UINT uiStrLen);

	//@3
	int EnableInterruptEvent_DI16(bool bEnable);
	int EnableInterruptEvent_DI40(bool bEnable);


private:
    // Output bit for forward valve of lead frame picker
    PCI_ADS_IO_WRITE_BIT m_LfPickerForwardValveBit;
    // Output bit for backward valve of lead frame picker
    PCI_ADS_IO_WRITE_BIT m_LfPickerBackwardValveBit;
    // Input bit for forward sensor of lead frame picker
    PCI_ADS_IO_READ_BIT m_LfPickerForwardSensorBit;
    // Input bit for backward sensor of lead frame picker
    PCI_ADS_IO_READ_BIT m_LfPickerBackwardSensorBit;
public:
    // Set output state for bit
    int SetOutputBitState(short nOutputBitId, short nState);
    // Get input state for bit
    int GetInputBitState(short nInputBitId, short *pnState);
private:
    // Input bit for buffer sensor of lead frame picker
    PCI_ADS_IO_READ_BIT m_LfPickerBufferSensorBit;
    // Input bit for wire clamp buttom
    PCI_ADS_IO_READ_BIT m_WireClampButtonBit;
    // Input bit for air tension button
    PCI_ADS_IO_READ_BIT m_AirTensionButtonBit;
public:

};

extern CDigitalIO theDigitalIO;
