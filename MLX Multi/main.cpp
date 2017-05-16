/*
 * MLX Multi.cpp
 *
 * Created: 12/05/2017 23:25:52
 * Author : Conway
 */ 
#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "uart.h"
#include "Adafruit_MLX90614.h"
#include "HCSR04.h"

Adafruit_MLX90614 MLX_5a(0x5A);
Adafruit_MLX90614 MLX_5b(0x5B);
Adafruit_MLX90614 MLX_5c(0x5C);
Adafruit_MLX90614 MLX_5d(0x5D);
//Adafruit_MLX90614 MLX_5d(0x5d);

HC_SR04 sonic;

volatile unsigned long int count = 0; 
ISR(TIMER0_COMPA_vect)
{
	count++;
}

ISR(TIMER1_OVF_vect){
	//PORTB ^= _BV(PORTB5);
	//TIFR1 = 0;
}
/*********************************************************************/
unsigned long int millis(void)
{
	return count;
}
/*********************************************************************/
void setup(void)
{		
	cli();
	uint8_t rflag = MCUSR;
	MCUSR = 0;
	
	//DDRB |= _BV(PORTB5);
	//PORTB &= ~_BV(PORTB5);
	
	TCNT0 = 0;
	TCNT1 = 0;
	TCCR0A |= (1<<WGM01);
	OCR0A = 249;
	TIMSK0 |= (1 << OCIE0A);
	TIMSK1 |= _BV(TOIE1);
	TCCR0B |= (1<<CS01) | (1<<CS00);
	TCCR1B |= (1<<CS11);
	sei();
	Serial.init(9600);
	sonic.init();
	#ifdef _DEBUG
		Serial.send( rflag ,HEX);
		Serial.sendln("> Booting...");	
	#endif // _DEBUG
}
/*********************************************************************/
void IR_sensorRead(void )
{
	char str[25];
	char str_out[100] = {""};
	double obj_1,obj_2,obj_3,obj_4,amb_1,amb_2,amb_3,amb_4;
	unsigned long int m_read_time = 0;

	sonic.read();
	//Read 0x5A
	m_read_time = millis();
	#ifdef _DEBUG
		//Serial.sendln("> Read 0x5a...");
	#endif
				
	obj_1 = MLX_5a.readObjectTempC();
	amb_1 = MLX_5a.readAmbientTempC();
	//Read 0x5B
	obj_2 = MLX_5b.readObjectTempC();
	amb_2 = MLX_5b.readAmbientTempC();
	//Read 0x5C
	obj_3 = MLX_5c.readObjectTempC();
	amb_3 = MLX_5c.readAmbientTempC();
	//Read 0x5D
	obj_4 = MLX_5d.readObjectTempC();
	amb_4 = MLX_5d.readAmbientTempC();	
	
	sprintf(str, "T%d,%0.1f,%0.1f,%i,%lu\r\n", 1, obj_1, amb_1, (int)sonic.Distance,m_read_time);
	strcat(str_out,str);

	sprintf(str, "T%d,%.1f,%0.1f,%i,%lu\r\n", 2, obj_2, amb_2, (int)sonic.Distance,m_read_time);
	strcat(str_out,str);

	sprintf(str, "T%d,%0.1f,%0.1f,%i,%lu\r\n", 3, obj_3, amb_3, (int)sonic.Distance,m_read_time);
	strcat(str_out,str);
	
	sprintf(str, "T%d,%0.1f,%0.1f,%i,%lu\r\n", 4, obj_4, amb_4, (int)sonic.Distance,m_read_time);
	strcat(str_out,str);
	
	Serial.send(str_out);
}
/*********************************************************************/
int main(void)
{	
	setup();
	while (1){
		unsigned long start_time;
	    start_time = millis();	
		#ifdef _DEBUG
			Serial.sendln("> Read Sensors...");
		#endif

		IR_sensorRead();
		//_delay_ms(250);		
		while((millis()-start_time) <= 250);
	}
}
/*********************************************************************/
