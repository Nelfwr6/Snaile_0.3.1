/*******************************************************************************

* Module name: MQTT.h

*

* Copyright 2021 Neona Embedded Labz as an unpublished work.

* All Rights Reserved.

*

* The information contained herein is confidential

* property of Neona Embedded Labz. The use, copying, transfer

* or disclosure of such information is prohibited except by

* express written agreement with Company.

*

* History:-

*

* First written on 12/12/2021 by Vishnu Soman.

*

* Module Description:

* Header file for MQTT commands

*

*******************************************************************************/
#define Registered 1
#define Not_Registered 0					

#define PROVISIONED 1
#define NOT_PROVISIONED 0

#define MQTT_SUCCUSS	1
#define MQTT_FAIL	0
/***************************************** EXTERNAL VRIABLES***************************************************/

extern  uint8_t Subscribed_data[400];

extern  uint8_t Certificate_buffer[4000];

extern uint8_t New_PEM_Cert_Buffer[2000];
extern uint8_t New_Pvt_Cert_Buffer[2000];
extern uint8_t certificateOwnershipToken[470];
extern uint8_t PEM_CERT,PVT_CERT;

extern volatile uint8_t MQT_CMD_RES[25];
extern uint8_t MQTT_RECV_FLAG;
extern uint8_t OTA_HTTP_URL[1500];
extern uint8_t MQTT_PUB_SUCC_FLAG,MQTT_OPEN_FLAG,MQTT_STAT_FAIL,MQT_PDP_RECONNECT_FLAG,MQTT_PUB_FAIL_FLAG,\
MQTT_PDP_FAIL_FLAG,MQT_RECONNECT_FLAG,Reconnect_flag,MQTT_SUB_SUCC_FLAG,MQTT_CONN_SUCCESS_FLAG;
extern uint8_t u8_temp[25];
extern uint8_t first_time_provisioning;
/******************************************* FUNCTIONS *********************************************************/

uint8_t u8_Wait_For_mqtt_Connection(void);
uint8_t Publish_Handshake(void);
uint8_t Subscribe_to_AWS_Topics(void);
uint8_t Registration_request(void);
uint8_t AWS_Thing_Provisioning(void);
uint8_t MQTT_Reception_Handler(uint8_t * Subscribed_data);
uint8_t u8_MQTT_PDP_Activate(void);
uint8_t OTA_Acknowledgement(void);
uint8_t Config_Acknowledgement(void);
uint8_t Mqtt_SSL_Config(void);
uint8_t Mqtt_SSL_Update_to_RAM(void);
uint8_t mqtt_checking (void);
void Provisioning_And_Registration(uint8_t * Certificate_Buffer);
void Subscribe_to_Rails_Topic(void);
uint8_t Rails_registration(void);
