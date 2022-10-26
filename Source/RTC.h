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

* First written on 08/22/2014 by Reshma Joseph.

*

* Module Description:

* Header file for ADC Application program

*

*******************************************************************************/
/**************************************************************************************/
#ifndef _RTC_H  
#define _RTC_H  

/******************************* SETTINGS *********************************************/
/*************************** HEADER FILES *********************************************/
#include "r_cg_macrodriver.h"

/**************************************************************************************/
/********************************  MACROS ************************************/
/********************************************* EXTERNAL VRIABLES***************************************************/

extern uint8_t u8_system_current_date, u8_system_current_month, u8_system_current_year;
extern uint8_t u8_system_current_hour,  u8_system_current_min, u8_system_current_sec;

extern uint8_t u8_restart_time_hr,  u8_restart_time_min, u8_restart_time_sec;
extern uint8_t u8_restart_date, u8_restart_month, u16_restart_year;
/******************************************* FUNCTIONS *********************************************************/
uint8_t DecimalToBCD(uint8_t decimalByte);
uint8_t BCDToDecimal(uint8_t bcdByte);
void v_update_date_time(void);
void v_RTC_Get_Time(void);


/****************************************************************************************************************/
#endif