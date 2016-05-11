#ifndef _INTERACT_H
#define _INTERACT_H

#ifndef _MSC_VER
#define WINAPI __stdcall
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum FUNCITEM
{
//	TYPE0=0,			//属性设置
//	SET_UNIT_FLAG,
	AUTO_SET=0,
	INIT_BOARD,
	SET_OUTMODE,
	SET_HOME_MODE,
	SET_DIR,
	ENABLE_SD,
	ENABLE_EL,
	ENABLE_ORG,
	ENABLE_ALM,
	SET_SD_LOGIC,
	SET_EL_LOGIC,
	SET_ORG_LOGIC,
	SET_ALM_LOGIC,
	SET_GETPOS_MODE,
	SET_ENCODER_MODE,
	OPEN_LIST,
	CLOSE_LIST,
	ADD_LIST,
	START_LOOKAHEAD,
	END_LOOKAHEAD,
//	TYPE1,				//参数设置
	SET_MAXSPEED,
	SET_CONSPEED,
	SET_PROFILE,
	SET_VECTOR_CONSPEED,
	SET_VECTOR_PROFILE,
	C_SET_VECTOR_PROFILE,
	C_SET_MAX_ACCEL,
	C_SET_MULTIPLE,
	C_SET_CURVE_VERTEX,
	SET_ELLIPSE_RATIO,
	SET_S_CURVE,
	SET_S_SECTION,
	SET_ABS_POS,
	RESET_POS,
	SET_STEPS_PR,
	SET_ENC_THREAD,
//	SET_UNIT,
//	TYPE2,				//运动指令
	CON_PMOVE,
	CON_PMOVE2,
	CON_PMOVE3,
	CON_PMOVE4,
//	CON_PMOVE_TO,
	FAST_PMOVE,
	FAST_PMOVE2,
	FAST_PMOVE3,
	FAST_PMOVE4,
//	FAST_PMOVE_TO,
	CON_VMOVE,
	CON_VMOVE2,
	CON_VMOVE3,
	CON_VMOVE4,
	FAST_VMOVE,
	FAST_VMOVE2,
	FAST_VMOVE3,
	FAST_VMOVE4,
	CON_HMOVE,
	CON_HMOVE2,
	CON_HMOVE3,
	CON_HMOVE4,
	FAST_HMOVE,
	FAST_HMOVE2,
	FAST_HMOVE3,
	FAST_HMOVE4,
	CON_LINE2,
	CON_LINE3,
	CON_LINE4,
//	CON_LINEN_TO,
	FAST_LINE2,
	FAST_LINE3,
	FAST_LINE4,
//	FAST_LINEN_TO,
	ARC_CENTER,
	FAST_ARC_CENTER,
	ARC_FINAL,
//	HELICAL_MOVE,
//	TYPE3,				//停止指令
	SUDDEN_STOP,
	SUDDEN_STOP2,
	SUDDEN_STOP3,
	SUDDEN_STOP4,
	SUDDEN_STOP_LIST,
	DECEL_STOP,
	DECEL_STOP2,
	DECEL_STOP3,
	DECEL_STOP4,
	DECEL_STOP_LIST,
	MOVE_PAUSE,
	MOVE_PAUSE_LIST,
	MOVE_RESUME,
	MOVE_RESUME_LIST,
	DELAY_TIME,
//	TYPE4,				//IO指令
	CHECKIN_BYTE,
	CHECKIN_BIT,
	OUTPORT_BIT,
	OUTPORT_BYTE,
	CHECK_SFR,
	CHECK_SFR_BIT,
	OUTPORT,
	INPORT,
//	TYPE5,				//特殊功能
	SET_BACKLASH,
	START_BACKLASH,
	END_BACKLASH,
	SET_WATCHDOG_TIME,
	RESET_WATCHDOG,
	START_WATCHDOG,
	STOP_WATCHDOG,
	GET_WATCHDOG_STATUS,
	ENABLE_IO_POS,
	SET_POSCMP_SOURCE,
	SET_IO_POS,
	ENABLE_LOCK_ENC,
	RESET_LOCKED_FLAG,
	GET_LOCKED_FLAG,
	GET_LOCKED_ENCODER,
	ENABLE_GEAR,
	ENABLE_HANDWHEEL,
	ENABLE_SOFTLIMIT,
	SET_SOFTLIMIT,
	SET_SOFTLIMIT_DATA,
	CHECK_SOFTLIMIT,
	ENABLE_POSERR_LIMIT,
	SET_POSERR_LIMIT,
	GET_POSERR_LIMIT,
	ENABLE_INPUT_MODE,
	SET_IM_DEADBAND,
	ENABLE_ISR,
	SET_ISR_ROUTINE,
	SET_ISR_FACTOR,
	GET_ISR_EVENT,
	SET_RAMP,
	CHANGE_SPEED,
//	CHANGE_POS,
	SET_TAN_FLAG,
	SET_TAN_INITPOS,
	SET_TAN_TUNE,
	SET_TAN_MAP,
	SET_TAN_AXIS,
	SET_TAN_PROFILE,
	SET_TAN_STOPANGLE,
	SET_TAN_IO,
	GET_TAN_LASTPOS,
	ENABLE_PWM,
	SET_PWM_FREQ,
	SET_PWM_RATIO,
//	TYPE6,				//查询函数
	GET_MAX_AXE,
	GET_BOARD_NUM,
	GET_AXE,
//	GET_UNIT,
	CHECK_IC,
	GET_ABS_POS,
	GET_REL_POS,
	GET_ENCODER,
	GET_DONE_SOURCE,
	GET_CONSPEED,
	GET_VECTOR_CONSPEED,
	GET_PROFILE,
	GET_VECTOR_PROFILE,
	GET_RATE,
	GET_CUR_DIR,
	CHECK_STATUS,
	CHECK_DONE,
	CHECK_LIMIT,
	CHECK_HOME,
	CHECK_SD,
	CHECK_ALARM,
	CHECK_DELAY_STATUS,
	GET_CMD_COUNTER,
	RESET_CMD_COUNTER,
	SET_CMD_COUNTER,
	GET_ERR,
	GET_LAST_ERR,
	RESET_ERR,
	GET_LIB_VER,
	GET_SYS_VER,
	GET_CARD_VER,
};
/////////////////////////////////////////////////////
int WINAPI auto_set(void);
int WINAPI init_board(void);
int WINAPI set_unit_flag(int flag);
//轴属性设置函数
int WINAPI set_outmode(int ch,int mode,int logic);
int WINAPI set_home_mode(int ch,int origin_mode);
int WINAPI set_dir(int ch,int dir);
int WINAPI enable_sd(int ch,int flag);
int WINAPI enable_el(int ch,int flag);
int WINAPI enable_org(int ch,int flag);
int WINAPI enable_alm(int ch,int flag);
int WINAPI set_sd_logic(int ch,int mode);
int WINAPI set_el_logic(int ch,int mode);
int WINAPI set_org_logic(int ch,int mode);
int WINAPI set_alm_logic(int ch,int mode);

//编码器属性设置函数
int WINAPI set_encoder_mode(int ch,int mode,int multip,int count_unit);
int WINAPI set_getpos_mode(int ch,int mode);

int WINAPI open_list();
int WINAPI close_list();
int WINAPI add_list();
int WINAPI start_lookahead();
int WINAPI end_lookahead();

//参数设置函数
int WINAPI set_maxspeed(int ch , double speed);
int WINAPI set_conspeed(int ch , double conspeed);
int WINAPI set_vector_conspeed(double conspeed);
int WINAPI set_profile(int ch , double vl , double vh , double ad);
int WINAPI set_vector_profile(double vec_vl , double vec_vh ,double vec_ad);
int WINAPI c_set_vector_profile(double vec_vl , double vec_vh ,double vec_ad);
int WINAPI c_set_max_accel(double vec_ad);
int WINAPI c_set_multiple(double mul);
int WINAPI c_set_curve_vertex(int nmode);
int WINAPI set_ellipse_ratio(double ratio);
int WINAPI set_s_curve(int ch,int mode);
int WINAPI set_s_section(int ch,double accel_sec,double decel_sec);
int WINAPI set_abs_pos(int ch,double pos);
int WINAPI reset_pos(int ch);
int WINAPI set_unit(int ch,double dl);
int WINAPI set_steps_pr(int ch,int rd);
int WINAPI set_enc_thread(int ch,int rd);

//运动指令函数
int WINAPI con_pmove(int ch,double step);
int WINAPI con_pmove2(int ch1,double step1,int ch2,double step2);
int WINAPI con_pmove3(int ch1,double step1,int ch2,double step2,int ch3,double step3);
int WINAPI con_pmove4(int ch1,double step1,int ch2,double step2,int ch3,double step3,int ch4,double step4);
int WINAPI con_pmove_to(int ch, double step);
int WINAPI fast_pmove(int ch,double step);
int WINAPI fast_pmove2(int ch1,double step1,int ch2,double step2);
int WINAPI fast_pmove3(int ch1,double step1,int ch2,double step2,int ch3,double step3);
int WINAPI fast_pmove4(int ch1,double step1,int ch2,double step2,int ch3,double step3,int ch4,double step4);
int WINAPI fast_pmove_to(int ch, double step);
int WINAPI con_vmove(int ch,int dir);
int WINAPI con_vmove2(int ch1,int dir1,int ch2,int dir2);
int WINAPI con_vmove3(int ch1,int dir1,int ch2,int dir2,int ch3,int dir3);
int WINAPI con_vmove4(int ch1,int dir1,int ch2,int dir2,int ch3,int dir3,int ch4,int dir4);
int WINAPI fast_vmove(int ch,int dir);
int WINAPI fast_vmove2(int ch1,int dir1,int ch2,int dir2);
int WINAPI fast_vmove3(int ch1,int dir1,int ch2,int dir2,int ch3,int dir3);
int WINAPI fast_vmove4(int ch1,int dir1,int ch2,int dir2,int ch3,int dir3,int ch4,int dir4);
int WINAPI con_hmove(int ch ,int dir1);
int WINAPI con_hmove2(int ch1,int dir1,int ch2,int dir2);
int WINAPI con_hmove3(int ch1,int dir1,int ch2,int dir2,int ch3,int dir3);
int WINAPI con_hmove4(int ch1,int dir1,int ch2,int dir2,int ch3,int dir3,int ch4,int dir4);
int WINAPI fast_hmove(int ch,int dir);
int WINAPI fast_hmove2(int ch1,int dir1,int ch2,int dir2);
int WINAPI fast_hmove3(int ch1,int dir1,int ch2,int dir2,int ch3,int dir3);
int WINAPI fast_hmove4(int ch1,int dir1,int ch2,int dir2,int ch3,int dir3,int ch4,int dir4);
int WINAPI con_line2(int ch1,double step1,int ch2, double step2);
int WINAPI con_line3(int ch1,double step1,int ch2,double step2,int ch3,double step3);
int WINAPI con_line4(int ch1,double step1,int ch2,double step2,int ch3,double step3,int ch4,double step4);
int WINAPI con_linen_to(int chnum, int *ch, double *pos);
int WINAPI fast_line2(int ch1,double step1,int ch2,double step2);
int WINAPI fast_line3(int ch1,double step1,int ch2,double step2,int ch3,double step3);
int WINAPI fast_line4(int ch1,double step1,int ch2,double step2,int ch3,double step3,int ch4,double step4);
int WINAPI fast_linen_to(int chnum, int *ch, double *pos);
int WINAPI arc_center(int ch1,int ch2,double cen1,double cen2,double angle );
int WINAPI arc_final(int ch1,int ch2,int dir,double fx,double fy,double r);
int WINAPI fast_arc_center(int ch1,int ch2,double cen1,double cen2,double angle );
int WINAPI helical_move(int ch1,int ch2,int ch3,double cen1,double cen2,double angle, double pitch);

//制动函数
int WINAPI sudden_stop(int ch);
int WINAPI sudden_stop2(int ch1,int ch2);
int WINAPI sudden_stop3(int ch1,int ch2,int ch3);
int WINAPI sudden_stop4(int ch1,int ch2,int ch3,int ch4);
int WINAPI sudden_stop_list();
int WINAPI decel_stop(int ch);
int WINAPI decel_stop2(int ch1,int ch2);
int WINAPI decel_stop3(int ch1,int ch2,int ch3);
int WINAPI decel_stop4(int ch1,int ch2,int ch3,int ch4);
int WINAPI decel_stop_list();
int WINAPI move_pause(int ch);
int WINAPI move_pause_list();
int WINAPI move_resume(int ch);
int WINAPI move_resume_list();
int WINAPI delay_time(int time);

//I/O口操作函数
int WINAPI checkin_byte(int cardno);
int WINAPI checkin_bit(int cardno,int bitno);
int WINAPI outport_bit(int cardno,int bitno,int status);
int WINAPI outport_byte(int cardno,int data);
int WINAPI check_sfr(int cardno);
int WINAPI check_sfr_bit(int cardno,int bitno);
int WINAPI Outport(int portid,unsigned char byte); 
int WINAPI Inport(int portid);

//特殊功能
int WINAPI start_backlash(int ch);
int WINAPI end_backlash(int ch);
int WINAPI set_backlash(int ch,double blash);
int WINAPI set_watchdog_time(int cardno,long time);
int WINAPI reset_watchdog(int cardno);
int WINAPI start_watchdog(int cardno);
int WINAPI stop_watchdog(int cardno);
int WINAPI get_watchdog_status(int cardno);
int WINAPI enable_io_pos(int ch,int flag);
int WINAPI set_poscmp_source(int ch,int mode);
int WINAPI set_io_pos(int ch,int open_pos,int close_pos);
int WINAPI enable_lock_enc(int ch,int mode);
int WINAPI reset_locked_flag(int ch);
int WINAPI get_locked_flag(int ch);
int WINAPI get_locked_encoder(int ch,int num,long *enc);
int WINAPI enable_gear(int slave, int master, double ratio, int mode);
int WINAPI enable_handwheel(int ch, int mul,int mode);
int WINAPI enable_softlimit(int ch, int mode);
int WINAPI set_softlimit(int ch, int source, int action);
int WINAPI set_softlimit_data(int ch, double nel, double pel);
int WINAPI check_softlimit(int ch);
int WINAPI enable_poserr_limit(int ch, int mode);
int WINAPI set_poserr_limit(int ch, double limit, int mul);
int WINAPI get_poserr_limit(int ch, double *error);
int WINAPI enable_input_mode(int ch,int mode);
int WINAPI set_im_deadband(int ch, double db);
int WINAPI enable_isr(int cardno, int mode);
int WINAPI set_isr_routine(void ( * MyIsr)( ));
int WINAPI set_isr_factor(int ch,int factor);
int WINAPI get_isr_event(int ch,int *event);
int WINAPI set_ramp(int ch, double *ad, double *ratio,int num);
int WINAPI change_speed(int ch,double speed);
//int WINAPI change_pos(int ch, double pos);
int WINAPI set_tan_initpos(double degree,int flag);
int WINAPI set_tan_tune(double degree);
int WINAPI set_tan_flag(int flag);
int WINAPI set_tan_map(double pulseperdeg);
int WINAPI set_tan_axis(int ch,int dir);
int WINAPI set_tan_profile(double vl,double vh,double ad);
int WINAPI set_tan_stopangle(double stopagl,double liftagl,int nFlag);
int WINAPI set_tan_io(int cardno,int bitno,int time);//ms
int WINAPI get_tan_lastpos(double *degree);//度

///////////////////////////////////////////////////////////////
//位置和状态查询函数
int WINAPI get_max_axe();
int WINAPI get_board_num();
int WINAPI get_axe(int cardno);
int WINAPI get_unit(int ch,double* dl);
int WINAPI check_IC(int cardno);
int WINAPI get_abs_pos(int ch,double *pos);
int WINAPI get_rel_pos(int ch,double *pos);
int WINAPI get_encoder(int ch,long *count);
int WINAPI get_done_source(int ch,long *src);

//PWM输出控制
int WINAPI enable_pwm(int cardno, int en);
int WINAPI set_pwm_freq(int cardno, double freq);
int WINAPI set_pwm_ratio(int cardno, double ratio);

double WINAPI get_conspeed(int ch);
double WINAPI get_vector_conspeed();
int    WINAPI get_profile(int ch,double *vl,double *vh,double *ad);
int    WINAPI get_vector_profile(double *vec_vl,double *vec_vh,double *vec_ad);
double WINAPI get_rate(int ch);
int    WINAPI get_cur_dir(int ch);
int    WINAPI check_status(int ch);
int    WINAPI check_done(int ch);
int    WINAPI check_limit(int ch);
int    WINAPI check_home(int ch);
int    WINAPI check_SD(int ch);
int    WINAPI check_alarm(int ch);
int	   WINAPI check_delay_status();
int    WINAPI get_cmd_counter();
int	   WINAPI reset_cmd_counter();
int	   WINAPI set_cmd_counter(int counter);
int    WINAPI check_timer_num(unsigned long *num);
//错误代码操作函数
int WINAPI get_last_err();
int WINAPI get_err(int index,int *data);
int	WINAPI reset_err();

//版本读取函数
int WINAPI get_lib_ver(long* major,long *minor1,long *minor2);
int WINAPI get_sys_ver(long* major,long *minor1,long *minor2);
int WINAPI get_card_ver(int cardno,long* type,long* major,long *minor1,long *minor2);

#ifdef __cplusplus
}
#endif

#endif
