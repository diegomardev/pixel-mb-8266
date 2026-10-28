// Enlaza el cpp REAL de Adafruit NeoMatrix; solo GFX/NeoPixel se sustituyen.
#include "PixelMatrix.h"
#include <cassert>
#include <iostream>
int main() {
  // Reproduce exactamente el error del firmware 1.0.1 con tipos uint8_t.
  Adafruit_NeoMatrix legacy(MATRIX_WIDTH,MATRIX_HEIGHT,MATRIX_PIN,MATRIX_LAYOUT,MATRIX_TYPE);
  assert(legacy.numPixels()==12288); assert(legacy.getPin()==82);
  PixelMatrix matrix;
  assert(matrix.configurationValid()); assert(matrix.numPixels()==256);
  assert(matrix.getPin()==4 && matrix.width()==32 && matrix.height()==8);
  matrix.fillScreen(0);
  matrix.drawPixel(0,0,0xffff); assert(matrix.getPixelColor(0)!=0);
  matrix.drawPixel(1,0,0xffff); assert(matrix.getPixelColor(15)!=0);
  matrix.fillScreen(0);
  for(int y=0;y<8;y++)for(int x=0;x<32;x++)matrix.drawPixel(x,y,0xffff);
  for(int i=0;i<256;i++)assert(matrix.getPixelColor(i)!=0);
  std::cout<<"Regresion confirmada: antes 12288 LED/GPIO82; corregido 256 LED/GPIO4 y mapeo 32x8\n";
}
