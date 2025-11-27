#include "main.hpp"

void UserStartInit()
{     
  SetDiscreteOutputs();      // Определение дискретных выходов микроконтроллера (pins)
  CDIN_STORAGE::UserLedOn(); // Включается для визуального контроля инициализации  
  EMC_Init_Check();          // Инициализации ext RAM и шины расширения. Контроль ext RAM
}

void main(void)
{                   
  NVIC_SetPriorityGrouping(Priorities::G4S8);   // Распределение векторов по группам. Реализовано в файле IntPriority.h
  Timers_Initializing();                        // Инициализация таймеров.
  static CCRC16 crc16;                          // CRC16  

  CEEPSettings::getInstance().init_EEP(&crc16); // Singleton класса CEEPSettings (уставки рабочие и по умолчанию).
                                                
  // Загрузка уставок (RAM <- EEPROM) 
  if(CEEPSettings::getInstance().loadSettings() != StatusRet::SUCCESS)          
  { /*  Сообщение: "Ошибка CRC. Используются уставки по умолчанию!" */ }  
   
  static CDMAcontroller cont_dma; // Управление каналами DMA.
  
  static CUART COM_Port(CUART::EUartInstance::UART_TERMINAL);       // COM Port (uart0 - Terminal)
  static CUART RS485_01(CUART::EUartInstance::UART_RS485_01);       // RS485-1 (uart2)
  static CUART RS485_02(CUART::EUartInstance::UART_RS485_02);       // RS485-2 (uart3)
  
  static CSET_SPI set_spi0(CSET_SPI::ESPIInstance::SPI_PORTS);      // Конфигурация SPI-0 - порты ввода/вывода 
  static CSET_SPI set_spi1(CSET_SPI::ESPIInstance::SPI_E_ADC);      // Конфигурация SPI-1 - внешнее АЦП
  static CSET_SPI set_spi2(CSET_SPI::ESPIInstance::SPI_ESP32);      // Конфигурация SPI-2 - WiFi 
   
  static CCAN can1(CCAN::ECAN_Id_Instance::CAN1_Id);                // CAN-1   
  static CCAN can2(CCAN::ECAN_Id_Instance::CAN2_Id);                // CAN-2   
  
  static CDAC0 dac0;                                                // DAC-0 (Controller dac)
  static CDAC_PWM pwm_dac1(CDAC_PWM::EPWM_DACInstance::PWM_DAC1);   // DAC-1 (PWM1:5, Cos_phi)
  static CDAC_PWM pwm_dac2(CDAC_PWM::EPWM_DACInstance::PWM_DAC2);   // DAC-2 (PWM1:4. 4...20mA)
  
  static CIADC i_adc;                                   // Внутренее ADC.
  static CADC adc(set_spi1.getTypeDef());               // Внешнее ADC.
  
  static CSPI_ports spi_ports(set_spi0.getTypeDef());   // Дискретные входы и выходы доступные по SPI.

  static CDin_cpu din_cpu;                              // Дискретные входы контроллера (порты Pi0 и Pi1 по аналогии с СМ3)
                                                        // Выходы контроллера (порт Po0 по аналогии с СМ3 в dIOStorage.hpp)
  
  static CSDCard sd_card;                               // Инициализация CD карты.  
  if(sd_card.init() != StatusRet::SUCCESS) 
  { /* Сообщение: "CD карта не определена!"*/ }
   
  static CEMAC_DRV emac_drv;                            // Драйверы EMAC.  
  if(emac_drv.initEMAC() != StatusRet::SUCCESS) 
  { /* Сообщение: "Нет готовности Ethernet!"*/ }
  
  static CRTC rt_clock;                                                 // Системные часы
  static CPULSCALC puls_calc(adc);                                      // Измерение/вычисление всех аналоговых данных.                                                                                                             
  static CSIFU sifu(puls_calc);                                         // СИФУ.                                                                                                             
  static CREM_OSC rem_osc(cont_dma, puls_calc);                         // Дистанционный осциллограф (WiFi модуль на ESP32).                                                    
  CProxyHandlerTIMER::getInstance().set_pointers(&sifu, &rem_osc);      // Proxy Singleton доступа к Handler TIMER.                                                                  
  
  sifu.init_and_start();                                                // Старт SIFU
   
  CDIN_STORAGE::UserLedOff();                                           // Визуальный контроль окончания инициализации
  
  static auto& settings = CEEPSettings::getInstance().getSettings();    // Тестовые отладочные указатели. 
  static auto& data_e_adc = CADC_STORAGE::getInstance().getExternal();  // В production не используются
  
  while(true)
  {        
    settings = CEEPSettings::getInstance().getSettings();
    
    i_adc.measure_5V();         // Измерение напряжения питания +/- 5V (внутреннее ADC)        
    din_cpu.input_Pi0();        // Чтение состояния дискретных входов контроллера Pi0   
    spi_ports.rw();             // Запись в дискретные выходы и чтение дискретных входов доступных по SPI    
    rt_clock.update_now();      // Обновление экземпляра структуы SDateTime данными из RTC    
    Pause_us(3);
  } 
}


