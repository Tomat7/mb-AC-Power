
void setup_LogCfg(char *fname)
{
	log_cfg_ln(String(fname));
	log_cfg_ln(String(" . compiled on ") + __DATE__ + String(" ") + __TIME__);
	log_cfg_ln(String(" . ESP32 Arduino core ") + ESP32_ARDUINO_VERSION + " / ESP-IDF " + ESP.getSdkVersion());
//	log_cfg(   String(" . ") + ESP.getChipModel() + " rev." + String(ESP.getChipRevision()) + " / ");
  log_cfg(   String(" . board: ") + ARDUINO_BOARD + " / " + ESP.getChipModel() + " rev." + String(ESP.getChipRevision()));
	log_cfg_ln(" / " + String(ESP.getChipCores()) + " cores " + String(ESP.getCpuFreqMHz()) + " MHz");
//  log_cfg_ln(String(" . board: ") + ARDUINO_BOARD + " / " + ESP.getChipModel() + " rev." + String(ESP.getChipRevision()) + " / ");
	log_cfg_ln(String(" . version: ") + VERSION_CODE);
}

void setup_TEH()
{
#ifdef USE_3PHASE
	TEH.initTR();						// один раз!
	TEH.initZC(ZCINT_MODE, false);		// один раз!
	TEH.initADC(ADC_RATE, ADC_WAVES);	// один раз!
	TEH.setADCratio(I_RATIO, U_RATIO, ANGLE_LAG);	// можно запускать повторно для корректировки
	TEH.setRMScorrection(NULL, Ucorr);	// можно запускать повторно во время работы
	log_cfg_addstr(TEH.LibConfig);
#else
	TEH.init(I_RATIO, U_RATIO);
#endif
}

void setup_Pins()
{
	pinMode(TICKTOCK_LED, OUTPUT);
	pinMode(COOLER_PIN, OUTPUT);
//	pinMode(PIN_COOLER2, OUTPUT);
	digitalWrite(COOLER_PIN, HIGH);
//	digitalWrite(PIN_COOLER2, HIGH);
}

void setup_Serial()
{
#ifdef SERIAL_CONFIG
	serial_Setup();
#endif
}

void setup_Encoder()
{
#ifdef USE_ENCODER
	encoder_Setup();
	log_cfg_ln("+ ENCODER configured OK.");
#else 
	log_cfg_ln("-- NO ENCODER in this version.");
#endif
}

void setup_OTA()
{
#ifdef USE_OTA
	ota_Setup();
	log_cfg_ln("+ OTA activated.");
#else
	log_cfg_ln("-- NO OTA in this version.");
#endif
}

void setup_Web()
{
#ifdef USE_WEBSERVER
	web_Setup();
	log_cfg_ln("+ HTTP server started.");
#else
	log_cfg_ln("-- NO HTTP server in this version.");
#endif //USE_WEBSERVER
}

void setup_Telnet()
{
#ifdef USE_TELNET
	telnet_Setup();
	log_cfg_ln("+ TELNET server started.");
#else
	log_cfg_ln("-- NO TELNET server in this version.");
#endif	// USE_TELNET
}

void setup_Modbus()
{
#ifdef USE_MODBUS
	modbus_Setup();
	log_cfg_ln("+ MODBUS Slave on TCP-port 502 started.");
#else
	log_cfg_ln("-- NO MODBUS in this version.");
#endif //USE_MODBUS
}

void setup_PinInfo()
{
	display_PinInfo();
#ifdef USE_TASK_INFO
	smInfoEnd = xSemaphoreCreateBinary();
#endif
}

void setup_DS()
{
#ifdef USE_DS18B20
	ds_Setup();
#endif
}
