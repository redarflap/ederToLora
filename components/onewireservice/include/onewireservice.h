#pragma once
#include <string>
#include <memory>
#include <functional>
#include "driver/gpio.h"
#include "onewirebus.h"
#include "onewiredevice.h"
#include "onewireds18b20.h"

class OneWireService
{

private:
  std::vector<SPOneWireBus> buses;

public:
  OneWireService(std::vector<gpio_num_t> gpios);
  ~OneWireService() {}

  void init();
  int searchDevices();
  std::vector<SPOneWireDevice> getDevices();
  std::vector<SPOneWireDS18B20> getDS18B20s();
  std::vector<TemperatureReading> readTemperatures();
};
