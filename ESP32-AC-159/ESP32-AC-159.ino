#define VERSION_CODE "159"
#define PLC_ID 39 // config_plc.h for details.

#include <WiFi.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <HttpsOTAUpdate.h>
#include <ArduinoOTA.h>
#include <Wire.h>

#include <OneWire.h>
#include <DStemp.h>
#include <ReadDigKey.h>
#include <LiquidCrystal_I2C.h>
#include <ASOLED.h>
#include <ModbusIP_ESP8266.h>
#include <ACpower3.h>

#include <esp_arduino_version.h>
#include <core_version.h>
#include "config.h"
#include "config_wifi.h"
#include "config_plc.h"
#include "config_libs.h"
#include "variables.h"
#include "macros.h"


void setup()
{
	strVersion = SKETCHINFO;
	setup_LogCfg((char*)__FILE__);
	setup_Pins();

	tick_Led();
	setup_Serial();

	tick_Led();
	setup_Display();
	setup_Encoder();
	setup_Network();
	setup_OTA();
	setup_Web();
	setup_Telnet();
	setup_Modbus();

	tick_Led();
	delay(3000);

	tick_Led();
	setup_PinInfo();
	setup_DS();

	tick_Led();
	TEH.initTR();						// один раз!
	TEH.initZC(ZCINT_MODE, false);		// один раз!
	TEH.initADC(ADC_RATE, ADC_WAVES);	// один раз!
	TEH.setADCratio(I_RATIO, U_RATIO, ANGLE_LAG);	// можно запускать повторно для корректировки
	TEH.setRMScorrection(NULL, Ucorr);	// можно запускать повторно во время работы
	log_cfg_addstr(TEH.LibConfig);

	tick_Led();
	log_cfg_ln(" . Free memory: ", String(ESP.getFreeHeap()));
	log_cfg_ln("+ READY to work.");
	msPrint = millis();
}

void loop()
{
	TEH.control();
	//TEH.control(5000);  // ручное управление углом, без стабилизации!

	if ((millis() - msPrint) > msShowPeriod)
	{
		update_Info();
		tick_Led();
		msPrint = millis();
	}

	check_Modbus();
	check_OTA();
	check_Web();
	check_Telnet();
	check_Encoder();
}
