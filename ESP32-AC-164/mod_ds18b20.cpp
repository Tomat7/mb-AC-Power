// ds18b20.cpp

#include "mod_ds18b20.h"
#include "display.h"
#include "logging.h"
#include "func.h"
#include "teh.h"

#ifdef USE_DS18B20
#include <DStemp.h>
DSThermometer DS[] = DS_PIN;
const int nSensor = sizeof(DS) / sizeof(DSThermometer);
float dsTemp[nSensor] = { -99 };
uint8_t dsErrors[nSensor] = { 0 };
bool dsIsParasite[nSensor] = { true };
#else 
#warning "Temperature not configured! Nothing to compile. "
#endif

//uint32_t test;

#ifdef USE_DS18B20

void ds_Setup()
{
	for (int i = 0; i < nSensor; i++)
	{
		DS[i].init(DS_CONVTIME);
		display_DSpin(i);
		// DS[i].printConfig();
		String dsOK;
		if (DS[i].Connected) dsOK = "+ ";
		else dsOK = "-- ";
		dsOK += "DS " + String(i) + ": ";
		log_cfg_ln(dsOK, DS[i].LibConfig());
	}
}

void ds_Update()
{
	for (int i = 0; i < nSensor; i++)
	{
		check_TEH();
		DS[i].check();
		dsIsParasite[i] = DS[i].Parasite;
#ifdef DS_ERRORS
		//DS[i].adjust();
		float t = DS[i].Temp;
		if (t > DS_MIN_T)				  { dsErrors[i] = 0; dsTemp[i] = t; }
		else if (dsErrors[i] > DS_ERRORS) { dsErrors[i]++;	 dsTemp[i] = t;	}
		else							  {	dsErrors[i]++; }
#else	
		dsTemp[i] = DS[i].Temp;
#endif // DS_ERRORS

#ifdef SERIAL_INFO
		String dsInfo = ".  DS " + String(i, DEC) + ": " + String(dsTemp[i], 2) + " | parasite: " +
			String(DS[i].Parasite, DEC) + " | " + String(DS[i].TimeConv, DEC);
		if (showInfo > 0) log_info_ln(dsInfo);
#endif // SERIAL_INFO
	}
}

String ds_getLibVersion(int id)
{
	return DS[id].LibVersion;
}

int8_t ds_getTHreg(int id)
{
	return DS[id].THreg;
}

int8_t ds_getTLreg(int id)
{
	return DS[id].TLreg;
}

#endif	// USE_DS18B20

