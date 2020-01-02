#include "sys.h"
#include "joystick.h"
#include "hw_config.h"
#include "delay.h"
#include "usart.h"

void  Joystick_Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1, ENABLE );	  //ʹ��ADC2ͨ��ʱ��
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	//AD1 A0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//AD1 A1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//AD1 A2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//AD1 A3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//AD1 A4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//AD1 A5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���

	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����	
	ADC_StartCalibration(ADC1);	 //����ADУ׼ 
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼���� 
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_239Cycles5 );
//	ADC_SoftwareStartConvCmd(ADC2, ENABLE);		//ʹ��ָ����ADC2�����ת����������

}		

//���ADCֵ
u16 Get_Adc1(u8 ch)   
{
	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	//ADC2,ADCͨ��,����ʱ��Ϊ239.5����
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC2�����ת����������	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
	return ADC_GetConversionValue(ADC1);	//�������һ��ADC2�������ת�����
}

u16 *Get_Axis_abs(u16 *num)
{
	
	num[1]=Get_Adc1(ADC_Channel_0);
	delay_ms(1);
	num[0]=Get_Adc1(ADC_Channel_1);
	delay_ms(1);
	num[2]=Get_Adc1(ADC_Channel_2);
	delay_ms(1);
	num[5]=Get_Adc1(ADC_Channel_3);
	delay_ms(1);
	num[4]=Get_Adc1(ADC_Channel_4);
	delay_ms(1);
	num[3]=Get_Adc1(ADC_Channel_5);
	delay_ms(1);
	
	return num;
}

/*
* Button GPIO, Axis ADC Init
*/
void Joystick_Init(){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��ʱ��
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	//����jtag����Ҫʹ��AFIO
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	Joystick_Adc_Init();
}

void ButtonGronp1_Scan(){
	u8 i=0;
	u8 bit=0;
	Joystick_SaveBtnGroup(BTN_GROUP1,0);
	/* btn 0 - 4*/
	for(i=12;i<=15;i++){
		if(PBin(i)==0){
			Joystick_SaveBtn(BTN_GROUP1, bit, 1);
		}
		bit++;
	}
	/* btn 5 - 7 */
	for(i=3;i<=6;i++){
		if(PBin(i)==0){
			Joystick_SaveBtn(BTN_GROUP1, bit, 1);
		}
		bit++;
	}
}


void ButtonGronp2_Scan(){
	u8 i=0;
	u8 bit=0;
	Joystick_SaveBtnGroup(BTN_GROUP2,0);
		
	/* btn 8 - 10 */
	for(i=7;i<=9;i++){
		if(PBin(i)==0){
			Joystick_SaveBtn(BTN_GROUP2, bit, 1);
		}
		bit++;
	}
	/* btn 11 */
	if(PBin(0)==0){
		Joystick_SaveBtn(BTN_GROUP2, bit, 1);
	}
	bit++;
	/* btn 12 */
	if(PBin(1)==0){
		Joystick_SaveBtn(BTN_GROUP2, bit, 1);
	}
	bit++;
	/* btn 13 */
	if(PBin(10)==0){
		Joystick_SaveBtn(BTN_GROUP2, bit, 1);
	}
	bit++;
	/* btn 14 */
	if(PBin(11)==0){
		Joystick_SaveBtn(BTN_GROUP2, bit, 1);
	}
	bit++;
	/* btn 15 */
	if(PAin(8)==0){
		Joystick_SaveBtn(BTN_GROUP2, bit, 1);
	}
	bit++;
}

uint8_t Get_Button16(){
	return PBin(14);
}

void joystick_calibration(){
	int i,j,k;
	u16 axis[6];
		for(k=0;k<3;k++){
			for(i=0;i<6;i++)
				axis[i]=0;
			Joystick_SaveAxisArray(axis);
			Joystick_Send();
			delay_ms(20);
			j=4096;
			while(j>=0){
				for(i=0;i<6;i++)
					axis[i]=4096-j;
				Joystick_SaveAxisArray(axis);
				Joystick_Send();
				delay_ms(50);
				j-=100;
			}
			
			j=4096;
			while(j>=0){
				for(i=0;i<6;i++)
					axis[i]=j;
				Joystick_SaveAxisArray(axis);
				Joystick_Send();
				delay_ms(50);
				j-=100;
			}
		}
		for(i=0;i<6;i++)
			axis[i]=2048;
		Joystick_SaveAxisArray(axis);
		Joystick_Send();
		delay_ms(800);
		delay_ms(800);
}

