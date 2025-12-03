#include "terminal.hpp"
#include "tree.hpp"

CTERMINAL::CTERMINAL(CTerminalUartDriver& uartDrv) : uartDrv(uartDrv) 
{
    // создание меню
    MENU = makeMENU();
    currentList = &MENU;

    // очистка экрана
    unsigned char clr_data[] = {"                \r\n"};
    uartDrv.sendBuffer(clr_data, sizeof(clr_data));
    uartDrv.sendBuffer(clr_data, sizeof(clr_data));
    uartDrv.sendBuffer(clr_data, sizeof(clr_data));

    // выключение светодиода
    unsigned char led_off[] = { static_cast<unsigned char>(ELED::LED_OFF), '\r' };
    uartDrv.sendBuffer(led_off, sizeof(led_off));

    // первая отрисовка меню
    render_menu();
}

// Конструкторы узла
CTERMINAL::MenuNode::MenuNode(const std::string& t) : title(t) {}
CTERMINAL::MenuNode::MenuNode(const std::string& t, void* v, VarType vt, bool edit) : title(t), value(v), type(vt), editable(edit) {}

// Определения статических членов
std::vector<CTERMINAL::MenuNode> CTERMINAL::MENU;
std::vector<CTERMINAL::MenuNode>* CTERMINAL::currentList = nullptr;
std::stack<CTERMINAL::Frame> CTERMINAL::history;
unsigned char CTERMINAL::selectedIndex = 0; 
unsigned char CTERMINAL::indexTop = 0;
unsigned char CTERMINAL::cursorPos = 0; 

void CTERMINAL::get_key()
{
  unsigned char input_key;
  if(uartDrv.poll_rx(input_key))
  {
    switch(input_key)
    {   
    case Up:    UP();     break;
    case Down:  DOWN();   break;
    case Enter: ENTER();  break;
    case Escape:ESCAPE(); break;
    }
  }
}

// Отображение двух строк
void CTERMINAL::render_menu() const 
{
  std::string string_line = "";
  // Верхняя строка
  const auto& node0 = (*currentList)[indexTop];
  string_line = (cursorPos == 0 ? "-" : "");
  string_line += node0.title; 
  std::string cp1251 = utf8_to_cp1251(string_line);
  cp1251 = padTo16(cp1251);    
  cp1251 += "\r\n";      
  uartDrv.sendBuffer(reinterpret_cast<const unsigned char*>(cp1251.c_str()), cp1251.size());
  
  // Нижняя строка (если есть)
  if (indexTop + 1 < currentList->size()) {
    const auto& node1 = (*currentList)[indexTop + 1];
    string_line = (cursorPos == 1 ? "-" : "");
    string_line += node1.title;
    std::string cp1251 = utf8_to_cp1251(string_line);
    cp1251 = padTo16(cp1251);    
    cp1251 += "\r";    
    uartDrv.sendBuffer(reinterpret_cast<const unsigned char*>(cp1251.c_str()), cp1251.size()); 
  } 
}

std::string CTERMINAL::padTo16(const std::string& s) 
{
  if (s.size() >= 16) return s.substr(0, 16);   // если длиннее — обрезаем
  return (s + std::string(16 - s.size(), ' ')); // дополняем пробелами
}

void CTERMINAL::UP() 
{
  if (currentList->empty()) return;     // защита от пустого списка
  
  unsigned char selected = indexTop + cursorPos;
  
  if (currentList->size() == 1)         // если всего один элемент — курсор всегда на нём
  {       
    indexTop = 0;
    cursorPos = 0;
  } else 
  {
    if (selected == 0)                  // циклический переход вверх
    {
      selected = currentList->size() - 1;
    }
    else
    {
      selected--;
    }
    if (selected == 0)                  // пересчёт окна и курсора
    {
      indexTop = 0;
      cursorPos = 0;
    } else 
    {
      indexTop = selected - 1;
      cursorPos = 1;
    }
  } 
  render_menu();
}

void CTERMINAL::DOWN() 
{
  if (currentList->empty()) return;     // защита от пустого списка
  
  unsigned char selected = indexTop + cursorPos;
  
  if (currentList->size() == 1)         // если всего один элемент — курсор всегда на нём
  {   
    indexTop = 0;
    cursorPos = 0;
  } else                                // циклический переход вниз
  {   
    if (selected + 1 >= currentList->size())
    {
      selected = 0;
    }
    else
    {
      selected++;
    }  
    if (selected == 0)                  // пересчёт окна и курсора
    {
      indexTop = 0;
      cursorPos = 0;
    } else 
    {
      indexTop = selected - 1;
      cursorPos = 1;
    }
  }
  render_menu();
}

void CTERMINAL::ENTER() 
{
  // Определяем текущий выбранный элемент по окну
  unsigned char selected = indexTop + cursorPos;
  auto& node = (*currentList)[selected];  
  if (!node.children.empty()) 
  {   
    history.push({currentList, selected});      // Сохраняем текущее состояние в стек истории  
    currentList = &node.children;               // Переходим в дочерний список
    indexTop = 0;                               // окно всегда с начала
    cursorPos = 0;                              // курсор на верхней строке
  }
  render_menu();
}

void CTERMINAL::ESCAPE() 
{
  if (!history.empty()) 
  {
    Frame f = history.top();
    history.pop();        
    currentList = f.list;               // Восстанавливаем список и выбранный индекс
    unsigned char selected = f.index;       
    if (selected == 0)                  // Восстанавливаем окно и курсор
    {
      indexTop = 0;
      cursorPos = 0;
    } else 
    {
      // Ставим выбранный элемент либо в верхнюю, либо в нижнюю строку окна
      indexTop = (selected > 0 ? selected - 1 : 0);
      cursorPos = (selected > 0 ? 1 : 0);
    }
  }
  render_menu();
}

// UTF-8 в Windows-1251
std::string CTERMINAL::utf8_to_cp1251(const std::string& utf8)
{
  std::string cp1251;
  cp1251.reserve(utf8.size());
  
  for (size_t i = 0; i < utf8.size(); )
  {
    unsigned char c = (unsigned char)utf8[i];
    if (c < 0x80) {
      cp1251.push_back(c);
      i++;
    } else if ((c == 0xD0 || c == 0xD1) && i + 1 < utf8.size()) {
      unsigned char c2 = (unsigned char)utf8[i+1];
      unsigned char out;
      if (c == 0xD0) {
        if (c2 >= 0x90 && c2 <= 0xBF) out = c2 + 0x30;          // А..Я
        else if (c2 == 0x81) out = 0xA8;                        // Ё
        else out = '?';
      } else {
        if (c2 >= 0x80 && c2 <= 0x8F) out = c2 + 0x70;          // а..п
        else if (c2 >= 0x90 && c2 <= 0x9F) out = c2 + 0x10;     // р..я
        else if (c2 == 0x91) out = 0xB8;                        // ё
        else out = '?';
      }
      cp1251.push_back(out);
      i += 2;
    } else {
      cp1251.push_back('?');
      i++;
    }
  }
  return cp1251;
}


