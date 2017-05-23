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
#include "VL53L0X.h"

Adafruit_MLX90614 MLX_5a(0x5A);
Adafruit_MLX90614 MLX_5b(0x5B);
//Adafruit_MLX90614 MLX_5c(0x5C);
//Adafruit_MLX90614 MLX_5d(0x5D);
//Adafruit_MLX90614 MLX_5e(0x5E);

HC_SR04 Sonic;
//VL53L0X TOF;
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
	#ifdef _DEBUG
		uint8_t rflag = MCUSR;
		MCUSR = 0;
	#endif // _DEBUG
	
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
	Sonic.init();
	#ifdef _DEBUG
		Serial.send( rflag ,HEX);
		Serial.sendln("> Booting...");	
	#endif // _DEBUG
// 	TOF.init();
// 	TOF.setTimeout(250);
// 	TOF.setSignalRateLimit(0.2);
// 	TOF.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 16);
// 	TOF.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 12);
}
/*********************************************************************/
void IR_sensorRead(void )
{
	char str[100], str_out[100] = {""};
	double obj_1,obj_2,obj_3,obj_4,amb_1,amb_2,amb_3,amb_4,mDistance = 0;
	unsigned long int m_read_time = 0;

 	Sonic.read();
// 	mDistance = TOF.readRangeContinuousMillimeters();	
 	#ifdef _DEBUG
 	Serial.sendln("> Read Distance...");
 	#endif
// 	//Read 0x5A
 	m_read_time = millis();	
	
	amb_1 = MLX_5a.readAmbientTempC();
 	obj_1 = MLX_5a.readObjectTempC();
// 
// 	//Read 0x5B	
 	amb_2 = MLX_5b.readAmbientTempC();
	obj_2 = MLX_5b.readObjectTempC();
// 	//if (obj_2 = MLX_5b.readObjectTempC() <10) 
// 
// 	//Read 0x5C	
//	amb_3 = MLX_5c.readAmbientTempC();
// 	obj_3 = MLX_5c.readObjectTempC();
//
// 	//Read 0x5D
// 	amb_4 = MLX_5d.readAmbientTempC();	
// 	obj_4 = MLX_5d.readObjectTempC();
// 	
 	sprintf(str, "{\"Sensor\":%d{\"Obj\":%0.1f,\"Amb\":%0.1f,\"Dist\":[%i,%.2f],\"Time\":%lu}}", 1, obj_1, amb_1, (int)Sonic.Distance,mDistance,m_read_time);
   	//strcat(str_out,str);
	Serial.send(str_out);
	
	sprintf(str, "{\"Sensor\":%d{\"Obj\":%0.1f,\"Amb\":%0.1f,\"Dist\":[%i,%.2f],\"Time\":%lu}}", 2, obj_2, amb_2, (int)Sonic.Distance,mDistance,m_read_time);
	//strcat(str_out,str);
	Serial.send(str_out);
//  sprintf(str, "{\"Sensor\":%d{\"obj\":%0.1f,\"amb\":%0.1f,\"Dist\":[%i,%.2f],\"Time\":%lu}}", 3, obj_3, amb_3 , (int)Sonic.Distance,mDistance,m_read_time);
//  strcat(str_out,str);
// 	
// 	sprintf(str, "{\"Sensor\":%d{\"obj\":%0.1f,\"amb\":%0.1f,\"Dist\":[%i,%.2f],\"Time\":%lu}}", 4, obj_4, amb_4 , (int)Sonic.Distance,mDistance,m_read_time);
// 	strcat(str_out,str);
	 	
	//Serial.send(str_out);
}
/*********************************************************************/
int main(void)
{	
	setup();
	// Start continuous back-to-back mode (take readings as
	// fast as possible).  To use continuous timed mode
	// instead, provide a desired inter-measurement period in
	// ms (e.g. sensor.startContinuous(100)).
	//char s[32];
	//sprintf(s,"%.3f",TOF.getSignalRateLimit());
	//Serial.send(s);
	//TOF.startContinuous(100);
	while (1){
		unsigned long int start_time;
	    start_time = millis();	
		#ifdef _DEBUG
			Serial.sendln("> Read Sensors...");
		#endif
		IR_sensorRead();
		//_delay_ms(250);	
		//sprintf(s,"%lu",start_time)	;
		//Serial.sendln(s);
		while((millis()-start_time) <= 250);
	}
}
/*********************************************************************/
