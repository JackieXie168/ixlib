// ----------------------------------------------------------------------------
//  Description      : Javascript interpreter
// ----------------------------------------------------------------------------
//  (c) Copyright 2000 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <ixlib_js_internals.hh>
#include <ixlib_token_javascript.hh>




using namespace ixion;
using namespace javascript;




// constant -------------------------------------------------------------------
constant::constant(ref<value> val) 
  : Value(val) {
  }




ref<value> 
constant::
evaluate(context const &ctx) const {
  return Value;
  }




// unary_operator -------------------------------------------------
unary_operator::unary_operator(value::operator_id opt,ref<expression> opn)
  : Operator(opt),Operand(opn) {
  }




ref<value> 
unary_operator::
evaluate(context const &ctx) const {
  return Operand->evaluate(ctx)->operatorUnary(Operator);
  }




// modifying_unary_operator ---------------------------------------------------
modifying_unary_operator::
modifying_unary_operator(value::operator_id opt,ref<expression> opn)
  : Operator(opt),Operand(opn) {
  }




ref<value> 
modifying_unary_operator::
evaluate(context const &ctx) const {
  return Operand->evaluate(ctx)->operatorUnaryModifying(Operator);
  }




// binary_operator ------------------------------------------------------------
binary_operator::binary_operator(value::operator_id opt,ref<expression> opn1,ref<expression> opn2)
  : Operator(opt),Operand1(opn1),Operand2(opn2) {
  }



  
ref<value> binary_operator::evaluate(context const &ctx) const {
  return Operand1->evaluate(ctx)->operatorBinary(Operator,*Operand2,ctx);
  }




// modifying_binary_operator --------------------------------------
modifying_binary_operator::
modifying_binary_operator(value::operator_id opt,ref<expression> opn1,ref<expression> opn2)
  : Operator(opt),Operand1(opn1),Operand2(opn2) {
  }



  
ref<value> 
modifying_binary_operator::
evaluate(context const &ctx) const {
  return Operand1->evaluate(ctx)->operatorBinaryModifying(Operator,*Operand2,ctx);
  }




// ternary_operator -----------------------------------------------------------
ternary_operator::
ternary_operator(ref<expression> opn1,ref<expression> opn2,ref<expression> opn3)
  : Operand1(opn1),Operand2(opn2),Operand3(opn3) {
  }




ref<value> 
ternary_operator::
evaluate(context const &ctx) const {
  if (Operand1->evaluate(ctx)->toBoolean())
    return Operand2->evaluate(ctx);
  else
    return Operand3->evaluate(ctx);
  }




// subscript_operation --------------------------------------------------------
subscript_operation::subscript_operation(ref<expression> opn1,ref<expression> opn2)
  : Operand1(opn1),Operand2(opn2) {
  }



  
ref<value> subscript_operation::evaluate(context const &ctx) const {
  ref<value> op2 = Operand2->evaluate(ctx);
  return Operand1->evaluate(ctx)->subscript(*op2);
  }




// lookup_operation -----------------------------------------------------------
lookup_operation::lookup_operation(string const &id)
  : Identifier(id) {
  }




lookup_operation::lookup_operation(ref<expression> opn,string const &id)
  : Operand(opn),Identifier(id) {
  }




ref<value> lookup_operation::evaluate(context const &ctx) const {
  ref<value> scope(ctx.CurrentScope);
  if (Operand.get() != NULL)
    scope = Operand->evaluate(ctx);
  return scope->lookup(Identifier);
  }




// assignment -----------------------------------------------------------------
assignment::
assignment(ref<expression> opn1,ref<expression> opn2)
  : Operand1(opn1),Operand2(opn2) {
  }



  
ref<value> 
assignment::evaluate(context const &ctx) const {
  return Operand1->evaluate(ctx)->assign(Operand2->evaluate(ctx)->duplicate());
  }




// function_call --------------------------------------------------------------
function_call::function_call(ref<expression> fun,parameter_expression_list const &pexps)
  : Function(fun),ParameterExpressionList(pexps) {
  }




ref<value> function_call::evaluate(context const &ctx) const {
  ref<value> func_value = Function->evaluate(ctx);
  
  value::parameter_list pvalues;
  FOREACH_CONST(first,ParameterExpressionList,parameter_expression_list) {
    pvalues.push_back((*first)->evaluate(ctx));
    }
  
  return func_value->call(ctx,pvalues);
  }




// construction ---------------------------------------------------------------
construction::construction(ref<expression> cls,parameter_expression_list const &pexps)
  : Class(cls),ParameterExpressionList(pexps) {
  }




ref<value> construction::evaluate(context const &ctx) const {
  ref<value> class_value = Class->evaluate(ctx);
  
  value::parameter_list pvalues;
  FOREACH_CONST(first,ParameterExpressionList,parameter_expression_list) {
    pvalues.push_back((*first)->evaluate(ctx));
    }
  
  return class_value->construct(ctx,pvalues);
  }
