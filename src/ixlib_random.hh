// ----------------------------------------------------------------------------
//  Description      : Random numbers
// ----------------------------------------------------------------------------
//  Remarks          :
//    max is never reached.
//
// ----------------------------------------------------------------------------
//  (c) Copyright 1996 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#ifndef IXLIB_RANDOM
#define IXLIB_RANDOM




#include <cstdlib>
#include <cmath>
#include <ctime>
#include <ixlib_base.hh>




namespace ixion {
  class float_random {
    double Seed;
  
    public:
    float_random()
      : Seed(1)
      { }
  
    void init()
      { Seed = time(NULL) / Pi; }
    void init(double seed)
      { Seed = seed; }
    double operator()(double max = 1) {
      Seed = pow(Seed+Pi,8);
      Seed -= floor(Seed);
      return Seed*max;
      }
    };
  
  
  
  
  class int_random {
      float_random	Generator;
  
    public:
      int_random()
        { }
    
      void init()
        { Generator.init(); }
      void init(unsigned seed)
        { Generator.init(seed); }
      unsigned operator()(unsigned max = 32768) {
        unsigned num = rng8() + (rng8() << 7) + (rng8() << 14) + (rng8() << 21) + (rng8() << 28);
        return num % max;
        }
    private:
      TUnsigned8 rng8() {
        return (TUnsigned8) (Generator()*256);
        }
    };
  }




#endif
