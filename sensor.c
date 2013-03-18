#include <stdio.h>
#include <p24F16KA101.h>
#include "rtcc.h"
#include "sensor.h"
#include "serial_commands.h"
#include "utilities.h"

#define I2C1_RETRY_MAX 5
#define I2C_TIMEOUT 10000 //test this, may not need timeout
volatile unsigned char I2C1_RX_BUF[16];
volatile unsigned char I2C1_TX_BUF[16];

volatile char I2C1_NACK_F;	//if no ack, be 1
volatile char I2C1_MASTER_F;	//if occured i2c interrupt as master, 1

static unsigned char SENSOR_FLAG;
static unsigned char SENSOR_TIMEOUT_FLAG;
static unsigned long pulse_counts;

#define SENSOR_TIMER_ON T2CONbits.TON
//static unsigned char SENSOR_BUF[5];

//start based on interrupt
void __attribute__((interrupt,no_auto_psv)) _INT2Interrupt() {
  _INT2IF = 0; //clear INT2 interrupt flag
  SENSOR_FLAG = 1; //set high when interrupt detected
}

//timeout interrupt flag
void __attribute__((interrupt,no_auto_psv)) _T2Interrupt() {
  _T2IF = 0; //clear timer interrupt flag
  SENSOR_TIMEOUT_FLAG = 1;
}
//sample sensor: increment when there's an interrupt
void sample_sensor() {
  pulse_counts = 0;
  SENSOR_TIMER_ON = 1; 
  while(!SENSOR_TIMEOUT_FLAG) { 
    if(SENSOR_FLAG) {
      SENSOR_FLAG = 0; //clear interrupt flag
      pulse_counts++;
    }
  }
  SENSOR_TIMEOUT_FLAG = 0; 
  SENSOR_TIMER_ON = 0;
  
}

void configure_sensor() {
    _INT2EP = 1; //set INT2 for negedge detect
    _INT2IE = 1; //Set INT2 to enable
    _T2IE = 1; //enable Timer 2 interrupts
    _T3IE = 1; //enable Timer 3 interrupts
    T2CONbits.TCKPS = 3;
    T3CONbits.TCKPS = 3;
    T2CONbits.T32 = 1; //set T2 and T3 to be one 32 bit counter
}

void goto_sleep() {
  asm_sleep();
}