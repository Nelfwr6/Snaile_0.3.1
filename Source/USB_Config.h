/*******************************************************************************

* Module name: USB_Config.h

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

* Header file for ADC Application program

*

*******************************************************************************/
/**************************************************************************************/
#ifndef _USB_CONFIG_H  
#define _USB_CONFIG_H  

/******************************* SETTINGS *********************************************/
/*************************** HEADER FILES *********************************************/
#include "r_cg_macrodriver.h"
/**************************************************************************************/
/********************************** USER SETTINGS *************************************/
#define USB_PACKET_COUNTER	100
#define USB_CHANNEL		2
#define USB_DETECTION_TMO	60000
#define USB_STATUS		P3_bit.no0
#define USB_DETECTED		1
#define USB_NOT_DETECTED	0
#define USB_SUCCESS		2
#define USB_ERROR		4
#define RANGE			3



#define NETWORK_APN "internet"
#define NETWORK_USER	""
#define NETWORK_PASSWORD ""

#define SET_SCANNING_INTERVAL_CMND 0x02
#define SET_IR_CALIBERATION_CMND 0x10
#define SET_START_TIME_CMND 0x16
#define SET_END_TIME_CMND 0x17
#define SET_CURRENT_DATE_CMND 0x00
#define SET_CURRENT_TIME_CMND 0x01
#define CHECK_PASSWORD_CMND 0x13
#define SET_PASSWORD_CMND 0x05
#define GET_DEVICE_ID_CMND 0x11
#define GET_CONFIGURATION_CMND 0x12
#define SET_URL_CMND 0x07
#define GET_URL_CMND 0x15
#define SET_TIME_ZONE_CMND 0x20
#define SET_APN_CMND 0x99
#define GET_APN_CMND 0x98
//#define DEVICE_ID		"5026512293"
//#define DEVICE_ID		"5026503573"
//#define DEVICE_ID		"5026515403"
#define DEVICE_ID		"5026515692"
//#define DEVICE_ID		"5026528273"
//#define DEVICE_ID		"5026515833"
//#define DEVICE_ID		"5026516047"
//#define DEVICE_ID		"5026513150"
//#define DEVICE_ID		"5026518415"
//#define DEVICE_ID		"5026515866"
#define CONFIG_DF_DATA_SIZE 256

typedef union 
{
	uint8_t All[CONFIG_DF_DATA_SIZE];
	struct
	{
		uint32_t u32_scanning_timeout;
		uint16_t u8_day_start_time_hr;
		uint8_t u8_day_start_time_min;
		uint8_t u8_day_End_time_hr;
		uint8_t u8_day_End_time_min;	
		uint8_t u8_current_time_hr;
		uint8_t u8_current_time_min;
		uint8_t u8_current_time_sec;
		uint8_t u8_current_date;
		uint8_t u8_current_month;
		uint16_t  u16_current_year;
		uint8_t u8_password[8];
		uint16_t u16_GMT_hr;
		uint16_t u16_GMT_min;
		uint16_t u16_GMT_sec;
		uint8_t u8_timezone_sign;
		uint16_t u16_time_zone_hr;
		uint16_t u16_time_zone_min;
		uint8_t u8_URL[128];
		uint8_t u8_apn[18];
		uint16_t u16_Config_id;   //Configuration id 
		uint16_t u16_Version_id;  //Version id  
		//uint8_t u16_Version_id[7];  //Version id
		uint8_t u8_dummy1[6];
		uint8_t Registration_Flag;
		uint8_t Provinsion_Flag;
		uint8_t config_update_flag;
		
		//total bytes 1024 - (4+12+8+6+2+4+128+18+8)	
		uint8_t u8_Dummy_Byte[CONFIG_DF_DATA_SIZE - 199];
		uint32_t u32_Checksum_Value;
	}Member;
}D_CONFIG;

extern D_CONFIG D_Config_Packet;
extern uint8_t u8_usb_configration_buffer[USB_PACKET_COUNTER];
extern uint8_t USB_data_rev_flg, u8_USB_Config_Err, u8_usb_byte_counter;
extern uint16_t u16_USB_RX_Next_Byte_TMO;
extern uint8_t u8_usb_packet_counter;

extern uint8_t u8_timezone[22];
extern uint16_t u16_URL_length;
extern uint8_t u8_Data_Write_Flag;
extern uint8_t update_done_flag;


/************ FUNCTIONS *********************************************************/
void v_USB_RX_ISR(uint8_t u8_usb_rx );
uint8_t u8_USB_data_handling(void);
void USB_Response(uint8_t u8_USB_Status);
uint8_t ASCII_hex_converter(uint8_t u8_data);
void Reload_configuration(void);
void v_usb_configuration(void);
uint32_t u32_calculate_scanning_timeout(void);
void Update_Configuration(void);
void Update_restart_time(void);
//////  FUNCTIONS FOR USB INTERRUPT ///////
void set_usb_connected(void);
uint8_t get_usb_connection_status(void);
void clear_usb_connection_status(void);
void v_USB_RX_ISR(uint8_t);
char* clk_fun(void);
/*******************************************************************************/



#endif
