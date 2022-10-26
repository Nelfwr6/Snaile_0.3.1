/*******************************************************************************

* Module name: uart.c

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

* Uart program

*
***************************************************************************************/
/*************************** HEADER FILES *********************************************/


#include "r_cg_macrodriver.h"
#include "r_cg_serial.h"
#include <string.h>
uint8_t u8_uart0_Tx_Flag, u8_uart1_Tx_Flag, u8_uart2_Tx_Flag;
void v_cmd_send(uint8_t u8_send_data,uint8_t u8_channel)
{
     switch(u8_channel)
     {
	/*STMK0 =0U;     ENable INTST0 interrupt */
	case 0: //TXD0 = u8_send_data;
		R_UART0_Send(&u8_send_data, 1);
		while(!u8_uart0_Tx_Flag)
		{
			NOP();
		}
		u8_uart0_Tx_Flag = 0;
		break;
		
	case 1: //TXD1 = u8_send_data;
		R_UART1_Send(&u8_send_data, 1);
		while(!u8_uart1_Tx_Flag)
		{
			NOP();
		}
		u8_uart1_Tx_Flag = 0;
		break;
		
	case 2: //TXD2 = u8_send_data;
		R_UART2_Send(&u8_send_data, 1);
		while(!u8_uart2_Tx_Flag)
		{
			NOP();
		}
		u8_uart2_Tx_Flag = 0;
		break;
     }
}
void v_uart_str_send(const char *string,uint8_t u8_channel)
{
	while(*string)
	{
		v_cmd_send(*string++,u8_channel);
	}
}

void v_byte_array_send(uint8_t *u8_data, uint16_t u16_no_of_bytes,uint8_t u8_channel)
{
	uint8_t *u8_temp2 = u8_data + u16_no_of_bytes;
	for(; u8_data < u8_temp2; u8_data++)
		v_cmd_send(*u8_data,u8_channel);
}