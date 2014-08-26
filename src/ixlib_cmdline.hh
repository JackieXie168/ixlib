// ----------------------------------------------------------------------------
//  Description      : Command line handling.
// ----------------------------------------------------------------------------
//  Remarks          :
//    The mask match might be replaced by regexes in the future.
//
// ----------------------------------------------------------------------------
//  (c) Copyright 1997 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#ifndef IXLIB_CMDLINE
#define IXLIB_CMDLINE




#include <vector>
#include <ixlib_string.hh>




namespace ixion {
  class command_line {
    vector<string>       Parameters;
    string               Command;
  
    public:
    command_line(int argc = 0,char *argv[] = NULL);
  
    void extend(string const &par);
  
    TSize operator~() const
      { return Parameters.size(); }
    string operator[](TIndex no) const
      { return Parameters[no]; }
  
    string getCommand() const
      { return Command; }
  
    TSize count(string const &mask);
    string get(string const &mask,TIndex matchno = 0);
  
    private:
    static bool isHook(string const &text)
      { return text[0] == '@'; }
    static bool matchMask(string const &mask,string const &par);
    };
  }



#endif
