// ----------------------------------------------------------------------------
//  Description      : matrix tests
// ----------------------------------------------------------------------------
//  (c) Copyright 2000 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#include <iostream>
#include <iomanip>
#include <ixlib_matrix_impl.hh>
#include "test.hh"




using namespace ixion;




typedef matrix<double> mat;
template mat;




void item_formatter(ostream &os,bool first,bool last) {
  os << setw(10) << setprecision(4);
  }





BEGIN_TEST
  // mat A;
  // cin >> A;
  // mat b;
  // cin >> b;
  
  // mat x = A.linearSolve(b);
  // cout << x << endl << A*x;
  
  double u[] = { 9,-7,2,-7,11,-3,2,-3,4 };

  mat A(3,3,u);
  // cin >> A;
  mat cholesky = A.getCholesky();
  cout << cholesky << endl << A << endl << cholesky*cholesky.getTransposed();

  // cin >> A;
  // mat l,r;
  // A.getLR(l,r);
  // cout << l << r << l*r;
END_TEST
