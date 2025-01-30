
#pragma once
#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <memory>

class EderBuffer
{
private:
  // Internal data representation
  struct DataStruct
  {
    bool flag1;
    bool flag2;
    uint16_t value1;
    uint16_t value2;
  };

  DataStruct data;
  DataStruct previousData;
  bool criticalDataChanged;
  bool hasData;

  // Singleton instance
  static EderBuffer *instance;

  // Private constructor to ensure singleton pattern
  EderBuffer() : criticalDataChanged(false), hasData(false)
  {
    std::memset(&data, 0, sizeof(data));                 // Initialize data to zero
    std::memset(&previousData, 0, sizeof(previousData)); // Initialize previousData to zero
  }

public:
  // Get the singleton instance
  static EderBuffer *getInstance()
  {
    if (instance == nullptr)
    {
      instance = new EderBuffer();
    }
    return instance;
  }

  // Convert buffer to struct
  void updateFromBuffer(std::vector<uint8_t> buffer)
  {
    // Check for changes and update

    // Store the current values in previousData for future comparison
    previousData = data;

    // Update the struct with the new values
    data.flag1 = (buffer[0] & 0x01);
    data.flag2 = (buffer[0] & 0x02) >> 1;
    data.value1 = (static_cast<uint16_t>(buffer[1]) << 8) | buffer[2];
    data.value2 = (static_cast<uint16_t>(buffer[3]) << 8) | buffer[4];

    hasData = true;
  }

  // Convert struct to buffer
  std::vector<uint8_t> toBytes()
  {
    std::vector<uint8_t> data;
    // buffer[0] = 0;
    // if (data.flag1)
    //   buffer[0] |= 0x01;
    // if (data.flag2)
    //   buffer[0] |= 0x02;

    // buffer[1] = static_cast<uint8_t>((data.value1 >> 8) & 0xFF); // MSB of value1
    // buffer[2] = static_cast<uint8_t>(data.value1 & 0xFF);        // LSB of value1

    // buffer[3] = static_cast<uint8_t>((data.value2 >> 8) & 0xFF); // MSB of value2
    // buffer[4] = static_cast<uint8_t>(data.value2 & 0xFF);        // LSB of value2

    hasData = false;
    return data;
  }

  // Check if a field has changed
  bool hasCriticalDataChanged() const { return criticalDataChanged; }
  bool hasNewData() const { return hasData; }
};
