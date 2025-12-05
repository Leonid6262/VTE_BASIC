#pragma once

#include "proxy_pointer_var.hpp"
#include "AdcStorage.hpp"

// Фабрика дерева меню. Вынесена в отдельную функцию для удобства
inline std::vector<CTERMINAL::MenuNode> MENU_Factory()
{
  // Указатель Prox-Singleton концентратора отображаемых/редактируемых переменных
  CProxyPointerVar& v = CProxyPointerVar::getInstance();
  
  // В CTERMINAL в настоящий момент НЕ реализовано:
  // 1. Обработчик и рендер редактируемых переменных
  // 2. Комплексный обработчик и рендер редактируемых и индицируемых переменных
  // 3. Обработчик исполняемых методов по булевым операциям
  // 4. Режим текстовых сообщений
  // 5. Двуязычный режим работы терминала
    
  using node = CTERMINAL::MenuNode;
  
  static constexpr auto sshort = CTERMINAL::EVarType::SSHORT;
  static constexpr auto ushort = CTERMINAL::EVarType::USHORT;
  static constexpr auto ffloat = CTERMINAL::EVarType::FLOAT;
  static constexpr auto bbool  = CTERMINAL::EVarType::BOOL;
    
std::vector<node> MENU_UTF = {
        node("ИНДИКАЦИЯ", {
            node("АНАЛОГОВАЯ", {
                node("I-ROTOR",    {}, v.getIrotor(),      "A", v.i_rotor_disp,      0, sshort),
                node("U-ROTOR",    {}, v.getUrotor(),      "V", v.u_rotor_disp,      0, sshort),
                node("Irms-STATOR",{}, v.getIstatorRms(),  "A", v.i_stator_rms_disp, 1, sshort),
                node("Urms-STATOR",{}, v.getUstatorRms(),  "V", v.u_stator_rms_disp, 1, sshort),
                node("ALPHA-CUR",  {}, v.getAlphaCur(),  "deg", v.alpha_disp,        1, sshort),
            }),
            node("ДИСКРЕТНАЯ")
        }),
        node("УСТАВКИ", {
            node("РЕГУЛЯТОРОВ", {
                node("ТОКА"),
                node("COS PHI"),
                node("Q МОЩНОСТИ")
            }),
            node("ОГРАНИЧЕНИЙ"),
            node("АВАРИЙНЫЕ")
        }),
        node("РУЧНОЙ РЕЖИМ"),
        node("УСТАНОВКА ЧАСОВ")
    };

    return MENU_UTF;
}