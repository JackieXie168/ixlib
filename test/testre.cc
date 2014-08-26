// ----------------------------------------------------------------------------
//  Description      : Regular expressions tests.
// ----------------------------------------------------------------------------
//  (c) Copyright 1998 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <ixlib_re.hh>
#include "test.hh"




using namespace ixion;




string duplicate(string const &src,TSize count) {
  string result;
  while (count--) result += src;
  return result;
  }




BEGIN_TEST
  regex_string re;

  re = "duh";
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
  TEST(re.match(duplicate("dihhh daaah duuuuuuuuuuuuh doh",10)))
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
    
  re = "du+h";
  TEST(re.match(duplicate("dihhh daaah duuuuuuuuuuuuh doh",90000)))

  re = "1(\\D*)2+";
  TEST(re.replaceAll("1i222 1aaa2 1uuuuuuuuuuuu2 1o2","2\\01")
    == "2i1 2aaa1 2uuuuuuuuuuuu1 2o1");

  re = "ver(gess|scheiss|acht|knall)+en";
  TEST(re.match("verknallen"))
  TEST(re.match("verknallscheissen"))
  TEST(re.match("verknallgessscheissen"))
  TEST(!re.match("veren"))
  TEST(!re.match("vergesserachten"))
  
  re = "gun((\\d)+down(\\d)+)?jimbo";
  TEST(re.match("gun123down456jimbo"))
  TEST(re.countBackrefs() == 7)
  TEST(re.match("gunjimbo"))
  
  // todo: nested backrefs
END_TEST
