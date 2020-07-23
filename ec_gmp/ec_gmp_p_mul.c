// Point at Infinity is Denoted by (0,0)
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

//This represents a, b, p for the curve y^2 = x^3 + ax + b over F_p
struct Elliptic_Curve {
	mpz_t a;
	mpz_t b;
	mpz_t p;
};

//This represents x, y for the curve y^2 = x^3 + ax + b over F_p
struct Point {
	mpz_t x;
	mpz_t y;
};

//Global curve
struct Elliptic_Curve EC;

//This computes C = 2A, where P = A and R = C i.e.
//x_C = s^2 – (2 * x_A) (mod p)
//y_C = –y_A + s * (x_A – x_C) (mod p)
//s = (3 * x_A^2 + a) / (2 * y_A) (mod p) where a is elliptic curve parameter
void Point_Doubling(struct Point P, struct Point *R)
{
	mpz_t slope, temp;
	mpz_init(temp);
	mpz_init(slope);
	
	if(mpz_cmp_ui(P.y, 0) != 0) {

		//s = (3 * x_A^2 + a) / (2 * y_A) (mod p)
        mpz_mul_ui(temp, P.y, 2);
		mpz_invert(temp, temp, EC.p);
		mpz_mul(slope, P.x, P.x);
		mpz_mul_ui(slope, slope, 3);
		mpz_add(slope, slope, EC.a);
		mpz_mul(slope, slope, temp);
		mpz_mod(slope, slope, EC.p);
		
        //x_C = s^2 – (2 * x_A) (mod p)
        mpz_mul(R->x, slope, slope);
		mpz_sub(R->x, R->x, P.x);
		mpz_sub(R->x, R->x, P.x);
		mpz_mod(R->x, R->x, EC.p);

        //y_C = –y_A + s * (x_A – x_C) (mod p)
		mpz_sub(temp, P.x, R->x);
		mpz_mul(R->y, slope, temp);
		mpz_sub(R->y, R->y, P.y);
		mpz_mod(R->y, R->y, EC.p);
	} 
    else { //if y_A = 0 then 2A = O, where O is the point at infinity
		mpz_set_ui(R->x, 0);
		mpz_set_ui(R->y, 0);
	}
	mpz_clear(temp);
	mpz_clear(slope);
}

//This computes C = A + B, where P = A, Q = B, and R = C i.e.
//x_C = s^2 – x_A – x_B (mod p)
//y_C = –y_A + s * (x_A – x_C) (mod p)
//s = (y_A – y_B) / (x_A – x_B) (mod p)
void Point_Addition(struct Point P, struct Point Q, struct Point *R)
{
	mpz_mod(P.x, P.x, EC.p);
	mpz_mod(P.y, P.y, EC.p);
	mpz_mod(Q.x, Q.x, EC.p);
	mpz_mod(Q.y, Q.y, EC.p);

    //Quick return if a point is (0,0)
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

    //temp = p - y_B (mod p)
	if(mpz_cmp_ui(Q.y, 0) != 0) { 
		mpz_sub(temp, EC.p, Q.y);
		mpz_mod(temp, temp, EC.p);
	} else
		mpz_set_ui(temp, 0);

	//gmp_printf("\n temp=%Zd\n", temp);

    //if B = –A i.e. B = (x_A, –y_A) (mod p) then A + B = O where O is the point at infinity (can't divide by 0)
	if(mpz_cmp(P.y, temp) == 0 && mpz_cmp(P.x, Q.x) == 0) {
		mpz_set_ui(R->x, 0);
		mpz_set_ui(R->y, 0);
		mpz_clear(temp);
		return;
	}
	
    //if B = A i.e. A + B = 2A then point doubling equations are used
	if(mpz_cmp(P.x, Q.x) == 0 && mpz_cmp(P.y, Q.y) == 0)	{
		Point_Doubling(P, R);
		
		mpz_clear(temp);
		return;		
	} 
    else {
		mpz_t slope;
		mpz_init_set_ui(slope, 0);

        //s = (y_A – y_B) / (x_A – x_B) (mod p)
		//Note mpz_invert(x,a,m) returns an integer x where product of ax = 1 mod m
        mpz_sub(temp, P.x, Q.x);
		mpz_mod(temp, temp, EC.p);
		mpz_invert(temp, temp, EC.p);
		mpz_sub(slope, P.y, Q.y);
		mpz_mul(slope, slope, temp);
		mpz_mod(slope, slope, EC.p);
		
        //x_C = s^2 – x_A – x_B (mod p) 
        mpz_mul(R->x, slope, slope);
		mpz_sub(R->x, R->x, P.x);
		mpz_sub(R->x, R->x, Q.x);
		mpz_mod(R->x, R->x, EC.p);
		
        //y_C = –y_A + s * (x_A – x_C) (mod p)
        mpz_sub(temp, P.x, R->x);
		mpz_mul(R->y, slope, temp);
		mpz_sub(R->y, R->y, P.y);
		mpz_mod(R->y, R->y, EC.p);
		
		mpz_clear(temp);
		mpz_clear(slope);
		return;
	}
}

//Implemented as adding P to itself m times and storing result in R
//Uses "Double and Add method"
void Scalar_Multiplication(struct Point P, struct Point *R, mpz_t m)
{
	struct Point Q, T;
	mpz_init(Q.x); 
    mpz_init(Q.y);
	mpz_init(T.x); 
    mpz_init(T.y);
	long no_of_bits, loop;
	
    //Return the size of op (i.e. m) measured in number of digits in the base (i.e. 2)
	no_of_bits = mpz_sizeinbase(m, 2);
	mpz_set_ui(R->x, 0);
	mpz_set_ui(R->y, 0);
	if(mpz_cmp_ui(m, 0) == 0)
		return;
	
    //Assign Q = P
	mpz_set(Q.x, P.x);
	mpz_set(Q.y, P.y);

    //Test bit at location (i.e. 0) in m and return 0 or 1 accordingly.
    //Assign R = P if bit 0 in m equals 1
    //gmp_printf("\nThis is m: %Zd \n", m);
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

int main(int argc, char *argv[]){

	mpz_init(EC.a); 
	mpz_init(EC.b); 
	mpz_init(EC.p);
	
	struct Point P, k_pubA, k_pubB, joint_key_Alice, joint_key_Bob;
	mpz_init_set_ui(k_pubA.x, 0);
	mpz_init_set_ui(k_pubA.y, 0);
    mpz_init_set_ui(k_pubB.x, 0);
    mpz_init_set_ui(k_pubB.y, 0);
    
    mpz_init_set_ui(joint_key_Alice.x, 0);
    mpz_init_set_ui(joint_key_Alice.y, 0);
    mpz_init_set_ui(joint_key_Bob.x, 0);
    mpz_init_set_ui(joint_key_Bob.y, 0);
	
    mpz_init(P.x);
	mpz_init(P.y);
	
	mpz_t private_key_Alice;
	mpz_init(private_key_Alice);
    mpz_set_str(private_key_Alice, "0x4", 0);
    
    mpz_t private_key_Bob;
    mpz_init(private_key_Bob);
    mpz_set_str(private_key_Bob, "0x7", 0);

	mpz_set_str(EC.p, "0x11", 0);
	mpz_set_str(EC.a, "0x2", 0);
	mpz_set_str(EC.b, "0x2", 0);
	mpz_set_str(P.x, "0x5", 0);
	mpz_set_str(P.y, "0x1", 0);
		
	//Elliptic Curve Diffie–Hellman Key Exchange (ECDH)
    //Public parameters: {p = 17, a = 2, b = 2, P = (5,1)}
    //Alice's private key = 4
    //Bob's private key = 7
    
    //Alice computes k_pubA = private_key_Alice * P
    Scalar_Multiplication(P, &k_pubA, private_key_Alice);
    
    //Bob computes k_pubB = private_key_Bob * P
    Scalar_Multiplication(P, &k_pubB, private_key_Bob);

    //Alice and Bob exchange k_pubA and k_pubB
    //Alice computes joint secret key
    Scalar_Multiplication(k_pubB, &joint_key_Alice, private_key_Alice);

    //Bob computes joint secret key
    Scalar_Multiplication(k_pubA, &joint_key_Bob, private_key_Bob);
    
    //Verify joint keys are equal
    //if(mpz_cmp(joint_key_Alice.x, joint_key_Bob.x) == 0 && mpz_cmp(joint_key_Alice.y, joint_key_Bob.y) == 0){
        //printf("Keys are equal\n");
    //}
   
        

    //Encryption Example
    //Consider a message m = 9 sent from Alice to Bob. 
    //Alice chooses a random positive integer k = 5, a private key private_key_Alice = 4, 
    //and generates the public key k_pubA = private_key_Alice * P and produces 
    //the ciphertext ‘enc_m’ consisting of pair of points enc_m={k * P, m + k * k_pubB} 
    //where P = (5, 1) is the base point on the curve.
    mpz_t m;
    mpz_init(m);
    mpz_set_str(m, "0x9", 0);
    mpz_t k;
    mpz_init(k);
    mpz_set_str(m, "0x5", 0);
    struct Point enc_m_1, enc_m_2;
    mpz_init(enc_m_1.x);
    mpz_init(enc_m_1.y);
    mpz_init(enc_m_2.x);
    mpz_init(enc_m_2.y);

    //R=
    Scalar_Multiplication(P, &enc_m_1, k);
    struct Point tmp;
    mpz_init(tmp.x);
    mpz_init(tmp.y);

    //M=
    Scalar_Multiplication(P, &tmp, m);
    
    //k*Y
    Scalar_Multiplication(k_pubB, &enc_m_2, k);
    //S=
    Point_Addition(tmp, enc_m_2, &enc_m_2);

    

    //Decryption Example
    //To decrypt the ciphertext, Bob multiplies the 1st point in the pair enc_m by private_key_Bob and subtracts the result from the 2nd point as k * k_pubB - private_key_Bob * (k * P)  = m 
    mpz_neg(private_key_Bob, private_key_Bob);
    Scalar_Multiplication(enc_m_2, &tmp, private_key_Bob);
    //M=
    Point_Addition(enc_m_1, tmp, &tmp);
  
    int i = 0, j = 0;
    while(i == 0){
        if((mpz_cmp(tmp.x, P.x) == 0) && (mpz_cmp(tmp.y, P.y) == 0)){
            i = 1;
        }
        else{
            j++;
            Point_Addition(P, P, &P);
        }
    }

    printf("Final Result %i\n", j);
    

    /*	p = k x G == R = m x P	*/
	//Implemented as adding P to itself m times and storing result in R
    //Scalar_Multiplication(P, &R, m);
	
	//mpz_out_str(stdout, 16, R.x); puts("");
	//mpz_out_str(stdout, 16, R.y); puts("");

	// Free variables
	//mpz_clear(EC.a); mpz_clear(EC.b); mpz_clear(EC.p);
	//mpz_clear(R.x); mpz_clear(R.y);
	//mpz_clear(P.x); mpz_clear(P.y);
	//mpz_clear(m);
}
