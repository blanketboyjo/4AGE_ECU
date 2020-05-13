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
  //Initialize Idle, O2 and Start pins
  IDLE_SW_CONFIG;
  OX_CONFIG;
  START_SW_CONFIG;

  //Configure update timer
  //Setup update timer
  //Enable it on the main bus
  SENSORS_TC_START_BUS;

  //Give it a glk
  GCLK->PCHCTRL[SENSORS_TC_PCLK].reg =
      SENSORS_TC_GLK_SOURCE |
      GCLK_PCHCTRL_CHEN;
  
  //Software reset it
  SENSORS_TC->COUNT16.CTRLA.bit.SWRST =
      1;

  //Wait for reset to finish
  while(0 != (SENSORS_TC->COUNT16.CTRLA.reg & TC_CTRLA_SWRST));

  //Setup is based on configuration in pins.h
  SENSORS_TC->COUNT16.CTRLA.reg =
      SENSORS_TC_PRESCALE |
      TCC_CTRLA_RUNSTDBY;

  //Enable overflow interrupt after clearing interrupt flag
  SENSORS_TC->COUNT16.INTFLAG.reg   =
    TC_INTFLAG_OVF;
  SENSORS_TC->COUNT16.INTENSET.reg  =
    TC_INTENSET_OVF;

  //Enable interrupt at NVIC
  __NVIC_SetPriority(SENSORS_TC_IRQN, SENSORS_ISR_PRIORITY);
  __NVIC_EnableIRQ(SENSORS_TC_IRQN);
}

void sensorsStart(void){
  //Call subsystem start functions
  analogStart();
  pressureStart();
  //Start update timer
  SENSORS_TC->COUNT16.CTRLA.bit.ENABLE = 1;
}

void sensorsHalt(void){
  //Call subsystem halt functions
  analogHalt();
  pressureHalt();
  //Start update timer
  SENSORS_TC->COUNT16.CTRLA.bit.ENABLE = 0;
}


/**************************************************************************
*
*      Interrupt service routine
*
**************************************************************************/
//Poll digital sensors
void SENSORS_TC_ISR(void){
  //Clear overflow flag
  SENSORS_TC->COUNT16.INTFLAG.reg = TC_INTFLAG_OVF;

  //Update IDLE, OX, and START
  Sensors.IsCranking      = (PORT->Group[START_SW_GROUP].IN.reg >> START_SW_PIN)& 1;
  Sensors.ThrottleClosed  = (PORT->Group[IDLE_SW_GROUP].IN.reg  >>  IDLE_SW_PIN)& 1;
  Sensors.IsCranking      = (PORT->Group[OX_GROUP].IN.reg >> OX_PIN)& 1;

}