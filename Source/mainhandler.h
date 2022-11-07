
/*******************************************************************************

* Module name: EEPROM.h

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
#ifndef _MAIN_HANDLER_H_
#define _MAIN_HANDLER_H_

#define VERSION "0.3.1"

#define NORMAL_MODE 0
#define USB_MODE 1

#define LED_OFF  0
#define LED_ON  1


#define DEACTIVATED       0
#define ACTIVATED         1


#define DEBUG_LED_OFF() P7_bit.no0 = 1
#define DEBUG_LED_ON()  P7_bit.no0 = 0

#define GSM_DEBUG_LED      P7_bit.no0
#define GSM_PDP_STATUS_LED P5_bit.no0
#define GSM_DATA_STATUS_LED  P5_bit.no1
#define DEVICE_POWER_ENABLE_PIN P7_bit.no3
#define GSM_CONNECT_PORT "80"
#define DCONFIG_BLOCK 0
#define RFID_WRITE_START_BLOCK 1
#define RFID_WRITE_END_BLOCK   4

#define MAX_DATA_RETRY_TIMEOUT   10

#define GSM_RFID_DATA_LENGTH 240
#define APP_READ_RFID 0
#define APP_GSM_INITIALISE 1
#define APP_RFID_DATA_SCAN 2
#define APP_GSM_LOOP 3
#define APP_POWER_DOWN 4
#define APP_SEND_RFID_COMMAND 5
#define APP_RFID_DF_WRITE 6
#define APP_TCPIP_INIT 7
#define APP_TCPIP_NET_CONNECT 8
#define APP_GSM_POWER_ON 9
#define APP_TCPIP_SEND_DATA 10
#define APP_POWER_UP 11
#define APP_STOP_STATE 12

#define APP_URL_201 13
#define APP_URL_409 14


#define RFID_ERR 0x01
#define GSM_FAILED_ERR 0x02
#define GSM_PDP_ACTIVATION_FAILED_ERR 0x04
#define SSL_CONNECTION_FAILED_ERR		0x08
#define GSM_DATA_SEND_FAILED_ERR	0x10
#define GSM_DATA_RECV_FAILED_ERR	0x20

#define RFID_FAIL_RETRY_COUNT 2
#define RFID_SEND_RETRY_COUNT 10

#define COMPARE_FAIL 1
#define COMPARE_SUCCESS 0

#define DCONFIG_DF_START_ADDRESS 0xF1000  
#define DCONFIG_DF_START_POSITION 0
#define DCONFIG_DF_ALLOTTED_MEMORY 0x400

#define RFID_DF_START_ADDRESS 0xF1400	 
#define RFID_DF_START_POSITION 0x400
#define RFID_DF_ALLOTTED_MEMORY 0xC00		


///////////////////////////////////////////////
//for saving certificate 

#define CERT_DF_START_ADDRESS 0xF2000	 
#define CERT_DF_START_POSITION 0x1000  
#define CERT_DF_ALLOTTED_MEMORY 0x800
#define CERT_DF_DATA_SIZE1 2000


#define CERTPvt_DF_START_ADDRESS 0xF2800	
#define CERTPvt_DF_START_POSITION 0x1800		
#define CERTPvt_DF_ALLOTTED_MEMORY 0x800
#define CERTPvt_DF_DATA_SIZE2 2000

#define CERT_WRITE_START_BLOCK 4 
#define CERT_WRITE_END_BLOCK   8

///////////////////////////////////////////////

#define DEFAULT_SCANNING_TIME 3     //basil (by default it is 3)

#define DEVICE_START_TIME 1
#define DEVICE_WORKING_TIME 2
#define DEVICE_IDLE_TIME 3
#define DEVICE_ERASE_RFID_DF 4


#define GSM_SEL_HANDSHAKE_DATA 0
#define GSM_SEL_RFID_DATA 1
#define REGISTRATION_DATA 2


extern uint8_t *DConfig_DF_Ptr;
extern uint16_t u16_DConfig_DF_Write_Position;

/////////////////////////////////////////////////////
extern uint32_t u32_Cert_DF_Write_Position;
extern uint8_t *u8_Cert_DF_Read_Ptr;

extern uint32_t u32_CertPvt_DF_Write_Position;
extern uint8_t *u8_CertPvt_DF_Read_Ptr;

//extern uint8_t *CertPEM_DF_Ptr;
//extern uint8_t *CertPVT_DF_Ptr;
/////////////////////////////////////////////////////

extern uint32_t u32_RFID_DF_Write_Position, u32_current_scanning_timeout;
extern uint8_t *u8_RFID_DF_Read_Ptr;
extern uint8_t u8_Configuration_mode, u8_LED_Set_Blink_Count, u8_mode_change_flag, u8_Debug_LED_Set_Number_Of_Blinks; 

extern uint16_t u8_TestRFID[12];

extern volatile uint8_t u8_Key_interrupt_flag;
extern volatile uint8_t u8_Debug_Key_flag;
extern uint8_t u8_Debug_LED_Number_Of_Blinks;
extern uint16_t u16_DEBUG_LED_Blink_Count, u16_DEBUG_LED_Set_Blink_Count;
extern uint16_t u16_DATA_LED_Blink_Count, u16_PDP_LED_Blink_Count;
extern uint8_t u8_RFID_New_Data_Count, u8_GSM_Data_Format_Sel, u8_APP_State;
extern uint32_t u32_RFID_New_Data_Location_Ptr;
extern volatile uint8_t u8_Device_State;
uint8_t u8_byte_array_compare(uint8_t *u8_ptr1, uint8_t *u8_ptr2);
void Reload_configuration(void);
void Read_Dconfig_Data_Flash(void);
void Read_RFID_Data_Flash(void);

void Read_Cert_Data_Flash(void);
void Read_Cert_Data_Flash2(void);

uint32_t u32_checksum_calculate(uint8_t *, uint16_t );
void v_RFID_Handler(void);
void v_usb_mode_handler(void);
void v_normal_mode_handler(void);
void v_rtc_check_polling(void);
uint8_t v_GSM_Data_Format_Send(void);
uint8_t u8_Check_Working_Time(void);
#endif