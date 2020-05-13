/*
 * ECU.h
 *  Main wrapper for ECU
 *
 *
 *
 * Created: 5/29/2019 10:28:13 AM
 *  Author: Jordan
 */ 


#ifndef ECU_H_
#define ECU_H_
#include "Pins.h"
#include "Sensors.h"
#include "ExternalInterrupt.h"
#include "EFI.h"
#include "Dashboard.h"
#include "Clock_System.h"

void ECUConfig(void);
void ECUHalt(void);
void ECUStart(void);



#endif /* ECU_H_ */