void update_Info()
{
	start_ControlTask();

	// ----------------------------
	ms0 = millis();
	check_ZeroCross();
	TEH.setpower(selectPower());
	msDuration1 = millis() - ms0;
	// ----------------------------
	ms0 = millis();
	update_DS();
	correct_DS();
	update_Cooler();
	msTempDuration = millis() - ms0;
	// ----------------------------
	ms0 = millis();
	update_Modbus();
	check_WiFi();
	check_Serial();
	msNetDuration = millis() - ms0;
	// ----------------------------
	stop_ControlTask();
}

void print_Info()
{
	//	log_info_ln("RMScounter1: ", TEH.CounterRMS);
	start_ControlTask();
	msPrint = millis();

	// ----------------------------
	ms0 = millis();
	display_Info();
	msDispDuration = millis() - ms0;

	// ----------------------------
	ms0 = millis();
	D(INFOcore = xPortGetCoreID());
	print_Debug();
	print_More();
	print_Serial();
	reset_Counters();
	msPrintDuration = millis() - ms0;

	msInfoDuration = millis() - msPrint;
	//	log_info_ln("FULL_Info_(ms): ", msInfoDuration);

	stop_ControlTask();
}

void setPower(uint16_t Ptmp)
{
	Premote = Ptmp;
}

uint16_t selectPower() // Check here for LocalControl flag, Modbus heartbeat and priority of sources.
{
	uint16_t Plocal;
	if (isLocalMode) Plocal = Pencoder;
	else Plocal = Premote;
	return Plocal;
}

void check_ZeroCross()		// переделать на 3 фазы!!
{
	zcDetectedOK = (TEH_CounterZC > 50);
}

void reset_Counters()
{
#ifdef USE_3PHASE
	for (int i = 0; i < 3; i++)
	{
		TEH.CounterZC_raw[i] = 0;
		TEH.CounterZC[i] = 0;
		TEH.CounterTR[i] = 0;
	}
	TEH.CounterRMS = 0;
#else // USE_3PHASE
	TEH.CounterZC = 0;
	TEH.CounterTR = 0;
	TEH.CounterTRopen = 0;
	TEH.CounterTRclose = 0;
#endif
}

void tick_Led()
{
	static bool led_ON;
	led_ON = !led_ON;
	digitalWrite(TICKTOCK_LED, led_ON);
}

void update_Cooler()
{
	if (dsTemp[COOLER_DS_IDX] > COOLER_TEMP_START)
	{
		digitalWrite(COOLER_PIN, HIGH);
		//		digitalWrite(PIN_COOLER2, HIGH);
	}
	else
	{
		digitalWrite(COOLER_PIN, LOW);
		//		digitalWrite(PIN_COOLER2, LOW);
	}
}


String strUptime()
{
	char UpTimeStr[21];
	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;
	int days = hr / 24;
	snprintf(UpTimeStr, 20, "%2d days %2d:%02d:%02d", days, hr % 24, min % 60, sec % 60);
	return String(UpTimeStr);
}

String strFreeMem()
{
	char FreeMemStr[14];
	snprintf(FreeMemStr, 14, "%7d bytes", ESP.getFreeHeap());
	return String(FreeMemStr);
}


void start_ControlTask()
{
#ifdef USE_TASK_INFO
	xTaskCreate(
		Control_task,	// Function to implement the task
		"ControlTask",	// Name of the task
		10000,		    // Stack size in words
		NULL,			// Task input parameter
		2,				// Priority of the task
		&ControlTask);  // Task handle
//		0);				// Core where the task should run
//	log_info_ln("+ ControlTASK started OK");
#endif
}

void stop_ControlTask()
{
#ifdef USE_TASK_INFO
	xSemaphoreGive(smInfoEnd);
	cntControlTask = 0;
#endif
}

#ifdef USE_TASK_INFO
void Control_task(void * parameter)
{
	while (xSemaphoreTake(smInfoEnd, 0) != pdTRUE)
	{
		TEH.control();
		cntControlTask++;
		delay(2);
	}
	vTaskDelete(NULL);
}
#endif


bool needUpdate(int m)
{
	return ((millis() - msUpdate) > (msUpdatePeriod * m));
}

bool needPrint(int m)
{
	return ((millis() - msPrint) > (msPrintPeriod * m));
}