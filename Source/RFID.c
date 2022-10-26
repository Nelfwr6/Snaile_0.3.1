/*******************************************************************************

* Module name: rfid.c

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

* First written on 08/22/2014 by jomon

*

* Module Description:

* adc.c program

*
***************************************************************************************/
/*************************** HEADER FILES *********************************************/


#include "r_cg_macrodriver.h"
#include "RFID.h"
#include "r_cg_serial.h"
#include "uart.h"
#include "interval_timer.h"
#include <string.h>
#include "EEPROM.h"
/*******************************************************************************

* Global Variable Section

*******************************************************************************/
RFID_DF_DATA RFID_DF_Data_Packet;
/*
uint8_t pooling_command[7] = {0xBB, 0x00, 0x22, 0x00, 0x00, 0x22, 0x7E};
const uint8_t if_power[11] = {0xBB, 0x00, 0xF0, 0x00, 0x04, 0x03, 0x06, 0x01, 0xB0, 0xAE, 0x7E};*/

RFID_PACKET_FORMAT  u8_RFID_Pool[RFID_MAX_PACKET_COUNT]; 
uint8_t u8_RFID_Data_Received[RFID_MAX_PACKET_COUNT];           // data received flag
uint8_t u8_RFID_Packet_Counter = 0, u8_RFID_Get_Packet_Counter = 0;
uint8_t u8_RFID_data_ptr = 0; 
uint16_t u16_RFID_len, u16_RFID_RX_Tmo;
uint16_t u16_RFID_Response_Tmo = 0; 
uint8_t u8_RFID_Parameter[10], u8_RFID_Overflag = 0;	
/*****************************************************************************************
* Function name	 	: 	void v_rfid_rx_isr(uint8_t u8_rfid_rx)
* returns 		: 	void
* arg1 			: 	void
* Created by 		: 	Jomon Thomas
* Date created 		: 	08-22-2017
* Description 		: 	rfid isr function

******************************************************************************************/
void v_rfid_rx_isr(uint8_t u8_rfid_rx)
{
	u16_RFID_RX_Tmo = 1000;
	if(((!u8_RFID_data_ptr) &&(u8_rfid_rx == 0xBB)) && (!u8_RFID_Data_Received[u8_RFID_Packet_Counter]))  //start byte
	{
		 u8_RFID_Overflag = 0;
		 u8_RFID_Pool[u8_RFID_Packet_Counter].All[u8_RFID_data_ptr] = u8_rfid_rx;
		 u8_RFID_Data_Received[u8_RFID_Packet_Counter] = RFID_PACKET_START;
		 u8_RFID_data_ptr++;
	}
	else if(u8_RFID_Data_Received[u8_RFID_Packet_Counter] == RFID_PACKET_START)
	{ 
	   
	    if(u8_RFID_data_ptr == 5)
	    {
		    u16_RFID_len = (u8_RFID_Pool[u8_RFID_Packet_Counter].All[3] << 8);
		    u16_RFID_len |= u8_RFID_Pool[u8_RFID_Packet_Counter].All[4];
	    }
	    else if((u8_RFID_data_ptr == u16_RFID_len + 5) && (u8_RFID_data_ptr > 5))
	    {
		    u8_RFID_Pool[u8_RFID_Packet_Counter].Member.u8_RFID_checksum = u8_rfid_rx;
		    u8_RFID_data_ptr++;
		    return;
	    }
	    else if((u8_RFID_data_ptr == u16_RFID_len + 6) && (u8_RFID_data_ptr > 5))
	    {
		    u8_RFID_Pool[u8_RFID_Packet_Counter].Member.u8_RFID_END = u8_rfid_rx;
		    u8_RFID_data_ptr = 0;
		    u8_RFID_Data_Received[u8_RFID_Packet_Counter] = RFID_DATA_RX_COMPLETE;
		    u8_RFID_Packet_Counter++;
		    if(u8_RFID_Packet_Counter >= RFID_MAX_PACKET_COUNT)
		    {
		    	u8_RFID_Packet_Counter = 0;
		    }
		    return;
	    }
	    
	    if( u8_RFID_data_ptr <= (RFID_MAX_RX_BUFFER_COUNT - 3) ) //rows, byte over flow prevention 
	    {
	    	u8_RFID_Pool[u8_RFID_Packet_Counter].All[u8_RFID_data_ptr] = u8_rfid_rx;
	    }
	    u8_RFID_data_ptr++;	
	    
	}
	else
	u8_RFID_Overflag = 1;
}	

/*****************************************************************************************
* Function name	 	: 	void v_RFID_Scheduler(void)
* returns 		: 	void
* arg1 			: 	void
* Created by 		: 	Aarathy Ramesh
* Date created 		: 	08-22-2017
* Description 		: 	rfid isr function

******************************************************************************************/
void v_RFID_Scheduler(void)
{
	if(u16_RFID_RX_Tmo)
	{
		u16_RFID_RX_Tmo--;
	}
	
	if(u16_RFID_Response_Tmo)
	{
		u16_RFID_Response_Tmo--;
	}
	
	if((!u16_RFID_RX_Tmo) && (u8_RFID_Data_Received[u8_RFID_Packet_Counter] == RFID_PACKET_START))
	{
		u8_RFID_data_ptr = 0;
		u8_RFID_Data_Received[u8_RFID_Packet_Counter] = RFID_PACKET_EMPTY;
	}
	
}

/*****************************************************************************************
* Function name	 	: 	void v_RFID_Turn_ON(void)
* returns 		: 	void
* arg1 			: 	void
* Created by 		: 	Jomon Thomas
* Date created 		: 	08-22-2017
* Description 		: 	function for turn on rfid

******************************************************************************************/
void v_RFID_Turn_ON(void)
{
	
    RFID_POWER_EN_PIN = 1;		
    v_delay_ms(50); /*depend upon reader*/
    RFID_RF_EN_PIN = 1;
    v_delay_ms(500);/*depend upon reader*/
}


/*****************************************************************************************
* Function name	 	: 	void v_RFID_Turn_OFF(void)
* returns 		: 	void
* arg1 			: 	void
* Created by 		: 	Jomon Thomas
* Date created 		: 	08-22-2017
* Description 		: 	function to turn off rfid

******************************************************************************************/
void v_RFID_Turn_OFF(void)
{
	
    RFID_POWER_EN_PIN = 0;
    RFID_RF_EN_PIN = 0;	
    v_delay_ms(50);
}

/*****************************************************************************************
* Function name	 	: 	void v_RFID_Command_Send(uint8_t u8_command, uint8_t *u8_parameter, uint16_t u16_paramter_len)
* returns 		: 	void
* u8_type		: 	Type
* u8_command 		: 	Command
* u8_parameter		: 	Paramter
* u16_paramter_len      : 	Paramter length
* Created by 		: 	Aarathy Ramesh
* Date created 		: 	08-22-2017
* Description 		: 	function for Send RFID Command

******************************************************************************************/
void v_RFID_Command_Send(uint8_t u8_type, uint8_t u8_command, uint8_t *u8_parameter, uint16_t u16_paramter_len)
{
	uint16_t u16_i = 0, u16_paramter_len_nibble;
	uint8_t u8_checksum = 0;
	u8_checksum += u8_type;
	u8_checksum += u8_command;
	u16_paramter_len_nibble = u16_paramter_len >> 8;
	u8_checksum += (uint8_t)u16_paramter_len_nibble;
	u16_paramter_len_nibble = u16_paramter_len & 0x0F;
	u8_checksum += (uint8_t)u16_paramter_len_nibble;
	for(u16_i = 0; u16_i < u16_paramter_len; u16_i++)
	{	
		u8_checksum += u8_parameter[u16_i];
	}
	v_RFID_Data_Send(0xBB);
	v_RFID_Data_Send(u8_type);
	v_RFID_Data_Send(u8_command);
	u16_paramter_len_nibble = u16_paramter_len >> 8;
	v_RFID_Data_Send((uint8_t)u16_paramter_len_nibble);
	u16_paramter_len_nibble = u16_paramter_len & 0x0F;
	v_RFID_Data_Send((uint8_t)u16_paramter_len_nibble);
	for(u16_i = 0; u16_i < u16_paramter_len; u16_i++)
	{
		v_RFID_Data_Send(u8_parameter[u16_i]);
	}
	v_RFID_Data_Send(u8_checksum);
	v_RFID_Data_Send(0x7E);
}
/*****************************************************************************************
* Function name	 	: 	void v_RFID_Data_Send(uint8_t u8_data)
* returns 		: 	void
* u8_data		: 	Data
* Created by 		: 	Aarathy Ramesh
* Date created 		: 	08-22-2017
* Description 		: 	function for Send RFID Data

******************************************************************************************/
void v_RFID_Data_Send(uint8_t u8_data)
{
	v_cmd_send(u8_data, RFID_UART_CHANNEL);
	
}

/*****************************************************************************************
* Function name	 	: 	uint8_t u8_RFID_Check_Packet_Received(uint8_t u8_data)
* returns 		: 	void
* u8_data		: 	Data
* Created by 		: 	Aarathy Ramesh
* Date created 		: 	08-22-2017
* Description 		: 	function for Send RFID Data

******************************************************************************************/
uint8_t u8_RFID_Check_Packet_Received(uint16_t u16_tmo)
{
	u16_RFID_Response_Tmo = u16_tmo;
	while (u16_RFID_Response_Tmo != 0)
	{
		if(u8_RFID_Get_Packet_Counter >= RFID_MAX_PACKET_COUNT)
		{
			u8_RFID_Get_Packet_Counter = 0;
		}
		if(u8_RFID_Data_Received[u8_RFID_Get_Packet_Counter] == RFID_DATA_RX_COMPLETE)
		{
			u8_RFID_Get_Packet_Counter++;
			return u8_RFID_Get_Packet_Counter;
		}
	}
	return 0;
}

/*****************************************************************************************
* Function name	 	: 	uint8_t u8_RFID_Clear_buf(void)

* Date created 		: 	08-22-2017
* Description 		: 	function for clearing previous data

******************************************************************************************/
void v_clear_RFID_data_buf(void)
{
	for(u8_RFID_Packet_Counter = 0; u8_RFID_Packet_Counter < RFID_MAX_PACKET_COUNT; u8_RFID_Packet_Counter++)
	{
	    for(u8_RFID_data_ptr = 0; u8_RFID_data_ptr < 24; u8_RFID_data_ptr++)
	    {
	       u8_RFID_Pool[u8_RFID_Packet_Counter].All[u8_RFID_data_ptr]  = 0x00;
	    }
	}
	memset(u8_RFID_Data_Received, 0x00, 20);
	u8_RFID_Packet_Counter = 0;
	u8_RFID_Get_Packet_Counter = 0;
	u8_RFID_data_ptr = 0;

}



/*****************************************************************************************
* Function name	 	: 	uint8_t u8_RFID_RX_Checksum_Check(void)
* returns 		: 	status
* u8_data_ptr		: 	Data pointer
* Created by 		: 	Aarathy Ramesh
* Date created 		: 	08-22-2017
* Description 		: 	function for Send RFID Data

******************************************************************************************/
uint8_t u8_RFID_RX_Checksum_Check(uint8_t u8_packet)
{
	uint8_t u8_checksum = 0;
	uint16_t u16_i = 0, u16_len = u8_RFID_Pool[u8_packet].Member.u8_RFID_len_MSB << 8;
	u16_len |= u8_RFID_Pool[u8_packet].Member.u8_RFID_len_LSB;
	for(u16_i = 1; u16_i < 5; u16_i++)
	{
		u8_checksum += u8_RFID_Pool[u8_packet].All[u16_i];
	}
	for(u16_i = 0; u16_i < u16_len; u16_i++)
	{
		u8_checksum += u8_RFID_Pool[u8_packet].Member.Paramter[u16_i];
	}
	if( (u8_checksum == u8_RFID_Pool[u8_packet].Member.u8_RFID_checksum) && (u8_RFID_Pool[u8_packet].Member.u8_RFID_END == 0x7E) )
	{
		return  RFID_SUCCESS;
		
	}
	else
	{
		return RFID_CHECKSUM_ERR;
	}
	
}


/*****************************************************************************************
* Function name	 	: 	void v_RFID_Poling_Command_Send(void)
* returns 		: 	Packet
* arg		     	: 	None
* Created by 		: 	Aarathy Ramesh
* Date created 		: 	08-22-2017
* Description 		: 	function for Send RFID Data

******************************************************************************************/
uint8_t u8_RFID_Poling_Command_Send(void)
{
	uint8_t u8_packet;
	v_RFID_Command_Send(RFID_COMMAND_FRAME, RFID_POLLING_COMMAND, u8_RFID_Parameter, 0);  //sending command
	
	u8_packet = u8_RFID_Check_Packet_Received(10000);
	return u8_packet;
	
}


/*****************************************************************************************
* Function name	 	: 	void u8_RFID_High_Sensitivity_Command_Send(void)
* returns 		: 	Packet
* arg		     	: 	None
* Created by 		: 	jomon
* Date created 		: 	08-22-2017
* Description 		: 	function for Send RFID Data

******************************************************************************************/
uint8_t u8_RFID_High_Sensitivity_Command_Send(void)
{
	uint8_t u8_packet;
	u8_RFID_Parameter[0] = 0x00;
	
	v_RFID_Command_Send(RFID_HIGH_SENSITIVITY_TYPE, RFID_HIGH_SENSITIVITY_COMMAND, u8_RFID_Parameter, 1);  //sending command
	
	u8_packet = u8_RFID_Check_Packet_Received(10000);
	return u8_packet;
	
}


/*****************************************************************************************
* Function name	 	: 	void u8_RFID_Dense_Reader_Command_Send(void)
* returns 		: 	Packet
* arg		     	: 	None
* Created by 		: 	jomon
* Date created 		: 	08-22-2017
* Description 		: 	function for Send RFID Data

******************************************************************************************/
uint8_t u8_RFID_Dense_Reader_Command_Send(void)
{
	uint8_t u8_packet;
	u8_RFID_Parameter[0] = 0x01;
	
	v_RFID_Command_Send(RFID_DENSE_READER_TYPE, RFID_DENSE_READER_COMMAND, u8_RFID_Parameter, 1);  //sending command
	
	u8_packet = u8_RFID_Check_Packet_Received(10000);
	return u8_packet;
	
}


/*****************************************************************************************
* Function name	 	: 	uint8_t u8_RFID_Set_Query_Command_Send(void)
* returns 		: 	Packet
* arg		     	: 	None
* Created by 		: 	jomon
* Date created 		: 	08-22-2017
* Description 		: 	function for Send query Data

******************************************************************************************/
uint8_t u8_RFID_Set_Query_Command_Send(void)
{
	uint8_t u8_packet;
	u8_RFID_Parameter[0] = 0x10;
	//u8_RFID_Parameter[1] = 0x20; //q=4
	u8_RFID_Parameter[1] = 0x28;  //q=5
	v_RFID_Command_Send(RFID_COMMAND_FRAME, RFID_SET_QUERY_COMMAND, u8_RFID_Parameter, 2);  //sending command
	
	u8_packet = u8_RFID_Check_Packet_Received(10000);
	return u8_packet;
	
}

/*****************************************************************************************
* Function name	 	: 	uint8_t u8_RFID_Reception_Handler(uint8_t u8_packet)
* returns 		: 	None
* arg		     	: 	None
* Created by 		: 	Jomon Thomas
* Date created 		: 	08-22-2017
* Description 		: 	function for Send RFID Data

******************************************************************************************/
uint8_t u8_RFID_Reception_Handler(uint8_t u8_packet)
{
    uint8_t u8_status = 0;
    if(u8_RFID_RX_Checksum_Check(u8_packet) == RFID_SUCCESS )
    {
	    u8_status = RFID_SUCCESS;
	    switch(u8_RFID_Pool[u8_packet].Member.u8_RFID_Type)
	    {
		    case RFID_COMMAND_FRAME:
		    break;
		    case RFID_RESPONSE_FRAME:
		    if(u8_RFID_Pool[u8_packet].Member.Paramter[0] == RFID_NO_TAG)
		    	u8_status = RFID_NO_CARD_DETECTED;
		    break;
		    case RFID_NOTICE_FRAME:
		    u8_status =  RFID_CARD_DETECTD;
		    u8_RFID_Data_Received[u8_packet] = RFID_DATA_READ_COMPLETE;
		    break;
	    }
	    
	    
    }
    else
    u8_status = RFID_FAILED;
    u8_RFID_Data_Received[u8_packet] = RFID_PACKET_EMPTY;
	
    return u8_status;
	
	
}