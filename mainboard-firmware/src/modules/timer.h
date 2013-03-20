/*
 * timer.h
 * Control module for the PIC24 timer 1, 2 and 3
 */

#ifndef __timer_h__
#define __timer_h__

#include "../core/task_manager.h"

#define kTimerPrescaleMask 					0b11
#define PackTimerPrescale(flags, prescale) 	flags |= ((prescale&kTimerPrescaleMask) << kPrescaleShift)	//flags must have these bits starting as 0

//Timer control flags
enum
{
	k32BitModeSupported = 0,
	k32BitModeEnabled = 1,
	kDisableInIdle = 2,
	kGatedTimeMode = 3,
	kExternalTimeSource = 4,
	kSynchronizeSupported = 5,
	kSynchronizeEnabled = 6,
	kPrescaleShift = 7
};

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
	volatile unsigned int 	*control;
	volatile unsigned int 	*period_h;
	volatile unsigned int   *period_l;

	unsigned char 			flags;
	unsigned char			status;

	task_callback		  callback;
} TimerSpec;

//Module Control Functions
int timer_configure(TimerSpec *timer, unsigned char flags);
int timer_set_period(TimerSpec *timer, unsigned int low, unsigned int high);
int timer_set_callback(TimerSpec *timer, task_callback callback);
int timer_set_status(TimerSpec *timer, unsigned int status);

#endif