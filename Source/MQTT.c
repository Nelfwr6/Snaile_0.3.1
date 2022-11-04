/***********************************************************************************************************************

* Module name: MQTT.c

*

* Copyright 2021 Neona Embedded Labz as an unpublished work.

* All Rights Reserved.

*

* The information contained herein is confidential

* property of Neona Embedded Labz. The use, copying, transfer

* or disclosure of such information is prohibited except by

* express written agreement with Company.

*

* History:-

*

* First written on 12/12/2021 by Vishnu Soman.

*

* Module Description:

* MQTT program

*

***********************************************************************************************************************/


/*******************************************************************************

* Include section

*

*******************************************************************************/

#include "r_cg_macrodriver.h"
#include "r_cg_port.h"
/* Start user code for include. Do not edit comment generated here */
#include "gsm_gprs.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "interval_timer.h"
#include "r_cg_serial.h"
#include "mainhandler.h"
#include "USB_Config.h"
#include "ssl_certificates.h"
#include "Battery_measure.h"
#include "RTC.h"
#include "r_cg_timer.h"
#include "EEPROM.h"
#include "MQTT.h"
#include "mqtt_ssl.h"
#include "power_down.h"


/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/*******************************************************************************

* Global variable section


*******************************************************************************/
 uint8_t Subscribed_data[400];

uint8_t Certificate_buffer[4000];

volatile uint8_t MQT_CMD_RES[25];

uint8_t MQTT_RECV_FLAG = 0; /*to understand recv condition*/
uint8_t MQTT_OPEN_FLAG = 0,MQTT_STAT_FAIL = 0 , Reconnect_flag = 0;   
uint8_t MQTT_PUB_SUCC_FLAG = 0,MQTT_SUB_SUCC_FLAG = 0,MQTT_CONN_SUCCESS_FLAG = 0,MQTT_PUB_FAIL_FLAG = 0,\
MQTT_PDP_FAIL_FLAG = 0,MQT_PDP_RECONNECT_FLAG = 0,MQT_RECONNECT_FLAG = 0;
uint8_t OTA_HTTP_URL[1500];	//to copy the OTA url from the OTA-update request
uint8_t Parsed_Data[50];
uint8_t u8_temp[25];
uint8_t u8_SSL_String2[50];

uint8_t New_PEM_Cert_Buffer[2000];
uint8_t New_Pvt_Cert_Buffer[2000];
uint8_t certificateOwnershipToken[470];
uint8_t PEM_CERT = 0,PVT_CERT =0;
uint16_t length1 = 0,length2 = 0;
uint8_t time_buf[35];        //basil

/*******************************************************************************

* Function name : uint8_t u8_Wait_For_mqtt_Connection(void)

* returns 	: u8_Status

* arg 		: none

* Created by 	: Vishnu Soman

* Date created 	: 12-11-21

* Description 	: Mqtt connection

*******************************************************************************/

uint8_t u8_Wait_For_mqtt_Connection(void)
{
	uint8_t u8_Status = 0 ;
	uint8_t retry = 0;
	char Connect_buff[50];
	//u8_MQTT_PDP_Activate();
	//u8_Get_CCID_IMEI();
	
	
	
		//a1l18joq7lx62l-ats.iot.ca-central-1.amazonaws.com
		
		v_uart_str_send("AT+QMTOPEN=1,\"iot.snailecloud.ca\",8883\r\n",GSM_GPRS_CHANNEL);
		u8_Status = u8_GSM_GPRS_reception_Handler(10000);// max 120s needed
		//v_delay_ms(10000);
		u8_Status = u8_GSM_GPRS_reception_Handler(15000);
		if(u8_Status == 0)
		{
			u8_Status = u8_GSM_GPRS_reception_Handler(15000);
			if(u8_Status == 0)
			{
				u8_Status = u8_GSM_GPRS_reception_Handler(15000);
				if(u8_Status==0)
				{
					u8_Status = u8_GSM_GPRS_reception_Handler(5000);
				}
				if(u8_Status==0)
				{
					u8_Status = u8_GSM_GPRS_reception_Handler(5000);
					u8_Status = u8_GSM_GPRS_reception_Handler(10000);
				}
			}
			
		}	
				
			
		
		
		{
		sprintf(Connect_buff,"AT+QMTCONN=1,\"%s\"\r\n",IMEI_num);
		v_uart_str_send(Connect_buff,GSM_GPRS_CHANNEL);//223a9098764a487ab8dde4d137a6cd47//867624064513459
		//v_delay_ms(2000);
		u8_Status = u8_GSM_GPRS_reception_Handler(10000);
		//v_delay_ms(10000);
		u8_Status = u8_GSM_GPRS_reception_Handler(10000);
		}
		if(u8_Status == 1)
			u8_Status = GSM_SUCCESS;
		
		else 	
		     u8_Status = GSM_FAIL;
		  return u8_Status;
		/*
	        v_uart_str_send("AT+QMTOPEN=1,\"a6bjeuc149fae-ats.iot.ca-central-1.amazonaws.com\",8883\r\n",GSM_GPRS_CHANNEL);
		u8_Status = u8_GSM_GPRS_reception_Handler(15000);// max 120s needed
		v_delay_ms(2000);
		if(u8_Status == MQTT_OPEN_FLAG)
		v_uart_str_send("AT+QMTCONN=1,\"867624064513459\"\r\n",GSM_GPRS_CHANNEL);//bb3c606ada2f436c8815b7c8f1af269a\"db2cc565090f430f8b60408d6c84a438\"
		v_delay_ms(2000);
		u8_Status = u8_GSM_GPRS_reception_Handler(10000);//f11c131195564134819a2f13702b2ed8//9ccf2a8e5c9343cf9fc6815da85c7ef
		
		if(MQTT_CONN_SUCCESS_FLAG == 1)
		v_delay_ms(2000);
		
		Subscribe_to_AWS_Topics();
		v_delay_ms(2000);
		*/
		
		
		
}	

/*******************************************************************************

* Function name :uint8_t Subscribe_to_AWS_Topics(void)

* returns 	: u8_Status

* arg 		: none

* Created by 	: Vishnu Soman

* Date created 	: 18-11-21

* Description 	: Subscribe to MQTT Topics

*******************************************************************************/
uint8_t Subscribe_to_AWS_Topics(void)
{
	
	uint8_t u8_Status = 0,u8_i = 0 ;
	char publish_message[255];
	char Dummy_Publish[20];
	char Dummy_Publish_data[2];
	
	sprintf(publish_message,"AT+QMTSUB=1,1,\"$aws/certificates/create/json/accepted\",1,\"$aws/certificates/create/json/rejected\",1,\"$aws/provisioning-templates/snaile-rfid/provision/json/accepted\",1,\"$aws/provisioning-templates/snaile-rfid/provision/json/rejected\",1\r\n");
	v_uart_str_send(publish_message,GSM_GPRS_CHANNEL);
	v_delay_ms(5000);
	u8_Status = u8_GSM_GPRS_reception_Handler(15000);
	v_delay_ms(1000);
	
	

	
	
	sprintf(Dummy_Publish,"AT+QMTPUBEX=1,1,1,0,\"$aws/certificates/create/json\",2\r\n");
	v_uart_str_send(Dummy_Publish,GSM_GPRS_CHANNEL);
//	if(u8_Gsm_Rx_Pkt_Counter > 1 )
//	{
//	u8_Gsm_Rx_Pkt_Counter = 0;
//	}
	v_delay_ms(2000);
	
	u8_Status = u8_GSM_GPRS_reception_Handler(2000);
	//u8_Gsm_Rx_Pkt_Counter = 0;
	v_uart_str_send("{}",GSM_GPRS_CHANNEL);
	first_time_provisioning = 1;
	vu8_Recv_flag = 1;
//	u8_Status = u8_GSM_GPRS_reception_Handler(5000);
//	v_delay_ms(1000);
//	u8_Status = u8_GSM_GPRS_reception_Handler(5000);
//	v_delay_ms(1000);
//	u8_Status = u8_GSM_GPRS_reception_Handler(10000);
	v_delay_ms(10000);
//	u8_Status = u8_GSM_GPRS_reception_Handler(5000);
	
	if(strstr((char*)Certificate_buffer, "+QMTRECV") > 0)
	{
		MQTT_RECV_FLAG = 1;
		Provisioning_And_Registration(Certificate_buffer);
	}
	vu8_Recv_flag = 0;
	first_time_provisioning = 0;
	return u8_Status;
}
/*******************************************************************************

* Function name :uint8_t Publish_Handshake(void)

* returns 	: u8_Status

* arg 		: none

* Created by 	: Vishnu Soman

* Date created 	: 16-11-21

* Description 	: Publish  Handshake.

*******************************************************************************/
uint8_t Publish_Handshake(void)
{
	
	uint8_t u8_Status = 0,u8_i=0;
	char uart_sendbuffer[230];
	char publish_message[45];
    
	sprintf(uart_sendbuffer,"{\"message\": \"handshake\",\"imei\": \"%s\",\"ccid\": \"%s\",\"firmware_version\": \"%s\",\"configuration_version\": \"%02d\",\"battery_level\": \"%02d\",\"cellular_range\": \"%02ld\",\"timestamp\": \"%s\"}",(char*)IMEI_num,(char*)CCID_num,(char*)D_Config_Packet.Member.u16_Version_id,D_Config_Packet.Member.u16_Config_id,u16_battery_charge,u32_Network_Strength,(char*)clk_fun());
	u8_i=strlen(uart_sendbuffer);
	
	sprintf(publish_message,"AT+QMTPUBEX=1,1,1,0,\"rfid/from-device/%s\",%u\r\n",IMEI_num,u8_i);
	v_uart_str_send(publish_message,GSM_GPRS_CHANNEL);
	
	sprintf(uart_sendbuffer,"{\"message\": \"handshake\",\"imei\": \"%s\",\"ccid\": \"%s\",\"firmware_version\": \"%s\",\"configuration_version\": \"%02d\",\"battery_level\": \"%02d\",\"cellular_range\": \"%02ld\",\"timestamp\": \"%s\"}",(char*)IMEI_num,(char*)CCID_num,(char*)D_Config_Packet.Member.u16_Version_id,D_Config_Packet.Member.u16_Config_id,u16_battery_charge,u32_Network_Strength,clk_fun());
	v_uart_str_send(uart_sendbuffer,GSM_GPRS_CHANNEL);
	
	//for expecting any incoming messages
//	if(u8_Gsm_Rx_Pkt_Counter > 1 )
//	{
//	u8_Gsm_Rx_Pkt_Counter = 0;
//	}
	v_delay_ms(1000);
	u8_Status = u8_GSM_GPRS_reception_Handler(2000);
	v_delay_ms(1000);
	u8_Status = u8_GSM_GPRS_reception_Handler(4000);
	v_delay_ms(1000);
	u8_Status = u8_GSM_GPRS_reception_Handler(6000);
	v_delay_ms(1000);
	u8_Status = u8_GSM_GPRS_reception_Handler(8000);
	v_delay_ms(5000);
	u8_Status = u8_GSM_GPRS_reception_Handler(10000);
	
//	if(u8_Status == MQTT_PUB_SUCC_FLAG)
//	MQTT_Reception_Handler(Subscribed_data);
	
return u8_Status;
}


/*******************************************************************************

* Function name :uint8_t AWS_Thing_Provisioning(void)

* returns 	: u8_Status

* arg 		: none

* Created by 	: Vishnu Soman

* Date created 	: 22-11-21

* Description 	: AWS_Thing_Provisioning

*******************************************************************************/
uint8_t AWS_Thing_Provisioning(void)
{

   	uint16_t u8_Status = 0,u8_i=0;
	char uart_sendbuffer[580];
	char publish_message[45];
	char publish_command[60];
	
	//u8_Get_CCID_IMEI();
	
	u8_Status = u8_GSM_GPRS_reception_Handler(2000);
	v_delay_ms(2000);
	
	sprintf(uart_sendbuffer,"{\"certificateOwnershipToken\": \"%s\",\"parameters\": {\"imei\": \"%s\",\"ccid\": \"%s\"}}",(char*)certificateOwnershipToken,(char*)IMEI_num,(char*)CCID_num);
	u8_i=strlen(uart_sendbuffer);
	
	
	sprintf(publish_command,"AT+QMTPUBEX=1,1,1,0,\"$aws/provisioning-templates/snaile-rfid/provision/json\",%u\r\n",u8_i);
	v_uart_str_send(publish_command,GSM_GPRS_CHANNEL);
	v_delay_ms(2000);
		
	sprintf(uart_sendbuffer,"{\"certificateOwnershipToken\": \"%s\",\"parameters\": {\"imei\": \"%s\",\"ccid\": \"%s\"}}",(char*)certificateOwnershipToken,(char*)IMEI_num,(char*)CCID_num);
	v_uart_str_send(uart_sendbuffer,GSM_GPRS_CHANNEL);
	u8_Status = u8_GSM_GPRS_reception_Handler(5000);
	v_delay_ms(1000);
	u8_Status = u8_GSM_GPRS_reception_Handler(5000);
			
	D_Config_Packet.Member.Provinsion_Flag = PROVISIONED;
	D_Config_Packet.Member.u32_Checksum_Value = u32_checksum_calculate(&D_Config_Packet.All[0], CONFIG_DF_DATA_SIZE - 4);
	u16_DConfig_DF_Write_Position = DCONFIG_DF_START_POSITION;
//	Erase_Dconfig_Data_Flash();
	Write_Dconfig_Data_Flash();	
	return u8_Status;
}
/*******************************************************************************

* Function name :uint8_t Registration_request(void)

* returns 	: u8_Status

* arg 		: none

* Created by 	: Vishnu Soman

* Date created 	: 14-11-21

* Description 	: Publish Registration request

*******************************************************************************/
uint8_t Registration_request(void)
{
	
	uint16_t u8_Status = 0,u8_i=0;
	char uart_sendbuffer[200];
	char publish_message[45];
	
	//clk_fun();
	sprintf(uart_sendbuffer,"{\"message\": \"registration-request\",\"imei\": \"%s\",\"ccid\": \"%s\",\"firmware_version\": \"%s\",\"timestamp\": \"%s\"}",(char*)IMEI_num,(char*)CCID_num,(char*)D_Config_Packet.Member.u16_Version_id,(char*)clock);
	u8_i=strlen(uart_sendbuffer);
	
	sprintf(publish_message,"AT+QMTPUBEX=1,1,1,0,\"rfid/from-device/%s\",%u\r\n",(char*)IMEI_num,u8_i);
	v_uart_str_send(publish_message,GSM_GPRS_CHANNEL);
	u8_Status = u8_GSM_GPRS_reception_Handler(15000);
	//clk_fun();
	sprintf(uart_sendbuffer,"{\"message\": \"registration-request\",\"imei\": \"%s\",\"ccid\": \"%s\",\"firmware_version\": \"%s\",\"timestamp\": \"%s\"}",(char*)IMEI_num,(char*)CCID_num,(char*)D_Config_Packet.Member.u16_Version_id,(char*)clock);
	v_uart_str_send(uart_sendbuffer,GSM_GPRS_CHANNEL);
	
	//for expecting any incoming messages
	if(u8_Gsm_Rx_Pkt_Counter > 1 )
	{
	//u8_Gsm_Rx_Pkt_Counter=0;
	}
	v_delay_ms(2000);
	u8_Status = u8_GSM_GPRS_reception_Handler(2000);
	v_delay_ms(4000);
	u8_Status = u8_GSM_GPRS_reception_Handler(4000);
	v_delay_ms(6000);
	u8_Status = u8_GSM_GPRS_reception_Handler(6000);
	v_delay_ms(8000);
	u8_Status = u8_GSM_GPRS_reception_Handler(8000);
	v_delay_ms(10000);
	u8_Status = u8_GSM_GPRS_reception_Handler(10000);
	
	if(u8_Status != MQTT_PUB_SUCC_FLAG)
	{
		MQTT_Reception_Handler(Subscribed_data);
	}
	
	return u8_Status;
}
/*******************************************************************************

* Function name :void MQTT_Reception_Handler(uint8_t * Subscribed_data)

* returns 	: u8_Status

* arg 		: Subscribed_data

* Created by 	: Vishnu Soman

* Date created 	: 22-11-21

* Description 	: MQTT_Reception_Handler

*******************************************************************************/


uint8_t MQTT_Reception_Handler(uint8_t * Subscribed_data)
{
		
   char str2[100];
   char *p =&Subscribed_data;
   uint8_t u8_status = 0;
   
		if(MQTT_RECV_FLAG == 1)
		{
			p = strstr((char*)Subscribed_data,"\"message\":\"");
		        if(p)
			{
		   	p=p+sizeof("\"message\":\"")-1;
			strncpy(str2,p,strchr(p,'"')-p);
			str2[strchr(p,'"')-p]='\0';
			strcpy((char*)Parsed_Data,str2);
			}
			
			if(strcmp(str2,"registration-result") == 0)
			{
				
			p = strstr(Subscribed_data,"\"result\":\"success\"");
			if(p)
			{
//		   	p=p+sizeof("\"result\":\"")-1;
//			strncpy(str2,p,strchr(p,'"')-p);
//			str2[strchr(p,'"')-p]='\0';
//			}
//			if(strcmp(str2,"success") == 0)
//			{
			
				DEBUG_LED_OFF();	
				D_Config_Packet.Member.Registration_Flag = Registered;
				D_Config_Packet.Member.u32_Checksum_Value = u32_checksum_calculate(&D_Config_Packet.All[0], CONFIG_DF_DATA_SIZE - 4);
				u16_DConfig_DF_Write_Position = DCONFIG_DF_START_POSITION;
//				Erase_Dconfig_Data_Flash();
				Write_Dconfig_Data_Flash();
				MQTT_RECV_FLAG = 0;
			}
			
			}
			
			if(strcmp(str2,"restart-request") == 0)
			{
				
				//Reset_MCU();
				Update_restart_time();
				MQTT_RECV_FLAG = 0;
				return MQTT_SUCCUSS;
				
			}
	
 
			else if(strcmp(str2,"configuration-update-request") == 0)
			{
				Config_Acknowledgement();
				Update_Configuration();
				v_update_date_time();					//basil
				//Config_Acknowledgement();
				MQTT_RECV_FLAG = 0;
				//Publish_Handshake();				//basil
				
			}
	
			else if(strcmp(str2,"ota-update-request") == 0)
			{
				OTA_Acknowledgement();
				
				p = strstr((char*)Subscribed_data,"\"http_url\":\"");
			  	if(p)
				{
		   		 	p=p+sizeof("\"http_url\":\"")-1;
		   		  	
		   			strncpy(str2,p,strchr(p,'"')-p);
		   			str2[strchr(p,'"')-p]='\0';
					
					strcpy((char*)OTA_HTTP_URL,str2);
					MQTT_RECV_FLAG = 0;
			
				
				////////////////////////////// ota start////////////////////////
				 
				    v_Gsm_Power_Down();
				    R_UART1_Create();
   				    R_UART1_Start();
				    R_UART1_Receive(&gsm_rx_data,1);
				    v_Gsm_Hardware_Init();
			            u8_Gsm_Wait_For_AT_response(14000);
			            //u8_Update_SSL_Crt_To_UFS();
				    v_delay_ms(5000);
				    //Mqtt_SSL_Update_to_RAM();
				    //v_delay_ms(2000);
				    Rails_registration();
				    //Mqtt_SSL_Config();
				    //v_delay_ms(2000);
				    
				    
				    u8_status = u8_GPRS_Init();
				    if(u8_status == GSM_SUCCESS)
				    {
					DEBUG_LED_ON();	
				    }
				  //  u8_GSM_TCP_Activate((uint8_t *)&D_Config_Packet.Member.u8_apn[0],(uint8_t *)"",(uint8_t *)"");
				    u8_status = u8_GSM_TCP_Activate((uint8_t *)&D_Config_Packet.Member.u8_apn[0],(uint8_t *)"",(uint8_t *)"");
				    if(u8_status == GSM_SUCCESS)
				    {
					
					DEBUG_LED_OFF();	
					GSM_PDP_STATUS_LED = LED_ON;
				    }
				    v_delay_ms(1000);
				    u8_status = u8_HTTP_Open(OTA_HTTP_URL);
				    if((u8_status == GSM_SUCCESS))
				    {
					GSM_PDP_STATUS_LED = LED_OFF;
					GSM_DATA_STATUS_LED = LED_ON;
					v_delay_ms(1000);
					v_uart_str_send("AT+QFLDS=\"UFS\"\r",GSM_GPRS_CHANNEL);	
					u8_status = u8_GSM_GPRS_reception_Handler(1000); 
					if(u8_status == GSM_SUCCESS)					
					{								
						//v_uart_str_send("AT+QFDEL=\"*\"\r",GSM_GPRS_CHANNEL);		// Delete all data in RAM
						//u8_status = u8_GSM_GPRS_reception_Handler(1000);
						//if(u8_status == GSM_SUCCESS)
						//{	
							v_delay_ms(1000);
							u8_status = u8_HTTP_GET();
							if((u8_status == GSM_SUCCESS) || GSM_HTTP_READ)
							{
								GSM_PDP_STATUS_LED = LED_ON;
								GSM_DATA_STATUS_LED = LED_ON;
								v_delay_ms(1000);
								R_RTC_Set_ConstPeriodInterruptOff();
								R_RTC_Stop();
								Reset_MCU();
							}
						//}
					}
				    }
	
				    
				 
				/////////////////////////////test ota end////////////////////////
				}
			}
			else if(strcmp(str2,"handshake-result") == 0)
			{
				
			p = strstr(Subscribed_data,"\"result\":\"success\"");
			if(p)
			{
				return MQTT_SUCCUSS;
			}
			}
			else if(strcmp(str2,"tag-read-result") == 0)
			{
				
			p = strstr(Subscribed_data,"\"result\":\"success\"");
			if(p)
			{
				return MQTT_SUCCUSS;
			}
			}
			
		}
			
}
				
/*******************************************************************************
* Function name : uint8_t u8_GSM_PDP_Activate(void)
* returns 	: status
* arg		: none
* Created by 	: 
* Date created 	: 
* Description 	: mqtt pdp activate
*******************************************************************************/
uint8_t u8_MQTT_PDP_Activate(void)
{
	uint8_t u8_Status = 0;
	
	v_uart_str_send("AT+CGACT=1,1\r",GSM_GPRS_CHANNEL); 
	u8_Status = u8_GSM_GPRS_reception_Handler(10000);
	v_delay_ms(1000);
	v_uart_str_send("AT+CGACT=0,1\r",GSM_GPRS_CHANNEL);
	u8_Status = u8_GSM_GPRS_reception_Handler(10000);
	v_delay_ms(1000);
	v_uart_str_send("AT+CGACT=1,1\r",GSM_GPRS_CHANNEL); 
	u8_Status = u8_GSM_GPRS_reception_Handler(10000);
	v_delay_ms(1000);
	v_uart_str_send("AT+CGACT?\r",GSM_GPRS_CHANNEL); 
	u8_Status = u8_GSM_GPRS_reception_Handler(1000);
	
	return u8_Status;
	
}

/*******************************************************************************

* Function name : uint8_t Config_Acknowledgement(void)

* returns 	: u8_Status

* arg 		: none

* Created by 	: 

* Date created 	: 

* Description 	: 

*******************************************************************************/
uint8_t Config_Acknowledgement(void)
{
	
	uint16_t u8_Status = 0,u8_i=0;
	char uart_sendbuffer[200];
	char publish_message[45];
	
	
	if(update_done_flag == 1)
	{
		//clk_fun();
		sprintf(uart_sendbuffer,"{\"message\":\"configuration-update-success\",\"imei\":\"%s\",\"timestamp\":\"%s\"}",(char*)IMEI_num,(char*)clk_fun());
		u8_i=strlen(uart_sendbuffer);

		sprintf(publish_message,"AT+QMTPUBEX=1,1,1,0,\"rfid/from-device/%s\",%u\r\n",(char*)IMEI_num,u8_i);
		v_uart_str_send(publish_message,GSM_GPRS_CHANNEL);
		u8_Status = u8_GSM_GPRS_reception_Handler(15000);
		//clk_fun();
		sprintf(uart_sendbuffer,"{\"message\":\"configuration-update-success\",\"imei\":\"%s\",\"timestamp\":\"%s\"}",(char*)IMEI_num,(char*)clk_fun());
		v_uart_str_send(uart_sendbuffer,GSM_GPRS_CHANNEL);
		update_done_flag = 0;
		
	}
	else
	{       //clk_fun();
		sprintf(uart_sendbuffer,"{\"message\":\"configuration-update-received\",\"imei\":\"%s\",\"timestamp\":\"%s\"}",(char*)IMEI_num,(char*)clk_fun());
		u8_i=strlen(uart_sendbuffer);
		
		sprintf(publish_message,"AT+QMTPUBEX=1,1,1,0,\"rfid/from-device/%s\",%u\r\n",(char*)IMEI_num,u8_i);
		v_uart_str_send(publish_message,GSM_GPRS_CHANNEL);
		u8_Status = u8_GSM_GPRS_reception_Handler(15000);
		//clk_fun();
		sprintf(uart_sendbuffer,"{\"message\":\"configuration-update-received\",\"imei\":\"%s\",\"timestamp\":\"%s\"}",(char*)IMEI_num,(char*)clk_fun());
		v_uart_str_send(uart_sendbuffer,GSM_GPRS_CHANNEL);
	
	}
	
	
	if(u8_Gsm_Rx_Pkt_Counter > 2)
		{
		u8_Gsm_Rx_Pkt_Counter=0;
		}
	
	return u8_Status;
}



/*******************************************************************************

* Function name : uint8_t OTA_Acknowledgement(void)

* returns 	: u8_Status

* arg 		: none

* Created by 	: 

* Date created 	: 

* Description 	: 

*******************************************************************************/
uint8_t OTA_Acknowledgement(void)
{
	
	uint16_t u8_Status = 0,u8_i=0;
	char uart_sendbuffer[200];
	char publish_message[45];
	//clk_fun();
	sprintf(uart_sendbuffer,"{\"message\":\"ota-update-received\",\"imei\":\"%s\",\"timestamp\":\"%s\"}",(char*)IMEI_num,(char*)clk_fun());
	u8_i=strlen(uart_sendbuffer);
	
	sprintf(publish_message,"AT+QMTPUBEX=1,1,1,0,\"from-device/%s\",%u\r\n",IMEI_num,u8_i);
	v_uart_str_send(publish_message,GSM_GPRS_CHANNEL);
	u8_Status = u8_GSM_GPRS_reception_Handler(15000);
	//clk_fun();
	sprintf(uart_sendbuffer,"{\"message\":\"ota-update-received\",\"imei\":\"%s\",\"timestamp\":\"%s\"}",(char*)IMEI_num,(char*)clk_fun());
	v_uart_str_send(uart_sendbuffer,GSM_GPRS_CHANNEL);
	
	
	//if(OTA_update_done_flag == 1)
	//{
	//clk_fun();
	sprintf(uart_sendbuffer,"{\"message\":\"ota-update-success\",\"imei\":\"%s\",\"timestamp\":\"%s\"}",(char*)IMEI_num,(char*)clk_fun());
	u8_i=strlen(uart_sendbuffer);
	
	sprintf(publish_message,"AT+QMTPUBEX=1,1,1,0,\"from-device/%s\",%u\r\n",IMEI_num,u8_i);
	v_uart_str_send(publish_message,GSM_GPRS_CHANNEL);
	u8_Status = u8_GSM_GPRS_reception_Handler(15000);
	//clk_fun();
	sprintf(uart_sendbuffer,"{\"message\":\"ota-update-success\",\"imei\":\"%s\",\"timestamp\":\"%s\"}",(char*)IMEI_num,(char*)clk_fun());
	v_uart_str_send(uart_sendbuffer,GSM_GPRS_CHANNEL);
	//}
	
	
	if(u8_Gsm_Rx_Pkt_Counter > 2)
	{
		u8_Gsm_Rx_Pkt_Counter=0;
	}
	
	return u8_Status;
}


/*******************************************************************************

* Function name : uint8_t Mqtt_SSL_Update_to_RAM(void)

* returns 	: u8_Status

* arg 		: none

* Created by 	: 

* Date created 	: 

* Description 	: 

*******************************************************************************/

uint8_t Mqtt_SSL_Update_to_RAM(void)
{
	
	uint8_t u8_status = 0;
	uint16_t u16_CA_length = 0, u16_PEM_length,u16_Private_length = 0;
	
	u16_CA_length  = CA0_TOTAL_LENGTH;
	u16_PEM_length  = CC0_TOTAL_LENGTH;
	u16_Private_length  = CK0_TOTAL_LENGTH;
	
									
		    v_uart_str_send("AT+QFDEL=\"*\"\r",GSM_GPRS_CHANNEL);		// Delete all data in RAM
	            u8_status = u8_GSM_GPRS_reception_Handler(1000);
		   // if(u8_status == GSM_SUCCESS)
		     {	
			sprintf((char *)u8_SSL_String2,"AT+QFUPL=\"RAM:cacert.pem\",%d,100\r",u16_CA_length);			
			v_uart_str_send((char *)u8_SSL_String2,GSM_GPRS_CHANNEL);
			u8_status = u8_GSM_GPRS_reception_Handler(2000);
			if(u8_status == GSM_FILE_CONNECT)
			{
				
				v_byte_array_send(u8_CA1,418,1); //ram 411 //ufs 418
		                v_byte_array_send(u8_CA2,455,1); //448     //455
		                v_byte_array_send(u8_CA3,315,1); //309     //315
				u8_status = u8_GSM_GPRS_reception_Handler(5000);
				
				
				if(u8_status == GSM_SUCCESS)
				{			
					sprintf((char *)u8_SSL_String2,"AT+QFUPL=\"RAM:clientcert.pem\",%d,100\r",u16_PEM_length);
					v_uart_str_send((char *)u8_SSL_String2,GSM_GPRS_CHANNEL);
					u8_status = u8_GSM_GPRS_reception_Handler(2000);
					if(u8_status == GSM_FILE_CONNECT)
					{
				           
					    v_byte_array_send(u8_snaile_CRT_buff_1,483,1); //ram475  //ufs483
			                    v_byte_array_send(u8_snaile_CRT_buff_2,455,1); //448     //455
			                    v_byte_array_send(u8_snaile_CRT_buff_3,286,1); //281     //286
			                    u8_status = u8_GSM_GPRS_reception_Handler(10000);
						if(u8_status == GSM_SUCCESS)
						{
							sprintf((char *)u8_SSL_String2,"AT+QFUPL=\"RAM:clientkey.pem\",%d,100\r",u16_Private_length);
							v_uart_str_send((char *)u8_SSL_String2,GSM_GPRS_CHANNEL);
							u8_status = u8_GSM_GPRS_reception_Handler(2000);
							if(u8_status == GSM_FILE_CONNECT)
							{
								
								v_byte_array_send(u8_snaile_private_key_buff_1,422,1);  //ram415  //ufs422
				                                v_byte_array_send(u8_snaile_private_key_buff_2,455,1);  //448	       455
				                                v_byte_array_send(u8_snaile_private_key_buff_3,455,1);  //448	       455
				                                v_byte_array_send(u8_snaile_private_key_buff_4,347,1);  //337	       343
								
								u8_status = u8_GSM_GPRS_reception_Handler(5000);
							}
						}
					}
				}
			}
		     }
		
	return u8_status;
	
	
}

/*******************************************************************************

* Function name : uint8_t Mqtt_SSL_Config(void)

* returns 	: u8_status

* arg 		: none

* Created by 	: Vishnu Soman

* Date created 	: 

* Description 	: 

*******************************************************************************/

uint8_t Mqtt_SSL_Config(void)
{
	
	uint8_t u8_status = 0;
	
        v_uart_str_send("AT+QMTCFG=\"SSL\",1,1,2\r\n",GSM_GPRS_CHANNEL);
        u8_status = u8_GSM_GPRS_reception_Handler(1000);
	if(u8_status == GSM_SUCCESS)
	{
		v_uart_str_send("AT+QMTCFG=\"recv/mode\",1,0,1\r\n",GSM_GPRS_CHANNEL);
		u8_status = u8_GSM_GPRS_reception_Handler(1000);
		if(u8_status == GSM_SUCCESS)
		{
		
		v_uart_str_send("AT+QSSLCFG=\"cacert\",2,\"RAM:cacert.pem\"\r\n",GSM_GPRS_CHANNEL);
		u8_status = u8_GSM_GPRS_reception_Handler(1000);
			if(u8_status == GSM_SUCCESS)
			{
			v_uart_str_send("AT+QSSLCFG=\"clientcert\",2,\"RAM:clientcert.pem\"\r\n",GSM_GPRS_CHANNEL);
			u8_status = u8_GSM_GPRS_reception_Handler(1000);
			if(u8_status == GSM_SUCCESS)
			{
				v_uart_str_send("AT+QSSLCFG=\"clientkey\",2,\"RAM:clientkey.pem\"\r\n",GSM_GPRS_CHANNEL);
				u8_status = u8_GSM_GPRS_reception_Handler(1000);
				if(u8_status == GSM_SUCCESS)
				{
					v_uart_str_send("AT+QSSLCFG=\"seclevel\",2,2\r\n",GSM_GPRS_CHANNEL);
					u8_status = u8_GSM_GPRS_reception_Handler(1000);
					if(u8_status == GSM_SUCCESS)
					{
						v_uart_str_send("AT+QSSLCFG=\"sslversion\",2,4\r\n",GSM_GPRS_CHANNEL);
						u8_status = u8_GSM_GPRS_reception_Handler(1000);
					      if(u8_status == GSM_SUCCESS)
						{
						v_uart_str_send("AT+QSSLCFG=\"ciphersuite\",2,0xFFFF\r\n",GSM_GPRS_CHANNEL);
						u8_status = u8_GSM_GPRS_reception_Handler(15000);
						if(u8_status == GSM_SUCCESS)
						{
							v_uart_str_send("AT+QSSLCFG=\"ignorelocaltime\",2,1\r\n",GSM_GPRS_CHANNEL);
							u8_status = u8_GSM_GPRS_reception_Handler(15000);
							if(u8_status == GSM_SUCCESS)
							{				     // ignoremulticertchainverify
								v_uart_str_send("AT+QSSLCFG=\"ignoremulticertchainverify\",2,0\r\n",GSM_GPRS_CHANNEL);
								u8_status = u8_GSM_GPRS_reception_Handler(15000);
							
							}
						
						}
					    }
					}
			            }
				}
			     }
			}
		    }
	return u8_status;
}

/*******************************************************************************

* Function name : void Provisioning_And_Registration(uint8_t * Certificate_Buffer)

* returns 	: Certificate_Buffer

* arg 		: 

* Created by 	: 

* Date created 	: 

* Description 	: 

*******************************************************************************/
void Provisioning_And_Registration(uint8_t * Certificate_Buffer)
{
	
   char str2[3000];
   //char str3[1710];
  // char str4[300];
   char P_remove[2500];
   //char Pvt_remove[2000];
   uint8_t u8_SSL_String3[100];
   char *p=NULL,*q=NULL,*r=NULL;
   uint16_t Size_Pem = 0,Size_Pvt = 0;
   uint8_t cycle1 = 0,cycle_count = 0,u8_status = 0;
   uint32_t Full_Parse_Size =0;
   char *pch = NULL;
   char *qch = NULL;
   char symbol[5] = "\\";
  

        
	if(MQTT_RECV_FLAG == 1)
	{
		Full_Parse_Size = strlen(Certificate_Buffer);
		if(Full_Parse_Size)
		{
		MQTT_RECV_FLAG = 0;
		Erase_Cert_Data_Flash();
		//to parse new certificates 
		
		p = strstr((char*)Certificate_Buffer,"\"certificatePem\":\"");
			if(p)
			{
				
			p=p+sizeof("\"certificatePem\":\"")-1;
			strncpy(str2,p,strchr(p,'"')-p);
			str2[strchr(p,'"')-p-1]='\0';
			strcpy(P_remove,str2);
			
		cycle1 = 1;	
		pch = strtok((char*)P_remove,symbol);
			    memset(New_PEM_Cert_Buffer, 0x00, sizeof(New_PEM_Cert_Buffer));
			    while (pch != NULL )
			    {
				if(cycle1 == 1)
				{
			        strcat((char*)New_PEM_Cert_Buffer, pch);
				strcat((char*)New_PEM_Cert_Buffer,"\r\n");
			        pch = strtok(NULL,symbol);
				cycle1 = 2;
				}
				strcat((char*)New_PEM_Cert_Buffer, pch+1);
				strcat((char*)New_PEM_Cert_Buffer,"\r\n");
			        pch = strtok(NULL,symbol);
			    }
			Size_Pem = strlen(New_PEM_Cert_Buffer);
			
			PEM_CERT =1;
			//if(Size_Pem = u32_checksum_calculate(&New_PEM_Cert_Buffer,Size_Pem));
			u32_Cert_DF_Write_Position = CERT_DF_START_POSITION;
			v_Write_Cert_DF_data_flash();
			PEM_CERT =0;
			}
			
			
			if(write_success  == 1)
			{
			q = strstr((char*)Certificate_Buffer,"\"privateKey\":\"");
			if(q)
			{
				memset(P_remove, 0x00, sizeof(P_remove));
				memset(str2, 0x00, sizeof(str2));
				q=q+sizeof("\"privateKey\":\"")-1;
				strncpy(str2,q,strchr(q,'"')-q);
				str2[strchr(q,'"')-q-1]='\0';
				strcpy(P_remove,str2);
				
				cycle1 = 1;	
				qch = strtok((char*)P_remove,symbol);
				memset(New_Pvt_Cert_Buffer, 0x00, sizeof(New_Pvt_Cert_Buffer));
				while (qch != NULL)
				{
				    
				if(cycle1 == 1)
				{
			        strcat((char*)New_Pvt_Cert_Buffer, qch);
				strcat((char*)New_Pvt_Cert_Buffer,"\r\n");
			        qch = strtok(NULL,symbol);
				cycle1 = 3;
				}
				else
				{
				strcat((char*)New_Pvt_Cert_Buffer, qch+1);
				strcat((char*)New_Pvt_Cert_Buffer,"\r\n");
			        qch = strtok(NULL,symbol);	
				}
			    }
				Size_Pvt=strlen(New_Pvt_Cert_Buffer);
				
				PVT_CERT =1;
				//if(Size_Pvt = u32_checksum_calculate(&New_Pvt_Cert_Buffer,Size_Pvt));
				u32_CertPvt_DF_Write_Position = CERTPvt_DF_START_POSITION;
				v_Write_Cert_DF_data_flash();
				PVT_CERT =0;
				}
			}
			if(write_success  == 2)
			{
				r = strstr((char*)Certificate_Buffer,"\"certificateOwnershipToken\":\"");
				if(r)
				{
					memset(str2, 0x00, sizeof(str2));
					r=r+sizeof("\"certificateOwnershipToken\":\"")-1;
					strncpy(str2,r,strchr(r,'"')-r);
					str2[strchr(r,'"')-r]='\0';
					strcpy((char*)certificateOwnershipToken,str2);
				}
			}
	
	}
	}
	else
	{
	cycle_count++;	
	
	v_uart_str_send("AT+QMTPUBEX=1,1,1,0,\"$aws/certificates/create/json\",2\r\n",GSM_GPRS_CHANNEL);
	v_delay_ms(1000);
	
	
	u8_status = u8_GSM_GPRS_reception_Handler(2000);
	v_uart_str_send("{}",GSM_GPRS_CHANNEL);
	vu8_Recv_flag = 1;
	u8_status = u8_GSM_GPRS_reception_Handler(5000);
	v_delay_ms(1000);
	u8_status = u8_GSM_GPRS_reception_Handler(10000);
	v_delay_ms(1000);
	u8_status = u8_GSM_GPRS_reception_Handler(5000);
	
	if(cycle_count > 2)
	{
	Reset_MCU();	
	}
	}
	AWS_Thing_Provisioning();//subscribes to result topics and publishes privisioning request			
}
/*******************************************************************************

* Function name : uint8_t Rails_registration(void)

* returns 	: none

* arg 		: 

* Created by 	: 

* Date created 	: 

* Description 	: 

*******************************************************************************/							
uint8_t Rails_registration(void)
{

	uint8_t u8_status, i = 0; 
	char publish_message[255];
	uint8_t u8_SSL_String3[100],u8_SSL_String2[100];
	uint16_t u16_CA_length = 0;
	
	 Read_Cert_Data_Flash();
         Read_Cert_Data_Flash2();
	
	u8_Get_CCID_IMEI();
	
	v_uart_str_send("AT+CTZU=3\r\n",GSM_GPRS_CHANNEL); 
	u8_GSM_GPRS_reception_Handler(1000);
	
	//RTC_Update_From_GSM();    //from here ,next two lines (1058) basil
	//if(!update_done_flag)
	RTC_Update_From_GSM();
	
	//v_uart_str_send("AT+QICSGP=1,1,\"internet\",\"\",\"\",1\r\n",GSM_GPRS_CHANNEL); 
	//u8_status = u8_GSM_GPRS_reception_Handler(1000);
	
	v_uart_str_send("AT+QIACT=1\r\n",GSM_GPRS_CHANNEL); 
	u8_status = u8_GSM_GPRS_reception_Handler(10000);
	
	u16_CA_length  = CA0_TOTAL_LENGTH;
	length1 = strlen((char*)New_PEM_Cert_Buffer);
	length2 = strlen((char*)New_Pvt_Cert_Buffer);
	
	// Deleting all data in RAM
        v_uart_str_send("AT+QFDEL=\"*\"\r",GSM_GPRS_CHANNEL);		
        u8_status = u8_GSM_GPRS_reception_Handler(2000);
	
	//upload new saved certificates into RAM
        sprintf((char *)u8_SSL_String2,"AT+QFUPL=\"RAM:cacert.pem\",%d,100\r",u16_CA_length);			
	v_uart_str_send((char *)u8_SSL_String2,GSM_GPRS_CHANNEL);
	//v_delay_ms(2000);
	u8_status = u8_GSM_GPRS_reception_Handler(6000);
	if(u8_status == GSM_FILE_CONNECT)
	{
		v_byte_array_send(u8_CA1,418,1); 
                v_byte_array_send(u8_CA2,455,1); 
                v_byte_array_send(u8_CA3,315,1); 
		u8_status = u8_GSM_GPRS_reception_Handler(5000);
		if(u8_status == GSM_SUCCESS)
		{	
			v_delay_ms(2000);
			sprintf((char *)u8_SSL_String3,"AT+QFUPL=\"RAM:clientcert.pem\",%d,1000\r",length1);
			v_uart_str_send((char *)u8_SSL_String3,GSM_GPRS_CHANNEL);
			//v_delay_ms(2000);
			i = 10;
			while(i)
			{
				u8_status = u8_GSM_GPRS_reception_Handler(1000);
				if(u8_status == GSM_FILE_CONNECT)
				{
					break;
				}
				i--;
			}
			if(u8_status == GSM_FILE_CONNECT)
			{
				for(i = 0; i<(length1/500); i++)	
				{
					v_byte_array_send(New_PEM_Cert_Buffer+(500*i),500,1);
				}
				v_byte_array_send(New_PEM_Cert_Buffer+(500*i),length1 - (500*i),1);
			}
			u8_status = u8_GSM_GPRS_reception_Handler(5000);
		
			if(u8_status == GSM_SUCCESS)
			{
				v_delay_ms(2000);
				sprintf((char *)u8_SSL_String3,"AT+QFUPL=\"RAM:clientkey.pem\",%d,1000\r",length2);
				v_uart_str_send((char *)u8_SSL_String3,GSM_GPRS_CHANNEL);
				//v_delay_ms(2000);
						i = 10;
						while(i)
						{
						u8_status = u8_GSM_GPRS_reception_Handler(1000);
						if(u8_status == GSM_FILE_CONNECT)
						{
							break;
						}
						i--;
						}
				//u8_status = u8_GSM_GPRS_reception_Handler(6000);
				if(u8_status == GSM_FILE_CONNECT)
				{
					for(i = 0; i<(length2/500); i++)	
					{
						v_byte_array_send(New_Pvt_Cert_Buffer+(500*i),500,1);
					}
					v_byte_array_send(New_Pvt_Cert_Buffer+(500*i),length2 - (500*i),1);
				}
			}
		}
	}
	v_delay_ms(3000);
	u8_status = Mqtt_SSL_Config();
	v_delay_ms(1000);
	return u8_status;
}

	
void Subscribe_to_Rails_Topic(void)
{
	uint8_t u8_Status = 0,u8_i = 0 ;
	char publish_message[255];
	
	sprintf(publish_message,"AT+QMTSUB=1,1,\"rfid/to-device/%s\",1\r\n",(char*)IMEI_num);
	v_uart_str_send(publish_message,GSM_GPRS_CHANNEL);
	v_delay_ms(2000);
	u8_Status = u8_GSM_GPRS_reception_Handler(5000);
	v_delay_ms(1000);
}




/******************************************************************/








	
