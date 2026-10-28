#pragma once
#include <Arduino.h>
bool loadNetworkConfig(String &ssid, String &password);
bool saveNetworkConfig(const String &ssid, const String &password);
