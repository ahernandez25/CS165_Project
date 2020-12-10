#ifndef rendezvous_H
#define rendezvous_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../murmur/murmur3.h"

void add_obj(char obj_name[]); //set object to concatenate
void concatenate(char prox_name[]); // concatenate S
void create_temp_1DArray(int m, int n, char arr[][n]); //turn char 2d int 1d char m ist the proxy 1-6 n is the word "proxy"
void All_Proxy_Concatenated(int size, int len_proxy, char prox_names[][len_proxy]); //set concatenate in vector String_to_hash
uint32_t Hash(char concatenate_O_P[]); // hash strings using mrmurhash
void All_String_Hash(); // vector of hashes and find MAX hash
char * return_prox(int i); //return proxy max as *char
char* Rendezvous(char obj_name[],int len_proxy, char prox_names[][len_proxy]); // combine and return proxy 

#endif

/*
Function  Rendezvous takes in obj_name, len_proxy, prox_names as a 2d array

-set proxies names to 6 -> proxy1 =len_proxy
-prox_names[x][y]
   0 1 2 3 4 5 y
0  p r o x y 1   
1  p r o x y 2 
2  p r o x y 3 
3  p r o x y 4 
4  p r o x y 5 
5  p r o x y 6 
x

- will return a pointer char a = { 'p' 'r' 'o' 'x' 'y' '6' } so need to look through it

example:

char arr[][6] = {{"proxy1"}, {"proxy2"}, {"proxy3"},{"proxy4"}, {"proxy5"}, {"proxy6"}};
char obj[2]="ar";
char *n=Rendezvous("ar" or obj ,6, arr); // or both work 
int i;
printf("\n");
for(i = 0; i < 6; i++)
{
    printf("%c ", n[i]);
}

*/
