#pragma once

#include "settings_eep.hpp"

class CSPI_ports{
  
public:  
  
  CSPI_ports();

  unsigned char data_din[G_CONST::BYTES_RW_MAX];    //Входные данные din портов 
  
  void rw();
  
private: 
    
  unsigned int prev_TC0;                            // Значение таймера на предыдыущем цикле
   
  static constexpr unsigned int IOCON_SPI = 0x02;
  static constexpr unsigned int Hz_SPI    = 800000;
  static constexpr unsigned int bits_tr   = 8;
  
  static constexpr unsigned int CR0_CPOL_HI     = 1UL << 6;
  static constexpr unsigned int CR0_CPHA_SECOND = 1UL << 7;
  static constexpr unsigned int TIC_ms = 10000;
  static constexpr unsigned int OUT_E = 1UL << 8;
  static constexpr unsigned int HOLD  = 1UL << 7;

};


