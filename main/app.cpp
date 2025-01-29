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
  // onewire = svm.getOneWire();
  adc = svm.getAdc();

  // onewire->searchDevices();
  uart->setReceiveHandler([&](std::string data)
                          {
                            data.erase(std::remove(data.begin(), data.end(), '\r'), data.end());
                            data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
                            // lora->sendPacket((uint8_t *)data.data());
                          });

  std::vector<uint8_t> buffer;
  uint8_t mac[8] = {0};

  esp_efuse_mac_get_default(mac);
  for (auto val : mac)
  {
    buffer.push_back(val);
  }

  auto battery = (int)(adc->readChannel(ADC_CHANNEL_1) * 3.76);
  buffer.push_back(battery & 0xFF);
  buffer.push_back(battery >> 8 & 0xFF);

  // lora->sendPacket(buffer);

  // esp_deep_sleep(10000000);
}

void App::run()
{
  while (1)
  {
    ESP_LOGD(TAGAPP, "App loop ...");

    // uart->sendString(std::string("Test"));
    vTaskDelay(pdMS_TO_TICKS(30000));
  }
}
