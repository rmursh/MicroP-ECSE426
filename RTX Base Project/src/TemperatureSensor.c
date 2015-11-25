#include "TemperatureSensor.h"
#include <stdio.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "stm32f4xx_adc.h"



	/********************************************//**
 * Using GPIO to create standard inpur output
 ***********************************************/

GPIO_InitTypeDef  GPIO_InitStruct;

mAState ma;

ADC_InitTypeDef ADC_InitStruct;//Using GPIO to create standard input output



ADC_CommonInitTypeDef ADC_CommonInitStruct;
//static float temp_t;
static float Temperature;


void setupTemperatureSensor(void){
	    
	    gpioSetup();	
	    adcSetup();
	    //initializeMovingAverage(&ma);
    int init_buffer = 0;
		ma.newIndex = 0;
		ma.buffer_length = 0;
		while(init_buffer < DEPTH_D)
		{
			ma.filterBuffer[init_buffer] = 0;
			init_buffer++;
		}
	   
}

float runTemperatureSensor(float TemperatureValue, int i){
   

			// Temperature (in °C) = ((Vsense – V25) / Avg_Slope) + 25
			ADC_SoftwareStartConv(ADC1); //Start the conversion
			while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //Processing the conversion
			float temp, Vsense;
			temp = ADC_GetConversionValue(ADC1);//ADC_GetConversionValue(ADC1); //Return the converted data
	    Vsense = (3.0*temp)/ 4096;
			TemperatureValue = digitalValueInCelsius(Vsense);		
			TemperatureValue += 0;
			Temperature = TemperatureValue;
			return TemperatureValue;


}

 float runTemperatureSensorMoving(){
   
     
			// Temperature (in °C) = ((Vsense – V25) / Avg_Slope) + 25
			ADC_SoftwareStartConv(ADC1); //Start the conversion
			while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); //Processing the conversion
			float temp, Vsense;
	    temp = maFilter(&ma,ADC_GetConversionValue(ADC1));
	    Vsense = (3.0*temp)/ 4096;
			Temperature =  digitalValueInCelsius(Vsense);//Return the converted data
			//TemperatureValue = temp;
			//TemperatureValue += 0;
			//Temperature = TemperatureValue;
			return Temperature;


}
 



void gpioSetup(void){
 	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12;
	    
	  //GPIO_InitStruct.GPIO_Mode configures the pin mode the options are as follows
    // GPIO_Mode_IN (Input Mode)
    // GPIO_Mode_OUT (Output Mode)
    // GPIO_Mode_AF (Alternate Function)
    // GPIO_Mode_AN (Analog Mode)
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	
	  //GPIO_InitStruct.GPIO_Speed configures the clock speed, options are as follows
    // GPIO_Speed_2MHz
    // GPIO_Speed_25MHz
    // GPIO_Speed_50MHz
    // GPIO_Speed_100MHz
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	  //GPIO_InitStruct.GPIO_OType configures the pin type, options are as follows
    // GPIO_OType_PP (Push/Pull)
    // GPIO_OType_OD (Open Drain)
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
		
		//Configures pullup / pulldown resistors on pin, options are as follows
    // GPIO_PuPd_NOPULL (Disables internal pullup and pulldown resistors)
    // GPIO_PuPd_UP (Enables internal pullup resistors)
    // GPIO_PuPd_DOWN (Enables internal pulldown resistors)
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
		
		//This finally passes all the values to the GPIO_Init function
    //which takes care of setting the corresponding bits.
    GPIO_Init(GPIOD, &GPIO_InitStruct);
		
    //enables the peripheral clock to the GPIOA module.  

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

   //Defines Properties of GPIOA

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void adcSetup(void){
		
	   ADC_DeInit();

		 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

		 ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
		 ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div8;
		 ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
		 ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
		 ADC_CommonInit(&ADC_CommonInitStruct);

		 ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
		 ADC_InitStruct.ADC_ScanConvMode = DISABLE;
		 ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
		 ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
		 ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
		 ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
		 ADC_InitStruct.ADC_NbrOfConversion = 1;
		 ADC_Init(ADC1, &ADC_InitStruct);

			// ADC1 Configuration, ADC_Channel_TempSensor is actual channel 16
		 ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 1, ADC_SampleTime_144Cycles);

			// Enable internal temperature sensor
		 ADC_TempSensorVrefintCmd(ENABLE);

			// Enable ADC conversion
		 ADC_Cmd(ADC1, ENABLE);
}



float digitalValueInCelsius(float reading){
	float temp_t;
	temp_t = (((reading - 0.76f)/0.0025f) + 25.0f); //0.00073260073
	return temp_t;
	// ((Vin-V25)/avg_slope)+25
	
}

