// 
#include "mod_telnet.h"
#include "network.h"
#include "macros.h"
#include "logging.h"
#include "func.h"
#include "modules.h"


#ifdef USE_TELNET
WiFiServer telnetd(23);
WiFiClient rClients[MAX_SRV_CLIENTS];
String tParam, tVar, slVar;
#else
#pragma message "TELNET not configured! Nothing to compile. "
#endif //USE_TELNET


#ifdef USE_TELNET

void telnet_Setup()
{
	telnetd.begin();
	telnetd.setNoDelay(true);
}

void telnet_Stop(uint8_t y)
{
	String rStop = "RESET ESP32 by Telnet[" + String(y) + "]: disconnecting...";
	send_Syslog(rStop);
	telnet_Send("\r\n" + rStop + "\r\n");
	CPRINTLN(rStop);
	delay(10);
	for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
	{
		if (rClients[i]) rClients[i].stop();
	}
	delay(100);
}

void telnet_Send(String msg0)
{
	for (uint8_t s = 0; s < MAX_SRV_CLIENTS; s++)
	{
		if (rClients[s] && rClients[s].connected())
		{
			telnet_Print(msg0, s);
			delay(1);
		}
	}
}

void telnet_NewClient(uint8_t x)
{
	String rNewconnected = "New Telnet [" + String(x) + "]: " + IPtoStr(rClients[x].remoteIP());
	send_Syslog(rNewconnected);
	telnet_Send("\r\n" + rNewconnected + "\r\n");
	CPRINTLN(rNewconnected);
	delay(10);

	telnet_Print(strConfig, x);
	telnet_Print2("Uptime: ", strUptime(), x);
}

void telnet_LostClient(uint8_t x)
{
	rClients[x].stop();
	send_Syslog("Telnet [" + String(x) + "]: DISCONNECTED");
	CPRINT("Telnet client: ["); CPRINT(x); CPRINT("] DISCONNECTED");
}

void telnet_Check()
{
	uint8_t i;
	uint8_t n = 0;

	//check if there are any NEW clients
	if (telnetd.hasClient())
	{
		CPRINT("hasClient ");
		for (i = 0; i < MAX_SRV_CLIENTS; i++)		//find free/disconnected spot
		{
			CPRINTLN(i);
			if (!rClients[i]) CPRINTLN("noClient");
			if (!rClients[i].connected()) CPRINTLN("notConnected");

			if (!rClients[i] || !rClients[i].connected())
			{
				CPRINTLN("noClient, noConnected");
				if (rClients[i]) telnet_LostClient(i);
				rClients[i] = telnetd.available();	// NEW client!
				
				if (!rClients[i]) CPRINTLN("available broken");	// lost connection while setup client
				else telnet_NewClient(i);
				break;
			}
		}
		if (i >= MAX_SRV_CLIENTS) telnetd.available().stop(); 	//no free/disconnected spot so reject
	}

	//check EXISTING Telnet-clients for data
	for (i = 0; i < MAX_SRV_CLIENTS; i++)
	{
		if (rClients[i] && rClients[i].connected())
		{
			telnet_Input(i);
			n++;
		}
		else if (rClients[i]) telnet_LostClient(i);
	}

	if (!n)
	{
		showInfo = 0;
		msTelnetPeriod = SHOW_PERIOD;
	}
}


void telnet_Input(uint8_t x)
{
	tVar = "";
	slVar = "";
	tParam = "";

	while (rClients[x].available())
	{
		char ch = rClients[x].read();
		tVar += ch;
		if ((ch == '\n') || !rClients[x].available())
		{
			slVar = tVar;
			slVar.remove(tVar.lastIndexOf('\n') - 1);
			send_Syslog("Telnet [" + String(x) + "]: " + slVar);
			tVar.toUpperCase();

			if (tVar.substring(0, 2) == "SI") telnet_gotSI(x);
			else if (tVar.substring(0, 2) == "SP") telnet_gotSP(x);
			else if (tVar.substring(0, 2) == "TH") telnet_gotTH(x);
			else if (tVar.substring(0, 2) == "TL") telnet_gotTL(x);
			else if (tVar.substring(0, 4) == "TREG") telnet_gotTREG(x);
			else if (tVar.substring(0, 3) == "RST") telnet_gotRST(x);
			else if (tVar.substring(0, 4) == "STOP") telnet_gotSTOP(x);
			else if (tVar.substring(0, 4) == "INFO") telnet_gotINFO(x);
			else if (tVar.substring(0, 4) == "MORE") telnet_gotMORE(x);
			else if (tVar.substring(0, 5) == "DEBUG") telnet_gotDEBUG(x);
			else if (tVar.substring(0, 6) == "UPTIME") telnet_gotUPTIME(x);
			else if (tVar.substring(0, 4) == "CONF") telnet_gotCONF(x);
			else if (tVar.substring(0, 4) == "TIME") telnet_gotTIME(x);
			else if (tVar.substring(0, 4) == "EXIT") telnet_gotEXIT(x);
			else telnet_Println("", x);
			tVar = "";
		}
	}
}

#endif	// USE_TELNET

