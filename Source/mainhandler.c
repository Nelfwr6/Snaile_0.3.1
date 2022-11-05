		/*******************************************************************************

* Module name: mainhandler.c

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

#include "r_cg_macrodriver.h"
#include "USB_Config.h"
#include "Battery_measure.h"
#include "interval_timer.h"
#include "EEPROM.h"
#include "pfdl.h"
#include "pfdl_types.h"
#include <string.h>
#include "RTC.h"
#include "power_down.h"
#include "RFID.h"
#include "mainhandler.h"
#include "gsm_gprs.h"
#include "r_cg_timer.h"
#include "stdio.h"
#include "MQTT.h"
/*******************************************************************************

* Global Variable Section

*******************************************************************************/
uint8_t *DConfig_DF_Ptr = (uint8_t *)DCONFIG_DF_START_ADDRESS, u8_System_Err = 0, u8_RFID_New_Data_Count;
uint16_t u16_DConfig_DF_Write_Position = DCONFIG_DF_START_POSITION;
uint32_t u32_RFID_DF_Write_Position = RFID_DF_START_POSITION;
uint8_t *u8_RFID_DF_Read_Ptr = (uint8_t *)RFID_DF_START_ADDRESS;


//////////////////////////////////////////////////////////////////
uint32_t u32_Cert_DF_Write_Position = CERT_DF_START_POSITION;
uint8_t *u8_Cert_DF_Read_Ptr = (uint8_t *)CERT_DF_START_ADDRESS;

uint32_t u32_CertPvt_DF_Write_Position = CERTPvt_DF_START_POSITION;
uint8_t *u8_CertPvt_DF_Read_Ptr = (uint8_t *)CERTPvt_DF_START_ADDRESS;

//uint32_t *CertPEM_DF_Ptr = (uint32_t *)CERT_DF_START_ADDRESS;
//uint32_t *CertPVT_DF_Ptr = (uint32_t *)CERTPvt_DF_START_ADDRESS;
//////////////////////////////////////////////////////////////////


uint8_t u8_RFID_Current_Packet, u8_SSL_Connection_retry_count;
volatile uint8_t u8_Debug_Key_flag;
uint8_t u8_APP_State = APP_STOP_STATE, u8_RFID_Fail_Retry_Count, u8_PDP_activate_retry_count, u8_GSM_Init_Retry_count, u8_RFID_Send_Retry_Count;
uint32_t u32_current_scanning_timeout = DEFAULT_SCANNING_TIME;
uint32_t u32_RFID_New_Data_Location_Ptr = 0;
uint8_t u8_GSM_Data_Format_Sel = 0;
volatile uint8_t u8_Device_State = DEVICE_IDLE_TIME, u8_mqtt_status = 0x00, u8_reset_flag = 0;

uint8_t u8_debug_mode_check = 0, u8_count = 0;
uint16_t u32_Current_Retry_Timeout  = 0;

uint8_t u8_Wakeup_Hand_Shake_Stauts = DEACTIVATED;
uint8_t u8_previous_System_Err = 0;
//uint16_t u8_data_failure_count = 0;

uint8_t u8_HTTP_response[10], u8_LED_Set_Blink_Count, u8_Debug_LED_Number_Of_Blinks, u8_Debug_LED_Set_Number_Of_Blinks;
uint16_t u16_DEBUG_LED_Blink_Count, u16_DEBUG_LED_Set_Blink_Count;
uint16_t u16_DATA_LED_Blink_Count, u16_PDP_LED_Blink_Count;
uint8_t u8_Force_Start_Handshake_Flag = 0;
uint8_t u8_Force_Initial_RFID_Scan = 0;
uint8_t Registered_Flag = 1;
uint16_t u8_TestRFID[12];

extern uint8_t CCID_num[30];
extern uint8_t IMEI_num[30];
extern uint8_t clock[35];

/***********************************************************************************************************************
* Function Name: u32_checksum_calculate
* Description  : This function used to Write latest fanstate & Mute Condition
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
uint32_t u32_checksum_calculate(uint8_t *u8_data_ptr, uint16_t u16_len)
{
	uint32_t u32_Checksum = 0;
	uint16_t u16_i = 0;
	// Calculate check sum
	for(u16_i=0; u16_i < u16_len;u16_i++)
	{ 	
		u32_Checksum = u32_Checksum + u8_data_ptr[u16_i];
		
	}
	return u32_Checksum;
}

/*****************************************************************************************
* Function name	 	: 	void Reload_configuration(void)
* u8_USB_Status 	: 	USB status
* Created by 		: 	Drisya
* Date created 		: 	29-03-2017
* Description 		: 	
* Notes 		: 	
******************************************************************************************/
void Reload_configuration(void)
{
	volatile uint32_t u32_checksum;
	uint16_t j=0,k=0,i = 0;
	while(DConfig_DF_Ptr != (uint8_t *)(DCONFIG_DF_START_ADDRESS + DCONFIG_DF_ALLOTTED_MEMORY))
	{
		memset(&D_Config_Packet.All[0], 0x00, CONFIG_DF_DATA_SIZE);
		Read_Dconfig_Data_Flash(); 	// Read values from internal data flash
		u32_checksum = u32_checksum_calculate(&D_Config_Packet.All[0], CONFIG_DF_DATA_SIZE - 4);
		if((u32_checksum != D_Config_Packet.Member.u32_Checksum_Value))// || (!u32_checksum))
		{
			if(DConfig_DF_Ptr == (uint8_t *)(DCONFIG_DF_START_ADDRESS))
			{
				D_Config_Packet.Member.u32_scanning_timeout  =    DEFAULT_SCANNING_TIME;// in minutes
				D_Config_Packet.Member.u8_day_start_time_hr	= 9;
				D_Config_Packet.Member.u8_day_start_time_min	= 30;	 
				D_Config_Packet.Member.u8_day_End_time_hr	= 17;
				D_Config_Packet.Member.u8_day_End_time_min	= 30;
				
				D_Config_Packet.Member.u8_current_time_hr	= 9;
				D_Config_Packet.Member.u8_current_time_min	= 27;
				D_Config_Packet.Member.u8_current_time_sec	= 00;
				
				D_Config_Packet.Member.u8_current_date		= 01;
				D_Config_Packet.Member.u8_current_month		= 01;
				D_Config_Packet.Member.u16_current_year		= 20;
				//v_update_date_time();					//basil
				strcpy((char *)D_Config_Packet.Member.u8_password,"123456");
				strcpy((char *)D_Config_Packet.Member.u8_URL,"https://api.snailecloud.com/device/rfid/861075024991465/08935201641400662026");//08935201641400661952
				D_Config_Packet.Member.u16_GMT_hr = 05;
				D_Config_Packet.Member.u16_GMT_min = 30;
				D_Config_Packet.Member.u16_GMT_sec = 00;
				D_Config_Packet.Member.u8_timezone_sign= '+';
				D_Config_Packet.Member.u16_time_zone_hr = 00;
				D_Config_Packet.Member.u16_time_zone_min = 00;
				v_update_date_time();					//basil 23/09/2022
				/****************************************************************/
				D_Config_Packet.Member.u16_Config_id=001;
				D_Config_Packet.Member.u16_Version_id=VERSION;
				D_Config_Packet.Member.Registration_Flag= Not_Registered;
				D_Config_Packet.Member.Provinsion_Flag=NOT_PROVISIONED;
				/****************************************************************/
				strcpy((char *)D_Config_Packet.Member.u8_apn,"postm2m.lu"); //jionet //internet
				D_Config_Packet.Member.u32_Checksum_Value = u32_checksum_calculate(&D_Config_Packet.All[0], CONFIG_DF_DATA_SIZE - 4);
				u16_DConfig_DF_Write_Position = DCONFIG_DF_START_POSITION;
				//Erase_Dconfig_Data_Flash();
				Write_Dconfig_Data_Flash();
				return;
			}
			else
			{
				DConfig_DF_Ptr -= CONFIG_DF_DATA_SIZE;
				Read_Dconfig_Data_Flash();
				return;
			}
		}
		else
		DConfig_DF_Ptr += CONFIG_DF_DATA_SIZE;
	}
}



/***********************************************************************************************************************
* Function Name: Read_Dconfig_Data_Flash
* Description  : This function used to read latest fanstate & Mute Condition
* Arguments    : None
* Return Value : Error 
***********************************************************************************************************************/
void  Read_Dconfig_Data_Flash		(void)
{
	uint32_t i=0;
        uint32_t u32_checksum=0;
	
	DFLCTL_bit.no0 = 1;
	v_Micro_Delay(5);
	for(i = 0;i < CONFIG_DF_DATA_SIZE; i++)
	{
		D_Config_Packet.All[i] = DConfig_DF_Ptr[i];
	}
	DFLCTL_bit.no0 = 0;
	DFLEN = 0;
	
}

/***********************************************************************************************************************
* Function Name: Read_RFID_Data_Flash
* Description  : This function used to read latest fanstate & Mute Condition
* Arguments    : None
* Return Value : Error 
***********************************************************************************************************************/
void  Read_RFID_Data_Flash(void)
{
	uint32_t i=0;
	
	DFLCTL_bit.no0 = 1;
	v_Micro_Delay(5);
	for(i = 0; i < RFID_DF_DATA_SIZE; i++)
		RFID_DF_Data_Packet.All[i] = u8_RFID_DF_Read_Ptr[i];
	DFLCTL_bit.no0 = 0;
	DFLEN = 0;
}


/***********************************************************************************************************************
* Function Name: Read_RFID_Data_Flash
* Description  : This function used to read latest fanstate & Mute Condition
* Arguments    : None
* Return Value : Error 
***********************************************************************************************************************/
void Read_Cert_Data_Flash(void)
{
	uint32_t i=0;
	
	memset(New_PEM_Cert_Buffer, 0x00, sizeof(New_PEM_Cert_Buffer));
	u8_Cert_DF_Read_Ptr = CERT_DF_START_ADDRESS;
	DFLCTL_bit.no0 = 1;
	v_Micro_Delay(5);
	for(i = 0; i < CERT_DF_DATA_SIZE1; i++)
	New_PEM_Cert_Buffer[i] = u8_Cert_DF_Read_Ptr[i];
	DFLCTL_bit.no0 = 0;
	DFLEN = 0;

}

void Read_Cert_Data_Flash2(void)
{
	
	uint32_t i=0;
	u8_CertPvt_DF_Read_Ptr = CERTPvt_DF_START_ADDRESS;
	memset(New_Pvt_Cert_Buffer, 0x00, sizeof(New_Pvt_Cert_Buffer));
	DFLCTL_bit.no0 = 1;
	v_Micro_Delay(5);
	for(i = 0; i < CERTPvt_DF_DATA_SIZE2; i++)
		New_Pvt_Cert_Buffer[i] = u8_CertPvt_DF_Read_Ptr[i];
	DFLCTL_bit.no0 = 0;
	DFLEN = 0;
}

/***********************************************************************************************************************
* Function Name: Read_RFID_Data_Flash
* Description  : This function used to read latest fanstate & Mute Condition
* Arguments    : None
* Return Value : Error 
***********************************************************************************************************************/
uint8_t v_GSM_Data_Format_Send(void)
{
	uint8_t u8_i = 0, u8_j = 0, u8_data[3], u8_temp ,u8_Status,mqtt_pub_retry_cont=0;
	char publish_message[70],rfid_sendbuffer_2[512] = {0};
	uint8_t Rfid_Data_Length = 0;
	char *s = NULL;
	
	if(u8_GSM_Data_Format_Sel == GSM_SEL_RFID_DATA)
	{
		if(u8_RFID_New_Data_Count)
		{
			u8_RFID_DF_Read_Ptr = (uint8_t *)u32_RFID_New_Data_Location_Ptr;
			
			/***********************************************************************/
			
			memset(rfid_sendbuffer_2, 0, sizeof(rfid_sendbuffer_2));
			s =  rfid_sendbuffer_2;
			strcat(s,"{\"message\": \"tag-read\",\"imei\": \"");	
			//s += strlen(s);
			strcat(s,(char*)IMEI_num);
			//s += strlen(s);
			strcat(s,"\",\"tag_ids\": [\"");						
			//s += strlen(s);
		
			for(u8_i = 0;u8_i < u8_RFID_New_Data_Count; u8_i++)
			{
				if(u8_i > 0)
				{
					strcat(s,"\",\"");
				}
				Read_RFID_Data_Flash();
				if(RFID_DF_Data_Packet.Member.u32_Checksum_Value == u32_checksum_calculate(&RFID_DF_Data_Packet.All[0], 12))
				{
					
				for(u8_j = 0;u8_j < 12; u8_j++)
					{
						u8_temp = RFID_DF_Data_Packet.All[u8_j];
						if((u8_temp >> 4) <= 9)
						u8_data[0] = (u8_temp >> 4) + 0x30;
						else
						u8_data[0] = (u8_temp >> 4) + 0x57;
						if((u8_temp & 0x0F) <= 9)
						u8_data[1] = (u8_temp & 0x0F) + 0x30;
						else
						u8_data[1] = (u8_temp & 0x0F) + 0x57;
						u8_data[2] = 0;
						strcat(s, (char *)u8_data);
						s += strlen(s);
					}
					
					u8_RFID_DF_Read_Ptr += RFID_DF_DATA_SIZE;
				}
				
				else
				{
					break;
				}
			}
			strcat(s,"\"],");					
			s += strlen(s);
			strcat(s,"\"timestamp\": \"");
			s += strlen(s);
			//clk_fun();
			strcat(s,(char*)clk_fun());
			s += strlen(s);
			strcat(s,"\"}");
			
			Rfid_Data_Length = strlen(rfid_sendbuffer_2);
			
			sprintf((char*)publish_message,"AT+QMTPUBEX=1,1,1,0,\"rfid/from-device/%s\",%u\r\n",(char*)IMEI_num,Rfid_Data_Length);
			
			v_uart_str_send(publish_message, GSM_GPRS_CHANNEL);
			v_delay_ms(500);
			v_uart_str_send(rfid_sendbuffer_2, GSM_GPRS_CHANNEL);
			v_delay_ms(500);
			u8_Status = u8_GSM_GPRS_reception_Handler(5000);
			v_delay_ms(500);
			u8_Status = u8_GSM_GPRS_reception_Handler(5000);
			u8_Status = u8_GSM_GPRS_reception_Handler(5000);
			v_delay_ms(500);
			u8_Status = u8_GSM_GPRS_reception_Handler(5000);
			
			if(MQTT_PUB_RETRY_FLAG == 1 || MQTT_PUB_RETRY_FLAG ==2)
			{
				for (mqtt_pub_retry_cont=0; mqtt_pub_retry_cont < 3; mqtt_pub_retry_cont++)
				{
				u8_Status = u8_GSM_GPRS_reception_Handler(4000);
				v_delay_ms(500);
				}
				
			}
					
		}
	}

	
	else if(u8_GSM_Data_Format_Sel == REGISTRATION_DATA)
	{
		Registration_request();
		
	}
	
	else if(u8_GSM_Data_Format_Sel == GSM_SEL_HANDSHAKE_DATA)
	{
		u8_Status = Publish_Handshake();

		/*sprintf((char *)u8_GSM_Data_String, "?bcl=%02d&range=%02d",u16_battery_charge, u32_Network_Strength);
		
		v_uart_str_send((char *)u8_GSM_Data_String,GSM_GPRS_CHANNEL);*/
	}
	return u8_Status;
}
/*****************************************************************************************
* Function name	 	: 	void v_usb_mode_handler(void)
* return 		: 	none
* arg 			: 	none
* Created by 		: 	Aarathy Ramesh
* Date created 		: 	22-07-2017
* Description 		: 	USB Mode Handler
* Notes 		: 	
******************************************************************************************/
void v_usb_mode_handler(void)
{
	clear_usb_connection_status(); 
	v_usb_configuration();
}
/*****************************************************************************************
* Function name	 	: 	uint8_t u8_byte_array_compare(uint8_t *u8_ptr1, uint8_t *u8_ptr2)
* return 		: 	none
* arg 			: 	none
* Created by 		: 	Aarathy Ramesh
* Date created 		: 	22-07-2017
* Description 		: 	Normal Mode Handler
* Notes 		: 	
******************************************************************************************/
uint8_t u8_byte_array_compare(uint8_t *u8_ptr1, uint8_t *u8_ptr2)
{
	uint8_t u8_i = 0;
	for(;u8_i < 12; u8_i++ )
	{
		if(u8_ptr1[u8_i] != u8_ptr2[u8_i])
		return COMPARE_FAIL;
		
	}
	return COMPARE_SUCCESS;
}


/*****************************************************************************************
* Function name	 	: 	uint8_t v_zero_adding_for_low_length_rfid(void)
* return 		: 	none
* arg 			: 	none
* Created by 		: 	Jomon
* Date created 		: 	22-07-2017
* Description 		: 	removing the rf 
* Notes 		: 	
******************************************************************************************/
uint8_t v_zero_adding_for_low_length_rfid(void)
{
	uint16_t u16_RFID_len = 0;
	uint16_t i;
	
	u16_RFID_len = (u8_RFID_Pool[u8_RFID_Current_Packet - 1].All[3] << 8);
	u16_RFID_len |= u8_RFID_Pool[u8_RFID_Current_Packet - 1].All[4];
	
	
	if( (u16_RFID_len + 4) < (RFID_MAX_RX_BUFFER_COUNT - 3))
	{
		for( i = 0; i < (u16_RFID_len - 3); i++ )
		{
			u8_RFID_Pool[u8_RFID_Current_Packet - 1].All[(RFID_MAX_RX_BUFFER_COUNT - 3) - i] = u8_RFID_Pool[u8_RFID_Current_Packet - 1].All[(u16_RFID_len + 4) - i];
			u8_RFID_Pool[u8_RFID_Current_Packet - 1].All[(u16_RFID_len + 4) - i] = 0x00;
		}
		//u8_RFID_Pool[u8_RFID_Current_Packet - 1].All[u16_RFID_len + 3] = 0x00;	
		//u8_RFID_Pool[u8_RFID_Current_Packet - 1].All[u16_RFID_len + 4] = 0x00;
	}
	return 0;
}




/*****************************************************************************************
* Function name	 	: 	void v_normal_mode_handler(void)
* return 		: 	none
* arg 			: 	none
* Created by 		: 	Aarathy Ramesh
* Date created 		: 	22-07-2017
* Description 		: 	Normal Mode Handler
* Notes 		: 	
******************************************************************************************/
void v_normal_mode_handler(void)
{
	uint8_t u8_status, u8_i = 0;
	uint16_t u16_i;
	uint32_t u32_checksum;
	uint8_t u8_stop_flag;
	char uart_sendbuffer[215];
	char publish_message[45];
	switch(u8_APP_State)
	{

		case APP_STOP_STATE:
		u16_battery_charge_count = 0;
		u8_get_battery_status = 0;
		
		if(u8_reset_flag == 1)
		{
			R_RTC_Set_ConstPeriodInterruptOff();
			R_RTC_Stop();
			Reset_MCU();
		}
		
		if( (USB_STATUS == 1) && (get_usb_connection_status() == USB_DETECTED) )
	        {   
	        	clear_usb_connection_status();
			u8_mode_change_flag = 1;
			u8_stop_flag = 0;
	        }
		
		else if((u8_Device_State == DEVICE_START_TIME) || (u8_Debug_Key_flag) || (u8_Force_Start_Handshake_Flag == 1) )
		{       
			if(u8_Debug_Key_flag)
			{

				DEBUG_LED_ON();
				////u8_Device_State = DEVICE_IDLE_TIME;
				u8_previous_System_Err = u8_System_Err;

			}
			if( (u8_Device_State == DEVICE_START_TIME) ||  (u8_Force_Start_Handshake_Flag == 1) )
			{
				u8_Force_Start_Handshake_Flag = 0;
				u8_System_Err = 0;
				u8_previous_System_Err = 0;
				Erase_RFID_Data_Flash();
				u32_RFID_New_Data_Location_Ptr = 0;
				u8_RFID_New_Data_Count = 0;
				u32_Current_Retry_Timeout = 0;
				v_clear_RFID_data_buf();
				u8_Device_State = DEVICE_WORKING_TIME;
				u8_Wakeup_Hand_Shake_Stauts = ACTIVATED;
			}
			u8_stop_flag = 0;
			u8_debug_mode_check = u8_Debug_Key_flag;
			u8_Debug_Key_flag = 0;
			u8_GSM_Data_Format_Sel = GSM_SEL_HANDSHAKE_DATA;
			u8_APP_State = APP_POWER_UP;
		}
		else if(u8_Device_State == DEVICE_WORKING_TIME)
		{

			if( (u32_current_scanning_timeout >= D_Config_Packet.Member.u32_scanning_timeout)|| (u8_Force_Initial_RFID_Scan == 1) )    //  basil
			{
				if( (u8_Wakeup_Hand_Shake_Stauts == DEACTIVATED) || (u8_Force_Initial_RFID_Scan == 1) )
				{	
					u8_Force_Initial_RFID_Scan = 0;
					if(!u8_System_Err)
					{
				 		u32_RFID_New_Data_Location_Ptr = 0;
				        	u8_RFID_New_Data_Count = 0;
				   	}
				    	u8_System_Err = 0;
				    	u8_stop_flag = 0;
				    	u32_Current_Retry_Timeout = 0;
				    
				    	u8_APP_State = APP_POWER_UP;
				    	u8_GSM_Data_Format_Sel = GSM_SEL_RFID_DATA;
				}
				else if( (u8_Wakeup_Hand_Shake_Stauts == ACTIVATED) )
				{
					u8_stop_flag = 0;
					u8_APP_State = APP_POWER_UP;
				        u8_GSM_Data_Format_Sel = GSM_SEL_HANDSHAKE_DATA;
				}

				if(u32_current_scanning_timeout >= D_Config_Packet.Member.u32_scanning_timeout)
					u32_current_scanning_timeout = (u32_current_scanning_timeout % D_Config_Packet.Member.u32_scanning_timeout);
				   
			}
			else if( (u32_Current_Retry_Timeout >= MAX_DATA_RETRY_TIMEOUT) && (u8_System_Err > 0))
			{       
				u8_stop_flag = 0;
				u32_Current_Retry_Timeout = 0;
				if(u8_Wakeup_Hand_Shake_Stauts == ACTIVATED)
				{
					u8_APP_State = APP_POWER_UP;
				        u8_GSM_Data_Format_Sel = GSM_SEL_HANDSHAKE_DATA;	
				}
				else if(u8_RFID_New_Data_Count > 0)
				{
					u8_APP_State = APP_POWER_UP;
					u8_GSM_Data_Format_Sel = GSM_SEL_RFID_DATA;
				}
			}
			else 
				u8_stop_flag =1;
		
		}
		/*****************************************hr and min setting start*****************************/
		else if(u8_system_current_year == 20 && u8_system_current_month == 02   )
		{
			v_update_date_time();
			
		}
			
		/*****************************************hr and min setting end*****************************/	
		else
			u8_stop_flag = 1;
		if(u8_stop_flag)
		{
			Select_Power_Mode(STOP_MODE);
		}
		break;

		case APP_SEND_RFID_COMMAND:
		if(u8_RFID_Send_Retry_Count == 0)
		{
			u8_RFID_Set_Query_Command_Send();
			u8_RFID_Dense_Reader_Command_Send();
		}
		else if( u8_RFID_Send_Retry_Count == 5)
		{
			u8_RFID_High_Sensitivity_Command_Send();
		}
		else
		{
			NOP();
		}
		//////////////////////////////////////
		//memset rfid pool rfid data received
		//clear 
		
		v_clear_RFID_data_buf();
		//////////////////////////////////////
		u8_RFID_Current_Packet = u8_RFID_Poling_Command_Send();
		if(u8_RFID_Current_Packet)
		{
			u8_RFID_DF_Read_Ptr = (uint8_t *)RFID_DF_START_ADDRESS;
			u8_RFID_Fail_Retry_Count = 0;
			u8_APP_State = APP_RFID_DATA_SCAN;
			u8_RFID_Send_Retry_Count++;
		}
		else
		{
			u8_RFID_Fail_Retry_Count++;
			if(u8_RFID_Fail_Retry_Count >= RFID_FAIL_RETRY_COUNT)
			{
				u8_System_Err |= RFID_ERR;
				u8_APP_State = APP_POWER_DOWN;
			}
		}
		break;

		case APP_POWER_UP:
		R_TAU0_Create();
		R_ADC_Create();
		R_ADC_Set_OperationOn();
		u16_battery_charge_count = 0;
		R_SAU0_Create();
		//R_SAU1_Create();
		R_TAU0_Channel0_Start();
		if(u8_GSM_Data_Format_Sel == GSM_SEL_RFID_DATA)
		{
			if( (u8_System_Err == 0) || ( (u8_System_Err != 0)&&((u8_System_Err & RFID_ERR) != 0) ) )
			{
				v_RFID_Turn_ON();
				v_delay_ms(100);
				R_UART0_Start();           //RFID
				R_UART0_Receive(&rfid_rx_data, 1);
				u8_APP_State = APP_SEND_RFID_COMMAND;
			}
			else
			{
				u8_APP_State = APP_GSM_POWER_ON;
			}
		}
		else
		{
			
			while(!u8_get_battery_status)
			{
				NOP();
			}
			u8_APP_State = APP_GSM_POWER_ON;
		}
		break;


		case APP_RFID_DATA_SCAN:
		if(!u8_RFID_Current_Packet)
		{
			u8_RFID_Current_Packet = u8_RFID_Check_Packet_Received(1000);
			
		}
		
		if(u8_RFID_Current_Packet)
		{
			u8_status = u8_RFID_Reception_Handler(u8_RFID_Current_Packet - 1);
			if(u8_status == RFID_CARD_DETECTD)
			{
				v_zero_adding_for_low_length_rfid();
				u8_RFID_DF_Read_Ptr = (uint8_t *)RFID_DF_START_ADDRESS;
				while(1)
				{
					Read_RFID_Data_Flash();
					u32_checksum = u32_checksum_calculate(&RFID_DF_Data_Packet.All[0], RFID_DF_DATA_SIZE - 4);
					if((u32_checksum == RFID_DF_Data_Packet.Member.u32_Checksum_Value) && (u32_checksum))
					{
						u8_RFID_DF_Read_Ptr += RFID_DF_DATA_SIZE;
						u8_status = u8_byte_array_compare((uint8_t *)&u8_RFID_Pool[u8_RFID_Current_Packet - 1].All[8], (uint8_t *)&RFID_DF_Data_Packet.All[0]);
						if(!u8_status)
						{
							break;
						}
					}
					else
					{
						u32_RFID_DF_Write_Position = (uint32_t)u8_RFID_DF_Read_Ptr; 
						u32_RFID_DF_Write_Position = u32_RFID_DF_Write_Position - DCONFIG_DF_START_ADDRESS;
						if(!u8_RFID_New_Data_Count)
						u32_RFID_New_Data_Location_Ptr = (uint32_t)u8_RFID_DF_Read_Ptr;
						u8_RFID_New_Data_Count++;
						for(u8_i = 0; u8_i < 12; u8_i++)
						{
							RFID_DF_Data_Packet.All[u8_i] = u8_RFID_Pool[u8_RFID_Current_Packet - 1].All[u8_i + 8];							
						}
						RFID_DF_Data_Packet.Member.u32_Checksum_Value = u32_checksum_calculate(&RFID_DF_Data_Packet.All[0], 12);
						v_Write_RFID_DF_data();
						break;
					}
					if((RFID_DF_ALLOTTED_MEMORY + RFID_DF_START_POSITION) == u32_RFID_DF_Write_Position)
					break;
				}
			}
			u8_RFID_Current_Packet = 0;
		}
		else 
		{
			if(u8_RFID_Send_Retry_Count < RFID_SEND_RETRY_COUNT)
			{
				u8_APP_State = APP_SEND_RFID_COMMAND;
				
			}
			
			else if(u8_RFID_Send_Retry_Count >= RFID_SEND_RETRY_COUNT)
			{
				u8_RFID_Send_Retry_Count = 0;
				if(!u8_RFID_New_Data_Count)
					u8_APP_State = APP_POWER_DOWN;
				else
					u8_APP_State = APP_GSM_POWER_ON;
			}
		}
		
		break;



		case APP_GSM_POWER_ON:
		GSM_DATA_STATUS_LED = LED_OFF;
		u8_LED_Set_Blink_Count = 100;
		u16_PDP_LED_Blink_Count = 1;
		u16_DATA_LED_Blink_Count = 0;
		u16_DEBUG_LED_Blink_Count = 0;
		R_UART1_Create();
		R_UART1_Start(); 
		R_UART1_Receive(&gsm_rx_data,1);
		v_Gsm_Hardware_Init();
		u8_System_Err &= ~GSM_FAILED_ERR;
		u8_System_Err &= ~GSM_PDP_ACTIVATION_FAILED_ERR;
		u8_System_Err &= ~SSL_CONNECTION_FAILED_ERR;
		u8_System_Err &= ~GSM_DATA_SEND_FAILED_ERR;
		u8_System_Err &= ~GSM_DATA_RECV_FAILED_ERR;
		u8_GSM_Init_Retry_count = 0;
		u8_PDP_activate_retry_count = 0;
		u8_SSL_Connection_retry_count = 0;
		u8_APP_State = APP_GSM_INITIALISE;
		break;


		case APP_GSM_INITIALISE:
		u8_status = u8_GPRS_Init();
		if(u8_status == GSM_SUCCESS)
		{
			u16_DEBUG_LED_Blink_Count = 0;
			u16_DEBUG_LED_Set_Blink_Count = 0;
			u8_Debug_LED_Set_Number_Of_Blinks = 0;
			u8_GSM_Init_Retry_count = 0;
			if(u8_debug_mode_check == 1)
			{
				DEBUG_LED_ON();	
			}
			else
			{
				DEBUG_LED_OFF();	
			}

			u8_APP_State = APP_TCPIP_INIT;	
			u8_System_Err &= ~GSM_FAILED_ERR;
				
		}	
		else
		{	
			if(!u8_GSM_Init_Retry_count)
			{
				if(u8_status == GSM_GPRS_TMO_ERR) //time out error
				{
			        	u16_DEBUG_LED_Set_Blink_Count = 100;
					u8_Debug_LED_Set_Number_Of_Blinks  = 1;
					u16_DEBUG_LED_Blink_Count = 1;
					
				}
			        else if(u8_SIM_Insert == 0) //sim insert fail indication
			        {
					GSM_DEBUG_LED = 1;  //DEBUG LED OFF
				   
					u8_LED_Set_Blink_Count = 0;
				 	u16_PDP_LED_Blink_Count = 0;
				 	u16_DATA_LED_Blink_Count = 0;
					GSM_PDP_STATUS_LED = LED_ON;
					GSM_DATA_STATUS_LED = LED_OFF;
				   
				}
				else if( (u8_Network_Status != 1) && (u8_Network_Status != 5) ) //network connection error indication
				{
					GSM_DEBUG_LED = 1;  //DEBUG LED OFF
				    
					u8_LED_Set_Blink_Count = 0;
					u16_PDP_LED_Blink_Count = 0;
					u16_DATA_LED_Blink_Count = 0;
				 	GSM_PDP_STATUS_LED = LED_OFF;
				 	GSM_DATA_STATUS_LED = LED_ON;
				    
				}
			}
			u8_GSM_Init_Retry_count ++; 	
			// increment retry count
			if(u8_GSM_Init_Retry_count >=  GSM_INIT_RETRY_COUNT)
			{
				//Reset_MCU();
				u32_Current_Retry_Timeout = 0;
				u8_System_Err |= GSM_FAILED_ERR;
				u8_APP_State = APP_POWER_DOWN;
				v_delay_ms(1000);
			}
		}
		break;


		case APP_TCPIP_INIT:
		u8_status = Rails_registration();
		u8_status = u8_Get_Network_Signal_Strength();
		v_delay_ms(2000);
		//u8_status = u8_MQTT_Activate();
		//u8_status = u8_GSM_TCP_Activate((uint8_t *)&D_Config_Packet.Member.u8_apn[0],(uint8_t *)"",(uint8_t *)"");
		if(u8_status == GSM_SUCCESS)
		{
			//u8_System_Err &= ~GSM_PDP_ACTIVATION_FAILED_ERR;
		    	u8_APP_State = APP_TCPIP_NET_CONNECT;
			//u8_PDP_activate_retry_count = 0;
			u8_LED_Set_Blink_Count = 100;
			u16_PDP_LED_Blink_Count = 0;
			u16_DATA_LED_Blink_Count = 1;	
			GSM_PDP_STATUS_LED = LED_OFF;
		}
		else
		{
			u8_LED_Set_Blink_Count = 100;
			u16_PDP_LED_Blink_Count = 1;
			u16_DATA_LED_Blink_Count = 0;	
			GSM_DATA_STATUS_LED = LED_OFF;
			//u8_PDP_activate_retry_count++;
			/*if(u8_PDP_activate_retry_count >= 3)
			{
				u32_Current_Retry_Timeout = 0;
				u8_System_Err |= GSM_PDP_ACTIVATION_FAILED_ERR;
				u8_APP_State = APP_POWER_DOWN;
			}*/
		}
		break;
		
		case APP_TCPIP_NET_CONNECT:
		//u8_status = u8_Get_Network_Signal_Strength();
		//v_delay_ms(2000);
		
		if(u8_status == GSM_SUCCESS)
		{
			u8_status = u8_Wait_For_mqtt_Connection();
			v_delay_ms(1000);
			Subscribe_to_Rails_Topic();
			//if(u8_status == MQTT_SUB_SUCC_FLAG)
			//u8_status = u8_Get_Network_Signal_Strength();
			v_delay_ms(2000);
			//u8_status = u8_HTTP_Open_Connection((uint8_t *)&D_Config_Packet.Member.u8_URL[0], (uint8_t *)GSM_CONNECT_PORT);
			if(u8_status == GSM_SUCCESS)
			{
				
				//u8_SSL_Connection_retry_count = 0;
				//u8_System_Err &= ~SSL_CONNECTION_FAILED_ERR;
            			u8_APP_State = APP_TCPIP_SEND_DATA;
				//u8_SSL_Connection_retry_count = 0;
				
			}
			else if (u8_status == GSM_FAIL )// || u8_status == 0)
			{
				u8_count ++;
				if(u8_count >8)
				{
					u8_count = 0;
					u8_APP_State = APP_TCPIP_SEND_DATA;
				}
				
				v_Gsm_Power_Down();
				v_delay_ms(1000);
				u8_APP_State = APP_GSM_POWER_ON;//APP_TCPIP_INIT;
			}
			
			else
			{
				u8_LED_Set_Blink_Count = 100;
				u16_PDP_LED_Blink_Count = 0;
				u16_DATA_LED_Blink_Count = 1;	
				GSM_PDP_STATUS_LED = LED_OFF;
				
				
				u8_APP_State = APP_TCPIP_SEND_DATA;
				//u8_SSL_Connection_retry_count++;
				if(u8_SSL_Connection_retry_count > 5)
				{
		        	u32_Current_Retry_Timeout = 0;
				//u8_SSL_Connection_retry_count = 0;
				u8_APP_State = APP_POWER_DOWN;
				//u8_System_Err |= SSL_CONNECTION_FAILED_ERR;
				}
			}
		}
				else if(u8_SSL_Connection_retry_count == 5)
				{
					u8_LED_Set_Blink_Count = 100;
					u16_PDP_LED_Blink_Count = 1;
					u16_DATA_LED_Blink_Count = 0;	
					GSM_DATA_STATUS_LED = LED_OFF;
					v_Gsm_Power_Down();
					v_delay_ms(1000);
					u8_APP_State = APP_GSM_POWER_ON;//APP_TCPIP_INIT;
					//u8_System_Err |= SSL_CONNECTION_FAILED_ERR;
				}
		

            	break;

        	case APP_TCPIP_SEND_DATA:
//		if(D_Config_Packet.Member.Registration_Flag == Not_Registered)
//			{
//				;
//			}
	
//			if(D_Config_Packet.Member.Registration_Flag == Not_Registered)
//			{
//			u8_GSM_Data_Format_Sel = REGISTRATION_DATA;
//			v_GSM_Data_Format_Send();
//			}
	
//			//if(u8_status == GSM_SUCCESS)
//			else
//			{
			//u8_GSM_Data_Format_Sel = GSM_SEL_HANDSHAKE_DATA;
			u8_status = v_GSM_Data_Format_Send();
		        if(MQTT_PUB_FAIL_FLAG == 1 || MQTT_PUB_SUCC_FLAG1 != 1)
			{
				MQTT_PUB_SUCC_FLAG1 = 0;
				MQTT_PUB_FAIL_FLAG = 0;
				MQTT_PUB_RETRY_FLAG = 0;
				v_Gsm_Power_Down();
				v_delay_ms(1000);
				u8_APP_State = APP_GSM_POWER_ON;
				break;
			}
			
//			}
			//u32_Current_Retry_Timeout = 0;
			MQTT_PUB_SUCC_FLAG1 = 0;
			u8_status = u8_GSM_GPRS_reception_Handler(1000);
			if(u8_status == MQTT_PUB_SUCC_FLAG)
			{
				u8_mqtt_status = MQTT_SUCCUSS;
			}
			u8_status = u8_GSM_GPRS_reception_Handler(1000);
			if(u8_status == MQTT_PUB_SUCC_FLAG)
			{
				u8_mqtt_status = MQTT_SUCCUSS;
			}
			u8_status = u8_GSM_GPRS_reception_Handler(1000);
			if(u8_status == MQTT_PUB_SUCC_FLAG)
			{
				u8_mqtt_status = MQTT_SUCCUSS;
			}
			u8_status = u8_GSM_GPRS_reception_Handler(1000);
			if(u8_status == MQTT_PUB_SUCC_FLAG)
			{
				u8_mqtt_status = MQTT_SUCCUSS;
			}
			u8_status = u8_GSM_GPRS_reception_Handler(1000);
			if(u8_status == MQTT_PUB_SUCC_FLAG)
			{
				u8_mqtt_status = MQTT_SUCCUSS;
			}
			//u8_status = MQTT_Reception_Handler(Subscribed_data);
			if(u8_GSM_Data_Format_Sel == GSM_SEL_RFID_DATA)
			{
				if(u8_mqtt_status == MQTT_SUCCUSS )
				{
					u8_System_Err &= ~GSM_DATA_SEND_FAILED_ERR;
					//u8_APP_State = APP_POWER_DOWN;
				}
				else
				{
					
					u32_Current_Retry_Timeout = 0;
					u8_System_Err |= GSM_DATA_SEND_FAILED_ERR;
					u8_APP_State = APP_GSM_LOOP;
				}
			}
			u8_mqtt_status = 0x00;
			u8_APP_State = APP_GSM_LOOP;
			
			break;
			
			
		
		case APP_GSM_LOOP:
		u8_status = u8_GSM_GPRS_reception_Handler(30000);
//		MQTT_Reception_Handler(Subscribed_data);
		
		u8_APP_State = APP_POWER_DOWN;
		
		break;
		
		/*if(u8_status == GSM_TCP_DATA_RECEIVED)
		{			
			if(u8_Gsm_Rx_Status_Start_Counter == (GSM_GPRS_MAX_PACKET_COUNT - 1))
				u8_i = 0;
		 	else
				u8_i = u8_Gsm_Rx_Status_Start_Counter + 1;
				
			for(u16_i = 14; vu8_Gsm_Rx_Buffer[u8_i][u16_i] != 0x0d; u16_i++)
			{
				if(u16_i == GSM_GPRS_RX_BUFFER_SIZE)
				break;
				u8_HTTP_response[u16_i - 14] = vu8_Gsm_Rx_Buffer[u8_i][u16_i];
			}
			u8_HTTP_response[u16_i - 14] = 0;
			if(strcmp((char *)u8_HTTP_response, "204") == 0)
			{
				u16_DATA_LED_Blink_Count = 0;
			  	u8_System_Err &= ~GSM_DATA_RECV_FAILED_ERR;
				GSM_PDP_STATUS_LED = LED_OFF;
				GSM_DATA_STATUS_LED = LED_ON;
			}
			if(strcmp((char *)u8_HTTP_response, "201") == 0)
			{
				u16_DATA_LED_Blink_Count = 0;
			  	u8_System_Err &= ~GSM_DATA_RECV_FAILED_ERR;
				GSM_PDP_STATUS_LED = LED_OFF;
				GSM_DATA_STATUS_LED = LED_OFF;
				v_delay_ms(200);
				
				u16_DATA_LED_Blink_Count = 3;
				GSM_DATA_STATUS_LED = LED_ON;
				while(u16_DATA_LED_Blink_Count>0)
				{
					NOP();
				}
				D_Config_Packet.Member.u32_Checksum_Value = u32_checksum_calculate(&D_Config_Packet.All[0], CONFIG_DF_DATA_SIZE - 4);
				u16_DConfig_DF_Write_Position = DCONFIG_DF_START_POSITION;
				Write_Dconfig_Data_Flash();
				
			}
			if(strcmp((char *)u8_HTTP_response, "409") == 0)
			{
				u16_DATA_LED_Blink_Count = 0;
			  	u8_System_Err &= ~GSM_DATA_RECV_FAILED_ERR;
				GSM_PDP_STATUS_LED = LED_OFF;
				GSM_DATA_STATUS_LED = LED_OFF;
				v_delay_ms(200);
				
				u16_DATA_LED_Blink_Count = 3;
				GSM_PDP_STATUS_LED = LED_ON;
				while(u16_DATA_LED_Blink_Count>0)
				{
					NOP();
				}
				D_Config_Packet.Member.u32_Checksum_Value = u32_checksum_calculate(&D_Config_Packet.All[0], CONFIG_DF_DATA_SIZE - 4);
				u16_DConfig_DF_Write_Position = DCONFIG_DF_START_POSITION;
				Write_Dconfig_Data_Flash();

			}
			else
			{
				u8_System_Err |= GSM_DATA_RECV_FAILED_ERR;
			}
		}
		else
		{
			u8_System_Err |= GSM_DATA_RECV_FAILED_ERR;
			u32_Current_Retry_Timeout = 0;
		}
		u8_APP_State = APP_POWER_DOWN;
		
		break;
		*/
		
		case APP_POWER_DOWN:
		        if( (u8_previous_System_Err > 0) && (u8_debug_mode_check == 1) ) //resetting error status after debug key
			{
			   u8_System_Err = u8_previous_System_Err;
			   u8_previous_System_Err = 0;
			} 
			
		        if( (u8_System_Err == 0) && (u8_debug_mode_check == 0) )
			{
			   u32_Current_Retry_Timeout = 0;
			   u32_RFID_New_Data_Location_Ptr = 0;
			   u8_RFID_New_Data_Count = 0;
			   if(u8_Wakeup_Hand_Shake_Stauts == ACTIVATED)
			   {
				u8_Wakeup_Hand_Shake_Stauts = DEACTIVATED; 
				u8_Force_Initial_RFID_Scan = 1; 
			   }
			}
			    
			R_ADC_Set_OperationOff();
			R_UART0_Stop();           //RFID
			v_Gsm_Power_Down();
			v_RFID_Turn_OFF();
			GSM_PDP_STATUS_LED = LED_OFF;
			GSM_DATA_STATUS_LED = LED_OFF;
			DEBUG_LED_OFF();
			R_TAU0_Channel0_Stop();
			u16_DEBUG_LED_Set_Blink_Count = 0;
			u8_Debug_LED_Set_Number_Of_Blinks  = 0;
			u16_DEBUG_LED_Blink_Count = 0;
			u16_DATA_LED_Blink_Count = 0;
			u8_LED_Set_Blink_Count = 0;
			u16_PDP_LED_Blink_Count = 0;
			u16_battery_charge_count = 0;
			u8_get_battery_status = 0;
			R_KEY_Start();
			u8_debug_mode_check = 0;
			u8_APP_State = APP_STOP_STATE;
		break;
		
	}
}

/*****************************************************************************************
* Function name	 	: 	void v_rtc_check_polling(void)
* return 		: 	none
* arg 			: 	none
* Created by 		: 	Aarathy Ramesh
* Date created 		: 	22-07-2017
* Description 		: 	Normal Mode Handler
* Notes 		: 	
******************************************************************************************/
void v_rtc_check_polling(void)
{
//	Read_Dconfig_Data_Flash();
	v_RTC_Get_Time();
	

	u8_Device_State = u8_Check_Working_Time();
	
	if(u8_Device_State != DEVICE_IDLE_TIME)
	{
		if(u32_current_scanning_timeout < D_Config_Packet.Member.u32_scanning_timeout)
		{
			u32_current_scanning_timeout++;
		}
	}
	if(u8_System_Err > 0)
	{
	    if(u32_Current_Retry_Timeout < MAX_DATA_RETRY_TIMEOUT )	
		{
	    		u32_Current_Retry_Timeout ++;
		}
	}
	if(u8_system_current_date == u8_restart_date &&\
	u8_system_current_month == u8_restart_month &&\
	u8_system_current_year == u16_restart_year&&\
	u8_system_current_hour == u8_restart_time_hr&&\
	u8_system_current_min >= u8_restart_time_min)
	{
		u8_reset_flag = 1;
		//Reset_MCU();
	}
	
}
/*****************************************************************************************
* Function name	 	: 	uint8_t u8_Check_Working_Time(void)
* return 		: 	Status
* arg 			: 	none
* Created by 		: 	Aarathy Ramesh
* Date created 		: 	22-07-2017
* Description 		: 	Normal Mode Handler
* Notes 		: 	
******************************************************************************************/
uint8_t u8_Check_Working_Time(void)
{
	uint8_t u8_status;
	
	if((u8_system_current_hour == D_Config_Packet.Member.u8_day_start_time_hr) && (u8_system_current_min ==  D_Config_Packet.Member.u8_day_start_time_min))
	{
		u8_status = DEVICE_START_TIME;
		u32_current_scanning_timeout = 0;
		u8_Force_Start_Handshake_Flag = 1;
		 
	}
	else
	{
		if(u8_system_current_hour > D_Config_Packet.Member.u8_day_start_time_hr)
		{
			if(u8_system_current_hour < D_Config_Packet.Member.u8_day_End_time_hr)
			{
				u8_status = DEVICE_WORKING_TIME;
			}
			else if(u8_system_current_hour == D_Config_Packet.Member.u8_day_End_time_hr)
			{
				if(u8_system_current_min <=  D_Config_Packet.Member.u8_day_End_time_min)
					u8_status = DEVICE_WORKING_TIME;
				else 
					u8_status = DEVICE_IDLE_TIME;
			}
			else
				u8_status = DEVICE_IDLE_TIME;
		}
		else if(u8_system_current_hour == D_Config_Packet.Member.u8_day_start_time_hr)
		{
			if(u8_system_current_hour == D_Config_Packet.Member.u8_day_End_time_hr)
			{
				if( (u8_system_current_min >=  D_Config_Packet.Member.u8_day_start_time_min) && ( u8_system_current_min <= D_Config_Packet.Member.u8_day_End_time_min ) )
				{
					u8_status = DEVICE_WORKING_TIME;	
				}
				else
					u8_status = DEVICE_IDLE_TIME;	
			}
			else
			{
			
				if( u8_system_current_min >=  D_Config_Packet.Member.u8_day_start_time_min )
				{
					u8_status = DEVICE_WORKING_TIME;
				}
				else
					u8_status = DEVICE_IDLE_TIME;
			}
		}
		else
			u8_status = DEVICE_IDLE_TIME;
	}
	return u8_status;
}


