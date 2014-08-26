// ----------------------------------------------------------------------------
//  Description      : String object
// ----------------------------------------------------------------------------
//  Remarks          : none.
//
// ----------------------------------------------------------------------------
//  (c) Copyright 1999 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#ifndef IXLIB_STRING
#define IXLIB_STRING




#include <string>
#include <ixlib_base.hh>
#include <ixlib_exgen.hh>




namespace ixion {
  template<class InputIterator>
  inline string concat(InputIterator first,InputIterator last,string const &sep = " ") {
    string str;
    while (first != last) {
      if (str.size()) str += sep;
      str += *first++;
      }
    return str;
    }
  
  
  
  
  string findReplace(string const &target,string const &src,string const &dest);
  string findReplace(string const &target,char* src,char *dest);
  string findReplace(string const &target,char src,char dest);
  string upper(string const &original);
  string lower(string const &original);
  string removeLeading(string const &original,char ch = ' ');
  string removeTrailing(string const &original,char ch = ' ');
  string removeLeadingTrailing(string const &original,char ch = ' ');
  string parseCEscapes(string const &original);




  class string_hash {
    public:
      unsigned long operator()(string const &str) const;
    };
  }



#endif
