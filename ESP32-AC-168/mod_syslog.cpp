//
#include "mod_syslog.h"
#include "network.h"
#include "func.h"

#ifdef USE_SYSLOG
#include "Syslog.h"
#define SYSLOG_PORT 514
#define SYSLOG_SERVER 192, 168, 1, 91
//#define DEVICE_HOSTNAME ("PLC-" + String(PLC_ID)).c_str()
#define APP_NAME ""
IPAddress syslog_ip(SYSLOG_SERVER);
WiFiUDP udpClient;
Syslog syslog(udpClient, syslog_ip, SYSLOG_PORT, PLC_NAME, APP_NAME, LOG_KERN, SYSLOG_PROTO_BSD);
#else
#warning "SYSLOG not configured! Nothing to compile. "
#endif	// USE_SYSLOG

#ifdef USE_SYSLOG
void syslog_Setup()
{
	syslog_Info("Starting with IP: " + strIPaddr());
	syslog_Info("WiFi: " + strWiFiName() + " RSSI: " + strWiFiRSSI());
	syslog_Info("BSSID: " + strWiFiBSSID());
	syslog_Info("READY to work.");
}

void syslog_Info(String msg0)
{
	static uint32_t msgNo;
	msgNo++;
	syslog.logf(LOG_INFO, "[%d] %s", msgNo, msg0.c_str());
}
#endif	// USE_SYSLOG


