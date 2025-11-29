#include "terminal.hpp"

CTERMINAL::CTERMINAL(LPC_UART_TypeDef* UART) : UART(UART){}

// Конструкторы узла
CTERMINAL::MenuNode::MenuNode(const std::string& t) : title(t) {}
CTERMINAL::MenuNode::MenuNode(const std::string& t, void* v, VarType vt, bool edit)
    : title(t), value(v), type(vt), editable(edit) {}

// Конструктор терминала
CTERMINAL::CTERMINAL(unsigned short* Irotor, unsigned short* Urotor,
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

// Отображение меню
void CTERMINAL::render() const {
    std::cout << "=== MENU ===\n";
    for (size_t i = 0; i < currentList->size(); ++i) {
        const auto& node = (*currentList)[i];
        std::cout << (i == selectedIndex ? " > " : "   ")
                  << node.title;

        if (node.value) {
            std::cout << " = ";
            switch (node.type) {
                case USHORT: std::cout << *(static_cast<unsigned short*>(node.value)); break;
                case SHORT:  std::cout << *(static_cast<short*>(node.value)); break;
                case FLOAT:  std::cout << *(static_cast<float*>(node.value)); break;
                case BOOL:   std::cout << (*(static_cast<bool*>(node.value)) ? "true" : "false"); break;
                default: break;
            }
            if (node.editable) std::cout << " [edit]";
        }
        std::cout << "\n";
    }
    std::cout << "============\n";
}

// Навигация
void CTERMINAL::up() {
    if (selectedIndex > 0) selectedIndex--;
}

void CTERMINAL::down() {
    if (selectedIndex + 1 < currentList->size()) selectedIndex++;
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





void CTERMINAL::basic()
{
  
}