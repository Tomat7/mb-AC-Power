#ifdef USE_DS18B20

void ds_Setup()
{
	for (int i = 0; i < nSensor; i++)
	{
		DS[i].init(DS_CONVTIME);
		display_DSpin(i);
		// DS[i].printConfig();
		String dsOK;
		if (DS[i].Connected) dsOK = "+ ";
		else dsOK = "-- ";
		dsOK += "DS " + String(i) + ": ";
		log_cfg_ln(dsOK, DS[i].LibConfig());
	}
}

void ds_Update()
{
	for (int i = 0; i < nSensor; i++)
	{
#ifdef DS_ERRORS
/*
		if ((cntrDSerrors > 0) && (cntrDSerrors < DS_ERRORS))
		{
			cntrDSerrors++;
			return;
		}
		else if (cntrDSerrors == DS_ERRORS) 
		{
			DS[i].init();
			cntrDSerrors++;
			return;
		}
		else
		{
			DS[i].check();
		}
*/
		DS[i].check();
		//DS[i].adjust();
		float t = DS[i].Temp;
		if (t > DS_MIN_T)
		{
			dsTemp[i] = t;
			dsErrors[i] = 0;
		}
		else if (dsErrors[i] > DS_ERRORS)
		{
			dsTemp[i] = t;
			dsErrors[i]++;
		}
		else 
		{
			dsErrors[i]++;
		}
#else	
		DS[i].check();
		dsTemp[i] = DS[i].Temp;
#endif	// DS_ERRORS

#ifdef SERIAL_INFO
		String dsInfo = ".  DS " + String(i, DEC) + ": " + String(dsTemp[i], 2) + " | parasite: " +
			String(DS[i].Parasite, DEC) + " | " + String(DS[i].TimeConv, DEC);
		if (showInfo > 0) log_info_ln(dsInfo);
#endif	// SERIAL_INFO
	}
}

#endif	// USE_DS18B20
