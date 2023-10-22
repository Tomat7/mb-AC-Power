//
#include "mod_encoder.h"
#include "func.h"
#include "display.h"
#include "teh.h"
#include "Wire.h"

#ifdef USE_ENCODER
#include <ReadDigKey.h>
ReadDigKey key;
#else
#pragma message "ENCODER not configured! Nothing to compile. "
#endif

#ifdef USE_ENCODER

void encoder_Setup()
{
	key.add_key(ENCODER_pSW);
	key.add_enccoder(ENCODER_pCLK, ENCODER_pDT);
}

void encoder_Check()
{
	key.readkey();
	key.readencoder();
	uint32_t encTimeout = millis() - key.previous_millis_down;

	if (key.encoder != 0)
		encoder_Turn();

	if (isSetMenu && (encTimeout > SHOW_PERIOD * 5))
	{
		isSetMenu = false;
		lcdMode = SHOW_VA;
	}

	if (encTimeout > LCD_SAVER_TIMEOUT)
	{
		isScreenSaver = true;
		if (msTelnetPeriod == SHOW_PERIOD)
			msPrintPeriod = SHOW_PERIOD * 3;
		else
			msPrintPeriod = msTelnetPeriod;
	}
	else
	{
		isScreenSaver = false;
		if (msTelnetPeriod == SHOW_PERIOD)
			msPrintPeriod = SHOW_PERIOD;
		else
			msPrintPeriod = msTelnetPeriod;
	}

	if (key.shot_press())
		encoder_ShortPress(); // Короткое нажатие кнопки - показать ток и напряжение
	if (key.long_press())
		encoder_LongPress(); // Долгое нажатие кнопки - переход на локальное управление

	return;
}

void encoder_Turn()
{
	if (isSetMenu)
	{
		Pencoder += int(ENCODER_STEP * key.encoder * abs(key.encoder));
		Pencoder = (Pencoder < 0) ? 0 : (Pencoder > POWER_MAX) ? POWER_MAX : Pencoder;
		setPower(Pencoder); // мощность будет меняться во время кручения энкодера
		display_Power(Pencoder);
	}
	else
	{
		display_ReInit();
	}

	key.encoder = 0;

	return;
}

void encoder_ShortPress()
{
	if (isLocalMode)
	{
		isSetMenu = !isSetMenu;
		lcdMode = SHOW_POWER;
		log_debug_ln(". ENCODER Short while Local");
		if (isSetMenu)
			Pencoder = TEH.Pset;
		else
			log_info_ln(". ENCODER set Power: ", Pencoder);
	}
	else
	{
		lcdMode = static_cast<DispScreen>(static_cast<int>(lcdMode) + 1);
		log_debug_ln(". ENCODER Short while Remote");
	}

	display_Info();

	return;
}

void encoder_LongPress()
{
	display_Init();
	display_Clear();
	setPower(0);
	isSetMenu = false;
	isLocalMode = !isLocalMode;
	lcdMode = SHOW_POWER;
	Pencoder = 0;
	log_info_ln(". ENCODER Long Press");
	return;
}

#endif // USE_ENCODER
