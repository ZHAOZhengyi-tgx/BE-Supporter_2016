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

#ifndef __ROYA_DEF_H__
#define __ROYA_DEF_H__

#define ADC_2_BYTE

#ifdef  ADC_2_BYTE
typedef unsigned short ADC_TYPE;
#else
typedef unsigned int ADC_TYPE;
#endif

typedef struct
{
unsigned int  uiCounterKeyInt;
unsigned int  uiCounterKeyBSD;
unsigned int  uiWireFeedSensorTriggerCounter;
}KEY_INTERRUPT_COUNTER;

typedef struct
{
    unsigned int nCycleISR;
	unsigned int nRefSignalCycle;
	double dFreqISR_Hz;
}SIG_GEN_SINE_OUT_CFG;

#define ROYA_BSD_ADC_2_BYTE    0
#define ROYA_BSD_ADC_4_BYTE    1

#define ROYA_BSD_DAC_10b       10
#define ROYA_BSD_DAC_12b       12

#define ROYA_BSD_YEAR_2_BYTE   0
#define ROYA_BSD_YEAR_4_BYTE   1

#define ROYA_BSD_ADC_POLARITY_SINGLE   0
#define ROYA_BSD_ADC_BI_POLAR          1

#define DAC_OFFSET_10BIT    512
#define DAC_OFFSET_12BIT    2048
#define DAC_SIGNED_MAX_10BIT  (DAC_OFFSET_10BIT - 1)
#define DAC_SIGNED_MAX_12BIT  (DAC_OFFSET_12BIT - 1)

#endif // __ROYA_DEF_H__

