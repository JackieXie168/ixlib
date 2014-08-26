// ----------------------------------------------------------------------------
//  Description      : Token definitions for XML scanner
// ----------------------------------------------------------------------------
//  Remarks          :
//
// ----------------------------------------------------------------------------
//  (c) Copyright 2000 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#ifndef IXLIB_TOKENXML
#define IXLIB_TOKENXML





#include <ixlib_token_lex.hh>




// Token types for xml parsing
#define TT_XML_BEGIN_TAG		10
#define TT_XML_END_TAG			11
#define TT_XML_END_SHORT_TAG		12
#define TT_XML_CLOSE_TAG		13

#define TT_XML_ATTRIBUTE		20
#define TT_XML_ATTRIBUTE_WITH_VALUE	21

#define TT_XML_STRINGLITERAL		30

#define TT_XML_TEXT			40




#endif
