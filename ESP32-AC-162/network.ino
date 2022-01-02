// Внимание! Проверь пароль WIFI_PASS в config_wifi.h

#ifndef ESP32
#error "Wrong chip! Use ESP32." 
#endif // !ESP32

void setup_Network()   //Config Modbus IP
{
	log_cfg_ln("+ WiFi connecting to SSID: ", WIFI_NAME);

	display_MAC();
	log_cfg_ln(" . MAC: ", WiFi.macAddress());

	display_SSID();
	initETH();

	check_WiFi();
	//	if (!cntWiFiErrors) { log_cfg_ln(" + WiFi connected!"); }
	//	else { log_cfg_ln("-- WiFi NOT FOUND! Will try to connect later."); }
	serialIP();
	display_IP(IPtoStr(WiFi.localIP()));
	check_WiFi();

	if (!cntWiFiErrors) { log_cfg_ln(" + WiFi setup OK"); }
	else { log_cfg_ln("-- WiFi setup finished with ERROR"); }
}

void initETH()
{
	//  WiFi.disconnect(true);
#ifdef USE_DHCP
	log_cfg_ln(" . WiFi will use DCHP");
#else
	if (WiFi.config(local_IP, gateway, subnet)) log_cfg_ln(" . WiFi configured static IP");
	else log_cfg_ln("-- WiFi static IP-address ERROR");
#endif // USE_DHCP

	WiFi.mode(WIFI_STA);
	WiFi.setSleep(false);
	WiFi.begin(WIFI_NAME, WIFI_PASS);

	while ((WiFi.status() != WL_CONNECTED) && (cntWiFiErrors < (WIFI_TIMEOUT + WIFI_TIMEOUT)))
	{
		delay(300);
		display_String(".");
		cntWiFiErrors++;
		log_cfg(" .");
		tick_Led();
	}

	display_String("ok");
	log_cfg_ln(" ok");

}

void check_WiFi()
{
	if (WiFi.status() == WL_CONNECTED)
	{
		cntWiFiErrors = 0;
		if (showInfo > 0) log_info_ln("+ WiFi ok");
	}
	else
	{
		cntWiFiErrors++;
		log_info_ln("-- WiFi errors: ", cntWiFiErrors);
	}

	if (cntWiFiErrors > WIFI_TIMEOUT)
	{
		log_info_ln("-- WiFi not found. RESTART NOW!");
		CPRINTLN("-- WiFi not found. RESTART NOW!");
		display_Clear();
		display_ErrorWiFi();
		LD_printString_12x16("WiFi error!", LCDX1, 0);
		LD_printString_6x8("rebooting...", LCDX1, 7);
		delay(3000);
		ESP.restart();
	}

}

String IPtoStr(IPAddress ipAddr)
{
	static char IPbuff[17] = { "                " };
	sprintf(IPbuff, "%i.%i.%i.%i", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
	return String(IPbuff);
}


void displayIP()
{
	static char macbuff[17] = { "                " };
	sprintf(macbuff, "%i.%i.%i.%i", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
	LD_printString_6x8("IP:", LCDX1, 7);
	LD_printString_6x8(macbuff, LCDX1 + 22, 7);
}

void serialIP()
{
	log_cfg_ln(" . IP: ", IPtoStr(WiFi.localIP()));
	log_cfg_ln(" . MASK: ", IPtoStr(WiFi.subnetMask()));
	log_cfg_ln(" . GATEWAY: ", IPtoStr(WiFi.gatewayIP()));
}

