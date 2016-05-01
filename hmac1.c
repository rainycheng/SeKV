#include <stdio.h>
#include <string.h>
#include <openssl/hmac.h>

int main() {
    // The secret key for hashing
    const char key[] = "012345678";

    // The data that we're going to hash
    char data[] = "hello world";

    // Be careful of the length of string with the choosen hash engine. SHA1 needed 20 characters.
    // Change the length accordingly with your choosen hash engine.
    unsigned char* result;
    unsigned int len = 20;

    result = (unsigned char*)malloc(sizeof(char) * len);

    HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);

    // Using sha1 hash engine here.
    // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc
    HMAC_Init_ex(&ctx, key, strlen(key), EVP_sha1(), NULL);
    HMAC_Update(&ctx, (unsigned char*)&data, strlen(data));
    HMAC_Final(&ctx, result, &len);
    HMAC_CTX_cleanup(&ctx);

    printf("HMAC digest: ");

    for (int i = 0; i != len; i++)
        printf("%02x", (unsigned int)result[i]);

    printf("\n");

    free(result);

    return 0;
}
