#pragma once
#include "arduino.h"
#include "config.h"
#include "config_plc.h"

#if defined(OLED_SH1106)
#define LCDX1 0           // смещение 1-го "столбца" на экране
#define LCDX2 65          // смещение 2-го "столбца" на экране
#else
#define LCDX1 1           // смещение 1-го "столбца" на экране
#define LCDX2 67          // смещение 2-го "столбца" на экране
#endif

#ifdef SERIAL_DEBUG
#ifndef SERIAL_INFO
#define SERIAL_INFO
#endif
#define DPRINTF(a, ...) Serial.print(a); Serial.println(__VA_ARGS__)
#define DPRINTLN(...) (Serial.println(__VA_ARGS__))
#define DPRINT(...) (Serial.print(__VA_ARGS__))
#else
#define DPRINTF(a, ...)
#define DPRINTLN(...)
#define DPRINT(...)
#endif  // SERIAL_DEBUG

#ifdef SERIAL_INFO
#ifndef SERIAL_CONFIG
#define SERIAL_CONFIG
#endif
#define PRINTF(a, ...) Serial.print(a); Serial.println(__VA_ARGS__)
#define PRINTLN(...) (Serial.println(__VA_ARGS__))
#define PRINT(...) (Serial.print(__VA_ARGS__))
#else
#define PRINTF(a, ...)
#define PRINTLN(xArg)
#define PRINT(zArg)
#endif  // SERIAL_INFO

#ifdef SERIAL_CONFIG
#define CPRINTF(a, ...) Serial.print(a); Serial.println(__VA_ARGS__)
#define CPRINTLN(...) (Serial.println(__VA_ARGS__))
#define CPRINT(...) (Serial.print(__VA_ARGS__))
#else
#define CPRINTF(a, ...)
#define CPRINTLN(xArg)
#define CPRINT(zArg)
#endif  // SERIAL_CONFIG

#ifdef USE_3PHASE
#define TEH_INOW TEH.I[0]
#define TEH_UNOW TEH.U[0]
#define TEH_CounterZC TEH.CounterZC[0]
#define TEH_CounterTR TEH.CounterTR[0]
#else
#define TEH_INOW TEH.Inow
#define TEH_UNOW TEH.Unow
#define TEH_CounterZC TEH.CounterZC
#define TEH_CounterTR TEH.CounterTR
#endif


#ifdef DEV_DEBUG
#define D(a) a
#else
#define D(a)
#endif  // DEV_DEBUG



