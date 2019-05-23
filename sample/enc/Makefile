# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

# Detect C and C++ compiler options
# if not gcc, default to clang-7

COMPILER=$(notdir $(CC))
ifeq ($(COMPILER), gcc)
        USE_GCC = true
endif

ifeq ($(USE_GCC),)
        CC = clang-7
        COMPILER=clang
endif

CFLAGS=$(shell pkg-config oeenclave-$(COMPILER) --cflags)
CFLAGS+=$(shell pkg-config openssl_oe_enclave --cflags)
LDFLAGS=$(shell pkg-config oeenclave-$(COMPILER) --libs)

all:
	$(MAKE) build
	$(MAKE) keys
	$(MAKE) sign

build:
	@ echo "Compilers used: $(CC), $(CXX)"
	oeedger8r ../openssl_server.edl --trusted
	$(CC) -g -c $(CFLAGS) -DOE_API_VERSION=2 enc.c -o enc.o
	$(CC) -g -c $(CFLAGS) -DOE_API_VERSION=2 ./openssl_server_t.c -o ./openssl_server_t.o
	$(CC) -g -c $(CFLAGS) -DOE_API_VERSION=2 ssl_stubs.c -o ssl_stubs.o

        #Note liboecpio and liboesgxfsenclave are included here since they are not in the pc file built by the branch this was built on
        #This will be removed when file and socket POSIX APIs are available in the master branch of Open Enclave.
	$(CC) -o openssl_server_enc ./openssl_server_t.o enc.o ssl_stubs.o $(shell pkg-config openssl_oe_enclave --libs) /opt/openenclave-featureio2/lib/openenclave/enclave/liboecpio.a /opt/openenclave-featureio2/lib/openenclave/enclave/liboesgxfsenclave.a $(LDFLAGS)

sign:
	oesign sign -e openssl_server_enc -c openssl_server.conf -k private.pem

clean:
	rm -f enc.o openssl_server_enc openssl_server_enc.signed private.pem public.pem openssl_server_t.o openssl_server_t.h openssl_server_t.c openssl_server_args.h ../key.pem ../cert.pem ssl_stubs.o

keys:
	openssl genrsa -out private.pem -3 3072
	openssl rsa -in private.pem -pubout -out public.pem
	openssl genrsa -out ../key.pem
	openssl req -new -x509 -key ../key.pem -out ../cert.pem  -days 3650 -subj "/C=US/ST=Ohio/L=Columbus/O=Acme Company/OU=Acme/CN=Root"
