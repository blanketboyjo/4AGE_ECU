/*
 * EFI.h
 *  This contains the code for timing fuel injectors and ignition
 *  
 *  Dependencies: (The exact modules used are in Pins.h)
 *      TCC                    - Used for all the timing
 *      Event system Ch        - Used for NE timing capture
 *      External Interrupt     - Used for G and NE inputs
 *      ExternalInterrupt.h    - Must call config before EFIConfig();
 *      Sensors.h              - Contains critical data from sensors
 *      Map.h                  - Contains the ignition and injection maps
 *
 * Created: 5/22/2019 9:11:06 AM
 *  Author: Jordan
 */ 


#ifndef EFI_H_
#define EFI_H_
#include "sam.h"
#include "Pins.h"
#include "Sensors.h"
#include "ExternalInterrupt.h"
#include "Map.h"

void EFIConfig(void);
void EFIStart(void);
void EFIHalt(void);

#endif /* EFI_H_ */