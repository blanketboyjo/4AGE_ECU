/*
 * Map.h
 *  This library handles the fuel mapping system  
 *
 *  Dependencies:
 *    Pins.h    - Has critical configuration specs
 *    Sensors.h - Has data struct for calculations
 *
 * Created: 5/23/2019 8:57:40 AM
 *  Author: Jordan
 */ 


#ifndef MAP_H_
#define MAP_H_

#include "sam.h"
#include "Pins.h"
#include "Sensors.h"

int          getIgnitionAdvance(void);
unsigned int getInjectionTime(void);



#endif /* MAP_H_ */