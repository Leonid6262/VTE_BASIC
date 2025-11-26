#pragma once

#include "settings_eep.hpp"

class CDIN_STORAGE
{
public:
  
  //--- Входы контроллера, порт Pi0 ---
  inline bool Bl_Contact_Q1()   {return UData_din_f[static_cast<char>(EIBNumber::CPU_PORT)].b0;} // Блок контакт Q1
  inline bool Reg_Auto()        {return UData_din_f[static_cast<char>(EIBNumber::CPU_PORT)].b1;} // Режим "Автоматический"
  inline bool Reg_Drying()      {return UData_din_f[static_cast<char>(EIBNumber::CPU_PORT)].b2;} // Режим "Сушка"
  inline bool Setting_More()    {return UData_din_f[static_cast<char>(EIBNumber::CPU_PORT)].b3;} // Задание "Больше"
  inline bool Setting_Less()    {return UData_din_f[static_cast<char>(EIBNumber::CPU_PORT)].b4;} // Задание "Меньше"
  inline bool CU_from_testing() {return UData_din_f[static_cast<char>(EIBNumber::CPU_PORT)].b5;} // Сигнал "CU" от платы опробования
  inline bool Bl_HVS_NO()       {return UData_din_f[static_cast<char>(EIBNumber::CPU_PORT)].b6;} // NO блок контакт ВВ
  inline bool Bl_HVS_NC()       {return UData_din_f[static_cast<char>(EIBNumber::CPU_PORT)].b7;} // NC блок контакт ВВ
  
  //--- Входы контроллера byte-0 SPI ---
  inline bool DinControllerB0() {return UData_din_f[static_cast<char>(EIBNumber::CPU_SPI)].b0;}
  inline bool Stator_Key()      {return UData_din_f[static_cast<char>(EIBNumber::CPU_SPI)].b1;} // Статорный выкл.
  inline bool DinControllerB2() {return UData_din_f[static_cast<char>(EIBNumber::CPU_SPI)].b2;}
  inline bool DinControllerB3() {return UData_din_f[static_cast<char>(EIBNumber::CPU_SPI)].b3;}
  inline bool IN1()             {return UData_din_f[static_cast<char>(EIBNumber::CPU_SPI)].b4;} // Опция IN1
  inline bool IN2()             {return UData_din_f[static_cast<char>(EIBNumber::CPU_SPI)].b5;} // Опция IN2
  inline bool IN3()             {return UData_din_f[static_cast<char>(EIBNumber::CPU_SPI)].b6;} // Опция IN3
  inline bool DinControllerB7() {return UData_din_f[static_cast<char>(EIBNumber::CPU_SPI)].b7;}
  
  //--- Входы S600 byte-0 ---
  //
  //--- Входы S600 byte-1 ---
  //
  
  //--- Выходы контроллера SPI ---
  inline void Lamp_REDY         (bool state) {UData_dout[static_cast<char>(EOBNumber::CPU_SPI)].b0 = state;}
  inline void System_FAILURE    (bool state) {UData_dout[static_cast<char>(EOBNumber::CPU_SPI)].b1 = state;}
  inline void Excitation_Applied(bool state) {UData_dout[static_cast<char>(EOBNumber::CPU_SPI)].b2 = state;}
  inline void START_Premission  (bool state) {UData_dout[static_cast<char>(EOBNumber::CPU_SPI)].b3 = state;}
  inline void ON_Shunt_HVS      (bool state) {UData_dout[static_cast<char>(EOBNumber::CPU_SPI)].b4 = state;}
  inline void DoutControllerB5  (bool state) {UData_dout[static_cast<char>(EOBNumber::CPU_SPI)].b5 = state;}
  inline void DoutControllerB6  (bool state) {UData_dout[static_cast<char>(EOBNumber::CPU_SPI)].b6 = state;}
  inline void DoutControllerB7  (bool state) {UData_dout[static_cast<char>(EOBNumber::CPU_SPI)].b7 = state;}
  
  //--- Выходы S600 ---
  //
  
  // Номера считываемых байтов
  enum class EIBNumber
  {
    CPU_PORT    = 0, // Контроллер Pi0
    CPU_SPI     = 1, // Контроллер SPI    
    BYTE_0_EX1  = 2, // S600 byte-0 SPI    
    BYTE_1_EX1  = 3  // S600 byte-1 SPI    
  };
  
  // Номера записываемых байтов последовательности SPI
  enum class EOBNumber
  {
    BYTE_1_EX1 = 0,    // S600 byte-0 (в S600 2/1 - отсутствует)
    BYTE_0_EX1 = 1,    // S600 byte-1
    CPU_SPI    = 2     // Контроллер (всегда (BYTES_RW_MAX-1)), так как, замыкает последовательность)
  };
  
   /*                    Последовательность цикла:
                        
        MISO: <- byte_din_cpu  <- byte_0_din_ex1  <- byte_1_din_ex1
                      0                 1                  2
        MOSI: -> byte_dout_cpu -> byte_0_dout_ex1 -> byte_1_dout_ex1(*)
                      2                 1                  0

        (*) При использовании S600 2/1, byte_1_dout_ex1 - выходит в эфир     
  */
  
  union
  {
    unsigned char  all;         //Выходные данные порта Pi0 после фильтра и инверсии
    struct
    {  
      unsigned char   b0 : 1;
      unsigned char   b1 : 1;
      unsigned char   b2 : 1;
      unsigned char   b3 : 1;
      unsigned char   b4 : 1;
      unsigned char   b5 : 1;
      unsigned char   b6 : 1;
      unsigned char   b7 : 1;
    };
  } UData_din_f[G_CONST::BYTES_RW_MAX + 1];               //Данные din портов после фильтра
  
  void filter(unsigned char, unsigned int, unsigned char);
  
  union
  {
    unsigned char  all;               
    struct
    {  
      unsigned char   b0 : 1;
      unsigned char   b1 : 1;
      unsigned char   b2 : 1;
      unsigned char   b3 : 1;
      unsigned char   b4 : 1;
      unsigned char   b5 : 1;
      unsigned char   b6 : 1;
      unsigned char   b7 : 1;
    };
  } UData_dout[G_CONST::BYTES_RW_MAX];              //Данные для записи в д.выходы
  
  static CDIN_STORAGE& getInstance() 
  {
    static CDIN_STORAGE instance;
    return instance;
  }
  
private:
  
  static constexpr unsigned short N_BITS = 8;          // Количество бит в портах
  static constexpr unsigned int TIC_ms = 10000;
  
  signed int integrator[G_CONST::BYTES_RW_MAX + 1][N_BITS];                       //Интеграторы
  
  // Постоянные времени интегрирования фильтра (ms первый множитель) din портов. 
  // TIC_ms = 10000 дискрет таймера на 1ms.  
  // То есть, 50*TIC_ms = 50ms, 0*TIC_ms - нет фильтрации, и т.п. 
  static constexpr unsigned int cConst_integr_Pi0[G_CONST::BYTES_RW_MAX + 1][N_BITS] = 
  {
    {50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms},   // Контроллер Pi0
    {50*TIC_ms, 50*TIC_ms, 2000*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms}, // Контроллер SPI
    {50*TIC_ms, 50*TIC_ms, 2000*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms}, // S600 byte-0 SPI
    {50*TIC_ms, 50*TIC_ms, 2000*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms, 50*TIC_ms}, // S600 byte-1 SPI
  }; 
  
  CEEPSettings& settings;
  
  // --- Механизмы Singleton ---
  CDIN_STORAGE(); 
  CDIN_STORAGE(const CDIN_STORAGE&) = delete;
  CDIN_STORAGE& operator=(const CDIN_STORAGE&) = delete; 
  
};