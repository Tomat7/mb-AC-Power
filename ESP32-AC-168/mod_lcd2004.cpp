// 
#include "display.h"
#include "func.h"
#include "logging.h"
#include "modules.h"
#include "teh.h"
#include "Wire.h"

#ifdef USE_LCD
#include <LiquidCrystal_I2C.h>
#define I2C_SPEED 400000
//LiquidCrystal_I2C LCD(0x27);
LiquidCrystal_I2C LCD(0x27, 20, 4);
//DispScreen lcdMode = SHOW_POWER;
char degC = 223;
byte Backslash[8] = { 0b00000, 0b10000, 0b01000, 0b00100, 0b00010, 0b00001, 0b00000, 0b00000 };
#else
#warning "LCD not configured! Nothing to compile. "
#endif //USE_LCD


#ifdef USE_LCD

void lcd_Init()
{
	LCD.init();
	LCD.backlight();
//	Wire.setClock(I2C_SPEED);
	LCD.createChar(7, Backslash);
}

void lcd_ReInit()
{
	LCD.reinit();
//	LCD.backlight();
	//	Wire.setClock(I2C_SPEED);
//	LCD.createChar(7, Backslash);
}

void lcd_SetBackLight()
{
	static bool isBacklight = true;

	if (isScreenSaver)
	{
		lcdMode = static_cast<DispScreen>(static_cast<int>(lcdMode) + 1);
		if (TEH.Pset && isBacklight && (lcdMode == SHOW_NETCONFIG)) lcdMode = SHOW_POWER;

		if		(!TEH.Pset && isBacklight)
		{ 
			LCD.noBacklight(); 
			isBacklight = false; 
		}
		else if (TEH.Pset && !isBacklight)
		{ 
			LCD.backlight();
			isBacklight = true; 
		}
	}
	else if (!isBacklight) 
	{ 
		LCD.backlight(); 
		isBacklight = true; 
	}

	return;
}

void lcd_Clear()
{
	LCD.clear();
}


void lcd_String(String str0)
{
	LCD.print(str0);
}

void lcd_String(String str0, int X, int Y)
{
	LCD.setCursor(X, Y);
	LCD.print(str0);
}


void lcd_VerDate(char* chVer)
{
	lcd_String(chVer, 0, 0);
	lcd_String(__DATE__, 0, 1);
	log_cfg_ln("+ LCD configured OK.");
#ifdef USE_3PHASE
	lcd_String("3xPHASE", 13, 1);
#endif //USE_3PHASE
}

void lcd_LibInfo()
{
	lcd_String(TEH.LibVersion + " ", 0, 2);
	lcd_String(ds_getLibVersion(0) + "   ", 0, 3);
}

void lcd_Info()
{
	//	lcd_Clear();
	lcd_SetBackLight();

	if (lcdMode == SHOW_END) lcdMode = SHOW_POWER;

	if (lcdMode == SHOW_POWER)
	{
		lcd_Power("Pnow",  TEH.Pnow,  dsTemp[0], 0);
		lcd_Power("Pset",  TEH.Pset,  dsTemp[1], 1);
		lcd_Power("Angle", TEH.Angle, dsTemp[2], 2);
		lcd_Spinner();
		lcd_Status();
	}
	else if (lcdMode == SHOW_VA)		{ lcd_VA(); lcd_Spinner(); lcd_Status(); }
	else if (lcdMode == SHOW_NETCONFIG) { lcd_NetConfig(); }
	else if (lcdMode == SHOW_UPTIME)	{ lcd_UpTime(); lcd_LibInfo(); }

	return;
}

void lcd_Power(const char* Sname, uint16_t Pshow, float Tshow, int z)
{
	check_TEH();
	LCD.setCursor(0, z);
	if (z == 2) LCD.printf("              %5.2f%c", Tshow, degC);
	else LCD.printf("%s   %4dw  %5.2f%c", Sname, Pshow, Tshow, degC);
	return;
}

/* old style
void lcd_Power(const char* Sname, uint16_t Pshow, float Tshow, int z)
{
	char degC = 223;
	char buff0[21];
	int Tint = (int)Tshow;                        // compute the integer part of the float
	int Tfra = (int)((Tshow - (float)Tint) * 100);
	snprintf(buff0, 20, "%s %4dw  %3i.%02i%c ", Sname, Pshow, Tint, Tfra, degC);
	LCD.setCursor(0, z);
	LCD.print(buff0);
}
*/

void lcd_VA()
{
	check_TEH();
	for (int i = 0; i < 3; i++)
	{
		LCD.setCursor(0, i);
		LCD.printf("%4dw   %3dv  %5.2fa", TEH.P[i], (int)TEH.U[i], TEH.I[i]);
	}

	return;
}

void lcd_Status()
{
	check_TEH();
	String strStatus;
	if (isSetMenu) strStatus = "  _SET_";
	else if (isLocalMode) strStatus = "  local";

#ifdef USE_MODBUS
	else if (isMBmasterOK) strStatus = "  MB_Ok";
#else
	else if (cntWiFiErrors == 0) strStatus = "WiFi_Ok";
#endif
	else strStatus = "  _OFF_";

	LCD.setCursor(3, 3);
	LCD.printf("    %5d %s", TEH.Angle, strStatus.c_str());

	return;
}

void lcd_Spinner()
{
	check_TEH();
	const char* Spinner[4] = { "\\", "|", "/", "-" };
	static int s[3];
	LCD.setCursor(0, 3);

	for (int p = 0; p < 3; p++)
	{
		if ((s[p] < 3) && TEH.ZC[p])
		{
			s[p]++;
			LCD.print(Spinner[s[p]]);
		}
		else if (TEH.ZC[p])
		{
			s[p] = 0;
			LCD.write(byte(7)); //print our custom char backslash
		}
		else
		{
			LCD.print(".");
		}
	}

	return;
}


void lcd_NetConfig()
{
	lcd_String("    " + strIPaddr() + "    ", 0, 0);
	lcd_String("  " + strMACaddr()+ " ", 0, 1);
	lcd_String("Telnet TCP-port 23  ", 0, 2);
	lcd_String("Modbus TCP-port 502 ", 0, 3);
}

void lcd_UpTime()
{
	lcd_String(strUptime() + "   ", 0, 0);
	lcd_String("FreeMem" + strFreeMem(), 0, 1);
}

#endif //USE_LCD
