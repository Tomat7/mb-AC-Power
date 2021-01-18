//#include <Wire.h>
#include <LiquidCrystal_I2C.h>      // https://github.com/marcoschwartz/LiquidCrystal_I2C
LiquidCrystal_I2C lcd(I2C_LCDADDRESS, 16, 2);

// Коэффициенты датчика ACS712 |5А - 0.024414063 | 20А - 0.048828125 | 30A - 0.073242188 |
#define ACS_RATIO5 0.024414063
#define ACS_RATIO20 0.048828125
#define ACS_RATIO30 0.073242188

#define pSW 7     // энкодер SW кнопка
#define pCLK 8    // Pin для энкодера CLK
#define pDT 9     // Pin для энкодера DT
#include <ReadDigKey.h>
ReadDigKey key;

uint16_t inst_P = 0;          // установливаемая мощность
uint16_t keep_P;
//int mb_P = 0;
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

//================== Информация в порт и дисплей
void updInfo() //, float temp_)
{
  //dtostrf(temp_, 5, 2, show_);
  //if (mbMasterOK) show_ = "MB_ok_";
  //* 25.10.2018 *//dtostrf(temp_Kub, 4, 1, show_Kub);
  //dtostrf(TEH.Inow, 4, 1, show_Kub);
  PRINT("Angle: ");
  PRINTLN(TEH.Angle);
  PRINT(F("Pnow="));
  PRINTLN(TEH.Pnow);
  PRINT(F("Pset="));
  PRINTLN(TEH.Pset);
  PRINT(F("I="));
  PRINTLN(TEH.Inow);
  PRINT(F("U="));
  PRINTLN(TEH.Unow);
}

void updLcd()
{
  static char buff0[17] = { "                " };
  static char buff1[17] = { "                " };
 
  if (set_menu)
    sprintf(buff1, "Set %4d << %4d", inst_P, local_buf);
  else {
    sprintf(buff0, "Now %4dw %3i.%1i%c ", TEH.Pnow, Tint, Tfra, degC);
    // ** 25.10.2018 ** sprintf(buff0, "Now %4dw %s%c ", TEH.Pnow, show_Kub, degC);
    //sprintf(buff0, "Now %4dw I:%s", TEH.Pnow, show_Kub);
    sprintf(buff1, "Set %4dw ok_%3i", TEH.Pset, (uint8_t)(millis() / 1000));
    //sprintf(buff1, "Set %4dw %s", TEH.Pset, show_);
    //sprintf(buff1, "Set %5d U: %3d", TEH.Angle, uint16_t(TEH.Unow));
  }
  lcd.setCursor(0, 0); lcd.print(buff0);
  lcd.setCursor(0, 1); lcd.print(buff1);

  if ((!ModbusON) && (!set_menu))
  {
    lcd.setCursor(10, 1);
    lcd.cursor();
    lcd.blink();
    lcd.print("local_");
  }
  else if ((!mbMasterOK) && (!set_menu))
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
