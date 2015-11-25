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

float temp;

int Interrupt_Timer_Temperature_read;
// Flag that display the string "Temperature" once every mode change
osThreadId temperatureGetId;
float threshold = 50;
// thread ids for display_tilt_thread and alarm thread
//extern	osThreadId tilt_TurnDispFlag_thread;
//extern  osThreadId Alarm_LED_threadId;
//extern	osThreadId tilt_TurnDispFlag_thread;

void temperature_detection_thread (void const *argument) 
{
	
	
	/*Set configuration to ADC for acquiring Vadc*/
	adcSetup();
  setupTemperatureSensor();
	Temperature_Tim_Interrupt();
	temperatureGetId = osThreadGetId();
	
	while(1){
		
	    // wait for the signal data ready sent by the ISR
		osSignalWait(SIGNAL_TEMPERATURE_READY, osWaitForever);
		temp = runTemperatureSensorMoving();	
		Interrupt_Timer_Temperature_read=0;
	  //printf ("Temp:%f\n", temp); //--> FOR DEBUG PURPOSE
		//DisplayTemperature(temp);
	  

	}
}



void Temperature_Tim_Interrupt(void)
{
	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    TIM_TimeBaseInitTypeDef timerInitStructure;
	
    timerInitStructure.TIM_Prescaler = 4000;
    timerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    timerInitStructure.TIM_Period = 50;
    timerInitStructure.TIM_ClockDivision = 0;
	
    TIM_TimeBaseInit(TIM5, &timerInitStructure);
	
	  /* TIM Interrupts enable */
    TIM_ITConfig(TIM5,TIM_IT_Update, ENABLE);
	
    TIM_Cmd(TIM5, ENABLE);
	
	  NVIC_InitTypeDef nvicStructure;
	
    nvicStructure.NVIC_IRQChannel = TIM5_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 1;
    nvicStructure.NVIC_IRQChannelSubPriority = 2;
	
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
		
    NVIC_Init(&nvicStructure);
	
	  return;
	
}

void TIM5_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
  {
		// send a data ready signal to the temperature detection that calculates temperature and wake up the thread
		osSignalSet(temperatureGetId, SIGNAL_TEMPERATURE_READY);
     TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
  }
}