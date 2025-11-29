#include "terminal.hpp"

CTERMINAL::CTERMINAL(CTerminalUartDriver& uartDrv) : uartDrv(uartDrv) {}

// Конструкторы узла
CTERMINAL::MenuNode::MenuNode(const std::string& t) : title(t) {}
CTERMINAL::MenuNode::MenuNode(const std::string& t, void* v, VarType vt, bool edit)
    : title(t), value(v), type(vt), editable(edit) {}

// Конструктор терминала
void CTERMINAL::initMenu(unsigned short* Irotor, unsigned short* Urotor, 
                         short* Istat, float* coeff, bool* flag) {
    topMenu = {
        { "ИНДИКАЦИЯ", {
            { "Irotor", Irotor, USHORT, false },
            { "Urotor", Urotor, USHORT, false },
            { "Istat",  Istat,  SHORT,  false }
        }},
        { "УСТАВКИ", {
            { "Coeff", coeff, FLOAT, true }
        }},
        { "НАЛАДКА", {
            { "Enable", flag, BOOL, true }
        }}
    }; 

    currentList = &topMenu;
    selectedIndex = 0;
}

void CTERMINAL::get_key()
{
  uartDrv.poll_rx(cur_key);
}

// Отображение двух строк
void CTERMINAL::render() const {
    std::string out;

    for (int line = 0; line < 2; ++line) {
        unsigned char idx = indexTop + line;
        if (idx >= currentList->size()) break;

        const auto& node = (*currentList)[idx];

        // курсор
        out += (cursorPos == line ? ">" : " ");

        out += node.title;

        if (node.value) {
            out += " ";
            switch (node.type) {
                case USHORT: out += std::to_string(*(unsigned short*)node.value); break;
                case SHORT:  {
                    short v = *(short*)node.value;
                    out += (v >= 0 ? "+" : "") + std::to_string(v);
                } break;
                case FLOAT: {
                    float v = *(float*)node.value;
                    if (v >= 0) out += "+";
                    char buf[16];
                    snprintf(buf, sizeof(buf), "%.2f", v);
                    out += buf;
                } break;
                case BOOL: out += (*(bool*)node.value ? "true" : "false"); break;
                default: break;
            }
            if (node.editable) out += " <";
        }

        out += "\r"; // только CR, без LF
    }

    uartDrv.sendBuffer((const unsigned char*)out.c_str());//, out.size());
}
// Навигация вверх
void CTERMINAL::up() {
    if (cursorPos == 1) {
        cursorPos = 0; // просто перемещаем курсор на верхнюю строку
    } else if (indexTop > 0) {
        indexTop--;    // сдвигаем окно вверх
    }
    render();
}

// Навигация вниз
void CTERMINAL::down() {
    if (cursorPos == 0) {
        cursorPos = 1; // курсор на нижнюю строку
    } else if (indexTop + 2 < currentList->size()) {
        indexTop++;    // сдвигаем окно вниз
    }
    render();
}

void CTERMINAL::enter() {
    auto& node = (*currentList)[selectedIndex];
    if (!node.children.empty()) {
        history.push({currentList, (uint8_t)selectedIndex});
        currentList = &node.children;
        selectedIndex = 0;
    }
}

void CTERMINAL::esc() {
    if (!history.empty()) {
        Frame f = history.top();
        history.pop();
        currentList = f.list;
        selectedIndex = f.index;
    }
}

// Редактирование значения
void CTERMINAL::edit(int delta) {
    auto& node = (*currentList)[selectedIndex];
    if (node.value && node.editable) {
        switch (node.type) {
            case USHORT: *(static_cast<unsigned short*>(node.value)) += delta; break;
            case SHORT:  *(static_cast<short*>(node.value)) += delta; break;
            case FLOAT:  *(static_cast<float*>(node.value)) += delta * 0.1f; break;
            case BOOL:   *(static_cast<bool*>(node.value)) = !*(static_cast<bool*>(node.value)); break;
            default: break;
        }
    }
}

