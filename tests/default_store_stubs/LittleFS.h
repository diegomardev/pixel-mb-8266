#pragma once
#include "Arduino.h"
#include <filesystem>
#include <fstream>
#include <memory>
#include <utility>
inline size_t testWriteBudget=SIZE_MAX;
class File {
 std::shared_ptr<std::ofstream> output; bool valid=false,directory=false;
 public:
 File()=default;
 File(const std::filesystem::path &p,const char *mode){directory=std::filesystem::is_directory(p);if(*mode=='w'){output=std::make_shared<std::ofstream>(p,std::ios::binary);valid=bool(*output);}else valid=std::filesystem::exists(p);}
 explicit operator bool()const{return valid;}
 bool isDirectory()const{return directory;}
 size_t write(const uint8_t *data,size_t size){if(!output||size>testWriteBudget)return 0;output->write((const char *)data,size);if(testWriteBudget!=SIZE_MAX)testWriteBudget-=size;return *output?size:0;}
 size_t write(uint8_t value){return write(&value,1);}
 void flush(){if(output)output->flush();}
 void close(){if(output)output->close();valid=false;}
};
struct TestFS {
 std::filesystem::path root;
 std::filesystem::path path(const String &p)const{return root/p.substr(1);}
 bool exists(const String &p){return std::filesystem::exists(path(p));}
 File open(const String &p,const char *mode){return File(path(p),mode);}
 bool mkdir(const String &p){return std::filesystem::create_directory(path(p));}
 bool rename(const String &a,const String &b){std::error_code ec;std::filesystem::rename(path(a),path(b),ec);return !ec;}
 bool remove(const String &p){return std::filesystem::remove(path(p));}
};
inline TestFS LittleFS;
