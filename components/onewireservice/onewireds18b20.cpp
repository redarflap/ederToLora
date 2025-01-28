#include "onewireds18b20.h"
#include <esp_log.h>
#include "onewiredef.h"

void OneWireDS18B20::init()
{
  ESP_LOGD(TAG1WIRE, "Setting DS18B20 resolution on %016llX", device.address);
  ESP_ERROR_CHECK(ds18b20_set_resolution(ds18b20, DS18B20_RESOLUTION_12B));
}

TemperatureReading OneWireDS18B20::readTemperature()
{
  float temp;
  ESP_ERROR_CHECK(ds18b20_trigger_temperature_conversion(ds18b20));
  ESP_ERROR_CHECK(ds18b20_get_temperature(ds18b20, &temp));
  ESP_LOGD(TAG1WIRE, "DS18B20 [%016llX] temperature: %.2fC", device.address, temp);

  return TemperatureReading(device.address, temp);
}
