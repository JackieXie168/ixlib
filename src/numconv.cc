// ----------------------------------------------------------------------------
//  Description      : Numeric conversions
// ----------------------------------------------------------------------------
//  (c) Copyright 1999 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <ixlib_exgen.hh>
#include <ixlib_numconv.hh>
#include <stdio.h>




using namespace ixion;




// Data objects --------------------------------------------------------------
static string numChars = IXLIB_NUMCHARS;




// Helper ---------------------------------------------------------------------
string ixion::float2dec(double value, unsigned int precision) {
  char buf[precision+10];
  string cmd("%.");
  cmd += unsigned2dec(precision) + "e";
  sprintf((char *)&buf,cmd.c_str(),value);
  return string(buf);
  }




static unsigned long evaluate(string const &numstr,char radix = 10) {
  if (numstr.size() == 0) return 0;
  unsigned long value = 0, mulvalue = 1;
  TIndex index = numstr.size()-1;

  do {
    unsigned long digvalue;
    try {
      digvalue = numChars.find(numstr[index]);
      }
    catch (...) { EXGEN_THROWINFO(EC_CANNOTEVALUATE,numstr.c_str()) }
    value += mulvalue * digvalue;
    mulvalue *= radix;
  } while (index--);

  return value;
  }




// Exported subroutines -------------------------------------------------------
string ixion::unsigned2base(unsigned long value,char digits,char radix) {
  string temp;
  do {
    temp = numChars[value % radix]+temp;
    value /= radix;
    if (digits) digits--;
  } while (value || digits);
  return temp;
  }




string ixion::signed2base(signed long value,char digits,char radix) {
  if (value < 0) return "-"+unsigned2base(-value,digits,radix);
  else return unsigned2base(value,digits,radix);
  }




string ixion::bytes2dec(TSize bytes) {
  if (bytes>(TSize) 10*1024*1024)
    return unsigned2dec(bytes / ((TSize) 1024*1024))+" MB";
  if (bytes>(TSize) 10*1024)
    return unsigned2dec(bytes / ((TSize) 1024))+" KB";
  return unsigned2dec(bytes)+" Byte";
  }




double ixion::evalFloat(string const &value) {
  double result;
  int count = sscanf(value.c_str(), "%le", &result);
  if (count==0) EXGEN_THROWINFO(EC_CANNOTEVALUATE,value.c_str())
  else return result;
  }




unsigned long ixion::evalUnsigned(string const &value) {
  if (value.size() == 0) return 0;
  string numstr = upper(value);

  if (numstr.substr(0,2) == "0X")
    return evaluate(numstr.substr(2),0x10);
  if (numstr.substr(0,1) == "$")
    return evaluate(numstr.substr(1),0x10);

  char lastchar = numstr[numstr.size()-1];
  if (lastchar == 'H') return evaluate(numstr.substr(0,numstr.size()-1),0x10);
  if (lastchar == 'B') return evaluate(numstr.substr(0,numstr.size()-1),2);
  if (lastchar == 'D') return evaluate(numstr.substr(0,numstr.size()-1),10);
  if (lastchar == 'O') return evaluate(numstr.substr(0,numstr.size()-1),8);

  return evaluate(numstr);
  }




signed long ixion::evalSigned(string const &value) {
  if (value.size() == 0) return 0;
  if (value[0] == '-')
    return -evalUnsigned(value.substr(1));
  else {
    if (value[0] == '+')
      return evalUnsigned(value.substr(1));
    else
      return evalUnsigned(value);
    }
  }
