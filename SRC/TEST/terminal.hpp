#pragma once

#include "uart.hpp"
#include "din_cpu.hpp"
#include "dout_cpu.hpp"
#include "spi_ports.hpp"
#include "pause_us.hpp"
#include "settings_eep.hpp"
#include "SIFU.hpp"
#include "Compare.hpp"
#include "Compare.hpp"
#include "i_adc.hpp"
#include "adc.hpp"
#include "Tests.hpp"
#include "SDCard_init.hpp"
#include "test_eth.hpp"
#include "rtc.hpp"
#include <stdio.h>

class CTerminal{

public:  
  
  // Структура зависимостей класса 
 struct SDependencies {
    CUART&      rComPort;
    CDin_cpu&   rDin_cpu;
    CSPI_ports& rSpi_ports;
    CTESTS&     rTs;
    CDout_cpu&  rDout_cpu;
    CIADC&      rI_adc;
    CSDCard&    rSD_card;
    CTEST_ETH&  rTest_eth;
    CRTC&       rRt_clock;
    CSIFU&      rPuls;
    CCOMPARE&   rCompare;
  };
  
  CTerminal(const SDependencies&); 
  
  void terminal();  
  
private: 
  
  enum class ELED
  {
    LED_RED    = 0x01,
    LED_GREEN  = 0x02,
    LED_BLUE   = 0x03,
    LED_YELLOW = 0x04,
    LED_WHITE  = 0x09,
    LED_OFF    = 0x0B 
  };
  
  static const char* header_str[];
  
  signed short index_win;
  
  char receive_char;   
  char formVar[15];
  bool edit;
  bool fup, fdn;
  
  unsigned int prev_TC0;
  unsigned short ind_max;

  void char_to_bits(char*, char);
  
  const SDependencies& deps;
  
};
