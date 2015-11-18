  /*******************************************************************
  * @file    calibrate.h
  * @author  Razi Murshed
  * @date    21/10/2015
  * @brief   Calibrates the x y z axes
  *            
	********************************************************************/


#include <stdio.h>
#include <stdint.h>

/*
 * Function: get_calibration_data
 * ----------------------------
 * Get Calibrated data
 *   Returns: void
 *
 *   @param: int32_t*
 *   @param: float*
 *   returns: none
 */
void get_calibration_data(float[3], float* calibrated_data);