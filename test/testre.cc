// ----------------------------------------------------------------------------
//  Description      : Regular expressions tests.
// ----------------------------------------------------------------------------
//  (c) Copyright 1998 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <ixlib_re.hh>
#include "test.hh"




BEGIN_TEST
  xRegexString re("duh");
  TEST(re.match("Duh dah duh doh"))
  TEST(re.match("Duh dah doh duh"))
  TEST(re.match("duh dah doh deh"))
  TEST(!re.match("Duh dah deh doh"))
  
  re = "d\\.h";
  TEST(!re.match("dih doh"))
  TEST(re.match("dih d.h doh"))
  
  re = "du*h";
  TEST(re.match("dihhh daaah dh doh"))
  TEST(re.match("dihhh daaah duuuuuuuuuuuuh doh"))

  re = "du+h";
  TEST(!re.match("dihhh daaah dh doh"))
  TEST(re.match("dihhh daaah duuuuuuuuuuuuh doh"))
  TEST(re.getMatch() == "duuuuuuuuuuuuh")

  re = "du+?";
  TEST(re.match("dihhh daaah duuuuuu doh"))
  TEST(re.getMatch() == "du")
  
  re = "d(u+)";
  TEST(re.match("dihhh daaah duuuuuu doh"))
  TEST(re.getBackref(0) == "uuuuuu")
  
  re = "d(a*|e+)gblad";
  TEST(re.match("de allgemein daaagblad"))
  TEST(re.match("de allgemein deegblad"))
  TEST(!re.match("de allgemein duugblad"))
  
  re = "diz([^a-z]*)ba";
  TEST(!re.match("cadiz123bou"))
  TEST(re.match("cadiz1945battle"))
  TEST(re.getBackref(0) == "1945")
  
  re = "^cadiz$";
  TEST(!re.match("whatcadiz"))
  TEST(!re.match("cadizwhat"))
  TEST(re.match("cadiz"))
END_TEST
