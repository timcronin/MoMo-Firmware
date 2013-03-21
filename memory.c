#include "memory.h"
#include "sensor.h"
#include "serial_commands.h"
#include "rtcc.h"

//Buffer to read SPI to
//static unsigned char TX_BUF[140];
volatile unsigned long next_free;
volatile unsigned long next_read;
static char SPI1_FLAG;

//configure SPI
void configure_SPI() {
  SPI1CON1bits.MODE16 = 0; //communication is byte-wide
  SPI1CON1bits.MSTEN = 1; //SPI is in master mode
  _SPI1IE = 1; //enable SPI interrupts
  /*  TRISBbits.B15 = 0;
  TRISBbits.B14 = 1; //SDI is input
  TRISBbits.B13 = 0;
  TRISBbits.B12 = 0; */
  _TRISB15 = 0;
}

void __attribute__((interrupt,no_auto_psv)) _SPI1Interrupt() {
  _SPI1IF = 0;
  SPI1_FLAG = 1;

}
//Write a value to EEPROM
void mem_write(int addr, int val) {
  //Write to
  //Send WREN
  int timeout_ct = 0;
  int i = 0;
  int k = 0;
  //SEND val
  SPI1STATbits.SPIEN = 1;
  sendf(U2, "addr = %d \r\n", addr);
  SPI1BUF = 0x02;
  _LATB15 = 0; //pull SS low
  SPI1BUF = 0x02;
  while((!_SPI1IF) && timeout_ct < 1000)
    timeout_ct++; //wait while busy
  while(k++ < 200); //delay
  _LATB15 = 1; //toggle SS high
  _LATB15 = 0;
  //send address bytes MSB first
  for(i = 2; i > 0; i--) { 
    SPI1BUF = (addr >> i * 8) & 0xFF;
    timeout_ct = 0;
    while((SPI1STATbits.SPITBF) && timeout_ct < 1000)
      timeout_ct++; //wait while busy
    sendf(U2, "timeout_ct = %d\r\n", timeout_ct);
    if (timeout_ct == 1000)
      sends(U2, "Memory timed out on address\r\n");
  }

  //send values LSB first (because have to correspond to addresses)
  timeout_ct = 0;
  for(i = 0; i < 3; i++) { 
    SPI1BUF = (val >> i * 8) & 0xFF;
    timeout_ct = 0;
    while((SPI1STATbits.SPITBF) && timeout_ct < 1000)
      timeout_ct++; //wait while busy
    sendf(U2, "timeout_ct = %d\r\n", timeout_ct);
    if (timeout_ct == 1000)
      sends(U2, "Memory timed out on data\r\n");
  }
  SPI1STATbits.SPIEN = 0;
  //finish
  next_free++;
}

long mem_read(int addr) {
  //Write to
  //Send WREN
  int timeout_ct;
  int i = 0;
  unsigned long val = 0;
  //SEND val
  SPI1STATbits.SPIEN = 1;

  SPI1BUF = 0x02;
  while((SPI1STATbits.SPITBF) && timeout_ct < 1000)
    timeout_ct++; //wait while busy
  sendf(U2, "next_read = %d \r\n", next_read);
  //send address bytes MSB first
  for(i = 2; i > 0; i--) { 
    SPI1BUF = (next_read >> i * 8) & 0xFF;
    timeout_ct = 0;
    while((SPI1STATbits.SPITBF) && timeout_ct < 1000)
      timeout_ct++; //wait while busy
    sendf(U2, "timeout_ct = %d\r\n", timeout_ct);
    if (timeout_ct == 1000)
      sends(U2, "Memory timed out on address\r\n");
  }

  //send values LSB first (because have to correspond to addresses)
  timeout_ct = 0;
  for(i = 0; i < 3; i++) { 
    SPI1BUF = 0; //write 0 to initiate write to SPI1BUF
    timeout_ct = 0;
    while((SPI1STATbits.SPITBF) && timeout_ct < 1000)
      timeout_ct++; //wait while busy
    sendf(U2, "timeout_ct = %d\r\n", timeout_ct);
    if (timeout_ct == 1000)
      sends(U2, "Memory timed out on data\r\n");
    val = val | ((0xFF & SPI1BUF) << i * 8);
  }

  //finish
  SPI1STATbits.SPIEN = 0;
  next_read++;
  return val;
}
