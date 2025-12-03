#pragma once
#include "terminal.hpp"

inline std::vector<CTERMINAL::MenuNode> makeMENU()
{
  std::vector<CTERMINAL::MenuNode> MENU_UTF =
  {    
    {"ИНДИКАЦИЯ",{
      {"АНАЛОГОВАЯ",},
      {"ДИСКРЕТНАЯ",},
  }},
  {"УСТАВКИ",{
      {"РЕГУЛЯТОРОВ",{
          {"ТОКА",},
          {"COS PHI",},
          {"Q МОЩНОСТИ",},
    }},
      {"ОГРАНИЧЕНИЙ",},
      {"АВАРИЙНЫЕ",},
  }},
  {"РУЧНОЙ РЕЖИМ",},
  {"УСТАНОВКА ЧАСОВ",},
  };   
  
  return MENU_UTF;
}