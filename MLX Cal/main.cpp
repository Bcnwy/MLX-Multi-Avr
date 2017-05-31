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

const int m_sensorsCount = 2;
//Adafruit_MLX90614 MLX_5a(0x5A);
//Adafruit_MLX90614 MLX_5b(0x5B);
//Adafruit_MLX90614 MLX_5c(0x5C);
//Adafruit_MLX90614 MLX_5d(0x5D);
Adafruit_MLX90614 MLX[m_sensorsCount] = {0x5A,0x5B};

volatile unsigned long int count = 0; 
float tol = 1; 
ISR(TIMER0_COMPA_vect){
	count++;
}
ISR(TIMER1_OVF_vect){}
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
	#ifdef _DEBUG
		Serial.send( rflag ,HEX);
		Serial.sendln("> Booting...");	
	#endif // _DEBUG
}
/*********************************************************************/
/*********************************************************************/
uint8_t displayTemp(double * t){	
	char str[25], str_out[100]= "";
	
	sprintf(str, "T%d,%0.1f\r\n", 1, t[0]);
	strcat(str_out,str);

	sprintf(str, "T%d,%0.1f\r\n", 2, t[1]);
	strcat(str_out,str);

	/*sprintf(str, "T%d,%0.1f\r\n", 3, t[2]);
	strcat(str_out,str);
	
	sprintf(str, "T%d,%0.1f\r\n", 4, t[3]);
	strcat(str_out,str);*/
	
	Serial.send(str_out);
	return 0;
}
/*********************************************************************/
uint8_t displayEmiss(float * e){
	char str[20], str_out[80] = "";
	
	sprintf(str, "E%d,%1.1f\r\n", 1, e[0]);
	strcat(str_out,str);

	sprintf(str, "E%d,%0.1f\r\n", 2, e[1]);
	strcat(str_out,str);

	/*sprintf(str, "E%d,%0.1f\r\n", 3, e[2]);
	strcat(str_out,str);
	
	sprintf(str, "E%d,%0.1f\r\n", 4, e[3]);
	strcat(str_out,str);*/
	
	Serial.send(str_out);
	return 0;
}
/*********************************************************************/
void IR_sensorReads(double * t){
	for(int i=0;i<m_sensorsCount; i++){
		t[i] = MLX[i].readObjectTempC();
	}
}
/*********************************************************************/
float changeEmiss(Adafruit_MLX90614 * device, double caltemp, double currenttemp){
	float emiss = device->readEmissivity();
	if (caltemp < currenttemp) emiss += .05;
	else if (caltemp> currenttemp) emiss -= .05;
	
	device->setEmissivity(emiss);	
	return emiss;
}

uint8_t calibrateEmissivity(float * emissivity){
	const char instr[5] = {'3','5','.','0'};
	uint8_t ptr=0, loop=0;
	double usrtemp = 0, t[4];
	float E[4] = {1,1,1,1},prevE[4];
	
	Serial.send("> Temperature to calibrate to: ");
	Serial.send(instr);
	/*char inbyte = '0';
	while (inbyte != 0x0D){
		inbyte = Serial.read();
		Serial.send('1');
		//instr[ptr++] = tempbyte; 
	}
	Serial.sendln(instr);*/
	
	//if (sizeof(instr) <= 2){instr[3] = '0';}
	usrtemp += double(instr[0] - '0') * 10;
	usrtemp += double(instr[1] - '0');
	usrtemp += double(instr[3] - '0') / 10;
	char str[5];
	sprintf(str, ", %0.1f", usrtemp);
	Serial.sendln(str);

	for (int i=0;i<m_sensorsCount;i++){
		MLX[i].setEmissivity(1);
		t[i] = MLX[i].readObjectTempC();	
		
			prevE[i] = MLX[i].readEmissivity();
			E[i] = prevE[i];
			loop = 0;
			while(t[i] > (usrtemp+tol)||t[i] < (usrtemp-tol)){
				loop++;
				if (loop>18){
					E[i] = -1;
					MLX[i].setEmissivity(1);
					break;
				}
				else{
					E[i] = changeEmiss(&MLX[i], usrtemp, t[i]);
					_delay_ms(10);
					t[i] = MLX[i].readObjectTempC();	
				}
			}
	}
	//displayEmiss(prevE);
	for (int i=0;i<m_sensorsCount;i++){
		emissivity[i] = E[i];
	}
	return 0;
}

/*********************************************************************/
int main(void)
{	
	double  temps[4];
	float emiss[4];
	setup();
	//Serial.send(Serial.read());
	IR_sensorReads(temps);
	displayTemp(temps);
	
	calibrateEmissivity(emiss);	
	displayEmiss(emiss);
	
	IR_sensorReads(temps);
	displayTemp(temps);
}
/*********************************************************************/
