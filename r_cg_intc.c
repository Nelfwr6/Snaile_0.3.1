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
* File Name    : r_cg_intc.c
* Version      : CodeGenerator for RL78/G13 V2.05.06.02 [08 Nov 2021]
* Device(s)    : R5F100FL
* Tool-Chain   : CCRL
* Description  : This file implements device driver for INTC module.
* Creation Date: 22-07-2022
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_intc.h"
/* Start user code for include. Do not edit comment generated here */
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
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_INTC_Create
* Description  : This function initializes INTP module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_INTC_Create(void)
{
    PMK0 = 1U;    /* disable INTP0 operation */
    PIF0 = 0U;    /* clear INTP0 interrupt flag */
    PMK1 = 1U;    /* disable INTP1 operation */
    PIF1 = 0U;    /* clear INTP1 interrupt flag */
    PMK2 = 1U;    /* disable INTP2 operation */
    PIF2 = 0U;    /* clear INTP2 interrupt flag */
    PMK3 = 1U;    /* disable INTP3 operation */
    PIF3 = 0U;    /* clear INTP3 interrupt flag */
    PMK4 = 1U;    /* disable INTP4 operation */
    PIF4 = 0U;    /* clear INTP4 interrupt flag */
    PMK5 = 1U;    /* disable INTP5 operation */
    PIF5 = 0U;    /* clear INTP5 interrupt flag */
    /* Set INTP3 high priority */
    PPR13 = 0U;
    PPR03 = 0U;
    EGP0 = _08_INTP3_EDGE_RISING_SEL;
    /* Set INTP3 pin */
    PM3 |= 0x01U;
}

/***********************************************************************************************************************
* Function Name: R_INTC3_Start
* Description  : This function clears INTP3 interrupt flag and enables interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_INTC3_Start(void)
{
    PIF3 = 0U;    /* clear INTP3 interrupt flag */
    PMK3 = 0U;    /* enable INTP3 interrupt */
}

/***********************************************************************************************************************
* Function Name: R_INTC3_Stop
* Description  : This function disables INTP3 interrupt and clears interrupt flag.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_INTC3_Stop(void)
{
    PMK3 = 1U;    /* disable INTP3 interrupt */
    PIF3 = 0U;    /* clear INTP3 interrupt flag */
}

/***********************************************************************************************************************
* Function Name: R_KEY_Create
* Description  : This function initializes the key return module.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_KEY_Create(void)
{
    volatile uint8_t  w_count;

    KRMK = 1U;  /* disable INTKR operation */
    KRIF = 0U;  /* clear INTKR interrupt flag */
    /* Set INTKR low priority */
    KRPR1 = 1U;
    KRPR0 = 1U;
    KRM = _00_KR0_SIGNAL_DETECT_OFF | _02_KR1_SIGNAL_DETECT_ON | _00_KR2_SIGNAL_DETECT_OFF | _00_KR3_SIGNAL_DETECT_OFF;
    /* Set KR1 pin */
    PM7 |= 0x02U;

    /* Change the waiting time according to the system */
    for (w_count = 0U; w_count < KEY_WAITTIME; w_count++)
    {   
        NOP();
    }
}

/***********************************************************************************************************************
* Function Name: R_KEY_Start
* Description  : This function clears INTKR interrupt flag and enables interrupt.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_KEY_Start(void)
{
    KRIF = 0U;  /* clear INTKR interrupt flag */
    KRMK = 0U;  /* enable INTKR operation */
}

/***********************************************************************************************************************
* Function Name: R_KEY_Stop
* Description  : This function disables INTKR interrupt and clears interrupt flag.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
void R_KEY_Stop(void)
{
    KRMK = 1U;  /* disable INTKR operation */
    KRIF = 0U;  /* clear INTKR interrupt flag */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
