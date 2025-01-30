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
#include "ederbuffer.h"

// Initialize the static member
EderBuffer *EderBuffer::instance = nullptr;

void App::init()
{
  auto &svm = ServiceManager::getInstance();
  lora = svm.getLora();
  uart = svm.getUart();
  // onewire = svm.getOneWire();
  adc = svm.getAdc();

  auto eder = EderBuffer::getInstance();

  // onewire->searchDevices();

  // eder = "29.01.25;11:42:16;1.95;65;65;;30.7;;748;52;;;0.002;0.084;;;0;;0;0;0;0;0;;;;;;;;;;;00000000 00000000 00000000;;1;";
  // dataComplete = true;

  uart->setReceiveHandler([&](std::vector<uint8_t> data)
                          {
                            if (data.size() == 192)
                            {
                              eder->updateFromBuffer(data);
                              eder->print();
                            } });

  int timeout = 3000;
  while (!eder->hasNewData())
  {
    vTaskDelay(pdMS_TO_TICKS(10));

    timeout -= 10;
    if (timeout <= 0)
    {
      ESP_LOGW("EDER", "Timeout waiting for EDER data");
      vTaskDelay(pdMS_TO_TICKS(1000));
      (esp_deep_sleep(20000000));
    }
  }

  // Prepare LoRa buffer for sending
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

  auto ederData = eder->serializeBytes();
  buffer.insert(buffer.end(), ederData.begin(), ederData.end());

  // ESP_LOGI("EDER", "%s", buffer.data());
  // vTaskDelay(pdMS_TO_TICKS(5000));

  lora->sendPacket(buffer);

  esp_deep_sleep(30000000);
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
