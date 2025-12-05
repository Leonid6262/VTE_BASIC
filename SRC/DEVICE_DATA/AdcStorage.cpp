#include "AdcStorage.hpp"
#include "proxy_pointer_var.hpp"

CADC_STORAGE::CADC_STORAGE() : settings(CEEPSettings::getInstance())
{
  // Регистрация указателей в прокси фабрики меню
  CProxyPointerVar::getInstance().registerIrotor(getExternalPointer(CADC_STORAGE::ROTOR_CURRENT));
  CProxyPointerVar::getInstance().registerUrotor(getExternalPointer(CADC_STORAGE::ROTOR_VOLTAGE));
};
