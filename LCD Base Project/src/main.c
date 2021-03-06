/*
Final Project
Group 10
MicroProcessor Systems ECSE 426
Razi Murshed	
Danial Almani
Hannan Aslam
Oruj Oliver Ahmadov
Imranul Islam
*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_l3gd20.h"
#include "cc2500.h"
#include <stdio.h>
#include <string.h>

/*----------------------------------------------------------------------------
 * CONSTANTS
 *---------------------------------------------------------------------------*/
#define RX_PKT 0x01

/*----------------------------------------------------------------------------
 * GLOBAL VARIABLES
 *---------------------------------------------------------------------------*/
float received[2] = {0, 0} ;
float read[30];
float a;
uint8_t read2[5][5];
uint8_t plot[2];
uint8_t status;
uint8_t temp, test; 

/*----------------------------------------------------------------------------
 * THREAD ID Decleration
 *---------------------------------------------------------------------------*/
osThreadId	Rx_thread;
osThreadId  Tx_thread;

/*----------------------------------------------------------------------------
 * FUNCTIONS PROTOTYPE
 *---------------------------------------------------------------------------*/
void wireless_init(void);


/**
* @brief  Set Grid
* @param  void
* @retval Null
*/
void setGrid(){
    LCD_SetFont(&Font8x8);
	  LCD_SetTextColor(LCD_COLOR_BLACK);
		int i, j;
	  //Draws horizontal lines
    for(i =20; i <= 300; i +=10)
    {		
	   LCD_DrawLine(20, i, 200, LCD_DIR_HORIZONTAL);
		}
		//Draws vertical lines
		for(j = 20; j <= 220; j +=10)
    {		
	   LCD_DrawLine(j, 20, 280, LCD_DIR_VERTICAL);
		}
		LCD_DisplayStringLine(310, (uint8_t*)"         Y DISPLACEMENT     ");
		LCD_DisplayChar(90, 10, (uint8_t)'X');
    LCD_DisplayChar(110, 10, (uint8_t)'D');
		LCD_DisplayChar(120, 10, (uint8_t)'I');
		LCD_DisplayChar(130, 10, (uint8_t)'S');
		LCD_DisplayChar(140, 10, (uint8_t)'P');
		LCD_DisplayChar(150, 10, (uint8_t)'L');
		LCD_DisplayChar(160, 10, (uint8_t)'A');
		LCD_DisplayChar(170, 10, (uint8_t)'C');
		LCD_DisplayChar(180, 10, (uint8_t)'E');
		LCD_DisplayChar(190, 10, (uint8_t)'M');
		LCD_DisplayChar(200, 10, (uint8_t)'E');
		LCD_DisplayChar(210, 10, (uint8_t)'N');
		LCD_DisplayChar(220, 10, (uint8_t)'T');
		LCD_SetFont(&Font12x12);
		LCD_SetTextColor(LCD_COLOR_RED);
}


/**
* @brief  Display LCD
* @param  Pre-Set Argument
* @retval Null
*/
void DisplayLCD(void const *argument){
	char* s;
	int cnt, i = 0;
	  LCD_Clear(LCD_COLOR_WHITE);
	  LCD_SetFont(&Font8x8);
	  LCD_SetTextColor(LCD_COLOR_BLACK);
	  LCD_DisplayStringLine(1, (uint8_t*)"   NOW DISPLAYING TRAJECTORY     ");
	  LCD_DisplayStringLine(310, (uint8_t*)"         Y DISPLACEMENT     ");
		LCD_DisplayChar(90, 10, (uint8_t)'X');
    LCD_DisplayChar(110, 10, (uint8_t)'D');
		LCD_DisplayChar(120, 10, (uint8_t)'I');
		LCD_DisplayChar(130, 10, (uint8_t)'S');
		LCD_DisplayChar(140, 10, (uint8_t)'P');
		LCD_DisplayChar(150, 10, (uint8_t)'L');
		LCD_DisplayChar(160, 10, (uint8_t)'A');
		LCD_DisplayChar(170, 10, (uint8_t)'C');
		LCD_DisplayChar(180, 10, (uint8_t)'E');
		LCD_DisplayChar(190, 10, (uint8_t)'M');
		LCD_DisplayChar(200, 10, (uint8_t)'E');
		LCD_DisplayChar(210, 10, (uint8_t)'N');
		LCD_DisplayChar(220, 10, (uint8_t)'T');
     //setGrid();
	while(1){
		if((cnt > 1)&&(received[0] < 255)&&(received[1] < 255))
		{
			LCD_DrawUniLine((uint16_t)(120+received[0]*5), (uint16_t)(160-received[1]*5), plot[0], plot[1]);
		}
		plot[0] = (uint16_t)(120+received[0]*5); //0.862745
		plot[1] = (uint16_t)(160-received[1]*5);  //1.176
		cnt++;
		
		osDelay(200);
	}
}

/**
* @brief  Receive packets
* @param  Pre-Set Argument
* @retval Null
*/
void RxPacket(void const *argument){
	uint8_t mode_filter, transmit_mode;
   //uint8_t buf;
	uint8_t buf[2];
   GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
   mode_filter = 0x70;
   transmit_mode = 0x20;
	printf("Thread_started. waiting for signal\n");
   // put reciever in RX mode
	CC2500_Strobe(CC2500_STROBE_SRX, 0x00); 
	while(1){
		int i = 0;
		status = CC2500_Strobe(CC2500_STROBE_SRX, 0x00);
		CC2500_Read(&temp, CC2500_STATUS_REG_RXBYTES , 2);
		CC2500_Read(&test, CC2500_STATUS_REG_PKTSTATUS , 2);
		//Read only if correct packet length
		if(temp == CC2500_SETTING_PKTLEN)
		{
	  	status = CC2500_Strobe(CC2500_STROBE_SRX, 0x00);
		  CC2500_RxPackets((uint8_t*)received, CC2500_SETTING_PKTLEN ); //Store in float array
		}
		osDelay(100);
	}
}

/*----------------------------------------------------------------------------
 * THREAD DEFINITIONS
 *---------------------------------------------------------------------------*/
osThreadDef(RxPacket, osPriorityNormal, 1, 0);
osThreadDef(DisplayLCD, osPriorityNormal, 1, 0);
osThreadId DisplayLCD_thread;

/**
* @brief  Main Function
* @param  
* @retval int 0
*/
int main (void) {

  osKernelInitialize ();                    // initialize CMSIS-RTOS
  wireless_init();  //Initialize wireless
  LCD_Init();  //Initialize LCD
  LCD_LayerInit(); //Initialize Layer  
  /* Enable the LTDC controler */
  LTDC_Cmd(ENABLE);
  /* Set LCD foreground layer as the current layer */
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
	DisplayLCD_thread = osThreadCreate(osThread(DisplayLCD), NULL);
  Rx_thread = osThreadCreate(osThread(RxPacket), NULL);
	osKernelStart();
}

/**
* @brief  Initialize wireless 
* @param  
* @retval int 0
*/
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
* @brief  EXTI4 Interrupt Handler
* @param  No input
* @retval No return
*/
void EXTI4_IRQHandler(void) {
	//When the interrupt is raised, send a signal to the wireless thread
	if(EXTI_GetITStatus(CC2500_SPI_INT_EXTI_LINE) != RESET){
			osSignalSet(Rx_thread, RX_PKT);
		}	
	//Clear EXTI inruppt pending bit.
	EXTI_ClearITPendingBit(CC2500_SPI_INT_EXTI_LINE);
}