// 
#include "mod_modbus.h"
#include "func.h"
#include "logging.h"
#include "teh.h"
#include "modules.h"

#ifdef USE_MODBUS
#include <ModbusIP_ESP8266.h>
#include "Modbus.h"
ModbusIP mb;
#endif //USE_MODBUS


#ifdef USE_MODBUS

void modbus_Setup()
{
	mb.slave();
	mb.addHreg(hrSECONDS);
	for (int i = 0; i < nSensor; i++) mb.addHreg(hrDSTEMP0 + i); 	// Регистр температуры
	mb.addHreg(hrPSET);
	mb.addHreg(hrPNOW);
	mb.addHreg(hrANGLE);
	for (int i = 0; i < 3; i++) mb.addHreg(hrX0 + i);

#ifdef USE_3PHASE
	for (int i = 0; i < 3; i++)
	{
		mb.addHreg(hrP0 + i);
		mb.addHreg(hrI0 + i);
		mb.addHreg(hrU0 + i);
	}
#endif //USE_3PHASE

}

void modbus_Check()
{
	mb.task();
}

void modbus_Update()
{
	// если мастер онлайн - он должен записывать 0 в регистр SECONDS
	// это будет признаком "живости" Мастера Modbus'а для модуля
	// и наоборот: не 0 в SECONDS - признак "живости" модуля для Мастера
	// хотя Мастеру логичнее отслеживать "живость" по GetQuality
	bool wasMasterOK = isMBmasterOK;
	uint16_t Pmodbus = mb.Hreg(hrPSET);
/*
	if ((mb.Hreg(hrSECONDS) == 0) && !isLocalMode) mb.Hreg(hrSECONDS, millis() / 1000);
	isMBmasterOK = (((uint16_t)(millis() / 1000) - mb.Hreg(hrSECONDS)) < MB_TIMEOUT);
*/
	isMBmasterOK = modbus_MasterStatus();

	if (isMBmasterOK)
	{
		setPower(Pmodbus);  // test !!
		if (showInfo > 0) log_info_ln("+ MODBUS ok");
	}
	else
	{
		//	mb.Hreg(hrPSET, 0);
		if (showInfo > 0) log_info_ln("-- MODBUS Master OFFline ", mb.Hreg(hrSECONDS));
	}

	if (isMBmasterOK != wasMasterOK) setPower(0);

	mb.Hreg(hrPSET, TEH.Pset);

	for (int i = 0; i < nSensor; i++) mb.Hreg(hrDSTEMP0 + i, round(dsTemp[i] * 100));

	mb.Hreg(hrPNOW, TEH.Pavg);
	mb.Hreg(hrANGLE, TEH.Angle);


#ifdef USE_DS18B20
	mb.Hreg(hrX0, ds_getTHreg(2));
	mb.Hreg(hrX1, ds_getTLreg(2));
#endif

#ifdef USE_3PHASE
	for (int i = 0; i < 3; i++)
	{
		mb.Hreg(hrP0 + i, TEH.P[i]);
		mb.Hreg(hrI0 + i, round(TEH.I[i] * 1000));
		mb.Hreg(hrU0 + i, round(TEH.U[i]));
	}
#else
	mb.Hreg(hrI0, round(TEH.Inow * 1000));
	mb.Hreg(hrU0, round(TEH.Unow));
#endif //USE_3PHASE

}

bool modbus_MasterStatus()
{
	uint16_t secUptime = millis() / 1000;
	bool isHeartBeat = (mb.Hreg(hrSECONDS) == 0);
	bool MasterStatus = false;

	if (isHeartBeat && !isLocalMode) mb.Hreg(hrSECONDS, secUptime);
	MasterStatus = ((secUptime - mb.Hreg(hrSECONDS)) < MB_TIMEOUT);

	return MasterStatus;
}

#endif //USE_MODBUS

