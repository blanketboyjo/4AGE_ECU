/*
* Pressure.cpp
*
* Created: 5/6/2019 1:30:27 PM
*  Author: Jordan
*/ 
#include "Pressure.h"

#define MPRL_TRIGGER_COMMAND    0x0000AA
#define MPRL_READ_COMMAND       0x0000F0

volatile int m_pressureRunning      = 0;
volatile int m_pressureReady        = 0;

enum PressureState {HALT, TRIGGER, READ};
PressureState m_pressureState;

// This function configures all pins and devices
void    pressureConfig(void){
  //Pin config
  PRESSURE_MOSI_CONFIG;
  PRESSURE_MISO_CONFIG;
  PRESSURE_SCK_CONFIG;
  PRESSURE_CS_CONFIG;
  PRESSURE_CONV_CONFIG;
  PRESSURE_RESET_CONFIG;

  //Raise both CS pins
  PORT->Group[PRESSURE_CS_GROUP].OUTSET.reg = 
      (1<< PRESSURE_CS_PIN);


  //Lower flag
  m_pressureRunning   =   0;
  m_pressureState     =   HALT;

  //Setup PRESSURE_SERCOM in SPI master mode
  //Enable it on the bus
  PRESSURE_START_BUS;
  
  //Give it a clock source
  GCLK->PCHCTRL[PRESSURE_SERCOM_PCLK].reg =   
      PRESSURE_GLK_SOURCE  |
      GCLK_PCHCTRL_CHEN;
      
  //Reset SERCOM
  PRESSURE_SERCOM->SPI.CTRLA.bit.SWRST    =   
      1;

  //Wait for it complete
  while(1 == PRESSURE_SERCOM->SPI.CTRLA.bit.SWRST);

  PRESSURE_SERCOM->SPI.BAUD.reg           =   
      PRESSURE_SERCOM_BAUD;                     //See pins.h
  PRESSURE_SERCOM->SPI.CTRLA.reg          = 
      SERCOM_SPI_CTRLA_MODE(0x3)            |   //Master mode
      SERCOM_SPI_CTRLA_DOPO(PRESSURE_DOPO)  |   //See pins.h
      SERCOM_SPI_CTRLA_DIPO(PRESSURE_DIPO);     //See pins.h

  PRESSURE_SERCOM->SPI.LENGTH.reg         =   
      SERCOM_SPI_LENGTH_LENEN |  //Enable multibyte send
      SERCOM_SPI_LENGTH_LEN(3);  //Set length to 3 byte mode
  
  PRESSURE_SERCOM->SPI.CTRLC.bit.DATA32B  = 
      1;  //Enable 32bit read/write to data
  
  PRESSURE_SERCOM->SPI.CTRLB.bit.RXEN     = 
      1;  //Enable data read back
  
  PRESSURE_SERCOM->SPI.INTENSET.bit.RXC   = 
      1;  //Enable RX data interrupt
  
  PRESSURE_SERCOM->SPI.CTRLA.bit.ENABLE   = 
      1;  //Enable PRESSURE_SERCOM


  //Setup EIC
  PRESSURE_CONV_EIC_CONFIG;
  EIC->INTENSET.reg   = 
      (1 << PRESSURE_CONV_EIC);

  //Configure priorities
  NVIC_SetPriority(PRESSURE_CONV_IRQN,    PRESSURE_ISR_PRIORITY);
  NVIC_SetPriority(PRESSURE_SERCOM_IRQN,  PRESSURE_ISR_PRIORITY);

  //Enable interrupts at NVIC
  NVIC_EnableIRQ(PRESSURE_CONV_IRQN);
  NVIC_EnableIRQ(PRESSURE_SERCOM_IRQN);
}

// This function will kick off the reading routine
void    pressureStart(void){
  //Only start if not running 
  if(1 == m_pressureRunning) return;
  //Reset pressure sensors before starting
  PORT->Group[PRESSURE_RESET_GROUP].OUTCLR.reg = 
      (1 << PRESSURE_RESET_PIN);
  PORT->Group[PRESSURE_RESET_GROUP].DIRSET.reg = 
      (1 << PRESSURE_RESET_PIN);

  //Wait for a moment
  for(unsigned int i = 0; i <75000; i++);

  //Clear reset condition
  PORT->Group[PRESSURE_RESET_GROUP].DIRCLR.reg = 
      (1 << PRESSURE_RESET_PIN);

  //Wait for reset
  for(unsigned int i = 0; i <500000; i++);

  m_pressureState = TRIGGER;    //Prepare to manifold ambient sensor;
  //Lower CS
  PORT->Group[PRESSURE_CS_GROUP].OUTCLR.reg =    
      (1 << PRESSURE_CS_PIN);

  //Set sercom into 4 byte mode
  PRESSURE_SERCOM->SPI.LENGTH.reg =   
      SERCOM_SPI_LENGTH_LENEN     |
      SERCOM_SPI_LENGTH_LEN(3);

  //Send data
  PRESSURE_SERCOM->SPI.DATA.reg   =   
      MPRL_TRIGGER_COMMAND;

  m_pressureRunning   = 1;
  m_pressureReady     = 0;
}

//Stops the read cycle
void pressureHalt(void){
  //Only halt if running
  if(0 == m_pressureRunning) return;
  m_pressureState = HALT;
}


/**************************************************************************
*
*      Interrupt service routines
*
**************************************************************************/


//Conversion complete for Manifold
void PRESSURE_CONV_ISR(void){
  EIC->INTFLAG.reg = (1<<1);

  //Allow bus to settle
  for(unsigned int i = 0; i < 20; i++);

  //Set sercom into 4 byte mode
  PRESSURE_SERCOM->SPI.LENGTH.reg =   
      SERCOM_SPI_LENGTH_LENEN     |
      SERCOM_SPI_LENGTH_LEN(4);
  //Lower CS
  PORT->Group[PRESSURE_CS_GROUP].OUTCLR.reg =    
      (1 << PRESSURE_CS_PIN);
  //Send data
  PRESSURE_SERCOM->SPI.DATA.reg =   
      MPRL_READ_COMMAND;
}

//Sercom ISR, handles data io
void    PRESSURE_SERCOM_ISR(void){
  //Only end up here if there is data in the data buffer, clears flag
  unsigned int temp = PRESSURE_SERCOM->SPI.DATA.reg;

  //Case based on FSM state
  switch(m_pressureState){
    case TRIGGER:  //Got poll command
    //Set next state
    m_pressureState = READ;
    //Stall for a bit of time to match sensor spec
    for(unsigned int i = 0; i < 18; i++);
    //Raise CS for manifold sensor
    PORT->Group[PRESSURE_CS_GROUP].OUTSET.reg = 
        (1<< PRESSURE_CS_PIN);

    break;

    case READ: //Received sensor data by this point in time
    //Set next state
    m_pressureState = TRIGGER;

    //Stall for a bit of time to match sensor spec
    for(unsigned int i = 0; i < 18; i++);
    //Raise CS for manifold sensor
    PORT->Group[PRESSURE_CS_GROUP].OUTSET.reg      = (1<< PRESSURE_CS_PIN);

    //Process data into buffer
    temp = (temp >>8)& 0xFFFFFF;
    Sensors.ManifoldPressure = (((temp & 0xFF)<<16)|(temp & 0xFF00)|
                                ((temp & 0xFF0000)>>16))/1452;


    //Reset pressure sensors before starting
    PORT->Group[PRESSURE_RESET_GROUP].OUTCLR.reg = 
        (1 << PRESSURE_RESET_PIN);
    PORT->Group[PRESSURE_RESET_GROUP].DIRSET.reg = 
        (1 << PRESSURE_RESET_PIN);

    //Wait for a moment
    for(unsigned int i = 0; i <75000; i++);

    //Clear reset condition
    PORT->Group[PRESSURE_RESET_GROUP].DIRCLR.reg = 
        (1 << PRESSURE_RESET_PIN);

    //Wait for reset
    for(unsigned int i = 0; i <500000; i++);

    m_pressureState = TRIGGER;    //Prepare to manifold ambient sensor;
    //Lower CS
    PORT->Group[PRESSURE_CS_GROUP].OUTCLR.reg =    
        (1 << PRESSURE_CS_PIN);

    //Set sercom into 4 byte mode
    PRESSURE_SERCOM->SPI.LENGTH.reg     =   
        SERCOM_SPI_LENGTH_LENEN     |
        SERCOM_SPI_LENGTH_LEN(3);

    //Send data
    PRESSURE_SERCOM->SPI.DATA.reg =   
        MPRL_TRIGGER_COMMAND;
    m_pressureRunning   = 1;
    m_pressureReady     = 0;

    break;
    default: //Should never get here
    break;
  }
}