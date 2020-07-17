#ifndef _WS2811_H
#define _WS2811_H
#include "sys.h"

#define WS2811_CONTROL PAout(0)   	
#define HIGH 1
#define SEND_TIMES 20
#define LIGHT_TIME 500         //单位：秒

//////////////////////控制函数声明/////////////////////////////////////
void WS2811_Init(void);
void TIM3_Int_Init(u16 arr,u16 psc);
void l_dat0(void);
void l_dat1(void);
void Reset(void);
void send_single_data(const u8 *data);
void send_string_data(const u8 *data,u8 size);
void RGB_open(const u8 *data);
void RGB_Lighting(void);
void RGB_ALARM(void);
/////////////////////////////////////////////////////////////////////

#endif


