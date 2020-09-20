#include "EnclaveGmpTest_u.h"
#include <errno.h>

typedef struct ms_e_send_public_t {
	size_t ms_retval;
	char* ms_EC_p;
	size_t ms_EC_p_len;
	char* ms_EC_a;
	size_t ms_EC_a_len;
	char* ms_EC_b;
	size_t ms_EC_b_len;
	char* ms_Px;
	size_t ms_Px_len;
	char* ms_Py;
	size_t ms_Py_len;
	int* ms_rand_l;
	int* ms_u;
	int* ms_f;
} ms_e_send_public_t;

typedef struct ms_e_send_private_t {
	size_t ms_retval;
	char* ms_Rx;
	size_t ms_Rx_len;
	char* ms_Ry;
	size_t ms_Ry_len;
	int* ms_key;
	int* ms_secret;
	int* ms_fault;
} ms_e_send_private_t;

typedef struct ms_e_mpz_add_t {
	size_t ms_retval;
	char* ms_str_a;
	size_t ms_str_a_len;
	char* ms_str_b;
	size_t ms_str_b_len;
} ms_e_mpz_add_t;

typedef struct ms_e_mpz_mul_t {
	size_t ms_retval;
	char* ms_str_a;
	size_t ms_str_a_len;
	char* ms_str_b;
	size_t ms_str_b_len;
} ms_e_mpz_mul_t;

typedef struct ms_e_mpf_div_t {
	size_t ms_retval;
	char* ms_str_a;
	size_t ms_str_a_len;
	char* ms_str_b;
	size_t ms_str_b_len;
	int ms_digits;
} ms_e_mpf_div_t;

typedef struct ms_e_get_result_t {
	int ms_retval;
	char* ms_str_c;
	size_t ms_len;
} ms_e_get_result_t;

typedef struct ms_e_pi_t {
	size_t ms_retval;
	uint64_t ms_digits;
} ms_e_pi_t;

static const struct {
	size_t nr_ocall;
	void * table[1];
} ocall_table_EnclaveGmpTest = {
	0,
	{ NULL },
};
sgx_status_t tgmp_init(sgx_enclave_id_t eid)
{
	sgx_status_t status;
	status = sgx_ecall(eid, 0, &ocall_table_EnclaveGmpTest, NULL);
	return status;
}

sgx_status_t e_send_public(sgx_enclave_id_t eid, size_t* retval, char* EC_p, char* EC_a, char* EC_b, char* Px, char* Py, int* rand_l, int* u, int* f)
{
	sgx_status_t status;
	ms_e_send_public_t ms;
	ms.ms_EC_p = EC_p;
	ms.ms_EC_p_len = EC_p ? strlen(EC_p) + 1 : 0;
	ms.ms_EC_a = EC_a;
	ms.ms_EC_a_len = EC_a ? strlen(EC_a) + 1 : 0;
	ms.ms_EC_b = EC_b;
	ms.ms_EC_b_len = EC_b ? strlen(EC_b) + 1 : 0;
	ms.ms_Px = Px;
	ms.ms_Px_len = Px ? strlen(Px) + 1 : 0;
	ms.ms_Py = Py;
	ms.ms_Py_len = Py ? strlen(Py) + 1 : 0;
	ms.ms_rand_l = rand_l;
	ms.ms_u = u;
	ms.ms_f = f;
	status = sgx_ecall(eid, 1, &ocall_table_EnclaveGmpTest, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t e_send_private(sgx_enclave_id_t eid, size_t* retval, char* Rx, char* Ry, int* key, int* secret, int* fault)
{
	sgx_status_t status;
	ms_e_send_private_t ms;
	ms.ms_Rx = Rx;
	ms.ms_Rx_len = Rx ? strlen(Rx) + 1 : 0;
	ms.ms_Ry = Ry;
	ms.ms_Ry_len = Ry ? strlen(Ry) + 1 : 0;
	ms.ms_key = key;
	ms.ms_secret = secret;
	ms.ms_fault = fault;
	status = sgx_ecall(eid, 2, &ocall_table_EnclaveGmpTest, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t e_mpz_add(sgx_enclave_id_t eid, size_t* retval, char* str_a, char* str_b)
{
	sgx_status_t status;
	ms_e_mpz_add_t ms;
	ms.ms_str_a = str_a;
	ms.ms_str_a_len = str_a ? strlen(str_a) + 1 : 0;
	ms.ms_str_b = str_b;
	ms.ms_str_b_len = str_b ? strlen(str_b) + 1 : 0;
	status = sgx_ecall(eid, 3, &ocall_table_EnclaveGmpTest, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t e_mpz_mul(sgx_enclave_id_t eid, size_t* retval, char* str_a, char* str_b)
{
	sgx_status_t status;
	ms_e_mpz_mul_t ms;
	ms.ms_str_a = str_a;
	ms.ms_str_a_len = str_a ? strlen(str_a) + 1 : 0;
	ms.ms_str_b = str_b;
	ms.ms_str_b_len = str_b ? strlen(str_b) + 1 : 0;
	status = sgx_ecall(eid, 4, &ocall_table_EnclaveGmpTest, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t e_mpf_div(sgx_enclave_id_t eid, size_t* retval, char* str_a, char* str_b, int digits)
{
	sgx_status_t status;
	ms_e_mpf_div_t ms;
	ms.ms_str_a = str_a;
	ms.ms_str_a_len = str_a ? strlen(str_a) + 1 : 0;
	ms.ms_str_b = str_b;
	ms.ms_str_b_len = str_b ? strlen(str_b) + 1 : 0;
	ms.ms_digits = digits;
	status = sgx_ecall(eid, 5, &ocall_table_EnclaveGmpTest, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t e_get_result(sgx_enclave_id_t eid, int* retval, char* str_c, size_t len)
{
	sgx_status_t status;
	ms_e_get_result_t ms;
	ms.ms_str_c = str_c;
	ms.ms_len = len;
	status = sgx_ecall(eid, 6, &ocall_table_EnclaveGmpTest, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t e_pi(sgx_enclave_id_t eid, size_t* retval, uint64_t digits)
{
	sgx_status_t status;
	ms_e_pi_t ms;
	ms.ms_digits = digits;
	status = sgx_ecall(eid, 7, &ocall_table_EnclaveGmpTest, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

