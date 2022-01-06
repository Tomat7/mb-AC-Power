// teh.h
#include "arduino.h"
#include "config.h"
#include "config_plc.h"

#ifdef USE_3PHASE
#include "ACpower3.h"
extern ACpower3 TEH;
#else
#include "ACpower.h"
extern ACpower TEH;
#endif

void setup_TEH();
void check_TEH();
void reset_Counters();
