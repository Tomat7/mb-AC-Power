#ifdef USE_ENCODER

void encoder_Setup()
{
	key.add_key(ENCODER_pSW);
	key.add_enccoder(ENCODER_pCLK, ENCODER_pDT);
}

void encoder_Check()
{
	key.readkey();
	if (isSetMenu)
	{
		key.readencoder();     // можно не так часто
		if (key.encoder != 0) encoder_Turn();		// вращение
	}
	if ((millis() - msKeys) > 200)
	{
		msKeys = millis();
		encoder_Keys();
		//if (isSetMenu) display_Oled();
	}
}

void encoder_Keys()
{
	if (key.shot_press()) encoder_ShortPress();	// Короткое нажатие кнопки - показать ток и напряжение
	if (key.long_press()) encoder_LongPress();	// Долгое нажатие кнопки - переход на локальное управление
	if (key.encoder != 0) encoder_Turn();		// вращение

	if (isSetMenu && ((key.previous_millis_down + 5000) < millis()))
	{
		isSetMenu = false;
		lcdMode = SHOW_VA;
	}

	if ((key.previous_millis_down + 1000000) < millis()) { isScreenSaver = true; }
	else { isScreenSaver = false; }
}

void encoder_ShortPress()
{
	display_Init();

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
		display_Clear();
		display_Switch();
		log_debug_ln(". ENCODER Short while Remote");
	}

	display_Info();
}

void encoder_LongPress()
{
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
