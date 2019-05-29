/*
 * ExternalInterrupt.h
 *  Holds the configuration for the EIC, interrupts are configured and made externally
 *      Should always be used before any module configures it
 *  Dependencies
 *      GCLK 1 is running
 *
 * Created: 5/8/2019 8:50:55 AM
 *  Author: Jordan
 */ 



#ifndef EXTERNALINTERRUPT_H_
#define EXTERNALINTERRUPT_H_

#include "sam.h"

void externalInterruptConfig(void);
void externalInterruptStart(void);
void externalInterruptHalt(void);


#endif /* EXTERNALINTERRUPT_H_ */