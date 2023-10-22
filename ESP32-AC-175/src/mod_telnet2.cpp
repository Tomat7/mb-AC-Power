//
#include "mod_telnet.h"
#include "network.h"
#include "macros.h"
#include "logging.h"
#include "func.h"
#include "modules.h"

#ifdef USE_TELNET

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

// =======================

void telnet_gotSI(uint8_t x)
{
	tParam = tVar.substring(tVar.indexOf("SI", 2) + 3);
	float ShowInterval = tParam.toFloat();
	msTelnetPeriod = (uint16_t)(ShowInterval);
	CPRINTF("Set ShowPeriod from Telnet: ", ShowInterval);
	telnet_Print("ShowPeriod set to: ", x);
	telnet_Println(String(ShowInterval), x);
}

void telnet_gotSP(uint8_t x)
{
#ifdef USE_SETTELNET
	T1 = Var.substring(Var.indexOf("SP", 2) + 3);
	float Ptelnet = T1.toFloat();
	CPRINTF("Set Power from Telnet: ", Ptelnet);
	send_Syslog("Set power from Telnet " + String(x) + ": " + String(Ptelnet));
	telnet_Print("Power set to: ", x);
	telnet_Println(String(Ptelnet), x);
	setPower((uint16_t)Ptelnet);
#else
	CPRINTF("Set Power from Telnet: UNSUPPORTED");
	send_Syslog("Set power from Telnet: UNSUPPORTED!");
	telnet_Println("Set power UNSUPPORTED! ", x);
#endif
}

// ================================

void telnet_gotRST(uint8_t x)
{
	telnet_Stop(x);
	delay(1000);
	ESP.restart();
}

void telnet_gotSTOP(uint8_t x)
{
	showInfo = 0;
	CPRINTLN("Stop... ");
	telnet_Println("Stop...", x);
}

void telnet_gotINFO(uint8_t x)
{
	showInfo = 1;
	CPRINTLN("Info... ");
	telnet_Println("Info...", x);
}

void telnet_gotMORE(uint8_t x)
{
	showInfo = 2;
	CPRINTLN("More... ");
	telnet_Println("More...", x);
}

void telnet_gotDEBUG(uint8_t x)
{
	showInfo = 3;
	CPRINTLN("Debug... ");
	telnet_Println("Debug...", x);
}

void telnet_gotUPTIME(uint8_t x)
{
	CPRINTF("Uptime: ", strUptime());
	telnet_Print2("Uptime: ", strUptime(), x);
	CPRINTF("Free memory: ", strFreeMem());
	telnet_Print2("Free memory: ", strFreeMem(), x);
	telnet_Println("", x);
}

void telnet_gotCONF(uint8_t x)
{
	CPRINTLN("Config... ");
	telnet_Println(strConfig, x);
}

void telnet_gotTIME(uint8_t x)
{
	//Telnet_println(strGetLocalTime(), x);
	//showMore = true;
}

void telnet_gotEXIT(uint8_t x)
{
	telnet_Print("Goodbye...", x);
	String rDisconnected = "Telnet [" + String(x) + "]: disconnected";
	send_Syslog(rDisconnected);
	telnet_Send("\r\n" + rDisconnected + "\r\n");
	CPRINTLN(rDisconnected);
	delay(10);
	rClients[x].stop();
}

// =============================

void telnet_gotTREG(uint8_t x)
{
#ifdef USE_DSCORR
	for (int i = 0; i < nSensor; i++)
	{
		CPRINTF("Get TRegs from DSs", );
		telnet_Print("Tregs on DS" + String(i) + " TH: " + String(ds_getTHreg(i)) + " TL: " + String(ds_getTLreg(i)), x);
		telnet_Print("\r\n", x);
	}
	telnet_Println("", x);
#endif	// USE_DSCORR
}

void telnet_gotTH(uint8_t x)
{
#ifdef USE_DSCORR
	tParam = tVar.substring(tVar.indexOf("TH", 2) + 3);
	slVar = tParam.substring(0, 1);
	int DSid = slVar.toInt();
	slVar = tParam.substring(2);
	int TH = slVar.toInt();
	if ((DSid < nSensor) && (tParam.substring(1, 2) == "="))
	{
		CPRINTF("Set TH from Telnet: ", TH);
		telnet_Print("TH on DS" + String(DSid), x);
		telnet_Println(" set to: " + String(TH / 1000.0, 3), x);
		ds_setTregs(DSid, 12, (int8_t)TH, ds_getTLreg(DSid));
	}
	else telnet_Println("DS id ERROR " + String(DSid) + tParam.substring(1, 2) + String(TH), x);
#endif	// USE_DSCORR
}

void telnet_gotTL(uint8_t x)
{
#ifdef USE_DSCORR
	tParam = tVar.substring(tVar.indexOf("TL", 2) + 3);
	slVar = tParam.substring(0, 1);
	int DSid = slVar.toInt();
	slVar = tParam.substring(2);
	int TL = slVar.toInt();
	if ((DSid < nSensor) && (tParam.substring(1, 2) == "="))
	{
		CPRINTF("Set TL from Telnet: ", TL);
		telnet_Print("TL on DS" + String(DSid), x);
		telnet_Println(" set to: " + String(TL / 100.0), x);
		ds_setTregs(DSid, 12, ds_getTHreg(DSid), (int8_t)TL);
	}
	else telnet_Println("DS id ERROR " + String(DSid) + tParam.substring(1, 2) + String(TL), x);
#endif	// USE_DSCORR
}


#endif	// USE_TELNET
