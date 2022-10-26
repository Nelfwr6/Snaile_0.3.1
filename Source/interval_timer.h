/*******************************************************************************

* Module name: interval_timer.h

*

* Copyright 2013 Neona Embedded Labz as an unpublished work.

* All Rights Reserved.

*

* The information contained herein is confidential

* property of Neona Embedded Labz. The use, copying, transfer

* or disclosure of such information is prohibited except by

* express written agreement with Company.

*

* History:-

*

* First written on 11/24/14 By Reshma Joseph.

*

* Module Description:

* Header file for interval timer module initialisation for RL78 G12 

*
*******************************************************************************/
#ifndef INTERVAL_TIMER_H
#define INTERVAL_TIMER_H

#define TIME_HOURS 60000
#define DETECTION_DURATION	60000


void v_delay_ms(uint16_t u16_ms_delay);
void v_Scheduler(void);
void v_Micro_Delay(unsigned int D_Count);
void v_Button_Timer(void);

#endif