#include "app.h"
#include <memory>
#include "esp_sleep.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "config.h"
#include "servicemanager.h"
#include <string.h>
#include <esp_adc_cal_types_legacy.h>
#include <esp_adc_cal.h>
#include <driver/adc.h>

void App::init()
{
  auto &svm = ServiceManager::getInstance();
  lora = svm.getLora();
  uart = svm.getUart();
  onewire = svm.getOneWire();
  adc = svm.getAdc();

  onewire->searchDevices();
  uart->setReceiveHandler([&](std::string data)
                          {
                            data.erase(std::remove(data.begin(), data.end(), '\r'), data.end());
                            data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
                            lora->sendPacket((uint8_t *)data.data(), data.length()); });
}

void App::run()
{
  while (1)
  {
    ESP_LOGD(TAGAPP, "App loop ...");

    auto channels = adc->readAll();
    for (auto channel : channels)
    {
      ESP_LOGI(TAGAPP, "ADC channel %d measured %dmV", channel.first, channel.second);
    }

    for (auto sensor : onewire->getDS18B20s())
    {
      auto result = sensor->readTemperature();
      ESP_LOGI(TAGAPP, "DS18B20 [%016llX] Temperature %.2f", result.address, result.temp);
    }

    uart->sendString(std::string("Test"));
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}
