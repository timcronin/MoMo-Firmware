//timer.c
#include "timer.h"

TimerSpec Timer1 = 
{
	/*control*/
	/*periodlo*/
	/*periodh*/
	/*flags*/
	/*status*/
	/*callback*/

int timer_configure(TimerSpec *timer, unsigned char flags);
int timer_set_period(TimerSpec *timer, unsigned int low, unsigned int high=0);
int timer_set_callback(TimerSpec *timer, task_callback callback);
int timer_set_status(TimerSpec *timer, unsigned int status);
