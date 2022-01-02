// logging.h

#ifndef _LOGGING_h
#define _LOGGING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


extern String strConfig;

void log_cfg_addstr(String str0);
void log_cfg(String str0);
void log_cfg(String str0, String str1);
void log_cfg_ln(String str0);
void log_cfg_ln(String str0, String str1);
void log_info(String str0);
void log_info(String str0, int num1);
void log_info(String str0, float num1);
void log_info_ln(String str0);
void log_info_ln(String str0, String str1);
void log_info_ln(String str0, uint16_t num1);
void log_debug(String str0);
void log_debug_ln(String str0);
void log_debug_f(String str0, int16_t num1);

void send_Telnet(String msg0);

#endif

