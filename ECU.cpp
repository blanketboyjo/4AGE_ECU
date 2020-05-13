/*
 * ECU.cpp
 *
 * Created: 5/29/2019 10:29:59 AM
 *  Author: Jordan
 */ 

#include "ECU.h"

void ECUConfig(void){
  //Startup system at 100 MHz
  systemClockInit();
  //GCLK1 as 50Mhz for various devices
  genericClockInit(1,2);
  //GCLK2 as 2Mhz for Positioning and Terminal
  genericClockInit(2,50);
  
  //Configure all modules
  externalInterruptConfig();
  EFIConfig();
  sensorsConfig();
  //Delay for complete power up of FPGA
  for(unsigned int i = 0; i < 10000000; i++);
  dashboardConfig();
  __enable_irq();   
}
    
//Stop everything
void ECUHalt(void){
  externalInterruptHalt();
  sensorsHalt();
  EFIHalt();
  dashboardHalt();
}

//Start everything
void ECUStart(void){
  externalInterruptStart();
  sensorsStart();
  EFIStart();
  dashboardStart();
}
