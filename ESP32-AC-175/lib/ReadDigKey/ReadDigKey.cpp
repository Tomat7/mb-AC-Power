#include "ReadDigKey.h" 

// **********************************************************************************
ReadDigKey::ReadDigKey(byte _pin_beep) {
	Speaker_Pin = _pin_beep;
	if (Speaker_Pin != 0xFF)
	pinMode(Speaker_Pin, OUTPUT);
}

void ReadDigKey::add_key(byte pin_Key) {
	pinMode(pin_Key, INPUT_PULLUP);
	_key_array = (byte*)realloc(_key_array, (_key_quant + 1) * sizeof(byte));
	_key_array[_key_quant] = pin_Key;
	_key_quant++;
}
void ReadDigKey::add_enccoder(byte pin_CLK, byte pin_DT) {
	CLK_Pin = pin_CLK;
	DT_Pin = pin_DT;
	pinMode(CLK_Pin, INPUT);
	pinMode(DT_Pin, INPUT);
}

void ReadDigKey::readencoder() {
	static bool CLK_old, DT_old;
	bool CLK = digitalRead(CLK_Pin);
	bool DT = digitalRead(DT_Pin);
	if ((!CLK) && (CLK_old)) { // если состояние изменилось с положительного к нулю
		if ((millis() - previous_millis_down < 20) && (DT != DT_old)) return;
		encoder = DT ? 1 : -1;
		DT_old = DT;
		if (millis() - previous_millis_down < 10) encoder *= 4;
		else if (millis() - previous_millis_down < 30) encoder *= 3;
		else if (millis() - previous_millis_down < 50)  encoder *= 2;
		previous_millis_down = millis();
	}
	CLK_old = CLK; // сохраняем значение для следующего цикла
}

bool ReadDigKey::readkey() {
	byte key;
	static byte keyold;
	unsigned long mills = millis();
	for (key = 0; key < _key_quant; key++) if (!digitalRead(_key_array[key])) break;
	if (key != keyold) {
		keyold = key;
		_shot_delay = mills + 30;
	}
	if (_shot_delay > 0 && _shot_delay <= mills) {
		_shot_delay = 0;
		if (key == _key_quant) {
			if (mills < _long_delay) _shot = true;
			_long_delay = 0;
			_repeat_delay = 0;
			_long = false;
			_repeat = false;
		} else {
			_long_delay = mills + LONG_DELAY;
			_keypress = key;
			_beep_delay = mills + 50;
			previous_millis_down = millis();
		}
	}
	if (_long_delay > 0 && _long_delay <= mills) {
		_long_delay = 0;
		_long = true;
		_repeat_delay = mills + 700;
		_beep_delay = mills + 150;
	}
	if (_repeat_delay > 0 && _repeat_delay <= mills) {
		_repeat_delay = 0;
		_repeat = true;
	}

	if (Speaker_Pin != 0xFF) {
		static bool beepold = false;
		bool beep = _beep_delay > mills;
		if (beep != beepold) {
			beepold = beep;
			// digitalWrite(Speaker_Pin, beep);
		}
	}
	return _shot || _long || _repeat;
}

byte ReadDigKey::pressed() {
	return _key_array[_keypress];
}
bool ReadDigKey::shot_press() {
	bool rs = _shot;
	_shot = false;
	return rs;
}
bool ReadDigKey::long_press() {
	bool rs = _long;
	_long = false;
	return rs;
}
bool ReadDigKey::repeat() {
	static unsigned long previous_repeat;
	if (_repeat) {
		_long = false;
		if (millis() >= previous_repeat + 100) {
			previous_repeat = millis();
			_beep_delay = previous_repeat + 10;
			return true;
		}
	}
	return false;
}
uint8_t ReadDigKey::quant() {
	return _key_quant;
}

