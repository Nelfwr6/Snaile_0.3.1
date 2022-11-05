/***********************************************************************************************************************

* Module name: gsm_gprs.c

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

* First written on 12/03/2014 by Aarathy Ramesh.

* Edited by Reshma Joseph

* Module Description:

* GSM_GPRS program

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
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/*******************************************************************************

* Global variable section

*******************************************************************************/

/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
uint8_t u8_GSM_Status = GSM_POWERDOWN;
uint32_t u32_Gsm_At_Cmd_Tmo = 0;
uint8_t u8_TCP_Status = GSM_PDP_DEACTIVATED;
uint8_t u8_Network_Status = 0,u8_Recv_Data_Use_Flag = 0;

volatile uint16_t vu16_Gsm_Rx_Tmo = 0, u16_sub_pkt_counter = 0;
volatile uint8_t vu8_Gsm_Rx_Buffer[GSM_GPRS_MAX_PACKET_COUNT][GSM_GPRS_RX_BUFFER_SIZE];
volatile uint8_t vu8_Recv_Data[GPRS_RX_SSL_DATA_LEN], vu8_Recv_flag = 0;
volatile uint8_t vu8_GSM_Packet_Status[GSM_GPRS_MAX_PACKET_COUNT];
uint8_t u8_gsm_gprs_rx;
volatile uint8_t u8_Gsm_Rx_Pkt_Counter = 0, u8_Gsm_Rx_Status_Counter = 0, u8_Gsm_Rx_Status_Start_Counter = 0;
volatile uint16_t u16_Gsm_Cur_Buffer_Count = 0,u16_Recv_Data_Count = 0;
uint16_t u16_Server_Data_Length = 0;
volatile uint8_t u8_Server_Data_Recv_Flag = 0;
uint8_t u8_Gsm_Uart_Error_Flag = 0;
uint8_t u8_GSM_Response_Identifier[GSM_GPRS_RX_BUFFER_SIZE];
uint32_t u32_Gsm_Retry_Tmo = 0;
uint8_t u8_SIM_Insert = 0;
uint8_t u8_GSM_Com_Retry_count = 0;

uint8_t u8_Tcp_Reconnect_Count = 0;
uint8_t	u8_PDP_Status = 0, u8_GSM_Send_Flag = 0;

uint8_t u8_SSL_String[50];

uint8_t u8_Network_Sec,u8_Network_Min,u8_Network_Hr,u8_Network_Date,u8_Network_Month,u8_Network_Year;

uint8_t u8_CME_Error_Code = 0;


uint8_t u8_URC_Response[50];
uint8_t u8_SIM_Status[50];

uint8_t u8_network_Location_buf[10];
uint8_t u8_network_mode;
uint8_t u8_APN_Buf[50];
uint32_t u32_Network_Strength =0;

uint16_t u16_CA0_length = 0, u16_CC0_length,u16_CK0_length = 0;
uint8_t u8_GSM_Data_String[150];
extern uint8_t u8_GSM_Init_Retry_count;
extern volatile uint8_t u8_mqtt_status;
uint8_t u8_temp_buff[1024];
  uint8_t CCID_num[30];
  uint8_t IMEI_num[30];
uint8_t clock[35];
uint8_t u8_FW_File_Size[15] = {0},first_time_provisioning = 0;

/*******************************************************************************
* Function name : void Convert_Time_To_Bcd(void)
* returns 	: status
* arg		: none
* Created by 	: Jithin P P	
* Date created 	: 11-05-2017
* Description 	: 
*******************************************************************************/
void Convert_Time_To_Bcd(void)
{
	u8_Network_Date = Hex_to_Bcd(u8_Network_Date);
	u8_Network_Month = Hex_to_Bcd(u8_Network_Month);
	u8_Network_Year = Hex_to_Bcd(u8_Network_Year);
	u8_Network_Hr = Hex_to_Bcd(u8_Network_Hr);
	u8_Network_Min = Hex_to_Bcd(u8_Network_Min);
	u8_Network_Sec = Hex_to_Bcd(u8_Network_Sec);
}
/*******************************************************************************
* Function name : uint8_t	Hex_to_Bcd(uint8_t u8_tmpt)
* returns 		: status
* arg			: none
* Created by 	: Jithin P P	
* Date created 	: 11-05-2017
* Description 	: Function to convert hex values to bcd
*******************************************************************************/
uint8_t	Hex_to_Bcd(uint8_t u8_tmpt)
{
	uint8_t u8_t1,u8_t2,u8_nw_data;
	
	u8_t1 = u8_tmpt>>4;
	u8_t2 = u8_tmpt & 0x0f;
	u8_nw_data = (u8_t1 *10)+ u8_t2;
	return u8_nw_data;
}
/*******************************************************************************
* Function name : uint8_t u8_HTTP_Config_TCP(void)
* returns 	: status
* arg		: none
* Created by 	: 
* Date created 	: 
* Description 	: GPRS initialising
*******************************************************************************/
uint8_t u8_HTTP_Config_TCP(void)
{
	uint8_t u8_Status = 0;
	v_uart_str_send("AT+QHTTPCFG=\"sslctxid\",1\r",GSM_GPRS_CHANNEL);//Configure the SSLctxid as 1 

	u8_Status = u8_GSM_GPRS_reception_Handler(5000);
	if(u8_Status == GSM_SUCCESS)
	{

		v_uart_str_send("AT+QHTTPCFG=\"contextid\",1\r",GSM_GPRS_CHANNEL);//Configure the contextid as 1 
		u8_Status = u8_GSM_GPRS_reception_Handler(5000);
		if(u8_Status == GSM_SUCCESS)
		{
			v_uart_str_send("AT+QHTTPCFG=\"requestheader\",0\r",GSM_GPRS_CHANNEL);//Configure the Request header as 0 
			u8_Status = u8_GSM_GPRS_reception_Handler(5000);
			if(u8_Status == GSM_SUCCESS)
			{
				v_uart_str_send("AT+QHTTPCFG=\"responseheader\",0\r",GSM_GPRS_CHANNEL);//Configure the Response header as 0
				u8_Status = u8_GSM_GPRS_reception_Handler(5000);
			}
		}

	}
	return u8_Status;
}

/*******************************************************************************
* Function name : uint8_t u8_HTTP_Open(uint8_t * u8_url)
* returns 	: status
* arg		: uint8_t * u8_url
* Created by 	: 
* Date created 	: 
* Description 	: Open HTTP connection
*******************************************************************************/
uint8_t u8_HTTP_Open(uint8_t * u8_url)
{
	uint8_t u8_Status = 0;
	uint16_t u16_size = 0;
	u16_size = strlen(u8_url);
	sprintf((char *)u8_temp_buff,"AT+QHTTPURL=%d,60\r", u16_size);
	v_uart_str_send(u8_temp_buff, GSM_GPRS_CHANNEL);
	u8_Status = u8_GSM_GPRS_reception_Handler(5000);
	if(u8_Status == GSM_FILE_CONNECT)
	{
		v_uart_str_send(u8_url, GSM_GPRS_CHANNEL);
		u8_Status = u8_GSM_GPRS_reception_Handler(150000);
	}
	return u8_Status;						
}

/*******************************************************************************
* Function name : uint8_t u8_HTTP_GET(void)
* returns 	: status
* arg		: none
* Created by 	: 
* Date created 	: 
* Description 	: HTTP get 
*******************************************************************************/
uint8_t u8_HTTP_GET(void)
{	
	uint8_t u8_Status = 0;													
	v_uart_str_send("AT+QHTTPGET=600\r", GSM_GPRS_CHANNEL);
	u8_Status = u8_GSM_GPRS_reception_Handler(5000);
	u8_Status = u8_GSM_GPRS_reception_Handler(150000);
	if(u8_Status == GSM_HTTP_GET)
	{
		v_uart_str_send("AT+QHTTPREADFILE=\"OTA_BIN.bin\",600\r", GSM_GPRS_CHANNEL);
		u8_Status = u8_GSM_GPRS_reception_Handler(5000);
		u8_Status = u8_GSM_GPRS_reception_Handler(150000);

	}	
	return u8_Status;
}


			
/**************************************************************************************
Function name: void u8_comp_IMSI_IMEI(void)

Compare new IMEI & IMSI with data flash

**************************************************************************************

void u8_comp_IMSI_IMEI(void)
{
	char buffff[128];
	u8_Get_CCID_IMEI();
	sprintf(buffff, "\r\nIMEI %s \r\nIMSI %s \r\n", (char*)IMEI_num, (char*)IMSI_num);
	v_uart_str_send(buffff,USB_CHANNEL);
	sprintf(temp,"https://api.snailecloud.com/device/rfid/%s/%s", (char*)IMEI_num, (char*)IMSI_num);
	sprintf(buffff,"\r\nstrcmp : %d\r\n",strcmp((char*)D_Config_Packet.Member.u8_URL,temp));
	v_uart_str_send(buffff,USB_CHANNEL);
	if(strcmp((char*)D_Config_Packet.Member.u8_URL,temp)== 0 )
		 {
			v_uart_str_send("\r\nConflict - the device is already registered :",USB_CHANNEL);
			v_uart_str_send(temp,USB_CHANNEL);
			
		 }
		 else
		 {
			 
			strcpy((char *)D_Config_Packet.Member.u8_URL,temp);
			D_Config_Packet.Member.u32_Checksum_Value = u32_checksum_calculate(&D_Config_Packet.All[0], CONFIG_DF_DATA_SIZE - 4);
			u16_DConfig_DF_Write_Position = DCONFIG_DF_START_POSITION;
			Write_Dconfig_Data_Flash();
			
		 }
		 
		 
		 
		
}





*/





/*******************************************************************************
* Function name : uint8_t u8_GSM_PDP_Activate(void)
* returns 	: status
* arg		: none
* Created by 	: 
* Date created 	: 
* Description 	: GPRS initialising
*******************************************************************************/
uint8_t u8_GSM_PDP_Activate(uint8_t * u8_apn,uint8_t * u8_apnuser,uint8_t * u8_apnpass)
{
	uint8_t u8_Status = 0;
	v_uart_str_send("AT+QIDEACT=1\r",GSM_GPRS_CHANNEL); //Deactivate the PDP
	u8_Status = u8_GSM_GPRS_reception_Handler(150000);
	if((u8_Status == GSM_SUCCESS) || (u8_Status == GSM_FAIL))
	{
		sprintf((char *)u8_APN_Buf,"AT+QICSGP=1,1,\"%s\",\"%s\",\"%s\",1\r",u8_apn,u8_apnuser,u8_apnpass);
		v_uart_str_send((char *)u8_APN_Buf,GSM_GPRS_CHANNEL);
		u8_Status = u8_GSM_GPRS_reception_Handler(2000);
		if(u8_Status == GSM_SUCCESS)
		{
			v_uart_str_send("AT+QIACT=1\r",GSM_GPRS_CHANNEL);//Activate the PDP
			u8_Status = u8_GSM_GPRS_reception_Handler(150000);	
		}
	}
	return u8_Status;
}

/*******************************************************************************
* Function name : uint8_t u8_GSM_SSL_COnfig(void)
* returns 	: status
* arg		: none
* Created by 	: 
* Date created 	: 
* Description 	: GPRS initialising
*******************************************************************************/
uint8_t u8_GSM_SSL_Config(void)
{
	uint8_t u8_Status;
	v_uart_str_send("AT+QSSLCFG=\"cacert\",1,\"UFS:CA0\"\r",GSM_GPRS_CHANNEL);//Configure the SSL
	u8_Status = u8_GSM_GPRS_reception_Handler(150000);
	if(u8_Status == GSM_SUCCESS)
	{
		v_uart_str_send("AT+QSSLCFG=\"clientcert\",1,\"UFS:CC0\"\r",GSM_GPRS_CHANNEL);
		u8_Status = u8_GSM_GPRS_reception_Handler(150000);
		if(u8_Status == GSM_SUCCESS)
		{
			v_uart_str_send("AT+QSSLCFG=\"clientkey\",1,\"UFS:CK0\"\r",GSM_GPRS_CHANNEL);
			u8_Status = u8_GSM_GPRS_reception_Handler(150000);	
			if(u8_Status == GSM_SUCCESS)
			{
				v_uart_str_send("AT+QSSLCFG=\"ciphersuite\",1,0xFFFF\r",GSM_GPRS_CHANNEL);//Configure the ciphersuite
				u8_Status = u8_GSM_GPRS_reception_Handler(150000);	
				if(u8_Status == GSM_SUCCESS)
				{
					v_uart_str_send("AT+QSSLCFG=\"ignorelocaltime\",1,1\r",GSM_GPRS_CHANNEL);//Configure the ignorelocaltime
					u8_Status = u8_GSM_GPRS_reception_Handler(150000);
					if(u8_Status == GSM_SUCCESS)
					{
						sprintf((char *)u8_GSM_Data_String,"AT+QSSLCFG=\"seclevel\",1,%d\r",SSL_SEC_LEVEL);
						v_uart_str_send((char *)u8_GSM_Data_String,GSM_GPRS_CHANNEL);//Configure the security level as 2
						u8_Status = u8_GSM_GPRS_reception_Handler(150000);
						
					}
				}
			}
		}
	}
	return u8_Status;
}

/*******************************************************************************
* Function name : uint8_t u8_GSM_TCP_Activate(uint8_t * u8_apn,uint8_t * u8_apnuser,uint8_t * u8_apnpass)
* returns 	: status
* arg		: none
* Created by 	: 
* Date created 	: 
* Description 	: GPRS initialising
*******************************************************************************/
uint8_t u8_GSM_TCP_Activate(uint8_t * u8_apn,uint8_t * u8_apnuser,uint8_t * u8_apnpass)
{
	uint8_t u8_Status = 0;	
	u8_Status = Mqtt_SSL_Config();
	if(u8_Status == GSM_SUCCESS)
	{
		u8_Status = u8_HTTP_Config_TCP();
		if(u8_Status == GSM_SUCCESS)
		{
			u8_Status = u8_GSM_PDP_Activate(u8_apn, u8_apnuser, u8_apnpass);
		}
	}
	

	
	return u8_Status;
}


//for mqtt activation of ssl,configurations and pdp

///*******************************************************************************
//* Function name : uint8_t u8_MQTT_Activate(void)
//* returns 	: status
//* arg		: none
//* Created by 	: 
//* Date created 	: 
//* Description 	: GPRS initialising
//*******************************************************************************/
//uint8_t u8_MQTT_Activate(void)
//{
//	uint8_t u8_Status = 0;	
//	//	u8_Status = u8_MQTT_Configurations();
//		if(u8_Status == GSM_SUCCESS)
//		{
//			u8_Status = u8_MQTT_PDP_Activate();
//		}
	
	

	
//	return u8_Status;
//}

/*******************************************************************************

* Function name : uint8_t u8_Gsm_Wait_For_AT_response(void)

* returns 		: none

* arg 			: none

* Created by 	: Jomon Thomas

* Date created 	: 30-10-19

* Description 	: Power Up sequence1

*******************************************************************************/
uint8_t u8_Gsm_Wait_For_AT_response(uint32_t time)
{
	uint8_t u8_Status = 0;
	u32_Gsm_Retry_Tmo = time;
	
	do
	{
		v_uart_str_send("AT\r\n",GSM_GPRS_CHANNEL);
		u8_Status = u8_GSM_GPRS_reception_Handler(1000);
		if(u8_Status == GSM_SUCCESS)
			return u8_Status;
			
		v_delay_ms(1000);
		
	}while( u32_Gsm_Retry_Tmo > 0 );	  
}


/*******************************************************************************

* Function name : uint8_t u8_GPRS_Init(void)

* returns 		: status
* arg			: none

* Created by 	: Renjith Narayanan V

* Date created 	: 10/6/2016

* Description 	: Function to Initialise GPRS

*******************************************************************************/
uint8_t u8_Network_Select(void)
{
	uint16_t u16_i = 0;
	uint16_t u16_j = 0;
	uint16_t u16_pkt_num = 0;
	uint8_t u8_Status = 0;
	v_uart_str_send("AT+COPS=3,2\r\n",GSM_GPRS_CHANNEL);
	u8_Status = u8_GSM_GPRS_reception_Handler(1000);
	if(u8_Status == GSM_SUCCESS)
	{
		v_uart_str_send("AT+COPS?\r\n",GSM_GPRS_CHANNEL);
		u8_Status = u8_GSM_GPRS_reception_Handler(1000);
		if(u8_Status == GSM_SUCCESS)
		{	
		        
		        if(u8_Gsm_Rx_Status_Start_Counter < (GSM_GPRS_MAX_PACKET_COUNT - 1))
			u8_Gsm_Rx_Status_Start_Counter++;
		        else
			u8_Gsm_Rx_Status_Start_Counter = 0;
			
			if(vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][9] == '2')
			{
			        for(u16_i=12; vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][u16_i] != '"';u16_i++)	
				{
					u8_network_Location_buf[u16_j] = vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][u16_i];
					u16_j++;
				}
				u8_network_Location_buf[u16_j] = '\0';
				u16_i = u16_i + 2;
				u8_network_mode = vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][u16_i] - 0x30;
				if( u8_network_mode > 0)
				    u8_network_mode = 2;	
				else
				   u8_network_mode = 0;	

				u8_Status = GSM_SUCCESS;	
			}
			else
			  u8_Status = GSM_FAIL;
			  
		        if(u8_Status == GSM_SUCCESS)
			{
				sprintf((char *)u8_SSL_String,"AT+COPS=4,2,%s,%d\r",(char *)u8_network_Location_buf,u8_network_mode);
				v_uart_str_send((char *)u8_SSL_String,GSM_GPRS_CHANNEL);
				u8_Status = u8_GSM_GPRS_reception_Handler(30000);	
				if(u8_Status == GSM_SUCCESS)
				{
					return u8_Status;
					//v_uart_str_send("AT+COPS?\r\n",GSM_GPRS_CHANNEL);
			                //u8_Status = u8_GSM_GPRS_reception_Handler(1000);
				}
			}
		}			
	}	
	
	return u8_Status;
}

/*******************************************************************************

* Function name : uint8_t u8_Network_CFG(void)
* returns 		: status
* arg			: none

* Created by 	: Jithin PP

* Date created 	: 14/08/2017

* Description 	: Function to configure network settings

*******************************************************************************/
uint8_t u8_Network_CFG(void)
{
	uint8_t u8_Status = 0;	
	v_uart_str_send("AT+QCFG=\"roamservice\",255,1\r\n",GSM_GPRS_CHANNEL);//To enable data roaming service
	u8_Status = u8_GSM_GPRS_reception_Handler(600);//300ms needed
	if(u8_Status == GSM_SUCCESS)
	{
		v_uart_str_send("AT+QCFG=\"nwscanseq\",00\r\n",GSM_GPRS_CHANNEL);// Prefer WCDMA scan over// GSM for uc20 0,1
		u8_Status = u8_GSM_GPRS_reception_Handler(600);//300ms needed
		if(u8_Status == GSM_SUCCESS)
		{
			v_uart_str_send("AT+QCFG=\"nwscanmode\",0,1\r\n",GSM_GPRS_CHANNEL);//Set network mode WCDMA only
			u8_Status = u8_GSM_GPRS_reception_Handler(600);//300ms needed
			if(u8_Status == GSM_SUCCESS)
			{
				return u8_Status;
			}
		}
	}
	return u8_Status;
}


/*******************************************************************************

* Function name : uint8_t u8_Wait_For_NW_Connection(void)
* returns 		: status
* arg			: none

* Created by 	: Jomon

* Date created 	: 14/08/2019

* Description 	: Function to waiting and checking till newwork connected

*******************************************************************************/
uint8_t u8_Wait_For_NW_Connection(uint32_t time)
{
	uint8_t u8_Status = 0;
	u32_Gsm_Retry_Tmo = time;
	
	do{
		v_uart_str_send("AT+CGREG?\r\n",GSM_GPRS_CHANNEL);
		u8_Status = u8_GSM_GPRS_reception_Handler(1000);//300ms needed
		if(u8_Status == GSM_SUCCESS)
		{		
			if(u8_Gsm_Rx_Status_Start_Counter < (GSM_GPRS_MAX_PACKET_COUNT - 1))
				u8_Gsm_Rx_Status_Start_Counter++;
			else
				u8_Gsm_Rx_Status_Start_Counter = 0;	
				
			u8_Network_Status = vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][10] - 0x30;
			
			if((u8_Network_Status == 1) || (u8_Network_Status == 5))
				return u8_Status;
			else
				u8_Status = GSM_OPERATOR_ERROR;
		} 
		
		v_delay_ms(1000);
		    
	}
	while(u32_Gsm_Retry_Tmo > 0);
	
	return u8_Status;
}



/*******************************************************************************

* Function name : uint8_t u8_Wait_For_NW_Connection(void)
* returns 		: status
* arg			: none

* Created by 	: Jomon

* Date created 	: 14/08/2019

* Description 	: Function to waiting and checking till newwork connected

*******************************************************************************/
uint8_t u8_Wait_For_SIM_register(uint32_t time)
{
	uint8_t u8_Status = 0;
	uint16_t u16_i = 0, u16_j = 0;
	u32_Gsm_Retry_Tmo = time;
	
	do{
		v_uart_str_send("AT+CPIN?\r\n",GSM_GPRS_CHANNEL);
		u8_Status = u8_GSM_GPRS_reception_Handler(1000);//5s needed
		if(u8_Status == GSM_SUCCESS)
		{
			u16_i = 7;
			u16_j = 0;
			if(u8_Gsm_Rx_Status_Start_Counter < (GSM_GPRS_MAX_PACKET_COUNT - 1))
				u8_Gsm_Rx_Status_Start_Counter++;
			else
				u8_Gsm_Rx_Status_Start_Counter = 0;
				
			while(vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][u16_i] != 0x0d)
			{
				u8_SIM_Status[u16_j] = vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][u16_i];
				u16_i++;
				u16_j++;
			}
			u8_SIM_Status[u16_j] = '\0';
			if(strcmp((char *)u8_SIM_Status,"READY")==0)
			{
				//u8_GSM_Com_Retry_count = 0;
				u8_SIM_Insert = 1;
				return u8_Status;
			}
			else
			{
				u8_SIM_Insert = 0;
				u8_Status = GSM_FAIL;
			}
			
		}
		
		v_delay_ms(1000);
		    
	}
	while(u32_Gsm_Retry_Tmo > 0);
	
	return GSM_FAIL;
}



uint8_t u8_GPRS_Init(void)
{
	uint8_t u8_Status = 0;
	
	
	u8_Status = u8_Gsm_Wait_For_AT_response(14000);  //wait for 14 second
	if(u8_Status != GSM_SUCCESS)
		return u8_Status;
	
	v_uart_str_send("AT+QGMR\r\n",GSM_GPRS_CHANNEL);  //echo on  //OK
	//v_uart_str_send("AT+CGACT\r\n",GSM_GPRS_CHANNEL);  //arjun
	//v_uart_str_send("AT+CGDCONT\r\n",GSM_GPRS_CHANNEL);  //arjun
	u8_Status = u8_GSM_GPRS_reception_Handler(1000);	
		
	v_uart_str_send("ATE0\r\n",GSM_GPRS_CHANNEL);  //echo on  //OK
	u8_Status = u8_GSM_GPRS_reception_Handler(1000);
	if(u8_Status != GSM_SUCCESS)
		return u8_Status;
		
		v_uart_str_send("ATI\r\n",GSM_GPRS_CHANNEL);  
	u8_Status = u8_GSM_GPRS_reception_Handler(1000);
	if(u8_Status != GSM_SUCCESS)
		return u8_Status;
		
	u8_Status = u8_Wait_For_SIM_register(10000);  //wait for 10 second
	if(u8_Status != GSM_SUCCESS)
		return u8_Status;
		
	
	if(u8_GSM_Init_Retry_count > 0) //need to check
	{
		//auto modeselect
		v_uart_str_send("AT+COPS=0\r\n",GSM_GPRS_CHANNEL);  //auto mode
		u8_Status = u8_GSM_GPRS_reception_Handler(5000);
		if(u8_Status != GSM_SUCCESS)
			return u8_Status;
			
		
		//wait for network join
		u8_Status = u8_Wait_For_NW_Connection(300000);
		if(u8_Status != GSM_SUCCESS)
			return u8_Status;
	}
	else
	{	
		//wait for network join
		u8_Status = u8_Wait_For_NW_Connection(180000);  //180 second
		if(u8_Status != GSM_SUCCESS)
			return u8_Status;
	}
	
//	u8_Status = u8_Network_Select();  //need to chek
//	if(u8_Status != GSM_SUCCESS)
//		return u8_Status;
	
		
//	//wait for network join	
//	u8_Status = u8_Wait_For_NW_Connection(180000);  //180 second max wait time
//	if(u8_Status != GSM_SUCCESS)
//		return u8_Status;		
	
	
	
	//v_uart_str_send("AT+QICSGP=1,1,\"jionet\","","",1",GSM_GPRS_CHANNEL); 
	//u8_Status = u8_GSM_GPRS_reception_Handler(1000);
	

				
	return u8_Status;	
}
/***************************************************************************************************************/





/******************************************************************************************/

/*******************************************************************************

* Function name : uint8_t u8_Get_Network_Signal_Strength(void)

* returns 		: status

* arg 			: none

* Created by 	: Aarathy Ramesh

* Date created 	: 13-06-2017

* Description 	: return network signal strength in decimal value (u32_Network_Strength)

*******************************************************************************/
uint8_t u8_Get_Network_Signal_Strength(void)
{
	uint8_t u8_Status;
	uint16_t u16_i = 0;
	v_uart_str_send("AT+CSQ\r",GSM_GPRS_CHANNEL);					
	u8_Status = u8_GSM_GPRS_reception_Handler(2000);
	if(u8_Status == GSM_SUCCESS)
	{
		u16_i =6;
		if(u8_Gsm_Rx_Status_Start_Counter < (GSM_GPRS_MAX_PACKET_COUNT - 1))
			u8_Gsm_Rx_Status_Start_Counter++;
		else
			u8_Gsm_Rx_Status_Start_Counter = 0;
		u32_Network_Strength = vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][u16_i] - 0x30;
		while(vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][u16_i +1] != ',')
		{
			u16_i++;
			u32_Network_Strength = (u32_Network_Strength) * 10;
			u32_Network_Strength = (u32_Network_Strength + (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][u16_i]-0x30) );
		}
	}
	//u8_status = u8_GSM_GPRS_reception_Handler(2000);
	return u8_Status;
}
/*******************************************************************************

* Function name : uint8_t u8_Get_Network_Time(void)

* returns 		: status

* arg 			: none

* Created by 	: Aarathy Ramesh

* Date created 	: 13-06-2017

* Description 	: Function to get network time

*******************************************************************************/
uint8_t u8_Get_Network_Time(void)
{
	uint8_t u8_Status = 0, u8_temp1 = 0, u8_temp2 = 0;
	v_uart_str_send("AT+QLTS=1\r",GSM_GPRS_CHANNEL);							
	u8_Status = u8_GSM_GPRS_reception_Handler(1000);
	if(u8_Status == GSM_SUCCESS)
	{
		if(u8_Gsm_Rx_Status_Start_Counter < (GSM_GPRS_MAX_PACKET_COUNT - 1))
			u8_Gsm_Rx_Status_Start_Counter++;
		else
			u8_Gsm_Rx_Status_Start_Counter = 0;
		if(vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][8] != '"') 
		{
			u8_Network_Year = vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][8] - 0x30;
			u8_Network_Year = (u8_Network_Year<<4) | (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][9] - 0x30);
			u8_Network_Month = vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][11] - 0x30;
			u8_Network_Month = (u8_Network_Month<<4) | (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][12] - 0x30);
			u8_Network_Date = vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][14] - 0x30;
			u8_Network_Date = (u8_Network_Date<<4) | (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][15] - 0x30);
			u8_Network_Hr = vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][17] - 0x30;
			u8_Network_Hr = (u8_Network_Hr<<4) | (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][18] - 0x30);
			u8_Network_Min = vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][20] - 0x30;
			u8_Network_Min = (u8_Network_Min<<4) | (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][21] - 0x30);
			u8_Network_Sec = vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][23] - 0x30;
			u8_Network_Sec = (u8_Network_Sec<<4) | (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][24] - 0x30);
			
			u8_temp1 = (u8_Network_Year & 0xF0) >> 4;
			if((u8_temp1 < 0x1) || (u8_temp1 > 0x9))
				return GSM_FAIL;
			u8_temp2 = (u8_Network_Year & 0x0F);
			if((u8_temp2 > 0x9))
				return GSM_FAIL;
				
				
			u8_temp1 = (u8_Network_Month & 0xF0) >> 4;
			u8_temp2 = (u8_Network_Month & 0x0F);
			if(((u8_temp1 == 0) && (u8_temp2 > 0x9)) || ((u8_temp1 == 1) && (u8_temp2 > 0x2)) \
			|| (u8_temp1 > 1))
				return GSM_FAIL;
				
			u8_temp1 = (u8_Network_Date & 0xF0) >> 4;
			u8_temp2 = (u8_Network_Date & 0x0F);
			if(((u8_temp1 == 3) && (u8_temp2 > 1)) || (u8_temp1 > 3) || (u8_temp2 > 0x9))
				return GSM_FAIL;
				
			u8_temp1 = (u8_Network_Hr & 0xF0) >> 4;
			u8_temp2 = (u8_Network_Hr & 0x0F);
			if(((u8_temp1 == 2) && (u8_temp2 > 4)) || (u8_temp1 > 2) || (u8_temp2 > 0x9))
				return GSM_FAIL;
				
			u8_temp1 = (u8_Network_Min & 0xF0) >> 4;
			u8_temp2 = (u8_Network_Min & 0x0F);
			if(((u8_temp1 == 5) && (u8_temp2 > 9)) || (u8_temp1 > 5) || (u8_temp2 > 0x9))
				return GSM_FAIL;
				
			u8_temp1 = (u8_Network_Sec & 0xF0) >> 4;
			u8_temp2 = (u8_Network_Sec & 0x0F);
			if(((u8_temp1 == 5) && (u8_temp2 > 9)) || (u8_temp1 > 5) || (u8_temp2 > 0x9))
				return GSM_FAIL;
		}
		else
			return GSM_FAIL;

	}
	return u8_Status;
}




/*******************************************************************************

* Function name : void RTC_Update_From_GSM()

* returns 		: none

* arg 			: none

* Created by 	: jomon

* Date created 	: 07-05-19

* Description 	: GSM RTC time

*******************************************************************************/
void RTC_Update_From_GSM(void)
{
	uint8_t u8_Status = 0;
	uint16_t u16_i = 0;
	//uint8_t time_buf[35];
	
	v_uart_str_send("AT+CTZU=3\r\n",GSM_GPRS_CHANNEL); 
	u8_GSM_GPRS_reception_Handler(1000);
	
	v_delay_ms(10000);
	
	v_uart_str_send("AT+QLTS=1\r\n",GSM_GPRS_CHANNEL);   
	v_delay_ms(1000);					
	u8_GSM_GPRS_reception_Handler(1000);			
	
	/*v_uart_str_send("AT+CCLK?\r\n",GSM_GPRS_CHANNEL);	
	
	u8_Status = u8_GSM_GPRS_reception_Handler(1000);     
	if(u8_Status == GSM_SUCCESS)
	{
		if(u8_Gsm_Rx_Status_Start_Counter < (GSM_GPRS_MAX_PACKET_COUNT - 1))
			u8_Gsm_Rx_Status_Start_Counter++;
		else
			u8_Gsm_Rx_Status_Start_Counter = 0;
			
		if(vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][7] == '"')
		{
			D_Config_Packet.Member.u16_current_year = ( (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][8] - 0x30) * 10 ) + (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][9] - 0x30);
    			D_Config_Packet.Member.u8_current_month = ( (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][11] - 0x30) * 10 ) + (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][12] - 0x30);
    			D_Config_Packet.Member.u8_current_date = ( (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][14] - 0x30) * 10 ) + (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][15] - 0x30);
    			D_Config_Packet.Member.u8_current_time_hr = ( (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][17] - 0x30) * 10 ) + (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][18] - 0x30);
    			D_Config_Packet.Member.u8_current_time_min = ( (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][20] - 0x30) * 10 ) + (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][21] - 0x30);
    			D_Config_Packet.Member.u8_current_time_sec = ( (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][23] - 0x30) * 10 ) + (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][24] - 0x30);
			
			v_update_date_time();
			
			sprintf((char *)time_buf, "AT+CCLK=\"%02d/%02d/%02d,%02d:%02d:%02d+00\"\r\n", D_Config_Packet.Member.u16_current_year, D_Config_Packet.Member.u8_current_month, \
			D_Config_Packet.Member.u8_current_date, D_Config_Packet.Member.u8_current_time_hr, D_Config_Packet.Member.u8_current_time_min, \
			D_Config_Packet.Member.u8_current_time_sec);
			
			for(u16_i=9;u16_i<29;u16_i++)
			{
			clock[u16_i-9]=(char *)time_buf[u16_i];
			}

		
		}	
	}*/
	
}



/*******************************************************************************

* Function name : void GSM_Initial_Configuration(void)

* returns 		: none

* arg 			: none

* Created by 	: Vishnu Soman

* Date created 	: 20-07-22

* Description 	: GSM_Initial_Configuration

*******************************************************************************/
void GSM_Initial_Configuration(void)
{
	uint8_t u8_Status = 0;
	
	
	//DEBUG_LED_ON();
    	v_Gsm_Hardware_Init();
	
	u8_GPRS_Init();
	
	u8_Get_CCID_IMEI();
	
	
	
	RTC_Update_From_GSM();			//basil
	
	v_uart_str_send("AT+QICSGP=1,1,\"postm2m.lu\",\"\",\"\",1\r\n",GSM_GPRS_CHANNEL); 
	//u8_Status = u8_GSM_GPRS_reception_Handler(1000);
	
	v_uart_str_send("AT+QIACT=1\r\n",GSM_GPRS_CHANNEL); 
	u8_Status = u8_GSM_GPRS_reception_Handler(10000);
	
	Mqtt_SSL_Update_to_RAM();
	v_delay_ms(2000);
	
	Mqtt_SSL_Config();
	v_delay_ms(2000);
	
	u8_Wait_For_mqtt_Connection();
	v_delay_ms(1000);
	Subscribe_to_AWS_Topics();
	
	v_Gsm_Power_Down();
	GSM_PDP_STATUS_LED = LED_ON;
	GSM_PDP_STATUS_LED = LED_OFF;
}





/*******************************************************************************

* Function name : void RTC_Update_To_GSM(void)

* returns 		: none

* arg 			: none

* Created by 	: jomon

* Date created 	: 07-05-19

* Description 	: GSM RTC time

*******************************************************************************/
int RTC_Update_To_GSM(void)
{
	uint8_t time_buf[35];
	uint8_t u8_Status = 0;
	
	u8_Status = u8_Gsm_Wait_For_AT_response(14000);
	if(u8_Status != GSM_SUCCESS)
		return 1;
		
	memset(time_buf, 0x00, sizeof(time_buf));
	sprintf((char *)time_buf, "AT+CCLK=\"%02d/%02d/%02d,%02d:%02d:%02d+00\"\r\n", D_Config_Packet.Member.u16_current_year, D_Config_Packet.Member.u8_current_month, \
			D_Config_Packet.Member.u8_current_date, D_Config_Packet.Member.u8_current_time_hr, D_Config_Packet.Member.u8_current_time_min, \
			D_Config_Packet.Member.u8_current_time_sec);
	
	
	v_uart_str_send((char *)time_buf,GSM_GPRS_CHANNEL);
	u8_Status = u8_GSM_GPRS_reception_Handler(1000);
	if(u8_Status == GSM_SUCCESS)
		return 0;
	else
		return 1;
		
}



/*******************************************************************************

* Function name : uint8_t u8_HTTP_Open_Connection(uint8_t u8_url,uint8_t * u8_port)

* returns 		: status

* arg 			: Pass url end point and port number

* Created by 	: Aarathy Ramesh

* Date created 	: 13-06-2017

* Description 	: Function to HTTP Connection open

*******************************************************************************/
uint8_t u8_HTTP_Open_Connection(uint8_t * u8_url,uint8_t * u8_port)
{
	uint8_t u8_Status, u8_i;
	uint16_t u16_str_length = ((uint16_t)u8_RFID_New_Data_Count * 29);
	if(u8_GSM_Data_Format_Sel == GSM_SEL_RFID_DATA)
	u16_str_length = ((uint16_t)u8_RFID_New_Data_Count * 29);
	else
	{
		sprintf((char *)u8_GSM_Data_String, "?bcl=%02d&range=%02d",u16_battery_charge, u32_Network_Strength);
		u16_str_length = strlen((char *)u8_GSM_Data_String);
	}
	u16_str_length += strlen((const char *)u8_url);
	sprintf((char *)u8_GSM_Data_String,"AT+QHTTPURL=%d,%s\r",u16_str_length, HTTP_CONNECTION_TMO);
	v_uart_str_send((char *)u8_GSM_Data_String,GSM_GPRS_CHANNEL);
	u8_Status = u8_GSM_GPRS_reception_Handler(500);																
	if(u8_Status == GSM_FILE_CONNECT)
	{
		v_uart_str_send((char *)u8_url,GSM_GPRS_CHANNEL);
		//Application Specific
		v_GSM_Data_Format_Send();
		u8_Status = u8_GSM_GPRS_reception_Handler(150000);
	}
	if(u8_Status == GSM_SUCCESS)
	u8_TCP_Status = GSM_SSL_OPEN;
	else
	u8_TCP_Status = GSM_SSL_CLOSED;
	return u8_Status;
	
}






/*******************************************************************************

* Function name : uint8_t u8_GSM_HTTP_Data_Write(uint8_t *u8_Data, uint32_t u32_tmo)

* returns 		: status

* arg 			: none

* Created by 	: Aarathy Ramesh

* Date created 	: 9-5/2017

* Description 	: Function to store HTTP Send Data

*******************************************************************************/
uint8_t u8_GSM_HTTP_Data_Write(uint8_t *u8_Data, uint16_t u16_data_length)
{
	uint8_t u8_Status;
	sprintf((char *)u8_GSM_Data_String, "%d", u16_data_length);
       v_uart_str_send("AT+QHTTPPOST=",GSM_GPRS_CHANNEL);//Call the post command with size of post and timeout //73
       v_uart_str_send((char *)u8_GSM_Data_String,GSM_GPRS_CHANNEL);
       v_uart_str_send(",",GSM_GPRS_CHANNEL);
       v_uart_str_send(HTTP_INPUT_TMO,GSM_GPRS_CHANNEL);
       v_uart_str_send(",",GSM_GPRS_CHANNEL);
       v_uart_str_send(HTTP_RESPONSE_TMO, GSM_GPRS_CHANNEL);
       v_uart_str_send("\r\n",GSM_GPRS_CHANNEL);
       u8_Status = u8_GSM_GPRS_reception_Handler(150000);
       if(u8_Status == GSM_FILE_CONNECT)
       {
		v_byte_array_send(u8_Data, u16_data_length, GSM_GPRS_CHANNEL);
	        u8_Status = u8_GSM_GPRS_reception_Handler(1000);
        }
       return u8_Status;
}

/*******************************************************************************

* Function name : uint8_t u8_Update_SSL_Crt_To_UFS(void)

* returns 		: status

* arg 			: none

* Created by 	: Jihin P P

* Date created 	: 9-5/2017

* Description 	: Function to store SSL certificate file to Uc20 UFS

*******************************************************************************/
uint8_t u8_Update_SSL_Crt_To_UFS(void)
{
	uint8_t u8_status = 0;
	//u16_CA0_length = strlen((const char *)CA0_Buf);
	//u16_CC0_length	= strlen((const char *)CC0_Buf);	
	//u16_CK0_length	= strlen((const char *)CK0_Buf);
	u16_CA0_length  = Http_CA0_TOTAL_LENGTH;
	u16_CC0_length  = Http_CC0_TOTAL_LENGTH;
	u16_CK0_length  = Http_CK0_TOTAL_LENGTH;
	
	v_uart_str_send("AT+QFLDS=\"UFS\"\r",GSM_GPRS_CHANNEL);	
	u8_status = u8_GSM_GPRS_reception_Handler(1000); 
	if(u8_status == GSM_SUCCESS)					
	{								
		v_uart_str_send("AT+QFDEL=\"*\"\r",GSM_GPRS_CHANNEL);		// Delete all data in RAM
		u8_status = u8_GSM_GPRS_reception_Handler(1000);
		if(u8_status == GSM_SUCCESS)
		{	
			sprintf((char *)u8_SSL_String,"AT+QFUPL=\"CA0\",%d\r",u16_CA0_length);			
			v_uart_str_send((char *)u8_SSL_String,GSM_GPRS_CHANNEL);// 1758 for all aws server
			u8_status = u8_GSM_GPRS_reception_Handler(1000);
			if(u8_status == GSM_FILE_CONNECT)
			{
				////v_byte_array_send((uint8_t *)CA0_Buf,u16_CA0_length,GSM_GPRS_CHANNEL);
				v_byte_array_send(u8_comodo_buff_1,418,1);  
		                v_byte_array_send(u8_comodo_buff_2,455,1);   
		                v_byte_array_send(u8_comodo_buff_3,455,1);	
		                v_byte_array_send(u8_comodo_buff_4,347,1);
		                v_byte_array_send(u8_comodo_buff_5,455,1);
		                v_byte_array_send(u8_comodo_buff_6,455,1);
		                v_byte_array_send(u8_comodo_buff_7,262,1);
				u8_status = u8_GSM_GPRS_reception_Handler(5000);
				if(u8_status == GSM_SUCCESS)
				{
					sprintf((char *)u8_SSL_String,"AT+QFUPL=\"CC0\",%d\r",u16_CC0_length);
					v_uart_str_send((char *)u8_SSL_String,GSM_GPRS_CHANNEL);//1220 for neona,1224 for bluelog test,1240 for bluelog production
					u8_status = u8_GSM_GPRS_reception_Handler(1000);
					if(u8_status == GSM_FILE_CONNECT)
					{
				            ////v_byte_array_send((uint8_t *)CC0_Buf,u16_CC0_length,GSM_GPRS_CHANNEL);//1220 for neona,1224 for bluelog test, 1240for bluelog production
					    v_byte_array_send(u8_snailecl_buff_1,483,1); 
			                    v_byte_array_send(u8_snailecl_buff_2,455,1); 
			                    v_byte_array_send(u8_snailecl_buff_3,455,1);
			                    v_byte_array_send(u8_snailecl_buff_4,455,1);
			                    v_byte_array_send(u8_snailecl_buff_5,108,1); 	
					    u8_status = u8_GSM_GPRS_reception_Handler(5000);
						if(u8_status == GSM_SUCCESS)
						{
							sprintf((char *)u8_SSL_String,"AT+QFUPL=\"CK0\",%d\r",u16_CK0_length);
							v_uart_str_send((char *)u8_SSL_String,GSM_GPRS_CHANNEL);//1679 for neona,1675 for bluelog test,1702 for bluelog production
							u8_status = u8_GSM_GPRS_reception_Handler(1000);
							if(u8_status == GSM_FILE_CONNECT)
							{
								////v_byte_array_send((uint8_t *)CK0_Buf,u16_CK0_length,GSM_GPRS_CHANNEL);  //1679 for neona,1675 for bluelog test,1702 for bluelog production
								v_byte_array_send(u8_snaile_key_buff_1,422,1);  //423
				                                v_byte_array_send(u8_snaile_key_buff_2,455,1);	//455
				                                v_byte_array_send(u8_snaile_key_buff_3,455,1);
				                                v_byte_array_send(u8_snaile_key_buff_4,455,1);
				                                v_byte_array_send(u8_snaile_key_buff_5,455,1);
				                                v_byte_array_send(u8_snaile_key_buff_6,455,1);
				                                v_byte_array_send(u8_snaile_key_buff_7,455,1);
				                                v_byte_array_send(u8_snaile_key_buff_8,95,1); 
								u8_status = u8_GSM_GPRS_reception_Handler(5000);
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

* Function name : void v_gsm_gprs_rx_isr(void)

* returns 		: none

* arg 			: none

* Created by 	: Aarathy Ramesh

* Date created 	: 12/03/2014

* Description 	: GSM/GPRS Modem Reception ISR Function

*******************************************************************************/
void v_GSM_Rx_Isr(uint8_t u8_gsm_gprs_rx)
{
	if(vu8_Recv_flag == 1)
	{
		if(first_time_provisioning == 1)//only done once in the beginning of device run
		{
			memset(Certificate_buffer, 0x00, sizeof(Certificate_buffer));
			u16_sub_pkt_counter = 0;
		}
		else
		{
			memset(Subscribed_data, 0x00, sizeof(Subscribed_data));
			u16_sub_pkt_counter = 0;
		}
		vu8_Recv_flag = 2;
	}
	else if(vu8_Recv_flag == 2)
	{
		//vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Pkt_Counter][u16_Gsm_Cur_Buffer_Count++] = u8_gsm_gprs_rx;//u8_gsm_gprs_pkt_counter			
		if(first_time_provisioning == 1)//only done once in the beginning of device run
		{
			Certificate_buffer[u16_sub_pkt_counter++] = u8_gsm_gprs_rx;
		}
		else
		{
			Subscribed_data[u16_sub_pkt_counter++] = u8_gsm_gprs_rx;
		}
		
	}
	else
	{
			
		vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Pkt_Counter][u16_Gsm_Cur_Buffer_Count++] = u8_gsm_gprs_rx;//u8_gsm_gprs_pkt_counter			
		
		if(((vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Pkt_Counter][u16_Gsm_Cur_Buffer_Count-1] == 0x0A)&& (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Pkt_Counter][u16_Gsm_Cur_Buffer_Count-2] == 0x0d))\
		|| ((vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Pkt_Counter][u16_Gsm_Cur_Buffer_Count-1]==0x20) && (vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Pkt_Counter][u16_Gsm_Cur_Buffer_Count-2]=='>')))
		// Detect end of GSM packet ( \r\n or SPACE>)
		{	
			
			//vu8_Recv_flag = 0;
			vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Pkt_Counter][u16_Gsm_Cur_Buffer_Count] = 0; 
			vu8_GSM_Packet_Status[u8_Gsm_Rx_Pkt_Counter] = (uint8_t)u16_Gsm_Cur_Buffer_Count; //GSM_GPRS_PACKET_FULL
			u16_Gsm_Cur_Buffer_Count = 0;
			u8_Gsm_Rx_Pkt_Counter++;
			if(u8_Gsm_Rx_Pkt_Counter == GSM_GPRS_MAX_PACKET_COUNT)
				u8_Gsm_Rx_Pkt_Counter = 0;					
		}
	
	}

}




/*******************************************************************************

* Function name : void v_Gsm_Hardware_Init(void)

* returns 		: none

* arg 			: none

* Created by 	: Jithin P P

* Edited by 	: Jomon Thomas

* Date created 	: 07-05-17

* Description 	: Power Up sequence

*******************************************************************************/
void v_Gsm_Hardware_Init(void)
{
	u8_Gsm_Rx_Pkt_Counter = 0;
	u8_Gsm_Rx_Status_Counter = 0;
	u8_Gsm_Rx_Status_Start_Counter = 0;
	u16_Gsm_Cur_Buffer_Count = 0;
	u16_Recv_Data_Count = 0;
	 memset(vu8_Gsm_Rx_Buffer, 0x00, sizeof(vu8_Gsm_Rx_Buffer));
memset(vu8_GSM_Packet_Status, 0x00, sizeof(vu8_GSM_Packet_Status));
    GSM_POWER_ENABLE	= 1;       //GSM-P-EN (Enable input (1 enabled, 0 disabled))
    PS_SYNC		= 1;        //PS-SYNC(Enable/disable power save mode (1 disabled, 0 enabled))
    GSM_POWER_KEY = 0;	 
    GSM_RESET = 0;
    /* Power Section End */
    
    v_delay_ms(500);				// Delay for Power Supply to stabilise
	
    GSM_POWER_KEY = 1;	 			// GSM power key is Enabl
    v_delay_ms(900);				// 150 mSec delay
    GSM_POWER_KEY = 0;	 			// GSM power key is Disabled
   
    //GSM_WAKEUP_IN = 1;				// GSM Wake up command
    //GSM_W_DISABLE = 1;				// GSM wireless disable is disabled	
    
    u8_GSM_Status = GSM_INIT;		// Set GSM status for initialisation
}  



/*******************************************************************************

* Function name : void v_Gsm_Power_Down(void)

* returns 		: none

* arg 			: none

* Created by 	: Jithin P P

* Date created 	: 07-05-17

* Description 	: GSM Power down

*******************************************************************************/
void v_Gsm_Power_Down(void)
{
	R_UART1_Stop();          //GSM Uart OFF
	
	GSM_POWER_KEY = 1;	 			// GSM power key is Enabl
    	v_delay_ms(700);				// 150 mSec delay
    	GSM_POWER_KEY = 0;	 			// GSM power key is Disabled
	
	GSM_POWER_ENABLE = 0;
	PS_SYNC		= 0;
	u8_SIM_Insert = 0;
	u8_GSM_Status = GSM_POWERDOWN;
	u8_PDP_Status = GSM_PDP_DEACTIVATED;
	u8_TCP_Status = GSM_SSL_CLOSED;
	v_delay_ms(2000); 
}
/*******************************************************************************

* Function name : void v_GSM_Timeout_Handler(void)

* returns 		: none

* arg 			: none

* Created by 	: Jithin P P

* Date created 	: 07-05-17

* Description 	: GSM time out Handler

*******************************************************************************/
void v_GSM_Timeout_Handler(void)
{
	if(u32_Gsm_At_Cmd_Tmo)
		u32_Gsm_At_Cmd_Tmo --;
		
	if(u32_Gsm_Retry_Tmo)
		u32_Gsm_Retry_Tmo --;
		
}



/*******************************************************************************

* Function name :uint8_t u8_Get_CCID_IMEI(void)

* returns 	: u8_Status

* arg 		: none

* Created by 	: Vishnu Soman

* Date created 	: 16-10-21

* Description 	: Get CCID and IMEI.

*******************************************************************************/
uint8_t u8_Get_CCID_IMEI(void)

{
	uint8_t u8_Status,i= 0;
	char *t;
	
	
		
	v_uart_str_send("AT+CGSN\r",GSM_GPRS_CHANNEL);					
	u8_Status = u8_GSM_GPRS_reception_Handler(15000);
	if(u8_Status == GSM_SUCCESS)
		{		
			if(u8_Gsm_Rx_Status_Start_Counter < (GSM_GPRS_MAX_PACKET_COUNT - 1))
				u8_Gsm_Rx_Status_Start_Counter++;
			else
				u8_Gsm_Rx_Status_Start_Counter = 0;
				    //IMEI_num+1
			strcpy((char*)IMEI_num,(char*)vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter]);
			//IMEI_num[0]='0';
			t = strchr((char*)IMEI_num,'\r');
			if(*t == '\r')
			{
				*t = '\0';
			}
			
		}
	
		
	v_uart_str_send("AT+QCCID\r",GSM_GPRS_CHANNEL);	
	//v_delay_ms(1000);                                       //arjun
	
	//v_uart_str_send("AT+CGDCONT\r",GSM_GPRS_CHANNEL);         //arjun
	//v_delay_ms(1000);                                      //arjun

	//v_uart_str_send("AT+CGACT\r",GSM_GPRS_CHANNEL);	       //arjun
	//v_delay_ms(1000);                                      //arjun
	u8_Status = u8_GSM_GPRS_reception_Handler(15000);
	if(u8_Status == GSM_SUCCESS)
		{		
			if(u8_Gsm_Rx_Status_Start_Counter < (GSM_GPRS_MAX_PACKET_COUNT - 1))
				u8_Gsm_Rx_Status_Start_Counter++;
			else
				u8_Gsm_Rx_Status_Start_Counter = 0;
				  //for post sim  CCID_num+1
			strcpy((char*)CCID_num,(char*)&vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Status_Start_Counter][8]);
			//CCID_num[0]='0';
			//CCID_num[19]='\0';
			CCID_num[21]='\0';
			t = strchr((char*)CCID_num,'\r');
			if(*t=='\r')
			{
				*t = '\0';
			}
			
			
		}
		
		
		return u8_Status;
}



/*******************************************************************************

* Function name : uint8_t u8_GSM_GPRS_reception_Handler(uint32_t u32_tmo)

* returns 		: none

* u16_tmo 		: Wait time for reception
* u8_pkt_num 	: Store packet number to the pointer

* Created by 	: Aarathy Ramesh

* Date created 	: 12/03/2014

* Description 	: GSM/GPRS Modem Reception Data Handler

*******************************************************************************/
uint8_t u8_GSM_GPRS_reception_Handler(uint32_t u32_tmo)
{
	uint16_t u16_i = 0, u16_j = 0, u8_count = 0,u16_k = 0;	
	uint8_t u8_status=0,u8_q = 0;
	uint8_t u8_tmp = 0;//,first_time_provisioning = 0;
	uint16_t u16_pkt_num = 0;
	
	u32_Gsm_At_Cmd_Tmo = u32_tmo;		// Store time out value of AT command to handler variable
	u8_Gsm_Rx_Status_Start_Counter = u8_Gsm_Rx_Status_Counter;
	while(u32_Gsm_At_Cmd_Tmo)			// Loop until time out happens
	{
		if(vu8_GSM_Packet_Status[u8_Gsm_Rx_Status_Counter])
		{
			u16_pkt_num = u8_Gsm_Rx_Status_Counter;
			u8_count = vu8_GSM_Packet_Status[u8_Gsm_Rx_Status_Counter];
			for(u16_i = 0; u16_i < u8_count; u16_i++)
			
			{
				u8_GSM_Response_Identifier[u16_i] = vu8_Gsm_Rx_Buffer[u16_pkt_num][u16_i];
				
				if((u8_GSM_Response_Identifier[u16_i] == ':') || (u8_GSM_Response_Identifier[u16_i] == '\r')|| (u8_GSM_Response_Identifier[u16_i] == '>'))
				{
					if(u8_GSM_Response_Identifier[u16_i] == '>')
					{
						u8_GSM_Response_Identifier[u16_i + 1] = 0;
						u16_i++;
					}
					
					
					else
					u8_GSM_Response_Identifier[u16_i] = 0;
					
					break;
				}
				
			}
			
			u8_GSM_Response_Identifier[u16_i]  = '\0';
			
			vu8_GSM_Packet_Status[u8_Gsm_Rx_Status_Counter] = 0;
			
			u8_Gsm_Rx_Status_Counter++;
			
			if(u8_Gsm_Rx_Status_Counter == GSM_GPRS_MAX_PACKET_COUNT)
				u8_Gsm_Rx_Status_Counter = 0;		
			
			if(strcmp((char *)u8_GSM_Response_Identifier,"OK") == 0)
			{
				return 	GSM_SUCCESS;	
			}
			
			/********************************MQTT related stuffs starts here***************************************/
			
			
			if(strcmp((char *)u8_GSM_Response_Identifier,"+QMTRECV") == 0 )
			{
				
			
				MQTT_RECV_FLAG = 1;
				
				
				if(first_time_provisioning == 1)//only done once in the beginning of device run
				{
					//strcpy((char*)Certificate_buffer,(char*)&vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Pkt_Counter-1][145]);
					Provisioning_And_Registration(Certificate_buffer);
					first_time_provisioning = 2;
				}
				else
				{
					strcpy((char*)Subscribed_data,(char*)&vu8_Gsm_Rx_Buffer[u8_Gsm_Rx_Pkt_Counter-1][43]);//43 for config 
					u8_mqtt_status = MQTT_Reception_Handler(Subscribed_data);
					
				}
				
				
				
			}
			
			
			else if(strcmp((char *)u8_GSM_Response_Identifier,"+QMTSTAT") == 0)
			{
				u16_i =10;
				u16_j = 0;
				while(vu8_Gsm_Rx_Buffer[u16_pkt_num][u16_i] != ',')
				{
					MQT_CMD_RES[u16_j] = vu8_Gsm_Rx_Buffer[u16_pkt_num][u16_i];
					u16_i++;
					u16_j++;
				}
				MQT_CMD_RES[u16_j] = '\0';
				if(strcmp((char *)MQT_CMD_RES,"1") == 0)
				{	
					u8_Wait_For_mqtt_Connection();
					u8_status = MQTT_STAT_FAIL;
					return 	u8_status;
					
				}
				else
				{
					u8_status = MQTT_PDP_FAIL_FLAG;//status fail
					u8_status = GSM_FAIL;
					return 	u8_status;
				}
			}
			
			
			
			else if(strcmp((char *)u8_GSM_Response_Identifier,"+QMTOPEN") == 0)
			{
				u16_i =10;
				u16_j = 0;
				while(vu8_Gsm_Rx_Buffer[u16_pkt_num][u16_i] != '\r')
				{
					MQT_CMD_RES[u16_j] = vu8_Gsm_Rx_Buffer[u16_pkt_num][u16_i];
					u16_i++;
					u16_j++;
				}
				MQT_CMD_RES[u16_j] = '\0';
				if(strcmp((char *)MQT_CMD_RES,"1,0") == 0)
				{
					MQTT_OPEN_FLAG = 1 ;
					u8_status = MQTT_OPEN_FLAG;
					return 	u8_status;
					
				}
				else
				{
					u8_status = MQTT_PDP_FAIL_FLAG;
					u8_status = GSM_FAIL;
					return 	u8_status;
				}
			}
			
			else if(strcmp((char *)u8_GSM_Response_Identifier,"+QMTCONN") == 0)
			{
				u16_i =12;
				u16_j = 0;
				while(vu8_Gsm_Rx_Buffer[u16_pkt_num][u16_i] != '\r')
				{
					MQT_CMD_RES[u16_j] = vu8_Gsm_Rx_Buffer[u16_pkt_num][u16_i];
					u16_i++;
					u16_j++;
				}
				MQT_CMD_RES[u16_j] = '\0';
				if(strcmp((char *)MQT_CMD_RES,"0,0") == 0)
				{
					MQTT_CONN_SUCCESS_FLAG = 1 ;
					u8_status = MQTT_CONN_SUCCESS_FLAG;
					return 	u8_status;
					
				}
				else
				{
					u8_status = MQTT_PDP_FAIL_FLAG;
					u8_status = GSM_FAIL;
					return 	u8_status;
				}
			}
			
			
			
			else if(strcmp((char *)u8_GSM_Response_Identifier,"+QMTPUBEX") == 0)
			{
				u16_i =13; 
				u16_j = 0;
				while(vu8_Gsm_Rx_Buffer[u16_pkt_num][u16_i] != '\r')
				{
					MQT_CMD_RES[u16_j] = vu8_Gsm_Rx_Buffer[u16_pkt_num][u16_i];
					u16_i++;
					u16_j++;
				}
				MQT_CMD_RES[u16_j] = '\0';
				if(strcmp((char *)MQT_CMD_RES,"1,0") == 0)
				{
					
					MQTT_PUB_SUCC_FLAG1 = 1;
					u8_status = MQTT_PUB_SUCC_FLAG;
					return 	u8_status;
					
				}
				
				else if(strcmp((char *)MQT_CMD_RES,"1,1,1") == 0)
				{
					
				        MQTT_PUB_RETRY_FLAG = 1;
					u8_status = MQTT_PUB_RETRY_FLAG;
					return u8_status;
				}
				else if(strcmp((char *)MQT_CMD_RES,"1,1,2") == 0)
				{
					
				        MQTT_PUB_RETRY_FLAG = 2;
					u8_status = MQTT_PUB_RETRY_FLAG;
					return u8_status;
				}
				else if(strcmp((char *)MQT_CMD_RES,"1,1,3") == 0)
				{
					
				        MQTT_PUB_FAIL_FLAG = 1;
					u8_status = GSM_FAIL;
					return u8_status;
				}
				
				
				else if(strcmp((char *)MQT_CMD_RES,"1,2") == 0)
				{
					
				        MQTT_PUB_FAIL_FLAG = 1;
					u8_status = GSM_FAIL;
					return u8_status;
				}
				else
				{
			       	        MQTT_PUB_FAIL_FLAG = 1;
					u8_status = MQTT_PDP_FAIL_FLAG;
					u8_status = GSM_FAIL;
					return 	u8_status;
				}
			}
			
			else if(strcmp((char *)u8_GSM_Response_Identifier,"+QMTSUB") == 0)
			{
				u16_i =12;
				u16_j = 0;
				while(vu8_Gsm_Rx_Buffer[u16_pkt_num][u16_i] != ',')
				{
					MQT_CMD_RES[u16_j] = vu8_Gsm_Rx_Buffer[u16_pkt_num][u16_i];
					u16_i++;
					u16_j++;
				}
				MQT_CMD_RES[u16_j] = '\0';
				if(strcmp((char *)MQT_CMD_RES,"0") == 0)
				{
					u8_status = MQTT_SUB_SUCC_FLAG;
					return 	u8_status;
					
				}
				else
				{
					u8_status = MQTT_PDP_FAIL_FLAG;
					u8_status = GSM_FAIL;
					return 	u8_status;
				}
			}
			
			
			
			
			
			
			/********************************MQTT related stuffs ends here***************************************/
			
			else if(strcmp((char *)u8_GSM_Response_Identifier,"ERROR") == 0)
			{
				MQTT_STAT_FAIL = 1;
				u8_status = GSM_FAIL;
				return 	u8_status;	
			}
			
			else if(strcmp((char *)u8_GSM_Response_Identifier,"CONNECT") == 0)     ///+QFUPL
			{
				u8_status = GSM_FILE_CONNECT;
				
				return 	u8_status;
				
			}
			else if(strcmp((char *)u8_GSM_Response_Identifier,"+CME ERROR") == 0)
			{
				if(vu8_Gsm_Rx_Buffer[u16_pkt_num][13] == 0x0d)
					u8_CME_Error_Code = vu8_Gsm_Rx_Buffer[u16_pkt_num][12] - 0x30;
				else 
				{
					u8_CME_Error_Code = (vu8_Gsm_Rx_Buffer[u16_pkt_num][12] - 0x30) * 10;
					u8_CME_Error_Code += (vu8_Gsm_Rx_Buffer[u16_pkt_num][13] - 0x30);
				}
				if(u8_CME_Error_Code == 10)
					u8_SIM_Insert = 0;
				u8_status = GSM_OPERATOR_ERROR;
				return 	u8_status;	
			}
			else if((strcmp((char *)u8_GSM_Response_Identifier,">") == 0) && (u8_GSM_Send_Flag == 1))
			{
				u8_GSM_Send_Flag = 0;
				return 	GSM_TCP_SEND_READY;	
			}
			else if(strcmp((char *)u8_GSM_Response_Identifier,"+QSSLOPEN") == 0)
			{
				return GSM_SUCCESS;
			}	
			
			else if(strcmp((char *)u8_GSM_Response_Identifier,"+QHTTPPOST")==0)
			{
				u16_i =12;
				u16_j = 0;
				while(vu8_Gsm_Rx_Buffer[u16_pkt_num][u16_i] != ',')
				{
					u8_URC_Response[u16_j] = vu8_Gsm_Rx_Buffer[u16_pkt_num][u16_i];
					u16_i++;
					u16_j++;
				}
				u8_URC_Response[u16_j] = '\0';
				if(strcmp((char *)u8_URC_Response,"0") == 0)
				{
					u8_status = GSM_TCP_DATA_RECEIVED;
					return 	u8_status;
					
				}
				else
				{
					u8_TCP_Status = GSM_PDP_DEACTIVATED;
					u8_status = GSM_FAIL;
					return 	u8_status;
				}
			}
			else if(strcmp((char *)u8_GSM_Response_Identifier,"+QHTTPGET")==0)
			{
				u8_q = 0;
				if(vu8_Gsm_Rx_Buffer[u16_pkt_num][11] == 0x30)
				{				
					while(vu8_Gsm_Rx_Buffer[u16_pkt_num][u8_q]!= ',')
					{
						u8_q++;
						if(vu8_Gsm_Rx_Buffer[u16_pkt_num][u8_q]== ',')
						{
							u8_q += 1;
							while(vu8_Gsm_Rx_Buffer[u16_pkt_num][u8_q ]!= ',')
							{
								u8_q++; 
							}
							break;
						}					 
					}	

					u8_q += 1;						
					while(vu8_Gsm_Rx_Buffer[u16_pkt_num][u8_q ]!= 0x0d)
					{
						
						u8_FW_File_Size[u16_k] = vu8_Gsm_Rx_Buffer[u16_pkt_num][u8_q];
						u16_k++;
						u8_q++;					
					}
					return GSM_HTTP_GET;
				}					
			}			
			else if(strcmp((char *)u8_GSM_Response_Identifier,"+QHTTPREADFILE")==0)
			{
				if(vu8_Gsm_Rx_Buffer[u16_pkt_num][16] == 0x30)
					return GSM_HTTP_READ;					
			}							
			else if(strcmp((char *)u8_GSM_Response_Identifier,"SEND OK")==0)
			{
				return GSM_SUCCESS;
			} 
		}
	}
	return GSM_GPRS_TMO_ERR;
}

