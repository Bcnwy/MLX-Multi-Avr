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


// Adafruit_MLX90614 MLX_5a(0x5A);
// Adafruit_MLX90614 MLX_5b(0x5E);
// Adafruit_MLX90614 MLX_5c(0x5C);
// Adafruit_MLX90614 MLX_5d(0x5D);

const int m_sensorsCount = 2;
Adafruit_MLX90614 MLX[m_sensorsCount] = {0x5A,0x5B};//,0x5C,0x5D};
#ifdef _Sonic
	HC_SR04 Sonic;
#endif // Sonic
#ifdef _TOF
	VL53L0X TOF;
#endif // TOF
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

	#ifdef _DEBUG
		Serial.send( rflag ,HEX);
		Serial.sendln("> Booting...");	
	#endif // _DEBUG	
	#ifdef _Sonic
		Sonic.init();
	#endif // Sonic
	#ifdef _TOF
 		TOF.init();
 		TOF.setTimeout(250);
 		TOF.setSignalRateLimit(0.2);
 		TOF.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 16);
 		TOF.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 12);
		#ifdef _DEBUG
		Serial.sendln("> TOF Started");
		#endif // _DEBUG 
	#endif // TOF
}
/*********************************************************************/
void IR_sensorRead(void )
{
	char str[125], str_out[500] = {""};
	double obj[m_sensorsCount], amb[m_sensorsCount], mDistance = 0;
	unsigned long int m_read_time = 0;
	
	#ifdef _Sonic
		Sonic.read();
	#endif // _Sonic
 	#ifdef _TOF
		mDistance = TOF.readRangeContinuousMillimeters();	
	#endif // _TOF
	
	for(int i=0;i<m_sensorsCount; i++){
		obj[i] = MLX[i].readObjectTempC();
		amb[i] = MLX[i].readAmbientTempC();
	
 		sprintf(str, "[{\"Sensor\":%d,\"Obj\":%0.1f,\"Amb\":%0.1f,\"Distance\":[%i,%0.1f],\"uTime\":%lu}", i+1, obj[i], amb[i], (int)Sonic.Distance, mDistance, m_read_time);
   		strcat(str_out,str);
	}
	Serial.sendln(str_out);
}
/*********************************************************************/
void cal_emiss()
{
	// TODO: Cal routine
	Serial.sendln("cal...");
}
void set_emiss(uint8_t e)
{
	// TODO: Cal routine
	Serial.sendln("emiss...");
	for(int i=0;i<m_sensorsCount; i++){
		MLX[i].setEmissivity(e);
	}		
}
/*********************************************************************/
int main(void)
{			
	unsigned long int start_time;
	setup();
	#ifdef _TOF	
		TOF.startContinuous(100);
	#endif // _TOF

	while (1){
		
	    start_time = millis();	
		#ifdef _DEBUG
			Serial.sendln("> Read Sensors...");
		#endif
		//IR_sensorRead();
		
		if (Serial.available())
		{
			char* d_str;
			d_str = Serial.readln();
			Serial.sendln(d_str);	
			if(d_str[0] == 's'){
				char inbyte;
				// TODO: Implement settings menu 
				Serial.sendln("> Settings");
				Serial.sendln("1) Calibrate");
				Serial.sendln("2) Set Emissivity");
				inbyte = Serial.read();
				switch ((uint8_t)inbyte)
				{
				case 1: 
					cal_emiss();
					break;
				case 2: 
					Serial.sendln("Set E: ");
					inbyte = Serial.read();
					set_emiss((uint8_t)inbyte);
					break;
				}
			}
		}
		while((millis()-start_time) <= 1000);
	}
}
/*********************************************************************/
