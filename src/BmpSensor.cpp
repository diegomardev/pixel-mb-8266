#include "BmpSensor.h"
#include <Wire.h>
#include <math.h>

bool BmpSensor::readChipId(uint8_t address, uint8_t &id) {
  Wire.beginTransmission(address); Wire.write(uint8_t(0xD0));
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(address, uint8_t(1)) != 1) return false;
  id = Wire.read(); return true;
}
bool BmpSensor::probe() {
  detected_ = false; valid_ = false; observedId_ = 0;
  for (uint8_t order = 0; order < (BMP280_TRY_REVERSED_PINS ? 2 : 1); ++order) {
    const uint8_t sda = order ? BMP280_SCL_PIN : BMP280_SDA_PIN;
    const uint8_t scl = order ? BMP280_SDA_PIN : BMP280_SCL_PIN;
    Wire.begin(sda, scl); Wire.setClock(100000); Wire.setClockStretchLimit(15000);
    for (uint8_t address : {uint8_t(0x76), uint8_t(0x77)}) {
      uint8_t id = 0;
      if (!readChipId(address, id)) continue;
      observedId_ = id;
      if (id != BMP280_CHIPID) continue;
      if (!bmp_.begin(address, BMP280_CHIPID)) continue;
      bmp_.setSampling(Adafruit_BMP280::MODE_NORMAL, Adafruit_BMP280::SAMPLING_X2,
        Adafruit_BMP280::SAMPLING_X4, Adafruit_BMP280::FILTER_X4, Adafruit_BMP280::STANDBY_MS_500);
      address_ = address; sda_ = sda; scl_ = scl; detected_ = true;
      error_ = ""; return true;
    }
  }
  error_ = observedId_ == 0x60 ? "Detectado BME280 (0x60), no BMP280" :
    (observedId_ ? "Dispositivo I2C con ID distinto de BMP280" : "BMP280 no encontrado en D3/D1, 0x76 o 0x77");
  return false;
}
void BmpSensor::tick(uint32_t now) {
  if (!BMP280_ENABLED) { valid_ = false; error_ = "Sensor desactivado en Config.h"; return; }
  if (!detected_) {
    if (attempted_ && uint32_t(now - lastAttempt_) < BMP280_RETRY_MS) return;
    attempted_ = true; lastAttempt_ = now;
    if (!probe()) return;
    // Dar tiempo a la primera muestra tras configurar el modo normal.
    lastRead_ = now; return;
  }
  if (uint32_t(now - lastRead_) < BMP280_SAMPLE_MS) return;
  lastRead_ = now;
  uint8_t id = 0;
  if (!readChipId(address_, id) || id != BMP280_CHIPID) {
    detected_ = false; valid_ = false; lastAttempt_ = now;
    error_ = "Se ha perdido la comunicacion I2C"; return;
  }
  const float temp = bmp_.readTemperature();
  const float pressure = bmp_.readPressure() / 100.0F;
  if (!isfinite(temp) || !isfinite(pressure) || temp < -40 || temp > 85 || pressure < 300 || pressure > 1100) {
    valid_ = false; error_ = "Lectura de temperatura/presion invalida"; return;
  }
  temperature_ = roundf(temp * 10) / 10;
  pressure_ = roundf(pressure * 10) / 10;
  sampledAt_ = now; valid_ = true; error_ = "";
}
