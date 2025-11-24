#include "i_adc.hpp"
#include "LPC407x_8x_177x_8x.h"
#include "system_LPC177x.h"
#include <math.h>

CIADC::CIADC()
{
  LPC_IOCON->P0_23  = CH_ADC_IOCON;  // ch0 
  LPC_IOCON->P0_24  = CH_ADC_IOCON;  // ch1 
  
  LPC_SC->PCONP |= CLKPWR_PCONP_PCADC; 
  LPC_ADC->CNR = (((PeripheralClock/OSC_CLK) - 1) << SB_CLKDIV ) | PDN;
};

StatusRet CIADC::measure_5V()
{
  //Измерение напряжение питания +5V
  LPC_ADC->CNR &= CLEAR; 
  LPC_ADC->CNR |= ((1UL << CH_P5) | START);
  unsigned int prev_TC0 = LPC_TIM0->TC;
  while (!(LPC_ADC->GDR & DONE))
  {
    if((LPC_TIM0->TC - prev_TC0) > DELAY) 
    {
      P5_A = 0;
      N5_A = 0;
      return Error;
    }
  }
  P5_prev = K_P5 * ((LPC_ADC->DR[CH_P5] >> 4) & 0xFFF);
  P5_A = (P5_A + P5_prev) / 2.0f; 
 
  //Измерение напряжение питания -5V
  LPC_ADC->CNR &= CLEAR;
  LPC_ADC->CNR |= ((1UL << CH_N5) | START);
  prev_TC0 = LPC_TIM0->TC;
  while (!(LPC_ADC->GDR & DONE))
  {
    if((LPC_TIM0->TC - prev_TC0) > DELAY) 
    {
      P5_A = 0;
      N5_A = 0;
      return Error;
    }
  }  
  N5_prev = (K_N5 * ((LPC_ADC->DR[CH_N5] >> 4) & 0xFFF)) - (3 * P5_A);
  N5_A = (N5_A + N5_prev) / 2.0f;
  return Success;
}


