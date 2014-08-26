// ----------------------------------------------------------------------------
//  Description      : Command line parsing tests tests.
// ----------------------------------------------------------------------------
//  (c) Copyright 1998 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <iostream>
#include <fstream>
#include <ixlib_textfile.hh>
#include "test.hh"




using namespace ixion;




BEGIN_TEST
  ifstream input("src.txt");
  ofstream output("dst.txt");
  text_file textfile;
  
  input >> textfile;
  output << textfile;
END_TEST

