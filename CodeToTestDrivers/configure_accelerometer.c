  /*******************************************************************
  * @file    configure_accelerometer
  * @author  Razi Murshed
  * @date    21/10/2015
  * @brief   Initializes the MEMs Accelerometer
	********************************************************************/
	

#include "configure_accelerometer.h"
#include <stdint.h>

/*
 * Function: set_Accelerometer_MEM
 * ----------------------------
 * Set up configuration for Accelerometer 
 *   Returns: void
 *
 *   @param: none
 *   returns: void
 */
	void set_Accelerometer_MEM(void)
	{
			LIS3DSH_InitTypeDef LIS3DSH_InitStruct;
		/* Set the sampling frequency as 100Hz*/
			LIS3DSH_InitStruct.Power_Mode_Output_DataRate = LIS3DSH_DATARATE_100;
		/* Enable X, Y, Z axes*/
			LIS3DSH_InitStruct.Axes_Enable = ((uint8_t)0x07); 
//			LIS3DSH_InitStruct.Axes_Enable = LIS3DSH_Y_ENABLE;         
//			LIS3DSH_InitStruct.Axes_Enable = LIS3DSH_Z_ENABLE;  
		/* Set the range to +/- 2.0g */
			LIS3DSH_InitStruct.Full_Scale = LIS3DSH_FULLSCALE_2;
		/* Set the self-test*/
			LIS3DSH_InitStruct.Self_Test = LIS3DSH_SELFTEST_NORMAL;
		/*Set the continuos update*/
		  LIS3DSH_InitStruct.Continous_Update = LIS3DSH_ContinousUpdate_Enabled;
			LIS3DSH_Init(&LIS3DSH_InitStruct);
    /*Set the AA filter*/
			LIS3DSH_InitStruct.AA_Filter_BW = LIS3DSH_AA_BW_800;	
	}