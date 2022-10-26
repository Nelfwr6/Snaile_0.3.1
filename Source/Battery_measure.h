/*******************************************************************************

* Module name: adc.h

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

* First written on 08/22/2014 by Reshma Joseph.

*

* Module Description:

* Header file for ADC Application program

*

*******************************************************************************/
/**************************************************************************************/
#ifndef _BATTERY_MEASURE_H  
#define _BATTERY_MEASURE_H  

/******************************* SETTINGS *********************************************/
/*************************** HEADER FILES *********************************************/
#include "r_cg_macrodriver.h"
/**************************************************************************************/
/********************************  MACROS ************************************/
#define BATTERY_SAMPLE_COUNT 1024
#define BATTEY_MINIMUM_VOLTAGE 3600
#define BATTEY_MAXIMUM_VOLTAGE 4200
/********************************** USER SETTINGS *************************************/
extern uint8_t u8_get_battery_status;
extern uint16_t u16_battery_charge, u16_battery_charge_count;
/************ FUNCTIONS *********************************************************/
void v_battery_measure(void);
/*******************************************************************************/
#endif