#pragma once

#include <stdio.h>
#include <memory>
#include <functional>
#include "driver/gpio.h"
#include "onewire_types.h"
#include "ds18b20.h"
#include "onewiredevice.h"
#include "esp_log.h"

class OneWireBus
{
private:
  onewire_bus_handle_t handle;
  gpio_num_t gpio;
  std::vector<SPOneWireDevice> devices;
  /* data */
public:
  OneWireBus(gpio_num_t gpio) : gpio(gpio) {};
  ~OneWireBus() {};
  static std::shared_ptr<OneWireBus> Create(gpio_num_t gpio)
  {
    // Call the private constructor and wrap the result in a shared_ptr
    return std::make_shared<OneWireBus>(gpio);
  }

  void init();
  int searchDevices();
  std::vector<SPOneWireDevice> getDevices()
  {
    return devices;
  }
};

using SPOneWireBus = std::shared_ptr<OneWireBus>;
