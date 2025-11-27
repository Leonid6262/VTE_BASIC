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
  CFactory::init_settings();                    // Создание Singleton класса CEEPSettings (уставки рабочие и по умолчанию).                         
  
  if(CEEPSettings::getInstance().loadSettings() != StatusRet::SUCCESS)   // Загрузка уставок (RAM <- EEPROM)   
  { /*  "Ошибка CRC. Используются уставки по умолчанию!" */ }  
   
  auto terminal = CFactory::createTerminal();           // COM Port (uart0 - Terminal)
  auto rs485_01 = CFactory::createRS485_01();           // RS485-1 (uart2)
  auto rs485_02 = CFactory::createRS485_02();           // RS485-2 (uart3)
  
  auto set_spi0 = CFactory::initSpiPorts();             // Конфигурация SPI-0 - порты ввода/вывода
  auto set_spi1 = CFactory::initSpiADC();               // Конфигурация SPI-1 - внешнее АЦП
  auto set_spi2 = CFactory::initSpiESP32();             // Конфигурация SPI-2 - WiFi на ESP32
 
  auto set_can1 = CFactory::initCAN1();                 // Конфигурация CAN-1  
  auto set_can2 = CFactory::initCAN2();                 // Конфигурация CAN-2 
  
  auto dac0    = CFactory::createDAC0();                // DAC-0 (Controller dac)
  auto dac_cos = CFactory::createPWMDac1();             // DAC-1 (PWM1:5, Cos_phi)
  auto dac4_20 = CFactory::createPWMDac1();             // DAC-2 (PWM1:4. 4...20mA)
                                
  auto i_adc = CFactory::createIADC();                  // Внутренее ADC.
  auto adc   = CFactory::createEADC(set_spi1);          // Внешнее ADC. Подключено к SPI-1
  
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
  static CDMAcontroller cont_dma;                                       // Управление каналами DMA.
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


