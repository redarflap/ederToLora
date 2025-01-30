
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
    bool warningActive;        // [0/0]
    bool errorActive;          // [0/1]
    bool refillActive;         // [0/2]
    bool pump1Active;          // [0/3]
    bool rapidDegassingActive; // [0/5]

    int16_t level;             // [4 5] 0/0/ 0-100  %
    int16_t pressure;          // [6 7] 0-4000 bar*100
    int16_t targetPressureMin; // [8 9] 0-4000 bar*100
    int16_t targetPressureMax; // [10 11] 0-4000 bar*100
    int16_t temperature;       // [12 13] -500-1200 °C*10

    int16_t rpmPump1;        // [16 17] 0 - 10000 %*100
    int16_t pressureBottom1; // [28 29] -300 - 1300 mBar
    int16_t pressureTop1;    // [30 31] -300 - 1300 mBar

    int16_t tempMulticontrolPcb; // [36 37] °C * 100

    uint32_t pump1Seconds;      // [46..49]
    uint8_t refillMode;         // [63] 0 - limit
                                //      1 - uncontroller
                                //      2 - schedule
    uint8_t degassingStatus;    // [64] 0 - locked
                                //      1 - pressurizing
                                //      2 - degassing
                                //      3 - pause
                                //      4 - valve open
                                //      5 - overpressure
    int16_t refillMinThreshold; // [66 67] 0-100 %
    int16_t refillMaxThreshold; // [68 69] 0-100 %
    uint32_t refillTotal;       // [70..73] l
    int16_t refillRest;         // [74 75] l

    uint32_t degassingSeconds; // [88..91] s
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

  bool getFlag(std::vector<uint8_t> buffer, int byte, int bit)
  {
    return (buffer[byte] & bit) != 0;
  }

  int16_t getInt16(std::vector<uint8_t> buffer, int startByte)
  {
    return (static_cast<int16_t>(buffer[startByte]) << 8) | buffer[startByte + 1];
  }

  uint32_t getUint32(std::vector<uint8_t> buffer, int startByte)
  {
    return (static_cast<uint32_t>(buffer[startByte]) << 24) |
           (static_cast<uint32_t>(buffer[startByte + 1]) << 16) |
           (static_cast<uint32_t>(buffer[startByte + 2]) << 8) |
           buffer[startByte + 3];
  }

  void appendInt16(std::vector<uint8_t> &buffer, int16_t value)
  {
    size_t offset = buffer.size();
    buffer.resize(offset + sizeof(value));

    std::memcpy(buffer.data() + offset, &value, sizeof(value));
  }

  void appendUint32(std::vector<uint8_t> &buffer, uint32_t value)
  {
    size_t offset = buffer.size();
    buffer.resize(offset + sizeof(value));

    std::memcpy(buffer.data() + offset, &value, sizeof(value));
  }

  // Convert buffer to struct
  void updateFromBuffer(std::vector<uint8_t> buffer)
  {
    // Check for changes and update

    // Store the current values in previousData for future comparison
    previousData = data;

    // Update the struct with the new values
    data.warningActive = getFlag(buffer, 0, 0);
    data.errorActive = getFlag(buffer, 0, 1);
    data.refillActive = getFlag(buffer, 0, 2);
    data.pump1Active = getFlag(buffer, 0, 3);
    data.rapidDegassingActive = getFlag(buffer, 0, 5);

    data.level = getInt16(buffer, 4);
    data.pressure = getInt16(buffer, 6);
    data.targetPressureMin = getInt16(buffer, 8);
    data.targetPressureMax = getInt16(buffer, 10);
    data.temperature = getInt16(buffer, 12);
    data.rpmPump1 = getInt16(buffer, 16);
    data.pressureBottom1 = getInt16(buffer, 28);
    data.pressureTop1 = getInt16(buffer, 30);
    data.tempMulticontrolPcb = getInt16(buffer, 36);

    data.pump1Seconds = getUint32(buffer, 46);

    data.refillMode = buffer[63];
    data.degassingStatus = buffer[64];

    data.refillMinThreshold = getInt16(buffer, 66);
    data.refillMaxThreshold = getInt16(buffer, 68);
    data.refillTotal = getUint32(buffer, 70);
    data.refillRest = getInt16(buffer, 74);
    data.degassingSeconds = getUint32(buffer, 88);

    hasData = true;
    criticalDataChanged = previousData.refillActive != data.refillActive || previousData.pump1Active != data.pump1Active;
  }

  // Convert struct to buffer
  std::vector<uint8_t> serializeBytes()
  {
    std::vector<uint8_t> buffer;

    buffer.push_back(data.warningActive | data.errorActive << 1 | data.refillActive << 2 | data.pump1Active << 3 | data.rapidDegassingActive << 5);

    buffer.push_back(data.refillMode);
    buffer.push_back(data.degassingStatus);

    appendInt16(buffer, data.level);
    appendInt16(buffer, data.pressure);
    appendInt16(buffer, data.targetPressureMin);
    appendInt16(buffer, data.targetPressureMax);
    appendInt16(buffer, data.temperature);
    appendInt16(buffer, data.rpmPump1);
    appendInt16(buffer, data.pressureBottom1);
    appendInt16(buffer, data.pressureTop1);
    appendInt16(buffer, data.tempMulticontrolPcb);
    appendInt16(buffer, data.refillMinThreshold);
    appendInt16(buffer, data.refillMaxThreshold);
    appendInt16(buffer, data.refillRest);

    appendUint32(buffer, data.pump1Seconds);
    appendUint32(buffer, data.refillTotal);
    appendUint32(buffer, data.degassingSeconds);

    hasData = false;
    criticalDataChanged = false;
    return buffer;
  }

  // Check if a field has changed
  bool hasCriticalDataChanged() const { return criticalDataChanged; }
  bool hasNewData() const { return hasData; }

  void print()
  {
    ESP_LOGI("EDER", "Warning: %d", data.warningActive);
    ESP_LOGI("EDER", "Error: %d", data.errorActive);
    ESP_LOGI("EDER", "Refilling: %d", data.refillActive);
    ESP_LOGI("EDER", "Pump active: %d", data.pump1Active);
    ESP_LOGI("EDER", "Rapid degassing: %d", data.rapidDegassingActive);
    ESP_LOGI("EDER", "Level: %d%%", data.level);
    ESP_LOGI("EDER", "Pressure: %f bar", data.pressure / 100.);
    ESP_LOGI("EDER", "Target pressure: %f - %f bar", data.targetPressureMin / 100., data.targetPressureMax / 100.);
    ESP_LOGI("EDER", "Temperature: %f°C", data.temperature / 10.);
    ESP_LOGI("EDER", "Level: %d%%", data.rpmPump1 / 100);
    ESP_LOGI("EDER", "Pressure bottom: %dmBar", data.pressureBottom1);
    ESP_LOGI("EDER", "Pressure top: %dmBar", data.pressureTop1);
    ESP_LOGI("EDER", "Temperature PCB: %f°C", data.tempMulticontrolPcb / 100.);
    ESP_LOGI("EDER", "Pump1 runtime: %lus", data.pump1Seconds);
    ESP_LOGI("EDER", "Refill mode: %d", data.refillMode);
    ESP_LOGI("EDER", "Degassing status: %d", data.degassingStatus);
    ESP_LOGI("EDER", "Refill thresholds: %d .. %d%%", data.refillMinThreshold, data.refillMaxThreshold);
    ESP_LOGI("EDER", "Refill total: %lul", data.refillTotal);
    ESP_LOGI("EDER", "Refill rest: %dl", data.refillRest);
    ESP_LOGI("EDER", "Degassing runtime: %lus", data.degassingSeconds);
  }
};
