// ----------------------------------------------------------------------------
//  Description      : iXiONmedia XML parser
// ----------------------------------------------------------------------------
//  Remarks          :
//    tag::Children and tag::Text use the same indices. Thus, in the following
//    piece of XML 
//      <XML>  paff <EXPLOSION/> puff </XML>
//    Text[0] would be "paff", Children[0] would be "EXPLOSION", Text[1] would
//    be "puff".
//
// ----------------------------------------------------------------------------
//  (c) Copyright 1999 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#ifndef IXLIB_XML
#define IXLIB_XML




#include <vector>
#include <map>
#include <ixlib_exbase.hh>
#include <ixlib_scanner.hh>




// Error codes ----------------------------------------------------------------
#define ECXML_SYNTAX      	0
#define ECXML_NOTEXTHERE	1
#define ECXML_NOLITHERE		2
#define ECXML_LITEXPECTED	3
#define ECXML_UNEXPECTEDEND	4
#define ECXML_CLOSETAGEXPECTED	5
#define ECXML_CLOSETAGNAME	6




// xml_exception --------------------------------------------------------------
namespace ixion {
  struct xml_exception : public base_exception {
    xml_exception(TErrorCode error,char const *info = NULL,char *module = NULL,
      TIndex line = 0,char *category = NULL)
      : base_exception(error,info,module,line,"XML") {
      }
    xml_exception(TErrorCode error, TIndex line = 0, char const *info = 0);
    virtual char *getText() const;
    };




// xml_file -------------------------------------------------------------------
  class xml_file {
    protected:
      typedef scanner::token_list::const_iterator token_iterator;
    
    public:
      class tag {
        protected:
          string				Name; 
  
        public:
          map<string,string> 		Attributes;
          vector<tag *>			Children;
          vector<string>			Text;
  
          tag() {
            Text.push_back("");
            }
          tag(string const &name)
            : Name(name) {
            Text.push_back("");
            }
          tag(tag const &source);
          ~tag();
          void appendTag(tag *tag) {
            insertTag(Text.end(),tag);
            }
          void insertTag(vector<tag *>::iterator before,tag *tag);
          void insertTag(vector<string>::iterator before,tag *tag);
          tag *findTag(string const &name);
          
          void setName(string const &name) {
            Name = name;
            }
          string getName() const {
            return Name;
            }
  
        protected:
          void parse(token_iterator &first, token_iterator const &last);
          void write(ostream &ostr, TSize indent);
          
          void setLastText(string const &text) {
            Text.back() = text;
            }
  
          friend xml_file;
        };
      
      friend tag;
  
    protected:
      tag				*RootTag;
      
    public:  
      xml_file()
        : RootTag(NULL) {
        }
      ~xml_file() {
        if (RootTag) delete RootTag;
        }
        
      void read(istream &istr);
      void write(ostream &ostr);
      
      tag *getRootTag() {
        return RootTag;
        }
      void setRootTag(tag *newroot) {
        if (RootTag) delete RootTag;
        RootTag = newroot;
        }
      void clear() {
        setRootTag(NULL);
        }
        
    protected:
      void parse(scanner::token_list const &tokenlist);
      
      static void skipComment(token_iterator &first,token_iterator const &last);
    };
  }



#endif
