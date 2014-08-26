// ----------------------------------------------------------------------------
//  Description      : Regular expressions string object
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
    regex_exception(TErrorCode error,char const *info = NULL,char *module = NULL,TIndex line = 0);
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
  /**
  A class implementing a regular expression matcher.

  Backref numbering starts at \0.

  If you query anything about the last match, and that last match did
  never happen, behavior is undefined.

  regex_string::ReplaceAll does not set the MatchIndex/MatchGlobal
  members.

  The first call to regex_string::match() automatically parses the 
  current regex. Whenever you do any changes to the expression, you
  have to call regex_string::parse() yourself. (ATTENTION! This gives
  way for possibly hard-to-track bugs :-)

  What is there is compatible with perl5. However, not everything is
  there. Here's what's missing:
  
  <ul>
    <li> \Q-\E,\b,\B,\A,\Z,\z
    <li> discerning between line and string
    <li> (?#comments)
    <li> (?:clustering)
    <li> (?=positive lookahead assumptions)
    <li> (?!negative lookahead assumptions
    <li> (?<=positive lookbehind assumptions)
    <li> (?<!negative lookbehind assumptions
    <li> (?>independent substrings)
    <li> modifiers such as "case independent"
    </ul>
    
  as well as all the stuff involving perl code, naturally.
  None of these is actually hard to hack in. If you want them,
  pester me or try for yourself (and submit patches!)
  */
  class regex_string : public string {
    // various helper classes ---------------------------------------------------
    private:
      class backref_stack {
        private:
          struct backref_entry {
            enum { OPEN,CLOSE }           Type;
            TIndex                        Index;
            };
          
          typedef vector<backref_entry>   internal_stack;
          
          internal_stack                  Stack;
        public:
          typedef TSize                   rewind_info;
          
          void open(TIndex index);
          void close(TIndex index);
          
          rewind_info getRewindInfo() const;
          void rewind(rewind_info ri);
          void clear();
          
          TSize count();
          string get(TIndex number,string const &candidate) const;
        };
          
    
      
      
      
      class matcher {
        protected:
          matcher                   *Next;
          TSize                     MatchLength;
          bool                      DeleteNext;
      
        public:
          matcher();
          virtual ~matcher();
          
          TSize getMatchLength() const { 
            return MatchLength; 
            }
          TSize subsequentMatchLength() const;
          virtual TSize minimumMatchLength() const = 0;
          TSize minimumSubsequentMatchLength() const;
  
          matcher *getNext() const { 
            return Next; 
            }
          virtual void setNext(matcher *next,bool deletenext = true) {
            Next = next;
            DeleteNext = deletenext;
            }
          
	  // this routine must set the MatchLength member correctly.
	  virtual bool match(backref_stack &brstack,string const &candidate,TIndex at)
            = 0;
      
        protected:
          bool matchNext(backref_stack &brstack,string const &candidate,TIndex at) const { 
            return Next ? Next->match(brstack,candidate,at) : true; 
            }
        };
    
  
  
  
      class quantifier : public matcher {
        private:
          bool                      Greedy,MaxValid;
          TSize                     MinCount,MaxCount;
          matcher                   *Quantified;
    
        struct backtrack_stack_entry {
          TIndex                          Index;
          backref_stack::rewind_info      RewindInfo;
          };
  
        public:
          quantifier(bool greedy,TSize mincount);
          quantifier(bool greedy,TSize mincount,TSize maxcount);
          ~quantifier();
      
          TSize minimumMatchLength() const;
          
          void setQuantified(matcher *quantified) { 
            Quantified = quantified;
            }
          bool match(backref_stack &brstack,string const &candidate,TIndex at);
        };
    
  
  
  
      class string_matcher : public matcher {
          string               MatchStr;
      
        public:
          string_matcher(string const &matchstr);
          TSize minimumMatchLength() const {
            return MatchStr.size();
            }
          bool match(backref_stack &brstack,string const &candidate,TIndex at);
        };
    
  
  
  
      class any_char_matcher : public matcher {
        public:
          any_char_matcher() { 
            MatchLength = 1; 
            }
          TSize minimumMatchLength() const {
            return 1;
            }
          bool match(backref_stack &brstack,string const &candidate,TIndex at) { 
            return at < candidate.size() && matchNext(brstack,candidate,at+1); 
            }
        };
    
  
  
  
      class start_matcher : public matcher {
        public:
          start_matcher() { 
            MatchLength = 0; 
            }
          TSize minimumMatchLength() const {
            return 0;
            }
          bool match(backref_stack &brstack,string const &candidate,TIndex at) { 
            return (at == 0) && matchNext(brstack,candidate,at);
            }
        };
    
  
  
  
      class end_matcher : public matcher {
        public:
          end_matcher() { 
            MatchLength = 0;
            }
          TSize minimumMatchLength() const {
            return 0;
            }
          bool match(backref_stack &brstack,string const &candidate,TIndex at) { 
            return (at == candidate.size()) && matchNext(brstack,candidate,at);
            }
        };
    
  
  
  
      class class_matcher : public matcher {
        private:
          static TSize const      CharValues = 256;
          bool                    Set[CharValues];
          bool                    Negated;
    
        public:
          class_matcher(string const &cls);
          TSize minimumMatchLength() const {
            return 1;
            }
          bool match(backref_stack &brstack,string const &candidate,TIndex at);
      
        private:
          void expandClass(string const &cls);
        };
    
      
      
      
      class special_class_matcher : public matcher {
        public:
          enum type { DIGIT,NONDIGIT,ALNUM,NONALNUM,SPACE,NONSPACE };
  
        private:
          type		Type;
  
        public:
          special_class_matcher(type tp);
          TSize minimumMatchLength() const {
            return 1;
            }
          bool match(backref_stack &brstack,string const &candidate,TIndex at);
        };
  
  
  
  
      class backref_open_matcher : public matcher {
        public:
          backref_open_matcher() { 
            MatchLength = 0; 
            }
          TSize minimumMatchLength() const {
            return 0;
            }
          bool match(backref_stack &brstack,string const &candidate,TIndex at);
        };
    
  
  
  
      class backref_close_matcher : public matcher {
        public:
          backref_close_matcher() { 
            MatchLength = 0; 
            }
          TSize minimumMatchLength() const {
            return 0;
            }
          bool match(backref_stack &brstack,string const &candidate,TIndex at);
        };
    
  
  
  
      class alternative_matcher : public matcher {
          // The connector serves two purposes:
          // a) be a null-matcher that re-unites the different alternate token
          //    sequences
          // b) make the end of each sequence identifiable to be able to compute
          //    the match length
        
          class connector : public matcher {
            public:
              TSize minimumMatchLength() const {
                return 0;
                }
              bool match(backref_stack &brstack,string const &candidate,TIndex at);
            };
      
          typedef vector<matcher *>       wAltList;
          wAltList                        AltList;
          connector                       Connector;
      
        public:
          ~alternative_matcher();
  
          TSize minimumMatchLength() const;
          void setNext(matcher *next,bool deletenext = true);
          void addAlternative(matcher *matcher);
          bool match(backref_stack &brstack,string const &candidate,TIndex at);
        };
    
      class backref_matcher : public matcher {
          TIndex Backref;
    
        public:
          backref_matcher(TIndex backref);
          TSize minimumMatchLength() const {
            return 0;
            }
          bool match(backref_stack &brstack,string const &candidate,TIndex at);
        };
    
      // instance data ------------------------------------------------------------
      typedef string              Super;
      matcher                     *ParsedRegex;
      backref_stack               BackrefStack;
      string                      LastCandidate;
      TIndex                      MatchIndex;
      TSize                       MatchLength;
  
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
      TSize countBackrefs()
        { return BackrefStack.count(); }
      string getBackref(TIndex index)
        { return BackrefStack.get(index,LastCandidate); }
    
    private:
      matcher *parseRegex(string const &expr);
      quantifier *parseQuantifier(string const &expr,TIndex &at);
      bool isGreedy(string const &expr,TIndex &at);
    };
  }



#endif
