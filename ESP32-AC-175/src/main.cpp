//
#include "config.h"
#include "config_plc.h"

#include "setup.h"
#include "func.h"
#include "teh.h"

#pragma message(PLC_ID_MSG STRING(PLC_ID))
#pragma message("Debug level: " STRING(CORE_DEBUG_LEVEL))
#pragma message("Debug level: " STRING(ARDUHAL_LOG_LEVEL))
#pragma message("Debug level: " STRING(ARDUHAL_LOG_LEVEL_DEBUG))
#pragma message("Debug level: " STRING(CONFIG_LOG_DEFAULT_LEVEL))
#pragma message("Platform: " P_CODE)
#pragma message("IDF_ver: " IDF_VER)

void setup()
{
	/*
	String ota_string = WIFI_PASS + String(PLC_ID);
	const char *ota_chars = ota_string.c_str();
	Serial.printf(*ota_chars);
	Serial.println(ota_string);
	Serial.println(ota_chars);
	Serial.println(*ota_chars);
	strVersion = SKETCHINFO;
	*/

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
	setup_WiFi();

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
	
	if ((needUpdate(1) && ((millis() - TEH.MillisRMS) < 12)) || needUpdate(2))
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
