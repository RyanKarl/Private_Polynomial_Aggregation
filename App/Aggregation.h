#include <vector>

#include "gmp.h"
#include "gmpxx.h"

using std::vector;

struct Parameters{
  //p1
  //p2
  //N = p1*p2
  //e (generator)
  //c, hash parameter
};

Parameters setup(unsigned int tau){
  Parameters parms;
  //Initialization code goes here
  return parms;
}

vector<vector<mpz_class> > keygen(const Parameters & parms){
  vector<vector<mpz_class> > keys;
  
  return keys;
}

mpz_class encode(const mpz_class & x, const vector<mpz_class> & key, const Parameters & parms){
  mpz_class c;
  
  return c;
}
