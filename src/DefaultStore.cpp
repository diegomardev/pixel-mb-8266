#include <Arduino.h>
#include <LittleFS.h>
#include <memory>
#include <uzlib.h>
#include "DefaultStore.h"
#include "DefaultAnimations.h"

namespace {
// Byte reads are required for compressed data in ESP8266 flash (PROGMEM).
struct DefaultInflater {
  uzlib_uncomp stream{};
  const uint8_t *data;
  size_t size, offset = 0;
  uint8_t dictionary[1024];
  uint8_t output[128];
};
int readDefaultByte(uzlib_uncomp *stream) {
  auto *state = reinterpret_cast<DefaultInflater *>(stream);
  if (state->offset >= state->size) return -1;
  return pgm_read_byte(state->data + state->offset++);
}
bool extractDefault(const DefaultAnimation &entry, File &file) {
  std::unique_ptr<DefaultInflater> state(new(std::nothrow) DefaultInflater);
  if (!state) return false;
  state->data=entry.data; state->size=entry.size;
  uzlib_uncompress_init(&state->stream,state->dictionary,sizeof(state->dictionary));
  state->stream.source_read_cb=readDefaultByte;
  // zlib header value 2 means a 1024-byte window; reject larger windows.
  const int window=uzlib_zlib_parse_header(&state->stream);
  if(window<0 || window>2) return false;
  size_t written=0;
  for (;;) {
    state->stream.dest=state->output;
    state->stream.dest_limit=state->output+sizeof(state->output);
    const int result=uzlib_uncompress_chksum(&state->stream);
    const size_t count=state->stream.dest-state->output;
    if(result<0 || written+count>entry.rawSize) return false;
    if(count && file.write(state->output,count)!=count) return false;
    written+=count; yield();
    if(result==TINF_DONE) return written==entry.rawSize && state->offset==entry.size;
    if(result!=TINF_OK || !count) return false;
  }
}
}

bool ensureAnimationDirectory() {
  if (LittleFS.exists("/animations")) {
    File dir = LittleFS.open("/animations", "r"); return dir && dir.isDirectory();
  }
  return LittleFS.mkdir("/animations");
}
bool installDefaultAnimations() {
  if (!ensureAnimationDirectory()) return false;
  const bool previousInstallation=LittleFS.exists("/defaults-v1.done") || LittleFS.exists("/defaults-v2.done") || LittleFS.exists("/defaults-v3.done");
  uzlib_init();
  uint8_t installed=0;
  for (const auto &entry : DEFAULT_ANIMATIONS) {
    if(previousInstallation || LittleFS.exists("/defaults-v4.done")) break;
    const String target=String("/animations/")+entry.name+".awx";
    if(LittleFS.exists(target)) continue; // Preserve user files and resume an interrupted first boot.
    File tmp=LittleFS.open("/seed.tmp","w");
    if(!tmp) return false;
    const bool written=extractDefault(entry,tmp);
    tmp.flush(); tmp.close();
    if(!written || !LittleFS.rename("/seed.tmp",target)) { LittleFS.remove("/seed.tmp"); return false; }
    ++installed;
  }
  // Set the marker only when extraction has finished successfully.
  if(!LittleFS.exists("/defaults-v4.done")) {
    File marker=LittleFS.open("/defaults.tmp","w"); if(!marker) return false;
    const bool written=marker.write(uint8_t(1))==1; marker.flush(); marker.close();
    if(!written || !LittleFS.rename("/defaults.tmp","/defaults-v4.done")) { LittleFS.remove("/defaults.tmp"); return false; }
  }
  if(installed) Serial.printf("Animaciones predeterminadas instaladas: %u\n",installed);
  return true;
}
