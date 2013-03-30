//timer.c
#include "timer.h"


int timer_configure(TimerSpec *timer, unsigned char flags);
int timer_set_period(TimerSpec *timer, unsigned int low, unsigned int high=0);
int timer_set_callback(TimerSpec *timer, task_callback callback);
int timer_set_status(TimerSpec *timer, unsigned int status);


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
