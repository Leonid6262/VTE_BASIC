#include "terminal.hpp"
#include "pause_us.hpp"
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

    
    mode = EViewMode::Menu;   // текущий режим - Меню
    
    // первая отрисовка меню
    render_menu();
}

// Конструктор узла
CTERMINAL::MenuNode::MenuNode(const std::string& t,
                              std::vector<MenuNode> c,
                              void* v,
                              const std::string& u,
                              float s,
                              unsigned char p,
                              EVarType vt)
    : title(t),
      children(std::move(c)),
      value(v),
      unit(u),
      scale(s),
      precision(p),
      varType(vt) 
{}

  
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
    onKey(static_cast<EKey_code>(input_key));
  }
  else 
  {
    onKey(EKey_code::NONE); // нет нажатий
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

static std::string padRight(const std::string& s, unsigned char width)
{
  if (s.size() >= width) return s.substr(0, width);
  std::string out = s;
  out.append(width - out.size(), ' ');
  return out;
}

// Отображение окна переменной
void CTERMINAL::render_var(EEntry entry) const
{
  unsigned short listIndex = screenPosition + cursorLine;
  const auto& node = (*currentList)[listIndex];
  if(entry == CTERMINAL::EEntry::First)
  {
    // верхняя строка — имя переменной
    std::string title_cp1251 = utf8_to_cp1251(node.title);  // UTF-8 -> CP1251
    std::string line1 = padRight(title_cp1251, (16 - node.unit.size())) + node.unit + "\r\n";  
    uartDrv.sendBuffer(reinterpret_cast<const unsigned char*>(line1.c_str()), line1.size());    
  }

  // нижняя строка — значение переменной
  char buf[17];
  switch (node.varType)
  {
  case EVarType::SSHORT:
    {
      signed short raw = *static_cast<signed short*>(node.value);
      float val = raw * node.scale;
      snprintf(buf, sizeof(buf), "%.*f", node.precision, val);
      break;
    }
    
  case EVarType::USHORT:
    {
      unsigned short raw = *static_cast<unsigned short*>(node.value);
      float val = raw * node.scale;
      snprintf(buf, sizeof(buf), "%.*f", node.precision, val);
      break;
    }
    
  case EVarType::FLOAT:
    {
      float raw = *static_cast<float*>(node.value);
      float val = raw * node.scale;
      snprintf(buf, sizeof(buf), "%.*f", node.precision, val);
      break;
    }
    
  case EVarType::BOOL:
    {
      bool raw = *static_cast<bool*>(node.value);
      snprintf(buf, sizeof(buf), "%s", raw ? "ON" : "OFF");
      break;
    }
    
  default:
    snprintf(buf, sizeof(buf), "----");
    break;
  }
  
  std::string line2(buf);
  line2 = padTo16(line2) + "\r";
  uartDrv.sendBuffer(reinterpret_cast<const unsigned char*>(line2.c_str()), line2.size());
}

std::string CTERMINAL::padTo16(const std::string& s) 
{
  if (s.size() >= 16) return s.substr(0, 16);   // если длиннее — обрезаем
  return (s + std::string(16 - s.size(), ' ')); // дополняем пробелами
}

void CTERMINAL::onKey(EKey_code key)
{
  switch (key)
  {
  case EKey_code::DOWN:
    if (mode == EViewMode::Variable) navigateDownVar();
    else navigateDownMenu();
    break;
    
  case EKey_code::UP:
    if (mode == EViewMode::Variable) navigateUpVar();
    else navigateUpMenu();
    break;
    
  case EKey_code::ENTER:
    handleEnter();
    break;
    
  case EKey_code::ESCAPE:
    handleEscape();
    break;
    
  case EKey_code::FnENTER:  // Fn+Enter - Запись уставок
    {unsigned char led_green[] = { static_cast<unsigned char>(ELED::LED_GREEN), '\r' };
    uartDrv.sendBuffer(led_green, sizeof(led_green));
    CEEPSettings::getInstance().saveSettings();
    Pause_us(200000);
    unsigned char led_off[] = { static_cast<unsigned char>(ELED::LED_OFF), '\r' };
    uartDrv.sendBuffer(led_off, sizeof(led_off));}
    break;   
    
  case EKey_code::NONE:  
  default:
    // таймер для периодического обновления индикации
    static unsigned int prev_TC0 = LPC_TIM0->TC;
    unsigned int dTrs = LPC_TIM0->TC - prev_TC0; // дельта [0.1 мкс]
    
    if (mode == EViewMode::Variable && dTrs >= 2000000) { // 200 мс
      prev_TC0 = LPC_TIM0->TC;
      render_var(CTERMINAL::EEntry::Next);
    }
    break;
  }
}

// Навигация вниз по меню
void CTERMINAL::navigateDownMenu() {
  unsigned short listIndex = screenPosition + cursorLine;
  if (listIndex + 1 < currentList->size()) {
    if (cursorLine == FirstLine) {
      cursorLine = SecondLine;
    } else {
      screenPosition++;
    }
  } else {
    // циклический переход: с последнего на первый
    screenPosition = 0;
    cursorLine = FirstLine;
  }
  render_menu();
}
// Навигация вверх по меню
void CTERMINAL::navigateUpMenu() {
  unsigned short listIndex = screenPosition + cursorLine;
  if (listIndex > 0) {
    if (cursorLine == SecondLine) {
      cursorLine = FirstLine;
    } else {
      screenPosition--;
    }
  } else {
    // циклический переход: с первого на последний
    if (currentList->size() > 1) {
      screenPosition = currentList->size() - 2;
      cursorLine = SecondLine;
    } else {
      screenPosition = 0;
      cursorLine = FirstLine;
    }
  }
  render_menu();
}
// Навигация вниз по переменным
void CTERMINAL::navigateDownVar() {
  unsigned short listIndex = screenPosition + cursorLine;
  if (listIndex + 1 < currentList->size()) {
    if (cursorLine == FirstLine) {
      cursorLine = SecondLine;
    } else {
      screenPosition++;
    }
  } else {
    screenPosition = 0;
    cursorLine = FirstLine;
  }
  render_var(CTERMINAL::EEntry::First);
}
// Навигация вверх по переменным
void CTERMINAL::navigateUpVar() {
  unsigned short listIndex = screenPosition + cursorLine;
  if (listIndex > 0) {
    if (cursorLine == SecondLine) {
      cursorLine = FirstLine;
    } else {
      screenPosition--;
    }
  } else {
    if (currentList->size() > 1) {
      screenPosition = currentList->size() - 2;
      cursorLine = SecondLine;
    } else {
      screenPosition = 0;
      cursorLine = FirstLine;
    }
  }
  render_var(CTERMINAL::EEntry::First);
}
// Обработка ENTER
void CTERMINAL::handleEnter() {
  unsigned short listIndex = screenPosition + cursorLine;
  auto& node = (*currentList)[listIndex];
  
  if (!node.children.empty()) {
    history.push({currentList, screenPosition, cursorLine, listIndex});
    currentList    = &node.children;
    screenPosition = 0;
    cursorLine     = FirstLine;
    
    if (!currentList->empty() && (*currentList)[0].value) {
      mode = EViewMode::Variable;
      render_var(CTERMINAL::EEntry::First);
    } else {
      mode = EViewMode::Menu;
      render_menu();
    }
  }
  else if (node.value) {
    mode = EViewMode::Variable;
    render_var(CTERMINAL::EEntry::First);
  }
}
// Обработка ESCAPE
void CTERMINAL::handleEscape() {
  if (!history.empty()) {
    Frame f = history.top();
    history.pop();
    
    currentList    = f.currentList;
    screenPosition = f.screenPosition;
    cursorLine     = f.cursorLine;
    
    mode = EViewMode::Menu;
    render_menu();
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


