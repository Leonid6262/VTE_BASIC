#pragma once

#include "crc16.hpp"
#include "settings_eep.hpp"
#include "uart.hpp"
#include "set_spi.hpp"
#include "can.hpp"
#include "dac.hpp"
#include "adc.hpp"
#include "i_adc.hpp"

class CFactory {
public:
    
    static void init_settings();
  
    static CUART createTerminal();
    static CUART createRS485_01();
    static CUART createRS485_02();

    static CSET_SPI initSpiPorts();
    static CSET_SPI initSpiADC();
    static CSET_SPI initSpiESP32();

    static CCAN initCAN1();
    static CCAN initCAN2();

    static CDAC0 createDAC0();
    static CDAC_PWM createPWMDac1();
    static CDAC_PWM createPWMDac2();
    
    static CIADC createIADC();
    static CADC createEADC(const CSET_SPI& spi);
    
    
};

