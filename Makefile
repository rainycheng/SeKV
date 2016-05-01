CC=gcc
CFLAGS= -std=gnu11 
LIBS= -lmemcached 
LIBSGXDIR = /root/opensgx/libsgx
LLIBDIR = /usr/local/lib
LIBMEMD_LIBS = $(LLIBDIR)/libmemcached.a $(LLIBDIR)/libmemcachedutil.a $(LLIBDIR)/libhashkit.a
SGX_LIBS = $(LIBS) $(LIBSGXDIR)/sgx-entry.o $(LIBSGXDIR)/libsgx.a \
           $(LIBSGXDIR)/libpolarssl-sgx.a $(LIBSGXDIR)/libcrypto.a\
           /usr/local/libevent/lib/libevent.a /usr/lib/x86_64-linux-gnu/libsasl2.a
SGX_CFLAGS = $(CFLAGS) -I$(LIBSGXDIR)/include \
             -I/root/opensgx/user/include -I/root/opensgx/user/share/include \
             -fno-stack-protector -fvisibility=hidden
SGX_LDFLAGS = -nostartfiles -Wl,-T,/root/opensgx/user/sgx.lds


all: aes hmac hmac1 hmac2
 
aes: ssl_aes.c
	$(CC) $(CFLAGS) ssl_aes.c -o $@ $(LIBS)
hmac: ssl_hmac.c
	$(CC) $(CFLAGS) ssl_hmac.c -o $@ $(LIBS) 
hmac1: hmac.c
	$(CC) $(CFLAGS) hmac.c -o $@ $(LIBS)
hmac2: hmac1.c
	$(CC) $(CFLAGS) hmac1.c -o $@ $(LIBS)
client: simple_client.c
	$(CC) $(CFLAGS) simple_client.c -o $@ $(LIBS)
sgx_client: sgx_client.c
	$(CC) $(SGX_CFLAGS) $(SGX_LDFLAGS) sgx_client.c -g -o $@ $(SGX_LIBS)
clean:
	@rm -f aes hmac
