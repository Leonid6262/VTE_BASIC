#include "terminalUartDriver.hpp"

CTerminalUartDriver::CTerminalUartDriver(){};

void CTerminalUartDriver::init(LPC_UART_TypeDef* UART)
{
  this->UART = UART;
  txbuf.head = 0;
  txbuf.tail = 0;
}

CTerminalUartDriver& CTerminalUartDriver::getInstance() 
{
  static CTerminalUartDriver instance;
  return instance;
}

// ------------------------
// Отправка 16 байт
// ------------------------
bool CTerminalUartDriver::sendBuffer(const unsigned char* data) {
    // Проверка свободного места
    unsigned short free = (txbuf.tail + RingBuffer::SIZE - txbuf.head - 1) % RingBuffer::SIZE;
    if (free < 16) return false;

    for (int i = 0; i < 16; i++) {
        txbuf.push(data[i]);
    }

    // Пинок: если THR пуст
    if (UART->LSR & (1 << 5)) { // THRE flag
        unsigned char b;
        if (txbuf.pop(b)) UART->THR = b;
    }

    UART->IER |= (1 << 1); // THRE interrupt enable
    return true;
}

// ------------------------
// Опрос RX
// ------------------------
bool CTerminalUartDriver::poll_rx(unsigned char& byte) {
    if (UART->LSR & (1 << 0)) { // RDR flag
        byte = UART->RBR;
        return true;
    }
    return false;
}

void CTerminalUartDriver::irq_handler() {
    unsigned int iir = UART->IIR;
    if ((iir & 0x02) == 0x02) { // THRE interrupt
        int fifo_space = 16;
        while (fifo_space--) {
            unsigned char b;
            if (txbuf.pop(b)) {
                UART->THR = b;
            } else {
                UART->IER &= ~(1 << 1); // очередь пуста
                break;
            }
        }
    }
}

extern "C" void UART0_IRQHandler(void) {
    CTerminalUartDriver::getInstance().irq_handler();
}

