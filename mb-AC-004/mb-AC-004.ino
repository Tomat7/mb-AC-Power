#define VERSION "004"           // версия скетча (чтобы самому не запутаться :)
#define MAXPOWER 3000           // номинальная мощность ТЭНа ** важно ** от этого считаются другие параметры
#define OWPINS 6                // pin на котором ds18b20 (не забывайте про подтягивающий резистор 4.7 кОм!)
#define I2C_LCDADDRESS 0x3F       // адрес LCD дисплея - необходимо найти с помощью i2c_scanner.ino
#define SERIALSPEED 115200        // скорость в последовательном порту
#define U_RATIO 0.28751         // множитель для напряжения - нужно сделать "1" если схема не модифицировалась
//#define ETHERNET_ENC28J60     // ** с регулятором мощности шилд enc28j60 использовать не рекомендуется **
#define ETHERNET_WIZ5100        // если используется w5100 шилд - с регулятором мощности только он!

#define ETHERNET_ID 0x01
// младший байт MAC адреса ** для каждого устройства в сети MAC должен быть уникальным **
// также смотрите и правьте MACADDRESS, MAC0-2 в config.h

//#define ETHERNET_DHCP
// запрашивать IP адрес от DHCP сервера при старте (скетч занимает 99% памяти!)
// если закоментировать то адрес будет назначен принудительно
// *** посмотреть IPADDRESS в config.h ***

#include "config.h"
// *** обязательно заглянуть и поправить при необходимости! ***

void setup()
{
  Serial.begin(SERIALSPEED);    //активируем прием сообщения
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
  //TEH.init();
  TEH.init(U_RATIO);
  delay(300);
  Serial.println(F(SKETCHVERSION));
  //printFreeRam();
  setupNetMB();
  delay(msSHOWCONFIG);
  lcd.clear();
  lcdNetInfo();

  msReinit = millis();
  Serial.println(msReinit);
  mb.addHreg(hrSECONDS, msReinit);          // Like "alive" flag counter for future

  DS.init(DS_CONVTIME);
  Serial.print(0);
  Serial.println(DS.Connected);
  mb.addHreg(hrDSTEMP);

  mb.addHreg(hrPSET);
  mb.addHreg(hrPNOW);
  mb.addHreg(hrDSCONVPERIOD);
  mb.addHreg(hrANGLE);
  mb.addHreg(hrURATIO, TEH.Uratio * 100000);
  mb.addHreg(hrUNOW);
  mb.addHreg(hrINOW);

  msConvTimeout = 750;
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

  if ((millis() - msReinit) > msConvTimeout)
  {
    msReinit = millis();
    DS.check();
    float t = DS.Temp;
    mb.Hreg(hrDSTEMP, round( t * 100));  // заносим в регистр Modbus (температура * 100)
    mb.Hreg((hrDSCONVPERIOD), DS.TimeConv);  //сохраним время потраченное на преобразование
    printDS(t, 0);
    
    updInfo(DS.Temp); //, TEH.ADCperiod);
    mb.Hreg(hrANGLE, TEH.Angle / 4);
    //TEH.Uratio = float(mb.Hreg(hrURATIO) / 100000);
    //remote_buf = TEH.Pset;
    mb.Hreg(hrPNOW, TEH.Pnow);
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
        Serial.print("Master OFFline ");
        Serial.println(mb.Hreg(hrSECONDS));
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


