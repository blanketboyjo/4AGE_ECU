/*
 * Sensors.h
 *  This is a wrapper library for all of the different sensors, simple
 *  sensors are implemented in this library. It also houses the struct
 *  for passing system data to other systems. (Note: Ne and G are unique
 *  to EFI.h and not implemented in here for safety reasons)
 *
 * Dependencies:(See each library for additional dependencies)
 *  Analog.h    - Throttle, Air temp, water temp
 *  Pressure.h  - Manifold Sensor
 *  Pins.h      - Used 
 *  
 *
 * Created: 5/6/2019 10:25:54 AM
 *  Author: Jordan
 */ 


#ifndef SENSORS_H_
#define SENSORS_H_

#include "sam.h"
#include "Analog.h"
#include "Pressure.h"

struct SensorValues{
    unsigned int AirFlow;
    unsigned int Throttle;
    unsigned int AirTemp;
    unsigned int WaterTemp;
    unsigned int ManifoldPressure; 
    unsigned int RPM;
    unsigned int Period;
    unsigned int IsCranking;
    unsigned int ThrottleClosed;
};
extern volatile SensorValues Sensors;


void sensorsConfig(void);
void sensorsStart(void);
void sensorsHalt(void);


#endif /* SENSORS_H_ */