#define SKETCHVERSION __FILE__ " " __DATE__ " " __TIME__
#define SKETCHFILE __FILE__
#define SKETCHTIME " " __DATE__ " " __TIME__
#define SHOWVERSION "v" VERSION " " __DATE__
#define MACADDRESS MAC0, MAC1, 0xBE, 0xEF, 0x30, ETHERNET_MACID  // MAC адрес Ethernet шилда 
#define IPBASE 192, 168, 1, 30    // IP адрес Ethernet модуля если не используется DHCP
                                  // к младшему байту прибавляется ETHERNET_MACID
                                  // то есть при ETHERNET_MACID=1 IP будет 192.168.1.31 

#define I2C_LCDADDRESS 0x3F       // адрес LCD дисплея - необходимо найти с помощью i2c_scanner.ino
#define SERIALSPEED 115200        // скорость в последовательном порту

#define DS_CONVTIME 750           // как часто опрашивать DS18B20 (миллисекунды)
#define IO_REFRESH DS_CONVTIME/3  // как часто обновлять информацию на дисплее
#define MB_TIMEOUT 50             // как долго можно работать без мастера Модбаса (секунды)

#include "DStemp.h"
DSThermometer sensor[] = OWPINS;

#include "RegPower.h"
#include "power.h"

// *** задаём нумерацию регистров Modbus
#define hrConvTimeout sensCount
#define hrPset sensCount + 1
#define hrPnow sensCount + 2
#define hrSECONDS sensCount + 3

//===================================
#ifdef ETHERNET_DHCP      // на всякий случай, чтобы по старшему байту MAC адреса можно было определить  
#define MAC0 0x10         // с каким параметром скомпилирован скетч - DHCP или статика
#else
#define MAC0 0x00
#endif

#ifdef ETHERNET_ENC28J60        
#include <EtherCard.h>          
#include <ModbusIP_ENC28J60.h>  
#define MAC1 0x28               // если второй байт адреса равен 28 - скомпилировано под enc28j60 
#endif                          // на всякий случай, чтобы по второму (после старшего) байту MAC адреса
                                // можно было определить под какой шилд скомпилирован скетч
                                
#ifdef ETHERNET_WIZ5100
#include <Ethernet.h>
#include <ModbusIP.h>
#define MAC1 0x51               // если второй байт адреса равен 51 - скомпилировано под wiz5100
#endif

#include <Modbus.h>
ModbusIP mb;
//===============================

const int sensCount = sizeof(sensor) / sizeof(DSThermometer);
unsigned long msReinit, msLcd;
uint16_t msConvTimeout = DS_CONVTIME;
byte macID;

void setupNetMB()     //Config Modbus IP
{
  const byte mac[] = { MACADDRESS };
  Serial.print("MAC: ");
  for (byte i = 0; i < 6; ++i)
  {
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(':');
  }
  Serial.println();

#ifdef ETHERNET_DHCP
  mb.config(mac);
#else
  const byte ip[] = { IPBASE + ETHERNET_MACID };
  mb.config(mac, ip);
#endif

#ifdef ETHERNET_ENC28J60
  ether.printIp("NC28J60 IP: ", ether.myip);
  ether.printIp("MASK: ", ether.netmask);
  ether.printIp("GW: ", ether.gwip);
  //ether.printIp("DNS: ", ether.dnsip);
#endif

#ifdef ETHERNET_WIZ5100
  Serial.print(F("WIZ5100 IP: "));
  Serial.println(Ethernet.localIP());
#endif
}

bool mbHeartBeat()
{
  // если мастер онлайн - он должен записывать 0 в регистр SECONDS
  // это будет признаком "живости" Мастера Modbus'а для модуля
  // и наоборот: не 0 в SECONDS - признак "живости" модуля для Мастера
  // хотя Мастеру логичнее отслеживать "живость" по GetQuality
  //
  if (mb.Hreg(hrSECONDS) == 0) mb.Hreg(hrSECONDS, msReinit / 1000);
  if (((uint16_t)(msReinit / 1000) - mb.Hreg(hrSECONDS)) < MB_TIMEOUT)
  {
    return true;
  }
  else
  {
    return false; // если мастера нет больше 10 секунд - выключаем
  }
}

void printDS(float t, int i)
{
  //Serial.print(F("DS "));
  Serial.print(i);
  Serial.print(F(": "));
  Serial.print(t);
  Serial.print(F(" | "));
  Serial.println(sensor[i].Parasite);
  //Serial.print(F(" | "));
  //Serial.println(millis());
}

void printFreeRam ()
{
  Serial.print(F("Free RAM: "));
  extern int __heap_start, *__brkval;
  int v, r;
  r = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  Serial.println(r);
}

