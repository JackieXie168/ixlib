// ----------------------------------------------------------------------------
//  Description      : Javascript interpreter library
// ----------------------------------------------------------------------------
//  (c) Copyright 2000 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#define _BSD_SOURCE
#define _ISOC9X_SOURCE




#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <ixlib_js_internals.hh>
#include <ixlib_numconv.hh>
#include <ixlib_random.hh>




using namespace ixion;
using namespace javascript;




namespace {
  class eval : public value {
    protected:
      interpreter	&Interpreter;
    
    public:
      value_type getType() const { 
        return VT_FUNCTION; 
        } 
      eval(interpreter &interpreter)
        : Interpreter(interpreter) {
	}
      ref<value> call(context const &ctx,parameter_list const &parameters) const;
    };
    
  class Math : public value_with_methods {
    private:
      typedef value_with_methods super;

    protected:
      float_random	RNG;
      
    public:
      value_type getType() const {
        return VT_BUILTIN;
	}
      
      ref<value> duplicate() const;

      ref<value> lookup(string const &identifier,bool want_lvalue);
      ref<value> callMethod(string const &identifier,context const &ctx,parameter_list const &parameters);
    };
  }
  



// eval -----------------------------------------------------------------------
ref<value> 
eval::
call(context const &ctx,parameter_list const &parameters) const {
  if (parameters.size() != 1) {
    EXJS_THROWINFO(ECJS_INVALID_NUMBER_OF_ARGUMENTS,"eval")
    }
  if (parameters[0]->getType() != VT_STRING) return parameters[0];
  return Interpreter.execute(parameters[0]->toString());
  }




// parseInt -------------------------------------------------------------------
IXLIB_JS_DECLARE_FUNCTION(parseInt) {
  if (parameters.size() != 1 && parameters.size() != 2) {
    EXJS_THROWINFO(ECJS_INVALID_NUMBER_OF_ARGUMENTS,"parseInt")
    }
  unsigned radix = 10;
  if (parameters.size() == 2)
    radix = parameters[1]->toInt();
  return makeConstantInt(evalSigned(parameters[0]->toString(),radix));
  }




// parseFloat -----------------------------------------------------------------
IXLIB_JS_DECLARE_FUNCTION(parseFloat) {
  if (parameters.size() != 1) {
    EXJS_THROWINFO(ECJS_INVALID_NUMBER_OF_ARGUMENTS,"parseFloat")
    }
  return makeConstantFloat(evalFloat(parameters[0]->toString()));
  }




// isNaN ----------------------------------------------------------------------
IXLIB_JS_DECLARE_FUNCTION(isNaN) {
  if (parameters.size() != 1) {
    EXJS_THROWINFO(ECJS_INVALID_NUMBER_OF_ARGUMENTS,"isNaN")
    }
  int classification = fpclassify(parameters[0]->toFloat());
  return makeConstantBoolean(classification == FP_NAN);
  }




// isFinite -------------------------------------------------------------------
IXLIB_JS_DECLARE_FUNCTION(isFinite) {
  if (parameters.size() != 1) {
    EXJS_THROWINFO(ECJS_INVALID_NUMBER_OF_ARGUMENTS,"isFinite")
    }
  int classification = fpclassify(parameters[0]->toFloat());
  return makeConstantBoolean(classification != FP_NAN && classification != FP_INFINITE);
  }




// Math -----------------------------------------------------------------------
ref<value> Math::duplicate() const {
  // Math is not mutable
  return const_cast<Math *>(this);
  }




ref<value> Math::lookup(string const &identifier,bool want_lvalue) {
  #define MATH_CONSTANT(NAME,VALUE) \
    if (identifier == NAME) return makeConstantFloat(VALUE);
  
  MATH_CONSTANT("E",M_E)
  MATH_CONSTANT("LN10",M_LN10)
  MATH_CONSTANT("LN2",M_LN2)
  MATH_CONSTANT("LOG2E",M_LOG2E)  
  MATH_CONSTANT("LOG10E,",M_LOG10E)
  MATH_CONSTANT("PI",M_PI)
  MATH_CONSTANT("SQRT1_2",M_SQRT1_2)
  MATH_CONSTANT("SQRT2",M_SQRT2)

  return super::lookup(identifier,want_lvalue);
  }




ref<value> Math::callMethod(string const &identifier,context const &ctx,parameter_list const &parameters) {
  #define MATH_FUNCTION(NAME,C_NAME) \
    if (identifier == NAME) { \
      if (parameters.size() != 1) { \
        EXJS_THROWINFO(ECJS_INVALID_NUMBER_OF_ARGUMENTS,"Math." NAME) \
        } \
      return makeConstantFloat(C_NAME(parameters[0]->toFloat())); \
      }
  
  MATH_FUNCTION("abs",fabs)
  MATH_FUNCTION("acos",acos)
  MATH_FUNCTION("asin",asin)
  MATH_FUNCTION("atan",atan)
  MATH_FUNCTION("ceil",ceil)
  MATH_FUNCTION("cos",cos)
  MATH_FUNCTION("exp",exp)
  MATH_FUNCTION("floor",floor)
  MATH_FUNCTION("log",log)
  MATH_FUNCTION("round",round)
  MATH_FUNCTION("sin",sin)
  MATH_FUNCTION("sqrt",sqrt)
  MATH_FUNCTION("tan",tan)
  if (identifier == "atan2") {
    if (parameters.size() != 2) {
      EXJS_THROWINFO(ECJS_INVALID_NUMBER_OF_ARGUMENTS,"Math.atan2")
      }
    return makeConstantFloat(atan2(parameters[0]->toFloat(),parameters[1]->toFloat()));
    }
  if (identifier == "pow") {
    if (parameters.size() != 2) {
      EXJS_THROWINFO(ECJS_INVALID_NUMBER_OF_ARGUMENTS,"Math.pow")
      }
    return makeConstantFloat(pow(parameters[0]->toFloat(),parameters[1]->toFloat()));
    }
  if (identifier == "random") {
    if (parameters.size() != 0) {
      EXJS_THROWINFO(ECJS_INVALID_NUMBER_OF_ARGUMENTS,"Math.random")
      }
    return makeConstantFloat(RNG());
    }
  
  // *** FIXME: implement max, min
  EXJS_THROWINFO(ECJS_UNKNOWN_IDENTIFIER,("Math." + identifier).c_str())
  }




// external interface functions -----------------------------------------------
#define ADD_GLOBAL_OBJECT(NAME,TYPE) \
  { ref<value> x = new TYPE(); \
    EX_MEMCHECK(x.get()) \
    ip.RootScope.addMember(NAME,x); \
    }
  



void javascript::addGlobal(interpreter &ip) {
  ref<value> ev = new eval(ip);
  EX_MEMCHECK(ev.get())
  ip.RootScope.addMember("eval",ev);

  ADD_GLOBAL_OBJECT("parseInt",parseInt)
  ADD_GLOBAL_OBJECT("parseFloat",parseFloat)
  ADD_GLOBAL_OBJECT("isNaN",isNaN)
  ADD_GLOBAL_OBJECT("isFinite",isFinite)
  
  ip.RootScope.addMember("NaN",makeConstantFloat(NAN));
  ip.RootScope.addMember("Infinity",makeConstantFloat(INFINITY));
  ip.RootScope.addMember("undefined",makeUndefined());
  }




void javascript::addMath(interpreter &ip) {
  ADD_GLOBAL_OBJECT("Math",Math)
  }




void javascript::addStandardLibrary(interpreter &ip) {
  addGlobal(ip);
  addMath(ip);
  }
