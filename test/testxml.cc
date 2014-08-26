// ----------------------------------------------------------------------------
//  Description      : XML parsing tests.
// ----------------------------------------------------------------------------
//  (c) Copyright 1999 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <fstream>
#include <iostream>
#include <ixlib_xml.hh>
#include "test.hh"
#include <ixlib_string.hh>
#include <vector>




BEGIN_TEST
  vector<xString>	list;
  xString		s;
  
  list.push_back("hallo");
  cout << list.size() << " " << list.back() << endl;
  list.push_back("");
  cout << list.size() << " " << list.back() << endl;

  ifstream istr("test.xml");
  
  xXMLFile xml;
  xml.read(istr);

  ofstream ostr("test.xml.new");
  xml.write(ostr);
END_TEST
