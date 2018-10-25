#define PLC_ID 0x01
#define SKETCHVERSION "009"     // версия скетча (чтобы самому не запутаться :) new version DS lib
#define MAXPOWER 3500           // максимальная мощность ТЭНа - больше этой мощности регулятор установить не позволит
#define OWPINS 6                // pin на котором ds18b20 (не забывайте про подтягивающий резистор 4.7 кОм!)
#define I2C_LCDADDRESS 0x3F     // адрес LCD дисплея - необходимо найти с помощью i2c_scanner.ino
#define SERIALSPEED 115200      // скорость в последовательном порту
#define SERIAL_CONFIG           // если нужно показать в Serial _только_ конфигурацию при старте
//#define SERIAL_INFO             // если нужно постоянно выдавать информацию в Serial (см. макросы PRINT* в config.h)

#define U_RATIO 0.304           // множитель для напряжения - нужно сделать "1" если схема не модифицировалась
#define ETHERNET_WIZ5100        // используется w5100 шилд - с регулятором мощности только он!
//#define ETHERNET_ENC28J60     // с регулятором мощности шилд enc28j60 использовать не рекомендуется

#define IP_ADDR_BASE 192, 168, 1, 30        
// "начальный" IP-адрес контроллеров - к нему прибавляется PLC_ID, то есть при PLC_ID=4 IP-адрес будет 192.168.1.34
// *** чуть более подробное описание смотреть в config.h ***

//#define ETHERNET_DHCP
// запрашивать IP адрес от DHCP сервера при старте (скетч занимает 99% памяти!)
// если закоментировать то адрес будет назначен принудительно
// *** посмотреть IPADDRESS в config.h ***

#include "config.h"
// *** обязательно заглянуть и поправить при необходимости! ***

void setup()
{
#ifdef SERIAL_CONFIG
  Serial.begin(SERIALSPEED);    //активируем прием сообщения
  delay(300);
  Serial.println(F(SKETCHNAME));
#endif
  key.add_key(pSW);
  key.add_enccoder(pCLK, pDT);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  const char* SketchInfo = SHOWVERSION;
  lcd.setCursor(0, 0);
  lcd.print(SketchInfo);
  lcd.setCursor(0, 1);
  lcd.print("CONFIG ");
  lcd.print(MAXPOWER);
  lcd.print(" watt");

  setupNetMB();

#ifdef SERIAL_CONFIG
  TEH.init(ACS_RATIO20, U_RATIO);
  DS.init(DS_CONVTIME, true, false);
  //Serial.print(DS.Connected);
#else
  TEH.init(ACS_RATIO20, U_RATIO, false);
  DS.init(DS_CONVTIME, false, false);
#endif

  delay(msSHOWCONFIG);
  lcd.clear();
  lcdNetInfo();

  msReinit = millis();
  //PRINTLN(msReinit);
  mb.addHreg(hrSECONDS, msReinit);          // Like "alive" flag counter for future
  mb.addHreg(hrDSTEMP);
  mb.addHreg(hrPSET);
  mb.addHreg(hrPNOW);
  mb.addHreg(hrDSCONVPERIOD);
  mb.addHreg(hrANGLE);
  //mb.addHreg(hrURATIO, TEH.Uratio * 100000);
  mb.addHreg(hrUNOW);
  mb.addHreg(hrINOW);

  //msConvTimeout = DS_CONVTIME;
  printFreeRam();

  delay(msSHOWCONFIG);
  lcd.clear();
}

void loop()
{
  TEH.control();
  mb.task();
  key.readkey();
  if (set_menu) key.readencoder();     // можно не так часто
  mb.Hreg(hrPNOW, TEH.Pnow);

  if ((millis() - msReinit) > msConvTimeout)
  {
    msReinit = millis();
    DS.check();
    float t = DS.Temp;
    mb.Hreg(hrDSTEMP, round( t * 100));  // заносим в регистр Modbus (температура * 100)
    mb.Hreg((hrDSCONVPERIOD), DS.TimeConv);  //сохраним время потраченное на преобразование
    printDS(t, 0);

    updInfo(DS.Temp); //, TEH.ADCperiod);
    mb.Hreg(hrANGLE, TEH.Angle);
    //mb.Hreg(hrANGLE, TEH.usZeroCross);
    //remote_buf = TEH.Pset;
    //mb.Hreg(hrPNOW, TEH.Pnow);
    mb.Hreg(hrUNOW, TEH.Unow);
    mb.Hreg(hrINOW, TEH.Inow * 1000);

    if (ModbusON)
    {
      mbMasterOK = mbHeartBeat();
      if (mbMasterOK)
      {
        inst_P = mb.Hreg(hrPSET);
      }
      else {
        inst_P = 0;
        PRINT("Master OFFline ");
        PRINTLN(mb.Hreg(hrSECONDS));
        //lcd.setCursor(9, 0);
        //lcd.cursor();
        //lcd.blink();
      }
    }
    TEH.setpower(inst_P);
    mb.Hreg(hrPSET, TEH.Pset);
    updLcd();
    chkSerial();
  }

  if ((millis() - msLcd) > IO_REFRESH)
  {
    chkKeys();
    msLcd = millis();
    if (set_menu) updLcd();
  }
}


