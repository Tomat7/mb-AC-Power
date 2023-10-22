// ds18b20.cpp

#include "mod_ds18b20.h"
#include "display.h"
#include "logging.h"
#include "func.h"
#include "teh.h"
#include "modules.h"

#ifdef USE_DS18B20
#include <DStemp.h>
DSThermometer DS[] = DS_PIN;
const int nSensor = sizeof(DS) / sizeof(DSThermometer);
float dsTemp[nSensor] = {-99};
uint32_t dsErrors[nSensor] = {0};
bool dsIsParasite[nSensor] = {true};
#else
#pragma message "DS18B20 not configured! Nothing to compile. "
#endif

#ifdef USE_DS18B20

#ifndef DS_ERRORS
#define DS_ERRORS 1
#endif

void ds_Setup()
{
	for (int i = 0; i < nSensor; i++)
	{
		DS[i].initavg(DS_CONVTIME);
		display_DSpin(i);
		// DS[i].printConfig();
		String dsOK = "-- ";

		if (DS[i].Connected)
			dsOK = "+ ";

		dsOK += "DS " + String(i) + ": ";
		log_cfg_ln(dsOK, DS[i].LibConfig());
	}

	return;
}

void ds_Update()
{
	for (int i = 0; i < nSensor; i++)
	{
		check_TEH();
/*		
		// DS[i].control();			// = check() + adjust() with validation
		DS[i].check();
		DS[i].adjust();						// TH & TL register must keep coeffs for calculating
		float t = DS[i].Temp;
*/
		float t = DS[i].getTemp(); // = check() + adjust() + calculate and return AVERAGE temperature
/*tst*/	// t = DS[i].Temp;
		dsIsParasite[i] = DS[i].Parasite;

		if (t > DS_MIN_T)				  
			{ dsErrors[i] = 0; dsTemp[i] = t; }
		else if (dsErrors[i] > DS_ERRORS) 
			{ dsErrors[i]++;   dsTemp[i] = t; }
		else							  
			{ dsErrors[i]++; }

		if (dsErrors[i] == DS_ERRORS)
			send_Syslog("DS" + String(i) + " error: " + String(t));

#ifdef SERIAL_INFO
		String dsInfo = ".  DS " + String(i, DEC) + ": " + String(dsTemp[i], 2) + " | parasite: " +
						String(DS[i].Parasite, DEC) + " | " + String(DS[i].TimeConv, DEC);
		if (showInfo > 0)
			log_info_ln(dsInfo);
#endif // SERIAL_INFO
	}
	return;
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

bool ds_setTregs(int id, byte res, int8_t th, int8_t tl)
{
	return DS[id].write_regs(res, th, tl);
}

#endif // USE_DS18B20
