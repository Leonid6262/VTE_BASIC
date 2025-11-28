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
   
  static auto COMport      = CFactory::createCOMport(); // COM Port (uart0 - Terminal)
  static auto initRS485_01 = CFactory::initRS485_01();  // Конфигурация RS485-1 (uart2)
  static auto initRS485_02 = CFactory::initRS485_02();  // Конфигурация RS485-2 (uart3)

  static auto init_can1 = CFactory::initCAN1();         // Конфигурация CAN-1  
  static auto init_can2 = CFactory::initCAN2();         // Конфигурация CAN-2 
  
  static auto dac0    = CFactory::createDAC0();         // DAC-0 (Controller dac)
  static auto dac_cos = CFactory::createPWMDac1();      // DAC-1 (PWM1:5, Cos_phi)
  static auto dac4_20 = CFactory::createPWMDac2();      // DAC-2 (PWM1:4. 4...20mA)
                                
  static auto i_adc = CFactory::createIADC();           // Внутренее ADC.
  
  static auto spi_ports = CFactory::createSPIports();   // Входы и выходы доступные по SPI. Подключено к SPI-0 (см. CFactory)                                                
  static auto din_cpu   = CFactory::createDINcpu();     // Дискретные входы контроллера (порты Pi0 и Pi1 по аналогии с СМ3)
                                                        // Выходы контроллера (порт Po0 по аналогии с СМ3 в dIOStorage.hpp)
  static auto rt_clock = CFactory::createRTC();         // Системные часы
  
  static auto emac_drv = CFactory::createEMACdrv();     // Драйверы EMAC.  
  if(emac_drv.initEMAC() != StatusRet::SUCCESS) 
  { /* Сообщение: "Нет готовности Ethernet!"*/ }
   
  static auto cont_dma = CFactory::createDMAc();        // Управление каналами DMA. 

  CFactory::start_puls_system(cont_dma);                // Запуск СИФУ и всех её зависимостей
    
  CDIN_STORAGE::UserLedOff();                           // Визуальный контроль окончания инициализации
  
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


