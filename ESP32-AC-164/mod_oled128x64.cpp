// 
#include "display.h"
#include "logging.h"
#include "macros.h"
#include "network.h"
#include "func.h"
#include "teh.h"
#include "modules.h"

#ifdef USE_OLED
#include <ASOLED.h>
ASOLED LD(SH1106, 400000);
//ASOLED LD(SSD1306);
//DispScreen lcdMode = SHOW_POWER;
#else
#warning "OLED not configured! Nothing to compile. "
#endif //USE_OLED

#ifdef USE_OLED

void oled_Init()
{
	LD.init();
}

void oled_Clear()
{
	LD.clearDisplay();
}

void oled_String(String str0)
{
	LD.printString_6x8(str0.c_str());
}

void oled_String(String str0, int X, int Y)
{
	LD.setCursorXY(X, Y);
	LD.printString_6x8(str0.c_str());
}

void oled_Info()
{
	D(OLEDcore = xPortGetCoreID());
	oled_Clear();
	oled_Spinner();
	oled_Temp();
	oled_Power(TEH.Pset);
	oled_Status();

}

void oled_Spinner()
{
	/*
		const char* bufSpinner[] = { "\\", "|", "/", "-" };
		static int s;

		if ((s < 3) && (isZCdetectedOK)) s++;
		else s = 0;

		LD.printString_12x16(bufSpinner[s], 0, 6);
		LD.printNumber((long)cntWiFiErrors, 20, 6);
	*/
	check_TEH();
	const char* Spinner[4] = { "\\", "|", "/", "-" };

#ifdef USE_3PHASE
	static int s[3];
	for (int p = 0; p < 3; p++)
	{
		if ((s[p] < 3) && TEH.ZC[p])
		{
			s[p]++;
			LD.printString_12x16(Spinner[s[p]], p * 12, 6);
		}
		else if (TEH.ZC[p])
		{
			s[p] = 0;
			LD.printString_12x16(Spinner[s[p]], p * 12, 6);
		}
		else
		{
			LD.printString_12x16(".", p * 12, 6);
		}
	}
#else
	static int s;
	if ((s < 3) && TEH.ZC)
	{
		s++;
		LD.printString_12x16(Spinner[s], 0, 6);
	}
	else if (TEH.ZC)
	{
		s = 0;
		LD.printString_12x16(Spinner[s], 0, 6);
	}
	else
	{
		LD.printString_12x16(".", 0, 6);
	}
#endif
}

void oled_Temp()
{
	char cbuf[] = { "     " };
	//check_WiFi();
	dtostrf(cntWiFiErrors, 3, 0, cbuf);
	LD.printString_12x16(cbuf, 0, 3);

	for (int i = 0; i < (nSensor - 1); i++)
	{
		dtostrf(dsTemp[i], 5, 1, cbuf);
		LD.printString_12x16(cbuf, 0, (i * 3));
		if (dsIsParasite[i]) LD.printString_6x8("`");
		else LD.printString_6x8(" ");
	}
}

void oled_Power(uint16_t Pshow)
{
	char bufPnow[] = { "     " };
	char bufPset[] = { "     " };

	isShowPower = (lcdMode == SHOW_POWER);

	if (isShowPower)		// show P on display
	{
		dtostrf(TEH.Pnow, 4, 0, bufPnow);
		dtostrf(Pshow, 4, 0, bufPset);
	}
	else				// show U&I on display
	{
		dtostrf(TEH_UNOW, 4, 0, bufPnow);
		//dtostrf((round(TEH.Inow * 100) / 100), 5, 2, bufPset);
		dtostrf(TEH_INOW, 5, 2, bufPset);
		LD.printString_12x16("U", 68, 0);
	}

	if (isShowPower)
	{
		LD.printString_12x16(bufPnow, 80, 0);
		LD.printString_12x16(bufPset, 80, 3);
	}
	else
	{
		LD.printString_12x16("-", 80, 0);
		LD.printString_12x16("-", 68, 3);
}
}

void oled_Status()
{
	if (isSetMenu) LD.printString_12x16("_SET_", 69, 6);
	else if (!isLocalMode) LD.printString_12x16("local", 69, 6);

#ifdef USE_MODBUS
	else if (isMBmasterOK) LD.printString_12x16("MB_OK", 69, 6);
#else
	else if (cntWiFiErrors == 0) LD.printString_12x16("Wi_Fi_OK", 69, 6);
#endif
	else LD.printString_12x16(" _OFF", 69, 6);
}

void oled_VerDate(char* chVer)
{
	LD.printString_6x8(chVer, LCDX1, 0);
	LD.printString_6x8(__DATE__, LCDX1, 1);
	log_cfg_ln("+ OLED configured OK.");
#ifdef USE_3PHASE
	LD.printString_6x8("3xPHASE", 72, 1);
#endif //USE_3PHASE
}

void oled_ACpowerPins()
{
#ifdef USE_3PHASE
	uint8_t pins[3][4] = { PINS_PHASE0, PINS_PHASE1, PINS_PHASE2 };
	uint8_t phases = 3;
#else
	uint8_t pins[1][4] = { PIN_ZCROSS, PIN_TRIAC, PIN_I, PIN_U };
	uint8_t phases = 1;
#endif

	String strVer;
	strVer = TEH.LibVersion;
	char chVer[21];
	strVer.toCharArray(chVer, 21);
	//	LD.clearDisplay();
	LD.printString_6x8(chVer, LCDX1, 0);
	LD.printString_6x8(" pins: ZC  TR  I   U", LCDX1, 1);

	for (int i = 0; i < phases; i++)
	{
		LD.printString_6x8("phase", LCDX1, i + 2);
		LD.printNumber((long)i);
		LD.printString_6x8(" ");
		for (int j = 0; j < 4; j++)
		{
			LD.printNumber((long)pins[i][j]);
			if (j < 3) LD.printString_6x8(", ");
		}
	}
}

void oled_DSinfo()
{
	LD.printString_6x8("DS on pin: ", LCDX1, 7);
}

void oled_DSpin(int i)
{
	int pins[] = DS_PIN;
	LD.printNumber((long)pins[i]);
	if (i < (nSensor - 1)) LD.printString_6x8(", ");
}

void oled_Switch()
{
	LD.printString_12x16("switch ...", 2, 6); // индикатор короткого нажатия на энкодер
}

void oled_ErrorWiFi()
{
	LD.printString_12x16("   WiFi error!", LCDX1, 0);
	LD.printString_12x16("   rebooting...", LCDX1, 5);
}

void oled_OTAbegin()
{
	LD.printString_12x16("OTA update", LCDX1, 0);
}

void oled_OTAprogress(long otaPr)
{
	LD.printString_12x16("%% ", 24, 3);
	LD.printNumber((long)otaPr);
}

#endif //USE_OLED
