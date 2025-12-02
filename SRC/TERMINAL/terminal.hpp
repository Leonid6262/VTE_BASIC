#pragma once

#include "terminalUartDriver.hpp"
#include "SIFU.hpp"

#include <string>
#include <vector>
#include <stack>

class CTERMINAL{ 
  
public:
  
  CTERMINAL(CTerminalUartDriver& drv);

  // Типы переменных
  enum VarType { NONE, USHORT, SSHORT, FLOAT, BOOL };
  
  struct MenuNode 
  {
    std::string title;              // название пункта
    std::vector<MenuNode> children; // подменю
    void* value = nullptr;          // указатель на переменную
    VarType type = NONE;            // тип переменной
    bool editable = false;          // признак редактируемости
    
    MenuNode(const std::string& t);
    MenuNode(const std::string& t, void* v, VarType vt, bool edit = false);   
    MenuNode(const std::string& t, std::vector<MenuNode> ch) : title(t), children(std::move(ch)) {} 
  };
  
  void get_key();
 
private:
  
  CTerminalUartDriver& uartDrv;
  
  struct Frame 
  {
    std::vector<MenuNode>* list;
    unsigned char index;
  };  
  
  static std::vector<MenuNode> MENU;
  static std::vector<MenuNode>* currentList;
  static std::stack<Frame> history;
  
  void render_menu() const;
  void UP();
  void DOWN();
  void ENTER();
  void ESCAPE();
  
  static std::string padTo16(const std::string&);
  
  enum class ELED
  {
    LED_RED    = 0x01,
    LED_GREEN  = 0x02,
    LED_BLUE   = 0x03,
    LED_YELLOW = 0x04,
    LED_WHITE  = 0x09,
    LED_OFF    = 0x0B 
  };
  
  enum EKey_code
  {
    Up     = 0x2B,
    Down   = 0x2D,
    Enter  = 0x0D,
    Escape = 0x1B,
  };
  
  static unsigned char selectedIndex;
  static unsigned char indexTop;     // индекс верхней строки окна
  static unsigned char cursorPos;    // 0 = верхняя строка, 1 = нижняя строка

};
