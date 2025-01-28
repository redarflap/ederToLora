#pragma once
#include <string>
#include <memory>
#include <functional>
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include <map>

static constexpr const char *TAGADC = "ADC";

typedef struct
{
  adc_channel_t id;
  adc_atten_t attenuation;
  uint32_t fallbackVref;
} AdcChannelConfig;

class AdcService
{
private:
  adc_unit_t adcUnitId;
  adc_oneshot_unit_handle_t adcUnit;
  adc_bitwidth_t bitWidth;
  std::vector<AdcChannelConfig> channelConfigs;
  std::map<adc_atten_t, adc_cali_handle_t> calibrations;

public:
  AdcService(adc_unit_t adcUnitId, adc_bitwidth_t bitWidth, std::vector<AdcChannelConfig> channels) : adcUnitId(adcUnitId), bitWidth(bitWidth), channelConfigs(channels)
  {
  }
  ~AdcService();

  void init();
  void calibrate();
  int readChannel(adc_channel_t channel);
  std::map<adc_channel_t, int> readAll();
};
