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

Adafruit_MLX90614 MLX_5a(0x5a);
Adafruit_MLX90614 MLX_5b(0x5b);
Adafruit_MLX90614 MLX_5c(0x5c);
//Adafruit_MLX90614 MLX_5d(0x5d);

HC_SR04 sonic;

volatile unsigned long int count = 0; 
ISR(TIMER0_COMPA_vect)
{
	cli();
	count++;
	sei();
}
/*********************************************************************/
unsigned long int millis(void)
{
	unsigned long int d = count;
	return d;
}
/*********************************************************************/
int main(void)
{
	char str[16];
	double obj;
	double amb;
	unsigned long start_time;
	TCNT0 = 0;
	TCNT1 = 0;
	EIMSK |= (_BV(INT1) | _BV(INT0));
	EICRA |= (_BV(ISC01) | _BV(ISC00) | _BV(ISC11) | _BV(ISC10));
	TCCR0A |= (1<<WGM01);
	TCCR1A |= (1<<WGM12);
	OCR0A = 249;
	OCR1AH = 0xFF;
	OCR1AL = 0xFF;
	TIMSK0 |= (1 << OCIE0A);
	TIMSK1 |= (1 << OCIE1A);
	TCCR0B |= (1<<CS01) | (1<<CS00);
	TCCR1B |= (1<<CS11);
	sei();
	Serial.init(9600);
	sonic.init();
	Serial.sendln("> Booting...");

	while (1) 
    {	
		start_time = millis();
		Serial.sendln("> Read sensors");
		//sonic.read();
		obj = MLX_5a.readObjectTempC();
		amb = MLX_5a.readAmbientTempC();
		sprintf(str, "T%d,%0.1f,%0.1f,%i\r\n", 1, obj, amb, (int)sonic.Distance);
		Serial.send(str);
		obj = MLX_5b.readObjectTempC();
		amb = MLX_5b.readAmbientTempC();
		sprintf(str, "T%d,%0.1f,%0.1f,%i\r\n", 2, obj, amb, (int)sonic.Distance);
		Serial.send(str);
		obj = MLX_5c.readObjectTempC();
		amb = MLX_5c.readAmbientTempC();
		sprintf(str, "T%d,%0.1f,%0.1f,%i\r\n", 3, obj, amb, (int)sonic.Distance);
		Serial.send(str);
		Serial.sendln();
		
		while((millis()-start_time) <= 250);
	}
}

