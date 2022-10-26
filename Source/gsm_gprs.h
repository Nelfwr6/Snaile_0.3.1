/*******************************************************************************

* Module name: gsm.h

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

*

* Module Description:

* Header file for GSM commands

*

*******************************************************************************/

#ifndef _GSM_GPRS_H_
#define _GSM_GPRS_H_


#define GSM_GPRS_CHANNEL 	        1		// UART channel on board
#define GSM_GPRS_MAX_PACKET_COUNT 	30		// No of packets to be stored in receive buffer
#define GSM_GPRS_RX_BUFFER_SIZE 	200		// Maximum lenght of packet in receive buffer

#define GPRS_RX_SSL_DATA_LEN		10

#define	GSM_INIT_RETRY_COUNT		4

#define GSM_GPRS_PACKET_EMPTY 		0		
#define GSM_GPRS_PACKET_FULL 		1

#define GSM_GPRS_RX_TMO 			10		// Time out in mSec for packet bytes

#define CONNECT_3G 					1
#define CONNECT_2G 					2

//--------- Error codes ---------------//
#define GSM_GPRS_TMO_ERR 			0x00
#define GSM_SUCCESS 				0x01
#define GSM_OPERATOR_ERROR 			0x02
#define GSM_FAIL 					0x03
#define GSM_INIT 					0x04
#define GSM_TCP_SEND_READY 			0x05
#define GSM_FILE_CONNECT   			0x06
#define GSM_SSL_OPEN   				0x07
#define GSM_POST_SUCCESS			0x08
#define GSM_PDP_ACTIVATED			0x0a
#define GSM_PDP_DEACTIVATED			0x0b
#define GSM_NO_DATA	        		0x0d
#define GSM_SSL_SEND_OK				0x0e	
#define	GSM_POWERDOWN				0x0f
#define GSM_SSL_CLOSED 				0x10
#define	GSM_TCP_CLOSED				0x12
#define	GSM_TCP_ERROR				0x13
#define GSM_TCP_DATA_RECEIVED			0x14

#define GSM_HTTP_GET				0x15
#define GSM_HTTP_READ				0x16

#define MQTT_OPEN_SUCCESS 	50		
#define MQTT_CONN_SUCCESS	51		
#define MQTT_OPEN_FAIL		52		
#define MQTT_CONN_FAIL		53		
#define MQTT_PUB_FAIL		54		
#define MQTT_PUB_SUCCESS	55		
#define MQTT_SUB_SUCCESS	56		
#define MQTT_SUB_FAIL		57		

#define MQTT_TRUE 		1
#define MQTT_FALSE 		0


#define GSM_POWER_KEY             P2_bit.no7
#define GSM_POWER_ENABLE          P14_bit.no7  
#define PS_SYNC			  P14_bit.no6

#define GSM_RESET                 P2_bit.no6
#define GSM_WAKEUP_IN             P2_bit.no5
#define GSM_W_DISABLE             P2_bit.no4

#define HTTP_SEC_LEVEL 0
#define SSL_SEC_LEVEL 2
#define HTTP_CONNECTION_TMO "90"
#define HTTP_RESPONSE_TMO "90"
#define HTTP_INPUT_TMO "90"
/********************************************* EXTERNAL VRIABLES***************************************************/
extern uint8_t CCID_num[30];
extern uint8_t IMEI_num[30];


extern uint8_t clock[35];
extern uint8_t u8_GSM_Response_Identifier[GSM_GPRS_RX_BUFFER_SIZE];
extern uint8_t u8_gsm_gprs_rx;
extern volatile uint8_t u8_Gsm_Rx_Pkt_Counter,u8_Gsm_Rx_Status_Counter, u8_Gsm_Rx_Status_Start_Counter;
extern volatile uint8_t vu8_GSM_Packet_Status[GSM_GPRS_MAX_PACKET_COUNT], vu8_Recv_flag;

extern uint8_t u8_Network_Status, u8_SIM_Insert;
extern uint8_t	u8_GSM_Status,u8_TCP_Status, u8_GSM_Send_Flag;
extern uint8_t u8_GSM_Data_String[150];
extern uint8_t u8_PDP_Status,u8_Tcp_Status,u8_Recv_Data_Use_Flag;
extern uint16_t u16_Server_Data_Length; 
extern volatile uint8_t u8_Server_Data_Recv_Flag;
extern uint8_t u8_Network_Sec,u8_Network_Min,u8_Network_Hr,u8_Network_Date,u8_Network_Month,u8_Network_Year;
extern uint32_t u32_Network_Strength;
extern volatile uint8_t vu8_Gsm_Rx_Buffer[GSM_GPRS_MAX_PACKET_COUNT][GSM_GPRS_RX_BUFFER_SIZE];
/******************************************* FUNCTIONS *********************************************************/
extern uint8_t u8_Update_SSL_Crt_To_UFS			(void);
extern void 	v_GSM_Timeout_Handler			(void);
extern void 	v_Gsm_Hardware_Init				(void);
extern void 	v_Gsm_Power_Down				(void);
extern uint8_t 	u8_GSM_GPRS_reception_Handler	(uint32_t u32_tmo);
extern uint8_t	u8_GPRS_Init					(void);
extern uint8_t 	u8_Get_Network_Time				(void);
extern uint8_t 	u8_Get_Network_Signal_Strength	(void);
void 	Convert_Time_To_Bcd	(void);
uint8_t	Hex_to_Bcd (uint8_t u8_tmpt);

uint8_t u8_GSM_TCP_Activate(uint8_t * ,uint8_t * ,uint8_t * );
void v_GSM_Rx_Isr(uint8_t u8_gsm_gprs_rx);
uint8_t u8_HTTP_Open_Connection(uint8_t *u8_url,uint8_t * u8_port);

uint8_t u8_HTTP_Open(uint8_t * u8_url);
uint8_t u8_HTTP_GET(void);
uint8_t GetByte (uint32_t timeout, int8_t *data);
int8_t u8_File_Open(void);
void R_UART1_Send_String(__far int8_t buffer[]);
void R_UART1_Send_Byte(int8_t data) ;

uint8_t u8_GSM_HTTP_Data_Write(uint8_t *u8_Data, uint16_t u16_data_length);
void v_Gsm_PowerON(void);
void RTC_Update_From_GSM(void);
int RTC_Update_To_GSM(void);
uint8_t u8_Gsm_Wait_For_AT_response(uint32_t time);
void GSM_Initial_Configuration(void);
uint8_t u8_Get_CCID_IMEI(void);
uint8_t u8_MQTT_Activate(void);

uint8_t u8_GSM_GPRS_reception_Handler(uint32_t u32_tmo);

#endif