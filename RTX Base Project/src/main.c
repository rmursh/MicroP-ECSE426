/*

ECSE 426
Final Project

Razi Murshed 	
Hannan Aslam
Danial Saleem Almani		2604035455
Imranul Islam						
Oruj Ahmed	

*/

/*
	******************************************************************************
	* Constants
	******************************************************************************
*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  		// Device header
#include "stm32f4xx_conf.h"
#include <stdio.h>
#include "cc2500.h"
#include "math.h"
#include "lsm9ds1.h"
#include "Ext_Sensor.h"
#include "acc_thread.h"
#include "math.h"

extern float coordinates[30];

float a =0;
float send[2] ={0,0};
float test[30] = {-73,
-47,
71,
71,
-6,
-46,
-57,
97,
92,
-72,
92,
-30,
-80,
70,
25,
60,
-67,
60,
-11,
-62,
-64,
-1,
-91,
-75,
83,
70,
-70,
26,
31,
-92}; 
#define RX_PKT 0x01


float buf[2] = {0,0};

void wireless_init(void);

typedef struct packet {
   uint8_t sign[50];
   //uint8_t coordinates[50];
} Packet;

// ID for thread
osThreadId	Rx_thread;
osThreadId  Tx_thread;
osThreadId runAcc_thread;
osThreadId runGyro_thread;

// ID for mutex
osMutexDef(mutex);

/**
  * @brief 	Blinky Function (used to test the threads)
	* @param 	None
  * @retval None
  */

void Blinky_GPIO_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
}


/**
  * @brief  
	* @param 
  * @retval None
  */

void TxPacket(void const *argument){
	uint8_t mode_filter, transmit_mode;
   
	uint8_t a =0;
	uint8_t b  = 0;
	Packet pkt;
	float pitch =0;
	float roll = -60;
	//uint8_t buf[2];
   GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
   mode_filter = 0x70;
   transmit_mode = 0x20;
	
   // put reciever in RX mode

  
	//osSignalWait(RX_PKT, osWaitForever);
	//while(1){
		int i =0;
		osSignalWait(RX_PKT, osWaitForever);
		//GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
		while(i < 30)
		{
			int j;
			send[0] = coordinates[i];
			send[1] = coordinates[i+1];
			for(j = 0 ;  j < 5; j++)
			{
		   CC2500_TxPacket((uint8_t*)send, CC2500_SETTING_PKTLEN);
				osDelay(200);
			}
			
			i = i+2;
			osDelay(1000);
		}
		
//	while(1){
//		int i;
//		//osSignalWait(RX_PKT, osWaitForever);
//		roll++;
//		pitch = 0.025*pow(roll,2);
//      //turn on LED on packet RX
//		 buf[0] = roll ;
//	   buf[1] = pitch ;
//		//GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
//    CC2500_TxPacket((uint8_t*)buf, CC2500_SETTING_PKTLEN);
//		//CC2500_Read(&var, CC2500_STATUS_REG_TXBYTES, 2 );
//    //CC2500_Strobe(CC2500_STROBE_SRX, 0x00);
//		osDelay(1000);
//	  //CC2500_TxPacket(test2, CC2500_SETTING_PKTLEN);
//		//printf("TxBytes is %d , %d\n", var, sizeof(abc));
//		osSignalClear(Rx_thread, RX_PKT);

//	}
		//CC2500_TXData(angles);
		
		//CC2500_RxPackets((uint8_t*)&pkt, CC2500_SETTING_PKTLEN + 2);
		
	}
//}
	
/**
  * @brief  Configuration of Blue Push Button (used for start of transmition)
	* @param 
  * @retval None
  */

void EXTI_NVIC_Config_PushButton(void)
{
	GPIO_InitTypeDef GPIO_MEM_Init_struct;
	EXTI_InitTypeDef EXTI_MEM_struct;
	NVIC_InitTypeDef NVIC_MEM_struct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	// Enable SYSCFG clock 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	GPIO_StructInit(&GPIO_MEM_Init_struct);
	
	GPIO_MEM_Init_struct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_MEM_Init_struct.GPIO_Pin = GPIO_Pin_0;
	GPIO_MEM_Init_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
	GPIO_Init(GPIOA, &GPIO_MEM_Init_struct);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	
	EXTI_MEM_struct.EXTI_Line = EXTI_Line0;
  EXTI_MEM_struct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_MEM_struct.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_MEM_struct.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_MEM_struct);

  /* Enable and set EXTI Line0 Interrupt */ 
  NVIC_MEM_struct.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_MEM_struct.NVIC_IRQChannelPreemptionPriority = 0x01;
  NVIC_MEM_struct.NVIC_IRQChannelSubPriority = 0x03; // set priority to 2, accelerometer data ready interrupts gets priority of 1
  NVIC_MEM_struct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_MEM_struct);
	
	return;

}

/**
  * @brief  Interrupt Handler for the Blue Push Button
	* @param 	None
  * @retval None
  */

void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
		osSignalSet(Rx_thread, RX_PKT); 
  }
	EXTI_ClearITPendingBit(EXTI_Line0);
}

osThreadDef(TxPacket, osPriorityNormal, 1, 0);
osThreadDef(accel_thread, osPriorityNormal, 1, 0);
osThreadDef(gyro_thread, osPriorityNormal, 1, 0);

/**
  * @brief  
	* @param 
  * @retval None
  */


int main (void) {
  //uint8_t buf, reg;
  osKernelInitialize ();                    // initialize CMSIS-RTOS
  init_acc();
	init_gyro();
	EXTI_NVIC_Config_PushButton();
  // initialize peripherals here
  Blinky_GPIO_Init();
  wireless_init();
  mutex = osMutexCreate(osMutex(mutex));
  Rx_thread = osThreadCreate(osThread(TxPacket), NULL);
	runAcc_thread = osThreadCreate(osThread(accel_thread), NULL);
	runGyro_thread = osThreadCreate(osThread(gyro_thread), NULL);
	osKernelStart();
}


/**
  * @brief  Wirless transmitter CC2500 initialization
	* @param  None 
  * @retval None
  */

void wireless_init() {
	CC2500_Init();
	
	GPIO_InitTypeDef GPIO_init_st;
	NVIC_InitTypeDef NVIC_init_st;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	/* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//Configure GPIO pin 0 in input mode
	GPIO_init_st.GPIO_Mode = GPIO_Mode_IN;
	GPIO_init_st.GPIO_Pin = CC2500_SPI_INT_PIN;
	GPIO_init_st.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_init_st.GPIO_Speed = GPIO_High_Speed;
	GPIO_init_st.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(CC2500_SPI_INT_GPIO_PORT, &GPIO_init_st);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, CC2500_SPI_INT_EXTI_PIN_SOURCE);
	EXTI_InitStructure.EXTI_Line = CC2500_SPI_INT_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	// configure NVIC
	NVIC_init_st.NVIC_IRQChannel = CC2500_SPI_INT_EXTI_IRQn;
	NVIC_init_st.NVIC_IRQChannelCmd = ENABLE;
	NVIC_init_st.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_init_st.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_Init(&NVIC_init_st);
	
	NVIC_EnableIRQ(CC2500_SPI_INT_EXTI_IRQn);
}

/**
  * @brief  Interrupt Service Routine for EXTI12_IRQHandler
  */
void EXTI15_10_IRQHandler(void) {
	if(EXTI_GetITStatus(EXTI_Line12) != RESET)
    {
			osSignalSet(Rx_thread, RX_PKT);
		}	
	// clear EXTI inruppt pending bit.
	EXTI_ClearITPendingBit(EXTI_Line12);
}

