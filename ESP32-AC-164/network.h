// network.h
#include "arduino.h"
#include "config.h"
#include "config_plc.h"

#include <WiFi.h>

// Внимание! Проверь пароль WIFI_PASS в config_wifi.h

extern uint16_t cntWiFiErrors;

#ifndef ESP32
#error "Wrong chip! Use ESP32." 
#endif // !ESP32

void setup_Network();   //Config Modbus IP
void initETH();
void check_WiFi();
void displayIP();
void serialIP();

String IPtoStr(IPAddress ipAddr);
