/*
* ECU_Code.cpp
*
* Created: 4/29/2019 7:39:53 AM
* Author : Jordan
*/ 
//It won't sleep unless you comment this out
//#define DEBUG_MODE


#include "sam.h"
#include "ECU.h"


int main(void){
  ECUConfig();
  ECUStart();

  //Only allow sleep and sleep on exit if debug is disabled, it inhibits debugger
  //when sleep is enabled
#ifndef DEBUG_MODE
  //Setup sleep mode
  PM->SLEEPCFG.reg = PM_SLEEPCFG_SLEEPMODE_STANDBY;
  SCB->SCR |= (1 << 1); //Causes sleep on exit
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