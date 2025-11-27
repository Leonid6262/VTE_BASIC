#pragma once

#include "LPC407x_8x_177x_8x.h"
#include "system_LPC177x.h"
#include "spi_init.hpp"

class CSET_SPI{
public:    
  enum class ESPIInstance {
    SPI_PORTS,            // SPI-0 - порты ввода/вывода
    SPI_E_ADC,            // SPI-1 - внешнее АЦП
    SPI_ESP32             // SPI-2 - WiFi
  };
  
  CSET_SPI(ESPIInstance);
  
  LPC_SSP_TypeDef* getTypeDef() const;
  
private:
  
  static constexpr unsigned int Hz_SPI0    = 800000;
  static constexpr unsigned int Hz_SPI1    = 10000000;
  static constexpr unsigned int Hz_SPI2    = 1000000;  
  
  static constexpr unsigned int IOCON_SPI0    = 0x02;
  static constexpr unsigned int IOCON_SPI1    = 0x03;
  static constexpr unsigned int IOCON_SPI2    = 0x02;     
  static constexpr unsigned int D_MODE_PULLUP = 0x02 <<3;
  
  static constexpr unsigned int RXDMAE     = 1UL << 0;
  static constexpr unsigned int TXDMAE     = 1UL << 1;
   
  LPC_SSP_TypeDef* SSP;
  
};

