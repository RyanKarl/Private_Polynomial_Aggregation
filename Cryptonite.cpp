// g++ Cryptonite.cpp -o driver -lpbc -lgmp -lgmpxx -I/usr/local/include/pbc
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <cassert>
#include <string>

#include "gmp.h"
#include "gmpxx.h"

#include "pbc.h"
#include "pbc_test.h"

using std::cout;
using std::endl;

#define NUM_USERS 5
#define PRIME_REPS 20

struct User
{
    int id;
    mpz_class input;
    std::vector<mpz_class> points;
    mpz_class lagrange;
    element_t ciphertext;
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

    

int main() {

    assert(NUM_USERS >= 3);
 
    mpz_class prime;
    std::vector<int> polynomial;
    //r is prime
    prime = "730750818665451621361119245571504901405976559617";
    assert(mpz_probab_prime_p(prime.get_mpz_t(), PRIME_REPS));

     char s[3000] = "type a\nq 8780710799663312522437781984754049815806883199414208211028653399266475630880222957078625179422662221423155858769582317459277713367317481324925129998224791\nh 12016012264891146079388821366740534204802954401251311822919615131047207289359704531102844802183906537786776\nr 730750818665451621361119245571504901405976559617\nexp2 159\nexp1 107\nsign1 1\nsign0 1\n";

     pairing_t pairing;
     element_t x,pk,g1,h,hm,sigma;

     if (pairing_init_set_buf(pairing,s,strlen(s))){
        pbc_die("Pairing initialization failed.");
     } 
     if (!pairing_is_symmetric(pairing)){
        pbc_die("pairing must be symmetric");
     } 


    element_init_Zr(x, pairing);
    element_init_Zr(hm, pairing);
    element_init_G1(g1, pairing);
    element_init_G1(pk, pairing);
    element_init_G1(h, pairing);
    element_init_G1(sigma, pairing);
    
    element_random(g1);
    //element_printf("\n\n%B\n\n\n", g1);

    std::vector<User> user_vec;
    
    polynomial.resize(NUM_USERS);
    for(int i = 0; i < NUM_USERS; i++){
        polynomial[i] = i;
    }

    user_vec.resize(NUM_USERS);
    for(int i = 0; i < NUM_USERS; i++){
    
        user_vec[i].input = i;
        user_vec[i].id = i;
        user_vec[i].points.clear();
        element_init_G1(user_vec[i].ciphertext, pairing);
        
        //Assign each user set of points
        compute_points(polynomial, user_vec[i]);
        
        lagrange_coe(i, NUM_USERS, user_vec[i], prime);

    }


    //Significance of this number?
    //element_set_str(x, "128112216885054085779135890052683951891862528941", 10);
    //element_printf("%B\n\n\n", x);
    
    mpz_class time;
    element_from_hash(hm, (char *) "time", strlen("time"));
    element_to_mpz(time.get_mpz_t(), hm);
    
    element_t tmp_lagrange;
    element_init_G1(tmp_lagrange, pairing);
    for(int i = 0; i < NUM_USERS; i++){
        assert(user_vec[i].points.size() < NUM_USERS);
        //pk = g * x
        element_mul_mpz(pk, g1, user_vec[i].input.get_mpz_t());
        
        user_vec[i].lagrange *= user_vec[i].points[NUM_USERS-2-1];
        
        element_set_mpz(tmp_lagrange, user_vec[i].lagrange.get_mpz_t());
        
        element_mul_mpz(pk, pk, user_vec[i].lagrange.get_mpz_t());
        element_mul_mpz(pk, pk, time.get_mpz_t());
        element_add(user_vec[i].ciphertext, pk, tmp_lagrange);
    }

    for(int i = 1; i < NUM_USERS; i++){
    
        element_add(user_vec[i].ciphertext, tmp_lagrange, tmp_lagrange);

    }


    //DLS
    mpz_class ss, r;
    r = 1;
    ss = 0; //Do we need this?
    int result;
    mpz_class i;
    for(i = 0; i < prime; i++){
      element_mul_mpz(pk, g1, ss.get_mpz_t());
        result = element_cmp(pk, user_vec[NUM_USERS-1].ciphertext);
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

    return 0;
}
