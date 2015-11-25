#include <stdio.h>
#include <stdlib.h>
#include "cmsis_os.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include <configure_accelerometer.h>
#include <lis3dsh.h>
#include "MovingAverageFilter.h"
#include "calibrate.h"
#include "calculate_angle_tilted.h"
#include <MEM_Interrupt.h>
#include <SevenSegDisplay.h>
#include "math.h"
#include "accelerometer_thread.h"
#include "temperature_thread.h"
#include "keypad.h"

//extern	osThreadId temperature_TurnDispFlag_thread;

float roll, pitch;
int accelerometer_interrupt =1 ;

osThreadId tiltGetId;

void initialize_buffers_for_filter(mAState maState_x, mAState maState_y, mAState maState_z)
{
  int init_buffer = 0;               													// index used to initialize the buffer
	
	maState_x.newIndex = 0;
	maState_x.buffer_length = 0;
	while(init_buffer < DEPTH_D)
	{
		maState_x.filterBuffer[init_buffer] = 0;
		init_buffer++;
	}
	
	init_buffer = 0;               // index used to initialize the buffer
	
	maState_y.newIndex = 0;
	maState_y.buffer_length = 0;
	while(init_buffer < DEPTH_D)
	{
		maState_y.filterBuffer[init_buffer] = 0;
		init_buffer++;
	}
	
	init_buffer = 0;               // index used to initialize the buffer
	
	maState_z.newIndex = 0;
	maState_z.buffer_length = 0;
	while(init_buffer < DEPTH_D)
	{
		maState_z.filterBuffer[init_buffer] = 0;
		init_buffer++;
	}
}

void accelerometer_thread(void const *argument)
{
  mAState maState_x;
	mAState maState_y;
	mAState maState_z;
	roll=0, pitch=0;
	float raw_data[3] = {0, 0, 0};
	float calibrated_data[3] = {0, 0, 0};
  float filtered_data[3] = {0, 0, 0};
	initialize_buffers_for_filter(maState_x, maState_y,maState_z);
	set_Accelerometer_MEM();
	Delay(100);
  MEM_Interrupt_Config();
	Delay(100);
	EXTI_NVIC_Config();
	Delay(100);
	configTIM3();
  tiltGetId = osThreadGetId();
  
	LIS3DSH_ReadACC(raw_data);
	while(1)
	{	
			//Raw data from accelerometer
		  osSignalWait(SIGNAL_TILT_READY, osWaitForever);
			//Put values through filter
			filtered_data[0] = maFilter(&maState_x, raw_data[0]);
			filtered_data[1] = maFilter(&maState_y, raw_data[1]);
			filtered_data[2] = maFilter(&maState_z, raw_data[2]);
			get_calibration_data(filtered_data, calibrated_data);
			roll = calculate_roll(calibrated_data);
			pitch = calculate_pitch(calibrated_data);	
			LIS3DSH_ReadACC(raw_data);
				
								
			 
							
 }
}






//void EXTI0_IRQHandler(void)
//{
//  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
//  {
//		// call the push button interrupt isr
//		accelerometer_interrupt = 1 ;
//		// wait to avoid button debounce
//		
//    /* Clear the EXTI line 1 pending bit */
//    EXTI_ClearITPendingBit(EXTI_Line0);
//  }
//}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		//Clear the pending bit
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		//Set flag
		osSignalSet(tiltGetId, SIGNAL_TILT_READY);
	}
}

void configTIM3(void)
{
	//TIM3 on the APB1 bus with a clock frequency 42 MHz
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseInitTypeDef tim_init;
	NVIC_InitTypeDef tim_nvic;

	//The period the interrupt will be thrown by this timer will be 100 Hz. So we want to count to a certain value (period) at a clock frequency such that the result is 100 Hz.
	tim_init.TIM_Prescaler = 4000; //Used to divide the clock frequency, 42 MHz is way too fast to, so lets pick 30000 arbitrarily to slow it down
	tim_init.TIM_CounterMode = TIM_CounterMode_Up;
	tim_init.TIM_Period =  210; //42 MHz divided by 30000 is 1400 so now running at 1.4 kHz, count up to 7 at 1.4 kHz to get 200 Hz interrupt
	tim_init.TIM_ClockDivision = TIM_CKD_DIV1; //Don't want any clock division since we have already achieved our desired interrupt frequency

	TIM_TimeBaseInit(TIM3, &tim_init); //Initialize timer
		
	tim_nvic.NVIC_IRQChannel = TIM3_IRQn; //TIM3 because that is the timer we are using
	tim_nvic.NVIC_IRQChannelCmd = ENABLE; //We want to enable the line specified in the line above
	tim_nvic.NVIC_IRQChannelPreemptionPriority = 0x00; //Between 0-15 for priority level, lower number = higher priority: set to highest
	tim_nvic.NVIC_IRQChannelSubPriority = 0x00; //Between 0-15 for next priority level, lower number = higher priority: set to highest 
	
	NVIC_Init(&tim_nvic); //Initialize interrupt
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //This line essentially links the timer and interrupt 
	
	TIM_Cmd(TIM3, ENABLE); //Starts the timer
}
