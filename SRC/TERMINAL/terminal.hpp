#pragma once

#include "terminalUartDriver.hpp"

#include <string>
#include <vector>
#include <stack>
#include <cstdint>
#include <iostream>

class CTERMINAL{ 
  
public:
  
  CTERMINAL(CTerminalUartDriver& drv);
  
  void initMenu(unsigned short* Irotor, unsigned short* Urotor,
                short* Istat, float* coeff, bool* flag); 
  
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
  
  void get_key();
  
  void render() const;
  void up();
  void down();
  void enter();
  void esc();
  void edit(int delta);
 
private:
  
  CTerminalUartDriver& uartDrv;
  
  std::vector<MenuNode> topMenu;
  std::vector<MenuNode>* currentList;
  std::stack<Frame> history;
  
  unsigned char cur_key;
  unsigned char  selectedIndex;
  unsigned char indexTop = 0;     // индекс верхней строки окна
  unsigned char cursorPos = 0;    // 0 = верхняя строка, 1 = нижняя строка
  
  
};
