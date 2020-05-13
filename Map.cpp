/*
 * Map.cpp
 *
 * Created: 5/23/2019 9:12:27 AM
 *  Author: Jordan
 */ 
#include "Map.h"

//VE Table


//Returns ignition advance target
int getIgnitionAdvance(void){
  return EFI_DEFAULT_ADVANCE;
}

//Returns desired CCR for injection
unsigned int getInjectionTime(void){
  return EFI_INJECTOR_ON_CCR;
}

//Calculates idle based on temperature
unsigned int getIdle(void){
  //Default value
  unsigned int tempIdle = 1200;
  //Adjust idle for temperature
  if(70000 >= Sensors.WaterTemp){
    tempIdle  = 2200;
  }else if((82000 > Sensors.WaterTemp) && (70000 < Sensors.WaterTemp)){
    tempIdle += 1000 -((833 * (Sensors.WaterTemp - 70000))/10000);
  }
  return tempIdle;
}
