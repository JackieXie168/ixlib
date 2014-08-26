// ----------------------------------------------------------------------------
//  Description      : Reading and writing text files
// ----------------------------------------------------------------------------
//  (c) Copyright 1999 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#ifndef IXLIB_TEXTFILE
#define IXLIB_TEXTFILE




#include <vector>
#include <string>
#include <iostream>




namespace ixion {
  class text_file : public vector<string> {
    public:
    void read(istream &stream);
    void write(ostream &stream) const;
    };
  
  
  
  
  inline istream &operator>>(istream &istr,text_file &conf) {
    conf.read(istr);
    return istr;
    }
  
  
  
  
  inline ostream &operator<<(ostream &ostr,text_file const &conf) {
    conf.write(ostr);
    return ostr;
    }
  }
  
  
  

#endif
