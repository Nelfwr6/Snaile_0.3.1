/*******************************************************************************

* Module name: USB.c

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
#include "USB_Config.h"
#include "interval_timer.h"
#include "r_cg_serial.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "EEPROM.h"
#include "RTC.h"
#include "RFID.h"
#include "r_cg_rtc.h"
#include "mainhandler.h"
#include "RTC.h"
#include "gsm_gprs.h"
#include "r_cg_timer.h"
#include "r_cg_serial.h"
#include "MQTT.h"

/*******************************************************************************

* Global Variable Section

*******************************************************************************/
uint8_t u8_usb_configration_buffer[USB_PACKET_COUNTER],u8_USB_Cmd = 0,u8_usb_data[10],u8_range[10];
uint8_t u8_USB_data_rcv_status =0 ;
uint8_t u8_usb_byte_counter = 0,USB_data_rev_flg = 0;
uint16_t u16_USB_detection_timeout,u16_new_time = 0;
uint8_t u8_new_password[7];
uint16_t u16_URL_length = 0;
uint8_t usb_connection_success = 0;
uint16_t u16_apn_length;
rtc_counter_value_t  rtc_current_counter_value;
uint16_t u16_USB_RX_Next_Byte_TMO;
uint8_t u8_previous_data[25], u8_Data_Write_Flag, u8_USB_Config_Err = 0;
uint8_t u8_timezone[22] = "10:10:10GMT+05:30";
uint8_t update_done_flag = 0;

uint8_t u8_restart_time_hr,  u8_restart_time_min, u8_restart_time_sec;
uint8_t u8_restart_date, u8_restart_month, u16_restart_year;
/*****************************************************************************************
* Function name	 	: 	uint32_t u32_calculate_scanning_timeout(void)
* returns 		: 	scanning timeout
* Created by 		: 	jomon
* Date created 		: 	08-5-2017
* Description 		: 	Function for finding first time scanning timeout
******************************************************************************************/
uint32_t u32_calculate_scanning_timeout(void)
{
	uint16_t u16_Current_Time_In_minutes = 0;
	uint16_t u16_Start_Time_In_minutes = 0;
	uint32_t u32_Time_difference = 0;
	u8_Device_State = u8_Check_Working_Time();
	if(u8_Device_State == DEVICE_WORKING_TIME)
	{
	    u16_Current_Time_In_minutes = ((uint16_t)u8_system_current_hour * 60) + u8_system_current_min;
	    
	    u16_Start_Time_In_minutes = ((uint16_t) D_Config_Packet.Member.u8_day_start_time_hr  * 60) + D_Config_Packet.Member.u8_day_start_time_min;
	    
	    if(u16_Current_Time_In_minutes > u16_Start_Time_In_minutes)
	    {
		    u32_Time_difference =  ((uint32_t)u16_Current_Time_In_minutes) - ((uint32_t)u16_Start_Time_In_minutes);
		    u32_Time_difference = u32_Time_difference %  D_Config_Packet.Member.u32_scanning_timeout;
		    if(!u32_Time_difference)		   
		    {
			   u32_Time_difference = D_Config_Packet.Member.u32_scanning_timeout;
		    }
		    
	    }
	    else
	    u32_Time_difference = D_Config_Packet.Member.u32_scanning_timeout;
	    return u32_Time_difference;
	}
	
	return 0;
        
}



/*****************************************************************************************
* Function name	 	: 	uint8_t u8_USB_data_handling()
* returns 		: 	ADC result
* u8_ADC_Channel 	: 	ADC channel selection
* Created by 		: 	Reshma Joseph
* Date created 		: 	08-22-2014
* Edited By		:	Reshma Joseph
* Description 		: 	ADC channel selection and return the adc result
* Notes 		: 	Edited for ADC channel selection and return the adc result
******************************************************************************************/
uint8_t u8_USB_data_handling(void)
{
	//u32_detection_intervel = 0;
	uint32_t u32_temp, u32_temp1;
	uint8_t u8_k, u8_i;
	u8_Data_Write_Flag = 0;
	u8_USB_Cmd = 0;
   	u32_temp  = ((u8_usb_configration_buffer[0] << 4)& 0xF0);
    	u8_USB_Cmd = (uint8_t)(u32_temp);	
	u32_temp =  ( u8_usb_configration_buffer[1] & 0x0F);
	u8_USB_Cmd |= (uint8_t)u32_temp;
	switch(u8_USB_Cmd)
	{
		case SET_SCANNING_INTERVAL_CMND: //detection timeout
		 	u8_k = 4;
			u32_temp1 = 0;
		        for(u8_i = 2; u8_usb_configration_buffer[u8_i]!= 0x0d; u8_i++)
			{
			   u8_usb_data[u8_i - 2] = u8_usb_configration_buffer[u8_i];
			   u32_temp               = ASCII_hex_converter(u8_usb_data[u8_i - 2]);
			   u32_temp1             =(u32_temp1 << u8_k) | u32_temp;
			}
			u8_usb_data[u8_i - 2] = 0;
			D_Config_Packet.Member.u32_scanning_timeout = u32_temp1 / 60;
			u8_Data_Write_Flag = 1;
			u8_USB_data_rcv_status = USB_SUCCESS;
			break;
		case SET_IR_CALIBERATION_CMND:  //ir calibration
			u8_USB_data_rcv_status = USB_SUCCESS;
			break;
		case SET_START_TIME_CMND:  //day start hour minute
			u32_temp =  ASCII_hex_converter(u8_usb_configration_buffer[2]);
			u32_temp1             = (u32_temp << 4) | ASCII_hex_converter(u8_usb_configration_buffer[3]);
			D_Config_Packet.Member.u8_day_start_time_hr = (uint8_t)u32_temp1;
			u32_temp = ASCII_hex_converter(u8_usb_configration_buffer[4]);
			u32_temp1             = (u32_temp<<4) | ASCII_hex_converter(u8_usb_configration_buffer[5]);
			D_Config_Packet.Member.u8_day_start_time_min = (uint8_t)u32_temp1;
			u8_Data_Write_Flag = 1;
			u8_USB_data_rcv_status = USB_SUCCESS;
			break;
		case SET_END_TIME_CMND:  //day end hour and minute
			u32_temp = ASCII_hex_converter(u8_usb_configration_buffer[2]);
			u32_temp1             = (u32_temp<<4) | ASCII_hex_converter(u8_usb_configration_buffer[3]);
			D_Config_Packet.Member.u8_day_End_time_hr = (uint8_t)u32_temp1;
			u32_temp = ASCII_hex_converter(u8_usb_configration_buffer[4]);
			u32_temp1             = (u32_temp<<4) | ASCII_hex_converter(u8_usb_configration_buffer[5]);
			D_Config_Packet.Member.u8_day_End_time_min = (uint8_t)u32_temp1;
			
			//D_Config_Packet.Member.u32_scanning_timeout = u32_first_time_scanning_timeout();
			u8_Data_Write_Flag = 1;
			u8_USB_data_rcv_status = USB_SUCCESS;
			break;
		case SET_CURRENT_DATE_CMND:  //day month year
			u32_temp = ASCII_hex_converter(u8_usb_configration_buffer[2]);
			u32_temp1             = (u32_temp*10) + ASCII_hex_converter(u8_usb_configration_buffer[3]);
			
			D_Config_Packet.Member.u8_current_date = (uint8_t)u32_temp1;
			u32_temp = ASCII_hex_converter(u8_usb_configration_buffer[5]);
			u32_temp1             = (u32_temp*10) + ASCII_hex_converter(u8_usb_configration_buffer[6]);
			D_Config_Packet.Member.u8_current_month = (uint8_t)u32_temp1;
			u32_temp1 = 0;
			u32_temp = ASCII_hex_converter(u8_usb_configration_buffer[8]);
			u32_temp1             = (u32_temp*10) + ASCII_hex_converter(u8_usb_configration_buffer[9]);
			D_Config_Packet.Member.u16_current_year = (uint8_t)u32_temp1;
			u32_temp1 = 0;
			
			u8_USB_data_rcv_status = USB_SUCCESS;
			break;
		case SET_CURRENT_TIME_CMND:  //hour minute second
			u32_temp = ASCII_hex_converter(u8_usb_configration_buffer[2]);
			u32_temp1             = (u32_temp*10) + ASCII_hex_converter(u8_usb_configration_buffer[3]);
			D_Config_Packet.Member.u8_current_time_hr = (uint8_t)u32_temp1;
			u32_temp = ASCII_hex_converter(u8_usb_configration_buffer[5]);
			u32_temp1             = (u32_temp*10) + ASCII_hex_converter(u8_usb_configration_buffer[6]);
			D_Config_Packet.Member.u8_current_time_min = (uint8_t)u32_temp1;
			u32_temp1 = 0;
			u32_temp = ASCII_hex_converter(u8_usb_configration_buffer[8]);
			u32_temp1             = (u32_temp*10) + ASCII_hex_converter(u8_usb_configration_buffer[9]);
			D_Config_Packet.Member.u8_current_time_sec = (uint8_t)u32_temp1;
			v_update_date_time();
			
			if( RTC_Update_To_GSM() == 0)
			    u8_USB_data_rcv_status = USB_SUCCESS;
			else
			   u8_USB_data_rcv_status = USB_ERROR;	

			break;
		case CHECK_PASSWORD_CMND:  //password checking
			for(u8_i = 0; u8_i< 6; u8_i++)
					u8_new_password[u8_i ] = 0;
			
			
			for(u8_i = 2; u8_usb_configration_buffer[u8_i]!=0x0d; u8_i++)
				u8_new_password[u8_i - 2] = u8_usb_configration_buffer[u8_i];
		
			u8_i = (uint8_t)strcmp((char *)D_Config_Packet.Member.u8_password,(char *)u8_new_password);
			if(u8_i == 0)
				u8_USB_data_rcv_status = USB_SUCCESS;
                        else
				u8_USB_data_rcv_status = USB_ERROR;
			break;
		case SET_PASSWORD_CMND:  //new password
			for(u8_i = 2; u8_usb_configration_buffer[u8_i]!=0x0d; u8_i++)
				u8_new_password[u8_i - 2] = u8_usb_configration_buffer[u8_i];
			u8_new_password[u8_i - 2] = 0;
			strcpy((char *)D_Config_Packet.Member.u8_password,(char *)u8_new_password); 
			u8_USB_data_rcv_status = USB_SUCCESS;
			u8_Data_Write_Flag = 1;
			break;
		case GET_DEVICE_ID_CMND:  //device id send
			v_uart_str_send(DEVICE_ID,USB_CHANNEL);    // Device ID
			u8_USB_data_rcv_status = USB_SUCCESS;
			break;
		case GET_CONFIGURATION_CMND:  //previews data send
		        
    			v_RTC_Get_Time();
		        //R_RTC_Get_CounterValue(&rtc_current_counter_value);
			D_Config_Packet.Member.u8_current_time_sec  = u8_system_current_sec;
                        D_Config_Packet.Member.u8_current_time_min   = u8_system_current_min;
   		        D_Config_Packet.Member.u8_current_time_hr   = u8_system_current_hour;
                        D_Config_Packet.Member.u8_current_date  = u8_system_current_date;
                        D_Config_Packet.Member.u8_current_month = u8_system_current_month;
                        D_Config_Packet.Member.u16_current_year = u8_system_current_year;
		      
			u8_previous_data[0]  = D_Config_Packet.Member.u8_current_date;       //Previous Data
			u8_previous_data[1]  = D_Config_Packet.Member.u8_current_month;
			u8_previous_data[2]  = D_Config_Packet.Member.u16_current_year;
			
			u8_previous_data[3]  = D_Config_Packet.Member.u8_current_time_hr;
			u8_previous_data[4]  = D_Config_Packet.Member.u8_current_time_min;
			u8_previous_data[5]  = D_Config_Packet.Member.u8_current_time_sec;
			
			u8_previous_data[6]  = D_Config_Packet.Member.u8_day_start_time_hr;
			u8_previous_data[7]  = D_Config_Packet.Member.u8_day_start_time_min;
			
			u8_previous_data[8]  = D_Config_Packet.Member.u8_day_End_time_hr;
			u8_previous_data[9]  = D_Config_Packet.Member.u8_day_End_time_min;
			u32_temp = D_Config_Packet.Member.u32_scanning_timeout * 60000;
			u8_previous_data[10] = (uint8_t)(u32_temp >> 24);
			u8_previous_data[11] = (uint8_t)(u32_temp >> 16);
			u8_previous_data[12] = (uint8_t)(u32_temp >> 8);
			u8_previous_data[13] = (uint8_t)(u32_temp);
			sprintf((char *)u8_timezone, "%c%2d:%2d",(char)D_Config_Packet.Member.u8_timezone_sign, D_Config_Packet.Member.u16_time_zone_hr, D_Config_Packet.Member.u16_time_zone_min);
			for(u8_i = 14,u8_k = 11; u8_k <=20; u8_i++, u8_k++)
				u8_previous_data[u8_i] = u8_timezone[u8_k];
			u8_previous_data[u8_i] = '\0';
			v_byte_array_send(u8_previous_data,u8_i,USB_CHANNEL);
			u8_USB_data_rcv_status = USB_SUCCESS;
			break;	
		case SET_URL_CMND:  // new url
			for(u8_i =2;u8_usb_configration_buffer[u8_i] != 0x0d; u8_i++) // New URL
			{
				D_Config_Packet.Member.u8_URL[u8_i-2] = u8_usb_configration_buffer[u8_i];
			}
			D_Config_Packet.Member.u8_URL[u8_i-2] = 0x00;
			u16_URL_length = strlen((char *)D_Config_Packet.Member.u8_URL);
			u8_USB_data_rcv_status = USB_SUCCESS;
			u8_Data_Write_Flag = 1;
			break;
		case GET_URL_CMND:  //url sent
		        u16_URL_length = strlen((char *)D_Config_Packet.Member.u8_URL);
			v_byte_array_send(D_Config_Packet.Member.u8_URL,u16_URL_length,USB_CHANNEL); //URL
			u8_USB_data_rcv_status = USB_SUCCESS;
			break;
			
		case SET_TIME_ZONE_CMND: //TIME ZONE						
		 	for(u8_i = 2; u8_usb_configration_buffer[u8_i]!=0x0d; u8_i++)
				u8_timezone[u8_i - 2] = u8_usb_configration_buffer[u8_i]; 
			for(u8_i = 0; u8_i < 2; u8_i++)
			{
				u32_temp = u8_timezone[u8_i] - 0x30;
				D_Config_Packet.Member.u16_GMT_hr = (D_Config_Packet.Member.u16_GMT_hr * 10) + (uint16_t)u32_temp;
			}
			for(u8_i = 3; u8_i < 5; u8_i++)
			{
				u32_temp = u8_timezone[u8_i] - 0x30;
				D_Config_Packet.Member.u16_GMT_min = (D_Config_Packet.Member.u16_GMT_min * 10) + (uint16_t)u32_temp;
			}
			for(u8_i = 6; u8_i < 8; u8_i++)
			{
				u32_temp     = u8_timezone[u8_i] - 0x30;
				D_Config_Packet.Member.u16_GMT_sec = (D_Config_Packet.Member.u16_GMT_sec * 10) + (uint16_t)u32_temp;
			}
			D_Config_Packet.Member.u8_timezone_sign = u8_timezone[11];
			for(u8_i = 12; u8_i < 14; u8_i++)
			{
				u32_temp          = u8_timezone[u8_i] - 0x30;
				D_Config_Packet.Member.u16_time_zone_hr = (D_Config_Packet.Member.u16_time_zone_hr * 10) + (uint16_t)u32_temp;
			}
			for(u8_i = 15; u8_i < 17; u8_i++)
			{
				u32_temp          = u8_timezone[u8_i] - 0x30;
				D_Config_Packet.Member.u16_time_zone_min = (D_Config_Packet.Member.u16_time_zone_min * 10) + (uint16_t)u32_temp;
			}
			
			Erase_RFID_Data_Flash();
			u32_current_scanning_timeout = u32_calculate_scanning_timeout();
			u8_Data_Write_Flag = 1;
			
			
			u8_USB_data_rcv_status = USB_SUCCESS;
			break;
		
		case SET_APN_CMND:  //new APN
		       for(u8_i =2;u8_usb_configration_buffer[u8_i] != 0x0d; u8_i++) 
		       {
		           D_Config_Packet.Member.u8_apn[u8_i-2] = u8_usb_configration_buffer[u8_i];
		       }
		       D_Config_Packet.Member.u8_apn[u8_i-2] = 0x00;
		       u8_USB_data_rcv_status = USB_SUCCESS;
		       u8_Data_Write_Flag = 1;
		       break;
		case GET_APN_CMND:  //APN sent
		       u16_apn_length = strlen((char *)D_Config_Packet.Member.u8_apn);
		       v_byte_array_send(D_Config_Packet.Member.u8_apn,u16_apn_length,USB_CHANNEL); 
		       u8_USB_data_rcv_status = USB_SUCCESS;
		       break;
		default:
		       u8_USB_data_rcv_status = USB_ERROR;
		       break;  
	
	}
	if(u8_Data_Write_Flag)
	{
		D_Config_Packet.Member.u32_Checksum_Value = u32_checksum_calculate(&D_Config_Packet.All[0], CONFIG_DF_DATA_SIZE - 4);
		Write_Dconfig_Data_Flash();
		
		u32_RFID_New_Data_Location_Ptr = 0;
	        u8_RFID_New_Data_Count = 0;
		v_clear_RFID_data_buf();
	}
	memset(u8_usb_configration_buffer, 0x00, 100);
	
	return u8_USB_data_rcv_status;
}
/*****************************************************************************************
* Function name	 	: 	uint8_t ASCII_hex_converter(uint8_t u8_data)
* returns 		: 	hex value
* u8_data 		: 	ASCII value
* Created by 		: 	Drisya
* Date created 		: 	30-03-2017
* Description 		: 	ADC channel selection and return the adc result
* Notes 		: 	Edited for ADC channel selection and return the adc result
******************************************************************************************/
uint8_t ASCII_hex_converter(uint8_t u8_data)
{
	uint8_t u8_temp_data = 0;
	if((u8_data >= 0x30 )&& (u8_data <= 0x39))
		u8_temp_data             = (u8_data - 0x30);
	else 
	{
		u8_temp_data     = u8_data - 0x57;
	}
	return u8_temp_data;
}



/*****************************************************************************************
* Function name	 	: 	void USB_Response(void)
* u8_USB_Status 	: 	USB status
* Created by 		: 	Drisya
* Date created 		: 	29-03-2017
* Description 		: 	
* Notes 		: 	
******************************************************************************************/
void USB_Response(uint8_t u8_USB_Status)
{
	if((u8_USB_Status == USB_SUCCESS))
	{
		v_uart_str_send("OK",USB_CHANNEL);
		u8_USB_Status = 0;
		
	}
	else if(u8_USB_Status ==  USB_ERROR)
	{
		v_uart_str_send("ERROR",USB_CHANNEL);
		u8_USB_Status = 0;
	}
}




/*****************************************************************************************
* Function name	 	: 	void v_usb_configuration(void)
* u8_USB_Status 	: 	USB status
* Created by 		: 	jomon
* Date created 		: 	29-03-2017
* Description 		: 	function for configure usb data
* Notes 		: 	
******************************************************************************************/
void v_usb_configuration(void)
{
	uint8_t u8_USB_Debounce = 0;
	uint8_t u8_USB_RESPONCE_Status = 0;
	u8_usb_byte_counter = 0;
	
	while(USB_STATUS == 1)
	{
		v_delay_ms(100);
		u8_USB_Debounce ++;
		if(u8_USB_Debounce > 5)
		{
			R_SAU0_Create();
			R_UART1_Create();
			R_UART1_Start(); 
			
			v_Gsm_Hardware_Init();
			break;
		}
			
	}
	if(u8_USB_Debounce > 5)
	{	
	    while(1)
	    {
		if(USB_data_rev_flg == 1)
		{
			u8_USB_RESPONCE_Status =  u8_USB_data_handling();
			USB_data_rev_flg = 0;
		}
		if(u8_USB_RESPONCE_Status)
		{
			USB_data_rev_flg = 0;
	        	USB_Response(u8_USB_RESPONCE_Status);		//PC USB Interface mode for configuration settings	
		        u8_USB_RESPONCE_Status = 0;
			
		}
		//USB_STATUS = P3.0;    
		if( USB_STATUS == 0)
		{
			u8_USB_Debounce = 0;
			while(0 == USB_STATUS)
			{
				v_delay_ms(100);
				u8_USB_Debounce ++;
				if(u8_USB_Debounce > 10)
				{
					break;
				}
			}
			if(u8_USB_Debounce > 10)
			{ 	
				v_Gsm_Power_Down();
				break;
			}
		}	
	   }
      }
		
}







/*****************************************************************************************
* Function name	 	: 	usb_connected
* returns 		: 	status
* Created by 		: 	jomon
* Date created 		: 	06-22-2017
* Description 		: 	Functions for setting usb connection flag
******************************************************************************************/
void set_usb_connected(void)
{
	
    usb_connection_success = 1;	
}

/*****************************************************************************************
* Function name	 	: 	get_usb_connection_status
* returns 		: 	status
* Created by 		: 	jomon
* Date created 		: 	06-22-2017
* Description 		: 	Functions for setting usb connection flag
******************************************************************************************/
uint8_t get_usb_connection_status(void)
{
     	
    return usb_connection_success;	
}

/*****************************************************************************************
* Function name	 	: 	clear_usb_connection_status
* returns 		: 	none
* Created by 		: 	jomon
* Date created 		: 	06-22-2017
* Description 		: 	Functions for clearing usb connection flag
******************************************************************************************/

void clear_usb_connection_status(void)
{
	
    usb_connection_success = 0;	
}

/*****************************************************************************************
* Function name	 	: 	v_USB_RX_ISR
* returns 		: 	void
* u8_rx_data 		: 	RX data
* Created by 		: 	Jomon Thomas
* Date created 		: 	08-22-2017
* Description 		: 	function for turn on rfid

******************************************************************************************/
void v_USB_RX_ISR(uint8_t u8_rx_data)
{
	u8_usb_configration_buffer[u8_usb_byte_counter++] = u8_rx_data;
	u16_USB_RX_Next_Byte_TMO = 200;
     if((u8_usb_configration_buffer[u8_usb_byte_counter-1] == 0x0a)&& (u8_usb_configration_buffer[u8_usb_byte_counter - 2]== 0x0d)) 
     {
	  u8_usb_configration_buffer[u8_usb_byte_counter]   = 0;
	  USB_data_rev_flg = 1;
	  u8_usb_byte_counter = 0;
     }
     else
     	USB_data_rev_flg = 0;
     if(u8_usb_byte_counter == USB_PACKET_COUNTER)
     	u8_usb_byte_counter =0;
}
/****************************************************************************************

* Function name	 	: 	void Update_Configuration(void)
* returns 		: 	
* Created by 		: 	
* Date created 		: 	
* Description 		: 	

******************************************************************************************/

void Update_Configuration(void)

{
   char str1[512];
   char str2[50];
   char c[5];
   char *p =&Subscribed_data;
   
   
  
   			strcpy(str1,(char*)Subscribed_data);
 			p = strstr(str1,"\"device_current_date\":\"");
		   if(p)
			{
		   	p=p+sizeof("\"device_current_date\":\"")-1;
			strncpy(str2,p,strchr(p,'"')-p);
			str2[strchr(p,'"')-p]='\0';
		            c[0] = str2[0];
			    c[1] = str2[1];
			    c[2] = '\0';
			    D_Config_Packet.Member.u8_current_date	= atoi(c); 
			    c[0]=str2[3];
			    c[1]=str2[4];
			    c[2]='\0';
			    D_Config_Packet.Member.u8_current_month	= atoi(c);
			    c[0]=str2[6];
			    c[1]=str2[7];
			    //c[2]=str2[8];
			    //c[3]=str2[9];
			    c[2]='\0';
			   D_Config_Packet.Member.u16_current_year	 = atoi(c);
			    u8_Data_Write_Flag = 1;
			}
				
				
				
 			p = strstr(str1,"\"device_current_time\":\"");
		   	if(p)
			{
			   	p=p+sizeof("\"device_current_time\":\"")-1;
				strncpy(str2,p,strchr(p,'"')-p);
				str2[strchr(p,'"')-p]='\0';
				//u32_temp = ASCII_hex_converter(u8_usb_configration_buffer[2]);
			       // u32_temp1             = (u32_temp*10) + ASCII_hex_converter(u8_usb_configration_buffer[3]);
			       // D_Config_Packet.Member.u8_current_time_hr = (uint8_t)u32_temp1;
				c[0]=str2[0];
				c[1]=str2[1];
				c[2]='\0';
				D_Config_Packet.Member.u8_current_time_hr = atoi(c);
				c[0]=str2[3];
				c[1]=str2[4];
				c[2]='\0';
				D_Config_Packet.Member.u8_current_time_min = atoi(c);
				c[0]=str2[6];
				c[1]=str2[7];
				c[2]='\0';
				D_Config_Packet.Member.u8_current_time_sec	= atoi(c);
				u8_Data_Write_Flag = 1;

				 
			       

			}    
			//v_update_date_time();					//basil
			    
		  	p = strstr(str1,"\"start_time\":\"");
		   if(p)
			{
		   	p=p+sizeof("\"start_time\":\"")-1;
			strncpy(str2,p,strchr(p,'"')-p);
			str2[strchr(p,'"')-p]='\0';
			    c[0]=str2[0];
			    c[1]=str2[1];
			    c[2]='\0';
			    D_Config_Packet.Member.u8_day_start_time_hr = atoi(c);
			    c[0]=str2[3];
			    c[1]=str2[4];
			    c[2]='\0';
			    D_Config_Packet.Member.u8_day_start_time_min = atoi(c);
			     u8_Data_Write_Flag = 1;
			}
			
		
		   p = strstr(str1,"\"end_time\":\"");
		   if(p)
			{
		   	p=p+sizeof("\"end_time\":\"")-1;
			strncpy(str2,p,strchr(p,'"')-p);
			str2[strchr(p,'"')-p]='\0';
			    c[0]=str2[0];
			    c[1]=str2[1];
			    c[2]='\0';
			    D_Config_Packet.Member.u8_day_End_time_hr	= atoi(c);
			    c[0]=str2[3];
			    c[1]=str2[4];
			    c[2]='\0';
			   D_Config_Packet.Member.u8_day_End_time_min	= atoi(c);
			    u8_Data_Write_Flag = 1;
			    
			}
			
			    p = strstr(str1,"\"configuration_version\":\"");
		   if(p)
			{
		   	p=p+sizeof("\"configuration_version\":\"")-1;
			strncpy(str2,p,strchr(p,'"')-p);
			str2[strchr(p,'"')-p]='\0';
			 c[0]=str2[0];
			 c[1]=str2[1];
			 c[2]=str2[2];
			 c[3]='\0';
			 D_Config_Packet.Member.u16_Config_id	= atoi(c);
			  u8_Data_Write_Flag = 1;
			}
			
			p = strstr(str1,"\"scanning_interval\":\"");
		   if(p)
			{
		   	p=p+sizeof("\"scanning_interval\":\"")-1;
			strncpy(str2,p,strchr(p,'"')-p);
			str2[strchr(p,'"')-p]='\0';
		    	c[0]=str2[0];
		    	c[1]=str2[1];
		   	c[2]='\0';
		        D_Config_Packet.Member.u32_scanning_timeout  = atoi(c);
			 u8_Data_Write_Flag = 1;
			}
			
			p = strstr(str1,"\"device_timezone_sign\":\"");
		   if(p)
			{
		   	p=p+sizeof("\"device_timezone_sign\":\"")-1;
			strncpy(str2,p,strchr(p,'"')-p);
			str2[strchr(p,'"')-p]='\0';
			c[0]=str2[0];
			D_Config_Packet.Member.u8_timezone_sign= c[0];
			 u8_Data_Write_Flag = 1;
			}
			
			p = strstr(str1,"\"device_timezone\":\"");
		   if(p)
			{
		   	p=p+sizeof("\"device_timezone\":\"")-1;
			strncpy(str2,p,strchr(p,'"')-p);
			str2[strchr(p,'"')-p]='\0';
			c[0]=str2[0];
			c[1]=str2[1];
			c[2]='\0';
			D_Config_Packet.Member.u16_time_zone_hr = atoi(c);
			c[0]=str2[3];
			c[1]=str2[4];
			c[2]='\0';
			D_Config_Packet.Member.u16_time_zone_min = atoi(c);
		        u8_Data_Write_Flag = 1;
			
			}
			
			
		      
			
		       if(u8_Data_Write_Flag)
			{
				D_Config_Packet.Member.u32_Checksum_Value = u32_checksum_calculate(&D_Config_Packet.All[0], CONFIG_DF_DATA_SIZE - 4);
				Write_Dconfig_Data_Flash();
				update_done_flag = 1;
				
			}
			
	
}
	

void Update_restart_time(void)

{

   char str1[512];
   char str2[50];
   char c[5];
   char *p =&Subscribed_data;
   
   
  
   			strcpy(str1,(char*)Subscribed_data);
 			p = strstr(str1,"\"restart_date\":\"");
		   if(p)
			{
		   	p=p+sizeof("\"restart_date\":\"")-1;
			strncpy(str2,p,strchr(p,'"')-p);
			str2[strchr(p,'"')-p]='\0';
		            c[0] = str2[0];
			    c[1] = str2[1];
			    c[2] = '\0';
			    u8_restart_date	= atoi(c); 
			    c[0]=str2[3];
			    c[1]=str2[4];
			    c[2]='\0';
			    u8_restart_month	= atoi(c);
			    c[0]=str2[6];
			    c[1]=str2[7];
			    c[2]=str2[8];
			    c[3]=str2[9];
			    c[4]='\0';
			   u16_restart_year	 = atoi(c);
			    u8_Data_Write_Flag = 1;
			}
				
				
				
 			p = strstr(str1,"\"restart_time\":\"");
		   	if(p)
			{
			   	p=p+sizeof("\"restart_time\":\"")-1;
				strncpy(str2,p,strchr(p,'"')-p);
				str2[strchr(p,'"')-p]='\0';
				c[0]=str2[0];
				c[1]=str2[1];
				c[2]='\0';
				u8_restart_time_hr = atoi(c);
				c[0]=str2[3];
				c[1]=str2[4];
				c[2]='\0';
				u8_restart_time_min = atoi(c);
				c[0]=str2[6];
				c[1]=str2[7];
				c[2]='\0';
				u8_restart_time_sec	= atoi(c);
				u8_Data_Write_Flag = 1;
			}
			
			
}



/*****************************************************************
function name    : clock function updation


******************************************************************/

char* clk_fun(void)
{
        uint8_t time_buf[40];        //basil
        uint16_t u16_i = 0;          //basil
	//v_RTC_Get_Time();
	//memset(time_buf, 0x00, sizeof(time_buf));
	
        //sprintf((char *)time_buf, "AT+CCLK=\"20%02d-%02d-%02dT-%02d:%02d:%02d+%02d:%02d\"\r\n", u8_system_current_year, u8_system_current_month,u8_system_current_date, u8_system_current_hour, u8_system_current_min,u8_system_current_sec,D_Config_Packet.Member.u16_time_zone_hr,D_Config_Packet.Member.u16_time_zone_min);/*D_Config_Packet.Member.u8_timezone_sign);,/*///D_Config_Packet.Member.u16_time_zone_hr,D_Config_Packet.Member.u16_time_zone_min);
	sprintf((char *)time_buf, "AT+CCLK=\"20%02d-%02d-%02dT-%02d:%02d:%02d%c%02d:%02d\r\n", u8_system_current_year, u8_system_current_month,u8_system_current_date, u8_system_current_hour, u8_system_current_min,u8_system_current_sec,D_Config_Packet.Member.u8_timezone_sign,D_Config_Packet.Member.u16_time_zone_hr,D_Config_Packet.Member.u16_time_zone_min);		
			for(u16_i=9;u16_i<35;u16_i++)
			{
			clock[u16_i-9]=(char *)time_buf[u16_i];
			}
			return clock;
			
}			