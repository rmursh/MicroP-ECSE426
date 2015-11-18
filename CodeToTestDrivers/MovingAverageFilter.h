#include <stdint.h>

#define DEPTH_D 20


typedef struct MovingAverageState
{
	int newIndex;                            // index to the new position in buffer
	int buffer_length;                     // indicates full buffer flag 
	float filterBuffer[DEPTH_D];
} mAState;

float maFilter(mAState* ma, float new_value);

