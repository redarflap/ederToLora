#include <stdio.h>
#include "onewiredevice.h"
#include <esp_log.h>
#include <esp_err.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <onewire_device.h>
#include <onewireds18b20.h>
#include <onewiredef.h>

std::shared_ptr<OneWireDevice> OneWireDevice::Create(onewire_device_t *device)
{
  ds18b20_config_t ds_cfg = {};
  ds18b20_device_handle_t ds18b20;
  if (ds18b20_new_device(device, &ds_cfg, &ds18b20) == ESP_OK)
  {
    ESP_LOGI(TAG1WIRE, "Found a DS18B20, address: %016llX", device->address);
    return std::make_shared<OneWireDS18B20>(device, ds18b20);
  }
  else
  {
    ESP_LOGW(TAG1WIRE, "Found an unknown 1wire device, address: %016llX", device->address);
    return std::make_shared<OneWireDevice>(device);
  }
}
