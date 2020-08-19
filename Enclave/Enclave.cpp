//Enclave.cpp
//Jonathan S. Takeshita, Ryan Karl

//#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#include <vector>
#include <string>

#ifdef NENCLAVE
# include "Enclave.h"
#else
# include "Enclave_t.h"
#endif

using std::vector;
using std::string;

#ifndef NENCLAVE
# include <sgx_trts.h>
# define rand_bytes(r, n_bytes) (sgx_read_rand((unsigned char *) r, n_bytes) )
#else
# include <stdlib.h> //Need this for rand
# include <assert.h>
inline void rand_bytes(unsigned char * r, const size_t n_bytes){
  assert(r);
  for(size_t i = 0; i < n_bytes; i++){
    r[i] = (unsigned char) rand();
  }
  return;
}
#endif


//Need OCALLS for pipe I/O, setup, teardown
int enclave_aggregate(const void ** ctext_buffers, const size_t num_ctexts, void * out_buf, size_t out_size){


  return 0;

}

//Verbosity levels:
//0 no output (except in case of error)
//1 timing data
//2 logging
//3 data
