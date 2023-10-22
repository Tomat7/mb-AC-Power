/*
  DStemp.cpp - Library to operate with DS18B20
  Created by Tomat7, October 2017, 2018, 2021.
  
  See updates on https://github.com/Tomat7/DStemp 
  Review branches test* for datails, comments and debug options
  
  The code of setCorrection() from http://blog.asifalam.com/ds18b20-change-resolution/ (c) Asif Alam
*/

#ifndef DStemp_h
#define DStemp_h

#include "Arduino.h"
#include <OneWire.h>

#ifdef DS_LIBVERSION
#undef DS_LIBVERSION
#endif
#define DS_LIBVERSION "DStemp_v20221102"

#define DS_CONV_TIMEOUT 750
#define HI_RESOLUTION 12
#define DS_ARR_SZ 8				// Size of array for AVERAGE TEMP calculation 

#define DS_T_MIN -55			// Minimal temperature by DataSheet
#define DS_T_ERR_OTHER -59 		// sensor was found but something going wrong during conversation (rare)
#define DS_T_ERR_CRC -71		// sensor was found but CRC error (often)
#define DS_T_ERR_TIMEOUT -82	// sensor was found but conversation not finished within defined timeout (may be)
#define DS_T_ERR_NOSENSOR -99	// Sensor Not Connected (not found)
#define DS_T_CORR 80			// What temperature start correction (if available on TH & TL)


class DSThermometer
{
public:

	DSThermometer(uint8_t pin);
//    void init();
	void init(uint16_t convtimeout = DS_CONV_TIMEOUT);
	void initavg(uint16_t convtimeout = DS_CONV_TIMEOUT);

	bool check();
	void control();
	void printConfig();
	void adjust();
	bool write_regs(byte _bits, int8_t _th, int8_t _tl);

	float getTemp();
	float Temp = DS_T_MIN;
	
	unsigned long dsMillis;
	uint16_t TimeConv = DS_CONV_TIMEOUT;
    bool Connected = false;
	bool Parasite = false;
	int8_t THreg = 0;
	int8_t TLreg = 0;
	int8_t CFGreg = 0;
	
	String LibVersion = DS_LIBVERSION;
	String LibConfig();

private:
    //OneWire *_ds;
	OneWire ow;
	byte _pin;
    uint16_t _msConvTimeout;
	uint8_t _scrpadData[9]; // буфер данных
	int16_t _rawTemp;
	void ds_init();
	void ds_request();
	float ds_get_temp();
	bool ds_check_scrpad();
	bool ds_check_regs();
	bool ds_isParasite();
	bool ds_isConnected();
	void ds_write_regs(byte _cmd, int8_t _th, int8_t _tl);

	uint8_t _idx = 0;
	float _temp[DS_ARR_SZ] = { 0 };
	void fillAvg();
	float getAvg();
};
#endif
