// ----------------------------------------------------------------------------
//  Description      : Javascript interpreter
// ----------------------------------------------------------------------------
//  (c) Copyright 2000 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <cstdio>
#include <ixlib_i18n.hh>
#include <ixlib_numconv.hh>
#include <ixlib_re.hh>
#include <ixlib_string.hh>
#include <ixlib_js_internals.hh>
#include <ixlib_token_javascript.hh>




using namespace ixion;
using namespace javascript;




// value ----------------------------------------------------------------------
string value::toString() const {
  EXJS_THROWINFO(ECJS_CANNOT_CONVERT,(string(_("to string from "))+valueType2string(getType())).c_str())
  }




int value::toInt() const {
  EXJS_THROWINFO(ECJS_CANNOT_CONVERT,(string(_("to int from "))+valueType2string(getType())).c_str())
  }




double value::toFloat() const {
  EXJS_THROWINFO(ECJS_CANNOT_CONVERT,(string(_("to float from "))+valueType2string(getType())).c_str())
  }




bool value::toBoolean() const {
  EXJS_THROWINFO(ECJS_CANNOT_CONVERT,(string(_("to boolean from "))+valueType2string(getType())).c_str())
  }




string value::stringify() const {
  try {
    return toString();
    }
  catch (...) {
    return string("#<")+valueType2string(getType())+">";
    }
  }




ref<value> value::duplicate() const {
  EXJS_THROWINFO(ECJS_INVALID_OPERATION,(string(_("duplication of "))+valueType2string(getType())).c_str())
  }




ref<value> 
value::lookup(string const &identifier) {
  EXJS_THROWINFO(ECJS_INVALID_OPERATION,(_("lookup of ")+identifier+_(" on ")+valueType2string(getType())).c_str())
  }




ref<value> 
value::subscript(value const &index) {
  EXJS_THROWINFO(ECJS_INVALID_OPERATION,(string(_("subscript on "))+valueType2string(getType())).c_str())
  }




ref<value> 
value::call(context const &ctx,parameter_list const &parameters) const {
  EXJS_THROWINFO(ECJS_INVALID_OPERATION,(string(_("call on "))+valueType2string(getType())).c_str())
  }




ref<value> 
value::construct(context const &ctx,parameter_list const &parameters) const {
  EXJS_THROWINFO(ECJS_INVALID_OPERATION,(string(_("construction on "))+valueType2string(getType())).c_str())
  }




ref<value> value::assign(ref<value> op2) {
  EXJS_THROWINFO(ECJS_INVALID_OPERATION,(string(_("assignment to "))+valueType2string(getType())).c_str())
  }




ref<value> 
value::operatorUnary(operator_id op) const {
  EXJS_THROWINFO(ECJS_INVALID_OPERATION,(string(operator2string(op))+_(" on ")+valueType2string(getType())).c_str())
  }




ref<value> 
value::operatorBinary(operator_id op,expression const &op2,context const &ctx) const {
  if (op2.evaluate(ctx)->getType() == VT_NULL) return makeConstant(false);
  EXJS_THROWINFO(ECJS_INVALID_OPERATION,(string(operator2string(op))+_(" on ")+valueType2string(getType())).c_str())
  }




ref<value> 
value::operatorUnaryModifying(operator_id op) {
  EXJS_THROWINFO(ECJS_INVALID_OPERATION,(string(operator2string(op))+_(" on ")+valueType2string(getType())).c_str())
  }




ref<value> 
value::operatorBinaryModifying(operator_id op,expression const &op2,context const &ctx) {
  EXJS_THROWINFO(ECJS_INVALID_OPERATION,(string(operator2string(op))+_(" on ")+valueType2string(getType())).c_str())
  }




value::operator_id 
value::token2operator(scanner::token const &token,bool unary,bool prefix) {
  switch (token.Type) {
    case TT_JS_INCREMENT: return prefix ? OP_PRE_INCREMENT : OP_POST_INCREMENT;
    case TT_JS_DECREMENT: return prefix ? OP_PRE_DECREMENT : OP_POST_DECREMENT;
    case '+': return unary ? OP_UNARY_PLUS : OP_PLUS;
    case '-': return unary ? OP_UNARY_MINUS : OP_MINUS;
    case '!': return OP_LOG_NOT;
    case '~': return OP_BIN_NOT;
    case TT_JS_PLUS_ASSIGN: return OP_PLUS_ASSIGN;
    case TT_JS_MINUS_ASSIGN: return OP_MINUS_ASSIGN;
    case TT_JS_MULTIPLY_ASSIGN: return OP_MUTLIPLY_ASSIGN;
    case TT_JS_DIVIDE_ASSIGN: return OP_DIVIDE_ASSIGN;
    case TT_JS_MODULO_ASSIGN: return OP_MODULO_ASSIGN;
    case TT_JS_BIT_AND_ASSIGN: return OP_BIT_AND_ASSIGN;
    case TT_JS_BIT_OR_ASSIGN: return OP_BIT_OR_ASSIGN;
    case TT_JS_BIT_XOR_ASSIGN: return OP_BIT_XOR_ASSIGN;
    case TT_JS_LEFT_SHIFT_ASSIGN: return OP_LEFT_SHIFT_ASSIGN;
    case TT_JS_RIGHT_SHIFT_ASSIGN: return OP_RIGHT_SHIFT_ASSIGN;
    case '*': return OP_MULTIPLY;
    case '/': return OP_DIVIDE;
    case '%': return OP_MODULO;
    case '&': return OP_BIT_AND;
    case '|': return OP_BIT_OR;
    case '^': return OP_BIT_XOR;
    case TT_JS_LEFT_SHIFT: return OP_LEFT_SHIFT;
    case TT_JS_RIGHT_SHIFT: return OP_RIGHT_SHIFT;
    case TT_JS_LOGICAL_OR: return OP_LOGICAL_OR;
    case TT_JS_LOGICAL_AND: return OP_LOGICAL_AND;
    case TT_JS_EQUAL: return OP_EQUAL;
    case TT_JS_NOT_EQUAL: return OP_NOT_EQUAL;
    case TT_JS_IDENTICAL: return OP_IDENTICAL;
    case TT_JS_NOT_IDENTICAL: return OP_NOT_IDENTICAL;
    case TT_JS_LESS_EQUAL: return OP_LESS_EQUAL;
    case TT_JS_GREATER_EQUAL: return OP_GREATER_EQUAL;
    case '<': return OP_LESS;
    case '>': return OP_GREATER;
    default: EXJS_THROWINFO(ECJS_UNKNOWN_OPERATOR,token.Text.c_str())
    }
  }




char *
value::operator2string(operator_id op) {
  switch (op) {
    case OP_PRE_INCREMENT: return _("prefix ++");
    case OP_POST_INCREMENT: return _("postfix ++");
    case OP_PRE_DECREMENT: return _("prefix --");
    case OP_POST_DECREMENT: return _("postfix ++");

    case OP_UNARY_PLUS: return _("unary +");
    case OP_UNARY_MINUS: return _("unary -");
    case OP_LOG_NOT: return "!";
    case OP_BIN_NOT: return "~";

    case OP_PLUS_ASSIGN: return "+=";
    case OP_MINUS_ASSIGN: return "-=";
    case OP_MUTLIPLY_ASSIGN: return "*=";
    case OP_DIVIDE_ASSIGN: return "/=";
    case OP_MODULO_ASSIGN: return "%=";

    case OP_BIT_AND_ASSIGN: return "&=";
    case OP_BIT_OR_ASSIGN: return "|=";
    case OP_BIT_XOR_ASSIGN: return "^=";
    case OP_LEFT_SHIFT_ASSIGN: return "<<=";
    case OP_RIGHT_SHIFT_ASSIGN: return ">>=";

    case OP_PLUS: return "+";
    case OP_MINUS: return "-";
    case OP_MULTIPLY: return "*";
    case OP_DIVIDE: return "/";
    case OP_MODULO: return "%";

    case OP_BIT_AND: return "&";
    case OP_BIT_OR: return "|";
    case OP_BIT_XOR: return "^";
    case OP_LEFT_SHIFT: return "<<";
    case OP_RIGHT_SHIFT: return ">>";

    case OP_LOGICAL_OR: return "|";
    case OP_LOGICAL_AND: return "&";
    case OP_EQUAL: return "==";
    case OP_NOT_EQUAL: return "!=";
    case OP_IDENTICAL: return "===";
    case OP_NOT_IDENTICAL: return "!==";
    case OP_LESS_EQUAL: return "<=";
    case OP_GREATER_EQUAL: return ">=";
    case OP_LESS: return "<";
    case OP_GREATER: return ">";

    case OP_ASSIGN: return "=";
    default: EXJS_THROW(ECJS_UNKNOWN_OPERATOR)
    }
  }




char *value::valueType2string(value_type vt) {
  switch (vt) {
    case VT_UNDEFINED: return _("undefined");
    case VT_NULL: return _("null");
    case VT_INTEGER: return _("integer");
    case VT_FLOATING_POINT: return _("floating point");
    case VT_STRING: return _("string");
    case VT_FUNCTION: return _("function");
    case VT_OBJECT: return _("object");
    case VT_BUILTIN: return _("built-in object");
    case VT_HOST: return _("host object");
    case VT_SCOPE: return _("scope");
    case VT_BOUND_METHOD: return _("bound method");
    case VT_TYPE: return _("type");
    default: return _("unknown value type");
    }
  }




// value_with_methods ---------------------------------------------------------
value_with_methods::method::
method(string const &identifier,value_with_methods *parent)
  : Identifier(identifier),Parent(parent),ParentRef(parent) {
  }




ref<value> 
value_with_methods::method::
call(context const &ctx,parameter_list const &parameters) const {
  return Parent->callMethod(Identifier,ctx,parameters);
  }




ref<value> 
value_with_methods::
lookup(string const &identifier) {
  ref<value> result = new method(identifier,this);
  EX_MEMCHECK(result.get())
  return result;
  }




// null -----------------------------------------------------------------------
value::value_type null::getType() const {
  return VT_NULL;
  }




string null::toString() const {
  return "null";
  }




bool null::toBoolean() const {
  return false;
  }




ref<value> null::duplicate() const {
  return makeNull();
  }




ref<value> null::operatorBinary(operator_id op,expression const &op2,context const &ctx) const {
  if (op == OP_EQUAL) {
    ref<value> op2_val = op2.evaluate(ctx);
    return makeConstant(op2_val->getType() == VT_NULL);
    }
  else 
    return value::operatorBinary(op,op2,ctx);
  }




// const_floating_point -------------------------------------------------------
const_floating_point::const_floating_point(double value)
  : Value(value) {
  }




value::value_type const_floating_point::getType() const {
  return VT_FLOATING_POINT;
  }




int const_floating_point::toInt() const {
  return (int) Value;
  }




double const_floating_point::toFloat() const {
  return (double) Value;
  }




bool const_floating_point::toBoolean() const {
  return (bool) Value;
  }




string const_floating_point::toString() const {
  return float2dec(Value);
  }




ref<value> const_floating_point::duplicate() const {
  return makeValue(Value);
  }




ref<value> 
const_floating_point::
callMethod(string const &identifier,context const &ctx,parameter_list const &parameters) {
  IXLIB_JS_IF_METHOD("toInt",0,0)
    return makeConstant((signed long) Value);
  IXLIB_JS_IF_METHOD("toString",0,1) {
    unsigned radix = 10;
    if (parameters.size() == 1) radix = parameters[0]->toInt();
    if (radix == 10)
      return makeConstant(float2dec(Value));
    else
      return makeConstant(signed2base((int) Value,0,radix));
    }
  IXLIB_JS_IF_METHOD("toFixed",0,1) {
    unsigned digits = 0;
    if (parameters.size() == 1) digits = parameters[0]->toInt();
    
    char buffer[1024];
    sprintf(buffer,("%."+unsigned2dec(digits)+"f").c_str(),Value);
    return makeConstant(buffer);
    }
  IXLIB_JS_IF_METHOD("toExponential",0,1) {
    char buffer[1024];
    if (parameters.size() == 1) 
      sprintf(buffer,("%."+unsigned2dec(parameters[0]->toInt())+"e").c_str(),Value);
    else
      sprintf(buffer,"%e",Value);
    return makeConstant(buffer);
    }
  IXLIB_JS_IF_METHOD("toPrecision",0,1) {
    if (parameters.size() == 1)
      return makeConstant(float2dec(Value,parameters[0]->toInt()));
    else
      return makeConstant(float2dec(Value));
    }
  EXJS_THROWINFO(ECJS_UNKNOWN_IDENTIFIER,("float." + identifier).c_str())
  }




ref<value> 
const_floating_point::operatorUnary(operator_id op) const {
  switch (op) {
    case OP_UNARY_PLUS: return makeConstant(+Value);
    case OP_UNARY_MINUS: return makeConstant(-Value);
    case OP_LOG_NOT: return makeConstant(!Value);
    case OP_BIN_NOT: return makeConstant(~ (long) Value);
    default:
      return super::operatorUnary(op);
    }
  }




ref<value> 
const_floating_point::operatorBinary(operator_id op,expression const &op2,context const &ctx) const {
  #define PROMOTING_OPERATOR(OP) { \
    ref<value> op2val = op2.evaluate(ctx); \
    if (op2val->getType() == VT_NULL) return makeConstant(false); \
    if (op2val->getType() == VT_STRING) \
      return makeConstant(toString() OP op2val->toString()); \
    else \
      return makeConstant(Value OP op2val->toFloat()); \
    }

  if (op == OP_LOGICAL_OR) 
    return makeConstant(toBoolean() || op2.evaluate(ctx)->toBoolean());
  if (op == OP_LOGICAL_AND)
    return makeConstant(toBoolean() && op2.evaluate(ctx)->toBoolean());

  switch (op) {
    case OP_PLUS: PROMOTING_OPERATOR(+)
    case OP_MINUS: return makeConstant(Value - op2.evaluate(ctx)->toFloat());
    case OP_MULTIPLY: return makeConstant(Value * op2.evaluate(ctx)->toFloat());
    case OP_DIVIDE: return makeConstant(Value / op2.evaluate(ctx)->toFloat());
    case OP_EQUAL: PROMOTING_OPERATOR(==)
    case OP_NOT_EQUAL: PROMOTING_OPERATOR(!=)
    case OP_LESS_EQUAL: PROMOTING_OPERATOR(<=)
    case OP_GREATER_EQUAL: PROMOTING_OPERATOR(>=)
    case OP_LESS: PROMOTING_OPERATOR(<)
    case OP_GREATER: PROMOTING_OPERATOR(>)
    default:
      return super::operatorBinary(op,op2,ctx);
    }
  #undef PROMOTING_OPERATOR
  }




// floating_point -------------------------------------------------------------
floating_point::floating_point(double val) 
  : const_floating_point(val) {
  }




ref<value> 
floating_point::operatorUnaryModifying(operator_id op) {
  switch (op) {
    case OP_PRE_INCREMENT: 
      Value++; 
      return ref<value>(this);
    case OP_POST_INCREMENT: 
      // *** FIXME this should be an lvalue
      return makeConstant(Value++);
    case OP_PRE_DECREMENT:
      Value--;
      return ref<value>(this);
    case OP_POST_DECREMENT:
      // *** FIXME this should be an lvalue
      return makeConstant(Value--);
    default:
      return super::operatorUnaryModifying(op);
    }
  }




ref<value> 
floating_point::operatorBinaryModifying(operator_id op,expression const &op2,context const &ctx) {
  int val;
  switch (op) {
    case OP_PLUS_ASSIGN:
      Value += op2.evaluate(ctx)->toFloat();
      return ref<value>(this);
    case OP_MINUS_ASSIGN:
      Value -= op2.evaluate(ctx)->toFloat();
      return ref<value>(this);
    case OP_MUTLIPLY_ASSIGN:
      Value *= op2.evaluate(ctx)->toFloat();
      return ref<value>(this);
    case OP_DIVIDE_ASSIGN:
      Value /= op2.evaluate(ctx)->toFloat();
      return ref<value>(this);
    case OP_MODULO_ASSIGN:
      val = (int) Value;
      val %= (int) op2.evaluate(ctx)->toFloat();
      Value = val;
      return ref<value>(this);
    case OP_BIT_AND_ASSIGN:
      val = (int) Value;
      val &= (int) op2.evaluate(ctx)->toFloat();
      Value = val;
      return ref<value>(this);
    case OP_BIT_OR_ASSIGN:
      val = (int) Value;
      val |= (int) op2.evaluate(ctx)->toFloat();
      Value = val;
      return ref<value>(this);
    case OP_BIT_XOR_ASSIGN:
      val = (int) Value;
      val ^= (int) op2.evaluate(ctx)->toFloat();
      Value = val;
      return ref<value>(this);
    case OP_LEFT_SHIFT_ASSIGN:
      val = (int) Value;
      val <<= (int) op2.evaluate(ctx)->toFloat();
      Value = val;
      return ref<value>(this);
    case OP_RIGHT_SHIFT_ASSIGN:
      val = (int) Value;
      val >>= (int) op2.evaluate(ctx)->toFloat();
      Value = val;
      return ref<value>(this);
    default:
      return super::operatorBinaryModifying(op,op2,ctx);
    }
  }




// const_integer --------------------------------------------------------------
const_integer::const_integer(long value)
  : Value(value) {
  }




value::value_type const_integer::getType() const {
  return VT_INTEGER;
  }




int const_integer::toInt() const {
  return (int) Value;
  }




double const_integer::toFloat() const {
  return (double) Value;
  }




bool const_integer::toBoolean() const {
  return (bool) Value;
  }




string const_integer::toString() const {
  return float2dec(Value);
  }




ref<value> const_integer::duplicate() const {
  return makeValue(Value);
  }




ref<value> 
const_integer::
callMethod(string const &identifier,context const &ctx,parameter_list const &parameters) {
  IXLIB_JS_IF_METHOD("toFloat",0,0) 
    return makeConstant((double) Value);
  IXLIB_JS_IF_METHOD("toString",0,1) {
    unsigned radix = 10;
    if (parameters.size() == 1) radix = parameters[0]->toInt();
    return makeConstant(signed2base(Value,0,radix));
    }
  EXJS_THROWINFO(ECJS_UNKNOWN_IDENTIFIER,("integer." + identifier).c_str())
  }




ref<value> 
const_integer::operatorUnary(operator_id op) const {
  switch (op) {
    case OP_UNARY_PLUS: return makeConstant(+Value);
    case OP_UNARY_MINUS: return makeConstant(-Value);
    case OP_LOG_NOT: return makeConstant(!Value);
    case OP_BIN_NOT: return makeConstant(~ (long) Value);
    default:
      return super::operatorUnary(op);
    }
  }




ref<value> 
const_integer::operatorBinary(operator_id op,expression const &op2,context const &ctx) const {
  #define STRING_OPERATOR(OP) \
    if (op2val->getType() == VT_STRING) \
      return makeConstant(toString() OP op2val->toString());
  #define FLOAT_OPERATOR(OP) \
    if (op2val->getType() == VT_FLOATING_POINT) \
      return makeConstant(toFloat() OP op2val->toFloat());
  #define OPERATOR(OP) \
    return makeConstant(Value OP op2val->toInt());

  if (op == OP_LOGICAL_OR) 
    return makeConstant(toBoolean() || op2.evaluate(ctx)->toBoolean());
  if (op == OP_LOGICAL_AND)
    return makeConstant(toBoolean() && op2.evaluate(ctx)->toBoolean());

  ref<value> op2val = op2.evaluate(ctx); \
  switch (op) {
    case OP_PLUS: 
      STRING_OPERATOR(+)
      FLOAT_OPERATOR(+)
      OPERATOR(+)
    case OP_MINUS: 
      FLOAT_OPERATOR(-)
      OPERATOR(-)
    case OP_MULTIPLY: 
      FLOAT_OPERATOR(*)
      OPERATOR(*)
    case OP_DIVIDE: 
      FLOAT_OPERATOR(/)
      OPERATOR(/)
    case OP_MODULO: return makeConstant((long) Value % op2.evaluate(ctx)->toInt());
    case OP_BIT_AND: return makeConstant((long) Value & op2.evaluate(ctx)->toInt());
    case OP_BIT_OR: return makeConstant((long) Value | op2.evaluate(ctx)->toInt());
    case OP_BIT_XOR: return makeConstant((long) Value ^ op2.evaluate(ctx)->toInt());
    case OP_LEFT_SHIFT: return makeConstant((long) Value << op2.evaluate(ctx)->toInt());
    case OP_RIGHT_SHIFT: return makeConstant((long) Value >> op2.evaluate(ctx)->toInt());
    case OP_EQUAL: 
      STRING_OPERATOR(==)
      OPERATOR(==)
    case OP_NOT_EQUAL: 
      STRING_OPERATOR(!=)
      OPERATOR(!=)
    case OP_LESS_EQUAL: 
      STRING_OPERATOR(<=)
      OPERATOR(<=)
    case OP_GREATER_EQUAL: 
      STRING_OPERATOR(>=)
      OPERATOR(>=)
    case OP_LESS: 
      STRING_OPERATOR(<)
      OPERATOR(<)
    case OP_GREATER: 
      STRING_OPERATOR(>)
      OPERATOR(>)
    default:
      return super::operatorBinary(op,op2,ctx);
    }
  #undef PROMOTING_OPERATOR
  }




// integer --------------------------------------------------------------------
integer::integer(long val) 
  : const_integer(val) {
  }




ref<value> 
integer::operatorUnaryModifying(operator_id op) {
  switch (op) {
    case OP_PRE_INCREMENT: 
      Value++; 
      return ref<value>(this);
    case OP_POST_INCREMENT: 
      // *** FIXME this should be an lvalue
      return makeConstant(Value++);
    case OP_PRE_DECREMENT:
      Value--;
      return ref<value>(this);
    case OP_POST_DECREMENT:
      // *** FIXME this should be an lvalue
      return makeConstant(Value--);
    default:
      return super::operatorUnaryModifying(op);
    }
  }




ref<value> 
integer::operatorBinaryModifying(operator_id op,expression const &op2,context const &ctx) {
  int val;
  switch (op) {
    case OP_PLUS_ASSIGN:
      Value += op2.evaluate(ctx)->toInt();
      return ref<value>(this);
    case OP_MINUS_ASSIGN:
      Value -= op2.evaluate(ctx)->toInt();
      return ref<value>(this);
    case OP_MUTLIPLY_ASSIGN:
      Value *= op2.evaluate(ctx)->toInt();
      return ref<value>(this);
    case OP_DIVIDE_ASSIGN:
      Value /= op2.evaluate(ctx)->toInt();
      return ref<value>(this);
    case OP_MODULO_ASSIGN:
      val = (int) Value;
      val %= (int) op2.evaluate(ctx)->toInt();
      Value = val;
      return ref<value>(this);
    case OP_BIT_AND_ASSIGN:
      val = (int) Value;
      val &= (int) op2.evaluate(ctx)->toInt();
      Value = val;
      return ref<value>(this);
    case OP_BIT_OR_ASSIGN:
      val = (int) Value;
      val |= (int) op2.evaluate(ctx)->toInt();
      Value = val;
      return ref<value>(this);
    case OP_BIT_XOR_ASSIGN:
      val = (int) Value;
      val ^= (int) op2.evaluate(ctx)->toInt();
      Value = val;
      return ref<value>(this);
    case OP_LEFT_SHIFT_ASSIGN:
      val = (int) Value;
      val <<= (int) op2.evaluate(ctx)->toInt();
      Value = val;
      return ref<value>(this);
    case OP_RIGHT_SHIFT_ASSIGN:
      val = (int) Value;
      val >>= (int) op2.evaluate(ctx)->toInt();
      Value = val;
      return ref<value>(this);
    default:
      return super::operatorBinaryModifying(op,op2,ctx);
    }
  }




// js_string ------------------------------------------------------------------
js_string::js_string(string const &value)
  : Value(value) {
  }




value::value_type js_string::getType() const {
  return VT_STRING;
  }




string js_string::toString() const {
  return Value;
  }




bool js_string::toBoolean() const {
  return Value.size() == 0;
  }




string js_string::stringify() const {
  return '"'+Value+'"';
  }




ref<value> js_string::duplicate() const {
  return makeValue(Value);
  }




ref<value> js_string::lookup(string const &identifier) {
  if (identifier == "length") return makeConstant(Value.size());
  return super::lookup(identifier);
  }




ref<value> js_string::callMethod(string const &identifier,context const &ctx,parameter_list const &parameters) {
  IXLIB_JS_IF_METHOD("charAt",1,1) 
    return makeConstant(string(1,Value.at(parameters[0]->toInt())));
  IXLIB_JS_IF_METHOD("charCodeAt",1,1) 
    return makeConstant(Value.at(parameters[0]->toInt()));
  if (identifier == "concat") {
    string result = Value;
    FOREACH_CONST(first,parameters,parameter_list) 
      Value += (*first)->toString();
    return makeConstant(Value);
    }
  IXLIB_JS_IF_METHOD("indexOf",1,2) {
    string::size_type startpos = 0;
    if (parameters.size() == 2) startpos = parameters[1]->toInt();
    string::size_type result = Value.find(parameters[0]->toString(),startpos);
    if (result == string::npos) return makeConstant(-1);
    else return makeConstant(result);
    }
  IXLIB_JS_IF_METHOD("lastIndexOf",1,2) {
    string::size_type startpos = string::npos;
    if (parameters.size() == 2) startpos = parameters[1]->toInt();
    string::size_type result = Value.rfind(parameters[0]->toString(),startpos);
    if (result == string::npos) return makeConstant(-1);
    else return makeConstant(result);
    }
  // *** FIXME we need proper regexps
  IXLIB_JS_IF_METHOD("match",1,1) {
    regex_string re(parameters[0]->toString());
    return makeConstant(re.matchAt(Value,0));
    }
  IXLIB_JS_IF_METHOD("replace",2,2) {
    regex_string re(parameters[0]->toString());
    return makeConstant(re.replaceAll(Value,parameters[1]->toString()));
    }
  IXLIB_JS_IF_METHOD("search",1,1) {
    regex_string re(parameters[0]->toString());
    return makeConstant(re.match(Value));
    }
  IXLIB_JS_IF_METHOD("slice",2,2) {
    TIndex start = parameters[0]->toInt(),end = parameters[1]->toInt();
    return makeConstant(string(Value,start,end-start));
    }
  // *** FIXME implement split
  IXLIB_JS_IF_METHOD("substring",2,2) {
    TIndex start = parameters[0]->toInt(),end = parameters[1]->toInt();
    if (start > end) swap(start,end);
    return makeConstant(string(Value,start,end-start));
    }
  IXLIB_JS_IF_METHOD("toLowerCase",0,0) {
    return makeConstant(lower(Value));
    }
  IXLIB_JS_IF_METHOD("toUpperCase",0,0) {
    return makeConstant(upper(Value));
    }
  EXJS_THROWINFO(ECJS_UNKNOWN_IDENTIFIER,("String." + identifier).c_str())
  }




ref<value> js_string::operatorBinary(operator_id op,expression const &op2,context const &ctx) const {
  switch (op) {
    case OP_PLUS: return makeConstant(Value+op2.evaluate(ctx)->toString());
    case OP_EQUAL: {
      ref<value> op2val = op2.evaluate(ctx);
      if (op2val->getType() == VT_NULL) return makeConstant(false);
      return makeConstant(Value == op2val->toString());
      }
    case OP_NOT_EQUAL: return makeConstant(Value != op2.evaluate(ctx)->toString());
    case OP_LESS_EQUAL: return makeConstant(Value <= op2.evaluate(ctx)->toString());
    case OP_GREATER_EQUAL: return makeConstant(Value >= op2.evaluate(ctx)->toString());
    case OP_LESS: return makeConstant(Value < op2.evaluate(ctx)->toString());
    case OP_GREATER: return makeConstant(Value > op2.evaluate(ctx)->toString());
    default:
      return super::operatorBinary(op,op2,ctx);
    }
  }




ref<value> js_string::operatorBinaryModifying(operator_id op,expression const &op2,context const &ctx) {
  switch (op) {
    case OP_PLUS_ASSIGN:
      Value += op2.evaluate(ctx)->toString();
      return ref<value>(this);
    default:
      return super::operatorBinaryModifying(op,op2,ctx);
    }
  }




// lvalue ---------------------------------------------------------------------
lvalue::lvalue(ref<value> ref)
  : Reference(ref) {
  }




value::value_type lvalue::getType() const {
  return Reference->getType();
  }




string lvalue::toString() const {
  return Reference->toString();
  }




int lvalue::toInt() const {
  return Reference->toInt();
  }




double lvalue::toFloat() const {
  return Reference->toFloat();
  }




bool lvalue::toBoolean() const {
  return Reference->toBoolean();
  }




string lvalue::stringify() const {
  return Reference->stringify();
  }




ref<value> lvalue::duplicate() const {
  return Reference->duplicate();
  }




ref<value> lvalue::lookup(string const &identifier) {
  return Reference->lookup(identifier);
  }




ref<value> lvalue::subscript(value const &index) {
  return Reference->subscript(index);
  }




ref<value> lvalue::call(context const &ctx,parameter_list const &parameters) const {
  return Reference->call(ctx,parameters);
  }




ref<value> lvalue::assign(ref<value> op2) {
  Reference = op2;
  return this;
  }




ref<value> lvalue::operatorUnary(operator_id op) const {
  return Reference->operatorUnary(op);
  }




ref<value> lvalue::operatorBinary(operator_id op,expression const &op2,context const &ctx) const {
  return Reference->operatorBinary(op,op2,ctx);
  }




#define LVALUE_RETURN(VALUE) \
  ref<value> __result = VALUE; \
  if (__result.get() == Reference.get()) \
    return this; \
  else \
    return __result;




ref<value> lvalue::operatorUnaryModifying(operator_id op)  {
  LVALUE_RETURN(Reference->operatorUnaryModifying(op))
  }




ref<value> lvalue::operatorBinaryModifying(operator_id op,expression const &op2,context const &ctx) {
  LVALUE_RETURN(Reference->operatorBinaryModifying(op,op2,ctx))
  }




// constant_wrapper -----------------------------------------------------------
constant_wrapper::constant_wrapper(ref<value> val)
  : Constant(val) {
  }




value::value_type constant_wrapper::getType() const {
  return Constant->getType();
  }




string constant_wrapper::toString() const {
  return Constant->toString();
  }




int constant_wrapper::toInt() const {
  return Constant->toInt();
  }




double constant_wrapper::toFloat() const {
  return Constant->toFloat();
  }




bool constant_wrapper::toBoolean() const {
  return Constant->toBoolean();
  }




string constant_wrapper::stringify() const {
  return Constant->stringify();
  }




ref<value> 
constant_wrapper::duplicate() const {
  return Constant->duplicate();
  }



	  
ref<value> 
constant_wrapper::lookup(string const &identifier) {
  return Constant->lookup(identifier);
  }




ref<value> 
constant_wrapper::subscript(value const &index) {
  return Constant->subscript(index);
  }




ref<value> 
constant_wrapper::call(context const &ctx,parameter_list const &parameters) const {
  return Constant->call(ctx,parameters);
  }




ref<value> 
constant_wrapper::assign(ref<value> value) {
  EXJS_THROWINFO(ECJS_CANNOT_MODIFY_RVALUE,_("by assignment"))
  }




ref<value> 
constant_wrapper::operatorUnary(operator_id op) const {
  return Constant->operatorUnary(op);
  }




ref<value> 
constant_wrapper::operatorBinary(operator_id op,expression const &op2,context const &ctx) const {
  return Constant->operatorBinary(op,op2,ctx);
  }




ref<value> 
constant_wrapper::operatorUnaryModifying(operator_id op) {
  EXJS_THROWINFO(ECJS_CANNOT_MODIFY_RVALUE,operator2string(op))
  }




ref<value> 
constant_wrapper::operatorBinaryModifying(operator_id op,expression const &op2,context const &ctx) {
  EXJS_THROWINFO(ECJS_CANNOT_MODIFY_RVALUE,operator2string(op))
  }




// list_scope -----------------------------------------------------------------
ref<value> 
list_scope::lookup(string const &identifier) {
  member_map::iterator item = MemberMap.find(identifier);
  if (item != MemberMap.end())
    return item->second;
  
  FOREACH_CONST(first,SwallowedList,swallowed_list) {
    try {
      return (*first)->lookup(identifier);
      }
    catch (...) { }
    }
  EXJS_THROWINFO(ECJS_UNKNOWN_IDENTIFIER,identifier.c_str())
  }




void list_scope::unite(ref<value> scope) {
  SwallowedList.push_back(scope);
  }




void list_scope::separate(ref<value> scope) {
  FOREACH(first,SwallowedList,swallowed_list) {
    if (*first == scope) {
      SwallowedList.erase(first);
      return;
      }
    }
  EXGEN_THROW(EC_ITEMNOTFOUND)
  }




void list_scope::clearScopes() {
  SwallowedList.clear();
  }




bool list_scope::hasMember(string const &name) const {
  member_map::const_iterator item = MemberMap.find(name);
  return item != MemberMap.end();
  }




void list_scope::addMember(string const &name,ref<value> member) {
  MemberMap[name] = member;
  }




void list_scope::removeMember(string const &name) {
  MemberMap.erase(name);
  }




void list_scope::clearMembers() {
  MemberMap.clear();
  }




void list_scope::clear() {
  clearScopes();
  clearMembers();
  }




// function -------------------------------------------------------------------
function::function(parameter_name_list const &pnames,ref<expression> body,ref<value> lex_scope)
  : ParameterNameList(pnames),Body(body),LexicalScope(lex_scope) {
  }




ref<value> function::duplicate() const {
  // functions are not mutable
  return const_cast<function *>(this);
  }




ref<value> 
function::
call(context const &ctx,parameter_list const &parameters) const {
  ref<list_scope,value> scope = new list_scope;
  EX_MEMCHECK(scope.get())
  scope->unite(LexicalScope);
  context inner_context(scope);
  
  parameter_list::const_iterator 
    firstp = parameters.begin(),
    lastp = parameters.end();
  
  FOREACH_CONST(first,ParameterNameList,parameter_name_list) {
    if (firstp == lastp)
      scope->addMember(*first,makeLValue(makeNull()));
    else
      scope->addMember(*first,makeLValue((*firstp)->duplicate()));
    
    firstp++;
    }
  
  ref<value> result;
  
  try {
    result = Body->evaluate(inner_context);
    }
  catch (return_exception &fr) {
    result = fr.ReturnValue;
    }
  catch (break_exception &be) {
    if (be.HasLabel)
      EXJS_THROWINFOLINE(ECJS_INVALID_NON_LOCAL_EXIT,("break "+be.Label).c_str(),be.Line)
    else
      EXJS_THROWINFOLINE(ECJS_INVALID_NON_LOCAL_EXIT,"break",be.Line)
    }
  catch (continue_exception &ce) {
    if (ce.HasLabel)
      EXJS_THROWINFOLINE(ECJS_INVALID_NON_LOCAL_EXIT,("continue "+ce.Label).c_str(),ce.Line)
    else
      EXJS_THROWINFOLINE(ECJS_INVALID_NON_LOCAL_EXIT,"continue",ce.Line)
    }
  if (result.get()) return result->duplicate();
  return ref<value>(NULL);
  
  // ATTENTION: this is a scope cancellation point.
  }




// value creation -------------------------------------------------------------
ref<value> 
javascript::makeUndefined() {
  // *** FIXME: this is non-compliant
  ref<value> result(new null());
  EX_MEMCHECK(result.get())
  return result;
  }




ref<value> 
javascript::makeNull() {
  ref<value> result(new null());
  EX_MEMCHECK(result.get())
  return result;
  }




ref<value> 
javascript::makeValue(bool val) {
  return makeConstant((int) val);
  }




ref<value> 
javascript::makeConstant(bool val) {
  return makeConstant((int) val);
  }




ref<value> 
javascript::makeValue(signed int val) {
  ref<value> result(new integer(val));
  EX_MEMCHECK(result.get())
  return result;
  }




ref<value> 
javascript::makeConstant(signed int val) {
  ref<value> result(new const_integer(val));
  EX_MEMCHECK(result.get())
  return result;
  }




ref<value> 
javascript::makeValue(unsigned int val) {
  ref<value> result(new integer(val));
  EX_MEMCHECK(result.get())
  return result;
  }




ref<value> 
javascript::makeConstant(unsigned int val) {
  ref<value> result(new const_integer(val));
  EX_MEMCHECK(result.get())
  return result;
  }




ref<value> 
javascript::makeValue(signed long val) {
  ref<value> result(new integer(val));
  EX_MEMCHECK(result.get())
  return result;
  }




ref<value> 
javascript::makeConstant(signed long val) {
  ref<value> result(new const_integer(val));
  EX_MEMCHECK(result.get())
  return result;
  }




ref<value> 
javascript::makeValue(unsigned long val) {
  ref<value> result(new integer(val));
  EX_MEMCHECK(result.get())
  return result;
  }




ref<value> 
javascript::makeConstant(unsigned long val) {
  ref<value> result(new const_integer(val));
  EX_MEMCHECK(result.get())
  return result;
  }




ref<value> 
javascript::makeValue(double val) {
  ref<value> result(new floating_point(val));
  EX_MEMCHECK(result.get())
  return result;
  }




ref<value> 
javascript::makeConstant(double val) {
  ref<value> result(new const_floating_point(val));
  EX_MEMCHECK(result.get())
  return result;
  }




ref<value> 
javascript::makeValue(string const &val) {
  ref<value> result(new js_string(val));
  EX_MEMCHECK(result.get())
  return result;
  }




ref<value> 
javascript::makeConstant(string const &val) {
  return wrapConstant(makeValue(val));
  }




ref<value> javascript::makeArray(TSize size) {
  auto_ptr<js_array> result(new js_array(size));
  EX_MEMCHECK(result.get())
  return result.release();
  }




ref<value> 
javascript::makeLValue(ref<value> target) {
  ref<value> result = new lvalue(target);
  EX_MEMCHECK(result.get())
  return result;
  }




ref<value> 
javascript::wrapConstant(ref<value> val) {
  ref<value> result(new constant_wrapper(val));
  EX_MEMCHECK(result.get())
  return result;
  }




ref<expression> 
javascript::makeConstantExpression(ref<value> val) {
  ref<expression> result(new constant(val));
  EX_MEMCHECK(result.get())
  return result;
  }
