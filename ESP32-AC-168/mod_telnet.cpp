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
#else
#warning "TELNET not configured! Nothing to compile. "
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
				if (rClients[i])
				{
					rClients[i].stop();
					send_Syslog("Telnet [" + String(i) + "]: DISCONNECTED");
					CPRINT("Telnet client: ["); CPRINT(i); CPRINT("] DISCONNECTED");
				}

				rClients[i] = telnetd.available();	// NEW client!
				if (!rClients[i]) CPRINTLN("available broken");	// lost connection while setup client

				String rNewconnected = "New Telnet [" + String(i) + "]: " + IPtoStr(rClients[i].remoteIP());
				send_Syslog(rNewconnected);
				telnet_Send("\r\n" + rNewconnected + "\r\n");
				CPRINTLN(rNewconnected);
				delay(10);
								
				telnet_Print(strConfig, i);
				telnet_Print2("Uptime: ", strUptime(), i);
				break;
			}
		}
		if (i >= MAX_SRV_CLIENTS) { telnetd.available().stop(); }	//no free/disconnected spot so reject
	}

	//check EXISTING Telnet-clients for data
	for (i = 0; i < MAX_SRV_CLIENTS; i++)	
	{
		if (rClients[i] && rClients[i].connected()) 
		{ 
			telnet_Input(i); 
			n++;
		}
		else if (rClients[i])
		{
			rClients[i].stop();
			send_Syslog("Telnet [" + String(i) + "]: DISCONNECTED");
			CPRINT("Telnet client: ["); CPRINT(i); CPRINT("] DISCONNECTED");
		}
		/*
		if (!rClients[i] || !rClients[i].connected())
		{
			showInfo = 0;
			msPrintPeriod = SHOW_PERIOD;
		} */
	}
	
	if (!n)
	{
		showInfo = 0;
		msTelnetPeriod = SHOW_PERIOD;
	}
}



void telnet_Print(String msg0, uint8_t z)
{
	uint32_t len0 = msg0.length();
	rClients[z].write(msg0.c_str(), len0);
}

void telnet_Print2(String msg0, String msg1, uint8_t z)
{
	telnet_Print(msg0, z);
	telnet_Print(msg1 + "\r\n", z);
}

void telnet_Println(String msg0, uint8_t z)
{
	msg0 += "\r\n";
	telnet_Print(msg0, z);
	telnet_Print(String(PLC_NAME) + "[" + String(z) + "]" + "/" + String(showInfo) + "> ", z);
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

void telnet_Input(uint8_t x)
{
	String T1, Var, slVar;

	while (rClients[x].available())
	{
		char ch = rClients[x].read();
		Var += ch;
		if ((ch == '\n') || !rClients[x].available())
		{
			slVar = Var;
			slVar.remove(Var.lastIndexOf('\n') - 1);
			send_Syslog("Telnet [" + String(x) + "]: " + slVar);

			Var.toUpperCase();        // ??
			if (Var.substring(0, 2) == "SI")
			{
				T1 = Var.substring(Var.indexOf("SI", 2) + 3);
				float ShowInterval = T1.toFloat();
				msTelnetPeriod = (uint16_t)(ShowInterval);
				CPRINTF("Set ShowPeriod from Telnet: ", ShowInterval);
				telnet_Print("ShowPeriod set to: ", x);
				telnet_Println(String(ShowInterval), x);
			}
#ifdef USE_SETTELNET
			else if (Var.substring(0, 2) == "SP")
			{
				T1 = Var.substring(Var.indexOf("SP", 2) + 3);
				float Ptelnet = T1.toFloat();
				CPRINTF("Set Power from Telnet: ", Ptelnet);
				telnet_Print("Power set to: ", x);
				telnet_Println(String(Ptelnet), x);
				setPower((uint16_t)Ptelnet);
			}
#endif
			else if (Var.substring(0, 3) == "RST")
			{
			//	CPRINTLN("Restarting... ");
			//	telnet_Println("Restarting...", x);
				telnet_Stop(x);
				delay(1000);
				ESP.restart();
			}
			else if (Var.substring(0, 4) == "STOP")
			{
				showInfo = 0;
				CPRINTLN("Stop... ");
				telnet_Println("Stop...", x);
			}
			else if (Var.substring(0, 4) == "INFO")
			{
				showInfo = 1;
				CPRINTLN("Info... ");
				telnet_Println("Info...", x);
			}
			else if (Var.substring(0, 4) == "MORE")
			{
				showInfo = 2;
				CPRINTLN("More... ");
				telnet_Println("More...", x);
			}
			else if (Var.substring(0, 5) == "DEBUG")
			{
				showInfo = 3;
				CPRINTLN("Debug... ");
				telnet_Println("Debug...", x);
				//				Serial.println(strUptime());
				//				telnet_Print1(strUptime(), x);
								//showDebug = true;
			}
			else if (Var.substring(0, 6) == "UPTIME")
			{
				//Serial.println("Uptime... ");
				//Telnet_println("Uptime...", x);
				CPRINTF("Uptime: ", strUptime());
				telnet_Print2("Uptime: ", strUptime(), x);
				CPRINTF("Free memory: ", strFreeMem());
				telnet_Print2("Free memory: ", strFreeMem(), x);
				telnet_Println("", x);
				//showDebug = true;
			}
			else if (Var.substring(0, 4) == "CONF")
			{
				CPRINTLN("Config... ");
				telnet_Println(strConfig, x);
			}
			else if (Var.substring(0, 4) == "TIME")
			{
				//Telnet_println(strGetLocalTime(), x);
				//showMore = true;
			}
			else if (Var.substring(0, 4) == "EXIT")
			{
				telnet_Print("Goodbye...", x);
				String rDisconnected = "Telnet [" + String(x) + "]: disconnected";
				send_Syslog(rDisconnected);
				telnet_Send("\r\n"+rDisconnected +"\r\n");
				CPRINTLN(rDisconnected);
				delay(10);
				rClients[x].stop();
			}
			else
			{
				//telnet_Print("PLC-" + String(PLC_ID) + "/> ", x);
				//telnet_Print(String(PLC_NAME) + "[" + String(z) + "]" + "/" + String(showInfo) + "> ", z);
				telnet_Println("", x);
				//ch = rClients[x].read();
				//rClients[x].flush();
			}
			Var = "";
		}
	}
}

#endif	// USE_TELNET

