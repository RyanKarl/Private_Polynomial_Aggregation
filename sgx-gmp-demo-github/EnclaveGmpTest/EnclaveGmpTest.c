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

#include "EnclaveGmpTest_t.h"
#include <sgx_tgmp.h>
#include <sgx_trts.h>
#include <math.h>
#include <string.h>
#include "serialize.h"
#include <time.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUM_POINT_ADDITIONS 100
#define NUM_HASHES 100
#define USERS 100
#define FAULTS 10
#define HASH_STR "10000"
#define SIZE 1000

struct DataItem {
   int data;   
   int key;
};

struct Elliptic_Curve {
        mpz_t a;
        mpz_t b;
        mpz_t p;
};

struct Point {
        mpz_t x;
        mpz_t y;
};

struct Elliptic_Curve EC;

struct DataItem* hashArray[SIZE];
struct DataItem* dummyItem;
struct DataItem* item;

void *(*gmp_realloc_func)(void *, size_t, size_t);
void (*gmp_free_func)(void *, size_t);

void *reallocate_function(void *, size_t, size_t);
void free_function(void *, size_t);

void e_calc_pi (mpf_t *pi, uint64_t digits);

/*
 * Use a global to store our results. A real program would need something
 * more sophisticated than this.
 */

char *result;
size_t len_result= 0;

void tgmp_init()
{
	result= NULL;
	len_result= 0;

	mp_get_memory_functions(NULL, &gmp_realloc_func, &gmp_free_func);
	mp_set_memory_functions(NULL, &reallocate_function, &free_function);
}

void free_function (void *ptr, size_t sz)
{
	if ( sgx_is_within_enclave(ptr, sz) ) gmp_free_func(ptr, sz);
	else abort();
}

void *reallocate_function (void *ptr, size_t osize, size_t nsize)
{
	if ( ! sgx_is_within_enclave(ptr, osize) ) abort();

	return gmp_realloc_func(ptr, osize, nsize);
}

int e_get_result(char *str, size_t len)
{
	/* Make sure the application doesn't ask for more bytes than 
	 * were allocated for the result. */

	if ( len > len_result ) return 0;

	/*
	 * Marshal our result out of the enclave. Make sure the destination
	 * buffer is completely outside the enclave, and that what we are
	 * copying is completely inside the enclave.
	 */

	if ( result == NULL || str == NULL || len == 0 ) return 0;

	if ( ! sgx_is_within_enclave(result, len) ) return 0;

	if ( sgx_is_outside_enclave(str, len+1) ) { /* Include terminating NULL */
		strncpy(str, result, len); 
		str[len]= '\0';

		gmp_free_func(result, NULL);
		result= NULL;
		len_result= 0;

		return 1;
	}

	return 0;
}

size_t e_mpz_add(char *str_a, char *str_b)
{

	mpz_t a, b, c, m, Px, Py, Qx, Qy, Rx, Ry, ECp, ECa, ECb;

	mpz_init(m);
	mpz_init(Px);
	mpz_init(Py);
	mpz_init(Qx);
	mpz_init(Qy);
	mpz_init(Rx);
	mpz_init(Ry);
	mpz_init(ECp);
	mpz_init(ECa);
	mpz_init(ECb);

	mpz_set_str(ECp, "0xc1c627e1638fdc8e24299bb041e4e23af4bb5427", 0);
        mpz_set_str(ECa, "0xc1c627e1638fdc8e24299bb041e4e23af4bb5424", 0);
        mpz_set_str(ECb, "0x877a6d84155a1de374b72d9f9d93b36bb563b2ab", 0);
	mpz_set_str(Px, "0x010aff82b3ac72569ae645af3b527be133442131", 0);
        mpz_set_str(Py, "0x46b8ec1e6d71e5ecb549614887d57a287df573cc", 0);
	mpz_set_str(Qx, "0x41da1a8f74ff8d3f1ce20ef3e9d8865c96014fe3", 0);
        mpz_set_str(Qy, "0x73ca143c9badedf2d9d3c7573307115ccfe04f13", 0);
	mpz_set_str(m, "0x00542d46e7b3daac8aeb81e533873aabd6d74bb710", 0);

	for(int i = 0; i <= NUM_POINT_ADDITIONS; i++){

        mpz_mod(Px, Px, ECp);
        mpz_mod(Py, Py, ECp);
        mpz_mod(Qx, Qx, ECp);
        mpz_mod(Qy, Qy, ECp);

        if(mpz_cmp_ui(Px, 0) == 0 && mpz_cmp_ui(Py, 0) == 0) {
                mpz_set(Rx, Qx);
                mpz_set(Ry, Qy);
              
        }

        if(mpz_cmp_ui(Qx, 0) == 0 && mpz_cmp_ui(Qy, 0) == 0) {
                mpz_set(Rx, Px);
                mpz_set(Ry, Py);
                
        }

        mpz_t temp;
        mpz_init(temp);

        if(mpz_cmp_ui(Qy, 0) != 0) { 
                mpz_sub(temp, ECp, Qy);
                mpz_mod(temp, temp, ECp);
        } else
                mpz_set_ui(temp, 0);

        //gmp_printf("\n temp=%Zd\n", temp);

        if(mpz_cmp(Py, temp) == 0 && mpz_cmp(Px, Qx) == 0) {
                mpz_set_ui(Rx, 0);
                mpz_set_ui(Ry, 0);
                mpz_clear(temp);
       
        }
        
        if(mpz_cmp(Px, Qx) == 0 && mpz_cmp(Py, Qy) == 0)    {
                //Point_Doubling(P, R);
                
                mpz_clear(temp);
               
        } else {
                mpz_t slope;
                mpz_init_set_ui(slope, 0);
                mpz_sub(temp, Px, Qx); 
		mpz_mod(temp, temp, ECp);
                mpz_invert(temp, temp, ECp);
                mpz_sub(slope, Py, Qy);
                mpz_mul(slope, slope, temp);
		mpz_mod(slope, slope, ECp);
                mpz_mul(Rx, slope, slope);
                mpz_sub(Rx, Rx, Px);
                mpz_sub(Rx, Rx, Qx);
                mpz_mod(Rx, Rx, ECp);
                mpz_sub(temp, Px, Rx);
                mpz_mul(Ry, slope, temp);
                mpz_sub(Ry, Ry, Py);
		mpz_mod(Ry, Ry, ECp);
                
                mpz_clear(temp);
                mpz_clear(slope);
        }

	}


	/*
	 * Marshal untrusted values into the enclave so we don't accidentally
	 * leak secrets to untrusted memory.
	 *
	 * This is overkill for the trivial example in this function, but
	 * it's best to develop good coding habits.
	 */

	/* Clear the last, serialized result */

	

	if ( result != NULL ) {
		gmp_free_func(result, NULL);
		result= NULL;
		len_result= 0;
	}

	mpz_inits(a, b, c, NULL);

	/* Deserialize */

	if ( mpz_deserialize(&a, str_a) == -1 ) return 0;
	if ( mpz_deserialize(&b, str_b) == -1 ) return 0;

	mpz_add(c, a, b);

	/* Serialize the result */

	result= mpz_serialize(c);
	if ( result == NULL ) return 0;

	len_result= strlen(result);
	return len_result;

}

size_t e_mpz_mul(char *str_a, char *str_b)
{
	mpz_t a, b, c;
	
	for(int i=0; i<= NUM_HASHES; i++){

		unsigned char* str_hash = HASH_STR;

		unsigned long hash = 5381;
    		int x;

    		while (x = *str_hash++)
        	hash = ((hash << 5) + hash) + x; 

	}


	/* Marshal untrusted values into the enclave. */

	/* Clear the last, serialized result */

	if ( result != NULL ) {
		gmp_free_func(result, NULL);
		result= NULL;
		len_result= 0;
	}

	mpz_inits(a, b, c, NULL);

	/* Deserialize */

	if ( mpz_deserialize(&a, str_a) == -1 ) return 0;
	if ( mpz_deserialize(&b, str_b) == -1 ) return 0;

	mpz_mul(c, a, b);

	/* Serialize the result */

	result= mpz_serialize(c);
	if ( result == NULL ) return 0;

	len_result= strlen(result);
	return len_result;
}

size_t e_mpz_div(char *str_a, char *str_b)
{

	dummyItem = (struct DataItem*) malloc(sizeof(struct DataItem));
   	dummyItem->data = -1;  
   	dummyItem->key = -1; 

	for(int i=0; i<USERS; i++){

		struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
   		item->data = i;  
   		item->key = i;

   		int hashIndex = i % SIZE;

   		//move in array until an empty or deleted cell
   		while(hashArray[hashIndex] != NULL && hashArray[hashIndex]->key != -1) {
      			//go to next cell
      			++hashIndex;
		
      			//wrap around the table
      			hashIndex = hashIndex % SIZE;
   		}
	
   		hashArray[hashIndex] = item;

	}


	for(int j=0; j<FAULTS; j++){
 
   		int hashIndex = j % SIZE;

   		//move in array until an empty 
   		while(hashArray[hashIndex] != NULL) {

			if(hashArray[hashIndex]->key == j)
         			break; 

      			//go to next cell
	      		++hashIndex;

      			//wrap around the table
      		hashIndex %= SIZE;
   
		}	
	}

	mpz_t a, b, c;

	/* Marshal untrusted values into the enclave */

	/* Clear the last, serialized result */

	if ( result != NULL ) {
		gmp_free_func(result, NULL);
		result= NULL;
		len_result= 0;
	}

	mpz_inits(a, b, c, NULL);

	/* Deserialize */

	if ( mpz_deserialize(&a, str_a) == -1 ) return 0;
	if ( mpz_deserialize(&b, str_b) == -1 ) return 0;

	mpz_div(c, a, b);

	/* Serialize the result */

	result= mpz_serialize(c);
	if ( result == NULL ) return 0;

	len_result= strlen(result);
	return len_result;
}

size_t e_mpf_div(char *str_a, char *str_b, int digits)
{
	mpz_t a, b;
	mpf_t fa, fb, fc;

	/* Marshal untrusted values into the enclave */

	/* Clear the last, serialized result */

	if ( result != NULL ) {
		gmp_free_func(result, NULL);
		result= NULL;
		len_result= 0;
	}

	mpz_inits(a, b, NULL);
	mpf_inits(fa, fb, fc, NULL);

	/* Deserialize */

	if ( mpz_deserialize(&a, str_a) == -1 ) return 0;
	if ( mpz_deserialize(&b, str_b) == -1 ) return 0;

	mpf_set_z(fa, a);
	mpf_set_z(fb, b);

	mpf_div(fc, fa, fb);


	/* Serialize the result */

	result= mpf_serialize(fc, digits);
	if ( result == NULL ) return 0;

	len_result= strlen(result);
	return len_result;
}

/* Use the Chudnovsky equation to rapidly estimate pi */

#define DIGITS_PER_ITERATION 14.1816 /* Roughly */

mpz_t c3, c4, c5;
int pi_init= 0;

size_t e_pi (uint64_t digits)
{
	mpf_t pi;

	/* Clear the last, serialized result */

	if ( result != NULL ) {
		gmp_free_func(result, NULL);
		result= NULL;
		len_result= 0;
	}

	/*
	 * Perform our operations on a variable that's located in the enclave,
	 * then marshal the final value out of the enclave.
	 */

	mpf_init(pi);

	e_calc_pi(&pi, digits+1);

	/* Marshal our result to untrusted memory */

	mpf_set_prec(pi, mpf_get_prec(pi));

	result= mpf_serialize(pi, digits+1);
	if ( result == NULL ) return 0;

	len_result= strlen(result);
	return len_result;
}

void e_calc_pi (mpf_t *pi, uint64_t digits)
{
	uint64_t k, n;
	mp_bitcnt_t precision;
	static double bits= log2(10);
	mpz_t kf, kf3, threekf, sixkf, z1, z2, c4k, c5_3k;
	mpf_t C, sum, div, f2;

	n= (digits/DIGITS_PER_ITERATION)+1;
	precision= (digits * bits)+1;

	mpf_set_default_prec(precision);

	/* Re-initialize the pi variable to use our new precision */

	mpf_set_prec(*pi, precision);

	/*

		426880 sqrt(10005)    inf (6k)! (13591409+545140134k)
		------------------- = SUM ---------------------------
		         pi           k=0   (3k)!(k!)^3(-640320)^3k

		C / pi = SUM (6k)! * (c3 + c4*k) / (3k)!(k!)^3(c5)^3k

		C / pi = SUM f1 / f2

		pi = C / sum

	*/

	mpz_inits(sixkf, z1, z2, kf, kf3, threekf, c4k, c5_3k, NULL);
	mpf_inits(C, sum, div, f2, NULL);

	/* Calculate 'C' */

	mpf_sqrt_ui(C, 10005);
	mpf_mul_ui(C, C, 426880);

	if ( ! pi_init ) {
		/* Constants needed in 'sum'. */

		mpz_inits(c3, c4, c5, NULL);

		mpz_set_ui(c3, 13591409);
		mpz_set_ui(c4, 545140134);
		mpz_set_si(c5, -640320);

		pi_init= 1;
	}


	mpf_set_ui(sum, 0);

	for (k= 0; k< n; ++k) {
		/* Numerator */
		mpz_fac_ui(sixkf, 6*k);
		mpz_mul_ui(c4k, c4, k);
		mpz_add(c4k, c4k, c3);
		mpz_mul(z1, c4k, sixkf);
		mpf_set_z(div, z1);

		/* Denominator */
		mpz_fac_ui(threekf, 3*k);
		mpz_fac_ui(kf, k);
		mpz_pow_ui(kf3, kf, 3);
		mpz_mul(z2, threekf, kf3);
		mpz_pow_ui(c5_3k, c5, 3*k);
		mpz_mul(z2, z2, c5_3k);

		/* Divison */

		mpf_set_z(f2, z2);
		mpf_div(div, div, f2);

		/* Sum */

		mpf_add(sum, sum, div);
	}

	mpf_div(*pi, C, sum);

	mpf_clears(div, sum, f2, NULL);
}

