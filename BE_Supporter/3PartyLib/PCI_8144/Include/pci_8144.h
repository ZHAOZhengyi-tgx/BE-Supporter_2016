#ifndef _PCI_8144_H_
#define _PCI_8144_H_

#define _MYWIN32

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MYWIN32
#define FNTYPE PASCAL
#endif

#include "type_def.h"

// System & Initialization Section:3
I16 FNTYPE _8144_initial( I32 *CardIdInBit, I16 ManualId );
I16 FNTYPE _8144_close();
I16 FNTYPE _8144_get_version( I16 CardId, I32 *Dll_1, I32 *Dll_2, I32 *FirmwareVersion, I32 *DriverVersion );
I16 FNTYPE _8144_set_security_key(I16 CardId, U16 OldPassword, U16 NewPassword);
I16 FNTYPE _8144_check_security_key(I16 CardId, U16 Password);
I16 FNTYPE _8144_reset_security_key(I16 CardId);
I16 FNTYPE _8144_config_from_file();



//Motion Interface I/O  Section:4
I16 FNTYPE _8144_set_limit_logic(I16 AxisNo, I16 LimitLogic );
I16 FNTYPE _8144_get_limit_logic(I16 AxisNo, I16 *LimitLogic );
I16 FNTYPE _8144_get_mio_status(I16 AxisNo, I16 *MotionIoStatusInBit );
I16 FNTYPE _8144_set_mio_sensitivity( I16 AxisNo, I16 HighOrLow );
I16 FNTYPE _8144_set_pls_outmode( I16 AxisNo, I16 PulseLogic );
I16 FNTYPE _8144_set_pls_outmode2( I16 AxisNo, I16 PulseMode, I16 PulseLogic );

// Motion  Section:5
I16 FNTYPE _8144_tv_move( I16 AxisNo, F64 StrVel, F64 MaxVel, F64 Tacc );
I16 FNTYPE _8144_sv_move( I16 AxisNo, F64 StrVel, F64 MaxVel, F64 TSacc );
I16 FNTYPE _8144_start_tr_move( I16 AxisNo, F64 Distance, F64 StrVel, F64 MaxVel, F64 Tacc );
I16 FNTYPE _8144_start_sr_move( I16 AxisNo, F64 Distance, F64 StrVel, F64 MaxVel, F64 Tacc );
I16 FNTYPE _8144_set_external_start( I16 AxisNo, I16 Enable );
I16 FNTYPE _8144_emg_stop( I16 AxisNo );
I16 FNTYPE _8144_dec_stop( I16 AxisNo );
I16 FNTYPE _8144_slow_down( I16 AxisNo );
I16 FNTYPE _8144_speed_up( I16 AxisNo );
I16 FNTYPE _8144_enable_org_stop( I16 AxisNo, I16 Enable );
I16 FNTYPE _8144_enable_sd_signal( I16 AxisNo, I16 Enable );
I16 FNTYPE _8144_get_remaining_pulse( I16 AxisNo, F64 *RemainingPulse );
I16 FNTYPE _8144_set_remaining_pulse( I16 AxisNo, I32 RemainingPulse ); 
I16 FNTYPE _8144_home_move( I16 AxisNo, F64 StrVel, F64 MaxVel, F64 Tacc, I16 AccType );
I16 FNTYPE _8144_enable_get_command( I16 AxisNo, I16 Enable );
I16 FNTYPE _8144_get_command( I16 AxisNo, I32 *Cmd );
I16 FNTYPE _8144_set_command( I16 AxisNo, I32 Cmd );

// Motion status Section:6
I16 FNTYPE _8144_motion_done( I16 AxisNo, I16 *OperatingOrStop );
I16 FNTYPE _8144_motion_status( I16 AxisNo, I16 *MotionStatusInBit );
I16 FNTYPE _8144_home_status( I16 AxisNo, I16 *HomeStatusInBit );

// Interrupt Section:7
I16 FNTYPE _8144_set_motion_interrupt_factor( I16 AxisNo, I16 MotionIntFactorInBit );
I16 FNTYPE _8144_wait_single_motion_interrupt( I16 AxisNo, I16 MotionIntFactorBitNum, I32 TimeOutMs );
I16 FNTYPE _8144_set_gpio_interrupt_factor( I16 CardId, I32 GpioIntFactorInBit );
I16 FNTYPE _8144_wait_single_gpio_interrupt( I16 CardId, I16 GpioIntFactorBitNum, I32 TimeOutMs );
I16 FNTYPE _8144_wait_multiple_gpio_interrupt( I16 CardId, I32 GpioIntFactorInBits, I16 WaitOption, I32 TimeOutMs,  I32 *GpioIntTriggeredInBits );

// General purpose I/O Section:8
I16 FNTYPE _8144_get_gpio_input( I16 CardId, I16 *DiStatusInBit );
I16 FNTYPE _8144_get_gpio_input_channel( I16 CardId, I16 ChannelNum, I16 *DiStatus );
I16 FNTYPE _8144_set_gpio_output( I16 CardId, I16 DoValueInBit );
I16 FNTYPE _8144_set_gpio_output_channel( I16 CardId, I16 ChannelNum, I16 DoValue );
I16 FNTYPE _8144_get_gpio_output( I16 CardId, I16 *DoValueInBit );
I16 FNTYPE _8144_get_gpio_output_channel( I16 CardId, I16 ChannelNum, I16 *DoValue );

// Speed profile calculation Section:9
I16 FNTYPE _8144_get_tv_move_profile( I16 AxisNo, F64 StrVel, F64 MaxVel, F64 Tacc, F64 *StrVelP, F64 *MaxVelP, F64 *TaccP, F64 *MagnP );
I16 FNTYPE _8144_get_sv_move_profile( I16 AxisNo, F64 StrVel, F64 MaxVel, F64 Tacc, F64 *StrVelP, F64 *MaxVelP, F64 *TaccP, F64 *MagnP );
I16 FNTYPE _8144_get_start_tr_move_profile( I16 AxisNo, F64 Distance, F64 StrVel, F64 MaxVel, F64 Tacc, F64 *StrVelP, F64 *MaxVelP, F64 *TaccP, F64 *TConstP, F64 *TdecP, F64 *MagnP );
I16 FNTYPE _8144_get_start_sr_move_profile( I16 AxisNo, F64 Distance, F64 StrVel, F64 MaxVel, F64 Tacc, F64 *StrVelP, F64 *MaxVelP, F64 *TaccP, F64 *TConstP, F64 *TdecP, F64 *MagnP );

// Motion advance (Reserved functions)
// Do not use following fucntions. 
I16 FNTYPE _8144_one_axis_move( I16 AxisNo, U16 MotionType, U16 SpeedPattern, U16 Direction,  U16 AccDecCurveType, U32 R0_PE, U16 R1_FL, U16 R2_FH, U16 R3_AC, U16 R4_MA, U16 R5_RD );
I16 FNTYPE _8144_get_position( I16 AxisNo, F64 *Pos );


#ifdef __cplusplus
}
#endif

#endif
