#define VERSION_CODE "162"
#define PLC_ID 39 // config_plc.h for details.

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <Update.h>
#include <HttpsOTAUpdate.h>
#include <ArduinoOTA.h>
#include <Wire.h>

#include <OneWireNg.h>
#include <DStemp.h>
#include <ReadDigKey.h>
#include <LiquidCrystal_I2C.h>
#include <ASOLED.h>
#include <ModbusIP_ESP8266.h>
#include <ACpower3.h>

//#include <esp_arduino_version.h>
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
	delay(2000);

	tick_Led();
	setup_PinInfo();
	setup_DS();

	tick_Led();
	setup_TEH();

	tick_Led();
	log_cfg_ln(" . Free memory: ", String(ESP.getFreeHeap()));
	log_cfg_ln("+ READY to work.");
	msPrint = millis();
}

void loop()
{
	check_TEH();
	//TEH.control();
	//TEH.control(5000);  // ручное управление углом открытия, без стабилизации!

	if ((needUpdate(1) && ((millis() - TEH.MillisRMS) < 10)) || needUpdate(2))
	{
		update_Info();		// check ZC, set Power, get Temp form DS, update Modbus, check WiFi
		//tick_Led();
		msUpdate = millis();
	}

//	if ((millis() - msPrint) > msPrintPeriod)
	if ((needPrint(1) && ((millis() - TEH.MillisRMS) < 10)) || needPrint(2))
	{
		print_Info();		// refresh OLED or LCD, print to Serial or Telnet
		tick_Led();
		msPrint = millis();
	}

	check_OTA();
	check_Modbus();
	check_Web();
	check_Telnet();
	check_Encoder();

}
