// ----------------------------------------------------------------------------
//  Description      : Regular expressions string object.
// ----------------------------------------------------------------------------
//  (c) Copyright 1998 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <stack>
#include <cctype>
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




// regex_string::backref_stack ------------------------------------------------
void regex_string::backref_stack::open(TIndex index) {
  backref_entry entry = { backref_entry::OPEN,index };
  Stack.push_back(entry);
  }




void regex_string::backref_stack::close(TIndex index) {
  backref_entry entry = { backref_entry::CLOSE,index };
  Stack.push_back(entry);
  }




regex_string::backref_stack::rewind_info 
regex_string::backref_stack::getRewindInfo() const {
  return Stack.size();
  }




void regex_string::backref_stack::rewind(rewind_info ri) {
  Stack.erase(Stack.begin()+ri,Stack.end());
  }




void regex_string::backref_stack::clear() {
  Stack.clear();
  }




TSize regex_string::backref_stack::count() {
  TSize result = 0;
  FOREACH_CONST(first,Stack,internal_stack)
    if (first->Type == backref_entry::OPEN) result++;
  return result;
  }




string regex_string::backref_stack::get(TIndex number,string const &candidate) const {
  TIndex level = 0,next_index = 0;
  TIndex start;
  
  internal_stack::const_iterator first = Stack.begin(),last = Stack.end();
  while (first != last) {
    if (first->Type == backref_entry::OPEN) {
      if (number == next_index) {
        start = first->Index;
 	level++;
        break;
        }
      next_index++;
      level++;
      }
    if (first->Type == backref_entry::CLOSE) 
      level--;
    first++;
    }
  
  if (first == last)
    EX_THROW(regex,ECRE_INVBACKREF)

  first++;
    
  while (first != last) {
    if (first->Type == backref_entry::OPEN) 
      level++;
    if (first->Type == backref_entry::CLOSE) {
      level--;
      if (number == level)
        return candidate.substr(start,first->Index - start);
      }
    first++;
    }
  EX_THROW(regex,ECRE_UNBALBACKREF)
  }




// regex_string::matcher ------------------------------------------------------
regex_string::matcher::matcher()
  : Next(NULL) { 
  }




regex_string::matcher::~matcher() { 
  if (Next && DeleteNext) delete Next;
  }




TSize regex_string::matcher::subsequentMatchLength() const {
  TSize totalml = 0;
  matcher const *object = this;
  while (object) {
    totalml += object->MatchLength;
    object = object->Next;
    }
  return totalml;
  }




TSize regex_string::matcher::minimumSubsequentMatchLength() const  {
  TSize totalml = 0;
  matcher const *object = this;
  while (object) {
    totalml += object->minimumMatchLength();
    object = object->Next;
    }
  return totalml;
  }




// regex_string::quantifier ---------------------------------------------------
regex_string::quantifier::quantifier(bool greedy,TSize mincount)
  : Greedy(greedy),MaxValid(false),MinCount(mincount) { 
  }




regex_string::quantifier::quantifier(bool greedy,TSize mincount,TSize maxcount)
  : Greedy(greedy),MaxValid(true),MinCount(mincount),MaxCount(maxcount) { 
  }




regex_string::quantifier::~quantifier() { 
  if (Quantified) 
    delete Quantified; 
  }




TSize regex_string::quantifier::minimumMatchLength() const {
  if (Quantified) 
    return MinCount * Quantified->minimumMatchLength();
  else 
    return 0;
  }




bool regex_string::quantifier::match(backref_stack &brstack,string const &candidate,TIndex at) {
  // this routine does speculative matching, so it must pay close attention
  // to rewind the backref stack appropriately.
  // NB: matchNext does the rewinding automatically, whereas speculative
  // matches of the quantified portion must be rewound.
  
  // There should be at least one character in each match, we'd
  // run to Baghdad otherwise.
  
  if (!Quantified) 
    return matchNext(brstack,candidate,at);
  
  // calculate accurate maximum match count
  TSize quant_min = Quantified->minimumSubsequentMatchLength();
  if (quant_min == 0) quant_min = 1;
  
  TSize max_count = candidate.size() - at;
  if (Next) max_count -= Next->minimumSubsequentMatchLength();
  max_count = max_count/quant_min + 1;
  
  if (MaxValid) max_count = NUM_MIN(max_count,MaxCount);
  
  // check that at least MinCount matches take place (non-speculative)
  TIndex idx = at;
  for (TSize c = 1;c <= MinCount;c++)
    if (Quantified->match(brstack,candidate,idx))
      idx += Quantified->subsequentMatchLength();
    else 
      return false;
  
  // determine number of remaining matches
  TSize remcount = max_count-MinCount;
  
  // test for the remaining matches in a way that depends on Greedy flag
  if (Greedy) {
    // try to gobble up as many matches of quantified part as possible
    // (speculative)
    
    stack<backtrack_stack_entry> successful_indices;
    { backtrack_stack_entry entry = { idx,brstack.getRewindInfo() };
      successful_indices.push(entry);
      }
    
    while (Quantified->match(brstack,candidate,idx) && successful_indices.size()-1 < remcount) {
      idx += Quantified->subsequentMatchLength();
      backtrack_stack_entry entry = { idx,brstack.getRewindInfo() };
      successful_indices.push(entry);
      }
    
    // backtrack until rest of string also matches
    while (successful_indices.size() && !matchNext(brstack,candidate,successful_indices.top().Index)) {
      brstack.rewind(successful_indices.top().RewindInfo);
      successful_indices.pop();
      }
    
    if (successful_indices.size()) {
      MatchLength = successful_indices.top().Index - at;
      return true;
      }
    else return false;
    }
  else {
    for (TSize c = 0;c <= remcount;c++) {
      if (matchNext(brstack,candidate,idx)) {
        MatchLength = idx-at;
        return true;
        }
      // following part runs once too much, effectively: 
      // if c == remcount, idx may be increased, but the search fails anyway
      // => no problem
      if (Quantified->match(brstack,candidate,idx))
        idx += Quantified->subsequentMatchLength();
      else 
        return false;
      }
    return false;
    }
  }




// regex_string::string_matcher -----------------------------------------------
regex_string::string_matcher::string_matcher(string const &matchstr)
  : MatchStr(matchstr) { 
  MatchLength = MatchStr.size(); 
  }




bool regex_string::string_matcher::match(backref_stack &brstack,string const &candidate,TIndex at) {
  if (at+MatchStr.size() > candidate.size()) return false;
  return (string(candidate,at,MatchStr.size()) == MatchStr) &&
    matchNext(brstack,candidate,at+MatchStr.size());
  }




// regex_string::class_matcher ------------------------------------------------
regex_string::class_matcher::class_matcher(string const &cls)
  : Negated(false) {
  MatchLength = 1;
  
  if (cls.size() && cls[0] == XSTRRE_CLASSNEG) {
    expandClass(cls.substr(1));
    Negated = true;
    }
  else
    expandClass(cls);
  }




bool regex_string::class_matcher::match(backref_stack &brstack,string const &candidate,TIndex at) {
  if (at >= candidate.size()) return false;
  
  bool result = Set[candidate[at]];
  if (Negated) result = !result;
  
  return result && matchNext(brstack,candidate,at+1);
  }




void regex_string::class_matcher::expandClass(string const &cls) {
  memset(&Set,0,sizeof(Set));
  
  if (cls.size() == 0) return;
  Set[cls[0]] = true;
  char lastchar = cls[0];

  for (TIndex index = 1;index < cls.size();index++) {
    if ((cls[index] == XSTRRE_CLASSRANGE) && (index < cls.size()-1)) {
      for (char ch = lastchar+1;ch < cls[index+1];ch++)
        Set[ch] = true;
      }
    else Set[cls[index]] = true;
    lastchar = cls[index];
    }
  }




// regex_string::special_class_matcher ----------------------------------------
regex_string::special_class_matcher::special_class_matcher(type tp)
  : Type(tp) {
  MatchLength = 1;
  }




bool regex_string::special_class_matcher::match(backref_stack &brstack,string const &candidate,TIndex at) {
  if (at >= candidate.size()) return false;
        enum type { DIGIT,NONDIGIT,ALNUM,NONALNUM,SPACE,NONSPACE };
  
  bool result;
  switch (Type) {
    case DIGIT: result = isdigit(candidate[at]);
      break;
    case NONDIGIT: result = !isdigit(candidate[at]);
      break;
    case ALNUM: result = isalnum(candidate[at]);
      break;
    case NONALNUM: result = !isalnum(candidate[at]);
      break;
    case SPACE: result = isspace(candidate[at]);
      break;
    case NONSPACE: result = !isspace(candidate[at]);
      break;
    default:
      EX_THROW(regex,ECRE_INVESCAPE)
    }
  
  return result && matchNext(brstack,candidate,at+1);
  }




// regex_string::backref_open_matcher ------------------------------------------
bool regex_string::backref_open_matcher::match(backref_stack &brstack,string const &candidate,TIndex at) {
  backref_stack::rewind_info ri = brstack.getRewindInfo();
  brstack.open(at);
  
  bool result = matchNext(brstack,candidate,at);

  if (!result)
    brstack.rewind(ri);
  return result;
  }




// regex_string::backref_close_matcher -----------------------------------------
bool regex_string::backref_close_matcher::match(backref_stack &brstack,string const &candidate,TIndex at) {
  backref_stack::rewind_info ri = brstack.getRewindInfo();
  brstack.close(at);
  
  bool result = matchNext(brstack,candidate,at);

  if (!result)
    brstack.rewind(ri);
  return result;
  }




// regex_string::alternative_matcher::connector -------------------------------
bool regex_string::alternative_matcher::connector::match(backref_stack &brstack,string const &candidate,TIndex at) {
  return matchNext(brstack,candidate,at);
  }




// regex_string::alternative_matcher ------------------------------------------
regex_string::alternative_matcher::~alternative_matcher() {
  while (AltList.size()) {
    delete AltList.back();
    AltList.pop_back();
    }
  }




TSize regex_string::alternative_matcher::minimumMatchLength() const {
  TSize result = 0;
  bool is_first = true;
  FOREACH_CONST(first,AltList,wAltList)
    if (is_first) {
      result = (*first)->minimumMatchLength();
      is_first = true;
      }
    else {
      TSize current = (*first)->minimumMatchLength();
      if (current < result) result = current;
      }
  return result;
  }




void regex_string::alternative_matcher::setNext(matcher *next,bool deletenext = true) {
  matcher::setNext(next);
  Connector.setNext(next,false);
  }




void regex_string::alternative_matcher::addAlternative(matcher *mat) {
  AltList.push_back(mat);
  matcher *searchlast = mat,*last = NULL;
  while (searchlast) {
    last = searchlast;
    searchlast = searchlast->getNext();
    }
  last->setNext(&Connector,false);
  }




bool regex_string::alternative_matcher::match(backref_stack &brstack,string const &candidate,TIndex at) {
  vector<matcher *>::iterator first = AltList.begin(),last = AltList.end();
  while (first != last) {
    if ((*first)->match(brstack,candidate,at)) {
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
regex_string::backref_matcher::backref_matcher(TIndex backref)
  : Backref(backref) { 
  }




bool regex_string::backref_matcher::match(backref_stack &brstack,string const &candidate,TIndex at) {
  string matchstr = brstack.get(Backref,candidate);
  MatchLength = matchstr.size();

  if (at+matchstr.size() > candidate.size()) return false;
  return (candidate.substr(at,matchstr.size()) == matchstr) &&
    matchNext(brstack,candidate,at+matchstr.size());
  }




// regex_string ---------------------------------------------------------------
bool regex_string::match(string const &candidate,TIndex from) {
  LastCandidate = candidate;
  BackrefStack.clear();
  if (!ParsedRegex) parse();
  if (!ParsedRegex) return true;
  for (TIndex index = from;index < candidate.size();index++)
    if (ParsedRegex->match(BackrefStack,candidate,index)) {
      MatchIndex = index;
      MatchLength = ParsedRegex->subsequentMatchLength();
      return true;
      }
  return false;
  }




bool regex_string::matchAt(string const &candidate,TIndex at) {
  LastCandidate = candidate;
  BackrefStack.clear();
  if (!ParsedRegex) parse();
  if (ParsedRegex->match(BackrefStack,candidate,at)) {
    MatchIndex = at;
    MatchLength = ParsedRegex->subsequentMatchLength();
    return true;
    }
  return false;
  }




string regex_string::replaceAll(string const &candidate,string const &replacement,TIndex from) {
  string result;
  string tempreplacement;

  LastCandidate = candidate;
  if (!ParsedRegex) parse();

  for (TIndex index = from;index < candidate.size();) {
    BackrefStack.clear();
    if (ParsedRegex->match(BackrefStack,candidate,index)) {
      TIndex matchlength = ParsedRegex->subsequentMatchLength();
      tempreplacement = replacement;

      TSize backrefs = BackrefStack.count();
      for (TIndex i = 0;i < backrefs;i++)
        tempreplacement = findReplace(tempreplacement,XSTRRE_BACKREF+unsigned2dec(i),
          BackrefStack.get(i,LastCandidate));

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
          else {
	    switch (ch) {
	      case 'd': object = new special_class_matcher(special_class_matcher::DIGIT);
	        break;
	      case 'D': object = new special_class_matcher(special_class_matcher::NONDIGIT);
	        break;
	      case 'w': object = new special_class_matcher(special_class_matcher::ALNUM);
	        break;
	      case 'W': object = new special_class_matcher(special_class_matcher::NONALNUM);
	        break;
	      case 's': object = new special_class_matcher(special_class_matcher::SPACE);
	        break;
	      case 'S': object = new special_class_matcher(special_class_matcher::NONSPACE);
	        break;
	      default: object = new string_matcher(string(1,ch));
	      }
	    }
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
          alternative->addAlternative(firstobject);
          firstobject = NULL;
          lastobject = NULL;
          break;
          }
      case XSTRRE_CLASSSTART: {
          TIndex classend = expr.find(XSTRRE_CLASSSTOP,index);
          if (classend == string::npos)
            EX_THROW(regex,ECRE_UNTERMCLASS)
          object = new class_matcher(expr.substr(index,classend-index));
          EX_MEMCHECK(object)
  
          index = classend+1;
          break;
          }
      case XSTRRE_BACKREFSTART: {
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
        object = new string_matcher(expr.substr(index-1,1));
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

    // we need this for the alternative matcher, which also inserts
    // its connector
    matcher *searchlast = quantifier ? quantifier : object;
    while (searchlast) {
      lastobject = searchlast;
      searchlast = searchlast->getNext();
      }
    }
  if (alternative) {
    alternative->addAlternative(firstobject);
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

