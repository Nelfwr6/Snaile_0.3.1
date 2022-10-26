/*******************************************************************************

* Module name: rfid.h

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

* First written on 08/22/2014 by jomon.

*

* Module Description:

* Header file for ADC Application program

*

*******************************************************************************/
/**************************************************************************************/
#ifndef _RFID_H  
#define _RFID_H  

/******************************* SETTINGS *********************************************/
/*************************** HEADER FILES *********************************************/
#include "r_cg_macrodriver.h"
/**************************************************************************************/
/********************************** USER SETTINGS *************************************/
#define RFID_POWER_EN_PIN P3_bit.no1
#define RFID_RF_EN_PIN P1_bit.no5
#define RFID_UART_CHANNEL 0
#define RFID_MAX_RX_BUFFER_COUNT 24

#define RFID_RETRY_READ_COUNT          2
#define RFID_MAX_PACKET_COUNT          20

#define RF_PWR_EN              P3_bit.no1
#define RF_EN                  P1_bit.no5

#define RFID_SUCCESS		0
#define RFID_CHECKSUM_ERR	1
#define RFID_NO_CARD_DETECTED	2
#define RFID_CARD_DETECTD	4
#define RFID_RESPONSE		5
#define RFID_TIMEOUT_ERR        6
#define RFID_FAILED		7
#define RFID_DF_DATA_SIZE 16


#define RFID_COMMAND_FRAME        0x00
#define RFID_RESPONSE_FRAME       0x01
#define RFID_NOTICE_FRAME	  0x02
#define RFID_NO_TAG 		0x15

#define RFID_POLLING_COMMAND 0x22
#define RFID_SET_QUERY_COMMAND 0x0E
#define RFID_HIGH_SENSITIVITY_TYPE 0x00
#define RFID_HIGH_SENSITIVITY_COMMAND 0xf5
#define RFID_DENSE_READER_TYPE 0x00
#define RFID_DENSE_READER_COMMAND 0xf5

#define RFID_DATA_RX_COMPLETE 0x01
#define RFID_PACKET_START 0x02
#define RFID_PACKET_EMPTY 0x00
#define RFID_DATA_READ_COMPLETE 0x03
typedef union 
{
	uint8_t All[RFID_MAX_RX_BUFFER_COUNT];
	struct
	{
		uint8_t u8_RFID_Header;
		uint8_t u8_RFID_Type;
		uint8_t u8_RFID_Command;
		uint8_t u8_RFID_len_MSB;
		uint8_t u8_RFID_len_LSB;
		uint8_t Paramter[RFID_MAX_RX_BUFFER_COUNT - 7];
		uint8_t u8_RFID_checksum;
		uint8_t u8_RFID_END;
	}Member;
}RFID_PACKET_FORMAT;
typedef union 
{
	uint8_t All[RFID_DF_DATA_SIZE];
	struct
	{
		uint8_t u8_RFID_data[12];
		uint32_t u32_Checksum_Value;
	}Member;
}RFID_DF_DATA;
extern RFID_DF_DATA RFID_DF_Data_Packet;
extern uint8_t u8_Uart0_tx_Flag;
extern uint16_t u16_RFID_Response_Tmo;
extern RFID_PACKET_FORMAT u8_RFID_Pool[RFID_MAX_PACKET_COUNT];

extern uint8_t u8_RFID_Data_Received[RFID_MAX_PACKET_COUNT];
/************ FUNCTIONS *********************************************************/
void v_rfid_rx_isr(uint8_t u8_rfid_rx);
void v_RFID_Data_Send(uint8_t u8_data);
void v_RFID_Scheduler(void);
void v_RFID_Turn_OFF(void);
void v_RFID_Turn_ON(void);
uint8_t u8_RFID_RX_Checksum_Check(uint8_t);
uint8_t u8_RFID_Poling_Command_Send(void);
uint8_t u8_RFID_Set_Query_Command_Send(void);
uint8_t u8_RFID_Check_Packet_Received(uint16_t);
uint8_t u8_RFID_Reception_Handler(uint8_t);

uint8_t u8_RFID_Dense_Reader_Command_Send(void);
uint8_t u8_RFID_High_Sensitivity_Command_Send(void);
void v_clear_RFID_data_buf(void);
/*******************************************************************************/




#endif  ///
