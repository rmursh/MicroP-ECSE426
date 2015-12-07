
/*
	******************************************************************************
	* Headers
	******************************************************************************
*/

#include "Ext_Sensor.h"

/**
  * @brief 	Initializing Acclerometer Sensor
	* @param 	None
  * @retval None
  */	

void init_acc(){

	uint8_t ctrl = 0x00;
	LSM9DS1_Write(&ctrl, INT1_CTRL,1);
	LSM9DS1_Write(&ctrl, INT2_CTRL,1);
	
	Ext_Sensor_Struct accel_struct;
	accel_struct.Power_Mode_Output_DataRate = LSM9DS1_DATARATE_238_XL;
	accel_struct.Axes_Enable = LSM9DS1_X_ENABLE_XL | LSM9DS1_Y_ENABLE_XL | LSM9DS1_Z_ENABLE_XL;
	accel_struct.Decimation = LSM9DS1_DECIMATION_0;
	accel_struct.BW_select = LSM9DS1_BW_SELECT_ODR;			
	accel_struct.Full_Scale = LSM9DS1_FULLSCALE_2_XL;
	accel_struct.AA_Filter_BW = LSM9DS1_AA_BW_XL_408;
	LSM9DS1_Acc_Init(&accel_struct);

	/*Configure MEMS interrupts */
	EXT_DRY_INT accel_int;
	accel_int.Acc_Int = LSM9DS1_INT1_DRDY_XL_ENABLE; 
	LSM9DS1_INT_Config(&accel_int);

	/*Activating clocks*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); /* Activate SYSCFG in order to link PE0 to EXTI0 
																														Check Figure 27 in Reference Manual*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	 /* Activate E in order to use pin PE0, PE1, PE3 
																														used by MEMS */

	/*Configure PE0 used by MEMS_INT1 output of MEMS (check Schematics)*/
	GPIO_InitTypeDef GPIO_config_s;
	GPIO_config_s.GPIO_Mode = GPIO_Mode_IN;
	GPIO_config_s.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_config_s.GPIO_Pin = GPIO_Pin_0;
	GPIO_config_s.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_config_s.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOE, &GPIO_config_s);



  NVIC_InitTypeDef NVIC_InitStructure;																																								
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);																//  enable clock of tim3
																																											// tim3 global interrupt enables 
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;																			// NVIC channel to TIMIRQs
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;												// group priority to zero
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;																// sub group to 01
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;																			// enable it
  NVIC_Init(&NVIC_InitStructure);																											// initialize it
 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
																																											//  configuration of tim3 time base
  TIM_TimeBaseStructure.TIM_Period = 88;																								//	Period to 1
  TIM_TimeBaseStructure.TIM_Prescaler = 4000;																				// prescaler to 0
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;																				// clock division to 0
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;													//	enabled the counter
	TIM_TimeBaseStructure.TIM_RepetitionCounter =0;																			// disbaled repeation counter
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);																		// initilizing TIM3 to TIM_TimeBaseStructure
		
			  
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);																					// Enable TIM Interrupt 
	
	 
  TIM_Cmd(TIM3, ENABLE);																															 // Enable TIM3 counter 


	NVIC_EnableIRQ(TIM3_IRQn);	
	/*Configure EXTI0 to listen to interrupts */
//	EXTI_InitTypeDef EXTI_config_s;
//	EXTI_config_s.EXTI_Line = EXTI_Line0;
//	EXTI_config_s.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_config_s.EXTI_Trigger = EXTI_Trigger_Rising;  
//	EXTI_config_s.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_config_s);

//	/*Link PE0 to EXTI0*/ 
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);

//	/*Enable interrupt control and set priority of EXTI0*/
//	NVIC_InitTypeDef   NVIC_config_s;
//	NVIC_config_s.NVIC_IRQChannel = EXTI0_IRQn;							//Channel
//	NVIC_config_s.NVIC_IRQChannelPreemptionPriority = 0x00; //Set x higher than y
//	NVIC_config_s.NVIC_IRQChannelSubPriority = 0x00;				//Set x1 higher the x2
//	NVIC_config_s.NVIC_IRQChannelCmd = ENABLE;							//Enable
//	NVIC_Init(&NVIC_config_s);




	
}

/**
  * @brief 	Initializing Gyroscope Sensor
	* @param 	None
  * @retval None
  */	


void init_gyro(){

	Ext_Sensor_Struct gyro_struct;
	gyro_struct.Power_Mode_Output_DataRate = LSM9DS1_DATARATE_238_G;
	gyro_struct.Axes_Enable = LSM9DS1_X_ENABLE_G | LSM9DS1_Y_ENABLE_G | LSM9DS1_Z_ENABLE_G;
	gyro_struct.Full_Scale = LSM9DS1_FULLSCALE_245_G;
	gyro_struct.AA_Filter_BW = LSM9DS1_AA_BW_G_00;
	LSM9DS1_Gyro_Init(&gyro_struct);


	/*Configure Gyro interrupts */
	EXT_DRY_INT gyro_int;
//	mems_int_config_s.XL_Dataready_Interrupt = 0X00;//LSM9DS1_INT1_DRDY_XL_ENABLE; 
	gyro_int.Gyro_Int = LSM9DS1_INT1_DRDY_G_ENABLE;
	LSM9DS1_INT_Config(&gyro_int);

	/*Activating clocks*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); /* Activate SYSCFG in order to link PE0 to EXTI0 
																														Check Figure 27 in Reference Manual*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	 /* Activate E in order to use pin PE0, PE1, PE3 
																														used by MEMS */

	/*Configure PE1 used by Gyro output of MEMS (check Schematics)*/
	GPIO_InitTypeDef GPIO_config_s;
	GPIO_config_s.GPIO_Mode = GPIO_Mode_IN;
	GPIO_config_s.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_config_s.GPIO_Pin = GPIO_Pin_1;
	GPIO_config_s.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_config_s.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOE, &GPIO_config_s);


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
	tim_nvic.NVIC_IRQChannelPreemptionPriority = 0x00; //Between 0-15 for priority level, lower number = higher priority: set to highest
	tim_nvic.NVIC_IRQChannelSubPriority = 0x01; //Between 0-15 for next priority level, lower number = higher priority: set to highest 
	
	NVIC_Init(&tim_nvic); //Initialize interrupt
	
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //This line essentially links the timer and interrupt 
	
	TIM_Cmd(TIM4, ENABLE); //Starts the timer



//	/*Configure EXTI1 to listen to interrupts */
//	EXTI_InitTypeDef EXTI_config_s;
//	EXTI_config_s.EXTI_Line = EXTI_Line1;
//	EXTI_config_s.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_config_s.EXTI_Trigger = EXTI_Trigger_Rising;  
//	EXTI_config_s.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_config_s);

//	/*Link PE1 to EXTI1*/ 
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource1);

//	/*Enable interrupt control and set priority of EXTI0*/
//	NVIC_InitTypeDef   NVIC_config_s;
//	NVIC_config_s.NVIC_IRQChannel = EXTI1_IRQn;							//Channel
//	NVIC_config_s.NVIC_IRQChannelPreemptionPriority = 0x00; //Set x higher than y
//	NVIC_config_s.NVIC_IRQChannelSubPriority = 0x01;				//Set x1 higher the x2
//	NVIC_config_s.NVIC_IRQChannelCmd = ENABLE;							//Enable
//	NVIC_Init(&NVIC_config_s);

}


