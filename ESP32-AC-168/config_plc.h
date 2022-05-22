#pragma once

#if PLC_ID == 37

#define PLC_NAME "PLC-37"
#define SHOW_PERIOD	998				// как часто обновлять информацию на экране и в Serial
#define LCD_SAVER_TIMEOUT 1*60*1000	// Скринсейвер таймаут
#define WIFI_CHANNEL 6

#define USE_DHCP		// если нет USE_DHCP IP-address будет задан на основе PLC_ID.
//#define USE_3PHASE		// использовать 3 фазы (ACpower3 вместо ACpower)
#define USE_MODBUS		// устанавливать мощность из регистров Модбаса
//#define USE_SETSERIAL	// устанавливать мощность с СОМ-порта
//#define USE_WEBSERVER	// устанавливать/показывать мощность с/на WEB-странице
#define USE_SYSLOG		// SYSLOG facility for logging 
#define USE_TELNET		// запусткать telnetd для возможности диагностики (почти бесполезная фича :-)
//#define USE_SETTELNET	// устанавливать мощность с Telnet-подключения
#define USE_ENCODER		// устанавливать мощность с энкодера
#define USE_OLED		// можно работать и без дисплея
#define USE_OTA			// обновление по Wi-Fi (почти бесполезная фича :-)
#define USE_DS18B20		// использовать датчик температуры. коннфигурация ниже
//#define USE_DSCORR		// прописать корректировочные коэффициенты в DS при определенных условиях
//#define USE_TASK_INFO	// control() в отдельном task на время вывода на экран
#define SERIAL_CONFIG   // выводить в COM-порт конфигурацию
//#define SERIAL_INFO		// выводить в COM-порт информацию о работе
//#define SERIAL_DEBUG	// выводить в COM-порт дополнительную информацию
//#define DEV_DEBUG		// скомпилить DEBUG версию и выводить в COM-порт еще больше информации

// ACpower3 config
#define POWER_MAX 3500		// больше этой мощности установить не получится
#define ZCINT_MODE RISING	// по какому фронту отрабатывать прерывание детектора нуля
#define ADC_RATE 200		// количество отсчетов АЦП на ПОЛУволну - 200 (для расчета прерываний)
#define ADC_WAVES 4			// количество обсчитываемых ПОЛУволн 
#define U_RATIO 0.2
#define I_RATIO 0.02
#define ANGLE_LAG 1			// "послабление" при расчете угла - чем дольше тем медленнее нарастание
#define U_CORRECTION {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.5,0.6,0.7,2.8,8.9,12.0,14.1,15.2,17.3,18.4}
//voltage CORRECTION {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,150,160,170,180,190, 200, 210, 220, 230, 240}

// PIN configuration
#ifdef USE_3PHASE
#undef DEV_DEBUG
// phase 0
#define PIN_ZC0 25  // детектор нуля
#define PIN_TR0 26  // триак
#define PIN_I0 36  // датчик тока
#define PIN_U0 39  // датчик напряжения
#define PINS_PHASE0 PIN_ZC0, PIN_TR0, PIN_I0, PIN_U0
// phase 1
#define PIN_ZC1 14  // детектор нуля ??
#define PIN_TR1 27  // триак 
#define PIN_I1 32  // датчик тока
#define PIN_U1 33  // датчик напряжения
#define PINS_PHASE1 PIN_ZC1, PIN_TR1, PIN_I1, PIN_U1
// phase 2
#define PIN_ZC2 13  // детектор нуля
#define PIN_TR2 12  // триак ??
#define PIN_I2 34  // датчик тока
#define PIN_U2 35  // датчик напряжения
#define PINS_PHASE2 PIN_ZC2, PIN_TR2, PIN_I2, PIN_U2
#else
// одно-фазный регулятор
#define PIN_U 39
#define PIN_I 36
#define PIN_ZCROSS 25
#define PIN_TRIAC 26
#endif	// USE_3PHASE

// Encoder config
#define ENCODER_pSW 16		// энкодер SW кнопка
#define ENCODER_pCLK 4		// Pin для энкодера CLK
#define ENCODER_pDT 17		// Pin для энкодера DT
#define ENCODER_STEP 50

#endif	//PLC_ID == 37


#if PLC_ID == 39

#define PLC_NAME "PLC-39"
#define SHOW_PERIOD	998					// как часто обновлять информацию на экране и в Serial
#define LCD_SAVER_TIMEOUT 15*60*1000	// Скринсейвер таймаут
#define WIFI_CHANNEL 11

//#define USE_DHCP		// если нет USE_DHCP IP-address будет задан на основе PLC_ID.
#define USE_3PHASE		// использовать 3 фазы (ACpower3 вместо ACpower)
#define USE_MODBUS		// устанавливать мощность из регистров Модбаса
//#define USE_SETSERIAL	// устанавливать мощность с СОМ-порта
//#define USE_WEBSERVER	// устанавливать/показывать мощность с/на WEB-странице (почти бесполезная фича :-)
#define USE_SYSLOG		// SYSLOG facility logging 
#define USE_TELNET		// запусткать telnetd для возможности диагностики (почти бесполезная фича :-)
//#define USE_SETTELNET	// устанавливать мощность с Telnet-подключения
#define USE_ENCODER		// устанавливать мощность с энкодера
#define USE_LCD			// LCD2004 или
//#define USE_OLED		// OLED 128x64 можно работать и без дисплея
#define USE_OTA			// обновление по Wi-Fi (почти бесполезная фича :-)
#define USE_DS18B20		// использовать датчик температуры. коннфигурация ниже
//#define USE_DSCORR		// прописать корректировочные коэффициенты в DS при определенных условиях
//#define USE_TASK_INFO	// вывод информации в Serial в отдельном task, а не в loop() (не нужно!!)
#define SERIAL_CONFIG   // выводить в COM-порт конфигурацию
//#define SERIAL_INFO		// выводить в COM-порт информацию о работе
//#define SERIAL_DEBUG	// выводить в COM-порт дополнительную информацию
//#define DEV_DEBUG		// скомпилить DEBUG версию и выводить в COM-порт еще больше информации

// ACpower3 config
#define POWER_MAX 9000		// больше этой мощности установить не получится
#define ZCINT_MODE FALLING	// по какому фронту отрабатывать прерывание детектора нуля
#define ADC_RATE 200		// количество отсчетов АЦП на ПОЛУволну - 200-400 (для расчета прерываний)
#define ADC_WAVES 4			// количество обсчитываемых ПОЛУволн 
#define U_RATIO 0.1
#define I_RATIO 0.02
#define ANGLE_LAG 4			// "послабление" при расчете угла - чем больше тем плавнее изменение
//#define U_CORRECTION {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.5,0.6,0.7,2.8,8.9,12.0,14.1,15.2,17.3,18.4}
#define U_CORRECTION { 0, 0, 0, 0, -5, -9 -12,-15,-17,-19,-21,-23,-25,-27,-28,-29,-30,-31,-32,-33,-32,-29,-16, -5,  0 }
//#define U_CORRECTION { 0, 0, 0, 0, -7, -9 -10,-15,-17,-20,-21,-23,-25,-27,-27,-28,-30,-30,-32,-34,-32,-29,-16, -5,  0 }
//voltage CORRECTION { 0,10,20,30, 40, 50, 60, 70, 80, 90,100,110,120,130,140,150,160,170,180,190,200,210,220,230,240 }

// ACpower3 PINs	ZC	TR	I	U
#define PINS_PHASE0	15,	25,	36,	39
#define PINS_PHASE1 14,	26,	32,	33
#define PINS_PHASE2 13,	27,	34,	35

// Encoder config
#define ENCODER_pSW 17		// энкодер SW кнопка
#define ENCODER_pCLK 16		// Pin для энкодера CLK
#define ENCODER_pDT 4		// Pin для энкодера DT
#define ENCODER_STEP 50

#endif	// PLC_ID == 39

#ifndef USE_DHCP
#define IP_NETWORK 192, 168, 1, 0
#define ETHERNET_IP IP_NETWORK + PLC_ID  // задаём IP адрес Ethernet модуля 
#endif	// !USE_DHCP

#define MAX_SRV_CLIENTS 4
