/*

Copyright 2018 Intel Corporation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <sgx_urts.h>
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sgx_detect.h"
#include "EnclaveGmpTest_u.h"
#include "create_enclave.h"
#include "serialize.h"

#define ENCLAVE_NAME "EnclaveGmpTest.signed.so"

void usage ();

void usage () {
	fprintf(stderr, "usage: sgxgmppi digits\n");
	exit(1);
}

int main (int argc, char *argv[])
{
	sgx_launch_token_t token= { 0 };
	sgx_enclave_id_t eid= 0;
	sgx_status_t status;
	int updated= 0;
	unsigned long support;
	mpf_t pi;
	char *pi_str;
	uint64_t digits;
	int opt, rv;
	size_t len;

	while ( (opt= getopt(argc, argv, "h")) != -1 ) {
		switch (opt) {
		case 'h':
		default:
			usage();
		}
	}

	argc-= optind;
	argv+= optind;

	if ( argc != 1 ) usage();

	digits= strtoull(argv[0], NULL, 10);
	if ( digits == 0 ) {
		fprintf(stderr, "invalid digit count\n");
		return 1;
	}

#ifndef SGX_HW_SIM
	support= get_sgx_support();
	if ( ! SGX_OK(support) ) {
		sgx_support_perror(support);
		return 1;
	}
#endif

	status= sgx_create_enclave_search(ENCLAVE_NAME, SGX_DEBUG_FLAG,
		 &token, &updated, &eid, 0);
	if ( status != SGX_SUCCESS ) {
		if ( status == SGX_ERROR_ENCLAVE_FILE_ACCESS ) {
			fprintf(stderr, "sgx_create_enclave: %s: file not found\n",
				ENCLAVE_NAME);
			fprintf(stderr, "Did you forget to set LD_LIBRARY_PATH?\n");
		} else {
			fprintf(stderr, "%s: 0x%04x\n", ENCLAVE_NAME, status);
		}
		return 1;
	}

	fprintf(stderr, "Enclave launched\n");

	status= tgmp_init(eid);
	if ( status != SGX_SUCCESS ) {
		fprintf(stderr, "ECALL test_mpz_add: 0x%04x\n", status);
		return 1;
	}

	fprintf(stderr, "libtgmp initialized\n");

	mpf_init(pi);

	status= e_pi(eid, &len, digits);
	if ( status != SGX_SUCCESS ) {
		fprintf(stderr, "ECALL e_pi: 0x%04x\n", status);
		return 1;
	}
	if ( len == 0 ) {
		fprintf(stderr, "e_pi: no result\n");
		return 1;
	}

	pi_str= malloc(len+1);

	status= e_get_result(eid, &rv, pi_str, len);
	if ( status != SGX_SUCCESS ) {
		fprintf(stderr, "ECALL e_get_result: 0x%04x\n", status);
		return 1;
	}
	if ( rv == 0 ) {
		fprintf(stderr, "e_get_result: bad parameters");
		return 1;
	}

	if ( mpf_deserialize(&pi, pi_str, digits) == -1 ) {
		fprintf(stderr, "mpf_deserialize: bad result string");
		return 1;
	}

	gmp_printf("pi : %.*Ff\n", digits, pi);

	return 0;
}

