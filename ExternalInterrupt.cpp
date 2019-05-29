/*
 * ExternalInterrupt.cpp
 *
 * Created: 5/8/2019 8:50:55 AM
 *  Author: Jordan
 */ 

#include "ExternalInterrupt.h"

void externalInterruptConfig(void){
static int m_isConfigured = 0;
  //Only configure if not yet configured
  if(1 == m_isConfigured) return;
  //Enable EIC on bus
  MCLK->APBAMASK.bit.EIC_ = 
      1;

  //Give it a clock (GCLK 1)
  GCLK->PCHCTRL[EIC_GCLK_ID].reg =    
      GCLK_PCHCTRL_CHEN  |
      GCLK_PCHCTRL_GEN_GCLK1;

  //Software reset it really quick
  EIC->CTRLA.bit.SWRST = 
      1;

  //Wait for reset to end
  while(EIC->CTRLA.reg & EIC_CTRLA_SWRST == 1);
}

void externalInterruptStart(void){
  //Set configuration
  EIC->CTRLA.reg =   
      EIC_CTRLA_ENABLE;   //Just turn it on
}

void externalInterruptHalt(void){
  //Set configuration
  EIC->CTRLA.reg =   
      0;   //Stop it
}
