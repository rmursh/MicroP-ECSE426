  /*******************************************************************
  * @file    SevenSegLED.c
  * @author  Razi Murshed
  * @date    22/10/2015
  * @brief  - Configuration of LED GPIOs
  *         - Display floating point number using 7 seg LED.
  *         - Display Angle
	********************************************************************/
	
	
#include <stdint.h>
#include <stdio.h>
#include "stm32f4xx_adc.h"
#include "stm32f4xx.h"
#include "SevenSegDisplay.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_conf.h"
#include "math.h"
#include "core_cm4.h"
#include <cmsis_os.h>

int LED_interrupt = 0;

/*
 * Function: LED_GPIO_Config
 * ----------------------------
 * Configures GPIO port D pins(0 - 10, 14) for 7-segment and PWM LED 
 *   Returns: void
 *
 *   @param: void
 */

void initializeDisplay()
{
	LED_GPIO_Config();
	configTIM4();
}



void LED_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure_D;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	GPIO_StructInit(&GPIO_InitStructure_D);
	GPIO_InitStructure_D.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2  | GPIO_Pin_3 | GPIO_Pin_4  | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure_D.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure_D.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure_D.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure_D.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_Init(GPIOD, &GPIO_InitStructure_D);
}

/*
 * Function: SevenSegDisplay
 * ----------------------------
 * Displays single digit number in a segment 
 *   Returns: void
 *
 *   @param: int number - Number to display (0 - 9) and 10 for Dot
 *   @param: int position - segment (1, 2, 3) 
 *
 *   returns: void
 */
void SevenSegDisplay(int number,  int position)
{
	
	// reject out of bounds number and position
	if ((number < 0) || (number > 14) || (position < 1) || (position > 4))
		return;
	
	// Select line implementation
	if(position == SEGMENT_0)
	{
		GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_SET);
		GPIO_WriteBit(GPIOD, GPIO_Pin_9, Bit_SET);
		GPIO_WriteBit(GPIOD, GPIO_Pin_10, Bit_SET);
		GPIO_WriteBit(GPIOD, GPIO_Pin_11, Bit_RESET);
	}
	else if (position == SEGMENT_1)
	{
		GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_RESET);
		GPIO_WriteBit(GPIOD, GPIO_Pin_9, Bit_SET);
		GPIO_WriteBit(GPIOD, GPIO_Pin_10, Bit_SET);
		GPIO_WriteBit(GPIOD, GPIO_Pin_11, Bit_SET);
	}
	else if (position == SEGMENT_2)
	{
		GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_SET);
		GPIO_WriteBit(GPIOD, GPIO_Pin_9, Bit_RESET);
		GPIO_WriteBit(GPIOD, GPIO_Pin_10, Bit_SET);
		GPIO_WriteBit(GPIOD, GPIO_Pin_11, Bit_SET);
	}
	else
	{
		GPIO_WriteBit(GPIOD, GPIO_Pin_8, Bit_SET);
		GPIO_WriteBit(GPIOD, GPIO_Pin_9, Bit_SET);
		GPIO_WriteBit(GPIOD, GPIO_Pin_10, Bit_RESET);
		GPIO_WriteBit(GPIOD, GPIO_Pin_11, Bit_SET);
	}
	
	// Display number 0 -> 9 and dot
	switch(number)
	{
		case 0:
			GPIO_WriteBit (GPIOD, GPIO_Pin_0, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_1, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_2, Bit_SET);
		
			GPIO_WriteBit (GPIOD, GPIO_Pin_3, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_4, Bit_SET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_5, Bit_SET);
		
		  GPIO_WriteBit (GPIOD, GPIO_Pin_6, Bit_RESET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_7, Bit_RESET);
		
		
		  break;
		
		case 1:
			GPIO_WriteBit (GPIOD, GPIO_Pin_0, Bit_RESET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_1, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_2, Bit_SET);
		
			GPIO_WriteBit (GPIOD, GPIO_Pin_3, Bit_RESET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_4, Bit_RESET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_5, Bit_RESET);
		
		  GPIO_WriteBit (GPIOD, GPIO_Pin_6, Bit_RESET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_7, Bit_RESET);
		
		  break;
		
		case 2:
			GPIO_WriteBit (GPIOD, GPIO_Pin_0, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_1, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_2, Bit_RESET);
		
			GPIO_WriteBit (GPIOD, GPIO_Pin_3, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_4, Bit_SET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_5, Bit_RESET);
		
		  GPIO_WriteBit (GPIOD, GPIO_Pin_6, Bit_SET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_7, Bit_RESET);
		break;
		
		case 3:
			GPIO_WriteBit (GPIOD, GPIO_Pin_0, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_1, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_2, Bit_SET);
		
			GPIO_WriteBit (GPIOD, GPIO_Pin_3, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_4, Bit_RESET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_5, Bit_RESET);
		
		  GPIO_WriteBit (GPIOD, GPIO_Pin_6, Bit_SET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_7, Bit_RESET);
		break;
		
		case 4:
			GPIO_WriteBit (GPIOD, GPIO_Pin_0, Bit_RESET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_1, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_2, Bit_SET);
		
			GPIO_WriteBit (GPIOD, GPIO_Pin_3, Bit_RESET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_4, Bit_RESET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_5, Bit_SET);
		
		  GPIO_WriteBit (GPIOD, GPIO_Pin_6, Bit_SET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_7, Bit_RESET);
		break;
		
		case 5:
			GPIO_WriteBit (GPIOD, GPIO_Pin_0, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_1, Bit_RESET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_2, Bit_SET);
		
			GPIO_WriteBit (GPIOD, GPIO_Pin_3, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_4, Bit_RESET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_5, Bit_SET);
		
		  GPIO_WriteBit (GPIOD, GPIO_Pin_6, Bit_SET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_7, Bit_RESET);
		
		  break;
		
		case 6:
			GPIO_WriteBit (GPIOD, GPIO_Pin_0, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_1, Bit_RESET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_2, Bit_SET);
		
			GPIO_WriteBit (GPIOD, GPIO_Pin_3, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_4, Bit_SET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_5, Bit_SET);
		
		  GPIO_WriteBit (GPIOD, GPIO_Pin_6, Bit_SET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_7, Bit_RESET);
		
		  break;
		
		case 7:
			GPIO_WriteBit (GPIOD, GPIO_Pin_0, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_1, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_2, Bit_SET);
		
			GPIO_WriteBit (GPIOD, GPIO_Pin_3, Bit_RESET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_4, Bit_RESET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_5, Bit_RESET);
		
		  GPIO_WriteBit (GPIOD, GPIO_Pin_6, Bit_RESET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_7, Bit_RESET);
		
		  break;
		
		case 8:
			GPIO_WriteBit (GPIOD, GPIO_Pin_0, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_1, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_2, Bit_SET);
		
			GPIO_WriteBit (GPIOD, GPIO_Pin_3, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_4, Bit_SET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_5, Bit_SET);
		
		  GPIO_WriteBit (GPIOD, GPIO_Pin_6, Bit_SET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_7, Bit_RESET);
		
		  break;
		
		case 9:
			GPIO_WriteBit (GPIOD, GPIO_Pin_0, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_1, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_2, Bit_SET);
		
			GPIO_WriteBit (GPIOD, GPIO_Pin_3, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_4, Bit_RESET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_5, Bit_SET);
		
		  GPIO_WriteBit (GPIOD, GPIO_Pin_6, Bit_SET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_7, Bit_RESET);
		
		  break;
		
		//CASE 10 DOT
		case 10:
			GPIO_WriteBit (GPIOD, GPIO_Pin_0, Bit_RESET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_1, Bit_RESET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_2, Bit_RESET);
		
			GPIO_WriteBit (GPIOD, GPIO_Pin_3, Bit_RESET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_4, Bit_RESET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_5, Bit_RESET);
		
		  GPIO_WriteBit (GPIOD, GPIO_Pin_6, Bit_RESET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_7, Bit_SET);
		
		  break;
		// Case 11 12 keno?
		case 11:
			GPIO_WriteBit (GPIOD, GPIO_Pin_0, Bit_RESET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_1, Bit_RESET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_2, Bit_RESET);
		
			GPIO_WriteBit (GPIOD, GPIO_Pin_3, Bit_RESET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_4, Bit_RESET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_5, Bit_RESET);
		
		  GPIO_WriteBit (GPIOD, GPIO_Pin_6, Bit_SET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_7, Bit_RESET);
		
		  break;
		
		case 12:
			GPIO_WriteBit (GPIOD, GPIO_Pin_0, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_1, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_2, Bit_RESET);
		
			GPIO_WriteBit (GPIOD, GPIO_Pin_3, Bit_RESET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_4, Bit_RESET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_5, Bit_SET);
		
		  GPIO_WriteBit (GPIOD, GPIO_Pin_6, Bit_SET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_7, Bit_RESET);
		
		  break;
	
		case 13:
			GPIO_WriteBit (GPIOD, GPIO_Pin_0, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_1, Bit_RESET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_2, Bit_RESET);
		
			GPIO_WriteBit (GPIOD, GPIO_Pin_3, Bit_SET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_4, Bit_SET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_5, Bit_SET);
		
		  GPIO_WriteBit (GPIOD, GPIO_Pin_6, Bit_RESET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_7, Bit_RESET);	
		
		  break;
		
		case 14:
			GPIO_WriteBit (GPIOD, GPIO_Pin_0, Bit_RESET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_1, Bit_RESET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_2, Bit_RESET);
		
			GPIO_WriteBit (GPIOD, GPIO_Pin_3, Bit_RESET);
			GPIO_WriteBit (GPIOD, GPIO_Pin_4, Bit_RESET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_5, Bit_RESET);
		
		  GPIO_WriteBit (GPIOD, GPIO_Pin_6, Bit_RESET);	
		  GPIO_WriteBit (GPIOD, GPIO_Pin_7, Bit_RESET);	
		
		default:
			return;
	}
	
}

/*
 * Function: Delay
 * ----------------------------
 * Countdown timer
 *   Returns: void
 *
 *   @param: int nCount - countds down from this value
 *   returns: void
 */
void Delay(int nCount) 
{
  while(nCount--);
}

void DisplayTemperature(float temperature)
{
	//init the array
	float temp[4] = {0, 0, 0, 0};
	int div = 10;
	float ten=10.0;
	int temp_cast = 0;
	
	if (temperature < 0)
	{
		printf("Temperature:%f is less then zero.\n", temperature);
		return;
	}
	
	if (temperature < 100)
	{
		temp[0] = (int)(temperature / div);
		temp[1] = (int)(temperature - (temp[0])*div);
		temp[2] = 10; // dot not required
		temp_cast = (int)temperature;
		temp[3] = (temperature - (float)temp_cast)*ten;
	
	}
	else
	{
		return;
	}
	
 
	
	if (LED_interrupt==1)
	{
		SevenSegDisplay(temp[0], SEGMENT_1);
		Delay(100);
	}
	else if (LED_interrupt== 2)
 	{
		SevenSegDisplay(temp[1], SEGMENT_2);
		Delay(100);
	}
  else if (LED_interrupt== 3)
 	{
		SevenSegDisplay(temp[2], SEGMENT_2);
		Delay(100);
	}	
	else if (LED_interrupt== 4)
 	{
		SevenSegDisplay(temp[3], SEGMENT_3);
		Delay(100);
	}	
		else if (LED_interrupt== 5)
 	{
		SevenSegDisplay(13, SEGMENT_0);
		Delay(100);
	}
}

void LED_clear()
{
	SevenSegDisplay(14, SEGMENT_0);
	Delay(100);
	SevenSegDisplay(14, SEGMENT_1);
	Delay(100);
	SevenSegDisplay(14, SEGMENT_2);
	Delay(100);
	SevenSegDisplay(14, SEGMENT_3);
	Delay(100);
}

/*
 * Function: DisplayAngle
 * ----------------------------
 * Breaks the floating point angle into 2 segments
 *
 *   @param: float angle - tilt angle in float
 *   @param: uint8_t segment to display
 *   returns: void
 */


void DisplayAngle(float angle)
{
	
	//init the array
	int temp[4] = {0, 0, 0, 0};
	int div = 10;
	int div_100 = 100;
	float ten=10.0;
	int temp_cast = 0;
	int temp_angle= angle;
	
	// reject less than zero angle 
	if (angle < 0)
	{
		angle = fabs(angle);
	}
	
	if ((angle < 100)&&(angle > 10))
	{
		temp[0] = (int)(angle / div);
		temp[1] = (int)(angle - (temp[0])*div);
		temp[2] = 10; // dot
		temp_cast = (int)angle;
		temp[3] = (angle - (float)temp_cast)*ten;
		
		if (LED_interrupt==1)
		{
			SevenSegDisplay(temp[0], SEGMENT_1);
			Delay(100);
		}
		else if (LED_interrupt== 2)
		{
			SevenSegDisplay(temp[1], SEGMENT_2);
			Delay(100);
		}
		else if (LED_interrupt== 3)
		{
			SevenSegDisplay(temp[2], SEGMENT_2);
			Delay(100);
		}	
		else if (LED_interrupt== 4)
		{
			SevenSegDisplay(temp[3], SEGMENT_3);
			Delay(100);
		}	
			else if (LED_interrupt== 5)
		{
			SevenSegDisplay(12, SEGMENT_0);
			Delay(100);
		}	

	}
	else if ((angle < 10))
	{
		temp[0] = (int)(angle);
		temp[1] =  10; // dot
		temp_cast = (int)angle;
		temp[2] =  (angle - (float)temp_cast)*ten;
		temp[3] = (int)((angle)*100)%10;
		
		if (LED_interrupt==1)
		{
			SevenSegDisplay(temp[0], SEGMENT_1);
			Delay(100);
		}
		else if (LED_interrupt== 2)
		{
			SevenSegDisplay(temp[1], SEGMENT_1);
			Delay(100);
		}
		else if (LED_interrupt== 3)
		{
			SevenSegDisplay(temp[2], SEGMENT_2);
			Delay(100);
		}	
		else if (LED_interrupt== 4)
		{
			SevenSegDisplay(temp[3], SEGMENT_3);
			Delay(100);
		}	
			else if (LED_interrupt== 5)
		{
			SevenSegDisplay(12, SEGMENT_0);
			Delay(100);
		}	

	}
	else //if (angle >= 100)
	{
		temp[0] = (int)(angle / div_100);
		temp[1] = (int)((angle - (temp[0])*div_100)/div);
		temp[2] = (float)angle - ((temp[0])*div_100 + (temp[1])*div);
		
	if (LED_interrupt==1)
	{
		SevenSegDisplay(temp[0], SEGMENT_1);
		Delay(100);
	}
	else if (LED_interrupt== 2)
 	{
		SevenSegDisplay(temp[1], SEGMENT_2);
		Delay(100);
	}
  else if (LED_interrupt== 3)
 	{
		SevenSegDisplay(temp[2], SEGMENT_3);
		Delay(100);
	}	
	else if (LED_interrupt== 4)
 	{
		SevenSegDisplay(12, SEGMENT_0);
		Delay(100);
	}	
		else if (LED_interrupt== 5)
 	{
		LED_interrupt++;
	}
	}
	
	return;	
}

void configTIM4(void)
{
	//TIM2 on the APB1 bus with a clock frequency 42 MHz
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseInitTypeDef tim_init;
	NVIC_InitTypeDef tim_nvic;

	//The period the interrupt will be thrown by this timer will be 50 Hz. So we want to count to a certain value (period) at a clock frequency such that the result is 50 Hz.
	tim_init.TIM_Prescaler = 4000; //Used to divide the clock frequency, 42 MHz is way too fast to, so lets pick 30000 arbitrarily to slow it down
	tim_init.TIM_CounterMode = TIM_CounterMode_Up;
	tim_init.TIM_Period =  50; //42 MHz divided by 30000 is 1400 so now running at 1.4 kHz, count up to 28 at 1.4 kHz to get 50 Hz interrupt
	tim_init.TIM_ClockDivision = TIM_CKD_DIV1; //Don't want any clock division since we have already achieved our desired interrupt frequency

	TIM_TimeBaseInit(TIM4, &tim_init); //Initialize timer
		
	tim_nvic.NVIC_IRQChannel = TIM4_IRQn; //TIM2 because that is the timer we are using
	tim_nvic.NVIC_IRQChannelCmd = ENABLE; //We want to enable the line specified in the line above
	tim_nvic.NVIC_IRQChannelPreemptionPriority = 0x02; //Between 0-15 for priority level, lower number = higher priority: set to highest
	tim_nvic.NVIC_IRQChannelSubPriority = 0x02; //Between 0-15 for next priority level, lower number = higher priority: set to highest 
	
	NVIC_Init(&tim_nvic); //Initialize interrupt
	
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //This line essentially links the timer and interrupt 
	
	TIM_Cmd(TIM4, ENABLE); //Starts the timer
}

void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		LED_interrupt++;
     if(LED_interrupt > 5)
	{
		LED_interrupt = 1;
	}
		//Clear the pending bit
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}


