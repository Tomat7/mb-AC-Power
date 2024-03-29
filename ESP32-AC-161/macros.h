#ifndef _MACROS_h
#define _MACROS_h

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

#ifdef DEV_DEBUG
#define D(a) a
#else
#define D(a)
#endif  // DEV_DEBUG

#ifdef USE_OLED
#ifdef DEV_DEBUG
#define LD_DEBUG(a) LD.printString_12x16("  ", 36, 6); LD.printString_12x16(a, 36, 6)
#define LD_DEBUGM(a) LD.printString_6x8("  ", 20, 6); LD.printString_6x8(a, 20, 6)
#else
#define LD_DEBUG(a)
#define LD_DEBUGM(a)
#endif  // DEV_DEBUG
#define LD_init() LD.init()
#define LD_clearDisplay() LD.clearDisplay()
#define LD_printNumber(a) LD.printNumber(a)
#define LD_printString_6x8(a,b,c) LD.printString_6x8(a, b, c)
#define LD_printString_12x16(a,b,c) LD.printString_12x16(a, b, c)
#define LD_printChar_6x8(a) LD.printString_6x8(a)
#define LD_printChar_12x16(a) LD.printString_12x16(a)
#else
#define LD_init()
#define LD_clearDisplay()
#define LD_printNumber(a)
#define LD_printString_6x8(a,b,c)
#define LD_printString_12x16(a,b,c)
#define LD_printChar_6x8(a)
#define LD_printChar_12x16(a)
#define LD_DEBUG(a)
#define LD_DEBUGM(a)
#endif  // USE_OLED


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

/*
#define DISP_init() LD.init()
#define DISP_clear() LD.clearDisplay()
#define DISP_Number(a) LD.printNumber(a)
#define DISP_String_6x8(a,b,c) LD.printString_6x8(a, b, c)
#define DISP_String_12x16(a,b,c) LD.printString_12x16(a, b, c)
#define DISP_Char_6x8(a) LD.printString_6x8(a)
#define DISP_Char_12x16(a) LD.printString_12x16(a)
*/

#define INCLUDE_vTaskDelay 1

#define DELAYx vTaskDelay(300 / portTICK_PERIOD_MS)

#define WAIT100 vTaskDelay(100 / portTICK_PERIOD_MS)
#define WAIT200 vTaskDelay(200 / portTICK_PERIOD_MS)
#define WAIT300 vTaskDelay(300 / portTICK_PERIOD_MS)

#endif //_MACROS_h
