#pragma once

// Prox-Singleton концентратор переменных

class CProxyPointerVar 
{
public:
  static CProxyPointerVar& getInstance() 
  {
    static CProxyPointerVar instance;
    return instance;
  }
  
  // Регистрация указателей переменных
  void registerIrotor(signed short* ptr)      { pI_rotor = ptr; }
  void registerUrotor(signed short* ptr)      { pU_rotor = ptr; }
  void registerIstatorRms(signed short* ptr)  { pI_statorRms = ptr; }
  void registerUstatorRms(signed short* ptr)  { pU_statorRms = ptr; }
  
  void registerAlphaCur(signed short* ptr)    { pAlphaCur = ptr; }
  
  // Коэффициенты отображения
  const float i_rotor_disp      = 0.9f;
  const float u_rotor_disp      = 0.7f;
  const float i_stator_rms_disp = 0.8f;
  const float u_stator_rms_disp = 0.1f;
  const float alpha_disp        = 0.018f; // 180гр/10000
  
  // Доступ к указателям
  signed short* getIrotor()     const { return pI_rotor; }
  signed short* getUrotor()     const { return pU_rotor; }
  signed short* getIstatorRms() const { return pI_statorRms; }
  signed short* getUstatorRms() const { return pU_statorRms; }
  
  signed short* getAlphaCur()   const { return pAlphaCur; }
  
private:
  
  CProxyPointerVar() = default;
  
  signed short* pI_rotor;
  signed short* pU_rotor;
  signed short* pI_statorRms;
  signed short* pU_statorRms;
  
  signed short* pAlphaCur;
};