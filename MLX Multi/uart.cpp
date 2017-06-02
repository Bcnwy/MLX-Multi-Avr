/*
Serial converting functions to output data in bytes over serial (TXRX) connection
*/
//Include libraries
#include <avr/io.h>
#include "uart.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>

char buff[32];

USART Serial; //Initialize new instance of the serial class
//Setup Baud rate in header file
//set to 8 bits, no stop bit, no parity
ISR(USART_RX_vect){}

/*********************************************************************/
void USART::init(int BAUDRATE){
	UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRR0L = (uint8_t)(BAUD_PRESCALLER);
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1 << RXCIE0); // Enable the USART Receive Complete interrupt (USART_RXC)
	UCSR0C =
	/* no parity bit */
	(0 << UPM01) |
	(0 << UPM00) |
	/* asynchronous USART */
	(0 << UMSEL01) |
	(0 << UMSEL00) |
	/* one stop bit */
	(0 << USBS0) |
	/* 8-bits of data */
	(1 << UCSZ01) |
	(1 << UCSZ00);

	sei();
}
/*********************************************************************/
bool USART::available(void)
{
	 if (UCSR0A & (1<<RXC0)) {
		 return true;
	 }
	 return false;
}
/*********************************************************************/
bool USART::flush(void)
{
	unsigned char dummy;
	while ( UCSR0A & (1<<RXC0) ) dummy = UDR0;
	dummy = dummy; // suppress compiler warning
	return false;
}
/*********************************************************************/
unsigned char USART::read(void){

	while(!(UCSR0A & (1<<RXC0))); //wait until data is in buffer
	return UDR0;// read buffer
}
char* USART::readln(void){
	char inbyte;
	for (int i= 0; i< 32; i++){
		inbyte = Serial.read();
		if (inbyte != 0x0D && inbyte != 0x0A){
			buff[i] = inbyte;
		}
		else {return 0;}	
	}
	return buff;
}

/*********************************************************************/
void USART::send(uint8_t data){

	while(!(UCSR0A & (1<<UDRE0))); //wait until buffer is empty
	UDR0 = data; //Load data into buffer
}
/*********************************************************************/
//send one char at a time
void USART::send(const char* StringPtr)
{
	while(*StringPtr != 0x00)
	{
		send(*StringPtr++);
	}
}
/*********************************************************************/
void USART::send(int data)
{
	char str[5];
	sprintf(str,"%i",data);
	send(str);
}
void USART::send(int data, int base)
{
	char str[8];
	if (base == DEC){
		sprintf(str,"%d",data);
		}else{
		sprintf(str,"%x",data);
	}
	send(str);
}
/*********************************************************************/
//convert int to ASCII number, display in decimal or hex
void USART::send(uint8_t data, int base)
{
	char str[5];
	if (base == DEC){
		sprintf(str,"%d",data);
	}else{
		sprintf(str,"%x",data);	
	}
	send(str);
}
/*********************************************************************/
void USART::send(double data,int base)
{
	double intr;
	int frac = modf (data, &intr);
	send((int)intr,base);

	send(".");
	frac = (frac+0.005) * 100;
	send((uint8_t)frac,base);
}
 /*********************************************************************/