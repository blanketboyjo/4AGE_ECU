/*
 * Map.cpp
 *
 * Created: 5/23/2019 9:12:27 AM
 *  Author: Jordan
 */ 
#include "Map.h"

//Returns ignition advance target
int getIgnitionAdvance(void){
  return EFI_DEFAULT_ADVANCE;
}

//Returns desired CCR for injection
unsigned int getInjectionTime(void){
  return EFI_INJECTOR_ON_CCR;
}