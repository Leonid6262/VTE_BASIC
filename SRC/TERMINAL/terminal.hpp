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
  
struct MenuNode {
    std::string title;
    std::vector<MenuNode> children;
    void* value;

    // Универсальный конструктор
    MenuNode(const std::string& t,
             std::vector<MenuNode> c = {},
             void* v = nullptr);
};
  
  void get_key();
  
  // Статическая утилита перекодировки
  static std::string utf8_to_cp1251(const std::string& utf8);
  
private:
  
  CTerminalUartDriver& uartDrv;
  
struct Frame {
    std::vector<MenuNode>* currentList; // указатель на список
    unsigned short screenPosition;      // позиция окна
    unsigned short cursorLine;          // строка курсора (0 или 1)
    unsigned short listIndex;           // фактический индекс выбранного узла
};
  
  static std::vector<MenuNode> MENU;
  static std::vector<MenuNode>* currentList;
  static std::stack<Frame> history;
  
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
    UP     = 0x2B,
    DOWN   = 0x2D,
    ENTER  = 0x0D,
    ESCAPE = 0x1B,
  };
  
  void onKey(EKey_code);
  void render_menu() const;
  void render_var() const;
  
  //void UP();
  //void DOWN();
  //void ENTER();
  //void ESCAPE();
  
  
  static unsigned char listIndex;       // текущий индекс
  static unsigned char screenPosition;  // индекс первой строки окна
  static unsigned char cursorLine;      // позиция курсора, первая строка / вторая строка
  
  static constexpr unsigned char FirstLine  = 0;
  static constexpr unsigned char SecondLine = 1;
  
};
