#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libmemcached/memcached.h>
//gcc -o cc cc.c  -L /usr/local/libmemcached/lib -lmemcached
int main(int argc, char *argv[])
{
    memcached_st *memc;
    memcached_return rc;
    memcached_server_st *servers;
    char value[8191];
    //connect multi server
    memc = memcached_create(NULL);
    servers = memcached_server_list_append(NULL, "localhost", 11211, &rc);
    //servers = memcached_server_list_append(servers, "localhost", 11212, &rc);
    rc = memcached_server_push(memc, servers);
    memcached_server_free(servers);
    //Save multi data
    int i;
    char *keys[]= {"key1", "key2", "key3"};
    int key_length[]= {4, 4, 4};
    char *values[] = {"This is c first value", "This is c second value", "This is c third value"};
    int val_length[]= {21, 22, 21};
    for (i=0; i<3; i++)
    {
        rc = memcached_set(memc, keys[i],
        key_length[i], values[i], val_length[i], (time_t)180,
        (uint32_t)0);
        if (rc == MEMCACHED_SUCCESS)
        {
            printf("Save key:%s data:%s success\n",keys[i], values[i]);
        }
    }
    
    char return_key[MEMCACHED_MAX_KEY];
    int return_key_length;
    char *return_value;
    int return_value_length;
    uint32_t flags;
    rc = memcached_mget(memc, keys, key_length, 3);
    while ((return_value = memcached_fetch(memc, return_key,&return_key_length, &return_value_length, &flags, &rc))) {
        if (rc == MEMCACHED_SUCCESS)
        {
            printf("Fetch key:%s data:%s\n", return_key, return_value);
        }
    }
    //Delete multi data
    for (i=0; i<3; i++)
    {
        rc = memcached_set(memc, keys[i], key_length[i],
        values[i], val_length[i], (time_t)180, (uint32_t)0);
        rc = memcached_delete(memc, keys[i],
        key_length[i], (time_t)0);
        if (rc == MEMCACHED_SUCCESS)
        {
            printf("Delete %s success\n", keys[i], values[i]);
        }
    }
    //free
    memcached_free(memc);
    return 0;
}
