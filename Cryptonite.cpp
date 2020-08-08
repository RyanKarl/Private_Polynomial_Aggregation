#include <pbc.h>
#include <pbc_test.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utility.h>
#include <cmath.h>

#define NUM_USERS 5

std::vector<int> polynomial;
mpz_t prime;
mpz_set_str(prime, "730750818665451621361119245571504901405976559617");

struct User
{
    mpz_t id;
    mpz_t input;
    std::vector<mpz_t> points;
    mpf_t lagrange;
    mpz_t ciphertext;
};

void compute_points(std::vector<int> polynomial, User u){
    
    long tmp_id = mpz_get_si(u.id); 

    for(long i = 2; i < polynomial::size; i++){
        
        long tmp = 0;

        for(long j = 2; j <= i; j++){ 
            
            tmp = tmp + std::pow(tmp_id, j);

        }
        
        mpz_set_si(u.second, tmp);
        u.points.push_back(u.second);

    }
}

void lagrange_coe(int i, int n, User u){
    
    double ret = 1;

    mpf_t m_tmp_i, m_tmp_j;
    mpf_init(m_tmp_i);
    mpf_init(m_tmp_j);

    for(double j = 1; j <= n; j++){
        
        if(i != j){
            
            mpf_set_si(u.lagrange, ret);
            mpf_set_si(m_tmp_i, i);
            mpf_set_si(m_tmp_j, j);

            //ret = ret * (-j / (i - j))
            mpf_sub_ui(m_tmp_i, m_tmp_i, m_tmp_j);
            mpf_invert(m_tmp_i, m_tmp_i, prime);
            mpf_mul(m_tmp_j, m_tmp_j, m_tmp_i);
            mpf_mul(u.lagrange, m_tmp_j, m_tmp_j);
            mpf_mod(u.lagrange, u.lagrange, prime);
            
        }

        ret = mpf_get_si(u.lagrange);

    }

}
    

int main() {
 
    //r is prime

     char s[3000] = "type a\nq 8780710799663312522437781984754049815806883199414208211028653399266475630880222957078625179422662221423155858769582317459277713367317481324925129998224791\nh 12016012264891146079388821366740534204802954401251311822919615131047207289359704531102844802183906537786776\nr 730750818665451621361119245571504901405976559617\nexp2 159\nexp1 107\nsign1 1\nsign0 1\n";

     pairing_t pairing;
     element_t x,pk,g1,h,hm,sigma;

     if (pairing_init_set_buf(pairing,s,strlen(s))) pbc_die("Pairing initialization failed.");
     if (!pairing_is_symmetric(pairing)) pbc_die("pairing must be symmetric");


    element_init_Zr(x, pairing);
    element_init_Zr(hm, pairing);
    element_init_G1(g1, pairing);
    element_init_G1(pk, pairing);
    element_init_G1(h, pairing);
    element_init_G1(sigma, pairing);
    
    element_random(g1);
    //element_printf("\n\n%B\n\n\n", g1);

    std::vector<User> user_vec;
    
    for(int i = 1; i <= NUM_USERS; i++){
        polynomial.push_back.(i);
    }

    for(int i = 1; i <= NUM_USERS; i++){

        user_vec.push_back(User());
        
        mpz_init(user_vec[i-1].id);
        mpz_init(user_vec[i-1].input);
        mpz_init(user_vec[i-1].points[0]);
        mpf_init(user_vec[i-1].lagrange);

        mpz_set_si(user_vec[i].id, i);
        mpz_set_si(user_vec[i].input, i);
        
        //Assign each user set of points
        compute_points(polynomial, user_vec[i])
        
        lagrange_coe(i, NUM_USERS, user_vec[i]);

    }


    //Significance of this number?
    //element_set_str(x, "128112216885054085779135890052683951891862528941", 10);
    //element_printf("%B\n\n\n", x);
    
    mpz_t time;
    mpz_init(time);
    element_from_hash(hm, "time", strlen("time"));
    element_to_mpz(time, hm)

    for(int i = 1; i <= NUM_USERS; i++){

        //pk = g * x
        element_mul_mpz(pk, g1, user_vec[i-1].input);
        mpz_mul(user_vec[i-1].lagrange, user_vec[i-1].lagrange, user_vec[i-1].points[NUM_USERS-1]);
        element_mul_mpz(pk, pk, user_vec[i-1].lagrange);
        element_mul_mpz(pk, pk, user_vec[i-1].time);
        element_add(user_vec[i-1].ciphertext, pk, user_vec[i-1].lagrange);

    }

    for(int i = 1; i < NUM_USERS; i++){
    
        element_add(user_vec[i-1].ciphertext, user_vec[i].lagrange, user_vec[i-1].lagrange)

    }


    //DLS

    return 0;
}
