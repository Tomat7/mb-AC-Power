// func.h
#pragma once

#include "arduino.h"
#include "config.h"
#include "config_plc.h"
#include "logging.h"

extern volatile SemaphoreHandle_t smInfoEnd;

extern uint8_t showInfo;
extern int16_t Pencoder;
extern uint16_t Pweb, Pserial, Premote;
extern uint16_t msPrintPeriod, msUpdatePeriod;
extern uint32_t msPrint, msUpdate;

extern bool isScreenSaver;
extern bool isShowPower;
extern bool isSetMenu;
extern bool isLocalMode;
extern bool isMBmasterOK;

String strUptime();
String strFreeMem();
String strIPaddr();
String strMACaddr();
String strMACaddr(byte mac[6]);
String strWiFiName();
String strWiFiRSSI();
String strWiFiBSSID();

bool needUpdate(int m);
bool needPrint(int m);

uint16_t selectPower();
void setPower(uint16_t Ptmp);

void tick_Led();
void check_ZeroCross();		// переделать на 3 фазы!!
void update_Cooler();

void update_Info();
void print_Info();
void print_Serial();
void print_More();
void print_Debug();

void start_ControlTask();
void stop_ControlTask();
void Control_task(void* parameter);
