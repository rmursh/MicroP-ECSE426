
/*
	******************************************************************************
	* Headers
	******************************************************************************
*/


#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include <stdio.h>
#include "lsm9ds1.h"

/*
	******************************************************************************
	* Function Prototypes
	******************************************************************************
*/

void init_acc();
void init_gyro();