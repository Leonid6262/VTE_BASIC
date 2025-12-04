#pragma once

#include "AdcStorage.hpp"

inline std::vector<CTERMINAL::MenuNode> makeMENU()
{
  //=====Отображаемые значения (иллюстративно)===========
  auto& instans = CADC_STORAGE::getInstance();
  
  auto pI_rotor = instans.getExternalPointer(CADC_STORAGE::ROTOR_CURRENT);
  auto i_rotor_disp = 1.0f;
  
  auto pU_rotor = instans.getExternalPointer(CADC_STORAGE::ROTOR_VOLTAGE);
  auto u_rotor_disp = 1.0f;
  
  // Указатели на измеренные значения и параметры отображения, здесь
  // приведенны как иллюстрация. В Production метод makeMENU() должен
  // получать данные отображения через, как вариант, централизованной 
  // структуру Proxi Singletona или трансформироваться в отдельнфй
  // статический класс, разделяющий код обработки (CTERMINAL) от узлов и данных.
  
  // Так же, в CTERMINAL в настоящий момент не реализовано:
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
                node("I ROTOR", {}, pI_rotor, "d", i_rotor_disp, 0, sshort),
                node("U ROTOR", {}, pU_rotor, "d", u_rotor_disp, 0, sshort)
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