//gcc prototype.c -o prototype -lgmp
//./prototype <users> <faults>
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <gmp.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#define SIZE 1000
#define NUM_POINT_ADDITIONS 100
#define NUM_HASHES 100
#define USERS 100
#define FAULTS 10
#define HASH 1


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









struct DataItem {
   struct Point data;   
   int key; //user_id
   int secret;
   int fault;

};

struct DataItem* hashArray[SIZE];
struct DataItem* dummyItem;
struct DataItem* item;

//struct Point* pointArray[SIZE];
struct Point* dummyPoint;
struct Point* point;

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
	//	printf("Val = %d \n", *(ptr + i));
        //}

	return ptr;

}

const char* getfield(char* line, int num)
{
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

int main(int argc, char** argv)
{	

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
        //mpz_set_str(m, "0x00542d46e7b3daac8aeb81e533873aabd6d74bb710", 0);

	int users = (int) argv[1];
	int faults = (int) argv[2];


	//for (int i = 0; i < array_size; i++) {
    	//	mpz_init2(num_arr[i], 1024);
	//}

/*
	for(int j = 0; j <= USERS; j++){
		
		struct Point *point = (struct Point*) malloc(sizeof(struct Point));
   		item->data = data;
   		item->key = key;
   		item->secret = secret;
	
	}
*/
	FILE* stream = fopen("WELL-00001_20170317140005.csv", "r");

	rand_list = getRandomList(users);

	int   plaintext = 0, hash = HASH, counter = 0;
	char line[1024], int_to_hex[users];

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


	for(int i = 0; i < users; i++){
	
		search(i, m);
	}

	int hashIndex;

	for(int i = 0; i < users; i++){

		hashIndex = hashCode(i);

		Point_Addition(hashArray[hashIndex]->data, Aggregate, &Aggregate);
	}



/*	FILE* stream = fopen("WELL-00001_20170317140005.csv", "r");

    	char line[1024];
    	while (fgets(line, 1024, stream))
    	{
        	char* tmp = strdup(line);
        	//printf("Field 3 would be %d\n", (int) getfield(tmp, 3) % 6 );
        	free(tmp);
    	}
*/


	//for (int i = 0; i < argc; ++i)
        //cout << argv[i] << "\n";

	//int *test_func = getRandomList(5);

	//for(int i = 0; i < 5; ++i)
        //{
        //     printf("Val = %d \n", *(test_func + i));
        //}









//Old code for EC	
/*
	clock_t begin = clock();

        mpz_t  m, Px, Py, Qx, Qy, Rx, Ry, ECp, ECa, ECb;

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


	clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Point Addition: %f seconds\n", time_spent);
*/



/*	begin = clock();

	for(int i=0; i<= NUM_HASHES; i++){

                unsigned char* str_hash = HASH_STR;

                unsigned long hash = 5381;
                int x;

                while (x = *str_hash++)
                hash = ((hash << 5) + hash) + x;

        }

	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        printf("Hash: %f seconds\n", time_spent);
*/
/*	begin = clock();


        dummyItem = (struct DataItem*) malloc(sizeof(struct DataItem));
        dummyItem->data = -1;
        dummyItem->key = -1;

        for(int i=0; i<USERS; i++){

                struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
                item->data = i;  
                item->key = i;

                //get the hash 
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

                //get the hash 
                int hashIndex = j % SIZE;

                //move in array until an empty 
                while(hashArray[hashIndex] != NULL) {

                        if(hashArray[hashIndex]->key == j){
                                break;
                        }
                                //go to next cell
                                      ++hashIndex;

                                //wrap around the table
                                      hashIndex %= SIZE;

                }
        }

	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Table: %f seconds\n", time_spent);
*/
	return 0;

}
