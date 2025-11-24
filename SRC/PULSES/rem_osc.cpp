#include "rem_osc.hpp"
#include "pause_us.hpp"
#include "settings_eep.hpp"
#include "system_LPC177x.h"
#include <stdio.h>

// Старт передачи по DMA
void CREM_OSC::start_dma_transfer()
{
  // Старт чтения
  rContDMA.StartRxTransfer(CDMAcontroller::ChannelMap::SPI2_Rx_Channel, 
                           static_cast<unsigned int>((long long)rx_dma_buffer),                           
                           TRANSACTION_LENGTH);
  // Старт записи
  rContDMA.StartTxTransfer(CDMAcontroller::ChannelMap::SPI2_Tx_Channel, 
                           static_cast<unsigned int>((long long)tx_dma_buffer),                           
                           TRANSACTION_LENGTH);
}

/* Конфигурирование используемых каналов DMA (карту каналов с.м. в controllerDMA.hpp) */
/* BurstSize = 4. При EWidth::Short → 16 бит → 4 × 2 байта = 8 байт, при FIFO = 16 байт, с запасом */
void CREM_OSC::init_dma()
{
  // Конфигурация канала записи
  CDMAcontroller::SChannelConfig cfg_ch_tx
  {
    CDMAcontroller::ChannelMap::SPI2_Tx_Channel,  // Номер канала
    CDMAcontroller::ETransferType::TYPE_M2P,      // Тип канала
    CDMAcontroller::EConnNumber::SSP2_Tx,         // Номер периферийного подключения
    CDMAcontroller::DmaBurst::SIZE_4,             // Количество единичных элементов транзакции 
    CDMAcontroller::EWidth::SHORT,                // Размер единичного элемента
    OFF                                           // Разрешение/запрет события окончания передачи (ON/OFF)
  };
  
  rContDMA.init_M2P2M_Channel(&cfg_ch_tx); 
  
  // Конфигурация канала чтения
  CDMAcontroller::SChannelConfig cfg_ch_rx
  {
    CDMAcontroller::ChannelMap::SPI2_Rx_Channel,  // Номер канала
    CDMAcontroller::ETransferType::TYPE_P2M,      // Тип канала
    CDMAcontroller::EConnNumber::SSP2_Rx,         // Номер периферийного подключения
    CDMAcontroller::DmaBurst::SIZE_4,             // Количество единичных элементов транзакции
    CDMAcontroller::EWidth::SHORT,                // Размер единичного элемента
    OFF                                           // Разрешение/запрет события окончания приёма (ON/OFF)
  };
  
  rContDMA.init_M2P2M_Channel(&cfg_ch_rx); 

}

// Передача данных треков
void CREM_OSC::send_data()
{
  if(!StatusESP32) return;
  
/*  ip_ap_sta[0] = rx_dma_buffer[1];
  ip_ap_sta[1] = rx_dma_buffer[2];
  ip_ap_sta[2] = rx_dma_buffer[3];
  ip_ap_sta[3] = rx_dma_buffer[4];*/
  
  memset(rx_dma_buffer, 0, TRANSACTION_LENGTH*2);  
  tx_dma_buffer[0] = send_TRACKS; 
  for(unsigned char n_data = 1; n_data < number_actual_tracks + 1; n_data++)
  {
    tx_dma_buffer[n_data] = *set_init.pData[n_data - 1];
  }
  start_dma_transfer();
}

// Получение актуального количества треков
unsigned char CREM_OSC::get_actual_number()
{
  unsigned char count = 0;
  while ((count < NUMBER_TRACKS) && set_init.d_100p[count]) 
  {
    ++count;
  }
  return count;
}

// Передача в ESP32 коэффициентов отображения  
void CREM_OSC::transfer_disp_c()
{
  memset(tx_dma_buffer, 0, TRANSACTION_LENGTH*2);
  memset(rx_dma_buffer, 0, TRANSACTION_LENGTH*2);
  tx_dma_buffer[0]  = send_CIND;    
  for (unsigned short i = 1; i < NUMBER_TRACKS + 1; ++i)
  {
    tx_dma_buffer[i] = set_init.d_100p[i - 1];
  }
  start_dma_transfer();
  Pause_us(6000);
  start_dma_transfer();
  Pause_us(6000);
  start_dma_transfer();
  Pause_us(6000);
}

// Передача в ESP32 имён треков. Два имени за одну транзакцию
void CREM_OSC::transfer_name()
{
  unsigned char position;
  unsigned char track = 0;
  unsigned char HALF_LENGTH = ((TRANSACTION_LENGTH - 1) / 2 ) + 1;  //6 при длине TRANSACTION_LENGHT = 11
  for(unsigned short nt = 0; nt < (NUMBER_TRACKS / 2); nt++)
  {    
    memset(tx_dma_buffer, 0, TRANSACTION_LENGTH*2);
    memset(rx_dma_buffer, 0, TRANSACTION_LENGTH*2);
    
    position = 0;
    tx_dma_buffer[position++] = NAME_CODES[nt];   
    
    for(unsigned short nch = 0; nch < (NAME_LENGTH - 1) && set_init.Names[track][nch]; nch++)
    {
      tx_dma_buffer[position++] = set_init.Names[track][nch];
    }
    position = HALF_LENGTH;
    track++;
    for(short nch = 0; nch < (NAME_LENGTH - 1) && set_init.Names[track][nch]; nch++)
    {
      tx_dma_buffer[position++]  = set_init.Names[track][nch];
    }
    track++;
    start_dma_transfer();
    Pause_us(6000);
    start_dma_transfer();
    Pause_us(6000);
    start_dma_transfer();
    Pause_us(6000);
  }
}

// Определение режима работы и готовности ESP32 
void CREM_OSC::transfer_mode()
{
  StatusRet Ret;
  switch(set_init.mode)
  {
  case Operating_mode::Access_point:
    Ret = transfer_SN_ID();
    break;
  case Operating_mode::Station:
    Ret = transfer_SSID();
    if(Ret == Success)
    {
      Ret = transfer_Password();
    }    
    break;
  }
  memset(tx_dma_buffer, 0, TRANSACTION_LENGTH*2);
  memset(rx_dma_buffer, 0, TRANSACTION_LENGTH*2);
  StatusESP32 = static_cast<bool>(Ret);
}

// Передача в ESP32 двух последних цифр SN_ID
StatusRet CREM_OSC::transfer_SN_ID()
{
  memset(tx_dma_buffer, 0, TRANSACTION_LENGTH*2);
  memset(rx_dma_buffer, 0, TRANSACTION_LENGTH*2);
  tx_dma_buffer[0]  = send_SNID;
  char formVar[4];
  sprintf(formVar, "%02u", set_init.SNboard_number % 100);
  tx_dma_buffer[1] = formVar[0];
  tx_dma_buffer[2] = formVar[1];
  start_dma_transfer();
  Pause_us(6000);
  for(short n = 1; n < n_repeat; n++) 
  {
    memset(rx_dma_buffer, 0, TRANSACTION_LENGTH*2);
    start_dma_transfer();
    Pause_us(6000);    
  }
  if(echo_check() == Success)
  {
    return Success;
  }
  return Error;
}

// Передача в ESP32 SSID
StatusRet CREM_OSC::transfer_SSID()
{
  memset(tx_dma_buffer, 0, TRANSACTION_LENGTH*2);
  memset(rx_dma_buffer, 0, TRANSACTION_LENGTH*2);
  tx_dma_buffer[0]  = send_SSID;
  pack_chars(set_init.pSSID);
  start_dma_transfer();
  Pause_us(6000);
  for(short n = 1; n < n_repeat; n++) 
  {
    memset(rx_dma_buffer, 0, TRANSACTION_LENGTH*2);
    start_dma_transfer();
    Pause_us(6000);
    if(echo_check() == Success)
    {
      return Success;
    }
  }
  return Error;
}

// Передача в ESP32 Пароля
StatusRet CREM_OSC::transfer_Password()
{
  memset(tx_dma_buffer, 0, TRANSACTION_LENGTH*2);
  memset(rx_dma_buffer, 0, TRANSACTION_LENGTH*2);
  tx_dma_buffer[0]  = send_PASS;
  pack_chars(set_init.pPassword);
  start_dma_transfer();
  Pause_us(6000);
  for(short n = 1; n < n_repeat; n++) 
  {
    memset(rx_dma_buffer, 0, TRANSACTION_LENGTH*2);
    start_dma_transfer();
    Pause_us(6000);
    if(echo_check() == Success)
    {
      return Success;
    }
  }
  return Error;
}

// Контроль эха
StatusRet CREM_OSC::echo_check()
{  
  for(short n = 0; n < TRANSACTION_LENGTH;  n++)
  {
    if(rx_dma_buffer[n] != tx_dma_buffer[n])
    {
      return Error;
    }
  }
  return Success;
}

// Упаковка строки
void CREM_OSC::pack_chars(unsigned char* str)
{
  unsigned char position = 1;
  for(short ch = 0; (ch < G_CONST::SSID_PS_L) && str[ch]; ch += 2)
  {
    tx_dma_buffer[position++] = (str[ch] << 8) | str[ch + 1]; 
  }
}

// Конструктор
CREM_OSC::CREM_OSC(CDMAcontroller& rContDMA, SSET_init& set_init) : rContDMA(rContDMA), set_init(set_init)
{   
  init_SPI();
  init_dma();
  number_actual_tracks = get_actual_number();
  transfer_disp_c();
  transfer_name();
  transfer_mode();
}

// Инициализация SPI
void CREM_OSC::init_SPI()
{
  LPC_IOCON->P5_0  = IOCON_SPI;       //MOSI
  LPC_IOCON->P5_1  = IOCON_SPI;       //MISO
  LPC_IOCON->P5_2  = IOCON_SPI;       //SCK 
  LPC_IOCON->P1_27 = IOCON_PORT;      //Резервный вход/выход
  
  LPC_IOCON->P5_3 = IOCON_PORT;       //Програмный CS  
  LPC_GPIO5->DIR |= PROG_CS;
  LPC_GPIO5->CLR  = PROG_CS;
  
  // Используется SPI-2
  LPC_SC->PCONP |= CLKPWR_PCONP_PCSSP2;
  LPC_SSP2->CR0 = 0;
  LPC_SSP2->CR0 = SPI_Config::CR0_DSS(bits_tr);// | CR0_FRF_SPI;  | CR0_CPOL_HI; | CR0_CPHA_SECOND; 
  LPC_SSP2->CR1 = 0;
  SPI_Config::set_spi_clock(LPC_SSP2, Hz_SPI, PeripheralClock );
  LPC_SSP2->CR1 |= SPI_Config::CR1_SSP_EN ;
  LPC_SSP2->DMACR = TXDMAE | RXDMAE; // Приём и передача по DMA
  
}

