/*
 * Sensors.cpp
 *
 * Created: 5/6/2019 10:26:18 AM
 *  Author: Jordan
 */ 
#include "Sensors.h"
volatile SensorValues Sensors;

//Startup all sensors and sensor triggers
void sensorsConfig(void){
  //Clear out all sensor data buffers
  Sensors.AirFlow             = 0;
  Sensors.AirTemp             = 0;
  Sensors.Throttle            = 0;
  Sensors.WaterTemp           = 0;
  Sensors.ManifoldPressure    = 0;
  Sensors.RPM                 = 0;
  Sensors.Period              = 0;
  Sensors.IsCranking          = 0;
  
  //Initialize other systems
  analogConfig();
  pressureConfig();
}

void sensorsStart(void){
  //Call subsystem start functions
  analogStart();
  pressureStart();
}

void sensorsHalt(void){
  //Call subsystem halt functions
  analogHalt();
  pressureHalt();
}
