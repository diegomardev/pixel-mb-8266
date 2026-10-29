#pragma once
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <string>
#define PROGMEM
#define pgm_read_byte(p) (*(const uint8_t *)(p))
using String=std::string;
inline void yield() {}
struct TestSerial { template<class... T> void printf(const char *fmt,T... values){std::printf(fmt,values...);} };
inline TestSerial Serial;
