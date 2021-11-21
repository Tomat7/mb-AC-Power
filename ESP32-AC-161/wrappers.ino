
void update_DS()
{
#ifdef USE_DS18B20
	ds_Update();
#endif
}

void correct_DS()
{
#ifdef DSCORR
	//DS[0].writeCorrection(12, 0, 0);
	//DS[0].writeCorrection(12, -16, 120);
#endif
}

void update_Modbus()
{
#ifdef USE_MODBUS
	modbus_Update();
#endif
}

void check_Modbus()
{
#ifdef USE_MODBUS
	modbus_Check();
#endif
}

void check_Encoder()
{
#ifdef USE_ENCODER
	encoder_Check();
#endif
}

void check_Web()
{
#ifdef USE_WEBSERVER
	web_Check();
#endif
}

void check_OTA()
{
#ifdef USE_OTA
	ota_Check();
#endif
}

void check_Telnet()
{
#ifdef USE_TELNET
	telnet_Check();
#endif
}

void send_Telnet(String msg0)
{
#ifdef USE_TELNET
	telnet_Info(msg0);
#endif
}


