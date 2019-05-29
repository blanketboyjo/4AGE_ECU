/*
* EFI.cpp
*
* Created: 5/22/2019 9:11:16 AM
*  Author: Jordan, Nick
*/ 
#include "EFI.h"


enum IgniterFSM{
  IGT_IDLE    = 0,
  IGT_WAITING = 1,
  IGT_DWELL   = 2,
  IGT_SPARK   = 3
};


enum InjectorFSM{
  INJ_IDLE    = 0,
  INJ_WAITING = 1,
  INJ_DWELL   = 2            
};

IgniterFSM m_Igniter = IGT_IDLE;
InjectorFSM m_Injector[4] = {   INJ_IDLE,
  INJ_IDLE,
  INJ_IDLE,
  INJ_IDLE
};

volatile unsigned int m_isNotFirstPulse     = 0;
volatile unsigned int m_rolloverCount       = 0;
volatile unsigned int m_lastCCR             = 0;

volatile unsigned int m_igniterStartCCR     = EFI_TCC_MAX + 1;

volatile unsigned int m_nextNeIsStart       = 0;
volatile          int m_currentNe           = 5;
volatile unsigned int m_injectorNumber      = 0;
volatile unsigned int m_injectorTargets[4]  = {2,3,0,1};


//This function configures the necessary components of the EFI system
void EFIConfig(void){
  //Default out necessary outputs
  EFI_IGT_CONFIG;
  EFI_INJ0_CONFIG;
  EFI_INJ1_CONFIG;
  EFI_INJ2_CONFIG;
  EFI_INJ3_CONFIG;
  EFI_NE_CONFIG;
  EFI_G_CONFIG;

  //Clear all FSM states
  m_Injector[0]       = INJ_IDLE;
  m_Injector[1]       = INJ_IDLE;
  m_Injector[2]       = INJ_IDLE;
  m_Injector[3]       = INJ_IDLE;
  m_Igniter           = IGT_IDLE;

  //Clear all necessary variables
  m_isNotFirstPulse   = 0;
  m_lastCCR           = 0;
  m_injectorNumber    = 0;
  m_rolloverCount     = 0;
  m_igniterStartCCR   = EFI_TCC_MAX + 1;
  m_nextNeIsStart     = 0;




  //Setup timers, start running them, all interrupts off
  //Bus enable
  EFI_TCC_ENABLE_BUS;
  //Clock source
  GCLK->PCHCTRL[EFI_TCC_GCLK_ID].reg  =   
      EFI_TCC_GCLK_SOURCE |
      GCLK_PCHCTRL_CHEN;
  //Software reset timer
  EFI_TCC->CTRLA.bit.SWRST            = 
      1;

  //Wait for reset to take place
  while(0 != (EFI_TCC->CTRLA.reg & TCC_CTRLA_SWRST));

  //Setup, Divide by 4, enable capture on channel 0, runstandby
  EFI_TCC->CTRLA.reg =  
      TCC_CTRLA_PRESCALER_DIV4  |  //Divide input by 4
      TCC_CTRLA_CPTEN0          |  //Enable ch0 capture
      TCC_CTRLA_RUNSTDBY;          //Run timer in standby

  //Setup External interrupts for NE and G
  //Call config function for EIC before messing with settings
  externalInterruptConfig();

  //Use EIC configurations from Pins.h
  EFI_G_EIC_CONFIG;
  EFI_NE_EIC_CONFIG;

  //Enable events for NE and G
  EIC->EVCTRL.reg          |= 
      (1 << EFI_NE_EIC);

  //Configure Event system
  //Enable EVSYS on main bus
  MCLK->APBBMASK.bit.EVSYS_  = 
      1;

  //Give Desired channel a clock
  GCLK->PCHCTRL[EFI_TCC_NE_EVSYS_GLK_ID].reg      =   
      GCLK_PCHCTRL_GEN_GCLK0 |    // Generator 0 is source
      GCLK_PCHCTRL_CHEN;          // Enable Channel

  //Configure user mux
  EVSYS->USER[EFI_TCC_NE_EVSYS_USER_ID].reg       =   
      EVSYS_USER_CHANNEL(EFI_TCC_NE_EVSYS_CH + 1);

  //Connect events
  EVSYS->Channel[EFI_TCC_NE_EVSYS_CH].CHANNEL.reg =   
      EVSYS_CHANNEL_EVGEN(EFI_TCC_NE_EVSYS_EVGEN) |
      EVSYS_CHANNEL_RUNSTDBY                      |
      EVSYS_CHANNEL_EDGSEL_RISING_EDGE            |
      EVSYS_CHANNEL_PATH_SYNCHRONOUS;

  //Enable Timer Events
  EFI_TCC->EVCTRL.reg       =   
      TCC_EVCTRL_MCEI0;

  //Enable timer
  EFI_TCC->CTRLA.bit.ENABLE =   
      1;

  //Start timer
  EFI_TCC->CTRLBSET.reg     =   
      TCC_CTRLBSET_CMD_RETRIGGER;


  //Setup interrupt priorities
  NVIC_SetPriority(EFI_TCC_OVF_IRQN,  EFI_NE_OVF_PRIORITY);
  NVIC_SetPriority(EFI_TCC_NE_IRQN,   EFI_NE_PRIORITY);
  NVIC_SetPriority(EFI_G_IRQN,        EFI_NE_PRIORITY);
  NVIC_SetPriority(EFI_TCC_IGT_IRQN,  EFI_IGNITION_PRIORITY);
  NVIC_SetPriority(EFI_TCC_INJ0_IRQN, EFI_INJECTOR_PRIORITY);
  NVIC_SetPriority(EFI_TCC_INJ1_IRQN, EFI_INJECTOR_PRIORITY);
  NVIC_SetPriority(EFI_TCC_INJ2_IRQN, EFI_INJECTOR_PRIORITY);
  NVIC_SetPriority(EFI_TCC_INJ3_IRQN, EFI_INJECTOR_PRIORITY);

  //Enable interrupts at the NVIC
  NVIC_EnableIRQ(EFI_TCC_OVF_IRQN);
  NVIC_EnableIRQ(EFI_TCC_NE_IRQN);
  NVIC_EnableIRQ(EFI_TCC_IGT_IRQN);
  NVIC_EnableIRQ(EFI_G_IRQN);
  NVIC_EnableIRQ(EFI_TCC_INJ0_IRQN);
  NVIC_EnableIRQ(EFI_TCC_INJ1_IRQN);
  NVIC_EnableIRQ(EFI_TCC_INJ2_IRQN);
  NVIC_EnableIRQ(EFI_TCC_INJ3_IRQN);

}

//This function prepares and starts the injector system
void EFIStart(void){
  //Clear all FSM states
  m_Injector[0]       = INJ_IDLE;
  m_Injector[1]       = INJ_IDLE;
  m_Injector[2]       = INJ_IDLE;
  m_Injector[3]       = INJ_IDLE;
  m_Igniter           = IGT_IDLE;

  //Clear all necessary variables
  m_isNotFirstPulse   = 0;
  m_lastCCR           = 0;
  m_injectorNumber    = 0;
  m_rolloverCount     = 0;
  m_igniterStartCCR   = EFI_TCC_MAX + 1;

  //Clear all outputs
  PORT->Group[EFI_IGT_GROUP].OUTCLR.reg   = 
      (1<< EFI_IGT_PIN);
  PORT->Group[EFI_INJ0_GROUP].OUTCLR.reg  = 
      (1<< EFI_INJ0_PIN);
  PORT->Group[EFI_INJ1_GROUP].OUTCLR.reg  = 
      (1<< EFI_INJ1_PIN);
  PORT->Group[EFI_INJ2_GROUP].OUTCLR.reg  = 
      (1<< EFI_INJ2_PIN);
  PORT->Group[EFI_INJ3_GROUP].OUTCLR.reg  = 
      (1<< EFI_INJ3_PIN);

  //Enable required interrupts after clearing appropriate flags
  EFI_TCC->INTFLAG.reg    =   
      EFI_TCC_OVF_IFG|        // Clear overflow flag
      EFI_TCC_NE_IFG;         // Clear capture 0 flag
  EIC->INTFLAG.reg        =   
      (1 << EFI_G_EIC);       // Clear EIC flag

  EFI_TCC->INTENSET.reg   =   
      EFI_TCC_NE_IFG|         // NE capture enable
      EFI_TCC_OVF_IFG;        // Overflow capture enable
  EIC->INTENSET.reg       =   
      (1 << EFI_G_EIC);       // G sense enable
}

//Stop all interrupts, turn off injectors and igniter pulse
void EFIHalt(void){
  //Turn off all interrupts on timer
  EFI_TCC->INTENCLR.reg = 
      0xFFFFFF;    
  //Disable G interrupt
  EIC->INTENCLR.reg     = 
      (1 << EFI_G_EIC);

  //Clear all FSM states
  m_Injector[0]         = INJ_IDLE;
  m_Injector[1]         = INJ_IDLE;
  m_Injector[2]         = INJ_IDLE;
  m_Injector[3]         = INJ_IDLE;
  m_Igniter             = IGT_IDLE;

  //Clear all necessary variables
  m_isNotFirstPulse     = 0;
  m_lastCCR             = 0;
  m_injectorNumber      = 0;
  m_rolloverCount       = 0;
  m_igniterStartCCR     = EFI_TCC_MAX + 1;

  //Clear all outputs
  PORT->Group[EFI_IGT_GROUP].OUTCLR.reg   = 
      (1<< EFI_IGT_PIN);
  PORT->Group[EFI_INJ0_GROUP].OUTCLR.reg  = 
      (1<< EFI_INJ0_PIN);
  PORT->Group[EFI_INJ1_GROUP].OUTCLR.reg  = 
      (1<< EFI_INJ1_PIN);
  PORT->Group[EFI_INJ2_GROUP].OUTCLR.reg  = 
      (1<< EFI_INJ2_PIN);
  PORT->Group[EFI_INJ3_GROUP].OUTCLR.reg  = 
      (1<< EFI_INJ3_PIN);
}

/**************************************************************************
*
*      Interrupt service routines
*
**************************************************************************/
void EFI_G_ISR(void){
  EIC->INTFLAG.reg    = 
      (1<< EFI_G_EIC);
  m_nextNeIsStart     = 1;
}


void EFI_TCC_OVF_ISR(void){
  //Clear flag
  EFI_TCC->INTFLAG.reg = 
      EFI_TCC_OVF_IFG;
  //Increment counter
  m_rolloverCount++;
}

//EFI timing function, calculates pulse starts for fuel injectors and igniter
void EFI_TCC_NE_ISR(void){
  //This flag clears on CCR read so don't flag clear
  unsigned int tempCCR    = EFI_TCC->CC[EFI_TCC_NE_CC_NUM].reg;

  //Only calculate RPM if this is not the first pulse
  if(1 == m_isNotFirstPulse){
  //Calculate timing for last period
  unsigned int tempPeriod =   tempCCR ;
  tempPeriod             += (m_rolloverCount * (EFI_TCC_MAX));
  tempPeriod             -= m_lastCCR;

  m_rolloverCount         =   0;         
  //RPM conversion
  unsigned int tempRPM    =  (EFI_RPM_CONVERSION_RATIO/tempPeriod);

  //Store in the data struct
  Sensors.RPM             =   tempRPM;
  Sensors.Period          =   tempPeriod;

  if((100 < tempRPM)){
    //Calculate next time for spark
    unsigned int tempSparkStart = 
        (tempPeriod *(EFI_DEGREES_PER_NE + 
                      EFI_DISTRIBUTOR_ADVANCE - 
                      EFI_DEFAULT_ADVANCE))/18000;
    tempSparkStart += tempCCR;                    //Offset
    tempSparkStart -= EFI_IGNITION_DWELL_CCR;     //Subtract dwell
    tempSparkStart  = tempSparkStart & EFI_TCC_MAX; //Constrain it to timer limits

    //Load value directly into timer if FSM is idle
    //Catch instance where timer got stuck
    if((EFI_TCC_MAX + 1) == m_igniterStartCCR){
      if(IGT_IDLE == m_Igniter){
        EFI_TCC->CC[EFI_TCC_IGT_CC_NUM].reg = 
            tempSparkStart;
        //Enable interrupts after clearing interrupt flags
        EFI_TCC->INTFLAG.reg  = 
            EFI_TCC_IGT_IFG;
        EFI_TCC->INTENSET.reg = 
            EFI_TCC_IGT_IFG;
        //Set FSM to waiting
        m_Igniter             = IGT_WAITING;
        m_igniterStartCCR     = EFI_TCC_MAX + 1;
      }else{//Load CCR is a buffer register for when FSM is ready
        //Check that data can be loaded
        m_igniterStartCCR = tempSparkStart;
      }
    }else{//There should never be an instance where we calculate two in a row
      //before igniter goes off
      PORT->Group[EFI_IGT_GROUP].OUTCLR.reg = 
          (1 << EFI_IGT_PIN);
      m_igniterStartCCR = EFI_TCC_MAX + 1;
      //Enable interrupts after clearing interrupt flags
      EFI_TCC->INTFLAG.reg  = 
          EFI_TCC_IGT_IFG;
      EFI_TCC->INTENCLR.reg = 
          EFI_TCC_IGT_IFG;
      //Set FSM to waiting
      m_Igniter                  = IGT_IDLE;
    }            

    //Process Ne for location
    if(5 != m_currentNe)    m_currentNe = (m_currentNe + 1) & 0x3;
    if(1 == m_nextNeIsStart)m_currentNe = 0;
    m_nextNeIsStart = false;
    //Injection stuff
    if(EFI_FUEL_CUT_RPM > tempRPM){
      INJECTOR_CALC_LABEL:
      unsigned int tempInjectionTime = getInjectionTime();
      int cyc_req = (1 + tempInjectionTime/tempPeriod);
      //get 180's left (& 0x3 prevents result from being larger than 3)
      int cyc_left = (m_injectorTargets[m_injectorNumber] - m_currentNe) & 0x3;
      //late case
      if (-1 == (cyc_left - cyc_req)){
        //Fire now
        //Send output high
        //Set CC to current capture + pulse time
        //Clear flag
        //Enable interrupt
        EFI_TCC->CC[EFI_TCC_INJ_CC_BASE + m_injectorNumber].reg = 
            (EFI_TCC->CC[EFI_TCC_NE_CC_NUM].reg + tempInjectionTime)& EFI_TCC_MAX;

        m_Injector[m_injectorNumber] = INJ_IDLE; 
        switch(m_injectorNumber){
          case 0:
          PORT->Group[EFI_INJ0_GROUP].OUTSET.reg  = 
              (1 << EFI_INJ0_PIN);
          EFI_TCC->INTFLAG.reg  = 
              EFI_TCC_INJ0_IFG;
          EFI_TCC->INTENSET.reg = 
              EFI_TCC_INJ0_IFG;
          break;

          case 1:
          PORT->Group[EFI_INJ1_GROUP].OUTSET.reg = 
              (1 << EFI_INJ1_PIN);
          EFI_TCC->INTFLAG.reg  = 
              EFI_TCC_INJ1_IFG;
          EFI_TCC->INTENSET.reg = 
              EFI_TCC_INJ1_IFG;
          break;

          case 2:
          PORT->Group[EFI_INJ2_GROUP].OUTSET.reg = 
              (1 << EFI_INJ2_PIN);
          EFI_TCC->INTFLAG.reg  = 
              EFI_TCC_INJ2_IFG;
          EFI_TCC->INTENSET.reg = 
              EFI_TCC_INJ2_IFG;
          break;

          case 3:
          PORT->Group[EFI_INJ3_GROUP].OUTSET.reg = 
              (1 << EFI_INJ3_PIN);
          EFI_TCC->INTFLAG.reg  = 
              EFI_TCC_INJ3_IFG;
          EFI_TCC->INTENSET.reg = 
              EFI_TCC_INJ3_IFG;
          break;
          default:  //Should never get here
          m_injectorNumber = 0;
        }
        m_injectorNumber = (m_injectorNumber + 1)& 0x03;
        
        //Restart calculation for next injector in case another starts now
        goto INJECTOR_CALC_LABEL;
        
      }else if(0 == (cyc_left - cyc_req)){//on time case, if neither true don't calculate
        //increment CC register
        EFI_TCC->CC[EFI_TCC_INJ_CC_BASE + m_injectorNumber].reg =  
            (EFI_TCC->CC[EFI_TCC_NE_CC_NUM].reg + (cyc_req * tempPeriod - tempInjectionTime)) 
            & EFI_TCC_MAX;
        //Enable interrupts after clearing interrupt flags
        m_Injector[m_injectorNumber] = INJ_WAITING; 
        switch(m_injectorNumber){
          case 0:
          EFI_TCC->INTFLAG.reg  = 
              EFI_TCC_INJ0_IFG;
          EFI_TCC->INTENSET.reg = 
              EFI_TCC_INJ0_IFG;
          break;

          case 1:
          EFI_TCC->INTFLAG.reg  = 
              EFI_TCC_INJ1_IFG;
          EFI_TCC->INTENSET.reg = 
              EFI_TCC_INJ1_IFG;
          break;

          case 2:
          EFI_TCC->INTFLAG.reg  = 
              EFI_TCC_INJ2_IFG;
          EFI_TCC->INTENSET.reg = 
              EFI_TCC_INJ2_IFG;
          break;

          case 3:
          EFI_TCC->INTFLAG.reg  = 
              EFI_TCC_INJ3_IFG;
          EFI_TCC->INTENSET.reg = 
              EFI_TCC_INJ3_IFG;
          break;
          default:  //Should never get here
          m_injectorNumber = 0;
        }
        m_injectorNumber = (m_injectorNumber + 1)& 0x03;
        //Restart calculation for next injector in case another starts now
        goto INJECTOR_CALC_LABEL;
      }
    }else{//Fuel cut
      //Turn off all injectors
      PORT->Group[EFI_INJ0_PIN].OUTCLR.reg  = 
          (1 << EFI_INJ0_PIN);
      PORT->Group[EFI_INJ1_PIN].OUTCLR.reg  = 
          (1 << EFI_INJ1_PIN);
      PORT->Group[EFI_INJ2_PIN].OUTCLR.reg  = 
          (1 << EFI_INJ2_PIN);
      PORT->Group[EFI_INJ3_PIN].OUTCLR.reg  = 
          (1 << EFI_INJ3_PIN);
      //Set all FSMs to IDLE
      m_Injector[0]   = INJ_IDLE;
      m_Injector[1]   = INJ_IDLE;
      m_Injector[2]   = INJ_IDLE;
      m_Injector[3]   = INJ_IDLE;
    }
  }else{//Handles case of loss of NE
    //Send spark low, clear out 
    m_Igniter                               = IGT_IDLE;
    //Disable interrupt, send pin low
    EFI_TCC->INTENCLR.reg = EFI_TCC_IGT_IFG;
    PORT->Group[EFI_IGT_GROUP].OUTCLR.reg   = (1 << EFI_IGT_PIN);
    m_igniterStartCCR   = EFI_TCC_MAX + 1;
    m_isNotFirstPulse   = 0;
    m_nextNeIsStart     = 0;
    m_currentNe         = 5;
    m_injectorNumber    = 0;
  }

  }else{
  m_isNotFirstPulse = 1; 
  }    
  //Save current CCR for later
  m_lastCCR = tempCCR;

}


//ISR for igniter
void EFI_TCC_IGT_ISR(void){
  //Clear flag
  EFI_TCC->INTFLAG.reg = 
      EFI_TCC_IGT_IFG;

  switch(m_Igniter){

    //This starts the charge cycle
    case IGT_WAITING:
    //Pin high
    PORT->Group[EFI_IGT_GROUP].OUTSET.reg = 
        (1 << EFI_IGT_PIN);
    //Increment CCR by dwell time
    EFI_TCC->CC[EFI_TCC_IGT_CC_NUM].reg += EFI_IGNITION_DWELL_CCR;
    //Next state is dwell
    m_Igniter = IGT_DWELL;
    break;

    //This turns off the charge signal, creating the spark
    case IGT_DWELL:
    //Pin low
    PORT->Group[EFI_IGT_GROUP].OUTCLR.reg = 
        (1 << EFI_IGT_PIN);
    //Increment CCR by spark time
    EFI_TCC->CC[EFI_TCC_IGT_CC_NUM].reg += 
        EFI_IGNITION_SPARK_CCR;
    //Next state is spark
    m_Igniter = IGT_SPARK;
    break;

    //This is for the spark delay, 
    //this will also kick off the next ignition pulse or go back to idle
    case IGT_SPARK:
    //Check if there is a value in the next CCR
    if((EFI_TCC_MAX) >= m_igniterStartCCR){
    //Check if difference between start and current CC is greater than ~ 2 periods
    //If we missed the mark, skip wait cycle and go straight to dwell
      if( (Sensors.Period << 1) <= 
            (((m_igniterStartCCR - 20)  - EFI_TCC->CC[EFI_TCC_IGT_CC_NUM].reg) & EFI_TCC_MAX)){
        //Set pin high
        PORT->Group[EFI_IGT_GROUP].OUTSET.reg = 
            (1 << EFI_IGT_PIN);
        //Load into CCR
        EFI_TCC->CC[EFI_TCC_IGT_CC_NUM].reg = 
            (EFI_IGNITION_DWELL_CCR + m_igniterStartCCR)& EFI_TCC_MAX;
        //Next state is dwell
        m_Igniter = IGT_DWELL;

      }else{
        //Load start into CCR
        EFI_TCC->CC[EFI_TCC_IGT_CC_NUM].reg = 
            m_igniterStartCCR;
        //Next state is waiting
        m_Igniter = IGT_WAITING;
      }
      //Reset start CCR value
      m_igniterStartCCR = EFI_TCC_MAX + 1;
      break;
    }
    else{//If no new data turn off interrupt and go to idle
    EFI_TCC->INTENCLR.reg = 
        EFI_TCC_IGT_IFG;

    }    

    default: //Handles idle and everything else, stop interrupts
    EFI_TCC->INTENCLR.reg = 
        EFI_TCC_IGT_IFG;
    PORT->Group[EFI_IGT_GROUP].OUTCLR.reg = 
        (1 << EFI_IGT_PIN);
    m_Igniter  = IGT_IDLE;
    }
  }

  //Injector 0 ISR
  void EFI_TCC_INJ0_ISR(void){
  //Clear flag
  EFI_TCC->INTFLAG.reg = EFI_TCC_INJ0_IFG;

  //Process FSM
  switch(m_Injector[0]){
    //This is where the injector is turned ON prepares for dwell
    case INJ_WAITING:
    //Send Injector pin high
    PORT->Group[EFI_INJ0_GROUP].OUTSET.reg = 
        (1 << EFI_INJ0_PIN);
    //Increment CCR by Desired Dwell
    EFI_TCC->CC[EFI_TCC_INJ_CC_BASE + 0].reg += 
        getInjectionTime();
    //Next state is IDLE
    m_Injector[0] = INJ_IDLE;
    break;

    //This is where the injector is turned OFF returns to Idle state with interrupts off
    default:
    //Lower control output and disable interrupt
    PORT->Group[EFI_INJ0_GROUP].OUTCLR.reg = 
        (1 << EFI_INJ0_PIN);
    EFI_TCC->INTENCLR.reg = 
        EFI_TCC_INJ0_IFG;  
  }
}

//Injector 1 ISR
void EFI_TCC_INJ1_ISR(void){
  //Clear flag
  EFI_TCC->INTFLAG.reg = EFI_TCC_INJ1_IFG;

  //Process FSM
  switch(m_Injector[1]){
    //This is where the injector is turned ON prepares for dwell
    case INJ_WAITING:
    //Send Injector pin high
    PORT->Group[EFI_INJ1_GROUP].OUTSET.reg = 
        (1 << EFI_INJ1_PIN);
    //Increment CCR by Desired Dwell
    EFI_TCC->CC[EFI_TCC_INJ_CC_BASE + 1].reg += 
        getInjectionTime();
    //Next state is IDLE
    m_Injector[1] = INJ_IDLE;
    break;

    //This is where the injector is turned OFF returns to Idle state with interrupts off
    default:
    //Lower control output and disable interrupt
    PORT->Group[EFI_INJ1_GROUP].OUTCLR.reg = 
        (1 << EFI_INJ1_PIN);
    EFI_TCC->INTENCLR.reg = 
        EFI_TCC_INJ1_IFG;  
  }
}

//Injector 2 ISR
void EFI_TCC_INJ2_ISR(void){
  //Clear flag
  EFI_TCC->INTFLAG.reg = 
      EFI_TCC_INJ2_IFG;

  //Process FSM
  switch(m_Injector[2]){
    //This is where the injector is turned ON prepares for dwell
    case INJ_WAITING:
    //Send Injector pin high
    PORT->Group[EFI_INJ2_GROUP].OUTSET.reg = 
        (1 << EFI_INJ2_PIN);
    //Increment CCR by Desired Dwell
    EFI_TCC->CC[EFI_TCC_INJ_CC_BASE + 2].reg += 
        getInjectionTime();
    //Next state is IDLE
    m_Injector[2] = INJ_IDLE;
    break;

    //This is where the injector is turned OFF returns to Idle state with interrupts off
    default:
    //Lower control output and disable interrupt
    PORT->Group[EFI_INJ2_GROUP].OUTCLR.reg = 
        (1 << EFI_INJ2_PIN);
    EFI_TCC->INTENCLR.reg = 
        EFI_TCC_INJ2_IFG;  
  }
}

//Injector 3 ISR
void EFI_TCC_INJ3_ISR(void){
  //Clear flag
  EFI_TCC->INTFLAG.reg = 
      EFI_TCC_INJ3_IFG;

  //Process FSM
  switch(m_Injector[3]){
    //This is where the injector is turned ON prepares for dwell
    case INJ_WAITING:
    //Send Injector pin high
    PORT->Group[EFI_INJ3_GROUP].OUTSET.reg = 
        (1 << EFI_INJ3_PIN);
    //Increment CCR by Desired Dwell
    EFI_TCC->CC[EFI_TCC_INJ_CC_BASE + 3].reg += 
        getInjectionTime();
    //Next state is IDLE
    m_Injector[3] = INJ_IDLE;
    break;

    //This is where the injector is turned OFF returns to Idle state with interrupts off
    default:
    //Lower control output and disable interrupt
    PORT->Group[EFI_INJ3_GROUP].OUTCLR.reg = 
        (1 << EFI_INJ3_PIN);
    EFI_TCC->INTENCLR.reg = 
        EFI_TCC_INJ3_IFG;  
  }
}