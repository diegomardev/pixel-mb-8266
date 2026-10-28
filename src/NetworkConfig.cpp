#include "NetworkConfig.h"
#include <EEPROM.h>
namespace {
constexpr size_t PAYLOAD_SIZE = 102, RECORD_SIZE = PAYLOAD_SIZE + 4;
uint32_t checksum(const uint8_t *data) {
  uint32_t result = 2166136261UL;
  for (size_t i = 0; i < PAYLOAD_SIZE; ++i) result = (result ^ data[i]) * 16777619UL;
  return result;
}
bool valid(const String &ssid, const String &password) {
  return ssid.length() >= 1 && ssid.length() <= 32 &&
    (password.length() == 0 || (password.length() >= 8 && password.length() <= 63));
}
}
bool loadNetworkConfig(String &ssid, String &password) {
  uint8_t data[RECORD_SIZE]; EEPROM.begin(512);
  for (size_t i = 0; i < RECORD_SIZE; ++i) data[i] = EEPROM.read(i);
  EEPROM.end();
  if (memcmp(data, "AWN1", 4) || !memchr(data + 4, 0, 33) || !memchr(data + 37, 0, 65)) return false;
  uint32_t stored = 0;
  for (uint8_t i = 0; i < 4; ++i) stored |= uint32_t(data[PAYLOAD_SIZE + i]) << (i * 8);
  if (checksum(data) != stored) return false;
  String nextSsid(reinterpret_cast<char *>(data + 4)), nextPass(reinterpret_cast<char *>(data + 37));
  if (!valid(nextSsid, nextPass)) return false;
  ssid = nextSsid; password = nextPass; return true;
}
bool saveNetworkConfig(const String &ssid, const String &password) {
  if (!valid(ssid, password)) return false;
  uint8_t data[RECORD_SIZE] = {};
  memcpy(data, "AWN1", 4); memcpy(data + 4, ssid.c_str(), ssid.length());
  memcpy(data + 37, password.c_str(), password.length());
  uint32_t hash = checksum(data);
  for (uint8_t i = 0; i < 4; ++i) data[PAYLOAD_SIZE + i] = hash >> (i * 8);
  EEPROM.begin(512);
  for (size_t i = 0; i < RECORD_SIZE; ++i) EEPROM.write(i, data[i]);
  bool saved = EEPROM.commit(); EEPROM.end(); return saved;
}
