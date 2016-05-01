#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <openssl/bn.h>  
#include <openssl/hmac.h>
#include <openssl/rand.h>
  
void main(){  
//    BIGNUM *rnd;  
//    rnd = BN_new();  
    
    char data[] = "hello world";
    unsigned char digest[EVP_MAX_MD_SIZE] = {'\0'};
    unsigned int digest_len = 0;
/*
    int length;  
    char * show;  
    //BN_random  
    int bits = 128;  
    int top =0;  
    int bottom = 0;  
*/    
    unsigned char rkey[16]; 
    RAND_pseudo_bytes(rkey,sizeof(rkey));
    printf("sizeof(rkey):%ld\n",sizeof(rkey));
    printf("rkey: %c\n",rkey[15]); 
/*    //测试top = -1  
    top = 0;  
    bottom = 0;  
    BN_rand(rnd,bits,top,bottom);     
    length = BN_num_bits(rnd);  
    show = BN_bn2dec(rnd);  
    printf("length:%d,rnd:%s\n",length,show);  
    printf("%ld\n",sizeof(*rnd));
    printf("sizeof(data):%ld,strlen(data):%ld\n",sizeof(data),strlen(data));
*/
    HMAC(EVP_sha1(), rkey, sizeof(rkey), (unsigned char*)data, sizeof(data), digest, &digest_len);
    char mdString[41] = {'\0'};
    for(int i = 0; i < 20; i++)
         sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

    printf("HMAC digest: %s\n", mdString);

//    BN_free(rnd);  
}  
