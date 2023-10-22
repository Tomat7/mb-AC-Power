//
#include "network.h"
#include "display.h"
#include "logging.h"
#include "func.h"
#include "macros.h"

#ifdef ARDUINO_ARCH_ESP32
#pragma message "Check config files for DHCP or static IP!"
#else
#error "Wrong module defined. Use ESP32 only."
#endif // ARDUINO_ARCH_ESP32

// Внимание! Проверь пароль WIFI_PASS в config_wifi.h
#if WIFI_CHANNEL == 6
byte bssid[] = {0x74, 0x83, 0xC2, 0x3A, 0x2C, 0xF7};
#endif

#if WIFI_CHANNEL == 1
byte bssid[] = {0x18, 0xE8, 0x29, 0x09, 0xA1, 0xE2};
#endif

uint16_t cntWiFiErrors = 1;

#ifndef USE_DHCP
IPAddress local_IP(ETHERNET_IP);
IPAddress gateway(IP_NETWORK + 254);
IPAddress subnet(255, 255, 255, 0);
#endif

void setup_WiFi() // Config Modbus IP
{
	display_MAC();
	display_SSID();
	init_WiFi();
	check_WiFi();
}

void init_WiFi()
{
	log_cfg_ln("+ WiFi network setup: ");
	log_cfg_ln(" . MAC: ", WiFi.macAddress());
	log_cfg(" . IP: ");

#ifdef USE_DHCP
	log_cfg_ln("dynamic (DCHP)");
#else
	if (WiFi.config(local_IP, gateway, subnet))
		log_cfg_ln(IPtoStr(local_IP), " (preconfigured)");
	else
		log_cfg_ln("-- WiFi static IP-address ERROR");
#endif // USE_DHCP

	WiFi.mode(WIFI_STA);
	WiFi.setSleep(false);

#ifdef WIFI_CHANNEL
	log_cfg(" . Looking for SSID: " + String(WIFI_NAME) + ", AP: ");
	log_cfg_ln(strMACaddr(bssid) + " on channel: " + WIFI_CHANNEL);
	WiFi.begin(WIFI_NAME, WIFI_PASS, WIFI_CHANNEL, bssid, true);
#else
	log_cfg_ln(" . Looking for SSID: " + String(WIFI_NAME));
	WiFi.begin(WIFI_NAME, WIFI_PASS);
#endif // WIFI_CHANNEL

	while ((WiFi.status() != WL_CONNECTED) && (cntWiFiErrors < WIFI_TIMEOUT * 2))
	{
		delay(300);
		display_String(".");
		cntWiFiErrors++;
		log_cfg(" .");
		tick_Led();
	}

	if (cntWiFiErrors < WIFI_TIMEOUT * 2)
	{
		log_cfg_ln(" ok");
		log_cfg(" . IP: ", IPtoStr(WiFi.localIP()));
#ifdef USE_DHCP
		log_cfg_ln(" (DHCP)");
#else
		log_cfg_ln(" (static)");
#endif // USE_DHCP
		log_cfg_ln(" . MASK: ", IPtoStr(WiFi.subnetMask()));
		log_cfg_ln(" . GATEWAY: ", IPtoStr(WiFi.gatewayIP()));
		log_cfg_ln(" . AP MAC: ", WiFi.BSSIDstr());
		log_cfg_ln(" + WiFi setup OK");
		display_String("ok");
		display_IP(IPtoStr(WiFi.localIP()));
	}
	else
	{
		log_cfg_ln(" error");
		log_cfg_ln("-- WiFi setup finished with ERROR!");
	}
}

void check_WiFi()
{
	if (WiFi.status() == WL_CONNECTED)
	{
		cntWiFiErrors = 0;
		if (showInfo > 0)
			log_info_ln("+ WiFi ok");
	}
	else
	{
		cntWiFiErrors++;
		log_info_ln("-- WiFi errors: ", cntWiFiErrors);
	}

	if (cntWiFiErrors > WIFI_TIMEOUT)
	{
		log_info_ln(" ");
		log_info_ln("-- WiFi not found. RESTART NOW!");
		CPRINTLN("-- WiFi not found. RESTART NOW!");
		display_Clear();
		display_ErrorWiFi();
		delay(3000);
		ESP.restart();
	}
}

String IPtoStr(IPAddress ipAddr)
{
	return ipAddr.toString();
	/*	WiFi.subnetMask().toString();
		WiFi.gatewayIP().toString();
		static char IPbuff[17] = { "                " };
		sprintf(IPbuff, "%i.%i.%i.%i", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
		return String(IPbuff);
	*/
}

/*
void displayIP()
{
	static char macbuff[17] = { "                " };
	sprintf(macbuff, "%i.%i.%i.%i", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
	LD_printString_6x8("IP:", LCDX1, 7);
	LD_printString_6x8(macbuff, LCDX1 + 22, 7);
}
*/
/*
void serialIP()
{
	log_cfg_ln(" . IP: ", IPtoStr(WiFi.localIP()));
	log_cfg_ln(" . MASK: ", IPtoStr(WiFi.subnetMask()));
	log_cfg_ln(" . GATEWAY: ", IPtoStr(WiFi.gatewayIP()));
	log_cfg_ln(" . AP MAC: ", WiFi.BSSIDstr());
}
*/
