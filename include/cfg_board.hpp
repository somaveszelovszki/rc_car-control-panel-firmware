#pragma once

#include <micro/port/gpio.hpp>
#include <micro/port/timer.hpp>

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;

#define gpio_Led                micro::gpio_t{ GPIOA, GPIO_PIN_5 }
#define gpio_SpeedDir           micro::gpio_t{ GPIOA, GPIO_PIN_12 }

#define tim_System              micro::timer_t{ &htim3 }

#define tim_RcCtrl              micro::timer_t{ &htim1 }
#define timChnl_RcCtrlSteer     TIM_CHANNEL_1
#define timChnl_RcCtrlAccel     TIM_CHANNEL_2

#define tim_Drive          	    micro::timer_t{ &htim1 }
#define timChnl_DriveSteer      TIM_CHANNEL_3
#define timChnl_DriveAccel      TIM_CHANNEL_4
