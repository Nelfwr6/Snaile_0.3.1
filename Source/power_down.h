/*******************************************************************************

* Module name: RTC.h

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

* First written on 08/22/2014 by jomon

*

* Module Description:

* Header file for ADC Application program

*

*******************************************************************************/
/**************************************************************************************/
#ifndef _POWER_DOWN_H  
#define _POWER_DOWN_H  

/******************************* SETTINGS *********************************************/
/*************************** HEADER FILES *********************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_intc.h"
#include "r_cg_serial.h"
#include "r_cg_adc.h"
#include "r_cg_rtc.h"

#define TRUE    1
#define FALSE   0

#define MORNING 0
#define EVENING 1

#define STOP_MODE	1

uint8_t Select_Power_Mode (uint8_t level);
void Reset_MCU(void);
#endif //

