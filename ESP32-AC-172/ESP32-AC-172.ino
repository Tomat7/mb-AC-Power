//
#include "config.h"
#include "config_plc.h"
//#define PLC_ID PLC_NUM // config_plc.h for details.

#include <esp_arduino_version.h>
#include <core_version.h>
#include <SPI.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <Update.h>
#include <HttpsOTAUpdate.h>
#include <ArduinoOTA.h>
#include <WebServer.h>
#include <FS.h>
#include <Wire.h>

#include <Syslog.h>
#include <OneWireNg.h>
#include <DStemp.h>
#include <ReadDigKey.h>
#include <LiquidCrystal_I2C.h>
#include <ASOLED.h>
#include <ModbusIP_ESP8266.h>
#include <ACpower3.h>
//#include <ACpower.h>

#include "config.h"
#include "config_plc.h"
#include "macros.h"
#include "logging.h"
#include "network.h"
#include "func.h"
#include "teh.h"
#include "display.h"
#include "setup.h"
#include "mod_ds18b20.h"
#include "mod_modbus.h"
#include "mod_web.h"
#include "mod_ota.h"
#include "mod_telnet.h"
#include "mod_encoder.h"
#include "mod_serial.h"
#include "mod_syslog.h"
#include "modules.h"
//

void setup()
{
	/*
	String ota_string = WIFI_PASS + String(PLC_ID);
	const char *ota_chars = ota_string.c_str();
	Serial.printf(*ota_chars);
	Serial.println(ota_string);
	Serial.println(ota_chars);
	Serial.println(*ota_chars);
	*/
	//strVersion = SKETCHINFO;

	setup_Pins();
	tick_Led();
	setup_Serial();
	delay(1000);

	tick_Led();
	setup_LogCfg(__FILE__);
//	setup_LogCfg((char*)__FILE__);

	tick_Led();
	setup_Display(SKETCHINFO, VERSION_CODE);
	setup_Encoder();
	setup_Network();

	tick_Led();
	setup_Syslog();
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
		msUpdate = millis();
	}

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
