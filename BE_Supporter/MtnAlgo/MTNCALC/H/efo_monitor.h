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


//#if _MSC_VER >= 1000
#pragma once
//#endif // _MSC_VER >= 1000
#include "DigitalIO.h"  // 20081030




#define EFO_STATUS_TRIGGER   0x1
#define EFO_STATUS_SUCCESS   0x2

#define EFO_NO_TRIGGER  0
#define EFO_TRIGGER_NOT_GOOD  1
#define EFO_TRIGGER_AND_GOOD  2

class EFO_Monitor : public CObject
{
public:
	EFO_Monitor();
	virtual ~EFO_Monitor();
	
	void EFO_start_monitor();
	void EFO_stop_monitor();
	void EFO_resume_monitor();
	void EFO_pause_monitor();
	void EFO_reset_status();
	void EFO_prepare_trigger();
	int EFO_check_trigger();

	UINT IntEventThread();
	int iStatusEFO;
	unsigned int uiCountTriggerEFO;
	unsigned int uiCountSuccessEFO;
	unsigned int uiTrigCountBeforeTrigger;
	unsigned int uiSuccessCountBeforeTrigger;

	void EFO_enable_1739_int(LONG lDriverHandle1739);


private:
	// iStatusEFO = 0 : Fail
	//         = 1 : Triggered but not success
	//         = 2 : Abnormal success
	//         = 3 : Trigger and success
	BOOL m_bRunEFO_MonitorThread;

	void EFO_CheckStatusByDigitalIO();
	CWinThread*     m_pThreadIntPCI;
};

