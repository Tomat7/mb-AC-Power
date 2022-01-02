// display.cpp

#include "display.h"
#include "logging.h"
#include "func.h"
#include "modules.h"

DispScreen lcdMode = SHOW_POWER;

void setup_Display(String strVersion, String strVersionCode)
{
	String strVer;
	display_Init();
	display_Clear();

	strVer = strVersion.substring((1 + strVersion.lastIndexOf('\\')), strVersion.lastIndexOf('.'));
	strVer += " v";
	strVer += strVersionCode;
	char chVer[21];
	strVer.toCharArray(chVer, 21);
	display_VerDate(chVer);
}

void display_Init()
{
#if defined USE_OLED
	oled_Init();
#elif defined USE_LCD
	lcd_Init();
#endif
}

void display_Clear()
{
#if defined USE_OLED
	oled_Clear();
#elif defined USE_LCD
	lcd_Clear();
#endif
}

void display_String(String str0)
{
#if defined USE_OLED
	oled_String(str0);
#elif defined USE_LCD
	lcd_String(str0);
#endif
}

void display_String(String str0, int X, int Y)
{
#if defined USE_OLED
	oled_String(str0, X, Y);
#elif defined USE_LCD
	lcd_String(str0, X, Y);
#endif
}

void display_VerDate(char* chVer)
{
#if defined(USE_OLED)
	oled_VerDate(chVer);
#elif defined(USE_LCD)
	lcd_VerDate(chVer);
#else
	log_cfg_ln("-- NO any display in this version.");
#endif
}

void display_SSID()
{
#if defined(USE_OLED)
	oled_String(WIFI_NAME, 0, 5);
#elif defined USE_LCD
	lcd_String(strWiFiName(), 0, 3);
#endif 
}

void display_MAC()
{
#if defined(USE_OLED)
	oled_String(strMACaddr(), 15, 3);
#elif defined USE_LCD
	lcd_String(strMACaddr(), 2, 2);
#endif
}

void display_IP(String IPstr)
{
#if defined(USE_OLED)
	oled_String(IPstr, 0, 7);
#elif defined(USE_LCD)
	//	lcd_String(IPstr);
#endif
}

void display_PinInfo()
{
#if defined(USE_OLED)
	oled_Clear();
	oled_ACpowerPins();
	oled_DSinfo();
#elif defined(USE_LCD)
	lcd_Clear();
	lcd_String(strIPaddr(), 0, 0);
	lcd_LibInfo();
#endif
}

void display_DSpin(int i)
{
#if defined(USE_OLED)
	oled_DSpin(i);
#elif defined(USE_LCD)
	//	lcd_DSpin(i);
#endif
}

void display_Info()
{
#if defined(USE_OLED)
	oled_Info();
#elif defined(USE_LCD)
	lcd_Info();
#endif
}

void display_Power(int16_t Pencoder)
{
#ifdef USE_OLED
	oled_Power(Pencoder);
#elif defined USE_LCD
	lcd_Power("Penc", Pencoder, dsTemp[1], 1);
#endif
}

void display_Switch()
{
#if defined(USE_OLED)
	oled_Switch();
#elif defined USE_LCD
	lcd_String("switch...", 10, 3);
#endif
}

void display_ErrorWiFi()
{
#if defined(USE_OLED)
	oled_ErrorWiFi();
#elif defined(USE_LCD)
	lcd_String("    WiFi error!", 0, 1);
	lcd_String("    rebooting...", 0, 3);
#endif
}

void display_OTAbegin()
{
#if defined(USE_OLED)
	oled_Clear();
	oled_String("  OTA update", 0, 0);;
#elif defined USE_LCD
	lcd_Clear();
	lcd_String("Begin OTA...", 5, 0);
#endif
}

void display_OTAend()
{
#if defined(USE_OLED)
	oled_Clear();
	oled_String("  OTA OK!", 0, 6);
#elif defined USE_LCD
	lcd_Clear();
	lcd_String("+ OTA Ok!", 5, 1);
#endif
}

void display_OTAerror(String OTAerr)
{
#if defined(USE_OLED)
	oled_Clear();
	oled_String("  OTA error", 0, 2);
	oled_String(OTAerr, 0, 4);
	oled_String("rebooting...", 0, 6);
#elif defined USE_LCD
	lcd_Clear();
	lcd_String("- OTA error", 5, 1);
	lcd_String(OTAerr, 5, 2);
	lcd_String("rebooting...", 5, 3);
#endif
}


void display_OTAprogress(long OtaProgress)
{
#if defined(USE_OLED)
	oled_OTAprogress(OtaProgress);
#elif defined USE_LCD
	lcd_String("%% ", 8, 2);
	lcd_String(String(OtaProgress), 12, 2);
#endif
}


