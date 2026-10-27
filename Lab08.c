/*****************************************************************************
 *	    File: Lab08.c
 *  Lab Name: RS232 Communication Server
 *    Author: Dr. Greg Nordstrom
 *  Mod Date: 10/24/2026
 *  Modified: Christian Sorensen
 *  Mod Date: 10/24/2026
 * 
 * Processor: ATmega128A (on the ReadyAVR board)
 *
 * This program implements a communication server. Normally the server
 * simply echoes what was sent. However, there are two cases where
 * different behavior occurs:
 *    1. Pressing the ESC causes a soft reset (redisplays the title text)
 *    2. Typing the characters 'end' contiguously displays 'Bye' and halts
 *
 *****************************************************************************/

// define CPU speed (7.3728 MHz for the ATmega128A)
#define F_CPU   7372800UL

#define CR          0x0D
#define LF          0x0A
#define ESC         0x1B

#include <avr/io.h>
#include <util/delay.h>
#include "UARTLibrary.h" 

#define BAUDRATE 9600 // standard values from 1200 to 115200
#define DATABITS    8 // 7 or 8
#define STOPBITS    1 // 1 or 2
#define PARITY      0 // 0 = none, 1 = odd, 2 = even

void display_status_message();
void farewell();

int main(void)
{
	uart_init(BAUDRATE, DATABITS, STOPBITS, PARITY);	
	display_status_message();
	
	int state = 0, locked = 0;
	
	DDRC = 0xFF;
	PORTC = 0xFF;
	
	while(1)
	{
		int character = uart_rx();
		
		if (state == 0 && !locked)
		{
			if (character == 'e')
				state = 1;
			locked = 1;
		}
		
		if (state == 1 && !locked)
		{
			if (character != 'n')
				state = 0;
			
			if (character == 'e')
				state = 1;
				
			if (character == 'n')
				state = 2;
				
			locked = 1;
		}
		
		if (state == 2 && !locked)
		{
			if (character != 'd')
				state = 0;
				
			if (character == 'e')
				state = 1;
				
			if (character == 'd')
				state = 3;
		}
			
		if (state == 3)
			break;
		
		if (character == ESC)
			display_status_message();
			
		if (character == CR)
			uart_tx(LF);
			
		locked = 0;
		
		PORTC = 0xFE;
		_delay_ms(75);
		PORTC = 0xFF;
	}
	
	farewell();
	
	return 0;
}

void display_status_message()
{
	const char classMsg[] = "\r\n\r\nRS232 Communications Server\r\n      Status: Active\r\n    Christian Sorensen\r\n         EECE3624\r\n        Fall  2026\r\n\r\n\0";
	for (int i = 0; classMsg[i] != '\0'; i++)
		uart_tx(classMsg[i]);
}	

void farewell()
{
	const char classMsg[] = "\r\nBye\0";
	for (int i = 0; classMsg[i] != '\0'; i++)
		uart_tx(classMsg[i]);
}