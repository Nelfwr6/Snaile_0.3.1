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
#ifndef _EEPROM_H_ 
#define _EEPROM_H_  

/******************************* SETTINGS *********************************************/
/*************************** HEADER FILES *********************************************/
#include "r_cg_macrodriver.h"
/**************************************************************************************/
/********************************** USER SETTINGS *************************************/
extern uint8_t write_success;

/************ FUNCTIONS *********************************************************/
void 	Erase_Dconfig_Data_Flash(void);
void 	Write_Dconfig_Data_Flash(void);

void 	Erase_RFID_Data_Flash		(void);
void v_Write_RFID_DF_data(void);
void  Read_RFID_Data_Flash(void);



////////////////////////////////////////////
void v_Write_Cert_DF_data_flash(void);
void Erase_Cert_Data_Flash(void);
void  Read_Cert_Data_Flash(void);
////////////////////////////////////////////

/*******************************************************************************/

#endif
