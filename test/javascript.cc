// ----------------------------------------------------------------------------
//  Description      : javascript interpreter
// ----------------------------------------------------------------------------
//  (c) Copyright 2000 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <locale.h>
#include <ixlib_config.hh>
#include <ixlib_javascript.hh>




using namespace ixion;
using namespace javascript;




// complex call-in example ----------------------------------------------------
class callIn : public value {
    value_type getType() const { 
      return VT_FUNCTION; 
      } 
    ref<value> call(context const &ctx,parameter_list const &parameters) const;
  };




ref<value> callIn::call(context const &ctx,parameter_list const &parameters) const {
  if (parameters.size() != 1) {
    cout << "callIn needs exactly one parameter" << endl;
    return makeNull();
    }
  if (parameters[0]->getType() == VT_NULL) {
    cout << "got null" << endl;
    return makeNull();
    }
  else if (parameters[0]->getType() == VT_INTEGER) {
    cout << "got an int:" << parameters[0]->toInt() << endl;
    return makeNull();
    }
  else if (parameters[0]->getType() == VT_FLOATING_POINT) {
    cout << "got a float:" << parameters[0]->toFloat() << endl;
    return makeNull();
    }
  else if (parameters[0]->getType() == VT_STRING) {
    cout << "got a string:" << parameters[0]->toString() << endl;
    return makeConstant(17);
    }
  cout << "got something else" << endl;
  return makeNull();
  }
// end complex call-in example ------------------------------------------------




// simple call-in example -----------------------------------------------------
IXLIB_JS_DECLARE_FUNCTION(callIn2) {
  if (parameters.size() == 1) {
    cout << "callIn2 needs exactly not one parameter" << endl;
    return makeNull();
    }
  cout << "callIn2 called" << endl;
  return makeNull();
  }
// end simple call-in example -------------------------------------------------




int main() {
  try {
    ixlibInitI18n();
    
    interpreter ip;
    addStandardLibrary(ip);

    // complex call-in example ------------------------------------------------
    ref<value> ev = new callIn;
    EX_MEMCHECK(ev.get())
    ip.RootScope->addMember("callIn",ev);
    // end complex call-in example --------------------------------------------
    
    // simple call-in example -------------------------------------------------
    ev = new callIn2;
    EX_MEMCHECK(ev.get())
    ip.RootScope->addMember("callIn2",ev);
    // end simple call-in example ---------------------------------------------
    
    string command = "";
    while (!cin.eof()) {
      char buffer[1024];
      if (command.size() == 0)
        cout << ">> ";
      else
        cout << "... ";
      cin.getline(buffer,1024);
      command += buffer;
      try {
        try {
          ref<javascript::value> value = ip.execute(command);
          if (value.get())
            cout << value->stringify() << endl;
          command = "";
          }
        EX_CATCHCODE(javascript,ECJS_UNEXPECTED_EOF,)
	}
      catch (exception &ex) { 
        cerr << ex.what() << endl; 
	command = "";
        } 
      }
    cout << endl;
    }
  catch (exception &ex) { 
    cerr << ex.what() << endl; 
    return 1; 
    }
  }
