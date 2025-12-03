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
CTERMINAL::MenuNode::MenuNode(const std::string& t,
                              std::vector<MenuNode> c,
                              void* v)
    : title(t), children(std::move(c)), value(v) {}
    
// Определения статических членов
std::vector<CTERMINAL::MenuNode> CTERMINAL::MENU;
std::vector<CTERMINAL::MenuNode>* CTERMINAL::currentList = nullptr;
std::stack<CTERMINAL::Frame> CTERMINAL::history;
unsigned char CTERMINAL::listIndex = 0; 
unsigned char CTERMINAL::screenPosition = 0;
unsigned char CTERMINAL::cursorLine = 0; 

void CTERMINAL::get_key()
{
  unsigned char input_key;
  if(uartDrv.poll_rx(input_key))
  {
    onKey((EKey_code)input_key);
  }
}

// Отображение двух строк меню
void CTERMINAL::render_menu() const
{
  // выводим два элемента списка начиная с screenPosition
  for (int line = 0; line < 2; ++line) 
  {
    int listIndex = screenPosition + line;
    std::string text;    
    if (listIndex < currentList->size()) 
    {
      text = utf8_to_cp1251((*currentList)[listIndex].title);
    } 
    else 
    {
      text = "";
    }    
    // добавляем курсор, если это текущая строка
    if (line == cursorLine) 
    {
      text = "-" + text;
    }    
    text = padTo16(text);
    text += (line == 0 ? "\r\n" : "\r");
    
    uartDrv.sendBuffer(reinterpret_cast<const unsigned char*>(text.c_str()), text.size());
  }
}

// Отображение окна переменной
void CTERMINAL::render_var() const
{
  unsigned short listIndex = screenPosition + cursorLine;
  const auto& node = (*currentList)[listIndex];
  
  // верхняя строка — имя переменной
  std::string line1 = utf8_to_cp1251(node.title);
  line1 = padTo16(line1);
  line1 += "\r\n";
  uartDrv.sendBuffer(reinterpret_cast<const unsigned char*>(line1.c_str()), line1.size());
  
  // нижняя строка — значение переменной
  unsigned short val = *static_cast<unsigned short*>(node.value);
  char buf[17];
  snprintf(buf, sizeof(buf), "%5u", val);
  
  std::string line2(buf);
  line2 = padTo16(line2);
  line2 += "\r";
  uartDrv.sendBuffer(reinterpret_cast<const unsigned char*>(line2.c_str()), line2.size());
}

std::string CTERMINAL::padTo16(const std::string& s) 
{
  if (s.size() >= 16) return s.substr(0, 16);   // если длиннее — обрезаем
  return (s + std::string(16 - s.size(), ' ')); // дополняем пробелами
}

void CTERMINAL::onKey(EKey_code key)
{
  if (key == EKey_code::DOWN) 
  {
    if (cursorLine == FirstLine && screenPosition + 1 < currentList->size()) 
    {
      // курсор вниз, остаёмся в пределах экрана
      cursorLine = SecondLine;
    }
    else if (cursorLine == SecondLine && screenPosition + 2 < currentList->size()) 
    {
      // экран прокручивается вниз
      screenPosition++;
    }
  }
  else if (key == EKey_code::UP) 
  {
    if (cursorLine == SecondLine) 
    {
      // курсор вверх, остаёмся в пределах экрана
      cursorLine = FirstLine;
    }
    else if (cursorLine == FirstLine && screenPosition > 0) 
    {
      // экран прокручивается вверх
      screenPosition--;
    }
  }
  else if (key == EKey_code::ENTER) 
  {
    unsigned short listIndex = screenPosition + cursorLine;
    auto& node = (*currentList)[listIndex];
    
    if (!node.children.empty()) 
    {
      // сохраняем всё состояние
      history.push({currentList, screenPosition, cursorLine, listIndex});
      
      // переходим в подменю
      currentList = &node.children;
      screenPosition = 0;
      cursorLine = FirstLine;
      
      if (!currentList->empty() && (*currentList)[0].value) 
      {
        render_var();
      } 
      else 
      {
        render_menu();
      }
    }
    else if (node.value) 
    {
      render_var();
    }
  }
  else if (key == EKey_code::ESCAPE) 
  {
    if (!history.empty()) 
    {
      Frame f = history.top();
      history.pop();
      
      currentList    = f.currentList;
      screenPosition = f.screenPosition;
      cursorLine     = f.cursorLine;
      // listIndex можно использовать при необходимости напрямую
      
      render_menu();
    }
  }
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


