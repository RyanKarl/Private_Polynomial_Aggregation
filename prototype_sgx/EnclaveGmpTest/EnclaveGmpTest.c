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
#define HASH 1
#define SIZE 1000

struct Point {
        mpz_t x;
        mpz_t y;
};

struct DataItem {
   struct Point data;
   int key; //user_id
   int secret;
   int fault;
};

struct Elliptic_Curve {
        mpz_t a;
        mpz_t b;
        mpz_t p;
};


int *rand_list, users, faults;

struct Elliptic_Curve EC;
struct Point P, R;

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

void Point_Doubling(struct Point P, struct Point *R)
{
        mpz_t slope, temp;
        mpz_init(temp);
        mpz_init(slope);

        if(mpz_cmp_ui(P.y, 0) != 0) {
                mpz_mul_ui(temp, P.y, 2);
                mpz_invert(temp, temp, EC.p);
                mpz_mul(slope, P.x, P.x);
                mpz_mul_ui(slope, slope, 3);
                mpz_add(slope, slope, EC.a);
                mpz_mul(slope, slope, temp);
                mpz_mod(slope, slope, EC.p);
                mpz_mul(R->x, slope, slope);
                mpz_sub(R->x, R->x, P.x);
                mpz_sub(R->x, R->x, P.x);
                mpz_mod(R->x, R->x, EC.p);
                mpz_sub(temp, P.x, R->x);
                mpz_mul(R->y, slope, temp);
                mpz_sub(R->y, R->y, P.y);
                mpz_mod(R->y, R->y, EC.p);
        } else {
                mpz_set_ui(R->x, 0);
                mpz_set_ui(R->y, 0);
        }
        mpz_clear(temp);
        mpz_clear(slope);
}

void Point_Addition(struct Point P, struct Point Q, struct Point *R)
{
        mpz_mod(P.x, P.x, EC.p);
        mpz_mod(P.y, P.y, EC.p);
        mpz_mod(Q.x, Q.x, EC.p);
        mpz_mod(Q.y, Q.y, EC.p);

        if(mpz_cmp_ui(P.x, 0) == 0 && mpz_cmp_ui(P.y, 0) == 0) {
                mpz_set(R->x, Q.x);
                mpz_set(R->y, Q.y);
                return;
        }

        if(mpz_cmp_ui(Q.x, 0) == 0 && mpz_cmp_ui(Q.y, 0) == 0) {
                mpz_set(R->x, P.x);
                mpz_set(R->y, P.y);
                return;
        }

        mpz_t temp;
        mpz_init(temp);

        if(mpz_cmp_ui(Q.y, 0) != 0) {
                mpz_sub(temp, EC.p, Q.y);
                mpz_mod(temp, temp, EC.p);
        } else
                mpz_set_ui(temp, 0);

        //gmp_printf("\n temp=%Zd\n", temp);

        if(mpz_cmp(P.y, temp) == 0 && mpz_cmp(P.x, Q.x) == 0) {
                mpz_set_ui(R->x, 0);
                mpz_set_ui(R->y, 0);
                mpz_clear(temp);
                return;
        }

        if(mpz_cmp(P.x, Q.x) == 0 && mpz_cmp(P.y, Q.y) == 0)    {
                Point_Doubling(P, R);

                mpz_clear(temp);
                return;
        } else {
                mpz_t slope;
                mpz_init_set_ui(slope, 0);

                mpz_sub(temp, P.x, Q.x);
                mpz_mod(temp, temp, EC.p);
                mpz_invert(temp, temp, EC.p);
                mpz_sub(slope, P.y, Q.y);
                mpz_mul(slope, slope, temp);
                mpz_mod(slope, slope, EC.p);
                mpz_mul(R->x, slope, slope);
                mpz_sub(R->x, R->x, P.x);
                mpz_sub(R->x, R->x, Q.x);
                mpz_mod(R->x, R->x, EC.p);
                mpz_sub(temp, P.x, R->x);
                mpz_mul(R->y, slope, temp);
                mpz_sub(R->y, R->y, P.y);
                mpz_mod(R->y, R->y, EC.p);

                mpz_clear(temp);
                mpz_clear(slope);
                return;
        }
}

void Scalar_Multiplication(struct Point P, struct Point *R, mpz_t m)
{
        struct Point Q, T;
        mpz_init(Q.x); mpz_init(Q.y);
        mpz_init(T.x); mpz_init(T.y);
        long no_of_bits, loop;

        no_of_bits = mpz_sizeinbase(m, 2);
        mpz_set_ui(R->x, 0);
        mpz_set_ui(R->y, 0);
        if(mpz_cmp_ui(m, 0) == 0)
                return;

        mpz_set(Q.x, P.x);
        mpz_set(Q.y, P.y);
        if(mpz_tstbit(m, 0) == 1){
                mpz_set(R->x, P.x);
                mpz_set(R->y, P.y);
        }

        for(loop = 1; loop < no_of_bits; loop++) {
                mpz_set_ui(T.x, 0);
                mpz_set_ui(T.y, 0);
                Point_Doubling(Q, &T);

                //gmp_printf("\n %Zd %Zd %Zd %Zd ", Q.x, Q.y, T.x, T.y);
                mpz_set(Q.x, T.x);
                mpz_set(Q.y, T.y);
                mpz_set(T.x, R->x);
                mpz_set(T.y, R->y);
                if(mpz_tstbit(m, loop))
                        Point_Addition(T, Q, R);
        }

        mpz_clear(Q.x); mpz_clear(Q.y);
        mpz_clear(T.x); mpz_clear(T.y);
}

int hashCode(int key) {
   return key % SIZE;
}

/*
void search(int key, mpz_t m) {
   //get the hash
   int hashIndex = hashCode(key);
   char int_to_hex[1024];
   //move in array until an empty
   while(hashArray[hashIndex] != NULL) {

      if(hashArray[hashIndex]->key == key){

              if(hashArray[hashIndex]->fault == 0){

                        hashArray[hashIndex]->fault = 1;

                        int hash = HASH;
                        //mpz_set_str(m, int_to_hex, 0);
                        mpz_set_str(m, "0x0", 0);
                        sprintf(int_to_hex, "0x%x", hash);
                        mpz_add(m, m, int_to_hex);
                        sprintf(int_to_hex, "0x%x", *(rand_list + key));
                        mpz_add(m, m, int_to_hex);
                        Scalar_Multiplication(hashArray[hashIndex]->data, &hashArray[hashIndex]->data, m);

                        return;

              }
              else{

                      return;

              }

        }

      //go to next cell
      ++hashIndex;

      //wrap around the table
      hashIndex %= SIZE;
   }

   return;
}

*/

void insert(int key, struct Point data, int secret, int f) {

   struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
   item->data = data;
   item->key = key;
   item->fault = f;
   item->secret = secret;

   //get the hash
   int hashIndex = hashCode(key);

   //move in array until an empty or deleted cell
   while(hashArray[hashIndex] != NULL && hashArray[hashIndex]->key != -1) {
      //go to next cell
      ++hashIndex;

      //wrap around the table
      hashIndex %= SIZE;
   }

   hashArray[hashIndex] = item;
}

/*
int * getRandomList(int num_users){
        int sum = 0;
        int static *ptr;
        ptr = (int*) malloc(num_users * sizeof(int));

        srand( (unsigned)time( NULL ) );

        for(int i = 0; i < num_users - 1; i++)
        {

                *(ptr + i) = rand() % 11;
                sum += *(ptr + i);
                //printf("sum = %d \n", sum);
        }

        *(ptr + num_users - 1) = -sum;

        //for(int i = 0; i < num_users; ++i)
        //{
        //      printf("Val = %d \n", *(ptr + i));
        //}

        return ptr;

}

*/

const char* getfield(char* line, int num){
    const char* tok;
    for (tok = strtok(line, ",");
            tok && *tok;
            tok = strtok(NULL, ",\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}

size_t e_send_public(char *EC_p, char *EC_a, char *EC_b, char *Px, char *Py, int *rand_l, int *u, int *f){

	if ( result != NULL ) {
                gmp_free_func(result, NULL);
                result = NULL;
                len_result= 0;
        }

	//mpz_t EC1, EC2, EC3, P1, P2;
        //mpz_inits(EC1, EC2, EC3, P1, P2, NULL);
	mpz_t m;
	mpz_init(m);

        /* Deserialize */

        if ( mpz_deserialize(&EC.p, EC_p) == -1 ) return 0;
        if ( mpz_deserialize(&EC.a, EC_a) == -1 ) return 0;
	if ( mpz_deserialize(&EC.b, EC_b) == -1 ) return 0;
	if ( mpz_deserialize(&P.x, Px) == -1 ) return 0;
	if ( mpz_deserialize(&P.y, Py) == -1 ) return 0;

	//mpz_set_str(ECp, "0xc1c627e1638fdc8e24299bb041e4e23af4bb5427", 0);

	rand_list = *rand_l;
	users = u;
	faults = f;

	char int_to_hex[1024];

	for(int i = 0; i < users; i++){
	
                int hash = HASH;
                mpz_set_str(m, "0x0", 0);
                //quick fix
		//sprintf(int_to_hex, "0x%x", hash);
                //mpz_add(m, m, int_to_hex);
                //sprintf(int_to_hex, "0x%x", *(rand_list + i));
                //mpz_add(m, m, int_to_hex);
                Scalar_Multiplication(P, &R, m);

		insert(i, R, *(rand_list + i), 0);
	
	}

        return 0;


}

size_t e_send_private(char *Rx, char *Ry, int *key, int *secret, int *fault){

	if ( result != NULL ) {
                gmp_free_func(result, NULL);
                result = NULL;
                len_result= 0;
        }

	if ( mpz_deserialize(&R.x, Rx) == -1 ) return 0;
        if ( mpz_deserialize(&R.y, Ry) == -1 ) return 0;

	for(int i = 0; i < users; i++){
	
		hashArray[i]->data = R;	
	
	}	

	mpz_init_set_ui(R.x, 0);
        mpz_init_set_ui(R.y, 0);

	for(int i = 0; i < users; i++){

                Point_Addition(hashArray[i]->data, R, &R);
        }

	/* result1 = mpz_serialize(R.x);
	result2 = mpz_serialize(R.y);
        if ( result1 == NULL ) return 0;
	if ( result2 == NULL ) return 0;

        len_result1 = strlen(result1);
	len_result2 = strlen(result2);
	*/

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

/*
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



	if ( result != NULL ) {
		gmp_free_func(result, NULL);
		result= NULL;
		len_result= 0;
	}

	mpz_inits(a, b, c, NULL);


	if ( mpz_deserialize(&a, str_a) == -1 ) return 0;
	if ( mpz_deserialize(&b, str_b) == -1 ) return 0;

	mpz_div(c, a, b);


	result= mpz_serialize(c);
	if ( result == NULL ) return 0;

	len_result= strlen(result);
	return len_result;
}

*/

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

