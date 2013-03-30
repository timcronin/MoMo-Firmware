/*
 * timer.h
 * Control module for the PIC24 timer 1, 2 and 3
 */

#ifndef __timer_h__
#define __timer_h__

#include "../core/task_manager.h"
#include "../util/interrupts.h"

#define kTimerPrescaleMask 					0b11
#define PackTimerPrescale(flags, prescale) 	flags |= ((prescale&kTimerPrescaleMask) << kPrescaleShift)	//flags must have these bits starting as 0
#define TimerIFSRegister					IFS0
#define TimerIECRegister					IEC0
#define Timer1IPCRegister					IPC0
#define Timer2IPCRegister					IPC1
#define Timer3IPCRegister					IPC2

//Utility Functions (for code clarity)
#define PackTimerControlBits(ifs, ipc, ie)	(ifs&0b1111)<<8 | (ipc&0b1111)<<4 | (ie&0b1111)
#define TimerIFSBit(pack)					((pack>>8)&0b1111)
#define TimerIECBit(pack)					(pack&0b1111)
#define TimerIPCBit(pack)					((pack>>4)&0b1111)

#define ClearTimerIE(spec)					BIT_CLEAR(TimerIECRegister, TimerIECBit(spec->control_bits))
#define SetTimerIE(spec)					BIT_SET(TimerIECRegister, TimerIECBit(spec->control_bits))
#define ClearTimerFlag(spec)				BIT_CLEAR(TimerIFSRegister, TimerIFSBit(spec->control_bits))
#define ClearTimerPriority(spec)			CLEAR_PRI(spec->priority, TimerIPCBit(spec->control_bits))
#define SetTimerPriority(spec)				SET_PRI(spec->priority, TimerIPCBit(spec->control_bits))	//ClearTimerPriority must be called first

//Register bit offsets
enum
{
	kTimer1IPCBit = 12,
	kTimer1IFSBit = 3, //same as IECBit for all timers
	kTimer2IPCBit = 12,
	kTimer2IFSBit = 7,
	kTimer3IPCBit = 0,
	kTimer3IFSBit = 8,
};

//Timer control flags
enum
{
	k32BitModeEnabled = 1,
	kDisableInIdle = 2,
	kGatedTimeMode = 3,
	kExternalTimeSource = 4,
	kSynchronizeEnabled = 6,
	kPrescaleShift = 7
};

//Timer Capability Flags
{
	k32BitModeSupported = 0,
	kSynchronizeSupported = 1
}

//Bit offset of timer bits
enum
{
	kTimerSourceBit = 1,
	kTimerSyncBit = 2,
	kTimer32BitBit = 3,
	kTimerPrescaleShift = 4,
	kTimerGateBit = 6,
	kTimerIdleBit = 13,
	kTimerOnBit = 15
};

typedef struct
{
	const volatile unsigned int 	*control;
	const volatile unsigned int 	*period_h;
	const volatile unsigned int   	*period_l;
	const volatile unsigned int 	*priority;

	const unsigned int				control_bits;

	unsigned char 					flags;
	unsigned char					status;

	task_callback		  callback;
} TimerSpec;

//Module Control Functions
int timer_configure(TimerSpec *timer, unsigned char flags);
int timer_set_period(TimerSpec *timer, unsigned int low, unsigned int high=0);
int timer_set_callback(TimerSpec *timer, task_callback callback);
int timer_set_status(TimerSpec *timer, unsigned int status);

#endif