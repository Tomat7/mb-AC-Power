#define SKETCHNAME __FILE__ " " __DATE__ " " __TIME__
#define SHOWVERSION "v" SKETCHVERSION " " __DATE__
#define DS_CONVTIME 750   // как часто опрашивать DS18B20 (миллисекунды)
#define MB_TIMEOUT 70     // как долго можно работать/кликать_клапаном без мастера Модбаса (секунды)
#define msSHOWCONFIG 3000 // длительность показа конфигурации на дисплее при старте
#define IO_REFRESH DS_CONVTIME/3  // как часто обновлять информацию на дисплее

// === Ethernet config ===
#ifndef ETHERNET_DHCP                       // если не используется DHCP, укажем адрес вручную
#define IP_ADDR_BASE 192,168,1,30           // задаём "базовый" IP адрес Ethernet модулей
#define ETHERNET_IP IP_ADDR_BASE + PLC_ID   // к нему прибавляется PLC_ID,
#endif                                      // то есть при PLC_ID=4 IP-адрес будет 192.168.1.34

#define ETHERNET_MAC MAC0, MAC1, MAC2, 0xEE, 0x30, PLC_ID
// MAC адрес Ethernet шилда - для каждого устройства в сети MAC должен быть уникальным!
// также смотрите ниже и правьте MAC0-2 и ETHERNET_IP

#ifdef ETHERNET_DHCP                        // если не используется DHCP, укажем адрес вручную
#define MAC0 0x0A                           // на всякий случай, чтобы по старшему байту MAC адреса
#else                                       // можно было определить с каким параметром скомпилирован скетч
#define MAC0 0x00                           // "0A" DHCP (Auto) или "00" статика
#endif

#ifdef ETHERNET_ENC28J60
#include "EtherCard.h"
#include "ModbusIP_ENC28J60.h"
#define MAC1 0x28               // если второй байт адреса равен 28 - скомпилировано под enc28j60 
#define MAC2 0x60               // на всякий случай, чтобы по второму (после старшего) байту MAC адреса
#endif                          // можно было определить под какой шилд скомпилирован скетч

#ifdef ETHERNET_WIZ5100
#include "Ethernet.h"
#include "ModbusIP.h"
#define MAC1 0x51               // если второй байт MAC адреса равен 51 - скомпилировано под wiz5100
#define MAC2 0x00
#endif

// === немного подробнее про сеть ===
/*
   к младшему байту IP_ADDR_BASE прибавляется PLC_ID, то есть при PLC_ID==4, IP будет 192.168.1.34
   адрес должен быть из диапазона сети в которой планируется использовать модуль, и не должен совпадать
   с адресом любого другого устройства в сети. посмотреть IP адреса можно на ПК (подключенном и корректно
   работающем в сети) командой ipconfig /all (Windows) или ifconfig (Linux). смотреть на строки
   "IPv4 Address" или "inet addr". но лучше всего залить тестовый скетч для соответствующего модуля
   и посмотреть монитор порта - DHCP должен выдать адрес. это еще и подтвердит работоспособность модуля.
   File->Examples->Ethernet-> DhcpAddressPrinter для w5100
   File->Examples->EtherCard-> testDHCP для enc28j60

   https://ru.wikipedia.org/wiki/%D0%A7%D0%B0%D1%81%D1%82%D0%BD%D1%8B%D0%B9_IP-%D0%B0%D0%B4%D1%80%D0%B5%D1%81
   обычно в домашних сетях используются IP адреса вида:
    10.0.0.1 - 10.255.255.254
    172.16.0.1 - 172.31.255.254
    192.168.0.1 - 192.168.255.254
   вот "что-то подобное" и нужно тут прописать :)

   если не используется DHCP, то кроме адреса можно (иногда и нужно) прописать адрес шлюза по умолчанию
   (default gateway) и адрес(а) DNS сервера (в данном скетче смысла не имеет). шлюз по умолчанию необходим
   если Modbus master (SCADA) и slave (наше устройство) находятся в разных подсетях и между ними маршрутизатор.
   такая конфигурация может возникнуть если модули дома, а SCADA на работе. если понадобится, пишите - расскажу,
   хотя самый простой совет в этом случае использовать DHCP. тема действительно бесконечная, пишите - отвечу.
*/

//===================================
#define ACS_RATIO5 0.024414063
#define ACS_RATIO20 0.048828125
#define ACS_RATIO30 0.073242188

#define pSW 7     // энкодер SW кнопка
#define pCLK 8    // Pin для энкодера CLK
#define pDT 9     // Pin для энкодера DT

//===================================
#ifdef SERIAL_INFO
#ifndef SERIAL_CONFIG
#define SERIAL_CONFIG
#endif
#define PRINTLN(xArg) (Serial.println(xArg))
#define PRINTHEX(aArg, bArg) (Serial.print(aArg, bArg))
#define PRINT(zArg) (Serial.print(zArg))
#else
#define PRINTLN(xArg)
#define PRINT(zArg)
#define PRINTHEX(aArg, bArg)
#endif

//===================================
// *** задаём нумерацию регистров Modbus
#define hrSECONDS 0
#define hrDSTEMP 1
#define hrPSET 2
#define hrPNOW 3
#define hrDSCONVPERIOD hrPNOW + 1
#define hrANGLE hrPNOW + 2
#define hrURATIO hrPNOW + 3
#define hrUNOW hrPNOW + 4
#define hrINOW hrPNOW + 5

//=====================================

#include <LiquidCrystal_I2C.h>      // https://github.com/marcoschwartz/LiquidCrystal_I2C
LiquidCrystal_I2C lcd(I2C_LCDADDRESS, 16, 2);

#include <ReadDigKey.h>
ReadDigKey key;

#include <Modbus.h>
ModbusIP mb;

#include "DStemp.h"
DSThermometer DS(OWPINS);

#include "ACpower.h"
ACpower TEH(MAXPOWER, 3, 5, A0, A1);

//===================================

unsigned long msReinit, msLcd;
uint16_t msConvTimeout = DS_CONVTIME;
const byte mac[] = { ETHERNET_MAC };
unsigned long msTEH;
int msTEHperiod;

uint16_t inst_P = 0;          // установливаемая мощность
uint16_t keep_P;
uint16_t local_buf = 0;
uint16_t remote_buf = 0;
uint16_t max_P = MAXPOWER;
char show_Kub[7], show_[7];
char step_P = 50;          // Шаг установки мощности нагрузки,
char degC = 223;
String T1, Var;
int Tint, Tfra;
volatile bool set_menu = false;
volatile bool ModbusON = true;
volatile bool mbMasterOK = true;
