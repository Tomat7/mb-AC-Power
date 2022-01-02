// 
#include "mod_encoder.h"
#include "func.h"
#include "display.h"
#include "teh.h"

#ifdef USE_ENCODER
#include <ReadDigKey.h>
ReadDigKey key;
#else
#warning "ENCODER not configured! Nothing to compile. "
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

	if (isSetMenu)
	{
		if (key.encoder != 0)
		{
			encoder_Turn();
		}
		else if ((millis() - key.previous_millis_down) > 5000)
		{
			isSetMenu = false;
			lcdMode = SHOW_VA;
		}
	}

	if ((millis() - key.previous_millis_down) > LCD_SAVER_TIMEOUT)
	{
		isScreenSaver = true;
		msPrintPeriod = SHOW_PERIOD * 3;
	}
	else
	{
		isScreenSaver = false;
		msPrintPeriod = SHOW_PERIOD;
	}

	if (key.shot_press()) encoder_ShortPress();	// Короткое нажатие кнопки - показать ток и напряжение
	if (key.long_press()) encoder_LongPress();	// Долгое нажатие кнопки - переход на локальное управление

}


void encoder_ShortPress()
{
	if (isLocalMode)
	{
		isSetMenu = !isSetMenu;
		if (isSetMenu) Pencoder = TEH.Pset;
		lcdMode = SHOW_POWER;
		log_debug_ln(". ENCODER Short while Local");
	}
	else
	{
		lcdMode = static_cast<DispScreen>(static_cast<int>(lcdMode) + 1);
		log_debug_ln(". ENCODER Short while Remote");
	}

	display_Info();
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
}

void encoder_Turn()
{
	if (isSetMenu) {
		Pencoder += int(ENCODER_STEP * key.encoder * abs(key.encoder));
		Pencoder = (Pencoder < 0) ? 0 : (Pencoder > POWER_MAX) ? POWER_MAX : Pencoder;
		setPower(Pencoder);	// мощность будет меняться во время кручения энкодера
		display_Power(Pencoder);
	}
	key.encoder = 0;
}

#endif //USE_ENCODER

