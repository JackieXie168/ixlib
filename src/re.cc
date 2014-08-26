// ----------------------------------------------------------------------------
//  Description      : Regular expressions string object.
// ----------------------------------------------------------------------------
//  (c) Copyright 1998 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <ctype.h>
#include <ixlib_exgen.hh>
#include <ixlib_numeric.hh>
#include <ixlib_numconv.hh>
#include <ixlib_re.hh>




using namespace ixion;




// Error texts ----------------------------------------------------------------
static char *RegexPlainText[] = {
  N_("Invalid quantifier"),
  N_("Unbalanced backreference"),
  N_("Invalid escape sequence"),
  N_("Invalid backreference"),
  N_("Unterminated character class"),
  };




// regex_exception ------------------------------------------------------------
regex_exception::regex_exception(TErrorCode error,
  char const *info,char *module,TIndex line)
: base_exception(error,info,module,line,"RE") {
  }




char *regex_exception::getText() const {
  return _(RegexPlainText[Error]);
  }




// regex_string::matcher ------------------------------------------------------
TSize regex_string::matcher::collectMatchLength() const {
  TSize totalml = 0;
  matcher const *object = this;
  while (object) {
    totalml += object->MatchLength;
    object = object->Next;
    }
  return totalml;
  }




// regex_string::quantifier ---------------------------------------------------
bool regex_string::quantifier::match(regex_string::backref_heap &brheap,
string const &candidate,TIndex at) {
  // Maximum: There should be at least one character in each match, we'd
  // run to Baghdad otherwise.
  TSize maxcount =  candidate.size() - at;
  if (MaxValid) maxcount = NUM_MIN(maxcount,MaxCount);
  
  TSize matchcount = 0;
  if (Greedy) {
    matchcount = maxcount+1; 
    while (matchcount>MinCount) {
      matchcount--;
      if (matchCount(brheap,candidate,at,matchcount)) return true;
      }
    }
  else {
    matchcount = MinCount;
    while (matchcount <= maxcount) {
      if (matchCount(brheap,candidate,at,matchcount)) return true;
      matchcount++;
      }
    }
  return false;
  }




bool regex_string::quantifier::matchCount(regex_string::backref_heap &brheap,
string const &candidate,TIndex at,TSize matchcount) {
  MatchLength = 0;
  while (matchcount--) {
    if (!matchQuantified(brheap,candidate,at+MatchLength)) return false;
    MatchLength += getQuantifiedMatchLength();
    }

  return matchNext(brheap,candidate,at+MatchLength);
  }




// regex_string::class_matcher ------------------------------------------------
void regex_string::class_matcher::expandClass() {
  if (!CharClass.size()) return;
  string finalclass(1,CharClass[0]);
  char lastchar = CharClass[0];

  for (TIndex index = 1;index < CharClass.size();index++) {
    if ((CharClass[index] == XSTRRE_CLASSRANGE) && (index < CharClass.size()-1)) {
      for (char ch = lastchar+1;ch < CharClass[index+1];ch++)
        finalclass += ch;
      }
    else finalclass += CharClass[index];
    lastchar = CharClass[index];
    }
  CharClass = finalclass;
  }




// regex_string::backref_open_matcher ------------------------------------------
bool regex_string::backref_open_matcher::match(regex_string::backref_heap &brheap,
string const &candidate,TIndex at) {
  backref backref;
  backref.checkpoint(at);

  brheap.push_back(backref);

  bool result = matchNext(brheap,candidate,at);

  if (!result) { brheap.pop_back(); }
  return result;
  }




// regex_string::backref_close_matcher -----------------------------------------
bool regex_string::backref_close_matcher::match(regex_string::backref_heap &brheap,
string const &candidate,TIndex at) {
  if (brheap.size() == 0) EX_THROW(regex,ECRE_UNBALBACKREF)
  brheap.back().makeReady(at);
  return matchNext(brheap,candidate,at);
  }




// regex_string::alternative_matcher ------------------------------------------
regex_string::alternative_matcher::~alternative_matcher() {
  while (AltList.size()) {
    delete AltList.back();
    AltList.pop_back();
    }
  }




void regex_string::alternative_matcher::operator+=(matcher *mat) {
  AltList.push_back(mat);
  matcher *searchlast = mat,*last = NULL;
  while (searchlast) {
    last = searchlast;
    searchlast = searchlast->getNext();
    }
  last->setNext(&Connector,false);
  }




bool regex_string::alternative_matcher::match(regex_string::backref_heap &brheap,
string const &candidate,TIndex at) {
  vector<matcher *>::iterator first = AltList.begin(),last = AltList.end();
  while (first != last) {
    if ((*first)->match(brheap,candidate,at)) {
      MatchLength = 0;
      matcher const *object = *first;
      while (object != &Connector) {
        MatchLength += object->getMatchLength();
        object = object->getNext();
        }
      return true;
      }
    first++;
    }
  return false;
  }




// regex_string::backref_matcher ----------------------------------------------
bool regex_string::backref_matcher::match(backref_heap &brheap,
string const &candidate,TIndex at) {
  string matchstr = brheap[Backref].get(candidate);
  MatchLength = matchstr.size();

  if (at+matchstr.size() > candidate.size()) return false;
  return (candidate.substr(at,matchstr.size()) == matchstr) &&
    matchNext(brheap,candidate,at+matchstr.size());
  }




// regex_string ---------------------------------------------------------------
bool regex_string::match(string const &candidate,TIndex from) {
  LastCandidate = candidate;
  BackrefHeap.clear();
  if (!ParsedRegex) parse();
  if (!ParsedRegex) return true;
  for (TIndex index = from;index < candidate.size();index++)
    if (ParsedRegex->match(BackrefHeap,candidate,index)) {
      MatchIndex = index;
      MatchLength = ParsedRegex->collectMatchLength();
      return true;
      }
  return false;
  }




bool regex_string::matchAt(string const &candidate,TIndex at) {
  LastCandidate = candidate;
  BackrefHeap.clear();
  if (!ParsedRegex) parse();
  if (ParsedRegex->match(BackrefHeap,candidate,at)) {
    MatchIndex = at;
    MatchLength = ParsedRegex->collectMatchLength();
    return true;
    }
  return false;
  }




string regex_string::replaceAll(string const &candidate,
string const &replacement,TIndex from) {
  string result;
  string tempreplacement;

  LastCandidate = candidate;
  BackrefHeap.clear();

  if (!ParsedRegex) parse();

  for (TIndex index = from;index < candidate.size();) {
    if (ParsedRegex->match(BackrefHeap,candidate,index)) {
      TIndex matchlength = ParsedRegex->collectMatchLength();
      tempreplacement = replacement;

      vector<backref>::iterator first = BackrefHeap.begin(),
        last = BackrefHeap.end();
      TSize brindex = 0;
      while (first != last) {
        tempreplacement = findReplace(tempreplacement,XSTRRE_BACKREF+unsigned2dec(brindex),
          BackrefHeap[index].get(LastCandidate));
        first++;
	brindex++;
        }

      result += tempreplacement;
      index += matchlength;
      }
    else result += candidate[index++];
    }
  return result;
  }




regex_string::matcher *regex_string::parseRegex(string const &expr) {
  if (!expr.size()) return NULL;
  TIndex index = 0;
  matcher *firstobject,*lastobject = NULL;
  alternative_matcher *alternative = NULL;

  while (index < expr.size()) {
    matcher *object = NULL;
    quantifier *quantifier = NULL;
    bool quantified = true;
    char ch;

    // several objects may be inserted in one loop run

    switch (expr[index++]) {
      // case XSTRRE_BACKREF: (dupe)
      // case XSTRRE_ESCAPESEQ: (dupe)
      case XSTRRE_LITERAL: {
          if (index >= expr.size()) EX_THROW(regex,ECRE_INVESCAPE)

          ch = expr[index++];
          if (isdigit(ch))
            object = new backref_matcher(ch-'0');
          else
            object = new string_matcher(string(1,ch));
          EX_MEMCHECK(object)
          break;
        }
      case XSTRRE_ANYCHAR:
        object = new any_char_matcher;
        EX_MEMCHECK(object)
        break;
      case XSTRRE_START:
        quantified = false;
        object = new start_matcher;
        EX_MEMCHECK(object)
        break;
      case XSTRRE_END:
        quantified = false;
        object = new end_matcher;
        EX_MEMCHECK(object)
        break;
      case XSTRRE_ALTERNATIVE: {
          if (!alternative) {
            alternative = new alternative_matcher;
            EX_MEMCHECK(alternative)
            }
          (*alternative) += firstobject;
          firstobject = NULL;
          lastobject = NULL;
          break;
          }
      case XSTRRE_CLASSSTART:
        try {
          TIndex classend = expr.find(XSTRRE_CLASSSTOP,index);
          object = new class_matcher(expr.substr(index,classend-index));
          EX_MEMCHECK(object)

          index = classend+1;
          }
        EX_CONVERT(generic,EC_ITEMNOTFOUND,regex,ECRE_UNTERMCLASS)
        break;
      case XSTRRE_BACKREFSTART: {
          quantified = false;
          matcher *parsed;

          TSize brlevel = 1;
          for (TIndex searchstop = index;searchstop < expr.size();searchstop++) {
            if ((expr[searchstop] == XSTRRE_BACKREFSTART) &&
            (expr[searchstop-1] != XSTRRE_LITERAL))
              brlevel++;
            if ((expr[searchstop] == XSTRRE_BACKREFSTOP) &&
            (expr[searchstop-1] != XSTRRE_LITERAL)) {
              brlevel--;
              if (brlevel == 0) {
                parsed = parseRegex(expr.substr(index,searchstop-index));
                if (!parsed) EX_THROW(regex,ECRE_INVBACKREF)

                index = searchstop+1;
                break;
                }
              }
            }

          if (!parsed) EX_THROW(regex,ECRE_UNBALBACKREF)

          object = new backref_open_matcher;
          EX_MEMCHECK(object)
          object->setNext(parsed);

          matcher *closer = new backref_close_matcher;
          EX_MEMCHECK(closer);

          matcher *searchlast = parsed,*foundlast;
          while (searchlast) {
            foundlast = searchlast;
            searchlast = searchlast->getNext();
            }
          foundlast->setNext(closer);

          break;
          }
      case XSTRRE_BACKREFSTOP:
        EX_THROW(regex,ECRE_UNBALBACKREF)
      default:
        object = new string_matcher(string(1,expr[index-1]));
        EX_MEMCHECK(object)
        break;
      }

    if (object) {
      if (quantified) quantifier = parseQuantifier(expr,index);
      if (quantifier) {
        quantifier->setQuantified(object);
        if (lastobject) lastobject->setNext(quantifier);
        else firstobject = quantifier;
        }
      else {
        if (lastobject) lastobject->setNext(object);
        else firstobject = object;
        }
      }

    matcher *searchlast = quantifier ? quantifier : object;
    while (searchlast) {
      lastobject = searchlast;
      searchlast = searchlast->getNext();
      }
    }
  if (alternative) {
    (*alternative) += firstobject;
    return alternative;
    }
  else return firstobject;
  }




regex_string::quantifier *regex_string::parseQuantifier(string const &expr,TIndex &at) {
  quantifier *quant = NULL;
  if (at == expr.size()) return NULL;
  if (expr[at] == XSTRREQ_0PLUS) {
    quant = new quantifier(isGreedy(expr,++at),0);
    EX_MEMCHECK(quant)
    return quant;
    }
  if (expr[at] == XSTRREQ_1PLUS) {
    quant = new quantifier(isGreedy(expr,++at),1);
    EX_MEMCHECK(quant)
    return quant;
    }
  if (expr[at] == XSTRREQ_01) {
    quant = new quantifier(isGreedy(expr,++at),0,1);
    EX_MEMCHECK(quant)
    return quant;
    }
  if (expr[at] == XSTRREQ_START) {
    TSize min,max;

    at++;
    TIndex endindex;
    string quantspec;
    endindex = expr.find(XSTRREQ_STOP,at);
    if (endindex == string::npos) 
      EXGEN_THROW(ECRE_INVQUANTIFIER)

    at = endindex+1;

    try {
      try {
        TIndex rangeindex = quantspec.find(XSTRREQ_RANGE);
        if (rangeindex == quantspec.size()-1) {
          min = evalUnsigned(
	    quantspec.substr(0,rangeindex)+
	    quantspec.substr(rangeindex+1));
          quant = new quantifier(isGreedy(expr,at),min);
          }
        else {
          min = evalUnsigned(quantspec.substr(0,rangeindex));
          max = evalUnsigned(quantspec.substr(rangeindex+1));
          quant = new quantifier(isGreedy(expr,at),min,max);
          }
        }
      EX_CATCHCODE(generic,EC_ITEMNOTFOUND,
        min = evalUnsigned(quantspec);
        quant = new quantifier(isGreedy(expr,at),min,min);
        )
      EX_MEMCHECK(quant)
      return quant;
      }
    EX_CONVERT(generic,EC_CANNOTEVALUATE,regex,ECRE_INVQUANTIFIER)
    }
  return NULL;
  }




bool regex_string::isGreedy(string const &expr,TIndex &at)
{
  if (at == expr.size()) return true;
  if (expr[at] == XSTRREQ_NONGREEDY) {
    at++;
    return false;
  }
  else return true;
}

