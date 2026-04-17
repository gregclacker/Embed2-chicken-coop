#include <stdbool.h>
#include "stm32f4xx.h"
#include "hall_effect.h"

int main()
{
	initHallEffect();
	int i = 0;
	
	while(1)
	{
		if (!getFarSensorStatus())
			i = 1;
		
		else if (!getNearSensorStatus())
			i = 2;
	}
	
	return 0;
}