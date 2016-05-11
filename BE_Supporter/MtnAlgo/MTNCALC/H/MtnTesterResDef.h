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



#ifndef __MTN_TESTER_RES_DEF__
#define __MTN_TESTER_RES_DEF__

#include <WinUser.h>
// Definition of motion tester's resources


// System Debug Mode
void sys_set_flag_debug_mode(int iFlagMode);
int get_sys_flag_debug_mode();


// Communication related Macro-definition
#define FLAG_NO_CONNECTION_ACS			0
#define FLAG_CONNECTED_WITH_ACT_CARD	1
#define FLAG_CONNECTED_WITH_SIMULATOR	2

// System cleaning
void sys_set_flag_clean_env_on_exit(int iFlagClean);  // MtnTesterEntry.cpp
void aft_rand_burn_in_init_def_var();
void aft_rand_burn_in_init_def_var_hori_led_bonder();
void aft_rand_burn_in_init_def_var_1cup_v_led_bonder();
void aft_rand_burn_in_init_def_var_1_cup_vled_bonder();

void mtn_move_test_config_burn_in_condition();
#include "MtnWbDef.h"


///////
// 1. Timer Resource
///////
#define  IDT_SPEC_TEST_DLG_TIMER  WM_USER + 99

#define  IDT_TIMER_SYS_ENTRY  WM_USER + 101  

#define  IDT_TIMER_USG_PROF_DIO_DLG  WM_USER + 100  

#define  IDT_TIMER_ROYA_TEST_DLG  (WM_USER + 102)

#define  IDT_MOVE_TEST_DLG_TIMER  (WM_USER + 103)

#define  IDT_SERVO_CTRL_TUNING_DLG_TIMER (WM_USER + 104)

#define  IDT_VEL_STEP_GROUP_TEST_DLG_TIMER (WM_USER + 105)

#define IDT_TIMER_PARAMETER_DLG (WM_USER + 106)

#define IDT_PROF_CALC_TEST_DLG_TIMER  (WM_USER + 107)

#define  IDT_STEPPER_CTRL_DLG_TIMER (WM_USER + 108)

#endif // __MTN_TESTER_RES_DEF__