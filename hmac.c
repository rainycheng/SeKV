#include <stdio.h>
#include <string.h>
#include <openssl/hmac.h>

int main()
{
    // The key to hash
    char key[] = "012345678";

    // The data that we're going to hash using HMAC
    char data[] = "hello world";
    char data1[] = "helloi world";

    unsigned char digest[EVP_MAX_MD_SIZE] = {'\0'};
    unsigned int digest_len = 0;
    unsigned char digest1[EVP_MAX_MD_SIZE] = {'\0'};
    unsigned int digest_len1 = 0;

    // Using sha1 hash engine here.
    // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc
    HMAC(EVP_sha1(), key, strlen(key), (unsigned char*)data, strlen(data), digest, &digest_len);
    HMAC(EVP_sha1(), key, strlen(key), (unsigned char*)data1, strlen(data1), digest1, &digest_len1);
    //printf("%xx, len %u\n", digest, digest_len);
    printf("strcmp:%d\n",strcmp(digest,digest1));

    // Be careful of the length of string with the choosen hash engine. SHA1 produces a 20-byte hash value which rendered as 40 characters.
    // Change the length accordingly with your choosen hash engine
    char mdString[41] = {'\0'};
    for(int i = 0; i < 20; i++)
         sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

    printf("HMAC digest: %s\n", mdString);

    return 0;
}
