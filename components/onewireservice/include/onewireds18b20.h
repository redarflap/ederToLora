#pragma once

#include <ds18b20.h>
#include "onewiredevice.h"

typedef struct
{
  onewire_device_address_t address;
  float temp;
} TemperatureReading;

class OneWireDS18B20 : public OneWireDevice
{
private:
  ds18b20_device_handle_t ds18b20;

public:
  // Constructor
  explicit OneWireDS18B20(onewire_device_t *device, ds18b20_device_handle_t ds18b20) : OneWireDevice(device), ds18b20(ds18b20)
  {
  }

  // Destructor
  ~OneWireDS18B20() override = default;

  virtual Type getType() const { return Type::DS18B20; }

  void init() override;
  TemperatureReading readTemperature();
};

using SPOneWireDS18B20 = std::shared_ptr<OneWireDS18B20>;
