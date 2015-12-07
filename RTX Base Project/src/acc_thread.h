

/*
	******************************************************************************
	* Headers
	******************************************************************************
*/

#include "osObjects.h"


/*
	******************************************************************************
	* Function Prototypes
	******************************************************************************
*/


void accel_thread(void const *argument);
void gyro_thread(void const *argument);
void EXTI2_IRQHandler ();
void EXTI1_IRQHandler ();
void TIM3_IRQHandler(void); 
void TIM4_IRQHandler(void) ;

extern osMutexId mutex;