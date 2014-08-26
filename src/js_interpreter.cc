// ----------------------------------------------------------------------------
//  Description      : Javascript interpreter
// ----------------------------------------------------------------------------
//  (c) Copyright 2000 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <strstream>
#include <ixlib_base.hh>
#include <ixlib_scanjs.hh>
#include <ixlib_numconv.hh>
#include <ixlib_js_internals.hh>
#include <ixlib_token_javascript.hh>




#define ADVANCE \
  first++; \
  if (first == last) EXJS_THROW(ECJS_UNEXPECTED_EOF)
#define EXPECT(WHAT,STRINGIFIED) \
  if (first == last) EXJS_THROW(ECJS_UNEXPECTED_EOF) \
  if (first->Type != WHAT) \
    EXJS_THROWINFOTOKEN(ECJS_UNEXPECTED,("'"+first->Text+"' instead of " STRINGIFIED).c_str(),*first)
  



using namespace ixion;
using namespace javascript;




// garbage collectors ---------------------------------------------------------
IXLIB_GARBAGE_DECLARE_MANAGER(value)
IXLIB_GARBAGE_DECLARE_MANAGER(expression)




// exception texts ------------------------------------------------------------
static char *(PlainText[]) ={
  N_("Unterminated comment"),
  N_("Cannot convert"),
  N_("Invalid operation"),
  N_("Unexpected token encountered"),
  N_("Unexpected end of file"),
  N_("Cannot modify rvalue"),
  N_("Unknown identifier"),
  N_("Unknown operator"),
  N_("Invalid non-local exit"),
  N_("Invalid number of arguments")
  };




// javascript_exception -------------------------------------------------------
javascript_exception::javascript_exception(TErrorCode error, TIndex js_line, char const *info,char *module = NULL,
  TIndex line = 0)
: base_exception(error, NULL, module, line, "JS") {
  HasInfo = true;
  try {
    string temp = "l" + unsigned2dec(js_line);
    if (info) {
      temp += " : ";
      temp += info;
      }
    strcpy(Info,temp.c_str());
    }
  catch (...) { }
  }



char *javascript_exception::getText() const {
  return _(PlainText[Error]);
  }




// context --------------------------------------------------------------------
context::context(list_scope *rootscope)
  : CurrentScope(rootscope),RootScope(rootscope) {
  }




context::context(context const &ctx,list_scope *current_scope) 
  : CurrentScope(current_scope),RootScope(ctx.RootScope) {
  }




// interpreter -----------------------------------------------------------------
interpreter::interpreter() 
  : RootScopeRef(&RootScope) {
  ref<value> ac = new js_array_constructor();
  EX_MEMCHECK(ac.get())
  RootScope.addMember("Array",ac);
  }




ref<expression> interpreter::parse(string const &str) {
  // *** FIXME: this works around a bug in istrstream
  if (str.size() == 0) {
    return ref<expression>(NULL);
    }
  istrstream strm(str.data(),str.size());
  return parse(strm);
  }




ref<expression> interpreter::parse(istream &istr) {
  jsFlexLexer lexer(&istr);
  scanner scanner(lexer);
  
  scanner::token_list tokenlist = scanner.scan();
  scanner::token_iterator text = tokenlist.begin();
  return parseInstructionList(text,tokenlist.end(),false);
  }




ref<value> interpreter::execute(string const &str) {
  return execute(parse(str));
  }




ref<value> interpreter::execute(istream &istr) {
  return execute(parse(istr));
  }




ref<value> interpreter::execute(ref<expression> expr) {
  if (expr.get() == NULL) return ref<value>(NULL);
  return evaluateCatchExits(expr);
  }




ref<value> interpreter::evaluateCatchExits(ref<expression> expr) {
  ref<value> result;
  try {
    context ctx(&RootScope);
    result = expr->evaluate(ctx);
    }
  catch (return_exception &re) {
    EXJS_THROWINFOLINE(ECJS_INVALID_NON_LOCAL_EXIT,"return",re.Line)
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
  return result;
  }




ref<expression> 
interpreter::parseInstructionList(scanner::token_iterator &first,scanner::token_iterator const &last,bool scoped) {
  auto_ptr<instruction_list> ilist(
    scoped 
    ? new scoped_instruction_list()
    : new instruction_list());
  EX_MEMCHECK(ilist.get())
  
  while (first != last && first->Type != '}') {
    ref<expression> expr = parseInstruction(first,last);
    if (expr.get() != NULL) 
      ilist->add(expr);
    }
  return ilist.release();
  }




ref<expression> 
interpreter::parseSwitch(scanner::token_iterator &first,scanner::token_iterator const &last,string const &label) {
  ADVANCE
  EXPECT('(',"'(' in switch statement")
  ADVANCE
  
  ref<expression> discr = parseExpression(first,last);
  EXPECT(')',"')' in switch statement")
  ADVANCE
  
  EXPECT('{',"'{' in switch statement")
  ADVANCE
  
  auto_ptr<js_switch> sw;
  if (label.size()) {
    auto_ptr<js_switch> tsw(new js_switch(discr,label));
    sw = tsw;
    }
  else {
    auto_ptr<js_switch> tsw(new js_switch(discr));
    sw = tsw;
    }
  EX_MEMCHECK(sw.get())

  ref<expression> dvalue;
  auto_ptr<instruction_list> ilist;
  
  while (first != last && first->Type != '}') {
    if (first->Type == TT_JS_CASE) {
      if (ilist.get())
        sw->addCase(dvalue,ilist.release());
   
      auto_ptr<instruction_list> tilist(new instruction_list());
      EX_MEMCHECK(tilist.get())
      ilist = tilist;
      
      ADVANCE

      dvalue = parseExpression(first,last);
      EXPECT(':',"':' in case label")
      ADVANCE
      }
    else if (first->Type == TT_JS_DEFAULT) {
      if (ilist.get())
        sw->addCase(dvalue,ilist.release());
   
      auto_ptr<instruction_list> tilist(new instruction_list());
      EX_MEMCHECK(tilist.get())
      ilist = tilist;
      
      ADVANCE
      dvalue = NULL;
      EXPECT(':',"':' in default label")
      ADVANCE
      }
    else {
      ref<expression> expr = parseInstruction(first,last);
      if (ilist.get() && expr.get() != NULL) 
        ilist->add(expr);
      }
    }

  if (ilist.get())
    sw->addCase(dvalue,ilist.release());
    
  EXPECT('}',"'}' in switch statement")
  ADVANCE
  
  return sw.release();
  }




ref<expression> 
interpreter::parseVariableDeclaration(scanner::token_iterator &first,scanner::token_iterator const &last) {
  EXPECT(TT_JS_VAR,"var keyword")
  ADVANCE
  
  EXPECT(TT_JS_IDENTIFIER,"variable identifier")
  string id = first->Text;
  ADVANCE
  
  ref<expression> def;
  if (first->Type == '=') {
    ADVANCE
    def = parseExpression(first,last);
    }
  ref<expression> result = new variable_declaration(id,def);
  EX_MEMCHECK(result.get())

  if (first->Type == ',') {
    auto_ptr<instruction_list> ilist(new instruction_list());
    EX_MEMCHECK(ilist.get())
    ilist->add(result);
    
    while (first->Type == ',') {
      ADVANCE
      
      EXPECT(TT_JS_IDENTIFIER,"variable identifier")
      id = first->Text;
      ADVANCE
      
      if (first->Type == '=') {
        ADVANCE
        def = parseExpression(first,last);
        }
      ref<expression> decl = new variable_declaration(id,def);
      EX_MEMCHECK(decl.get())
      ilist->add(decl);

      }
    result = ilist.release();
    }

  return result;
  }




ref<expression> 
interpreter::parseInstruction(scanner::token_iterator &first,scanner::token_iterator const &last) {
  if (first == last) EXJS_THROW(ECJS_UNEXPECTED_EOF)
  
  string label;
  if (first+1 != last && first[1].Type == ':') {
    label = first->Text;
    ADVANCE
    ADVANCE
    }
  
  ref<expression> result;
  
  if (first->Type == '{') {
    ADVANCE
    result = parseInstructionList(first,last,true);
    EXPECT('}',"'}'")
    first++;
    }
  else if (first->Type == TT_JS_VAR) {
    result = parseVariableDeclaration(first,last);

    EXPECT(';',"';'")
    first++;
    }
  else if (first->Type == TT_JS_FUNCTION) {
    ADVANCE
    
    EXPECT(TT_JS_IDENTIFIER,"variable identifier")
    string id = first->Text;
    ADVANCE
    
    EXPECT('(',"'(' in function declaration")
    ADVANCE
    
    function::parameter_name_list pnames;
    
    while (first->Type != ')') {
      EXPECT(TT_JS_IDENTIFIER,"parameter identifier")
      pnames.push_back(first->Text);
      ADVANCE
      
      if (first->Type == ',') {
        ADVANCE 
	}
      }
    EXPECT(')',"')' in function declaration")
    ADVANCE

    EXPECT('{',"'{' in function definition")
    ADVANCE
    
    ref<expression> body = parseInstructionList(first,last,false);

    EXPECT('}',"'}'")
    first++;
    
    result = new function_declaration(id,pnames,body);
    EX_MEMCHECK(result.get())
    }
  else if (first->Type == TT_JS_IF) {
    ADVANCE
    
    EXPECT('(',"'(' in if clause")
    ADVANCE
    
    ref<expression> cond = parseExpression(first,last);
    EXPECT(')',"')' in if clause")
    first++;
    ref<expression> if_expr = parseInstruction(first,last);
    ref<expression> else_expr;
    
    if (first != last && first->Type == TT_JS_ELSE) {
      ADVANCE
      else_expr = parseInstruction(first,last);
      }
    result = new js_if(cond,if_expr,else_expr);
    EX_MEMCHECK(result.get())
    }
  else if (first->Type == TT_JS_SWITCH) {
    result = parseSwitch(first,last,label);
    }
  else if (first->Type == TT_JS_WHILE) {
    ADVANCE
    EXPECT('(',"'(' in while clause")
    ADVANCE
    
    ref<expression> cond = parseExpression(first,last);
    EXPECT(')',"')' in while clause")
    first++;

    ref<expression> loop_expr = parseInstruction(first,last);

    if (label.size()) {
      result = new js_while(cond,loop_expr,label);
      label = "";
      }
    else
      result = new js_while(cond,loop_expr);
    EX_MEMCHECK(result.get())
    }
  else if (first->Type == TT_JS_DO) {
    ADVANCE
    ref<expression> loop_expr = parseInstruction(first,last);

    EXPECT(TT_JS_WHILE,"'while' in do-while")
    ADVANCE
    
    EXPECT('(',"'(' in do-while clause")
    ADVANCE
    
    ref<expression> cond = parseExpression(first,last);
    EXPECT(')',"')' in do-while clause")
    first++;
    
    if (label.size()) {
      result = new js_do_while(cond,loop_expr,label);
      label = "";
      }
    else
      result = new js_do_while(cond,loop_expr);
    EX_MEMCHECK(result.get())
    }
  else if (first->Type == TT_JS_FOR) {
    ADVANCE
    
    EXPECT('(',"'(' in for clause")
    ADVANCE
    
    ref<expression> init_expr;
    if (first->Type == TT_JS_VAR)
      init_expr = parseVariableDeclaration(first,last);
    else
      init_expr = parseExpression(first,last);

    if (first->Type == TT_JS_IN) {
      // for (iterator in list)
      ADVANCE
      ref<expression> array_expr = parseExpression(first,last);

      EXPECT(')',"')' in for clause")
      first++;
      
      ref<expression> loop_expr = parseInstruction(first,last);
      
      if (label.size()) {
        result = new js_for_in(init_expr,array_expr,loop_expr,label);
        label = "";
        }
      else
        result = new js_for_in(init_expr,array_expr,loop_expr,label);
      }
    else {
      // for (;;) ...
      EXPECT(';',"';' in for clause")
      ADVANCE
      
      ref<expression> cond_expr = parseExpression(first,last);
  
      EXPECT(';',"';' in for clause")
      ADVANCE
      
      ref<expression> update_expr = parseExpression(first,last);
      
      EXPECT(')',"')' in for clause")
      first++;
  
      ref<expression> loop_expr = parseInstruction(first,last);
      
      if (label.size()) {
        result = new js_for(init_expr,cond_expr,update_expr,loop_expr,label);
        label = "";
        }
      else
        result = new js_for(init_expr,cond_expr,update_expr,loop_expr);
      }
    EX_MEMCHECK(result.get())
    }
  else if (first->Type == TT_JS_RETURN) {
    unsigned line = first->Line;
    ADVANCE
    
    if (first->Type != ';')
      result = new js_return(line,parseExpression(first,last));
    else
      result = new js_return(line,makeConstant(makeNull()));
    
    EX_MEMCHECK(result.get())
    EXPECT(';',"';'")
    first++;
    }
  else if (first->Type == TT_JS_BREAK) {
    unsigned line = first->Line;
    ADVANCE
    
    if (first->Type != ';') {
      EXPECT(TT_JS_IDENTIFIER,"break label")
      result = new js_break(line,first->Text);
      ADVANCE
      }
    else
      result = new js_break(line);
    
    EX_MEMCHECK(result.get())
    EXPECT(';',"';'")
    first++;
    }
  else if (first->Type == TT_JS_CONTINUE) {
    unsigned line = first->Line;
    ADVANCE
    
    if (first->Type != ';') {
      EXPECT(TT_JS_IDENTIFIER,"continue label")
      result = new js_continue(line,first->Text);
      ADVANCE
      }
    else
      result = new js_continue(line);
    
    EX_MEMCHECK(result.get())
    EXPECT(';',"';'")
    first++;
    }
  else if (first->Type == ';') {
    first++;
    result = makeConstant(ref<value>(NULL));
    }
  else {
    // was nothing else, must be expression
    result = parseExpression(first,last);
    EXPECT(';',"';'")
    first++;
    }
  
  if (label.size()) {
    result = new break_label(label,result);
    EX_MEMCHECK(result.get())
    }
  
  return result;
  }




static int const
PREC_COMMA = 10,		// , (if implemented)
PREC_THROW = 20,		// throw (if implemented)
PREC_ASSIGN = 30,  		// += and friends [ok]
PREC_CONDITIONAL = 40, 		// ?: [ok]
PREC_LOG_OR = 50,		// || [ok]
PREC_LOG_AND = 60,		// && [ok]
PREC_BIT_OR = 70,		// | [ok]
PREC_BIT_XOR = 80,		// ^ [ok]
PREC_BIT_AND = 90,		// & [ok]
PREC_EQUAL = 100,		// == != === !=== [ok]
PREC_COMP = 110,		// < <= > >= [ok]
PREC_SHIFT = 120,		// << >> [ok]
PREC_ADD = 130,			// + - [ok]
PREC_MULT = 140,		// * / % [ok]
PREC_UNARY = 160,		// new + - ++ -- ! ~
PREC_FUNCALL = 170,		// ()
PREC_SUBSCRIPT = 180,		// [] .
PREC_TERM = 200;		// literal identifier 




ref<expression> 
interpreter::parseExpression(scanner::token_iterator &first,scanner::token_iterator const &last,int precedence) {
  /*
  precedence:
  the called routine will continue parsing as long as the encountered 
  operators all have precedence greater or equal than the given parameter.
  
  those are the possible precedence values:
  */
  
  // an EOF condition cannot legally occur inside 
  // or at the end of an expression.
  
  if (first == last) EXJS_THROW(ECJS_UNEXPECTED_EOF)
  
  ref<expression> expr;

  // parse prefix unaries -----------------------------------------------------
  if (precedence <= PREC_UNARY)  {
    if (first->Type == TT_JS_NEW) {
      ADVANCE
      
      ref<expression> cls = parseExpression(first,last,PREC_SUBSCRIPT);
      
      EXPECT('(',"'(' in 'new' expression")
      ADVANCE

      function_call::parameter_expression_list pexps;
      while (first->Type != ')') {
        pexps.push_back(parseExpression(first,last));
        
        if (first->Type == ',') {
	  ADVANCE 
          }
        }
	
      EXPECT(')',"')' in 'new' expression")
      ADVANCE
      
      expr = new construction(cls,pexps);
      EX_MEMCHECK(expr.get())
      }
    if (first->Type == TT_JS_INCREMENT || first->Type == TT_JS_DECREMENT) {
      value::operator_id op = value::token2operator(*first,true,true);
      ADVANCE
      expr = new modifying_unary_operator(op,parseExpression(first,last,PREC_UNARY));
      EX_MEMCHECK(expr.get())
      }
    if (first->Type == '+' ||first->Type == '-'
    || first->Type == '!' || first->Type == '~') {
      value::operator_id op = value::token2operator(*first,true,true);
      ADVANCE
      expr = new unary_operator(op,parseExpression(first,last,PREC_UNARY));
      EX_MEMCHECK(expr.get())
      }
    }

  // parse parentheses --------------------------------------------------------
  if (first->Type == '(') {
    ADVANCE
    expr = parseExpression(first,last);
    EXPECT(')',"')'")
    ADVANCE
    }
  
  // parse term ---------------------------------------------------------------
  if (expr.get() == NULL && precedence <= PREC_TERM) {
    if (first->Type == TT_JS_LIT_INT) {
      expr = makeConstant(makeConstantInt(evalUnsigned(first->Text)));
      ADVANCE
      }
    else if (first->Type == TT_JS_LIT_FLOAT) {
      expr = makeConstant(makeConstantFloat(evalFloat(first->Text)));
      ADVANCE
      }
    else if (first->Type == TT_JS_LIT_STRING) {
      expr = makeConstant(makeConstantString(parseCEscapes(
        first->Text.substr(1,first->Text.size()-2)
	)));
      ADVANCE
      }
    else if (first->Type == TT_JS_LIT_TRUE) {
      expr = makeConstant(makeConstantBoolean(true));
      ADVANCE
      }
    else if (first->Type == TT_JS_LIT_FALSE) {
      expr = makeConstant(makeConstantBoolean(false));
      ADVANCE
      }
    else if (first->Type == TT_JS_LIT_UNDEFINED) {
      expr = makeConstant(makeUndefined());
      ADVANCE
      }
    else if (first->Type == TT_JS_IDENTIFIER) {
      expr = new lookup_operation(first->Text);
      EX_MEMCHECK(expr.get())
      ADVANCE
      }
    else if (first->Type == TT_JS_NULL) {
      expr = makeConstant(makeNull());
      ADVANCE
      }
    }
  
  if (expr.get() == NULL)
    EXJS_THROWINFOTOKEN(ECJS_UNEXPECTED,("'"+first->Text+"' instead of expression").c_str(),*first)

  bool parsed_something;
  do {
    parsed_something = false;
    
    // parse postfix "subscripts" ---------------------------------------------
    if (first->Type == '(' && precedence <= PREC_FUNCALL) {
      ADVANCE
      
      function_call::parameter_expression_list pexps;
      while (first->Type != ')') {
        pexps.push_back(parseExpression(first,last));
        
        if (first->Type == ',') {
	  ADVANCE 
          }
        }
	
      EXPECT(')',"')' in function call")
      ADVANCE
      
      expr = new function_call(expr,pexps);
      EX_MEMCHECK(expr.get())
      parsed_something = true;
      }
    // parse postfix unary ----------------------------------------------------
    else if ((first->Type == TT_JS_INCREMENT || first->Type == TT_JS_DECREMENT) && precedence <= PREC_UNARY) {
      value::operator_id op = value::token2operator(*first,true);
      expr = new modifying_unary_operator(op,expr);
      EX_MEMCHECK(expr.get())
      parsed_something = true;
      ADVANCE
      }
    // parse special binary operators -----------------------------------------
    else if (first->Type == '.' && precedence <= PREC_SUBSCRIPT) {
      ADVANCE
      expr = new lookup_operation(expr,first->Text);
      EX_MEMCHECK(expr.get())
      ADVANCE
      parsed_something = true;
      }
    else if (first->Type == '[' && precedence <= PREC_SUBSCRIPT) {
      ADVANCE
      
      ref<expression> index = parseExpression(first,last);
      
      EXPECT(']',"']' in subscript")
      ADVANCE
      
      expr = new subscript_operation(expr,index);
      EX_MEMCHECK(expr.get())

      parsed_something = true;
      }
    
    // parse regular binary operators -----------------------------------------
    #define BINARY_OP(PRECEDENCE,EXPRESSION,TYPE) \
      else if ((EXPRESSION) && precedence <= PRECEDENCE) { \
        value::operator_id op = value::token2operator(*first); \
        ADVANCE \
        ref<expression> right = parseExpression(first,last,PRECEDENCE); \
        expr = new TYPE##_operator(op,expr,right); \
        EX_MEMCHECK(expr.get()) \
        parsed_something = true; \
        }

    BINARY_OP(PREC_MULT,first->Type == '*' || first->Type == '/' || first->Type == '%',binary)
    BINARY_OP(PREC_ADD,first->Type == '+' || first->Type == '-',binary)
    BINARY_OP(PREC_SHIFT,first->Type == TT_JS_LEFT_SHIFT || first->Type == TT_JS_RIGHT_SHIFT,binary)
    BINARY_OP(PREC_COMP,first->Type == '>' || first->Type == '<'
      || first->Type == TT_JS_LESS_EQUAL || first->Type == TT_JS_GREATER_EQUAL
      || first->Type == TT_JS_IDENTICAL || first->Type == TT_JS_NOT_IDENTICAL,binary)
    BINARY_OP(PREC_EQUAL,first->Type == TT_JS_EQUAL || first->Type == TT_JS_NOT_EQUAL,binary)
    BINARY_OP(PREC_BIT_AND,first->Type == '&',binary)
    BINARY_OP(PREC_BIT_XOR,first->Type == '^',binary)
    BINARY_OP(PREC_BIT_OR,first->Type == '|',binary)
    BINARY_OP(PREC_LOG_AND,first->Type == TT_JS_LOGICAL_AND,binary)
    BINARY_OP(PREC_LOG_OR,first->Type == TT_JS_LOGICAL_OR,binary)
    else if (first->Type == '?') {
      ADVANCE
      ref<expression> op2 = parseExpression(first,last);
      if (first->Type != ':')
        EXJS_THROWINFO(ECJS_UNEXPECTED,(first->Text+" instead of ':'").c_str())
      ADVANCE
      ref<expression> op3 = parseExpression(first,last,PREC_CONDITIONAL);
      expr = new ternary_operator(expr,op2,op3);
      EX_MEMCHECK(expr.get())
      parsed_something = true;
      }
    else if (first->Type == '=' && precedence <= PREC_ASSIGN) {
      ADVANCE
      ref<expression> op2 = parseExpression(first,last);
      expr = new assignment(expr,op2);
      EX_MEMCHECK(expr.get())
      parsed_something = true;
      }
    BINARY_OP(PREC_ASSIGN,first->Type == TT_JS_PLUS_ASSIGN
      || first->Type == TT_JS_MINUS_ASSIGN
      || first->Type == TT_JS_MULTIPLY_ASSIGN
      || first->Type == TT_JS_DIVIDE_ASSIGN
      || first->Type == TT_JS_MODULO_ASSIGN
      || first->Type == TT_JS_BIT_XOR_ASSIGN
      || first->Type == TT_JS_BIT_AND_ASSIGN
      || first->Type == TT_JS_BIT_OR_ASSIGN	
      || first->Type == TT_JS_LEFT_SHIFT_ASSIGN
      || first->Type == TT_JS_RIGHT_SHIFT_ASSIGN,modifying_binary)
  } while (parsed_something);
  
  return expr;
  }
