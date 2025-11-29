#pragma once

#include "LPC407x_8x_177x_8x.h"

#include <string>
#include <vector>
#include <stack>
#include <cstdint>
#include <iostream>

class CTERMINAL{ 
  
public:
  
  CTERMINAL(LPC_UART_TypeDef*);
  
  // Типы переменных
  enum VarType { NONE, USHORT, SHORT, FLOAT, BOOL };
  
  struct MenuNode {
    std::string title;              // название пункта
    std::vector<MenuNode> children; // подменю
    void* value = nullptr;          // указатель на переменную
    VarType type = NONE;            // тип переменной
    bool editable = false;          // признак редактируемости
    
    MenuNode(const std::string& t);
    MenuNode(const std::string& t, void* v, VarType vt, bool edit = false);
    
    MenuNode(const std::string& t, std::vector<MenuNode> ch)
    : title(t), children(std::move(ch)) {}
    
    
  };
  
  struct Frame {
    std::vector<MenuNode>* list;
    uint8_t index;
  };
  
  CTERMINAL(unsigned short* Irotor, unsigned short* Urotor,
            short* Istat, float* coeff, bool* flag);
  
  void render() const;
  void up();
  void down();
  void enter();
  void esc();
  void edit(int delta);
  
  void basic();
  
private:
  
  LPC_UART_TypeDef* UART;
  
  std::vector<MenuNode> topMenu;
  std::vector<MenuNode>* currentList;
  uint8_t selectedIndex;
  std::stack<Frame> history;
  
  
};
