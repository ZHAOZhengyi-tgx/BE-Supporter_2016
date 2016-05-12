// 20100301 Port from *USG_BLK.h, consolidate, WB_USG_BLK.h and USG_BLK.h

#pragma once

#include "USG_BLK_DEF.h" // 20100301

void init_wb_usg_blk_seg();
void wb_usg_blk_set_segment(unsigned int uiBlk, unsigned int uiSeg, USG_UNIT_SEG *stpUSG_Seg);
void wb_usg_blk_get_segment(unsigned int uiBlk, unsigned int uiSeg, USG_UNIT_SEG *stpUSG_Seg);
void wb_usg_blk_set_trigger_address(unsigned int uiBlk, int iAddr);
int wb_usg_blk_get_trigger_address(unsigned int uiBlk);
void wb_usg_blk_set_nextblk_index(unsigned int uiBlk, unsigned int uiNextBlk);
unsigned int wb_usg_blk_get_nextblk_index(unsigned int uiBlk);
void wb_usg_blk_set_max_count_wait_trig_prot(unsigned int uiBlk, unsigned int uiWaitCnt);
unsigned int wb_usg_blk_get_max_count_wait_trig_prot(unsigned int uiBlk);

char *wb_usg_blk_get_text_error_flag(int iErrorFlag);
char *wb_usg_blk_get_text_status(int iStatusUSG);
char *wb_usg_blk_get_text_cmd(int iCmdUSG);

char wb_usg_fsm_get_flag_hardware_path();
void wb_usg_fsm_set_flag_via_1739u();
void wb_usg_fsm_set_flag_via_lpt1();

