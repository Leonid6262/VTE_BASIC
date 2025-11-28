#pragma once

#include "LPC407x_8x_177x_8x.h"

class CTERMINAL{ 
  
private:
  
  LPC_UART_TypeDef* UART;
  
public:
  
  CTERMINAL(LPC_UART_TypeDef*);
  
  void basic();
  
};
