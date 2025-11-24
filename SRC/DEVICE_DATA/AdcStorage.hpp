#pragma once

#include "settings_eep.hpp"

class CADC_STORAGE
{
public:
  
  // Список имён каналов внешнего АЦП
  enum EADC_NameCh{
    ROTOR_CURRENT       = 0,
    STATOR_VOLTAGE      = 1,
    ROTOR_VOLTAGE       = 2,
    LEAKAGE_CURRENT     = 3,    
    STATOR_CURRENT      = 4,             
    LOAD_NODE_CURRENT   = 5,                                       
    EXTERNAL_SETTINGS   = 6,             
    Name_ch07,          
    Name_ch08,            
    Name_ch09,            
    Name_ch10,            
    Name_ch11,            
    Name_ch12,           
    Name_ch13,           
    Name_ch14,            
    ch_HRf              = 15     // Промежуточный канал (0.5 Ref)
  };
  
  static CADC_STORAGE& getInstance() 
  {
    static CADC_STORAGE instance;
    return instance;
  }
  
  // --- Запись ---
  inline void setExternal(unsigned char channel, signed short value)
  {
    external_[channel] = value;
  }  
  inline void setInternal(unsigned char channel, signed short value)
  {
    internal_[channel] = value;
  }
  // --- Чтение ---
  inline signed short getExternal(unsigned char channel)
  {
    return external_[channel];
  }
  
  inline signed short getInternal(unsigned char channel)
  {
    return internal_[channel];
  }
  // --- Чтение указателей ---
  signed short* getExternalPointer(unsigned char channel)
  {
    return &external_[channel];
  }
  
private:

    signed short external_[G_CONST::NUMBER_CHANNELS]{};
    signed short internal_[G_CONST::NUMBER_I_CHANNELS]{};
    
    CADC_STORAGE(); 
    CADC_STORAGE(const CADC_STORAGE&) = delete;
    CADC_STORAGE& operator=(const CADC_STORAGE&) = delete; 
    
};