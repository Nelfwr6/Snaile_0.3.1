/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2011, 2021 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_main.c
* Version      : CodeGenerator for RL78/G13 V2.05.06.02 [08 Nov 2021]
* Device(s)    : R5F100FL
* Tool-Chain   : CCRL
* Description  : This file implements main function.
* Creation Date: 22-07-2022
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_intc.h"
#include "r_cg_serial.h"
#include "r_cg_adc.h"
#include "r_cg_timer.h"
#include "r_cg_rtc.h"
#include "r_cg_it.h"
/* Start user code for include. Do not edit comment generated here */
#include "adc.h"
#include "interval_timer.h"
#include "RTC.h"
#include "USB_Config.h"
#include "EEPROM.h"
#include "MQTT.h"
#include "RFID.h"
#include "power_down.h"
#include "mainhandler.h"
#include "gsm_gprs.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
void Reset_MCU (void);
D_CONFIG D_Config_Packet;
uint8_t m=0;
uint8_t u8_Configuration_mode, u8_mode_change_flag = 1;
uint8_t u8_i = 0, u8_j = 0;
uint8_t u8_status = 0; //TODO
int8_t ret;
extern volatile uint8_t gsm_rx_data;
extern volatile uint8_t usb_rx_data;
extern volatile uint8_t rfid_rx_data;
extern uint8_t OTA_HTTP_URL[1500];
//uint8_t u8_ota_url[] = "http://139.162.42.79:8080/downloads/Snaile_test_ota2.bin";


/* End user code. Do not edit comment generated here */
void R_MAIN_UserInit(void);

/***********************************************************************************************************************
* Function Name: main
* Description  : This function implements main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void main(void)
{
    R_MAIN_UserInit();
    /* Start user code. Do not edit comment generated here */
    //DEVICE_POWER_ENABLE_PIN = 0;
    //R_ADC_Set_OperationOn();
   // R_TAU0_Channel0_Start();
    R_KEY_Start();
    DEBUG_LED_OFF();
    R_RTC_Start();
    R_UART2_Start();//USB
    R_UART2_Receive(&usb_rx_data,1);
    R_INTC3_Start();        //USB Detection
    R_IT_Start();
    R_RTC_Set_ConstPeriodInterruptOff();// stopping RTC interrupt
    //R_RTC_Set_ConstPeriodInterruptOn(ONEMIN);
   // Erase_Dconfig_Data_Flash();
   
   
    Reload_configuration();
    if(D_Config_Packet.Member.u16_Version_id!=VERSION)
    {
	    D_Config_Packet.Member.u16_Version_id=VERSION;
	    D_Config_Packet.Member.u32_Checksum_Value = u32_checksum_calculate(&D_Config_Packet.All[0], CONFIG_DF_DATA_SIZE - 4);
	    u16_DConfig_DF_Write_Position = DCONFIG_DF_START_POSITION;
	    //Erase_Dconfig_Data_Flash();
	    Write_Dconfig_Data_Flash();
	    
    }
    ////////RTC//////////////
    R_TAU0_Channel0_Start();
    R_UART1_Create();
    R_UART1_Start();
    R_UART1_Receive(&gsm_rx_data,1);
    RTC_Update_From_GSM();           
    if(D_Config_Packet.Member.Provinsion_Flag == NOT_PROVISIONED)
    {
    	GSM_Initial_Configuration(); // this function provision the device and go to sleep mode
    }
    else
    {
	v_Gsm_Hardware_Init();
	u8_Gsm_Wait_For_AT_response(14000);
	RTC_Update_From_GSM();                  
	v_Gsm_Power_Down();
    }
    ///////RTC/////////////
    Erase_RFID_Data_Flash();
    v_RTC_Get_Time();
    
    
       // Read_Cert_Data_Flash();
       // Read_Cert_Data_Flash2();
	
	//v_uart_str_send("app started2\r\n",GSM_GPRS_CHANNEL);
    	//v_Gsm_Hardware_Init();
	

	
	
	 
    //////////////////////////////test ota start////////////////////////
//   R_UART1_Create();
//		R_UART1_Start(); 
		
//     R_UART1_Receive(&gsm_rx_buf, 1);
//		v_Gsm_Hardware_Init();
//    	u8_status = u8_GPRS_Init();
//    	if(u8_status == GSM_SUCCESS)
//	{
//		DEBUG_LED_ON();	
//	}
//	else
//	{
//		DEBUG_LED_ON();	
//		GSM_PDP_STATUS_LED = LED_ON;
//		while(1)
//		{
//			NOP();
//		}
//	}
//	u8_status = u8_GSM_TCP_Activate((uint8_t *)&D_Config_Packet.Member.u8_apn[0],(uint8_t *)"",(uint8_t *)"");
//	if(u8_status == GSM_SUCCESS)
//	{
		
//		DEBUG_LED_OFF();	
//		GSM_PDP_STATUS_LED = LED_ON;
//	}
//	else
//	{
//		while(1)
//		{
//			NOP();
//		}
//	}
//	//u8_GSM_Data_Format_Sel = 3;
//	v_delay_ms(1000);
//	u8_status = u8_HTTP_Open(u8_ota_url);
//	if((u8_status == GSM_SUCCESS))
//	{
//		GSM_PDP_STATUS_LED = LED_OFF;
//		GSM_DATA_STATUS_LED = LED_ON;
//		v_delay_ms(1000);
//		u8_status = u8_HTTP_GET();
//		if((u8_status == GSM_SUCCESS) || GSM_HTTP_READ)
//		{
//			GSM_PDP_STATUS_LED = LED_ON;
//			GSM_DATA_STATUS_LED = LED_ON;
//		}
//		else
//		{
//			while(1)
//			{
//				NOP();
//			}
//		}
//	}
//	else
//	{
//		while(1)
//		{
//			NOP();
//		}
//	}
	
//	v_delay_ms(10000);
//	DEBUG_LED_ON();	
//	GSM_PDP_STATUS_LED = LED_ON;
//	GSM_DATA_STATUS_LED = LED_ON;
//	Reset_MCU();
//	while(1)
//	{
//		NOP();
//	}
	/////////////////////////////test ota end////////////////////////
	
u8_Debug_Key_flag = 1;
    while (1U)
    {

	    if(u8_mode_change_flag == 1)
	    {
	  	 u8_mode_change_flag = 0;
		   if(USB_STATUS == 1)
		   {
			   if(u8_Configuration_mode != USB_MODE)
			   {
				u16_DATA_LED_Blink_Count = 0;
				u16_PDP_LED_Blink_Count = 0;
				DEBUG_LED_ON();
				GSM_PDP_STATUS_LED = LED_OFF;
				GSM_DATA_STATUS_LED = LED_OFF;
				R_UART2_Stop();
				R_SAU1_Create();
				R_UART2_Start();
				u8_USB_Config_Err = 0;
		  		u8_Configuration_mode = USB_MODE;
				R_TAU0_Create();
				R_TAU0_Channel0_Start();

			    }
		   } 
		   
	    }
	    else if(!USB_STATUS)
	    {
		    if(u8_Configuration_mode != NORMAL_MODE)
		    {
			GSM_PDP_STATUS_LED = LED_OFF;
			GSM_DATA_STATUS_LED = LED_OFF;
	    		u8_Configuration_mode = NORMAL_MODE;
			u8_APP_State = APP_POWER_DOWN;
			//u8_Button_Press = 0; //clearing unwanted press while usb mode
		    }
	    }
	   // u8_Configuration_mode = NORMAL_MODE;
		switch(u8_Configuration_mode)
		{
		    case NORMAL_MODE:
		    v_normal_mode_handler();
		    break;
		    case USB_MODE:
		    if(!u8_USB_Config_Err)
		    v_usb_mode_handler();
		    break;
		}
    }
    
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: R_MAIN_UserInit
* Description  : This function adds user code before implementing main function.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_MAIN_UserInit(void)
{
    /* Start user code. Do not edit comment generated here */
    EI();
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
