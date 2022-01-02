// ds18b20.h
#pragma once
#include "arduino.h"
#include "config.h"
#include "config_plc.h"

#ifdef USE_DS18B20

extern float dsTemp[];
extern const int nSensor;
extern bool dsIsParasite[];

void ds_Setup();
void ds_Update();
String ds_getLibVersion(int id);
int8_t ds_getTHreg(int id);
int8_t ds_getTLreg(int id);
//bool ds_isParasite(int id);

#endif