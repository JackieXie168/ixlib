// ----------------------------------------------------------------------------
//  Description      : iXiONmedia XML parser
// ----------------------------------------------------------------------------
//  (c) Copyright 1999 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <algorithm>
#include <iterator>
#include <ixlib_xml.hh>
#include <ixlib_numconv.hh>
#include <ixlib_token_xml.hh>
#include <ixlib_scanxml.hh>
#include <ixlib_exgen.hh>




using namespace ixion;




// Exception throw macro ------------------------------------------------------
#define EXXML_THROW(CODE,TOKEN)\
  throw xml_exception(CODE,(TOKEN).Line,NULL);




// Plain text rendering table -------------------------------------------------
static char *(PlainText[]) ={
  N_("Syntax error"),
  N_("No text expected in this context"),
  N_("No literal expected in this context"),
  N_("Literal expected"),
  N_("Unexpected end of input"),
  N_("Close tag expected"),
  N_("Close tag has different name"),
  };




// xml_exception --------------------------------------------------------------
xml_exception::xml_exception(TErrorCode error, TIndex line, char const *info)
: base_exception(error, NULL, NULL, 0, "XML") {
  HasInfo = true;
  try {
    string temp = "l" + unsigned2dec(line);
    if (info) {
      temp += " : ";
      temp += info;
      }
    strcpy(Info,temp.c_str());
    }
  catch (...) { }
  }




char *xml_exception::getText() const {
  return _(PlainText[Error]);
  }




// tag -----------------------------------------------------------------------
xml_file::tag::tag(xml_file::tag const &source)
: Name(source.Name),Attributes(source.Attributes),Text(source.Text) {
  vector<tag *>::const_iterator first = source.Children.begin(),
                           	 last = source.Children.end();
  
  while (first != last) {
    tag *tg = new tag(**first++);
    EX_MEMCHECK(tg)
    Children.push_back(tg);
    }
  
  copy(source.Text.begin(),source.Text.end(),back_inserter(Text));
  }
  
  
  
  
xml_file::tag::~tag() {
  vector<tag *>::iterator first = Children.begin(),last = Children.end();
  
  while (first != last) delete *first++;
  }
  
  
  
  
void xml_file::tag::insertTag(vector<tag *>::iterator before,tag *tag) {
  TSize index = before-Children.begin();
  vector<string>::iterator pos = Text.begin() + index;
  
  Children.insert(before,tag);
  Text.insert(pos,"");
  }



  
void xml_file::tag::insertTag(vector<string>::iterator before,tag *tg) {
  TSize index = before-Text.begin();
  if (index == 0) {
    Children.insert(Children.begin(),tg);
    Text.insert(Text.begin(),"");
    }
  else {
    vector<tag *>::iterator pos = Children.begin() + index - 1;

    Children.insert(pos,tg);
    Text.insert(before,"");
    }
  }



  
xml_file::tag *xml_file::tag::findTag(string const &name) {
  vector<tag *>::const_iterator first = Children.begin(),
  			   	 last = Children.end();
				 
  while (first != last) {
    if ((*first)->Name == name) return *first;
    first++;
    }				
  return NULL;
  }
  
  
  
  
void xml_file::tag::parse(token_iterator &first,token_iterator const &last) {
  string attrwvname;
  string stringlit;

  Name = first->Text.substr(1, first->Text.size()-1);
  first++;

  // parse attribute list
  bool quitflag = false;
  bool isshort = false;

  while (first != last && !quitflag) {
    switch  (first->Type) {
      case TT_XML_TEXT:
      	EXXML_THROW(ECXML_NOTEXTHERE,*first)
	break;
      case TT_XML_END_TAG:
        quitflag = true;
	first++;
        break;
      case TT_XML_END_SHORT_TAG:
        quitflag = true;
	isshort = true;
	first++;
        break;
      case TT_XML_ATTRIBUTE:
	Attributes[first->Text] = "";
	first++;
        break;
      case TT_XML_ATTRIBUTE_WITH_VALUE:
        attrwvname = first->Text;
	first++;
	if (first == last)
	  EX_THROW(xml,ECXML_UNEXPECTEDEND)
	if (first->Type != TT_XML_STRINGLITERAL)
	  EXXML_THROW(ECXML_LITEXPECTED,*first)
	stringlit = first->Text;
	Attributes.insert(
	  make_pair(attrwvname.substr(0, attrwvname.size()-1), stringlit.substr(1, stringlit.size()-2))
	);
	first++;
        break;
      case TT_XML_STRINGLITERAL:
      	EXXML_THROW(ECXML_NOLITHERE,*first)
	break;
      default:
        EXXML_THROW(ECXML_SYNTAX,*first);
        break;
      }
    }

  if (isshort) return;

  // parse contents
  quitflag = false;
  tag *tg;
  string currenttext = "";
  while (first != last && !quitflag) {
    switch (first->Type) {
      case TT_XML_TEXT:
      case TT_XML_STRINGLITERAL:
      case TT_XML_ATTRIBUTE:
      case TT_XML_ATTRIBUTE_WITH_VALUE:
        if (currenttext.size()) currenttext += ' ';
	currenttext += first->Text;
	first++;
	break;
      case TT_XML_BEGIN_TAG:
        tg = new tag;
	EX_MEMCHECK(tg)
	try {
	  tg->parse(first,last);
	  Children.push_back(tg);
	  }
	catch (...) {
	  delete tg;
	  throw;
	  }
	setLastText(currenttext);
	Text.push_back("");
        currenttext = "";
	break;
      case TT_XML_CLOSE_TAG:
	quitflag = true;
	break;
      case TT_XML_END_SHORT_TAG:
      default:
        EXXML_THROW(ECXML_SYNTAX,*first);
        break;
      }
    }
  setLastText(currenttext);

  // parse close tag
  if (first == last)
    EX_THROW(xml,ECXML_UNEXPECTEDEND)
  if (first->Type != TT_XML_CLOSE_TAG) 
    EXXML_THROW(ECXML_CLOSETAGEXPECTED,*first)
  if (first->Text.substr(2, first->Text.size()-3) != Name) 
    EXXML_THROW(ECXML_CLOSETAGNAME,*first)

  first++;
  }
  
  
  
  
void xml_file::tag::write(ostream &ostr, TSize indent) {
  string str = string(indent,' ') + '<' + Name + ' ';
  map<string,string>::const_iterator firstattr = Attributes.begin(),
                                       lastattr = Attributes.end();
  
  while (firstattr != lastattr) {
    if (firstattr->second == "")  
      str += firstattr->first + ' ';
    else
      str += firstattr->first + "=\"" + firstattr->second + "\" ";
    firstattr++;
    }
  if (Text.size() == 1 && Text[0].size() == 0) {
    ostr << (str + "/>") << endl;
    return;
    }
    
  ostr << (str + ">") << endl;
  
  vector<tag *>::const_iterator firsttag = Children.begin(),
  				 lasttag = Children.end();
  vector<string>::const_iterator firsttext = Text.begin(),
  				   lasttext = Text.end();
  
  while (firsttag != lasttag) {
    if (*firsttext != "") ostr << string(indent+2,' ') << (*firsttext) << endl;
    (*firsttag)->write(ostr, indent+2);
    firsttag++;
    firsttext++;
    }
  if (*firsttext != "") ostr << string(indent+2,' ') << (*firsttext) << endl;
  
  ostr << (string(indent,' ') + "</" + Name + ">") << endl;
  }
  
  
  
  
// xml_file -------------------------------------------------------------------
void xml_file::read(istream &istr) {
  clear();
  xmlFlexLexer		lexer(&istr);
  scanner		scanner(lexer);
  scanner::token_list	tokenlist = scanner.scan();
  tokenlist.swap(tokenlist);
  parse(tokenlist);
  }




void xml_file::write(ostream &ostr) {
  ostr << "<!-- Generated by ixlib XML -->" << endl;
  if (RootTag) RootTag->write(ostr,0);
  }




void xml_file::parse(scanner::token_list const &tokenlist) {
  token_iterator first = tokenlist.begin(),
    last = tokenlist.end();

  while (first != last) {
    switch (first->Type) {
      case TT_XML_TEXT:
      case TT_XML_ATTRIBUTE:
      case TT_XML_ATTRIBUTE_WITH_VALUE:
      case TT_XML_STRINGLITERAL:
      case TT_XML_BEGIN_TAG:
        RootTag = new tag;
	EX_MEMCHECK(RootTag)
	try {
	  RootTag->parse(first,last);
	  }
	catch (...) {
	  delete RootTag;
	  RootTag = NULL;
	  throw;
	  }
	break;
      default:
        EXXML_THROW(ECXML_SYNTAX,*first);
        break;
      }
    }
  }