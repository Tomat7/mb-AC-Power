// Внимание! Проверь пароль OTA_PASS в config_wifi.h и в board.txt
#ifdef USE_OTA

void ota_Setup()
{
	ArduinoOTA.setHostname(OTA_HOSTNAME);
	ArduinoOTA.setPassword(OTA_PASS);
	ArduinoOTA.setPort(OTA_PORT);

	ArduinoOTA.onStart([]()          // switch off all the PWMs during upgrade
		{
			TEH.stop();
			delay(10);

			String type;
			if (ArduinoOTA.getCommand() == U_FLASH) {
				type = "sketch";
			}
			else {
				type = "filesystem";
			}
			// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
			log_info_ln("");
			log_info_ln(" OTA update starting " + type);

			display_OTAbegin();
		});

	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
		{
			CPRINTF(" .. %u%%\r", (progress / (total / 100)));
			display_OTAprogress((long)(progress / (total / 100)));
//			log_info(".");
			tick_Led();
		});

	ArduinoOTA.onEnd([]()             // do a fancy thing with our board led at end
		{
			log_info_ln("");
			log_info_ln(" OTA update finished OK!");
			log_info_ln(" rebooting...");
			LD_printString_12x16("OTA OK!", LCDX1, 6);
			delay(3000);
			//modbusON = true;
		});

	ArduinoOTA.onError([](ota_error_t error)
		{
			String OTAErrorInfo;
			CPRINTF("OTA Error[%u]: ", error);
			LD_printString_12x16("OTA error", LCDX1, 3);

			if (error == OTA_AUTH_ERROR) {
				OTAErrorInfo = "OTA Auth Failed";
			}
			else if (error == OTA_BEGIN_ERROR) {
				OTAErrorInfo = "OTA Begin Failed";
			}
			else if (error == OTA_CONNECT_ERROR) {
				OTAErrorInfo = "OTA Connect Failed";
			}
			else if (error == OTA_RECEIVE_ERROR) {
				OTAErrorInfo = "OTA Receive Failed";
			}
			else if (error == OTA_END_ERROR) {
				OTAErrorInfo = "OTA End Failed";
			}

			log_info_ln(OTAErrorInfo);
			log_info_ln("rebooting...");
			LD_printString_6x8(OTAErrorInfo.c_str(), LCDX1, 5);
			LD_printString_6x8("reboot...", LCDX1, 6);
			delay(3000);
			(void)error;
			ESP.restart();
		});

	/* setup the OTA server */
	ArduinoOTA.begin();
}

void ota_Check()
{
	ArduinoOTA.handle();
}
#endif //USE_OTA
