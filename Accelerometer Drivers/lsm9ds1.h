 #include "stm32f4xx.h"
 #include "stm32f4xx_gpio.h"
 #include "stm32f4xx_rcc.h"
 #include "stm32f4xx_spi.h"

void LSM9DS1_Write(uint8_t* pBuffer, uint8_t WriteAddr);
static uint8_t LSM9DS1_SendByte(uint8_t byte);
uint32_t LSM9DS1_TIMEOUT_UserCallback(void);
void LSM9DS1_Read(uint8_t* pBuffer, uint8_t ReadAddr);
static void LSM9DS1_LowLevel_Init(void);


typedef struct
{
  uint8_t Power_Mode_Output_DataRate;         /* Ppower down or /active mode with output data rate 3.125 / 6.25 / 12.5 / 25 / 50 / 100 / 400 / 800 / 1600 HZ */
  uint8_t Axes_Enable;                        /* Axes enable */
  uint8_t Continous_Update;					 				  /* Block or update Low/High registers of data until all data is read */
	uint8_t AA_Filter_BW;												/* Choose anti-aliasing filter BW 800 / 400 / 200 / 50 Hz*/
  uint8_t Full_Scale;                         /* Full scale 2 / 4 / 6 / 8 / 16 g (Sensitivity) */
  uint8_t BW_Selection;                       /* Band Width selection*/
}	LSM9DS1_InitTypeDef;


void LSM9DS1_Init(LSM9DS1_InitTypeDef *LSM9DS1_InitStruct);
void LSM9DS1_ReadACC(float* out);

/**
  * @brief  LSM9DS1 SPI Interface pins
  */
#define LSM9DS1_SPI                       SPI1
#define LSM9DS1_SPI_CLK                   RCC_APB2Periph_SPI1

#define LSM9DS1_SPI_SCK_PIN               GPIO_Pin_5                  /* PA.05 */
#define LSM9DS1_SPI_SCK_GPIO_PORT         GPIOA                       /* GPIOA */
#define LSM9DS1_SPI_SCK_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define LSM9DS1_SPI_SCK_SOURCE            GPIO_PinSource5
#define LSM9DS1_SPI_SCK_AF                GPIO_AF_SPI1

#define LSM9DS1_SPI_MISO_PIN              GPIO_Pin_6                  /* PA.6 */
#define LSM9DS1_SPI_MISO_GPIO_PORT        GPIOA                       /* GPIOA */
#define LSM9DS1_SPI_MISO_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define LSM9DS1_SPI_MISO_SOURCE           GPIO_PinSource6
#define LSM9DS1_SPI_MISO_AF               GPIO_AF_SPI1

#define LSM9DS1_SPI_MOSI_PIN              GPIO_Pin_7                  /* PA.7 */
#define LSM9DS1_SPI_MOSI_GPIO_PORT        GPIOA                       /* GPIOA */
#define LSM9DS1_SPI_MOSI_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define LSM9DS1_SPI_MOSI_SOURCE           GPIO_PinSource7
#define LSM9DS1_SPI_MOSI_AF               GPIO_AF_SPI1

#define LSM9DS1_SPI_CS_PIN                GPIO_Pin_4                  /* PE.03 */
#define LSM9DS1_SPI_CS_GPIO_PORT          GPIOE                       /* GPIOE */
#define LSM9DS1_SPI_CS_GPIO_CLK           RCC_AHB1Periph_GPIOE

#define LSM9DS1_SPI_INT1_PIN              GPIO_Pin_0                  /* PE.00 */
#define LSM9DS1_SPI_INT1_GPIO_PORT        GPIOE                       /* GPIOE */
#define LSM9DS1_SPI_INT1_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define LSM9DS1_SPI_INT1_EXTI_LINE        EXTI_Line0
#define LSM9DS1_SPI_INT1_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define LSM9DS1_SPI_INT1_EXTI_PIN_SOURCE  EXTI_PinSource0
#define LSM9DS1_SPI_INT1_EXTI_IRQn        EXTI0_IRQn 

#define LSM9DS1_SPI_INT2_PIN              GPIO_Pin_1                  /* PE.01 */
#define LSM9DS1_SPI_INT2_GPIO_PORT        GPIOE                       /* GPIOE */
#define LSM9DS1_SPI_INT2_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define LSM9DS1_SPI_INT2_EXTI_LINE        EXTI_Line1
#define LSM9DS1_SPI_INT2_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define LSM9DS1_SPI_INT2_EXTI_PIN_SOURCE  EXTI_PinSource1
#define LSM9DS1_SPI_INT2_EXTI_IRQn        EXTI1_IRQn 




/******************************************************************************/
/*************************** START REGISTER MAPPING  **************************/
/******************************************************************************/

#define 	CTRL_REG6_XL	0x20			// ODR,FULLSCALE,BW

#define		CTRL_REG8 		0x22			// Continuous update

#define		CTRL_REG5_XL  0x1F			// Axes enable

#define		INT1_CTRL 		0x0C			//interrupt 1

#define 	INT_GEN_CFG_XL 0x06
/*******************************************************************************
*  Output registers: X, Y, Z axes 8 bit MSB/LSB registers (combined 16 bits result)
*  Read only register
*  Default value: 0x00 corresponds to 0g acceleration
*******************************************************************************/
#define LSM9DS1_OUT_X_L       	 	 0x28
#define LSM9DS1_OUT_X_H       	 	 0x29
#define LSM9DS1_OUT_Y_L       	 	 0x2A
#define LSM9DS1_OUT_Y_H       	 	 0x2B
#define LSM9DS1_OUT_Z_L       	 	 0x2C
#define LSM9DS1_OUT_Z_H       	 	 0x2D


/******************************************************************************/
/****************************** START BIT MAPPING  ****************************/
/******************************************************************************/



/** @defgroup Data_Rate_selection                 
  * @{
  */
	
#define LSM9DS1_PWRDWN								((uint8_t)0x00)
#define LSM9DS1_DATARATE_10						((uint8_t)0x20)
#define LSM9DS1_DATARATE_50						((uint8_t)0x40)
#define LSM9DS1_DATARATE_119					((uint8_t)0x60)
#define LSM9DS1_DATARATE_238					((uint8_t)0x80)
#define LSM9DS1_DATARATE_476					((uint8_t)0xA0)
#define LSM9DS1_DATARATE_952					((uint8_t)0xC0)


/** @defgroup Full_Scale_selection 
  * @{
  */
#define LSM9DS1_FULLSCALE_2						((uint8_t)0x00)
#define LSM9DS1_FULLSCALE_16					((uint8_t)0x08)
#define LSM9DS1_FULLSCALE_4						((uint8_t)0x10)
#define LSM9DS1_FULLSCALE_8						((uint8_t)0x18)

/** @defgroup BW selection 
  * @{
  */

#define LSM9DS1_BW_SCALE_ODR_DEFAULT						((uint8_t)0x00) 
#define LSM9DS1_BW_SCALE_ODR_BW_XL							((uint8_t)0x04) // We will choose this


/** @defgroup Antialiasing_Filter_BW 
  * @{
  */
#define LSM9DS1_AA_BW_408						((uint8_t)0x00)
#define LSM9DS1_AA_BW_211						((uint8_t)0x01)
#define LSM9DS1_AA_BW_105						((uint8_t)0x02)
#define LSM9DS1_AA_BW_50						((uint8_t)0x03) // We will choose this



/** @defgroup Continous_update
  * @{
  */
	
#define LSM9DS1_Continous_update_default		((uint8_t)0x00) // we will chose this
#define LSM9DS1_Continous_update						((uint8_t)0x40)


/** @defgroup Direction_XYZ_selection 
  * @{
  */
#define LSM9DS1_X_ENABLE            ((uint8_t)0x08)
#define LSM9DS1_Y_ENABLE            ((uint8_t)0x10)
#define LSM9DS1_Z_ENABLE            ((uint8_t)0x20)


	 /** @defgroup Sensitivity 
  * @{
  */
 #define LSM9DS1_SENSITIVITY_2G    	0.061 		      
 #define LSM9DS1_SENSITIVITY_4G		  0.122			 
 #define LSM9DS1_SENSITIVITY_8G		  0.244	   	 
 #define LSM9DS1_SENSITIVITY_16G    0.732   