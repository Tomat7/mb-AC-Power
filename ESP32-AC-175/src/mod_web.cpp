// 
#include "mod_web.h"
#include "macros.h"
#include "logging.h"
#include "func.h"
#include "teh.h"
#include "modules.h"

#ifdef USE_WEBSERVER
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
WebServer httpd(80);
#else
#pragma message "WEB not configured! Nothing to compile. "
#endif //USE_WEBSERVER

#ifdef USE_WEBSERVER

void web_Setup()
{
	if (MDNS.begin(OTA_HOSTNAME)) { log_cfg_ln("+ MDNS responder started."); }
	httpd.on("/", web_handleRoot);
	httpd.on("/on", web_handleON);
	httpd.on("/off", web_handleOFF);
	httpd.on("/reset", web_handleRST);
	//httpd.on("/inline", [] () { httpd.send(200, "text/plain", "this works as well"); });
	httpd.onNotFound(web_handleNotFound);
	httpd.begin();
}

void web_Check()
{
	httpd.handleClient();
}

void web_handleRoot()
{
	httpd.send(200, "text/html", web_SendHTML(TEH.Pset));
}

void web_handleON()
{
	if ((httpd.args() == 1) && (httpd.argName(0) == "p"))
	{
		String sPweb = httpd.arg(0);
		Pweb = sPweb.toInt();
		setPower(Pweb);
	}
	httpd.send(200, "text/html", web_SendHTML(Pweb));
}

void web_handleOFF()
{
	Pweb = 0;
	setPower(Pweb);
	httpd.send(200, "text/html", web_SendHTML(Pweb));
}

void web_handleRST()
{
	Pweb = 0;
	setPower(Pweb);
	TEH.stop();
	delay(20);
	ESP.restart();
	httpd.send(200, "text/html", web_SendHTML(Pweb));
}

void web_handleNotFound()
{
	//String UriPath = httpd.uri();
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += httpd.uri();
	//message += UriPath.substring(1, 3);
	//message += UriPath.substring(3);
	message += "\nMethod: ";
	message += (httpd.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += httpd.args();
	message += "\n";

	for (uint8_t i = 0; i < httpd.args(); i++)
	{
		message += " " + httpd.argName(i) + ": " + httpd.arg(i) + "\n";
	}

	httpd.send(404, "text/plain", message);
}

String web_SendHTML(uint16_t Ptmp)
{
	char HtmlStr[50];
	//char CmpDate[10];
	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;

	String ptr = "<!DOCTYPE html> <html>\n";
	ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
	ptr += "<meta http-equiv=\"refresh\" content=\"5;url=/\">\n";
	ptr += "<title>Power Controller</title>\n";
	ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
	ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
	ptr += ".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
	ptr += ".button-on {background-color: #3366CC;}\n"; //#3498db #336699
	ptr += ".button-on:active {background-color: #003399;}\n"; //#2980b9
	ptr += ".button-off {background-color: #FF0033;}\n";
	ptr += ".button-off:active {background-color: #CC0000;}\n";
	ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
	ptr += "</style>\n";
	ptr += "</head>\n";
	ptr += "<body>\n";
	ptr += "<h1>ESP32 AC Power regulator</h1>\n";

	snprintf(HtmlStr, 50, "<h3>Pset: %4d, Pnow: %4d.</h3>\n", Ptmp, TEH.Pnow);
	ptr += HtmlStr;
	snprintf(HtmlStr, 50, "<h3>Temp1: %.2f </h3>\n", dsTemp[0]);
	ptr += HtmlStr;
	D(snprintf(HtmlStr, 50, "<p>_Ucntr: %4d, _Icntr: %4d.</p>\n", TEH._Ucntr, TEH._Icntr));
	D(ptr += HtmlStr);
	D(snprintf(HtmlStr, 50, "<p>_ZCcntr/sec: %4d, RMScntr/sec: %4d.</p>\n", TEH_CounterZC, TEH.CounterRMS));
	D(ptr += HtmlStr);

	snprintf(HtmlStr, 50, "<p>Uptime: %03d:%02d:%02d</p>\n", hr, min % 60, sec % 60);
	ptr += HtmlStr;

	ptr += "<a class=\"button button-off\" href=\"/off\">STOP</a>\n";
	//ptr += "<p><a class=\"button button-on\" href=\"/on?p=100\">100</a>\n";

	String PButton = "<p><a class=\"button button-on\" href=\"/on?p=";

#ifdef WEB_POWER1
	snprintf(HtmlStr, 50, "%d\">%d</a>", WEB_POWER1, WEB_POWER1);
	ptr += PButton + HtmlStr;
#endif
#ifdef WEB_POWER2
	snprintf(HtmlStr, 50, "%d\">%d</a>", WEB_POWER2, WEB_POWER2);
	ptr += PButton + HtmlStr;
#endif
#ifdef WEB_POWER3
	snprintf(HtmlStr, 50, "%d\">%d</a>", WEB_POWER3, WEB_POWER3);
	ptr += PButton + HtmlStr;
#endif
#ifdef WEB_POWER4
	snprintf(HtmlStr, 50, "%d\">%d</a>", WEB_POWER4, WEB_POWER4);
	ptr += PButton + HtmlStr;
#endif
#ifdef WEB_POWER5
	snprintf(HtmlStr, 50, "%d\">%d</a>", WEB_POWER5, WEB_POWER5);
	ptr += PButton + HtmlStr;
#endif
#ifdef WEB_POWER6
	snprintf(HtmlStr, 50, "%d\">%d</a>", WEB_POWER6, WEB_POWER6);
	ptr += PButton + HtmlStr;
#endif
#ifdef WEB_POWER7
	snprintf(HtmlStr, 50, "%d\">%d</a>", WEB_POWER7, WEB_POWER7);
	ptr += PButton + HtmlStr;
#endif

	/* ptr += "<p><a class=\"button button-on\" href=\"/on?p=1000\">1000</a>\n";
	   ptr += "<p><a class=\"button button-on\" href=\"/on?p=1500\">1500</a>\n";
	   ptr += "<p><a class=\"button button-on\" href=\"/on?p=1700\">1700</a>\n";
	   ptr += "<p><a class=\"button button-on\" href=\"/on?p=3000\">3000</a>\n";
	  */

	snprintf(HtmlStr, 50, "<p>Compiled on %s (C) Tomat7</p>\n", __DATE__);
	ptr += HtmlStr;

	if (isMBmasterOK) snprintf(HtmlStr, 50, "<p>Modbus OK</p>\n");
	else snprintf(HtmlStr, 50, "<p>Modbus OFF</p>\n");
	ptr += HtmlStr;

	if (!isLocalMode)
	{
		snprintf(HtmlStr, 50, "<p>Local mode</p>\n");
		ptr += HtmlStr;
	}

	ptr += "</body>\n";
	ptr += "</html>\n";
	return ptr;
}

void DateToBuff(char const* date, char* buff)
{
	int month, day, year;
	static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
	sscanf(date, "%s %d %d", buff, &day, &year);
	month = (strstr(month_names, buff) - month_names) / 3 + 1;
	snprintf(buff, 10, "%d%02d%02d", year, month, day);
}
#endif //USE_WEBSERVER

