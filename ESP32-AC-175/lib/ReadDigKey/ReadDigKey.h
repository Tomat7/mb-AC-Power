/*
ReadDigKey.h - 
Created by G.Alin.
License: LGPL 3
Библиотека для обработки кнопок и энкодера. Определяет короткое нажатие кнопки, длинное и повторение нажатия
Устраняет дребезг контактов. 
Выдает направление вращения Энкодера
*/

#ifndef READDIGKEY_H
#define READDIGKEY_H

#include "Arduino.h"


class ReadDigKey {
private:
	byte _keypress;
	bool _shot = false, _long = false, _repeat = false;
	unsigned long _shot_delay = 0, _long_delay = 0, _repeat_delay = 0, _beep_delay = 0;
	byte _key_quant=0;
	byte *_key_array;
	byte Speaker_Pin=0, CLK_Pin=0, DT_Pin=0;
public:
	uint16_t LONG_DELAY = 700;
	ReadDigKey(byte _pin_beep = 0xFF);
	void add_key(byte pin_Key);
	void add_enccoder(byte pin_CLK, byte pin_DT);
	bool readkey();
	void readencoder();
	byte pressed();
	bool shot_press();
	bool long_press();
	bool repeat();
	uint8_t quant();
	unsigned long previous_millis_down = millis();
	unsigned long previous_millis_enc = millis();
	int16_t encoder = 0;
};

#endif
