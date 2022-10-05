// mod_syslog.h
#pragma once
#include "arduino.h"
#include "config.h"
#include "config_plc.h"

#ifdef USE_SYSLOG
extern IPAddress syslog_ip;
void syslog_Setup();
void syslog_Info(String msg0);
#endif //USE_SYSLOG