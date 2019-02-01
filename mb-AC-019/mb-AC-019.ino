#define PLC_ID 0x01
#define SKETCHVERSION "019"     // версия скетча (чтобы самому не запутаться :) new version DS lib
#define MAXPOWER 3500           // максимальная мощность ТЭНа - больше этой мощности регулятор установить не позволит
#define OWPINS 6                // pin на котором ds18b20 (не забывайте про подтягивающий резистор 4.7 кОм!)
#define I2C_LCDADDRESS 0x3F     // адрес LCD дисплея - необходимо найти с помощью i2c_scanner.ino
#define SERIALSPEED 115200      // скорость в последовательном порту
#define U_RATIO 0.304           // множитель для напряжения - нужно сделать 1 если схема не модифицировалась
#define I_RATIO ACS_RATIO20     // множитель датчика тока. в данном случае используется ACS712 20A
#define SERIAL_CONFIG           // если нужно показать в Serial _только_ конфигурацию при старте
//#define SERIAL_INFO             // если нужно постоянно выдавать информацию в Serial (см. макросы PRINT* в config.h)

// === конфигурация сети === подробности в config.h ===
#define ETHERNET_WIZ5100        // используется w5100 шилд - с регулятором мощности только он!
//#define ETHERNET_ENC28J60     // с регулятором мощности шилд enc28j60 использовать не рекомендуется
//#define ETHERNET_DHCP         // получить IP адрес от DHCP сервера при старте (жрёт память!)

#include "config.h"             // *** обязательно заглянуть и поправить при необходимости! ***

void setup()
{
#ifdef SERIAL_CONFIG
	Serial.begin(SERIALSPEED);    //активируем прием сообщения
	delay(300);
	Serial.println(F(SKETCHNAME));
#endif

	key.add_key(pSW);
	key.add_enccoder(pCLK, pDT);

	setupNetModbus();
	printNet();

	TEH.init(I_RATIO, U_RATIO);
	DS.init(DS_CONVTIME);

#ifdef SERIAL_CONFIG
	TEH.printConfig();
	DS.printConfig();
#endif

	delay(msSHOWCONFIG);
	lcd.clear();
	lcdNetInfo();

	msReinit = millis();
	//PRINTLN(msReinit);

	setupRegModbus();
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
		DS.check();
		float t = DS.Temp;
		Tint = (int)t;                        // compute the integer part of the float
		Tfra = (int)((t - (float)Tint) * 10); // compute 1 decimal places (and convert it to int)
		msReinit = millis();

		printDS(t);
		printAC();

		mb.Hreg(hrDSTEMP, round(t * 100));  // заносим в регистр Modbus (температура * 100)
		mb.Hreg(hrDSCONVPERIOD, DS.TimeConv);  //сохраним время потраченное на преобразование
		mb.Hreg(hrANGLE, TEH.Angle);
		mb.Hreg(hrUNOW, TEH.Unow);
		mb.Hreg(hrINOW, TEH.Inow * 1000);

		if (ModbusON)
		{
			chkModbusMaster();
			if (mbMasterOK)
			{
				inst_P = mb.Hreg(hrPSET);
			}
			else
			{
				inst_P = 0;
				PRINT("Master OFFline ");
				PRINTLN(mb.Hreg(hrSECONDS));
			}
		}
		TEH.setpower(inst_P);
		mb.Hreg(hrPSET, TEH.Pset);
		lcdUpdate();
		chkSerial();
	}

	if ((millis() - msLcd) > IO_REFRESH)
	{
		msLcd = millis();
		chkKeys();
		if (set_menu) lcdUpdate();
	}
}
