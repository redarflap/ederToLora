#include <string.h>

#include "esp_log.h"
#include <esp_err.h>
#include "onewireservice.h"
#include "onewire_bus.h"
#include "onewirebus.h"
#include "onewiredef.h"
#include "onewireds18b20.h"
#include <freertos/FreeRTOS.h>

OneWireService::OneWireService(std::vector<gpio_num_t> gpios)
{
  for (gpio_num_t gpio : gpios)
  {
    buses.push_back(OneWireBus::Create(gpio));
  }
}

void OneWireService::init()
{
  ESP_LOGI(TAG1WIRE, "Initializing service ...");
  vTaskDelay(pdMS_TO_TICKS(2000));
  for (SPOneWireBus bus : buses)
  {
    bus->init();
  }
}

int OneWireService::searchDevices()
{
  int found = 0;
  for (SPOneWireBus bus : buses)
  {
    found += bus->searchDevices();
  }
  return found;
}

std::vector<SPOneWireDevice> OneWireService::getDevices()
{
  std::vector<SPOneWireDevice> devices;

  for (auto bus : buses)
  {
    std::copy_if(bus->getDevices().begin(), bus->getDevices().end(), std::back_inserter(devices),
                 [](const SPOneWireDevice &device)
                 { return true; });
  }

  return devices;
}

std::vector<SPOneWireDS18B20> OneWireService::getDS18B20s()
{
  std::vector<SPOneWireDS18B20> devices;
  for (SPOneWireBus bus : buses)
  {
    for (auto device : bus->getDevices())
    {
      if (device->getType() == OneWireDevice::Type::DS18B20)
      {
        devices.push_back(
            std::static_pointer_cast<OneWireDS18B20>(device));
      }
    }
  }

  ESP_LOGD(TAG1WIRE, "Got %d devices", devices.size());

  return devices;
}

std::vector<TemperatureReading> OneWireService::readTemperatures()
{
  ESP_LOGD(TAG1WIRE, "Reading temperatures");
  std::vector<SPOneWireDS18B20>
      sensors = getDS18B20s();
  std::vector<TemperatureReading> data;

  for (auto ds18b20 : sensors)
  {
    data.push_back(ds18b20->readTemperature());
  }
  return data;
}
