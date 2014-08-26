// ----------------------------------------------------------------------------
//  Description      : javascript interpreter
// ----------------------------------------------------------------------------
//  (c) Copyright 2000 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <ixlib_javascript.hh>




using namespace ixion;
using namespace javascript;




int main() {
  try {
    interpreter ip;
    addStandardLibrary(ip);
    
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
