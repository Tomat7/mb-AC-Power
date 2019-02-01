#include <arduino.h>
#line 1 "C:\\Users\\star0413\\Documents\\Arduino\\mb-AC-Power\\mb-AC-019\\mb-AC-019.ino"
#line 1 "C:\\Users\\star0413\\Documents\\Arduino\\mb-AC-Power\\mb-AC-019\\mb-AC-019.ino"
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


//
//
void setupNetModbus();
void setupRegModbus();
void printNet();
void lcdCfgInfo();
void lcdNetInfo();
void chkModbusMaster();
void printDS(float t);
void printAC();
void lcdUpdate();
void chkKeys();
void chkSerial();
void printFreeRam();

#line 19 "C:\\Users\\star0413\\Documents\\Arduino\\mb-AC-Power\\mb-AC-019\\mb-AC-019.ino"
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

#line 1 "C:\\Users\\star0413\\Documents\\Arduino\\mb-AC-Power\\mb-AC-019\\functions.ino"

void setupNetModbus()     //Config Modbus IP
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

#ifdef ETHERNET_DHCP
    mb.config(mac);
#else
    const byte ip[] = { ETHERNET_IP };
    mb.config(mac, ip);
#endif
}

void setupRegModbus()
{
    mb.addHreg(hrSECONDS, msReinit);          // Like "alive" flag counter for future
    mb.addHreg(hrDSTEMP);
    mb.addHreg(hrPSET);
    mb.addHreg(hrPNOW);
    mb.addHreg(hrDSCONVPERIOD);
    mb.addHreg(hrANGLE);
    mb.addHreg(hrUNOW);
    mb.addHreg(hrINOW);
}

void printNet()
{
#ifdef SERIAL_CONFIG
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
#endif
}

void lcdCfgInfo()
{
    lcd.init();
    lcd.backlight();
    lcd.clear();
    const char* SketchInfo = SHOWVERSION;
    lcd.setCursor(0, 0);
    lcd.print(SketchInfo);
    lcd.setCursor(0, 1);
    sprintf(SketchInfo, "CONFIG %4d watt", MAXPOWER);
    lcd.print(SketchInfo);
}

void lcdNetInfo()
{
    lcd.setCursor(0, 0);
    for (byte i = 0; i < 6; ++i)
    {
        char mh[2];
        String macStr = String(mac[i], HEX);
        macStr.toCharArray(mh, 3);
        lcd.print(mh);
        if (i < 5) lcd.print(':');
    }

    lcd.setCursor(0, 1);
    for (byte i = 0; i < 4; ++i)
    {
        lcd.print(Ethernet.localIP()[i]);
        if (i < 3) lcd.print(".");
    }
}

void chkModbusMaster()	// HeartBeat (следим за Modbus master)
{
    // если мастер онлайн - он должен записывать 0 в регистр SECONDS
    // это будет признаком "живости" Мастера Modbus'а для модуля
    // и наоборот: не 0 в SECONDS - признак "живости" модуля для Мастера
    // хотя Мастеру логичнее отслеживать "живость" по GetQuality
    //
    if ((mb.Hreg(hrSECONDS) == 0) && ModbusON) mb.Hreg(hrSECONDS, msReinit / 1000);

    if (((uint16_t)(msReinit / 1000) - mb.Hreg(hrSECONDS)) < MB_TIMEOUT) mbMasterOK = true;
    else mbMasterOK = false;
    // если мастера нет больше XX секунд - выключаем
    return;
}

void printDS(float t)
{
    PRINT(F("DS "));
    //PRINT(i);
    PRINT(F(": "));
    PRINT(t);
    PRINT(F(" | "));
    PRINT(DS.Parasite);
    PRINT(F(" | "));
    PRINTLN(millis());
}

//=== Информация в порт
void printAC() //, float temp_)
{
    PRINT("Angle: ");
    PRINTLN(TEH.Angle);
    PRINT(F("Pnow="));
    PRINTLN(TEH.Pnow);
    PRINT(F("Pset="));
    PRINTLN(TEH.Pset);
}

//=== Информация в дисплей
void lcdUpdate()
{
    static char buff0[17] = { "                " };
    static char buff1[17] = { "                " };

    if (set_menu)
        sprintf(buff1, "Set %4d << %4d", inst_P, local_buf);
    else {
        sprintf(buff0, "Now %4dw %3i.%1i%c ", TEH.Pnow, Tint, Tfra, degC);
        // ** 25.10.2018 ** sprintf(buff0, "Now %4dw %s%c ", TEH.Pnow, show_Kub, degC);
        //sprintf(buff0, "Now %4dw I:%s", TEH.Pnow, show_Kub);
        // !ok! ** 16.11.2018 ** sprintf(buff1, "Set %4dw ok_%3i", TEH.Pset, (uint8_t)(millis() / 1000));
        sprintf(buff1, "Set %4dw ok_%3i", TEH.Pset, msTEHperiod);
        //sprintf(buff1, "Set %4dw %s", TEH.Pset, show_);
        //sprintf(buff1, "Set %5d U: %3d", TEH.Angle, uint16_t(TEH.Unow));
    }
    lcd.setCursor(0, 0); lcd.print(buff0);
    lcd.setCursor(0, 1); lcd.print(buff1);

    if (!ModbusON && !set_menu)
    {
        lcd.setCursor(10, 1);
        lcd.cursor();
        lcd.blink();
        lcd.print("local_");
    }
    else if (!mbMasterOK && !set_menu)
    {
        lcd.setCursor(10, 1);
        lcd.print("MB_OFF");
        lcd.cursor();
        lcd.blink();
        lcd.setCursor(9, 0);
    }
}

void chkKeys()
{
    //key.readkey();
    if (key.shot_press())     // Короткое нажатие кнопки
    {
        if (!ModbusON)
        {
            set_menu = !set_menu;
            if (set_menu) local_buf = inst_P;
            else inst_P = local_buf;  //мощность будет меняться только после короткого нажатия
        }
        else
        {
            lcd.setCursor(0, 1);            // индикатор короткого нажатия на энкодер
            lcd.print(" try long press "); // можно закоментарить эти строки
        }
    }
    // Долгое нажатие кнопки
    // ==================== !! здесь переход с МБ на локальное управление !!
    if (key.long_press()) {
        set_menu = false;
        local_buf = 0;
        inst_P = 0;
        ModbusON = !ModbusON;
    }
    //key.readencoder();
    if (key.encoder)                   // вращение
    {
        if (set_menu) {
            local_buf += int(step_P * key.encoder * abs(key.encoder));
            //local_buf = (local_buf > max_P) ? max_P : local_buf;
            local_buf = (local_buf < 0) ? 0 : (local_buf > max_P) ? max_P : local_buf;
            //inst_P = local_buf;  // мощность будет меняться во время кручения энкодера, удобно с лампочкой
        }
        key.encoder = 0;
    }
    if (key.previous_millis_down + 10000 < millis()) set_menu = false;
}

void chkSerial()
{
#ifdef SERIAL_INFO
    while (Serial.available()) //Serial port, пока не конец сообщения, читаем данные и формируем строку
    {
        char ch = Serial.read();
        Var += ch;
        if (ch == '\n')
        {
            Var.toUpperCase();        // ??
            if (Var.substring(0, 2) == "SP")
            {
                T1 = Var.substring(Var.indexOf("SP", 2) + 3); //команда
                inst_P = T1.toFloat();          //Выставленная мощность с Serial
                TEH.setpower(inst_P);
                Var = "";
            }
        }
    }
#endif
}

void printFreeRam()
{
#ifdef SERIAL_CONFIG
    Serial.print(F("Free RAM: "));
    extern int __heap_start, *__brkval;
    int v, r;
    r = (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
    Serial.println(r);
#endif
}


