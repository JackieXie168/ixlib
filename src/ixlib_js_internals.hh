// ----------------------------------------------------------------------------
//  Description      : Javascript interpreter
// ----------------------------------------------------------------------------
//  (c) Copyright 2000 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#ifndef IXLIB_JS_INTERNALS
#define IXLIB_JS_INTERNALS




#include <ixlib_javascript.hh>




namespace ixion {
  namespace javascript {
    struct context {
      ref<list_scope,value>	CurrentScope;
      
      context(ref<list_scope,value> scope);
      };
    
    struct return_exception {
      ref<value>	ReturnValue;
      unsigned	Line;
      };

    struct break_exception {
      bool		HasLabel;
      string 		Label;
      unsigned	Line;
      };
    
    struct continue_exception {
      bool		HasLabel;
      string		Label;
      unsigned	Line;
      };

    // values -----------------------------------------------------------------
    class null : public value {
      private:
        typedef value super;
        
      public:
        value_type getType() const;
        string toString() const;
        bool toBoolean() const;
        
        ref<value> duplicate() const;
        
        ref<value> operatorBinary(operator_id op,expression const &op2,context const &ctx) const;
      };
    
    class const_floating_point : public value_with_methods {
      private:
        typedef value_with_methods 	super;

      protected:
        double 				Value;
      
      public:
        const_floating_point(double value);
    
        value_type getType() const;
        int toInt() const;
        double toFloat() const;
        bool toBoolean() const;
        string toString() const;
        
        ref<value> duplicate() const;
	
        ref<value> callMethod(string const &identifier,context const &ctx,parameter_list const &parameters);
	
        ref<value> operatorUnary(operator_id op) const;
        ref<value> operatorBinary(operator_id op,expression const &op2,context const &ctx) const;
      };

    class floating_point : public const_floating_point {
      private:
        typedef const_floating_point	super;
        
      public:
        floating_point(double value);

        ref<value> operatorUnaryModifying(operator_id op);
        ref<value> operatorBinaryModifying(operator_id op,expression const &op2,context const &ctx);
      };
      
    class const_integer : public value_with_methods {
      private:
        typedef value_with_methods 	super;

      protected:
        long 				Value;
      
      public:
        const_integer(long value);
    
        value_type getType() const;
        int toInt() const;
        double toFloat() const;
        bool toBoolean() const;
        string toString() const;
        
        ref<value> duplicate() const;
	
        ref<value> callMethod(string const &identifier,context const &ctx,parameter_list const &parameters);
	
        ref<value> operatorUnary(operator_id op) const;
        ref<value> operatorBinary(operator_id op,expression const &op2,context const &ctx) const;
      };

    class integer : public const_integer {
      private:
        typedef const_integer	super;
        
      public:
        integer(long value);

        ref<value> operatorUnaryModifying(operator_id op);
        ref<value> operatorBinaryModifying(operator_id op,expression const &op2,context const &ctx);
      };
      
    class js_string : public value_with_methods {
      private:
        typedef value_with_methods	super;
        
      protected:
        string 				Value;
      
      public:
        js_string(string const &value);
    
        value_type getType() const;
        string toString() const;
        bool toBoolean() const;
	string stringify() const;

        ref<value> duplicate() const;
        
        ref<value> lookup(string const &identifier);
        ref<value> callMethod(string const &identifier,context const &ctx,parameter_list const &parameters);

        ref<value> operatorBinary(operator_id op,expression const &op2,context const &ctx) const;
        ref<value> operatorBinaryModifying(operator_id op,expression const &op2,context const &ctx);
      };
      
    class lvalue : public value {
      protected:
        ref<value>	Reference;
      
      public:
        lvalue(ref<value> ref);
        
        value_type getType() const;
        string toString() const;
        int toInt() const;
        double toFloat() const;
        bool toBoolean() const;
	string stringify() const;

        ref<value> duplicate() const;
        
        ref<value> lookup(string const &identifier);
        ref<value> subscript(value const &index);
        ref<value> call(context const &ctx,parameter_list const &parameters) const;
        ref<value> assign(ref<value> op2);

        ref<value> operatorUnary(operator_id op) const;
        ref<value> operatorBinary(operator_id op,expression const &op2,context const &ctx) const;
        ref<value> operatorUnaryModifying(operator_id op);
        ref<value> operatorBinaryModifying(operator_id op,expression const &op2,context const &ctx);
      };

    class constant_wrapper : public value {
      protected:
        ref<value>	Constant;

      public:
        constant_wrapper(ref<value> val);
        
        value_type getType() const;
        string toString() const;
        int toInt() const;
        double toFloat() const;
        bool toBoolean() const;
	string stringify() const;

        ref<value> duplicate() const;
        
        ref<value> lookup(string const &identifier);
        ref<value> subscript(value const &index);
        ref<value> call(context const &ctx,parameter_list const &parameters) const;
        ref<value> assign(ref<value> value);

        ref<value> operatorUnary(operator_id op) const;
        ref<value> operatorBinary(operator_id op,expression const &op2,context const &ctx) const;
        ref<value> operatorUnaryModifying(operator_id op);
        ref<value> operatorBinaryModifying(operator_id op,expression const &op2,context const &ctx);
      };

    class function : public value {
      public:
        typedef vector<string>		parameter_name_list;
        
      protected:
        parameter_name_list		ParameterNameList;
        ref<expression>			Body;
	ref<value>			LexicalScope;
      
      public:
        function(parameter_name_list const &pnames,ref<expression> body,ref<value> lex_scope);

        value_type getType() const{
          return VT_FUNCTION;
          }
        
        ref<value> duplicate() const;
        
        ref<value> call(context const &ctx,parameter_list const &parameters) const;
      };
        
    class js_array_constructor : public javascript::value {
      public:
        value_type getType() const {
          return VT_TYPE;
          }
        
        ref<value> duplicate() const;
  
        ref<value> construct(javascript::context const &ctx,parameter_list const &parameters) const;
      };

    // expressions ----------------------------------------------------------
    // the constant must already be passed in in non-modifiable form
    class constant : public expression {
      protected:
        ref<value>		Value;
      public:
        constant(ref<value> val);
        ref<value> evaluate(context const &ctx) const;
      };
    
    class unary_operator : public expression {
      protected:
        value::operator_id	Operator;
        ref<expression>	Operand;
      
      public:
        unary_operator(value::operator_id opt,ref<expression> opn);
        ref<value> evaluate(context const &ctx) const;
      };

    class modifying_unary_operator : public expression {
      protected:
        value::operator_id	Operator;
        ref<expression>	Operand;
      
      public:
        modifying_unary_operator(value::operator_id opt,ref<expression> opn);
        ref<value> evaluate(context const &ctx) const;
      };
      
    class binary_operator : public expression {
      protected:
        value::operator_id	Operator;
        ref<expression>	Operand1;
        ref<expression>	Operand2;
      
      public:
        binary_operator(value::operator_id opt,ref<expression> opn1,ref<expression> opn2);
        ref<value> evaluate(context const &ctx) const;
      };

    class modifying_binary_operator : public expression {
      protected:
        value::operator_id	Operator;
        ref<expression>	Operand1;
        ref<expression>	Operand2;
      
      public:
        modifying_binary_operator(value::operator_id opt,ref<expression> opn1,ref<expression> opn2);
        ref<value> evaluate(context const &ctx) const;
      };
    
    class ternary_operator : public expression {
      protected:
        ref<expression>	Operand1;
        ref<expression>	Operand2;
        ref<expression>	Operand3;
      
      public:
        ternary_operator(ref<expression> opn1,ref<expression> opn2,ref<expression> opn3);
        ref<value> evaluate(context const &ctx) const;
      };
    
    class subscript_operation : public expression {
      protected:
        ref<expression>	Operand1;
        ref<expression>	Operand2;
      
      public:
        subscript_operation(ref<expression> opn1,ref<expression> opn2);
        ref<value> evaluate(context const &ctx) const;
      };

    class lookup_operation : public expression {
      protected:
        ref<expression>	Operand;
        string		Identifier;
      
      public:
        lookup_operation(string const &id);
        lookup_operation(ref<expression> opn,string const &id);
        ref<value> evaluate(context const &ctx) const;
      };
    
    class assignment : public expression {
      protected:
        ref<expression>	Operand1;
        ref<expression>	Operand2;
      
      public:
        assignment(ref<expression> opn1,ref<expression> opn2);
        ref<value> evaluate(context const &ctx) const;
      };
    
    class function_call : public expression {
      public:
        typedef vector<ref<expression> >	parameter_expression_list;
      
      protected:
        ref<expression>		Function;
        parameter_expression_list	ParameterExpressionList;
      
      public:
        function_call(ref<expression> fun,parameter_expression_list const &pexps);
        ref<value> evaluate(context const &ctx) const;
      };

    class construction : public expression {
      public:
        typedef vector<ref<expression> >	parameter_expression_list;
      
      protected:
        ref<expression>		Class;
        parameter_expression_list	ParameterExpressionList;
      
      public:
        construction(ref<expression> cls,parameter_expression_list const &pexps);
        ref<value> evaluate(context const &ctx) const;
      };

    // instructions ---------------------------------------------------------
    class variable_declaration : public expression {
      protected:
        string		Identifier;
        ref<expression>	DefaultValue;
      
      public:
        variable_declaration(string const &id,ref<expression> def_value);
        ref<value> evaluate(context const &ctx) const;
      };

    class constant_declaration : public expression {
      protected:
        string		Identifier;
        ref<expression>	DefaultValue;
      
      public:
        constant_declaration(string const &id,ref<expression> def_value);
        ref<value> evaluate(context const &ctx) const;
      };

    class function_declaration : public expression {
      public:
        typedef function::parameter_name_list	parameter_name_list;

      protected:
        string			Identifier;
        parameter_name_list	ParameterNameList;
        ref<expression>		Body;
      
      public:
        function_declaration(string const &id,parameter_name_list const &pnames,
          ref<expression> body);
        ref<value> evaluate(context const &ctx) const;
      };
    
    class instruction_list : public expression {
      protected:
        typedef vector<ref<expression> >	expression_list;
        expression_list				ExpressionList;
      
      public:
        ref<value> evaluate(context const &ctx) const;
        void add(ref<expression> expr);
      };
    
    class scoped_instruction_list : public instruction_list {
      public:
        ref<value> evaluate(context const &ctx) const;
      };
    
    class js_if : public expression {
      protected:
        ref<expression>		Conditional;
        ref<expression>		IfExpression;
        ref<expression>		IfNotExpression;
      
      public:
        js_if(ref<expression> cond,ref<expression> ifex,ref<expression> ifnotex);
        ref<value> evaluate(context const &ctx) const;
      };

    class js_while : public expression {
      protected:
        ref<expression>		Conditional;
        ref<expression>		LoopExpression;
        bool				HasLabel;
        string			Label;
      
      public:
        js_while(ref<expression> cond,ref<expression> loopex);
        js_while(ref<expression> cond,ref<expression> loopex,string const &Label);
        ref<value> evaluate(context const &ctx) const;
      };

    class js_do_while : public expression {
      protected:
        ref<expression>		Conditional;
        ref<expression>		LoopExpression;
        bool				HasLabel;
        string			Label;
      
      public:
        js_do_while(ref<expression> cond,ref<expression> loopex);
        js_do_while(ref<expression> cond,ref<expression> loopex,string const &Label);
        ref<value> evaluate(context const &ctx) const;
      };

    class js_for : public expression {
      protected:
        ref<expression>		Initialization;
        ref<expression>		Conditional;
        ref<expression>		Update;
        ref<expression>		LoopExpression;
        bool			HasLabel;
        string			Label;
      
      public:
        js_for(ref<expression> init,ref<expression> cond,ref<expression> update,ref<expression> loop);
        js_for(ref<expression> init,ref<expression> cond,ref<expression> update,ref<expression> loop,string const &label);
        ref<value> evaluate(context const &ctx) const;
      };
    
    class js_for_in : public expression {
      protected:
        ref<expression>		Iterator;
        ref<expression>		Array;
        ref<expression>		LoopExpression;
        bool			HasLabel;
        string			Label;
      
      public:
        js_for_in(ref<expression> iter,ref<expression> array,ref<expression> loop);
        js_for_in(ref<expression> iter,ref<expression> array,ref<expression> loop,string const &label);
        ref<value> evaluate(context const &ctx) const;
      };
    
    class js_return : public expression {
      protected:
        ref<expression>		ReturnValue;
        unsigned			Line;
      
      public:
        js_return(unsigned line,ref<expression> retval);
        ref<value> evaluate(context const &ctx) const;
      };

    class js_break : public expression {
      protected:
        bool				HasLabel;
        string			Label;
        unsigned			Line;
      
      public:
        js_break(unsigned line);
        js_break(unsigned line,string const &label);
        ref<value> evaluate(context const &ctx) const;
      };

    class js_continue : public expression {
      protected:
        bool				HasLabel;
        string			Label;
        unsigned			Line;
      
      public:
        js_continue(unsigned line);
        js_continue(unsigned line,string const &label);
        ref<value> evaluate(context const &ctx) const;
      };
    
    class break_label : public expression {
      protected:
        string			Label;
        ref<expression>		Expression;
      
      public:
        break_label(string const &label,ref<expression> expr);
        ref<value> evaluate(context const &ctx) const;
      };

    class js_switch : public expression {
      protected:
        bool 				HasLabel;
        string			Label;
        ref<expression>		Discriminant;
        
        struct case_label {
          ref<expression>		DiscriminantValue;
          ref<expression>		Expression;
          };
        typedef vector<case_label> 	case_list;
        case_list			CaseList;
      
      public:
        js_switch(ref<expression> discriminant);
        js_switch(ref<expression> discriminant,string const &label);
        ref<value> evaluate(context const &ctx) const;
        void addCase(ref<expression> dvalue,ref<expression> expr);
      };
    }
  }




#endif