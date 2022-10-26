/*******************************************************************************

* Module name: ssl_certificates.c

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

* First written on 08/22/2014 by jomon

*

* Module Description:

* Uart program

*
***************************************************************************************/
/*************************** HEADER FILES *********************************************/

#include "r_cg_macrodriver.h"
#include "r_cg_serial.h"


/****************************************COMODO_PEM************************************/
const uint8_t u8_comodo_buff_1[510] ="-----BEGIN CERTIFICATE-----\n\
MIIEkjCCA3qgAwIBAgIQCgFBQgAAAVOFc2oLheynCDANBgkqhkiG9w0BAQsFADA/\n\
MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n\
DkRTVCBSb290IENBIFgzMB4XDTE2MDMxNzE2NDA0NloXDTIxMDMxNzE2NDA0Nlow\n\
SjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxIzAhBgNVBAMT\n\
GkxldCdzIEVuY3J5cHQgQXV0aG9yaXR5IFgzMIIBIjANBgkqhkiG9w0BAQEFAAOC\n\
AQ8AMIIBCgKCAQEAnNMM8FrlLke3cl03g7NoYzDq1zUmGSXhvb418XCSL7e4S0EF\n";

const uint8_t u8_comodo_buff_2[510] ="q6meNQhY7LEqxGiHC6PjdeTm86dicbp5gWAf15Gan/PQeGdxyGkOlZHP/uaZ6WA8\n\
SMx+yk13EiSdRxta67nsHjcAHJyse6cF6s5K671B5TaYucv9bTyWaN8jKkKQDIZ0\n\
Z8h/pZq4UmEUEz9l6YKHy9v6Dlb2honzhT+Xhq+w3Brvaw2VFn3EK6BlspkENnWA\n\
a6xK8xuQSXgvopZPKiAlKQTGdMDQMc2PMTiVFrqoM7hD8bEfwzB/onkxEz0tNvjj\n\
/PIzark5McWvxI0NHWQWM6r6hCm21AvA2H3DkwIDAQABo4IBfTCCAXkwEgYDVR0T\n\
AQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8EBAMCAYYwfwYIKwYBBQUHAQEEczBxMDIG\n\
CCsGAQUFBzABhiZodHRwOi8vaXNyZy50cnVzdGlkLm9jc3AuaWRlbnRydXN0LmNv\n";

const uint8_t u8_comodo_buff_3[510] ="bTA7BggrBgEFBQcwAoYvaHR0cDovL2FwcHMuaWRlbnRydXN0LmNvbS9yb290cy9k\n\
c3Ryb290Y2F4My5wN2MwHwYDVR0jBBgwFoAUxKexpHsscfrb4UuQdf/EFWCFiRAw\n\
VAYDVR0gBE0wSzAIBgZngQwBAgEwPwYLKwYBBAGC3xMBAQEwMDAuBggrBgEFBQcC\n\
ARYiaHR0cDovL2Nwcy5yb290LXgxLmxldHNlbmNyeXB0Lm9yZzA8BgNVHR8ENTAz\n\
MDGgL6AthitodHRwOi8vY3JsLmlkZW50cnVzdC5jb20vRFNUUk9PVENBWDNDUkwu\n\
Y3JsMB0GA1UdDgQWBBSoSmpjBH3duubRObemRWXv86jsoTANBgkqhkiG9w0BAQsF\n\
AAOCAQEA3TPXEfNjWDjdGBX7CVW+dla5cEilaUcne8IkCJLxWh9KEik3JHRRHGJo\n";

const uint8_t u8_comodo_buff_4[510] ="uM2VcGfl96S8TihRzZvoroed6ti6WqEBmtzw3Wodatg+VyOeph4EYpr/1wXKtx8/\n\
wApIvJSwtmVi4MFU5aMqrSDE6ea73Mj2tcMyo5jMd6jmeWUHK8so/joWUoHOUgwu\n\
X4Po1QYz+3dszkDqMp4fklxBwXRsW10KXzPMTZ+sOPAveyxindmjkW8lGy+QsRlG\n\
PfZ+G6Z6h7mjem0Y+iWlkYcV4PIWL1iwBi8saCbGS5jN2p8M+X+Q7UNKEkROb3N6\n\
KOqkqm57TH2H3eDJAkSnh6/DNFu0Qg==\n\
-----END CERTIFICATE-----\n\
-----BEGIN CERTIFICATE-----\n";

const uint8_t u8_comodo_buff_5[510] ="MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n\
MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n\
DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n\
PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n\
Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n\
AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n\
rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n";

const uint8_t u8_comodo_buff_6[510] ="OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n\
xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n\
7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n\
aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n\
HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n\
SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n\
ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n";

const uint8_t u8_comodo_buff_7[510] ="AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n\
R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n\
JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n\
Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n\
-----END CERTIFICATE-----\n";



/****************************************SNAILECL_PEM************************************/
const uint8_t u8_snailecl_buff_1[510]="-----BEGIN CERTIFICATE-----\n\
MIIFdzCCA18CEBPkQLu0iYxWtgCMqbFSMWwwDQYJKoZIhvcNAQELBQAwgaExCzAJ\n\
BgNVBAYTAkNBMRAwDgYDVQQIDAdPbnRhcmlvMRMwEQYDVQQHDApIdW50c3ZpbGxl\n\
MRQwEgYDVQQKDAtTbmFpbGUgSW5jLjEVMBMGA1UECwwMU25haWxlIENsb3VkMRgw\n\
FgYDVQQDDA9zbmFpbGVjbG91ZC5jb20xJDAiBgkqhkiG9w0BCQEWFWFkbWluQHNu\n\
YWlsZWNsb3VkLmNvbTAeFw0xNzA0MTkwNTMzMzNaFw0zNTA0MTUwNTMzMzNaMFIx\n\
CzAJBgNVBAYTAlVTMRMwEQYDVQQIDApDYWxpZm9ybmlhMRYwFAYDVQQHDA1TYW4g\n\
RnJhbmNpc2NvMRYwFAYDVQQDDA1TbmFpbGUgRGV2aWNlMIICIjANBgkqhkiG9w0B\n";

const uint8_t u8_snailecl_buff_2[510]="AQEFAAOCAg8AMIICCgKCAgEAp0WGZXALKObfnzLNA4YD2g/BumARmgEyP1lmid/B\n\
7LuaXQFK9d50vfLENqx/iWPg89eZrQTQyRr49JLwKixzj+8ADcf2Ol66jKpiIysz\n\
mlf7BMf6DgrKkvQGvDIiEsqcTfuhl0/Sf3yzwy9tdfGMSphzG5WwoVuiSOQELwYY\n\
nI5c1cEJztGgFCS86/4RloPXtxONJoymyj+pcupkYIG9fJGA3+2BzNlU8YY9u8TA\n\
cC5kqFBzPQvwOwH5yAz+n6MhqAWBZ0kNN82kW7oKpXjsnwTghyRHSM/+KpQJlbLd\n\
zrWys6IvJSKNDmMdd3Q+Bi8SmLLL5B59vju4N6NT/SJjZyjqOxyRVk6YqzrZ5d2V\n\
Tc+DUsOCCbguAu7ht0ltBezQgRfUGGxD9GUmvAMBRrHkh8BTbA1MgVgbvubwzQQg\n";

const uint8_t u8_snailecl_buff_3[510]="GIFpCIDWZPs1TFEje48S0f/wDq9W/G81FeeECYKcwN4wkteNb/qEa4PHL6pXEapp\n\
HE0mmlrP3F+ks5QigkFWPavjRJTB/tmVuxZqe8qYDGEU25AB5XFhFYypUooeupuF\n\
Bdf00K+S6pD1pT2oSA+glhDvgdoSfOaipjYvgak5/mfdZn2CmZSQHz/aUPeLcQsC\n\
CUO79UYHmJEPCRqr9xAnrajssq9/mdofz5NoJV97tTPwSvIxPhIn7LB4qk2hV1dC\n\
oMMCAwEAATANBgkqhkiG9w0BAQsFAAOCAgEAWn7D0hrrMP7lxE++1Rb5gajrcZ9R\n\
5B0m19EKrQizG6GdXYDiVI8n5makS1OFBeojx2BihanZ1B7/XubZ6mhEnhbT8hc+\n\
X1t2HhNIBa11ZhY5jcPm4PTeYIVzOSSKsY/Srjgifumv8LA8Chzye8jl7GtMS7PY\n";

const uint8_t u8_snailecl_buff_4[510]="lu8bXKEt1+31rRWmDqUO6zVZNTk1S4wBg9mCnGD2Loir76GhZzJZD6qhtmz/fBpY\n\
B6ZBX/KXxdKuy58OZ8G48g8wOJ0aCHovaOj7t5XhXdHX7fdVPR3ODmiIckvzfepV\n\
c8mjjKVFysxS8qyewzbc4jY5iilmu0He+WwkJdSE1PY2nER0/WWwKd6VhQeJ+oIC\n\
T3/6PzdUmnRGbY4lovuP4UVrztWYPtVr5d5jsWtouUml1oyYXTfI+hSKfxjK4Vld\n\
e14UPU5650gJqZrTMG/foSri3roHzaxhQpPibHIpJfX9FjmQP00wwLuSaYhuxUHD\n\
y9nZdM4ulWbJFpHRc5f04c0Qb+bfM1TMb+709rwCCxUSkc+DhdpKPyutdQrvUyHi\n\
xSmoHOhIXdLcEMQPAM04xX+jIdADgz7rot41uwG3RpIqPjvW6T7WORYq1sUK06TI\n";

const uint8_t u8_snailecl_buff_5[510]="JXuSEX8Z8FT+CsOiQDbw5lVpxXK6M7MjLSTzfAyGIca5/aCz6lG5dy7h+bOYBe4U\n\
c5RBnuQih6sS7cY=\n\
-----END CERTIFICATE-----\n";


/****************************************SNAILECL_KEY************************************/
const uint8_t u8_snaile_key_buff_1[500] = "-----BEGIN RSA PRIVATE KEY-----\n\
MIIJKgIBAAKCAgEAp0WGZXALKObfnzLNA4YD2g/BumARmgEyP1lmid/B7LuaXQFK\n\
9d50vfLENqx/iWPg89eZrQTQyRr49JLwKixzj+8ADcf2Ol66jKpiIyszmlf7BMf6\n\
DgrKkvQGvDIiEsqcTfuhl0/Sf3yzwy9tdfGMSphzG5WwoVuiSOQELwYYnI5c1cEJ\n\
ztGgFCS86/4RloPXtxONJoymyj+pcupkYIG9fJGA3+2BzNlU8YY9u8TAcC5kqFBz\n\
PQvwOwH5yAz+n6MhqAWBZ0kNN82kW7oKpXjsnwTghyRHSM/+KpQJlbLdzrWys6Iv\n\
JSKNDmMdd3Q+Bi8SmLLL5B59vju4N6NT/SJjZyjqOxyRVk6YqzrZ5d2VTc+DUsOC\n";

const uint8_t u8_snaile_key_buff_2[500] ="CbguAu7ht0ltBezQgRfUGGxD9GUmvAMBRrHkh8BTbA1MgVgbvubwzQQgGIFpCIDW\n\
ZPs1TFEje48S0f/wDq9W/G81FeeECYKcwN4wkteNb/qEa4PHL6pXEappHE0mmlrP\n\
3F+ks5QigkFWPavjRJTB/tmVuxZqe8qYDGEU25AB5XFhFYypUooeupuFBdf00K+S\n\
6pD1pT2oSA+glhDvgdoSfOaipjYvgak5/mfdZn2CmZSQHz/aUPeLcQsCCUO79UYH\n\
mJEPCRqr9xAnrajssq9/mdofz5NoJV97tTPwSvIxPhIn7LB4qk2hV1dCoMMCAwEA\n\
AQKCAgEAiCf9p+A2FUsox8twOegChqPAy9abPUKwBgTdhGkQz4Q9uGru+4fieyN7\n\
OA3dmpYkbBgZTU7YLHq6H0rkF1ArmQOwhYc5FHY7X3jpfTsTPBVVjKTDFF8QWeIt\n";

const uint8_t u8_snaile_key_buff_3[500] ="AU/FkIpABPJIrIQbEg8NvFmEvjaqPaYuPEvyhMW0KaUBCs3phrYTNyN0ulbD+OGW\n\
rnoXEu+qzakqgjCHK6X6ZkIJBs1U0msV/jC0MWzOyfK9mXQkOKxq3t7tjsmC+GU5\n\
Tu/U61i8cqpYlRxPmBbaLJgZRmxSH3a21QpgxBIaD8qEqsf36OTZ24Ku4YloTEe9\n\
mpiSTDJQ7t7LPJrdO9Ycs51NJoWxZtu5tKtIYAPlA4ETxELEH2PAda5paZ8FUhoo\n\
xvUVFktmsAoBpXcB8PPbjGWGXouP+Am9wk+ztYefvTqQeRhS3hwAM1hG3Xu0buTX\n\
aZjqgLe+wV/sYLs+gujxQt1UmuztHjiTJsl6On5eb5D3WNc6HDrKwHBwwjnORUPS\n\
gyTfcahPjfQ9nvWHN8o2JkhJr+GgsVBNhiupJT4jRu7PDq2PNzxeUEOCfdcl7yOF\n";

const uint8_t u8_snaile_key_buff_4[500] ="x+OXtCOu6ufC+rc/5vdytRgvvvmF5TmHXP8NVTGAOdtlI0IscTDIKP/xpsJAZkLZ\n\
UBQ9kDRMZE1m2i660Hiugk+P2Uv52y4FyBZEzz8Q6GVEOtdT1XECggEBANzsP0po\n\
h2SFgq89GwfJMeT06LMWpQWAHjVF4x2/ujkCGqdQY7n9OFBJ4+00Y5Xetzg4MB0D\n\
Z1xLradqqEMlBtEABP9oD0rVzE6xTXi3cHZuF/88M6IsorRI5w/uQQd/lRh6tRm2\n\
tvaQPWZay5RZovHcR4keezwB6aM2kiXFo9gG14/HkaydW1D85kL7qeJz0FBiSrr0\n\
qkEwWNTf36YXEPxQGTdErvyfCqzfHqgCKq/IMKM5t7+QId0GjGgKrx87aO+AWQtS\n\
ZIgPIciMmULiG7Nz/ul1jKfll1e7OCa4Rgknoz8JukLghdWOWkeNd3H4O4qLbgv2\n";

const uint8_t u8_snaile_key_buff_5[500] ="pbVZVopUA+umQ5UCggEBAMHUifBx3pYNLBsNzI9n4RodHa5pdFcVbaOWWkynjLcJ\n\
NURd0Qt/46nYfr6SlKX/QGxcP7QZIE1ZUf4vJFFvq3M0PS0Oce6sGLjtDrrhbyLL\n\
nuna9NZJ8bDsAwqYzqq+QmQIPiSlG2hTfYMdZ/G1lZujacpTYE0umnEId6FkhBl6\n\
ImN4Ro77NW3FuimBYD4ScjLQ6kgXZe7hf8JJ8+Z1JC+z0gmLVb6blMpUTX7FPlcg\n\
5L9E8Zk79+/OsFkqXH5iq7LSpRbkblXVIXEDb+PEgM5S6hXrJfz+rKqS54We33sp\n\
RGj85Wi2cpUTLtUolOfkxdbD99XVuhi7CJoxJ/HevPcCggEBALyB9atXMBgs+Arx\n\
WAw3uBRGifyuWNctsBkC7j2nKI6WZgnYRZAhKpkPw6XWqxbpuGsHPr6y9dJCXQXa\n";

const uint8_t u8_snaile_key_buff_6[500] ="s4K/qGZT3u/UaiAP+Eo5bgKQfC0B2gYW53P9JEQzZL4qLWDpk14U7kznpI0VHN8m\n\
oEmQhsJblMrCsKLOev6ihOgoEZs2k18DCiFBD/c2JICJXzd0XQ8KH/wQ4unHCDV0\n\
ZdDCI+M3sZyARdQXHTPPGsh/OM9PQXk1ub2CjfKcMNhiLXW36t1aJiDMCC40wz3u\n\
AqSYbMyeMKgifoeRF6drgAlB+bi8EsaqVT3VeT11P0GIy5/7EUsxmBArQNBST86u\n\
y72a3FUCggEBAJjY4W2aXcw11vH+j7EGWZweGanPvs5tN2goyExqU6yCtl6THKXU\n\
09K7xjbBoDbGcicBF7sNlUtphB7NTfZp+Ol/6D3/hhiP3W72gQcdfmYR6969qxTQ\n\
eqbpdfyFQ3OhVPlNBrQe/t3l+LPMHIuehbuVwXarCOXUOjWoV64NdZ4hkbyevAVE\n";

const uint8_t u8_snaile_key_buff_7[500] ="U32czpCsVRidszAfTqBK0zbs27J6Xrr85Ow5AQhFt/f29/4VPIcbQ22xorCxzVLo\n\
1ztj4exuW0HOzVbWhRjoSZml2BkmfLU/39LTz4Q5SluW+NIJxki4X8LfZ/LieoV4\n\
WiL0XZn5xnKXFOum3WjigT2/P6lX2ggBfUECggEAChuCBjXViF40ueUMpaD+aL/X\n\
JKXRs6bBu4KoPDQ0nXRi2CXUaHtsSPiL1uHSktjwGk6hWF+DjM+dW1IufX0dJUrk\n\
1JVR08sC7rx9sCkuV/Uy+zohf2kENkxrHRDkspPLW/j0VOmDp4OZ7MQVPwlgMwdt\n\
kVc+NgdH0qDHH7jYGrhoQOJidJdC0jaPgHf19LJaXQs8ehzuc8dinv0uSJov5OE2\n\
Z6157NrzwaM+06L4yC0LLKSehAVkoV955wZVa1BOpWPXVzFt282HILe2a5copaI1\n";

const uint8_t u8_snaile_key_buff_8[200] ="7SjZIWU+u337JgQoAMJf1bJ+uaSnGV3gXSaaTcSXipp6uwD9UeAetkcwM6Pnrg==\n\
-----END RSA PRIVATE KEY-----\n";

