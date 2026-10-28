#include "NetworkConfig.h"
#include <EEPROM.h>
#include <cassert>
#include <iostream>
FakeEEPROM EEPROM;
int main() {
  String ssid="fallback",password="original";
  assert(!loadNetworkConfig(ssid,password)); assert(ssid=="fallback" && password=="original");
  assert(saveNetworkConfig("Mi WiFi","secreto123"));
  assert(loadNetworkConfig(ssid,password)); assert(ssid=="Mi WiFi" && password=="secreto123");
  EEPROM.bytes[9]^=1; assert(!loadNetworkConfig(ssid,password)); assert(ssid=="Mi WiFi");
  assert(!saveNetworkConfig("","12345678")); assert(!saveNetworkConfig(String(33,'x'),"12345678"));
  assert(!saveNetworkConfig("Red","short")); assert(!saveNetworkConfig("Red",String(64,'x')));
  assert(saveNetworkConfig("Red abierta","")); assert(loadNetworkConfig(ssid,password)); assert(password.empty());
  assert(saveNetworkConfig(String(32,'s'),String(63,'p'))); assert(loadNetworkConfig(ssid,password));
  assert(ssid.length()==32 && password.length()==63);
  EEPROM.commitOK=false; assert(!saveNetworkConfig("Red","12345678"));
  std::cout<<"NetworkConfig.cpp: persistencia, checksum, limites y error de escritura correctos\n";
}
