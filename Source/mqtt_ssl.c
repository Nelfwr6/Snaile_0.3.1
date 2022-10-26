/*******************************************************************************

* Module name: mqtt_ssl.c

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

* mqtt_ssl

*
***************************************************************************************/
/*************************** HEADER FILES *********************************************/

#include "r_cg_macrodriver.h"
#include "r_cg_serial.h"
#include "mqtt_ssl.h"

///////////////////////////////////////////Provisioning certificates///////////////////////////////////////////

/****************************************CA certificate************************************/

const uint8_t u8_CA1[510] ="-----BEGIN CERTIFICATE-----\n\
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n\
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n\
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n\
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n\
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n\
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n";

const uint8_t u8_CA2[510] ="9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n\
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n\
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n\
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n\
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n\
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n\
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n";

const uint8_t u8_CA3[510] ="U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n\
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n\
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n\
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n\
rqXRfboQnoZsG4q5WTP468SQvvG5\n\
-----END CERTIFICATE-----\n";


/*************************************************************************************************/


/****************************************PEM certificate start************************************/


const uint8_t u8_snaile_CRT_buff_1[510]="-----BEGIN CERTIFICATE-----\n\
MIIDWTCCAkGgAwIBAgIUajVifPGkv8XT0ith9hFLDF3ejeMwDQYJKoZIhvcNAQEL\n\
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n\
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIyMDgxMTAwMzEy\n\
MloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n\
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOAZGJ3x5KRhsW470v9L\n\
zu7gZvVgk9KzqymOoFamcXKNvSVSGU+sahkkv6xLx72wdC0MAHX6+ng0Cg8T9e2w\n\
c+SZilFhqe0/6g9ZR4LF/hQFIL7VWi+88twAaHA4YTMznjQKLVD2hkLQgyJUXHu5\n";

const uint8_t u8_snaile_CRT_buff_2[510]="4/oqJPVEblDOD5LLKGUpOxAqwbi45t2xMcpe7z0FD3dP7XRgv0kDSe+BW1u8OHq0\n\
vdKy7aq7jk/iqe2VWQ9WkRoLQu/gUJDlIiShUmBx+gBsDmmowzJWhkF0Qo0+VBpB\n\
U9QZPcFJrpeWNM4WYB1eebuwgmjDAth6M0Ra1i3OxsXT4ygqjz47rI2ijvBiUJfH\n\
sg0CAwEAAaNgMF4wHwYDVR0jBBgwFoAUwCYZH3EFa7vitYEPkUiunILeeH0wHQYD\n\
VR0OBBYEFPKI6Pi8x0glBAtoPGvOk3q1Wx7lMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n\
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQB1Mg5k8znIQk2AjX8LWXFrkpsA\n\
RhU+G1qLdvwEO+4tzxAfXt2ieV1vW83TriFTlWl8njElpCqGTO/WiBLvGtpD4Rr/\n";

const uint8_t u8_snaile_CRT_buff_3[510]="L1e6RJSGAFdyoFAlFx65HUhMVZ55M2QhWcGpoWmgX7q1YkZB7Es5tmjqAoopWNta\n\
gh30rORjR09x/37r2D+tR69q1PdLYy5rC28CfJgV7XpQjq408rvVOsdwzCZZiUwQ\n\
gIlzLOW8poJlZu3cqmnFy2ydC4OuoqGzbT7RA/bgCzpFkhUzXxXeVksEqZhoIBip\n\
jPOn5QzQiS8H5wnHKAkUkBmG17+WUvMuuMM55hHs+2Uj2eoHChm/kVeO632Z\n\
-----END CERTIFICATE-----\n";


/***************************************PEM certificate end***********************************/

/*******************************************Private key start ********************************/


const uint8_t u8_snaile_private_key_buff_1[500] ="-----BEGIN RSA PRIVATE KEY-----\n\
MIIEpAIBAAKCAQEA4BkYnfHkpGGxbjvS/0vO7uBm9WCT0rOrKY6gVqZxco29JVIZ\n\
T6xqGSS/rEvHvbB0LQwAdfr6eDQKDxP17bBz5JmKUWGp7T/qD1lHgsX+FAUgvtVa\n\
L7zy3ABocDhhMzOeNAotUPaGQtCDIlRce7nj+iok9URuUM4PkssoZSk7ECrBuLjm\n\
3bExyl7vPQUPd0/tdGC/SQNJ74FbW7w4erS90rLtqruOT+Kp7ZVZD1aRGgtC7+BQ\n\
kOUiJKFSYHH6AGwOaajDMlaGQXRCjT5UGkFT1Bk9wUmul5Y0zhZgHV55u7CCaMMC\n\
2HozRFrWLc7GxdPjKCqPPjusjaKO8GJQl8eyDQIDAQABAoIBAQDZoOWcNZai4NDr\n";

const uint8_t u8_snaile_private_key_buff_2[500] ="yaNOFLPgngK+10yXea6GjCFAJSHVarq+bK1yB3F9+4c87tcKaztXp+kmqlbltzvx\n\
iqPxccQKw+VkxSC9660FvOFKqbbmgFhdm1qbM7byWl8SZF5Y2A8SYLPDhbW7Wz2O\n\
cDjkWxGAF7+OhSdQx9yt5SfUnibiuaG1pv/EZhvId+tChltS4pffegTpFvFx1kuf\n\
MjHCYemkPlOgBJB12iRteczYM9VYmdmWgcvfpAlE831wPtG/90cg5Dkm1gCR8dtP\n\
sFULaiTF8nwJi62UhfsmuWgkX3wRulBCjp72EEgebADxO1zniEbkvUKkpiHCzBeN\n\
pTeLW5CtAoGBAPdXougyjalfdciiqrRqt7LscSkwwp114xuX6K2wC0v0uMYgid36\n\
S8KtSybMsnfHLzd5ED3YQ9+PcgvCdHxDH6pQW937U5o1eCcY9Hdf1r6WfpGrPPAw\n";

const uint8_t u8_snaile_private_key_buff_3[500] ="DuMVI5w7O4wX7gtAwpItxgQmf0SEFL2jgyB86Xc4YsKKNSep/palVuNTAoGBAOfx\n\
LKBslZW0e+n5DUCpb9yX+jAEHhZ2v012hKxAPUjlLFdjSh9XJMf7tFkNLYC9VE+m\n\
Hh3TNresGtIhz/d3ryRz/4B86RfZmX/PCIZdvM1iOe6BpdMKxZcGJ4GZvo/RhBJH\n\
/uOxsZKlWUflmgJt5NsvVIxYLEE3pk03KHHcYskfAoGBAIvwekvzJjEYwFQGrJws\n\
8Mf7fD64xrM92oNnHA8A1jNgPxZhnH8JuMlpMGbAU4zvFejFxwmmjSg0SWsb1rnj\n\
ZEwA8SkTpAZQkC1WsiHECISGMxAJmQ2vswyJjj87SAKIG3ckabE40prQiRrju7Rb\n\
ibnc0l5QeZfliBNr242x+AnfAoGAJM6A5nRUyIz7nQWItNouhjMX4MbGf4oYkZJd\n";

const uint8_t u8_snaile_private_key_buff_4[500] ="O3h2QTsc7kEzYgXSMGohFICi1+szlfAml1UNOdh16SK0ffDgOkiuIxWvZ7mAQmf4\n\
UlpNu8n6p36rcfIjqPOxET/0nZ4B92S1biykoUDcCizFY91Mk/vOzvavDyplmZtH\n\
L8ppYsECgYAScClZYaTbR6y3b5ey0wWL5lDd1mBj/ZNiYQ9O/nElBcGV0pgqvtPC\n\
UiXztJi7unFI7v6r83gTfyeumS4MAeaK0m+QvBKY/SuJZidBUyQunzS4wg30xgwf\n\
oUr2HvBwUFPvztBYjH3NrtUX08tVYwxhLLF/jPedOe7EG0P6j4lDuA==\n\
-----END RSA PRIVATE KEY-----\n";


/******************************************Private key end************************************/

///////////////////////////////////////////Provisioning certificates end///////////////////////////////////////////


////////////////////////////////////////Rails certificates start/////////////////////////////////////////////////////

/****************************************CA certificate************************************/
/*
const uint8_t u8_CA1[510] ="-----BEGIN CERTIFICATE-----\n\
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n\
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n\
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n\
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n\
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n\
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n";

const uint8_t u8_CA2[510] ="9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n\
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n\
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n\
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n\
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n\
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n\
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n";

const uint8_t u8_CA3[510] ="U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n\
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n\
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n\
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n\
rqXRfboQnoZsG4q5WTP468SQvvG5\n\
-----END CERTIFICATE-----\n";
*/

/*************************************************************************************************/


/****************************************PEM certificate start************************************/
/*

const uint8_t u8_snaile_CRT_buff_1[510]="-----BEGIN CERTIFICATE-----\n\
MIIDWTCCAkGgAwIBAgIUOcj6xrk2exwGq4JxZMl9uDRCpWEwDQYJKoZIhvcNAQEL\n\
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n\
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIyMDQyODAzMjAx\n\
MloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n\
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMOlW+Keh3W+WyzVz4yl\n\
RZbNMz5t/H/ac7aOPCwsfbannRqm/ul2T/1GTKpHWCHfCeey/+d3KGcXcboWjsUD\n\
PiA90PK00Hb6mNTiBYg1VhLoiem6lvmMli48QsQ/sQCDZIGl0FsnaRl3VSCUCqiO\n";

const uint8_t u8_snaile_CRT_buff_2[510]="v+pIOlA1xtnS09V1VtbseGGiItIwBz+SwNvxu9d9BZc40q72YLPYiIyqLmstBLBQ\n\
0rS0ktkViJ8A/tc1Uxm9hyE63U+0XrCFIWM4u/fy74upj+4WHdpYAK9gbfvF4YNy\n\
joI0E83kJwzHuKGegpfGSrGaK0420BsZwCAsQRnnLp6/0fNWMJsKr4F7bNow2jlc\n\
DUECAwEAAaNgMF4wHwYDVR0jBBgwFoAU/zVhoqK3gweHNrpovOHipm7BcjkwHQYD\n\
VR0OBBYEFP2aCeuKxzx7Ke+H3k/51xAL0idTMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n\
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAMxRQOzEFViBTICo9UH8zXT1cB\n\
Fc3IqGuxqNuFbV4jNk631PgaZd5sy56AuR7gCOloDGoMnJ8eSUMOnGe9QtKROlr9\n";

const uint8_t u8_snaile_CRT_buff_3[510]="DraFr1TVfRaafJIxfFAfBRh9u0FBjludkWQgvdm1+3NA9XUGmFjYq4GqCKW1AL+l\n\
EANjudIodwHQ09hHnZAaV15Z2YeRd7JhG2UJJSJgqeYZaA7v9PZYsBSXrxpiQtyx\n\
sLRqSJZp797kPI55Dv+APGGOXvQGDlNzTHkLNkanqJiFeY5LmZG/EU78Z04poxVj\n\
HrasZMLBm6DMPKLs78wGSq9cfaMZ0bHM/A45C/T194jHO5bT98UICyDICB19\n\
-----END CERTIFICATE-----\n";
*/
/***************************************PEM certificate end***********************************/

/*******************************************Private key start ********************************/
/*

const uint8_t u8_snaile_private_key_buff_1[500] ="-----BEGIN RSA PRIVATE KEY-----\n\
MIIEpAIBAAKCAQEAw6Vb4p6Hdb5bLNXPjKVFls0zPm38f9pzto48LCx9tqedGqb+\n\
+YyWLjxCxD+xAINkgaXQWydpGXdVIJQKqI6/6kg6UDXG2dLT1XVW1ux4YaIi0jAH\n\
P5LA2/G7130FlzjSrvZgs9iIjKouay0EsFDStLSS2RWInwD+1zVTGb2HITrdT7Re\n\
sIUhYzi79/Lvi6mP7hYd2lgAr2Bt+8Xhg3KOgjQTzeQnDMe4oZ6Cl8ZKsZorTjbQ\n\
GxnAICxBGecunr/R81YwmwqvgXts2jDaOVwNQQIDAQABAoIBAFPBPozNgQAO9clD\n\
3w4+0oO0ZPOc8jNpuY+1ZF0kzsybBM6cHxSi6atrpPZghII7kWn3X0Yf5NP9XVpk\n";

const uint8_t u8_snaile_private_key_buff_2[500] ="vcnpkXcL/59neLb2oObBu8yLyMVfd2rvziIW5VP1yD9rZMLrfONjsMbVIFEenPrN\n\
ZaKzOcV6FjywSOiHMNh42yw3H3Hrz46kPvLqiI1tsi19EmBeXpvU/nPqsVjFIjXY\n\
6bwvh1YYp0jCQ0slguFvUTNN+UM4UZEqAapjaY0D5H+ti7CIZi95CzomTpkLotmG\n\
YPuzbOllXSqZV8cFchUn4kTvIdBYt8rAGR1lm6vit/EbFct76I9gFyNYajiLV06u\n\
/98swIECgYEA4DX6iSJ9sVvXoJXlF5ajNlFQgDNjOVf2C6k8e1ay/JdsmO73d5hZ\n\
/4cqT1RsEwvrhL9JBugPxjcffmXH+k+rjCs7fTmPjzmeuVp9L83iFLXicxYvjb1z\n\
RnY3M7x/vIuKyeN7zenTluVyoK6yHMAClIrQ9BqvfTzfzsl4PJ3n9D0CgYEA32KU\n";

const uint8_t u8_snaile_private_key_buff_3[500] ="aRkgfUm8PHDUTKErs6G4ncY7mAZwAKQOLVPWmIbVr+IALH8W/hTSkGlsM526BhBs\n\
VstCS566HJ3m0jagTGTqAVHb/+H9mbhW+oqHayed/uJGt4b6k6bAE9TEHw1dMaUz\n\
tcmhUwr/9e+rQwKQrMWDNLt83sA2vLSdU/hWGVUCgYBH7QkSNR5dZzklKvDzQD/b\n\
jPpmJhYOr0kfNItShd2lam5MIV0B4L3+Wz4XdN22vnwvea3TqV5mGAjiY4RjqGs/\n\
7B6JbY3ARJvGmmDOlR2Rx0d95AAdWElCsWnEDxRrR9ieK3vr8rcOEU1UfrZkwrQR\n\
MvcVV/o0EvM5QyhvSbi/gQKBgQCJ9B5FPYBD/3phNSjW3h3YBIK35Qk+nJMkwiwD\n\
MChx76sdwIts71uqs/9JseFhiwGr5w6RF1usyayUKau3nIaoRxYAlKYxw0XTXfVR\n";

const uint8_t u8_snaile_private_key_buff_4[500] ="KqXwtfmrw9/6TUCIWM18McSgjKdGFoj1tjyLxVpHbqisbHQeTqf1s+esxLXJGh2j\n\
EtVRNQKBgQCuRSMDVFOLZw8QeQCmzJQGqFBlBSHbRdj55Z4S3Pre9XXN6v71i+vO\n\
4BWS+neKYOgU5YxPCdRuXWxtPJeoLgUwHKRwB1GexQ4XmCNvgE/K8ncIA9AZ36V6\n\
TBH31Ro+SRy5+1odh2U1JW9fL2NTQchZ11gVf9XdncA6SLfc8urLSA==\n\
-----END RSA PRIVATE KEY-----\n";

*/
/******************************************Private key end************************************/

////////////////////////////////////////Rails certificates/////////////////////////////////////////////////////










