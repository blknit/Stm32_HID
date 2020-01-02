#include "usb_lib.h" 
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb_lib.h"
#include "usb_istr.h"
#include "led.h"
#include "usart.h"
#include "delay.h"
#include "hw_config.h"

extern __IO uint8_t EP1_Complete;
extern __IO uint8_t EP2_Complete;
extern __IO uint8_t EP3_Complete;

HIDOUT_CALLBACK CustomOutCb;
HIDOUT_CALLBACK KeyBoardOutCb;
 
/////////////////////////////////////////////////////////////////////////////////
//��USB����ͨ�ò��ִ���,ST����USB����,�˲��ִ��붼���Թ���.
//�˲��ִ���һ�㲻��Ҫ�޸�!
//USB�����жϷ�����
void USBWakeUp_IRQHandler(void) 
{
	EXTI->PR|=1<<18;//���USB�����жϹ���λ
} 

//USB�жϴ�����
void USB_LP_CAN1_RX0_IRQHandler(void) 
{
	USB_Istr();
} 

//USBʱ�����ú���,USBclk=48Mhz @HCLK=72Mhz
void Set_USBClock(void)
{
 /* Select USBCLK source */
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  /* Enable the USB clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);	
}


//USB�˳��͹���ģʽ
//�û��������������ش���(������������ʱ�ӵ�)
void Leave_LowPowerMode(void)
{
	DEVICE_INFO *pInfo=&Device_Info;
	printf("leave low power mode\r\n"); 
	if (pInfo->Current_Configuration!=0)bDeviceState=CONFIGURED; 
	else bDeviceState = ATTACHED; 
} 

//USB�ж�����
void USB_Interrupts_Config(void)
{ 
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	EXTI->IMR|=1<<18;	//������18�ϵ��ж�
 	EXTI->RTSR|=1<<18;	//line 18���¼��������ش���	 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}	

//USB�ӿ�����(����1.5K��������,ս��V3����Ҫ����,������)
//NewState:DISABLE,������
//         ENABLE,����
void USB_Cable_Config (FunctionalState NewState)
{ 
	if (NewState!=DISABLE)printf("usb pull up enable\r\n"); 
	else printf("usb pull up disable\r\n"); 
}

//USBʹ������/����
//enable:0,�Ͽ�
//       1,��������	   
void USB_Port_Set(u8 enable)
{
	RCC->APB2ENR|=1<<2;    //ʹ��PORTAʱ��	   	 
	if(enable)_SetCNTR(_GetCNTR()&(~(1<<1)));//�˳��ϵ�ģʽ
	else
	{	  
		_SetCNTR(_GetCNTR()|(1<<1));  // �ϵ�ģʽ
		GPIOA->CRH&=0XFFF00FFF;
		GPIOA->CRH|=0X00033000;
		PAout(12)=0;	    		  
	}
}  

//��ȡSTM32��ΨһID
//����USB������Ϣ
void Get_SerialNum(void)
{
	u32 Device_Serial0, Device_Serial1, Device_Serial2;
	Device_Serial0 = *(u32*)(0x1FFFF7E8);
	Device_Serial1 = *(u32*)(0x1FFFF7EC);
	Device_Serial2 = *(u32*)(0x1FFFF7F0);
	Device_Serial0 += Device_Serial2;
	if (Device_Serial0 != 0)
	{
		IntToUnicode (Device_Serial0,&Joystick_StringSerial[2] , 8);
		IntToUnicode (Device_Serial1,&Joystick_StringSerial[18], 4);
	}
}

//��32λ��ֵת����unicode.
//value,Ҫת����ֵ(32bit)
//pbuf:�洢��ַ
//len:Ҫת���ĳ���
void IntToUnicode (u32 value , u8 *pbuf , u8 len)
{
	u8 idx = 0;
	for( idx = 0 ; idx < len ; idx ++)
	{
		if( ((value >> 28)) < 0xA )
		{
			pbuf[ 2* idx] = (value >> 28) + '0';
		}
		else
		{
			pbuf[2* idx] = (value >> 28) + 'A' - 10; 
		} 
		value = value << 4; 
		pbuf[ 2* idx + 1] = 0;
	}
}


/**
  * Function Name : Mouse_Send.
  * Description   : as Name Description
  * Input         : buf and len
  * Output        : None.
  * Return value  : None.
  */

void Mouse_Send(uint8_t *buf,uint8_t bLen)
{  
  /* Reset the control token to inform upper layer that a transfer is ongoing */
  EP2_Complete = 0;
  /* Copy mouse position info in ENDP1 Tx Packet Memory Area*/
  USB_SIL_Write(EP2_IN, buf, bLen);
  /* Enable endpoint for transmission */
  SetEPTxValid(ENDP2);
}

void KeyBoard_Send(uint8_t *buf,uint8_t bLen)
{  
  /* Reset the control token to inform upper layer that a transfer is ongoing */
  EP1_Complete = 0;
  /* Copy mouse position info in ENDP1 Tx Packet Memory Area*/
  USB_SIL_Write(EP1_IN, buf, bLen);
  /* Enable endpoint for transmission */
  SetEPTxValid(ENDP1);
}

uint32_t KeyBoard_Get(uint8_t *buf)
{  
	uint32_t len;
  /* Reset the control token to inform upper layer that a transfer is ongoing */
  /* Copy mouse position info in ENDP1 Tx Packet Memory Area*/
  len = USB_SIL_Read(EP1_OUT, buf);
  /* Enable endpoint for transmission */
  SetEPRxValid(ENDP1);
	return len;
}

void CustomHID_Send(uint8_t *buf,uint8_t bLen)
{  
  /* Reset the control token to inform upper layer that a transfer is ongoing */
  EP3_Complete = 0;
  /* Copy mouse position info in ENDP1 Tx Packet Memory Area*/
  USB_SIL_Write(EP3_IN, buf, bLen);
  /* Enable endpoint for transmission */
  SetEPTxValid(ENDP3);
}


uint32_t CustomHID_Get(uint8_t *buf)
{  
	uint32_t len;
  /* Reset the control token to inform upper layer that a transfer is ongoing */
  /* Copy mouse position info in ENDP1 Tx Packet Memory Area*/
  len = USB_SIL_Read(EP3_OUT, buf);
  /* Enable endpoint for transmission */
  SetEPRxValid(ENDP3);
	return len;
}


//���USB��/д�����Ѿ����
void Led_RW_OFF(void)
{
	LED1=1;
}

//��ʾUSB��δ�������
void USB_NotConfigured_LED(void)
{
	printf("USB NotConfigured\r\n");
}




















