#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>





#ifdef __GNUC__
#define FORCE_INLINE __attribute__((always_inline)) inline
#else
#define FORCE_INLINE inline
#endif



//-----------------------------------------------------------------------------
//// Platform-specific functions and macros

static FORCE_INLINE uint32_t
rotl32 (uint32_t x, int8_t r)
{
  return (x << r) | (x >> (32 - r));
}

static FORCE_INLINE uint64_t
rotl64 (uint64_t x, int8_t r)
{
  return (x << r) | (x >> (64 - r));
}

#define	ROTL32(x,y)	rotl32(x,y)
#define ROTL64(x,y)	rotl64(x,y)

#define BIG_CONSTANT(x) (x##LLU)

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

#define getblock(p, i) (p[i])

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

static FORCE_INLINE uint32_t
fmix32 (uint32_t h)
{
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}

//----------

static FORCE_INLINE uint64_t
fmix64 (uint64_t k)
{
  k ^= k >> 33;
  k *= BIG_CONSTANT (0xff51afd7ed558ccd);
  k ^= k >> 33;
  k *= BIG_CONSTANT (0xc4ceb9fe1a85ec53);
  k ^= k >> 33;

  return k;
}

//-----------------------------------------------------------------------------

void
MurmurHash3_x86_32 (const void *key, int len, uint32_t seed, void *out)
{
  const uint8_t *data = (const uint8_t *) key;
  const int nblocks = len / 4;
  int i;

  uint32_t h1 = seed;

  uint32_t c1 = 0xcc9e2d51;
  uint32_t c2 = 0x1b873593;

  //----------
  // body


  const uint32_t *blocks = (const uint32_t *) (data + nblocks * 4);

  for (i = -nblocks; i; i++)
    {
      uint32_t k1 = getblock (blocks, i);

      k1 *= c1;
      k1 = ROTL32 (k1, 15);
      k1 *= c2;

      h1 ^= k1;
      h1 = ROTL32 (h1, 13);
      h1 = h1 * 5 + 0xe6546b64;
    }

  //----------
  //  tail


  const uint8_t *tail = (const uint8_t *) (data + nblocks * 4);

  uint32_t k1 = 0;

  switch (len & 3)
    {
    case 3:
      k1 ^= tail[2] << 16;
    case 2:
      k1 ^= tail[1] << 8;
    case 1:
      k1 ^= tail[0];
      k1 *= c1;
      k1 = ROTL32 (k1, 15);
      k1 *= c2;
      h1 ^= k1;
    };

  //----------
  // finalization


  h1 ^= len;

  h1 = fmix32 (h1);

  *(uint32_t *) out = h1;
}

//-----------------------------------------------------------------------------



void
MurmurHash3_x86_128 (const void *key, const int len, uint32_t seed, void *out)
{
  const uint8_t *data = (const uint8_t *) key;
  const int nblocks = len / 16;
  int i;

  uint32_t h1 = seed;
  uint32_t h2 = seed;
  uint32_t h3 = seed;
  uint32_t h4 = seed;

  uint32_t c1 = 0x239b961b;
  uint32_t c2 = 0xab0e9789;
  uint32_t c3 = 0x38b34ae5;
  uint32_t c4 = 0xa1e38b93;

  //----------
  //  // body


  const uint32_t *blocks = (const uint32_t *) (data + nblocks * 16);

  for (i = -nblocks; i; i++)
    {
      uint32_t k1 = getblock (blocks, i * 4 + 0);
      uint32_t k2 = getblock (blocks, i * 4 + 1);
      uint32_t k3 = getblock (blocks, i * 4 + 2);
      uint32_t k4 = getblock (blocks, i * 4 + 3);

      k1 *= c1;
      k1 = ROTL32 (k1, 15);
      k1 *= c2;
      h1 ^= k1;

      h1 = ROTL32 (h1, 19);
      h1 += h2;
      h1 = h1 * 5 + 0x561ccd1b;

      k2 *= c2;
      k2 = ROTL32 (k2, 16);
      k2 *= c3;
      h2 ^= k2;

      h2 = ROTL32 (h2, 17);
      h2 += h3;
      h2 = h2 * 5 + 0x0bcaa747;

      k3 *= c3;
      k3 = ROTL32 (k3, 17);
      k3 *= c4;
      h3 ^= k3;

      h3 = ROTL32 (h3, 15);
      h3 += h4;
      h3 = h3 * 5 + 0x96cd1c35;

      k4 *= c4;
      k4 = ROTL32 (k4, 18);
      k4 *= c1;
      h4 ^= k4;

      h4 = ROTL32 (h4, 13);
      h4 += h1;
      h4 = h4 * 5 + 0x32ac3b17;
    }

  //----------
  //  tail

  const uint8_t *tail = (const uint8_t *) (data + nblocks * 16);

  uint32_t k1 = 0;
  uint32_t k2 = 0;
  uint32_t k3 = 0;
  uint32_t k4 = 0;

  switch (len & 15)
    {
    case 15:
      k4 ^= tail[14] << 16;
    case 14:
      k4 ^= tail[13] << 8;
    case 13:
      k4 ^= tail[12] << 0;
      k4 *= c4;
      k4 = ROTL32 (k4, 18);
      k4 *= c1;
      h4 ^= k4;

    case 12:
      k3 ^= tail[11] << 24;
    case 11:
      k3 ^= tail[10] << 16;
    case 10:
      k3 ^= tail[9] << 8;
    case 9:
      k3 ^= tail[8] << 0;
      k3 *= c3;
      k3 = ROTL32 (k3, 17);
      k3 *= c4;
      h3 ^= k3;

    case 8:
      k2 ^= tail[7] << 24;
    case 7:
      k2 ^= tail[6] << 16;
    case 6:
      k2 ^= tail[5] << 8;
    case 5:
      k2 ^= tail[4] << 0;
      k2 *= c2;
      k2 = ROTL32 (k2, 16);
      k2 *= c3;
      h2 ^= k2;

    case 4:
      k1 ^= tail[3] << 24;
    case 3:
      k1 ^= tail[2] << 16;
    case 2:
      k1 ^= tail[1] << 8;
    case 1:
      k1 ^= tail[0] << 0;
      k1 *= c1;
      k1 = ROTL32 (k1, 15);
      k1 *= c2;
      h1 ^= k1;
    };

  //----------
  //  // finalization

  h1 ^= len;
  h2 ^= len;
  h3 ^= len;
  h4 ^= len;

  h1 += h2;
  h1 += h3;
  h1 += h4;
  h2 += h1;
  h3 += h1;
  h4 += h1;

  h1 = fmix32 (h1);
  h2 = fmix32 (h2);
  h3 = fmix32 (h3);
  h4 = fmix32 (h4);

  h1 += h2;
  h1 += h3;
  h1 += h4;
  h2 += h1;
  h3 += h1;
  h4 += h1;

  ((uint32_t *) out)[0] = h1;
  ((uint32_t *) out)[1] = h2;
  ((uint32_t *) out)[2] = h3;
  ((uint32_t *) out)[3] = h4;
}

//-----------------------------------------------------------------------------

void
MurmurHash3_x64_128 (const void *key, const int len,
		     const uint32_t seed, void *out)
{
  const uint8_t *data = (const uint8_t *) key;
  const int nblocks = len / 16;
  int i;

  uint64_t h1 = seed;
  uint64_t h2 = seed;

  uint64_t c1 = BIG_CONSTANT (0x87c37b91114253d5);
  uint64_t c2 = BIG_CONSTANT (0x4cf5ad432745937f);

  //----------
  //  // body


  const uint64_t *blocks = (const uint64_t *) (data);

  for (i = 0; i < nblocks; i++)
    {
      uint64_t k1 = getblock (blocks, i * 2 + 0);
      uint64_t k2 = getblock (blocks, i * 2 + 1);

      k1 *= c1;
      k1 = ROTL64 (k1, 31);
      k1 *= c2;
      h1 ^= k1;

      h1 = ROTL64 (h1, 27);
      h1 += h2;
      h1 = h1 * 5 + 0x52dce729;

      k2 *= c2;
      k2 = ROTL64 (k2, 33);
      k2 *= c1;
      h2 ^= k2;

      h2 = ROTL64 (h2, 31);
      h2 += h1;
      h2 = h2 * 5 + 0x38495ab5;
    }

  //----------
  //  // tail

  const uint8_t *tail = (const uint8_t *) (data + nblocks * 16);

  uint64_t k1 = 0;
  uint64_t k2 = 0;

  switch (len & 15)
    {
    case 15:
      k2 ^= (uint64_t) (tail[14]) << 48;
    case 14:
      k2 ^= (uint64_t) (tail[13]) << 40;
    case 13:
      k2 ^= (uint64_t) (tail[12]) << 32;
    case 12:
      k2 ^= (uint64_t) (tail[11]) << 24;
    case 11:
      k2 ^= (uint64_t) (tail[10]) << 16;
    case 10:
      k2 ^= (uint64_t) (tail[9]) << 8;
    case 9:
      k2 ^= (uint64_t) (tail[8]) << 0;
      k2 *= c2;
      k2 = ROTL64 (k2, 33);
      k2 *= c1;
      h2 ^= k2;

    case 8:
      k1 ^= (uint64_t) (tail[7]) << 56;
    case 7:
      k1 ^= (uint64_t) (tail[6]) << 48;
    case 6:
      k1 ^= (uint64_t) (tail[5]) << 40;
    case 5:
      k1 ^= (uint64_t) (tail[4]) << 32;
    case 4:
      k1 ^= (uint64_t) (tail[3]) << 24;
    case 3:
      k1 ^= (uint64_t) (tail[2]) << 16;
    case 2:
      k1 ^= (uint64_t) (tail[1]) << 8;
    case 1:
      k1 ^= (uint64_t) (tail[0]) << 0;
      k1 *= c1;
      k1 = ROTL64 (k1, 31);
      k1 *= c2;
      h1 ^= k1;
    };

  //----------
  //  // finalization

  h1 ^= len;
  h2 ^= len;

  h1 += h2;
  h2 += h1;

  h1 = fmix64 (h1);
  h2 = fmix64 (h2);

  h1 += h2;
  h2 += h1;

  ((uint64_t *) out)[0] = h1;
  ((uint64_t *) out)[1] = h2;
}


// gobal var




#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


//#include "murmur3.h"


typedef struct  {
    int object_leng; //Leng of object string
    int num_proxy;  // number of proxy
    int num_bit_conc; //char [][num_bit_conc] how long is the string obj+proxy
    int proxint; //holds index of mav
    
    uint32_t All_Hash[7];    // Holds strings that are hashed
    uint32_t max_hash;              // holds max hash
    
    char concatenate_temp[20]; //holds concatenate object_proxy
    char char_temp[20];        // hold other strings
    char obj_n[20];    // object name
	char String_to_hash[6][20]; // holds strings to be hashed in 2d array

} r_struct;


//gobal var
r_struct one = { 0,0,0,0,{0},0, {"0"}, {"0"}, {"0"}, {"0"} };
char *proxy1="proxy1";
char *proxy2="proxy2";
char *proxy3="proxy3";
char *proxy4="proxy4";
char *proxy5="proxy5";
char *proxy6="proxy6";



void add_obj(char obj_name[]) //set object to concatenate
{
    one.num_proxy=6; //6 proxy
    one.object_leng=strlen(obj_name); //leng of object
    strncpy(one.obj_n, obj_name, one.object_leng);//one.obj_n=obj_name;
}
    
void concatenate(char prox_name[]) // concatenate S
{
    int siz=strlen(prox_name)+one.object_leng; // obj and proxy size
    one.num_bit_conc=siz;
    char temp[siz] ;
    int i;
    for(i=0;i<siz;i++)
    {
        if(i<one.object_leng) //set object
        {
            temp[i]=one.obj_n[i];
        }
        else //set proxy
        {
            temp[i]=prox_name[i-one.object_leng];
        }
    }
    
	
	strncpy(one.concatenate_temp, temp, one.num_bit_conc);//one.concatenate_temp=temp; hold obj+prox
	
}
	

void create_temp_1DArray(int m, int n, char arr[][n]) //turn char 2d int 1d char m ist the proxy 1-6 n is the word "proxy"
{ 
    int j; 
    //for (i = 0; i < m; i++) 
    for (j = 0; j < n; j++) 
    {
        one.char_temp[j]=arr[m][j];
        //printf("%d ", arr[m][j]); 
    }
}
	
	void All_Proxy_Concatenated(int size, int len_proxy, char prox_names[][len_proxy]) //set concatenate in vector String_to_hash
	{		
	    //[number of prox][lenth of each proxy]
        int size_array=size;
        int g=0;
        int i;
        int j;
        
        char temp[20];
        //printf(" size %i \n",size);
        for( i=0; i< size_array; i++)
        {
            // for( j=0; j< len_proxy; j++)
            // {
            //     temp[j]=prox_names[i][j];
            // }
            create_temp_1DArray(i, len_proxy, prox_names);
            //printf(" string to hash \n");
            strncpy(temp, one.char_temp, len_proxy);//temp=one.char_temp;
            if(strlen(temp) > 0)// check if sting exist
            {
                concatenate(temp);
                for(g =0; g< one.num_bit_conc; g++)
                {
                    one.String_to_hash[i][g]=one.concatenate_temp[g];
                    //printf(" %c ", one.String_to_hash[i][g]);
                }
                //printf("\n");
            }
        }
        
	}
	
	uint32_t Hash(char concatenate_O_P[]) // hash strings using mrmurhash
	{
        uint32_t seed = 23;
        uint32_t hash[1];
        char *temp = concatenate_O_P;
        
        const char *key = temp;
        // MurmurHash3_x86_32(argv[1], strlen(argv[1]), seed, hash);
        MurmurHash3_x86_32(key, (uint32_t) strlen(key), seed, hash); // 0x07556ca6
        //printf("x86_32: %08x\n", hash[0]); 
        
        return hash[0];
	}


    void All_String_Hash() // vector of hashes and find MAX hash
    {
        for (int i=0; i < one.num_proxy ; i++)
        {
            create_temp_1DArray(i, one.num_bit_conc, one.String_to_hash);
            
            one.All_Hash[i]=Hash(one.char_temp);
            //printf("hash 32 %u \n",one.All_Hash[i]);
            if (i==0)
            {
                one.max_hash=one.All_Hash[i];
                one.proxint=i;
            }
            else
            {
                if(one.max_hash< one.All_Hash[i])
                {
                    one.max_hash=one.All_Hash[i];
                    one.proxint=i;
                }
            }
        }
    }
    
// void Map_hash_concatenated() // map hash to strings
//     {   
//         for(int i=0; i< All_Hash.size();i++)
//         {
//             hash_concatenate.insert(pair<uint32_t, string>(All_Hash[i], String_to_hash[i]));
//         }
//     } 
    
char * return_prox(int i)
{
    if(i ==0)
    {
        return proxy1;
    }
    if(i ==1)
    {
        return proxy2;
    }
    if(i ==2)
    {
        return proxy3;
    }
    if(i ==3)
    {
        return proxy4;
    }
    if(i ==4)
    {
        return proxy5;
    }
    if(i ==5)
    {
        return proxy6;
    }
}
char* Rendezvous(char obj_name[],int len_proxy, char prox_names[][len_proxy]) // combine and return proxy 
{
    add_obj(obj_name);
    All_Proxy_Concatenated(6,6,prox_names);
    All_String_Hash();
    //Map_hash_concatenated();
    //string hold_con=hash_concatenate.find(max_hash)->second;
    char* proxy_return=return_prox(one.proxint);
    // for( int i=0; i < hold_con.size(); i++)
    // {
    //     if(i>object_leng-1)
    //     {
    //         proxy_return=proxy_return+hold_con[i];
    //     }
    // }
    // needst to request from this proxy
    return proxy_return;
}




int
main ()
{
char arr[][6] = {{"abcabc"}, {"dcsdcs"}, {"aedaed"},{"dceccs"}, {"pppaed"}};

  char *n=Rendezvous("ar",6, arr);
  int i;
   printf("\n");
  for(i = 0; i < 6; i++)
    {
        printf("%c ", n[i]);
    }
  return 0;
}