#pragma once

#include "LPC407x_8x_177x_8x.h"
#include "system_LPC177x.h"

class CSPI{
public:    
  enum class ESPIInstance {
    SPI_PORTS,            // SPI-0 - порты ввода/вывода
    SPI_E_ADC,            // SPI-1 - внешнее АЦП
    SPI_ESP32             // SPI-2 - WiFi
  };
  
  CSPI(ESPIInstance);
  
private: 
  
  static constexpr unsigned int IOCON_SPI0    = 0x02;
  static constexpr unsigned int IOCON_SPI1    = 0x03;
  static constexpr unsigned int IOCON_SPI2    = 0x02;     
  static constexpr unsigned int D_MODE_PULLUP = 0x02 <<3;
  
  
  LPC_SSP_TypeDef* SPI_N;
  
};

