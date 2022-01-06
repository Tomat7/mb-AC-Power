// 
#include "modules.h"

#ifdef USE_DS18B20
void update_DS() { ds_Update(); }
#else
const int nSensor = 3;
float dsTemp[3] = { -90 };
uint8_t dsErrors[3] = { 0 };
bool dsIsParasite[3] = { true };
void update_DS() {}
#endif

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

void check_Serial()
{
#ifdef SERIAL_CONFIG
	serial_Check();
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
	telnet_Send(msg0);
#endif
}

void send_Syslog(String msg0)
{
#ifdef USE_SYSLOG
	syslog_Info(msg0);
#endif
}
