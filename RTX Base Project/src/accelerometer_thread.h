#define SIGNAL_TILT_READY 0x3
#define SIGNAL_DISP_TILT 0x04

extern int GLB_TiltAngle_RollPitch_display_flag;


void tilt_DispFlag_thread(void const * argument);
void accelerometer_thread(void const * argument);
void LED_DisplayTiltAngle(float roll, float pitch);
void configTIM3(void);


