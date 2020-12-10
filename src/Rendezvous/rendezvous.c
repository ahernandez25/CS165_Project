#include "rendezvous.h"

typedef struct r_structs {
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
	int i;
        for (i=0; i < one.num_proxy ; i++)
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
// Function to apply Rendezvous
char* Rendezvous(char obj_name[],int len_proxy, char prox_names[][len_proxy]) // combine and return proxy 
{
    add_obj(obj_name);
    All_Proxy_Concatenated(6,6,prox_names);
    All_String_Hash();
    char* proxy_return=return_prox(one.proxint);
    return proxy_return;
}
