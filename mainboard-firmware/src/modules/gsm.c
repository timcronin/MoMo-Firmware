/*
 * File:   sms_commands.c
 * Author: Austin
 *
 * Created on November 18, 2012, 11:50 PM
 */

#include "uart.h"
#include "rtcc.h"
#include "common.h"
#include "pme.h"
#include "gsm.h"

void gsm_init()
{
    //Configure pins to control WISMO power and module enable
    GSM_POWER_PIN = 0;
    GSM_MODULE_ON_PIN = 1;
    GSM_MODULE_ON_OD = 1;
    
    //Set direction to out
    GSM_MODULE_ON_TRIS = 0;
    GSM_POWER_TRIS = 0;
}

int gsm_send_at_cmd( const char* cmd )
{
    sends( U1, cmd );
    sends( U1, "\r\n" );
}

void gsm_send_sms( const char* destination, const char* message )
{
    sends(U1, "AT+CMGS=\"");
    sends(U1, destination );
    sends(U1, "\"\r");
    wait_ms( 1 ); // TODO: Wait for the > char on U1
    sends( U1, message );
    put( U1, 0x1A ); // send ctrl-z
}

void gsm_configure_serial()
{
    uart_parameters params_uart1;

    //Clock enter high-speed mode

    peripheral_enable(kUART1Module);

    params_uart1.baud = 115200;
    params_uart1.hw_flowcontrol = 0;
    params_uart1.parity = NoParity;
    configure_uart( U1, &params_uart1 );
}

void gsm_on()
{
    gsm_configure_serial();

    GSM_POWER_ON();
    wait_ms( 1 );
    GSM_MODULE_ON_PIN = 0;
}

void gsm_off()
{
    GSM_POWER_OFF();
    peripheral_disable(kUART1Module);
    //Clock leave high speed mode
}