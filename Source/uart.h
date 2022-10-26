#ifndef _UART_H_
#define _UART_H_
extern uint8_t u8_uart0_Tx_Flag, u8_uart1_Tx_Flag, u8_uart2_Tx_Flag;
void v_cmd_send(uint8_t u8_send_data,uint8_t u8_channel);
void v_uart_str_send(const char *string,uint8_t u8_channel);
void v_byte_array_send(uint8_t *u8_data, uint16_t u16_no_of_bytes,uint8_t u8_channel);
#endif