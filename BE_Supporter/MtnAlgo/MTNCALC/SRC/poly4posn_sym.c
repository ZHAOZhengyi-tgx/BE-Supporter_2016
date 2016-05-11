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


typedef struct
{
	double dDist_m;  // p
	double dMaxVel_si;
	double dMaxAcc_si; 
	double dMaxJerk_si;
	double dMaxDerJerk_si;
	double dSampleTime_s;  // <=0: Continuoust time
	double dPosnUnit_m;
	double dNumDecimal;  // 

}CMDGEN_POLY4POSN_PRECAL_INPUT;

typedef struct
{
	double dTimeConstDerJerk;
	double dTimeConstJerk;
	double dTimeConstAcc;
	double dTimeConstVel;
}CMDGEN_POLY4POSN_PRECAL_OUTPUT;

/// function mtn_api_poly4posn_precal
/// function [t,dd]=make4(varargin)

//// [t,dd] = make4(p,v,a,j,d,Ts,r,s)
////
//// Calculate timing for symmetrical 4th order profiles. 
////
//// inputs:
////      p    = desired path (specify positive)              [m]
////      v    = velocity bound (specify positive)            [m/s]
////      a    = acceleration bound (specify positive)        [m/s2]
////      j    = jerk bound (specify positive)                [m/s3]
////      d    = derivative of jerk bound (specify positive)  [m/s4]
////      Ts   = sampling time            [s]        (optional, if not specified or 0: continuous time)
////      r    = position resolution      [m]        (optional, if not specified: 10*eps) 
////      s    = number of decimals for digitized 
////             derivative of jerk bound            (optional, if not specified: 15)
////
//// outputs:
////      t(1) = constant djerk phase duration
////      t(2) = constant jerk phase duration 
////      t(3) = constant acceleration phase duration 
////      t(4) = constant velocity phase duration 
////      
////       t1               t1               t1     t1  
////       .-.              .-.              .-.    .-.
////       | |              | |              | |    | |
////       | |t2    t3    t2| |   t4       t2| | t3 | |t2   
////       '-'--.-.----.-.--' '---------.-.--'-'----'-'--.-.--
////            | |    | |              | |              | |
////            | |    | |              | |              | |
////            '-'    '-'              '-'              '-'
////            t1     t1               t1               t1
////
//// In case of discrete time, derivative of jerk bound d is reduced to dd and
//// quantized to ddq using position resolution r and number of significant decimals s
//// Two position correction terms are calculated to 'repair' the position error 
//// resulting from using ddq instead of dd:
////   cor1  gives the number of position increments that can equally be divided 
////         over the entire trajectory duration
////   cor2  gives the remaining number of position increments
//// The result is given as:
////                          dd = [ ddq  cor1  cor2  dd ]
////

//// Paul Lambrechts, TUE fac. WTB, last modified: June 15, 2004.
////

