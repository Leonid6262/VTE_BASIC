#include "main.hpp"

void UserStartInit()
{     
  SetDiscreteOutputs();      // Определение дискретных выходов микроконтроллера (pins)
  CDIN_STORAGE::UserLedOn(); // Включается для визуального контроля инициализации  
  EMC_Init_Check();          // Инициализации ext RAM и шины расширения. Контроль ext RAM
}

void main(void)
{                   
  NVIC_SetPriorityGrouping(Priorities::G4S8);           // Распределение векторов по группам. Реализовано в файле IntPriority.h
  Timers_Initializing();                                // Инициализация таймеров.
  
  static auto terminal  = CFactory::createTERMINAL();   // Пультовый терминал. Подключен к UART-0 (см. CFactory)  
  if(CFactory::load_settings() == StatusRet::ERROR)     // Загрузка уставок (RAM <- EEPROM)   
  { 
    /*  "Ошибка CRC. Используются уставки по умолчанию!" */ 
  }                                  
  static auto i_adc     = CFactory::createIADC();       // Внутренее ADC.  
  static auto spi_ports = CFactory::createSPIports();   // Входы и выходы доступные по SPI. Подключено к SPI-0 (см. CFactory)                                                
  static auto din_cpu   = CFactory::createDINcpu();     // Дискретные входы контроллера (порты Pi0 и Pi1 по аналогии с СМ3)                                                        // Выходы контроллера (порт Po0 по аналогии с СМ3 в dIOStorage.hpp)
  static auto rt_clock  = CFactory::createRTC();        // Системные часы  
  static auto cont_dma  = CFactory::createDMAc();       // Управление каналами DMA. 
  
  CFactory::start_puls_system(cont_dma);                // Запуск СИФУ и всех её зависимостей. (см. CFactory)
  
  terminal.make_menu();
  
  CDIN_STORAGE::UserLedOff();                           // Визуальный контроль окончания инициализации

  while(true)
  {                
    i_adc.measure_5V();         // Измерение напряжения питания +/- 5V (внутреннее ADC)        
    din_cpu.input_Pi0();        // Чтение состояния дискретных входов контроллера Pi0   
    spi_ports.rw();             // Запись в дискретные выходы и чтение дискретных входов доступных по SPI    
    rt_clock.update_now();      // Обновление экземпляра структуы SDateTime данными из RTC    
    terminal.get_key();         // Пультовый терминал
    Pause_us(3);
  } 
}


