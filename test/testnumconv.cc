// ----------------------------------------------------------------------------
//  Description      : numeric conversion tests.
// ----------------------------------------------------------------------------
//  (c) Copyright 1998 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <ixlib_numconv.hh>
#include "test.hh"




BEGIN_TEST
  TEST(numcUnsigned2Dec(239434) == "239434")
  TEST(numcUnsigned2Hex(0xAFFE) == "AFFE")
  TEST(numcSigned2Bin(-0xAA) == "-10101010")
  TEST(numcBytes2Dec(1024*1024) == "1024 KB")
  TEST(numcEvalUnsigned("0xaffe") == 0xaffe);
  TEST(numcEvalSigned("-1010b") == -0xa);
END_TEST
