#include <stdio.h>
#include <MovingAverageFilter.h>


float maFilter(mAState* ma, float new_value)
{
	
	float average = 0;
	float movingSum;
	int i;
	
	//puts new_value in the buffer
	ma->filterBuffer[ma->newIndex] = new_value;
	
	// increments the newIndex, if newIndex points to the last 
	// element of the buffer then newIndex is made 0 to point to
	// beginning of the buffer again
	if(ma->newIndex ==  DEPTH_D-1)
	{
		ma->newIndex = 0;
	}
	else
	{
		ma->newIndex++;
	}
	
	// If filter empty then divide sum by newIndex+1
	if (ma->buffer_length < DEPTH_D)
	{
		movingSum = 0;
		i=0;
		while(i <= ma->buffer_length)
		{
			movingSum = movingSum + (ma->filterBuffer[i]);
			i++;
		}
		
		average = movingSum / (ma->buffer_length+1);
		ma->buffer_length++;
		
	}
	else // if filter full then divide sum by filter size
	{
		movingSum = 0;
		i=0;
		while(i < DEPTH_D)
		{
			movingSum = movingSum + (ma->filterBuffer[i]);
			i++;
		}
		average = movingSum / DEPTH_D;
	}
	
	return average;
	
}

