/*
 * Terminal.h
 *  This handles the UART interface for terminal usage
 *  It is configured for 115200 baud
 *
 *
 * Dependencies:
 *  - Pins.h has all of the pin outs/necessary defines
 *  - A clock source running at 2Mhz (See pins.h)
 *  - A sercom with specified pin outs (See pins.h)
 *  - Interrupts must be enabled for the buffer system to work
 *      -This is designed for a system with separate interrupts for each flag
 *
 * Created: 5/13/2019 8:33:06 AM
 *  Author: Jordan
 */ 


#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "sam.h"
#include "Pins.h"
#include "Sensors.h"


void terminalConfig(void);
void terminalSend(unsigned int data);
void terminalSendStream(unsigned int *data, unsigned int length);
void terminalFlushInput(void);


unsigned int terminalRead(void);
unsigned int terminalAvailable(void);


#endif /* TERMINAL_H_ */