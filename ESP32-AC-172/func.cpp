//
#include "func.h"
#include "network.h"
#include "modules.h"
#include "macros.h"
#include "teh.h"

TaskHandle_t ControlTask = NULL;
volatile SemaphoreHandle_t smInfoEnd;
volatile uint16_t cntControlTask = 0;
volatile uint32_t INFOcore = 6, OLEDcore = 8;

uint8_t showInfo = 0;
uint16_t Pweb = 0, Pserial = 0, Premote = 0;
int16_t Pencoder = 0;
uint16_t msPrintPeriod = SHOW_PERIOD, msUpdatePeriod = SHOW_PERIOD, msTelnetPeriod = SHOW_PERIOD;
uint32_t msPrint = 0, msUpdate = 0;
uint32_t ms0 = 0, msDispDuration, msTempDuration, msNetDuration, msDuration1, msPrintDuration;

bool isScreenSaver = false;
bool isShowPower = true;
bool isSetMenu = false;
bool isLocalMode = false;

bool isMBmasterOK = true;
bool isZCdetectedOK = false;

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

void tick_Led()
{
  static bool led_ON;
  led_ON = !led_ON;
  digitalWrite(TICKTOCK_LED, led_ON);
}

bool needUpdate(int m)
{
  return ((millis() - msUpdate) > (msUpdatePeriod * m));
}

bool needPrint(int m)
{
  return ((millis() - msPrint) > (msPrintPeriod * m));
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

String strIPaddr()
{
  //return IPtoStr(WiFi.localIP());
  return WiFi.localIP().toString();
}

String strMACaddr()
{
  return WiFi.macAddress();
}

String strMACaddr(byte mac[6])
{
  String macStr;
  for (byte i = 0; i < 6; ++i)
  {
    macStr += String(mac[i], HEX);
    if (i < 5) macStr += ":";
  }
  macStr.toUpperCase();
  return macStr;
}

String strWiFiName()
{
  return WIFI_NAME;
}

String strWiFiRSSI()
{
  return String(WiFi.RSSI());
  //	return WiFi.RSSI().toString();
}

String strWiFiBSSID()
{
  return WiFi.BSSIDstr();
}

void check_ZeroCross()		// переделать на 3 фазы!!
{
  isZCdetectedOK = (TEH_CounterZC > 50);
}


void update_Cooler()
{
  if (dsTemp[COOLER_DS_ID] > COOLER_DS_TEMP) digitalWrite(COOLER_PIN, HIGH);
  else digitalWrite(COOLER_PIN, LOW);
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
  log_info_ln("+ ControlTASK started OK");
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
void Control_task(void* parameter)
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


void print_Serial()
{
  if (showInfo > 0)
  {
    log_info_ln("==================");
    log_info_ln("Pset: ", TEH.Pset);
    log_info_ln("Pnow: ", TEH.Pnow);
    log_info_ln("Angle: ", TEH.Angle);

    log_info_ln("Dur1Period(ms): ", msDuration1);
    log_info_ln("TempPeriod(ms): ", msTempDuration);
    log_info_ln("DispPeriod(ms): ", msDispDuration);
    log_info_ln("NetwPeriod(ms): ", msNetDuration);
    log_info_ln("PrntPeriod(ms): ", msPrintDuration);
    //		log_info_ln("FULL_Info_(ms): ", msInfoDuration);
  }
  DPRINTLN(showInfo);
}

void print_More()		// MORE start
{
  if (showInfo > 1)
  {
    log_info_ln("===");
    log_info_ln("Pweb: ", Pweb);
    log_info_ln("Penc: ", Pencoder);
    log_info_ln("RMScounter/sec: ", TEH.CounterRMS);
#ifdef USE_3PHASE
    log_info_ln("Ucounter: ", TEH.CounterU);
    log_info_ln("Icounter: ", TEH.CounterI);
#endif
    log_info_ln("X2: ", TEH.X2);
  }
}

void print_Debug()
{
  if (showInfo > 2)
  {
    log_info_ln("===");
#ifdef USE_3PHASE
    for (int i = 0; i < 3; i++)
    {
      if (TEH.ZC[i]) log_info_ln(String(i) + " ZC - ok");
      else log_info_ln(String(i) + " no ZC");
      //			log_info_ln("  T =  ", String(dsTemp[i]));
      log_info_ln("  ZCross_raw/sec: ", TEH.CounterZC_raw[i]);
      TEH.CounterZC_raw[i] = 0;
      log_info_ln("  ZCross/sec: ", TEH.CounterZC[i]);
      TEH.CounterZC[i] = 0;
      log_info_ln("  TriacInts/sec: ", TEH.CounterTR[i]);
      TEH.CounterTR[i] = 0;

      log_info("  I = ", TEH.I[i]);
      log_info("  U = ", TEH.U[i]);
      log_info("  P = ", TEH.P[i]);
      log_info("  A = ", TEH.A[i]);
      log_info_ln("  T = ", String(dsTemp[i]));
    }
#else
    for (int i = 0; i < 3; i++) log_info_ln("  T = ", String(dsTemp[i]) + " DSerr: " + String(dsErrors[i]));
    log_info("U = ", TEH.Unow);
    log_info_ln("  I = ", TEH.Inow);
    log_info("ZCross/sec: " + String(TEH.CounterZC)); //_zcCounter = 0;
    log_info_ln("TriacInts/sec: ", TEH.CounterTR); //_trCounter = 0;
    //D(log_info_f("TriacOpens/sec: ", TEH.CounterTRopen);) //_trOpens = 0;
    //D(log_info_f("TriacClose/sec: ", TEH.CounterTRclose);) //_trOpens = 0;
    //D(log_info_f("TimerOpen: ", TEH.TimerTRopen);) //_tmrRead = 0;
    //D(log_info_f("TimerClose: ", TEH.TimerTRclose);) //_tmrRead = 0;
    log_info_ln("--");
#endif // USE_3PHASE

    // DEBUG2 start
    //log_debug_f("_Summ ", int(_summ));	// ?? wtf
    //log_debug_f("_U ", (sqrt((float)_summ / (float)_cntr) * U_RATIO));  // ?? wtf
    //D(log_info_f("Umax: ", adcUmax);)
    //D(log_info_f("Imax: ", adcImax);)
    //D(log_info_ln("--");)
    D(log_info_ln("INFO core ", INFOcore));
    D(log_info_ln("OLED core ", OLEDcore));
    /*
      D(log_info_ln("RMS core ", TEH.RMScore));
      D(log_info_ln("RMS prio ", TEH.RMSprio));
      D(log_info_ln("ZCROSS core ", TEH.ZCcore));
      D(log_info_ln("ZCROSS prio ", TEH.ZCprio));
      D(log_info_ln("TRIAC core ", TEH.TRIACcore));
      D(log_info_ln("TRIAC prio ", TEH.TRIACprio));
      D(log_info_ln("ADC core ", TEH.ADCcore));
      D(log_info_ln("ADC prio ", TEH.ADCprio));
    */
    D(log_info_ln("PRINT core ", xPortGetCoreID()));
    D(log_info_ln("PRINT prio ", uxTaskPriorityGet(NULL)));

    D(log_info_ln("--"));

    //D(log_info_f("ZCdn duration ", usZCduration);)
    //D(log_info_f("ZCtoTRIAC ", usZCtoTRIAC);)
    //D(log_info_f("TRIAC open uSec ", usTRopen);)
    //D(log_info_f("portTICK_PERIOD ", portTICK_PERIOD_MS);)
    //log_debug_f("local_buf ", Pencoder);
    // DEBUG end
    //log_info_ln(strInfo);
    //strInfo = "";
  }

}
