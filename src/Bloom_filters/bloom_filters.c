#include "bloom_filters.h"

typedef struct  {
    int m;			//bit cell
    int k;			// hash function
    uint32_t seed;		//changes to create new functions
    int hold_hash[60];	//h1...hk
    int fill_array[60];	//ints to fill  
    int Bloom_fil[60];	//  
} bf_struct;

//gobal
bf_struct one = { 60, 0, 0, {0}, {0}, {0} };

// functions
void set_num_hashf (int k1)	//will set num hash functions
{
   // printf("This is set %i \n", k1);
    one.m = 60;
    one.k = k1;
    one.seed = 12;
    //printf("This is set k %i \n", one.k);
    //0-100 fill with 0
    int i;
    for(i=0;i<60;i++ )
    {
        one.Bloom_fil[i]=0; //fill bloom to 0
    }
}

int Hash(char obj_name[20], uint32_t seed) // hash strings using mrmurhash
{
    int return_hash=0;
    char *temp = obj_name;

    uint32_t hash[1];
    char string_hash[11]; 
    //hash using Murmur
    const char *key = temp;
    
    MurmurHash3_x86_32(key, (uint32_t) strlen(key), seed, hash); //cout<< "hash" <<hash[0] <<" = ";
    
    // transform uint32_t to char
    snprintf(string_hash, sizeof string_hash, "%lu", (unsigned long)hash[0]); //cout<< "hash in string"<<string_hash <<endl ;
    
    //convert char to int inverse of first two 
    int tens = string_hash[1] - '0';
    int ones = string_hash[0] - '0';
    return_hash= tens*10+ones;
    if(return_hash>59)
    {
      return_hash=return_hash-40;
    }
    //printf("This is hash %i \n", return_hash);
    return return_hash;
}

void hash_k (char obj_name[20])	// creates k num of has functions
{
    //int temp[20];	//hold to return 
    uint32_t seed_hold = one.seed;	//gives different hashs depending on seed val
    //printf("This is hk\n");
    //hash same obj using different seeds
    int i;
    //printf("This is seed %i \n", seed_hold);
    for ( i = 0; i < one.k; i++)	//gives h1(obj_name), ... ,hk(obj_name)stores in vec
    {
        one.hold_hash[i]=Hash(obj_name,seed_hold);
        seed_hold++;
        //printf("This is hash_hold %i \n", one.hold_hash[i]);
        //cout<<"temp  "<<temp[i]<<endl;
    }
    
}

void fill_Bloom()
{
  int i;
  for (i = 0; i < one.k; i++)
    {
      one.Bloom_fil[one.hold_hash[i]] = 1;
    }
  
    for (i = 0; i < one.m; i++)	//clear
    {
      one.hold_hash[i]=0;
    }
}

void insert (char obj_name[20]) //insert hash to bloom
{
  hash_k(obj_name);
  fill_Bloom();
}

int query (char obj_name[20]) // check if hash is in bloom 1=true 0 =false
{
 hash_k (obj_name);
int i;
  for (i = 0; i < one.k; i++)
    {
        if (one.Bloom_fil[one.hold_hash[i]] == 0)
          {
              //cout<<"f"<<endl;
            return 0;
          }

    }
  for (i = 0; i < one.m; i++)	//clear
    {
      one.hold_hash[i]=0;
    }
  //cout<<"t"<<endl;
  return 1;
}
