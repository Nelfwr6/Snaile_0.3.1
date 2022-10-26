/*******************************************************************************

* Module name: adc.c

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

* adc program

*
***************************************************************************************/
/*************************** HEADER FILES *********************************************/


#include "r_cg_macrodriver.h"
#include "r_cg_adc.h"
#include "adc.h"


/*******************************************************************************

* Global Variable Section

*******************************************************************************/
uint16_t u16_ADC_Value = 0;

/*****************************************************************************************
* Function name	 	: 	void	v_Init_ADC	(void)
* returns 		: 	void
* arg1 			: 	void
* Created by 		: 	Reshma Joseph
* Date created 		: 	08-22-2014
* Edited By		:	Reshma Joseph
* Description 		: 	ADC initialisation
* Notes 		: 	Edited for ADC initialisation
******************************************************************************************/

void	v_Init_ADC	(void)
{
	R_ADC_Set_OperationOn();							        // Sample and Hold disabled
}
/*****************************************************************************************
* Function name	 	: 	uint16_t u16_AD_Conv(uint8_t u8_ADC_Channel)
* returns 		: 	ADC result
* u8_ADC_Channel 	: 	ADC channel selection
* Created by 		: 	Reshma Joseph
* Date created 		: 	08-22-2014
* Edited By		:	Reshma Joseph
* Description 		: 	ADC channel selection and return the adc result
* Notes 		: 	Edited for ADC channel selection and return the adc result
******************************************************************************************/
uint16_t u16_AD_Conv(uint8_t u8_ADC_Channel)
{
	ADS = u8_ADC_Channel;
	R_ADC_Start();
	while(ADCS == 1)
	{
		NOP();
	}
	R_ADC_Get_Result(&u16_ADC_Value);
	R_ADC_Stop();									//Stops ADC
	ADIF = 0;
	return u16_ADC_Value;
	
}

