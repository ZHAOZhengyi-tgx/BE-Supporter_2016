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

#include "stdafx.h"
#include "EfoMonitor1739U.h"
#include "driver.h"
#include "Event.h"
#include "WinTiming.h"

static PT_DioWriteBit stEfoMonitor1739U_OutputDebugBit;

static PT_DioWriteBit stEfoMonitor1739U_OutputDualFAB_Bit;  // 20121022
static	unsigned int uiCounterSuccessSpark;
static  unsigned int uiCounterTriggerSpark;
// New implementation of EFO-spark GoodNG monitor
// Initialization (Constructor)
// Input:   Driver Card Handler
//			Interrupt address 
// Variable: iSuccessCounter
// Functions:
//			WarmReset
//			GetCurrentCounter
//			StartMonitor
//			CheckLastSparkGoodFlag

// Enable Specific interrupt event by DI16 of digital IO card, interface by handle
int EnableInterruptEvent_DI16_1739Hdl(LONG lDriverHandle, bool bEnable)
{
	LRESULT         ErrCode; 
    WCHAR           strErrMsg[80];
    PT_EnableEvent	EventSetting;

	EventSetting.EventType = ADS_EVT_DI_INTERRUPT16;
	EventSetting.Enabled = bEnable;
	EventSetting.Count = 1;
	ErrCode = DRV_EnableEvent( lDriverHandle, &EventSetting );
	if ( ErrCode != 0) 
    {
		DRV_GetErrorMessage( ErrCode, (LPSTR)strErrMsg );
		AfxMessageBox((LPCSTR)strErrMsg);
		return ErrCode;
	}
    return 0;
}
// 20090811, interface by handle
int EnableInterruptEvent_DI40_1739Hdl(LONG lDriverHandle, bool bEnable)
{
	LRESULT         ErrCode; 
    WCHAR           strErrMsg[80];
    PT_EnableEvent	EventSetting;

	EventSetting.EventType = ADS_EVT_DI_INTERRUPT40;
	EventSetting.Enabled = bEnable;
	EventSetting.Count = 1;
	ErrCode = DRV_EnableEvent( lDriverHandle, &EventSetting );
	if ( ErrCode != 0) 
    {
		DRV_GetErrorMessage( ErrCode, (LPSTR)strErrMsg );
		AfxMessageBox((LPCSTR)strErrMsg);
		return ErrCode;
	}
    return 0;
}
// Enable interrupt event of digital IO card
int EnableInterruptEvent_1739Hdl(LONG lDriverHandle, bool bEnable)
{
	LRESULT         ErrCode; 
    WCHAR           strErrMsg[80];
    PT_EnableEvent	EventSetting;

	EventSetting.EventType = ADS_EVT_INTERRUPT;
	EventSetting.Enabled = bEnable;
	EventSetting.Count = 1;
	ErrCode = DRV_EnableEvent( lDriverHandle, &EventSetting );
	if ( ErrCode != 0) 
    {
		DRV_GetErrorMessage( ErrCode, (LPSTR)strErrMsg );
		//MessageBox( (LPCSTR)m_szErrMsg, "Driver Message", MB_OK );
		return ErrCode;
	}

    return 0;
}
// Routines related with Interrupt Event Thread
UINT efo_int_event_1739u_proc( LPVOID pParam )
{
    EFO_MONITOR_1739U* pObject = (EFO_MONITOR_1739U *)pParam;
	return pObject->EFO_Monitor_1739U_IntEventThread(); 	
}
// 2009Aug26
void EFO_MONITOR_1739U::EFO_Monitor_1739U_SetDebugFlag(char cFlag)
{
	iDebugFlag = cFlag;
}

int EFO_MONITOR_1739U::EFO_Monitor_1739U_GetGebugFlag()
{
	return iDebugFlag;
}
// 20121207
extern unsigned short FbMonitorDlg_get_address_read_EFO_good_ng();
unsigned short usReadByte_EFO_GoodNG_By1739U = 4;
BOOL EFO_MONITOR_1739U::EFO_Monitor_1739U_Init(LONG lDriverHandle)
{
	usReadByte_EFO_GoodNG_By1739U = FbMonitorDlg_get_address_read_EFO_good_ng(); // 20121207

	iDebugFlag = 0;
	stEfoMonitor1739U_OutputDebugBit.port = EFO_MONITOR_1739U_OUT_BIT_CHANNEL;
	stEfoMonitor1739U_OutputDebugBit.bit = EFO_MONITOR_1739U_OUT_BIT_ADDRESS;

	stEfoMonitor1739U_OutputDualFAB_Bit.port = EFO_MONITOR_1739U_OUT_BIT_CHANNEL;     // 20121022
	stEfoMonitor1739U_OutputDualFAB_Bit.bit = EFO_MONITOR_1739U_DUAL_FAB_BIT_ADDRESS; // 20121022

	uiCounterSuccessSpark = 0;
	uiCounterTriggerSpark = 0;
	if(lDriverHandle != NULL)
	{
		lDriverHandle_1739U_EfoMonitor = lDriverHandle;
//		
		uiPrevCounterSuccessSpark = uiCounterSuccessSpark;

		EnableInterruptEvent_1739Hdl(lDriverHandle_1739U_EfoMonitor, TRUE);
		EnableInterruptEvent_DI16_1739Hdl(lDriverHandle_1739U_EfoMonitor, TRUE);
		EnableInterruptEvent_DI40_1739Hdl(lDriverHandle_1739U_EfoMonitor, TRUE);

		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

BOOL EFO_MONITOR_1739U::EFO_StartMonitor_1739U()
{
	if(m_bRunEFO_MonitorThread1739U == FALSE)
	{
		m_bRunEFO_MonitorThread1739U = TRUE;
		m_pThreadInt1739U = AfxBeginThread(efo_int_event_1739u_proc, this, THREAD_PRIORITY_TIME_CRITICAL);
		m_pThreadInt1739U->m_bAutoDelete = FALSE;
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

// Check interrupt event status
int EFO_MONITOR_1739U::EFO_Monitor_Check1739U_IntEvent(unsigned short*  pusEventType)
{
    LRESULT    lResult;
	unsigned short  usEventType;
	PT_CheckEvent	ptCheckEvent;

    ptCheckEvent.EventType = &usEventType;
    usEventType = ADS_EVT_INTERRUPT; // ADS_EVT_DI_INTERRUPT16; //65535;

    lResult = DRV_CheckEvent(lDriverHandle_1739U_EfoMonitor, &ptCheckEvent );

    *pusEventType = usEventType;
    
    return lResult;
}
extern char mtest_fb_check_get_flag_enable_dual_fab();
static LARGE_INTEGER liFreqOS_FbMonitor1739U; 
unsigned int uiTimerFreqFactorMonitor1739U_10KHz = 5;
UINT EFO_MONITOR_1739U::EFO_Monitor_1739U_IntEventThread()
{
	static unsigned short usEventPCI_StatusEFO;
	unsigned int uiCounterLast;
	unsigned int uiCounterCurr;
	
	QueryPerformanceFrequency(&liFreqOS_FbMonitor1739U);
	while(m_bRunEFO_MonitorThread1739U)
	{
		// Trigger EFO 1739U_Card Byte, Bit to 0
		stEfoMonitor1739U_OutputDebugBit.state = 0;
		DRV_DioWriteBit(lDriverHandle_1739U_EfoMonitor, &stEfoMonitor1739U_OutputDebugBit);

		// Sleep less than 1ms
		uiCounterLast = GetPentiumTimeCount_per_100us(liFreqOS_FbMonitor1739U.QuadPart); // timeGetTime();
		uiCounterCurr = uiCounterLast; // timeGetTime();
		
		while((uiCounterCurr - uiCounterLast < uiTimerFreqFactorMonitor1739U_10KHz) 
			||	( (double) uiCounterCurr + UINT_MAX - uiCounterLast < uiTimerFreqFactorMonitor1739U_10KHz 
			     && (double)uiCounterLast + uiTimerFreqFactorMonitor1739U_10KHz <= UINT_MAX
				 )
		  ) // if within uiTimerFreqFactor_10KHz * 0.1mSec 
		{	
			Sleep(0);
			uiCounterCurr = GetPentiumTimeCount_per_100us(liFreqOS_FbMonitor1739U.QuadPart);
		}

		// Trigger EFO 1739U_Card Byte, Bit to 1
		stEfoMonitor1739U_OutputDebugBit.state = 1;
		DRV_DioWriteBit(lDriverHandle_1739U_EfoMonitor, &stEfoMonitor1739U_OutputDebugBit);


		//
		uiCounterTriggerSpark ++;
		Sleep(30);
		EFO_Monitor_1739U_CheckGoodNG();  // 20110803

		//if ( EFO_Monitor_Check1739U_IntEvent(&usEventPCI_StatusEFO) == SUCCESS)
		//{   // 
		//	if(usEventPCI_StatusEFO == ADS_EVT_INTERRUPT) // usEventPCI_StatusEFO == ADS_EVT_DI_INTERRUPT16 || 
		//	{
		//		uiCounterSuccessSpark ++;
		//		// 2009Aug26
		//		if(iDebugFlag)
		//		{
		//			stEfoMonitor1739U_OutputDebugBit.state = 0;
		//			DRV_DioWriteBit(lDriverHandle_1739U_EfoMonitor, &stEfoMonitor1739U_OutputDebugBit);
		//		}
		Sleep(20);
		if(mtest_fb_check_get_flag_enable_dual_fab() == TRUE) // 20121022
		{
			stEfoMonitor1739U_OutputDualFAB_Bit.state = 1 - stEfoMonitor1739U_OutputDualFAB_Bit.state;
			DRV_DioWriteBit(lDriverHandle_1739U_EfoMonitor, &stEfoMonitor1739U_OutputDualFAB_Bit);
		} // 20121022

		//	}
		//	else if(usEventPCI_StatusEFO == ADS_EVT_DI_INTERRUPT40 
		//		|| usEventPCI_StatusEFO == ADS_EVT_INTERRUPT)
		//	{
		//		uiCounterSuccessSpark ++;
		//	}
		//}
		Sleep(50);
	}
	return 0;
}

void EFO_MONITOR_1739U::EFO_StopMonitor_1739U()
{
	EnableInterruptEvent_1739Hdl(lDriverHandle_1739U_EfoMonitor, FALSE);
	EnableInterruptEvent_DI16_1739Hdl(lDriverHandle_1739U_EfoMonitor, FALSE); 
	EnableInterruptEvent_DI40_1739Hdl(lDriverHandle_1739U_EfoMonitor, FALSE);

	if(m_bRunEFO_MonitorThread1739U)
	{
		m_pThreadInt1739U = NULL;
		m_bRunEFO_MonitorThread1739U = FALSE;
	}
}

UINT EFO_MONITOR_1739U::EFO_Monitor_1739U_GetSuccessCounter()
{
	return uiCounterSuccessSpark;
}

UINT EFO_MONITOR_1739U::EFO_Monitor_1739U_GetTriggerCounter()
{
	return uiCounterTriggerSpark;
}

static unsigned short usDigitalIO1739_ReadByteValueEFO_GoodNG;
static unsigned short usBitPattern1739U_EFO_GoodNG = 0x80;
void EFO_MONITOR_1739U::EFO_Monitor_1739U_CheckGoodNG()
{
		static PT_DioReadPortByte ptDioReadPortByteEFO_GoodNG;
		ptDioReadPortByteEFO_GoodNG.port = usReadByte_EFO_GoodNG_By1739U;  // 20121207 // 0x2; // usByteRead1739U_EFO_GoodNG; // 0x4;  // Byte-4, from 0
		ptDioReadPortByteEFO_GoodNG.value = (USHORT far *)&usDigitalIO1739_ReadByteValueEFO_GoodNG;

		DRV_DioReadPortByte(lDriverHandle_1739U_EfoMonitor,
					(LPT_DioReadPortByte)&ptDioReadPortByteEFO_GoodNG);

//		int iBitEFO_GoodNG_By1739 = 2^7;
		if(usDigitalIO1739_ReadByteValueEFO_GoodNG & usBitPattern1739U_EFO_GoodNG)  // bit 7 from 0
		{  // 1: fail
		}
		else
		{  // 0: success
			uiCounterSuccessSpark ++;
		}
}

UINT EFO_MONITOR_1739U::EFO_Monitor_PrepareTriggerCheck()
{
	uiPrevCounterSuccessSpark = uiCounterSuccessSpark;
	// 2009Aug26
	if(iDebugFlag)
	{
		stEfoMonitor1739U_OutputDebugBit.state = 1;
		DRV_DioWriteBit(lDriverHandle_1739U_EfoMonitor, &stEfoMonitor1739U_OutputDebugBit);
	}
	return uiCounterSuccessSpark;
}

BOOL EFO_MONITOR_1739U::EFO_Monitor_CheckGoodOnce()
{
	if((uiCounterSuccessSpark == (uiPrevCounterSuccessSpark + 1)) 
		|| 
		(uiCounterSuccessSpark == 0 && uiPrevCounterSuccessSpark == UINT_MAX) 
		)// Once a good signal is detected, the BSD start_flag is set automatically
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

