// ota.h
#pragma once
#include "arduino.h"
#include "config.h"
#include "config_plc.h"

// Внимание! Проверь пароль OTA_PASS в config_wifi.h и в board.txt
#ifdef USE_OTA

#define OTA_HOSTNAME PLC_NAME " POWER_v" VERSION_CODE

void ota_Setup();
void ota_Check();

#endif //USE_OTA
