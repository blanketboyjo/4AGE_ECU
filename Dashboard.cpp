/*
* Dashboard.cpp
*
* Created: 5/23/2019 9:28:52 AM
*  Author: Jordan
*/ 
#include "Dashboard.h"


#define ASCII_ESC                           0x1B

/**************************************************************************
*
*                 Dashboard Screen Locations
*
***************************************************************************/
                                          //x   ,   y
#define DASHBOARD_TITLE                     20  ,   0


#define DASHBOARD_IDLE_LABEL                0   ,   2
#define DASHBOARD_IDLE_DATA                 15  ,   2

#define DASHBOARD_RPM_LABEL                 0   ,   3
#define DASHBOARD_RPM_DATA                  15  ,   3

#define DASHBOARD_ADVANCE_LABEL             0   ,   4
#define DASHBOARD_ADVANCE_DATA              15  ,   4

#define DASHBOARD_PRESSURE_LABEL            0   ,   5
#define DASHBOARD_PRESSURE_DATA             15  ,   5

#define DASHBOARD_AIR_TEMP_LABEL            0   ,   6
#define DASHBOARD_AIR_TEMP_DATA             15  ,   6

#define DASHBOARD_WATER_TEMP_LABEL          0   ,   7
#define DASHBOARD_WATER_TEMP_DATA           15  ,   7

#define DASHBOARD_THROTTLE_LABEL            0   ,   8
#define DASHBOARD_THROTTLE_DATA             15  ,   8
#define DASHBOARD_IDLE_SW_DATA              25  ,   8

#define DASHBOARD_AFM_LABEL                 0   ,   9
#define DASHBOARD_AFM_DATA                  15  ,   9




#define DASHBOARD_ARRAY_LIMIT               32
unsigned int m_buffer[DASHBOARD_ARRAY_LIMIT];       


/**************************************************************************
*
*                       Enums for use
*
***************************************************************************/

enum VT100_COLOR{
  BLACK       = 0,
  RED         = 1,
  GREEN       = 2,
  YELLOW      = 3,
  BLUE        = 4,
  MAGENTA     = 5,
  CYAN        = 6,
  WHITE       = 7
};

enum VT100_ATTRIBUTE{
  NONE        = 0,
  BOLD        = 1,
  LOW_INT     = 2,
  UNDERLINE   = 4,
  BLINK       = 5,
  REVERSE     = 6,
  INVISBILE   = 7
};


enum VT100_CLEAR{
  CURSOR_DOWN = 0,
  CURSOR_UP   = 1,
  SCREEN      = 2
};

enum VT100_TEXT_SIZE{
  TALL_UPPER  = 0,
  TALL_LOWER  = 1,
  NORMAL      = 2,
  WIDE        = 3
};



/**************************************************************************
*
*                 Private Functions
*
***************************************************************************/

//Clears terminal based on enums above
void m_clearTerminal(VT100_CLEAR clearType){
  //Prepare message
  m_buffer[0] = ASCII_ESC;
  m_buffer[1] = '[';
  m_buffer[2] = clearType + '0';
  m_buffer[3] = 'J';

  //Send message
  terminalSendStream(m_buffer,4);
}

//Moves to a desired location
void m_moveCursorTo(unsigned int x, unsigned int y){
  unsigned int length     = 2;
  unsigned int placesX    = 0;
  unsigned int placesY    = 0;
  unsigned int tempX      = x;
  unsigned int tempY      = y;
  m_buffer[0]             = ASCII_ESC;
  m_buffer[1]             = '[';


  //Get number of places for X and Y
  while(x != 0){
    x = x/10;
    placesX++;
  }
  while(y != 0){
    y = y/10;
    placesY++;
  }
  if(0 == placesX)placesX = 1;
  if(0 == placesY)placesY = 1;

  //Increment final index by #of decimals in X
  length += placesY;
  //Parse X into array
  for(unsigned int i = 1; i <= placesY; i++){
    m_buffer[length - i] = tempY % 10 + '0';
    tempY = tempY/10;
  }

  m_buffer[length] = ';';
  //Increment index to account for ;
  length++;

  //Increment final index by #of decimals in X
  length += placesX;
  //Parse X into array
  for(unsigned int i = 1; i <= placesX; i++){
    m_buffer[length - i] = tempX % 10 + '0';
    tempX = tempX/10;
  }

  m_buffer[length] = 'f';
  //Increment index to account for ;
  length++;
  //Send message
  terminalSendStream(m_buffer, length);    
}


void m_setFontSize(VT100_TEXT_SIZE size){
  //Prepare message, enums handle actual text size
  m_buffer[0] = ASCII_ESC;
  m_buffer[1] = '#';
  m_buffer[2] = size + '3';

  //Send message
  terminalSendStream(m_buffer,3);   
}

//Sets font color, based on enums above
void m_setFontColor(VT100_COLOR color){
  //Prepare message, enums handle colors
  m_buffer[0] = ASCII_ESC;
  m_buffer[1] = '[';
  m_buffer[2] = '3';
  m_buffer[3] = color + '0';
  m_buffer[4] = 'm';

  //Send message
  terminalSendStream(m_buffer,5);
}

//Sets background color, based on enums above
void m_setBackgroundColor(VT100_COLOR color){
  //Prepare message, enums handle colors
  m_buffer[0] = ASCII_ESC;
  m_buffer[1] = '[';
  m_buffer[2] = '4';
  m_buffer[3] = color + '0';
  m_buffer[4] = 'm';

  //Send message
  terminalSendStream(m_buffer,5);
}


//Sets an attribute, based on enums above
void m_setAttribute(VT100_ATTRIBUTE atr){
  //Prepare message, enums handle settings
  m_buffer[0] = ASCII_ESC;
  m_buffer[1] = '[';
  m_buffer[2] = atr + '0';
  m_buffer[3] = 'm';

  //Send message
  terminalSendStream(m_buffer,4);
}

//This function converts a unsigned fixed point to signed
void m_printUintValue(unsigned int value, int length, int decimalPlaces = -1){
  //Return if length is > DASHBOARD_ARRAY_LIMIT, or is 0
  if(((DASHBOARD_ARRAY_LIMIT - 1) < length) || (0 >= length))return;

  //Increment length by 1 to account for decimal place
  if(-1 != decimalPlaces)length ++;    

  //Loop i until it matches the desired length
  for(int i = 0; i<= length; i++){
    //Blank if all zeros and passed decimal point
    if(i > decimalPlaces){
      if(value > 0){
        m_buffer[length - i]    =  (value % 10) + '0';
        value                   =   value / 10;
      }else{
        m_buffer[length - i]    =   ' ';
      }
    }else if(i < decimalPlaces){
      m_buffer[length - i]        =  (value % 10) + '0';
      value                       =   value / 10;

    }else{//Place '.'
      m_buffer[length - i]        =   '.';
    }
  }
  //Print it out
  terminalSendStream(m_buffer,length + 1);
}

//This function converts a signed fixed point to signed
void m_printIntValue(int value, int length, int decimalPlaces = -1){
  //Return if length is > DASHBOARD_ARRAY_LIMIT, or is 0
  if(((DASHBOARD_ARRAY_LIMIT - 1) < length) || (0 >= length))return;
  m_buffer[0] = ' ';
  //Invert sign if value is negative, place sign
  if(0 > value){
  m_buffer[0]    =   '-';
  value               = - value;
  }

  //Increment length by 1 to account for decimal place
  if(-1 != decimalPlaces)length ++;    

  //Loop i until it matches the desired length
  for(int i = 0; i< length; i++){
    //Blank if all zeros and passed decimal point
    if(i > decimalPlaces){
      if(value != 0){
        m_buffer[length - i]    =  (value % 10) + '0';
        value                   =   value / 10;
      }else{
        m_buffer[length - i]    =   ' ';
      }
    }else if(i < decimalPlaces){
      m_buffer[length - i]        =  (value % 10) + '0';
      value                       =   value / 10;
    }else{//Place '.'
      m_buffer[length - i]        =   '.';
    }
  }
  //Print it out
  terminalSendStream(m_buffer,length + 1);
}


//This function places all of the necessary labels in place
void m_printLabels(void){
  m_clearTerminal(SCREEN);
  m_moveCursorTo(DASHBOARD_TITLE);

  //Print Title
  m_buffer[0]     = 'E';
  m_buffer[1]     = 'C';
  m_buffer[2]     = 'U';
  m_buffer[3]     = ' ';
  m_buffer[4]     = 'M';
  m_buffer[5]     = 'e';
  m_buffer[6]     = 't';
  m_buffer[7]     = 'r';
  m_buffer[8]     = 'i';
  m_buffer[9]     = 'c';
  m_buffer[10]    = 's';
  terminalSendStream(m_buffer,11); 

  m_moveCursorTo(DASHBOARD_IDLE_LABEL);
  m_buffer[0]     = 'I';
  m_buffer[1]     = 'd';
  m_buffer[2]     = 'l';
  m_buffer[3]     = 'e';
  m_buffer[4]     = ' ';
  m_buffer[5]     = 'T';
  m_buffer[6]     = 'a';
  m_buffer[7]     = 'r';
  m_buffer[8]     = 'g';
  m_buffer[9]     = 'e';
  m_buffer[10]    = 't';
  m_buffer[11]    = ':';
  terminalSendStream(m_buffer,12); 

  m_moveCursorTo(DASHBOARD_IDLE_DATA);
  m_printUintValue(EFI_FUEL_CUT_RPM,4);

  m_moveCursorTo(DASHBOARD_RPM_LABEL);
  m_buffer[0]     = 'C';
  m_buffer[1]     = 'u';
  m_buffer[2]     = 'r';
  m_buffer[3]     = 'r';
  m_buffer[4]     = 'e';
  m_buffer[5]     = 'n';
  m_buffer[6]     = 't';
  m_buffer[7]     = ' ';
  m_buffer[8]     = 'R';
  m_buffer[9]     = 'P';
  m_buffer[10]    = 'M';
  m_buffer[11]    = ':';
  terminalSendStream(m_buffer,12);

  m_moveCursorTo(DASHBOARD_ADVANCE_LABEL);
  m_buffer[0]     = 'A';
  m_buffer[1]     = 'd';
  m_buffer[2]     = 'v';
  m_buffer[3]     = 'a';
  m_buffer[4]     = 'n';
  m_buffer[5]     = 'c';
  m_buffer[6]     = 'e';
  m_buffer[7]     = ':';
  terminalSendStream(m_buffer,8);

  m_moveCursorTo(DASHBOARD_PRESSURE_LABEL);
  m_buffer[0]     = 'M';
  m_buffer[1]     = 'a';
  m_buffer[2]     = 'n';
  m_buffer[3]     = 'i';
  m_buffer[4]     = 'f';
  m_buffer[5]     = 'o';
  m_buffer[6]     = 'l';
  m_buffer[7]     = 'd';
  m_buffer[8]     = ' ';
  m_buffer[9]     = 'K';
  m_buffer[10]    = 'P';
  m_buffer[11]    = 'a';
  m_buffer[12]    = ':';
  terminalSendStream(m_buffer,13);

  m_moveCursorTo(DASHBOARD_AIR_TEMP_LABEL);
  m_buffer[0]     = 'A';
  m_buffer[1]     = 'i';
  m_buffer[2]     = 'r';
  m_buffer[3]     = ' ';
  m_buffer[4]     = 'T';
  m_buffer[5]     = 'e';
  m_buffer[6]     = 'm';
  m_buffer[7]     = 'p';
  m_buffer[8]     = ':';
  terminalSendStream(m_buffer,19);

  m_moveCursorTo(DASHBOARD_WATER_TEMP_LABEL);
  m_buffer[0]     = 'W';
  m_buffer[1]     = 'a';
  m_buffer[2]     = 't';
  m_buffer[3]     = 'e';
  m_buffer[4]     = 'r';
  m_buffer[5]     = ' ';
  m_buffer[6]     = 'T';
  m_buffer[7]     = 'e';
  m_buffer[8]     = 'm';
  m_buffer[9]     = 'p';
  m_buffer[10]    = ':';
  terminalSendStream(m_buffer,11);

  m_moveCursorTo(DASHBOARD_THROTTLE_LABEL);
  m_buffer[0]     = 'T';
  m_buffer[1]     = 'h';
  m_buffer[2]     = 'r';
  m_buffer[3]     = 'o';
  m_buffer[4]     = 't';
  m_buffer[5]     = 't';
  m_buffer[6]     = 'l';
  m_buffer[7]     = 'e';
  m_buffer[8]    = ':';
  terminalSendStream(m_buffer,9);

  m_moveCursorTo(DASHBOARD_AFM_LABEL);
  m_buffer[0]     = 'A';
  m_buffer[1]     = 'i';
  m_buffer[2]     = 'r';
  m_buffer[3]     = ' ';
  m_buffer[4]     = 'f';
  m_buffer[5]     = 'l';
  m_buffer[6]     = 'o';
  m_buffer[7]     = 'w';
  m_buffer[8]     = ' ';
  m_buffer[9]     = 'M';
  m_buffer[10]    = 'e';
  m_buffer[11]    = 't';
  m_buffer[12]    = 'e';
  m_buffer[13]    = 'r';
  m_buffer[14]    = ':';
  terminalSendStream(m_buffer,15);
}

//Updates all the values
void m_dashboardUpdate(void){
  //Print AFM
  m_moveCursorTo(DASHBOARD_AFM_DATA);
  m_printUintValue(Sensors.AirFlow,4);

  //Print THROTTLE
  m_moveCursorTo(DASHBOARD_THROTTLE_DATA);
  m_printUintValue(Sensors.Throttle,4);

  //Print Air temp
  m_moveCursorTo(DASHBOARD_AIR_TEMP_DATA);
  m_printUintValue(Sensors.AirTemp,4);

  //Print water temp
  m_moveCursorTo(DASHBOARD_WATER_TEMP_DATA);
  m_printUintValue(Sensors.WaterTemp,4);

  //Print RPM
  m_moveCursorTo(DASHBOARD_RPM_DATA);
  m_printUintValue(Sensors.RPM,4);

  //Print Pressure
  m_moveCursorTo(DASHBOARD_PRESSURE_DATA);
  m_printUintValue(Sensors.ManifoldPressure,5,2);

  m_moveCursorTo(DASHBOARD_IDLE_SW_DATA);

  //Print state of throttle switch
  if(1 == Sensors.ThrottleClosed){
    m_buffer[0] = 'C';
    m_buffer[1] = 'l';
    m_buffer[2] = 'o';
    m_buffer[3] = 's';
    m_buffer[4] = 'e';
    m_buffer[5] = 'd';
    terminalSendStream(m_buffer,6);
  }else{
    m_buffer[0] = 'O';
    m_buffer[1] = 'p';
    m_buffer[2] = 'e';
    m_buffer[3] = 'n';
    m_buffer[4] = ' ';
    m_buffer[5] = ' ';
    terminalSendStream(m_buffer,6);
  }

    m_moveCursorTo(DASHBOARD_ADVANCE_DATA);
    m_printIntValue(getIgnitionAdvance(),4,2);

  }

 
/**************************************************************************
*
*                 Public Functions
*
***************************************************************************/

//Sets up terminal
void dashboardConfig(void){

  //Ensure the terminal is configured
  terminalConfig();
  m_moveCursorTo(0,0);
  m_setFontColor(WHITE);
  m_printLabels();

  //Setup update timer
  //Enable it on the main bus
  DASHBOARD_TC_START_BUS;

  //Give it a glk
  GCLK->PCHCTRL[DASHBOARD_TC_PCLK].reg =  
      DASHBOARD_TC_GLK_SOURCE |
      GCLK_PCHCTRL_CHEN;
                                    
  //Software reset it
  DASHBOARD_TC->COUNT16.CTRLA.bit.SWRST = 
      1;

  //Wait for reset to finish
  while(0 != (DASHBOARD_TC->COUNT16.CTRLA.reg & TC_CTRLA_SWRST));

  //Setup is based on configuration in pins.h
  DASHBOARD_TC->COUNT16.CTRLA.reg =   
      DASHBOARD_TC_PRESCALE |
      TCC_CTRLA_RUNSTDBY;

  //Enable overflow interrupt after clearing interrupt flag
  DASHBOARD_TC->COUNT16.INTFLAG.reg   = 
      TC_INTFLAG_OVF;
  DASHBOARD_TC->COUNT16.INTENSET.reg  = 
      TC_INTENSET_OVF;

  //Enable interrupt at NVIC
  __NVIC_SetPriority(DASHBOARD_TC_IRQN, DASHBOARD_ISR_PRIORITY);
  __NVIC_EnableIRQ(DASHBOARD_TC_IRQN);


}

//Start update timer
void dashboardStart(void){
  DASHBOARD_TC->COUNT16.CTRLA.bit.ENABLE = 1;
}

//Stop update timer
void dashboardHalt(void){
  DASHBOARD_TC->COUNT16.CTRLA.bit.ENABLE = 0;
}

/**************************************************************************
*
*      Interrupt service routines
*
**************************************************************************/
//Update dashboard, if there is data act on it
void DASHBOARD_TC_ISR(void){
  //Clear overflow flag
  DASHBOARD_TC->COUNT16.INTFLAG.reg = TC_INTFLAG_OVF;

  //Check if terminal has data
  if(0 < terminalAvailable()){
    //Read first in buffer, flush the rest
    unsigned int input = terminalRead();
    terminalFlushInput();
    switch(input){
    case 'R':
    case 'r':
      //If R is input refresh screen
      m_printLabels();
    break;
    }
  }

  //Update screen
  m_dashboardUpdate();
}
