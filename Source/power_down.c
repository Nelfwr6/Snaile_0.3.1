/*******************************************************************************
* Module name: interval_timer.c
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
* First written on 11/22/2014 by jomon.
*
* Module Description:
* RL78 G12 interval_timer program
*
*******************************************************************************/
/*******************************************************************************
* Include section
*
*******************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
/* Start user code for include. Do not edit comment generated here */
#include "power_down.h"
#include "interval_timer.h"
#include "r_cg_timer.h"
#include "adc.h"
#include "MQTT.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */

/* End user code. Do not edit comment generated here */



/***********************************************************************************************************************
* Function Name: uint8_t Select_Power_Mode (uint8_t level)
* Description  : To enter in to the stop mode
* Arguments    : level
* Return Value : None
***********************************************************************************************************************/
uint8_t Select_Power_Mode (uint8_t level)
{
    /* Declare a status flag */
    uint8_t status = 1;
    
    /* Temporary clock setting storage */
    uint8_t per0_copy;
    
    /*/* Change the drive voltage to capacitor split mode to reduce power consumption */

    /* Only execute the power mode selected */
    switch (level)
    {
        /* HALT mode */
        /*case HALT_Mode:
        {
            
            
            /* Send the MCU into HALT mode */
          /*  HALT();
        }
        break;
        
        /* STOP mode */
        case STOP_MODE:
        {
            /* Remove clock to all peripherals other than RTC */
            PER0 &= 0x80U;   // 8 for RTC anf 4 for UART
            /* RTC clock supplied to RTC and Interval Timer only */
            OSMC = 0x80;
            
            /* Add 1 instruction delay */
            NOP();
            NOP();
	    NOP();
	    NOP();
            /* Send the MCU into STOP mode */
            STOP();
            
            /* Add 1 instruction delay */
	  // v_Micro_Delay(10000);
            NOP();
	    NOP();
	    NOP();
	    NOP();
	    NOP();
	    NOP();
        }
        break;
        
        /* SNOOZE mode */
        
        default:
        {
            /* Return an error if the selected mode is not HALT, STOP or SNOOZE */
            status = 0;
        }
        break;
    }
    
    /* Add 5 instruction delays */
    NOP();
    
    /* Return the status */
    return (status);
}
/***********************************************************************************************************************
* Function Name: uint8_t Select_Power_Mode (uint8_t level)
* Description  : To enter in to the stop mode
* Arguments    : level
* Return Value : None
***********************************************************************************************************************/
void Reset_MCU(void)
{
    static const unsigned char illegalInstruction = 0xFF;
    void (*dummy) (void) = (void (*)(void))&illegalInstruction;
    dummy();
}

