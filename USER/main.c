#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "WS2811.h"

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数

	WS2811_Init();    //灯管初始化
	while(1)
	{
			RGB_Lighting();
	}
}
