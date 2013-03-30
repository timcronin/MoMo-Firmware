
#ifndef __interrupts_h__
#define __interrupts_h__

#include <p24F16KA101.h>

#define CLEAR_PRI(reg, lsb)     ((*reg) ^= (0b111 << lsb))
#define SET_PRI(reg, lsb, pri)  ((*reg) |= (pri&0b111) << lsb)     //CLEAR_PRI must first be called

//Routines
void configure_interrupts();

#endif
