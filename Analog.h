/*
 * Analog.h
 *  This contains all of the code for processing the analog sensors in the system
 *
 * Dependencies:
 *  Interrupts must be globally enabled elsewhere
 *  Config must be called before calling start
 *  GClk1 must be running at 50 MHz
 *  ADC0 is used
 *  ADC1 is used
 *  System Event is used  
 *
 * Created: 4/29/2019 7:41:31 AM
 *  Author: Jordan
 */ 
#ifndef ANALOG_H_
#define ANALOG_H_

#include "sam.h"
#include "Pins.h"
#include "Sensors.h"




void    analogConfig(void);
void    analogStart(void);
void    analogHalt(void);
int     analogIsRunning(void);

#endif /* ANALOG_H_ */