#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include <stdio.h>
#include "cc2500.h"
#include "math.h"

uint8_t abc[5][5] ={
	          {16,17,18,19,3},
            {0,1,2,3,4},
            {4,5,6,7,8},
            {12,13,14,15,7},
						{8,9,10,11,6}
            };
uint8_t test[10] = {16,17,18,19,3,4,7,7,9,4};
uint8_t test2[10] =  {8,9,10,11,6,6,7,12,4,56};
float received[2] = {0, 0};
#define RX_PKT 0x01

uint8_t var;
angle_data angles;
uint8_t buf[2] = {0,0};

void wireless_init(void);

typedef struct packet {
   uint8_t Src_addr;
   uint8_t Seq_num;
   uint8_t Aux_rssi;
   uint8_t Rssi;
   uint8_t Crc_id;
} Packet;

// ID for thread
osThreadId	Rx_thread;
osThreadId  Tx_thread;
	
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

void TxPacket(void const *argument){
	uint8_t mode_filter, transmit_mode;
   //uint8_t buf;
	
	Packet pkt;
   GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
   mode_filter = 0x70;
   transmit_mode = 0x20;
	printf("Thread_started. waiting for signal\n");
   // put reciever in RX mode
	angles.pitch =0;
	angles.roll = 0;
  
	osSignalWait(RX_PKT, osWaitForever);
	while(1){
		int i;
		//osSignalWait(RX_PKT, osWaitForever);
		angles.roll++;
		angles.pitch = (uint8_t)20*sin(angles.roll/PI)+20;
      //turn on LED on packet RX
		 buf[0] = angles.roll ;
	   buf[1] = angles.pitch ;
		//GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
    CC2500_TxPacket((uint8_t*)abc, CC2500_SETTING_PKTLEN);
		CC2500_Read(&var, CC2500_STATUS_REG_TXBYTES, 2 );
    //CC2500_Strobe(CC2500_STROBE_SRX, 0x00);
		osDelay(1000);
	  //CC2500_TxPacket(test2, CC2500_SETTING_PKTLEN);
		printf("TxBytes is %d , %d\n", var, sizeof(abc));
		osSignalClear(Rx_thread, RX_PKT);

	}
}


osThreadDef(TxPacket, osPriorityNormal, 1, 0);

/*
 * main: initialize and start the system
 */
int main (void) {
  //uint8_t buf, reg;
  osKernelInitialize ();                    // initialize CMSIS-RTOS
  EXTI_NVIC_Config_PushButton();
  // initialize peripherals here
  Blinky_GPIO_Init();
  wireless_init();

  Rx_thread = osThreadCreate(osThread(TxPacket), NULL);
	osKernelStart();
}

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

void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
		osSignalSet(Rx_thread, RX_PKT); 
  }
	EXTI_ClearITPendingBit(EXTI_Line0);
}

/**
  * @brief  Interrupt Service Routine for EXTI12_IRQHandler
  */
//void EXTI15_10_IRQHandler(void) {
//	if(EXTI_GetITStatus(EXTI_Line12) != RESET)
//    {
//			osSignalSet(Rx_thread, RX_PKT);
//		}	
//	// clear EXTI inruppt pending bit.
//	EXTI_ClearITPendingBit(EXTI_Line12);
//}