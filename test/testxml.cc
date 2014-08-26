// ----------------------------------------------------------------------------
//  Description      : XML parsing tests.
// ----------------------------------------------------------------------------
//  (c) Copyright 1999 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <fstream>
#include <iostream>
#include <vector>
#include <ixlib_xml.hh>
#include <ixlib_string.hh>
#include "test.hh"




using namespace ixion;
BEGIN_TEST
  vector<string>	list;
  string		s;
  
  list.push_back("hallo");
  cout << list.size() << " " << list.back() << endl;
  list.push_back("");
  cout << list.size() << " " << list.back() << endl;

  ifstream istr("test.xml");
  
  xml_file xml;
  xml.read(istr);

  ofstream ostr("test.xml.new");
  xml.write(ostr);
END_TEST
