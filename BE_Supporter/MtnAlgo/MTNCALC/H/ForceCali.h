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



#pragma once
#include <windows.h>
#include "MtnApi.h"

#include "ForceCaliRel.h"
//#define BUFFER_ID_SEARCH_CONTACT 7
//typedef enum
//{
//	DPRAM_ADDR_START_BUF_7          = 0x330, // Trigger buffer 7 start searching 
//	DPRAM_ADDR_DONE_SEARCH_CONTACT  = 0x334, // 
//};



int mtn_search_contract_and_force_control(HANDLE m_hHandle, SEARCH_CONTACT_AND_FORCE_CONTROL_INPUT *stSearchContactAndForceControl, TEACH_CONTACT_OUTPUT *stpTeachContactResult);

//(0.1087 * 5.0/ 9 * 1.3)
#define DEF_FORCE_GRAM_PER_ADC_UPP   (1.0)
#define DEF_FORCE_GRAM_PER_ADC_LOW   (0.01)
// 20121231, (DEF_MAX_FORCE_GRAM/ 8192.0)

double mtn_get_force_ratio();
int mtn_set_force_ratio(double dNewRatio);
int mtn_teach_contact_acs(HANDLE m_hHandle, TEACH_CONTACT_INPUT *stpTeachContactParameter, TEACH_CONTACT_OUTPUT *stpTeachContactResult);


int acsc_upload_search_contact_parameter(HANDLE m_hHandle, TEACH_CONTACT_INPUT *stpTeachContact);
int acsc_download_search_contact_parameter(HANDLE m_hHandle, TEACH_CONTACT_INPUT *stpTeachContact);
int acsc_upload_search_contact_result(HANDLE m_hHandle, TEACH_CONTACT_OUTPUT *stpTeachContactResult);
int acsc_download_force_control_blk(HANDLE m_hHandle, FORCE_BLOCK_ACS *stpForceBlock);
void mtn_debug_print_to_file(FILE *fpData, SEARCH_CONTACT_AND_FORCE_CONTROL_INPUT *stpSearchContactAndForceControl, TEACH_CONTACT_OUTPUT *stpTeachContactResult);
