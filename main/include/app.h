#pragma once

#include <stdio.h>
#include <memory>
#include <esp_sleep.h>
#include "loraservice.h"
#include "uartservice.h"
#include "onewireservice.h"
#include "adcservice.h"

static constexpr const char *TAGAPP = "APP";

class App
{
private:
  std::shared_ptr<UartService> uart;
  std::shared_ptr<LoraService> lora;
  std::shared_ptr<OneWireService> onewire;
  std::shared_ptr<AdcService> adc;

public:
  App() {};
  ~App() {};

  void init();
  void run();
};
