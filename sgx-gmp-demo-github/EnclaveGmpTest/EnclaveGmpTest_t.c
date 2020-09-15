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

typedef struct ms_e_mpz_div_t {
	size_t ms_retval;
	char* ms_str_a;
	size_t ms_str_a_len;
	char* ms_str_b;
	size_t ms_str_b_len;
} ms_e_mpz_div_t;

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

static sgx_status_t SGX_CDECL sgx_e_mpz_div(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_e_mpz_div_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_e_mpz_div_t* ms = SGX_CAST(ms_e_mpz_div_t*, pms);
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

	ms->ms_retval = e_mpz_div(_in_str_a, _in_str_b);

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
	struct {void* ecall_addr; uint8_t is_priv; uint8_t is_switchless;} ecall_table[7];
} g_ecall_table = {
	7,
	{
		{(void*)(uintptr_t)sgx_tgmp_init, 0, 0},
		{(void*)(uintptr_t)sgx_e_mpz_add, 0, 0},
		{(void*)(uintptr_t)sgx_e_mpz_mul, 0, 0},
		{(void*)(uintptr_t)sgx_e_mpz_div, 0, 0},
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


