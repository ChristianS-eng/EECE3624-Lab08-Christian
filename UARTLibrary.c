/*****************************************************************************
 *	    File: UARTLibrary.c
 *  Lab Name: (Needed for Lab07 and Lab08)
 *    Author: Dr. Greg Nordstrom
 *   Created: 3/3/2019
 *  Modified: Christian Sorensen
 *  Mod Date: 10/24/2026
 * Processor: ATmega128A (on the ReadyAVR board)
 *
 * NOTE: This library only works with the default USART1 port
 * on the ReadyAVR (so JP2 must have jumpers on PD3 and PD2).
 */

#include <avr/io.h>
#include "UARTLibrary.h"

// Use uart_init to initialize the USART hardware in the AVR processor.
// The function takes the following arguments:
//    - desired baud rate (standard values from 2400 to 115200),
//    - number of databits (7 or 8),
//    - number of stop bits (1 or 2),
//    - parity (0 = no parity, 1 = odd parity, 2 = even parity).
// No error checking is performed and the function does not return a value.

void uart_init(long unsigned int baudrate, unsigned char databits, unsigned char stopbits, unsigned char parity)
{	
	// scale down
	long unsigned int baud_prescale = F_OSC / (baudrate * 16UL) - 1;
	
	// set baud rate (via UBRR1 high and low registers)
	UBRR1H = (baud_prescale >> 8);
	UBRR1L = baud_prescale;
	
	// set databits
	switch (databits)
	{
		case 7: UCSR1C = (1 << UCSZ11); break;
		case 8: UCSR1C = (1 << UCSZ11) | (1 << UCSZ10); break;
	}
	
	// set parity
	switch (parity)
	{
		case 0: break; // no parity
		case 1: UCSR1C |= (1 << UPM11) | (1 << UPM10); break; // odd parity
		case 2: UCSR1C |= (1 << UPM11); break; // even parity
	}
	
	// set stopbits
	switch (stopbits)
	{
		case 1: break;
		case 2: UCSR1C |= (1 << USBS1); break;
	}
	
	// enable receiver and transmitter
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);
}

// This function waits for space in the USART transmit buffer. When space
// is available, the function places an unsigned character value (passed in
// as the function's only argument) in the buffer and returns. No error
// checking is performed and the function does not return a value.
void uart_tx(unsigned char data)
{
	while(!(UCSR1A & (1 << UDRE1)));
	UDR1 = data;
}

// This function blocks until a character is received in the USART input
// buffer. Once a character arrives, it is echoed back to sender and
// passed back to the caller as an unsigned character.
// No error checking is performed.
// Returns ch to main to watch for special cases.
unsigned char uart_rx(void)
{
    while (!(UCSR1A&(1<<RXC1)));
	unsigned char ch = UDR1;
	uart_tx(ch);
	return ch;
}