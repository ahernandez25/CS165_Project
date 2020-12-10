
#ifndef BLOOM_FILTER_H_   /* Include guard */
#define BLOOM_FILTER_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "murmur3.h"

void set_num_hashf (int k1);	//will set num hash functions
int Hash(char obj_name[60], uint32_t seed); // hash strings using mrmurhash
void hash_k (char obj_name[60]);	// creates k num of has functions
void fill_Bloom(); //fill in bloom array
void insert (char obj_name[60]); //insert hash to bloom
int query (char obj_name[60]); // check if hash is in bloom 1=true 0 =false

// to use 
/*
Note:

- objects in char array[]   
        ex:char a[2]="ar";
- query return int 1=true 0 =false.

Steps:
1) set number of hash wanted h1...hk
        set_num_hashf(2); //we create 2 diferent hash for the same object 
2)insert(char obj[]) takes one object and creates hashes and fills bloom filter

3)query(char obj[]) will return 0 or 1 

example:
  set_num_hashf(2); 
  char a[2]="ar";
  insert(a);
  int s[2];
  int i;
  s[0]=query("ar"); // return 1
  s[1]=query("a");  //return 0

*/

#endif