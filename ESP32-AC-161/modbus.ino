#ifdef USE_MODBUS

void modbus_Setup()
{
	mb.slave();
	mb.addHreg(hrSECONDS);
	for (int i = 0; i < nSensor; i++) mb.addHreg(hrDSTEMP0 + i); 	// Регистр температуры
	mb.addHreg(hrPSET);
	mb.addHreg(hrPNOW);
	mb.addHreg(hrANGLE);
	for (int i = 0; i < 3; i++) mb.addHreg(hrX0 + i);

#ifdef USE_3PHASE
	for (int i = 0; i < 3; i++)
	{
		mb.addHreg(hrP0 + i);
		mb.addHreg(hrI0 + i);
		mb.addHreg(hrU0 + i);
	}
#endif //USE_3PHASE

}

void modbus_Check()
{
	mb.task();
}

void modbus_Update()
{
	// если мастер онлайн - он должен записывать 0 в регистр SECONDS
	// это будет признаком "живости" Мастера Modbus'а для модуля
	// и наоборот: не 0 в SECONDS - признак "живости" модуля для Мастера
	// хотя Мастеру логичнее отслеживать "живость" по GetQuality
	bool prevMasterOK = mbMasterOK;

	if ((mb.Hreg(hrSECONDS) == 0) && !isLocalMode) mb.Hreg(hrSECONDS, msPrint / 1000);
	mbMasterOK = (((uint16_t)(msPrint / 1000) - mb.Hreg(hrSECONDS)) < MB_TIMEOUT);

	Pmodbus = mb.Hreg(hrPSET);

	if (mbMasterOK)
	{
		setPower(Pmodbus);  // test !!
		if (showInfo > 0) log_info_ln("+ MODBUS ok");
	}
	else
	{
		//	mb.Hreg(hrPSET, 0);
		if (showInfo > 0) log_info_ln("-- MODBUS Master OFFline ", mb.Hreg(hrSECONDS));
	}

	if (mbMasterOK != prevMasterOK) setPower(0);

	mb.Hreg(hrPSET, TEH.Pset);
	for (int i = 0; i < nSensor; i++) { mb.Hreg(hrDSTEMP0 + i, round(dsTemp[i] * 100)); }

	mb.Hreg(hrPNOW, TEH.Pavg);
	mb.Hreg(hrANGLE, TEH.Angle);

#ifdef USE_DS18B20
	mb.Hreg(hrX0, DS[0].THreg);
	mb.Hreg(hrX1, DS[0].TLreg);
#endif

#ifdef USE_3PHASE
	for (int i = 0; i < 3; i++) 
	{ 
		mb.Hreg(hrP0 + i, TEH.P[i]);
		mb.Hreg(hrI0 + i, round(TEH.I[i] * 1000));
		mb.Hreg(hrU0 + i, round(TEH.U[i]));
	}
#else
	mb.Hreg(hrI0, round(TEH_INOW * 1000));
	mb.Hreg(hrU0, round(TEH_UNOW));
#endif //USE_3PHASE

}

#endif //USE_MODBUS
