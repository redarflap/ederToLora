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

  bool dataStarted = false;
  bool dataComplete = false;
  std::string ederData;
  uart->setReceiveHandler([&](std::string data)
                          {
                            if (data.starts_with("\r\n"))
                            {
                              if (!dataStarted)
                              {
                                dataStarted = true;
                                ederData.clear();
                                ESP_LOGI("EDER", "Data started");
                                return;
                              }

                              ESP_LOGI("EDER", "Data complete");
                              dataStarted = false;
                              dataComplete = true;
                              return;
                            }

                            if (dataStarted)
                            {
                              data.erase(std::remove(data.begin(), data.end(), '\r'), data.end());
                              data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
                              ederData.append(data);
                            }

                            // lora->sendPacket((uint8_t *)data.data());
                          });

  while (!dataComplete)
  {
    vTaskDelay(pdMS_TO_TICKS(5));
  }

  ESP_LOGI("EDER", "%s", ederData.data());

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

  for (auto byte : ederData)
  {
    buffer.push_back(byte);
  }

  lora->sendPacket(buffer);

  esp_deep_sleep(20000000);
}

void App::run()
{
  // while (1)
  // {
  //   ESP_LOGD(TAGAPP, "App loop ...");

  //   // uart->sendString(std::string("Test"));
  //   vTaskDelay(pdMS_TO_TICKS(30000));
  // }
}
