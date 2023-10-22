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

  if (isLocalMode)
    Plocal = Pencoder;
  else
    Plocal = Premote;

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
  // return IPtoStr(WiFi.localIP());
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
    if (i < 5)
      macStr += ":";
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

void check_ZeroCross() // переделать на 3 фазы!!
{
  isZCdetectedOK = (TEH_CounterZC > 50);
}

void update_Cooler()
{
  if (dsTemp[COOLER_DS_ID] > COOLER_DS_TEMP)
    digitalWrite(COOLER_PIN, HIGH);
  else
    digitalWrite(COOLER_PIN, LOW);
}

void start_ControlTask()
{
#ifdef USE_TASK_INFO
  xTaskCreate(
      Control_task,  // Function to implement the task
      "ControlTask", // Name of the task
      10000,         // Stack size in words
      NULL,          // Task input parameter
      2,             // Priority of the task
      &ControlTask); // Task handle
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
void Control_task(void *parameter)
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
