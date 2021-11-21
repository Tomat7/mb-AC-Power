#pragma once

#ifdef ARDUINO_ARCH_ESP32
#warning "Check config files!"
#else
#error "Wrong module defined. Use ESP32 only."
#endif //ARDUINO_ARCH_ESP32

#ifdef USE_3PHASE
#include "ACpower3.h"
ACpower3 TEH(PINS_PHASE0, PINS_PHASE1, PINS_PHASE2, POWER_MAX);
#else
#include <ACpower.h>
ACpower TEH(POWER_MAX, PIN_ZCROSS, PIN_TRIAC, PIN_U, PIN_I);
#endif

#ifdef U_CORRECTION
static float Ucorr[25] = U_CORRECTION;
#else
#define Ucorr NULL
#endif

#include <WiFi.h>
#ifndef USE_DHCP
IPAddress local_IP(ETHERNET_IP);
IPAddress gateway(IP_NETWORK + 254);
IPAddress subnet(255, 255, 255, 0);
#endif


#ifdef USE_OTA
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#endif //USE_OTA

#ifdef USE_WEBSERVER
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
WebServer httpd(80);
#endif //USE_WEBSERVER

#ifdef USE_TELNET
WiFiServer telnetd(23);
WiFiClient rClients[MAX_SRV_CLIENTS];
#endif //USE_WEBSERVER

#ifdef USE_MODBUS
#include <ModbusIP_ESP8266.h>
//#include "Modbus.h"
ModbusIP mb;
#endif //USE_MODBUS

#ifdef USE_OLED
#include <ASOLED.h>
ASOLED LD(SH1106, 400000);
//ASOLED LD(SSD1306);
#endif //USE_OLED

#ifdef USE_LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C LCD(0x27, 20, 4);
#define I2C_SPEED 400000
byte Backslash[8] =
{
  0b00000,
  0b10000,
  0b01000,
  0b00100,
  0b00010,
  0b00001,
  0b00000,
  0b00000
};
#endif //USE_LCD

#ifdef USE_ENCODER
#include <ReadDigKey.h>
ReadDigKey key;
#endif

#ifdef USE_DS18B20
#include <DStemp.h>
DSThermometer DS[] = DS_PIN;
const int nSensor = sizeof(DS) / sizeof(DSThermometer);
float dsTemp[nSensor] = { -99 };
uint8_t dsErrors[nSensor] = { 0 };
#else 
const int nSensor = 0;
float dsTemp[1];
uint8_t dsErrors[1];
#endif
