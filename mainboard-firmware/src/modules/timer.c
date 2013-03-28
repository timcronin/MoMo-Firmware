//timer.c

#include "timer.h"

const volatile unsigned int 	*control;
const volatile unsigned int 	*period_h;
const volatile unsigned int   	*period_l;
const volatile unsigned int 	*priority;

const unsigned int				control_bits;

unsigned char 			flags;
unsigned char			status;

task_callback		  callback;

TimerSpec Timer1Spec = 
{
	T1CON,
	0,
	PR1,
	Timer1IPCRegister,
	PackTimerControlBits( kTimer1IFSBit, kTimer1IPCBit, kTimer1IFSBit),
	0,
	0,
	0
}