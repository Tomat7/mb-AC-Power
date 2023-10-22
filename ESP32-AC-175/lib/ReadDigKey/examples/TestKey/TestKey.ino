#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif
// ************* Настройка кнопок клавиатуры ***********************
#include <ReadDigKey.h>
ReadDigKey key;

//#define Pin_speaker 4
#define pSW 7			// энкодер SW кнопка
#define pCLK 3          // Pin для энкодера CLK
#define pDT 2			// Pin для энкодера DT


void setup() {
  Serial.begin(115200);
  Serial.println("Press SW key...");
  key.add_key(pSW);
  key.add_enccoder(pCLK, pDT);

}

void loop() {
  key.readkey();						// Чтение клавиатуры
  if (key.shot_press()) {               // Короткое нажатие 
	  if (key.pressed()== pSW) Serial.println("Shot press Key");
  }
  if (key.long_press()) {				// Длинное нажатие 
	  if (key.pressed() == pSW) Serial.println("Long press Key");
  }
  if (key.repeat()) {           // Длинное нажатие 
	  if (key.pressed() == pSW) Serial.println("Repeat press Key");
  }
  key.readencoder();
  if (key.encoder > 0) { Serial.print("Encoder Left "); Serial.println(key.encoder, DEC); }
  if (key.encoder < 0) { Serial.print("Encoder Right "); Serial.println(key.encoder, DEC); }
  key.encoder = 0;
}

