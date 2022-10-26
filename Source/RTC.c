
/*************************** HEADER FILES *********************************************

* Module name: adc.c

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

* adc.c program

*
***************************************************************************************/
/*************************** HEADER FILES *********************************************/


#include "r_cg_macrodriver.h"
#include "r_cg_adc.h"
#include "adc.h"
#include "r_cg_rtc.h"
#include "RTC.h"
#include "interval_timer.h"
#include "USB_Config.h"
#include "power_down.h"
#include "EEPROM.h"
#include "usb_config.h"
//#include "gsm_gprs.h"        //basil
/*******************************************************************************

* Global Variable Section

*******************************************************************************/

uint8_t u8_system_current_hour,  u8_system_current_min, u8_system_current_sec;
uint8_t u8_system_current_date, u8_system_current_month, u8_system_current_year;

rtc_counter_value_t rtc_System_Time;


/*****************************************************************************************
* Function name	 	: 	void	v_Init_ADC	(void)
* returns 		: 	void
* arg1 			: 	void
* Created by 		: 	Reshma Joseph
* Date created 		: 	08-22-2014
* Edited By		:	Reshma Joseph
* Description 		: 	ADC initialisation
* Notes 		: 	Edited for ADC initialisation
******************************************************************************************/
void v_update_date_time(void)
{
	uint8_t u8_date, u8_month, u8_year, u8_time_sec, u8_time_minute, u8_time_hour;
	uint8_t u8_temp;
	u8_date		= D_Config_Packet.Member.u8_current_date;
	u8_month	= D_Config_Packet.Member.u8_current_month;
	u8_year		= D_Config_Packet.Member.u16_current_year;
	u8_time_sec	= D_Config_Packet.Member.u8_current_time_sec;
	u8_time_minute	= D_Config_Packet.Member.u8_current_time_min;
	u8_time_hour	= D_Config_Packet.Member.u8_current_time_hr;
	u8_temp = (((u8_date/10) << 4) | (u8_date %10));
	rtc_System_Time.day = u8_temp;
	u8_temp = (((u8_month/10) << 4) | (u8_month %10));
	rtc_System_Time.month = u8_temp;
	u8_temp = (((u8_year/10) << 4) | (u8_year %10));
	rtc_System_Time.year = u8_temp;
	u8_temp = (((u8_time_sec/10) << 4) | (u8_time_sec %10));
	rtc_System_Time.sec = u8_temp;
	u8_temp = (((u8_time_minute/10) << 4) | (u8_time_minute %10));
	rtc_System_Time.min = u8_temp;
	u8_temp = (((u8_time_hour/10) << 4) | (u8_time_hour %10));
	rtc_System_Time.hour = u8_temp;
	R_RTC_Set_ConstPeriodInterruptOff();// stopping RTC interrupt
	R_RTC_Set_CounterValue(rtc_System_Time);// setting RTC with current seconds, hours, minutes
	R_RTC_Set_ConstPeriodInterruptOn(ONEMIN);// starting RTC interrupt with the period HALF SECONDS
	
	u8_system_current_hour = BCDToDecimal(rtc_System_Time.hour);
    	u8_system_current_min = BCDToDecimal(rtc_System_Time.min);
	u8_system_current_sec = BCDToDecimal(rtc_System_Time.sec);
    	u8_system_current_date = BCDToDecimal(rtc_System_Time.day);
	u8_system_current_month = BCDToDecimal(rtc_System_Time.month);
    	u8_system_current_year = BCDToDecimal(rtc_System_Time.year);
	
}




/*****************************************************************************************
* Function name	 	: 	uint8_t BCDToDecimal(uint8_t bcdByte)
* returns 		: 	void
* arg1 			: 	void
* Created by 		: 	jomon
* Date created 		: 	08-22-2017
* Notes 		: 	Function for converting BCD value t DECIMAL value
******************************************************************************************/
uint8_t BCDToDecimal(uint8_t bcdByte)
{
  return (((bcdByte & 0xF0) >> 4) * 10) + (bcdByte & 0x0F);
}



/*****************************************************************************************
* Function name	 	: 	uint8_t BCDToDecimal(uint8_t bcdByte)
* returns 		: 	void
* arg1 			: 	void
* Created by 		: 	jomon
* Date created 		: 	08-22-2017
* Notes 		: 	Function for converting DECIMAL value t BCD value
******************************************************************************************/
uint8_t DecimalToBCD(uint8_t decimalByte)
{
  return (((decimalByte / 10) << 4) | (decimalByte % 10));
}
/*****************************************************************************************
* Function name	 	: 	void BCDToDecimal(uint8_t bcdByte)
* returns 		: 	void
* arg1 			: 	void
* Created by 		: 	Aarathy Ramesh
* Date created 		: 	08-22-2017
* Notes 		: 	Function for converting DECIMAL value t BCD value
******************************************************************************************/
void v_RTC_Get_Time(void)
{
    
        R_RTC_Get_CounterValue(&rtc_System_Time); 
        u8_system_current_hour = BCDToDecimal(rtc_System_Time.hour);
	u8_system_current_min = BCDToDecimal(rtc_System_Time.min);
	u8_system_current_sec = BCDToDecimal(rtc_System_Time.sec);
	u8_system_current_date = BCDToDecimal(rtc_System_Time.day);
	u8_system_current_month = BCDToDecimal(rtc_System_Time.month);
	u8_system_current_year = BCDToDecimal(rtc_System_Time.year);
    
}