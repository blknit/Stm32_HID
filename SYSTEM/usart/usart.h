#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 


//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��


#define UART1RECVSIZE 512

extern uint8_t uartRecvBuf[UART1RECVSIZE];
extern int uartRecvLen;

void USART1_Init(u32 bound);
void USART2_Init(u32 bound);

void USART1_Putc(char ch);
void USART1_Send(char *data);
void USART2_Putc(char ch);
void USART2_Send(char *data);


#endif


