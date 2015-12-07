/*
	******************************************************************************
	* Includes
	******************************************************************************
*/




#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include <stdio.h>
#include "lsm9ds1.h"
#include"math.h"
#include "acc_thread.h"
#include "osObjects.h"
#define PI 3.14159265
#define Filter_Depth 30


// Mutex used
osMutexId mutex;

// Struct used for Buffer (Filter)
typedef struct {
    float average;              					 //the sum or average of the buffer
    int new_index;              					 //the index which keeps track of the new measurement placement
    float buffer [Filter_Depth];       		 //the array keeps buffer contents
} Filter_State;


/*
	******************************************************************************
	* GLOBAL VARIABLES
	******************************************************************************
*/

static volatile uint_fast16_t	flag1 =0;
static volatile uint_fast16_t	flag2=0;
float output[3];
float output_gyro[3];
float yawAngle = 0;
int stepDetected = 0;
float coordinates[30] = {0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};
int upCounter = 0;
int downCounter = 0;
int up = 0;
int down = 0;
float previous_angle = 0;
int array_index = 2;
int steps = 0;
float offset = 0;
int angleUpCounter = 0;
int angleDownCounter = 0;


/**
  * @brief 	Moving average filter
	* @param 	
  * @retval 
  */	
	
float movingAverageFilter(Filter_State* filterState, float measurement){										//Moving Average Filter
	
	filterState->average = 0;																																	// CLEAR BUFFER AVERAGE
	
																																														//float bufferAverage = 0; 
	for (int i=0; i < Filter_Depth - 1; i++) {																								//initilaizing buffer average
		filterState->buffer[i] = filterState->buffer[i+1]; 																			//updating buffer content
	}
	filterState->buffer[Filter_Depth-1] = measurement; 																				//add the new measurement
	for (int i=0; i < Filter_Depth; i++) {
		filterState->average = filterState->average + filterState->buffer[i]; 									//get buffer average
	}
	filterState->average = filterState->average/Filter_Depth;
	filterState->average = filterState->average;
	return filterState->average;
	
}

/**
  * @brief 	This function detecting the steps based on the values recieved from X-Acc
	* @param 	Values from x-Acc sensor
  * @retval Returns Steps detected
  */
int detect_steps(float measurement,int *upCounter, int *downCounter, int *up, int *down) {
    float thresholdDown = -700;
    float thresholdUp = 700;
    int upCounterConstant = 1;
    int downCounterConstant = 1;

    if (measurement > thresholdUp) {
        *downCounter = 0;
        *upCounter = *upCounter + 1;

        if (*upCounter > upCounterConstant) {
            //up state detected?
            *up = 2;
        }
    } else if (measurement < thresholdDown) {
        *upCounter = 0; // reset up state counter?
        *downCounter = *downCounter + 1;

        if (*downCounter > downCounterConstant) {
            //down state detected?
            *down = 3;
        }
    }
    if ((*down) - (*up)== 1) {
        stepDetected = 1;
        *down = 0;
        *downCounter = 0;
        *upCounter = 0;
        *up = 0;
    }	else {
        stepDetected = 0;
    }
    return stepDetected;
}
/**
  * @brief 	Uses steps and angles detected to calculated the coordinates
	* @param 	Steps and Angles detected
  * @retval None
  */
void get_coordinates(float current_angle, float *previous_angle, float *output_array, int *array_index, int *steps) {
    float radian = PI/180;
    //Average step length in meter
    float length = 2.0;
    if (fabs((*previous_angle) - current_angle) < 10) {
        *steps=*steps + 1;
    } else {
        output_array[*array_index] = output_array[*array_index-2] +  ((*steps)*length)*sin(*previous_angle*radian);
        output_array[*array_index+1] = output_array[*array_index-1] + ((*steps)*length)*cos(*previous_angle*radian);
        *array_index=*array_index+2;
        *previous_angle = current_angle;
        *steps=1;
    }
}
/**
  * @brief 	Detects angle based on z-gyrosope axis (from sensor)
	* @param 	Values from sensors
  * @retval Angle deteted
  */
float getAngle(float measurement, int *upCounter, int *downCounter) {
	float angle = 0;
	float upConstant = 100;
	float downConstant = -100;
	int upCounterConstant = 90;
	int downCounterConstant = 90;
	
	if (measurement > upConstant) {
		*downCounter = 0;
		*upCounter = *upCounter + 1;
		if (*upCounter > upCounterConstant) {
			*upCounter = 0;
			angle  = 90;
		}
	} else if (measurement < downConstant) {
		*upCounter = 0;
		*downCounter = *downCounter + 1;
		if (*downCounter > downCounterConstant) {
			*downCounter = 0;
			angle  = -90;
		}
	}
	
	return angle;
}
/**
  * @brief 	Acceleration thread
	* @param 	
  * @retval None
  */
void accel_thread(void const *argument){
	while(1)
	{


//	osSignalWait(0x01, osWaitForever);
				if (flag1==1){																	//Setting flags for Ext interrupts, freq of 
		
			flag1= 0;
				osMutexWait(mutex,osWaitForever);	
				LSM9DS1_ReadACC(output);														// Acceleation values are stored in output array.
				osMutexRelease(mutex);	
				if (detect_steps(output[0]*1000, &upCounter , &downCounter, &up, &down) == 1) 
				{
					get_coordinates(yawAngle, &previous_angle, coordinates, &array_index, &steps);	
				}
			  printf("%f \n",output[0]*1000);
			}
	}
}
/**
  * @brief 	Gyroscope Thread
	* @param 	None
  * @retval None
  */
void gyro_thread(void const *argument){
	while(1){
		
		
 		//	osSignalWait(0x01, osWaitForever);
			if (flag2==1){
			flag2= 0;
			osMutexWait(mutex,osWaitForever);	
			LSM9DS1_ReadGYRO(output_gyro);
			osMutexRelease(mutex);	
			yawAngle = yawAngle + getAngle(output_gyro[2], &angleUpCounter, &angleDownCounter);
			// these three below lines are for testing purposes. Can be removed.
			
		
		
		}
	}
}

/**
  * @brief 	Timer3 handler
	* @param 	None
  * @retval None
  */
void TIM3_IRQHandler(void) {																	//TIM3 Interrupt Handler
	 if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
	
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		flag1=1;											//Signal set for Temp Thread
		//osSignalSet(runAcc_thread,0x01);
		
  }

}
/**
  * @brief 	Timer4 handler
	* @param 	None
  * @retval None
  */
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		//Clear the pending bit
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		flag2=1;											
		
	}
}

//void EXTI1_IRQHandler(void){
//	flag2 = 1;
//	EXTI_ClearITPendingBit(EXTI_Line1);
//	//osSignalSet(Gyro_thread, 0x1);
//}


//void EXTI2_IRQHandler ()																			// EXTI2 Interrupt Handler 
//{
//	
//	
//		if (EXTI_GetFlagStatus(EXTI_Line2) != RESET){
//	
//	//printf(" ACC interrupt");
//    EXTI_ClearITPendingBit(EXTI_Line2);
//		flag1=1;
//		}
//	
//}



