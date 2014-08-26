// ----------------------------------------------------------------------------
//  Description      : Javascript interpreter
// ----------------------------------------------------------------------------
//  (c) Copyright 2000 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <ixlib_js_internals.hh>
#include <ixlib_token_javascript.hh>




using namespace ixion;
using namespace javascript;




// variable_declaration -------------------------------------------------------
variable_declaration::variable_declaration(string const &id,ref<expression> def_value)
  : Identifier(id),DefaultValue(def_value) {
  }




ref<value> variable_declaration::evaluate(context const &ctx,bool want_lvalue) const {
  ref<value> def;
  if (DefaultValue.get() != NULL) def = DefaultValue->evaluate(ctx)->duplicate();
  else def = makeNull();
  
  ctx.CurrentScope->addMember(Identifier,def);
  return ref<value>(ctx.CurrentScope->lookup(Identifier,true));
  }




// function_declaration -------------------------------------------
function_declaration::
function_declaration(string const &id,parameter_name_list const &pnames,
ref<expression> body)
  : Identifier(id),ParameterNameList(pnames),Body(body) {
  }




ref<value> function_declaration::evaluate(context const &ctx,bool want_lvalue) const {
  ref<value> fun = new function(ParameterNameList,Body);
  EX_MEMCHECK(fun.get())
  ctx.CurrentScope->addMember(Identifier,fun);
  return ref<value>(NULL);
  }




// instruction_list -----------------------------------------------------------
ref<value> 
instruction_list::evaluate(context const &ctx,bool want_lvalue) const {
  ref<value> result;
  FOREACH_CONST(first,ExpressionList,expression_list)
    result = (*first)->evaluate(ctx);
  return result;
  }




void instruction_list::add(ref<expression> expr) {
  ExpressionList.push_back(expr);
  }




// scoped_instruction_list ----------------------------------------
ref<value> scoped_instruction_list::evaluate(context const &ctx,bool want_lvalue) const {
  list_scope scope;
  scope.unite(ctx.CurrentScope);
  no_free_ref<value> scope_ref(&scope);
  context inner_context(ctx,&scope);
  ref<value> result = instruction_list::evaluate(inner_context,want_lvalue);
  if (result.get()) return result->duplicate();
  return ref<value>(NULL);
  // ATTENTION: this is a scope cancellation point.
  // Therefore any values passed out must be made lvalue-free, i.e. duplicated
  }




// js_if ----------------------------------------------------------------------
js_if::js_if(ref<expression> cond,ref<expression> ifex,ref<expression> ifnotex)
  : Conditional(cond),IfExpression(ifex),IfNotExpression(ifnotex) {
  }




ref<value> js_if::evaluate(context const &ctx,bool want_lvalue) const {
  if (Conditional->evaluate(ctx)->toBoolean())
    return IfExpression->evaluate(ctx);
  else 
    if (IfNotExpression.get()) 
      return IfNotExpression->evaluate(ctx);
    else 
      return ref<value>(NULL);
  }




// js_while -------------------------------------------------------------------
js_while::js_while(ref<expression> cond,ref<expression> loopex)
  : Conditional(cond),LoopExpression(loopex),HasLabel(false) {
  }




js_while::js_while(ref<expression> cond,ref<expression> loopex,string const &label)
  : Conditional(cond),LoopExpression(loopex),HasLabel(true),Label(label) {
  }




ref<value> js_while::evaluate(context const &ctx,bool want_lvalue) const {
  ref<value> result;
  while (Conditional->evaluate(ctx)->toBoolean()) {
    try {
      result = LoopExpression->evaluate(ctx);
      }
    catch (break_exception &be) {
      if (!be.HasLabel || (HasLabel && be.HasLabel && be.Label == Label))
        break;
      else throw;
      }
    catch (continue_exception &ce) {
      if (!ce.HasLabel || (HasLabel && ce.HasLabel && ce.Label == Label))
        continue;
      else throw;
      }
    }
  return result;
  }




// js_do_while ----------------------------------------------------------------
js_do_while::js_do_while(ref<expression> cond,ref<expression> loopex)
  : Conditional(cond),LoopExpression(loopex),HasLabel(false) {
  }




js_do_while::js_do_while(ref<expression> cond,ref<expression> loopex,string const &label)
  : Conditional(cond),LoopExpression(loopex),HasLabel(true),Label(label) {
  }




ref<value> js_do_while::evaluate(context const &ctx,bool want_lvalue) const {
  ref<value> result;
  do {
    try {
      result = LoopExpression->evaluate(ctx);
      }
    catch (break_exception &be) {
      if (!be.HasLabel || (HasLabel && be.HasLabel && be.Label == Label))
        break;
      else throw;
      }
    catch (continue_exception &ce) {
      if (!ce.HasLabel || (HasLabel && ce.HasLabel && ce.Label == Label))
        continue;
      else throw;
      }
  } while (Conditional->evaluate(ctx)->toBoolean());
  return result;
  }




// js_for ---------------------------------------------------------------------
js_for::js_for(ref<expression> init,ref<expression> cond,ref<expression> update,ref<expression> loop)
  : Initialization(init),Conditional(cond),Update(update),LoopExpression(loop),
  HasLabel(false) {
  }




js_for::js_for(ref<expression> init,ref<expression> cond,ref<expression> update,ref<expression> loop,string const &label)
  : Initialization(init),Conditional(cond),Update(update),LoopExpression(loop),
  HasLabel(true),Label(label) {
  }




ref<value> js_for::evaluate(context const &ctx,bool want_lvalue) const {
  ref<value> result;
  for (Initialization->evaluate(ctx);Conditional->evaluate(ctx)->toBoolean();
  Update->evaluate(ctx)) {
    try {
      result = LoopExpression->evaluate(ctx);
      }
    catch (break_exception &be) {
      if (!be.HasLabel || (HasLabel && be.HasLabel && be.Label == Label))
        break;
      else throw;
      }
    catch (continue_exception &ce) {
      if (!ce.HasLabel || (HasLabel && ce.HasLabel && ce.Label == Label))
        continue;
      else throw;
      }
    }
  return result;
  }




// js_for_in ------------------------------------------------------------------
js_for_in::js_for_in(ref<expression> iter,ref<expression> array,ref<expression> loop)
  : Iterator(iter),Array(array),LoopExpression(loop),HasLabel(false) {
  }




js_for_in::js_for_in(ref<expression> iter,ref<expression> array,ref<expression> loop,string const &label)
  : Iterator(iter),Array(array),LoopExpression(loop),
  HasLabel(true),Label(label) {
  }




ref<value> js_for_in::evaluate(context const &ctx,bool want_lvalue) const {
  list_scope scope;
  scope.unite(ctx.CurrentScope);
  no_free_ref<value> scope_ref(&scope);
  context inner_context(ctx,&scope);

  ref<value> result;
  ref<value> iterator = Iterator->evaluate(inner_context,true);
  ref<value> array = Array->evaluate(inner_context);
  
  TSize size = array->lookup("length",false)->toInt();
  
  for (TIndex i = 0;i < size;i++) {
    try {
      iterator->assign(array->subscript(*makeConstantInt(i),false));
      result = LoopExpression->evaluate(inner_context);
      }
    catch (break_exception &be) {
      if (!be.HasLabel || (HasLabel && be.HasLabel && be.Label == Label))
        break;
      else throw;
      }
    catch (continue_exception &ce) {
      if (!ce.HasLabel || (HasLabel && ce.HasLabel && ce.Label == Label))
        continue;
      else throw;
      }
    }
  if (result.get()) return result->duplicate();
  return ref<value>(NULL);
  // ATTENTION: this is a scope cancellation point.
  // Therefore any values passed out must be made lvalue-free, i.e. duplicated
  }




// js_return ------------------------------------------------------------------
js_return::js_return(unsigned line,ref<expression> retval)
  : ReturnValue(retval),Line(line) {
  }




ref<value> js_return::evaluate(context const &ctx,bool want_lvalue) const {
  ref<value> retval;
  if (ReturnValue.get())
    retval = ReturnValue->evaluate(ctx);
  return_exception rex;
  rex.ReturnValue = retval;
  rex.Line = Line;
  throw rex;
  }




// js_break -------------------------------------------------------------------
js_break::js_break(unsigned line)
  : HasLabel(false),Line(line) {
  }




js_break::js_break(unsigned line,string const &label)
  : HasLabel(true),Label(label),Line(line) {
  }




ref<value> js_break::evaluate(context const &ctx,bool want_lvalue) const {
  break_exception be;
  be.HasLabel = HasLabel;
  be.Label = Label;
  be.Line = Line;
  throw be;
  }




// js_continue ----------------------------------------------------------------
js_continue::js_continue(unsigned line)
  : HasLabel(false),Line(line) {
  }




js_continue::js_continue(unsigned line,string const &label)
  : HasLabel(true),Label(label),Line(line) {
  }




ref<value> js_continue::evaluate(context const &ctx,bool want_lvalue) const {
  continue_exception ce;
  ce.HasLabel = HasLabel;
  ce.Label = Label;
  ce.Line = Line;
  throw ce;
  }




// break_label ----------------------------------------------------------------
break_label::break_label(string const &label,ref<expression> expr)
  : Label(label),Expression(expr) {
  }




ref<value> 
break_label::evaluate(context const &ctx,bool want_lvalue) const {
  try {
    return Expression->evaluate(ctx,want_lvalue);
    }
  catch (break_exception &be) {
    if (be.HasLabel && be.Label == Label) return ref<value>(NULL);
    else throw;
    }
  }




// js_switch  -----------------------------------------------------------------
js_switch::js_switch(ref<expression> discriminant)
  : HasLabel(false),Discriminant(discriminant) {
  }




js_switch::js_switch(ref<expression> discriminant,string const &label)
  : HasLabel(true),Label(label),Discriminant(discriminant) {
  }




ref<value> 
js_switch::
evaluate(context const &ctx,bool want_lvalue) const {
  list_scope scope;
  scope.unite(ctx.CurrentScope);
  no_free_ref<value> scope_ref(&scope);
  context inner_context(ctx,&scope);
  
  ref<value> discr = Discriminant->evaluate(inner_context,want_lvalue);
  
  case_list::const_iterator expr,def;
  bool expr_found = false,def_found = false;
  FOREACH_CONST(first,CaseList,case_list) {
    if (first->DiscriminantValue.get()) {
      if (first->DiscriminantValue->evaluate(inner_context,want_lvalue)->
      operatorBinary(value::OP_EQUAL,*Discriminant,inner_context)->toBoolean()) {
        expr = first;
	expr_found = true;
	break;
	}
      }
    else {
      if (!def_found) {
        def = first;
        def_found = true;
	}
      }
    }
  
  try {
    case_list::const_iterator exec,last = CaseList.end();
    if (expr_found)
      exec = expr; 
    else if (def_found) 
      exec = def;
    else
      return ref<value>(NULL);

    ref<value> result;
    while (exec != last) {
      result = exec->Expression->evaluate(inner_context,want_lvalue);
      exec++;
      }
    if (result.get()) return result->duplicate();
    return ref<value>(NULL);
    }
  catch (break_exception &be) {
    if (!be.HasLabel || (HasLabel && be.HasLabel && be.Label == Label)) 
      return ref<value>(NULL);
    else 
      throw;
    }
  
  // ATTENTION: this is a scope cancellation point.
  // Therefore any values passed out must be made lvalue-free, i.e. duplicated
  }




void js_switch::addCase(ref<expression> dvalue,ref<expression> expr) {
  case_label cl;
  cl.DiscriminantValue = dvalue;
  cl.Expression = expr;
  CaseList.push_back(cl);
  }
