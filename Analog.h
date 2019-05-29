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

//These values are for throwing errors
#define THROTTLE_MAX    0xFFF
#define THROTTLE_MIN    0
#define THROTTLE_SAFE   0
#define AIR_FLOW_MAX    0xFFF
#define AIR_FLOW_MIN    0
#define AIR_FLOW_SAFE   0
#define AIR_TEMP_MAX    0xFFF
#define AIR_TEMP_MIN    0
#define AIR_TEMP_SAFE   0
#define WATER_TEMP_MAX  0xFFF
#define WATER_TEMP_MIN  0
#define WATER_TEMP_SAFE 0

#define ANALOG_INTERRUPT_PRIORITY 2



void    analogConfig(void);
void    analogStart(void);
void    analogHalt(void);
int     analogIsRunning(void);

#endif /* ANALOG_H_ */