//Enclave.h
//Jonathan S. Takeshita, Ryan Karl, Mark Horeni

#ifndef ENCLAVE_H
#define ENCLAVE_H

#include <gmp.h>

#include "Enclave_defines.h"

#if defined(__cplusplus)
# include <vector>
# include <utility>
# include <gmpxx.h>
using std::vector;
using std::pair;
extern "C" {
#endif

int enclave_aggregate(const void ** ctext_buffers, const size_t num_ctexts, void * out_buf, size_t out_size);

#if defined(__cplusplus)
}
#endif

#endif
