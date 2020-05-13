/*
* Analog.cpp
*
* Created: 5/2/2019 11:39:33 PM
*  Author: Jordan
*/ 
#include "Analog.h"
//Local variable for if the system is running
int m_analogRunning;
unsigned int m_rawWaterTemp;
unsigned int m_rawAirTemp;


//This will setup the necessary systems
void analogConfig(void){
  //Set flag
  m_analogRunning = 0;

  //Clear sensor data to be sure
  Sensors.AirFlow     = 0;
  Sensors.AirTemp     = 0;
  Sensors.WaterTemp   = 0;
  Sensors.Throttle    = 0;
  //Setup pins
  AIR_TEMP_CONFIG;
  AIR_FLOW_METER_CONFIG;
  THROTTLE_CONFIG;
  WATER_TEMP_CONFIG;


  //Enable ADC bus clocks
  MCLK->APBDMASK.bit.ADC1_ = 1;
  MCLK->APBDMASK.bit.ADC0_ = 1;

  //ADC GCLK source is GCLK_0
  GCLK->PCHCTRL[ADC0_GCLK_ID].reg =
      GCLK_PCHCTRL_GEN_GCLK1  |   //  GCLK_1 as source
      GCLK_PCHCTRL_CHEN;          //  Enable peripheral clk

  GCLK->PCHCTRL[ADC1_GCLK_ID].reg =   
      GCLK_PCHCTRL_GEN_GCLK1  |   //  GCLK_1 as source
      GCLK_PCHCTRL_CHEN;          //  Enable peripheral clk

  //Reset ADC0
  ADC0->CTRLA.bit.SWRST = 
      1;

  // Wait for ADC to finish reset, bit will stay high until reset clears the bit
  while(ADC0->CTRLA.bit.SWRST);

  //Enable ADC1, aim is standby running, ~480Khz sampling (one sample)
  ADC0->CTRLA.reg     =   
      ADC_CTRLA_PRESCALER_DIV16   |   //  Divide input clock by 16
      ADC_CTRLA_RUNSTDBY          ;   //  Run in standby mode

  //Setup averaging
  ADC0->AVGCTRL.reg   =   
      ADC_AVGCTRL_ADJRES(4)       |   //  Based on table 45-3
      ADC_AVGCTRL_SAMPLENUM_256;      //  128 Samples to average    

  //Clear CTRLB to be safe
  ADC0->CTRLB.reg     =   
      ADC_CTRLB_RESSEL_12BIT;         //  We want 12 bit output
  ADC0->REFCTRL.reg   =   
      ADC_REFCTRL_REFSEL_INTVCC1;     //  Use VDDANA
  ADC0->INPUTCTRL.reg =   
      AIR_TEMP_AMUX;
  ADC0->INTENSET.bit.RESRDY = 
      1;

  //Reset ADC1
  ADC1->CTRLA.bit.SWRST = 
      1;

  // Wait for ADC to finish reset, bit will stay high until reset clears the bit
  while(ADC1->CTRLA.bit.SWRST);

  //Enable ADC1, aim is standby running, ~480Khz sampling (one sample)
  ADC1->CTRLA.reg     =   
      ADC_CTRLA_PRESCALER_DIV16   |   //  Divide input clock by 16
      ADC_CTRLA_RUNSTDBY          ;   //  Run in standby mode

  //Setup averaging
  ADC1->AVGCTRL.reg   =   
      ADC_AVGCTRL_ADJRES(4)       |   //  Based on table 45-3
      ADC_AVGCTRL_SAMPLENUM_256;      //  256 Samples to average

  //Clear CTRLB to be safe
  ADC1->CTRLB.reg     = 
      ADC_CTRLB_RESSEL_12BIT;           //  We want 12 bit output
  ADC1->REFCTRL.reg   = 
      ADC_REFCTRL_REFSEL_INTVCC1;       //  Use VDDANA
  ADC1->INPUTCTRL.reg = 
      AIR_TEMP_AMUX;                
  ADC1->INTENSET.bit.RESRDY = 
      1;



  //Enable ADCs
  ADC0->CTRLA.bit.ENABLE =   
      1;
  ADC1->CTRLA.bit.ENABLE =   
      1;

  //Make interrupts lower priority
  NVIC_SetPriority(ADC0_1_IRQn, ANALOG_ISR_PRIORITY);
  NVIC_SetPriority(ADC1_1_IRQn, ANALOG_ISR_PRIORITY);

  //Enable both ADC interrupts
  NVIC_EnableIRQ(ADC0_1_IRQn);
  NVIC_EnableIRQ(ADC1_1_IRQn);

}

//Starts ADC
void analogStart(void){
  //Exit if already running
  if(1 == m_analogRunning)return;
  //Setup Amux
  ADC0->INPUTCTRL.reg = 
      AIR_FLOW_METER_AMUX;
  ADC1->INPUTCTRL.reg = 
      AIR_TEMP_AMUX;
  //Start ADC0
  ADC0->SWTRIG.reg    = 
      ADC_SWTRIG_START;
  //Set running flag
  m_analogRunning = 1;
}

//Stops timer
void analogHalt(void){
  //Exit if already stopped
  if(0 == m_analogRunning)return;
  m_analogRunning = 0;
}

//Returns 1 is sensor is running
int analogIsRunning(void){
  return m_analogRunning;
}

//Verifies data from ADC before saving it
void ADC0_1_Handler(void){
  //Check which data was just sampled

  unsigned int adcValue = ADC0->RESULT.reg;
  switch(ADC0->INPUTCTRL.reg & ADC_INPUTCTRL_MUXPOS_Msk){
    case AIR_FLOW_METER_AMUX:
    //Check that value does not exceed limits
    if(adcValue > AIR_FLOW_MAX){
      //Increment AIR Flow high fault count

      //Load safe value
      Sensors.AirFlow = AIR_FLOW_SAFE;
    }else if(adcValue < AIR_FLOW_MIN){
      //Increment AIR Flow low fault count

      //Load safe value
      Sensors.AirFlow = AIR_FLOW_SAFE;
    }else{
      //Clear fault counts
      //Load data
      Sensors.AirFlow = adcValue;
    }
    //Load next ADCMUX for next conversion
    ADC0->INPUTCTRL.reg = AIR_FLOW_METER_AMUX;
    //Only start next conversion if analog should be running
    if(0 == m_analogRunning)break;
    ADC1->SWTRIG.reg    = ADC_SWTRIG_START;
    break;

    default:   //Really it should never get here
    ADC0->INPUTCTRL.reg = AIR_FLOW_METER_AMUX;
    break;
  }
}

//Verifies data from ADC before saving it
void ADC1_1_Handler(void){
  //Check which data was just sampled
  unsigned int adcValue = ADC1->RESULT.reg;
  switch(ADC1->INPUTCTRL.reg & ADC_INPUTCTRL_MUXPOS_Msk){
    case AIR_TEMP_AMUX:
    //Check that value does not exceed limits
    if(adcValue > AIR_TEMP_MAX){
      //Load safe value
      Sensors.AirTemp = AIR_TEMP_SAFE;

    }else if(adcValue < AIR_TEMP_MIN){
      //Load safe value
      Sensors.AirTemp = AIR_TEMP_SAFE;
      
    }else{
    //Clear fault counts
    //Load data
    Sensors.AirTemp = adcValue;
    }
    
    //Load next ADCMUX for next conversion
    ADC1->INPUTCTRL.reg = WATER_TEMP_AMUX;

    //Only start next conversion if analog should be running
    if(0 == m_analogRunning)break;
    ADC1->SWTRIG.reg    = ADC_SWTRIG_START;

    break;

    case WATER_TEMP_AMUX:
    //Check that value does not exceed limits
    if(adcValue > WATER_TEMP_MAX){
      //Load safe value
      Sensors.WaterTemp = WATER_TEMP_SAFE;
      
    }else if(adcValue < WATER_TEMP_MIN){
      //Load safe value
      Sensors.WaterTemp = WATER_TEMP_SAFE;
      
    }else{
      //Calculate temperature
      if(adcValue < 488){         //176-212
        Sensors.WaterTemp = 277930 - (adcValue * 209);
      }else if(adcValue < 566){   //140-176'
        Sensors.WaterTemp = 400769 - (adcValue * 462);       
      }else if(adcValue < 836){   //104 - 140
        Sensors.WaterTemp = 215333 - (adcValue * 133);
      }else if(adcValue < 1309){  //68 - 104
        Sensors.WaterTemp = 167552 - (adcValue * 76); 
      }else{//32-68
        Sensors.WaterTemp = 126934 - (adcValue * 45);
      }
    }
    //Load next ADCMUX for next conversion
    ADC1->INPUTCTRL.reg = THROTTLE_AMUX;

    //Only start next conversion if analog should be running
    if(0 == m_analogRunning)break;
    ADC1->SWTRIG.reg    = ADC_SWTRIG_START;
    break;

    case THROTTLE_AMUX:
    //Check that value does not exceed limits
    if(adcValue > THROTTLE_MAX){
      //Load safe value
      Sensors.Throttle = THROTTLE_SAFE;
      
    }else if(adcValue < THROTTLE_MIN){
      //Load safe value
      Sensors.Throttle = THROTTLE_SAFE;
    }else{
      //Load data
      Sensors.Throttle = adcValue;
    }

    //Load next ADCMUX for next conversion
    ADC1->INPUTCTRL.reg = AIR_TEMP_AMUX;
    //Only start next conversion if analog should be running
    if(0 == m_analogRunning)break;
    ADC0->SWTRIG.reg    = ADC_SWTRIG_START;
    break;

    default:   //Really it should never get here
    ADC1->INPUTCTRL.reg = AIR_TEMP_AMUX;
    ADC0->SWTRIG.reg    = ADC_SWTRIG_START;
    break;
  }
}
