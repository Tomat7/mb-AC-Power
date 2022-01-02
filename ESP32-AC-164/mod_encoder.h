// encoder.h
#pragma once
#include "arduino.h"
#include "config.h"
#include "config_plc.h"

void encoder_Setup();
void encoder_Check();
void encoder_ShortPress();
void encoder_LongPress();
void encoder_Turn();
