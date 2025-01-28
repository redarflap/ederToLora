#include <stdio.h>
#include "app.h"
#include "servicemanager.h"
#include <esp_sleep.h>
#include <esp_log.h>

extern "C" void app_main(void)
{
  ESP_LOGI("MAIN", "App starting");
  vTaskDelay(pdMS_TO_TICKS(500));
  auto &svm = ServiceManager::getInstance();
  svm.setupServices(esp_sleep_get_wakeup_cause());
  svm.initServices(esp_sleep_get_wakeup_cause());

  App myapp = App();
  myapp.init();
  myapp.run();
}
