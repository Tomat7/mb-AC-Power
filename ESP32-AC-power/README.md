# ACpower
Arduino ESP32 Hi-Power regulator for AC devices

__Warning! Hi voltage is dangerous! Be careful!__
=================================================

See schemas and other details/examples on http://forum.homedistiller.ru/index.php?topic=166750.0
### Connections (for ESP32 Wemos Lolin32):
* **39** - Voltage meter (https://learn.openenergymonitor.org/electricity-monitoring/voltage-sensing/measuring-voltage-with-an-acac-power-adapter)
* **36** - ACS712 (https://learn.openenergymonitor.org/electricity-monitoring/ct-sensors/interface-with-arduino)
* **25** - ZeroCross detector ()
* **26** - Triac ()


#### Combining voltage&current meters, updated algorithm for ESP32 (c) Tomat7.
=============================================================
 