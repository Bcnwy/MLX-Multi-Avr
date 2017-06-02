/*
UART header file 
For sending serial commands using the hardware UART in the ATmega88A
*/
#ifndef uart_h
#define uart_h
#include "config.h"

#ifndef  F_CPU
//#define F_CPU 16000000UL
#error "Uart F_CPU not defined"
#endif

//#define BAUD_PRESCALLER (((F_CPU + (8UL*BAUDRATE))  / (BAUDRATE * 16UL)) - 1)
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1) 
#define HEX 16
#define DEC 10
#include <stdio.h>
//Declaration of functions
// USART class for sending bytes and chars in HEX or Decimal over serial
class USART
{
	public:
		void init(int BAUDRATE = 9600); 
		bool available(void);
		unsigned char read(void);
		char* readln(void);
		bool flush(void);
		void send(uint8_t data);
		void send(char data) {send((uint8_t)data); };
		void send(uint8_t* data) {send((uint8_t)*data); };
			
		void send(int data);
		void send(int data,int base);
	
		void send(const char* StringPtr);
		void send(uint8_t data, int base);
		void send(double data, int base);
			
		void sendln(void) {send("\r\n");};
		void sendln(const char* c) {send(c);send("\r\n");};
		void sendln(char c)   {send(c);send("\r\n");};
		//void sendln(unsigned int data, uint8_t base) {send(data,base);send("\n\r");};	
		//no protected or private functions left undeclared

	protected:
	private:
};
extern USART Serial; //Re-name class for external use
// USART Serial;
#endif