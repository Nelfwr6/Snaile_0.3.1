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
#include <stdlib.h>
#include "string.h"
#include "r_cg_serial.h"
#include "r_cg_timer.h"
#include "r_xmodem.h"
#include "r_typedefs.h"
#include "fsl.h"
#include "fsl_types.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/*******************************************************************************

* Global variable section

*******************************************************************************/

/* Start user code for global. Do not edit comment generated here */
/********************************************************************************
Macro definitions
********************************************************************************/
// Debug mode
#define FLASH_END_ADDRESS 0x07FBFF   // need to reserve last 512bytes and memory
                                    // per block is 0x400 = 1024 bytes, so,
                                    // 0xFFFF - 0x400  = 0xFC00 - 1 = 0xFBFF
        

// Release mode
//#define FLASH_END_ADDRESS 0x0FFFF



#define FLASH_START_ADDRESS 0x3000
#define CHECKSUM_ADDRESS    (uint16_t)(FLASH_END_ADDRESS - 3)
/* End user code. Do not edit comment generated here */

/********************************************************************************
* Function Name : WriteFlash
* Description	: This function initializes the TAU0 Channel 0 module.		
* Parameters	: addr - 
*					start address to write
*				  data[] -
*				  	data buffer to write
*				  len -
*				  	size of data buffer
* Return value  : fsl_u08, status_code                                          
*                   = 0x00(FSL_OK), normal                                      
*                   = 0x05(FSL_ERR_PARAMETER), parameter error                   
*                   = 0x10(FSL_ERR_PROTECTION), protection error                 
*                   = 0x1C(FSL_ERR_WRITE), write error                           
*                   = 0x1F(FSL_ERR_FLOW), last operation has not finished, yet.  
*                                      or violates the precondition.             
*                                      or FSL is suspending.                     
*                   = 0xFF(FSL_BUSY), normal and means "process was started"
********************************************************************************/
fsl_u08 WriteFlash(uint32_t addr, int8_t data[], uint8_t size)
{
	__near fsl_write_t my_fsl_write_str;	
	
	my_fsl_write_str.fsl_data_buffer_p_u08 = (__near fsl_u08 *) data;
	my_fsl_write_str.fsl_word_count_u08 = size/4;
	my_fsl_write_str.fsl_destination_address_u32 = addr;
		
	return FSL_Write((__near fsl_write_t*) &my_fsl_write_str);
}

/********************************************************************************
* Function Name : GetByte
* Description	: This function recieves a byte of UART0 data.		
* Parameters	: timeout -
*					time limit for reception
*				  data -
*				  	data buffer pointer
* Return value  : uint8_t, status
*					TRUE or FALSE
********************************************************************************/
uint8_t GetByte (uint32_t timeout, int8_t *data)
{
	uint32_t tick_count;
	
	tick_count = timeout;
	R_TAU0_Channel0_Start();
	
	/* wait for a byte to arrive */
	while ((SRIF1 == 0) && (tick_count))
	{
		WDTE = 0xac;
		if (TMIF00 == 1)
		{
			TMIF00 = 0;
			R_TAU0_Channel0_Stop();
			if (--tick_count)
			{
				R_TAU0_Channel0_Start();
			}
		}
	}
	
	if (tick_count == 0)
	{
		*data = '0';
		return FALSE;
	}
		
	*data=(uint8_t) RXD1;
	SRIF1 = 0;

	return TRUE;
}


void v_delay_ms(uint16_t ms_delay)
{
	uint32_t tick_count;
	
	tick_count = ms_delay;
	R_TAU0_Channel0_Start();
	
	/* wait for a byte to arrive */
	while (tick_count)
	{
		WDTE = 0xac;
		if (TMIF00 == 1)
		{
			TMIF00 = 0;
			R_TAU0_Channel0_Stop();
			if (--tick_count)
			{
				R_TAU0_Channel0_Start();
			}
		}
	}
}

void convert(char *data,uint32_t a) //definition
{
     uint32_t temp=a;
     uint32_t digits=0;
     uint16_t i=0,mu=1,x=0;
     uint16_t j=0;
     //exponent component
     if(temp)
     {
	     while(temp!=0)
	     {
	         temp=temp/10;
	         digits++;          
	     }
	     while(digits!=0)
	     {
	         if(digits==1)mu=1;
	         else  for(j=2;j<=digits;j++)mu=mu*10;
	         
	         x=a/mu;
	         a=a-((int)x*mu);
	         data[i]=0x30+((int)x);
	         i++;
	         digits--;
	         mu=1;
	     }
     }
     else
     data[i++] = 0x30;
     
    data[i]='\0';
}

uint8_t u8_Gsm_Wait_For_AT_response(uint32_t time)
{
	int8_t Status = 0;
	int8_t data[100] = {0};
	uint16_t count = 0;
	while (time){
    			WDTE = 0xAC;
			R_UART1_Send_String((__far int8_t*)"AT\r\n");
    			count = 0;
			while (1)
			{
				WDTE = 0xac;
				/* recieve each byte of Rx data with 1 second timeout condition*/
				Status = GetByte(1000, &data[count]);   
				if (Status == FALSE)
				{
					break;
				}
	
				count++;
				if(count > 100)
				count = 0;
			}
    			if (count > 0 )
			{
				Status = ATResponse_Handler(data);
				if(Status == RIL_ATRSP_SUCCESS)
				break;
			}
			time--;
			
    		}
	return Status;
}


int8_t u8_File_Open(void)
{
	int8_t Status = -1;	
	int8_t data[100] = {0};
	uint16_t count = 0;
			R_UART1_Send_String((__far int8_t*)"AT+QFOPEN=\"OTA_BIN.bin\",2\r");
    			count = 0;
			while (1)
			{
				WDTE = 0xac;
				/* recieve each byte of Rx data with 1 second timeout condition*/
				Status = GetByte(10000, &data[count]);   
				if (Status == FALSE)
				{
					break;
				}
				count++;
				if(count > 100)
				count = 0;
			}
    			if (count > 0 )
			{
				Status = ATResponse_Handler(data);
				//break;
			}
	return Status;
}


uint8_t u8_File_Close(void)
{
	uint8_t Status = 0;	
	int8_t data[100] = {0};
	uint16_t count = 0;
	R_UART1_Send_String((__far int8_t*)"AT+QFCLOSE=4\r");
//	u8_Status = u8_GSM_GPRS_reception_Handler(10);
	//R_UART1_Send_String((__far int8_t*)"AT+QFOPEN=\"UFS:OTA_BIN.bin\",2\r");
	count = 0;
	while (1)
	{
		WDTE = 0xac;
		/* recieve each byte of Rx data with 1 second timeout condition*/
		Status = GetByte(300, &data[count]);   
		if (Status == FALSE)
		{
			break;
		}
		count++;
		if(count > 100)
		count = 0;
	}
	if (count > 0 )
	{
		Status = ATResponse_Handler(data);
		//break;
	}
	return Status;
}


uint8_t u8_File_Erase(void)
{
   
	uint8_t Status = 0;	
	int8_t data[100] = {0};
	uint16_t count = 0;
	v_delay_ms(1000);
    R_UART1_Send_String((__far int8_t*)"AT+QFDEL=\"*\"\r");//OTA_BIN.bin
    v_delay_ms(2000);
   // u8_Status = u8_GSM_GPRS_reception_Handler(10);
 count = 0;
	while (1)
	{
		WDTE = 0xac;
		/* recieve each byte of Rx data with 1 second timeout condition*/
		Status = GetByte(500, &data[count]);   
		if (Status == FALSE)
		{
			break;
		}
		count++;
		if(count > 100)
		count = 0;
	}
	if (count > 0 )
	{
		Status = ATResponse_Handler(data);
		//break;
	}
    return Status;
    
    
}



uint8_t u8_Read_from_UFS(uint16_t flash_start_address)
{
	
	uint16_t		ret;
	uint8_t			expected_blk_num;
	uint8_t			retry_counter;
	uint32_t		rx_byte_count;
	uint8_t			rx_byte_buffer_index;
	uint8_t			status;
	uint8_t			checksum;
	uint8_t			start_condition;
	uint32_t		Address;
	uint8_t			tmp;
	uint32_t		ROM_checksum;
	uint32_t		UART_checksum;
	uint8_t			CHK[4] = {0};
	uint8_t			rx_buffer[XMODEM_BUFFER_SIZE];
	__far int8_t* 	ptr,* ptr2;
	
	uint32_t offset = 0, file_size = 0, Data_count = 0;
	uint32_t count = 0, i =0, j = 0;
	char *p = 0;
	int8_t Status = 0;	
	int8_t data[1200] = {0},temp_buff[100];	
	
	/* first block number is 1 */
	expected_blk_num = 1;

	start_condition = TRUE;
	
	UART_checksum = 0;
	
	ROM_checksum = 0;

	Address = flash_start_address;	
	
	
	

//		sprintf((char *)u8_temp_buff,"AT+QFSEEK=3000,%d,0\r",offset);
		//sprintf((char *)u8_temp_buff,"AT+QFSEEK=?\r");
//		R_UART1_Send_String((__far int8_t*)u8_temp_buff);
//		u8_Status = u8_GSM_GPRS_reception_Handler(10);
//		if(u8_Status == GSM_SUCCESS)
//		{   sprintf((char *)u8_temp_buff,"AT+QFREAD=3000,%d\r",u32_Data_count);
//			R_UART1_Send_String((__far int8_t*)u8_temp_buff);
//		    u8_Data_Read_Flag = 1;
//			u8_Status = u8_GSM_GPRS_reception_Handler(10);

//			u8_Status = u8_GSM_GPRS_reception_Handler(100);
//		}

	//sprintf((char *)temp_buff,"AT+QFSEEK=3000,%d,0\r",offset);
	R_UART1_Send_String((__far int8_t*)"AT+QFSEEK=4,0,2\r");
	count = 0;
	while (1)
	{
		WDTE = 0xac;
		/* recieve each byte of Rx data with 1 second timeout condition*/
		Status = GetByte(1000, &data[count]);   
		if (Status == FALSE)
		{
			break;
		}
		count++;
		if(count > 100)
		count = 0;
	}
	if (count > 0 )
	{
		Status = ATResponse_Handler(data);
		//break;
	}
	
	R_UART1_Send_String((__far int8_t*)"AT+QFPOSITION=4\r");
	count = 0;
	while (1)
	{
		WDTE = 0xac;
		/* recieve each byte of Rx data with 1 second timeout condition*/
		Status = GetByte(1000, &data[count]);   
		if (Status == FALSE)
		{
			break;
		}
		count++;
		if(count > 100)
		count = 0;
	}
	if (count > 0 )
	{
		Status = ATResponse_Handler(data);
		
		if(Status == RIL_ATRSP_SUCCESS)
		{
			p = strstr((__far char*)data,(__far char*)"+QFPOSITION: ");
			if(p)
			{	
				count = strlen("+QFPOSITION: ");
				while(p[count] != '\r')
				{
					//R_UART1_Send_Byte(p[count]);
					temp_buff[i++] = p[count++]; 
				}
				i -= 1;
				j = i;
				while(i != 0)
				{
					file_size = file_size + (uint32_t)(temp_buff[i] - '0') * powerof(10,j-i);
					i--;
				}
				file_size = file_size + (uint32_t)(temp_buff[i] - '0') * powerof(10,j-i);
				
				offset = 0;
				i = 0;
				Data_count = file_size/1024;
				while(1)
				{
					
					R_UART1_Send_String((__far int8_t*)"AT+QFSEEK=4,");
					convert(temp_buff, offset);
					R_UART1_Send_String((__far int8_t*)temp_buff);
					R_UART1_Send_String((__far int8_t*)",0\r");
					count = 0;
					while (1)
					{
						WDTE = 0xac;
						/* recieve each byte of Rx data with 1 second timeout condition*/
						Status = GetByte(1500, &data[count]);   
						if (Status == FALSE)
						{
							break;
						}
						count++;
						if(count > 100)
						count = 0;
					}
					if (count > 0 )
					{
						Status = ATResponse_Handler(data);
						//break;
					}
						
					//sprintf((__far char *)temp_buff,"AT+QFREAD=3000,%d\r",Data_count);
					R_UART1_Send_String((__far int8_t*)"AT+QFREAD=4,1024\r");
					count = 0;
					while (1)
					{
						WDTE = 0xac;
						/* recieve each byte of Rx data with 1 second timeout condition*/
						Status = GetByte(1000, &data[count]);   
						if (Status == FALSE)
						{
							break;
						}
						
						
						count++;
						if(count > 1199)
						count = 0;
					}
					if (count > 0 )
					{
						Status = ATResponse_Handler(data);
						
						if(Status == RIL_ATRSP_CONNECT || Status == RIL_ATRSP_SUCCESS || Status == RIL_ATRSP_CONTINUE)
						{
							
							p = strstr((__far char*)data,(__far char*)"CONNECT ");
							p = strstr((__far char*)p,(__far char*)"\r\n");
							//p = data;
							p += 2;
							
							if(i == Data_count)
							{
								for(count = 0; count<(file_size - Data_count * 1024)/128 + 1; count++)
								{
									if(count == (file_size - Data_count * 1024)/128)
									{
										memset(&rx_buffer[0],0x00,128);
										memcpy(&rx_buffer[0],p,(file_size - Data_count * 1024)%128);
									}
									else
									{
										memset(&rx_buffer[0],0x00,128);
										memcpy(&rx_buffer[0],p,128);
									}
									
									for (rx_byte_buffer_index=0; rx_byte_buffer_index<128; rx_byte_buffer_index++)
									{
										checksum += rx_buffer[ rx_byte_buffer_index];
										tmp = rx_buffer[ rx_byte_buffer_index];
										UART_checksum += tmp;
									}
									if ((Address<= FLASH_END_ADDRESS) && (Address >= flash_start_address))
									{
										
										status = WriteFlash(Address, (int8_t*)&rx_buffer[0], 128);
									}
									else 
									{
										status = XM_ADDRESS_ERROR;
									}
									Address += 128;
									p += 128;
								}
								ptr=(__far int8_t*)flash_start_address;
								ptr2 = ptr;//+ Address;
								for (ptr=(__far int8_t*)flash_start_address; ptr<ptr2+ Address; ptr++)
								{
									tmp = *ptr;
									ROM_checksum += tmp;
								}
								if (1)//ROM_checksum == UART_checksum)
								{
									CHK[0] = (uint8_t)(UART_checksum & 0x00FF);//(ROM_checksum & 0x00FF);
									CHK[1] = (uint8_t)((UART_checksum & 0xFF00)>>8);//((ROM_checksum & 0xFF00)>>8);
									CHK[2] = (uint8_t)(Address & 0x00FF);
									CHK[3] = (uint8_t)((Address & 0xFF00)>>8);

				                    			if (flash_start_address < FLASH_START_ADDRESS)
									{
										/* Write checksum value into the end of boot area*/
										WriteFlash(0x1FFC, (int8_t*)CHK, 4);
									}
									else
									{
										/* Write checksum value into the end of flash area*/
										WriteFlash(CHECKSUM_ADDRESS, (int8_t*)CHK, 4);
									}
									
									return (XM_OK);
								}
								else
								{
									R_UART1_Send_Byte((int8_t)(ROM_checksum & 0x00FF));
									R_UART1_Send_Byte((int8_t)((ROM_checksum & 0xFF00)>>8));
									R_UART1_Send_Byte((int8_t)(UART_checksum & 0x00FF));
									R_UART1_Send_Byte((int8_t)((UART_checksum & 0xFF00)>>8));
									R_UART1_Send_Byte(status);
									while(1)
									{
										NOP();
									}
									return (XM_PROG_FAIL);
								}
							}
							else
							{
								for(count = 0; count<8; count++)
								{
									memset(&rx_buffer[0],0,128);
									memcpy(&rx_buffer[0],p,128);
									
									for (rx_byte_buffer_index=0; rx_byte_buffer_index < 128; rx_byte_buffer_index++)
									{
										//R_UART1_Send_Byte(rx_buffer[ rx_byte_buffer_index]);
										checksum += rx_buffer[ rx_byte_buffer_index];
										tmp = rx_buffer[ rx_byte_buffer_index];
										UART_checksum += tmp;
									}
									
									if ((Address<= FLASH_END_ADDRESS) && (Address >= flash_start_address))
									{
										
										status = WriteFlash(Address, (int8_t*)&rx_buffer[0], 128);
										
									}
									else 
									{
										status = XM_ADDRESS_ERROR;
										
										return (status);
									}
									if (status == FSL_OK)
									{
										/* if programming routine passed, then increase the flash address */
										Address += 128;
										p += 128;
										//expected_blk_num++;
										//R_UART1_Send_Byte(ACK);
									}
									else
									{
										/* programming failed */
										//R_UART1_Send_Byte(NAK);
										/* cancel xmodem download */
										//R_UART1_Send_Byte(CAN);
										return (XM_PROG_FAIL);
									}
									
								}
				
							}
							
							
							//file_size = file_size - 128;
							offset += 1024;
							i++;
						}
						else if(Status == RIL_ATRSP_FAILED)
						{
							break;
						}
					}
				}
			}
		}
		//break;
	}
	return Status;
	
}


void GSM_Initial_Configuration(void)
{
	v_Gsm_Hardware_Init();
	u8_Gsm_Wait_For_AT_response(14);
	
	//DEBUG_LED_ON();
	
	//v_Gsm_Power_Down();	//TODO
	//DEBUG_LED_OFF();
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
    GSM_POWER_ENABLE	= 1;       //GSM-P-EN (Enable input (1 enabled, 0 disabled))
    PS_SYNC		= 1;        //PS-SYNC(Enable/disable power save mode (1 disabled, 0 enabled))
    GSM_POWER_KEY = 0;	 
    GSM_RESET = 0;
    /* Power Section End */
    
    v_delay_ms(500);				// Delay for Power Supply to stabilise
	
    GSM_POWER_KEY = 1;	 			// GSM power key is Enabl
    v_delay_ms(150);				// 150 mSec delay
    GSM_POWER_KEY = 0;	 			// GSM power key is Disabled
   
    //GSM_WAKEUP_IN = 1;				// GSM Wake up command
    //GSM_W_DISABLE = 1;				// GSM wireless disable is disabled	
    
 //   u8_GSM_Status = GSM_INIT;		// Set GSM status for initialisation
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
	v_delay_ms(2000); 
}

int32_t ATResponse_Handler(char* line)
{
	//Ql_UART_Write(m_myUartPort, (u8*)line, len);

	
	if (strstr(line,"ERROR"))
	{
		return  RIL_ATRSP_FAILED;
	}
	else if (strstr(line,"+CME ERROR"))
	{
		return  RIL_ATRSP_FAILED;
	}
	else if (strstr(line,"+CMS ERROR:"))
	{
		return  RIL_ATRSP_FAILED;
	}
	else if (strstr(line,"CONNECT"))
	{
		return  RIL_ATRSP_CONNECT;
	}
	else if (strstr(line,"+QFOPEN:"))
	{
		return  RIL_ATRSP_QFOPEN;
	}
	else if (strstr(line,"OK"))
	{
		return  RIL_ATRSP_SUCCESS;
	}
	return RIL_ATRSP_CONTINUE; //continue wait
}

int32_t powerof(int8_t base, int8_t exp)
{
	int32_t result = 1;
	while (exp != 0) {
        result *= base;
        --exp;
    }
	return result;
}
