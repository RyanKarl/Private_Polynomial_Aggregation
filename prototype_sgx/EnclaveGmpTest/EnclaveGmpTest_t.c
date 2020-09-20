#include "EnclaveGmpTest_t.h"

#include "sgx_trts.h" /* for sgx_ocalloc, sgx_is_outside_enclave */
#include "sgx_lfence.h" /* for sgx_lfence */

#include <errno.h>
#include <mbusafecrt.h> /* for memcpy_s etc */
#include <stdlib.h> /* for malloc/free etc */

#define CHECK_REF_POINTER(ptr, siz) do {	\
	if (!(ptr) || ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_UNIQUE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_ENCLAVE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_within_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define ADD_ASSIGN_OVERFLOW(a, b) (	\
	((a) += (b)) < (b)	\
)


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

static sgx_status_t SGX_CDECL sgx_tgmp_init(void* pms)
{
	sgx_status_t status = SGX_SUCCESS;
	if (pms != NULL) return SGX_ERROR_INVALID_PARAMETER;
	tgmp_init();
	return status;
}

static sgx_status_t SGX_CDECL sgx_e_send_public(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_e_send_public_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_e_send_public_t* ms = SGX_CAST(ms_e_send_public_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_EC_p = ms->ms_EC_p;
	size_t _len_EC_p = ms->ms_EC_p_len ;
	char* _in_EC_p = NULL;
	char* _tmp_EC_a = ms->ms_EC_a;
	size_t _len_EC_a = ms->ms_EC_a_len ;
	char* _in_EC_a = NULL;
	char* _tmp_EC_b = ms->ms_EC_b;
	size_t _len_EC_b = ms->ms_EC_b_len ;
	char* _in_EC_b = NULL;
	char* _tmp_Px = ms->ms_Px;
	size_t _len_Px = ms->ms_Px_len ;
	char* _in_Px = NULL;
	char* _tmp_Py = ms->ms_Py;
	size_t _len_Py = ms->ms_Py_len ;
	char* _in_Py = NULL;
	int* _tmp_rand_l = ms->ms_rand_l;
	size_t _len_rand_l = sizeof(int);
	int* _in_rand_l = NULL;
	int* _tmp_u = ms->ms_u;
	size_t _len_u = sizeof(int);
	int* _in_u = NULL;
	int* _tmp_f = ms->ms_f;
	size_t _len_f = sizeof(int);
	int* _in_f = NULL;

	CHECK_UNIQUE_POINTER(_tmp_EC_p, _len_EC_p);
	CHECK_UNIQUE_POINTER(_tmp_EC_a, _len_EC_a);
	CHECK_UNIQUE_POINTER(_tmp_EC_b, _len_EC_b);
	CHECK_UNIQUE_POINTER(_tmp_Px, _len_Px);
	CHECK_UNIQUE_POINTER(_tmp_Py, _len_Py);
	CHECK_UNIQUE_POINTER(_tmp_rand_l, _len_rand_l);
	CHECK_UNIQUE_POINTER(_tmp_u, _len_u);
	CHECK_UNIQUE_POINTER(_tmp_f, _len_f);

	//
	// fence after pointer checks
	//
	sgx_lfence();

	if (_tmp_EC_p != NULL && _len_EC_p != 0) {
		_in_EC_p = (char*)malloc(_len_EC_p);
		if (_in_EC_p == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_EC_p, _len_EC_p, _tmp_EC_p, _len_EC_p)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

		_in_EC_p[_len_EC_p - 1] = '\0';
		if (_len_EC_p != strlen(_in_EC_p) + 1)
		{
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_tmp_EC_a != NULL && _len_EC_a != 0) {
		_in_EC_a = (char*)malloc(_len_EC_a);
		if (_in_EC_a == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_EC_a, _len_EC_a, _tmp_EC_a, _len_EC_a)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

		_in_EC_a[_len_EC_a - 1] = '\0';
		if (_len_EC_a != strlen(_in_EC_a) + 1)
		{
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_tmp_EC_b != NULL && _len_EC_b != 0) {
		_in_EC_b = (char*)malloc(_len_EC_b);
		if (_in_EC_b == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_EC_b, _len_EC_b, _tmp_EC_b, _len_EC_b)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

		_in_EC_b[_len_EC_b - 1] = '\0';
		if (_len_EC_b != strlen(_in_EC_b) + 1)
		{
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_tmp_Px != NULL && _len_Px != 0) {
		_in_Px = (char*)malloc(_len_Px);
		if (_in_Px == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_Px, _len_Px, _tmp_Px, _len_Px)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

		_in_Px[_len_Px - 1] = '\0';
		if (_len_Px != strlen(_in_Px) + 1)
		{
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_tmp_Py != NULL && _len_Py != 0) {
		_in_Py = (char*)malloc(_len_Py);
		if (_in_Py == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_Py, _len_Py, _tmp_Py, _len_Py)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

		_in_Py[_len_Py - 1] = '\0';
		if (_len_Py != strlen(_in_Py) + 1)
		{
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_tmp_rand_l != NULL && _len_rand_l != 0) {
		if ( _len_rand_l % sizeof(*_tmp_rand_l) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		_in_rand_l = (int*)malloc(_len_rand_l);
		if (_in_rand_l == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_rand_l, _len_rand_l, _tmp_rand_l, _len_rand_l)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

	}
	if (_tmp_u != NULL && _len_u != 0) {
		if ( _len_u % sizeof(*_tmp_u) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		_in_u = (int*)malloc(_len_u);
		if (_in_u == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_u, _len_u, _tmp_u, _len_u)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

	}
	if (_tmp_f != NULL && _len_f != 0) {
		if ( _len_f % sizeof(*_tmp_f) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		_in_f = (int*)malloc(_len_f);
		if (_in_f == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_f, _len_f, _tmp_f, _len_f)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

	}

	ms->ms_retval = e_send_public(_in_EC_p, _in_EC_a, _in_EC_b, _in_Px, _in_Py, _in_rand_l, _in_u, _in_f);
	if (_in_EC_p)
	{
		_in_EC_p[_len_EC_p - 1] = '\0';
		_len_EC_p = strlen(_in_EC_p) + 1;
		if (memcpy_s((void*)_tmp_EC_p, _len_EC_p, _in_EC_p, _len_EC_p)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_in_EC_a)
	{
		_in_EC_a[_len_EC_a - 1] = '\0';
		_len_EC_a = strlen(_in_EC_a) + 1;
		if (memcpy_s((void*)_tmp_EC_a, _len_EC_a, _in_EC_a, _len_EC_a)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_in_EC_b)
	{
		_in_EC_b[_len_EC_b - 1] = '\0';
		_len_EC_b = strlen(_in_EC_b) + 1;
		if (memcpy_s((void*)_tmp_EC_b, _len_EC_b, _in_EC_b, _len_EC_b)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_in_Px)
	{
		_in_Px[_len_Px - 1] = '\0';
		_len_Px = strlen(_in_Px) + 1;
		if (memcpy_s((void*)_tmp_Px, _len_Px, _in_Px, _len_Px)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_in_Py)
	{
		_in_Py[_len_Py - 1] = '\0';
		_len_Py = strlen(_in_Py) + 1;
		if (memcpy_s((void*)_tmp_Py, _len_Py, _in_Py, _len_Py)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_in_rand_l) {
		if (memcpy_s(_tmp_rand_l, _len_rand_l, _in_rand_l, _len_rand_l)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_in_u) {
		if (memcpy_s(_tmp_u, _len_u, _in_u, _len_u)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_in_f) {
		if (memcpy_s(_tmp_f, _len_f, _in_f, _len_f)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}

err:
	if (_in_EC_p) free(_in_EC_p);
	if (_in_EC_a) free(_in_EC_a);
	if (_in_EC_b) free(_in_EC_b);
	if (_in_Px) free(_in_Px);
	if (_in_Py) free(_in_Py);
	if (_in_rand_l) free(_in_rand_l);
	if (_in_u) free(_in_u);
	if (_in_f) free(_in_f);
	return status;
}

static sgx_status_t SGX_CDECL sgx_e_send_private(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_e_send_private_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_e_send_private_t* ms = SGX_CAST(ms_e_send_private_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_Rx = ms->ms_Rx;
	size_t _len_Rx = ms->ms_Rx_len ;
	char* _in_Rx = NULL;
	char* _tmp_Ry = ms->ms_Ry;
	size_t _len_Ry = ms->ms_Ry_len ;
	char* _in_Ry = NULL;
	int* _tmp_key = ms->ms_key;
	size_t _len_key = sizeof(int);
	int* _in_key = NULL;
	int* _tmp_secret = ms->ms_secret;
	size_t _len_secret = sizeof(int);
	int* _in_secret = NULL;
	int* _tmp_fault = ms->ms_fault;
	size_t _len_fault = sizeof(int);
	int* _in_fault = NULL;

	CHECK_UNIQUE_POINTER(_tmp_Rx, _len_Rx);
	CHECK_UNIQUE_POINTER(_tmp_Ry, _len_Ry);
	CHECK_UNIQUE_POINTER(_tmp_key, _len_key);
	CHECK_UNIQUE_POINTER(_tmp_secret, _len_secret);
	CHECK_UNIQUE_POINTER(_tmp_fault, _len_fault);

	//
	// fence after pointer checks
	//
	sgx_lfence();

	if (_tmp_Rx != NULL && _len_Rx != 0) {
		_in_Rx = (char*)malloc(_len_Rx);
		if (_in_Rx == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_Rx, _len_Rx, _tmp_Rx, _len_Rx)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

		_in_Rx[_len_Rx - 1] = '\0';
		if (_len_Rx != strlen(_in_Rx) + 1)
		{
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_tmp_Ry != NULL && _len_Ry != 0) {
		_in_Ry = (char*)malloc(_len_Ry);
		if (_in_Ry == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_Ry, _len_Ry, _tmp_Ry, _len_Ry)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

		_in_Ry[_len_Ry - 1] = '\0';
		if (_len_Ry != strlen(_in_Ry) + 1)
		{
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_tmp_key != NULL && _len_key != 0) {
		if ( _len_key % sizeof(*_tmp_key) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		_in_key = (int*)malloc(_len_key);
		if (_in_key == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_key, _len_key, _tmp_key, _len_key)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

	}
	if (_tmp_secret != NULL && _len_secret != 0) {
		if ( _len_secret % sizeof(*_tmp_secret) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		_in_secret = (int*)malloc(_len_secret);
		if (_in_secret == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_secret, _len_secret, _tmp_secret, _len_secret)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

	}
	if (_tmp_fault != NULL && _len_fault != 0) {
		if ( _len_fault % sizeof(*_tmp_fault) != 0)
		{
			status = SGX_ERROR_INVALID_PARAMETER;
			goto err;
		}
		_in_fault = (int*)malloc(_len_fault);
		if (_in_fault == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_fault, _len_fault, _tmp_fault, _len_fault)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

	}

	ms->ms_retval = e_send_private(_in_Rx, _in_Ry, _in_key, _in_secret, _in_fault);
	if (_in_Rx)
	{
		_in_Rx[_len_Rx - 1] = '\0';
		_len_Rx = strlen(_in_Rx) + 1;
		if (memcpy_s((void*)_tmp_Rx, _len_Rx, _in_Rx, _len_Rx)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_in_Ry)
	{
		_in_Ry[_len_Ry - 1] = '\0';
		_len_Ry = strlen(_in_Ry) + 1;
		if (memcpy_s((void*)_tmp_Ry, _len_Ry, _in_Ry, _len_Ry)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_in_key) {
		if (memcpy_s(_tmp_key, _len_key, _in_key, _len_key)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_in_secret) {
		if (memcpy_s(_tmp_secret, _len_secret, _in_secret, _len_secret)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_in_fault) {
		if (memcpy_s(_tmp_fault, _len_fault, _in_fault, _len_fault)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}

err:
	if (_in_Rx) free(_in_Rx);
	if (_in_Ry) free(_in_Ry);
	if (_in_key) free(_in_key);
	if (_in_secret) free(_in_secret);
	if (_in_fault) free(_in_fault);
	return status;
}

static sgx_status_t SGX_CDECL sgx_e_mpz_add(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_e_mpz_add_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_e_mpz_add_t* ms = SGX_CAST(ms_e_mpz_add_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_str_a = ms->ms_str_a;
	size_t _len_str_a = ms->ms_str_a_len ;
	char* _in_str_a = NULL;
	char* _tmp_str_b = ms->ms_str_b;
	size_t _len_str_b = ms->ms_str_b_len ;
	char* _in_str_b = NULL;

	CHECK_UNIQUE_POINTER(_tmp_str_a, _len_str_a);
	CHECK_UNIQUE_POINTER(_tmp_str_b, _len_str_b);

	//
	// fence after pointer checks
	//
	sgx_lfence();

	if (_tmp_str_a != NULL && _len_str_a != 0) {
		_in_str_a = (char*)malloc(_len_str_a);
		if (_in_str_a == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_str_a, _len_str_a, _tmp_str_a, _len_str_a)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

		_in_str_a[_len_str_a - 1] = '\0';
		if (_len_str_a != strlen(_in_str_a) + 1)
		{
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_tmp_str_b != NULL && _len_str_b != 0) {
		_in_str_b = (char*)malloc(_len_str_b);
		if (_in_str_b == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_str_b, _len_str_b, _tmp_str_b, _len_str_b)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

		_in_str_b[_len_str_b - 1] = '\0';
		if (_len_str_b != strlen(_in_str_b) + 1)
		{
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}

	ms->ms_retval = e_mpz_add(_in_str_a, _in_str_b);

err:
	if (_in_str_a) free(_in_str_a);
	if (_in_str_b) free(_in_str_b);
	return status;
}

static sgx_status_t SGX_CDECL sgx_e_mpz_mul(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_e_mpz_mul_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_e_mpz_mul_t* ms = SGX_CAST(ms_e_mpz_mul_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_str_a = ms->ms_str_a;
	size_t _len_str_a = ms->ms_str_a_len ;
	char* _in_str_a = NULL;
	char* _tmp_str_b = ms->ms_str_b;
	size_t _len_str_b = ms->ms_str_b_len ;
	char* _in_str_b = NULL;

	CHECK_UNIQUE_POINTER(_tmp_str_a, _len_str_a);
	CHECK_UNIQUE_POINTER(_tmp_str_b, _len_str_b);

	//
	// fence after pointer checks
	//
	sgx_lfence();

	if (_tmp_str_a != NULL && _len_str_a != 0) {
		_in_str_a = (char*)malloc(_len_str_a);
		if (_in_str_a == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_str_a, _len_str_a, _tmp_str_a, _len_str_a)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

		_in_str_a[_len_str_a - 1] = '\0';
		if (_len_str_a != strlen(_in_str_a) + 1)
		{
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_tmp_str_b != NULL && _len_str_b != 0) {
		_in_str_b = (char*)malloc(_len_str_b);
		if (_in_str_b == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_str_b, _len_str_b, _tmp_str_b, _len_str_b)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

		_in_str_b[_len_str_b - 1] = '\0';
		if (_len_str_b != strlen(_in_str_b) + 1)
		{
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}

	ms->ms_retval = e_mpz_mul(_in_str_a, _in_str_b);

err:
	if (_in_str_a) free(_in_str_a);
	if (_in_str_b) free(_in_str_b);
	return status;
}

static sgx_status_t SGX_CDECL sgx_e_mpf_div(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_e_mpf_div_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_e_mpf_div_t* ms = SGX_CAST(ms_e_mpf_div_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_str_a = ms->ms_str_a;
	size_t _len_str_a = ms->ms_str_a_len ;
	char* _in_str_a = NULL;
	char* _tmp_str_b = ms->ms_str_b;
	size_t _len_str_b = ms->ms_str_b_len ;
	char* _in_str_b = NULL;

	CHECK_UNIQUE_POINTER(_tmp_str_a, _len_str_a);
	CHECK_UNIQUE_POINTER(_tmp_str_b, _len_str_b);

	//
	// fence after pointer checks
	//
	sgx_lfence();

	if (_tmp_str_a != NULL && _len_str_a != 0) {
		_in_str_a = (char*)malloc(_len_str_a);
		if (_in_str_a == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_str_a, _len_str_a, _tmp_str_a, _len_str_a)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

		_in_str_a[_len_str_a - 1] = '\0';
		if (_len_str_a != strlen(_in_str_a) + 1)
		{
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_tmp_str_b != NULL && _len_str_b != 0) {
		_in_str_b = (char*)malloc(_len_str_b);
		if (_in_str_b == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_str_b, _len_str_b, _tmp_str_b, _len_str_b)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

		_in_str_b[_len_str_b - 1] = '\0';
		if (_len_str_b != strlen(_in_str_b) + 1)
		{
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}

	ms->ms_retval = e_mpf_div(_in_str_a, _in_str_b, ms->ms_digits);

err:
	if (_in_str_a) free(_in_str_a);
	if (_in_str_b) free(_in_str_b);
	return status;
}

static sgx_status_t SGX_CDECL sgx_e_get_result(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_e_get_result_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_e_get_result_t* ms = SGX_CAST(ms_e_get_result_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_str_c = ms->ms_str_c;



	ms->ms_retval = e_get_result(_tmp_str_c, ms->ms_len);


	return status;
}

static sgx_status_t SGX_CDECL sgx_e_pi(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_e_pi_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_e_pi_t* ms = SGX_CAST(ms_e_pi_t*, pms);
	sgx_status_t status = SGX_SUCCESS;



	ms->ms_retval = e_pi(ms->ms_digits);


	return status;
}

SGX_EXTERNC const struct {
	size_t nr_ecall;
	struct {void* ecall_addr; uint8_t is_priv; uint8_t is_switchless;} ecall_table[8];
} g_ecall_table = {
	8,
	{
		{(void*)(uintptr_t)sgx_tgmp_init, 0, 0},
		{(void*)(uintptr_t)sgx_e_send_public, 0, 0},
		{(void*)(uintptr_t)sgx_e_send_private, 0, 0},
		{(void*)(uintptr_t)sgx_e_mpz_add, 0, 0},
		{(void*)(uintptr_t)sgx_e_mpz_mul, 0, 0},
		{(void*)(uintptr_t)sgx_e_mpf_div, 0, 0},
		{(void*)(uintptr_t)sgx_e_get_result, 0, 0},
		{(void*)(uintptr_t)sgx_e_pi, 0, 0},
	}
};

SGX_EXTERNC const struct {
	size_t nr_ocall;
} g_dyn_entry_table = {
	0,
};


