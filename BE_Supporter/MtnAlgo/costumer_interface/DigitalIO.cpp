// DigitalIO.cpp : implementation file
/*
History 
07 Oct, 2008 			--- Zhang Dong  @1
- Port from MotAlgo Library
08 Oct, 2008			--- Zhengyi @2
- Add functions to get private variables
09 Aug, 2009			--- Zhengyi 
- Modify card address for USG, @3
- Add accessing cards by handle
*/

#include "stdafx.h"
#include "DigitalIO.h"

// CDigitalIO

CDigitalIO::CDigitalIO()
{
}

CDigitalIO::~CDigitalIO()
{
}

// Initialize IO PCI card

int CDigitalIO::InitializeIoCard(void)
{
	// Initialize PCI I/O Card
    LRESULT     ErrCode; 
    int         nOutEntries;
    WCHAR       strErrMsg[80];

	if ((ErrCode = DRV_DeviceGetNumOfList((SHORT far *)&m_IoCardInfo.sNumOfDevices)) !=
		SUCCESS)
	{
		DRV_GetErrorMessage(ErrCode, (LPSTR)strErrMsg);
	}
	if (m_IoCardInfo.sNumOfDevices > MAX_DEVICES)
		m_IoCardInfo.sNumOfDevices = MAX_DEVICES;
	// retrieve the information of all installed devices
	if ((ErrCode = DRV_DeviceGetList((DEVLIST far *)&m_IoCardInfo.stDeviceList[0],
		(SHORT)m_IoCardInfo.sNumOfDevices, (SHORT far *)&nOutEntries)) != (LONG)SUCCESS)
	{
		DRV_GetErrorMessage(ErrCode,(LPSTR)strErrMsg);
	}

	if(m_IoCardInfo.sNumOfDevices >= 1 && DIGITAL_IO_DEVICE < m_IoCardInfo.sNumOfDevices)
	{
		// Open Device
		ErrCode = DRV_DeviceOpen(m_IoCardInfo.stDeviceList[USG_10H_P2_DO_DEVICE_ID].dwDeviceNum,  // DIGITAL_IO_DEVICE, @2
			(LONG far *)&m_UsgOutputByte.DriverHandle);
		if (ErrCode != SUCCESS)
		{
			//AfxMessageBox(_T("Device open error !"));
		}

		m_UsgOutputByte.stPCIADS_WritePortByte.port = USG_OUTPUT_BYTE_CHANNEL; 
		m_UsgOutputByte.stPCIADS_WritePortByte.mask = 0xFF;

        // Open Device
		ErrCode = DRV_DeviceOpen(m_IoCardInfo.stDeviceList[USG_10H_P2_DO_DEVICE_ID].dwDeviceNum,   // DIGITAL_IO_DEVICE, @2
			(LONG far *)&m_UsgOutPutBit.DriverHandle);
		if (ErrCode != SUCCESS)
		{
			//AfxMessageBox(_T("Device open error !"));
		}

		m_UsgOutPutBit.stPCIADS_WriteBit.port = USG_OUTPUT_BIT_CHANNEL; 
		m_UsgOutPutBit.stPCIADS_WriteBit.bit  = USG_OUTPUT_BIT_ADDRESS;

        // Open Device
		ErrCode = DRV_DeviceOpen(m_IoCardInfo.stDeviceList[DIGITAL_IO_DEVICE].dwDeviceNum,
			(LONG far *)&m_LfPickerForwardValveBit.DriverHandle);
		if (ErrCode != SUCCESS)
		{
			//AfxMessageBox(_T("Device open error !"));
		}

		m_LfPickerForwardValveBit.stPCIADS_WriteBit.port  = CYLINDER_VALVE_BIT_CHANNEL; 
		m_LfPickerForwardValveBit.stPCIADS_WriteBit.bit   = LF_PICKER_FORWARD_VALVE_BIT_ADDRESS;
        m_LfPickerForwardValveBit.stPCIADS_WriteBit.state = false;

        // Open Device
		ErrCode = DRV_DeviceOpen(m_IoCardInfo.stDeviceList[DIGITAL_IO_DEVICE].dwDeviceNum,
			(LONG far *)&m_LfPickerBackwardValveBit.DriverHandle);
		if (ErrCode != SUCCESS)
		{
			//AfxMessageBox(_T("Device open error !"));
		}

		m_LfPickerBackwardValveBit.stPCIADS_WriteBit.port  = CYLINDER_VALVE_BIT_CHANNEL; 
		m_LfPickerBackwardValveBit.stPCIADS_WriteBit.bit   = LF_PICKER_BACKWARD_VALVE_BIT_ADDRESS;
        m_LfPickerBackwardValveBit.stPCIADS_WriteBit.state = false;

        // Open Device
		ErrCode = DRV_DeviceOpen(m_IoCardInfo.stDeviceList[DIGITAL_IO_DEVICE].dwDeviceNum,
			(LONG far *)&m_LfPickerForwardSensorBit.DriverHandle);
		if (ErrCode != SUCCESS)
		{
			//AfxMessageBox(_T("Device open error !"));
		}

		m_LfPickerForwardSensorBit.stPCIADS_ReadBit.port  = CYLINDER_SENSOR_BIT_CHANNEL; 
		m_LfPickerForwardSensorBit.stPCIADS_ReadBit.bit   = LF_PICKER_FORWARD_SENSOR_BIT_ADDRESS;
        m_LfPickerForwardSensorBit.stPCIADS_ReadBit.state = &m_LfPickerForwardSensorBit.unState;

        // Open Device
		ErrCode = DRV_DeviceOpen(m_IoCardInfo.stDeviceList[DIGITAL_IO_DEVICE].dwDeviceNum,
			(LONG far *)&m_LfPickerBackwardSensorBit.DriverHandle);
		if (ErrCode != SUCCESS)
		{
			//AfxMessageBox(_T("Device open error !"));
		}

		m_LfPickerBackwardSensorBit.stPCIADS_ReadBit.port  = CYLINDER_SENSOR_BIT_CHANNEL; 
		m_LfPickerBackwardSensorBit.stPCIADS_ReadBit.bit   = LF_PICKER_BACKWARD_SENSOR_BIT_ADDRESS;
        m_LfPickerBackwardSensorBit.stPCIADS_ReadBit.state = &m_LfPickerBackwardSensorBit.unState;

        // Open Device
		ErrCode = DRV_DeviceOpen(m_IoCardInfo.stDeviceList[DIGITAL_IO_DEVICE].dwDeviceNum,
			(LONG far *)&m_LfPickerBufferSensorBit.DriverHandle);
		if (ErrCode != SUCCESS)
		{
			//AfxMessageBox(_T("Device open error !"));
		}

		m_LfPickerBufferSensorBit.stPCIADS_ReadBit.port  = CYLINDER_SENSOR_BIT_CHANNEL; 
		m_LfPickerBufferSensorBit.stPCIADS_ReadBit.bit   = LF_PICKER_BACKWARD_SENSOR_BIT_ADDRESS;
        m_LfPickerBufferSensorBit.stPCIADS_ReadBit.state = &m_LfPickerBackwardSensorBit.unState;

        // Open Device
		ErrCode = DRV_DeviceOpen(m_IoCardInfo.stDeviceList[DIGITAL_IO_DEVICE].dwDeviceNum,
			(LONG far *)&m_WireClampButtonBit.DriverHandle);
		if (ErrCode != SUCCESS)
		{
			//AfxMessageBox(_T("Device open error !"));
		}

		m_WireClampButtonBit.stPCIADS_ReadBit.port  = KEYPAD_BIT_CHANNEL; 
		m_WireClampButtonBit.stPCIADS_ReadBit.bit   = WIRE_CLAMP_BUTTON_BIT_ADDRESS;
        m_WireClampButtonBit.stPCIADS_ReadBit.state = &m_WireClampButtonBit.unState;

        // Open Device
		ErrCode = DRV_DeviceOpen(m_IoCardInfo.stDeviceList[DIGITAL_IO_DEVICE].dwDeviceNum,
			(LONG far *)&m_AirTensionButtonBit.DriverHandle);
		if (ErrCode != SUCCESS)
		{
			//AfxMessageBox(_T("Device open error !"));
		}

		m_AirTensionButtonBit.stPCIADS_ReadBit.port  = KEYPAD_BIT_CHANNEL; 
		m_AirTensionButtonBit.stPCIADS_ReadBit.bit   = AIR_TENSION_BUTTON_BIT_ADDRESS;
        m_AirTensionButtonBit.stPCIADS_ReadBit.state = &m_AirTensionButtonBit.unState;
    }

    return 0;
}
// Enable Specific interrupt event by DI16 of digital IO card, 20081103
int CDigitalIO::EnableInterruptEvent_DI16(bool bEnable)
{
	LRESULT         ErrCode; 
    WCHAR           strErrMsg[80];
    PT_EnableEvent	EventSetting;

	EventSetting.EventType = ADS_EVT_DI_INTERRUPT16;
	EventSetting.Enabled = bEnable;
	EventSetting.Count = 1;
	ErrCode = DRV_EnableEvent( m_UsgOutputByte.DriverHandle, &EventSetting );
	if ( ErrCode != 0) 
    {
		DRV_GetErrorMessage( ErrCode, (LPSTR)strErrMsg );
		AfxMessageBox((LPCSTR)strErrMsg);
		return (int)ErrCode;
	}
    return 0;
}
// 20081103
int CDigitalIO::EnableInterruptEvent_DI40(bool bEnable)
{
	LRESULT         ErrCode; 
    WCHAR           strErrMsg[80];
    PT_EnableEvent	EventSetting;

	EventSetting.EventType = ADS_EVT_DI_INTERRUPT40;
	EventSetting.Enabled = bEnable;
	EventSetting.Count = 1;
	ErrCode = DRV_EnableEvent( m_UsgOutputByte.DriverHandle, &EventSetting );
	if ( ErrCode != 0) 
    {
		DRV_GetErrorMessage( ErrCode, (LPSTR)strErrMsg );
		AfxMessageBox((LPCSTR)strErrMsg);
		return (int)ErrCode;
	}
    return 0;
}


// Enable interrupt event of digital IO card
int CDigitalIO::EnableInterruptEvent(bool bEnable)
{
	LRESULT         ErrCode; 
    WCHAR           strErrMsg[80];
    PT_EnableEvent	EventSetting;

	EventSetting.EventType = ADS_EVT_INTERRUPT;
	EventSetting.Enabled = bEnable;
	EventSetting.Count = 1;
	ErrCode = DRV_EnableEvent( m_UsgOutputByte.DriverHandle, &EventSetting );
	if ( ErrCode != 0) 
    {
		DRV_GetErrorMessage( ErrCode, (LPSTR)strErrMsg );
		//MessageBox( (LPCSTR)m_szErrMsg, "Driver Message", MB_OK );
		return (int)ErrCode;
	}

    return 0;
}

// Check interrupt event status
int CDigitalIO::CheckInterruptEvent(unsigned short*  pusEventType)
{
    LRESULT    lResult;
	unsigned short  usEventType;
	PT_CheckEvent	ptCheckEvent;

    ptCheckEvent.EventType = &usEventType;
    usEventType = ADS_EVT_INTERRUPT; // ADS_EVT_DI_INTERRUPT16; //65535;

    lResult = DRV_CheckEvent(m_UsgOutputByte.DriverHandle, &ptCheckEvent );

    *pusEventType = usEventType;
    
    return (int)lResult;
}

// Write bit to USG output
int CDigitalIO::UsgWriteOutputBit(unsigned short usState)
{
    m_UsgOutPutBit.stPCIADS_WriteBit.state = usState;
	DRV_DioWriteBit(m_UsgOutPutBit.DriverHandle, &m_UsgOutPutBit.stPCIADS_WriteBit);

    return 0;
}

// Write byte data to USG output
int CDigitalIO::UsgWriteOutputByte(unsigned short usState)
{
    m_UsgOutputByte.stPCIADS_WritePortByte.state = usState;
    DRV_DioWritePortByte(m_UsgOutputByte.DriverHandle, &m_UsgOutputByte.stPCIADS_WritePortByte);
    return 0;
}

// @2
USHORT CDigitalIO::usGetUsgCardsChannel()
{
	return m_UsgOutputByte.stPCIADS_WritePortByte.port;
}

void CDigitalIO::GetUsgOutPutBit_port_bit(USHORT *port, USHORT *bit)
{
	*port = m_UsgOutPutBit.stPCIADS_WriteBit.port;
	*bit = m_UsgOutPutBit.stPCIADS_WriteBit.bit;
}

void CDigitalIO::GetCardsName(USHORT usIdxCard, char *strText, UINT uiStrLen)
{
	sprintf_s(strText, uiStrLen, "%s", m_IoCardInfo.stDeviceList[usIdxCard].szDeviceName);
}


// Set output state for bit
int CDigitalIO::SetOutputBitState(short nOutputBitId, short nState)
{
    switch(nOutputBitId)
    {
    case LF_PICKER_FORWARD_VALVE:
        m_LfPickerForwardValveBit.stPCIADS_WriteBit.state = nState;
        DRV_DioWriteBit(m_LfPickerForwardValveBit.DriverHandle, &m_LfPickerForwardValveBit.stPCIADS_WriteBit);
        break;
    case LF_PICKER_BACKWARD_VALVE:
        m_LfPickerBackwardValveBit.stPCIADS_WriteBit.state = nState;
        DRV_DioWriteBit(m_LfPickerBackwardValveBit.DriverHandle, &m_LfPickerBackwardValveBit.stPCIADS_WriteBit);
        break;
    default:
        break;
    }

    return 0;
}

// Get input state for bit
int CDigitalIO::GetInputBitState(short nInputBitId, short *pnState)
{
    switch(nInputBitId)
    {
    case LF_PICKER_FORWARD_SENSOR:
        DRV_DioReadBit(m_LfPickerForwardSensorBit.DriverHandle, &m_LfPickerForwardSensorBit.stPCIADS_ReadBit);
        *pnState = !(m_LfPickerForwardSensorBit.unState);
        break;
    case LF_PICKER_BACKWARD_SENSOR:
        DRV_DioReadBit(m_LfPickerBackwardSensorBit.DriverHandle, &m_LfPickerBackwardSensorBit.stPCIADS_ReadBit);
        *pnState = !(m_LfPickerBackwardSensorBit.unState);
    case LF_PICKER_BUFFER_SENSOR:
        DRV_DioReadBit(m_LfPickerBufferSensorBit.DriverHandle, &m_LfPickerBufferSensorBit.stPCIADS_ReadBit);
        *pnState = !(m_LfPickerBufferSensorBit.unState);
        break;
    case WIRE_CLAMP_BUTTON:
        DRV_DioReadBit(m_WireClampButtonBit.DriverHandle, &m_WireClampButtonBit.stPCIADS_ReadBit);
        *pnState = !(m_WireClampButtonBit.unState);
        break;
    case AIR_TENSION_BUTTON:
        DRV_DioReadBit(m_AirTensionButtonBit.DriverHandle, &m_AirTensionButtonBit.stPCIADS_ReadBit);
        *pnState = !(m_AirTensionButtonBit.unState);
        break;
    default:
        break;
    }

    return 0;
}
