// ----------------------------------------------------------------------------
//  Description      : Javascript interpreter
// ----------------------------------------------------------------------------
//  (c) Copyright 2000 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#ifndef IXLIB_JAVASCRIPT
#define IXLIB_JAVASCRIPT




#include <vector>
#include <hash_map>
#include <ixlib_string.hh>
#include <ixlib_exbase.hh>
#include <ixlib_garbage.hh>
#include <ixlib_scanner.hh>




// Error codes ----------------------------------------------------------------
#define ECJS_UNTERMINATED_COMMENT		0
#define ECJS_CANNOT_CONVERT			1
#define ECJS_INVALID_OPERATION			2
#define ECJS_UNEXPECTED				3
#define ECJS_UNEXPECTED_EOF			4
#define ECJS_CANNOT_MODIFY_RVALUE		5
#define ECJS_UNKNOWN_IDENTIFIER			6
#define ECJS_UNKNOWN_OPERATOR			7
#define ECJS_INVALID_NON_LOCAL_EXIT		8
#define ECJS_INVALID_NUMBER_OF_ARGUMENTS	9
#define ECJS_INVALID_TOKEN			10
#define ECJS_CANNOT_REDECLARE			11




// helpful macros -------------------------------------------------------------
#define IXLIB_JS_ASSERT_PARAMETERS(NAME,ARGMIN,ARGMAX) \
  if (parameters.size() < ARGMIN || parameters.size() > ARGMAX) \
    EXJS_THROWINFO(ECJS_INVALID_NUMBER_OF_ARGUMENTS,NAME)

#define IXLIB_JS_IF_METHOD(NAME,ARGMIN,ARGMAX) \
  if (identifier == NAME) \
    if (parameters.size() < ARGMIN || parameters.size() > ARGMAX) \
      EXJS_THROWINFO(ECJS_INVALID_NUMBER_OF_ARGUMENTS,NAME) \
    else

#define IXLIB_JS_DECLARE_FUNCTION(NAME) \
  namespace { \
    class NAME : public value { \
      public: \
        value_type getType() const { \
          return VT_FUNCTION; \
          } \
        ixion::ref<ixion::javascript::value> call(context const &ctx,parameter_list const &parameters) const; \
      }; \
    } \
  ixion::ref<ixion::javascript::value> NAME::call(context const &ctx,parameter_list const &parameters) const

#define IXLIB_JS_CONVERT_PARAMETERS_0 \
  
  


// Exception throw macros -----------------------------------------------------
#define EXJS_THROW(CODE)\
  EX_THROW(javascript,CODE)
#define EXJS_THROWINFO(CODE,INFO)\
  EX_THROWINFO(javascript,CODE,INFO)
#define EXJS_THROWINFOTOKEN(CODE,INFO,TOKEN)\
  EXJS_THROWINFOLINE(CODE,INFO,(TOKEN).Line)
#define EXJS_THROWINFOLINE(CODE,INFO,LINE)\
  throw ixion::javascript_exception(CODE,LINE,INFO,__FILE__,__LINE__);




namespace ixion {
  // javascript_exception -----------------------------------------------------
  struct javascript_exception : public base_exception {
    javascript_exception(TErrorCode error,char const *info = NULL,char *module = NULL,
      TIndex line = 0)
      : base_exception(error,info,module,line,"JS") {
      }
    javascript_exception(TErrorCode error, TIndex js_line = 0, char const *info = 0,char *module = NULL,
      TIndex line = 0);
    virtual char *getText() const;
    };




  // javascript ---------------------------------------------------------------
  /**
  This code tries to be an implementation of ECMAScript 4, as available at 
  http://www.mozilla.org/js/
  Note that ES4 is still in the process of standardization. 

  It is meant to behave like an ES4 interpreter in strict mode, none
  of the backward-compatible braindead-isms like newline semicolon
  insertion and other stuff will ever be implemented.

  This is the list of its shortcomings:
  <ul>
    <li> exceptions
    <li> proper error reporting with line numbers
    <li> classes,namespaces,packages
    <li> constness
    <li> Number/String constructor and class methods
    <li> real regexp's
    <li> the methods listed in FIXME's (js_library.cc js_value.cc)
    <li> cannot cross-assign predefined methods [won't be]
    <li> Grammatical semicolon insertion [won't be]
    <li> type declaration [won't be]
    </ul>
  */
  namespace javascript {
    class context;
    class expression;
    
    class value {
      public:
        enum operator_id { 
          // unary, modifying
          OP_PRE_INCREMENT,OP_POST_INCREMENT,
          OP_PRE_DECREMENT,OP_POST_DECREMENT,
          // unary, non-modifying
          OP_UNARY_PLUS,OP_UNARY_MINUS,
          OP_LOG_NOT,OP_BIN_NOT,
          // binary, modifying
          OP_PLUS_ASSIGN,OP_MINUS_ASSIGN,
          OP_MUTLIPLY_ASSIGN,OP_DIVIDE_ASSIGN,OP_MODULO_ASSIGN,
          OP_BIT_AND_ASSIGN,OP_BIT_OR_ASSIGN,OP_BIT_XOR_ASSIGN,
          OP_LEFT_SHIFT_ASSIGN,OP_RIGHT_SHIFT_ASSIGN,
          // binary, non-modifying
          OP_PLUS,OP_MINUS,
          OP_MULTIPLY,OP_DIVIDE,OP_MODULO,
          OP_BIT_AND,OP_BIT_OR,OP_BIT_XOR,
          OP_LEFT_SHIFT,OP_RIGHT_SHIFT,
          OP_LOGICAL_OR,OP_LOGICAL_AND,
          OP_EQUAL,OP_NOT_EQUAL,OP_IDENTICAL,OP_NOT_IDENTICAL,
          OP_LESS_EQUAL,OP_GREATER_EQUAL,OP_LESS,OP_GREATER,
          // special
          OP_ASSIGN,
          };
        
        enum value_type {
          VT_UNDEFINED,VT_NULL,
          VT_INTEGER,VT_FLOATING_POINT,VT_STRING,
          VT_FUNCTION,VT_OBJECT,VT_BUILTIN,VT_HOST,
	  VT_SCOPE,VT_BOUND_METHOD,VT_TYPE
          };
        typedef vector<ref<value> >	parameter_list;

        virtual ~value() {
          }
      
        virtual value_type getType() const = 0;
        virtual string toString() const;
        virtual int toInt() const;
        virtual double toFloat() const;
        virtual bool toBoolean() const;
	// toString is meant as a type conversion, whereas stringify
	// is for debuggers and the like
	virtual string stringify() const;
        
        // this operation is defined to eliminate any wrappers
        virtual ref<value> duplicate() const;

        virtual ref<value> lookup(string const &identifier);
        virtual ref<value> subscript(value const &index);
        virtual ref<value> call(context const &ctx,parameter_list const &parameters) const;
        virtual ref<value> construct(context const &ctx,parameter_list const &parameters) const;
        virtual ref<value> assign(ref<value> op2);
        
        virtual ref<value> operatorUnary(operator_id op) const;
        virtual ref<value> operatorBinary(operator_id op,expression const &op2,context const &ctx) const;
        virtual ref<value> operatorUnaryModifying(operator_id op);
        virtual ref<value> operatorBinaryModifying(operator_id op,expression const &op2,context const &ctx);
        
        static operator_id token2operator(scanner::token const &token,bool unary = false,bool prefix = false);
        static char *operator2string(operator_id op);
	static char *valueType2string(value_type vt);
      };
    
    // obviously, any value can have methods, but with this neat little
    // interface implementing methods has just become easier.
    class value_with_methods : public value {
      protected:
        class method : public value {
          protected:
            string			Identifier;
            value_with_methods		*Parent;
            ref<value>			ParentRef;
            
          public:
            value_type getType() const {
              return VT_BOUND_METHOD;
              }

            method(string const &identifier,value_with_methods *parent);
            ref<value> call(context const &ctx,parameter_list const &parameters) const;
          };
            
      public:
        ref<value> lookup(string const &identifier);
        virtual ref<value> callMethod(string const &identifier,context const &ctx,parameter_list const &parameters) = 0;
      };

    // obviously, any value can already represent a scope ("lookup" member!).
    // the list_scope class is an explicit scope that can "swallow" 
    // (=unite with) other scopes and keeps a list of registered members
    class list_scope : public value {
      protected:
        typedef hash_map<string,ref<value>,string_hash>		member_map;
        typedef vector<ref<value> >				swallowed_list;
        
        member_map	MemberMap;
        swallowed_list	SwallowedList;
        
      public:
        value_type getType() const {
	  return VT_SCOPE;
	  }

        ref<value> lookup(string const &identifier);

        void unite(ref<value> scope);
        void separate(ref<value> scope);
	void clearScopes();
        
        bool hasMember(string const &name) const;
	void addMember(string const &name,ref<value> member);
        void removeMember(string const &name);
	void clearMembers();
	
	void clear();
      };
    
    class js_array : public value_with_methods {
      private:
        typedef value_with_methods	super;
        
      protected:
        typedef vector<ref<value> >	value_array;
        value_array			Array;
  
      public:
        js_array() {
          }
        js_array(TSize size);
        js_array(value_array::const_iterator first,value_array::const_iterator last)
          : Array(first,last) {
          }
        js_array(js_array const &src)
          : Array(src.Array) {
          }
        
        value_type getType() const {
          return VT_BUILTIN;
          }

	string stringify() const;
        
        ref<value> duplicate() const;
  
        ref<value> lookup(string const &identifier);
        ref<value> subscript(value const &index);
        ref<value> callMethod(string const &identifier,javascript::context const &ctx,parameter_list const &parameters);
  
        TSize size() const {
	  return Array.size();
	  }
	void resize(TSize size);
	ref<value> &operator[](TIndex idx);
      };

    class expression {
      public:
        virtual ~expression() {
          }
        virtual ref<value> evaluate(context const &ctx) const = 0;
      };
    
    ref<value> makeUndefined();
    ref<value> makeNull();
    ref<value> makeValue(bool val);
    ref<value> makeConstant(bool val);
    ref<value> makeValue(signed long val);
    ref<value> makeConstant(signed long val);
    ref<value> makeValue(signed int val);
    ref<value> makeConstant(signed int val);
    ref<value> makeValue(unsigned long val);
    ref<value> makeConstant(unsigned long val);
    ref<value> makeValue(unsigned int val);
    ref<value> makeConstant(unsigned int val);
    ref<value> makeValue(double val);
    ref<value> makeConstant(double val);
    ref<value> makeValue(string const &val);
    ref<value> makeConstant(string const &val);
    ref<value> makeArray(TSize size = 0);
    ref<value> makeLValue(ref<value> target);
    ref<value> wrapConstant(ref<value> val);
    ref<expression> makeConstantExpression(ref<value> val);

    class interpreter {
      public:
        ref<list_scope,value>			RootScope;
      
      public:
        interpreter();
	~interpreter();
        
        ref<expression> parse(string const &str);
        ref<expression> parse(istream &istr);
        ref<value> execute(string const &str);
        ref<value> execute(istream &istr);
        ref<value> execute(ref<expression> expr);
    
      private:
        ref<value> evaluateCatchExits(ref<expression> expr);
      
        ref<expression> parseInstructionList(
          scanner::token_iterator &first,scanner::token_iterator const &last,bool scoped);
        ref<expression> parseSwitch(
          scanner::token_iterator &first,scanner::token_iterator const &last,string const &label);
        ref<expression> parseVariableDeclaration(
          scanner::token_iterator &first,scanner::token_iterator const &last);
        ref<expression> parseConstantDeclaration(
          scanner::token_iterator &first,scanner::token_iterator const &last);
        ref<expression> parseInstruction(
          scanner::token_iterator &first,scanner::token_iterator const &last);
        ref<expression> parseExpression(
          scanner::token_iterator &first,scanner::token_iterator const &last,
          int precedence = 0);
      };

    void addGlobal(interpreter &ip);
    void addMath(interpreter &ip);
    void addStandardLibrary(interpreter &ip);
    }
  }




#endif