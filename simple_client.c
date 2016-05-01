#include <libmemcached/memcached.h>
#include <stdio.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#define RDTSC_LL(val)     asm volatile("rdtsc" : "=A" (val))
typedef unsigned long long cycle_t;

int main(int argc, char **argv) {
  //memcached_servers_parse (char *server_strings);
  memcached_server_st *servers = NULL;
  memcached_st *memc;
  memcached_return rc;
  char *key = "keystring";
  char *value = "keyvalue";

  char *retrieved_value;
  size_t value_length;
  uint32_t flags;
  
  unsigned char digest[EVP_MAX_MD_SIZE] = {'\0'};
  unsigned int digest_len = 0;
  
  unsigned char rkey[16];
  RAND_pseudo_bytes(rkey,sizeof(rkey));

  memc = memcached_create(NULL);
  servers = memcached_server_list_append(servers, "localhost", 11211, &rc);
  rc = memcached_server_push(memc, servers);

  if (rc == MEMCACHED_SUCCESS)
    fprintf(stderr, "Added server successfully\n");
  else
    fprintf(stderr, "Couldn't add server: %s\n", memcached_strerror(memc, rc));

  cycle_t tStart;
  RDTSC_LL(tStart);
  AES_KEY aes;
//    unsigned char key[AES_BLOCK_SIZE];        // AES_BLOCK_SIZE = 16
    unsigned char iv[AES_BLOCK_SIZE];        // init vector
//    unsigned char* input_string;
    unsigned char* encrypt_string;
    unsigned char* decrypt_string;
    unsigned int len;        // encrypt length (in multiple of AES_BLOCK_SIZE)
    unsigned int i;
   
    len = 0;
    if ((strlen(value) + 1) % AES_BLOCK_SIZE == 0) {
        len = strlen(value) + 1;
    } else {
        len = ((strlen(value) + 1) / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    }
    for (i=0; i<AES_BLOCK_SIZE; ++i) {
        iv[i] = 0;
    }
    if (AES_set_encrypt_key(rkey, 128, &aes) < 0) {
        fprintf(stderr, "Unable to set encryption key in AES\n");
        exit(-1);
    }

    // alloc encrypt_string
    encrypt_string = (unsigned char*)calloc(len, sizeof(unsigned char));
    if (encrypt_string == NULL) {
        fprintf(stderr, "Unable to allocate memory for encrypt_string\n");
        exit(-1);
    }

    // encrypt (iv will change)
    AES_cbc_encrypt(value, encrypt_string, len, &aes, iv, AES_ENCRYPT);
    cycle_t tEnd;
    RDTSC_LL(tEnd);
    printf("tStart:%ld\n",tStart);
    printf("tEnd  :%ld\n",tEnd);
    printf("Encrypt time: %lf us\n",(tEnd-tStart)/2600.0);
  rc = memcached_set(memc, key, strlen(key), encrypt_string, strlen(encrypt_string), (time_t)0, (uint32_t)0);

  if (rc == MEMCACHED_SUCCESS)
    fprintf(stderr, "Key stored successfully\n");
  else
    fprintf(stderr, "Couldn't store key: %s\n", memcached_strerror(memc, rc));

  retrieved_value = memcached_get(memc, key, strlen(key), &value_length, &flags, &rc);
  printf("Yay!\n");

    // alloc decrypt_string
    decrypt_string = (unsigned char*)calloc(len, sizeof(unsigned char));
    if (decrypt_string == NULL) {
        fprintf(stderr, "Unable to allocate memory for decrypt_string\n");
        exit(-1);
    }

    // Set decryption key
    for (i=0; i<AES_BLOCK_SIZE; ++i) {
        iv[i] = 0;
    }
    if (AES_set_decrypt_key(rkey, 128, &aes) < 0) {
        fprintf(stderr, "Unable to set decryption key in AES\n");
        exit(-1);
    }

    // decrypt
    AES_cbc_encrypt(retrieved_value, decrypt_string, len, &aes, iv,
            AES_DECRYPT);

    printf("decrpt_string:%s\n",decrypt_string);

  if (rc == MEMCACHED_SUCCESS) {
    fprintf(stderr, "Key retrieved successfully\n");
    printf("The key '%s' returned value '%s'.\n", key, retrieved_value);
    free(retrieved_value);
  }
  else
    fprintf(stderr, "Couldn't retrieve key: %s\n", memcached_strerror(memc, rc));

  return 0;
}
