/*
* Clock_System.cpp
*
* Created: 4/29/2019 11:21:10 AM
*  Author: Jordan
*/ 
#include "Clock_System.h"

/*
*   Starts system clock at 100 MHz using 24MHz crystal as reference
*/
void systemClockInit(void){

  //Start up 24Mhz external oscillator
  OSCCTRL->XOSCCTRL[0].reg =  
      OSCCTRL_XOSCCTRL_ENABLE     |   //  Turn on oscillator
      OSCCTRL_XOSCCTRL_XTALEN     |   //  Set as crystal no clock in
      OSCCTRL_XOSCCTRL_RUNSTDBY   |   //  Always run
      OSCCTRL_XOSCCTRL_IMULT(0x06)|   //  Set based on table for 24MHz
      OSCCTRL_XOSCCTRL_IPTAT(0x03);   //  Set based on table for 24MHz

  while(OSCCTRL->STATUS.bit.XOSCRDY0 != 1);//Wait for oscillator to be ready



  //Setup DPLL with 24Mhz/12 input as reference, target is 100Mhz
  OSCCTRL->Dpll[0].DPLLCTRLB.reg =    
      OSCCTRL_DPLLCTRLB_DIV(5)    |   //  Divide 24Mhz to 2Mhz for device limits
      OSCCTRL_DPLLCTRLB_REFCLK_XOSC0; //  Input is Oscillator 0 (24Mhz)

  OSCCTRL->Dpll[0].DPLLRATIO.reg =    
      OSCCTRL_DPLLRATIO_LDR(49);      //  Ratio is 100/2 = 50 but LDR is ratio -1

  OSCCTRL->Dpll[0].DPLLCTRLA.reg =    
      OSCCTRL_DPLLCTRLA_ENABLE    |   //  Enable it
      OSCCTRL_DPLLCTRLA_RUNSTDBY;     //  Always run

  //Wait for DPLL to be ready
  while((OSCCTRL->Dpll[0].DPLLSTATUS.reg & 
        (OSCCTRL_DPLLSTATUS_CLKRDY | OSCCTRL_DPLLSTATUS_LOCK))  != 
        (OSCCTRL_DPLLSTATUS_CLKRDY | OSCCTRL_DPLLSTATUS_LOCK));

  //Setup GCLK 0 (M_CLK) to 100 Mhz (aka DPLL)
  GCLK->GENCTRL[0].reg    =   
      GCLK_GENCTRL_SRC_DPLL0      |   //  Source is DPLL
      GCLK_GENCTRL_GENEN          |   //  Enable Generic Clock
      GCLK_GENCTRL_DIV(00)        |   //  Divide input by 0
      GCLK_GENCTRL_RUNSTDBY;          //  Run in stand by

  //Turn of DFLL since it is no longer used
  OSCCTRL->DFLLCTRLA.reg &= ~(OSCCTRL_DFLLCTRLA_ENABLE);
}


void genericClockInit(unsigned int number, unsigned int divider){
  if((0 == number) ||(12 <= number)) return;  //  Valid gclk (#0 isn't valid)s
  if(1 == number) divider &= 0xFFFF;          //  Divider validity forGCLK_1
  else divider &= 0xFF;                       //  Divider validity for other GCLK_n

  //Setup GCLKn with DPLL0 as ref, and set divider, will not run in standby unless requested
  GCLK->GENCTRL[number].reg    =  
      GCLK_GENCTRL_SRC_DPLL0          |   //  Source is DPLL
      GCLK_GENCTRL_RUNSTDBY           |   //  Run clk in standby
      GCLK_GENCTRL_GENEN              |   //  Enable Generic Clock
      GCLK_GENCTRL_OE                 |   //  Enable the output
      GCLK_GENCTRL_DIV(divider);          //  Include divider   
}
