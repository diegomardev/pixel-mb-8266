#pragma once
#include <Arduino.h>
#include <LittleFS.h>
extern "C" uint32_t _FS_start, _FS_end;
inline bool mountAnimationStorage() {
  LittleFSConfig config; config.setAutoFormat(false); LittleFS.setConfig(config);
  if(LittleFS.begin()) return true;
  const uint32_t start=reinterpret_cast<uintptr_t>(&_FS_start)-0x40200000;
  const uint32_t end=reinterpret_cast<uintptr_t>(&_FS_end)-0x40200000;
  if(end<=start || end>ESP.getFlashChipRealSize()) return false;
  alignas(4) uint8_t block[256];
  for(uint32_t offset=start;offset<end;offset+=sizeof(block)) {
    if(!ESP.flashRead(offset,reinterpret_cast<uint32_t *>(block),sizeof(block))) return false;
    for(size_t i=0;i<sizeof(block);++i) if(block[i]!=0xff) return false;
    yield();
  }
  // Format only a fully erased partition. Never erase existing/damaged user files.
  return LittleFS.format() && LittleFS.begin();
}
