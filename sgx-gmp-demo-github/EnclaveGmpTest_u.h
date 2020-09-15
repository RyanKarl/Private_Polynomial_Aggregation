#ifndef ENCLAVEGMPTEST_U_H__
#define ENCLAVEGMPTEST_U_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "sgx_edger8r.h" /* for sgx_status_t etc. */


#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif


sgx_status_t tgmp_init(sgx_enclave_id_t eid);
sgx_status_t e_mpz_add(sgx_enclave_id_t eid, size_t* retval, char* str_a, char* str_b);
sgx_status_t e_mpz_mul(sgx_enclave_id_t eid, size_t* retval, char* str_a, char* str_b);
sgx_status_t e_mpz_div(sgx_enclave_id_t eid, size_t* retval, char* str_a, char* str_b);
sgx_status_t e_mpf_div(sgx_enclave_id_t eid, size_t* retval, char* str_a, char* str_b, int digits);
sgx_status_t e_get_result(sgx_enclave_id_t eid, int* retval, char* str_c, size_t len);
sgx_status_t e_pi(sgx_enclave_id_t eid, size_t* retval, uint64_t digits);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
