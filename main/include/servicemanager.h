#pragma once

#include <memory>
#include "uartservice.h"
#include "loraservice.h"
#include "onewireservice.h"
#include "adcservice.h"

static constexpr const char *TAGSVM = "SVM";

class ServiceManager
{
public:
  void setupServices(esp_sleep_wakeup_cause_t wakeupCause)
  {

    uart = std::make_shared<UartService>(UartServiceConfig{
        .uartPort = UART_NUM_2,
        .gpioTx = GPIO_NUM_18,
        .gpioRx = GPIO_NUM_17,
        .uartConfig = uart_config_t{
            .baud_rate = 9600,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .rx_flow_ctrl_thresh = 122,
        }}

    );
    lora = std::make_shared<LoraService>(
        LoraServiceConfig{
            .spiHost = SPI2_HOST,
            .gpioMISO = GPIO_NUM_37,
            .gpioMOSI = GPIO_NUM_35,
            .gpioSCL = GPIO_NUM_36,
            .gpioCS = GPIO_NUM_5,
            .gpioRST = GPIO_NUM_6,
            .loraFrequency = static_cast<long>(868e6),
            .crc = true,
            .codingRate = 5,
            .bandwith = 7,
            .spreadingFactor = 7,
        });

    // std::vector<gpio_num_t> onewireGpio = {GPIO_NUM_1, GPIO_NUM_33, GPIO_NUM_38};
    // onewire = std::make_shared<OneWireService>(onewireGpio);

    std::vector<AdcChannelConfig> channels = {
        AdcChannelConfig{
            .id = ADC_CHANNEL_1,
            .attenuation = ADC_ATTEN_DB_12,
            .fallbackVref = 1100,
        },
    };
    adc = std::make_shared<AdcService>(ADC_UNIT_1, ADC_BITWIDTH_12, channels);
  }

  void initServices(esp_sleep_wakeup_cause_t wakeupCause)
  {
    if (uart.get())
      uart->init();
    if (lora.get())
      lora->init();
    // if (onewire.get())
    //   onewire->init();
    if (adc.get())
      adc->init();
  }

  static ServiceManager &getInstance()
  {
    static ServiceManager instance;
    return instance;
  }

  std::shared_ptr<UartService> getUart() { return uart; }
  std::shared_ptr<LoraService> getLora() { return lora; }
  std::shared_ptr<OneWireService> getOneWire() { return onewire; }
  std::shared_ptr<AdcService> getAdc() { return adc; }

private:
  ServiceManager() = default; // Private constructor
  ~ServiceManager() = default;

  ServiceManager(const ServiceManager &) = delete;
  ServiceManager &operator=(const ServiceManager &) = delete;

  std::shared_ptr<UartService> uart;
  std::shared_ptr<LoraService> lora;
  std::shared_ptr<OneWireService> onewire;
  std::shared_ptr<AdcService> adc;
};
