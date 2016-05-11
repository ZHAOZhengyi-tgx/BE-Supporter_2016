#ifndef _ADLINK_ERROR_CODE_DEF_H
#define _ADLINK_ERROR_CODE_DEF_H

#define ERR_NoError						(I16)0		//No Error	

// System Error ( -1 ~ -1000 )
#define ERR_OSVersion					(I16)-1		// Operation System type mismatched
#define ERR_OpenDriverFailed			(I16)-2		// Open device driver failed - Create driver interface failed
#define ERR_InsufficientMemory			(I16)-3		// System memory insufficiently
#define ERR_DeviceNotInitial			(I16)-4		// Cards not be initialized
#define ERR_NoDeviceFound				(I16)-5		// Cards not found(No card in your system)
#define ERR_CardIdDuplicate				(I16)-6		// Cards' ID Number duplicate 
#define ERR_DeviceAlreadyInitialed		(I16)-7		// Cards have been initialed 
#define ERR_InterruptNotEnable			(I16)-8		// Cards' interrupt events not enable
#define ERR_TimeOut						(I16)-9		// Function time out	
#define ERR_ParametersInvalid			(I16)-10	// Function input parameters are invalid
#define ERR_SetEEPROM					(I16)-11	// Set data to EEPROM failed
#define ERR_GetEEPROM					(I16)-12	// Get data from EEPROM failed
#define ERR_MotionBusy					(I16)-13	// Motion in operation

// Win32 API Error
#define ERR_Win32Error					(I16)-1000	// Check Win32 ErrorCode define [ win32 error code = -( returnCode + ERR_Win32Error) ]

// Warning
#define WAR_AccDecTimeTooLarge			(I16)1		// Tacc/Tdec's value is too large
#define WAR_AccDecTimeTooSmall			(I16)2		// Tacc/Tdec's value is too small
#define WAR_RampDownPointExceed			(I16)3		// Reserved
#define WAR_DistanceTooShort			(I16)4		// Distance is too short

#endif  //_ADLINK_ERROR_CODE_DEF_H
