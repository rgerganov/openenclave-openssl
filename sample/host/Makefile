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

CFLAGS=$(shell pkg-config oehost-$(COMPILER) --cflags)
LDFLAGS=$(shell pkg-config oehost-$(COMPILER) --libs)

build:
	@ echo "Compilers used: $(CC), $(CXX)"
	oeedger8r ../openssl_server.edl --untrusted
	$(CC) -g -c $(CFLAGS) host.c
	$(CC) -g -c $(CFLAGS) openssl_server_u.c
	$(CC) -o openssl_serverhost openssl_server_u.o host.o $(LDFLAGS)

clean:
	rm -f openssl_serverhost host.o openssl_server_u.o openssl_server_u.c openssl_server_u.h openssl_server_args.h
