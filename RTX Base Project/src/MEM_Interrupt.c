 /*******************************************************************
  * @file    MEM_Interrupt.c
  * @author  Razi Murshed
  * @date    21/10/2015
  * @brief   Configuration of MEM and EXTI and NVIC
  * 
	********************************************************************/
	
#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_exti.h"
#include "lis3dsh.h"
#include "misc.h"
#include "MEM_Interrupt.h"

/*
 * Function: EXTI_NVIC_Config
 * ----------------------------
 * Configure EXTI and NVIC pin PE0 line0
 *   Returns: void
 *
 *   @param: none
 *   returns: none
 */
void EXTI_NVIC_Config(void)
{
	GPIO_InitTypeDef GPIO_MEM_Init_struct;
	EXTI_InitTypeDef EXTI_MEM_struct;
	NVIC_InitTypeDef NVIC_MEM_struct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	// Enable SYSCFG clock 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	GPIO_StructInit(&GPIO_MEM_Init_struct);
	
	GPIO_MEM_Init_struct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_MEM_Init_struct.GPIO_Pin = GPIO_Pin_0;
	GPIO_MEM_Init_struct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
	GPIO_Init(GPIOE, &GPIO_MEM_Init_struct);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);
	
	EXTI_MEM_struct.EXTI_Line = EXTI_Line0;
  EXTI_MEM_struct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_MEM_struct.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_MEM_struct.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_MEM_struct);
	

  /* Enable and set EXTI Line0 Interrupt */ 
  NVIC_MEM_struct.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_MEM_struct.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_MEM_struct.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_MEM_struct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_MEM_struct);
	
	return;

}

/*
 * Function: MEM_Interrupt_Config
 * ----------------------------
 * Configure Interrupt for MEM
 *   Returns: void
 *
 *   @param: none
 *   returns: none
 */
void MEM_Interrupt_Config(void)
{ 
  LIS3DSH_DRYInterruptConfigTypeDef LIS3DSH_InterruptStruct;    
  /* Set configuration of Internal MEM Interrupt of LIS3DSH*/
  LIS3DSH_InterruptStruct.Dataready_Interrupt = LIS3DSH_DATA_READY_INTERRUPT_ENABLED;
  LIS3DSH_InterruptStruct.Interrupt_signal = LIS3DSH_ACTIVE_HIGH_INTERRUPT_SIGNAL;
  LIS3DSH_InterruptStruct.Interrupt_type = LIS3DSH_INTERRUPT_REQUEST_PULSED;
  LIS3DSH_DataReadyInterruptConfig(&LIS3DSH_InterruptStruct);
  /* Configure Interrupt control register: enable Data Ready interrupt1 */
  uint8_t ctrl = 0;
	ctrl = 0x4;
	LIS3DSH_Write(&ctrl, LIS3DSH_CTRL_REG3, 1);
	return;
}



 
