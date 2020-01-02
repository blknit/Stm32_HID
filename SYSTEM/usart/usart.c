#include "sys.h"
#include "usart.h"

uint8_t uartRecvBuf[UART1RECVSIZE];
int uartRecvLen=0;

////�ض���fputc���� ,����� USART2
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;   
	return ch;
}
//#endif 

 
 
#if EN_USART1_RX   //���ʹ���˽���


#endif	

//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	

void USART1_Init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��

	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9

	//USART1_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 
}



void USART2_Init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	//USART2_TX   GPIOA PIN2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PIN 2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART2_RX	  GPIOA PIN3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PIN 3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������2�����ж�
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2 
}


void USART2_Send(char *data)                	//����2����
{
	while(*data!=0)
		USART2_Putc(*(data++));
}
void USART2_Putc(char ch)              	//����2�����ַ�
{
	while((USART2->SR&0X40)==0);//�ȴ��������   
	USART2->DR = (u8) ch; 
}
void USART1_Send(char *data)                	//����1����
{
	while(*data!=0)USART1_Putc(*(data++));
}
void USART1_Putc(char ch)              	//����1�����ַ�
{
	while((USART1->SR&0X40)==0);//�ȴ��������   
	USART1->DR = (u8) ch; 
}




void process_recv(uint16_t v)
{
	
}

int u1RecvLen=0;

void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 recvByte;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
	{  
		recvByte=USART_ReceiveData(USART1);
		if(recvByte=='\n')
		{
			uartRecvLen=u1RecvLen;
			if(u1RecvLen>1 && uartRecvBuf[u1RecvLen-1]=='\r')
			{
				uartRecvLen-=1;
			}
			u1RecvLen=0;
		}else{
			if(u1RecvLen>=UART1RECVSIZE)
			{
				u1RecvLen=0;
			}
			uartRecvBuf[u1RecvLen]=recvByte;
			u1RecvLen++;
		}
		
		/*if(Usart1Recv[0]==0){
			Usart1Cursor=0;
		}
		if(recvByte!='\n'){
			Usart1Recv[Usart1Cursor++]=recvByte;
		}else{
			Usart1Recv[Usart1Cursor]=0;
		}
		*/
	}
}




void USART2_IRQHandler(void)                	//??1??????
{
	u8 recvByte;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
	{
		recvByte=USART_ReceiveData(USART1);
		
		
	}
} 




