// telnet.h
#pragma once
#include "arduino.h"
#include "config.h"
#include "config_plc.h"

#ifdef USE_TELNET

void telnet_Setup();
void telnet_Check();
void telnet_Stop();
void telnet_Print(String msg0, uint8_t z);
void telnet_Print1(String msg0, uint8_t z);
void telnet_Print2(String msg0, String msg1, uint8_t z);
void telnet_Println(String msg0, uint8_t z);
void telnet_Info(String msg0);
void telnet_Input(uint8_t x);

#endif	// USE_TELNET


