/*******************************************************************************

* Module name: battery_measure.c

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

* battery_measure.c program

*
***************************************************************************************/
/*************************** HEADER FILES *********************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_adc.h"
#include "interval_timer.h"
#include "adc.h"
#include "Battery_measure.h"
/*******************************************************************************

* Global Variable Section

*******************************************************************************/
uint32_t u32_battery_sample_charge = 0; 
uint16_t u16_battery_charge = 0;
uint8_t u8_get_battery_status = 0;
uint16_t u16_battery_charge_count = 0;

/*****************************************************************************************
* Function name	 	: 	uint16_t battery_measure(void)
* returns 		: 	uint16_t
* arg1 			: 	void
* Created by 		: 	Drisya.K
* Date created 		: 	02-24-2017 
* Edited By		:	Drisya.K
* Description 		: 	get the higher value
* Notes 		: 	
******************************************************************************************/
void v_battery_measure(void)
{
	if(u8_get_battery_status == 0)
	{
		u32_battery_sample_charge += 	u16_AD_Conv(AD2);
		u16_battery_charge_count++;
		if(u16_battery_charge_count == BATTERY_SAMPLE_COUNT)
		{
			/*u32_battery_sample_charge = u32_battery_sample_charge >> 10;
			
			u32_battery_sample_charge = (u32_battery_sample_charge * 7590); //*1024000(Three decimal point)
			u32_battery_sample_charge = u32_battery_sample_charge >> 10;
			
			if(u32_battery_sample_charge < BATTEY_MINIMUM_VOLTAGE)
			     u32_battery_sample_charge = BATTEY_MINIMUM_VOLTAGE;
			u32_battery_sample_charge = (u32_battery_sample_charge - BATTEY_MINIMUM_VOLTAGE) * 100;
			u32_battery_sample_charge = u32_battery_sample_charge/(BATTEY_MAXIMUM_VOLTAGE - BATTEY_MINIMUM_VOLTAGE);  //(4.2 - 3.6 = 0.6)
			
			if(u32_battery_sample_charge > 100)
				u32_battery_sample_charge = 100;
			u16_battery_charge = (uint16_t)u32_battery_sample_charge;
			
			u16_battery_charge_count =0;
			u32_battery_sample_charge = 0;
			u8_get_battery_status = 1;*/
			
			u32_battery_sample_charge = u32_battery_sample_charge / BATTERY_SAMPLE_COUNT;
			
			u32_battery_sample_charge = (u32_battery_sample_charge * ((3.3/1024)/(132/100))) * 1000;
			if(u32_battery_sample_charge < 1550)
			     u32_battery_sample_charge = 1550;
			if(u32_battery_sample_charge > 1800)
		             u32_battery_sample_charge = 1800;
			u32_battery_sample_charge = ((float)(u32_battery_sample_charge - 1550)/(1800 - 1550)) * 100;  //(4.2 - 3.6 = 0.6)
			u16_battery_charge = (uint16_t)u32_battery_sample_charge;
			
			u16_battery_charge_count =0;
			u32_battery_sample_charge = 0;
			u8_get_battery_status = 1;
			
			
		}
	}
	
}
