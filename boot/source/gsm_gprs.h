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
#define GSM_GPRS_MAX_PACKET_COUNT 	8		// No of packets to be stored in receive buffer
#define GSM_GPRS_RX_BUFFER_SIZE 	40		// Maximum lenght of packet in receive buffer

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

#define GSM_POWER_KEY             P2_bit.no7
#define GSM_POWER_ENABLE          P14_bit.no7  
#define PS_SYNC			  P14_bit.no6

#define GSM_RESET                 P2_bit.no6
#define GSM_WAKEUP_IN             P2_bit.no5
#define GSM_W_DISABLE             P2_bit.no4

typedef enum {
    RIL_ATRSP_FAILED   =  -1,
    RIL_ATRSP_SUCCESS  =   0,
    RIL_ATRSP_CONTINUE =   1,
    RIL_ATRSP_CONNECT  =   2,
    RIL_ATRSP_QFOPEN   =   3,
}Enum_ATRspError;

/********************************************* EXTERNAL VRIABLES***************************************************/


void v_Gsm_Hardware_Init(void);
void v_Gsm_PowerON(void);
void v_Gsm_Power_Down(void);
uint8_t u8_Gsm_Wait_For_AT_response(uint32_t time);
void GSM_Initial_Configuration(void);
void v_delay_ms(uint16_t ms_delay);
int32_t ATResponse_Handler(char* line);
int8_t u8_File_Open(void);
uint8_t u8_File_Close(void);
uint8_t u8_File_Erase(void);
uint8_t u8_Read_from_UFS(uint16_t flash_start_address);
int32_t powerof(int8_t base, int8_t exp);
uint8_t GetByte (uint32_t timeout, int8_t *data);
#endif