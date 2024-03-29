/*
* Оригинальная идея (c) Sebra
* Базовый алгоритм регулирования (c) Chatterbox
* Алгоритм с привязкой расчетов к детектору нуля, поддержка ESP32 и перевод в библиотеку (c) Tomat7
*/

#ifndef ACpower3_macros_h
#define ACpower3_macros_h

#if defined(ESP32)

#define DELAYx vTaskDelay(300 / portTICK_PERIOD_MS)
#endif // ESP32

#ifdef DEBUG3
#define DEBUG2
#define DEBUG1
#define DEBUG0
#define D(a) a
#else
#define D(a)
#endif

#ifdef DEBUG2
#define DEBUG1
#define DEBUG0
#define DPRINTLN(...) (Serial.println(__VA_ARGS__))
#define DPRINT(...) (Serial.print(__VA_ARGS__))
#define DPRINTF(a, ...) Serial.print(a); Serial.println(__VA_ARGS__)
#else
#define DPRINTLN(...)
#define DPRINT(...)
#define DPRINTF(a, ...)
#endif

#ifdef DEBUG1
#define DEBUG0
#define PRINTF(a, ...) Serial.print(a); Serial.println(__VA_ARGS__)
#define PRINTLN(...) (Serial.println(__VA_ARGS__))
#define PRINT(...) (Serial.print(__VA_ARGS__))
#else
#define PRINTLN(...)
#define PRINT(...)
#define PRINTF(a, ...)
#endif


#endif // ACpower3_macros_h
