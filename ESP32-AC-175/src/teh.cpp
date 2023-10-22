// 
#include "teh.h"
#include "logging.h"

#ifdef USE_3PHASE
//#include "ACpower3.h"
ACpower3 TEH(PINS_PHASE0, PINS_PHASE1, PINS_PHASE2, POWER_MAX);
#else
//#include <ACpower.h>
ACpower TEH(POWER_MAX, PIN_ZCROSS, PIN_TRIAC, PIN_U, PIN_I);
#endif

#ifdef U_CORRECTION
static float Ucorr[25] = U_CORRECTION;
#else
#define Ucorr NULL
#endif

void setup_TEH()
{
#ifdef USE_3PHASE
	TEH.initTR();						// один раз!
	TEH.initZC(ZCINT_MODE, false);		// один раз!
	TEH.initADC(ADC_RATE, ADC_WAVES);	// один раз!
	TEH.setADCratio(I_RATIO, U_RATIO, ANGLE_LAG);	// можно запускать повторно для корректировки
	TEH.setRMScorrection(NULL, Ucorr);	// можно запускать повторно во время работы
#else
	TEH.init(I_RATIO, U_RATIO);
#endif
	
	log_cfg_addstr(TEH.LibConfig);
	return;
}

void check_TEH()
{
	TEH.control();
	//TEH.control(5000);  // ручное управление углом открытия, без стабилизации!
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


