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
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "EnclaveGmpTest_u.h"
#include "create_enclave.h"
#include "sgx_detect.h"
#include "serialize.h"

#define SIZE 1000
#define NUM_POINT_ADDITIONS 100
#define NUM_HASHES 100
#define USERS 100
#define FAULTS 10
#define HASH 1

#define ENCLAVE_NAME "EnclaveGmpTest.signed.so"

int *rand_list;

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

struct DataItem {
   struct Point data;
   int key; //user_id
   int secret;
   int fault;

};

struct DataItem* hashArray[SIZE];
struct DataItem* dummyItem;
struct DataItem* item;
struct Point* dummyPoint;
struct Point* point;


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










int main (int argc, char *argv[])
{
	sgx_launch_token_t token= { 0 };
	sgx_enclave_id_t eid= 0;
	sgx_status_t status;
	int updated= 0;
	int rv= 0;
	unsigned long support;
	mpz_t a, b, c;
	mpf_t fc;
	char *str_a, *str_b, *str_c, *str_fc;
	size_t len;
	int digits= 12; /* For demo purposes */

	int  *users, *faults;

	if ( argc != 3 ) {
		fprintf(stderr, "usage: sgxgmptest users faults\n");
		return 1;
	}

	/*
	mpz_init_set_str(a, argv[1], 10);
	mpz_init_set_str(b, argv[2], 10);
	mpz_init(c);
	mpf_init(fc);
	*/
	
	users = (int) argv[1];
	faults = (int) argv[2];

	mpz_init(EC.a);
        mpz_init(EC.b);
        mpz_init(EC.p);

        struct Point P, R, Aggregate;
        mpz_init_set_ui(R.x, 0);
        mpz_init_set_ui(R.y, 0);
        mpz_init(P.x);
        mpz_init(P.y);
        mpz_init_set_ui(Aggregate.x, 0);
        mpz_init_set_ui(Aggregate.y, 0);

        mpz_t m;
        mpz_init(m);

        mpz_set_str(EC.p, "0xc1c627e1638fdc8e24299bb041e4e23af4bb5427", 0);
        mpz_set_str(EC.a, "0xc1c627e1638fdc8e24299bb041e4e23af4bb5424", 0);
        mpz_set_str(EC.b, "0x877a6d84155a1de374b72d9f9d93b36bb563b2ab", 0);
	mpz_set_str(P.x, "0x010aff82b3ac72569ae645af3b527be133442131", 0);
        mpz_set_str(P.y, "0x46b8ec1e6d71e5ecb549614887d57a287df573cc", 0);

	FILE* stream = fopen("WELL-00001_20170317140005.csv", "r");

        rand_list = getRandomList(&users);

        int   plaintext = 0, hash = HASH, counter = 0;
        char line[1024], int_to_hex[&users];

	while (fgets(line, 1024, stream) && counter < (users)){
                char* tmp = strdup(line);
                //printf("Field 3 would be %d\n", (int) getfield(tmp, 3) % 6 );

                plaintext = (int) getfield(tmp, 3) % 7;
                sprintf(int_to_hex, "0x%x", plaintext);
                mpz_set_str(m, int_to_hex, 0);
                sprintf(int_to_hex, "0x%x", hash);
                mpz_add(m, m, int_to_hex);
                sprintf(int_to_hex, "0x%x", *(rand_list + counter));
                mpz_add(m, m, int_to_hex);
                Scalar_Multiplication(P, &R, m);

		if (counter > faults){

                        insert(counter, R, *(rand_list + counter), 1);

                }
                else{

                        insert(counter, P, *(rand_list + counter), 0);

                }

                free(tmp);
                counter++;
        }

	int hashIndex;

	for(int i = 0; i < &users; i++){

                search(i, m);
        }

        for(int i = 0; i < &users; i++){

                hashIndex = hashCode(i);

                Point_Addition(hashArray[hashIndex]->data, Aggregate, &Aggregate);
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













	//Send public params into enclave

	char *EC_p, *EC_a, *EC_b, *Px, *Py, *Rx, *Ry;
	EC_p = mpz_serialize(EC.p);
	EC_a = mpz_serialize(EC.a);
	EC_b = mpz_serialize(EC.b);
	Px = mpz_serialize(P.x);
	Py = mpz_serialize(P.y);
	//rand_list
	
	status = e_send_public(eid, &len, EC_p, EC_a, EC_b, Px, Py, rand_list, users, faults);
	
	for(int i = 0; i < users; i++){
	
		Rx = mpz_serialize(hashArray[i]->data.x);
		Ry = mpz_serialize(hashArray[i]->data.y);
		
		status = e_send_private(eid, &len, Rx, Ry, hashArray[i]->key, hashArray[i]->secret, hashArray[i]->fault);
	
	}
	







	str_a= mpz_serialize(a);
	str_b= mpz_serialize(b);
	//if ( str_a == NULL || str_b == NULL ) {
	//	fprintf(stderr, "could not convert mpz to string");
	//	return 1;
	//}

	/* Add the numbers */
	clock_t begin = clock();

	status= e_mpz_add(eid, &len, str_a, str_b);

        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Time for Point Addition: %f seconds\n", time_spent);


	/*
	if ( status != SGX_SUCCESS ) {
		fprintf(stderr, "ECALL test_mpz_add_ui: 0x%04x\n", status);
		return 1;
	}
	if ( !len ) {
		fprintf(stderr, "e_mpz_add: invalid result\n");
		return 1;
	}
	*/

	str_c= malloc(len+1);
	status= e_get_result(eid, &rv, str_c, len);
	if ( status != SGX_SUCCESS ) {
		fprintf(stderr, "ECALL e_mpz_get_result: 0x%04x\n", status);
		return 1;
	}
	if ( rv == 0 ) {
		fprintf(stderr, "e_get_result: bad parameters\n");
		return 1;
	}

	if ( mpz_deserialize(&c, str_c) == -1 ) {
		fprintf(stderr, "mpz_deserialize: bad integer string\n");
		return 1;
	}

	//gmp_printf("iadd : %Zd + %Zd = %Zd\n\n", a, b, c);

	/* Multiply the numbers */
	
	begin = clock();
	
	status= e_mpz_mul(eid, &len, str_a, str_b);
	
	end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Time for Hash: %f seconds\n", time_spent);

	/*
	if ( status != SGX_SUCCESS ) {
		fprintf(stderr, "ECALL test_mpz_mul: 0x%04x\n", status);
		return 1;
	}
	if ( !len ) {
		fprintf(stderr, "e_mpz_mul: invalid result\n");
		return 1;
	}

	str_c= realloc(str_c, len+1);

	status= e_get_result(eid, &rv, str_c, len);
	if ( status != SGX_SUCCESS ) {
		fprintf(stderr, "ECALL e_mpz_get_result: 0x%04x\n", status);
		return 1;
	}
	if ( rv == 0 ) {
		fprintf(stderr, "e_get_result: bad parameters\n");
		return 1;
	}

	if ( mpz_deserialize(&c, str_c) == -1 ) {
		fprintf(stderr, "mpz_deserialize: bad float string\n");
		return 1;
	}

	gmp_printf("imul : %Zd * %Zd = %Zd\n\n", a, b, c);
	*/


	/* Integer division */

	begin = clock();

	//status= e_mpz_div(eid, &len, str_a, str_b);

	end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Time for Table: %f seconds\n", time_spent);

	/*
	if ( status != SGX_SUCCESS ) {
		fprintf(stderr, "ECALL test_mpz_div: 0x%04x\n", status);
		return 1;
	}
	if ( len == 0 ) {
		fprintf(stderr, "e_mpz_div: invalid result\n");
		return 1;
	}

	str_c= realloc(str_c, len+1);
	status= e_get_result(eid, &rv, str_c, len);
	if ( status != SGX_SUCCESS ) {
		fprintf(stderr, "ECALL e_mpz_get_result: 0x%04x\n", status);
		return 1;
	}
	if ( rv == 0 ) {
		fprintf(stderr, "e_get_result: bad parameters\n");
		return 1;
	}

	if ( mpz_deserialize(&c, str_c) == -1 ) {
		fprintf(stderr, "mpz_deserialize: bad float string\n");
		return 1;
	}

	gmp_printf("idiv : %Zd / %Zd = %Zd\n\n", a, b, c);
	*/

    /*
	 * Floating point division.
	 *
	 * When fetching floating point results, we need to specify the number
	 * of decimal digits. For this demo, we'll use 12.
	 */

	status= e_mpf_div(eid, &len, str_a, str_b, digits);
	if ( status != SGX_SUCCESS ) {
		fprintf(stderr, "ECALL test_mpz_div: 0x%04x\n", status);
		return 1;
	}
	if ( len == 0 ) {
		fprintf(stderr, "e_mpf_div: invalid result\n");
		return 1;
	}

	str_fc= malloc(len+1);

	status= e_get_result(eid, &rv, str_fc, len);
	if ( status != SGX_SUCCESS ) {
		fprintf(stderr, "ECALL e_get_result: 0x%04x\n", status);
		return 1;
	}
	if ( rv == 0 ) {
		fprintf(stderr, "e_get_result: bad parameters\n");
		return 1;
	}

	if ( mpf_deserialize(&fc, str_fc, digits) == -1 ) {
		fprintf(stderr, "mpf_deserialize: bad float string\n");
		return 1;
	}

	//gmp_printf("fdiv : %Zd / %Zd = %.*Ff\n\n", a, b, digits, fc);

	return 0;
}

