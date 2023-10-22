// telnet.h
#pragma once
#include "arduino.h"
#include "config.h"
#include "config_plc.h"
#include "network.h"

#ifdef USE_TELNET

extern WiFiServer telnetd;
extern WiFiClient rClients[];
extern String tParam, tVar, slVar;

void telnet_Setup();
void telnet_Check();
void telnet_Stop(uint8_t y);
void telnet_Print(String msg0, uint8_t z);
void telnet_Print2(String msg0, String msg1, uint8_t z);
void telnet_Println(String msg0, uint8_t z);
void telnet_Send(String msg0);
void telnet_Input(uint8_t x);

void telnet_gotSI(uint8_t x);
void telnet_gotSP(uint8_t x);

void telnet_gotRST(uint8_t x);
void telnet_gotSTOP(uint8_t x);
void telnet_gotINFO(uint8_t x);
void telnet_gotMORE(uint8_t x);
void telnet_gotDEBUG(uint8_t x);
void telnet_gotUPTIME(uint8_t x);
void telnet_gotCONF(uint8_t x);
void telnet_gotTIME(uint8_t x);
void telnet_gotEXIT(uint8_t x);

void telnet_gotTREG(uint8_t x);
void telnet_gotTH(uint8_t x);
void telnet_gotTL(uint8_t x);

#endif	// USE_TELNET


