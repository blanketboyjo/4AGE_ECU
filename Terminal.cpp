/*
* Terminal.cpp
*  See Terminal.h for function information
*
* Created: 5/13/2019 8:51:27 AM
*  Author: Jordan
*/ 
#include "Terminal.h"

unsigned int m_TXBuffer[TERMINAL_BUFFER_SIZE];
unsigned int m_RXBuffer[TERMINAL_BUFFER_SIZE];
unsigned int m_TXIndexWrite,m_TXIndexTarget, m_RXIndexLoad, m_RXIndexRead;
unsigned int m_isConfigured = 0;

//Sets up pins and sercom
void terminalConfig(void){
  //Only configure once
  if(0 == m_isConfigured){
    m_isConfigured  = 1;
    //Zero out the index values;
    m_TXIndexWrite  = 0;
    m_TXIndexTarget = 0;
    m_RXIndexLoad   = 0;
    m_RXIndexRead   = 0;

    //Startup SERCOM on main bus
    TERMINAL_START_BUS;
    //Give it a main clock
    GCLK->PCHCTRL[TERMINAL_SERCOM_PCLK].reg =   
        TERMINAL_GLK_SOURCE  |   //  GCLK speced in source
        GCLK_PCHCTRL_CHEN;

    //Software reset it
    TERMINAL_SERCOM->USART.CTRLA.bit.SWRST = 
        1;

    //Wait for reset
    while(1 == TERMINAL_SERCOM->USART.CTRLA.bit.SWRST);

    //Configure it
    TERMINAL_SERCOM->USART.CTRLA.reg =  
        SERCOM_USART_CTRLA_MODE(1)              |   //Internal clock USART
        SERCOM_USART_CTRLA_RXPO(TERMINAL_RXPO)  |   //Configuration from pins.h
        SERCOM_USART_CTRLA_TXPO(TERMINAL_TXPO)  |   //Configuration from pins.h
        SERCOM_USART_CTRLA_DORD;                    //MSB first

    //Enable RX/TX
    TERMINAL_SERCOM->USART.CTRLB.reg =  
        SERCOM_USART_CTRLB_RXEN         |   //Enable RX
        SERCOM_USART_CTRLB_TXEN         |   //Enable TX
        SERCOM_USART_CTRLB_CHSIZE(0x00) ;   //8 bit character size

    //Divider
    TERMINAL_SERCOM->USART.BAUD.reg =   
        TERMINAL_BAUD;   //Configuration from pins.h

    //Start it
    TERMINAL_SERCOM->USART.CTRLA.bit.ENABLE = 
        1;

    //Wait for it to boot up
    while(TERMINAL_SERCOM->USART.SYNCBUSY.reg);

    //Configure the pins
    TERMINAL_TX_CONFIG;
    TERMINAL_RX_CONFIG;

    //Setup interrupt priorities
    NVIC_SetPriority(TERMINAL_ISR_DRE_NUM, TERMINAL_ISR_PRIORITY);
    NVIC_SetPriority(TERMINAL_ISR_RX_NUM,  TERMINAL_ISR_PRIORITY);

    //Enable interrupts
    NVIC_EnableIRQ(TERMINAL_ISR_DRE_NUM);
    NVIC_EnableIRQ(TERMINAL_ISR_RX_NUM);

    //Enable RX interrupt
    TERMINAL_SERCOM->USART.INTENSET.reg = 
        SERCOM_USART_INTENSET_RXC;
  }        
}

//Sends a single byte
void terminalSend(unsigned int data){
  //Load data into buffer at load index
  m_TXBuffer[m_TXIndexTarget] = data;

  //Increment load index
  m_TXIndexTarget = (m_TXIndexTarget + 1)& (TERMINAL_BUFFER_SIZE - 1);

  //Handle load index rollover
  if(m_TXIndexTarget > TERMINAL_BUFFER_SIZE)m_TXIndexTarget = 0;

  //Check if data is being sent (if not load data to data buffer)
  if(0 == TERMINAL_SERCOM->USART.INTENSET.bit.DRE){
    //Make temporary index
    unsigned int tempIndex = m_TXIndexWrite;

    //Increment index, handle rollover
    m_TXIndexWrite = (m_TXIndexWrite + 1)& (TERMINAL_BUFFER_SIZE - 1);

    //Load into data buffer
    TERMINAL_SERCOM->USART.DATA.reg = m_TXBuffer[tempIndex];

    //Enable DRE interrupt
    TERMINAL_SERCOM->USART.INTENSET.reg = SERCOM_USART_INTENSET_DRE;
  }
}

//Sends a string of data to terminal
void terminalSendStream(unsigned int *data, unsigned int length){
  //Loop through until all data loaded
  for(unsigned int i = 0; i<length; i++){
    terminalSend(data[i]);
  }
}

unsigned int terminalRead(void){
  //Store data to send in a temporary variable
  unsigned int temp = m_RXBuffer[m_RXIndexRead];

  //Increment index, handle rollover
  m_RXIndexRead = (m_RXIndexRead + 1)& (TERMINAL_BUFFER_SIZE - 1);
  //Return desired data
  return temp;
}

//Returns how many bytes can be read from the Rx buffer
unsigned int terminalAvailable(void){
  //Check if load index has rolled over
  if(m_RXIndexLoad < m_RXIndexRead){
    //Return loadIndex + buffersize - readIndex
    return m_RXIndexLoad + TERMINAL_BUFFER_SIZE - m_RXIndexRead;
  }else{//No rollover condition
    return m_RXIndexLoad - m_RXIndexRead;
  }
}

//Sets sets data indexes to same value, effectively skipping buffer
void terminalFlushInput(void){
  m_RXIndexRead = m_RXIndexLoad;
}


/*********************************************************************************
*
*                       Interrupt service routines
*
**********************************************************************************/

//This for getting data back from terminal
void TERMINAL_ISR_RX_FUNC(void){
  //Read data into Rx buffer
  m_RXBuffer[m_RXIndexLoad] = TERMINAL_SERCOM->USART.DATA.reg;
  //Increment index, handle rollover
  m_RXIndexLoad ++;
  if(m_RXIndexLoad >= TERMINAL_BUFFER_SIZE)m_RXIndexLoad = 0;
}

//This is for loading new data
void TERMINAL_ISR_DRE_FUNC(void){
  //Check if there is actually data to send
  unsigned int dataLeftToSend;
  dataLeftToSend  = (m_TXIndexTarget - m_TXIndexWrite)& (TERMINAL_BUFFER_SIZE -1);

  if(0 == dataLeftToSend){
    //Disable this interrupt
    TERMINAL_SERCOM->USART.INTENCLR.reg = 
        SERCOM_USART_INTENSET_DRE;

  }else{
    //Load data
    TERMINAL_SERCOM->USART.DATA.reg = 
        m_TXBuffer[m_TXIndexWrite];
    //Increment index, handle rollover
    m_TXIndexWrite = (m_TXIndexWrite + 1)& (TERMINAL_BUFFER_SIZE - 1);    

  }
}