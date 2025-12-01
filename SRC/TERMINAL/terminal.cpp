#include "terminal.hpp"
#include "tree.hpp"

unsigned short Irotor = 123;
unsigned short Urotor = 456;
unsigned short Istat = 789;
float coeff = -123.64f;
bool flag = true;

CTERMINAL::CTERMINAL(CTerminalUartDriver& uartDrv) : uartDrv(uartDrv) 
{
  initMenu();
  render();
}

// Конструкторы узла
CTERMINAL::MenuNode::MenuNode(const std::string& t) : title(t) {}
CTERMINAL::MenuNode::MenuNode(const std::string& t, void* v, VarType vt, bool edit) : title(t), value(v), type(vt), editable(edit) {}

// Конструктор дерева
void CTERMINAL::initMenu()
{
  MENU = makeMENU();
  currentList = &MENU;
  selectedIndex = 0;
}  

void CTERMINAL::get_key()
{
  unsigned char input_key;
  if(uartDrv.poll_rx(input_key))
  {
    switch(input_key)
    {   
    case Up: 
      up();
      break;
    case Down:
      down();
      break;
    case Enter:
      enter();
      break;
    case Escape:
      escape();
      break;
    }
    cur_key = input_key;
  }
}

// Отображение двух строк
void CTERMINAL::render() const 
{
  std::string string_line;
  
  for (int n_line = 0; n_line < 2; ++n_line) 
  {
    unsigned char idx = indexTop + n_line;
    if (idx >= currentList->size()) break;
    
    const auto& node = (*currentList)[idx];
    
    // курсор
    string_line += (cursorPos == n_line ? ">" : " ");
    
    string_line += node.title;
    
    if (node.value) 
    {
      string_line += " ";
      switch (node.type) 
      {
      case USHORT: string_line += std::to_string(*(unsigned short*)node.value); break;
      case SHORT:  
        {
          short v = *(short*)node.value;
          string_line += (v >= 0 ? "+" : "") + std::to_string(v);
        } 
        break;
      case FLOAT: 
        {
          float v = *(float*)node.value;
          if (v >= 0) string_line += "+";
          char buf[16];
          snprintf(buf, sizeof(buf), "%.2f", v);
          string_line += buf;
        } 
        break;
      case BOOL: 
        string_line += (*(bool*)node.value ? "true" : "false"); 
        break;
      default: break;
      }
      if (node.editable) string_line += " <";
    }
    string_line += "\r"; // только CR, без LF
  } 
  //uartDrv.sendBuffer((unsigned char*)string_line.c_str());//, out.size());
}

void CTERMINAL::up() 
{
  if (cursorPos == 1) 
  {
    cursorPos = 0; // просто перемещаем курсор на верхнюю строку
  } else if (indexTop > 0) 
  {
    indexTop--;    // сдвигаем окно вверх
  }
  render();
}

void CTERMINAL::down() 
{
  if (cursorPos == 0) 
  {
    cursorPos = 1; // курсор на нижнюю строку
  } else if (indexTop + 2 < currentList->size()) 
  {
    indexTop++;    // сдвигаем окно вниз
  }
  render();
}

void CTERMINAL::enter() 
{
  auto& node = (*currentList)[selectedIndex];
  if (!node.children.empty()) 
  {
    history.push({currentList, (uint8_t)selectedIndex});
    currentList = &node.children;
    selectedIndex = 0;
  }
}

void CTERMINAL::escape() 
{
  if (!history.empty()) 
  {
    Frame f = history.top();
    history.pop();
    currentList = f.list;
    selectedIndex = f.index;
  }
}

// Редактирование значения
void CTERMINAL::edit(int delta) 
{
  auto& node = (*currentList)[selectedIndex];
  if (node.value && node.editable) 
  {
    switch (node.type) 
    {
    case USHORT: *(static_cast<unsigned short*>(node.value)) += delta; break;
    case SHORT:  *(static_cast<short*>(node.value)) += delta; break;
    case FLOAT:  *(static_cast<float*>(node.value)) += delta * 0.1f; break;
    case BOOL:   *(static_cast<bool*>(node.value)) = !*(static_cast<bool*>(node.value)); break;
    default: break;
    }
  }
}

