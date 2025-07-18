#ifndef __USART_H
#define __USART_H

extern void USART3_SendByte(u16 dat);
extern void USART3Write(u8* data,u16 len);

void USART3_Configuration(void);

#define DIR485_H GPIOC->BSRR=1<<1
#define DIR485_L GPIOC->BRR=1<<1

#endif
