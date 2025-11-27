#pragma once

//Адреcа EXT_RAM (1МБ)
namespace EXT_RAM_ADR 
{
  constexpr unsigned char* RAM_BEGIN   = reinterpret_cast<unsigned char*>(0x80000000);
  constexpr unsigned char* RAM_END     = reinterpret_cast<unsigned char*>(0x800FFFFF);
  constexpr unsigned char* ADR_TEST_55 = reinterpret_cast<unsigned char*>(0x80055555);
  constexpr unsigned char* ADR_TEST_AA = reinterpret_cast<unsigned char*>(0x800AAAAA);  
}

void EMC_Init_Check( void );



