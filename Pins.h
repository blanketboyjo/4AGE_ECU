/*
 * Pins.h
 *
 * Created: 4/29/2019 7:40:30 AM
 *  Author: Jordan
 */ 


#ifndef PINS_H_
#define PINS_H_
#include "sam.h"

/*****************************************************************************
 *
 *                              ISR Priorities
 *
******************************************************************************/

#define PRESSURE_ISR_PRIORITY       4
#define ANALOG_ISR_PRIORITY         4
#define EFI_NE_PRIORITY             2
#define EFI_IGNITION_PRIORITY       1
#define EFI_INJECTOR_PRIORITY       2
#define EFI_NE_OVF_PRIORITY         3
#define DASHBOARD_ISR_PRIORITY      6
#define TERMINAL_ISR_PRIORITY       5
#define SENSORS_ISR_PRIORITY        3


/*****************************************************************************
 *
 *                              
 *
******************************************************************************/


/*****************************************************************************
 *
 *                              Analog Pins/Config
 *
******************************************************************************/
//These values are for throwing errors
#define THROTTLE_MAX    12412
#define THROTTLE_MIN    250
#define THROTTLE_SAFE   0
#define AIR_FLOW_MAX    12420
#define AIR_FLOW_MIN    3000
#define AIR_FLOW_SAFE   0
#define AIR_TEMP_MAX    2300
#define AIR_TEMP_MIN    300
#define AIR_TEMP_SAFE   0
#define WATER_TEMP_MAX  2300
#define WATER_TEMP_MIN  300
#define WATER_TEMP_SAFE 0


//PA03 uses ADC0
#define AIR_FLOW_METER_PIN      3
#define AIR_FLOW_METER_GROUP    0
#define AIR_FLOW_METER_PMUX     PMUX_FUNC_B
#define AIR_FLOW_METER_AMUX     ADC_INPUTCTRL_MUXPOS_AIN1
#define AIR_FLOW_METER_CONFIG   PIN_CONFIG_PMUXO(AIR_FLOW_METER_PIN,   \
                                                 AIR_FLOW_METER_GROUP, \
                                                 AIR_FLOW_METER_PMUX)


//PB05 uses ADC1
#define AIR_TEMP_PIN            5
#define AIR_TEMP_GROUP          1           
#define AIR_TEMP_PMUX           PMUX_FUNC_B
#define AIR_TEMP_AMUX           ADC_INPUTCTRL_MUXPOS_AIN7
#define AIR_TEMP_CONFIG         PIN_CONFIG_PMUXO(AIR_TEMP_PIN,   \
                                                 AIR_TEMP_GROUP, \
                                                 AIR_TEMP_PMUX)

//PB04 uses ADC1
#define THROTTLE_PIN            4
#define THROTTLE_GROUP          1
#define THROTTLE_PMUX           PMUX_FUNC_B
#define THROTTLE_AMUX           ADC_INPUTCTRL_MUXPOS_AIN6
#define THROTTLE_CONFIG         PIN_CONFIG_PMUXE(THROTTLE_PIN,   \
                                                 THROTTLE_GROUP, \
                                                 THROTTLE_PMUX)

//PB06 uses ADC1
#define WATER_TEMP_PIN          6
#define WATER_TEMP_GROUP        1
#define WATER_TEMP_PMUX         PMUX_FUNC_B
#define WATER_TEMP_AMUX         ADC_INPUTCTRL_MUXPOS_AIN8
#define WATER_TEMP_CONFIG       PIN_CONFIG_PMUXE(WATER_TEMP_PIN,   \
                                                 WATER_TEMP_GROUP, \
                                                 WATER_TEMP_PMUX)

/*****************************************************************************
 *
 *                      Air Pressure Pins
 *
******************************************************************************/
#define PRESSURE_SERCOM         SERCOM0
#define PRESSURE_SERCOM_PCLK    SERCOM0_GCLK_ID_CORE
#define PRESSURE_DOPO           0x00
#define PRESSURE_DIPO           0x03
#define PRESSURE_START_BUS      MCLK->APBAMASK.bit.SERCOM0_ =   1;
#define PRESSURE_GLK_SOURCE     GCLK_PCHCTRL_GEN_GCLK1
//Baud Register is (50MHz/750KHz/2) - 1 = 33 (round up 34)
#define PRESSURE_SERCOM_BAUD    34

#define PRESSURE_CONV_IRQN      EIC_1_IRQn
#define PRESSURE_SERCOM_IRQN    SERCOM0_2_IRQn
#define PRESSURE_CONV_ISR       EIC_1_Handler
#define PRESSURE_SERCOM_ISR     SERCOM0_2_Handler


//PA04 is MOSI
#define PRESSURE_MOSI_PIN       4
#define PRESSURE_MOSI_GROUP     0
#define PRESSURE_MOSI_PMUX      PMUX_FUNC_D
#define PRESSURE_MOSI_CONFIG    PIN_CONFIG_PMUXE(PRESSURE_MOSI_PIN,   \
                                                 PRESSURE_MOSI_GROUP, \
                                                 PRESSURE_MOSI_PMUX)

//PA05 is SCK
#define PRESSURE_SCK_PIN        5
#define PRESSURE_SCK_GROUP      0
#define PRESSURE_SCK_PMUX       PMUX_FUNC_D
#define PRESSURE_SCK_CONFIG     PIN_CONFIG_PMUXO(PRESSURE_SCK_PIN,    \
                                                 PRESSURE_SCK_GROUP,  \
                                                 PRESSURE_SCK_PMUX)

//PA07 is MISO
#define PRESSURE_MISO_PIN       7
#define PRESSURE_MISO_GROUP     0
#define PRESSURE_MISO_PMUX      PMUX_FUNC_D
#define PRESSURE_MISO_CONFIG    PIN_CONFIG_PMUXO(PRESSURE_MISO_PIN,   \
                                                 PRESSURE_MISO_GROUP, \
                                                 PRESSURE_MISO_PMUX)

//PA06 is CS for Manifold
#define PRESSURE_CS_PIN         2
#define PRESSURE_CS_GROUP       0
#define PRESSURE_CS_CONFIG      PIN_CONFIG_OUTPUT(PRESSURE_CS_PIN,     \
                                                  PRESSURE_CS_GROUP)

//PA01 is Conversion complete for Manifold
#define PRESSURE_CONV_EIC       1
#define PRESSURE_CONV_EIC_CONFIG EIC->CONFIG[0].reg &= ~(EIC_CONFIG_SENSE1_Msk); \
                                 EIC->CONFIG[0].reg |= (EIC_CONFIG_SENSE1_RISE);
#define PRESSURE_CONV_PIN       1
#define PRESSURE_CONV_GROUP     0
#define PRESSURE_CONV_PMUX      PMUX_FUNC_A
#define PRESSURE_CONV_CONFIG    PIN_CONFIG_PMUXO(PRESSURE_CONV_PIN,     \
                                                 PRESSURE_CONV_GROUP,   \
                                                 PRESSURE_CONV_PMUX)

//PA13 is Pressure sensor reset
#define PRESSURE_RESET_PIN      13
#define PRESSURE_RESET_GROUP    0
#define PRESSURE_RESET_CONFIG   PIN_CONFIG_INPUT(PRESSURE_RESET_PIN,    \
                                                 PRESSURE_RESET_GROUP)

/*****************************************************************************
 *
 *                          EFI configuration
 *
 *****************************************************************************/
#define EFI_TCC_FINAL_FREQ          500000
#define EFI_NE_PER_ROTATION         2
//All values in 1/100th of a degree
#define EFI_DEGREES_PER_NE          36000/EFI_NE_PER_ROTATION
#define EFI_DISTRIBUTOR_ADVANCE     1000
#define EFI_DEFAULT_ADVANCE         1400
//In units of 10^-5 sec
#define EFI_IGNITION_DWELL_TIME     500
#define EFI_IGNITION_SPARK_TIME     70
#define EFI_INJECTOR_DWELL_TIME     100      
#define EFI_INJECTOR_ON_TIME_VE     1290
#define EFI_FUEL_CUT_RPM            3000

//Converts time to timer counts
#define EFI_IGNITION_DWELL_CCR      (EFI_IGNITION_DWELL_TIME * EFI_TCC_FINAL_FREQ/100000)
#define EFI_IGNITION_SPARK_CCR      (EFI_IGNITION_SPARK_TIME * EFI_TCC_FINAL_FREQ/100000)
#define EFI_INJECTOR_DWELL_CCR      (EFI_INJECTOR_DWELL_TIME * EFI_TCC_FINAL_FREQ/100000)
#define EFI_INJECTOR_ON_CCR         (EFI_INJECTOR_ON_TIME    * EFI_TCC_FINAL_FREQ/100000)




//Final frequency is Frequency of source / 4
#define EFI_RPM_CONVERSION_RATIO    60*EFI_TCC_FINAL_FREQ/EFI_NE_PER_ROTATION
#define EFI_TCC                     TCC0
#define EFI_TCC_MAX                 0xFFFFFF
#define EFI_TCC_GCLK_ID             TCC0_GCLK_ID
#define EFI_TCC_ENABLE_BUS          MCLK->APBBMASK.bit.TCC0_    = 1;
#define EFI_TCC_GCLK_SOURCE         GCLK_PCHCTRL_GEN_GCLK2
#define EFI_TCC_NE_EVSYS_USER_ID    19     
#define EFI_TCC_NE_EVSYS_CH         1
#define EFI_TCC_NE_EVSYS_GLK_ID     EVSYS_GCLK_ID_1
#define EFI_TCC_NE_EVSYS_EVGEN      0x15



#define EFI_TCC_NE_CC_NUM           0
#define EFI_TCC_IGT_CC_NUM          1
#define EFI_TCC_INJ_CC_BASE         2

#define EFI_TCC_OVF_IRQN            TCC0_0_IRQn
#define EFI_TCC_NE_IRQN             TCC0_1_IRQn
#define EFI_TCC_IGT_IRQN            TCC0_2_IRQn
#define EFI_TCC_INJ0_IRQN           TCC0_3_IRQn
#define EFI_TCC_INJ1_IRQN           TCC0_4_IRQn
#define EFI_TCC_INJ2_IRQN           TCC0_5_IRQn
#define EFI_TCC_INJ3_IRQN           TCC0_6_IRQn
#define EFI_G_IRQN                  EIC_8_IRQn

#define EFI_TCC_OVF_ISR             TCC0_0_Handler
#define EFI_TCC_NE_ISR              TCC0_1_Handler
#define EFI_TCC_IGT_ISR             TCC0_2_Handler
#define EFI_TCC_INJ0_ISR            TCC0_3_Handler
#define EFI_TCC_INJ1_ISR            TCC0_4_Handler
#define EFI_TCC_INJ2_ISR            TCC0_5_Handler
#define EFI_TCC_INJ3_ISR            TCC0_6_Handler
#define EFI_G_ISR                   EIC_8_Handler

#define EFI_TCC_OVF_IFG             TCC_INTFLAG_OVF
#define EFI_TCC_NE_IFG              TCC_INTFLAG_MC0
#define EFI_TCC_IGT_IFG             TCC_INTFLAG_MC1
#define EFI_TCC_INJ0_IFG            TCC_INTFLAG_MC2
#define EFI_TCC_INJ1_IFG            TCC_INTFLAG_MC3
#define EFI_TCC_INJ2_IFG            TCC_INTFLAG_MC4
#define EFI_TCC_INJ3_IFG            TCC_INTFLAG_MC5

//NE will be on PB03 (EIC3)
#define EFI_NE_EIC                  3
#define EFI_NE_EIC_CONFIG           EIC->CONFIG[0].reg &= ~(EIC_CONFIG_SENSE3_Msk); \
                                    EIC->CONFIG[0].reg |= (EIC_CONFIG_SENSE3_RISE);
#define EFI_NE_PIN                  3
#define EFI_NE_GROUP                1
#define EFI_NE_PMUX                 PMUX_FUNC_A
#define EFI_NE_CONFIG               PIN_CONFIG_PMUXO(EFI_NE_PIN,    \
                                                     EFI_NE_GROUP,  \
                                                     EFI_NE_PMUX)

//G will be on PB08 (EIC8)
#define EFI_G_EIC                   8
#define EFI_G_EIC_CONFIG            EIC->CONFIG[1].reg &= ~(EIC_CONFIG_SENSE0_Msk); \
                                    EIC->CONFIG[1].reg |= (EIC_CONFIG_SENSE0_FALL);
#define EFI_G_PIN                   8
#define EFI_G_GROUP                 1
#define EFI_G_PMUX                  PMUX_FUNC_A
#define EFI_G_CONFIG                PIN_CONFIG_PMUXE(EFI_G_PIN,     \
                                                     EFI_G_GROUP,   \
                                                     EFI_G_PMUX)





//Igniter Control pin (PB02)
#define EFI_IGT_PIN     2
#define EFI_IGT_GROUP   1
#define EFI_IGT_CONFIG  PIN_CONFIG_OUTPUT_LOW(EFI_IGT_PIN,  \
                                              EFI_IGT_GROUP)

//Injector0 Control pin (PA22)
#define EFI_INJ0_PIN    22
#define EFI_INJ0_GROUP  0
#define EFI_INJ0_CONFIG PIN_CONFIG_OUTPUT_LOW(EFI_INJ0_PIN, \
                                              EFI_INJ0_GROUP)

//Injector1 Control pin (PA23)
#define EFI_INJ1_PIN    23
#define EFI_INJ1_GROUP  0
#define EFI_INJ1_CONFIG PIN_CONFIG_OUTPUT_LOW(EFI_INJ1_PIN, \
                                              EFI_INJ1_GROUP)

//Injector2 Control pin (PA24)
#define EFI_INJ2_PIN    24
#define EFI_INJ2_GROUP  0
#define EFI_INJ2_CONFIG PIN_CONFIG_OUTPUT_LOW(EFI_INJ2_PIN, \
                                              EFI_INJ2_GROUP)

//Injector3 Control pin (PA25)
#define EFI_INJ3_PIN    25
#define EFI_INJ3_GROUP  0
#define EFI_INJ3_CONFIG PIN_CONFIG_OUTPUT_LOW(EFI_INJ3_PIN, \
                                              EFI_INJ3_GROUP)

/*****************************************************************************
 *
 *                          Terminal Pins
 *
 *****************************************************************************/
#define TERMINAL_TX_PIN         31
#define TERMINAL_TX_GROUP       1
#define TERMINAL_TX_PMUX        PMUX_FUNC_D
#define TERMINAL_TX_CONFIG      PIN_CONFIG_PMUXO(TERMINAL_TX_PIN,   \
                                                 TERMINAL_TX_GROUP, \
                                                 TERMINAL_TX_PMUX)

#define TERMINAL_RX_PIN         30
#define TERMINAL_RX_GROUP       1
#define TERMINAL_RX_PMUX        PMUX_FUNC_D
#define TERMINAL_RX_CONFIG      PIN_CONFIG_PMUXE(TERMINAL_RX_PIN,   \
                                                 TERMINAL_RX_GROUP, \
                                                 TERMINAL_RX_PMUX)


#define TERMINAL_SERCOM         SERCOM5
#define TERMINAL_SERCOM_PCLK    SERCOM5_GCLK_ID_CORE
#define TERMINAL_TXPO           0x00
#define TERMINAL_RXPO           0x01
#define TERMINAL_START_BUS      MCLK->APBDMASK.bit.SERCOM5_ = 1;
#define TERMINAL_GLK_SOURCE     GCLK_PCHCTRL_GEN_GCLK2
//This value absolutely needs to be a power of 2
#define TERMINAL_BUFFER_SIZE    512

#define TERMINAL_ISR_RX_NUM     SERCOM5_2_IRQn
#define TERMINAL_ISR_RX_FUNC    SERCOM5_2_Handler
#define TERMINAL_ISR_DRE_NUM    SERCOM5_0_IRQn
#define TERMINAL_ISR_DRE_FUNC   SERCOM5_0_Handler
#define TERMINAL_BAUD_115200    5138
#define TERMINAL_BAUD_9600      60503
#define TERMINAL_BAUD           TERMINAL_BAUD_115200

/*****************************************************************************
 *
 *                          Dashboard Config
 *
 *****************************************************************************/
#define DASHBOARD_TC            TC0
#define DASHBOARD_TC_START_BUS  MCLK->APBAMASK.bit.TC0_ = 1;
#define DASHBOARD_TC_GLK_SOURCE GCLK_PCHCTRL_GEN_GCLK1
#define DASHBOARD_TC_PCLK       TC0_GCLK_ID
#define DASHBOARD_TC_PRESCALE   TC_CTRLA_PRESCALER_DIV256

#define DASHBOARD_TC_IRQN       TC0_IRQn
#define DASHBOARD_TC_ISR        TC0_Handler


/*****************************************************************************
 *
 *                          Sensors.h Config/Pins
 *
 *****************************************************************************/

#define SENSORS_TC            TC1
#define SENSORS_TC_START_BUS  MCLK->APBAMASK.bit.TC1_ = 1;
#define SENSORS_TC_GLK_SOURCE GCLK_PCHCTRL_GEN_GCLK0
#define SENSORS_TC_PCLK       TC1_GCLK_ID
#define SENSORS_TC_PRESCALE   TC_CTRLA_PRESCALER_DIV1
#define SENSORS_TC_ISR        TC1_Handler
#define SENSORS_TC_IRQN       TC1_IRQn

//Idle switch is PA21
#define IDLE_SW_PIN         21
#define IDLE_SW_GROUP       0
#define IDLE_SW_CONFIG      PIN_CONFIG_INPUT(IDLE_SW_PIN, \
                                             IDLE_SW_GROUP)
//O2 sensor is PB17
#define OX_PIN              17
#define OX_GROUP            1
#define OX_CONFIG           PIN_CONFIG_INPUT(OX_PIN,\
                                             OX_GROUP)

//Start switch is PA20
#define START_SW_PIN        20
#define START_SW_GROUP      0
#define START_SW_CONFIG     PIN_CONFIG_INPUT(START_SW_PIN,      \
                                             START_SW_GROUP)

//Engine On Pin is PB09
#define ENGINE_ON_PIN       9
#define ENGINE_ON_GROUP     1
#define ENGINE_ON_CONFIG    PIN_CONFIG_INPUT(ENGINE_ON_PIN,     \
                                             ENGINE_ON_GROUP)
      

//Stay powered pin is PA18
#define STAY_ALIVE_PIN      18
#define STAY_ALIVE_GROUP    0
#define STAY_ALIVE_CONFIG   PIN_CONFIG_OUTPUT_LOW(STAY_ALIVE_PIN,STAY_ALIVE_GROUP)

/*****************************************************************************
 *
 *                  Pin manipulation, do not edit below here
 *
 *****************************************************************************/
#define PIN_CONFIG_PMUXE(pin, port, func) DISABLE_PMUX(pin,port); SET_INPUT(pin,port);\
                                          CLEAR_MUXE(pin,port);   SET_MUXE(pin,port,func);\
                                          ENABLE_PMUX(pin,port);
#define PIN_CONFIG_PMUXO(pin, port, func) DISABLE_PMUX(pin,port); SET_INPUT(pin,port);\
                                          CLEAR_MUXO(pin,port);   SET_MUXO(pin,port,func);\
                                          ENABLE_PMUX(pin,port);

#define PIN_CONFIG_INPUT(pin, port)       DISABLE_PMUX(pin,port);SET_INPUT(pin,port);\
                                          ENABLE_INPUT(pin, port);
#define PIN_CONFIG_OUTPUT(pin, port)      DISABLE_PMUX(pin,port);SET_OUTPUT(pin,port);
#define PIN_CONFIG_OUTPUT_LOW(pin, port)  DISABLE_PMUX(pin,port);SET_LOW(pin,port);\
                                          SET_OUTPUT(pin,port);
#define PIN_CONFIG_OUTPUT_HIGH(pin, port) DISABLE_PMUX(pin,port);SET_HIGH(pin,port);\
                                          SET_OUTPUT(pin,port);

#define SET_HIGH(pin, port)         PORT->Group[port].OUTSET.reg =\
                                        (1 << pin)
#define SET_LOW(pin, port)          PORT->Group[port].OUTCLR.reg =\
                                        (1 << pin)
#define SET_INPUT(pin, port)        PORT->Group[port].DIRCLR.reg =\
                                        (1 << pin)
#define SET_OUTPUT(pin, port)       PORT->Group[port].DIRSET.reg =\
                                        (1 << pin)
#define ENABLE_PMUX(pin, port)      PORT->Group[port].PINCFG[pin].reg |=\
                                        PORT_PINCFG_PMUXEN
#define DISABLE_PMUX(pin, port)     PORT->Group[port].PINCFG[pin].reg &=\
                                      ~(PORT_PINCFG_PMUXEN)
#define ENABLE_INPUT(pin, port)     PORT->Group[port].PINCFG[pin].reg |=\
                                        PORT_PINCFG_INEN
#define DISABLE_INPUT(pin, port)    PORT->Group[port].PINCFG[pin].reg &=\
                                      ~(PORT_PINCFG_INEN)
#define CLEAR_MUXO(pin, port)       PORT->Group[port].PMUX[(pin >> 1)].reg &=\
                                      ~(PORT_PMUX_PMUXO_Msk)
#define SET_MUXO(pin, port, val)    PORT->Group[port].PMUX[(pin >> 1)].reg |=\
                                       (PORT_PMUX_PMUXO(val))
#define CLEAR_MUXE(pin, port)       PORT->Group[port].PMUX[(pin >> 1)].reg &=\
                                      ~(PORT_PMUX_PMUXE_Msk)
#define SET_MUXE(pin, port, val)    PORT->Group[port].PMUX[(pin >> 1)].reg |=\
                                       (PORT_PMUX_PMUXE(val))

#define PMUX_FUNC_A 0x00
#define PMUX_FUNC_B 0x01
#define PMUX_FUNC_C 0x02
#define PMUX_FUNC_D 0x03
#define PMUX_FUNC_E 0x04
#define PMUX_FUNC_F 0x05
#define PMUX_FUNC_G 0x06
#define PMUX_FUNC_H 0x07
#define PMUX_FUNC_I 0x08
#define PMUX_FUNC_J 0x09
#define PMUX_FUNC_K 0x0A
#define PMUX_FUNC_L 0x0B
#define PMUX_FUNC_M 0x0C
#define PMUX_FUNC_N 0x0D

#endif /* PINS_H_ */