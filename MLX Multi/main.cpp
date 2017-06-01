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
Adafruit_MLX90614 MLX_5b(0x5E);
Adafruit_MLX90614 MLX_5c(0x5C);
Adafruit_MLX90614 MLX_5d(0x5D);

const int m_sensorsCount = 2;
Adafruit_MLX90614 MLX[m_sensorsCount] = {0x5A,0x5B};

HC_SR04 Sonic;
VL53L0X TOF;
volatile unsigned long int count = 0; 
ISR(TIMER0_COMPA_vect){
	count++;
}
ISR(TIMER1_OVF_vect){}
/*********************************************************************/
unsigned long int millis(void){
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
//  	TOF.init();
//  	TOF.setTimeout(250);
//  	TOF.setSignalRateLimit(0.2);
//  	TOF.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 16);
//  	TOF.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 12);
	 
	 	#ifdef _DEBUG
	 	Serial.sendln("> TOF...");
	 	#endif // _DEBUG
}
/*********************************************************************/
void IR_sensorRead(void )
{
	char str[125], str_out[500] = {""};
	double obj[m_sensorsCount], amb[m_sensorsCount], mDistance = 0;
	unsigned long int m_read_time = 0;

 	Sonic.read();
 	//mDistance = TOF.readRangeContinuousMillimeters();	

	for(int i=0;i<m_sensorsCount; i++){
		obj[i] = MLX[i].readObjectTempC();
		amb[i] = MLX[i].readAmbientTempC();
	
 		sprintf(str, "[{\"Sensor\":%d,\"Obj\":%0.1f,\"Amb\":%0.1f,\"Distance\":[%i,%0.1f],\"uTime\":%lu}", i+1, obj[i], amb[i], (int)Sonic.Distance, mDistance, m_read_time);
   		strcat(str_out,str);
	}
	Serial.sendln(str_out);
}
/*********************************************************************/
int main(void)
{	
	setup();
	//TOF.startContinuous(100);
	while (1){
		unsigned long int start_time;
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
