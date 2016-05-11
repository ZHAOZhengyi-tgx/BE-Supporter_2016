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

// 
#ifndef _WIN_TIMING_
#define _WIN_TIMING_


#define COUNT_PER_SEC_FOR_10US       (100000)
#define COUNT_PER_SEC_FOR_100US       (10000)

extern void init_win_timing();
extern void exit_win_timing();
void high_precision_sleep_ms(unsigned int uiWait_ms);
unsigned int uint_round_double(double dIn);
bool GetPentiumClockEstimateFromRegistry(unsigned __int64 *frequency);

double GetPentiumTimeAsDouble(unsigned __int64 frequency);

int GetPentiumTimeCount_per_10us(unsigned __int64 frequency);

int GetPentiumTimeCount_per_100us(unsigned __int64 frequency);

int GetPentiumTimeCount_per_1s(unsigned __int64 frequency);

unsigned int mtn_cto_get_start_cnt();

unsigned int mtn_cto_get_curr_cnt();

void mtn_cto_tick_start_time_u1s(LARGE_INTEGER liFreqOperSys);

bool mtn_cto_is_time_out_u1s(LARGE_INTEGER liFreqOperSys, unsigned int uiTimeOut_u1s);

void mtn_cto_tick_start_time_u100us(LARGE_INTEGER liFreqOperSys);

bool mtn_cto_is_time_out(LARGE_INTEGER liFreqOperSys, unsigned int uiTimeOut_u100us);

#endif  // _WIN_TIMING_