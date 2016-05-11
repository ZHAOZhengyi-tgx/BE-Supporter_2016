
// 20090908 Replace Sleep to be high_precision_sleep_ms
#include "stdafx.h"
#include "efo_monitor.h"

#include "WinTiming.h"


CDigitalIO theDigitalIO_DetectEFO;

// Global utilities to check EFO

EFO_Monitor::EFO_Monitor()
{
	m_bRunEFO_MonitorThread = FALSE;
	m_pThreadIntPCI = NULL;
	iStatusEFO =0;
	uiCountTriggerEFO = 0;
	uiCountSuccessEFO = 0;

}

EFO_Monitor::~EFO_Monitor()
{
}

void EFO_Monitor::EFO_CheckStatusByDigitalIO()
{
unsigned short usEventPCI_StatusEFO;

	if ( theDigitalIO_DetectEFO.CheckInterruptEvent(&usEventPCI_StatusEFO) == SUCCESS)
	{   // 
		if(usEventPCI_StatusEFO == ADS_EVT_DI_INTERRUPT40	//ADS_EVT_DI_INTERRUPT16 
			|| usEventPCI_StatusEFO == ADS_EVT_INTERRUPT )
		{
			iStatusEFO |= EFO_STATUS_SUCCESS;
		}
		else if (usEventPCI_StatusEFO== ADS_EVT_DI_INTERRUPT40)
		{
			iStatusEFO |= EFO_STATUS_TRIGGER;
		}
	}

	return;
}

void EFO_Monitor::EFO_prepare_trigger()
{
	uiCountTriggerEFO = 0;
	uiCountSuccessEFO = 0;
	uiTrigCountBeforeTrigger = uiCountTriggerEFO;
	uiSuccessCountBeforeTrigger = uiCountSuccessEFO;
	iStatusEFO =0;

}

int EFO_Monitor::EFO_check_trigger()
{
	int iRet = EFO_NO_TRIGGER; // By default, no trigger

	if(
		(uiCountSuccessEFO - uiSuccessCountBeforeTrigger >= 1)
		||
		((uiCountSuccessEFO + uiCountTriggerEFO 
		- uiTrigCountBeforeTrigger- uiSuccessCountBeforeTrigger) >= 2) 
		)
	{
		iRet = EFO_TRIGGER_AND_GOOD;
	}
	else if( (uiCountTriggerEFO > uiTrigCountBeforeTrigger)
		|| (uiCountSuccessEFO > uiSuccessCountBeforeTrigger))
	{
		iRet = EFO_TRIGGER_NOT_GOOD;
	}

	return iRet;

}
void EFO_Monitor::EFO_reset_status()
{
	iStatusEFO =0;
}

// Routines related with Interrupt Event Thread
UINT EFO_IntEventThreadProc( LPVOID pParam )
{
    EFO_Monitor* pObject = (EFO_Monitor *)pParam;
	return pObject->IntEventThread(); 	
}


void EFO_Monitor::EFO_start_monitor()
{
	theDigitalIO_DetectEFO.InitializeIoCard();

	theDigitalIO_DetectEFO.EnableInterruptEvent(TRUE); // 20081008, do not check OK or not, refer to DigitalIO.cpp
	theDigitalIO_DetectEFO.EnableInterruptEvent_DI16(TRUE);
	theDigitalIO_DetectEFO.EnableInterruptEvent_DI40(TRUE);

	m_pThreadIntPCI = AfxBeginThread(EFO_IntEventThreadProc, this);

	m_pThreadIntPCI->m_bAutoDelete = FALSE;

	m_bRunEFO_MonitorThread = TRUE;
	
	Sleep(1);
}

//void EFO_Monitor::EFO_enable_1739_int(LONG DriverHandle)
//{
//	EnableInterruptEvent_DI40_1739Hdl(DriverHandle, TRUE);
//	EnableInterruptEvent_DI16_1739Hdl(DriverHandle, TRUE);
//}

void EFO_Monitor::EFO_stop_monitor()
{
	DWORD ExitCode ;
	theDigitalIO_DetectEFO.EnableInterruptEvent(FALSE); // 20081008 do not check OK or not, refer to DigitalIO.cpp

	if(m_bRunEFO_MonitorThread)
	{
		WaitForSingleObject(m_pThreadIntPCI->m_hThread, 100);
		delete m_pThreadIntPCI;

		GetExitCodeThread(m_pThreadIntPCI->m_hThread,&ExitCode);
		if(ExitCode==STILL_ACTIVE)
			TerminateThread(m_pThreadIntPCI->m_hThread,ExitCode);

		m_pThreadIntPCI = NULL;
		m_bRunEFO_MonitorThread = FALSE;
	}
}


UINT EFO_Monitor::IntEventThread()
{
	while(m_bRunEFO_MonitorThread)
	{
		EFO_CheckStatusByDigitalIO();
		if(iStatusEFO & EFO_STATUS_TRIGGER)
		{
			uiCountTriggerEFO ++;		// Triggered once	
		}
		if(iStatusEFO & EFO_STATUS_SUCCESS)
		{
			uiCountSuccessEFO ++;   // Success once
		}
		EFO_reset_status();
		Sleep(50);
	}
	return 0;
}

void EFO_Monitor::EFO_resume_monitor()
{
}

void EFO_Monitor::EFO_pause_monitor()
{
}


