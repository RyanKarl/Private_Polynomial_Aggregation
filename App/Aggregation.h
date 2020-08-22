#ifndef AGGREGATION_H
#define AGGREGATION_H

#include <vector>
#include <utility>

#include <gmp.h>
#include <gmpxx.h>

#include "../Enclave/Enclave_defines.h"

#define NUM_USERS 5
#define PRIME_REPS 20

using std::vector;
using std::pair;

struct User{
       
    int id;
    mpz_class input;
    std::vector<mpz_class> points;
    mpz_class lagrange;
    element_t ciphertext;
};

Parameters parms{
    mpz_class N;
    pairing_t pairing;
    element_t g1, x, hm, pk, h, sigma;
    //H is std::hash
    //LSH is a Locality Sensitive Hash 
    //NIOT is a Noninteractive Oblivious Transfer protocol
};


void compute_points(const std::vector<int> & polynomial, User & u){
       assert(u.points.empty());
       mpz_class tmp_id;
       for(long i = 2; i < polynomial.size(); i++){
           mpz_class tmp = 0;
           for(long j = 2; j <= i; j++){
               tmp_id = u.id;
               mpz_pow_ui(tmp_id.get_mpz_t(), tmp_id.get_mpz_t(), j);
               tmp += tmp_id;
         }
           u.points.push_back(tmp);
       }
     return;
  }


void lagrange_coe(const int i, const int n, User & u, const mpz_class & p){
   u.lagrange = 1;
   mpz_class tmp;
   for(int j = 1; j <= n; j++){
       if(i != j){
         u.lagrange *= -j;
         tmp = i-j;
         mpz_invert(tmp.get_mpz_t(), tmp.get_mpz_t(), p.get_mpz_t());
         u.lagrange *= tmp;
         u.lagrange %= p;
         if(u.lagrange < 0){
           u.lagrange += p;
         }
         //ret = ret * (-j / (i - j)) (modular)       
       }
   }
   return;
}



Parameters setup(Parameters params){
    //Parameters params;
    assert(NUM_USERS >= 3);
   
    mpz_class prime;
    std::vector<int> polynomial;
    //r is prime
    prime = "730750818665451621361119245571504901405976559617";
    assert(mpz_probab_prime_p(prime.get_mpz_t(), PRIME_REPS));
   
    params.N = prime;

    char s[3000] = "type a\nq                                                               8780710799663312522437781984754049815806883199414208211028653399266475630880222957078625     179422662221423155858769582317459277713367317481324925129998224791\nh                        1201601226489114607938882136674053420480295440125131182291961513104720728935970453110284     4802183906537786776\nr 730750818665451621361119245571504901405976559617\nexp2 159\nexp1      107\nsign1 1\nsign0 1\n";
   
    pairing_t pairing;
    element_t x,pk,g1,h,hm,sigma;
   
    if (pairing_init_set_buf(pairing,s,strlen(s))){
           pbc_die("Pairing initialization failed.");
        }
    if (!pairing_is_symmetric(pairing)){
           pbc_die("pairing must be symmetric");
        }
   
   
    element_init_Zr(params.x, pairing);
    element_init_Zr(params.hm, pairing);
    element_init_G1(params.g1, pairing);
    element_init_G1(params.pk, pairing);
    element_init_G1(params.h, pairing);
    element_init_G1(params.sigma, pairing);
   
    element_random(params.g1);
    //element_printf("\n\n%B\n\n\n", g1);


  return params;
}

std::vector<User> keygen(const Parameters & parms, std::vector<User> user_vec;){

    std::vector<int> polynomial;

    polynomial.resize(NUM_USERS);
    for(int i = 0; i < NUM_USERS; i++){
          polynomial[i] = i;
    }
 
      user_vec.resize(NUM_USERS);
      for(int i = 0; i < NUM_USERS; i++){
 
          user_vec[i].input = i;
          user_vec[i].id = i;
          user_vec[i].points.clear();
          element_init_G1(user_vec[i].ciphertext, params.pairing);
    
          //Assign each user set of points
          compute_points(polynomial, user_vec[i]);
 
      } 

  return user_vec;
}

std::vec<User> encode(const mpz_class & x, std::vec<User> user_vec, const Parameters & parms){

    for(int i = 0; i < NUM_USERS; i++){
        lagrange_coe(i, NUM_USERS, user_vec[i], params.N);
    }

    mpz_class time;
    element_from_hash(hm, (char *) "time", strlen("time"));
    element_to_mpz(time.get_mpz_t(), hm);
 
    element_t tmp_lagrange, placeholder;
    element_init_G1(tmp_lagrange, params.pairing);
    element_init_G1(placeholder, params.pairing);
    for(int i = 0; i < NUM_USERS; i++){
          assert(user_vec[i].points.size() < NUM_USERS);
          //pk = g * x
          element_mul_mpz(params.pk, params.g1, x);
 
          user_vec[i].lagrange *= user_vec[i].points[NUM_USERS-2-1];
 
          element_set_mpz(tmp_lagrange, user_vec[i].lagrange.get_mpz_t());
 
          element_mul_mpz(placeholder, params.pk, user_vec[i].lagrange.get_mpz_t());
          element_mul_mpz(placeholder, placeholder, time.get_mpz_t());
          element_add(user_vec[i].ciphertext, placeholder, tmp_lagrange);
      }

  return user_vec;
}

void aggregate(const std:vector<User> user_vec, const Parameters & parms){
	
    mpz_class ret;

    for(int i = 1; i < NUM_USERS; i++){
  
          element_add(user_vec[i].ciphertext, user_vec[i-1].ciphertext, user_vec[i].ciphertext); 
      }
  
    //DLS
    mpz_class ss, r;
    r = 1;
    ss = 0; 
    int result;
    mpz_class i;
    for(i = 0; i < prime; i++){
        element_mul_mpz(params.pk, params.g1, ss.get_mpz_t());
        result = element_cmp(params.pk, user_vec[NUM_USERS-1].ciphertext);
        if(result){
              ss += r;
          }
        else{
              cout << "Decryption Successful" << endl;
              break;
          }
      }
      if(result){
        cout << "DECRYPTION FAILED, " << (++i).get_mpz_t() << " iterations" << endl;
        return 1;
      }

}

#endif
