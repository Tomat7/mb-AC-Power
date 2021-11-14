#ifdef USE_TELNET

void telnet_Setup()
{
	telnetd.begin();
	telnetd.setNoDelay(true);
}

void telnet_Check()
{
	uint8_t i;

	//check if there are any NEW clients
	if (telnetd.hasClient())
	{
		for (i = 0; i < MAX_SRV_CLIENTS; i++)				//find free/disconnected spot
		{

			if (!rClients[i] || !rClients[i].connected())
			{
				if (rClients[i]) rClients[i].stop();
				rClients[i] = telnetd.available();
				if (!rClients[i]) CPRINTLN("available broken");

				CPRINT("New client: "); CPRINT(i); CPRINT(' '); CPRINTLN(rClients[i].remoteIP());
				telnet_Print(strConfig, i);
				telnet_Print2("Uptime: ", strUptime(), i);
				break;
			}
		}
		if (i >= MAX_SRV_CLIENTS) { telnetd.available().stop(); }	//no free/disconnected spot so reject
	}

	for (i = 0; i < MAX_SRV_CLIENTS; i++)		//check Telnet-clients for data
	{
		if (rClients[i] && rClients[i].connected()) { telnet_Input(i); }
		else { if (rClients[i]) rClients[i].stop(); }

		if (!rClients[i] || !rClients[i].connected())
		{
			showInfo = 0;
			msShowPeriod = SHOW_PERIOD;
		}
	}
}

void telnet_Stop()
{
	CPRINTLN("- TELNET: stop all connection.");
	for (uint8_t i = 0; i < MAX_SRV_CLIENTS; i++)
	{
		if (rClients[i]) rClients[i].stop();
	}
	delay(100);
}

void telnet_Print(String msg0, uint8_t z)
{
	uint32_t len0 = msg0.length();
	//	char ch0[len0];
	//	msg0.toCharArray(ch0, len0);
	rClients[z].write(msg0.c_str(), len0);
}

void telnet_Print1(String msg0, uint8_t z)
{
	telnet_Print("\r\n", z);
	telnet_Println(msg0, z);
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
	telnet_Print("PLC-" + String(PLC_ID) + "/> ", z);
}

void telnet_Info(String msg0)
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
	String T1, Var;

	while (rClients[x].available())
	{
		char ch = rClients[x].read();
		Var += ch;
		if ((ch == '\n') || !rClients[x].available())
		{
			Var.toUpperCase();        // ??
			if (Var.substring(0, 2) == "SI")
			{
				T1 = Var.substring(Var.indexOf("SI", 2) + 3);
				float ShowInterval = T1.toFloat();
				CPRINTF("Set ShowPeriod from Telnet: ", ShowInterval);
				telnet_Print("ShowPeriod set to: ", x);
				telnet_Println(String(ShowInterval), x);
				msShowPeriod = (uint16_t)(ShowInterval);
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
				CPRINTLN("Restarting... ");
				telnet_Println("Restarting...", x);
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
			else
			{
				telnet_Print("PLC-" + String(PLC_ID) + "/> ", x);
				//ch = rClients[x].read();
				//rClients[x].flush();
			}
			Var = "";
		}
	}
}

#endif	// USE_TELNET

