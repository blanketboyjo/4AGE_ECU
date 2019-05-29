/*
 * Dashboard.h
 *  This handles the updates of the terminal readback
 *  It utilizes VT100 to make the display as clean as possible
 *
 *  Dependencies:
 *      Terminal.h  - Hardware layer
 *      Sensors.h   - Used for data
 *      TC          - Used for periodic update 
 *
 * Created: 5/23/2019 9:27:45 AM
 *  Author: Jordan
 */ 


#ifndef DASHBOARD_H_
#define DASHBOARD_H_

#include "sam.h"
#include "Sensors.h"
#include "Map.h"
#include "Terminal.h"

void dashboardConfig(void);
void dashboardStart(void);
void dashboardHalt(void);

#endif /* DASHBOARD_H_ */