/* Simple AES GCM test program, uses the same NIST data used for the FIPS 
 * self test but uses the application level EVP APIs. 
 */  
#include <stdio.h>  
#include <openssl/bio.h>  
#include <openssl/evp.h>  
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#define RDTSC_LL(val)     asm volatile("rdtsc" : "=A" (val))
typedef unsigned long long cycle_t;  

/* AES-GCM test data from NIST public test vectors */  
unsigned char *gcm_pt;
int pt_len;

static const unsigned char gcm_key[] = {  
        0xee,0xbc,0x1f,0x57,0x48,0x7f,0x51,0x92,0x1c,0x04,0x65,0x66,  
        0x5f,0x8a,0xe6,0xd1,0x65,0x8b,0xb2,0x6d,0xe6,0xf8,0xa0,0x69,  
        0xa3,0x52,0x02,0x93,0xa5,0x72,0x07,0x8f  
};  
  
static const unsigned char gcm_iv[] = {  
        0x99,0xaa,0x3e,0x68,0xed,0x81,0x73,0xa0,0xee,0xd0,0x66,0x84  
};  
  
unsigned char gcm_pte[] = {  
        0xf5,0x6e,0x87,0x05,0x5b,0xc3,0x2d,0x0e,0xeb,0x31,0xb2,0xea,  
        0xcc,0x2b,0xf2,0xa5  
};  
  
static const unsigned char gcm_aad[] = {  
        0x4d,0x23,0xc3,0xce,0xc3,0x34,0xb4,0x9b,0xdb,0x37,0x0c,0x43,  
        0x7f,0xec,0x78,0xde  
};  
  
static const unsigned char gcm_ct[] = {  
        0xf7,0x26,0x44,0x13,0xa8,0x4c,0x0e,0x7c,0xd5,0x36,0x86,0x7e,  
        0xb9,0xf2,0x17,0x36  
};  
  
static const unsigned char gcm_tag[] = {  
        0x67,0xba,0x05,0x10,0x26,0x2a,0xe4,0x87,0xd7,0x37,0xee,0x62,  
        0x98,0xf7,0x7e,0x0c  
};  
  
void aes_gcm_encrypt(void)  
{  
        EVP_CIPHER_CTX *ctx;  
        int outlen, tmplen;  
        unsigned char outbuf[1048576];  
//        printf("AES GCM Encrypt:\n");  
//        printf("Plaintext:\n");  
//        BIO_dump_fp(stdout, gcm_pt, pt_len);  
//        printf("gcm_pt size: %d\n",pt_len);

	ctx = EVP_CIPHER_CTX_new();  
        /* Set cipher type and mode */  
        EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);  
        /* Set IV length if default 96 bits is not appropriate */  
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, sizeof(gcm_iv), NULL);  
        /* Initialise key and IV */  
        EVP_EncryptInit_ex(ctx, NULL, NULL, gcm_key, gcm_iv);  
        /* Zero or more calls to specify any AAD */  
        EVP_EncryptUpdate(ctx, NULL, &outlen, gcm_aad, sizeof(gcm_aad));  
        /* Encrypt plaintext */  
        EVP_EncryptUpdate(ctx, outbuf, &outlen, gcm_pt, pt_len);  
        /* Output encrypted block */  
//        printf("Ciphertext:\n");  
//        BIO_dump_fp(stdout, outbuf, outlen);  
        /* Finalise: note get no output for GCM */  
        EVP_EncryptFinal_ex(ctx, outbuf, &outlen);  
        /* Get tag */  
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, outbuf);  
        /* Output tag */  
//        printf("Tag:\n");  
//        BIO_dump_fp(stdout, outbuf, 16);  
        EVP_CIPHER_CTX_free(ctx);  
}  
  
void aes_gcm_decrypt(void)  
{  
        EVP_CIPHER_CTX *ctx;  
        int outlen, tmplen, rv;  
        unsigned char outbuf[1048576];  
//        printf("AES GCM Derypt:\n");  
//        printf("Ciphertext:\n");  
//        BIO_dump_fp(stdout, gcm_ct, sizeof(gcm_ct));  
        ctx = EVP_CIPHER_CTX_new();  
        /* Select cipher */  
        EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);  
        /* Set IV length, omit for 96 bits */  
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, sizeof(gcm_iv), NULL);  
        /* Specify key and IV */  
        EVP_DecryptInit_ex(ctx, NULL, NULL, gcm_key, gcm_iv);  
#if 0  
        /* Set expected tag value. A restriction in OpenSSL 1.0.1c and earlier 
         * required the tag before any AAD or ciphertext */  
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, sizeof(gcm_tag), gcm_tag);  
#endif  
        /* Zero or more calls to specify any AAD */  
        EVP_DecryptUpdate(ctx, NULL, &outlen, gcm_aad, sizeof(gcm_aad));  
        /* Decrypt plaintext */  
        EVP_DecryptUpdate(ctx, outbuf, &outlen, gcm_ct, sizeof(gcm_ct));  
        /* Output decrypted block */  
//        printf("Plaintext:\n");  
//        BIO_dump_fp(stdout, outbuf, outlen);  
        /* Set expected tag value. Works in OpenSSL 1.0.1d and later */  
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, sizeof(gcm_tag), gcm_tag);  
        /* Finalise: note get no output for GCM */  
        rv = EVP_DecryptFinal_ex(ctx, outbuf, &outlen);  
        /* Print out return value. If this is not successful authentication 
         * failed and plaintext is not trustworthy. 
         */  
//        printf("Tag Verify %s\n", rv > 0 ? "Successful!" : "Failed!");  
        EVP_CIPHER_CTX_free(ctx);  
 }  
  
int main(int argc, char **argv)  
{  
        unsigned char *plain;
	int i;
	pt_len = 1024*atoi(argv[1]);
	plain = (unsigned char *)malloc(sizeof(unsigned char)*pt_len);
	printf("pt_len:%d\n",pt_len);
	for(i=0;i<pt_len;i++)
	   plain[i]=gcm_pte[i%16];
        gcm_pt = plain;
//        printf("plain size: %d gcm_pt size: %d\n",sizeof(gcm_pte),sizeof(plain));
	
	cycle_t tStart, tEnd;
	
	RDTSC_LL(tStart);
	
	aes_gcm_encrypt();  
        
	RDTSC_LL(tEnd);
	printf("Encrypt time: %lf us\n",(tEnd-tStart)/2600.0);
	
	RDTSC_LL(tStart);
	aes_gcm_decrypt();
	RDTSC_LL(tEnd);
	printf("tStart:%ld\n",tStart);
    	printf("tEnd  :%ld\n",tEnd);
    	printf("Decrypt time: %lf us\n",(tEnd-tStart)/2600.0); 
 
} 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
