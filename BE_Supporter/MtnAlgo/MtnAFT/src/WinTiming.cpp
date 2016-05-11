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


// History
// YYYYMMDD  Author			Notes
// 20081006  Zhengyi        Created pseudo-code from ACS controller buffer program
// 20090508 Zhengyi Add Checking Time Out 
#include "stdafx.h"
//#include "math.h"
#include <windows.h>
#include <Mmsystem.h>
#include "WinTiming.h"
#include <Limits.h>
//static union
//{
//	LARGE_INTEGER; 
//	LONGLONG
//}liFreqOS_WinTiming = (LARGE_INTEGER)573571072;

LARGE_INTEGER liFreqOS_WinTiming = {(LONGLONG)573571072};

// 573571072, // 573,571,072 Hz, Assume AMDTurion 64x2

void init_win_timing()
{
	// Get OS Frequency
	timeBeginPeriod(1);
	QueryPerformanceFrequency(&liFreqOS_WinTiming); 
//	GetPentiumClockEstimateFromRegistry((unsigned __int64 *)&liFreqOS_WinTiming);
}

void exit_win_timing()
{
	timeEndPeriod(1);
}

void high_precision_sleep_ms(unsigned int uiWait_ms)
{
        // note: BE SURE YOU CALL timeBeginPeriod(1) at program startup!!!
        // note: BE SURE YOU CALL timeEndPeriod(1) at program exit!!!
        // note: that will require linking to winmm.lib
        // note: never use static initializers (like this) with Winamp plug-ins!
        static LARGE_INTEGER m_prev_end_of_frame; // = 0;  
        QueryPerformanceCounter(&m_prev_end_of_frame);
        // = 60;
        
        LARGE_INTEGER t;

        if (m_prev_end_of_frame.QuadPart != 0)
        {
            int ticks_to_wait = (int)(liFreqOS_WinTiming.QuadPart *uiWait_ms / 1000); // / max_fps;
            int done = 0;
            do
            {
                QueryPerformanceCounter(&t);
                
                int ticks_passed = (int)((__int64)t.QuadPart - (__int64)m_prev_end_of_frame.QuadPart);
                int ticks_left = ticks_to_wait - ticks_passed;

                if( (t.QuadPart < m_prev_end_of_frame.QuadPart) && 
					(((double)t.QuadPart + UINT_MAX - ticks_to_wait) > (double)(m_prev_end_of_frame.QuadPart))    // time wrap
				   )
                    done = 1;
                if (ticks_passed >= ticks_to_wait)
                    done = 1;
                
                if (!done)
                {
                    // if > 0.002s left, do high_precision_sleep_ms(1), which will actually sleep some 
                    //   steady amount, probably 1-2 ms,
                    //   and do so in a nice way (cpu meter drops; laptop battery spared).
                    // otherwise, do a few high_precision_sleep_ms(0)'s, which just give up the timeslice,
                    //   but don't really save cpu or battery, but do pass a tiny
                    //   amount of time.
                    if (ticks_left > (int)(liFreqOS_WinTiming.QuadPart*15/1000))
					{
						Sleep(10);
					}
					else if (ticks_left > (int)(liFreqOS_WinTiming.QuadPart*2/1000))
					{
                        Sleep(1);
					}
                    else                        
					{
                        for (int i=0; i<10; i++) 
                            Sleep(0);  // causes thread to give up its timeslice
					}
                }
            }
            while (!done);            
        }

//        m_prev_end_of_frame = t;

}

int int_round_double(double dIn)
{
	// consider overflow;
	if( dIn >= INT_MAX)
		return INT_MAX;
	else if(dIn <= INT_MIN)
		return INT_MIN;
	else
	{
		int iRet;
		iRet = (int)dIn;
		if( (dIn - (double)iRet) > 0.5 )
		{
			return iRet + 1;
		}
		else
		{
			return iRet;
		}
	}
}

unsigned int uint_round_double(double dIn)
{
	// consider overflow;
	if( dIn >= UINT_MAX - 1)
		return UINT_MAX;
	else
	{
		unsigned int iRet;
		iRet = (unsigned int)(dIn + 0.5);
		return iRet;
	}
}


double GetPentiumTimeAsDouble(unsigned __int64 frequency)
{
	// returns < 0 on failure; otherwise, returns current cpu time, in seconds.
	// warning: watch out for wraparound!
	
	if (frequency==0)
        return -1.0;

    // get high-precision time:
    __try
    {
        unsigned __int64 high_perf_time;
        unsigned __int64 *dest = &high_perf_time;
        __asm 
        {
            _emit 0xf        // these two bytes form the 'rdtsc' asm instruction,
            _emit 0x31       //  available on Pentium I and later.
            mov esi, dest
            mov [esi  ], eax    // lower 32 bits of tsc
            mov [esi+4], edx    // upper 32 bits of tsc
        }
        __int64 time_s     = (__int64)(high_perf_time / frequency);  // unsigned->sign conversion should be safe here
        __int64 time_fract = (__int64)(high_perf_time % frequency);  // unsigned->sign conversion should be safe here
        // note: here, we wrap the timer more frequently (once per week) 
        // than it otherwise would (VERY RARELY - once every 585 years on
        // a 1 GHz), to alleviate floating-point precision errors that start 
        // to occur when you get to very high counter values.  
        double ret = (time_s % (60*60*24*7)) + (double)time_fract/(double)((__int64)frequency);
        return ret;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return -1.0;
    }

    return -1.0;
}

int GetPentiumTimeCount_per_10us(unsigned __int64 frequency)
{
	// returns < 0 on failure; otherwise, returns current cpu time, in seconds.
	// warning: watch out for wraparound!
	
	if (frequency==0)
        return -1;

    // get high-precision time:
    __try
    {
        unsigned __int64 high_perf_time;
        unsigned __int64 *dest = &high_perf_time;
        __asm 
        {
            _emit 0xf        // these two bytes form the 'rdtsc' asm instruction,
            _emit 0x31       //  available on Pentium I and later.
            mov esi, dest
            mov [esi  ], eax    // lower 32 bits of tsc
            mov [esi+4], edx    // upper 32 bits of tsc
        }
        __int64 time_s     = (__int64)(high_perf_time / frequency);  // unsigned->sign conversion should be safe here
        __int64 time_fract = (__int64)(high_perf_time % frequency);  // unsigned->sign conversion should be safe here
        // note: here, we wrap the timer more frequently (once per week) 
        // than it otherwise would (VERY RARELY - once every 585 years on
        // a 1 GHz), to alleviate floating-point precision errors that start 
        // to occur when you get to very high counter values.  
        int ret = (int)((double)time_fract/(double)((__int64)frequency) * COUNT_PER_SEC_FOR_10US);
        return ret;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return -1;
    }

    return -1;
}


int GetPentiumTimeCount_per_100us(unsigned __int64 frequency)
{
	// returns < 0 on failure; otherwise, returns current cpu time, in seconds.
	// warning: watch out for wraparound!
	
	if (frequency==0)
        return -1;

    // get high-precision time:
    __try
    {
        unsigned __int64 high_perf_time;
        unsigned __int64 *dest = &high_perf_time;
        __asm 
        {
            _emit 0xf        // these two bytes form the 'rdtsc' asm instruction,
            _emit 0x31       //  available on Pentium I and later.
            mov esi, dest
            mov [esi  ], eax    // lower 32 bits of tsc
            mov [esi+4], edx    // upper 32 bits of tsc
        }
        __int64 time_s     = (__int64)(high_perf_time / frequency);  // unsigned->sign conversion should be safe here
        __int64 time_fract = (__int64)(high_perf_time % frequency);  // unsigned->sign conversion should be safe here
        // note: here, we wrap the timer more frequently (once per week) 
        // than it otherwise would (VERY RARELY - once every 585 years on
        // a 1 GHz), to alleviate floating-point precision errors that start 
        // to occur when you get to very high counter values.  
		double dRet = ((double)time_fract/(double)((__int64)frequency) * COUNT_PER_SEC_FOR_100US);
        unsigned int ret = uint_round_double(dRet);
        return ret;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return -1;
    }

    return -1;
}

int GetPentiumTimeCount_per_1s(unsigned __int64 frequency)
{
	// returns < 0 on failure; otherwise, returns current cpu time, in seconds.
	// warning: watch out for wraparound!
	
	if (frequency==0)
        return -1;

    // get high-precision time:
    __try
    {
        unsigned __int64 high_perf_time;
        unsigned __int64 *dest = &high_perf_time;
        __asm 
        {
            _emit 0xf        // these two bytes form the 'rdtsc' asm instruction,
            _emit 0x31       //  available on Pentium I and later.
            mov esi, dest
            mov [esi  ], eax    // lower 32 bits of tsc
            mov [esi+4], edx    // upper 32 bits of tsc
        }
        __int64 time_s     = (__int64)(high_perf_time / frequency);  // unsigned->sign conversion should be safe here
//        __int64 time_fract = (__int64)(high_perf_time % frequency);  // unsigned->sign conversion should be safe here
        // note: here, we wrap the timer more frequently (once per week) 
        // than it otherwise would (VERY RARELY - once every 585 years on
        // a 1 GHz), to alleviate floating-point precision errors that start 
        // to occur when you get to very high counter values.  
//		double dRet = ((double)time_fract/(double)((__int64)frequency) * COUNT_PER_SEC_FOR_100US);
        unsigned int ret = (int)time_s;
        return ret;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return -1;
    }

    return -1;
}
// 20090508 Modules for checking time out-- cto
static unsigned int uiTimeOutCntStart;
static unsigned int uiTimeCountCurr;

unsigned int mtn_cto_get_start_cnt()
{
	return uiTimeOutCntStart;
}

unsigned int mtn_cto_get_curr_cnt()
{
	return uiTimeCountCurr;
}

void mtn_cto_tick_start_time_u1s(LARGE_INTEGER liFreqOperSys)
{
	uiTimeOutCntStart = GetPentiumTimeCount_per_1s(liFreqOperSys.QuadPart); 
}

bool mtn_cto_is_time_out_u1s(LARGE_INTEGER liFreqOperSys, unsigned int uiTimeOut_u1s) 
{
	return FALSE;

#ifdef __USED__
	uiTimeCountCurr = GetPentiumTimeCount_per_1s(liFreqOperSys.QuadPart);
	if( (uiTimeCountCurr - uiTimeOutCntStart > uiTimeOut_u1s)
		||	(( (double) uiTimeCountCurr + UINT_MAX - (double)uiTimeOutCntStart >= uiTimeOut_u1s )
				&& ((double)uiTimeOutCntStart + uiTimeOut_u1s > (double)UINT_MAX)
		 )
	 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
#endif // __USED__
}  // 20090508

// a pair of functions to Check Time Out(CTO)
void mtn_cto_tick_start_time_u100us(LARGE_INTEGER liFreqOperSys)
{
	uiTimeOutCntStart = GetPentiumTimeCount_per_100us(liFreqOperSys.QuadPart); 
}

bool mtn_cto_is_time_out(LARGE_INTEGER liFreqOperSys, unsigned int uiTimeOut_u100us) 
{
	uiTimeCountCurr = GetPentiumTimeCount_per_100us(liFreqOperSys.QuadPart);
	if( (uiTimeCountCurr - uiTimeOutCntStart > uiTimeOut_u100us)
		||	(( (double) uiTimeCountCurr + UINT_MAX - (double)uiTimeOutCntStart >= uiTimeOut_u100us )
				&& ((double)uiTimeOutCntStart + uiTimeOut_u100us > (double)UINT_MAX)
		 )
	 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}  // 20090508

// WARNING: YOU DON'T REALLY WANT TO USE THIS FUNCTION
bool GetPentiumClockEstimateFromRegistry(unsigned __int64 *frequency) 
{ 
	HKEY                        hKey; 
	DWORD                       cbBuffer; 
	LONG                        rc; 
	
	*frequency = 0;
	rc = RegOpenKeyEx( 
		HKEY_LOCAL_MACHINE, 
		_T("Hardware\\Description\\System\\CentralProcessor\\0"), 
		0, 
		KEY_READ, 
		&hKey 
		); 
	if (rc == ERROR_SUCCESS) 
	{ 
		cbBuffer = sizeof (DWORD); 
		DWORD freq_mhz;
		rc = RegQueryValueEx 
			( 
			hKey, 
			_T("~MHz"), 
			NULL, 
			NULL, 
			(LPBYTE)(&freq_mhz), 
			&cbBuffer 
			); 
		if (rc == ERROR_SUCCESS)
			*frequency = freq_mhz*1024*1024;
		RegCloseKey (hKey); 
	} 
	return (*frequency > 0); 
}
