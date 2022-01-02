#pragma once
#include <esp_arduino_version.h>
#include <core_version.h>

#define PLC_ID 37
#define VERSION_CODE "163"

// PLC configuration // *** Moved to config_plc.h ***
// PIN configuration // *** Moved to config_plc.h ***
// Коэффициенты для измерителей тока и напряжения // *** Перенесено в config_plc.h ***
// Смещение нуля датчиков тока и напряжения - калибруются в библиотеке ACpower

#define SHOW_PERIOD	1000				// как часто обновлять информацию на экране и в Serial
#define SKETCHINFO __DATE__ " " __TIME__ " " __FILE__

#ifdef ESP_ARDUINO_VERSION_MAJOR
#define ESP32_ARDUINO_VERSION ESP_ARDUINO_VERSION_MAJOR + "." + \
		ESP_ARDUINO_VERSION_MINOR + "." + ESP_ARDUINO_VERSION_PATCH
#else 
#define ESP32_ARDUINO_VERSION ARDUINO_ESP32_RELEASE
#endif

#ifdef BUILTIN_LED
#define TICKTOCK_LED BUILTIN_LED
#else
#define TICKTOCK_LED 5
#endif

// Cooler fans pins 
#define COOLER_PIN 2
//#define PIN_COOLER2 12
#define COOLER_DS_IDX 1
#define COOLER_TEMP_START 30

// DS18B20 configuration
#define DS_PIN { 18, 23, 19 }    // pin на котором ds18b20 (не забывайте про подтягивающий резистор 4.7 кОм!)
#define DS_CONVTIME 800
#define DS_ERRORS 2
#define DS_MIN_T -55

// WEB page "button names"
#define WEB_POWER1 100
#define WEB_POWER2 500
#define WEB_POWER3 1000
#define WEB_POWER4 1500
#define WEB_POWER5 1700
#define WEB_POWER6 2400
#define WEB_POWER7 3000

// Modbus registers config
#define hrSECONDS 0
#define hrDSTEMP0 1
#define hrPSET 4
#define hrPNOW 5
#define hrANGLE 6
#define hrX0 7
#define hrX1 8
#define hrX2 9
#define hrP0 10
#define hrI0 hrP0 + 3
#define hrU0 hrI0 + 3

// Network & Timeouts
#define MB_TIMEOUT 30		// сколько можно работать без Мастера не сбрасывая мощность
#define WIFI_TIMEOUT 10		// сколько можно работать без WIFI до перезагрузки

#define WIFI_NAME "Tomat1"
#define WIFI_PASS "filimon7"
#define OTA_PASS "filimon7"
#define OTA_PORT 8266

