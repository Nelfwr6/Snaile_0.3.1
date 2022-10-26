/*******************************************************************************

* Module name: EEPROM.c

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
#include "EEPROM.h"
#include "pfdl.h"
#include "pfdl_types.h"
#include <string.h>
#include "RTC.h"
#include "RFID.h"
#include "mainhandler.h"
#include "gsm_gprs.h"
#include "MQTT.h"

/*******************************************************************************

* Global Variable Section

*******************************************************************************/

uint8_t  Error = 0,write_success = 0;
/***********************************************************************************************************************
* Function Name: Erase_Dconfig_Data_Flash
* Description  : This function used to Erase all 4KB Data Flash
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void 	Erase_Dconfig_Data_Flash(void)
{
	uint16_t DF_Block;
	uint8_t DF_Error = 0, Err_Flag = 1;
	pfdl_status_t my_status;
	
	__near pfdl_request_t request_pfdl;
	pfdl_descriptor_t pfdl_init;

	pfdl_init.fx_MHz_u08 = 12;			// Clock Frequency
	pfdl_init.wide_voltage_mode_u08 = 0x00;		// Full voltage mode - 0x00 - 2.7V-5.5V
	    						// Wide voltage mode - 0x01 - 1.8V-5.5V
	while(Err_Flag)
	{
		my_status = PFDL_Open(&pfdl_init);
	  	if (my_status == PFDL_OK)
	    	{
			DF_Block = DCONFIG_BLOCK;
			while(DF_Block < 0x01)
			{	
				request_pfdl.index_u16 = DF_Block;
				request_pfdl.command_enu = PFDL_CMD_ERASE_BLOCK;
				my_status = PFDL_Execute(&request_pfdl);
				while(my_status == PFDL_BUSY)
				{
		    			my_status = PFDL_Handler();
				}
				if(my_status == PFDL_OK)
		 			DF_Block ++;
			}
			if(my_status == PFDL_ERR_ERASE )
				DF_Error ++;
			else if(my_status == PFDL_OK)
				Err_Flag = 0;
		}
		else if(my_status != PFDL_OK )
			DF_Error ++;
		PFDL_Close();
		if(DF_Error > 4)
		{
			break;
		}
	}	
	DFLEN = 0;
}

/***********************************************************************************************************************
* Function Name: Write_Dconfig_Data_Flash
* Description  : This function used to Write latest fanstate & Mute Condition
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void 	Write_Dconfig_Data_Flash(void)
 {
	uint8_t DF_Error = 0, Err_Flag = 1,CS = 0,temp[120];
	pfdl_status_t my_status;
	__near pfdl_request_t request_pfdl;
	pfdl_descriptor_t pfdl_init;
	pfdl_init.fx_MHz_u08 = 12;					// Clock Frequency
	pfdl_init.wide_voltage_mode_u08 = 0x00;		// Full voltage mode - 0x00 - 2.7V-5.5V
	if(DCONFIG_DF_START_POSITION == u16_DConfig_DF_Write_Position)			// Ensure and erased block
	{
		Erase_Dconfig_Data_Flash();    										// Wide voltage mode - 0x01 - 1.8V-5.5V
	}
	while(Err_Flag)
	{	
		my_status = PFDL_Open(&pfdl_init);
	  	if (my_status == PFDL_OK)
	    	{
			request_pfdl.index_u16 = u16_DConfig_DF_Write_Position;
			request_pfdl.data_pu08 = &D_Config_Packet.All[0];
			request_pfdl.bytecount_u16 = CONFIG_DF_DATA_SIZE; //32
			request_pfdl.command_enu = PFDL_CMD_WRITE_BYTES; 
			my_status = PFDL_Execute(&request_pfdl);
			while(my_status == PFDL_BUSY)
			{
				my_status = PFDL_Handler();
			}
			if(my_status == PFDL_OK)
				Err_Flag = 0;			
			else if (my_status == PFDL_ERR_WRITE)
				DF_Error ++;			
		}
		else if(my_status != PFDL_OK )
			DF_Error ++;
		PFDL_Close();	
		if(DF_Error > 4)
		{
			//Drive_Error = 5;
			break;
		}
	}
	u16_DConfig_DF_Write_Position += CONFIG_DF_DATA_SIZE;
	if((DCONFIG_DF_ALLOTTED_MEMORY + DCONFIG_DF_START_POSITION) == u16_DConfig_DF_Write_Position) // Block end reached
	{
		u16_DConfig_DF_Write_Position = DCONFIG_DF_START_POSITION;
	}
}

/***********************************************************************************************************************
* Function Name: Erase_RFID_Data_Flash
* Description  : This function used to Erase all 4KB Data Flash
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void 	Erase_RFID_Data_Flash		(void)
{
	uint16_t DF_Block;
	uint8_t DF_Error = 0, Err_Flag = 1;
	pfdl_status_t my_status;
	
	__near pfdl_request_t request_pfdl;
	pfdl_descriptor_t pfdl_init;

	pfdl_init.fx_MHz_u08 = 12;			// Clock Frequency
	pfdl_init.wide_voltage_mode_u08 = 0x00;		// Full voltage mode - 0x00 - 2.7V-5.5V
	    						// Wide voltage mode - 0x01 - 1.8V-5.5V
	while(Err_Flag)
	{
		my_status = PFDL_Open(&pfdl_init);
	  	if (my_status == PFDL_OK)
	    	{
			DF_Block = RFID_WRITE_START_BLOCK;
			while(DF_Block < RFID_WRITE_END_BLOCK)
			{	
				request_pfdl.index_u16 = DF_Block;
				request_pfdl.command_enu = PFDL_CMD_ERASE_BLOCK;
				my_status = PFDL_Execute(&request_pfdl);
				while(my_status == PFDL_BUSY)
				{
		    			my_status = PFDL_Handler();
				}
				if(my_status == PFDL_OK)
		 			DF_Block ++;
			}
			if(my_status == PFDL_ERR_ERASE )
				DF_Error ++;
			else if(my_status == PFDL_OK)
				Err_Flag = 0;
		}
		else if(my_status != PFDL_OK )
			DF_Error ++;
		PFDL_Close();
		if(DF_Error > 4)
		{
			break;
		}
	}	
	DFLEN = 0;
}
/*****************************************************************************************
* Function name	 	: 	void v_Write_RFID_DF_data(gsm_data_t *gsm_data)
* Created by 		: 	jomon
* Date created 		: 	29-03-2017
* gsm_data_t          : gsm structure
* Description 		: 	
* Notes 		: 	
******************************************************************************************/  
void v_Write_RFID_DF_data(void)
{
	uint8_t DF_Error = 0,j,u8_checksum = 0,temp[8];
	uint8_t Err_Flag = 1;
	uint16_t i;
	pfdl_status_t my_status;
	__near pfdl_request_t request_pfdl;
	pfdl_descriptor_t pfdl_init;
	if(u32_RFID_DF_Write_Position == RFID_DF_START_POSITION)
	{
	   Erase_RFID_Data_Flash();			
	}
	
	pfdl_init.fx_MHz_u08 = 12;					// Clock Frequency
	pfdl_init.wide_voltage_mode_u08 = 0x00;		// Full voltage mode - 0x00 - 2.7V-5.5V
	    										// Wide voltage mode - 0x01 - 1.8V-5.5V
	while(Err_Flag)
	{	
		my_status = PFDL_Open(&pfdl_init);
	  	if (my_status == PFDL_OK)
	    	{
			request_pfdl.index_u16 = (uint16_t)u32_RFID_DF_Write_Position;//DF_Address;
			request_pfdl.data_pu08 = &RFID_DF_Data_Packet.All[0];  
			request_pfdl.bytecount_u16 = RFID_DF_DATA_SIZE;
			request_pfdl.command_enu = PFDL_CMD_WRITE_BYTES; 
			my_status = PFDL_Execute(&request_pfdl);
			while(my_status == PFDL_BUSY)
			{
				my_status = PFDL_Handler();
			}
			if(my_status == PFDL_OK)
				Err_Flag = 0;			
			else if (my_status == PFDL_ERR_WRITE)
				DF_Error ++;			
		}
		else if(my_status != PFDL_OK )
			DF_Error ++;
		PFDL_Close();	
		if(DF_Error > 4)
		{
			//Drive_Error = 5;
			break;
		}
	}
	DFLEN = 0;
	u32_RFID_DF_Write_Position += RFID_DF_DATA_SIZE;
	if((RFID_DF_ALLOTTED_MEMORY + RFID_DF_START_POSITION) == u32_RFID_DF_Write_Position)
	{
		u32_RFID_DF_Write_Position = RFID_DF_START_POSITION;
	}
			
}
 
 /*****************************************************************************************
* Function name	 	: 	void v_Write_Cert_DF_data(void)
* Created by 		: 	
* Date created 		: 	
* Description 		: 	
* Notes 		: 	
******************************************************************************************/
void v_Write_Cert_DF_data_flash(void)
{
	uint8_t DF_Error = 0,j,u8_checksum = 0,temp[8];
	uint8_t Err_Flag = 1;
	uint16_t i;
	pfdl_status_t my_status;
	__near pfdl_request_t request_pfdl;
	pfdl_descriptor_t pfdl_init;
	//if(u32_Cert_DF_Write_Position == CERT_DF_START_POSITION)
	//{
	//   ;//Erase_RFID_Data_Flash();			
	//}
	
	pfdl_init.fx_MHz_u08 = 12;					// Clock Frequency
	pfdl_init.wide_voltage_mode_u08 = 0x00;		// Full voltage mode - 0x00 - 2.7V-5.5V
	    										// Wide voltage mode - 0x01 - 1.8V-5.5V
	if(PEM_CERT == 1)
	{
											
	while(Err_Flag)
	{	
		my_status = PFDL_Open(&pfdl_init);
	  	if (my_status == PFDL_OK)
	    	{
			request_pfdl.index_u16 = (uint16_t)u32_Cert_DF_Write_Position;//DF_Address;
			request_pfdl.data_pu08 = &New_PEM_Cert_Buffer;  
			request_pfdl.bytecount_u16 = CERT_DF_DATA_SIZE1;
			request_pfdl.command_enu = PFDL_CMD_WRITE_BYTES; 
			my_status = PFDL_Execute(&request_pfdl);
			while(my_status == PFDL_BUSY)
			{
				my_status = PFDL_Handler();
			}
			if(my_status == PFDL_OK)
				Err_Flag = 0;			
			else if (my_status == PFDL_ERR_WRITE)
				DF_Error ++;			
		}
		else if(my_status != PFDL_OK )
			DF_Error ++;
		PFDL_Close();	
		if(DF_Error > 4)
		{
			//Drive_Error = 5;
			break;
		}
	}
	DFLEN = 0;
	u32_Cert_DF_Write_Position += CERT_DF_DATA_SIZE1;
	if((CERT_DF_ALLOTTED_MEMORY + CERT_DF_START_POSITION) == u32_Cert_DF_Write_Position)
	{
		u32_Cert_DF_Write_Position = CERT_DF_START_POSITION;
	}
	}
	
	else if(PVT_CERT == 1)
	{
		
	while(Err_Flag)
	{	
		my_status = PFDL_Open(&pfdl_init);
	  	if (my_status == PFDL_OK)
	    	{
			request_pfdl.index_u16 = (uint16_t)u32_CertPvt_DF_Write_Position;//DF_Address;
			request_pfdl.data_pu08 = &New_Pvt_Cert_Buffer;  
			request_pfdl.bytecount_u16 = CERTPvt_DF_DATA_SIZE2;
			request_pfdl.command_enu = PFDL_CMD_WRITE_BYTES; 
			my_status = PFDL_Execute(&request_pfdl);
			while(my_status == PFDL_BUSY)
			{
				my_status = PFDL_Handler();
			}
			if(my_status == PFDL_OK)
				Err_Flag = 0;			
			else if (my_status == PFDL_ERR_WRITE)
				DF_Error ++;			
		}
		else if(my_status != PFDL_OK )
			DF_Error ++;
		PFDL_Close();	
		if(DF_Error > 4)
		{
			//Drive_Error = 5;
			break;
		}
	}
	DFLEN = 0;
	u32_CertPvt_DF_Write_Position += CERTPvt_DF_DATA_SIZE2;
	if((CERTPvt_DF_ALLOTTED_MEMORY + CERTPvt_DF_START_POSITION) == u32_Cert_DF_Write_Position)
	{
		u32_CertPvt_DF_Write_Position = CERTPvt_DF_START_POSITION;
	}
	}
		
	write_success ++;
}


/***********************************************************************************************************************
* Function Name: Erase_RFID_Data_Flash
* Description  : This function used to Erase 4KB-8KB Data Flash
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void Erase_Cert_Data_Flash(void)
{
	uint16_t DF_Block;
	uint8_t DF_Error = 0, Err_Flag = 1;
	pfdl_status_t my_status;
	
	__near pfdl_request_t request_pfdl;
	pfdl_descriptor_t pfdl_init;

	pfdl_init.fx_MHz_u08 = 12;			// Clock Frequency
	pfdl_init.wide_voltage_mode_u08 = 0x00;		// Full voltage mode - 0x00 - 2.7V-5.5V
	    						// Wide voltage mode - 0x01 - 1.8V-5.5V
	while(Err_Flag)
	{
		my_status = PFDL_Open(&pfdl_init);
	  	if (my_status == PFDL_OK)
	    	{
			DF_Block = CERT_WRITE_START_BLOCK;
			while(DF_Block < CERT_WRITE_END_BLOCK)
			{	
				request_pfdl.index_u16 = DF_Block;
				request_pfdl.command_enu = PFDL_CMD_ERASE_BLOCK;
				my_status = PFDL_Execute(&request_pfdl);
				while(my_status == PFDL_BUSY)
				{
		    			my_status = PFDL_Handler();
				}
				if(my_status == PFDL_OK)
		 			DF_Block ++;
			}
			if(my_status == PFDL_ERR_ERASE )
				DF_Error ++;
			else if(my_status == PFDL_OK)
				Err_Flag = 0;
		}
		else if(my_status != PFDL_OK )
			DF_Error ++;
		PFDL_Close();
		if(DF_Error > 4)
		{
			break;
		}
	}	
	DFLEN = 0;
}






