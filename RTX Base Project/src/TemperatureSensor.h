#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "MovingAverageFilter.h"



void gpioSetup(void);
void adcSetup(void);
float runTemperatureSensor(float TemperatureValue, int i);
float runTemperatureSensorMoving();
void setupTemperatureSensor(void);
float digitalValueInCelsius(float reading);
void testFilter(void);


