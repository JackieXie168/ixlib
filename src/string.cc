// ----------------------------------------------------------------------------
//  Description      : String object
// ----------------------------------------------------------------------------
//  (c) Copyright 1999 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <cstring>
#include <cctype>
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
