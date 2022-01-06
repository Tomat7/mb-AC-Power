// display.h
#pragma once
#include "arduino.h"
#include "config.h"
#include "config_plc.h"
//#include "teh.h"

enum DispScreen
{
	SHOW_POWER,		// присваивается 0
	SHOW_VA,		// присваивается 1
	SHOW_NETCONFIG, // присваивается 2
	SHOW_UPTIME,	// присваивается 3
	SHOW_END,		// присваивается 4
//	COLOR_PINK,		// присваивается 5
//	COLOR_YELLOW,	// присваивается 6
//	COLOR_MAGENTA	// присваивается 7
};

extern DispScreen lcdMode;

void setup_Display(String strVersion, String strVersionCode);
void display_Init();
void display_ReInit();
void display_Clear();
void display_String(String str0);
void display_String(String str0, int X, int Y);
void display_VerDate(char* chVer);
void display_SSID();
void display_MAC();
void display_IP(String IPstr);
void display_PinInfo();
void display_DSpin(int i);
void display_Info();
void display_Power(int16_t Pencoder);
void display_Switch();
void display_ErrorWiFi();
void display_OTAbegin();
void display_OTAprogress(long OtaProgress);
void display_OTAend();
void display_OTAerror(String OTAerr);


#ifdef USE_LCD
void lcd_Init();
void lcd_ReInit();
void lcd_SetBackLight();
void lcd_Clear();
void lcd_String(String str0);
void lcd_String(String str0, int X, int Y);
void lcd_VerDate(char* chVer);
void lcd_LibInfo();
void lcd_Info();
void lcd_Power(const char* Sname, uint16_t Pshow, float Tshow, int z);
void lcd_VA();
void lcd_Status();
void lcd_Spinner();
void lcd_NetConfig();
void lcd_UpTime();
#endif //USE_LCD


#ifdef USE_OLED
void oled_Init();
void oled_Clear();
void oled_String(String str0);
void oled_String(String str0, int X, int Y);
void oled_Info();
void oled_Spinner();
void oled_Temp();
void oled_Power(uint16_t Pshow);
void oled_Status();
void oled_VerDate(char* chVer);
void oled_ACpowerPins();
void oled_DSinfo();
void oled_DSpin(int i);
void oled_Switch();
void oled_ErrorWiFi();
void oled_OTAbegin();
void oled_OTAprogress(long otaPr);
#endif //USE_OLED
