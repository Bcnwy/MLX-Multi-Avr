/*
 * config.h
 *
 * Created: 12/05/2017 23:30:48
 *  Author: Conway
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

#define F_CPU 16000000UL

#define set_pin(port, pin) (port |= (1 << pin))
#define clr_pin(port, pin) (port &= ~(1 << pin))
#define out_pin(port, pin) (port |= _BV( pin))
#define in_pin(port, pin) (port &= ~(1 << pin))
#define _BV(bit) (1 << (bit))

unsigned long int millis(void);
#define _DEBUG
//#define _TOF
#define _Sonic
#endif /* CONFIG_H_ */