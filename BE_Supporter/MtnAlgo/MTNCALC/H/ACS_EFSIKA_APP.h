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




#ifndef ACS_EFSIKA_APP
#define ACS_EFSIKA_APP

// application interface from ACSC library
extern int _ACSCLIB_ WINAPI acsc_GetFPosition(HANDLE Handle, int Axis, double* FPosition, ACSC_WAITBLOCK* Wait);
extern int _ACSCLIB_ WINAPI acsc_GetMotorState(HANDLE Handle, int Axis, int* State, ACSC_WAITBLOCK* Wait);

// The function defines a value of motion velocity.
extern int _ACSCLIB_ WINAPI acsc_SetVelocity(HANDLE Handle, int Axis, double Velocity, ACSC_WAITBLOCK* Wait);
extern int _ACSCLIB_ WINAPI acsc_GetVelocity(HANDLE Handle, int Axis, double* Velocity, ACSC_WAITBLOCK* Wait);


// The function defines a value of motion acceleration.
extern int _ACSCLIB_ WINAPI acsc_SetAcceleration(HANDLE Handle, int Axis, double Acceleration, ACSC_WAITBLOCK* Wait);


// The function retrieves a value of motion acceleration.
extern int _ACSCLIB_ WINAPI acsc_GetAcceleration(HANDLE Handle, int Axis, double* Acceleration, ACSC_WAITBLOCK* Wait);

// The function defines a value of motion deceleration.
extern int _ACSCLIB_ WINAPI acsc_SetDeceleration(HANDLE Handle, int Axis, double Deceleration, ACSC_WAITBLOCK* Wait);


// The function retrieves a value of motion deceleration.
extern int _ACSCLIB_ WINAPI acsc_GetDeceleration(HANDLE Handle, int Axis, double* Deceleration, ACSC_WAITBLOCK* Wait);


// The function defines a value of motion jerk.
extern int _ACSCLIB_ WINAPI acsc_SetJerk(HANDLE Handle, int Axis, double Jerk, ACSC_WAITBLOCK* Wait);


// The function retrieves a value of motion jerk.
extern int _ACSCLIB_ WINAPI acsc_GetJerk(HANDLE Handle, int Axis, double* Jerk, ACSC_WAITBLOCK* Wait);


// The function defines a value of kill deceleration.
extern int _ACSCLIB_ WINAPI acsc_SetKillDeceleration(HANDLE Handle, int Axis, double KillDeceleration, ACSC_WAITBLOCK* Wait);


// The function retrieves a value of kill deceleration.
extern int _ACSCLIB_ WINAPI acsc_GetKillDeceleration(HANDLE Handle, int Axis, double* KillDeceleration, ACSC_WAITBLOCK* Wait);


// The function provides on-the-fly change a value of motion velocity.
extern int _ACSCLIB_ WINAPI acsc_SetVelocityImm(HANDLE Handle, int Axis, double Velocity, ACSC_WAITBLOCK* Wait);


// The function provides on-the-fly change a value of motion acceleration.
extern int _ACSCLIB_ WINAPI acsc_SetAccelerationImm(HANDLE Handle, int Axis, double Acceleration, ACSC_WAITBLOCK* Wait);


// The function provides on-the-fly change a value of motion deceleration.
extern int _ACSCLIB_ WINAPI acsc_SetDecelerationImm(HANDLE Handle, int Axis, double Deceleration, ACSC_WAITBLOCK* Wait);


// The function provides on-the-fly change a value of motion jerk.
extern int _ACSCLIB_ WINAPI acsc_SetJerkImm(HANDLE Handle, int Axis, double Jerk, ACSC_WAITBLOCK* Wait);

// The function provides on-the-fly change a value of kill deceleration.
extern int _ACSCLIB_ WINAPI acsc_SetKillDecelerationImm(HANDLE Handle, int Axis, double KillDeceleration, ACSC_WAITBLOCK* Wait);

// The function assigns a current value of target position for several axes.
extern int _ACSCLIB_ WINAPI acsc_SetTargetPositionM(HANDLE Handle, int* Axes, double *TargetPositions, ACSC_WAITBLOCK* Wait);

// The function assigns a current value of target position.
extern int _ACSCLIB_ WINAPI acsc_SetTargetPosition(HANDLE Handle, int Axis, double TargetPosition, ACSC_WAITBLOCK* Wait);

// The function retrieves a current value of motor feedback position.
extern int _ACSCLIB_ WINAPI acsc_GetTargetPosition(HANDLE Handle, int Axis, double* TargetPosition, ACSC_WAITBLOCK* Wait);

// The function assigns a current value of target position.
extern int _ACSCLIB_ WINAPI acsc_SetFPosition(HANDLE Handle, int Axis, double FPosition, ACSC_WAITBLOCK* Wait);

// The function retrieves a current value of motor feedback position.
extern int _ACSCLIB_ WINAPI acsc_GetFPosition(HANDLE Handle, int Axis, double* FPosition, ACSC_WAITBLOCK* Wait);

// The function assigns a current value of reference position.
extern int _ACSCLIB_ WINAPI acsc_SetRPosition(HANDLE Handle, int Axis, double RPosition, ACSC_WAITBLOCK* Wait);

// The function retrieves a current value of reference position.
extern int _ACSCLIB_ WINAPI acsc_GetRPosition(HANDLE Handle, int Axis, double* RPosition, ACSC_WAITBLOCK* Wait);

// The function retrieves a current value of motor feedback velocity.
extern int _ACSCLIB_ WINAPI acsc_GetFVelocity(HANDLE Handle, int Axis, double* FVelocity, ACSC_WAITBLOCK* Wait);

// The function retrieves a current value of reference velocity.
extern int _ACSCLIB_ WINAPI acsc_GetRVelocity(HANDLE Handle, int Axis, double* RVelocity, ACSC_WAITBLOCK* Wait);

// The function activates a motor.
extern int _ACSCLIB_ WINAPI acsc_Enable(HANDLE Handle, int Axis, ACSC_WAITBLOCK* Wait);

// The function activates several motors.
extern int _ACSCLIB_ WINAPI acsc_EnableM(HANDLE Handle, int* Axes, ACSC_WAITBLOCK* Wait);

// The function shuts off a motor.
extern int _ACSCLIB_ WINAPI acsc_Disable(HANDLE Handle, int Axis, ACSC_WAITBLOCK* Wait);

// The function shuts off several motors.
extern int _ACSCLIB_ WINAPI acsc_DisableM(HANDLE Handle, int* Axes, ACSC_WAITBLOCK* Wait);

// The function shuts off all motors.
extern int _ACSCLIB_ WINAPI acsc_DisableAll(HANDLE Handle, ACSC_WAITBLOCK* Wait);

// The function creates a coordinate system for multi-axis motion.
extern int _ACSCLIB_ WINAPI acsc_Group(HANDLE Handle, int* Axes, ACSC_WAITBLOCK* Wait);
extern int _ACSCLIB_ WINAPI acsc_ff(void **ptr);

// The function breaks down an axis group created before by the acsc_Group function.
extern int _ACSCLIB_ WINAPI acsc_Split(HANDLE Handle, int* Axes, ACSC_WAITBLOCK* Wait);

// The function breaks down all axis groups created before by the acsc_Group function.
extern int _ACSCLIB_ WINAPI acsc_SplitAll(HANDLE Handle, ACSC_WAITBLOCK* Wait);

// The function starts up a motion that is waiting in the specified motion queue.
extern int _ACSCLIB_ WINAPI acsc_Go(HANDLE Handle, int Axis, ACSC_WAITBLOCK* Wait);

// The function synchronously starts up several motions 
// that are waiting in the specified motion queues. 
extern int _ACSCLIB_ WINAPI acsc_GoM(HANDLE Handle, int* Axes, ACSC_WAITBLOCK* Wait);

// The function terminates a motion using the full deceleration profile.
extern int _ACSCLIB_ WINAPI acsc_Halt(HANDLE Handle, int Axis, ACSC_WAITBLOCK* Wait);

// The function terminates several motions using the full deceleration profile.
extern int _ACSCLIB_ WINAPI acsc_HaltM(HANDLE Handle, int* Axes, ACSC_WAITBLOCK* Wait);

// The function terminates a motion using reduced deceleration profile.
extern int _ACSCLIB_ WINAPI acsc_Kill(HANDLE Handle, int Axis, ACSC_WAITBLOCK* Wait);

// The function terminates several motions using reduced deceleration profile.
extern int _ACSCLIB_ WINAPI acsc_KillM(HANDLE Handle, int* Axes, ACSC_WAITBLOCK* Wait);

// The function terminates all currently executed motions.
extern int _ACSCLIB_ WINAPI acsc_KillAll(HANDLE Handle, ACSC_WAITBLOCK* Wait);

// The function terminates a motion immediately and 
// provides a smooth transition to the next motion.
extern int _ACSCLIB_ WINAPI acsc_Break(HANDLE Handle, int Axis, ACSC_WAITBLOCK* Wait);

// The function terminates several motions immediately and 
// provides a smooth transition to the next motions.
extern int _ACSCLIB_ WINAPI acsc_BreakM(HANDLE Handle, int* Axes, ACSC_WAITBLOCK* Wait);

// The function initiates a single-axis motion to the specified point.
extern int _ACSCLIB_ WINAPI acsc_ToPoint(HANDLE Handle, int Flags, int Axis, double Point, ACSC_WAITBLOCK* Wait);

// The function initiates a multi-axis motion to the specified point
extern int _ACSCLIB_ WINAPI acsc_ToPointM(HANDLE Handle, int Flags, int* Axes, double* Point, ACSC_WAITBLOCK* Wait);

// The function initiates a single-axis motion to the specified point 
// using the specified velocity or end velocity.
extern int _ACSCLIB_ WINAPI acsc_ExtToPoint(HANDLE Handle, int Flags, int Axis, double Point, double Velocity, double EndVelocity, ACSC_WAITBLOCK* Wait);

// The function initiates a multi-axis motion to the specified point 
// using the specified velocity or end velocity.
extern int _ACSCLIB_ WINAPI acsc_ExtToPointM(HANDLE Handle, int Flags, int* Axes, double* Point, double Velocity, double EndVelocity, ACSC_WAITBLOCK* Wait);

// The function initiates a single-axis track motion.
extern int _ACSCLIB_ WINAPI acsc_Track(HANDLE Handle, int Flags, int Axis, ACSC_WAITBLOCK* Wait);

// The function initiates a multile-axis track motion.
extern int _ACSCLIB_ WINAPI acsc_TrackM(HANDLE Handle, int Flags, int *Axis, ACSC_WAITBLOCK* Wait);

// The function clears the current faults and results of previous faults stored in the MERR variable.
// For single axis
extern int _ACSCLIB_ WINAPI acsc_FaultClear(HANDLE Handle, int Axis, ACSC_WAITBLOCK* Wait);


// The function clears the current faults and results of previous faults stored in the MERR variable.
// For multiple axes
extern int _ACSCLIB_ WINAPI acsc_FaultClearM(HANDLE Handle, int *Axes, ACSC_WAITBLOCK* Wait);

// The function writes system configuration-obsolete
extern int _ACSCLIB_ WINAPI acsc_SetConf(HANDLE Handle, int Key, int Index, double Value, ACSC_WAITBLOCK* Wait);

// The function reads system configuration-obsolete
extern int _ACSCLIB_ WINAPI acsc_GetConf(HANDLE Handle, int Key, int Index, double *Value, ACSC_WAITBLOCK* Wait);

// The function writes system configuration
extern int _ACSCLIB_ WINAPI acsc_Setconf(HANDLE Handle, int Key, int Index, int Value, ACSC_WAITBLOCK* Wait);

// The function reads system configuration
extern int _ACSCLIB_ WINAPI acsc_Getconf(HANDLE Handle, int Key, int Index, int *Value, ACSC_WAITBLOCK* Wait);

// The function initiates a single-axis jog motion.
extern int _ACSCLIB_ WINAPI acsc_Jog(HANDLE Handle, int Flags, int Axis, double Velocity, ACSC_WAITBLOCK* Wait);

// The function initiates a multi-axis jog motion.
extern int _ACSCLIB_ WINAPI acsc_JogM(HANDLE Handle, int Flags, int* Axes, int* Direction, double Velocity, ACSC_WAITBLOCK* Wait);

// The function initiates calculating of master value for an axis.
extern int _ACSCLIB_ WINAPI acsc_SetMaster(HANDLE Handle, int Axis, char* Formula, ACSC_WAITBLOCK* Wait);

// The function initiates a master-slave motion.
extern int _ACSCLIB_ WINAPI acsc_Slave(HANDLE Handle, int Flags, int Axis, ACSC_WAITBLOCK* Wait);

// The function initiates a master-slave motion with a limited following area.
extern int _ACSCLIB_ WINAPI acsc_SlaveStalled(HANDLE Handle, int Flags, int Axis, double Left, double Right, ACSC_WAITBLOCK* Wait);

// The function initiates a single-axis multi-point motion.
extern int _ACSCLIB_ WINAPI acsc_MultiPoint(HANDLE Handle, int Flags, int Axis, double Dwell, ACSC_WAITBLOCK* Wait);

// The function initiates a multi-axis multi-point motion.
extern int _ACSCLIB_ WINAPI acsc_MultiPointM(HANDLE Handle, int Flags, int* Axes, double Dwell, ACSC_WAITBLOCK* Wait);

// The function initiates a single-axis spline motion. 
// The motion follows an arbitrary path defined by a set of points.
extern int _ACSCLIB_ WINAPI acsc_Spline(HANDLE Handle, int Flags, int Axis, double Period, ACSC_WAITBLOCK* Wait);

// The function initiates a multi-axis spline motion. 
// The motion follows an arbitrary path defined by a set of points.
extern int _ACSCLIB_ WINAPI acsc_SplineM(HANDLE Handle, int Flags, int* Axes, double Period, ACSC_WAITBLOCK* Wait);

// The function initiates a multi-axis segmented motion.
extern int _ACSCLIB_ WINAPI acsc_Segment(HANDLE Handle, int Flags, int* Axes, double* Point, ACSC_WAITBLOCK* Wait);

// The function adds a linear segment to a segmented motion.
extern int _ACSCLIB_ WINAPI acsc_Line(HANDLE Handle, int* Axes, double* Point, ACSC_WAITBLOCK* Wait);

// The function adds a linear segment to a segmented motion and specifies a motion velocity.
extern int _ACSCLIB_ WINAPI acsc_ExtLine(HANDLE Handle, int* Axes, double* Point, double Velocity, ACSC_WAITBLOCK* Wait);

// The function adds an arc segment to a segmented motion and specifies the coordinates 
// of center point, coordinates of the final point and the direction of rotation.
extern int _ACSCLIB_ WINAPI acsc_Arc1(HANDLE Handle, int* Axes, double* Center, double* FinalPoint, int Rotation, ACSC_WAITBLOCK* Wait);

// The function adds an arc segment to a segmented motion and specifies the coordinates 
// of center point, coordinates of the final point, direction of rotation and 
// the vector velocity for the current segment.
extern int _ACSCLIB_ WINAPI acsc_ExtArc1(HANDLE Handle, int* Axes, double* Center, double* FinalPoint, int Rotation, double Velocity, ACSC_WAITBLOCK* Wait);

// The function adds an arc segment to a segmented motion and specifies the coordinates 
// of center point and rotation angle.
extern int _ACSCLIB_ WINAPI acsc_Arc2(HANDLE Handle, int* Axes, double* Center, double Angle, ACSC_WAITBLOCK* Wait);

// The function adds an arc segment to a segmented motion and specifies the coordinates 
// of center point, rotation angle and the vector velocity for the current segment.
extern int _ACSCLIB_ WINAPI acsc_ExtArc2(HANDLE Handle, int* Axes, double* Center, double Angle, double Velocity, ACSC_WAITBLOCK* Wait);

// The function provides a smooth transition between two segments of segmented motion.
extern int _ACSCLIB_ WINAPI acsc_Stopper(HANDLE Handle, int* Axes, ACSC_WAITBLOCK* Wait);

// The function sets a projection matrix for segmented motion.
extern int _ACSCLIB_ WINAPI acsc_Projection(HANDLE Handle, int* Axes, char* Matrix, ACSC_WAITBLOCK* Wait);


// The function adds a point to a single-axis multi-point or spline motion.
extern int _ACSCLIB_ WINAPI acsc_AddPoint(HANDLE Handle, int Axis, double Point, ACSC_WAITBLOCK* Wait);


// The function adds a point to a multi-axis multi-point or spline motion.
extern int _ACSCLIB_ WINAPI acsc_AddPointM(HANDLE Handle, int* Axes, double* Point, ACSC_WAITBLOCK* Wait);


// The function adds a point to a single-axis multi-point or spline motion.
extern int _ACSCLIB_ WINAPI acsc_ExtAddPoint(HANDLE Handle, int Axis, double Point, double Rate, ACSC_WAITBLOCK* Wait);


// The function adds a point to a multi-axis multi-point or spline motion.
extern int _ACSCLIB_ WINAPI acsc_ExtAddPointM(HANDLE Handle, int* Axes, double* Point, double Rate, ACSC_WAITBLOCK* Wait);


// The function adds a point to a single-axis PVT spline motion.
extern int _ACSCLIB_ WINAPI acsc_AddPVTPoint(HANDLE Handle, int Axis, double Point, double Velocity,double TimeInterval, ACSC_WAITBLOCK* Wait);


// The function adds a point to a multi-axis PVT spline motion.
extern int _ACSCLIB_ WINAPI acsc_AddPVTPointM(HANDLE Handle, int *Axis, double *Point, double *Velocity,double TimeInterval, ACSC_WAITBLOCK* Wait);


// The function adds a point to a single-axis PV spline motion.
extern int _ACSCLIB_ WINAPI acsc_AddPVPoint(HANDLE Handle, int Axis, double Point, double Velocity, ACSC_WAITBLOCK* Wait);


// The function adds a point to a multi-axis PV spline motion.
extern int _ACSCLIB_ WINAPI acsc_AddPVPointM(HANDLE Handle, int *Axis, double *Point, double *Velocity, ACSC_WAITBLOCK* Wait);


// The function adds an array of points to a single-axis multi-point or spline motion.
extern int _ACSCLIB_ WINAPI acsc_AddMPoint(HANDLE Handle, int Axis, char* Name, int Count, ACSC_WAITBLOCK* Wait);


// The function adds an array of points to a multi-axis multi-point or spline motion.
extern int _ACSCLIB_ WINAPI acsc_AddMPointM(HANDLE Handle, int* Axes, char* Name, int Count, ACSC_WAITBLOCK* Wait);


// The function informs the controller, that no more points will be specified for the current single-axis motion.
extern int _ACSCLIB_ WINAPI acsc_EndSequence(HANDLE Handle, int Axis, ACSC_WAITBLOCK* Wait);


// The function informs the controller, that no more points or segments will be specified for the current multi-axis motion.
extern int _ACSCLIB_ WINAPI acsc_EndSequenceM(HANDLE Handle, int* Axes, ACSC_WAITBLOCK* Wait);


// The function initiates data collection.
extern int _ACSCLIB_ WINAPI acsc_Collect(HANDLE Handle, int Flags, char* Array, int NSample, int Period, char** Vars, ACSC_WAITBLOCK* Wait);

// The function initiates data collection.
extern int _ACSCLIB_ WINAPI acsc_CollectB(HANDLE Handle, int Flags, char* Array, int NSample, int Period, char* Vars, ACSC_WAITBLOCK* Wait);

// The function terminates data collection.
extern int _ACSCLIB_ WINAPI acsc_StopCollect(HANDLE Handle, ACSC_WAITBLOCK* Wait);


// The function retrieves the current motor state.
extern int _ACSCLIB_ WINAPI acsc_GetMotorState(HANDLE Handle, int Axis, int* State, ACSC_WAITBLOCK* Wait);


// The function retrieves the current axis state.
extern int _ACSCLIB_ WINAPI acsc_GetAxisState(HANDLE Handle, int Axis, int* State, ACSC_WAITBLOCK* Wait);


// The function retrieves the current state of  the index and mark variables.
extern int _ACSCLIB_ WINAPI acsc_GetIndexState(HANDLE Handle, int Axis, int* State, ACSC_WAITBLOCK* Wait);


// The function resets the specified bits of the index/mark state.
extern int _ACSCLIB_ WINAPI acsc_ResetIndexState(HANDLE Handle, int Axis, int Mask, ACSC_WAITBLOCK* Wait);


// The function retrieves the current state of  the program buffer.
extern int _ACSCLIB_ WINAPI acsc_GetProgramState(HANDLE Handle, int Buffer, int* State, ACSC_WAITBLOCK* Wait);


// The function retrieves the current state of  the specified digital input.
extern int _ACSCLIB_ WINAPI acsc_GetInput(HANDLE Handle, int Port, int Bit, int* Value, ACSC_WAITBLOCK* Wait);


// The function retrieves the current state of  the specified digital input port.
extern int _ACSCLIB_ WINAPI acsc_GetInputPort(HANDLE Handle, int Port, int* Value, ACSC_WAITBLOCK* Wait);


// The function retrieves the current state of  the specified digital output.
extern int _ACSCLIB_ WINAPI acsc_GetOutput(HANDLE Handle, int Port, int Bit, int* Value, ACSC_WAITBLOCK* Wait);


// The function retrieves the current state of  the specified digital output port.
extern int _ACSCLIB_ WINAPI acsc_GetOutputPort(HANDLE Handle, int Port, int* Value, ACSC_WAITBLOCK* Wait);


// The function sets the specified digital output to the specified value.
extern int _ACSCLIB_ WINAPI acsc_SetOutput(HANDLE Handle, int Port, int Bit, int Value, ACSC_WAITBLOCK* Wait);


// The function sets the specified digital output port to the specified value.
extern int _ACSCLIB_ WINAPI acsc_SetOutputPort(HANDLE Handle, int Port, int Value, ACSC_WAITBLOCK* Wait);


// The function retrieves the current numerical value of the specified analog inputs.
extern int _ACSCLIB_ WINAPI acsc_GetAnalogInput(HANDLE Handle, int Port, int* Value, ACSC_WAITBLOCK* Wait);


// The function retrieves the current numerical value of  the specified analog outputs.
extern int _ACSCLIB_ WINAPI acsc_GetAnalogOutput(HANDLE Handle, int Port, int* Value, ACSC_WAITBLOCK* Wait);


// The function writes the current numerical value to the specified analog outputs.
extern int _ACSCLIB_ WINAPI acsc_SetAnalogOutput(HANDLE Handle, int Port, int Value, ACSC_WAITBLOCK* Wait);


// The function retrieves the current state of  the specified extended input.
extern int _ACSCLIB_ WINAPI acsc_GetExtInput(HANDLE Handle, int Port, int Bit, int* Value, ACSC_WAITBLOCK* Wait);


// The function retrieves the current state of  the specified extended input port.
extern int _ACSCLIB_ WINAPI acsc_GetExtInputPort(HANDLE Handle, int Port, int* Value, ACSC_WAITBLOCK* Wait);


// The function retrieves the current state of  the specified extended output.
extern int _ACSCLIB_ WINAPI acsc_GetExtOutput(HANDLE Handle, int Port, int Bit, int* Value, ACSC_WAITBLOCK* Wait);


// The function retrieves the current state of  the specified extended output port.
extern int _ACSCLIB_ WINAPI acsc_GetExtOutputPort(HANDLE Handle, int Port, int* Value, ACSC_WAITBLOCK* Wait);


// The function sets the specified extended output to the specified value.
extern int _ACSCLIB_ WINAPI acsc_SetExtOutput(HANDLE Handle, int Port, int Bit, int Value, ACSC_WAITBLOCK* Wait);


// The function sets the specified extended output port to the specified value.
extern int _ACSCLIB_ WINAPI acsc_SetExtOutputPort(HANDLE Handle, int Port, int Value, ACSC_WAITBLOCK* Wait);


// The function retrieves the set of bits, that indicate the motor or system faults.
extern int _ACSCLIB_ WINAPI acsc_GetFault(HANDLE Handle, int Axis, int* Fault, ACSC_WAITBLOCK* Wait);


// The function retrieves the mask, that defines which controller faults are examined and processed.
extern int _ACSCLIB_ WINAPI acsc_GetFaultMask(HANDLE Handle, int Axis, int* Mask, ACSC_WAITBLOCK* Wait);


// The function sets the mask, that enables/disables the examination and processing of the controller faults.
extern int _ACSCLIB_ WINAPI acsc_SetFaultMask(HANDLE Handle, int Axis, int Mask, ACSC_WAITBLOCK* Wait);


// The function enables the specified motor or system fault.
extern int _ACSCLIB_ WINAPI acsc_EnableFault(HANDLE Handle, int Axis, int Fault, ACSC_WAITBLOCK* Wait);


// The function disables the specified motor or system fault.
extern int _ACSCLIB_ WINAPI acsc_DisableFault(HANDLE Handle, int Axis, int Fault, ACSC_WAITBLOCK* Wait);


// The function retrieves the mask, that defines for which motor or system faults the controller provides default response.
extern int _ACSCLIB_ WINAPI acsc_GetResponseMask(HANDLE Handle, int Axis, int* Mask, ACSC_WAITBLOCK* Wait);


// The function sets the mask, that defines for which motor or system faults the controller provides default response.
extern int _ACSCLIB_ WINAPI acsc_SetResponseMask(HANDLE Handle, int Axis, int Mask, ACSC_WAITBLOCK* Wait);


// The function enables the response to the specified motor or system fault.
extern int _ACSCLIB_ WINAPI acsc_EnableResponse(HANDLE Handle, int Axis, int Response, ACSC_WAITBLOCK* Wait);


// The function disables the default response to the specified motor or system fault.
extern int _ACSCLIB_ WINAPI acsc_DisableResponse(HANDLE Handle, int Axis, int Response, ACSC_WAITBLOCK* Wait);


// The function retrieves the current state of  the specified safety input.
extern int _ACSCLIB_ WINAPI acsc_GetSafetyInput(HANDLE Handle, int Axis, int Input, int* Value, ACSC_WAITBLOCK* Wait);


// The function retrieves the current state of  the specified safety input port.
extern int _ACSCLIB_ WINAPI acsc_GetSafetyInputPort(HANDLE Handle, int Axis, int* Value, ACSC_WAITBLOCK* Wait);


// The function retrieves the set of bits that define inversion for the specified safety input port.
extern int _ACSCLIB_ WINAPI acsc_GetSafetyInputPortInv(HANDLE Handle, int Axis, int* Value, ACSC_WAITBLOCK* Wait);


// The function sets the set of bits that define inversion for the specified safety input port.
extern int _ACSCLIB_ WINAPI acsc_SetSafetyInputPortInv(HANDLE Handle, int Axis, int Value, ACSC_WAITBLOCK* Wait);


// The function waits for the end of a motion.
extern int _ACSCLIB_ WINAPI acsc_WaitMotionEnd(HANDLE Handle, int Axis, int Timeout);


// The function waits for the logical end of a motion.
extern int _ACSCLIB_ WINAPI acsc_WaitLogicalMotionEnd(HANDLE Handle, int Axis, int Timeout);


// The function waits for the end of data collection.
extern int _ACSCLIB_ WINAPI acsc_WaitCollectEnd(HANDLE Handle, int Timeout);


// The function waits for the program termination in the specified buffer.
extern int _ACSCLIB_ WINAPI acsc_WaitProgramEnd(HANDLE Handle, int Buffer, int Timeout);


// The function waits for the specified state of the specified motor.
extern int _ACSCLIB_ WINAPI acsc_WaitMotorEnabled(HANDLE Handle, int Axis, int State, int Timeout);


// The function waits for the specified state of digital input.
extern int _ACSCLIB_ WINAPI acsc_WaitInput(HANDLE Handle, int Port, int Bit, int State, int Timeout);


// The function waits for user-defined condition.
extern int _ACSCLIB_ WINAPI acsc_WaitUserCondition(HANDLE Handle, ACSC_USER_CONDITION_FUNC UserCondition, int Timeout);


// The function installs a user-defined callback function for the specified interrupt condition
extern int _ACSCLIB_ WINAPI acsc_SetCallback(HANDLE Handle, ACSC_INTR_CALLBACK_FUNC Callback, int Interrupt);


// The function installs a user-defined callback function for the specified interrupt condition
// with specified card context
extern int _ACSCLIB_ WINAPI acsc_SetCallbackExt(HANDLE Handle, ACSC_INTR_CALLBACK_FUNC_EXT Callback,void* CardContext, int Interrupt);


// The function installs a user-defined callback function for the specified interrupt condition
// with specified card context
extern int _ACSCLIB_ WINAPI acsc_InstallCallback(HANDLE Handle, ACSC_USER_CALLBACK_FUNCTION Callback,void* CardContext, int Interrupt);


// The function sets the priority for all callback threads. 
extern int _ACSCLIB_ WINAPI acsc_SetCallbackPriority(HANDLE Handle, int Priority);


// The function sets the mask for specified interrupt
extern int _ACSCLIB_ WINAPI acsc_SetInterruptMask(HANDLE Handle, int Interrupt, unsigned int Mask);


// The function sets the mask for specified callback
extern int _ACSCLIB_ WINAPI acsc_SetCallbackMask(HANDLE Handle, int Interrupt, unsigned __int64 Mask);


// The function retrieves the mask for specified interrupt
extern int _ACSCLIB_ WINAPI acsc_GetInterruptMask(HANDLE Handle, int Interrupt, unsigned int* Mask);


// The function retrieves the mask for specified callback
extern int _ACSCLIB_ WINAPI acsc_GetCallbackMask(HANDLE Handle, int Interrupt, unsigned __int64* Mask);


// The function creates the persistent global variable.
extern int _ACSCLIB_ WINAPI acsc_DeclareVariable(HANDLE Handle, int Type, char* Name, ACSC_WAITBLOCK* Wait);


// The function deletes all persistent global variables.
extern int _ACSCLIB_ WINAPI acsc_ClearVariables(HANDLE Handle, ACSC_WAITBLOCK* Wait);


// The function retrieves the firmware version of the controller.
extern int _ACSCLIB_ WINAPI acsc_GetFirmwareVersion(HANDLE Handle, char* Version, int Count, int* Received, ACSC_WAITBLOCK* Wait);


// The function retrieves the controller serial number.
extern int _ACSCLIB_ WINAPI acsc_GetSerialNumber(HANDLE Handle, char* SerialNumber, int Count, int* Received, ACSC_WAITBLOCK* Wait);

// The function terminates a motion using reduced deceleration profile with Reason of kill.
extern int _ACSCLIB_ WINAPI acsc_KillExt(HANDLE Handle, int Axis,int Reason, ACSC_WAITBLOCK* Wait);

// The function disables motor and sets Reason of disable.
extern int _ACSCLIB_ WINAPI acsc_DisableExt(HANDLE Handle, int Axis,int Reason, ACSC_WAITBLOCK* Wait);

// The function retrieves the reason why the motor was disabled.
extern int _ACSCLIB_ WINAPI acsc_GetMotorError(HANDLE Handle, int Axis, int* Error, ACSC_WAITBLOCK* Wait);

// The function retrieves the termination code of the last executed motion of the specified axis.
extern int _ACSCLIB_ WINAPI acsc_GetMotionError(HANDLE Handle, int Axis, int* Error, ACSC_WAITBLOCK* Wait);

// The function retrieves the error code of the last program error encountered in the specified buffer.
extern int _ACSCLIB_ WINAPI acsc_GetProgramError(HANDLE Handle, int Buffer, int* Error, ACSC_WAITBLOCK* Wait);
////////////////////////////////////////////////////////////////////////////////////////////////
//These functions access directly DPRAM via PCI bus only
////////////////////////////////////////////////////////////////////////////////////////////////
extern int _ACSCLIB_ WINAPI acsc_ReadDPRAMInteger(HANDLE Handle, int index, int* result);
extern int _ACSCLIB_ WINAPI acsc_WriteDPRAMInteger(HANDLE Handle, int index, int data);
extern int _ACSCLIB_ WINAPI acsc_ReadDPRAMReal(HANDLE Handle, int index, double* result);
extern int _ACSCLIB_ WINAPI acsc_WriteDPRAMReal(HANDLE Handle, int index, double data);

///
//This function defines communication server IP address
///
extern int _ACSCLIB_ WINAPI acsc_SetServer(char *IP);

///
//This function defines communication server IP address and Port
///
extern int _ACSCLIB_ WINAPI acsc_SetServerExt(char *IP,int Port);
extern int _ACSCLIB_ WINAPI acsc_SetServerExtLogin(char *IP, int Port, char *Username, char *Password, char *Domain);

///
//These functions are related to Position Event Generation(PEG)
///
extern int _ACSCLIB_ WINAPI acsc_PegI(HANDLE Handle,int Flags,int Axis,double Width,int FirstPoint,int Interval,int LastPoint,int TbNumber,double TbPeriod, ACSC_WAITBLOCK* Wait);
extern int _ACSCLIB_ WINAPI acsc_PegR(HANDLE Handle,int Flags,int Axis,double Width,char* PointArray,char* StateArray,int TbNumber,double TbPeriod, ACSC_WAITBLOCK* Wait);

extern int _ACSCLIB_ WINAPI acsc_PegInc(HANDLE Handle,int Flags,int Axis,double Width,double FirstPoint,double Interval,double LastPoint,int TbNumber,double TbPeriod, ACSC_WAITBLOCK* Wait);
extern int _ACSCLIB_ WINAPI acsc_PegRandom(HANDLE Handle,int Flags,int Axis,double Width,char* PointArray,char* StateArray,int TbNumber,double TbPeriod, ACSC_WAITBLOCK* Wait);

extern int _ACSCLIB_ WINAPI acsc_AssignPins(HANDLE Handle,int Axis,unsigned short Mask, ACSC_WAITBLOCK* Wait);
extern int _ACSCLIB_ WINAPI acsc_StopPeg(HANDLE Handle,int Axis, ACSC_WAITBLOCK* Wait);

///
//These functions are used for reading data from text files
///
extern int _ACSCLIB_ WINAPI acsc_LoadFileToIntegerVariable(HANDLE Handle, int NBuf, char *Var, int From1, int To1, int From2, int To2, char* Filename, ACSC_WAITBLOCK* Wait);
extern int _ACSCLIB_ WINAPI acsc_LoadFileToRealVariable(HANDLE Handle, int NBuf, char *Var, int From1, int To1, int From2, int To2, char* Filename, ACSC_WAITBLOCK* Wait);

extern int _ACSCLIB_ WINAPI acsc_DownloadBinVariableToFile(HANDLE Handle, int NBuf, char *Var, int Size, int From1, int To1, int From2, int To2, char* Filename, char* Format, ACSC_WAITBLOCK* Wait);
extern int _ACSCLIB_ WINAPI acsc_SetLogFileOptions(HANDLE Handle,ACSC_LOG_DETALIZATION_LEVEL Detalization, ACSC_LOG_DATA_PRESENTATION Presentation);


// The function initiates local communication with SPiiPlus-PC Firmware
HANDLE _ACSCLIB_ WINAPI acsc_OpenCommLocalPC();


// The function initiates communication via CAN network
HANDLE _ACSCLIB_ WINAPI acsc_OpenCommCAN(int DeviceType, int Node_ID, int Port, int Rate, void* Attributes);



// to include later 
//
extern HANDLE _ACSCLIB_ WINAPI acsc_OpenCommSerial(int Channel, int Rate);
// for compatibility with the previous library
extern HANDLE _ACSCLIB_ WINAPI zl_OpenCommSerial(int Channel, int Rate);


// The function initiates communication via Ethernet
// function of the new library
extern HANDLE _ACSCLIB_ WINAPI acsc_OpenCommEthernet(char* Address, int Port);
// for compatibility with the previous library
extern HANDLE _ACSCLIB_ WINAPI zl_OpenCommEthernet(char* Address, int Port);


// The function initiates direct communication with simulator

// function of the new library
extern HANDLE _ACSCLIB_ WINAPI acsc_OpenCommDirect();
// for compatibility with the previous library
extern HANDLE _ACSCLIB_ WINAPI zl_OpenCommDirect();
void _ACSCLIB_ WINAPI acsc_GetTimes(__int64 Times[]);
void _ACSCLIB_ WINAPI acsc_ResetTimes();

// The functions collects IP addresses of SPiiPlus cards connected to the local segment
extern int _ACSCLIB_ acsc_GetEthernetCards(in_addr *IPaddresses,int Max, int *Ncontrollers,unsigned long BroadcastAddress);

// The function initiates communication via PCI device driver

extern HANDLE _ACSCLIB_ WINAPI acsc_OpenCommPCI(int SlotNumber);


// The function retrieves a PCI bus information for the SB1218PCI cards installed

extern int _ACSCLIB_ WINAPI acsc_GetPCICards(ACSC_PCI_SLOT* Cards, int Count, int* ObtainedCards);


// The function closes communication (for all kinds of communications)

// function of the new library
extern int _ACSCLIB_ WINAPI acsc_CloseComm(HANDLE Handle);
// for compatibility with the previous library
extern int _ACSCLIB_ WINAPI zl_CloseComm(HANDLE Handle);


// The function sends message

// function of the new library
extern int _ACSCLIB_ WINAPI acsc_Send(HANDLE Handle, char* Buf, int Count, ACSC_WAITBLOCK* Wait);
// for compatibility with the previous library
extern int _ACSCLIB_ WINAPI zl_Send(HANDLE Handle, char* Buf, int Count, int Timeout);


// The function receives message

// function of the new library
extern int _ACSCLIB_ WINAPI acsc_Receive(HANDLE Handle, char* Buf, int Count, int* Received, ACSC_WAITBLOCK* Wait);
// for compatibility with the previous library
extern int _ACSCLIB_ WINAPI zl_Receive(HANDLE Handle, char* Buf, int Count, int* Received, int Timeout);


// The function provides transaction: sends request and receives reply

// function of the new library
int	_ACSCLIB_ WINAPI acsc_Transaction(HANDLE Handle, char* OutBuf, int OutCount, char* InBuf, int InCount, int* Received, ACSC_WAITBLOCK* Wait);

// for compatibility with the previous library
extern int _ACSCLIB_ WINAPI zl_Transaction(HANDLE Handle, char* OutBuf, int OutCount, char* InBuf, int InCount, int* Received, int Timeout);


// The function sends a command to the controller and analyzes the controller response.

extern int _ACSCLIB_ WINAPI acsc_Command(HANDLE Handle, char* OutBuf, int OutCount, ACSC_WAITBLOCK* Wait);


// The function waits for completion of asynchronous call and retrieves a data.

extern int _ACSCLIB_ WINAPI acsc_WaitForAsyncCall(HANDLE Handle, void* Buf, int* Received, ACSC_WAITBLOCK* Wait, int Timeout);


// The function cancels any asynchronous (non-waiting) call

extern int _ACSCLIB_ WINAPI acsc_CancelOperation(HANDLE Handle, ACSC_WAITBLOCK* Wait);


// The function retrieves the last error code

// function of the new library
int	_ACSCLIB_ WINAPI acsc_GetLastError();
// for compatibility with the previous library
int	_ACSCLIB_ WINAPI zl_GetLastError(HANDLE Handle);


// The function retrieves the explanation of an error code. 

int	_ACSCLIB_ WINAPI acsc_GetErrorString(HANDLE Handle, int ErrorCode, char* ErrorStr, int Count, int* Received);


// The function sets communication call delay

extern int _ACSCLIB_ WINAPI acsc_SetQueueOverflowTimeout(HANDLE Handle, int Timeout);


// The function retrieves communication call delay 

extern int _ACSCLIB_ WINAPI acsc_GetQueueOverflowTimeout(HANDLE Handle);

// The function sets communication timeout

extern int _ACSCLIB_ WINAPI acsc_SetTimeout(HANDLE Handle, int Timeout);


// The function retrieves communication timeout

extern int _ACSCLIB_ WINAPI acsc_GetTimeout(HANDLE Handle);


// The function retrieves default communication timeout

extern int _ACSCLIB_ WINAPI acsc_GetDefaultTimeout(HANDLE Handle);


// The function sets the number of iterations of one transaction

extern int _ACSCLIB_ WINAPI acsc_SetIterations(HANDLE Handle, int Iterations);


// The function sets communication options

extern int _ACSCLIB_ WINAPI acsc_SetCommOptions(HANDLE Handle, unsigned int Options);


// The function retrieves communication options

extern int _ACSCLIB_ WINAPI acsc_GetCommOptions(HANDLE Handle, unsigned int* Options);


// The function retrieves ACSC Library version

unsigned int _ACSCLIB_ WINAPI acsc_GetLibraryVersion();


// The function loads ACSPL+ program to the specified program buffer.

extern int _ACSCLIB_ WINAPI acsc_LoadBuffer(HANDLE Handle, int Buffer, char* Text, int Count, ACSC_WAITBLOCK* Wait);


// The function loads ACSPL+ program to the specified program buffer.
// Service lines are ignored.

extern int _ACSCLIB_ WINAPI acsc_LoadBufferIgnoreServiceLines(HANDLE Handle, int Buffer, char* Text, int Count, ACSC_WAITBLOCK* Wait);


// The function loads ACSPL+ programs to the specified program buffers.
// From *.prg file

extern int _ACSCLIB_ WINAPI acsc_LoadBuffersFromFile(HANDLE Handle,  char* Filename, ACSC_WAITBLOCK* Wait);


// The function appends one or more ACSPL+ lines to the program in the specified program buffer.

extern int _ACSCLIB_ WINAPI acsc_AppendBuffer(HANDLE Handle, int Buffer, char* Text, int Count, ACSC_WAITBLOCK* Wait);


// The function downloads text to a program buffer. The function is similar to the function acsc_AppendBuffer() 

// function of the new library
extern int _ACSCLIB_ WINAPI acsc_DownloadBuffer(HANDLE Handle, int Buffer, char* Text, int Count, ACSC_WAITBLOCK* Wait);
// for compatibility with the previous library
extern int _ACSCLIB_ WINAPI zl_DownloadBuffer(HANDLE Handle, int NBuf, char* Text, int Count, int Timeout);


// The function uploads text from a program buffer 

// function of the new library
extern int _ACSCLIB_ WINAPI acsc_UploadBuffer(HANDLE Handle, int Buffer, int Offset, char* Text, int Count, int* Received, ACSC_WAITBLOCK* Wait);
// for compatibility with the previous library
extern int _ACSCLIB_ WINAPI zl_UploadBuffer(HANDLE Handle, int NBuf, int Offset, char* Text, int Count, int* Received, int Timeout);


// The function deletes the specified ACSPL+ program lines in the specified program buffer.

extern int _ACSCLIB_ WINAPI acsc_ClearBuffer(HANDLE Handle, int Buffer, int FromLine, int ToLine, ACSC_WAITBLOCK* Wait);


// The function compiles ACSPL+ program in the specified program buffer(s).

extern int _ACSCLIB_ WINAPI acsc_CompileBuffer(HANDLE Handle, int Buffer, ACSC_WAITBLOCK* Wait);


// The function starts up ACSPL+ program in the specified program buffer.

extern int _ACSCLIB_ WINAPI acsc_RunBuffer(HANDLE Handle, int Buffer, char* Label, ACSC_WAITBLOCK* Wait);


// The function stops ACSPL+ program in the specified program buffer(s).

extern int _ACSCLIB_ WINAPI acsc_StopBuffer(HANDLE Handle, int Buffer, ACSC_WAITBLOCK* Wait);


// The function suspends ACSPL+ program in the specified program buffer(s).

extern int _ACSCLIB_ WINAPI acsc_SuspendBuffer(HANDLE Handle, int Buffer, ACSC_WAITBLOCK* Wait);


// The function reads value(s) from integer variable

// function of the new library
extern int _ACSCLIB_ WINAPI acsc_ReadInteger(HANDLE Handle, int NBuf, char* Var, int From1, int To1, int From2, int To2, int* Values, ACSC_WAITBLOCK* Wait);
// for compatibility with the previous library
extern int _ACSCLIB_ WINAPI zl_ReadInteger(HANDLE Handle, int NBuf, char* Var, int From1, int To1, int From2, int To2, int* Values, int Timeout);


// The function writes value(s) to integer variable

// function of the new library
extern int _ACSCLIB_ WINAPI acsc_WriteInteger(HANDLE Handle, int NBuf, char* Var, int From1, int To1, int From2, int To2, int* Values, ACSC_WAITBLOCK* Wait);
// for compatibility with the previous library
extern int _ACSCLIB_ WINAPI zl_WriteInteger(HANDLE Handle, int NBuf, char* Var, int From1, int To1, int From2, int To2, int* Values, int Timeout);


// The function reads value(s) from real variable

// function of the new library
extern int _ACSCLIB_ WINAPI acsc_ReadReal(HANDLE Handle, int NBuf, char* Var, int From1, int To1, int From2, int To2, double* Values, ACSC_WAITBLOCK* Wait);
// for compatibility with the previous library
extern int _ACSCLIB_ WINAPI zl_ReadReal(HANDLE Handle, int NBuf, char* Var, int From1, int To1, int From2, int To2, double* Values, int Timeout);


// The function writes value(s) to real variable

// function of the new library
extern int _ACSCLIB_ WINAPI acsc_WriteReal(HANDLE Handle, int NBuf, char* Var, int From1, int To1, int From2, int To2, double* Values, ACSC_WAITBLOCK* Wait);
// for compatibility with the previous library
extern int _ACSCLIB_ WINAPI zl_WriteReal(HANDLE Handle, int NBuf, char* Var, int From1, int To1, int From2, int To2, double* Values, int Timeout);


// The function captures communication

// function of the new library
extern int _ACSCLIB_ WINAPI acsc_CaptureComm(HANDLE Handle);
// for compatibility with the previous library
extern int _ACSCLIB_ WINAPI zl_CaptureComm(HANDLE Handle);

// The function releases communication

// function of the new library
extern int _ACSCLIB_ WINAPI acsc_ReleaseComm(HANDLE Handle);
// for compatibility with the previous library
extern int _ACSCLIB_ WINAPI zl_ReleaseComm(HANDLE Handle);

// The function initiates history buffer
// function of the new library
LP_ACSC_HISTORYBUFFER _ACSCLIB_ WINAPI acsc_OpenHistoryBuffer(HANDLE Handle, int Size);
// for compatibility with the previous library
LP_ZL_HISTORYBUFFER _ACSCLIB_ WINAPI zl_OpenHistoryBuffer(HANDLE Handle, int Size);


// The function closes history buffer

// function of the new library
extern int _ACSCLIB_ WINAPI acsc_CloseHistoryBuffer(HANDLE Handle);
// for compatibility with the previous library
extern int _ACSCLIB_ WINAPI zl_CloseHistoryBuffer(HANDLE Handle);
// The function retrieves the contents of the history buffer
extern int _ACSCLIB_ WINAPI acsc_GetHistory(HANDLE Handle, char* Buf, int Count, int* Received, BOOL bClear);
// for compatibility with the previous library
extern int _ACSCLIB_ WINAPI zl_GetHistory(HANDLE Handle, char* Buf, int Count, int* Received, BOOL bClear);

#endif  // ACS_EFSIKA_APP