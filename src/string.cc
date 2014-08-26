// ----------------------------------------------------------------------------
//  Description      : String object
// ----------------------------------------------------------------------------
//  (c) Copyright 1999 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <cstring>
#include <cctype>
#include <ixlib_numconv.hh>
#include <ixlib_string.hh>




using namespace ixion;




// String utility functions ---------------------------------------------------
string ixion::findReplace(string const &target,string const &src,string const &dest) {
  string result = target;
  TIndex foundpos = string::npos;
  TIndex n = src.size();
  while ((foundpos = result.find(src)) != string::npos)
    result.replace(foundpos,n,dest);
  return result;
  }




string ixion::findReplace(string const &target,char* src,char *dest) {
  string result = target;
  TSize foundpos = string::npos;
  TSize n = strlen(src);
  while ((foundpos = result.find(src)) != string::npos)
    result.replace(foundpos,n,dest);
  return result;
  }




string ixion::findReplace(string const &target,char src,char dest) {
  string result = target;
  string::iterator first = result.begin(),last = result.end();

  while (first != last) {
    if (*first == src) *first = dest;
    first++;
    }
  return result;
  }




string ixion::upper(string const &original) {
  string temp(original);
  string::iterator first = temp.begin(),last = temp.end();
  
  while (first != last) {
    *first = toupper(*first);
    first++;
    }
  return temp;
  }




string ixion::lower(string const &original) {
  string temp(original);
  string::iterator first = temp.begin(),last = temp.end();
  
  while (first != last) {
    *first = tolower(*first);
    first++;
    }
  return temp;
  }




string ixion::removeLeading(string const &original,char ch) {
  string copy(original);
  string::iterator first = copy.begin(), last = copy.end();
  
  while (first != last && *first == ch) first++;
  if (first != copy.begin()) copy.erase(copy.begin(),first);
  return copy;
  }




string ixion::removeTrailing(string const &original,char ch) {
  string copy(original);
  string::iterator first = copy.begin(), last = copy.end();
  
  if (first != last) {
    last--;
    while (first != last && *last == ch) last--;
    if (*last != ch) last++;
    }
  
  if (last != copy.end()) copy.erase(last,copy.end());
  return copy;
  }




string ixion::removeLeadingTrailing(string const &original,char ch) {
  string copy(original);
  string::iterator first = copy.begin(), last = copy.end();
  
  while (first != last && *first == ch) first++;
  if (first != copy.begin()) copy.erase(copy.begin(),first);
  
  first = copy.begin();
  last = copy.end();

  if (first != last) {
    last--;
    while (first != last && *last == ch) last--;
    if (*last != ch) last++;
    }

  if (last != copy.end()) copy.erase(last,copy.end());
  return copy;
  }




string ixion::parseCEscapes(string const &original) {
  string result = "";
  string::const_iterator first = original.begin(),last = original.end();
  while (first != last) {
    if (*first == '\\') {
      first++;
      if (first == last) { 
        result += '\\';
	break;
	}
      
      #define GET_TEMP_STRING(LENGTH) \
        if (original.end()-first < LENGTH) \
	  EXGEN_THROWINFO(EC_INVALIDPAR,"invalid escape sequence") \
	tempstring = string(first,first+LENGTH); \
	first += LENGTH;
      
      char value;
      string tempstring;
      switch (*first) {
        case 'b': result += '\b'; first++; break;
        case 'f': result += '\f'; first++; break;
        case 'n': result += '\n'; first++; break;
        case 't': result += '\t'; first++; break;
        case 'v': result += '\v'; first++; break;
	case 'X':
	case 'x': first++;
	  GET_TEMP_STRING(2)
	  value = evalNumeral(tempstring,16);
	  result += value;
	  break;
	case 'u': first++;
	  GET_TEMP_STRING(4)
	  value = evalNumeral(tempstring,16);
	  result += value;
	  break;
	case '0':
	  GET_TEMP_STRING(3)
	  value = evalNumeral(tempstring,8);
	  result += value;
	  break;
	default: result += *first++;
	}
      }
    else result += *first++;
    }
  return result;
  }




// string_hash ----------------------------------------------------------------
unsigned long ixion::string_hash::operator()(string const &str) const {
  // the sgi stl uses the same hash algorithm
  unsigned long h = 0; 
  FOREACH_CONST(first,str,string)
    h = 5*h + *first;
  return h;
  }
