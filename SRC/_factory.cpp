#include "factory.hpp"

CUART CFactory::createCOMport()         { return CUART(CUART::EUartInstance::UART_TERMINAL);     }
CUART CFactory::initRS485_01()          { return CUART(CUART::EUartInstance::UART_RS485_01);     }
CUART CFactory::initRS485_02()          { return CUART(CUART::EUartInstance::UART_RS485_02);     }
CCAN CFactory::initCAN1()               { return CCAN(CCAN::ECAN_Id_Instance::CAN1_Id);          }
CCAN CFactory::initCAN2()               { return CCAN(CCAN::ECAN_Id_Instance::CAN2_Id);          }
CDAC0 CFactory::createDAC0()            { return CDAC0();                                        }
CDAC_PWM CFactory::createPWMDac1()      { return CDAC_PWM(CDAC_PWM::EPWM_DACInstance::PWM_DAC1); }
CDAC_PWM CFactory::createPWMDac2()      { return CDAC_PWM(CDAC_PWM::EPWM_DACInstance::PWM_DAC2); }
CIADC CFactory::createIADC()            { return CIADC();                                        }
CDin_cpu CFactory::createDINcpu()       { return CDin_cpu();                                     }
CEMAC_DRV CFactory::createEMACdrv()     { return CEMAC_DRV();                                    }
CRTC CFactory::createRTC()              { return CRTC();                                         }
CDMAcontroller CFactory::createDMAc()   { return CDMAcontroller();                               }

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

CPULSCALC CFactory::createPULSCALC()
{
  CSET_SPI init_spi(CSET_SPI::ESPIInstance::SPI_E_ADC); // Конфигурация SPI-1 - внешнее АЦП
  static CADC adc(init_spi.getTypeDef());               // Внешнее ADC. Подключено к SPI-1
  return CPULSCALC(adc);                                 
}
void CFactory::start_puls_system(CDMAcontroller& rCont_dma)
{
  CSET_SPI init_spi1(CSET_SPI::ESPIInstance::SPI_E_ADC);                // Конфигурация SPI-1 - внешнее АЦП
  static CADC adc(init_spi1.getTypeDef());                              // Внешнее ADC. Подключено к SPI-1
  static CPULSCALC puls_calc(adc);                                      // Измерение и обработка всех аналоговых сигналов.
  static CSIFU sifu(puls_calc);                                         // СИФУ.
  CSET_SPI init_dma_spi(CSET_SPI::ESPIInstance::SPI_ESP32);             // Конфигурация SPI-2 - WiFi на ESP32
  static CREM_OSC rem_osc(rCont_dma, puls_calc);                        // Дистанционный осциллограф (WiFi модуль на ESP32).   
  CProxyHandlerTIMER::getInstance().set_pointers(&sifu, &rem_osc);      // Proxy Singleton доступа к Handler TIMER. 
  sifu.init_and_start();                                                // Старт SIFU
}

