/*
 * Clock_System.h
 *  This library is for handling the various generic clocks in
 *  the MCU. The systemClockInit function starts the MCU at 100MHz
 *  
 *  
 *  Dependencies:
 *      24Mhz crystal oscillator on PA14/PA15
 *      DPLL0
 *      
 *  
 * Created: 4/29/2019 7:56:45 AM
 *  Author: Jordan
 */ 




#ifndef CLOCK_SYSTEM_H_
#define CLOCK_SYSTEM_H_
#include <sam.h>

void systemClockInit(void);
void genericClockInit(unsigned int number, unsigned int divider);
#endif /* CLOCK_SYSTEM_H_ */