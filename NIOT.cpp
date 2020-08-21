#include <stdio.h> 
#include <stdlib.h> 
#include <bitset>
#include <gmp.h>
#include "gmpxx.h"
#define NUM_BITS 100


void gen_rand_bits(mpz_class len, mpz_class & bitstring){
    std::string temp;
    for(mpz_class i = 0; i<len; i++) 
        temp.append(std::to_string(rand() % 2));
        
    //std::bitset<NUM_BITS> bitstring(temp);
    mpz_set_str(bitstring, temp, 2);

}


int main(){

  
    //STEP 1

    int rand_i = rand() % 2;

    mpz_t NIOT_prime, prime_range, g, beta_0, beta_1, x_i, C_NIOT, y_0, y_1, gamma_0, gamma_1;
    gmp_randstate_t state;
    mpz_set_str(prime_range, "100000", 10);
    mpz_nextprime(NIOT_prime, prime_range);
    
    gmp_randinit_default (state);
    mpz_urandomm(g, state, prime_range);
    mpz_urandomm(C, state, prime_range);

    mpz_urandomm(x_i, state, prime_range);

    mpz_powm(beta_0, g, x_i, NIOT_prime);
    mpz_powm(beta_1, g, x_i, NIOT_prime);
    mpz_invert(beta_1, beta_1, NIOT_prime);
    mpz_mul(beta_1, beta_1, C);


    std::pair<mpz_t, mpz_t> public_key (beta_0, beta_1);
    std::pair<int, mpz_t> private_key (0, x_i);

    //STEP 2

    mpz_t verify;

    mpz_mul(verify, beta_0, beta_1);
    
    if (mpz_cmp(verify, C) == 0){
        printf("Verification Sucussful");
    }


    //STEP 3
    mpz_urandomm(y_0, state, prime_range);
    mpz_urandomm(y_1, state, prime_range);

    mpz_mpz_powm(gamma_0, beta_0, y_0, NIOT_prime);
    mpz_mpz_powm(gamma_1, beta_1, y_1, NIOT_prime);

    mpz_t r_1, r_2, plen, b_0, b_1;
    size_t NIOT_prime_len mpz_sizeinbase(NIOT_prime, 2);
    
    std::string NIOT_plen = std::to_string(NIOT_prime_len);
    mpz_set_str(plen, NIOT_plen, 10); 
    
    gen_rand_bits(plen, r_1);
    gen_rand_bits(plen, r_2);

   
    mpz_and(b_0, r_0, gamma_0);
    mpz_set_str(b_0, std::to_str(mpz_popcount(b_0)), 10); 

    mpz_and(b_1, r_1, gamma_1);
    mpz_set_str(b_1, std::to_str(mpz_popcount(b_1)), 10); 

    

    return 0;    

}
