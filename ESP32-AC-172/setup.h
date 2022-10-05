// setup.h
#pragma once

#include "arduino.h"
#include "config.h"
#include "config_plc.h"
#include "logging.h"
#include "func.h"
#include "modules.h"

//void setup_LogCfg(char* fname);
void setup_LogCfg(String fname);
void setup_Pins();
void setup_Serial();
void setup_Encoder();
void setup_OTA();
void setup_Web();
void setup_Telnet();
void setup_Modbus();
void setup_PinInfo();
void setup_DS();
void setup_Syslog();

