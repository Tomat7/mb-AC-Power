// ota.h
#include "arduino.h"
#include "config.h"
#include "config_plc.h"

// Внимание! Проверь пароль OTA_PASS в config_wifi.h и в board.txt
#ifdef USE_OTA

void ota_Setup();
void ota_Check();

#endif //USE_OTA

