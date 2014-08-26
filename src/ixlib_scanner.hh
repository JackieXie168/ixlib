// ----------------------------------------------------------------------------
//  Description      : scanner for xTextFile
// ----------------------------------------------------------------------------
//  Remarks          :
//    - if more than one TokenRule match, the one that has the lowes
//      index in the TokenRuleTable is valid.
//
// ----------------------------------------------------------------------------
//  (c) Copyright 1999 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#ifndef IXLIB_SCANNER
#define IXLIB_SCANNER




#include <ixlib_base.hh>
#include <ixlib_exbase.hh>
#include <vector>
#include <ixlib_string.hh>
#include <FlexLexer.h>




// possible errors during execution -------------------------------------------
#define ECSCAN_UNKNOWN_TOKEN      	0
#define ECSCAN_EOF		      	1




// scanner_exception ----------------------------------------------------------
namespace ixion {
  struct scanner_exception : public base_exception {
    scanner_exception(TErrorCode const error, TIndex const line, string const &info);
    virtual char *getText() const;
    };




// scanner --------------------------------------------------------------------
  class scanner {
    public:
      struct token {
        TIndex        Type;
        TIndex        Line;
        string       Text;
        };
  
      typedef vector<token>		token_list;
      typedef vector<token>::iterator	token_iterator;
  
      scanner(FlexLexer &lexer);
    
      bool reachedEOF() const;
      token getNextToken();
      token_list scan();
    
    protected:
      FlexLexer	&Lexer;
      token	CurrentToken;	
    };
  }




#endif