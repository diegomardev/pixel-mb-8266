#include "BmpSensor.h"
#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
TwoWire Wire;BmpReadings readings;
void reset(){Wire=TwoWire();readings=BmpReadings();}
int main(){
  reset();BmpSensor normal;normal.tick(0);assert(normal.detected()&&!normal.valid());
  normal.tick(1999);assert(!normal.valid());normal.tick(2000);
  assert(normal.valid()&&normal.sdaPin()==0&&normal.sclPin()==5&&normal.address()==0x76);
  assert(std::abs(normal.temperatureC()-23.5F)<0.01F&&std::abs(normal.pressureHpa()-1002.3F)<0.01F);
  Wire.online=false;normal.tick(4000);assert(!normal.valid()&&!normal.detected());
  Wire.online=true;normal.tick(13999);assert(!normal.detected());
  normal.tick(14000);normal.tick(16000);assert(normal.valid());
  readings.pressurePa=0;normal.tick(18000);assert(!normal.valid());
  readings.pressurePa=101325;readings.temp=NAN;normal.tick(20000);assert(!normal.valid());
  reset();Wire.expectedSda=5;Wire.expectedScl=0;Wire.expectedAddress=0x77;
  BmpSensor reversed;reversed.tick(0);reversed.tick(2000);
  assert(reversed.valid()&&reversed.sdaPin()==5&&reversed.sclPin()==0&&reversed.address()==0x77);
  reset();Wire.id=0x60;BmpSensor bme;bme.tick(0);
  assert(!bme.detected()&&!bme.valid()&&bme.observedChipId()==0x60&&strstr(bme.error(),"BME280"));
  reset();Wire.online=false;BmpSensor absent;absent.tick(0);assert(!absent.valid()&&!absent.detected());
  reset();readings.beginOK=false;BmpSensor failed;failed.tick(0);assert(!failed.detected());
  std::cout<<"BMP280: pines/address alternativos, conversion hPa, redondeo, ausente, recuperacion y lecturas invalidas OK\n";
}
