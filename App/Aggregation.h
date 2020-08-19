#ifndef AGGREGATION_H
#define AGGREGATION_H

#include <vector>
#include <utility>

#include <gmp.h>
#include <gmpxx.h>

#include "../Enclave/Enclave_defines.h"

using std::vector;
using std::pair;

Parameters setup(unsigned int tau){
  Parameters parms;
  //Initialization code goes here
  return parms;
}

vector<vector<mpz_class> > keygen(const Parameters & parms){
  vector<vector<mpz_class> > keys;
  
  return keys;
}

std::pair<mpz_class, mpz_class> encode(const mpz_class & x, const vector<mpz_class> & key, const Parameters & parms){
  std::pair <mpz_class, mpz_class> c;
  
  return c;
}

mpz_class aggregate(const vector<pair<mpz_class, mpz_class> > & ciphertexts, const Parameters & parms){
	mpz_class ret;

	return ret;
}

#endif
