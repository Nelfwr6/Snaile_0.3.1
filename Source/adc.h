/*******************************************************************************

* Module name: adc.h

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
#ifndef _ADC_H  
#define _ADC_H  

/******************************* SETTINGS *********************************************/
/*************************** HEADER FILES *********************************************/
#include "r_cg_macrodriver.h"
/**************************************************************************************/
/********************************** USER SETTINGS *************************************/

//-------- ADC CHANNEL SELECTION----------------------------------------------------------------------//

#define 	AD0          			(0x00U) /* ANI0 */
#define	 	AD1          			(0x01U) /* ANI1 */
#define 	AD2          			(0x02U) /* ANI2 */
#define 	AD3          			(0x03U) /* ANI3 */

#define 	AD4         			(0x10U) /* ANI16 */
#define 	AD5         			(0x11U) /* ANI17 */
#define 	AD6         			(0x12U) /* ANI18 */
#define 	AD7         			(0x13U) /* ANI19 */




/************ FUNCTIONS *********************************************************/
extern void v_Init_ADC (void);
extern uint16_t u16_AD_Conv(uint8_t u8_ADC_Channel);
/*******************************************************************************/
#endif