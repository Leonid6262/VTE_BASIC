#include "spi.hpp"

CSPI::CSPI(ESPIInstance SN) 
{   
  switch(SN)
  {
  case ESPIInstance::SPI_PORTS:
    SPI_N = LPC_SSP0; 
    LPC_IOCON->P0_15  = IOCON_SPI0;       // SCK0
    LPC_IOCON->P0_17  = IOCON_SPI0;       // MISO0
    LPC_IOCON->P0_18  = IOCON_SPI0;       // MOSI0
    LPC_SC->PCONP |= CLKPWR_PCONP_PCSSP0; // Включение питания SPI0 
    /* Настройка частоты */
    
    break;
  case ESPIInstance::SPI_E_ADC:   
    SPI_N = LPC_SSP1; 
    LPC_IOCON->P4_20 = D_MODE_PULLUP | IOCON_SPI1; // SCK1 
    LPC_IOCON->P4_21 = D_MODE_PULLUP | IOCON_SPI1; // SSEL1
    LPC_IOCON->P4_22 = D_MODE_PULLUP | IOCON_SPI1; // MISO1
    LPC_IOCON->P4_23 = D_MODE_PULLUP | IOCON_SPI1; // MOSI1    
    LPC_SC->PCONP |= CLKPWR_PCONP_PCSSP1;          // Включение питания SPI1    
    /* Настройка частоты */
    
    break;
  case ESPIInstance::SPI_ESP32:     
    SPI_N = LPC_SSP2; 
    LPC_IOCON->P5_0  = IOCON_SPI2;              //MOSI
    LPC_IOCON->P5_1  = IOCON_SPI2;              //MISO
    LPC_IOCON->P5_2  = IOCON_SPI2;              //SCK 
    LPC_SC->PCONP |= CLKPWR_PCONP_PCSSP2;       // Включение питания SPI2
    /* Настройка частоты */
    
    break;
  }  
}







