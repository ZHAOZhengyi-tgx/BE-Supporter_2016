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
/****************************************************************************/
//All the codes in the software are subject to the GNU General Public License("GPL") or 
//GNU Lesser General Public License("LGPL").
//Please see the GNU and LPGL Web sites to view the terms of each license.
/****************************************************************************/
//#if     _MSC_VER > 1000
//#pragma once
//#endif

#ifndef __MTNCONFG_H__
#define __MTNCONFG_H__    1

#include "mtndefin.h"
//#include "mtnapi.h"

//#ifdef  __cplusplus
//extern "C" {
//#endif

//// Structure for config file, read and write
#ifndef __config_profile_block
typedef struct __config_profile_block
{
	char prf_blk_name[MTN_API_MAX_STRLEN_FEATURENAME];	/* expects string less than MTN_API_MAX_STRLEN_FEATURENAME characters */
    long start_file_pos;
    long end_file_pos;
    short num_of_lines;
}MTN_CFG_PRF_BLK;
#endif // __config_profile_block


/****************************************************************************/
/* Format:                                                                  */
/*                                                                          */
/* [CONFIG_BLK1]                                                            */
/* config1_1 = "parameter string 1"                                         */
/* config1_2 = 123456    # number                                           */
/* # this is a remark line                                                  */
/* [CONFIG_BLK2]                                                            */
/* config2_1 = "parameter string 1"                                         */
/* config2_2 = 123456    # number                                           */
/* config2_3 = 123                                                          */
/*                                                                          */
/* explaination:                                                            */
/* cfg_table[1].prf_blk_name = CONFIG_BLK1                                  */
/* cfg_table[1].start_file_pos point to 'c' in config1_1                    */
/* cfg_table[1].end_file_pos point to the 'CRLF' in "this is a remark line" */
/* cfg_table[1].num_of_lines is 4 for CONFIG_BLK1, inclusive of remark      */
/*                                                                          */
/* Notes:                                                                   */
/* 1.   The comment/remark line is started with "#" or ";"                  */
/* 2.   Do not put more than 1 line of remark before the                    */
/*      subsequent [block].                                                 */
/*      Example:                                                            */
/* # This is an example.                    > Can be more than 1 line       */
/* #                                        > since this is the remark      */
/* # Remark1       --> remark for CFG_BLK1  > beofre 1st block              */
/* [CFG_BLK1]                                                               */
/* prof1 = "1,2,3"                                                          */
/* prof2 = <456,789> ;remark                                                */
/* prof3 = "This is a string"                                               */
/* prof4 = 100                                                              */
/* # Remark2      --> remark for CFG_BLK2  > Restrict to 1 line             */
/* [CFG_BLK2]                                                               */
/* prof1 = <200>                                                            */
/* # Remark3      --> remark for CFG_BLK2  > Restrict to 1 line             */
/* [CFG_BLK3]                                                               */
/* prof1 = 12 #remark                                                       */
/* prof2 = "Hello"                                                          */
/****************************************************************************/

short mtn_cfg_OpenConfigFile(char *p_config_pathname);
short mtn_cfg_ReadConfigString(char *p_blkname, char *p_cfg, char *p_parameter);
short mtn_cfg_ReadConfigIntValue(char *p_blkname, char *p_cfg, int *p_parameter);
short mtn_cfg_ReadConfigRealValue(char *p_blkname, char *p_cfg, float *p_parameter);
short mtn_cfg_CloseConfigFile(void);
short mtn_cfg_PutConfigString(char *p_blockname, char *p_config, char *p_parameter);

//#ifdef  __cplusplus
//}
//#endif

#endif
