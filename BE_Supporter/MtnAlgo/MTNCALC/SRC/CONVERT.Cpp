/*---------------------------------------------------------------------------
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



 * Module       : convert.c
 *--------------------------------------------------------------------------*/
#include <math.h>
#include "mtncalc.h"

//
// Fs is the motion sampling frequency
// Unit of EncResolution is um
// Unit of Fs is kHz
// For example,
//  a) X-Y stage
//     EncResolution = 0.2 um
//     Fs = 4 kHz
//  b) Indexer
//     EncResolution = 1 um
//     Fs = 2 kHz
//

MOT_ALGO_DBL ConvertTimeToDiscrete(MOT_ALGO_DBL Fs, MOT_ALGO_DBL Tms)
{
    return (ceil(Fs * Tms));
}

/////////////////////////////////////////////////////////////////
// Purpose: Convert Discrete unit to SI unit
/////////////////////////////////////////////////////////////////
// Unit of return value is um
MOT_ALGO_DBL ConvertDToSI(MOT_ALGO_DBL EncResolution, MOT_ALGO_DBL Dist)
{
    return (Dist * EncResolution);
}

// Unit of return value is mm/s
MOT_ALGO_DBL ConvertVToSI(MOT_ALGO_DBL EncResolution, MOT_ALGO_DBL Fs, MOT_ALGO_DBL Vmax)
{
    return (Vmax * EncResolution * Fs);
}

// Unit of return value is m/s^2
MOT_ALGO_DBL ConvertAToSI(MOT_ALGO_DBL EncResolution, MOT_ALGO_DBL Fs, MOT_ALGO_DBL Amax)
{
    return (Amax * EncResolution * Fs * Fs);
}

/////////////////////////////////////////////////////////////////
// 
// Purpose: Convert SI unit to Discrete unit
// 
/////////////////////////////////////////////////////////////////
// Unit of
//      EncResolution is um
//      Dist is um
//      return value is count
MOT_ALGO_DBL ConvertDToDiscrete(MOT_ALGO_DBL EncResolution, MOT_ALGO_DBL Dist)
{
    return (Dist / EncResolution);
}

// Unit of
//      EncResolution is um
//      Fs is kHz
//      Vmax is mm/s
//      return value is count/sample
MOT_ALGO_DBL ConvertVToDiscrete(MOT_ALGO_DBL EncResolution, MOT_ALGO_DBL Fs, MOT_ALGO_DBL Vmax)
{
    return (Vmax / (Fs * EncResolution));
}

// Unit of
//      EncResolution is um
//      Fs is kHz
//      Amax is m/s^2
//      return value is count/sample/sample
MOT_ALGO_DBL ConvertAToDiscrete(MOT_ALGO_DBL EncResolution, MOT_ALGO_DBL Fs, MOT_ALGO_DBL Amax)
{
    return (Amax / (Fs * Fs * EncResolution));
}


short Calc__round(MOT_ALGO_DBL a)
{
    if ((ceil(a) - a) > 0.5f)
    {
       a = ceil(a) - 1.0f;
    }
    else
    {
       a = ceil(a);
    }
    return ((short) a);
}

