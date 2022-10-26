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
* File Name    : r_cg_serial_user.c
* Version      : CodeGenerator for RL78/G13 V2.05.06.02 [08 Nov 2021]
* Device(s)    : R5F100FL
* Tool-Chain   : CCRL
* Description  : This file implements device driver for Serial module.
* Creation Date: 6/2/2022
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_serial.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#pragma interrupt r_uart1_interrupt_send(vect=INTST1)
#pragma interrupt r_uart1_interrupt_receive(vect=INTSR1)
#pragma interrupt r_uart1_interrupt_error(vect=INTSRE1)
/* Start user code for pragma. Do not edit comment generated here */
#if 0
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
extern volatile uint8_t * gp_uart1_tx_address;         /* uart1 send buffer address */
extern volatile uint16_t  g_uart1_tx_count;            /* uart1 send data number */
extern volatile uint8_t * gp_uart1_rx_address;         /* uart1 receive buffer address */
extern volatile uint16_t  g_uart1_rx_count;            /* uart1 receive data number */
extern volatile uint16_t  g_uart1_rx_length;           /* uart1 receive data length */
/* Start user code for global. Do not edit comment generated here */
#endif
#if 0
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_uart1_interrupt_receive
* Description  : This function is INTSR1 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_uart1_interrupt_receive(void)
{
    volatile uint8_t rx_data;
    
    rx_data = RXD1;

    if (g_uart1_rx_length > g_uart1_rx_count)
    {
        *gp_uart1_rx_address = rx_data;
        gp_uart1_rx_address++;
        g_uart1_rx_count++;
    }
}

/***********************************************************************************************************************
* Function Name: r_uart1_interrupt_error
* Description  : This function is INTSRE1 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_uart1_interrupt_error(void)
{
    volatile uint8_t err_type;

    *gp_uart1_rx_address = RXD1;
    err_type = (uint8_t)(SSR03 & 0x0007U);
    SIR03 = (uint16_t)err_type;
}

/***********************************************************************************************************************
* Function Name: r_uart1_interrupt_send
* Description  : This function is INTST1 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void __near r_uart1_interrupt_send(void)
{
    if (g_uart1_tx_count > 0U)
    {
        TXD1 = *gp_uart1_tx_address;
        gp_uart1_tx_address++;
        g_uart1_tx_count--;
    }
}

/* Start user code for adding. Do not edit comment generated here */
#endif
/***********************************************************************************************************************
* Function Name: R_UART1_ReceiveStart
* Description  : This function receives UART1 data.
* Arguments    : rxlength -
                     Length of receive data pointer
                 rxbuf -
                     Receive buffer pointer
* Return Value : NORMAL_END -
                     Normal end
                 ERROR -
                     Receive data error
***********************************************************************************************************************/
uint8_t R_UART1_ReceiveStart(uint16_t *rxlength, uint8_t *rxbuf)
{
    uint16_t datacount = 0U;
    uint8_t  rxdata;
    uint8_t  len[2];
    uint8_t  ret       = NORMAL_END;
    uint8_t  rxstatus  = NO_RECEIVE;
    uint8_t  lencount  = 0U;

    *rxlength = 0xFFFFU;                                                      /* Initialize packet length data */

    do
    {
        /* ---- Wait for receive data ---- */
        while ((SSR03 & 0x20U) == 0x00U)
        {
            NOP();
        }

        /* ---- Check receive error ---- */
        if ((SSR03 & 0x27U) == 0x20U)
        {
            rxdata = RXD1;                                                  /* Get receive data */

            switch (rxstatus)
            {
                /* ---- When rxstatus = NO_RECEIVE ---- */
                case NO_RECEIVE:
                    if (rxdata == 0x01U)
                    {
                        rxstatus = START_CODE;                              /* Change rxstatus */
                    }
                    else
                    {
                        ret = ERROR;                                        /* Set return value */
                    }
                    break;
                /* ---- When rxstatus = START_CODE ---- */
                case START_CODE:
                    len[lencount] = rxdata;
                    lencount++;                                             /* Increase receive count ( length data ) */

                    if (lencount == 2U)
                    {
                        rxstatus  = PACKET_SIZE;                            /* Change rxstatus */
                        *rxlength = ((uint16_t)len[0] << 8) | len[1];   /* Set packet length */
                    }
                    else
                    {
                        /* Do Nothing */
                    }
                    break;
                /* ---- When rxstatus = PACKET_SIZE ---- */
                case PACKET_SIZE:
                    *rxbuf = rxdata;                                        /* Set receive data */
                    rxbuf++;                                                /* Increase rx buffer pointer */
                    datacount++;                                            /* Increase receive count ( data ) */
                    break;

                default:
                    break;
            }
        }
        else
        {
            ret = ERROR;
            break;
        }
    }
    while ((datacount != *rxlength) && (ret == NORMAL_END));
    return ret;
}

/***********************************************************************************************************************
* Function Name: R_UART1_ReceiveStart
* Description  : This function sends UART1 data.
* Arguments    : txlength -
                     Length of send data
                 txbuf -
                     Send buffer pointer
* Return Value : NORMAL_END -
                     Normal end
                 ERROR -
                     Paramter data error
***********************************************************************************************************************/
uint8_t R_UART1_SendStart(uint8_t txlength, uint8_t *txbuf)
{
    uint8_t ret;

    /* --- Check parameter ( txlength ) ---- */
    if (txlength > 0U)
    {
        ret = NORMAL_END;

        do
        {
            /* ---- Wait for data set ---- */
            while ((SSR02 & 0x20U) == 0x20U)
            {
                NOP();
            }
            TXD1 = *txbuf;                                                  /* Send data */
            txbuf++;                                                        /* Increase tx buffer pointer */
            txlength--;                                                     /* Decrease txlength */
        }
        while (txlength != 0U);
    }
    else
    {
        ret = ERROR;
    }
    return ret;
}
/* End user code. Do not edit comment generated here */
