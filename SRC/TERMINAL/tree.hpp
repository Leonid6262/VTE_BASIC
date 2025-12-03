#pragma once
#include "terminal.hpp"


unsigned short test_var1 = 34561;
unsigned short test_var2 = 230;


inline std::vector<CTERMINAL::MenuNode> makeMENU()
{
    using Node = CTERMINAL::MenuNode;

std::vector<Node> MENU_UTF = {
        Node("ИНДИКАЦИЯ", {
            Node("АНАЛОГОВАЯ", {
                Node("TVAR1", {}, &test_var1),
                Node("TVAR2", {}, &test_var2)
            }),
            Node("ДИСКРЕТНАЯ")
        }),
        Node("УСТАВКИ", {
            Node("РЕГУЛЯТОРОВ", {
                Node("ТОКА"),
                Node("COS PHI"),
                Node("Q МОЩНОСТИ")
            }),
            Node("ОГРАНИЧЕНИЙ"),
            Node("АВАРИЙНЫЕ")
        }),
        Node("РУЧНОЙ РЕЖИМ"),
        Node("УСТАНОВКА ЧАСОВ")
    };

    return MENU_UTF;
}