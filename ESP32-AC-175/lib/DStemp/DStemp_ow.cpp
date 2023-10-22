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

// Scratchpad locations. Thanks to Miles Burton <miles@mnetcs.com> for DallasTemperature library
#define TEMP_LSB 0
#define TEMP_MSB 1
#define HIGH_ALARM_TEMP 2
#define LOW_ALARM_TEMP 3
#define CONFIGURATION 4
#define INTERNAL_BYTE 5
#define COUNT_REMAIN 6
#define COUNT_PER_C 7
#define SCRATCHPAD_CRC 8

// OneWire commands
#define STARTCONVO 0x44		 // Tells device to take a temperature reading and put it on the scratchpad
#define COPYSCRATCH 0x48	 // Copy scratchpad to EEPROM
#define READSCRATCH 0xBE	 // Read from scratchpad
#define WRITESCRATCH 0x4E	 // Write to scratchpad
#define RECALLSCRATCH 0xB8	 // Recall from EEPROM to scratchpad
#define READPOWERSUPPLY 0xB4 // Determine if device needs parasite power
#define SKIP_ROM 0xCC		 // Only ONE sensor on wire

void DSThermometer::ds_init()
{
	//Connected = ow.reset();
	dsMillis = millis();

	if (ds_isConnected())
	{
		Parasite = ds_isParasite();
		ds_request();
	}
	else
	{
		Temp = DS_T_ERR_NOSENSOR;
		Parasite = false;
		THreg = 0;
		TLreg = 0;
		CFGreg = 0;
	}
}

void DSThermometer::ds_request()
{
	ow.reset();
	ow.write(SKIP_ROM);
	ow.write(STARTCONVO, Parasite);
	dsMillis = millis();
}

float DSThermometer::ds_get_temp()
{
	float owTemp;

	if (ds_isConnected())
	{
		if (ds_check_regs())
			owTemp = (float)_rawTemp * 0.0625; // стащил из примера OneWire
		else
			owTemp = DS_T_ERR_CRC; // -71, ошибка CRC
	}
	else
		owTemp = DS_T_ERR_OTHER; // -59, другой косяк (тоже может быть)

	return owTemp;
}

bool DSThermometer::ds_check_scrpad()
{
	ow.write(SKIP_ROM);
	ow.write(READSCRATCH);		   // Read Scratchpad
	ow.read_bytes(_scrpadData, 9); // чтение памяти датчика, 9 байтов
	return (OneWire::crc8(_scrpadData, 8) == _scrpadData[SCRATCHPAD_CRC]);
}

bool DSThermometer::ds_check_regs()
{
	if (ds_check_scrpad())
	{
		_rawTemp = ((int16_t)_scrpadData[TEMP_MSB] << 8) | ((int16_t)_scrpadData[TEMP_LSB]);
		THreg = (int8_t)_scrpadData[HIGH_ALARM_TEMP];
		TLreg = (int8_t)_scrpadData[LOW_ALARM_TEMP];
		CFGreg = (int8_t)_scrpadData[CONFIGURATION];
		return true;
	}
	return false;
}

bool DSThermometer::ds_isConnected()
{
	//	byte addr[8];
	//	ds.reset_search();
	//	Connected = (ds.search(addr));
	Connected = ow.reset();
	return Connected;
}

bool DSThermometer::ds_isParasite()
{
	//	ds.reset();
	ow.write(SKIP_ROM);
	ow.write(READPOWERSUPPLY);
	if (ow.read_bit() == 0)
		return true;
	else
		return false;
}

void DSThermometer::ds_write_regs(byte _cmd, int8_t _th, int8_t _tl)
{
	ow.reset();
	ow.write(SKIP_ROM);		// No address - only one DS on line
	ow.write(WRITESCRATCH); // Write scratchpad command
	ow.write(_th);			// TH data
	ow.write(_tl);			// TL data
	ow.write(_cmd);			// Configuration Register (resolution) 7F=12bits 5F=11bits 3F=10bits 1F=9bits
	ow.reset();				// This "reset" sequence is mandatory

	ow.write(SKIP_ROM);
	ow.write(COPYSCRATCH, Parasite); // Copy Scratchpad command
	delay(20);						 // added 20ms delay to allow 10ms long EEPROM write operation (DallasTemperature)

	if (Parasite)
		delay(10); // 10ms delay
	ow.reset();
}

