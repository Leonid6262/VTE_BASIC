#include "factory.hpp"

CUART CFactory::createCOMport()    { return CUART(CUART::EUartInstance::UART_TERMINAL);     }
CUART CFactory::initRS485_01()     { return CUART(CUART::EUartInstance::UART_RS485_01);     }
CUART CFactory::initRS485_02()     { return CUART(CUART::EUartInstance::UART_RS485_02);     }
CCAN CFactory::initCAN1()          { return CCAN(CCAN::ECAN_Id_Instance::CAN1_Id);          }
CCAN CFactory::initCAN2()          { return CCAN(CCAN::ECAN_Id_Instance::CAN2_Id);          }
CDAC0 CFactory::createDAC0()       { return CDAC0();                                        }
CDAC_PWM CFactory::createPWMDac1() { return CDAC_PWM(CDAC_PWM::EPWM_DACInstance::PWM_DAC1); }
CDAC_PWM CFactory::createPWMDac2() { return CDAC_PWM(CDAC_PWM::EPWM_DACInstance::PWM_DAC2); }
CIADC CFactory::createIADC()       { return CIADC();                                        }
CDin_cpu CFactory::createDINcpu()  { return CDin_cpu();                                     }
CEMAC_DRV CFactory::createEMACdrv(){ return CEMAC_DRV();                                    }
CRTC CFactory::createRTC()         { return CRTC();                                         }

void CFactory::init_spi2()         { CSET_SPI init_spi(CSET_SPI::ESPIInstance::SPI_ESP32);  }

void CFactory::init_settings()
{
  static CCRC16 crc16;                           
  CEEPSettings::getInstance().init_EEP(&crc16);
}
CSPI_ports CFactory::createSPIports()
{
  CSET_SPI init_spi(CSET_SPI::ESPIInstance::SPI_PORTS); // Конфигурация SPI-0 - порты ввода/вывода
  return CSPI_ports(init_spi.getTypeDef());
}
CADC CFactory::createEADC() 
{
  CSET_SPI init_spi(CSET_SPI::ESPIInstance::SPI_E_ADC); // Конфигурация SPI-1 - внешнее АЦП
  return CADC(init_spi.getTypeDef());
}


