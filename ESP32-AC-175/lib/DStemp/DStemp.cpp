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

/*
#if defined(__AVR__)
	#define IF_AVR(...) (__VA_ARGS__)
	#else
	#define IF_AVR(...)
#endif

#if defined(ESP32) || defined(ESP8266)
	#define IF_ESP(...) (__VA_ARGS__)
	#else
	#define IF_ESP(...)
#endif
*/

/*
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
*/

DSThermometer::DSThermometer(uint8_t pin) : ow(pin)
{
	_pin = pin;
	_msConvTimeout = DS_CONV_TIMEOUT;
}

void DSThermometer::init(uint16_t convtimeout)
{
	_msConvTimeout = convtimeout;
	ds_init();
}

void DSThermometer::control()
{
	if (check())
		adjust();
}

bool DSThermometer::check()
{
	uint16_t msConvDuration = millis() - dsMillis;

	if (Connected && (ow.read_bit() == 1)) // вроде готов отдать данные
	{
		Temp = ds_get_temp(); // можем ещё получить -71 (CRC error) или -59 (other error)
		TimeConv = msConvDuration;
	}
	else if (msConvDuration > _msConvTimeout) // был подключен, но успел и не готов отдать данные
		Temp = DS_T_ERR_TIMEOUT;			  // -82, датчик был, но оторвали на ходу или не успел
	else
		return false;

	if (Temp > DS_T_MIN)
		ds_request();
	else
		ds_init();

	return true;
}

void DSThermometer::adjust()
{
	if (Temp > DS_T_CORR)
	{
		Temp *= (1 + (float)THreg * 0.001); // коррекция пропорциональная
		Temp += (float)TLreg * 0.01;		// коррекция сдвиг
	}
}

bool DSThermometer::write_regs(byte _bits, int8_t _th, int8_t _tl) // (c) Asif Alam's Blog http://blog.asifalam.com/ds18b20-change-resolution/
{
	if ((Connected) && (Temp > DS_T_MIN))
	{
		byte reg_cmd;

		switch (_bits)
		{
		case 9:  reg_cmd = 0x1F; break;
		case 10: reg_cmd = 0x3F; break;
		case 11: reg_cmd = 0x5F; break;
		case 12: reg_cmd = 0x7F; break;
		default: reg_cmd = 0x7F; break;
		}

		ds_write_regs(reg_cmd, _th, _tl);
		return true;
	}
	return false;
}

void DSThermometer::printConfig()
{
	Serial.println(LibConfig());
}

String DSThermometer::LibConfig()
{
	return LibVersion + " on pin " + String(_pin);
}
