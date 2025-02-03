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

void App::sendLora()
{

  // Prepare LoRa buffer for sending
  std::vector<uint8_t> buffer;
  uint8_t mac[8] = {0};

  esp_efuse_mac_get_default(mac);
  for (auto val : mac)
  {
    buffer.push_back(val);
  }

  // auto battery = (int)(adc->readChannel(ADC_CHANNEL_1) * 3.76);
  // buffer.push_back(battery & 0xFF);
  // buffer.push_back(battery >> 8 & 0xFF);

  auto eder = EderBuffer::getInstance()->serializeBytes();
  buffer.insert(buffer.end(), eder.begin(), eder.end());

  lora->idle();
  lora->sendPacket(buffer);
  lora->sleep();
}

void App::init()
{
  auto &svm = ServiceManager::getInstance();
  lora = svm.getLora();
  uart = svm.getUart();
  // onewire = svm.getOneWire();
  adc = svm.getAdc();

  auto eder = EderBuffer::getInstance();
  std::vector<uint8_t> rcvBuffer = {};

  // onewire->searchDevices();

  // eder = "29.01.25;11:42:16;1.95;65;65;;30.7;;748;52;;;0.002;0.084;;;0;;0;0;0;0;0;;;;;;;;;;;00000000 00000000 00000000;;1;";
  // dataComplete = true;

  uart->setReceiveHandler([&](std::vector<uint8_t> data)
                          {
                            if (rcvBuffer.size() < 192) {
                              rcvBuffer.insert(rcvBuffer.end(), data.begin(), data.end());
                            }

                            if (rcvBuffer.size() == 192)
                            {
                              eder->updateFromBuffer(rcvBuffer);
                              eder->print();
                            } });
}

void App::run()
{
  auto lastSent = getTimeMS() - 30000;

  auto eder = EderBuffer::getInstance();
  while (1)
  {
    auto timeSinceSent = getTimeMS() - lastSent;

    if (eder->hasCriticalDataChanged() || (eder->hasNewData() && timeSinceSent > 30000))
    {
      sendLora();
      lastSent = getTimeMS();
    }

    if (timeSinceSent > 120000)
    {
      ESP_LOGW("EDER", "Timeout waiting for EDER data");
      vTaskDelay(pdMS_TO_TICKS(1000));
      (esp_deep_sleep(30000000));
    }

    ESP_LOGD(TAGAPP, "App loop ...");

    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

uint32_t App::getTimeMS()
{
  return xTaskGetTickCount() * portTICK_PERIOD_MS;
}
