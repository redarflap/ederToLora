#pragma once

#include <memory>
#include "onewire_types.h"
#include "onewire_bus.h"
#include "ds18b20.h"
#include "ds18b20_types.h"

// Base class representing a generic OneWire device
class OneWireDevice
{

public:
  enum class Type
  {
    Unknown,
    DS18B20
  };

  onewire_device_t device;
  virtual Type getType() const { return Type::Unknown; }

  // Constructor
  explicit OneWireDevice(onewire_device_t *_device)
  {
    device = *_device;
  }

  // Virtual destructor for proper polymorphic behavior
  virtual ~OneWireDevice() = default;

  // Factory method for detecting and creating the correct device type
  static std::shared_ptr<OneWireDevice> Create(onewire_device_t *addr);

  virtual void init() {};
};

using SPOneWireDevice = std::shared_ptr<OneWireDevice>;
