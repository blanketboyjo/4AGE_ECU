/*
 * Pressure.h
 *    This library handles the MPRL absolute pressure sensor
 *
 *  Dependencies:
 *    Pins.h              - Has all configuration
 *    Interrupts          - Must be enabled
 *    SECROM              - Utilized in SPI configuration
 *    External interrupt  - Utilized for conversion complete
 *    GPIO                - Utilized to initialize the sensor
 *    Sensors.h           - Houses struct for data output
 *
 * Created: 5/6/2019 11:07:16 AM
 *  Author: Jordan
 */ 

#ifndef PRESSURE_H_
#define PRESSURE_H_
#include "sam.h"
#include "Pins.h"
#include "Sensors.h"

void    pressureConfig(void);
void    pressureStart(void);
void    pressureHalt(void);

#endif /* PRESSURE_H_ */