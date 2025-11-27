#include "factory.hpp"

void CFactory::init_settings()
{
  static CCRC16 crc16;                           
  CEEPSettings::getInstance().init_EEP(&crc16); 
}
CUART CFactory::createTerminal()   { return CUART(CUART::EUartInstance::UART_TERMINAL);     }
CUART CFactory::createRS485_01()   { return CUART(CUART::EUartInstance::UART_RS485_01);     }
CUART CFactory::createRS485_02()   { return CUART(CUART::EUartInstance::UART_RS485_02);     }
CCAN CFactory::initCAN1()          { return CCAN(CCAN::ECAN_Id_Instance::CAN1_Id);          }
CCAN CFactory::initCAN2()          { return CCAN(CCAN::ECAN_Id_Instance::CAN2_Id);          }
CDAC0 CFactory::createDAC0()       { return CDAC0();                                        }
CDAC_PWM CFactory::createPWMDac1() { return CDAC_PWM(CDAC_PWM::EPWM_DACInstance::PWM_DAC1); }
CDAC_PWM CFactory::createPWMDac2() { return CDAC_PWM(CDAC_PWM::EPWM_DACInstance::PWM_DAC2); }
CSET_SPI CFactory::initSpiPorts()  { return CSET_SPI(CSET_SPI::ESPIInstance::SPI_PORTS);    }
CSET_SPI CFactory::initSpiADC()    { return CSET_SPI(CSET_SPI::ESPIInstance::SPI_E_ADC);    }
CSET_SPI CFactory::initSpiESP32()  { return CSET_SPI(CSET_SPI::ESPIInstance::SPI_ESP32);    }
CIADC CFactory::createIADC()       { return CIADC();                                        }
                 

CADC CFactory::createEADC(const CSET_SPI& spi) {
    return CADC(spi.getTypeDef());
}
