  /*******************************************************************
  * @file    SevenSegDisplay.h
  * @author  Razi Murshed
  * @date    22/10/2015
  * @brief  - Configuration of LED GPIOs
  *         - Display floating point number using 7 seg LED.
  *         - Display Angle
  *
	********************************************************************/
	
#define SEGMENT_1 3
#define SEGMENT_2 2
#define SEGMENT_3 1
#define SEGMENT_0 4

#define SEGMENT_DELAY 0x430EB    //delay between corresponding segments

void initializeDisplay();
/*
 * Function: Delay
 * ----------------------------
 * Countdown timer
 *   Returns: void
 *
 *   @param: uint32_t nCount - countds down from this value
 *   returns: void
 */
void Delay(int nCount) ;

/*
 * Function: SevenSegLED
 * ----------------------------
 * Displays single digit number in a segment 
 *   Returns: void
 *
 *   @param: int number - Number to display (0 - 9) and 10 for Dot
 *   @param: int position - segment (1, 2, 3) 
 *
 *   returns: void
 */
void SevenSegDisplay(int number,  int position);

/*
 * Function: LED_GPIO_Config
 * ----------------------------
 * Configures GPIO port D pins(0 - 10, 14) for 7-segment and PWM LED 
 *   Returns: void
 *
 *   @param: void
 */
void LED_GPIO_Config(void);

/*
 * Function: DisplayAngle
 * ----------------------------
 * Breaks the floating point angle into 2 segments
 *
 *   @param: float angle - tilt angle in float
 *   @param: int segment to display
 *   returns: void
 */
void DisplayAngle(float angle);

void DisplayTemperature(float temperature);

void configTIM4();

void LED_clear();







