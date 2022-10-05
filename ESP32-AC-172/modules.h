// modules.h
#pragma once

#include "arduino.h"
#include "config.h"
#include "config_plc.h"
#include "display.h"

#ifdef USE_MODBUS
#include "mod_modbus.h"
#endif

#ifdef USE_DS18B20
#include "mod_ds18b20.h"
#else
extern float dsTemp[];
extern const int nSensor;
extern bool dsIsParasite[];
#endif

#ifdef USE_ENCODER
#include "mod_encoder.h"
#endif

#ifdef SERIAL_CONFIG
#include "mod_serial.h"
#endif

#ifdef USE_TELNET
#include "mod_telnet.h"
#endif

#ifdef USE_OTA
#include "mod_ota.h"
#endif

#ifdef USE_WEBSERVER
#include "mod_web.h"
#endif

#ifdef USE_SYSLOG
#include "mod_syslog.h"
#endif

void update_DS();
void correct_DS();
void update_Modbus();
void check_Serial();
void check_Modbus();
void check_Encoder();
void check_Web();
void check_OTA();
void check_Telnet();
void send_Telnet(String msg0);
void send_Syslog(String msg0);
