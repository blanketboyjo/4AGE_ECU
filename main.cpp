/*
* ECU_Code.cpp
*
* Created: 4/29/2019 7:39:53 AM
* Author : Jordan
*/ 
//It won't sleep unless you comment this out
//#define DEBUG_MODE


#include "sam.h"
#include "Pins.h"
#include "Clock_System.h"
#include "Sensors.h"
#include "ExternalInterrupt.h"
#include "EFI.h"
#include "Dashboard.h"

#define SLEEP_ON_EXIT (1 << 1)


int main(void){
  //Startup system at 100 MHz
  systemClockInit();
  //GCLK1 as 50Mhz for various devices
  genericClockInit(1,2);
  //GCLK2 as 2Mhz for Positioning and Terminal
  genericClockInit(2,50);

  externalInterruptConfig();
  EFIConfig();
  sensorsConfig();
  dashboardConfig();

  //Start pinInterrupts
  __enable_irq();
  externalInterruptStart();
  sensorsStart();
  EFIStart();
  dashboardStart();
  //Only allow sleep and sleep on exit if debug is disabled, it inhibits debugger
  //when sleep is enabled
#ifndef DEBUG_MODE
  //Setup sleep mode
  PM->SLEEPCFG.reg = PM_SLEEPCFG_SLEEPMODE_STANDBY;
  SCB->SCR |= SLEEP_ON_EXIT;
  //Wait for this to happen
  while(PM->SLEEPCFG.reg != PM_SLEEPCFG_SLEEPMODE_STANDBY);
  //Ensure sleeping is allowed    
  while(0 == PM->INTFLAG.bit.SLEEPRDY);
  //Put it to sleep
  __WFI();
#else
  while(1){
  for(unsigned int i = 0; i<10000000; i++);

  }
#endif
}