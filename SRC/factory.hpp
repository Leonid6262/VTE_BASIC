#pragma once

#include "crc16.hpp"
#include "settings_eep.hpp"
#include "set_uart.hpp"
#include "set_spi.hpp"
#include "spi_ports.hpp"
#include "can.hpp"
#include "dac.hpp"
#include "adc.hpp"
#include "i_adc.hpp"
#include "din_cpu.hpp"
#include "emac.hpp"
#include "rtc.hpp"
#include "controllerDMA.hpp"
#include "SIFU.hpp"
#include "puls_calc.hpp"
#include "handlers_IRQ.hpp"
#include "terminalUartDriver.hpp"
#include "terminal.hpp"
#include "AdcStorage.hpp"
#include "proxy_pointer_var.hpp"

class CFactory {
public:
    
    static bool load_settings();
  
    static CSET_UART initRS485_01();
    static CSET_UART initRS485_02();
    
    static CCAN initCAN1();
    static CCAN initCAN2();

    static CDAC0 createDAC0();
    static CDAC_PWM createPWMDac1();
    static CDAC_PWM createPWMDac2();
    
    static CIADC createIADC();
   
    static CSPI_ports createSPIports();    
    static CDin_cpu createDINcpu();
    
    static CEMAC_DRV createEMACdrv();
    
    static CDMAcontroller createDMAc();
   
    static CRTC createRTC();
    
    static CTERMINAL createTERMINAL();
    
    static void start_puls_system(CDMAcontroller&);
};

