/*******************************************************************************
* Module name: interval_timer.c
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
* First written on 11/22/2014 by Reshma Joseph.
*
* Module Description:
* RL78 G12 interval_timer program
*
*******************************************************************************/
/*******************************************************************************
* Include section
*
*******************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
/* Start user code for include. Do not edit comment generated here */
#include "RFID.h"
#include "gsm_gprs.h"
#include "Battery_measure.h"
#include "mainhandler.h"
#include "USB_Config.h"

#include "r_cg_timer.h"
#include "r_cg_intc.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
uint16_t u16_Delay_ms = 0;
volatile uint8_t u8_Key_interrupt_flag;

uint8_t u8_Key_Press_Timeout_counter = 0;
uint8_t u8_Button_Press = 0;
uint8_t power_on_status = 1;
uint8_t power_on_led_indication = 0;
uint8_t power_of_led_indication = 0;
uint8_t u8_button_press_wait = 0;
#define WAIT_TIME 10

/* End user code. Do not edit comment generated here */



/*******************************************************************************
* Function name : extern void v_Scheduler(void)
* returns 	: none
* arg 		: none
* Created by 	: Sreelakshmi T M
* Date created 	: 11/24/2014
* Description 	: ISR for scheduler
*******************************************************************************/
void v_Scheduler(void)
{
	if(u16_Delay_ms)
	{
		u16_Delay_ms--;
	}
	if(u16_PDP_LED_Blink_Count)
	{
		u16_PDP_LED_Blink_Count--;
		if(!u16_PDP_LED_Blink_Count)
		{
			GSM_DATA_STATUS_LED = LED_OFF;
			GSM_PDP_STATUS_LED = ~GSM_PDP_STATUS_LED;
			u16_PDP_LED_Blink_Count = u8_LED_Set_Blink_Count;
		}
	}
	if(u16_DATA_LED_Blink_Count)
	{
		u16_DATA_LED_Blink_Count--;
		if(!u16_DATA_LED_Blink_Count)
		{
			GSM_PDP_STATUS_LED = LED_OFF;
			GSM_DATA_STATUS_LED = ~GSM_DATA_STATUS_LED;
			u16_DATA_LED_Blink_Count = u8_LED_Set_Blink_Count;
		}
	}
	if(u16_DEBUG_LED_Blink_Count)
	{
		u16_DEBUG_LED_Blink_Count--;
		if( (!u16_DEBUG_LED_Blink_Count)) 
		{
			u8_Debug_LED_Number_Of_Blinks++;
			GSM_DEBUG_LED = ~GSM_DEBUG_LED;
			u16_DEBUG_LED_Blink_Count = u16_DEBUG_LED_Set_Blink_Count;
			if(u8_Debug_LED_Number_Of_Blinks  >= (u8_Debug_LED_Set_Number_Of_Blinks * 2))
			{
				DEBUG_LED_OFF();
				u8_Debug_LED_Number_Of_Blinks = 0;
				u16_DEBUG_LED_Blink_Count = 1000;
			}			
		}
	}

	if(u8_Configuration_mode == NORMAL_MODE)
	{
		v_battery_measure();
	}
	v_RFID_Scheduler();
	v_GSM_Timeout_Handler();
	if(u16_USB_RX_Next_Byte_TMO)
	{
		u16_USB_RX_Next_Byte_TMO--;
	}
	if(u8_Configuration_mode == NORMAL_MODE)
	{
		if((!u16_USB_RX_Next_Byte_TMO) && (u8_usb_byte_counter))
		{
			u8_usb_byte_counter = 0;
			USB_data_rev_flg = 0;
		}
	}
	else if(u8_Configuration_mode == USB_MODE)
	{
		if((!u16_USB_RX_Next_Byte_TMO) && (u8_usb_byte_counter) && (!USB_data_rev_flg))
		{
			u8_usb_byte_counter = 0;
			USB_data_rev_flg = 0;
		}
	}
	
	
	
}


/*******************************************************************************
* Function name : void v_Button_Timer(void)
* returns 	: none
* arg 		: none
* Created by 	: jomon
* Date created 	: 09/24/2019
* Description 	: ISR for Button Timer
*******************************************************************************/
void v_Button_Timer(void)
{
	
	if(u8_Key_interrupt_flag == 1)
	{
	   u8_Key_interrupt_flag = 0;
	   u8_button_press_wait = 0;
	   u8_Key_Press_Timeout_counter = WAIT_TIME;
	   u8_Button_Press++; 
	}
	
	
	if(u8_Key_Press_Timeout_counter > 0)
	{
		u8_Key_Press_Timeout_counter--;
		
		if(u8_Key_Press_Timeout_counter == 0)
		{
		    if( (power_on_status == 0) && (u8_Button_Press != 3) )
		    {
			    DEVICE_POWER_ENABLE_PIN = 0;  //going to power off if false key press(other than doublr tap) while device is in poweroff mode
		    }
		
			
		    switch(u8_Button_Press)
		    {
			 case 1:
			 	//debuf flag activate
				if( (power_on_status == 1) && (USB_STATUS == 0) )
				{
					u8_Debug_Key_flag = 1;
				}
			 break;
			 
			 case 3:
			       //poweroff
			       if(power_on_status == 0)
			       {
			       		power_on_status = 1;
					power_on_led_indication = 7;
					DEBUG_LED_ON();
			       }
			       else
			       {
			       	      
			       }
			 break;
			 
			 case 4:
			      if(power_on_status == 1)
			      {  
				 //DEVICE_POWER_ENABLE_PIN = 0; //device will power off here no need to clear the status flag
			       	 power_of_led_indication = 7;
			       
			         u16_DEBUG_LED_Blink_Count = 0;
			         u16_DATA_LED_Blink_Count = 0;
			         u16_PDP_LED_Blink_Count = 0;
			         
				 GSM_POWER_ENABLE = 0;    //Turning off the gsm
				 RFID_POWER_EN_PIN = 0;   //Turning off the RFID
    				 RFID_RF_EN_PIN = 0;      //Turning off the RFID
				 
			         DEBUG_LED_OFF();
			         GSM_PDP_STATUS_LED = LED_OFF;
			         GSM_DATA_STATUS_LED = LED_ON;      
			      }
			 
			     //not used case
			 break;
			 
			 default:
			     //unknown case
			 break;
			    
		    }
		    
		    u8_Button_Press = 0;
		}
	}
	
	
	if( (u8_Button_Press == 0) && (power_on_status == 0) )
	{
		u8_button_press_wait++;
		if(u8_button_press_wait == 10)
		{
			DEVICE_POWER_ENABLE_PIN = 0;
		}
	}
	
	
	if(power_on_led_indication > 0)
	{
		power_on_led_indication--;
		if(power_on_led_indication == 5)
		{
			DEBUG_LED_OFF();
			GSM_PDP_STATUS_LED = LED_ON;
			GSM_DATA_STATUS_LED = LED_OFF;
		}
		if(power_on_led_indication == 3)
		{
			DEBUG_LED_OFF();
			GSM_PDP_STATUS_LED = LED_OFF;
			GSM_DATA_STATUS_LED = LED_ON;
		}
		if(power_on_led_indication == 0)
		{
			DEBUG_LED_OFF();
			GSM_PDP_STATUS_LED = LED_OFF;
			GSM_DATA_STATUS_LED = LED_OFF;
		}
	
	}
	
	if(power_of_led_indication > 0)
	{
		power_of_led_indication--;
		if(power_of_led_indication == 5)
		{
			DEBUG_LED_OFF();
			GSM_PDP_STATUS_LED = LED_ON;
			GSM_DATA_STATUS_LED = LED_OFF;
		}
		if(power_of_led_indication == 3)
		{
			DEBUG_LED_ON();
			GSM_PDP_STATUS_LED = LED_OFF;
			GSM_DATA_STATUS_LED = LED_OFF;
		}
		if(power_of_led_indication == 0)
		{
			DEBUG_LED_OFF();
			power_on_status = 0;
			u8_Button_Press = 0;
			DEVICE_POWER_ENABLE_PIN = 0; //device will power off here no need to clear the status flag
		}
	
	}
	
	
}



/*******************************************************************************
* Function name 	: void	v_Micro_Delay		(unsigned int D_Count)
* returns 		: none
* u16_mill_delay 	: delay in us
* Created by 		: Reshma Joseph
* Date created 		: 11/28/2014
* Description 		: delay in ms 
*******************************************************************************/
void v_Micro_Delay(unsigned int D_Count)
{
	unsigned int i,k;
	for(i=0;i<D_Count;i++)
	{
		for(k=0;k<500;k++)
		{
			NOP();
		}
	}
}


/*******************************************************************************
* Function name 	: void	v_delay_ms(uint16_t u16_mill_delay)
* returns 		: none
* u16_mill_delay 	: delay in ms
* Created by 		: Reshma Joseph
* Date created 		: 11/28/2014
* Description 		: delay in ms 
*******************************************************************************/
void v_delay_ms(uint16_t u16_ms_delay)
{
	u16_Delay_ms = u16_ms_delay;
	while(u16_Delay_ms)
	{
		NOP();
	}
}



