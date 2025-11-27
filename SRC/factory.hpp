#pragma once

#include "crc16.hpp"
#include "settings_eep.hpp"
#include "uart.hpp"
#include "set_spi.hpp"
#include "spi_ports.hpp"
#include "can.hpp"
#include "dac.hpp"
#include "adc.hpp"
#include "i_adc.hpp"
#include "din_cpu.hpp"
#include "emac.hpp"
#include "rtc.hpp"

class CFactory {
public:
    
    static void init_settings();
  
    static CUART createCOMport();
    static CUART initRS485_01();
    static CUART initRS485_02();
    
    static void init_spi2();

    static CCAN initCAN1();
    static CCAN initCAN2();

    static CDAC0 createDAC0();
    static CDAC_PWM createPWMDac1();
    static CDAC_PWM createPWMDac2();
    
    static CIADC createIADC();
    static CADC createEADC();
    
    static CSPI_ports createSPIports();    
    static CDin_cpu createDINcpu();
    
    static CEMAC_DRV createEMACdrv();
    
    static CRTC createRTC();
};

