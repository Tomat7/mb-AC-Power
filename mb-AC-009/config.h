#define SKETCHNAME __FILE__ " " __DATE__ " " __TIME__
#define SHOWVERSION "v" SKETCHVERSION " " __DATE__
#define DS_CONVTIME 750   // как часто опрашивать DS18B20 (миллисекунды)
#define MB_TIMEOUT 70     // как долго можно работать/кликать_клапаном без мастера Модбаса (секунды)
#define msSHOWCONFIG 3000 // длительность показа конфигурации на дисплее при старте
#define IO_REFRESH DS_CONVTIME/3  // как часто обновлять информацию на дисплее

#define ETHERNET_MAC MAC0, MAC1, MAC2, 0xEE, 0x30, PLC_ID 
// MAC адрес Ethernet шилда 
// *** для каждого устройства в сети MAC должен быть уникальным ***
// также смотрите ниже и правьте MAC0-2 и ETHERNET_IP

#ifndef ETHERNET_DHCP                       // если не используется DHCP
#define ETHERNET_IP IP_ADDR_BASE + PLC_ID   // задаём IP адрес Ethernet модуля 
#define MAC0 0x00                           // на всякий случай, чтобы по старшему байту MAC адреса
#else                                       // можно было определить с каким параметром скомпилирован скетч
#define MAC0 0x0A                           // "0A" DHCP (Auto) или "00" статика
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

/*
   к младшему байту прибавляется ETHERNET_ID, то есть при ETHERNET_ID==1, IP будет 192.168.1.31
   адрес должен быть из диапазона сети в которой планируется использовать модуль, и не должен совпадать
   с адресом любого другого устройства в сети. посмотреть IP адреса можно командой ipconfig /all (Windows)
   или ifconfig (Linux) с ПК подключенного и корректно работающего в сети - смотреть на строки
   "IPv4 Address" или "inet addr". но лучше всего залить тестовый скетч для соответствующего модуля
   и посмотреть монитор порта - DHCP должен выдать адрес. это еще и подтвердит работоспособность модуля.
   File->Examples->Ethernet-> DhcpAddressPrinter для w5100
   File->Examples->EtherCard-> testDHCP для enc28j60

   https://ru.wikipedia.org/wiki/%D0%A7%D0%B0%D1%81%D1%82%D0%BD%D1%8B%D0%B9_IP-%D0%B0%D0%B4%D1%80%D0%B5%D1%81
   обычно в домашних сетях используются IP адреса вида:
    10.0.0.1 - 10.255.255.254
    172.16.0.1 - 172.31.255.254
    192.168.0.1 - 192.168.255.254
   вот "что-то подобное" и нужно тут прописать :) тема бесконечная, пишите - отвечу.

   если не используется DHCP, то кроме адреса можно (иногда и нужно) прописать адрес шлюза по умолчанию
   (default gateway) и адрес(а) DNS сервера (в данном скетче смысла не имеет). шлюз по умолчанию необходим
   если Modbus master (SCADA) и slave (наше устройство) находятся в разных подсетях и между ними маршрутизатор.
   такая конфигурация может возникнуть если модули дома, а SCADA на работе. если понадобится, пишите - расскажу,
   хотя самый простой совет в этом случае использовать DHCP. тема действительно бесконечная, пишите - отвечу.
*/

#ifdef SERIAL_INFO
#define PRINTLN(xArg) (Serial.println(xArg))
#define PRINTHEX(aArg, bArg) (Serial.print(aArg, bArg))
#define PRINT(zArg) (Serial.print(zArg))
#else
#define PRINTLN(xArg)
#define PRINT(zArg)
#define PRINTHEX(aArg, bArg)
#endif

#ifdef SERIAL_INFO
#ifndef SERIAL_CONFIG
#define SERIAL_CONFIG
#endif
#endif
//===================================

#include "Modbus.h"
ModbusIP mb;

#include "DStemp3.h"
DSThermometer DS(OWPINS);

#include "ACpower.h"
ACpower TEH(MAXPOWER, 3, 5, A0, A1);

#include "power.h"

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
//===================================

unsigned long msReinit, msLcd;
uint16_t msConvTimeout = DS_CONVTIME;
const byte mac[] = { ETHERNET_MAC };

void setupNetMB()     //Config Modbus IP
{
#ifdef SERIAL_CONFIG
  Serial.print("MAC: ");
  for (byte i = 0; i < 6; ++i)
  {
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(':');
  }
  Serial.println();
#endif

#ifndef ETHERNET_DHCP
  const byte ip[] = { ETHERNET_IP };
  mb.config(mac, ip);
#else
  mb.config(mac);
#endif

#ifdef ETHERNET_ENC28J60
#ifdef SERIAL_CONFIG
  ether.printIp("NC28J60 IP: ", ether.myip);
  ether.printIp("MASK: ", ether.netmask);
  ether.printIp("GW: ", ether.gwip);
  //ether.printIp("DNS: ", ether.dnsip);
#endif
#endif

#ifdef ETHERNET_WIZ5100
#ifdef SERIAL_CONFIG
  Serial.print(F("WIZ5100 IP: "));
  Serial.println(Ethernet.localIP());
#endif
#endif
}

void lcdNetInfo()
{
  lcd.setCursor(0, 0);
  //lcd.print("MAC: ");
  for (byte i = 0; i < 6; ++i)
  {
    char mh[2];
    String macStr = String(mac[i], HEX);
    //macStr.toUpperCase();
    macStr.toCharArray(mh, 3);
    lcd.print(mh);
    if (i < 5) lcd.print(':');
  }

  lcd.setCursor(0, 1);
  //lcd.print("IP: ");
  for (byte i = 0; i < 4; ++i)
  {
    lcd.print(Ethernet.localIP()[i]);
    if (i < 3) lcd.print(".");
  }
}

bool mbHeartBeat()
{
  // если мастер онлайн - он должен записывать 0 в регистр SECONDS
  // это будет признаком "живости" Мастера Modbus'а для модуля
  // и наоборот: не 0 в SECONDS - признак "живости" модуля для Мастера
  // хотя Мастеру логичнее отслеживать "живость" по GetQuality
  //
  if (mb.Hreg(hrSECONDS) == 0) mb.Hreg(hrSECONDS, msReinit / 1000);
  if (((uint16_t)(msReinit / 1000) - mb.Hreg(hrSECONDS)) < MB_TIMEOUT) return true;
  else return false;                                                    
  // если мастера нет больше XX секунд - выключаем
}

void printDS(float t, int i)
{
  PRINT(F("DS "));
  PRINT(i);
  PRINT(F(": "));
  PRINT(t);
  PRINT(F(" | "));
  PRINT(DS.Parasite);
  PRINT(F(" | "));
  PRINTLN(millis());
}

void printFreeRam ()
{
#ifdef SERIAL_CONFIG
  Serial.print(F("Free RAM: "));
  extern int __heap_start, *__brkval;
  int v, r;
  r = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  Serial.println(r);
#endif
}
