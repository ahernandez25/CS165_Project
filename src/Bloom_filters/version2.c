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


typedef struct  {
    int m;			//bit cell
    int k;			// hash function
    uint32_t seed;		//changes to create new functions
    int hold_hash[20];	//h1...hk
    int fill_array[20];	//ints to fill  
    int Bloom_fil[20];	//  
} bf_struct;

bf_struct one = { 20, 0, 0, {0}, {0}, {0} };

// functions


void set_num_hashf (int k1)	//will set num hash functions
{
    printf("This is set %i \n", k1);
    one.m = 20;
    one.k = k1;
    one.seed = 12;
    printf("This is set k %i \n", one.k);
    //0-100 fill with 0
    int i;
    for(i=0;i<100;i++ )
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
    printf("This is hash %i \n", return_hash);
    return return_hash;
}

void hash_k (char obj_name[20])	// creates k num of has functions
{
    //int temp[20];	//hold to return 
    uint32_t seed_hold = one.seed;	//gives different hashs depending on seed val
    printf("This is hk\n");
    //hash same obj using different seeds
    int i;
    printf("This is seed %i \n", seed_hold);
    for ( i = 0; i < one.k; i++)	//gives h1(obj_name), ... ,hk(obj_name)stores in vec
    {
        one.hold_hash[i]=Hash(obj_name,seed_hold);
        seed_hold++;
        printf("This is hash_hold %i \n", one.hold_hash[i]);
        //cout<<"temp  "<<temp[i]<<endl;
    }
    
    //hold_hash=temp;
}

// void transform_hash()	// since vector size 0-99 need only 2 values: first and last digit (combined)
// {
//   fill_array=hold_hash;
//   //delete  hold_hash
  
//   for (int i = 0; i < m; i++)	//clear
//     {
//       hold_hash[i]=0;
//     }

// }

void fill_Bloom()
{
  for (int i = 0; i < one.k; i++)
    {
      one.Bloom_fil[one.hold_hash[i]] = 1;
    }
  
    for (int i = 0; i < one.m; i++)	//clear
    {
      one.hold_hash[i]=0;
    }
}

void insert (char obj_name[20]) //insert hash to bloom
{
  hash_k(obj_name);
  //transform_hash();
  fill_Bloom();
}

int query (char obj_name[20]) // check if hash is in bloom 1=true 0 =false
{
 hash_k (obj_name);
  //transform_hash ();
  for (int i = 0; i < one.k; i++)
    {
        if (one.Bloom_fil[one.hold_hash[i]] == 0)
          {
              //cout<<"f"<<endl;
            return 0;
          }

    }
  for (int i = 0; i < one.m; i++)	//clear
    {
      one.hold_hash[i]=0;
    }
  //cout<<"t"<<endl;
  return 1;
}


int
main ()
{
  char a[4]="ar";
  
  set_num_hashf(2);
  insert(a);
  int s[2];
  int i;
 
  
  s[0]=query("ar");
  s[1]=query("a"); 
  for(i = 0; i < 2; i++)
    {
        printf("%i \n", s[i]);
    }

//hash_k(a);
  return 0;
}