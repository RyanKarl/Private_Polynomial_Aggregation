//gcc hash.c -o hash -lgmp
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <gmp.h>

#define SIZE 1000
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

struct DataItem* hashArray[SIZE];
struct DataItem* dummyItem;
struct DataItem* item;

int main()
{

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

	begin = clock();

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

	begin = clock();


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

	return 0;

}
