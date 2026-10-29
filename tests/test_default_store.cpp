#include <Arduino.h>
#include <LittleFS.h>
#include "DefaultStore.h"
#include "DefaultAnimations.h"
#include <cassert>
#include <iterator>
#include <vector>
std::vector<char> read(const std::filesystem::path &p){std::ifstream f(p,std::ios::binary);return {std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>()};}
int main(int argc,char **argv){
 assert(argc==3);const std::filesystem::path temp=argv[1],golden=argv[2];
 std::filesystem::create_directories(temp);LittleFS.root=temp;
 assert(sizeof(DEFAULT_ANIMATIONS)/sizeof(DEFAULT_ANIMATIONS[0])==42);
 // Simulate power/write failure before the first file can be completed.
 testWriteBudget=0;assert(!installDefaultAnimations());assert(!LittleFS.exists("/defaults-v4.done"));assert(!LittleFS.exists("/seed.tmp"));
 testWriteBudget=SIZE_MAX;assert(installDefaultAnimations());assert(LittleFS.exists("/defaults-v4.done"));
 for(const auto &entry:DEFAULT_ANIMATIONS)assert(read(temp/"animations"/(String(entry.name)+".awx"))==read(golden/(String(entry.name)+".awx")));
 // Deletions and user edits persist across subsequent boots.
 std::filesystem::remove(temp/"animations/1027.awx");std::ofstream(temp/"animations/693.awx")<<"user edit";
 assert(installDefaultAnimations());assert(!LittleFS.exists("/animations/1027.awx"));assert(read(temp/"animations/693.awx")==std::vector<char>({'u','s','e','r',' ','e','d','i','t'}));
 // Old-install markers suppress the new package without filling existing boards.
 std::filesystem::create_directories(temp/"existing");LittleFS.root=temp/"existing";std::ofstream(LittleFS.root/"defaults-v1.done")<<1;
 assert(installDefaultAnimations());assert(std::filesystem::is_empty(LittleFS.root/"animations"));
 std::puts("Default library: 42 exact extractions, interrupted install retry, checksums, preserved edits/deletions and old installations passed.");
}
