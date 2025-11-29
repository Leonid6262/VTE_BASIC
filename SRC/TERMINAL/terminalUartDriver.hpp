#pragma once

#include "LPC407x_8x_177x_8x.h"

class CTerminalUartDriver
{
public:
  
  static CTerminalUartDriver& getInstance(); 
  
  // Отправка ровно 16 байт (возвращает false если очередь переполнена)
  bool sendBuffer(const uint8_t* data);
  
  // Опрос приёма одного байта (возвращает true если байт есть)
  bool poll_rx(uint8_t& byte);

  void irq_handler(); 
  void init(LPC_UART_TypeDef*);
  
private:
  
  struct RingBuffer {
    static constexpr unsigned short SIZE = 128;
    unsigned char buf[SIZE];
    unsigned short head = 0;
    unsigned short tail = 0;
    
    bool empty() const { return head == tail; }
    bool full()  const { return ((head + 1) % SIZE) == tail; }
    
    void push(unsigned char b) {
      if (!full()) {
        buf[head] = b;
        head = (head + 1) % SIZE;
      }
    }
    
    bool pop(unsigned char& b) {
      if (!empty()) {
        b = buf[tail];
        tail = (tail + 1) % SIZE;
        return true;
      }
      return false;
    }
  };
  
  RingBuffer txbuf;
  
  LPC_UART_TypeDef* UART;
  
  CTerminalUartDriver(); 
  CTerminalUartDriver(const CTerminalUartDriver&) = delete;
  CTerminalUartDriver& operator=(const CTerminalUartDriver&) = delete; 
  
};