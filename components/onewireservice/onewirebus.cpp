#include <stdio.h>
#include "onewirebus.h"
#include "onewire_bus.h"
#include <esp_log.h>
#include <esp_err.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "onewire_device.h"
#include "onewiredef.h"

void OneWireBus::init()
{
  ESP_LOGD(TAG1WIRE, "Initializing bus on pin: %d", gpio);
  onewire_bus_config_t bus_config = {
      .bus_gpio_num = gpio,
  };
  onewire_bus_rmt_config_t rmt_config = {
      .max_rx_bytes = 10, // 1byte ROM command + 8byte ROM number + 1byte device command
  };

  ESP_ERROR_CHECK(onewire_new_bus_rmt(&bus_config, &rmt_config, &handle));
}

int OneWireBus::searchDevices()
{

  ESP_LOGD(TAG1WIRE, "Searching devices bus pin: %d", gpio);

  // ds18b20_device_handle_t ds18b20s[MAX_DS18B20_PER_GPIO];
  onewire_device_iter_handle_t iter;
  onewire_device_t next_onewire_device;
  esp_err_t search_result = ESP_OK;

  // create 1-wire device iterator, which is used for device search
  ESP_ERROR_CHECK(onewire_new_device_iter(handle, &iter));
  do
  {
    search_result = onewire_device_iter_get_next(iter, &next_onewire_device);
    if (search_result == ESP_OK)
    {
      devices.push_back(OneWireDevice::Create(&next_onewire_device));
    }
    else
    {
      break;
    }

    vTaskDelay(pdMS_TO_TICKS(100));

  } while (search_result != ESP_ERR_NOT_FOUND);

  // ESP_ERROR_CHECK(onewire_del_device_iter(iter));
  //  onewire_bus_del(bus);

  return 0;
}
