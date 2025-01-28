#include <string.h>
#include "esp_log.h"
#include <esp_err.h>
#include "adcservice.h"
#include <unordered_set>
#include "freertos/FreeRTOS.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include <esp_adc/adc_cali_scheme.h>
#include <map>

AdcService::~AdcService()
{
  for (auto cali : calibrations)
  {
    ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(cali.second));
  }
  ESP_ERROR_CHECK(adc_oneshot_del_unit(adcUnit));
}

void AdcService::init()
{
  ESP_LOGI(TAGADC, "Initializing service ...");

  adc_oneshot_unit_init_cfg_t init_config1 = {
      .unit_id = adcUnitId,
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adcUnit));

  for (auto channel : channelConfigs)
  {
    ESP_LOGD(TAGADC, "Set channel %d to bitwidth: %d attenuation: %d", channel.id, bitWidth, channel.attenuation);
    adc_oneshot_chan_cfg_t config = {
        .atten = channel.attenuation,
        .bitwidth = bitWidth,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adcUnit, channel.id, &config));
  }

  calibrate();
}

void AdcService::calibrate()
{
  std::unordered_set<adc_atten_t> attenuations;
  for (auto channel : channelConfigs)
  {
    attenuations.insert(channel.attenuation);
  }

  for (auto attenuation : attenuations)
  {
    adc_cali_handle_t calHandle = NULL;
    adc_cali_curve_fitting_config_t cali_cfg = {
        .unit_id = adcUnitId,
        .atten = attenuation,
        .bitwidth = bitWidth,
    };
    bool cali_enabled = adc_cali_create_scheme_curve_fitting(&cali_cfg, &calHandle) == ESP_OK;
    if (cali_enabled)
    {
      ESP_LOGD(TAGADC, "ADC calibration initialized for attenuation %d", attenuation);

      calibrations.insert(std::pair(attenuation, calHandle));
    }
    else
    {
      ESP_LOGW(TAGADC, "ADC calibration for attenuation %d unavailable. Using raw readings.", attenuation);
    }
  }
}

int AdcService::readChannel(adc_channel_t channel)
{

  // Step 3: Perform ADC reading
  int raw_value = 0;
  ESP_ERROR_CHECK(adc_oneshot_read(adcUnit, channel, &raw_value));
  ESP_LOGD(TAGADC, "Channel %d raw ADC Value: %d", channel, raw_value);

  adc_atten_t channelAttenuation = {};
  for (auto channelConfig : channelConfigs)
  {
    if (channelConfig.id == channel)
    {
      channelAttenuation = channelConfig.attenuation;
    }
  }

  int voltage = 0;
  // Step 4: If calibration is available, convert raw reading to voltage
  if (calibrations.contains(channelAttenuation))
  // if (false)
  {
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(calibrations[channelAttenuation], raw_value, &voltage));
    ESP_LOGD(TAGADC, "Calibrated Voltage: %d mV", voltage);
  }

  return voltage;
}

std::map<adc_channel_t, int> AdcService::readAll()
{

  std::map<adc_channel_t, int>
      results = {};
  for (auto channel : channelConfigs)
  {
    // vTaskDelay(pdMS_TO_TICKS(50));
    results[channel.id] = readChannel(channel.id);
  }
  return results;
}
