/* -------- definitions ------- */

%option c++ yylineno noyywrap prefix="xml"

%{
#include <ixlib_token_xml.hh>
%}

WHITESPACE      [ \t\n]
DIGIT           [0-9]
NONDIGIT        [_a-zA-Z]
ID_COMPONENT    [_a-zA-Z0-9]
IDENTIFIER      {NONDIGIT}{ID_COMPONENT}*

%x Attribute
%x Comment

/* -------- rules -------- */
%%

\<{IDENTIFIER}					BEGIN(Attribute); return TT_XML_BEGIN_TAG;
<Attribute>\>					BEGIN(INITIAL); return TT_XML_END_TAG;
<Attribute>\/\>					BEGIN(INITIAL); return TT_XML_END_SHORT_TAG;
<Attribute>{IDENTIFIER}\=			return TT_XML_ATTRIBUTE_WITH_VALUE;
<Attribute>{IDENTIFIER}				return TT_XML_ATTRIBUTE;
<Attribute>\"[^\"]*\"				return TT_XML_STRINGLITERAL;

\<\!\-+						BEGIN(Comment);
<Comment>\-+\>					BEGIN(INITIAL);
<Comment>[^\-]+					/* nothing */
<Comment>\-+[^\>]				/* nothing */

<INITIAL,Attribute,Comment>{WHITESPACE}+	/* nothing */
<INITIAL,Attribute,Comment>.    		return TT_UNKNOWN;
<INITIAL,Attribute,Comment><<EOF>>    		return TT_EOF;

\<\/{IDENTIFIER}\>				return TT_XML_CLOSE_TAG;
[^\< \t\n]+					return TT_XML_TEXT;
