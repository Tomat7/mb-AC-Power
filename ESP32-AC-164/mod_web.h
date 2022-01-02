// web.h
#include "arduino.h"
#include "config.h"
#include "config_plc.h"

#ifdef USE_WEBSERVER

void web_Setup();
void web_Check();
void web_handleRoot();
void web_handleON();
void web_handleOFF();
void web_handleRST();
void web_handleNotFound();
String web_SendHTML(uint16_t Ptmp);

#endif //USE_WEBSERVER

