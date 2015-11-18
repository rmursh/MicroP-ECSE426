#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_adc.h"
#include "core_cm4.h"
#include <cmsis_os.h>
#include <SevenSegDisplay.h>
#include "TemperatureSensor.h"
#include "temperature_thread.h"
#include "SevenSegDisplay.h"
#include "accelerometer_thread.h"
#include "keypad.h"
#include "DisplayLED.h"

extern float temp;
extern float roll;
extern float pitch;
int disp_interrupt;
int displayState = 0;
int displayRollPitch = 0;
int thresh = 40;


void DispFlag_thread(void const * argument)
{
	// Extracts the semaphore object from the argument.
	// This semaphore object was created in the main
	osSemaphoreId* semaphore;
	semaphore = (osSemaphoreId*)argument;
	keypad_init();
	configTIM2();
	extern	osThreadId TurnDispFlag_thread;
	
	uint8_t ResourceLocked = 1;  // Flag gets turned on if semaphore_lock achieved
	uint32_t tokentemp;          // Return of semwait
	
	// Wait for the semaphore with 1ms timeout
  osEvent event = osSignalWait(SIGNAL_DISP_TEMPERATURE, 1);

	while(1)
	{
			
		tokentemp = osSemaphoreWait (*semaphore, osWaitForever);
    ResourceLocked=1; // turn flag on if resource_locked		
		
    while (ResourceLocked)
		{
			
			// wait for a signal with 1ms timeout
			//event = osSignalWait(SIGNAL_DISP_TEMPERATURE, 1);
			if (event.status == osEventTimeout)
			{
				if(temp < thresh)
				{	
					// in case of timeout display
					if(displayState == 1)
					{
						DisplayTemperature(temp);
					}
					else
					{
						if(displayRollPitch == 1)
						{
						 DisplayAngle(roll);
						}
						else
						{
							DisplayAngle(pitch);
						}
					}
			 }
			 else
			 {
				 if(disp_interrupt < 100)
				 {
					if(displayState == 1)
					{
						DisplayTemperature(temp);
					}
					else
					{
						if(displayRollPitch == 1)
						{
						 DisplayAngle(roll);
						}
						else
						{
							DisplayAngle(pitch);
						}
					}
				 }
        else
				 {
					 LED_clear();
				   //Delay(100);
					 
				 }
			 }
				
			}
			else
			{
				// signal received which is not timeout
				// clear the signal flag for the tilt display thread
				//osSignalClear (TurnDispFlag_thread, SIGNAL_DISP_TILT);
				
				
				// turn the resource_locked flag off
				ResourceLocked = 0;
				tokentemp=0;
				
				
				// release the semaphore 
				osSemaphoreRelease(*semaphore);
				
				//wait 5ms for the other thread to grab the semaphore
			  osDelay(5);
			}
		}
		
	}
	
}

void configTIM2(void)
{
	//TIM3 on the APB1 bus with a clock frequency 42 MHz
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	TIM_TimeBaseInitTypeDef tim_init;
	NVIC_InitTypeDef tim_nvic;

	//The period the interrupt will be thrown by this timer will be 100 Hz. So we want to count to a certain value (period) at a clock frequency such that the result is 100 Hz.
	tim_init.TIM_Prescaler = 4000; //Used to divide the clock frequency, 42 MHz is way too fast to, so lets pick 30000 arbitrarily to slow it down
	tim_init.TIM_CounterMode = TIM_CounterMode_Up;
	tim_init.TIM_Period =  100; //42 MHz divided by 30000 is 1400 so now running at 1.4 kHz, count up to 7 at 1.4 kHz to get 200 Hz interrupt
	tim_init.TIM_ClockDivision = TIM_CKD_DIV1; //Don't want any clock division since we have already achieved our desired interrupt frequency

	TIM_TimeBaseInit(TIM2, &tim_init); //Initialize timer
		
	tim_nvic.NVIC_IRQChannel = TIM2_IRQn; //TIM3 because that is the timer we are using
	tim_nvic.NVIC_IRQChannelCmd = ENABLE; //We want to enable the line specified in the line above
	tim_nvic.NVIC_IRQChannelPreemptionPriority = 0x03; //Between 0-15 for priority level, lower number = higher priority: set to highest
	tim_nvic.NVIC_IRQChannelSubPriority = 0x00; //Between 0-15 for next priority level, lower number = higher priority: set to highest 
	
	NVIC_Init(&tim_nvic); //Initialize interrupt
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //This line essentially links the timer and interrupt 
	
	TIM_Cmd(TIM2, ENABLE); //Starts the timer
}

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
		disp_interrupt++;
			 if(disp_interrupt > 200)
					 {
						 disp_interrupt =0;
					 }
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
}

void keypad_thread(void const *argument){
	while(1)
	{
		int keyRead = NO_KEY_PRESSED; 
			keyRead = keypad_read();
		
		// Mode change
		if (keyRead == -2)
		{
			displayState++;
			displayState %= 2;
			printf("%d\n", keyRead);
		}
		else if (keyRead == 1)
		{
			displayRollPitch = 1;

			printf("%d\n", keyRead);
		}
		else if (keyRead == 2)
		{
			displayRollPitch = 2;
			printf("%d\n", keyRead);
		}
		

				
		osDelay(5);
	}
}