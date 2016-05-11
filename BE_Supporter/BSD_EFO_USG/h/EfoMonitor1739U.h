

#ifndef __EFO_MONITOR_1739U__
#define __EFO_MONITOR_1739U__

// 20110726
#define EFO_MONITOR_1739U_OUT_BIT_CHANNEL    3
#define EFO_MONITOR_1739U_OUT_BIT_ADDRESS    0

#define EFO_MONITOR_1739U_DUAL_FAB_BIT_ADDRESS    5

class EFO_MONITOR_1739U : public CObject
{
private:
	LONG lDriverHandle_1739U_EfoMonitor;
	unsigned int uiPrevCounterSuccessSpark;
	int iDebugFlag;

public:
	BOOL EFO_Monitor_1739U_Init(LONG lDriverHandle);

	CWinThread*     m_pThreadInt1739U;
	BOOL			m_bRunEFO_MonitorThread1739U;

	BOOL EFO_StartMonitor_1739U();
	void EFO_StopMonitor_1739U();

	UINT EFO_Monitor_1739U_IntEventThread();
	int EFO_Monitor_Check1739U_IntEvent(unsigned short*  pusEventType);
	UINT EFO_Monitor_1739U_GetSuccessCounter();
	UINT EFO_Monitor_1739U_GetTriggerCounter();
	void EFO_Monitor_1739U_CheckGoodNG();
	UINT EFO_Monitor_PrepareTriggerCheck();
	BOOL EFO_Monitor_CheckGoodOnce();
	// 2009Aug26
	void EFO_Monitor_1739U_SetDebugFlag(char cFlag);
	int EFO_Monitor_1739U_GetGebugFlag();
};

#endif  // __EFO_MONITOR_1739U__