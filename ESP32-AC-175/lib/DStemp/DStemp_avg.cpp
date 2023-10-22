/*
	DStemp.cpp - Library to operate with DS18B20
	Created by Tomat7. October 2017, 2018, 2021.
*/
/*
	check() returns NOTHING!
	only check for temperature changes from OneWire sensor and update Temp
	
	-99  - sensor not found
	-82  - sensor was found but conversation not finished within defined timeout (may be)
	-71  - sensor was found but CRC error (often)
	-59  - sensor was found but something going wrong during conversation (rare)
	
	Connected == 0 значит датчика нет - no sensor found
	в dsMillis хранится millis() c момента запроса или крайнего Init()
	и от dsMillis отсчитывается msConvTimeout
*/

// See updates on https://github.com/Tomat7/DStemp 
// Review branches test* for datails, comments and debug options

#include "Arduino.h"
#include "DStemp.h"
#include <OneWire.h>

void DSThermometer::initavg(uint16_t convtimeout)
{
	for (int i = 0; i < DS_ARR_SZ; i++) _temp[i] = DS_T_MIN;
    init(convtimeout);
//	_msConvTimeout = convtimeout;
//    initOW();
}


void DSThermometer::fillAvg()
{
//	static int i = 0;
	if (_idx >= DS_ARR_SZ) _idx = 0;
	_temp[_idx] = Temp;
	_idx++;
	return;
}

float DSThermometer::getAvg()
{
	uint8_t _min = 0;	// индекс минимального элемента
	uint8_t _max = 0;	// индекс маскимального элемента
	uint8_t _qty = 0;
	float summT = 0;
	
	for (int i = 0; i < DS_ARR_SZ; i++)
	{
		if (_temp[i] > DS_T_MIN) 
		{
			_qty++;
			summT += _temp[i];
			if (_temp[i] < _temp[_min]) _min = i;
			if (_temp[i] > _temp[_max]) _max = i;
		}
	}
	
	if (_qty == DS_ARR_SZ)
	{
		summT -= _temp[_min];
		summT -= _temp[_max];
		summT /= (DS_ARR_SZ - 2);
	} 
	else if (_qty > 1) summT /= _qty;
	else summT = Temp;
	
	return summT;
}

float DSThermometer::getTemp()
{
	static float AvgTemp = DS_T_MIN;
	if (check())
	{
		adjust();
		fillAvg();
		AvgTemp = getAvg();
	}
	return AvgTemp;
}

