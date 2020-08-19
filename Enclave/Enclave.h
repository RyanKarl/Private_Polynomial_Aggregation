//Enclave.h
//Jonathan S. Takeshita, Ryan Karl, Mark Horeni

#ifndef ENCLAVE_H
#define ENCLAVE_H

#if defined(__cplusplus)
#include <vector>
#include <utility>
using std::vector;
using std::pair;
extern "C" {
#endif

mpz_class aggregate(const vector<pair<mpz_class> > & ciphertexts, const Parameters & parms){
	mpz_class ret;

	return ret;
}

int enclave_aggregate(const void ** ctext_buffers, const size_t num_ctexts, void * out_buf, size_t out_size);

#if defined(__cplusplus)
}
#endif

#endif
