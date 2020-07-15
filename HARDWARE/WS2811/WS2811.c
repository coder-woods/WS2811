#include "WS2811.h"

static void delay(unsigned long int us);

static u8 Gard[3]={0};  // BLUE RED GREEN
static u16 TIM3_count=0;
static u8 Light_blue=0;
static u8 Light_red=0;
static u8 Light_green=0;

//==================================WS2811初始化======================================//
void WS2811_Init(void)
{
		GPIO_InitTypeDef  GPIO_InitStructure;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟

		//GPIOA0初始化设置
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//下拉拉
		GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
    Reset();
	
		TIM3_Int_Init(1000,84);                   //TIM3 1KHZ
}
//==================================通用定时器3初始化================================//
//arr：自动重装值sc：时钟预分频数     定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET){ //溢出中断
			TIM3_count++;
			if((TIM3_count%(LIGHT_TIME*3))==0){
					Light_blue=1;
					Light_red=0;
					Light_green=0;
			}
			else if((TIM3_count%(LIGHT_TIME*3))==(LIGHT_TIME*1)){
					Light_blue=0;
					Light_red=1;
					Light_green=0;
			}
			else if((TIM3_count%(LIGHT_TIME*3))==(LIGHT_TIME*2)){
					Light_blue=0;
					Light_red=0;
					Light_green=1;
			}
			else if(TIM3_count>(LIGHT_TIME*3))
				TIM3_count=0;
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}

//===============高速编码模式BIT0(高电平时间:0.5us 低电平时间:2us )===============//
void l_dat0(void)
{
		WS2811_CONTROL|=HIGH;
		delay(3);
		WS2811_CONTROL&=~HIGH;
		delay(15);
}

//===============高速编码模式BIT1(高电平时间:5us 低电平时间:0.5us)================//
void l_dat1(void)
{
		WS2811_CONTROL|=HIGH;
		delay(16);
		WS2811_CONTROL&=~HIGH;
		delay(2);
}

//===================================RGB复位====================================//
void Reset(void)
{
		WS2811_CONTROL&=~HIGH;
		delay(2000);
}

//============================发送RGB灰度数据===================================//
void send_single_data(const u8 *data) 											//数据格式:R7~R0~G7~G0~B7~B0
{
		s8 i=0,j=0;
		for(i = 0; i < 3; i++){
				for(j = 7; j > -1; j--){
						if((*(data+i)&(HIGH<<j))==(HIGH<<j)) 
							l_dat1(); 
						else 
							l_dat0();
				}
		}
}

void send_string_data(const u8 *data,u8 size) 											
{
		u8 i=0;
		for(i=0;i<size;i++)
		{
			send_single_data(data);	 
		}
    Reset();
}
//==================================RGB常亮=====================================//
void RGB_open(const u8 *data) //RGB常亮
{
		send_single_data(data);
}

//=================================RGB闪烁========= BLUE RED GREEN ===============//
void RGB_Lighting(void)
{
		if(Light_blue){
				Gard[0]=255;
				Gard[1]=0;
				Gard[2]=0;
				send_string_data(Gard,SEND_TIMES);              //发送次数与灯管长度有关
			  Light_blue=0;
		}
		else if(Light_red){
			Gard[0]=0;
			Gard[1]=255;
			Gard[2]=0;
			send_string_data(Gard,SEND_TIMES); 
			Light_red=0;
		}
		else if(Light_green){
				Gard[0]=0;
				Gard[1]=0;
				Gard[2]=255;
				send_string_data(Gard,SEND_TIMES); 
				Light_green=0;
		}
}

//=================================延时========= BLUE RED GREEN ===============//
static void delay(unsigned long int us)
{
		unsigned long int j;
		for (j = 5*us; j> 0; j--) ;
}





