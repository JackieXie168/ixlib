// ----------------------------------------------------------------------------
//  Description      : Regular expressions string object.
// ----------------------------------------------------------------------------
//  Remarks          :
//    matcher::match must set the MatchLength member correctly.
//    Hopefully, one day this will have full perl5 functionality.
//    Currently, you cannot quantify anything that generates backreferences,
//    not even alternatives.
//    Backref numbering starts at \0.
//    If you query anything about the last match, and that last match did
//    never happen, behavior is undefined.
//    regex_string::ReplaceAll does not set the MatchIndex/MatchGlobal
//    members.
//    The fisrt call to regex_string::match() automatically parses the 
//    current regex. Whenever you do any changes to the expression, you
//    have to call regex_string::parse() yourself. (ATTENTION! This gives
//    way for possibly hard-to-track bugs :-)
//
// ----------------------------------------------------------------------------
//  (c) Copyright 1998 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#ifndef IXLIB_RE
#define IXLIB_RE




#include <vector>
#include <ixlib_exgen.hh>
#include <ixlib_string.hh>




// Regex exceptions -----------------------------------------------------------
#define ECRE_INVQUANTIFIER              0
#define ECRE_UNBALBACKREF               1
#define ECRE_INVESCAPE                  2
#define ECRE_INVBACKREF                 3
#define ECRE_UNTERMCLASS                4




namespace ixion {
  class regex_exception : public base_exception {
    public:
    regex_exception(TErrorCode error,
      char const *info = NULL,char *module = NULL,TIndex line = 0);
    virtual char *getText() const;
    };
  }




// Regex token macros ---------------------------------------------------------
#define XSTRRE_LITERAL          '\\'
#define XSTRRE_BACKREF          '\\'
#define XSTRRE_ESCAPESEQ        '\\'
#define XSTRRE_ANYCHAR          '.'
#define XSTRRE_START            '^'
#define XSTRRE_END              '$'
#define XSTRRE_ALTERNATIVE      '|'
#define XSTRRE_CLASSSTART       '['
#define XSTRRE_CLASSNEG         '^'
#define XSTRRE_CLASSRANGE       '-'
#define XSTRRE_CLASSSTOP        ']'

#define XSTRRE_BACKREFSTART     '('
#define XSTRRE_BACKREFSTOP      ')'

#define XSTRREQ_0PLUS           '*'
#define XSTRREQ_1PLUS           '+'
#define XSTRREQ_01              '?'
#define XSTRREQ_START           '{'
#define XSTRREQ_RANGE           ','
#define XSTRREQ_STOP            '}'
#define XSTRREQ_NONGREEDY       '?'




namespace ixion {
  class regex_string : public string {
    // various helper classes ---------------------------------------------------
    protected:
    class backref {
      bool         	Ready;
      TIndex        	At;
      TSize         	Size;
  
      public:
      void checkpoint(TIndex at)
        {
          At = at;
          Ready = false;
        }
      void makeReady(TIndex end)
        {
          Size = end-At;
          Ready = true;
        }
      string get(string const &candidate) const
        {
          if (!Ready) EX_THROW(regex,ECRE_UNBALBACKREF)
          return string(candidate,At,Size);
        }
    };
  
    typedef vector<backref> backref_heap;
  
    class matcher {
      protected:
      matcher         	*Next;
      TSize               MatchLength;
      bool                DeleteNext;
  
      public:
      matcher()
        : Next(NULL)
        { }
      virtual ~matcher()
        { if (Next && DeleteNext) delete Next; }
      TSize getMatchLength() const
        { return MatchLength; }
      TSize collectMatchLength() const;
      matcher *getNext() const
        { return Next; }
      void setNext(matcher *next,bool deletenext = true) {
        Next = next;
        DeleteNext = deletenext;
        }
      virtual bool match(backref_heap &brheap,string const &candidate,TIndex at)
        = 0;
  
      protected:
      bool matchNext(backref_heap &brheap,string const &candidate,TIndex at) const
        { return Next ? Next->match(brheap,candidate,at) : true; }
      };
  
    class quantifier : public matcher {
      bool                 	Greedy,MaxValid;
      TSize                	MinCount,MaxCount;
      matcher         	 	*Quantified;
  
      public:
      quantifier(bool greedy,TSize mincount)
        : Greedy(greedy),MaxValid(false),MinCount(mincount)
        { }
      quantifier(bool greedy,TSize mincount,TSize maxcount)
        : Greedy(greedy),MaxValid(true),MinCount(mincount),MaxCount(maxcount)
        { }
      virtual ~quantifier()
        { if (Quantified) delete Quantified; }
  
      void setQuantified(matcher *quantified)
        { Quantified = quantified; }
      virtual bool match(backref_heap &brheap,string const &candidate,
        TIndex at);
  
      protected:
      bool matchQuantified(backref_heap &brheap,string const &candidate,
        TIndex at)
        { return Quantified ? Quantified->match(brheap,candidate,at) : true; }
      TSize getQuantifiedMatchLength()
        { return Quantified ? Quantified->collectMatchLength() : 0; }
      bool matchCount(backref_heap &brheap,string const &candidate,TIndex at,
        TSize matchcount);
      };
  
    class string_matcher : public matcher {
      string               MatchStr;
  
      public:
      string_matcher(string const &matchstr)
        : MatchStr(matchstr)
        { MatchLength = MatchStr.size(); }
      virtual bool match(backref_heap &brheap,string const &candidate,TIndex at) {
        if (at+MatchStr.size() > candidate.size()) return false;
        return (string(candidate,at,MatchStr.size()) == MatchStr) &&
          matchNext(brheap,candidate,at+MatchStr.size());
        }
      };
  
    class any_char_matcher : public matcher {
      public:
      any_char_matcher()
        { MatchLength = 1; }
      virtual bool match(backref_heap &brheap,string const &candidate,TIndex at)
        { return matchNext(brheap,candidate,at+1); }
      };
  
    class start_matcher : public matcher {
      public:
      start_matcher()
        { MatchLength = 0; }
      virtual bool match(backref_heap &brheap,string const &candidate,TIndex at)
        { return (at == 0) && matchNext(brheap,candidate,at); }
      };
  
    class end_matcher : public matcher {
      public:
      end_matcher()
        { MatchLength = 0; }
      virtual bool match(backref_heap &brheap,string const &candidate,TIndex at)
        { return (at == candidate.size()) && matchNext(brheap,candidate,at); }
      };
  
    class class_matcher : public matcher {
      string CharClass;
      bool Negated;
  
      public:
      class_matcher(string const &charclass)
        : CharClass(charclass),Negated(false) {
        MatchLength = 1;
        if (CharClass.size() && CharClass[0] == XSTRRE_CLASSNEG) {
          CharClass.erase(CharClass.begin());
          Negated = true;
          }
        expandClass();
        }
      virtual bool match(backref_heap &brheap,string const &candidate,TIndex at) {
        try {
          if (at >= candidate.size()) return false;
          return
            (Negated ? !isInClass(candidate[at]) : isInClass(candidate[at])) &&
            matchNext(brheap,candidate,at+1);
          }
        EX_CATCHCODE(generic,EC_INDEX,return false;)
        }
  
      private:
      bool isInClass(char const ch) const
        { return CharClass.find(ch) != string::npos; }
      void expandClass();
      };
  
    class backref_open_matcher : public matcher {
      public:
      backref_open_matcher()
        { MatchLength = 0; }
      virtual bool match(backref_heap &brheap,string const &candidate,TIndex at);
      };
  
    class backref_close_matcher : public matcher {
      public:
      backref_close_matcher()
        { MatchLength = 0; }
      virtual bool match(backref_heap &brheap,string const &candidate,TIndex at);
      };
  
    class alternative_matcher : public matcher {
      // The connector serves two purposes:
      // a) be a null-matcher that re-unites the different alternate token
      //    sequences
      // b) make the end of each sequence identifiable to be able to compute
      //    the match length
    
      class connector : public matcher {
        matcher **pNext;
  
        public:
        connector(matcher **pnext)
          : pNext(pnext)
          { MatchLength = 0; }
        virtual bool match(backref_heap &brheap,string const &candidate,TIndex at) {
          Next = *pNext;
          return matchNext(brheap,candidate,at);
          }
        };
  
      vector<matcher *> 		AltList;
      connector        		Connector;
  
      public:
      alternative_matcher()
        : Connector(&Next)
        { }
      ~alternative_matcher();
      void operator+=(matcher *matcher);
      virtual bool match(backref_heap &brheap,string const &candidate,
        TIndex at);
      };
  
    class backref_matcher : public matcher {
      TIndex Backref;
  
      public:
      backref_matcher(TIndex backref)
        : Backref(backref)
        { }
      virtual bool match(backref_heap &brheap,string const &candidate,
        TIndex at);
      };
  
    // instance data ------------------------------------------------------------
    typedef string       Super;
    matcher		*ParsedRegex;
    backref_heap     	BackrefHeap;
    string               LastCandidate;
    TIndex                MatchIndex;
    TSize                 MatchLength;
  
    // interface ----------------------------------------------------------------
    public:
    regex_string()
      : ParsedRegex(NULL),MatchIndex(0),MatchLength(0)
      { parse(); }
    regex_string(regex_string const &re)
      : Super(re),ParsedRegex(NULL),MatchIndex(0),MatchLength(0)
      { }
    regex_string(Super const &str,size_type pos = 0,size_type n = npos)
      : Super(str,pos,n),ParsedRegex(NULL),MatchIndex(0),MatchLength(0)
      { }
    regex_string(char const *s,size_type n)
      : Super(s,n),ParsedRegex(NULL),MatchIndex(0),MatchLength(0)
      { }
    regex_string(char const *s)
      : Super(s),ParsedRegex(NULL),MatchIndex(0),MatchLength(0)
      { }
    regex_string(size_type n, char c)
      : Super(n,c),ParsedRegex(NULL),MatchIndex(0),MatchLength(0)
      { }
    template<class InputIterator>
    regex_string(InputIterator begin,InputIterator end)
      : Super(begin,end),ParsedRegex(NULL),MatchIndex(0),MatchLength(0)
      { }
    
    ~regex_string()
      { if (ParsedRegex) delete ParsedRegex; }
  
    regex_string &parse()
      {
        matcher *newregex = parseRegex(*this);
        if (!newregex) return *this;
        if (ParsedRegex) delete ParsedRegex;
        ParsedRegex = newregex;
        return *this;
      }
    bool match(string const &candidate,TIndex from = 0);
    bool matchAt(string const &candidate,TIndex at = 0);
    string replaceAll(string const &candidate,string const &replacement,
      TIndex from = 0);
  
    // Queries pertaining to the last match
    TIndex getMatchIndex()
      { return MatchIndex; }
    TSize getMatchLength()
      { return MatchLength; }
    string getMatch()
      { return LastCandidate.substr(MatchIndex,MatchLength); }
    string getBackref(TIndex index)
      { return BackrefHeap[index].get(LastCandidate); }
  
    protected:
    matcher *parseRegex(string const &expr);
    quantifier *parseQuantifier(string const &expr,TIndex &at);
    bool isGreedy(string const &expr,TIndex &at);
    };
  }



#endif

