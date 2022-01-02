// mb.h
#pragma once
#include "arduino.h"
#include "config.h"
#include "config_plc.h"

extern bool isMBmasterOK;

void modbus_Setup();
void modbus_Check();
void modbus_Update();
