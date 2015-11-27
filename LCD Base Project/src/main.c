/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
//#include "cmsis_os.h"


#include "stm32f4xx.h"                  // Device header

#include "stm32f4xx_conf.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_l3gd20.h"
//#include "background16bpp.h"
#include "cc2500.h"

#include <stdio.h>
#include <string.h>

//#include "LCDInterface.h"

#define RX_PKT 0x01

uint8_t read[2];
uint8_t status;

uint8_t temp; 



// ID for thread
osThreadId	Rx_thread;
osThreadId  Tx_thread;

void wireless_init(void);

typedef struct packet {
   uint8_t Src_addr;
   uint8_t Seq_num;
   uint8_t Aux_rssi;
   uint8_t Rssi;
   uint8_t Crc_id;
} Packet;


	
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

void RxPacket(void const *argument){
	uint8_t mode_filter, transmit_mode;
   //uint8_t buf;
	uint8_t buf[2];
	Packet pkt;
   GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
   mode_filter = 0x70;
   transmit_mode = 0x20;
	printf("Thread_started. waiting for signal\n");
   // put reciever in RX mode
	CC2500_Strobe(CC2500_STROBE_SRX, 0x00);
	
   
	while(1){
		int i;
		//osSignalWait(RX_PKT, osWaitForever);
		

		status = CC2500_Strobe(CC2500_STROBE_SRX, 0x00);
		CC2500_Read(&temp, CC2500_STATUS_REG_RXBYTES , 2);
		if(temp==2)
		{
	  	status = CC2500_Strobe(CC2500_STROBE_SRX, 0x00);
		  CC2500_RxPackets(read, 2);
		}
		osDelay(100);
		printf("Read is %d %d \n", read[0], read[1]);

    

	}
}

osThreadDef(RxPacket, osPriorityNormal, 1, 0);

/*
 * main: initialize and start the system
 */
int main (void) {

  osKernelInitialize ();                    // initialize CMSIS-RTOS
  wireless_init();
  Rx_thread = osThreadCreate(osThread(RxPacket), NULL);
	osKernelStart();
}

void wireless_init() {
	
	
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
	
	
	
	EXTI_GenerateSWInterrupt(EXTI_Line4);
	CC2500_Init();
}

/**
  * @brief  Interrupt Service Routine for EXTI12_IRQHandler
  */
void EXTI4_IRQHandler(void) {
	//When the interrupt is raised, send a signal to the wireless thread
	if(EXTI_GetITStatus(CC2500_SPI_INT_EXTI_LINE) != RESET){
			osSignalSet(Rx_thread, RX_PKT);
		}	
	//Clear EXTI inruppt pending bit.
	EXTI_ClearITPendingBit(CC2500_SPI_INT_EXTI_LINE);
}