/*******************************************************************************

* Module name: mqtt_ssl.h

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

* First written on 12/06/2022 by Vishnu Soman

*

* Module Description:

* Header file for ADC Application program

*

*******************************************************************************/
/**************************************************************************************/



#define CA0_TOTAL_LENGTH  1188// same for provisional and rails
#define CC0_TOTAL_LENGTH  1224//1224 for provisional//1220 for rails
#define CK0_TOTAL_LENGTH  1679//1675 for provisional//1614 for rails


extern const uint8_t u8_CA1[510];
extern const uint8_t u8_CA2[510];
extern const uint8_t u8_CA3[510];

extern const uint8_t u8_snaile_CRT_buff_1[510];
extern const uint8_t u8_snaile_CRT_buff_2[510];
extern const uint8_t u8_snaile_CRT_buff_3[510];

extern const uint8_t u8_snaile_private_key_buff_1[500];
extern const uint8_t u8_snaile_private_key_buff_2[500];
extern const uint8_t u8_snaile_private_key_buff_3[500];
extern const uint8_t u8_snaile_private_key_buff_4[500];

