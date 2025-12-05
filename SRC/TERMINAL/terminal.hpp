#pragma once

#include "terminalUartDriver.hpp"

#include <string>
#include <vector>
#include <stack>

class CTERMINAL{ 
  
public:
  
  CTERMINAL(CTerminalUartDriver& drv);
  void make_menu();
  
  // Типы переменных
  enum class EVarType { NONE, USHORT, SSHORT, FLOAT, BOOL };

  struct MenuNode {
    std::string title;
    std::vector<MenuNode> children;
    void* value;
    
    std::string unit;
    float scale;
    unsigned char precision;
    EVarType varType;
    
    // Универсальный конструктор (только объявление!)
    MenuNode(const std::string& t,
             std::vector<MenuNode> c = {},
             void* v = nullptr,
             const std::string& u = "",
             float s = 1.0f,
             unsigned char p = 0,
             EVarType vt = EVarType::NONE);
    
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
  
  enum class EKey_code
  {
    NONE    = 0x00,
    UP      = 0x2B,
    DOWN    = 0x2D,
    ENTER   = 0x0D,
    ESCAPE  = 0x1B,
    FnENTER = 0x78
  };
  
  enum class EEntry {
    First,
    Next
  };
  
  enum class EViewMode {
    Menu,
    Variable
  };
  
  EViewMode mode;
 
  void navigateDownVar();
  void navigateDownMenu();
  void navigateUpVar();
  void navigateUpMenu();
  void handleEnter();
  void handleEscape();
  
  void onKey(EKey_code);
  void render_menu() const;
  void render_var(EEntry) const;
  
  static unsigned char listIndex;       // текущий индекс
  static unsigned char screenPosition;  // индекс первой строки окна
  static unsigned char cursorLine;      // позиция курсора, первая строка / вторая строка
  
  static bool dataViewMode;
  
  static constexpr unsigned char FirstLine  = 0;
  static constexpr unsigned char SecondLine = 1;
  
};
