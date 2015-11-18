  /*******************************************************************
  * @file    calculate_angle_tilted.c
  * @author  Razi Murshed
  * @date    21/10/2010
  * @brief   Roll and pitch calculator
  *
	********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "calculate_angle_tilted.h"
#include "math.h"


/*
 * Function: calculate_pitch
 * ----------------------------
 * Calculate Pitch
 *   Returns: float
 *
 *   @param: float*
 *   returns: float
 */
float calculate_roll(float* calibrated_data)
{
	float pitch = 0;
	float degrees = 180/PI;
	float A_x1 = calibrated_data[0];
	float A_y1 = calibrated_data[1];
	float A_z1 = calibrated_data[2];
	float A_y1_square = pow(A_y1, 2);
	float A_z1_square = pow(A_z1, 2);
	float denominator = sqrt(A_y1_square+A_z1_square);
	
	pitch = atan(A_x1/denominator) * degrees;
	
	return fabs(pitch);
}


/*
 * Function: calculate_roll
 * ----------------------------
 * Calculate Roll
 *   Returns: float
 *
 *   @param: float*
 *   returns: float
 */
float calculate_pitch(float* calibrated_data)
{
	float roll = 0;
	float degrees = 180/PI;
	float A_x1 = calibrated_data[0];
	float A_y1 = calibrated_data[1];
	float A_z1 = calibrated_data[2];
	float A_x1_square = pow(A_x1, 2);
	float A_z1_square = pow(A_z1, 2);
	float denominator = sqrt(A_x1_square+A_z1_square);
	
	roll = atan(A_y1/denominator) * degrees - 3;
	
	return fabs(roll);
}