#ifdef SERIAL_CONFIG
void serial_Setup()
{
	Serial.begin(115200);
	delay(1000);
	Serial.print(strConfig);
}
#endif //SERIAL_CONFIG

void check_Serial()
{
#ifdef USE_SETSERIAL
	String T1, Var;

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
				Pserial = T1.toFloat();          //Выставленная мощность с Serial
				setPower(Pserial);
			}
			else if (Var.substring(0, 3) == "RST")
			{
				ESP.restart();
			}
			else if (Var.substring(0, 4) == "MORE")
			{
				showInfo = 2;
			}
			else if (Var.substring(0, 5) == "DEBUG")
			{
				showInfo = 3;
			}
			Var = "";
		}
	}
#endif //USE_SETSERIAL
}


