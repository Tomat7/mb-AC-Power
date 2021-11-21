#pragma once

TaskHandle_t ControlTask = NULL;
volatile static SemaphoreHandle_t smInfoEnd;
volatile uint16_t cntControlTask = 0;

uint16_t Pweb = 0, Pmodbus = 0, Pserial = 0, Premote = 0;
int16_t Pencoder = 0;
uint32_t msPrint = 0, msUpdate = 0, msKeys = 0, msDisplay = 0, ms0 = 0;
uint32_t msInfoDuration = 0, msDispDuration, msTempDuration, msNetDuration, msDuration1, msPrintDuration;
uint16_t cntWiFiErrors = 0;

bool isShowPower = true;
bool isSetMenu = false;
bool isScreenSaver = false;
bool isLocalMode = false;
bool mbMasterOK = true;
bool zcDetectedOK = false;
volatile uint32_t INFOcore = 6, OLEDcore = 8;

uint8_t showInfo = 0;
uint16_t msPrintPeriod = SHOW_PERIOD, msUpdatePeriod = SHOW_PERIOD;

String strVersion, strVer, strConfig; // , strInfo;
char degC = 223;

enum DispScreen
{
	SHOW_POWER, // присваивается 0
	SHOW_VA, // присваивается 1
	SHOW_NETCONFIG, // присваивается 2
	SHOW_UPTIME, // присваивается 3
	SHOW_END, // присваивается 4
//	COLOR_PINK, // присваивается 5
//	COLOR_YELLOW, // присваивается 6
//	COLOR_MAGENTA // присваивается 7
};
DispScreen lcdMode = SHOW_POWER;

