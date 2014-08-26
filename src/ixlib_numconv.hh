// ----------------------------------------------------------------------------
//  Description      : Numeric conversions
// ----------------------------------------------------------------------------
//  Remarks          : 
//    These used to be part of the string:: class, which was a bit sucky
//    in terms of design.
//
// ----------------------------------------------------------------------------
//  (c) Copyright 1999 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#ifndef IXLIB_NUMCONV
#define IXLIB_NUMCONV




#include <ixlib_base.hh>
#include <ixlib_string.hh>




// Macros ---------------------------------------------------------------------
#define IXLIB_NUMCHARS 		"0123456789ABCDEF"




// Functions ------------------------------------------------------------------
namespace ixion {
  string float2dec(double value,unsigned int precision = 20);
  string unsigned2base(unsigned long value,char digits = 0,char radix = 10);
  inline string unsigned2dec(unsigned long value,char digits = 0)
    { return unsigned2base(value,digits,10); }
  inline string unsigned2hex(unsigned long value,char digits = 0)
    { return unsigned2base(value,digits,16); }
  inline string unsigned2bin(unsigned long value,char digits = 0)
    { return unsigned2base(value,digits,2); }
  inline string unsigned2oct(unsigned long value,char digits = 0)
    { return unsigned2base(value,digits,8); }
  string signed2base(signed long value,char digits = 0,char radix = 10);
  inline string signed2dec(signed long value,char digits = 0)
    { return signed2base(value,digits,10); }
  inline string signed2hex(signed long value,char digits = 0)
    { return signed2base(value,digits,16); }
  inline string signed2bin(signed long value,char digits = 0)
    { return signed2base(value,digits,2); }
  inline string signed2oct(signed long value,char digits = 0)
    { return signed2base(value,digits,8); }
  
  string bytes2dec(TSize bytes);
  
  unsigned long evalNumeral(string const &numeral,unsigned radix = 10);
  double evalFloat(string const &numeral);
  unsigned long evalUnsigned(string const &numeral,unsigned default_base = 10);
  signed long evalSigned(string const &numeral,unsigned default_base = 10);
  }




#endif
